#include "../../include/pdf/che_pdf_contents.h"
#include "../../include/pdf/che_pdf_graphicsstate.h"


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

HE_BOOL CHE_PDF_ContentsParser::Parse( const CHE_PDF_StreamPtr & pContents )
{
	if ( ! pContents || mpConstructor == NULL )
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

HE_BOOL CHE_PDF_ContentsParser::Parse( const CHE_PDF_ArrayPtr & pContentArray )
{
	if ( ! pContentArray || mpConstructor == NULL )
	{
		return FALSE;
	}

	CHE_DynBuffer buf( 1024 * 4, 1024 * 8, GetAllocator() );

	for ( HE_DWORD i = 0; i < pContentArray->GetCount(); ++i ) 
	{
		CHE_PDF_StreamPtr pStream = pContentArray->GetElement( i, OBJ_TYPE_STREAM )->GetStreamPtr();
		if ( pStream )
		{
			CHE_PDF_StreamAcc streamAcc( GetAllocator() );
			streamAcc.Attach( pStream );
			buf.Write( streamAcc.GetData(), streamAcc.GetSize() );
			streamAcc.Detach();
		}
	}
	if ( buf.GetSize() > 0 )
	{
		ParseImp( &buf );
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
	CHE_PDF_ObjectPtr pTmpNode;
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

				if ( mbInlineImage && mParamFalg > 0 )
				{
					switch ( mParamFalg )
					{
					case 1:
						mBpc = wordDes.str.GetInteger();
						break;
					case 6:
						mHeight = wordDes.str.GetInteger();
						break;
					case 9:
						mWidth = wordDes.str.GetInteger();
						break;
					default:
						break;
					}
					mParamFalg = 0;
				}
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
				if ( mName.GetLength() > 0 )
				{
					mString = wordDes.str;
				}
				else
				{
					mName = wordDes.str;
				}
				if ( mbInlineImage )
				{
					if ( mParamFalg > 0 )
					{
						switch ( mParamFalg )
						{
						case 2:
							mpColorSpace = CHE_PDF_Name::Create( wordDes.str, GetAllocator() );
							break;
						case 3:
							mpDecode = CHE_PDF_Name::Create( wordDes.str, GetAllocator() );
							break;
						case 4:
							mpDecodeParam = CHE_PDF_Name::Create( wordDes.str, GetAllocator() );
							break;
						case 5:
							mpFilter = CHE_PDF_Name::Create( wordDes.str, GetAllocator() );
							break;
						default:
							break;
						}
						mParamFalg = 0;
					}else
					{
						switch ( StringToDWORD( wordDes.str ) )
						{
						case C('B','P','C'):	mParamFalg = 1; break;
						case B('C','S'):		mParamFalg = 2; break;
						case A('D'):			mParamFalg = 3; break;
						case B('D','P'):		mParamFalg = 4; break;
						case A('F'):			mParamFalg = 5; break;
						case A('H'):			mParamFalg = 6; break;
						case B('I','M'):		mParamFalg = 7; break;
						case A('I'):			mParamFalg = 8; break;
						case A('W'):			mParamFalg = 9; break;
						default:				mParamFalg = 0; break;
						}
					}
				}
				break;
			}
		case PARSE_WORD_STRING:
			{
				mString = wordDes.str;
				mpObj = CHE_PDF_String::Create( wordDes.str, GetAllocator() );
				break;
			}
		case PARSE_WORD_ARRAY_B:
			{
				sParser.SetPos( wordDes.offset );
				pTmpNode = sParser.GetArrayPtr();
				mpObj = pTmpNode;
				if ( mbInlineImage )
				{
					if ( mParamFalg > 0 )
					{
						switch ( mParamFalg )
						{
						case 3:
							mpDecode = mpObj;
							break;
						case 4:
							mpDecodeParam = mpObj;
							break;
						case 5:
							mpFilter = mpObj;
							break;
						default:
							break;
						}
						mParamFalg = 0;
					}
				}
				break;
			}
		case PARSE_WORD_DICT_B:
			{
				sParser.SetPos( wordDes.offset );
				pTmpNode = sParser.GetDictionaryPtr();
				mpObj = pTmpNode;
				break;
			}
		default:
			{
				if ( mbInlineImage && mParamFalg == 7 )
				{
					if ( wordDes.str == "true" )
					{
						mbMask = true;
					}
					else if ( wordDes.str == "false" )
					{
						mbMask = false;
					}
					mParamFalg = 0;
					break;
				}
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
		case B('E','I'):		Handle_EI();		break;
		case C('E','M','C'):	Handle_EMC();		break;
		case B('E','T'):		Handle_ET();		break;
		case B('E','X'):		Handle_EX();		break;
		case A('F'):			Handle_F();			break;
		case A('G'):			Handle_G();			break;
		case B('I','D'):		Handle_ID(&sParser);break;
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

		//������õĲ�����
		mOpdFloatStack.clear();
		mpObj.reset();
		mName.Clear();
		mString.Clear();
	}

	//������õĲ�����
	mOpdFloatStack.clear();
	mpObj.reset();
	mName.Clear();
	mString.Clear();

	pRead->Release();
	HE_DestoryIHERead( pRead );
}

HE_BOOL CHE_PDF_ContentsParser::CheckOpdCount( size_t count )
{
	if ( mOpdFloatStack.size() >= count )
	{
		return TRUE;
	}
	return FALSE;
}

HE_VOID CHE_PDF_ContentsParser::Handle_dquote()
{
	mpConstructor->Operator_Tstar();
	if ( mpObj )
	{
		CHE_PDF_Text * pText = GetAllocator()->New<CHE_PDF_Text>( GetAllocator() );
		mpConstructor->Operator_Append( pText );
		pText->SetTextObject( mpObj );
	}
}

HE_VOID CHE_PDF_ContentsParser::Handle_squote()
{
	if ( CheckOpdCount( 2 ) )
	{
		mpConstructor->State_TextWordSpace( mOpdFloatStack[0] );
		mpConstructor->State_TextCharSpace( mOpdFloatStack[1] );
	}
	if ( mpObj )
	{
		CHE_PDF_Text * pText = GetAllocator()->New<CHE_PDF_Text>( GetAllocator() );
		mpConstructor->Operator_Append( pText );
		pText->SetTextObject( mpObj );
	}
}

HE_VOID CHE_PDF_ContentsParser::Handle_B()
{
	if ( mpPath )
	{
		mpPath->SetFillMode( Mode_Nonzero );
		mpPath->SetPaintType( Paint_FillStroke );
		mpConstructor->Operator_Append( mpPath );
		mpPath = NULL;
	}
	if ( mpClipPath )
	{
		mpConstructor->Operator_Clip( mpClipPath );
		mpClipPath = NULL;
	}
}

HE_VOID CHE_PDF_ContentsParser::Handle_Bstar()
{
	if ( mpPath )
	{
		mpPath->SetFillMode( Mode_EvenOdd );
		mpPath->SetPaintType( Paint_FillStroke );
		mpConstructor->Operator_Append( mpPath );
		mpPath = NULL;
	}
	if ( mpClipPath )
	{
		mpConstructor->Operator_Clip( mpClipPath );
		mpClipPath = NULL;
	}
}

HE_VOID CHE_PDF_ContentsParser::Handle_BDC()
{
	if ( mName.GetLength() > 0 &&  IsPdfDictPtr( mpObj ) )
	{
		CHE_PDF_Mark * pMark = GetAllocator()->New<CHE_PDF_Mark>( GetAllocator() );
		pMark->SetMarkType( Mark_BDC );
		pMark->SetTag( mName );
		if ( mpObj )
		{
			pMark->SetProperty( mpObj->GetDictPtr() );
		}
		else if ( mString.GetLength() > 0 )
		{
			CHE_PDF_ObjectPtr objPtr = mpContentResMgr->GetResObj( CONTENTRES_PROPERTIES, mString );
			if ( IsPdfRefPtr( objPtr ) )
			{
				objPtr = objPtr->GetRefPtr()->GetRefObj( OBJ_TYPE_DICTIONARY );
			}
			if ( IsPdfDictPtr( objPtr ) )
			{
				pMark->SetProperty( objPtr->GetDictPtr() );
			}
		}
		mpConstructor->Operator_Append( pMark );
	}
}

HE_VOID CHE_PDF_ContentsParser::Handle_BI()
{
	mbInlineImage = TRUE;
}

HE_VOID CHE_PDF_ContentsParser::Handle_BMC()
{
	if ( mName.GetLength() > 0 )
	{
		CHE_PDF_Mark * pMark = GetAllocator()->New<CHE_PDF_Mark>( GetAllocator() );
		pMark->SetMarkType( Mark_BMC );
		pMark->SetTag( mName );
		mpConstructor->Operator_Append( pMark );
	}
}

HE_VOID CHE_PDF_ContentsParser::Handle_BT()
{
	mpConstructor->State_TextMatirx( CHE_PDF_Matrix() );
}

HE_VOID CHE_PDF_ContentsParser::Handle_BX()
{

}

HE_VOID CHE_PDF_ContentsParser::Handle_CS()
{
	if ( mName.GetLength() > 0 )
	{
		CHE_PDF_ColorSpace * pColorSpace = NULL;
		if ( mName == "DeviceGray" )
		{
			pColorSpace = GetAllocator()->New<CHE_PDF_ColorSpace>( COLORSAPCE_DEVICE_GRAY, GetAllocator() );
		}else if ( mName == "DeviceRGB" )
		{
			pColorSpace = GetAllocator()->New<CHE_PDF_ColorSpace>( COLORSAPCE_DEVICE_RGB, GetAllocator() );
		}else if ( mName == "DeviceCMYK" )
		{
			pColorSpace = GetAllocator()->New<CHE_PDF_ColorSpace>( COLORSAPCE_DEVICE_CMYK, GetAllocator() );
		}else if ( mName == "Pattern" )
		{
			pColorSpace = GetAllocator()->New<CHE_PDF_ColorSpace>( COLORSAPCE_SPECIAL_PATTERN, GetAllocator() );
		}else{
			CHE_PDF_ObjectPtr pTmpObj = mpContentResMgr->GetResObj( CONTENTRES_COLORSPACE, mName );
			if ( !pTmpObj )
			{
				return;
			}
			if ( pTmpObj->GetType() == OBJ_TYPE_REFERENCE )
			{
				CHE_PDF_ReferencePtr refPtr = pTmpObj->GetRefPtr();
				pTmpObj = refPtr->GetRefObj( OBJ_TYPE_ARRAY ); 
				if ( !pTmpObj )
				{
					pTmpObj = refPtr->GetRefObj( OBJ_TYPE_NAME );
				}
			}
			if ( pTmpObj->GetType() == OBJ_TYPE_NAME )
			{
				CHE_PDF_NamePtr pName = pTmpObj->GetNamePtr();
				CHE_ByteString name = pName->GetString();
				if ( name == "DeviceGray" || name == "G" )
				{
					pColorSpace = GetAllocator()->New<CHE_PDF_ColorSpace>( COLORSAPCE_DEVICE_GRAY, GetAllocator() );
				}else if ( name == "DeviceRGB" || name == "RGB" )
				{
					pColorSpace = GetAllocator()->New<CHE_PDF_ColorSpace>( COLORSAPCE_DEVICE_RGB, GetAllocator() );
				}else if ( name == "DeviceCMYK" || name == "CMYK" )
				{
					pColorSpace = GetAllocator()->New<CHE_PDF_ColorSpace>( COLORSAPCE_DEVICE_CMYK, GetAllocator() );
				}else if ( name == "Pattern" )
				{
					pColorSpace = GetAllocator()->New<CHE_PDF_ColorSpace>( COLORSAPCE_SPECIAL_PATTERN, GetAllocator() );
				}
			}else if ( pTmpObj->GetType() == OBJ_TYPE_ARRAY )
			{
				CHE_PDF_ArrayPtr pArray = pTmpObj->GetArrayPtr();
				pTmpObj = pArray->GetElement( 0, OBJ_TYPE_NAME );
				if ( !pTmpObj )
				{
					return;
				}
				CHE_PDF_NamePtr pName = pTmpObj->GetNamePtr();
				CHE_ByteString name = pName->GetString();
				if ( name == "DeviceGray" || name == "G" )
				{
					pColorSpace = GetAllocator()->New<CHE_PDF_ColorSpace>( COLORSAPCE_DEVICE_GRAY, GetAllocator() );
				}else if ( name == "DeviceRGB" || name == "RGB" )
				{
					pColorSpace = GetAllocator()->New<CHE_PDF_ColorSpace>( COLORSAPCE_DEVICE_RGB, GetAllocator() );
				}else if ( name == "DeviceCMYK" || name == "CMYK" )
				{
					pColorSpace = GetAllocator()->New<CHE_PDF_ColorSpace>( COLORSAPCE_DEVICE_CMYK, GetAllocator() );
				}else if ( name == "Pattern" )
				{
					pColorSpace = GetAllocator()->New<CHE_PDF_ColorSpace>( COLORSAPCE_SPECIAL_PATTERN, mName, pArray->Clone(), GetAllocator() );
				}else if ( name == "CalGray" )
				{
					pColorSpace = GetAllocator()->New<CHE_PDF_ColorSpace>( COLORSAPCE_CIEBASE_CALGRAY, mName, pArray->Clone(), GetAllocator() );
				}else if ( name == "CalRGB" )
				{
					pColorSpace = GetAllocator()->New<CHE_PDF_ColorSpace>( COLORSAPCE_CIEBASE_CALRGB, mName, pArray->Clone(), GetAllocator() );
				}else if ( name == "CalCMYK" )
				{
					pColorSpace = GetAllocator()->New<CHE_PDF_ColorSpace>( COLORSAPCE_CIEBASE_CALCMYK, mName, pArray->Clone(), GetAllocator() );
				}else if ( name == "Lab" )
				{
					pColorSpace = GetAllocator()->New<CHE_PDF_ColorSpace>( COLORSAPCE_CIEBASE_CALLAB, mName, pArray->Clone(), GetAllocator() );
				}else if ( name == "ICCBased" )
				{
					pColorSpace = GetAllocator()->New<CHE_PDF_ColorSpace>( COLORSAPCE_CIEBASE_ICCBASED, mName, pArray->Clone(), GetAllocator() ); //zctodo ICCBased Ӧ�ü����жϾ������ɫ�ռ������
				}else if ( name == "Indexed" || name == "I" )
				{
					pColorSpace = GetAllocator()->New<CHE_PDF_ColorSpace>( COLORSAPCE_SPECIAL_INDEXED, mName, pArray->Clone(), GetAllocator() );
				}else if ( name == "Separation" )
				{
					pColorSpace = GetAllocator()->New<CHE_PDF_ColorSpace>( COLORSAPCE_SPECIAL_SEPARATION, mName, pArray->Clone(), GetAllocator() );
				}else if ( name == "DeviceN" )
				{
					pColorSpace = GetAllocator()->New<CHE_PDF_ColorSpace>( COLORSAPCE_SPECIAL_DEVICEN, mName, pArray->Clone(), GetAllocator() );
				}
			}
		}
		if ( pColorSpace )
		{
			mpConstructor->State_StrokeColorSpace( pColorSpace );
		}
	}
}

HE_VOID CHE_PDF_ContentsParser::Handle_DP()
{
	if ( mName.GetLength() > 0 && IsPdfDictPtr( mpObj ) )
	{
		CHE_PDF_Mark * pMark = GetAllocator()->New<CHE_PDF_Mark>( GetAllocator() );
		pMark->SetMarkType( Mark_DP );
		pMark->SetTag( mName );
		if ( mpObj )
		{
			pMark->SetProperty( mpObj->GetDictPtr() );
		}
		else if ( mString.GetLength() > 0 )
		{
			CHE_PDF_ObjectPtr objPtr = mpContentResMgr->GetResObj( CONTENTRES_PROPERTIES, mString );
			if ( IsPdfRefPtr( objPtr ) )
			{
				objPtr = objPtr->GetRefPtr()->GetRefObj( OBJ_TYPE_DICTIONARY );
			}
			if ( IsPdfDictPtr( objPtr ) )
			{
				pMark->SetProperty( objPtr->GetDictPtr() );
			}
		}
		mpConstructor->Operator_Append( pMark );
	}
}

HE_VOID CHE_PDF_ContentsParser::Handle_Do()
{
	if ( mName.GetLength() > 0 && mpContentResMgr )
	{
		CHE_PDF_ObjectPtr pTmpObj = mpContentResMgr->GetResObj( CONTENTRES_XOBJECT, mName );
		if ( !pTmpObj )
		{
			return;
		}
		CHE_PDF_ReferencePtr pRef = pTmpObj->GetRefPtr();
		if ( !pRef )
		{
			return;
		}
		pTmpObj = pRef->GetRefObj( OBJ_TYPE_STREAM );
		if ( !pTmpObj )
		{
			return;
		}
		CHE_PDF_StreamPtr pStm = pTmpObj->GetStreamPtr();
		CHE_PDF_DictionaryPtr pStmDict = pStm->GetDictPtr();
		if ( !pStmDict )
		{
			return;
		}
		pTmpObj = pStmDict->GetElement( "Subtype", OBJ_TYPE_NAME );
		if ( !pTmpObj )
		{
			return;
		}
		CHE_PDF_NamePtr pSubtypeName = pTmpObj->GetNamePtr();
		if ( !pSubtypeName )
		{
			return;
		}
		if ( pSubtypeName->GetString() == "Image" )
		{
			CHE_PDF_ObjectPtr pObj = mpContentResMgr->GetResObj( CONTENTRES_XOBJECT, mName );
			if ( pObj && IsPdfRefPtr( pObj ) )
			{
				CHE_PDF_RefImage * pImage = GetAllocator()->New<CHE_PDF_RefImage>( mName, pObj->GetRefPtr(), GetAllocator() );
				mpConstructor->Operator_Append( pImage );
			}
		}else if ( pSubtypeName->GetString() == "Form" )
		{
			CHE_PDF_Form * pForm = GetAllocator()->New<CHE_PDF_Form>( mName, GetAllocator() );
			CHE_PDF_ObjectPtr pTmpObj;
			CHE_PDF_DictionaryPtr pResDict;
			pTmpObj = pStmDict->GetElement( "Resources", OBJ_TYPE_DICTIONARY );
			if ( pTmpObj )
			{
				pResDict = pTmpObj->GetDictPtr();
			}
			//todo : get the matrix from form dict
			CHE_PDF_Matrix extMatrix = mpConstructor->GetExtMatrix();
			CHE_PDF_Matrix curMatrix = mpConstructor->GetCurMatrix();
			curMatrix.Concat( extMatrix );
			IHE_PDF_ContentListConstructor * pConstructor =  CreateConstructor( &pForm->GetList(), curMatrix, GetAllocator() );
			CHE_PDF_ContentResMgr * pContentResMgr = &( pForm->GetList().GetResMgr() );
			pContentResMgr->SetDict( pResDict );
			CHE_PDF_ContentsParser contentsParser( pContentResMgr, mpFontMgr, pConstructor );
			contentsParser.Parse( pStm );
			mpConstructor->Operator_Append( pForm );
			pConstructor->GetAllocator()->Delete( pConstructor );
		}
	}
}

HE_VOID CHE_PDF_ContentsParser::Handle_EI()
{
	mbInlineImage = FALSE;
	mbInterpolate = FALSE;
	mbMask = FALSE;
	mWidth = 0;
	mHeight = 0;
	mBpc = 0;

	mpColorSpace.reset();
	mpFilter.reset();
	mpDecode.reset();
	mpDecodeParam.reset();
}

HE_VOID CHE_PDF_ContentsParser::Handle_EMC()
{
	CHE_PDF_Mark * pMark = GetAllocator()->New<CHE_PDF_Mark>( GetAllocator() );
	pMark->SetMarkType( Mark_EMC );
	mpConstructor->Operator_Append( pMark );
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
	if ( CheckOpdCount( 1 ) )
	{
		CHE_PDF_ColorSpace * pColorSpace = GetAllocator()->New<CHE_PDF_ColorSpace>( COLORSAPCE_DEVICE_GRAY, GetAllocator() );
		mpConstructor->State_StrokeColorSpace( pColorSpace );
		CHE_PDF_Color * pColor = GetAllocator()->New<CHE_PDF_Color>( GetAllocator() );
		pColor->mConponents.push_back( mOpdFloatStack[0] );
		mpConstructor->State_StrokeColor( pColor );
	}
	mpConstructor->State_StrokeColor( NULL );
}

HE_VOID CHE_PDF_ContentsParser::Handle_ID( CHE_PDF_SyntaxParser * pParser )
{
	if ( pParser == NULL || mbInlineImage == FALSE )
	{
		return;
	}
	HE_BOOL bOver = FALSE;
	HE_DWORD dwRet = 1;
	HE_BYTE byteRet[4];
	std::vector<HE_BYTE> buffer;
	while( !bOver )
	{
		dwRet = pParser->ReadBytes( byteRet, 2 );
		if ( byteRet[0] == '\r' && byteRet[1] == '\n' )
		{ 
			pParser->Seek( 2 );
		}if ( byteRet[0] == '\n' || byteRet[0] == '\t' ||
			  byteRet[0] == '\f' || byteRet[0] == '\0' || byteRet[0] == ' ' )
		{
			pParser->Seek( 1 );
		}
		break;
	}
	while( dwRet )
	{
		dwRet = pParser->ReadBytes( byteRet, 1 );
		if (	byteRet[0] == '\r' || byteRet[0] == '\n' || byteRet[0] == '\t' ||
				byteRet[0] == '\f' || byteRet[0] == '\0' || byteRet[0] == ' ' )
		{
			dwRet = pParser->ReadBytes( byteRet, 4 );
			if ( byteRet[1] == 'E' && byteRet[2] == 'I' )
			{
				pParser->Seek( 1 );
				break;
			}else if ( byteRet[1] == '\n' && byteRet[2] == 'E' && byteRet[3] == 'I' )
			{
				pParser->Seek( 2 );
				break;
			}
			buffer.push_back( byteRet[0] );
			pParser->Seek( 1 );
		}else{
			buffer.push_back( byteRet[0] );
			pParser->Seek( dwRet );
		}
	}


	CHE_PDF_DictionaryPtr pDict = CHE_PDF_Dictionary::Create( GetAllocator() );
	if ( mpFilter )
	{
		pDict->SetAtObj( "Filter", mpFilter );
	}
// 	if ( mpDecode )
// 	{
// 		pDict->SetAtObj( "Decode", mpDecode );
// 	}
	if ( mpDecodeParam )
	{
		pDict->SetAtObj( "DecodeParam", mpDecodeParam );
	}
	pDict->SetAtInteger( "Length", buffer.size() );

	CHE_PDF_ColorSpace * pColorspace = NULL;
	if ( mpColorSpace )
	{
		if ( mpColorSpace->GetType() == OBJ_TYPE_NAME )
		{
			CHE_ByteString str = mpColorSpace->GetNamePtr()->GetString();
			pColorspace = GetColorSpace( str, GetAllocator() );
			if ( pColorspace == NULL )
			{
				CHE_PDF_ObjectPtr pObj = mpContentResMgr->GetResObj( CONTENTRES_COLORSPACE, str );
				if ( ! pObj )
				{
					/*assert(0);*/
					/*error*/
				}
				if ( pObj->GetType() == OBJ_TYPE_NAME )
				{
					pColorspace = GetColorSpace( pObj->GetNamePtr(), GetAllocator() );
				}else if ( pObj->GetType() == OBJ_TYPE_ARRAY )
				{
					pColorspace = GetColorSpace( pObj->GetArrayPtr(), GetAllocator() );
				}else{
					/*assert(0);*/
					/*error*/
				}
			}
		}else if ( mpColorSpace->GetType() == OBJ_TYPE_ARRAY )
		{
			pColorspace = GetColorSpace( mpColorSpace->GetArrayPtr(), GetAllocator() );
		}
		/*assert( 0 );*/
		/*error*/
	}

	CHE_PDF_StreamPtr pStream = CHE_PDF_Stream::Create( &buffer[0], buffer.size(), pDict, 0, 0, NULL, GetAllocator() );
	if ( pStream )
	{
		CHE_PDF_StreamAcc stmAcc( GetAllocator() );
		if ( stmAcc.Attach( pStream ) )
		{
			CHE_PDF_InlineImage * pImage = GetAllocator()->New<CHE_PDF_InlineImage>(	mbMask, mWidth, mHeight, mBpc,
																						stmAcc.GetData(), stmAcc.GetSize(), mpDecode,
																						pColorspace, GetAllocator() );
			mpConstructor->Operator_Append( pImage );
		}
		stmAcc.Detach();
	}
}

HE_VOID CHE_PDF_ContentsParser::Handle_J()
{
	if ( CheckOpdCount( 1 ) )
	{
		PDF_GSTATE_LINECAP lineCap = LineCap_Butt;
		switch ( unsigned int( mOpdFloatStack[0] ) )
		{
		default:
		case 0:
			lineCap = LineCap_Butt;
			break;
		case 1:
			lineCap = LineCap_Round;
			break;
		case 2:
			lineCap = LineCap_Square;
			break;
		}
		mpConstructor->State_LineCap( lineCap );
	}
}

HE_VOID CHE_PDF_ContentsParser::Handle_K()
{
	if ( CheckOpdCount( 4 ) )
	{
		CHE_PDF_ColorSpace * pColorSpace = GetAllocator()->New<CHE_PDF_ColorSpace>( COLORSAPCE_DEVICE_CMYK, GetAllocator() );
		mpConstructor->State_StrokeColorSpace( pColorSpace );
		CHE_PDF_Color * pColor = GetAllocator()->New<CHE_PDF_Color>( GetAllocator() );
		pColor->mConponents.push_back( mOpdFloatStack[0] );
		pColor->mConponents.push_back( mOpdFloatStack[1] );
		pColor->mConponents.push_back( mOpdFloatStack[2] );
		pColor->mConponents.push_back( mOpdFloatStack[3] );
		mpConstructor->State_StrokeColor( pColor );
	}
}
HE_VOID CHE_PDF_ContentsParser::Handle_M()
{
	if ( CheckOpdCount( 1 ) )
	{
		mpConstructor->State_MiterLimit( mOpdFloatStack[0] );
	}
}

HE_VOID CHE_PDF_ContentsParser::Handle_MP()
{
	if ( mName.GetLength() > 0 )
	{
		CHE_PDF_Mark * pMark = GetAllocator()->New<CHE_PDF_Mark>( GetAllocator() );
		pMark->SetMarkType( Mark_MP );
		pMark->SetTag( mName );
		mpConstructor->Operator_Append( pMark );
	}
}

HE_VOID CHE_PDF_ContentsParser::Handle_Q()
{
	mpConstructor->Operator_PopGState();
}

HE_VOID CHE_PDF_ContentsParser::Handle_RG()
{
	if ( CheckOpdCount( 3 ) )
	{
		CHE_PDF_ColorSpace * pColorSpace = GetAllocator()->New<CHE_PDF_ColorSpace>( COLORSAPCE_DEVICE_RGB, GetAllocator() );
		mpConstructor->State_StrokeColorSpace( pColorSpace );
		CHE_PDF_Color * pColor = GetAllocator()->New<CHE_PDF_Color>( GetAllocator() );
		pColor->mConponents.push_back( mOpdFloatStack[0] );
		pColor->mConponents.push_back( mOpdFloatStack[1] );
		pColor->mConponents.push_back( mOpdFloatStack[2] );
		mpConstructor->State_StrokeColor( pColor );
	}
}

HE_VOID CHE_PDF_ContentsParser::Handle_S()
{
	if ( mpPath )
	{
		mpPath->SetPaintType( Paint_Stroke );
		mpConstructor->Operator_Append( mpPath );
		mpPath = NULL;
	}
	if ( mpClipPath )
	{
		mpConstructor->Operator_Clip( mpClipPath );
		mpClipPath = NULL;
	}
}
HE_VOID CHE_PDF_ContentsParser::Handle_SC()
{
	if ( CheckOpdCount( 1 ) )
	{
		CHE_PDF_Color * pColor = GetAllocator()->New<CHE_PDF_Color>( GetAllocator() );
		for ( size_t i = 0; i < mOpdFloatStack.size(); ++i )
		{
			pColor->mConponents.push_back( mOpdFloatStack[i] );
		}
		mpConstructor->State_StrokeColor( pColor );
	}
}

HE_VOID CHE_PDF_ContentsParser::Handle_SCN()
{
	if ( CheckOpdCount( 1 ) )
	{
		CHE_PDF_Color * pColor = GetAllocator()->New<CHE_PDF_Color>( GetAllocator() );
		for ( size_t i = 0; i < mOpdFloatStack.size(); ++i )
		{
			pColor->mConponents.push_back( mOpdFloatStack[i] );
		}
		mpConstructor->State_StrokeColor( pColor );
	}else if ( mName.GetLength() > 0 )
	{
		CHE_PDF_ObjectPtr objPtr;
		objPtr = mpContentResMgr->GetResObj( CONTENTRES_PATTERN, mName );
		if ( IsPdfRefPtr( objPtr ) )
		{
			CHE_PDF_ColorSpace * pColorSpace = GetAllocator()->New<CHE_PDF_ColorSpace>( COLORSAPCE_SPECIAL_PATTERN, mName, objPtr->GetRefPtr()->Clone(), GetAllocator() );
			if ( pColorSpace )
			{
				mpConstructor->State_StrokeColorSpace( pColorSpace );
			}
		}
	}
}

HE_VOID CHE_PDF_ContentsParser::Handle_Tstar()
{
	mpConstructor->Operator_Tstar();
}

HE_VOID CHE_PDF_ContentsParser::Handle_TD()
{
	if ( CheckOpdCount( 2 ) )
	{
		mpConstructor->Operator_TD( mOpdFloatStack[0], mOpdFloatStack[1] );
	}
}

HE_VOID CHE_PDF_ContentsParser::Handle_TJ()
{
	if ( mpObj )
	{
		CHE_PDF_Text * pText = GetAllocator()->New<CHE_PDF_Text>( GetAllocator() );
		mpConstructor->Operator_Append( pText );
		pText->SetTextObject( mpObj );
	}
}

HE_VOID CHE_PDF_ContentsParser::Handle_TL()
{
	if ( CheckOpdCount( 1 ) )
	{
		mpConstructor->State_TextLeading( mOpdFloatStack[0] );
	}
}

HE_VOID CHE_PDF_ContentsParser::Handle_Tc()
{
	if ( CheckOpdCount( 1 ) )
	{
		mpConstructor->State_TextCharSpace( mOpdFloatStack[0] );
	}
}

HE_VOID CHE_PDF_ContentsParser::Handle_Td()
{
	if ( CheckOpdCount( 2 ) )
	{
		mpConstructor->Operator_Td( mOpdFloatStack[0], mOpdFloatStack[1] );
	}
}

HE_VOID CHE_PDF_ContentsParser::Handle_Tf()
{
	if ( CheckOpdCount( 1 ) && mName.GetLength() > 0 )
	{
		//todo load font
		CHE_PDF_ObjectPtr pTmpObj = mpContentResMgr->GetResObj( CONTENTRES_FONT, mName );
		if ( pTmpObj )
		{
			if ( pTmpObj->GetType() == OBJ_TYPE_REFERENCE )
			{
				CHE_PDF_Font * pFont = mpFontMgr->LoadFont( pTmpObj->GetRefPtr() );
				if ( pFont )
				{
					mpConstructor->State_TextFont( mName, pFont );
				}
			}else if ( pTmpObj->GetType() == OBJ_TYPE_DICTIONARY )
			{
				//todo
			}
		}
		mpConstructor->State_TextFontSize( mOpdFloatStack[0] );
	}
}

HE_VOID CHE_PDF_ContentsParser::Handle_Tj()
{
	if ( mpObj )
	{
		CHE_PDF_Text * pText = GetAllocator()->New<CHE_PDF_Text>( GetAllocator() );
		mpConstructor->Operator_Append( pText );
		pText->SetTextObject( mpObj );
	}
}

HE_VOID CHE_PDF_ContentsParser::Handle_Tm()
{
	if ( CheckOpdCount( 6 ) )
	{
		CHE_PDF_Matrix matrix;
		matrix.a = mOpdFloatStack[0];
		matrix.b = mOpdFloatStack[1];
		matrix.c = mOpdFloatStack[2];
		matrix.d = mOpdFloatStack[3];
		matrix.e = mOpdFloatStack[4];
		matrix.f = mOpdFloatStack[5];
		mpConstructor->State_TextMatirx( matrix );
	}
}

HE_VOID CHE_PDF_ContentsParser::Handle_Tr()
{
	if ( CheckOpdCount( 1 ) )
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
			textRm = TextRenderMode_Clip;
			break;
		}
		mpConstructor->State_TextRenderMode( textRm );
	}
}

