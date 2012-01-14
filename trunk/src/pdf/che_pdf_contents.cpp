#include "../../include/pdf/che_pdf_contents.h"


#define A(a)		(a)
#define B(a,b)		(a | b<<8)
#define C(a,b,c)	(a | b<<8 | c<<16)

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
	for ( HE_DWORD i = length; i > 0; --i )
	{
		valRet = valRet<<8;
		valRet |= str.GetData()[i-1];
	}
	return valRet;
}

HE_BOOL CHE_PDF_ContentsParser::Parse( const CHE_PDF_Stream * pContents )
{
	if ( pContents == NULL || mpConstructor == NULL )
	{
		return FALSE;
	}

	CHE_DynBuffer buf( pContents->GetRawSize() * 2, pContents->GetRawSize(), GetAllocator() );
	CHE_PDF_StreamAcc streamAcc( GetAllocator() );
	streamAcc.Attach( pContents );
	buf.Write( streamAcc.GetData(), streamAcc.GetSize() );
	streamAcc.Detach();

	ParseImp( &buf );

	return TRUE;
}

HE_BOOL CHE_PDF_ContentsParser::Parse( const CHE_PDF_Array * pContentArray )
{
	if ( pContentArray == NULL || mpConstructor == NULL )
	{
		return FALSE;
	}

	CHE_PDF_Object * pTmpObj = NULL;
	CHE_PDF_Stream * pStream = NULL;
	for ( HE_DWORD i = 0; i < pContentArray->GetCount(); ++i ) 
	{
		pTmpObj = pContentArray->GetElement( i, OBJ_TYPE_STREAM );
		pStream = pTmpObj->ToStream();
		if ( pStream )
		{
			CHE_DynBuffer buf( pStream->GetRawSize() * 2, pStream->GetRawSize(), GetAllocator() );
			CHE_PDF_StreamAcc streamAcc( GetAllocator() );
			streamAcc.Attach( pStream );
			buf.Write( streamAcc.GetData(), streamAcc.GetSize() );
			streamAcc.Detach();
			ParseImp( &buf );
		}
	}

	return TRUE;
}

