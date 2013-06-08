#include <cassert>
#include "../../include/pdf/che_pdf_filter.h"
#include "../../include/pdf/che_pdf_objectstring.h"
#include "../../include/pdf/che_pdf_contentstring.h"



HE_BOOL CHE_PDF_ContentString::ColorToBuf( const CHE_PDF_Color & color, CHE_DynBuffer & buf )
{
	for ( size_t i = 0; i < color.mConponents.size(); ++i )
	{
		CHE_PDF_ObjectString::FloatToBuf( color.mConponents[i], buf );
		CHE_PDF_ObjectString::StringToBuf( " ", buf );
	}

	return TRUE;
}



HE_BOOL CHE_PDF_ContentString::ColorSpaceToBuf( const CHE_PDF_ColorSpace & colorSpace, CHE_DynBuffer & buf )
{
	switch ( colorSpace.GetType() )
	{
	case COLORSPACE_DEVICE_GRAY:
		CHE_PDF_ObjectString::StringToBuf( "/DeviceGray", buf );
		break;
	case COLORSPACE_DEVICE_RGB:
		CHE_PDF_ObjectString::StringToBuf( "/DeviceRGB", buf );
		break;
	case COLORSPACE_DEVICE_CMYK:
		CHE_PDF_ObjectString::StringToBuf( "/DeviceCMYK", buf );
		break;
	case COLORSPACE_CIEBASE_CALGRAY:
	case COLORSPACE_CIEBASE_CALRGB:
	case COLORSPACE_CIEBASE_CALCMYK:
	case COLORSPACE_CIEBASE_CALLAB:
	case COLORSPACE_CIEBASE_ICCBASED:
	case COLORSPACE_SPECIAL_INDEXED:
	case COLORSPACE_SPECIAL_SEPARATION:
	case COLORSPACE_SPECIAL_DEVICEN:
		{
			CHE_ByteString resName = colorSpace.GetResName();
			if ( resName.GetLength() != 0 )
			{
				CHE_PDF_ObjectString::StringToBuf( "/", buf );
				CHE_PDF_ObjectString::StringToBuf( resName, buf );
			}
			break;
		}
	case COLORSPACE_SPECIAL_PATTERN:
		CHE_PDF_ObjectString::StringToBuf( "/Pattern", buf );
		break;
	default:
		break;
	}

	return TRUE;
}



HE_BOOL CHE_PDF_ContentString::PdfMatrixToBuf( const CHE_Matrix & matrix, CHE_DynBuffer & buf )
{
	CHE_PDF_ObjectString::FloatToBuf( matrix.a, buf );
	CHE_PDF_ObjectString::SpaceToBuf( buf );

	CHE_PDF_ObjectString::FloatToBuf( matrix.b, buf );
	CHE_PDF_ObjectString::SpaceToBuf( buf );

	CHE_PDF_ObjectString::FloatToBuf( matrix.c, buf );
	CHE_PDF_ObjectString::SpaceToBuf( buf );

	CHE_PDF_ObjectString::FloatToBuf( matrix.d, buf );
	CHE_PDF_ObjectString::SpaceToBuf( buf );

	CHE_PDF_ObjectString::FloatToBuf( matrix.e, buf );
	CHE_PDF_ObjectString::SpaceToBuf( buf );

	CHE_PDF_ObjectString::FloatToBuf( matrix.f, buf );

	return TRUE;
}



HE_BOOL CHE_PDF_ContentString::PathDataToBuf( const CHE_PDF_Path * pPath, CHE_DynBuffer & buf )
{
	if ( pPath == NULL )
	{
		return FALSE;
	}

	if ( pPath->mItems.size() == 0 )
	{
		return FALSE;
	}

	for ( size_t i = 0; i < pPath->mItems.size(); ++i )
	{
		switch ( pPath->mItems[i].type )
		{
		case PathItem_MoveTo:
			{
				CHE_PDF_ObjectString::FloatToBuf( pPath->mItems[++i].value, buf );
				CHE_PDF_ObjectString::SpaceToBuf( buf );
				CHE_PDF_ObjectString::FloatToBuf( pPath->mItems[++i].value, buf );
				CHE_PDF_ObjectString::SpaceToBuf( buf );
				CHE_PDF_ObjectString::StringToBuf( "m\n", buf );
				break;
			}
		case PathItem_LineTo:
			{
				CHE_PDF_ObjectString::FloatToBuf( pPath->mItems[++i].value, buf );
				CHE_PDF_ObjectString::SpaceToBuf( buf );
				CHE_PDF_ObjectString::FloatToBuf( pPath->mItems[++i].value, buf );
				CHE_PDF_ObjectString::SpaceToBuf( buf );
				CHE_PDF_ObjectString::StringToBuf( "l\n", buf );
				break;
			}
		case PathItem_CurveTo:
			{
				CHE_PDF_ObjectString::FloatToBuf( pPath->mItems[++i].value, buf );
				CHE_PDF_ObjectString::SpaceToBuf( buf );
				CHE_PDF_ObjectString::FloatToBuf( pPath->mItems[++i].value, buf );
				CHE_PDF_ObjectString::SpaceToBuf( buf );
				CHE_PDF_ObjectString::FloatToBuf( pPath->mItems[++i].value, buf );
				CHE_PDF_ObjectString::SpaceToBuf( buf );
				CHE_PDF_ObjectString::FloatToBuf( pPath->mItems[++i].value, buf );
				CHE_PDF_ObjectString::SpaceToBuf( buf );
				CHE_PDF_ObjectString::FloatToBuf( pPath->mItems[++i].value, buf );
				CHE_PDF_ObjectString::SpaceToBuf( buf );
				CHE_PDF_ObjectString::FloatToBuf( pPath->mItems[++i].value, buf );
				CHE_PDF_ObjectString::SpaceToBuf( buf );
				CHE_PDF_ObjectString::StringToBuf( "c\n", buf );
				break;
			}
		case PathItem_Rectangle:
			{
				CHE_PDF_ObjectString::FloatToBuf( pPath->mItems[++i].value, buf );
				CHE_PDF_ObjectString::SpaceToBuf( buf );
				CHE_PDF_ObjectString::FloatToBuf( pPath->mItems[++i].value, buf );
				CHE_PDF_ObjectString::SpaceToBuf( buf );
				CHE_PDF_ObjectString::FloatToBuf( pPath->mItems[++i].value, buf );
				CHE_PDF_ObjectString::SpaceToBuf( buf );
				CHE_PDF_ObjectString::FloatToBuf( pPath->mItems[++i].value, buf );
				CHE_PDF_ObjectString::SpaceToBuf( buf );
				CHE_PDF_ObjectString::StringToBuf( "re\n", buf );
				break;
			}
		case PathItem_Close:
			{
				CHE_PDF_ObjectString::StringToBuf( "h\n", buf );
				break;
			}
		default:
			return FALSE;
		}
	}

	return TRUE;
}