HE_VOID CHE_PDF_ContentsParser::Handle_Ts()
{
	if ( CheckOpdCount( 1 ) )
	{
		mpConstructor->State_TextRise( mOpdFloatStack[0] );
	}
}

HE_VOID CHE_PDF_ContentsParser::Handle_Tw()
{
	if ( CheckOpdCount( 1 ) )
	{
		mpConstructor->State_TextWordSpace( mOpdFloatStack[0] );
	}
}

HE_VOID CHE_PDF_ContentsParser::Handle_Tz()
{
	if ( CheckOpdCount( 1 ) )
	{
		mpConstructor->State_TextScaling( mOpdFloatStack[0] );
	}
}

HE_VOID CHE_PDF_ContentsParser::Handle_W()
{
	if ( mpClipPath )
	{
		mpClipPath->GetAllocator()->Delete( mpClipPath );
		mpClipPath = NULL;
	}
	if ( mpPath )
	{
		mpClipPath = (CHE_PDF_Path *)( mpPath->Clone() );
		mpClipPath->SetFillMode( Mode_Nonzero );
	}
}

HE_VOID CHE_PDF_ContentsParser::Handle_Wstar()
{
	if ( mpClipPath )
	{
		mpClipPath->GetAllocator()->Delete( mpClipPath );
		mpClipPath = NULL;
	}
	if ( mpPath )
	{
		mpClipPath = (CHE_PDF_Path *)( mpPath->Clone() );
		mpClipPath->SetFillMode( Mode_EvenOdd );
	}
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
		mpConstructor->Operator_Append( mpPath );
		mpPath = NULL;
	}
	if ( mpClipPath )
	{
		mpConstructor->Operator_Clip( mpClipPath );
		mpClipPath = NULL;
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
		mpConstructor->Operator_Append( mpPath );
		mpPath = NULL;
	}
	if ( mpClipPath )
	{
		mpConstructor->Operator_Clip( mpClipPath );
		mpClipPath = NULL;
	}
}

