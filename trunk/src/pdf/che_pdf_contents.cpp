#include "../../include/pdf/che_pdf_contents.h"


#define A(a)		(a)
#define B(a, b)		(a | b << 8)
#define C(a, b, c)	(a | b << 8 | c << 16)

HE_DWORD StringToDWORD( CHE_ByteString & str )
{
	if ( str.GetLength() == 0 )
	{
		return 0;
	}
	HE_DWORD length = 4;
	if ( str.GetLength() < length )
	{
		length = str.GetLength();
	}
	HE_DWORD valRet = 0;
	for ( HE_DWORD i = length-1; i >= 0; i )
	{
		valRet = valRet<<8;
		valRet &= str.GetData()[i];
	}
	return valRet;
}


HE_VOID CHE_PDF_ContentsParser::Parse( const CHE_PDF_Stream * pContents, const CHE_PDF_Dictionary * pResources, std::vector<CHE_PDF_ContentObject*> & vectorRet )
{
	if ( pContents == NULL || pResources == NULL )
	{
		return;
	}
	CHE_PDF_Parser * pParser = pContents->GetParser();
	if ( pContents->GetParser() == NULL )
	{
		return;
	}

	CHE_DynBuffer buf( 1024 * 4, 1024 *4, GetAllocator() );
	CHE_PDF_StreamAcc streamAcc( GetAllocator() );
	streamAcc.Attach( pContents );
	buf.Write( streamAcc.GetData(), streamAcc.GetSize() );
	streamAcc.Detach();

	IHE_Read * pRead = HE_CreateMemBufRead( buf.GetData(), buf.GetSize(), GetAllocator() );
	if ( pRead == NULL )
	{
		return;
	}

	CHE_PDF_SyntaxParser sParser( pParser, GetAllocator() );
	if ( sParser.InitParser( pRead ) == FALSE )
	{
		pRead->Release();
		HE_DestoryIHERead( pRead );
		return;
	}

	CHE_PDF_Dictionary* pFontDict = NULL;
	CHE_PDF_Dictionary* pExtGStateDict = NULL;
	CHE_PDF_Object * pTmpObj = NULL;
	CHE_Stack<CHE_PDF_Object*> OpdStack( GetAllocator() );
	CHE_PDF_ParseWordDes wordDes( GetAllocator() );
	CHE_PDF_Object * pTmpNode = NULL;
	HE_BOOL	bOpd = TRUE;
	CHE_Queue<CHE_GraphicsObject*> SupPathQueue( GetAllocator() );
	HE_FLOAT fBeginX = 0, fBeginY = 0;
	HE_FLOAT fCurX = 0, fCurY = 0;
	HE_BOOL	bConnect = FALSE;
	HE_BOOL	bSubPathClosed = FALSE;
	HE_BOOL		bClipPath = FALSE;
	PDF_FILL_MODE	ClipFillMode = FILL_MODE_NOZERO;
	HE_FLOAT	fPosiX = 0;
	HE_FLOAT	fPosiY = 0;
	HE_FLOAT	fCharSpace = 0;
	HE_FLOAT	fWordSpace = 0;
	HE_DWORD	dwScale = 100;
	HE_FLOAT	fLeading = 0;
	HE_DWORD	dwSize = 0;
	HE_BYTE		byteRender = 0;
	HE_DWORD	dwRise = 0;
	HE_BOOL		bKnockout = FALSE;
	HE_FLOAT	fMatrixA = 1;
	HE_FLOAT	fMatrixB = 0;
	HE_FLOAT	fMatrixC = 0;
	HE_FLOAT	fMatrixD = 1;
	HE_FLOAT	fMatrixE = 0;
	HE_FLOAT	fMatrixF = 0;
	HE_DWORD	dwFontObjNum = 0;

	if ( pResources != NULL )
	{
		pTmpObj = pResources->GetElement( "Font", OBJ_TYPE_DICTIONARY );
		if ( pTmpObj )
		{
			pFontDict = pTmpObj->ToDict();
		}
		pTmpObj = pResources->GetElement( "ExtGState", OBJ_TYPE_DICTIONARY );
		if ( pTmpObj )
		{
			pExtGStateDict = pTmpObj->ToDict();
		}
	}


	CHE_PDF_OrderObject * pClipOrder = CHE_PDF_OrderObject::Create( GetAllocator() );

	while( sParser.GetWord( wordDes ) == TRUE )
	{
		bOpd = TRUE;
		switch ( wordDes.type )
		{
		case PARSE_WORD_INTEGER:
			{
				pTmpNode = CHE_PDF_Number::Create( wordDes.str.GetInteger(), 0, 0, pParser, GetAllocator() );
				OpdStack.Push( pTmpNode );
				break;
			}
		case PARSE_WORD_FLOAT:
			{
				pTmpNode = CHE_PDF_Number::Create( wordDes.str.GetFloat(), 0, 0, pParser, GetAllocator() );
				OpdStack.Push( pTmpNode );
				break;
			}
		case PARSE_WORD_NAME:
			{
				pTmpNode = CHE_PDF_Name::Create( wordDes.str, 0, 0, pParser, GetAllocator() );
				OpdStack.Push( pTmpNode );
				break;
			}
		case PARSE_WORD_STRING:
			{
				pTmpNode = CHE_PDF_String::Create( wordDes.str, 0, 0, pParser, GetAllocator() );
				OpdStack.Push( pTmpNode );
				break;
			}
		case PARSE_WORD_ARRAY_B:
			{
				sParser.SetPos( wordDes.offset );
				pTmpNode = sParser.GetArray();
				OpdStack.Push( pTmpNode );
				break;
			}
		case PARSE_WORD_DICT_B:
			{
				sParser.SetPos( wordDes.offset );
				pTmpNode = sParser.GetDictionary();
				OpdStack.Push( pTmpNode );
				break;
			}
		default:
			{
				bOpd = FALSE;
				break;
			}
		}
		if ( bOpd == TRUE )
		{
			continue;
		}

		switch ( StringToDWORD( wordDes.str ) )
		{
		case A('"'):			Handle_dquote();	break;
		case A('\''):			Handle_squote();	break;
		case A('B'):			Handle_B();			break;
		case B('B','*'):		Handle_Bstar();		break;
		case C('B','D','C'):	Handle_BDC();		break;
		case B('B','I'):		Handle_BI();		break;
		case C('B','M','C'):	Handle_BMC();		break;
		case B('B','T'):		Handle_BT();		break;
		case B('B','X'):		Handle_BX();		break;
		case B('C','S'):		Handle_CS();		break;
		case B('D','P'):		Handle_DP();		break;
		case B('D','o'):		Handle_Do();		break;
		case C('E','M','C'):	Handle_EMC();		break;
		case B('E','T'):		Handle_ET();		break;
		case B('E','X'):		Handle_EX();		break;
		case A('F'):			Handle_F();			break;
		case A('G'):			Handle_G();			break;
		case A('J'):			Handle_J();			break;
		case A('K'):			Handle_K();			break;
		case A('M'):			Handle_M();			break;
		case B('M','P'):		Handle_MP();		break;
		case A('Q'):			Handle_Q();			break;
		case B('R','G'):		Handle_RG();		break;
		case A('S'):			Handle_S();			break;
		case B('S','C'):		Handle_SC();		break;
		case C('S','C','N'):	Handle_SCN();		break;
		case B('T','*'):		Handle_Tstar();		break;
		case B('T','D'):		Handle_TD();		break;
		case B('T','J'):		Handle_TJ();		break;
		case B('T','L'):		Handle_TL();		break;
		case B('T','c'):		Handle_Tc();		break;
		case B('T','d'):		Handle_Td();		break;
		case B('T','f'):		Handle_Tf();		break;
		case B('T','j'):		Handle_Tj();		break;
		case B('T','m'):		Handle_Tm();		break;
		case B('T','r'):		Handle_Tr();		break;
		case B('T','s'):		Handle_Ts();		break;
		case B('T','w'):		Handle_Tw();		break;
		case B('T','z'):		Handle_Tz();		break;
		case A('W'):			Handle_W();			break;
		case B('W','*'):		Handle_Wstar();		break;
		case A('b'):			Handle_b();			break;
		case B('b','*'):		Handle_bstar();		break;
		case A('c'):			Handle_c();			break;
		case B('c','m'):		Handle_cm();		break;
		case B('c','s'):		Handle_cs();		break;
		case A('d'):			Handle_d();			break;
		case B('d','0'):		Handle_d0();		break;
		case B('d','1'):		Handle_d1();		break;
		case A('f'):			Handle_f();			break;
		case B('f','*'):		Handle_fstar();		break;
		case A('g'):			Handle_g();			break;
		case B('g','s'):		Handle_gs();		break;
		case A('h'):			Handle_h();			break;
		case A('i'):			Handle_i();			break;
		case A('j'):			Handle_j();			break;
		case A('k'):			Handle_k();			break;
		case A('l'):			Handle_l();			break;
		case A('m'):			Handle_m();			break;
		case A('n'):			Handle_n();			break;
		case A('q'):			Handle_q();			break;
		case B('r','e'):		Handle_re();		break;
		case B('r','g'):		Handle_rg();		break;
		case B('r','i'):		Handle_ri();		break;
		case A('s'):			Handle_s();			break;
		case B('s','c'):		Handle_sc();		break;
		case C('s','c','n'):	Handle_scn();		break;
		case B('s','h'):		Handle_sh();		break;
		case A('v'):			Handle_v();			break;
		case A('w'):			Handle_w();			break;
		case A('y'):			Handle_y();			break;
		default:break;
		}

		//清除无用的操作数
		while ( OpdStack.Pop( pTmpNode ) == TRUE )
		{
			if ( pTmpNode != NULL )
			{
				pTmpNode->Release();
			}
		}
	}

	pRead->Release();
	HE_DestoryIHERead( pRead );

	while ( OpdStack.Pop( pTmpNode ) == TRUE )
	{
		if ( pTmpNode != NULL )
		{
			pTmpNode->Release();
		}
	}
	//清除无用的subpath
	CHE_GraphicsObject * pTmpGraph = NULL;
	while ( SupPathQueue.Pop( pTmpGraph ) == TRUE )
	{
		if ( pTmpGraph != NULL )
		{
			pTmpGraph->Release();
		}
	}
}