HE_BOOL CHE_PDF_ContentString::PathToBuf( const CHE_PDF_Path * pPath, CHE_DynBuffer & buf )
{
	if ( pPath == NULL )
	{
		return FALSE;
	}

	PathDataToBuf( pPath, buf );

	switch ( pPath->GetPaintType() )
	{
	case Paint_Fill:
		{
			if ( pPath->GetFillMode() == Mode_Nonzero )
			{
				CHE_PDF_ObjectString::StringToBuf( "f\n", buf ); 
			}else{
				CHE_PDF_ObjectString::StringToBuf( "f*\n", buf );
			}
			break;
		}
	case Paint_Stroke:
		{
			CHE_PDF_ObjectString::StringToBuf( "S\n", buf );
			break;
		}
	case Paint_FillStroke:
		{
			if ( pPath->GetFillMode() == Mode_Nonzero )
			{
				CHE_PDF_ObjectString::StringToBuf( "B\n", buf ); 
			}else{
				CHE_PDF_ObjectString::StringToBuf( "B*\n", buf );
			}
			break;
		}
	case Paint_None:
		{
			CHE_PDF_ObjectString::StringToBuf( "n\n", buf );
			break;
		}
	default:
		return FALSE;
	}

	return TRUE;
}



HE_VOID CHE_PDF_ContentString::TextBlockBeginToBuf( CHE_DynBuffer & buf )
{
	CHE_PDF_ObjectString::StringToBuf( "BT\n", buf );
}



HE_VOID CHE_PDF_ContentString::TextBlockEndToBuf( CHE_DynBuffer & buf )
{
	CHE_PDF_ObjectString::StringToBuf( "ET\n", buf );
}



HE_BOOL CHE_PDF_ContentString::TextToBuf( const CHE_PDF_Text * pText, CHE_DynBuffer & buf )
{
	if ( pText == NULL )
	{
		return FALSE;
	}

	CHE_PDF_ObjectPtr objPtr = pText->GetTextObject();

	if ( IsPdfStringPtr( objPtr ) )
	{
		CHE_PDF_ObjectString::PdfObjPtrToBuf( objPtr, buf );
		CHE_PDF_ObjectString::StringToBuf( " Tj\n", buf );
	}
	else if ( IsPdfArrayPtr( objPtr ) )
	{
		CHE_PDF_ObjectString::PdfObjPtrToBuf( objPtr, buf );
		CHE_PDF_ObjectString::StringToBuf( " TJ\n", buf );
	}else{
		return FALSE;
	}

	return TRUE;
}



HE_BOOL CHE_PDF_ContentString::RefImageToBuf( const CHE_PDF_RefImage * pImage, CHE_DynBuffer & buf )
{
	if ( pImage == NULL )
	{
		return FALSE;
	}

	CHE_ByteString str = pImage->GetName();

	if ( str.GetLength() == 0 )
	{
		return FALSE;
	}

	CHE_PDF_ObjectString::StringToBuf( "/", buf );
	CHE_PDF_ObjectString::StringToBuf( str, buf );
	CHE_PDF_ObjectString::StringToBuf( " Do\n", buf );

	return TRUE;
}



HE_BOOL CHE_PDF_ContentString::InlineImageToBuf( const CHE_PDF_InlineImage * pImage, CHE_DynBuffer & buf )
{
	if ( pImage == NULL )
	{
		return FALSE;
	}

	CHE_PDF_ObjectString::StringToBuf( "BI\n", buf );

	CHE_PDF_ObjectString::StringToBuf( "/W ", buf );
	CHE_PDF_ObjectString::DWORDToBuf( pImage->GetWidth(), buf );
	CHE_PDF_ObjectString::StringToBuf( "\n", buf );

	CHE_PDF_ObjectString::StringToBuf( "/H ", buf );
	CHE_PDF_ObjectString::DWORDToBuf( pImage->GetHight(), buf );
	CHE_PDF_ObjectString::StringToBuf( "\n", buf );

	CHE_PDF_ObjectString::StringToBuf( "/BPC ", buf );
	CHE_PDF_ObjectString::DWORDToBuf( pImage->GetBitps(), buf );
	CHE_PDF_ObjectString::StringToBuf( "\n", buf );

	if ( pImage->IsMask() )
	{
		CHE_PDF_ObjectString::StringToBuf( "/IM true\n", buf );
	}

	if ( pImage->GetColorspace() )
	{
		CHE_PDF_ObjectString::StringToBuf( "/CS /", buf );
		if ( pImage->GetColorspace()->GetResName().GetLength() > 0 )
		{
			CHE_PDF_ObjectString::StringToBuf( pImage->GetColorspace()->GetResName(), buf );
		}else{
			CHE_PDF_ObjectString::StringToBuf( GetColorSpaceName( pImage->GetColorspace() ), buf );
		}
		CHE_PDF_ObjectString::StringToBuf( "\n", buf );
	}

	if ( pImage->GetDecode() )
	{
		CHE_PDF_ObjectString::StringToBuf( "/D ", buf );
		CHE_PDF_ObjectString::PdfObjPtrToBuf( pImage->GetDecode(), buf );
		CHE_PDF_ObjectString::StringToBuf( "\n", buf );
	}

	CHE_PDF_ObjectString::StringToBuf( "/F [/A85 /Fl]\n", buf );

	CHE_PDF_ObjectString::StringToBuf( "ID\n", buf );

	//压缩数据
	CHE_PDF_FlateFilter flateFilter;
	CHE_PDF_ASCII85Filter ascii85Filter;
	CHE_DynBuffer tmpBuf;
	CHE_DynBuffer bynBuf;
	flateFilter.Encode( pImage->GetData(), pImage->GetDataSize(), tmpBuf );
	ascii85Filter.Encode( tmpBuf.GetData(), tmpBuf.GetSize(), bynBuf );
	buf.Write( bynBuf );

	CHE_PDF_ObjectString::StringToBuf( "\nEI\n", buf );

	return TRUE;
}



HE_BOOL CHE_PDF_ContentString::ShadingToBuf( const CHE_PDF_Shading * pShading, CHE_DynBuffer & buf )
{
	if ( pShading == NULL || pShading->GetName().GetLength() == 0 )
	{
		return FALSE;
	}

	CHE_PDF_ObjectString::StringToBuf( "/", buf );
	CHE_PDF_ObjectString::StringToBuf( pShading->GetName(), buf );
	CHE_PDF_ObjectString::StringToBuf( " sh\n", buf );

	return TRUE;
}



HE_BOOL CHE_PDF_ContentString::MarkToBuf( const CHE_PDF_Mark * pMark, CHE_DynBuffer & buf )
{
	if ( pMark == NULL )
	{
		return FALSE;
	}

	switch ( pMark->GetMarkType() )
	{
	case Mark_MP:
		{
			CHE_PDF_ObjectString::StringToBuf( "/", buf );
			CHE_PDF_ObjectString::StringToBuf( pMark->GetTag(), buf );
			CHE_PDF_ObjectString::StringToBuf( " MP\n", buf );
			break;
		}
	case Mark_DP:
		{
			CHE_PDF_ObjectString::StringToBuf( "/", buf );
			CHE_PDF_ObjectString::StringToBuf( pMark->GetTag(), buf );
			CHE_PDF_ObjectString::SpaceToBuf( buf );
			if ( pMark->GetPropertyResName().GetLength() > 0 )
			{
				CHE_PDF_ObjectString::StringToBuf( pMark->GetPropertyResName(), buf );
			}else{
				CHE_PDF_ObjectString::PdfObjPtrToBuf( pMark->GetProperty(), buf );
			}
			CHE_PDF_ObjectString::StringToBuf( " DP\n", buf );
			break;
		}
	case Mark_BMC:
		{
			CHE_PDF_ObjectString::StringToBuf( "/", buf );
			CHE_PDF_ObjectString::StringToBuf( pMark->GetTag(), buf );
			CHE_PDF_ObjectString::StringToBuf( " BMC\n", buf );
			break;
		}
	case Mark_BDC:
		{
			CHE_PDF_ObjectString::StringToBuf( "/", buf );
			CHE_PDF_ObjectString::StringToBuf( pMark->GetTag(), buf );
			CHE_PDF_ObjectString::SpaceToBuf( buf );
			if ( pMark->GetPropertyResName().GetLength() > 0 )
			{
				CHE_PDF_ObjectString::StringToBuf( pMark->GetPropertyResName(), buf );
			}else{
				CHE_PDF_ObjectString::PdfObjPtrToBuf( pMark->GetProperty(), buf );
			}
			CHE_PDF_ObjectString::StringToBuf( " BDC\n", buf );
			break;
		}
	case Mark_EMC:
		{
			CHE_PDF_ObjectString::StringToBuf( "EMC\n", buf );
			break;
		}
	default:
		return FALSE;
	}

	return TRUE;
}