HE_VOID CHE_PDF_ContentsParser::Handle_c()
{
	if ( CheckOpdCount( 6 ) )
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
	if ( CheckOpdCount( 6 ) )
	{
		CHE_PDF_Matrix matrix;
		matrix.a = mOpdFloatStack[0];
		matrix.b = mOpdFloatStack[1];
		matrix.c = mOpdFloatStack[2];
		matrix.d = mOpdFloatStack[3];
		matrix.e = mOpdFloatStack[4];
		matrix.f = mOpdFloatStack[5];
		mpConstructor->State_ConcatMatrix( matrix );
	}
}

HE_VOID CHE_PDF_ContentsParser::Handle_cs()
{
	if ( mName.GetLength() > 0 )
	{
		CHE_PDF_ColorSpace * pColorSpace = NULL;
		if ( mName == "DeviceGray" )
		{
			pColorSpace = GetAllocator()->New<CHE_PDF_ColorSpace>( COLORSAPCE_DEVICE_GRAY, GetAllocator() );
		}else if ( mName == "DeviceRGB" )
		{
			pColorSpace = GetAllocator()->New<CHE_PDF_ColorSpace>( COLORSAPCE_DEVICE_RGB, GetAllocator() );
		}else if ( mName == "DeviceCMYK" )
		{
			pColorSpace = GetAllocator()->New<CHE_PDF_ColorSpace>( COLORSAPCE_DEVICE_CMYK, GetAllocator() );
		}else if ( mName == "Pattern" )
		{
			pColorSpace = GetAllocator()->New<CHE_PDF_ColorSpace>( COLORSAPCE_SPECIAL_PATTERN, GetAllocator() );
		}else{
			CHE_PDF_ObjectPtr pTmpObj = mpContentResMgr->GetResObj( CONTENTRES_COLORSPACE, mName );
			if ( !pTmpObj )
			{
				return;
			}
			if ( pTmpObj->GetType() == OBJ_TYPE_REFERENCE )
			{
				CHE_PDF_ReferencePtr refPtr = pTmpObj->GetRefPtr();
				pTmpObj = refPtr->GetRefObj( OBJ_TYPE_ARRAY );
				if ( ! pTmpObj )
				{
					pTmpObj = refPtr->GetRefObj( OBJ_TYPE_NAME );
				}
			}
			if ( pTmpObj->GetType() == OBJ_TYPE_NAME )
			{
				CHE_PDF_NamePtr namePtr = pTmpObj->GetNamePtr();
				CHE_ByteString name = namePtr->GetString();
				if ( name == "DeviceGray" || name == "G" )
				{
					pColorSpace = GetAllocator()->New<CHE_PDF_ColorSpace>( COLORSAPCE_DEVICE_GRAY, GetAllocator() );
				}else if ( name == "DeviceRGB" || name == "RGB" )
				{
					pColorSpace = GetAllocator()->New<CHE_PDF_ColorSpace>( COLORSAPCE_DEVICE_RGB, GetAllocator() );
				}else if ( name == "DeviceCMYK" || name == "CMYK" )
				{
					pColorSpace = GetAllocator()->New<CHE_PDF_ColorSpace>( COLORSAPCE_DEVICE_CMYK, GetAllocator() );
				}else if ( name == "Pattern" )
				{
					pColorSpace = GetAllocator()->New<CHE_PDF_ColorSpace>( COLORSAPCE_SPECIAL_PATTERN, GetAllocator() );
				}
			}else if ( pTmpObj->GetType() == OBJ_TYPE_ARRAY )
			{
				CHE_PDF_ArrayPtr arrayPtr = pTmpObj->GetArrayPtr();
				pTmpObj = arrayPtr->GetElement( 0, OBJ_TYPE_NAME );
				if ( !pTmpObj )
				{
					return;
				}
				CHE_PDF_NamePtr namePtr = pTmpObj->GetNamePtr();
				CHE_ByteString name = namePtr->GetString();
				if ( name == "DeviceGray" || name == "G" )
				{
					pColorSpace = GetAllocator()->New<CHE_PDF_ColorSpace>( COLORSAPCE_DEVICE_GRAY, GetAllocator() );
				}else if ( name == "DeviceRGB" || name == "RGB" )
				{
					pColorSpace = GetAllocator()->New<CHE_PDF_ColorSpace>( COLORSAPCE_DEVICE_RGB, GetAllocator() );
				}else if ( name == "DeviceCMYK" || name == "CMYK" )
				{
					pColorSpace = GetAllocator()->New<CHE_PDF_ColorSpace>( COLORSAPCE_DEVICE_CMYK, GetAllocator() );
				}else if ( name == "Pattern" )
				{
					pColorSpace = GetAllocator()->New<CHE_PDF_ColorSpace>( COLORSAPCE_SPECIAL_PATTERN, mName, arrayPtr, GetAllocator() );
				}else if ( name == "CalGray" )
				{
					pColorSpace = GetAllocator()->New<CHE_PDF_ColorSpace>( COLORSAPCE_CIEBASE_CALGRAY, mName, arrayPtr, GetAllocator() );
				}else if ( name == "CalRGB" )
				{
					pColorSpace = GetAllocator()->New<CHE_PDF_ColorSpace>( COLORSAPCE_CIEBASE_CALRGB, mName, arrayPtr, GetAllocator() );
				}else if ( name == "CalCMYK" )
				{
					pColorSpace = GetAllocator()->New<CHE_PDF_ColorSpace>( COLORSAPCE_CIEBASE_CALCMYK, mName, arrayPtr, GetAllocator() );
				}else if ( name == "Lab" )
				{
					pColorSpace = GetAllocator()->New<CHE_PDF_ColorSpace>( COLORSAPCE_CIEBASE_CALLAB, mName, arrayPtr, GetAllocator() );
				}else if ( name == "ICCBased" )
				{
					pColorSpace = GetAllocator()->New<CHE_PDF_ColorSpace>( COLORSAPCE_CIEBASE_ICCBASED, mName, arrayPtr, GetAllocator() );
				}else if ( name == "Indexed" || name == "I" )
				{
					pColorSpace = GetAllocator()->New<CHE_PDF_ColorSpace>( COLORSAPCE_SPECIAL_INDEXED, mName, arrayPtr, GetAllocator() );
				}else if ( name == "Separation" )
				{
					pColorSpace = GetAllocator()->New<CHE_PDF_ColorSpace>( COLORSAPCE_SPECIAL_SEPARATION, mName, arrayPtr, GetAllocator() );
				}else if ( name == "DeviceN" )
				{
					pColorSpace = GetAllocator()->New<CHE_PDF_ColorSpace>( COLORSAPCE_SPECIAL_DEVICEN, mName, arrayPtr, GetAllocator() );
				}
			}
		}
		if ( pColorSpace )
		{
			mpConstructor->State_FillColorSpace( pColorSpace );
		}
	}
}

