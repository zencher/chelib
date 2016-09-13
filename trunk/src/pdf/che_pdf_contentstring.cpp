#include "../../include/che_pdf_filter.h"
#include "../../include/che_pdf_objectstring.h"
#include "../../include/che_pdf_contentstring.h"

#include <cassert>

namespace chelib {

bool CPDF_ContentString::ColorToBuf( const CPDF_Color & color, CDynBuffer & buf )
{
	uint32 count = color.GetComponentCount();
	for ( uint32 i = 0; i < count; ++i )
	{
		CPDF_ObjectString::FloatToBuf( color.GetComponent(i), buf );
		CPDF_ObjectString::StringToBuf( " ", buf );
	}
	return true;
}



bool CPDF_ContentString::ColorSpaceToBuf( const CPDF_ColorSpace & colorSpace, CDynBuffer & buf )
{
	switch ( colorSpace.GetColorSpaceType() )
	{
	case COLORSPACE_DEVICE_GRAY:
		CPDF_ObjectString::StringToBuf( "/DeviceGray", buf );
		break;
	case COLORSPACE_DEVICE_RGB:
		CPDF_ObjectString::StringToBuf( "/DeviceRGB", buf );
		break;
	case COLORSPACE_DEVICE_CMYK:
		CPDF_ObjectString::StringToBuf( "/DeviceCMYK", buf );
		break;
	/*case COLORSPACE_CIEBASE_CALGRAY:
	case COLORSPACE_CIEBASE_CALRGB:
	//case COLORSPACE_CIEBASE_CALCMYK:
	case COLORSPACE_CIEBASE_CALLAB:
	case COLORSPACE_CIEBASE_ICCBASED:
	case COLORSPACE_SPECIAL_INDEXED:
	case COLORSPACE_SPECIAL_SEPARATION:
	case COLORSPACE_SPECIAL_DEVICEN:
uint32uint32uint32	case COLORSPACE_SPECIAL_PATTERN:
		//zctodo
        return false;
		{
			CByteString resName = colorSpace.GetResName();
			if ( resName.GetLength() != 0 )
			{
				CPDF_ObjectString::StringToBuf( "/", buf );
				CPDF_ObjectString::StringToBuf( resName, buf );
			}
			break;
		}*/
		break;
	default:
		break;
	}
	return true;
}



bool CPDF_ContentString::PdfMatrixToBuf( const CMatrix & matrix, CDynBuffer & buf )
{
	CPDF_ObjectString::FloatToBuf( matrix.a, buf );
	CPDF_ObjectString::SpaceToBuf( buf );

	CPDF_ObjectString::FloatToBuf( matrix.b, buf );
	CPDF_ObjectString::SpaceToBuf( buf );

	CPDF_ObjectString::FloatToBuf( matrix.c, buf );
	CPDF_ObjectString::SpaceToBuf( buf );

	CPDF_ObjectString::FloatToBuf( matrix.d, buf );
	CPDF_ObjectString::SpaceToBuf( buf );

	CPDF_ObjectString::FloatToBuf( matrix.e, buf );
	CPDF_ObjectString::SpaceToBuf( buf );

	CPDF_ObjectString::FloatToBuf( matrix.f, buf );

	return TRUE;
}



bool CPDF_ContentString::PathDataToBuf( const CPDF_Path * pPath, CDynBuffer & buf )
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
				CPDF_ObjectString::FloatToBuf( pPath->mItems[++i].value, buf );
				CPDF_ObjectString::SpaceToBuf( buf );
				CPDF_ObjectString::FloatToBuf( pPath->mItems[++i].value, buf );
				CPDF_ObjectString::SpaceToBuf( buf );
				CPDF_ObjectString::StringToBuf( "m\n", buf );
				break;
			}
		case PathItem_LineTo:
			{
				CPDF_ObjectString::FloatToBuf( pPath->mItems[++i].value, buf );
				CPDF_ObjectString::SpaceToBuf( buf );
				CPDF_ObjectString::FloatToBuf( pPath->mItems[++i].value, buf );
				CPDF_ObjectString::SpaceToBuf( buf );
				CPDF_ObjectString::StringToBuf( "l\n", buf );
				break;
			}
		case PathItem_CurveTo:
			{
				CPDF_ObjectString::FloatToBuf( pPath->mItems[++i].value, buf );
				CPDF_ObjectString::SpaceToBuf( buf );
				CPDF_ObjectString::FloatToBuf( pPath->mItems[++i].value, buf );
				CPDF_ObjectString::SpaceToBuf( buf );
				CPDF_ObjectString::FloatToBuf( pPath->mItems[++i].value, buf );
				CPDF_ObjectString::SpaceToBuf( buf );
				CPDF_ObjectString::FloatToBuf( pPath->mItems[++i].value, buf );
				CPDF_ObjectString::SpaceToBuf( buf );
				CPDF_ObjectString::FloatToBuf( pPath->mItems[++i].value, buf );
				CPDF_ObjectString::SpaceToBuf( buf );
				CPDF_ObjectString::FloatToBuf( pPath->mItems[++i].value, buf );
				CPDF_ObjectString::SpaceToBuf( buf );
				CPDF_ObjectString::StringToBuf( "c\n", buf );
				break;
			}
		case PathItem_Rectangle:
			{
				CPDF_ObjectString::FloatToBuf( pPath->mItems[++i].value, buf );
				CPDF_ObjectString::SpaceToBuf( buf );
				CPDF_ObjectString::FloatToBuf( pPath->mItems[++i].value, buf );
				CPDF_ObjectString::SpaceToBuf( buf );
				CPDF_ObjectString::FloatToBuf( pPath->mItems[++i].value, buf );
				CPDF_ObjectString::SpaceToBuf( buf );
				CPDF_ObjectString::FloatToBuf( pPath->mItems[++i].value, buf );
				CPDF_ObjectString::SpaceToBuf( buf );
				CPDF_ObjectString::StringToBuf( "re\n", buf );
				break;
			}
		case PathItem_Close:
			{
				CPDF_ObjectString::StringToBuf( "h\n", buf );
				break;
			}
		default:
			return FALSE;
		}
	}

	return TRUE;
}