HE_BOOL CHE_PDF_ContentString::FormToBuf( const CHE_PDF_Form * pForm, CHE_DynBuffer & buf )
{
	if ( pForm == NULL )
	{
		return FALSE;
	}

	if ( pForm->GetName().GetLength() > 0 )
	{
		CHE_PDF_ObjectString::StringToBuf( "/", buf );
		CHE_PDF_ObjectString::StringToBuf( pForm->GetName(), buf );
		CHE_PDF_ObjectString::SpaceToBuf( buf );
		CHE_PDF_ObjectString::StringToBuf( " Do\n", buf );

		return TRUE;
	}

	return FALSE;
}



HE_BOOL CHE_PDF_ContentString::TextStateToBuf( const CHE_PDF_GState * pTextState, CHE_DynBuffer & buf, HE_BOOL bNewBlock )
{
	if ( pTextState == NULL )
	{
		return FALSE;
	}

	HE_FLOAT					val = 0;
	CHE_ByteString				name;
	CHE_Matrix				textMatrix;
	GRAPHICS_STATE_TEXTRENDERMODE	textRenderMode = TextRenderMode_Fill;

	pTextState->GetTextCharSpace( val );

	if ( ! IsDefTextCharSpace( val ) )
	{
		CHE_PDF_ObjectString::FloatToBuf( val, buf );
		CHE_PDF_ObjectString::StringToBuf( " Tc\n", buf );
	}

	pTextState->GetTextWordSpace( val );

	if ( ! IsDefTextWrodSpace( val ) )
	{
		CHE_PDF_ObjectString::FloatToBuf( val, buf );
		CHE_PDF_ObjectString::StringToBuf( " Tw\n", buf );
	}

	pTextState->GetTextFontResName( name );
	pTextState->GetTextFontSize( val );

	if ( name.GetLength() > 0 || bNewBlock )
	{
		CHE_PDF_ObjectString::StringToBuf( "/", buf );
		CHE_PDF_ObjectString::StringToBuf( name, buf );
		CHE_PDF_ObjectString::SpaceToBuf( buf );
		CHE_PDF_ObjectString::FloatToBuf( val, buf );
		CHE_PDF_ObjectString::StringToBuf( " Tf\n", buf );
	}

	pTextState->GetTextMatrix( textMatrix );

	if ( ! IsDefMatrix( textMatrix ) || bNewBlock )
	{
		PdfMatrixToBuf( textMatrix, buf );
		CHE_PDF_ObjectString::StringToBuf( " Tm\n", buf );
	}

	pTextState->GetTextScaling( val );

	if ( ! IsDefTextScaling( val ) )
	{
		CHE_PDF_ObjectString::FloatToBuf( val, buf );
		CHE_PDF_ObjectString::StringToBuf( " Tz\n", buf );
	}

	pTextState->GetTextRise( val );

	if ( ! IsDefTextRise( val ) )
	{
		CHE_PDF_ObjectString::FloatToBuf( val, buf );
		CHE_PDF_ObjectString::StringToBuf( " Ts\n", buf );
	}

	pTextState->GetTextRenderMode( textRenderMode );

	if ( ! IsDefTextRenderMode( textRenderMode ) )
	{
		HE_ULONG tmpVal = (HE_ULONG)( textRenderMode );
		CHE_PDF_ObjectString::DWORDToBuf( tmpVal, buf );
		CHE_PDF_ObjectString::StringToBuf( " Tr\n", buf );
	}

	return TRUE;
}

HE_BOOL CHE_PDF_ContentString::TextStateToBuf( const CHE_PDF_GState * pCurTextState, const CHE_PDF_GState * pTargetTextState, CHE_DynBuffer & buf, HE_BOOL bNewBlock )
{
	if ( pCurTextState == NULL )
	{
		return TextStateToBuf( pTargetTextState, buf );
	}

	//zctodo 是否需要输出一个默认的文本图形状态
	if ( pTargetTextState == NULL )
	{
		return FALSE;
	}

	HE_FLOAT					curVal = 0, targetVal = 0;
	CHE_ByteString				curName, targetName;
	CHE_Matrix				curMatrix, targetMatrix;
	GRAPHICS_STATE_TEXTRENDERMODE	curRM = TextRenderMode_Fill, targetRM = TextRenderMode_Fill;

	pCurTextState->GetTextCharSpace( curVal );
	pTargetTextState->GetTextCharSpace( targetVal );

	if ( ! IsFloatEqual( curVal, targetVal ) )
	{
		CHE_PDF_ObjectString::FloatToBuf( targetVal, buf );
		CHE_PDF_ObjectString::StringToBuf( " Tc\n", buf );
	}

	pCurTextState->GetTextWordSpace( curVal );
	pTargetTextState->GetTextWordSpace( targetVal );

	if ( ! IsFloatEqual( curVal, targetVal ) )
	{
		CHE_PDF_ObjectString::FloatToBuf( targetVal, buf );
		CHE_PDF_ObjectString::StringToBuf( " Tw\n", buf );
	}

	pCurTextState->GetTextFontResName( curName );
	pTargetTextState->GetTextFontResName( targetName );
	pCurTextState->GetTextFontSize( curVal );
	pTargetTextState->GetTextFontSize( targetVal );

	if ( curName != targetName || ! IsFloatEqual( curVal, targetVal ) || bNewBlock )
	{
		CHE_PDF_ObjectString::StringToBuf( "/", buf );
		CHE_PDF_ObjectString::StringToBuf( targetName, buf );
		CHE_PDF_ObjectString::SpaceToBuf( buf );
		CHE_PDF_ObjectString::FloatToBuf( targetVal, buf );
		CHE_PDF_ObjectString::StringToBuf( " Tf\n", buf );
	}

	pCurTextState->GetTextMatrix( curMatrix );
	pTargetTextState->GetTextMatrix( targetMatrix );

	if ( ( curMatrix != targetMatrix ) ||  /*( ! IsDefMatrix( targetMatrix ) && */bNewBlock /*)*/ )
	{
		PdfMatrixToBuf( targetMatrix, buf );
		CHE_PDF_ObjectString::StringToBuf( " Tm\n", buf );
	}

	pCurTextState->GetTextScaling( curVal );
	pTargetTextState->GetTextScaling( targetVal );

	if ( ! IsFloatEqual( curVal, targetVal ) )
	{
		CHE_PDF_ObjectString::FloatToBuf( targetVal, buf );
		CHE_PDF_ObjectString::StringToBuf( " Tz\n", buf );
	}

	pCurTextState->GetTextRise( curVal );
	pTargetTextState->GetTextRise( targetVal );

	if ( ! IsFloatEqual( curVal, targetVal ) )
	{
		CHE_PDF_ObjectString::FloatToBuf( targetVal, buf );
		CHE_PDF_ObjectString::StringToBuf( " Ts\n", buf );
	}

	pCurTextState->GetTextRenderMode( curRM );
	pTargetTextState->GetTextRenderMode( targetRM );

	if ( curRM != targetRM )
	{
		HE_ULONG tmpVal = (HE_ULONG)( targetRM );
		CHE_PDF_ObjectString::DWORDToBuf( tmpVal, buf );
		CHE_PDF_ObjectString::StringToBuf( " Tr\n", buf );
	}

	return TRUE;
}