HE_VOID CHE_PDF_ContentsParser::Handle_d()
{
	if ( CheckOpdCount( 1 ) )
	{
		PDF_GSTATE_DASHPATTERN dashPattern;
		dashPattern.dashPhase = mOpdFloatStack[0];

		if ( mpObj && mpObj->GetType() == OBJ_TYPE_ARRAY )
		{
			CHE_PDF_ObjectPtr objPtr;
			CHE_PDF_NumberPtr numberPtr;
			CHE_PDF_ArrayPtr arrayPtr = mpObj->GetArrayPtr();
			for ( HE_DWORD i = 0; i < arrayPtr->GetCount(); ++i )
			{
				objPtr = arrayPtr->GetElement( i );
				if ( objPtr && objPtr->GetType() == OBJ_TYPE_NUMBER )
				{
					numberPtr = objPtr->GetNumberPtr();
					dashPattern.dashArray.push_back( numberPtr->GetFloat() );
				}
			}
		}
		mpConstructor->State_LineDash( dashPattern );
	}
	
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
		mpConstructor->Operator_Append( mpPath );
		mpPath = NULL;
	}
	if ( mpClipPath )
	{
		mpConstructor->Operator_Clip( mpClipPath );
		mpClipPath = NULL;
	}
}

HE_VOID CHE_PDF_ContentsParser::Handle_fstar()
{
	if ( mpPath )
	{
		mpPath->SetFillMode( Mode_EvenOdd );
		mpPath->SetPaintType( Paint_Fill );
		mpConstructor->Operator_Append( mpPath );
		mpPath = NULL;
	}
	if ( mpClipPath )
	{
		mpConstructor->Operator_Clip( mpClipPath );
		mpClipPath = NULL;
	}
}