bool CPDF_ContentString::PathToBuf( const CPDF_Path * pPath, CDynBuffer & buf )
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
				CPDF_ObjectString::StringToBuf( "f\n", buf ); 
			}else{
				CPDF_ObjectString::StringToBuf( "f*\n", buf );
			}
			break;
		}
	case Paint_Stroke:
		{
			CPDF_ObjectString::StringToBuf( "S\n", buf );
			break;
		}
	case Paint_FillStroke:
		{
			if ( pPath->GetFillMode() == Mode_Nonzero )
			{
				CPDF_ObjectString::StringToBuf( "B\n", buf ); 
			}else{
				CPDF_ObjectString::StringToBuf( "B*\n", buf );
			}
			break;
		}
	case Paint_None:
		{
			CPDF_ObjectString::StringToBuf( "n\n", buf );
			break;
		}
	default:
		return FALSE;
	}

	return TRUE;
}



void CPDF_ContentString::TextBlockBeginToBuf( CDynBuffer & buf )
{
	CPDF_ObjectString::StringToBuf( "BT\n", buf );
}



void CPDF_ContentString::TextBlockEndToBuf( CDynBuffer & buf )
{
	CPDF_ObjectString::StringToBuf( "ET\n", buf );
}



bool CPDF_ContentString::TextToBuf( const CPDF_Text * pText, CDynBuffer & buf )
{
	if ( pText == NULL )
	{
		return FALSE;
	}

	CPDF_ObjectPtr objPtr = pText->GetTextObject();

	if ( IsPdfStringPtr( objPtr ) )
	{
		CPDF_ObjectString::ObjectToBuf( objPtr, buf );
		CPDF_ObjectString::StringToBuf( " Tj\n", buf );
	}
	else if ( IsPdfArrayPtr( objPtr ) )
	{
		CPDF_ObjectString::ObjectToBuf( objPtr, buf );
		CPDF_ObjectString::StringToBuf( " TJ\n", buf );
	}else{
		return FALSE;
	}

	return TRUE;
}




/*bool CPDF_ContentString::RefImageToBuf( const CPDF_RefImage * pImage, CDynBuffer & buf )
{
	if ( pImage == NULL )
	{
		return FALSE;
	}

	CByteString str = pImage->GetName();

	if ( str.GetLength() == 0 )
	{
		return FALSE;
	}

	CPDF_ObjectString::StringToBuf( "/", buf );
	CPDF_ObjectString::StringToBuf( str, buf );
	CPDF_ObjectString::StringToBuf( " Do\n", buf );

	return TRUE;
}*/




bool CPDF_ContentString::InlineImageToBuf( const CPDF_InlineImage * pImage, CDynBuffer & buf )
{
	if ( pImage == NULL )
	{
		return FALSE;
	}

	CPDF_ObjectString::StringToBuf( "BI\n", buf );

	CPDF_ObjectString::StringToBuf( "/W ", buf );

	CPDF_ObjectString::IntegerToBuf( pImage->GetWidth(), buf );
	CPDF_ObjectString::StringToBuf( "\n", buf );

	CPDF_ObjectString::StringToBuf( "/H ", buf );
	CPDF_ObjectString::IntegerToBuf( pImage->GetHeight(), buf );
	CPDF_ObjectString::StringToBuf( "\n", buf );

	CPDF_ObjectString::StringToBuf( "/BPC ", buf );
	CPDF_ObjectString::IntegerToBuf( pImage->GetBpc(), buf );

	if ( pImage->IsMask() )
	{
		CPDF_ObjectString::StringToBuf( "/IM true\n", buf );
	}

	if ( pImage->GetColorspace() )
	{

        //zctodo
		CPDF_ObjectString::StringToBuf( "/CS /", buf );
		/*if ( pImage->GetColorspace()->GetResName().GetLength() > 0 )
		{
			CPDF_ObjectString::StringToBuf( pImage->GetColorspace()->GetResName(), buf );
		}else{
			CPDF_ObjectString::StringToBuf( pImage->GetColorspace()->GetName(), buf );

		}*/
		CPDF_ObjectString::StringToBuf( "\n", buf );
	}

	if ( pImage->GetDecode() )
	{
		CPDF_ObjectString::StringToBuf( "/D ", buf );

		CPDF_ObjectString::ObjectToBuf( pImage->GetDecode(), buf );
		CPDF_ObjectString::StringToBuf( "\n", buf );
	}

	CPDF_ObjectString::StringToBuf( "/F [/A85 /Fl]\n", buf );

	CPDF_ObjectString::StringToBuf( "ID\n", buf );

	//压缩数据
	CPDF_FlateFilter flateFilter;
	CPDF_ASCII85Filter ascii85Filter;
	CDynBuffer tmpBuf;
	CDynBuffer bynBuf;
	flateFilter.Encode( pImage->GetData(), pImage->GetDataSize(), tmpBuf );
	ascii85Filter.Encode( tmpBuf.GetData(), tmpBuf.GetSize(), bynBuf );
	buf.Write( bynBuf );

	CPDF_ObjectString::StringToBuf( "\nEI\n", buf );

	return TRUE;
}

bool CPDF_ContentString::MarkToBuf( const CPDF_Mark * pMark, CDynBuffer & buf )
{
	if ( pMark == NULL )
	{
		return FALSE;
	}

	switch ( pMark->GetMarkType() )
	{
	case Mark_MP:
		{
			CPDF_ObjectString::StringToBuf( "/", buf );
			CPDF_ObjectString::StringToBuf( pMark->GetTag(), buf );
			CPDF_ObjectString::StringToBuf( " MP\n", buf );
			break;
		}
	case Mark_DP:
		{
			CPDF_ObjectString::StringToBuf( "/", buf );
			CPDF_ObjectString::StringToBuf( pMark->GetTag(), buf );
			CPDF_ObjectString::SpaceToBuf( buf );
			if ( pMark->GetPropertyResName().GetLength() > 0 )
			{
				CPDF_ObjectString::StringToBuf( pMark->GetPropertyResName(), buf );
			}else{

				CPDF_ObjectString::ObjectToBuf( pMark->GetProperty(), buf );
			}
			CPDF_ObjectString::StringToBuf( " DP\n", buf );
			break;
		}
	case Mark_BMC:
		{
			CPDF_ObjectString::StringToBuf( "/", buf );
			CPDF_ObjectString::StringToBuf( pMark->GetTag(), buf );
			CPDF_ObjectString::StringToBuf( " BMC\n", buf );
			break;
		}
	case Mark_BDC:
		{
			CPDF_ObjectString::StringToBuf( "/", buf );
			CPDF_ObjectString::StringToBuf( pMark->GetTag(), buf );
			CPDF_ObjectString::SpaceToBuf( buf );
			if ( pMark->GetPropertyResName().GetLength() > 0 )
			{
				CPDF_ObjectString::StringToBuf( pMark->GetPropertyResName(), buf );
			}else{
				CPDF_ObjectString::ObjectToBuf( pMark->GetProperty(), buf );
			}
			CPDF_ObjectString::StringToBuf( " BDC\n", buf );
			break;
		}
	case Mark_EMC:
		{
			CPDF_ObjectString::StringToBuf( "EMC\n", buf );
			break;
		}
	default:
		return FALSE;
	}

	return TRUE;
}