HE_BOOL CHE_PDF_ContentString::ExtGStateToBuf( const CHE_PDF_ExtGState * pExtGState, CHE_DynBuffer & buf )
{
	if ( pExtGState == NULL )
	{
		return FALSE;
	}

	std::list<CHE_ByteString>::const_iterator it = pExtGState->mExtDictNameList.begin();
	for ( ; it != pExtGState->mExtDictNameList.end(); ++it )
	{
		CHE_PDF_ObjectString::StringToBuf( "/", buf );
		CHE_PDF_ObjectString::StringToBuf( *it, buf );
		CHE_PDF_ObjectString::StringToBuf( " gs\n", buf );
	}

	return TRUE;
}



HE_BOOL CHE_PDF_ContentString::ExtGStateToBuf( const CHE_PDF_ExtGState * pCurExtGState, const CHE_PDF_ExtGState * pTargetExtGState, CHE_DynBuffer & buf )
{
	if ( pCurExtGState == NULL && pTargetExtGState == NULL )
	{
		return FALSE;
	}

	if ( pCurExtGState != NULL && pTargetExtGState == NULL )
	{
		return FALSE;
	}

	if ( pCurExtGState == NULL && pTargetExtGState != NULL )
	{
		return ExtGStateToBuf( pTargetExtGState, buf );
	}

	if ( pCurExtGState->mExtDictNameList.size() >= pTargetExtGState->mExtDictNameList.size() )
 	{
		return FALSE;
	}

	std::list<CHE_ByteString>::const_iterator it1, it2;

	it1 = pCurExtGState->mExtDictNameList.begin();
	it2 = pTargetExtGState->mExtDictNameList.begin();

	for ( ; it1 != pCurExtGState->mExtDictNameList.end(); ++it1, ++it2 );

	for ( ; it2 != pTargetExtGState->mExtDictNameList.end(); ++it2 )
	{
		CHE_PDF_ObjectString::StringToBuf( "/", buf );
		CHE_PDF_ObjectString::StringToBuf( *it2, buf );
		CHE_PDF_ObjectString::StringToBuf( " gs\n", buf );
	}

	return TRUE;
}



HE_BOOL CHE_PDF_ContentString::ClipStateToBuf( CHE_Matrix & curMatrix, CHE_PDF_ClipState * pClipState, CHE_DynBuffer & buf, HE_BOOL bInTextBlock )
{
	if ( pClipState == NULL )
	{
		return FALSE;
	}

	CHE_Matrix clipMatrix;
	CHE_PDF_ContentObject * pClipElement = NULL;

	std::list<CHE_PDF_ClipStateItem*>::iterator it;
	for ( it = pClipState->mClipElementList.begin(); it != pClipState->mClipElementList.end(); ++it )
	{
		pClipElement = (*it)->GetElement();
		if ( pClipElement )
		{
			/*clipMatrix = (*it)->GetMatrix();
			if ( clipMatrix != curMatrix )
			{
				CHE_Matrix revertMatrix;
				revertMatrix.Invert( curMatrix );
				clipMatrix.Concat( revertMatrix );

				if ( ! IsDefMatrix( clipMatrix ) )
				{
					PdfMatrixToBuf( clipMatrix, buf );
					CHE_PDF_ObjectString::StringToBuf( " cm\n", buf );
				}

				curMatrix = (*it)->GetMatrix();
			}*/

			if ( pClipElement->GetType() == ContentType_Text )
			{
				if ( ! bInTextBlock )
				{
					TextBlockBeginToBuf( buf );
				}
				CHE_PDF_Text * pTextElement = (CHE_PDF_Text*)( pClipElement );
				TextStateToBuf( pTextElement->GetGState(), buf );
				TextToBuf( pTextElement, buf );
				if ( ! bInTextBlock )
				{
					TextBlockEndToBuf( buf );
				}
			}else if ( pClipElement->GetType() == ContentType_Path )
			{
				CHE_PDF_Path * pPath = (CHE_PDF_Path*)( pClipElement );
				PathDataToBuf( pPath, buf );
				if ( pPath->GetFillMode() == Mode_Nonzero )
				{
					CHE_PDF_ObjectString::StringToBuf( "W n\n", buf );
				}else if ( pPath->GetFillMode() == Mode_EvenOdd )
				{
					CHE_PDF_ObjectString::StringToBuf( "W* n\n", buf );
				}
			}
		}
	}

	return TRUE; 
}



HE_BOOL CHE_PDF_ContentString::ClipStateToBuf(	CHE_Matrix & curMatrix, CHE_PDF_ClipState * pCurClipState,
												const CHE_PDF_ClipState * pTargetClipState, CHE_DynBuffer & buf, HE_BOOL bInTextBlock )
{
// 	if ( pClipState1 == NULL && pClipState2 == NULL )
// 	{
// 		return FALSE;
// 	}
// 
// 	if ( pClipState1 != NULL && pClipState2 == NULL )
// 	{
// 		return FALSE;
// 	}
// 
// 	if ( pExtGState1 == NULL && pExtGState2 != NULL )
// 	{
// 		std::list<CHE_ByteString>::const_iterator it = pExtGState2->mExtDictNameList.begin();
// 		for ( ; it != pExtGState2->mExtDictNameList.end(); ++it )
// 		{
// 			CHE_PDF_ObjectString::StringToBuf( "/", buf );
// 			CHE_PDF_ObjectString::StringToBuf( *it, buf );
// 			CHE_PDF_ObjectString::StringToBuf( " gs\n", buf );
// 		}
// 		return TRUE;
// 	}
// 
// 	if ( pExtGState1->mExtDictNameList.size() >= pExtGState2->mExtDictNameList.size() )
// 	{
// 		return FALSE;
// 	}
// 
// 	std::list<CHE_ByteString>::const_iterator it1, it2;
// 	it1 = pExtGState1->mExtDictNameList.begin();
// 	it2 = pExtGState2->mExtDictNameList.begin();
// 
// 	for ( ; it1 != pExtGState1->mExtDictNameList.end(); ++it1, ++it2 );
// 
// 	for ( ; it2 != pExtGState2->mExtDictNameList.end(); ++it2 )
// 	{
// 		CHE_PDF_ObjectString::StringToBuf( "/", buf );
// 		CHE_PDF_ObjectString::StringToBuf( *it2, buf );
// 		CHE_PDF_ObjectString::StringToBuf( " gs\n", buf );
// 	}
	return TRUE;
}