HE_VOID CHE_PDF_ContentsParser::Handle_g()
{
	if ( CheckOpdCount( 1 ) )
	{
		CHE_PDF_ColorSpace * pColorSpace = GetAllocator()->New<CHE_PDF_ColorSpace>( COLORSAPCE_DEVICE_GRAY, GetAllocator() );
		mpConstructor->State_FillColorSpace( pColorSpace );
		CHE_PDF_Color * pColor = GetAllocator()->New<CHE_PDF_Color>( GetAllocator() );
		pColor->mConponents.push_back( mOpdFloatStack[0] );
		mpConstructor->State_FillColor( pColor );
	}
}

HE_VOID CHE_PDF_ContentsParser::Handle_gs()
{
	if ( mName.GetLength() > 0 )
	{
		CHE_PDF_ObjectPtr objPtr;
		CHE_PDF_DictionaryPtr dictPtr;
		objPtr = mpContentResMgr->GetResObj( CONTENTRES_EXTGSTATE, mName );
		if ( objPtr )
		{
			if ( objPtr->GetType() == OBJ_TYPE_REFERENCE )
			{
				objPtr = objPtr->GetRefPtr()->GetRefObj( OBJ_TYPE_DICTIONARY );
				if ( objPtr )
				{
					dictPtr = objPtr->GetDictPtr();
				}
			}else if ( objPtr->GetType() == OBJ_TYPE_DICTIONARY )
			{
				dictPtr = objPtr->GetDictPtr();
			}
			if ( dictPtr )
			{
				mpConstructor->State_ExtGState( mName, dictPtr );
			}
		}
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
	if ( CheckOpdCount( 1 ) )
	{
		mpConstructor->State_Flatness( mOpdFloatStack[0] );
	}
}

HE_VOID CHE_PDF_ContentsParser::Handle_j()
{
	if ( CheckOpdCount( 1 ) )
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
		mpConstructor->State_LineJoin( lineJoin );
	}
}