/*bool CPDF_ContentString::FormToBuf( const CPDF_Form * pForm, CDynBuffer & buf )

{
	if ( pForm == NULL )
	{
		return FALSE;
	}

	if ( pForm->GetName().GetLength() > 0 )
	{
		CPDF_ObjectString::StringToBuf( "/", buf );
		CPDF_ObjectString::StringToBuf( pForm->GetName(), buf );
		CPDF_ObjectString::SpaceToBuf( buf );
		CPDF_ObjectString::StringToBuf( " Do\n", buf );

		return TRUE;
	}
	return FALSE;
}*/



bool CPDF_ContentString::TextStateToBuf( const CPDF_GState * pTextState, CDynBuffer & buf, bool bNewBlock )
{
	if ( pTextState == NULL )
	{
		return FALSE;
	}

	FLOAT                           val = 0;
	CByteString                  name;
	CMatrix                      textMatrix;
	GRAPHICS_STATE_TEXTRENDERMODE	textRenderMode = TextRenderMode_Fill;

	pTextState->GetTextCharSpace( val );

	if ( ! IsDefTextCharSpace( val ) )
	{
		CPDF_ObjectString::FloatToBuf( val, buf );
		CPDF_ObjectString::StringToBuf( " Tc\n", buf );
	}

	pTextState->GetTextWordSpace( val );

	if ( ! IsDefTextWrodSpace( val ) )
	{
		CPDF_ObjectString::FloatToBuf( val, buf );
		CPDF_ObjectString::StringToBuf( " Tw\n", buf );
	}

	pTextState->GetTextFontResName( name );
	pTextState->GetTextFontSize( val );

	if ( name.GetLength() > 0 || bNewBlock )
	{
		CPDF_ObjectString::StringToBuf( "/", buf );
		CPDF_ObjectString::StringToBuf( name, buf );
		CPDF_ObjectString::SpaceToBuf( buf );
		CPDF_ObjectString::FloatToBuf( val, buf );
		CPDF_ObjectString::StringToBuf( " Tf\n", buf );
	}

	pTextState->GetTextMatrix( textMatrix );

	if ( ! IsDefMatrix( textMatrix ) || bNewBlock )
	{
		PdfMatrixToBuf( textMatrix, buf );
		CPDF_ObjectString::StringToBuf( " Tm\n", buf );
	}

	pTextState->GetTextScaling( val );

	if ( ! IsDefTextScaling( val ) )
	{
		CPDF_ObjectString::FloatToBuf( val, buf );
		CPDF_ObjectString::StringToBuf( " Tz\n", buf );
	}

	pTextState->GetTextRise( val );

	if ( ! IsDefTextRise( val ) )
	{
		CPDF_ObjectString::FloatToBuf( val, buf );
		CPDF_ObjectString::StringToBuf( " Ts\n", buf );
	}

	pTextState->GetTextRenderMode( textRenderMode );

	if ( ! IsDefTextRenderMode( textRenderMode ) )
	{

		uint32 tmpVal = (uint32)( textRenderMode );
		CPDF_ObjectString::IntegerToBuf( tmpVal, buf );
		CPDF_ObjectString::StringToBuf( " Tr\n", buf );
	}

	return TRUE;
}

bool CPDF_ContentString::TextStateToBuf( const CPDF_GState * pCurTextState, const CPDF_GState * pTargetTextState, CDynBuffer & buf, bool bNewBlock )
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
	FLOAT                           curVal = 0, targetVal = 0;
	CByteString                  curName, targetName;
	CMatrix                      curMatrix, targetMatrix;
	GRAPHICS_STATE_TEXTRENDERMODE	curRM = TextRenderMode_Fill, targetRM = TextRenderMode_Fill;

	pCurTextState->GetTextCharSpace( curVal );
	pTargetTextState->GetTextCharSpace( targetVal );

	if ( ! IsFloatEqual( curVal, targetVal ) )
	{
		CPDF_ObjectString::FloatToBuf( targetVal, buf );
		CPDF_ObjectString::StringToBuf( " Tc\n", buf );
	}

	pCurTextState->GetTextWordSpace( curVal );
	pTargetTextState->GetTextWordSpace( targetVal );

	if ( ! IsFloatEqual( curVal, targetVal ) )
	{
		CPDF_ObjectString::FloatToBuf( targetVal, buf );
		CPDF_ObjectString::StringToBuf( " Tw\n", buf );
	}

	pCurTextState->GetTextFontResName( curName );
	pTargetTextState->GetTextFontResName( targetName );
	pCurTextState->GetTextFontSize( curVal );
	pTargetTextState->GetTextFontSize( targetVal );

	if ( curName != targetName || ! IsFloatEqual( curVal, targetVal ) || bNewBlock )
	{
		CPDF_ObjectString::StringToBuf( "/", buf );
		CPDF_ObjectString::StringToBuf( targetName, buf );
		CPDF_ObjectString::SpaceToBuf( buf );
		CPDF_ObjectString::FloatToBuf( targetVal, buf );
		CPDF_ObjectString::StringToBuf( " Tf\n", buf );
	}

	pCurTextState->GetTextMatrix( curMatrix );
	pTargetTextState->GetTextMatrix( targetMatrix );

	if ( ( curMatrix != targetMatrix ) ||  /*( ! IsDefMatrix( targetMatrix ) && */bNewBlock /*)*/ )
	{
		PdfMatrixToBuf( targetMatrix, buf );
		CPDF_ObjectString::StringToBuf( " Tm\n", buf );
	}

	pCurTextState->GetTextScaling( curVal );
	pTargetTextState->GetTextScaling( targetVal );

	if ( ! IsFloatEqual( curVal, targetVal ) )
	{
		CPDF_ObjectString::FloatToBuf( targetVal, buf );
		CPDF_ObjectString::StringToBuf( " Tz\n", buf );
	}

	pCurTextState->GetTextRise( curVal );
	pTargetTextState->GetTextRise( targetVal );

	if ( ! IsFloatEqual( curVal, targetVal ) )
	{
		CPDF_ObjectString::FloatToBuf( targetVal, buf );
		CPDF_ObjectString::StringToBuf( " Ts\n", buf );
	}

	pCurTextState->GetTextRenderMode( curRM );
	pTargetTextState->GetTextRenderMode( targetRM );

	if ( curRM != targetRM )
	{
		uint32 tmpVal = (uint32)( targetRM );
		CPDF_ObjectString::IntegerToBuf( tmpVal, buf );
		CPDF_ObjectString::StringToBuf( " Tr\n", buf );
	}

	return TRUE;
}