HE_VOID CHE_PDF_ContentsParser::ParseImp( CHE_DynBuffer * pStream )
{
	if ( !pStream )
	{
		return;
	}

	IHE_Read * pRead = HE_CreateMemBufRead( pStream->GetData(), pStream->GetSize(), GetAllocator() );
	if ( pRead == NULL )
	{
		return;
	}

	CHE_PDF_SyntaxParser sParser( NULL, GetAllocator() );
	if ( sParser.InitParser( pRead ) == FALSE )
	{
		pRead->Release();
		HE_DestoryIHERead( pRead );
		return;
	}

	CHE_PDF_ParseWordDes wordDes( GetAllocator() );
	CHE_PDF_Object * pTmpNode = NULL;
	HE_FLOAT tmpValue = 0;
	HE_BOOL	bOpd = TRUE;

	while( sParser.GetWord( wordDes ) == TRUE )
	{
		bOpd = TRUE;
		switch ( wordDes.type )
		{
		case PARSE_WORD_INTEGER:
			{
				tmpValue = (HE_FLOAT)( wordDes.str.GetInteger() );
				mOpdFloatStack.push_back( tmpValue );
				break;
			}
		case PARSE_WORD_FLOAT:
			{
				tmpValue = wordDes.str.GetFloat();
				mOpdFloatStack.push_back( tmpValue );
				break;
			}
		case PARSE_WORD_NAME:
			{
				mName = wordDes.str;
				break;
			}
		case PARSE_WORD_STRING:
			{
				mString = wordDes.str;
				break;
			}
		case PARSE_WORD_ARRAY_B:
			{
				sParser.SetPos( wordDes.offset );
				pTmpNode = sParser.GetArray();
				mpObj = pTmpNode;
				break;
			}
		case PARSE_WORD_DICT_B:
			{
				sParser.SetPos( wordDes.offset );
				pTmpNode = sParser.GetDictionary();
				mpObj = pTmpNode;
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
		mOpdFloatStack.clear();
		if ( mpObj )
		{
			mpObj->Release();
			mpObj = NULL;
		}
	}

	//清除无用的操作数
	mOpdFloatStack.clear();
	if ( mpObj )
	{
		mpObj->Release();
		mpObj = NULL;
	}

	pRead->Release();
	HE_DestoryIHERead( pRead );
}

HE_VOID CHE_PDF_ContentsParser::Handle_dquote()
{
	CHE_PDF_Text * pText = GetAllocator()->New<CHE_PDF_Text>( GetAllocator() );
	mpConstructor->Append( pText );
}

HE_VOID CHE_PDF_ContentsParser::Handle_squote()
{
	CHE_PDF_Text * pText = GetAllocator()->New<CHE_PDF_Text>( GetAllocator() );
	mpConstructor->Append( pText );
}

HE_VOID CHE_PDF_ContentsParser::Handle_B()
{
	if ( mpPath )
	{
		mpPath->SetFillMode( Mode_Nonzero );
		mpPath->SetPaintType( Paint_FillStroke );
		mpConstructor->Append( mpPath );
		mpPath = NULL;
	}
}

HE_VOID CHE_PDF_ContentsParser::Handle_Bstar()
{
	if ( mpPath )
	{
		mpPath->SetFillMode( Mode_EvenOdd );
		mpPath->SetPaintType( Paint_FillStroke );
		mpConstructor->Append( mpPath );
		mpPath = NULL;
	}
}

HE_VOID CHE_PDF_ContentsParser::Handle_BDC()
{

}

HE_VOID CHE_PDF_ContentsParser::Handle_BI()
{

}

HE_VOID CHE_PDF_ContentsParser::Handle_BMC()
{

}

HE_VOID CHE_PDF_ContentsParser::Handle_BT()
{

}

HE_VOID CHE_PDF_ContentsParser::Handle_BX()
{

}

HE_VOID CHE_PDF_ContentsParser::Handle_CS()
{
	mpConstructor->SetStrokeColorSpace();
}

HE_VOID CHE_PDF_ContentsParser::Handle_DP()
{

}

HE_VOID CHE_PDF_ContentsParser::Handle_Do()
{
	if ( mName.GetLength() > 0 )
	{
		CHE_PDF_Image * pImage = GetAllocator()->New<CHE_PDF_Image>( mName.GetData(), GetAllocator() );
		mpConstructor->Append( pImage );
	}
}

HE_VOID CHE_PDF_ContentsParser::Handle_EMC()
{

}

HE_VOID CHE_PDF_ContentsParser::Handle_ET()
{

}

HE_VOID CHE_PDF_ContentsParser::Handle_EX()
{

}

HE_VOID CHE_PDF_ContentsParser::Handle_F()
{
	Handle_f(); 
}

HE_VOID CHE_PDF_ContentsParser::Handle_G()
{
	mpConstructor->SetStrokeColor();
}

HE_VOID CHE_PDF_ContentsParser::Handle_J()
{
	if ( mOpdFloatStack.size() >= 1 )
	{
		PDF_GSTATE_LINEJOIN lineJoin = LineJoin_Miter;
		switch ( unsigned int( mOpdFloatStack[0] ) )
		{
		default:
		case 0:
			lineJoin = LineJoin_Miter;
			break;
		case 1:
			lineJoin = LineJoin_Round;
			break;
		case 2:
			lineJoin = LineJoin_Bevel;
			break;
		}
		mpConstructor->SetLineJion( lineJoin );
	}
}

HE_VOID CHE_PDF_ContentsParser::Handle_K()
{
	mpConstructor->SetStrokeColor();
}
HE_VOID CHE_PDF_ContentsParser::Handle_M()
{
	if ( mOpdFloatStack.size() >= 1 )
	{
		mpConstructor->SetMiterLimit( mOpdFloatStack[0] );
	}
}

HE_VOID CHE_PDF_ContentsParser::Handle_MP()
{

}

HE_VOID CHE_PDF_ContentsParser::Handle_Q()
{
	mpConstructor->PopGState();
}

HE_VOID CHE_PDF_ContentsParser::Handle_RG()
{
	mpConstructor->SetStrokeColor();
}

HE_VOID CHE_PDF_ContentsParser::Handle_S()
{
	if ( mpPath )
	{
		mpPath->SetPaintType( Paint_Stroke );
		mpConstructor->Append( mpPath );
		mpPath = NULL;
	}
}
HE_VOID CHE_PDF_ContentsParser::Handle_SC()
{

}

HE_VOID CHE_PDF_ContentsParser::Handle_SCN()
{

}

HE_VOID CHE_PDF_ContentsParser::Handle_Tstar()
{

}

HE_VOID CHE_PDF_ContentsParser::Handle_TD()
{
	CHE_PDF_Matrix matrix;
	mpConstructor->SetTextMatirx( matrix );
}

HE_VOID CHE_PDF_ContentsParser::Handle_TJ()
{
	CHE_PDF_Text * pText = GetAllocator()->New<CHE_PDF_Text>( GetAllocator() );
	mpConstructor->Append( pText );
}

HE_VOID CHE_PDF_ContentsParser::Handle_TL()
{
	mpConstructor->SetTextLeading( 0 );
}

HE_VOID CHE_PDF_ContentsParser::Handle_Tc()
{
	if ( mOpdFloatStack.size() >= 1 )
	{
		mpConstructor->SetTextWordSpace( mOpdFloatStack[0] );
	}
}

HE_VOID CHE_PDF_ContentsParser::Handle_Td()
{
	CHE_PDF_Matrix matrix;
	mpConstructor->SetTextMatirx( matrix );
}

HE_VOID CHE_PDF_ContentsParser::Handle_Tf()
{
	mpConstructor->SetTextFont();
	mpConstructor->SetTextFontSize( 0 );
}

HE_VOID CHE_PDF_ContentsParser::Handle_Tj()
{
	CHE_PDF_Text * pText = GetAllocator()->New<CHE_PDF_Text>( GetAllocator() );
	mpConstructor->Append( pText );
}

HE_VOID CHE_PDF_ContentsParser::Handle_Tm()
{
	CHE_PDF_Matrix matrix;
	mpConstructor->SetTextMatirx( matrix );
}

HE_VOID CHE_PDF_ContentsParser::Handle_Tr()
{
	if ( mOpdFloatStack.size() >= 1 )
	{
		PDF_GSTATE_TEXTRENDERMODE textRm = TextRenderMode_Fill;
		switch ( unsigned int( mOpdFloatStack[0] ) )
		{
		default:
		case 0:
			textRm = TextRenderMode_Fill;
			break;
		case 1:
			textRm = TextRenderMode_Stroke;
			break;
		case 2:
			textRm = TextRenderMode_FillStroke;
			break;
		case 3:
			textRm = TextRenderMode_Invisible;
			break;
		case 4:
			textRm = TextRenderMode_FillClip;
			break;
		case 5:
			textRm = TextRenderMode_StrokeClip;
			break;
		case 6:
			textRm = TextRenderMode_FillStrokeClip;
			break;
		case 7:
			textRm = TextRenderMode_Invisible;
			break;
		}
		mpConstructor->SetTextRenderMode( textRm );
	}
}

HE_VOID CHE_PDF_ContentsParser::Handle_Ts()
{
	if ( mOpdFloatStack.size() >= 1 )
	{
		mpConstructor->SetTextRise( mOpdFloatStack[0] );
	}
}

HE_VOID CHE_PDF_ContentsParser::Handle_Tw()
{
	if ( mOpdFloatStack.size() > 1 )
	{
		mpConstructor->SetTextWordSpace( mOpdFloatStack[0] );
	}
}

HE_VOID CHE_PDF_ContentsParser::Handle_Tz()
{
	if ( mOpdFloatStack.size() > 1 )
	{
		mpConstructor->SetTextRise( mOpdFloatStack[0] );
	}
}

HE_VOID CHE_PDF_ContentsParser::Handle_W()
{
	mpConstructor->AddClip();
}

HE_VOID CHE_PDF_ContentsParser::Handle_Wstar()
{
	mpConstructor->AddClip();
}

HE_VOID CHE_PDF_ContentsParser::Handle_b()
{
	if ( mpPath )
	{
		CHE_PDF_PathItem pathItem;
		pathItem.type = PathItem_Close;
		mpPath->mItems.push_back( pathItem );
		mpPath->SetFillMode( Mode_Nonzero );
		mpPath->SetPaintType( Paint_FillStroke );
		mpConstructor->Append( mpPath );
		mpPath = NULL;
	}
}

HE_VOID CHE_PDF_ContentsParser::Handle_bstar()
{
	if ( mpPath )
	{
		CHE_PDF_PathItem pathItem;
		pathItem.type = PathItem_Close;
		mpPath->mItems.push_back( pathItem );
		mpPath->SetFillMode( Mode_EvenOdd );
		mpPath->SetPaintType( Paint_FillStroke );
		mpConstructor->Append( mpPath );
		mpPath = NULL;
	}
}

HE_VOID CHE_PDF_ContentsParser::Handle_c()
{
	if ( mOpdFloatStack.size() >= 6 )
	{
		if ( mpPath )
		{
			CHE_PDF_PathItem pathItem;
			pathItem.type = PathItem_CurveTo;
			mpPath->mItems.push_back( pathItem );
			pathItem.value = mOpdFloatStack[0];
			mpPath->mItems.push_back( pathItem );
			pathItem.value = mOpdFloatStack[1];
			mpPath->mItems.push_back( pathItem );
			pathItem.value = mOpdFloatStack[2];
			mpPath->mItems.push_back( pathItem );
			pathItem.value = mOpdFloatStack[3];
			mpPath->mItems.push_back( pathItem );
			mCurX = pathItem.value = mOpdFloatStack[4];
			mpPath->mItems.push_back( pathItem );
			mCurY = pathItem.value = mOpdFloatStack[5];
			mpPath->mItems.push_back( pathItem );
		}
	}
}

HE_VOID CHE_PDF_ContentsParser::Handle_cm()
{
	CHE_PDF_Matrix matrix;
	if ( mOpdFloatStack.size() >= 6 )
	{
		matrix.a = mOpdFloatStack[0];
		matrix.b = mOpdFloatStack[1];
		matrix.c = mOpdFloatStack[2];
		matrix.d = mOpdFloatStack[3];
		matrix.e = mOpdFloatStack[4];
		matrix.f = mOpdFloatStack[5];
	}
	//todo
	mpConstructor->SetMatrix( matrix );
}

HE_VOID CHE_PDF_ContentsParser::Handle_cs()
{
	mpConstructor->SetFillColorSpace();
}

HE_VOID CHE_PDF_ContentsParser::Handle_d()
{
	PDF_GSTATE_DASHPATTERN dashPattern;
	dashPattern.dashPhase = 0;
	mpConstructor->SetLineDash( dashPattern );
}

HE_VOID CHE_PDF_ContentsParser::Handle_d0()
{
}

HE_VOID CHE_PDF_ContentsParser::Handle_d1()
{
}

HE_VOID CHE_PDF_ContentsParser::Handle_f()
{
	if ( mpPath )
	{
		mpPath->SetFillMode( Mode_Nonzero );
		mpPath->SetPaintType( Paint_Fill );
		mpConstructor->Append( mpPath );
		mpPath = NULL;
	}
}

HE_VOID CHE_PDF_ContentsParser::Handle_fstar()
{
	if ( mpPath )
	{
		mpPath->SetFillMode( Mode_EvenOdd );
		mpPath->SetPaintType( Paint_Fill );
		mpConstructor->Append( mpPath );
		mpPath = NULL;
	}
}

HE_VOID CHE_PDF_ContentsParser::Handle_g()
{
	mpConstructor->SetFillColor();
}

HE_VOID CHE_PDF_ContentsParser::Handle_gs()
{
	if ( mName.GetLength() > 0 )
	{
		mpConstructor->SetExtGState( mName.GetData(), NULL );
	}
}

HE_VOID CHE_PDF_ContentsParser::Handle_h()
{
	if ( mpPath )
	{
		CHE_PDF_PathItem pathItem;
		pathItem.type = PathItem_Close;
		mpPath->mItems.push_back( pathItem );
	}
}

HE_VOID CHE_PDF_ContentsParser::Handle_i()
{
	if ( mOpdFloatStack.size() >= 1 )
	{
		mpConstructor->SetFlatness( unsigned char(mOpdFloatStack[0]) );
	}
}

HE_VOID CHE_PDF_ContentsParser::Handle_j()
{
	if ( mOpdFloatStack.size() >= 1 )
	{
		PDF_GSTATE_LINEJOIN lineJion = LineJoin_Miter;
		switch ( unsigned int( mOpdFloatStack[0] ) )
		{
		default:
		case 0:
			lineJion = LineJoin_Miter;
			break;
		case 1:
			lineJion = LineJoin_Round;
			break;
		case 2:
			lineJion = LineJoin_Bevel;
			break;
		}
		mpConstructor->SetLineJion( lineJion );
	}
}

HE_VOID CHE_PDF_ContentsParser::Handle_k()
{
	mpConstructor->SetFillColor();
}

HE_VOID CHE_PDF_ContentsParser::Handle_l()
{
	if ( mOpdFloatStack.size() >= 2 )
	{
		if ( mpPath )
		{
			CHE_PDF_PathItem pathItem;
			pathItem.type = PathItem_LineTo;
			mpPath->mItems.push_back( pathItem );
			mCurX = pathItem.value = mOpdFloatStack[0];
			mpPath->mItems.push_back( pathItem );
			mCurY = pathItem.value = mOpdFloatStack[1];
			mpPath->mItems.push_back( pathItem );
		}
	}
}

HE_VOID CHE_PDF_ContentsParser::Handle_m()
{
	if ( mOpdFloatStack.size() >= 2 )
	{
		if ( ! mpPath )
		{
			mpPath = GetAllocator()->New<CHE_PDF_Path>( GetAllocator() );
		}
		CHE_PDF_PathItem pathItem;
		pathItem.type = PathItem_MoveTo;
		mpPath->mItems.push_back( pathItem );
		mCurX = pathItem.value = mOpdFloatStack[0];
		mpPath->mItems.push_back( pathItem );
		mCurY = pathItem.value = mOpdFloatStack[1];
		mpPath->mItems.push_back( pathItem );
	}
}

HE_VOID CHE_PDF_ContentsParser::Handle_n()
{
	if ( mpPath )
	{
		mpPath->SetFillMode( Mode_Nonzero );
		mpPath->SetPaintType( Paint_None );
		mpConstructor->Append( mpPath );
		mpPath = NULL;
	}
}

HE_VOID CHE_PDF_ContentsParser::Handle_q()
{
	mpConstructor->PushGState();
}

HE_VOID CHE_PDF_ContentsParser::Handle_re()
{
	if ( mOpdFloatStack.size() >= 4 )
	{
		if ( ! mpPath )
		{
			mpPath = GetAllocator()->New<CHE_PDF_Path>( GetAllocator() );
		}
		CHE_PDF_PathItem pathItem;
		pathItem.type = PathItem_Rectangle;
		mpPath->mItems.push_back( pathItem );
		pathItem.value = mOpdFloatStack[0];
		mpPath->mItems.push_back( pathItem );
		pathItem.value = mOpdFloatStack[1];
		mpPath->mItems.push_back( pathItem );
		pathItem.value = mOpdFloatStack[2];
		mpPath->mItems.push_back( pathItem );
		pathItem.value = mOpdFloatStack[3];
		mpPath->mItems.push_back( pathItem );
		pathItem.type = PathItem_Close;
		mpPath->mItems.push_back( pathItem );
	}
}

HE_VOID CHE_PDF_ContentsParser::Handle_rg()
{
	mpConstructor->SetFillColor();
}

HE_VOID CHE_PDF_ContentsParser::Handle_ri()
{
	mpConstructor->SetRenderingIntents( RI_AbsoluteColorimetric );
}

HE_VOID CHE_PDF_ContentsParser::Handle_s()
{
	if ( mpPath )
	{
		CHE_PDF_PathItem pathItem;
		pathItem.type = PathItem_Close;
		mpPath->mItems.push_back( pathItem );
		mpPath->SetPaintType( Paint_Stroke );
		mpConstructor->Append( mpPath );
		mpPath = NULL;
	}
}

HE_VOID CHE_PDF_ContentsParser::Handle_sc()
{

}

HE_VOID CHE_PDF_ContentsParser::Handle_scn()
{

}

HE_VOID CHE_PDF_ContentsParser::Handle_sh()
{
	if ( mName.GetLength() > 0 )
	{
		CHE_PDF_Shading * pShading = GetAllocator()->New<CHE_PDF_Shading>( mName.GetData(), GetAllocator() );
		mpConstructor->Append( pShading );
	}
}

HE_VOID CHE_PDF_ContentsParser::Handle_v()
{
	if ( mOpdFloatStack.size() >= 4 )
	{
		if ( mpPath )
		{
			CHE_PDF_PathItem pathItem;
			pathItem.type = PathItem_CurveTo;
			pathItem.value = mCurX;
			mpPath->mItems.push_back( pathItem );
			pathItem.value = mCurY;
			mpPath->mItems.push_back( pathItem );
			mpPath->mItems.push_back( pathItem );
			pathItem.value = mOpdFloatStack[0];
			mpPath->mItems.push_back( pathItem );
			pathItem.value = mOpdFloatStack[1];
			mpPath->mItems.push_back( pathItem );
			pathItem.value = mOpdFloatStack[2];
			mpPath->mItems.push_back( pathItem );
			pathItem.value = mOpdFloatStack[3];
			mpPath->mItems.push_back( pathItem );
		}
	}
}

HE_VOID CHE_PDF_ContentsParser::Handle_w()
{
	if ( mOpdFloatStack.size() >= 1 )
	{
		mpConstructor->SetLineWidth( mOpdFloatStack[0] );
	}
}

HE_VOID CHE_PDF_ContentsParser::Handle_y()
{
	if ( mOpdFloatStack.size() >= 4 )
	{
		if ( mpPath )
		{
			CHE_PDF_PathItem pathItem;
			pathItem.type = PathItem_CurveTo;
			pathItem.value = mOpdFloatStack[0];
			mpPath->mItems.push_back( pathItem );
			pathItem.value = mOpdFloatStack[1];
			mpPath->mItems.push_back( pathItem );
			pathItem.value = mCurX;
			mpPath->mItems.push_back( pathItem );
			pathItem.value = mCurY;
			mpPath->mItems.push_back( pathItem );
			pathItem.value = mOpdFloatStack[2];
			mpPath->mItems.push_back( pathItem );
			pathItem.value = mOpdFloatStack[3];
			mpPath->mItems.push_back( pathItem );
		}
	}
}

CHE_ByteString CHE_PDF_ContentResMgr::CreateName( PDF_CONTENTRES_TYPE type, CHE_PDF_Object * pObj )
{
	CHE_PDF_Dictionary * pSubDict = NULL;
	pSubDict = GetSubDict( type );
	if ( !pSubDict )
	{
		return "";
	}
	CHE_ByteString nameRet;
	switch( type )
	{
	case CONTENTRES_EXTGSTATE:
		{
			nameRet = RequestName( pSubDict, "GS" );
			break;
		}
	case CONTENTRES_COLORSPACE:
		{
			nameRet = RequestName( pSubDict, "CS" );
			break;
		}
	case CONTENTRES_PATTERN:
		{
			nameRet = RequestName( pSubDict, "P" );
			break;
		}
	case CONTENTRES_SHADING:
		{
			nameRet = RequestName( pSubDict, "S" );
			break;
		}
	case CONTENTRES_XOBJECT:
		{
			nameRet = RequestName( pSubDict, "XOBJ" );
			break;
		}
	case CONTENTRES_FONT:
		{
			nameRet = RequestName( pSubDict, "F" );
			break;
		}
	default:
		break;
	}
	if ( nameRet.GetLength() > 0 )
	{
		pSubDict->SetAtObj( nameRet, pObj );
	}
	return nameRet;
}

// CHE_ByteString CHE_PDF_ContentResMgr::CreateName( PDF_CONTENTRES_TYPE type, const CHE_ByteString & name, CHE_Object * pObj );
// 
// HE_BOOL	CHE_PDF_ContentResMgr::DeleteName( PDF_CONTENTRES_TYPE type, const CHE_ByteString & name );
// 
// CHE_Object * CHE_PDF_ContentResMgr::GetResObj( PDF_CONTENTOBJ_TYPE type, const CHE_ByteString & name );
// 
// CHE_Object * CHE_PDF_ContentResMgr::GetResObj( const CHE_ByteString & name );

CHE_PDF_Dictionary * CHE_PDF_ContentResMgr::GetSubDict( PDF_CONTENTRES_TYPE type )
{
	if ( !mpResDict )
	{
		return NULL;
	}
	CHE_PDF_Object * pTmp = NULL;
	switch ( type )
	{
	case CONTENTRES_EXTGSTATE:
		{
			pTmp = mpResDict->GetElement( "ExtGState", OBJ_TYPE_DICTIONARY );
			break;
		}
	case CONTENTRES_COLORSPACE:
		{
			pTmp = mpResDict->GetElement( "ColorSpace", OBJ_TYPE_DICTIONARY );
			break;
		}
	case CONTENTRES_PATTERN:
		{
			pTmp = mpResDict->GetElement( "Pattern", OBJ_TYPE_DICTIONARY );
			break;
		}
	case CONTENTRES_SHADING:
		{
			pTmp = mpResDict->GetElement( "Shading", OBJ_TYPE_DICTIONARY );
			break;
		}
	case CONTENTRES_XOBJECT:
		{
			pTmp = mpResDict->GetElement( "XObjext", OBJ_TYPE_DICTIONARY );
			break;
		}
	case CONTENTRES_FONT:
		{
			pTmp = mpResDict->GetElement( "Font", OBJ_TYPE_DICTIONARY );
			break;
		}
	default:
		break;
	}
	if ( pTmp )
	{
		return pTmp->ToDict();
	}else{
		return CreateSubDict( type );
	}
}


CHE_PDF_Dictionary * CHE_PDF_ContentResMgr::CreateSubDict( PDF_CONTENTRES_TYPE type )
{
	CHE_PDF_Dictionary * pTmpDict = CHE_PDF_Dictionary::Create( mpResDict->GetAllocator() );
	switch ( type )
	{
	case CONTENTRES_EXTGSTATE:
		{
			mpResDict->SetAtDictionary( "ExtGState", pTmpDict );
			break;
		}
	case CONTENTRES_COLORSPACE:
		{
			mpResDict->SetAtDictionary( "ColorSpace", pTmpDict );
			break;
		}
	case CONTENTRES_PATTERN:
		{
			mpResDict->SetAtDictionary( "Pattern", pTmpDict );
			break;
		}
	case CONTENTRES_SHADING:
		{
			mpResDict->SetAtDictionary( "Shading", pTmpDict );
			break;
		}
	case CONTENTRES_XOBJECT:
		{
			mpResDict->SetAtDictionary( "XObject", pTmpDict );
			break;
		}
	case CONTENTRES_FONT:
		{
			mpResDict->SetAtDictionary( "Font", pTmpDict );
			break;
		}
	default:
		break;
	}
	return pTmpDict;
}

CHE_ByteString CHE_PDF_ContentResMgr::RequestName( CHE_PDF_Dictionary * pSubDict, const CHE_ByteString & name )
{
	if ( !pSubDict )
	{
		return "";
	}
	CHE_PDF_Object * pTmpObj = pSubDict->GetElement( name );
	if ( !pTmpObj )
	{
		return name;
	}else
	{
		HE_CHAR tmpStr[128];
		HE_DWORD tmpInt = 0;
		bool bContinue = true;
		while ( bContinue )
		{
			sprintf_s( tmpStr, "%s%d", name.GetData(), tmpInt );
			pTmpObj = pSubDict->GetElement( tmpStr );
			if ( !pTmpObj )
			{
				bContinue = false;
				continue;;
			}
			++tmpInt;
		}
		return tmpStr;
	}
	return "";
}

class CContentViewListConstructor : public IHE_PDF_ContentListConstructor
{
public:
	CContentViewListConstructor( std::vector<CHE_PDF_ContentObject*> * pVector, CHE_Allocator * pAllocator = NULL )
		: mpVector(pVector), IHE_PDF_ContentListConstructor( pAllocator ) {}
	~CContentViewListConstructor() {}

	void SetMatrix( const CHE_PDF_Matrix & matrix ) {}
	void SetLineWidth( HE_FLOAT lineWidth ) {}
	void SetLineCap( PDF_GSTATE_LINECAP lineCap ) {}
	void SetLineJion( PDF_GSTATE_LINEJOIN lineJion ) {}
	void SetMiterLimit( HE_FLOAT miterLimit ) {}
	void SetLineDash( const PDF_GSTATE_DASHPATTERN & dashPattern ) {}
	void SetRenderingIntents( PDF_GSTATE_RENDERINTENTS ri ) {}
	void SetFlatness( unsigned char flatness ) {}
	void SetExtGState( const std::string & name, const CHE_PDF_Dictionary * pDict ) {}
	void SetStrokeColor() {}
	void SetFillColor() {}
	void SetStrokeColorSpace() {}
	void SetFillColorSpace() {}
	void SetTextFont() {}
	void SetTextFontSize( HE_FLOAT size ) {}
	void SetTextCharSpace( HE_FLOAT charspace ) {}
	void SetTextWordSpace( HE_FLOAT wordspace ) {}
	void SetTextHScaling( HE_FLOAT scaling ) {}
	void SetTextLeading( HE_FLOAT leading ) {}
	void SetTextRise( HE_FLOAT rise ) {}
	void SetTextMatirx( const CHE_PDF_Matrix & matrix ) {}
	void SetTextRenderMode( PDF_GSTATE_TEXTRENDERMODE rm ) {}

	void PushGState()
	{
		CHE_PDF_PushGState * pNode = GetAllocator()->New<CHE_PDF_PushGState>();
		mpVector->push_back( pNode );
	}

	void PopGState()
	{
		CHE_PDF_PopGState * pNode = GetAllocator()->New<CHE_PDF_PopGState>();
		mpVector->push_back( pNode );
	}

	void AddClip() {}

	void Append( CHE_PDF_ContentObject * pObject )
	{
		if ( pObject )
		{
			mpVector->push_back( pObject );
		}
	}

	void Over() {}

private:
	std::vector<CHE_PDF_ContentObject*> * mpVector;
};

IHE_PDF_ContentListConstructor * CreateConstructor( std::vector<CHE_PDF_ContentObject*> * pVector, CHE_Allocator * pAllocator /*= NULL*/ )
{
	if ( !pVector )
	{
		return NULL;
	}
	if ( pAllocator == NULL )
	{
		return new CContentViewListConstructor( pVector );
	}else
	{
		return pAllocator->New<CContentViewListConstructor>( pVector, pAllocator );
	}
}