HE_VOID CHE_PDF_ContentsParser::Handle_k()
{
	if ( CheckOpdCount( 4 ) )
	{
		CHE_PDF_ColorSpace * pColorSpace = GetAllocator()->New<CHE_PDF_ColorSpace>( COLORSAPCE_DEVICE_CMYK, GetAllocator() );
		mpConstructor->State_FillColorSpace( pColorSpace );
		CHE_PDF_Color * pColor = GetAllocator()->New<CHE_PDF_Color>( GetAllocator() );
		pColor->mConponents.push_back( mOpdFloatStack[0] );
		pColor->mConponents.push_back( mOpdFloatStack[1] );
		pColor->mConponents.push_back( mOpdFloatStack[2] );
		pColor->mConponents.push_back( mOpdFloatStack[3] );
		mpConstructor->State_FillColor( pColor );
	}
}

HE_VOID CHE_PDF_ContentsParser::Handle_l()
{
	if ( CheckOpdCount( 2 ) )
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
	if ( CheckOpdCount( 2 ) )
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
	if ( mpClipPath )
	{
		mpConstructor->Operator_Clip( mpClipPath );
		mpClipPath = NULL;
	}
	if ( mpPath )
	{
		mpPath->GetAllocator()->Delete( mpPath );
		mpPath = NULL;
	}
}