HE_BOOL CHE_PDF_ContentString::GStateToBuf( CHE_PDF_GState * & pGSData, CHE_DynBuffer & buf, CHE_Stack<CHE_PDF_GState*> & gstack, HE_BOOL bInTextBlock )
{
	if ( ! pGSData )
	{
		pGSData = GetDefaultAllocator()->New<CHE_PDF_GState>( GetDefaultAllocator() );
	}

	HE_BOOL						bNewTextBlock = FALSE;
	HE_BOOL						bNeedStackPush = FALSE;
	HE_FLOAT					floatVal = 0;
	GRAPHICS_STATE_LINECAP			lineCap = LineCap_Butt;
	GRAPHICS_STATE_LINEJOIN			lineJoin = LineJoin_Miter;
	GRAPHICS_STATE_DASHPATTERN		lineDash;
	GRAPHICS_STATE_RENDERINTENTS	ri = RI_AbsoluteColorimetric;

	CHE_PDF_ClipState * pClipState = pGSData->GetClipState();
	if ( pClipState && pClipState->mClipElementList.size() > 0 )
	{
		bNeedStackPush = TRUE;
	}

	CHE_PDF_ExtGState * pExtState = pGSData->GetExtGState();
	if ( pExtState && pExtState->mExtDictNameList.size() > 0 )
	{
		bNeedStackPush = TRUE;
	}

	if ( bNeedStackPush )
	{
		if ( bInTextBlock )
		{
			bNewTextBlock = TRUE;
			TextBlockEndToBuf( buf );
		}

		CHE_PDF_ObjectString::StringToBuf( "q\n", buf );
		gstack.Push( NULL );
	}

	CHE_Matrix curMatrix;

	if ( pClipState != NULL )
	{
		ClipStateToBuf( curMatrix, pClipState, buf, bInTextBlock );
	}

	CHE_Matrix target = pGSData->GetMatrix();
	if ( IsDefMatrix( curMatrix ) )
	{
		if ( target != curMatrix )
		{
			if ( bInTextBlock && ! bNewTextBlock )
			{
				bNewTextBlock = TRUE;
				TextBlockEndToBuf( buf );
			}
			PdfMatrixToBuf( target, buf );
			CHE_PDF_ObjectString::StringToBuf( " cm\n", buf );
		}
	}else if ( target != curMatrix )
	{
		if ( bInTextBlock && ! bNewTextBlock )
		{
			bNewTextBlock = TRUE;
			TextBlockEndToBuf( buf );
		}
		CHE_Matrix revertMatrix;
		revertMatrix.Invert( curMatrix );
		target.Concat( revertMatrix );
		PdfMatrixToBuf( target, buf );
		CHE_PDF_ObjectString::StringToBuf( " cm\n", buf );
	}

	ExtGStateToBuf( pExtState, buf );

	pGSData->GetLineWidth( floatVal );

	if ( ! IsDefLineWidth( floatVal ) )
	{
		CHE_PDF_ObjectString::FloatToBuf( floatVal, buf );
		CHE_PDF_ObjectString::StringToBuf( " w\n", buf );
	}

	pGSData->GetMiterLimit( floatVal );

	if ( ! IsDefMiterLimit( floatVal ) )
	{
		CHE_PDF_ObjectString::FloatToBuf( floatVal, buf );
		CHE_PDF_ObjectString::StringToBuf( " M\n", buf );
	}
	
	pGSData->GetLineCap( lineCap );

	if ( ! IsDefLineCap( lineCap ) )
	{
		switch ( lineCap )
		{
		case LineCap_Butt:
			CHE_PDF_ObjectString::IntegerToBuf( 0, buf );
			CHE_PDF_ObjectString::StringToBuf( " J\n", buf );
			break;
		case LineCap_Round:
			CHE_PDF_ObjectString::IntegerToBuf( 1, buf );
			CHE_PDF_ObjectString::StringToBuf( " J\n", buf );
			break;
		case LineCap_Square:
			CHE_PDF_ObjectString::IntegerToBuf( 2, buf );
			CHE_PDF_ObjectString::StringToBuf( " J\n", buf );
			break;
		default:
			break;
		}
	}

	pGSData->GetLineJoin( lineJoin );

	if ( ! IsDefLineJoin( lineJoin ) )
	{
		switch ( lineJoin )
		{
		case LineJoin_Miter:
			CHE_PDF_ObjectString::IntegerToBuf( 0, buf );
			CHE_PDF_ObjectString::StringToBuf( " j\n", buf );
			break;
		case LineJoin_Round:
			CHE_PDF_ObjectString::IntegerToBuf( 1, buf );
			CHE_PDF_ObjectString::StringToBuf( " j\n", buf );
		case LineJoin_Bevel:
			CHE_PDF_ObjectString::IntegerToBuf( 2, buf );
			CHE_PDF_ObjectString::StringToBuf( " j\n", buf );
		default:
			break;
		}
	}

	pGSData->GetLineDash( lineDash );

	if ( ! IsDefLineDash( lineDash ) )
	{
		CHE_PDF_ObjectString::StringToBuf( "[", buf );
		for ( size_t i = 0; lineDash.dashArray.size(); ++i )
		{
			CHE_PDF_ObjectString::StringToBuf( " ", buf );
			CHE_PDF_ObjectString::FloatToBuf( lineDash.dashArray[i], buf );
		}
		CHE_PDF_ObjectString::StringToBuf( " ] ", buf );
		CHE_PDF_ObjectString::FloatToBuf( lineDash.dashPhase, buf );
		CHE_PDF_ObjectString::StringToBuf( " d\n", buf );
	}

	ri = pGSData->GetRenderIntents();
	if ( ! IsDefRenderIntents( ri ) )
	{
		switch ( ri )
		{
		case RI_AbsoluteColorimetric:
			CHE_PDF_ObjectString::IntegerToBuf( 0, buf );
			CHE_PDF_ObjectString::StringToBuf( " ri\n", buf );
			break;
		case RI_RelativeColorimetric:
			CHE_PDF_ObjectString::IntegerToBuf( 1, buf );
			CHE_PDF_ObjectString::StringToBuf( " ri\n", buf );
			break;
		case RI_Saturation:
			CHE_PDF_ObjectString::IntegerToBuf( 2, buf );
			CHE_PDF_ObjectString::StringToBuf( " ri\n", buf );
			break;
		case RI_Perceptual:
			CHE_PDF_ObjectString::IntegerToBuf( 3, buf );
			CHE_PDF_ObjectString::StringToBuf( " ri\n", buf );
			break;
		default:
			break;
		}
	}

	floatVal = pGSData->GetFlatness();
	if ( ! IsDefFlatness( floatVal )  )
	{
		CHE_PDF_ObjectString::FloatToBuf( floatVal, buf );
		CHE_PDF_ObjectString::StringToBuf( " i\n", buf );
	}
	
	CHE_PDF_ColorSpace fillColorSpace( COLORSPACE_DEVICE_GRAY );
	CHE_PDF_Color fillColor;
	pGSData->GetFillColorSpace( fillColorSpace );
	pGSData->GetFillColor( fillColor );

	if ( ! IsDefColorSpace( fillColorSpace ) || ! IsDefColor( fillColor ) )
	{
		if ( ! fillColorSpace.IsDeviceColorSpace() )
		{
			ColorSpaceToBuf( fillColorSpace, buf );
			CHE_PDF_ObjectString::StringToBuf( " cs\n", buf );
		}

		if ( fillColor.mConponents.size() > 0 )
		{
			ColorToBuf( fillColor,  buf );

			switch (  fillColorSpace.GetType() )
			{
			case COLORSPACE_DEVICE_GRAY:
				CHE_PDF_ObjectString::StringToBuf( "g\n", buf );
				break;
			case COLORSPACE_DEVICE_RGB:
				CHE_PDF_ObjectString::StringToBuf( "rg\n", buf );
				break;
			case COLORSPACE_DEVICE_CMYK:
				CHE_PDF_ObjectString::StringToBuf( "k\n", buf );
				break;
			case COLORSPACE_CIEBASE_CALGRAY:
			case COLORSPACE_CIEBASE_CALRGB:
			case COLORSPACE_CIEBASE_CALCMYK:
			case COLORSPACE_CIEBASE_CALLAB:
				CHE_PDF_ObjectString::StringToBuf( "sc\n", buf );
				break;
			case COLORSPACE_CIEBASE_ICCBASED:
			case COLORSPACE_SPECIAL_INDEXED:
			case COLORSPACE_SPECIAL_SEPARATION:
			case COLORSPACE_SPECIAL_DEVICEN:
				CHE_PDF_ObjectString::StringToBuf( "scn\n", buf );
				break;
			}
		}else if ( fillColorSpace.GetType() == COLORSPACE_SPECIAL_PATTERN )
		{
			CHE_PDF_ObjectString::StringToBuf( "/", buf );
			CHE_PDF_ObjectString::StringToBuf( fillColorSpace.GetResName(), buf );
			CHE_PDF_ObjectString::StringToBuf( " scn\n", buf );
		}
	}

	CHE_PDF_ColorSpace strokeColorSpace( COLORSPACE_DEVICE_GRAY );
	CHE_PDF_Color strokeColor;
	pGSData->GetStrokeColorSpace( strokeColorSpace );
	pGSData->GetStrokeColor( strokeColor );

	if ( ! IsDefColorSpace( strokeColorSpace ) || ! IsDefColor( strokeColor ) )
	{
		if ( ! strokeColorSpace.IsDeviceColorSpace() )
		{
			ColorSpaceToBuf( strokeColorSpace, buf );
			CHE_PDF_ObjectString::StringToBuf( " CS\n", buf );
		}

		if ( strokeColor.mConponents.size() > 0 )
		{
			ColorToBuf( strokeColor,  buf );

			switch (  strokeColorSpace.GetType() )
			{
			case COLORSPACE_DEVICE_GRAY:
				CHE_PDF_ObjectString::StringToBuf( "G\n", buf );
				break;
			case COLORSPACE_DEVICE_RGB:
				CHE_PDF_ObjectString::StringToBuf( "RG\n", buf );
				break;
			case COLORSPACE_DEVICE_CMYK:
				CHE_PDF_ObjectString::StringToBuf( "K\n", buf );
				break;
			case COLORSPACE_CIEBASE_CALGRAY:
			case COLORSPACE_CIEBASE_CALRGB:
			case COLORSPACE_CIEBASE_CALCMYK:
			case COLORSPACE_CIEBASE_CALLAB:
				CHE_PDF_ObjectString::StringToBuf( "SC\n", buf );
				break;
			case COLORSPACE_SPECIAL_PATTERN:
				CHE_PDF_ObjectString::StringToBuf( "/", buf );
				CHE_PDF_ObjectString::StringToBuf( strokeColorSpace.GetResName(), buf );
				CHE_PDF_ObjectString::SpaceToBuf( buf );
			case COLORSPACE_CIEBASE_ICCBASED:
			case COLORSPACE_SPECIAL_INDEXED:
			case COLORSPACE_SPECIAL_SEPARATION:
			case COLORSPACE_SPECIAL_DEVICEN:
				CHE_PDF_ObjectString::StringToBuf( "SCN\n", buf );
				break;
			}
		}
	}

	if ( bInTextBlock )
	{
		if ( bNewTextBlock )
		{
			TextBlockBeginToBuf( buf );
		}
		TextStateToBuf( pGSData, buf, bNewTextBlock );
	}

	return TRUE; 
}