bool CPDF_ContentString::ExtGStateToBuf( const CPDF_ExtGState * pExtGState, CDynBuffer & buf )
{
	if ( pExtGState == NULL )
	{
		return FALSE;
	}


    //zctodo
	/*std::list<CByteString>::const_iterator it = pExtGState->mExtDictNameList.begin();

	for ( ; it != pExtGState->mExtDictNameList.end(); ++it )
	{
		CPDF_ObjectString::StringToBuf( "/", buf );
		CPDF_ObjectString::StringToBuf( *it, buf );
		CPDF_ObjectString::StringToBuf( " gs\n", buf );
	}*/

	return TRUE;
}



bool CPDF_ContentString::ExtGStateToBuf( const CPDF_ExtGState * pCurExtGState, const CPDF_ExtGState * pTargetExtGState, CDynBuffer & buf )
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


    //zctodo
	/*if ( pCurExtGState->mExtDictNameList.size() >= pTargetExtGState->mExtDictNameList.size() )
 	{
		return FALSE;
	}

	std::list<CByteString>::const_iterator it1, it2;

	it1 = pCurExtGState->mExtDictNameList.begin();
	it2 = pTargetExtGState->mExtDictNameList.begin();

	for ( ; it1 != pCurExtGState->mExtDictNameList.end(); ++it1, ++it2 );

	for ( ; it2 != pTargetExtGState->mExtDictNameList.end(); ++it2 )
	{
		CPDF_ObjectString::StringToBuf( "/", buf );
		CPDF_ObjectString::StringToBuf( *it2, buf );
		CPDF_ObjectString::StringToBuf( " gs\n", buf );
	}*/

	return TRUE;
}



bool CPDF_ContentString::ClipStateToBuf( CMatrix & curMatrix, CPDF_ClipState * pClipState, CDynBuffer & buf, bool bInTextBlock )
{
	if ( pClipState == NULL )
	{
		return FALSE;
	}

	CMatrix clipMatrix;
	CPDF_ContentObject * pClipElement = NULL;

	std::list<CPDF_ClipStateItem*>::iterator it;
	for ( it = pClipState->mClipElementList.begin(); it != pClipState->mClipElementList.end(); ++it )
	{
		pClipElement = (*it)->GetElement();
		if ( pClipElement )
		{
			/*clipMatrix = (*it)->GetMatrix();
			if ( clipMatrix != curMatrix )
			{
				CMatrix revertMatrix;
				revertMatrix.Invert( curMatrix );
				clipMatrix.Concat( revertMatrix );

				if ( ! IsDefMatrix( clipMatrix ) )
				{
					PdfMatrixToBuf( clipMatrix, buf );
					CPDF_ObjectString::StringToBuf( " cm\n", buf );
				}

				curMatrix = (*it)->GetMatrix();
			}*/

			if ( pClipElement->GetType() == ContentType_Text )
			{
				if ( ! bInTextBlock )
				{
					TextBlockBeginToBuf( buf );
				}
				CPDF_Text * pTextElement = (CPDF_Text*)( pClipElement );
				TextStateToBuf( pTextElement->GetGState(), buf );
				TextToBuf( pTextElement, buf );
				if ( ! bInTextBlock )
				{
					TextBlockEndToBuf( buf );
				}
			}else if ( pClipElement->GetType() == ContentType_Path )
			{
				CPDF_Path * pPath = (CPDF_Path*)( pClipElement );
				PathDataToBuf( pPath, buf );
				if ( pPath->GetFillMode() == Mode_Nonzero )
				{
					CPDF_ObjectString::StringToBuf( "W n\n", buf );
				}else if ( pPath->GetFillMode() == Mode_EvenOdd )
				{
					CPDF_ObjectString::StringToBuf( "W* n\n", buf );
				}
			}
		}
	}

	return TRUE; 
}



bool CPDF_ContentString::ClipStateToBuf(	CMatrix & curMatrix, CPDF_ClipState * pCurClipState,
												const CPDF_ClipState * pTargetClipState, CDynBuffer & buf, bool bInTextBlock )
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
// 		std::list<CByteString>::const_iterator it = pExtGState2->mExtDictNameList.begin();
// 		for ( ; it != pExtGState2->mExtDictNameList.end(); ++it )
// 		{
// 			CPDF_ObjectString::StringToBuf( "/", buf );
// 			CPDF_ObjectString::StringToBuf( *it, buf );
// 			CPDF_ObjectString::StringToBuf( " gs\n", buf );
// 		}
// 		return TRUE;
// 	}
// 
// 	if ( pExtGState1->mExtDictNameList.size() >= pExtGState2->mExtDictNameList.size() )
// 	{
// 		return FALSE;
// 	}
// 
// 	std::list<CByteString>::const_iterator it1, it2;
// 	it1 = pExtGState1->mExtDictNameList.begin();
// 	it2 = pExtGState2->mExtDictNameList.begin();
// 
// 	for ( ; it1 != pExtGState1->mExtDictNameList.end(); ++it1, ++it2 );
// 
// 	for ( ; it2 != pExtGState2->mExtDictNameList.end(); ++it2 )
// 	{
// 		CPDF_ObjectString::StringToBuf( "/", buf );
// 		CPDF_ObjectString::StringToBuf( *it2, buf );
// 		CPDF_ObjectString::StringToBuf( " gs\n", buf );
// 	}
	return TRUE;
}