HE_VOID CHE_PDF_ContentsParser::Handle_q()
{
	mpConstructor->Operator_PushGState();
}

HE_VOID CHE_PDF_ContentsParser::Handle_re()
{
	if ( CheckOpdCount( 4 ) )
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
	}
}

HE_VOID CHE_PDF_ContentsParser::Handle_rg()
{
	if ( CheckOpdCount( 3 ) )
	{
		CHE_PDF_ColorSpace * pColorSpace = GetAllocator()->New<CHE_PDF_ColorSpace>( COLORSAPCE_DEVICE_RGB, GetAllocator() );
		mpConstructor->State_FillColorSpace( pColorSpace );
		CHE_PDF_Color * pColor = GetAllocator()->New<CHE_PDF_Color>( GetAllocator() );
		pColor->mConponents.push_back( mOpdFloatStack[0] );
		pColor->mConponents.push_back( mOpdFloatStack[1] );
		pColor->mConponents.push_back( mOpdFloatStack[2] );
		mpConstructor->State_FillColor( pColor );
	}
}

HE_VOID CHE_PDF_ContentsParser::Handle_ri()
{
	if ( mName.GetLength() > 0 )
	{
		if ( mName == "AbsoluteColorimetric" )
		{
			mpConstructor->State_RenderIntents( RI_AbsoluteColorimetric );
		}else if ( mName == "RelativeColorimetric" )
		{
			mpConstructor->State_RenderIntents( RI_RelativeColorimetric );
		}else if ( mName == "Saturation" )
		{
			mpConstructor->State_RenderIntents( RI_Saturation );
		}else if ( mName == "Perceptual" )
		{
			mpConstructor->State_RenderIntents( RI_Perceptual );
		}	 
	}
}

HE_VOID CHE_PDF_ContentsParser::Handle_s()
{
	if ( mpPath )
	{
		CHE_PDF_PathItem pathItem;
		pathItem.type = PathItem_Close;
		mpPath->mItems.push_back( pathItem );
		mpPath->SetPaintType( Paint_Stroke );
		mpConstructor->Operator_Append( mpPath );
		mpPath = NULL;
	}
	if ( mpClipPath )
	{
		mpConstructor->Operator_Clip( mpClipPath );
		mpClipPath = NULL;
	}
}

HE_VOID CHE_PDF_ContentsParser::Handle_sc()
{
	if ( CheckOpdCount( 1 ) )
	{
		CHE_PDF_Color * pColor = GetAllocator()->New<CHE_PDF_Color>( GetAllocator() );
		for ( size_t i = 0; i < mOpdFloatStack.size(); ++i )
		{
			pColor->mConponents.push_back( mOpdFloatStack[i] );
		}
		mpConstructor->State_FillColor( pColor );
	}
}

HE_VOID CHE_PDF_ContentsParser::Handle_scn()
{
	if ( CheckOpdCount( 1 ) )
	{
		CHE_PDF_Color * pColor = GetAllocator()->New<CHE_PDF_Color>( GetAllocator() );
		for ( size_t i = 0; i < mOpdFloatStack.size(); ++i )
		{
			pColor->mConponents.push_back( mOpdFloatStack[i] );
		}
		mpConstructor->State_FillColor( pColor );
	}else if ( mName.GetLength() > 0 )
	{
		CHE_PDF_ObjectPtr objPtr;
		objPtr = mpContentResMgr->GetResObj( CONTENTRES_PATTERN, mName );
		if ( IsPdfRefPtr( objPtr ) )
		{
			CHE_PDF_ColorSpace * pColorSpace = GetAllocator()->New<CHE_PDF_ColorSpace>( COLORSAPCE_SPECIAL_PATTERN, mName, objPtr->GetRefPtr()->Clone(), GetAllocator() );
			if ( pColorSpace )
			{
				mpConstructor->State_FillColorSpace( pColorSpace );
			}
		}
	}
}

HE_VOID CHE_PDF_ContentsParser::Handle_sh()
{
	if ( mName.GetLength() > 0 )
	{
		CHE_PDF_Shading * pShading = GetAllocator()->New<CHE_PDF_Shading>( mName, GetAllocator() );
		mpConstructor->Operator_Append( pShading );
	}
}

HE_VOID CHE_PDF_ContentsParser::Handle_v()
{
	if ( CheckOpdCount( 4 ) )
	{
		if ( mpPath )
		{
			CHE_PDF_PathItem pathItem;
			pathItem.type = PathItem_CurveTo;
			mpPath->mItems.push_back( pathItem );
			pathItem.value = mCurX;
			mpPath->mItems.push_back( pathItem );
			pathItem.value = mCurY;
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
	if ( CheckOpdCount( 1 ) )
	{
		mpConstructor->State_LineWidth( mOpdFloatStack[0] );
	}
}

HE_VOID CHE_PDF_ContentsParser::Handle_y()
{
	if ( CheckOpdCount( 4 ) )
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
			pathItem.value = mOpdFloatStack[2];
			mpPath->mItems.push_back( pathItem );
			pathItem.value = mOpdFloatStack[3];
			mpPath->mItems.push_back( pathItem );
		}
	}
}

class CContentListConstructor : public IHE_PDF_ContentListConstructor
{
public:
	CContentListConstructor( CHE_PDF_ContentObjectList * pList, const CHE_PDF_Matrix & matrix, CHE_Allocator * pAllocator = NULL )
		: mpList(pList), mpGState(NULL), mExtMatrix(matrix), mTextLeading(0), IHE_PDF_ContentListConstructor( pAllocator )
	{
		mpGState = GetAllocator()->New<CHE_PDF_GState>( GetAllocator() );
	}

	~CContentListConstructor()
	{
		if ( mpGState )
		{
			mpGState->GetAllocator()->Delete( mpGState );
		}
		while ( mGStateStack.size() > 0 )
		{
			mpGState = mGStateStack[mGStateStack.size()-1];
			mGStateStack.pop_back();
			if ( mpGState )
			{
				mpGState->GetAllocator()->Delete( mpGState );
			}
		}
	 }

	HE_VOID Init() {}

	HE_VOID Over() {}

	HE_VOID State_Matrix( const CHE_PDF_Matrix & matrix )
	{
		GetGState()->SetMatrix( matrix );
	}

	HE_VOID State_ConcatMatrix( const CHE_PDF_Matrix & matrix )
	{
		CHE_PDF_Matrix tmpMatirx = matrix;
		CHE_PDF_Matrix curMatrix = GetGState()->GetMatrix();
		tmpMatirx.Concat( curMatrix );
		GetGState()->SetMatrix( tmpMatirx );
	}

	HE_VOID State_LineWidth( const HE_FLOAT & lineWidth )
	{
		GetGState()->SetLineWidth( lineWidth );
	}

	HE_VOID State_LineCap( const PDF_GSTATE_LINECAP & lineCap )
	{
		GetGState()->SetLineCap( lineCap );
	}