HE_BOOL CHE_PDF_ContentString::GStateToBuf( CHE_PDF_GState * & pCurGSData, CHE_PDF_GState * & pTargetGSData,
											CHE_DynBuffer & buf, CHE_Stack<CHE_PDF_GState*> & stack, HE_BOOL bInTextBlock )
{
	if ( ! pTargetGSData )
	{
		pTargetGSData = GetDefaultAllocator()->New<CHE_PDF_GState>( GetDefaultAllocator() );
	}

	if ( ! pCurGSData )
	{
		return GStateToBuf( pTargetGSData, buf, stack, bInTextBlock );
	}

	HE_BOOL						bNewTextBlock = FALSE;
	HE_FLOAT					curVal = 0, targetVal = 0;
	GRAPHICS_STATE_LINECAP			curLineCap = LineCap_Butt, targetLineCap = LineCap_Butt;
	GRAPHICS_STATE_LINEJOIN			curLineJoin = LineJoin_Miter, targetLineJoin = LineJoin_Miter;
	GRAPHICS_STATE_DASHPATTERN		curLineDash, targetLineDash;
	GRAPHICS_STATE_RENDERINTENTS	curRI = RI_AbsoluteColorimetric, targetRI = RI_AbsoluteColorimetric;

	CHE_Matrix curMatrix = pCurGSData->GetMatrix();

	CHE_PDF_ExtGState * pCurExtGState = pCurGSData->GetExtGState();
	CHE_PDF_ExtGState * pTargetExtGState = pTargetGSData->GetExtGState();

	//根据ExtGState判断图形状态需不需要出栈，如果需要则设置好出堆栈后的当前图形状态数据
	if ( ! IsExtGStateEqual( pCurExtGState, pTargetExtGState ) )
	{
		if ( IsExtGStateContinue( pCurExtGState, pTargetExtGState ) )
		{
			if ( ! pCurGSData )
			{
				stack.Push( NULL );
			}else{
				stack.Push( pCurGSData->Clone() );
			}
			CHE_PDF_ObjectString::StringToBuf( "q\n", buf );
		}else{
			HE_BOOL	bRet = TRUE;
			CHE_PDF_GState * pTmpGSData = NULL;
			while( bRet )
			{
				if ( pTmpGSData )
				{
					pTmpGSData->GetAllocator()->Delete( pTmpGSData );
					pTmpGSData = NULL;
				}
				bRet = stack.Pop( pTmpGSData );
				if ( ! bRet )
				{
					break;
				}
				CHE_PDF_ObjectString::StringToBuf( "Q\n", buf );

				if ( pCurGSData )
				{
					pCurGSData->GetAllocator()->Delete( pCurGSData );
				}
				pCurGSData = pTmpGSData;

				pCurExtGState = ( pCurGSData ) ? pCurGSData->GetExtGState() : NULL; 

				if (	pCurGSData == NULL || IsExtGStateEqual( pCurExtGState, pTargetExtGState ) ||
						IsExtGStateContinue( pCurExtGState, pTargetExtGState ) )
				{
					break;
				}
			}

			if ( ! pCurGSData )
			{
				pCurGSData = GetDefaultAllocator()->New<CHE_PDF_GState>( GetDefaultAllocator() );
			}
			stack.Push( pCurGSData->Clone() );
			CHE_PDF_ObjectString::StringToBuf( "q\n", buf );

			curMatrix = pCurGSData->GetMatrix();
		}
	}
	if ( pCurGSData == NULL )
	{
		pCurGSData = GetDefaultAllocator()->New<CHE_PDF_GState>();
	}

	CHE_PDF_ClipState * pCurClipState = pCurGSData->GetClipState();
	CHE_PDF_ClipState * pTargetClipState = pTargetGSData->GetClipState();

	if ( ! IsClipStateEqual( pCurClipState, pTargetClipState ) )
	{
		if ( bInTextBlock )
		{
			bNewTextBlock = TRUE;
			TextBlockEndToBuf( buf );
		}
		if ( IsClipStateContinue( pCurClipState, pTargetClipState ) )
		{
			//zctodo
		}else{
			HE_BOOL	bRet = TRUE;
			CHE_PDF_GState * pTmpGSData = NULL;
			while( bRet )
			{
				if ( pTmpGSData )
				{
					pTmpGSData->GetAllocator()->Delete( pTmpGSData );
					pTmpGSData = NULL;
				}
				bRet = stack.Pop( pTmpGSData );
				if ( ! bRet )
				{
					break;
				}
				CHE_PDF_ObjectString::StringToBuf( "Q\n", buf );

				if ( pCurGSData )
				{
					pCurGSData->GetAllocator()->Delete( pCurGSData );
				}
				pCurGSData = pTmpGSData;

				pCurClipState = ( pCurGSData ) ? pCurGSData->GetClipState() : NULL; 

				if (	pCurGSData == NULL || pCurClipState == NULL || IsClipStateEqual( pCurClipState, pTargetClipState ) ||
						IsClipStateContinue( pCurClipState, pTargetClipState ) )
				{
					break;
				}
			}
			if ( ! pCurGSData )
			{
				pCurGSData = GetDefaultAllocator()->New<CHE_PDF_GState>( GetDefaultAllocator() );
			}
			stack.Push( pCurGSData->Clone() );
			CHE_PDF_ObjectString::StringToBuf( "q\n", buf );
			
			curMatrix = pCurGSData->GetMatrix();
		}
	}

	pCurExtGState = pCurGSData->GetExtGState();
	pTargetExtGState = pTargetGSData->GetExtGState();

	pCurClipState = pCurGSData->GetClipState();
	pTargetClipState = pTargetGSData->GetClipState();

	if ( pCurClipState )
	{
		CHE_PDF_ContentString::ClipStateToBuf( curMatrix, pCurClipState, pTargetClipState, buf, bInTextBlock );
	}else{
		CHE_PDF_ContentString::ClipStateToBuf( curMatrix, pTargetClipState, buf, bInTextBlock );
	}

	CHE_PDF_ContentString::ExtGStateToBuf( pCurExtGState, pTargetExtGState, buf );

	CHE_Matrix targetMatrix = pTargetGSData->GetMatrix();

	if ( curMatrix != targetMatrix )
	{
		if ( bInTextBlock && ! bNewTextBlock )
		{
			bNewTextBlock = TRUE;
			TextBlockEndToBuf( buf );
		}

		if ( ! IsDefMatrix( curMatrix ) )
		{
			CHE_Matrix revertMatrix;
			revertMatrix.Invert( curMatrix );
			targetMatrix.Concat( revertMatrix );
		}

		PdfMatrixToBuf( targetMatrix, buf );
		CHE_PDF_ObjectString::StringToBuf( " cm\n", buf );
	}

	pCurGSData->GetLineWidth( curVal );
	pTargetGSData->GetLineWidth( targetVal );

	if ( ! IsFloatEqual( curVal, targetVal ) )
	{
		CHE_PDF_ObjectString::FloatToBuf( targetVal, buf );
		CHE_PDF_ObjectString::StringToBuf( " w\n", buf );
	}

	pCurGSData->GetMiterLimit( curVal );
	pTargetGSData->GetMiterLimit( targetVal );

	if ( ! IsFloatEqual( curVal, targetVal ) )
	{
		CHE_PDF_ObjectString::FloatToBuf( targetVal, buf );
		CHE_PDF_ObjectString::StringToBuf( " M\n", buf );
	}

	pCurGSData->GetLineCap( curLineCap );
	pTargetGSData->GetLineCap( targetLineCap );

	if ( curLineCap != targetLineCap )
	{
		switch ( targetLineCap )
		{
		case LineCap_Butt:
			CHE_PDF_ObjectString::IntegerToBuf( 0, buf );
			CHE_PDF_ObjectString::StringToBuf( " J\n", buf );
			break;
		case LineCap_Round:
			CHE_PDF_ObjectString::IntegerToBuf( 1, buf );
			CHE_PDF_ObjectString::StringToBuf( " J\n", buf );
			break;
		case LineCap_Square:
			CHE_PDF_ObjectString::IntegerToBuf( 2, buf );
			CHE_PDF_ObjectString::StringToBuf( " J\n", buf );
			break;
		default:
			break;
		}
	}

	pCurGSData->GetLineJoin( curLineJoin );
	pTargetGSData->GetLineJoin( targetLineJoin );

	if ( curLineJoin != targetLineJoin )
	{
		switch ( targetLineJoin )
		{
		case LineJoin_Miter:
			CHE_PDF_ObjectString::IntegerToBuf( 0, buf );
			CHE_PDF_ObjectString::StringToBuf( " j\n", buf );
			break;
		case LineJoin_Round:
			CHE_PDF_ObjectString::IntegerToBuf( 1, buf );
			CHE_PDF_ObjectString::StringToBuf( " j\n", buf );
			break;
		case LineJoin_Bevel:
			CHE_PDF_ObjectString::IntegerToBuf( 2, buf );
			CHE_PDF_ObjectString::StringToBuf( " j\n", buf );
		default:
			break;
		}
	}

	pCurGSData->GetLineDash( curLineDash );
	pTargetGSData->GetLineDash( targetLineDash );

	if (	! IsFloatEqual( curLineDash.dashPhase, targetLineDash.dashPhase ) ||
			curLineDash.dashArray.size() != targetLineDash.dashArray.size() )
	{
		CHE_PDF_ObjectString::StringToBuf( "[", buf );
		for ( HE_ULONG i = 0; i < targetLineDash.dashArray.size(); ++i )
		{
			CHE_PDF_ObjectString::StringToBuf( " ", buf );
			CHE_PDF_ObjectString::FloatToBuf( targetLineDash.dashArray[i], buf );
		}
		CHE_PDF_ObjectString::StringToBuf( " ] ", buf );
		CHE_PDF_ObjectString::FloatToBuf( targetLineDash.dashPhase, buf );
		CHE_PDF_ObjectString::StringToBuf( " d\n", buf );
	}else{
		HE_BOOL bSame = TRUE;
		for ( HE_ULONG i = 0; i < targetLineDash.dashArray.size(); ++i )
		{
			if ( ! IsFloatEqual( curLineDash.dashArray[i], targetLineDash.dashArray[i] ) )
			{
				bSame = FALSE;
				break;
			}
		}
		if ( ! bSame )
		{
			CHE_PDF_ObjectString::StringToBuf( "[", buf );
			for ( size_t i = 0; targetLineDash.dashArray.size(); ++i )
			{
				CHE_PDF_ObjectString::StringToBuf( " ", buf );
				CHE_PDF_ObjectString::FloatToBuf( targetLineDash.dashArray[i], buf );
			}
			CHE_PDF_ObjectString::StringToBuf( " ] ", buf );
			CHE_PDF_ObjectString::FloatToBuf( targetLineDash.dashPhase, buf );
			CHE_PDF_ObjectString::StringToBuf( " d\n", buf );
		}
	}

	curRI = pCurGSData->GetRenderIntents();
	targetRI = pTargetGSData->GetRenderIntents();

	if ( curRI != targetRI )
	{
		switch ( targetRI )
		{
		case RI_AbsoluteColorimetric:
			CHE_PDF_ObjectString::IntegerToBuf( 0, buf );
			CHE_PDF_ObjectString::StringToBuf( " ri\n", buf );
			break;
		case RI_RelativeColorimetric:
			CHE_PDF_ObjectString::IntegerToBuf( 1, buf );
			CHE_PDF_ObjectString::StringToBuf( " ri\n", buf );
			break;
		case RI_Saturation:
			CHE_PDF_ObjectString::IntegerToBuf( 2, buf );
			CHE_PDF_ObjectString::StringToBuf( " ri\n", buf );
			break;
		case RI_Perceptual:
			CHE_PDF_ObjectString::IntegerToBuf( 3, buf );
			CHE_PDF_ObjectString::StringToBuf( " ri\n", buf );
			break;
		default:
			break;
		}
	}

	curVal = pCurGSData->GetFlatness();
	targetVal = pTargetGSData->GetFlatness();

	if ( ! IsFloatEqual( curVal, targetVal ) )
	{
		CHE_PDF_ObjectString::FloatToBuf( targetVal, buf );
		CHE_PDF_ObjectString::StringToBuf( " i\n", buf );
	}

	CHE_PDF_ColorSpace curFillColorSpace( COLORSPACE_DEVICE_GRAY ), targetFillColorSpace( COLORSPACE_DEVICE_GRAY );
	CHE_PDF_Color curFillColor, targetFillColor;

	pCurGSData->GetFillColorSpace( curFillColorSpace );
	pTargetGSData->GetFillColorSpace( targetFillColorSpace );
	pCurGSData->GetFillColor( curFillColor );
	pTargetGSData->GetFillColor( targetFillColor );

	if ( ! IsColorSpaceEqual( curFillColorSpace, targetFillColorSpace ) || ! IsColorEqual( curFillColor, targetFillColor ) )
	{
		if ( ! targetFillColorSpace.IsDeviceColorSpace() )
		{
			ColorSpaceToBuf( targetFillColorSpace, buf );
			CHE_PDF_ObjectString::StringToBuf( " cs\n", buf );
		}

		if ( targetFillColor.mConponents.size() > 0 )
		{
			ColorToBuf( targetFillColor, buf );

			switch (  targetFillColorSpace.GetType() )
			{
			case COLORSPACE_DEVICE_GRAY:
				CHE_PDF_ObjectString::StringToBuf( "g\n", buf );
				break;
			case COLORSPACE_DEVICE_RGB:
				CHE_PDF_ObjectString::StringToBuf( "rg\n", buf );
				break;
			case COLORSPACE_DEVICE_CMYK:
				CHE_PDF_ObjectString::StringToBuf( "k\n", buf );
				break;
			case COLORSPACE_CIEBASE_CALGRAY:
			case COLORSPACE_CIEBASE_CALRGB:
			case COLORSPACE_CIEBASE_CALCMYK:
			case COLORSPACE_CIEBASE_CALLAB:
				CHE_PDF_ObjectString::StringToBuf( "sc\n", buf );
				break;
			case COLORSPACE_CIEBASE_ICCBASED:
			case COLORSPACE_SPECIAL_INDEXED:
			case COLORSPACE_SPECIAL_SEPARATION:
			case COLORSPACE_SPECIAL_DEVICEN:
				CHE_PDF_ObjectString::StringToBuf( "scn\n", buf );
				break;
			}
		}else if ( targetFillColorSpace.GetType() == COLORSPACE_SPECIAL_PATTERN )
		{
			CHE_PDF_ObjectString::StringToBuf( "/", buf );
			CHE_PDF_ObjectString::StringToBuf( targetFillColorSpace.GetResName(), buf );
			CHE_PDF_ObjectString::StringToBuf( " scn\n", buf );
		}
	}

	CHE_PDF_ColorSpace curStrokeColorSpace( COLORSPACE_DEVICE_GRAY ), targetStrokeColorSpace( COLORSPACE_DEVICE_GRAY );
	CHE_PDF_Color curStrokeColor, targetStrokeColor;

	pCurGSData->GetStrokeColorSpace( curStrokeColorSpace );
	pTargetGSData->GetStrokeColorSpace( targetStrokeColorSpace );
	pCurGSData->GetStrokeColor( curStrokeColor );
	pTargetGSData->GetStrokeColor( targetStrokeColor );

	if ( ! IsColorSpaceEqual( curStrokeColorSpace, targetStrokeColorSpace ) || ! IsColorEqual( curStrokeColor, targetStrokeColor ) )
	{
		if ( ! targetStrokeColorSpace.IsDeviceColorSpace() )
		{
			ColorSpaceToBuf( targetStrokeColorSpace, buf );
			CHE_PDF_ObjectString::StringToBuf( " CS\n", buf );
		}

		if ( targetStrokeColor.mConponents.size() > 0 )
		{
			ColorToBuf( targetStrokeColor, buf );

			switch (  targetStrokeColorSpace.GetType() )
			{
			case COLORSPACE_DEVICE_GRAY:
				CHE_PDF_ObjectString::StringToBuf( "G\n", buf );
				break;
			case COLORSPACE_DEVICE_RGB:
				CHE_PDF_ObjectString::StringToBuf( "RG\n", buf );
				break;
			case COLORSPACE_DEVICE_CMYK:
				CHE_PDF_ObjectString::StringToBuf( "K\n", buf );
				break;
			case COLORSPACE_CIEBASE_CALGRAY:
			case COLORSPACE_CIEBASE_CALRGB:
			case COLORSPACE_CIEBASE_CALCMYK:
			case COLORSPACE_CIEBASE_CALLAB:
				CHE_PDF_ObjectString::StringToBuf( "SC\n", buf );
				break;
			case COLORSPACE_CIEBASE_ICCBASED:
			case COLORSPACE_SPECIAL_INDEXED:
			case COLORSPACE_SPECIAL_SEPARATION:
			case COLORSPACE_SPECIAL_DEVICEN:
				CHE_PDF_ObjectString::StringToBuf( "SCN\n", buf );
				break;
			}
		}else if ( targetStrokeColorSpace.GetType() == COLORSPACE_SPECIAL_PATTERN )
		{
			CHE_PDF_ObjectString::StringToBuf( "/", buf );
			CHE_PDF_ObjectString::StringToBuf( targetStrokeColorSpace.GetResName(), buf );
			CHE_PDF_ObjectString::StringToBuf( " SCN\n", buf );
		}
	}

	if ( bInTextBlock )
	{
		if ( bNewTextBlock )
		{
			TextBlockBeginToBuf( buf );
		}
		TextStateToBuf( pCurGSData, pTargetGSData, buf, bNewTextBlock );
	}

	return TRUE;
}