bool CPDF_ContentString::GStateToBuf( CPDF_GState * & pGSData, CDynBuffer & buf, std::stack<CPDF_GState*> & gstack, bool bInTextBlock )
{
	if ( ! pGSData )
	{
		pGSData = GetDefaultAllocator()->New<CPDF_GState>( GetDefaultAllocator() );
	}

	bool                            bNewTextBlock = FALSE;
	bool                            bNeedStackPush = FALSE;
	FLOAT                           floatVal = 0;
	GRAPHICS_STATE_LINECAP			lineCap = LineCap_Butt;
	GRAPHICS_STATE_LINEJOIN			lineJoin = LineJoin_Miter;
	GRAPHICS_STATE_DASHPATTERN		lineDash;
	GRAPHICS_STATE_RENDERINTENTS	ri = RI_AbsoluteColorimetric;

	CPDF_ClipState * pClipState = pGSData->GetClipState();
	if ( pClipState && pClipState->mClipElementList.size() > 0 )
	{
		bNeedStackPush = TRUE;
	}

    //zctodo
	/*CPDF_ExtGState * pExtState = pGSData->GetExtGState();
	if ( pExtState && pExtState->mExtDictNameList.size() > 0 )
	{
		bNeedStackPush = TRUE;
	}*/

	if ( bNeedStackPush )
	{
		if ( bInTextBlock )
		{
			bNewTextBlock = TRUE;
			TextBlockEndToBuf( buf );
		}

		CPDF_ObjectString::StringToBuf( "q\n", buf );

		gstack.push( NULL );
	}

	CMatrix curMatrix;

	if ( pClipState != NULL )
	{
		ClipStateToBuf( curMatrix, pClipState, buf, bInTextBlock );
	}

	CMatrix target = pGSData->GetMatrix();
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
			CPDF_ObjectString::StringToBuf( " cm\n", buf );
		}
	}else if ( target != curMatrix )
	{
		if ( bInTextBlock && ! bNewTextBlock )
		{
			bNewTextBlock = TRUE;
			TextBlockEndToBuf( buf );
		}
		CMatrix revertMatrix;
		revertMatrix.Invert( curMatrix );
		target.Concat( revertMatrix );
		PdfMatrixToBuf( target, buf );
		CPDF_ObjectString::StringToBuf( " cm\n", buf );
	}

    //zctodo
	//ExtGStateToBuf( pExtState, buf );

	pGSData->GetLineWidth( floatVal );

	if ( ! IsDefLineWidth( floatVal ) )
	{
		CPDF_ObjectString::FloatToBuf( floatVal, buf );
		CPDF_ObjectString::StringToBuf( " w\n", buf );
	}

	pGSData->GetMiterLimit( floatVal );

	if ( ! IsDefMiterLimit( floatVal ) )
	{
		CPDF_ObjectString::FloatToBuf( floatVal, buf );
		CPDF_ObjectString::StringToBuf( " M\n", buf );
	}
	
	pGSData->GetLineCap( lineCap );

	if ( ! IsDefLineCap( lineCap ) )
	{
		switch ( lineCap )
		{
		case LineCap_Butt:
			CPDF_ObjectString::IntegerToBuf( 0, buf );
			CPDF_ObjectString::StringToBuf( " J\n", buf );
			break;
		case LineCap_Round:
			CPDF_ObjectString::IntegerToBuf( 1, buf );
			CPDF_ObjectString::StringToBuf( " J\n", buf );
			break;
		case LineCap_Square:
			CPDF_ObjectString::IntegerToBuf( 2, buf );
			CPDF_ObjectString::StringToBuf( " J\n", buf );
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
			CPDF_ObjectString::IntegerToBuf( 0, buf );
			CPDF_ObjectString::StringToBuf( " j\n", buf );
			break;
		case LineJoin_Round:
			CPDF_ObjectString::IntegerToBuf( 1, buf );
			CPDF_ObjectString::StringToBuf( " j\n", buf );
		case LineJoin_Bevel:
			CPDF_ObjectString::IntegerToBuf( 2, buf );
			CPDF_ObjectString::StringToBuf( " j\n", buf );
		default:
			break;
		}
	}

	pGSData->GetLineDash( lineDash );

	if ( ! IsDefLineDash( lineDash ) )
	{
		CPDF_ObjectString::StringToBuf( "[", buf );
		for ( size_t i = 0; lineDash.dashArray.size(); ++i )
		{
			CPDF_ObjectString::StringToBuf( " ", buf );
			CPDF_ObjectString::FloatToBuf( lineDash.dashArray[i], buf );
		}
		CPDF_ObjectString::StringToBuf( " ] ", buf );
		CPDF_ObjectString::FloatToBuf( lineDash.dashPhase, buf );
		CPDF_ObjectString::StringToBuf( " d\n", buf );
	}

	ri = pGSData->GetRenderIntents();
	if ( ! IsDefRenderIntents( ri ) )
	{
		switch ( ri )
		{
		case RI_AbsoluteColorimetric:
			CPDF_ObjectString::IntegerToBuf( 0, buf );
			CPDF_ObjectString::StringToBuf( " ri\n", buf );
			break;
		case RI_RelativeColorimetric:
			CPDF_ObjectString::IntegerToBuf( 1, buf );
			CPDF_ObjectString::StringToBuf( " ri\n", buf );
			break;
		case RI_Saturation:
			CPDF_ObjectString::IntegerToBuf( 2, buf );
			CPDF_ObjectString::StringToBuf( " ri\n", buf );
			break;
		case RI_Perceptual:
			CPDF_ObjectString::IntegerToBuf( 3, buf );
			CPDF_ObjectString::StringToBuf( " ri\n", buf );
			break;
		default:
			break;
		}
	}

	floatVal = pGSData->GetFlatness();
	if ( ! IsDefFlatness( floatVal )  )
	{
		CPDF_ObjectString::FloatToBuf( floatVal, buf );
		CPDF_ObjectString::StringToBuf( " i\n", buf );
	}
    
    CPDF_ColorSpacePtr fillColorSpace;// = CPDF_ColorSpace::CreateDeviceGray();
	
	//CPDF_ColorSpace fillColorSpace( COLORSPACE_DEVICE_GRAY );
	CPDF_Color fillColor;
	pGSData->GetFillColorSpace( fillColorSpace );
	pGSData->GetFillColor( fillColor );

	if ( ! IsDefColorSpace( fillColorSpace ) || ! IsDefColor( fillColor ) )
	{
		/*if ( ! fillColorSpace.IsDeviceColorSpace() )
		{
			ColorSpaceToBuf( fillColorSpace, buf );
			CPDF_ObjectString::StringToBuf( " cs\n", buf );
		}*/

		//if ( fillColor.GetComponentCount() > 0 )
		{
			ColorToBuf( fillColor,  buf );

			switch ( fillColorSpace->GetColorSpaceType() )
			{
			case COLORSPACE_DEVICE_GRAY:
				CPDF_ObjectString::StringToBuf( "g\n", buf );
				break;
			case COLORSPACE_DEVICE_RGB:
				CPDF_ObjectString::StringToBuf( "rg\n", buf );
				break;
			case COLORSPACE_DEVICE_CMYK:
				CPDF_ObjectString::StringToBuf( "k\n", buf );
				break;
			case COLORSPACE_CIEBASE_CALGRAY:
			case COLORSPACE_CIEBASE_CALRGB:
			case COLORSPACE_CIEBASE_CALLAB:
				CPDF_ObjectString::StringToBuf( "sc\n", buf );
				break;
			case COLORSPACE_CIEBASE_ICCBASED:
			case COLORSPACE_SPECIAL_INDEXED:
			case COLORSPACE_SPECIAL_SEPARATION:
			case COLORSPACE_SPECIAL_DEVICEN:
				CPDF_ObjectString::StringToBuf( "scn\n", buf );
                break;
            case COLORSPACE_SPECIAL_PATTERN:
                CPDF_ObjectString::StringToBuf( "scn\n", buf );
				break;
			}
		}
        //else if ( fillColorSpace->GetColorSpaceType() == COLORSPACE_SPECIAL_PATTERN )
		//{
		//	CPDF_ObjectString::StringToBuf( "/", buf );
		//	//CPDF_ObjectString::StringToBuf( fillColorSpace.GetResName(), buf );
		//	CPDF_ObjectString::StringToBuf( " scn\n", buf );
		//}
	}

    CPDF_ColorSpacePtr strokeColorSpace;

	CPDF_Color strokeColor;
	pGSData->GetStrokeColorSpace( strokeColorSpace );
	pGSData->GetStrokeColor( strokeColor );

	if ( ! IsDefColorSpace( strokeColorSpace ) || ! IsDefColor( strokeColor ) )
	{
		/*if ( ! strokeColorSpace.IsDeviceColorSpace() )
		{
			ColorSpaceToBuf( strokeColorSpace, buf );
			CPDF_ObjectString::StringToBuf( " CS\n", buf );
		}*/

		if ( strokeColor.GetComponentCount() > 0 )
		{
			ColorToBuf( strokeColor,  buf );


			switch (  strokeColorSpace->GetColorSpaceType() )
			{
			case COLORSPACE_DEVICE_GRAY:
				CPDF_ObjectString::StringToBuf( "G\n", buf );
				break;
			case COLORSPACE_DEVICE_RGB:
				CPDF_ObjectString::StringToBuf( "RG\n", buf );
				break;
			case COLORSPACE_DEVICE_CMYK:
				CPDF_ObjectString::StringToBuf( "K\n", buf );
				break;
			case COLORSPACE_CIEBASE_CALGRAY:
			case COLORSPACE_CIEBASE_CALRGB:
			case COLORSPACE_CIEBASE_CALLAB:
				CPDF_ObjectString::StringToBuf( "SC\n", buf );
				break;
			case COLORSPACE_SPECIAL_PATTERN:
				CPDF_ObjectString::StringToBuf( "/", buf );

				//CPDF_ObjectString::StringToBuf( strokeColorSpace.GetResName(), buf );
				CPDF_ObjectString::SpaceToBuf( buf );
			case COLORSPACE_CIEBASE_ICCBASED:
			case COLORSPACE_SPECIAL_INDEXED:
			case COLORSPACE_SPECIAL_SEPARATION:
			case COLORSPACE_SPECIAL_DEVICEN:
				CPDF_ObjectString::StringToBuf( "SCN\n", buf );
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
	return true;
}

bool CPDF_ContentString::GStateToBuf( CPDF_GState * & pCurGSData, CPDF_GState * & pTargetGSData,
											CDynBuffer & buf, std::stack<CPDF_GState*> & stack, bool bInTextBlock )
{
	/*if ( ! pTargetGSData )
	{
		pTargetGSData = GetDefaultAllocator()->New<CPDF_GState>( GetDefaultAllocator() );
	}

	if ( ! pCurGSData )
	{
		return GStateToBuf( pTargetGSData, buf, stack, bInTextBlock );
	}

	bool                            bNewTextBlock = FALSE;
	FLOAT                           curVal = 0, targetVal = 0;
	GRAPHICS_STATE_LINECAP			curLineCap = LineCap_Butt, targetLineCap = LineCap_Butt;
	GRAPHICS_STATE_LINEJOIN			curLineJoin = LineJoin_Miter, targetLineJoin = LineJoin_Miter;
	GRAPHICS_STATE_DASHPATTERN		curLineDash, targetLineDash;
	GRAPHICS_STATE_RENDERINTENTS	curRI = RI_AbsoluteColorimetric, targetRI = RI_AbsoluteColorimetric;

	CMatrix curMatrix = pCurGSData->GetMatrix();

	CPDF_ExtGState * pCurExtGState = pCurGSData->GetExtGState();
	CPDF_ExtGState * pTargetExtGState = pTargetGSData->GetExtGState();

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
			CPDF_ObjectString::StringToBuf( "q\n", buf );
		}else{
			bool	bRet = TRUE;
			CPDF_GState * pTmpGSData = NULL;
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
				CPDF_ObjectString::StringToBuf( "Q\n", buf );

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
				pCurGSData = GetDefaultAllocator()->New<CPDF_GState>( GetDefaultAllocator() );
			}
			stack.Push( pCurGSData->Clone() );
			CPDF_ObjectString::StringToBuf( "q\n", buf );

			curMatrix = pCurGSData->GetMatrix();
		}
	}
	if ( pCurGSData == NULL )
	{
		pCurGSData = GetDefaultAllocator()->New<CPDF_GState>();
	}

	CPDF_ClipState * pCurClipState = pCurGSData->GetClipState();
	CPDF_ClipState * pTargetClipState = pTargetGSData->GetClipState();

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
			bool	bRet = TRUE;
			CPDF_GState * pTmpGSData = NULL;
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
				CPDF_ObjectString::StringToBuf( "Q\n", buf );

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
				pCurGSData = GetDefaultAllocator()->New<CPDF_GState>( GetDefaultAllocator() );
			}
			stack.Push( pCurGSData->Clone() );
			CPDF_ObjectString::StringToBuf( "q\n", buf );
			
			curMatrix = pCurGSData->GetMatrix();
		}
	}

	pCurExtGState = pCurGSData->GetExtGState();
	pTargetExtGState = pTargetGSData->GetExtGState();

	pCurClipState = pCurGSData->GetClipState();
	pTargetClipState = pTargetGSData->GetClipState();

	if ( pCurClipState )
	{
		CPDF_ContentString::ClipStateToBuf( curMatrix, pCurClipState, pTargetClipState, buf, bInTextBlock );
	}else{
		CPDF_ContentString::ClipStateToBuf( curMatrix, pTargetClipState, buf, bInTextBlock );
	}

	CPDF_ContentString::ExtGStateToBuf( pCurExtGState, pTargetExtGState, buf );

	CMatrix targetMatrix = pTargetGSData->GetMatrix();

	if ( curMatrix != targetMatrix )
	{
		if ( bInTextBlock && ! bNewTextBlock )
		{
			bNewTextBlock = TRUE;
			TextBlockEndToBuf( buf );
		}

		if ( ! IsDefMatrix( curMatrix ) )
		{
			CMatrix revertMatrix;
			revertMatrix.Invert( curMatrix );
			targetMatrix.Concat( revertMatrix );
		}

		PdfMatrixToBuf( targetMatrix, buf );
		CPDF_ObjectString::StringToBuf( " cm\n", buf );
	}

	pCurGSData->GetLineWidth( curVal );
	pTargetGSData->GetLineWidth( targetVal );

	if ( ! IsFloatEqual( curVal, targetVal ) )
	{
		CPDF_ObjectString::FloatToBuf( targetVal, buf );
		CPDF_ObjectString::StringToBuf( " w\n", buf );
	}

	pCurGSData->GetMiterLimit( curVal );
	pTargetGSData->GetMiterLimit( targetVal );

	if ( ! IsFloatEqual( curVal, targetVal ) )
	{
		CPDF_ObjectString::FloatToBuf( targetVal, buf );
		CPDF_ObjectString::StringToBuf( " M\n", buf );
	}

	pCurGSData->GetLineCap( curLineCap );
	pTargetGSData->GetLineCap( targetLineCap );

	if ( curLineCap != targetLineCap )
	{
		switch ( targetLineCap )
		{
		case LineCap_Butt:
			CPDF_ObjectString::IntegerToBuf( 0, buf );
			CPDF_ObjectString::StringToBuf( " J\n", buf );
			break;
		case LineCap_Round:
			CPDF_ObjectString::IntegerToBuf( 1, buf );
			CPDF_ObjectString::StringToBuf( " J\n", buf );
			break;
		case LineCap_Square:
			CPDF_ObjectString::IntegerToBuf( 2, buf );
			CPDF_ObjectString::StringToBuf( " J\n", buf );
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
			CPDF_ObjectString::IntegerToBuf( 0, buf );
			CPDF_ObjectString::StringToBuf( " j\n", buf );
			break;
		case LineJoin_Round:
			CPDF_ObjectString::IntegerToBuf( 1, buf );
			CPDF_ObjectString::StringToBuf( " j\n", buf );
			break;
		case LineJoin_Bevel:
			CPDF_ObjectString::IntegerToBuf( 2, buf );
			CPDF_ObjectString::StringToBuf( " j\n", buf );
		default:
			break;
		}
	}

	pCurGSData->GetLineDash( curLineDash );
	pTargetGSData->GetLineDash( targetLineDash );

	if (	! IsFloatEqual( curLineDash.dashPhase, targetLineDash.dashPhase ) ||
			curLineDash.dashArray.size() != targetLineDash.dashArray.size() )
	{
		CPDF_ObjectString::StringToBuf( "[", buf );
		for ( HE_ULONG i = 0; i < targetLineDash.dashArray.size(); ++i )
		{
			CPDF_ObjectString::StringToBuf( " ", buf );
			CPDF_ObjectString::FloatToBuf( targetLineDash.dashArray[i], buf );
		}
		CPDF_ObjectString::StringToBuf( " ] ", buf );
		CPDF_ObjectString::FloatToBuf( targetLineDash.dashPhase, buf );
		CPDF_ObjectString::StringToBuf( " d\n", buf );
	}else{
		bool bSame = TRUE;
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
			CPDF_ObjectString::StringToBuf( "[", buf );
			for ( size_t i = 0; targetLineDash.dashArray.size(); ++i )
			{
				CPDF_ObjectString::StringToBuf( " ", buf );
				CPDF_ObjectString::FloatToBuf( targetLineDash.dashArray[i], buf );
			}
			CPDF_ObjectString::StringToBuf( " ] ", buf );
			CPDF_ObjectString::FloatToBuf( targetLineDash.dashPhase, buf );
			CPDF_ObjectString::StringToBuf( " d\n", buf );
		}
	}

	curRI = pCurGSData->GetRenderIntents();
	targetRI = pTargetGSData->GetRenderIntents();

	if ( curRI != targetRI )
	{
		switch ( targetRI )
		{
		case RI_AbsoluteColorimetric:
			CPDF_ObjectString::IntegerToBuf( 0, buf );
			CPDF_ObjectString::StringToBuf( " ri\n", buf );
			break;
		case RI_RelativeColorimetric:
			CPDF_ObjectString::IntegerToBuf( 1, buf );
			CPDF_ObjectString::StringToBuf( " ri\n", buf );
			break;
		case RI_Saturation:
			CPDF_ObjectString::IntegerToBuf( 2, buf );
			CPDF_ObjectString::StringToBuf( " ri\n", buf );
			break;
		case RI_Perceptual:
			CPDF_ObjectString::IntegerToBuf( 3, buf );
			CPDF_ObjectString::StringToBuf( " ri\n", buf );
			break;
		default:
			break;
		}
	}

	curVal = pCurGSData->GetFlatness();
	targetVal = pTargetGSData->GetFlatness();

	if ( ! IsFloatEqual( curVal, targetVal ) )
	{
		CPDF_ObjectString::FloatToBuf( targetVal, buf );
		CPDF_ObjectString::StringToBuf( " i\n", buf );
	}

	CPDF_ColorSpace curFillColorSpace( COLORSPACE_DEVICE_GRAY ), targetFillColorSpace( COLORSPACE_DEVICE_GRAY );
	CPDF_Color curFillColor, targetFillColor;

	pCurGSData->GetFillColorSpace( curFillColorSpace );
	pTargetGSData->GetFillColorSpace( targetFillColorSpace );
	pCurGSData->GetFillColor( curFillColor );
	pTargetGSData->GetFillColor( targetFillColor );

	if ( ! IsColorSpaceEqual( curFillColorSpace, targetFillColorSpace ) || ! IsColorEqual( curFillColor, targetFillColor ) )
	{
		if ( ! targetFillColorSpace.IsDeviceColorSpace() )
		{
			ColorSpaceToBuf( targetFillColorSpace, buf );
			CPDF_ObjectString::StringToBuf( " cs\n", buf );
		}

		if ( targetFillColor.GetComponentCount() > 0 )
		{
			ColorToBuf( targetFillColor, buf );

			switch (  targetFillColorSpace.GetType() )
			{
			case COLORSPACE_DEVICE_GRAY:
				CPDF_ObjectString::StringToBuf( "g\n", buf );
				break;
			case COLORSPACE_DEVICE_RGB:
				CPDF_ObjectString::StringToBuf( "rg\n", buf );
				break;
			case COLORSPACE_DEVICE_CMYK:
				CPDF_ObjectString::StringToBuf( "k\n", buf );
				break;
			case COLORSPACE_CIEBASE_CALGRAY:
			case COLORSPACE_CIEBASE_CALRGB:
			case COLORSPACE_CIEBASE_CALCMYK:
			case COLORSPACE_CIEBASE_CALLAB:
				CPDF_ObjectString::StringToBuf( "sc\n", buf );
				break;
			case COLORSPACE_CIEBASE_ICCBASED:
			case COLORSPACE_SPECIAL_INDEXED:
			case COLORSPACE_SPECIAL_SEPARATION:
			case COLORSPACE_SPECIAL_DEVICEN:
				CPDF_ObjectString::StringToBuf( "scn\n", buf );
				break;
			}
		}else if ( targetFillColorSpace.GetType() == COLORSPACE_SPECIAL_PATTERN )
		{
			CPDF_ObjectString::StringToBuf( "/", buf );
			CPDF_ObjectString::StringToBuf( targetFillColorSpace.GetResName(), buf );
			CPDF_ObjectString::StringToBuf( " scn\n", buf );
		}
	}

	CPDF_ColorSpace curStrokeColorSpace( COLORSPACE_DEVICE_GRAY ), targetStrokeColorSpace( COLORSPACE_DEVICE_GRAY );
	CPDF_Color curStrokeColor, targetStrokeColor;

	pCurGSData->GetStrokeColorSpace( curStrokeColorSpace );
	pTargetGSData->GetStrokeColorSpace( targetStrokeColorSpace );
	pCurGSData->GetStrokeColor( curStrokeColor );
	pTargetGSData->GetStrokeColor( targetStrokeColor );

	if ( ! IsColorSpaceEqual( curStrokeColorSpace, targetStrokeColorSpace ) || ! IsColorEqual( curStrokeColor, targetStrokeColor ) )
	{
		if ( ! targetStrokeColorSpace.IsDeviceColorSpace() )
		{
			ColorSpaceToBuf( targetStrokeColorSpace, buf );
			CPDF_ObjectString::StringToBuf( " CS\n", buf );
		}

		if ( targetStrokeColor.GetComponentCount() > 0 )
		{
			ColorToBuf( targetStrokeColor, buf );

			switch (  targetStrokeColorSpace.GetType() )
			{
			case COLORSPACE_DEVICE_GRAY:
				CPDF_ObjectString::StringToBuf( "G\n", buf );
				break;
			case COLORSPACE_DEVICE_RGB:
				CPDF_ObjectString::StringToBuf( "RG\n", buf );
				break;
			case COLORSPACE_DEVICE_CMYK:
				CPDF_ObjectString::StringToBuf( "K\n", buf );
				break;
			case COLORSPACE_CIEBASE_CALGRAY:
			case COLORSPACE_CIEBASE_CALRGB:
			case COLORSPACE_CIEBASE_CALCMYK:
			case COLORSPACE_CIEBASE_CALLAB:
				CPDF_ObjectString::StringToBuf( "SC\n", buf );
				break;
			case COLORSPACE_CIEBASE_ICCBASED:
			case COLORSPACE_SPECIAL_INDEXED:
			case COLORSPACE_SPECIAL_SEPARATION:
			case COLORSPACE_SPECIAL_DEVICEN:
				CPDF_ObjectString::StringToBuf( "SCN\n", buf );
				break;
			}
		}else if ( targetStrokeColorSpace.GetType() == COLORSPACE_SPECIAL_PATTERN )
		{
			CPDF_ObjectString::StringToBuf( "/", buf );
			CPDF_ObjectString::StringToBuf( targetStrokeColorSpace.GetResName(), buf );
			CPDF_ObjectString::StringToBuf( " SCN\n", buf );
		}
	}

	if ( bInTextBlock )
	{
		if ( bNewTextBlock )
		{
			TextBlockBeginToBuf( buf );
		}
		TextStateToBuf( pCurGSData, pTargetGSData, buf, bNewTextBlock );

	}*/

	return true;
}

}//namespace