	HE_VOID State_LineJoin( const PDF_GSTATE_LINEJOIN & lineJoin )
	{
		GetGState()->SetLineJoin( lineJoin );
	}

	HE_VOID State_MiterLimit( const HE_FLOAT & miterLimit )
	{
		GetGState()->SetMiterLimit( miterLimit );
	}

	HE_VOID State_LineDash( const PDF_GSTATE_DASHPATTERN & dashPattern )
	{
		GetGState()->SetLineDash( dashPattern );
	}

	HE_VOID State_RenderIntents( const PDF_GSTATE_RENDERINTENTS & ri )
	{
		GetGState()->SetRenderIntents( ri );
	}

	HE_VOID State_Flatness( const HE_FLOAT & flatness )
	{
		GetGState()->SetFlatness( flatness );
	}

	HE_VOID State_ExtGState( const CHE_ByteString & resName, CHE_PDF_DictionaryPtr dictPtr )
	{
		GetGState()->PushExtGState( resName, dictPtr );
	}

	HE_VOID State_FillColor( CHE_PDF_Color * pColor )
	{
		GetGState()->SetFillColor( pColor );
	}

	HE_VOID State_StrokeColor( CHE_PDF_Color * pColor )
	{
		GetGState()->SetStrokeColor( pColor );
	}

	HE_VOID State_FillColorSpace( CHE_PDF_ColorSpace * pColorSpace )
	{
		GetGState()->SetFillColorSpace( pColorSpace );
	}

	HE_VOID State_StrokeColorSpace( CHE_PDF_ColorSpace * pColorSpace )
	{
		GetGState()->SetStrokeColorSpace( pColorSpace );
	}

	HE_VOID State_TextMatirx( const CHE_PDF_Matrix & matrix )
	{
		GetGState()->SetTextMatrix( matrix );
	}

	HE_VOID State_TextFont( const CHE_ByteString & resName, CHE_PDF_Font * pFont )
	{
		GetGState()->SetTextFontResName( resName );
		if ( pFont )
		{
			GetGState()->SetTextFont( pFont );
		}
	}

	HE_VOID State_TextFontSize( const HE_FLOAT & size )
	{
		GetGState()->SetTextFontSize( size );
	}

	HE_VOID State_TextCharSpace( const HE_FLOAT & charSpace )
	{
		GetGState()->SetTextCharSpace( charSpace );
	}

	HE_VOID State_TextWordSpace( const HE_FLOAT & wordSpace )
	{
		GetGState()->SetTextWordSpace( wordSpace );
	}

	HE_VOID State_TextScaling( const HE_FLOAT & scaling )
	{
		GetGState()->SetTextScaling( scaling );
	}

	HE_VOID State_TextLeading( const HE_FLOAT & leading )
	{
		mTextLeading = leading;
		//GetGState()->SetTextLeading( leading );
	}

	HE_VOID State_TextRise( const HE_FLOAT & rise )
	{
		GetGState()->SetTextRise( rise );
	}

	HE_VOID State_TextRenderMode( const PDF_GSTATE_TEXTRENDERMODE rm )
	{
		GetGState()->SetTextRenderMode( rm );
	}

	HE_VOID Operator_Td( const HE_FLOAT & tx, const HE_FLOAT & ty )
	{
		CHE_PDF_Matrix matrix, tmpMatrix;
		GetGState()->GetTextMatrix( matrix );
		tmpMatrix.e = tx;
		tmpMatrix.f = ty;
		tmpMatrix.Concat( matrix );
		GetGState()->SetTextMatrix( tmpMatrix );
	}

	HE_VOID Operator_TD( const HE_FLOAT & tx, const HE_FLOAT & ty )
	{
		mTextLeading = -ty;
		Operator_Td( tx, ty );
	}

	HE_VOID Operator_Tstar()
	{
		Operator_Td( 0, -mTextLeading );
	}
	
	HE_VOID Operator_PushGState()
	{
		if ( mpGState )
		{
			mGStateStack.push_back( mpGState->Clone() );
		}else
		{
			mGStateStack.push_back( NULL );
		}
	}
	
	HE_VOID Operator_PopGState()
	{
		if ( mpGState )
		{
			mpGState->GetAllocator()->Delete( mpGState );
			mpGState = NULL;
		}
		if ( mGStateStack.size() > 0 )
		{
			mpGState = mGStateStack[mGStateStack.size()-1];
			mGStateStack.pop_back();
		}
	}

	HE_VOID Operator_Clip( CHE_PDF_ContentObject * pObject )
	{
		if ( pObject && ( pObject->GetType() == ContentType_Path || pObject->GetType() == ContentType_Text ) )
		{
			GetGState()->PushClipElement( pObject );
		}
	}

	HE_VOID Operator_Append( CHE_PDF_ContentObject * pObject )
	{
		if ( pObject )
		{
			switch( pObject->GetType() )
			{
			case ContentType_Text:
				{
					if ( mpGState )
					{
						PDF_GSTATE_TEXTRENDERMODE rm = TextRenderMode_Fill;
						mpGState->GetTextRenderMode( rm );
						switch ( rm )
						{
						case TextRenderMode_FillClip:
						case TextRenderMode_StrokeClip:
						case TextRenderMode_FillStrokeClip:
							{
								Operator_Clip( pObject );
								break;
							}
						case TextRenderMode_Clip:
							{
								pObject->SetGState( mpGState->Clone() );
								Operator_Clip( pObject );
								return;
							}
						case TextRenderMode_Fill:
						case TextRenderMode_Stroke:
						case TextRenderMode_FillStroke:
							break;
						default: return;
						}
					}
				}
			case ContentType_Path:
			case ContentType_RefImage:
			case ContentType_InlineImage:
			case ContentType_Form:
			case ContentType_Shading:
				{
					pObject->SetGState( mpGState->Clone() );
					break;
				}
			case ContentType_Mark:
				{
					break;
				}
			default:
				return;
			}
			mpList->Append( pObject );

			CHE_PDF_Matrix tmpMatrix = pObject->GetExtMatrix();
			tmpMatrix.Concat( mExtMatrix );
			pObject->SetExtMatrix( tmpMatrix );
		}
	}

	CHE_PDF_Matrix GetExtMatrix()
	{
		return mExtMatrix;
	}

	CHE_PDF_Matrix GetCurMatrix()
	{
		if ( mpGState )
		{
			return mpGState->GetMatrix();
		}
		return CHE_PDF_Matrix();
	}

private:
	CHE_PDF_GState * GetGState()
	{
		if ( !mpGState )
		{
			mpGState = GetAllocator()->New<CHE_PDF_GState>( GetAllocator() );
		}
		return mpGState;
	}

	CHE_PDF_ContentObjectList * mpList;
	std::vector<CHE_PDF_GState*> mGStateStack;
	CHE_PDF_GState * mpGState;
	CHE_PDF_Matrix mExtMatrix;
	HE_FLOAT mTextLeading;
};

IHE_PDF_ContentListConstructor * CreateConstructor( CHE_PDF_ContentObjectList * plist,
													const CHE_PDF_Matrix & matrix,
													CHE_Allocator * pAllocator /*= NULL*/ )
{
	if ( plist == NULL )
	{
		return NULL;
	}
	if ( pAllocator == NULL )
	{
		return GetDefaultAllocator()->New<CContentListConstructor>( plist, matrix, GetDefaultAllocator() );
	}else
	{
		return pAllocator->New<CContentListConstructor>( plist, matrix, pAllocator );
	}
}

HE_VOID DestoryConstructor( IHE_PDF_ContentListConstructor * pConstructor )
{
	if ( pConstructor )
	{
		CContentListConstructor * pTmpConstructor = (CContentListConstructor *)pConstructor;
		pTmpConstructor->GetAllocator()->Delete( pTmpConstructor );
	}
}