#include "../../include/pdf/che_pdf_graphicsstate.h"
#include "../../include/pdf/che_pdf_contentobjs.h"

CHE_PDF_StrokeState * CHE_PDF_StrokeState::Clone() const
{
	CHE_PDF_StrokeState * pRet = GetAllocator()->New<CHE_PDF_StrokeState>( GetAllocator() );
	pRet->mLineCap = mLineCap;
	pRet->mLineDash = mLineDash;
	pRet->mLineJoin = mLineJoin;
	pRet->mLineWidth = mLineWidth;
	pRet->mMiterLimit = mMiterLimit;
	pRet->mLineDash.dashPhase = mLineDash.dashPhase;
	pRet->mLineDash.dashArray = mLineDash.dashArray;
	return pRet;
}

CHE_PDF_TextGState * CHE_PDF_TextGState::Clone() const
{
	CHE_PDF_TextGState * pRet = GetAllocator()->New<CHE_PDF_TextGState>( GetAllocator() );
	pRet->mMatrix = mMatrix;
	pRet->mCharSpace = mCharSpace;
	pRet->mWordSpace = mWordSpace;
	pRet->mFontName = mFontName;
	pRet->mFontSize = mFontSize;
	pRet->mpFont = mpFont;
	pRet->mLeading = mLeading;
	pRet->mScaling = mScaling;
	pRet->mRise = mRise;
	pRet->mRMode = mRMode;
	return pRet;
}

CHE_PDF_ClipStateItem::~CHE_PDF_ClipStateItem()
{
	if ( mpClipElement )
	{
		mpClipElement->GetAllocator()->Delete( mpClipElement );
	}
}

HE_BOOL CHE_PDF_ClipStateItem::SetElement( CHE_PDF_ContentObject * pElement )
{
	if ( pElement == NULL )
	{
		return FALSE;
	}
	if ( pElement->GetType() != ContentType_Text && pElement->GetType() != ContentType_Path )
	{
		return FALSE;
	}
	mpClipElement = pElement;
	return TRUE;
}

CHE_PDF_ClipStateItem * CHE_PDF_ClipStateItem::Clone() const
{
	CHE_PDF_ClipStateItem * pRet = GetAllocator()->New<CHE_PDF_ClipStateItem>( GetAllocator() );
	pRet->SetElement( mpClipElement->Clone() );
	return pRet;
}

CHE_PDF_ClipState::~CHE_PDF_ClipState()
{
	if ( mClipElementList.size() )
	{
		std::list<CHE_PDF_ClipStateItem*>::iterator it = mClipElementList.begin();
		for ( ; it != mClipElementList.end(); ++it )
		{
			if ( *it )
			{
				(*it)->GetAllocator()->Delete( *it );
			}
		}
	}
}

HE_BOOL CHE_PDF_ClipState::Append( CHE_PDF_ClipStateItem * pItem )
{
	if ( pItem == NULL )
	{
		return FALSE;
	}

	mClipElementList.push_back( pItem );

	return TRUE;
}

CHE_PDF_ClipState * CHE_PDF_ClipState::Clone() const
{
	CHE_PDF_ClipState * pRet = GetAllocator()->New<CHE_PDF_ClipState>( GetAllocator() );
	std::list<CHE_PDF_ClipStateItem*>::const_iterator it = mClipElementList.begin();
	for ( ; it != mClipElementList.end(); ++it )
	{
		pRet->Append( (*it)->Clone() );
	}
	return pRet;
}

HE_BOOL CHE_PDF_ExtGState::PushExtStateName( const CHE_ByteString & name, CHE_PDF_DictionaryPtr dictPtr )
{
	if ( ! dictPtr )
	{
		return FALSE;
	}
	std::list<CHE_ByteString>::iterator it = mExtDictNameList.begin();
	for ( ; it != mExtDictNameList.end(); ++it )
	{
		if ( *it == name )
		{
			mExtDictNameList.erase( it );
			break;
		}
	}
	mExtDictNameList.push_back( name );
	CHE_PDF_ObjectPtr pTmpObj = dictPtr->GetElement( "CA", OBJ_TYPE_NUMBER );
	if ( pTmpObj )
	{
		mStrokeAlpha = pTmpObj->GetNumberPtr()->GetFloat();
	}
	pTmpObj = dictPtr->GetElement( "ca", OBJ_TYPE_NUMBER );
	if ( pTmpObj )
	{
		mFillAlpha = pTmpObj->GetNumberPtr()->GetFloat();
	}
	return TRUE;
}

CHE_PDF_ExtGState * CHE_PDF_ExtGState::Clone() const
{
	CHE_PDF_ExtGState * pRet = GetAllocator()->New<CHE_PDF_ExtGState>( GetAllocator() );
	pRet->mExtDictNameList = mExtDictNameList;
	pRet->mFillAlpha = mFillAlpha;
	pRet->mStrokeAlpha = mStrokeAlpha;
	return pRet;
}

bool CHE_PDF_ExtGState::operator == ( const CHE_PDF_ExtGState & gs ) const
{
	if ( ! IsFloatEqual( mFillAlpha, gs.mFillAlpha ) ||
		 ! IsFloatEqual( mStrokeAlpha, gs.mFillAlpha ) )
	{
		return false;
	}

	std::list<CHE_ByteString>::const_iterator it = mExtDictNameList.begin();
	std::list<CHE_ByteString>::const_iterator tmpIt = gs.mExtDictNameList.begin();

	for ( ; it != mExtDictNameList.end() && tmpIt != gs.mExtDictNameList.end() ; ++it, ++tmpIt )
	{
		if ( *it != *tmpIt )
		{
			return false;
		}
	}

	if ( tmpIt != gs.mExtDictNameList.end() || it != mExtDictNameList.end() )
	{
		return false;
	}
	
	return true;
}

bool CHE_PDF_ExtGState::operator != ( const CHE_PDF_ExtGState & gs ) const
{
	return ! operator==( gs );
}

CHE_PDF_GState::~CHE_PDF_GState()
{
	if ( mpStrokeState )
	{
		mpStrokeState->GetAllocator()->Delete( mpStrokeState );
	}
	if ( mpTextState )
	{
		mpTextState->GetAllocator()->Delete( mpTextState );
	}
	if ( mpClipState )
	{
		mpClipState->GetAllocator()->Delete( mpClipState );
	}
	if ( mpExtState )
	{
		mpExtState->GetAllocator()->Delete( mpExtState );
	}
	if ( mpFillColor )
	{
		mpFillColor->GetAllocator()->Delete( mpFillColor );
	}
	if ( mpFillColorSpace )
	{
		mpFillColorSpace->GetAllocator()->Delete( mpFillColorSpace );
	}
	if ( mpStrokeColor )
	{
		mpStrokeColor->GetAllocator()->Delete( mpStrokeColor );
	}
	if ( mpStrokeColorSpace )
	{
		mpStrokeColorSpace->GetAllocator()->Delete( mpStrokeColorSpace );
	}
}

CHE_PDF_StrokeState * CHE_PDF_GState::MakeStrokeState()
{
	if ( !mpStrokeState )
	{
		mpStrokeState = GetAllocator()->New<CHE_PDF_StrokeState>( GetAllocator() );
	}
	return mpStrokeState;
}

CHE_PDF_TextGState * CHE_PDF_GState::MakeTextState()
{
	if ( !mpTextState )
	{
		mpTextState = GetAllocator()->New<CHE_PDF_TextGState>( GetAllocator() );
	}
	return mpTextState;
}

CHE_PDF_ClipState *	CHE_PDF_GState::MakeClipState()
{
	if ( !mpClipState )
	{
		mpClipState = GetAllocator()->New<CHE_PDF_ClipState>( GetAllocator() );
	}
	return mpClipState;
}

CHE_PDF_ExtGState *	CHE_PDF_GState::MakeExtGState()
{
	if ( !mpExtState )
	{
		mpExtState = GetAllocator()->New<CHE_PDF_ExtGState>( GetAllocator() );
	}
	return mpExtState;
}

CHE_PDF_GState * CHE_PDF_GState::Clone() const
{
	CHE_PDF_GState * pRet = GetAllocator()->New<CHE_PDF_GState>( GetAllocator() );
	pRet->mMatrix = mMatrix;
	pRet->mFlag = mFlag;
	if ( mpFillColor )
	{
		pRet->mpFillColor = mpFillColor->Clone();
	}
	if ( mpFillColorSpace )
	{
		pRet->mpFillColorSpace = mpFillColorSpace->Clone();
	}
	if ( mpStrokeColor )
	{
		pRet->mpStrokeColor = mpStrokeColor->Clone();
	}
	if ( mpStrokeColorSpace )
	{
		pRet->mpStrokeColorSpace = mpStrokeColorSpace->Clone();
	}
	if ( mpStrokeState )
	{
		pRet->mpStrokeState = mpStrokeState->Clone();
	}
	if ( mpTextState )
	{
		pRet->mpTextState = mpTextState->Clone();
	}
	if ( mpClipState )
	{
		pRet->mpClipState = mpClipState->Clone();
	}
	if ( mpExtState )
	{
		pRet->mpExtState = mpExtState->Clone();
	}
	return pRet;
}

HE_VOID	CHE_PDF_GState::GetFillColor( CHE_PDF_Color & colorRet ) const
{
	if ( mFlag & GSTATE_FLAG_FillColor )
	{
		colorRet = *mpFillColor;
		return;
	}
	
	colorRet.mConponents.clear();

	if ( mFlag & GSTATE_FLAG_FillColorSpace )
	{
		switch ( mpFillColorSpace->GetType() )
		{
		case COLORSPACE_DEVICE_GRAY:
		case COLORSPACE_CIEBASE_CALGRAY:
		case COLORSPACE_SPECIAL_SEPARATION:
		case COLORSPACE_SPECIAL_DEVICEN:
		case COLORSPACE_SPECIAL_INDEXED:
			colorRet.mConponents.push_back( 0 );
			break;
		case COLORSPACE_DEVICE_RGB:
		case COLORSPACE_CIEBASE_CALRGB:
			colorRet.mConponents.push_back( 0 );
			colorRet.mConponents.push_back( 0 );
			colorRet.mConponents.push_back( 0 );
			break;
		case COLORSPACE_DEVICE_CMYK:
		case COLORSPACE_CIEBASE_CALCMYK:
			colorRet.mConponents.push_back( 0 );
			colorRet.mConponents.push_back( 0 );
			colorRet.mConponents.push_back( 0 );
			colorRet.mConponents.push_back( 0 );
			break;
		case COLORSPACE_CIEBASE_CALLAB:
			colorRet.mConponents.push_back( 0 );
			colorRet.mConponents.push_back( 0 );
			colorRet.mConponents.push_back( 0 );
			break;
		case COLORSPACE_CIEBASE_ICCBASED:
// 			colorRet.mConponents.push_back( 0 );
// 			colorRet.mConponents.push_back( 0 );
// 			colorRet.mConponents.push_back( 0 );
// 			colorRet.mConponents.push_back( 0 );
// 			break;
		case COLORSPACE_SPECIAL_PATTERN:break;
		default:break;
		}
	}else{
		colorRet.mConponents.push_back( 0 );
	}
}

HE_VOID CHE_PDF_GState::GetStrokeColor( CHE_PDF_Color & colorRet ) const
{
	if ( mFlag & GSTATE_FLAG_StrokeColor )
	{
		colorRet = *mpStrokeColor;
		return;
	}

	colorRet.mConponents.clear();

	if ( mFlag & GSTATE_FLAG_StrokeColorSpace )
	{
		switch ( mpStrokeColorSpace->GetType() )
		{
		case COLORSPACE_DEVICE_GRAY:
		case COLORSPACE_CIEBASE_CALGRAY:
		case COLORSPACE_SPECIAL_SEPARATION:
		case COLORSPACE_SPECIAL_DEVICEN:
		case COLORSPACE_SPECIAL_INDEXED:
			colorRet.mConponents.push_back( 0 );
			break;
		case COLORSPACE_DEVICE_RGB:
		case COLORSPACE_CIEBASE_CALRGB:
			colorRet.mConponents.push_back( 0 );
			colorRet.mConponents.push_back( 0 );
			colorRet.mConponents.push_back( 0 );
			break;
		case COLORSPACE_DEVICE_CMYK:
		case COLORSPACE_CIEBASE_CALCMYK:
			colorRet.mConponents.push_back( 0 );
			colorRet.mConponents.push_back( 0 );
			colorRet.mConponents.push_back( 0 );
			colorRet.mConponents.push_back( 0 );
			break;
		case COLORSPACE_CIEBASE_CALLAB:
			colorRet.mConponents.push_back( 0 );
			colorRet.mConponents.push_back( 0 );
			colorRet.mConponents.push_back( 0 );
			break;
		case COLORSPACE_CIEBASE_ICCBASED:
// 			colorRet.mConponents.push_back( 0 );
// 			colorRet.mConponents.push_back( 0 );
// 			colorRet.mConponents.push_back( 0 );
// 			colorRet.mConponents.push_back( 0 );
// 			break;
		case COLORSPACE_SPECIAL_PATTERN:break;
		default:break;
		}
	}else{
		colorRet.mConponents.push_back( 0 );
	}
}

HE_VOID CHE_PDF_GState::GetFillColorSpace( CHE_PDF_ColorSpace & colorSpaceRet ) const
{
	if ( mFlag & GSTATE_FLAG_FillColorSpace )
	{
		colorSpaceRet = *mpFillColorSpace;
		return;
	}
	CHE_PDF_ColorSpace csRet( COLORSPACE_DEVICE_GRAY );
	colorSpaceRet = csRet;
}

HE_VOID CHE_PDF_GState::GetStrokeColorSpace( CHE_PDF_ColorSpace & colorSpaceRet ) const
{
	if ( mFlag & GSTATE_FLAG_StrokeColorSpace )
	{
		colorSpaceRet = *mpStrokeColorSpace;
		return;
	}
	CHE_PDF_ColorSpace csRet( COLORSPACE_DEVICE_GRAY );
	colorSpaceRet = csRet;
}

HE_VOID	CHE_PDF_GState::GetLineWidth( HE_FLOAT & lineWidthRet ) const
{
	if ( mFlag & GSTATE_FLAG_LineWidth )
	{
		lineWidthRet = mpStrokeState->GetLineWidth();
		return;
	}
	lineWidthRet = 1;
}

HE_VOID CHE_PDF_GState::GetMiterLimit( HE_FLOAT & miterLimitRet ) const
{
	if ( mFlag & GSTATE_FLAG_MiterLimit )
	{
		miterLimitRet = mpStrokeState->GetMiterLimit();
		return;
	}
	miterLimitRet = 10;
}

HE_VOID CHE_PDF_GState::GetLineCap( GRAPHICS_STATE_LINECAP & lineCapRet ) const
{
	if ( mFlag & GSTATE_FLAG_LineCap )
	{
		lineCapRet = mpStrokeState->GetLineCap();
		return;
	}
	lineCapRet = LineCap_Butt;
}

HE_VOID CHE_PDF_GState::GetLineJoin( GRAPHICS_STATE_LINEJOIN & lineJoinRet ) const
{
	if ( mFlag & GSTATE_FLAG_LineJoin )
	{
		lineJoinRet = mpStrokeState->GetLineJoin();
		return;
	}
	lineJoinRet = LineJoin_Miter;
}

HE_VOID CHE_PDF_GState::GetLineDash( GRAPHICS_STATE_DASHPATTERN & lineDash ) const
{
	if ( mFlag & GSTATE_FLAG_LineDash )
	{
		lineDash = mpStrokeState->GetLineDash();
		return;
	}
	lineDash.dashPhase = 0;
	lineDash.dashArray.clear();
}

HE_VOID CHE_PDF_GState::GetTextMatrix( CHE_Matrix & matrixRet ) const
{
	if ( mFlag & GSTATE_FLAG_TextMatirx )
	{
		matrixRet = mpTextState->GetMatrix();
		return;
	}
	matrixRet = CHE_Matrix();
}

HE_VOID CHE_PDF_GState::GetTextFontSize( HE_FLOAT & fontSizeRet ) const
{
	if ( mFlag & GSTATE_FLAG_Font )
	{
		fontSizeRet = mpTextState->GetFontSize();
		return;
	}
	fontSizeRet = 1;
}

HE_VOID CHE_PDF_GState::GetTextCharSpace( HE_FLOAT & charSpaceRet ) const
{
	if ( mFlag & GSTATE_FLAG_CharSpace )
	{
		charSpaceRet = mpTextState->GetCharSpace();
		return;
	}
	charSpaceRet = 0;
}

HE_VOID CHE_PDF_GState::GetTextWordSpace( HE_FLOAT & wordSpaceRet ) const
{
	if ( mFlag & GSTATE_FLAG_WordSpace )
	{
		wordSpaceRet = mpTextState->GetWordSpace();
		return;
	}
	wordSpaceRet = 0;
}

HE_VOID CHE_PDF_GState::GetTextScaling( HE_FLOAT & scalingRet ) const
{
	if ( mFlag & GSTATE_FLAG_Scaling )
	{
		scalingRet = mpTextState->GetScaling();
		return;
	}
	scalingRet = 100;
}

HE_VOID CHE_PDF_GState::GetTextRise( HE_FLOAT & riseRet ) const
{
	if ( mFlag & GSTATE_FLAG_Rise )
	{
		riseRet = mpTextState->GetRise();
		return;
	}
	riseRet = 0;
}

CHE_PDF_Font * CHE_PDF_GState::GetTextFont() const
{
	if ( mFlag & GSTATE_FLAG_Font )
	{
		return mpTextState->GetFont();
	}
	return NULL;
}

HE_VOID CHE_PDF_GState::GetTextFontResName( CHE_ByteString & resNameRet ) const
{
	if ( mFlag & GSTATE_FLAG_Font )
	{
		resNameRet = mpTextState->GetFontResName();
		return;
	}
	resNameRet.Clear();
}

HE_VOID CHE_PDF_GState::GetTextRenderMode( GRAPHICS_STATE_TEXTRENDERMODE & rm ) const
{
	if ( mFlag & GSTATE_FLAG_RenderMode )
	{
		rm = mpTextState->GetRenderMode();
		return;
	}
	rm = TextRenderMode_Fill;
}

HE_VOID CHE_PDF_GState::SetMatrix( const CHE_Matrix & matirx )
{
	mMatrix = matirx;
}

HE_VOID CHE_PDF_GState::SetRenderIntents( const GRAPHICS_STATE_RENDERINTENTS & ri )
{
	mFlag |= GSTATE_FLAG_RenderIntents;
	mRenderIntents = ri;
}
HE_VOID CHE_PDF_GState::SetFlatness( const HE_FLOAT & flatness )
{
	mFlag |= GSTATE_FLAG_Flatness;
	mFlatness = flatness;
}

HE_BOOL CHE_PDF_GState::SetFillColor( CHE_PDF_Color * pColor )
{
	if ( pColor )
	{
		if ( mpFillColor )
		{
			mpFillColor->GetAllocator()->Delete( mpFillColor );
		}
		mpFillColor = pColor;
		mFlag |= GSTATE_FLAG_FillColor;
		return TRUE;
	}
	return FALSE;
}

HE_BOOL CHE_PDF_GState::SetStrokeColor( CHE_PDF_Color * pColor )
{
	if ( pColor )
	{
		if ( mpStrokeColor )
		{
			mpStrokeColor->GetAllocator()->Delete( mpStrokeColor );
		}
		mpStrokeColor = pColor;
		mFlag |= GSTATE_FLAG_StrokeColor;
		return TRUE;
	}
	return FALSE;
}

HE_BOOL CHE_PDF_GState::SetFillColorSpace( CHE_PDF_ColorSpace * pColorSpace )
{
	if ( pColorSpace )
	{
		if ( mpFillColorSpace )
		{
			mpFillColorSpace->GetAllocator()->Delete( mpFillColorSpace );
		}
		mpFillColorSpace = pColorSpace;
		mFlag |= GSTATE_FLAG_FillColorSpace;
		return TRUE;
	}
	return FALSE;
}

HE_BOOL CHE_PDF_GState::SetStrokeColorSpace( CHE_PDF_ColorSpace * pColorSpace )
{
	if ( pColorSpace )
	{
		if ( mpStrokeColorSpace )
		{
			mpStrokeColorSpace->GetAllocator()->Delete( mpStrokeColorSpace );
		}
		mpStrokeColorSpace = pColorSpace;
		mFlag |= GSTATE_FLAG_StrokeColorSpace;
		return TRUE;
	}
	return FALSE;
}

HE_VOID	CHE_PDF_GState::SetLineWidth( const HE_FLOAT & lineWidth )
{
	MakeStrokeState()->SetLineWidth( lineWidth );
	mFlag |= GSTATE_FLAG_LineWidth;
}

HE_VOID	CHE_PDF_GState::SetMiterLimit( const HE_FLOAT & miterLimit )
{
	MakeStrokeState()->SetMiterLimit( miterLimit );
	mFlag |= GSTATE_FLAG_MiterLimit;
}

HE_VOID	CHE_PDF_GState::SetLineCap( const GRAPHICS_STATE_LINECAP & lineCap )
{
	MakeStrokeState()->SetLineCap( lineCap );
	mFlag |= GSTATE_FLAG_LineCap;
}

HE_VOID	CHE_PDF_GState::SetLineJoin( const GRAPHICS_STATE_LINEJOIN & lineJion )
{
	MakeStrokeState()->SetLineJoin( lineJion );
	mFlag |= GSTATE_FLAG_LineJoin;
}

HE_VOID	CHE_PDF_GState::SetLineDash( const GRAPHICS_STATE_DASHPATTERN & dashPattern )
{
	MakeStrokeState()->SetLineDash( dashPattern );
	mFlag |= GSTATE_FLAG_LineDash;
}

HE_VOID CHE_PDF_GState::SetTextMatrix( const CHE_Matrix & matrix )
{
	MakeTextState()->SetMatirx( matrix );
	mFlag |= GSTATE_FLAG_TextMatirx;
}

HE_VOID CHE_PDF_GState::SetTextFont( CHE_PDF_Font * pFont )
{
	MakeTextState()->SetFont( pFont );
	mFlag |= GSTATE_FLAG_Font;
}

HE_VOID CHE_PDF_GState::SetTextFontSize( const HE_FLOAT & size )
{
	MakeTextState()->SetFontSize( size );
	mFlag |= GSTATE_FLAG_Font;
}

HE_VOID CHE_PDF_GState::SetTextFontResName( const CHE_ByteString & resName )
{
	MakeTextState()->SetFontResName( resName );
	mFlag |= GSTATE_FLAG_Font;
}

HE_VOID CHE_PDF_GState::SetTextCharSpace( const HE_FLOAT & charSpace )
{
	MakeTextState()->SetCharSpace( charSpace );
	mFlag |= GSTATE_FLAG_CharSpace;
}

HE_VOID CHE_PDF_GState::SetTextWordSpace( const HE_FLOAT & wordSpace )
{
	MakeTextState()->SetWordSpace( wordSpace );
	mFlag |= GSTATE_FLAG_WordSpace;
}

// HE_VOID CHE_PDF_GState::SetTextLeading( const HE_FLOAT & leading )
// {
// 	MakeTextState()->SetLeading( leading );
// 	mFlag |= GSTATE_FLAG_Leading;
// }

HE_VOID CHE_PDF_GState::SetTextScaling( const HE_FLOAT & scaling )
{
	MakeTextState()->SetScaling( scaling );
	mFlag |= GSTATE_FLAG_Scaling;
}

HE_VOID CHE_PDF_GState::SetTextRise( const HE_FLOAT & rise )
{
	MakeTextState()->SetRise( rise );
	mFlag |= GSTATE_FLAG_Rise;
}

HE_VOID CHE_PDF_GState::SetTextRenderMode( const GRAPHICS_STATE_TEXTRENDERMODE & rm )
{
	MakeTextState()->SetRenderMode( rm );
	mFlag |= GSTATE_FLAG_RenderMode;
}

HE_BOOL CHE_PDF_GState::PushClipElement( CHE_PDF_ContentObject * pElement )
{
	if ( !pElement )
	{
		return FALSE;
	}
	CHE_PDF_ClipStateItem * pItem = GetAllocator()->New<CHE_PDF_ClipStateItem>( GetAllocator() );
	pItem->SetElement( pElement );
	MakeClipState()->Append( pItem );
	return TRUE;
}

HE_BOOL CHE_PDF_GState::PushExtGState( const CHE_ByteString & resName, CHE_PDF_DictionaryPtr dictPtr )
{
	MakeExtGState()->PushExtStateName( resName, dictPtr );
    
    //todo
    //make the common gstate data into the common gstate holder
    if ( dictPtr )
    {
        CHE_PDF_ObjectPtr objPtr;
        objPtr = dictPtr->GetElement( "LW", OBJ_TYPE_NUMBER );
        if ( objPtr )
        {
            SetLineWidth( objPtr->GetNumberPtr()->GetFloat() );
        }
        objPtr = dictPtr->GetElement( "ML", OBJ_TYPE_NUMBER );
        if ( objPtr )
        {
            SetMiterLimit( objPtr->GetNumberPtr()->GetFloat() );
        }
        objPtr = dictPtr->GetElement( "LC", OBJ_TYPE_NUMBER );
        if ( objPtr ) {
            switch ( objPtr->GetNumberPtr()->GetInteger() )
            {
            case 0:
                SetLineCap( LineCap_Butt );
                break;
            case 1:
                SetLineCap( LineCap_Round );
                break;
            case 2:
                SetLineCap( LineCap_Square );
                break;
            default:
                break;
            }
        }
        objPtr = dictPtr->GetElement( "LJ", OBJ_TYPE_NUMBER );
        if ( objPtr ) {
            switch ( objPtr->GetNumberPtr()->GetInteger() )
            {
                case 0:
                    SetLineJoin( LineJoin_Miter );
                    break;
                case 1:
                    SetLineJoin( LineJoin_Round );
                    break;
                case 2:
                    SetLineJoin( LineJoin_Bevel );
                    break;
                default:
                    break;
            }
        }
        //more work to do
    }

	return TRUE;
}

HE_VOID	CHE_PDF_GState::CopyTextState( CHE_PDF_GState * pGState )
{
	if ( mpTextState )
	{
		mpTextState->GetAllocator()->Delete( mpTextState );
		mpTextState = NULL;
		mFlag &= ~GSTATE_FLAG_Font;
		mFlag &= ~GSTATE_FLAG_TextMatirx;
		mFlag &= ~GSTATE_FLAG_CharSpace;
		mFlag &= ~GSTATE_FLAG_WordSpace;
		mFlag &= ~GSTATE_FLAG_Scaling;
		mFlag &= ~GSTATE_FLAG_Rise;
		mFlag &= ~GSTATE_FLAG_RenderMode;
	}
	if ( pGState && pGState->mpTextState )
	{
		mpTextState = pGState->mpTextState->Clone();
		mFlag |= pGState->mFlag & GSTATE_FLAG_Font;
		mFlag |= pGState->mFlag & GSTATE_FLAG_TextMatirx;
		mFlag |= pGState->mFlag & GSTATE_FLAG_CharSpace;
		mFlag |= pGState->mFlag & GSTATE_FLAG_WordSpace;
		mFlag |= pGState->mFlag & GSTATE_FLAG_Scaling;
		mFlag |= pGState->mFlag & GSTATE_FLAG_Rise;
		mFlag |= pGState->mFlag & GSTATE_FLAG_RenderMode;
	}
}

HE_BOOL IsFloatEqual( const HE_FLOAT & val1, const HE_FLOAT & val2 )
{
	if ( fabsf( val1 - val2 ) <= FLT_EPSILON )
	{
		return TRUE;
	}
	return FALSE;
}

HE_BOOL IsDefLineWidth( const HE_FLOAT & lineWidth )
{
	return IsFloatEqual( lineWidth, 1 );
}

HE_BOOL IsDefLineCap( const GRAPHICS_STATE_LINECAP & lineCap )
{
	if ( lineCap == LineCap_Butt )
	{
		return TRUE;
	}
	return FALSE;
}

HE_BOOL IsDefLineJoin( const GRAPHICS_STATE_LINEJOIN & lineJoin )
{
	if ( lineJoin == LineJoin_Miter )
	{
		return TRUE;
	}
	return FALSE;
}

HE_BOOL IsDefMiterLimit( const HE_FLOAT & miterLimit )
{
	return IsFloatEqual( miterLimit, 10 );
}

HE_BOOL IsDefLineDash( const GRAPHICS_STATE_DASHPATTERN & lineDash )
{
	if ( IsFloatEqual( lineDash.dashPhase, 0 ) && ( lineDash.dashArray.size() == 0 ) )
	{
		return TRUE;
	}
	return FALSE;
}

HE_BOOL IsDefMatrix( const CHE_Matrix & textMatrix )
{
	if ( IsFloatEqual( textMatrix.a, 1 ) && IsFloatEqual( textMatrix.b, 0 ) && IsFloatEqual( textMatrix.c, 0 ) &&
		 IsFloatEqual( textMatrix.d, 1 ) && IsFloatEqual( textMatrix.e, 0 ) && IsFloatEqual( textMatrix.f, 0 ) )
	{
		return TRUE;
	}
	return FALSE;
}

HE_BOOL IsDefTextCharSpace( const HE_FLOAT & charSpace )
{
	return IsFloatEqual( charSpace, 0 );	
}

HE_BOOL IsDefTextWrodSpace( const HE_FLOAT & wordSpace )
{
	return IsFloatEqual( wordSpace, 0 );
}

HE_BOOL IsDefTextRise( const HE_FLOAT & textRise )
{
	return IsFloatEqual( textRise, 0 );
}

HE_BOOL IsDefTextScaling( const HE_FLOAT & textScaling )
{
	return IsFloatEqual( textScaling, 100 );
}

HE_BOOL IsDefTextRenderMode( const GRAPHICS_STATE_TEXTRENDERMODE & rm )
{
	if ( rm == TextRenderMode_Fill )
	{
		return TRUE;
	}
	return FALSE;
}

HE_BOOL IsDefFlatness( const HE_FLOAT & flatness )
{
	return IsFloatEqual( flatness, 0 );
}

HE_BOOL IsDefRenderIntents( const GRAPHICS_STATE_RENDERINTENTS & ri )
{
	if ( ri == RI_AbsoluteColorimetric )
	{
		return TRUE;
	}
	return FALSE;
}

HE_BOOL IsDefColorSpace( const CHE_PDF_ColorSpace & colorSpace )
{
	if ( colorSpace.GetType() == COLORSPACE_DEVICE_GRAY )
	{
		return TRUE;
	}
	return FALSE;
}

HE_BOOL IsDefColor( const CHE_PDF_Color & color )
{
	if ( color.mConponents.size() == 1 && IsFloatEqual( color.mConponents[0], 0 ) )
	{
		return TRUE;
	}
	return FALSE;
}

HE_BOOL	IsColorSpaceEqual( const CHE_PDF_ColorSpace & cs1, const CHE_PDF_ColorSpace & cs2 )
{
	if ( cs1.GetType() == cs2.GetType() && cs1.GetResName() == cs2.GetResName() )
	{
		return TRUE;
	}
	return FALSE;
}

HE_BOOL IsColorEqual( const CHE_PDF_Color & c1, const CHE_PDF_Color & c2 )
{
	if ( c1.mConponents.size() != c2.mConponents.size() )
	{
		return FALSE;
	}
	for ( HE_ULONG i = 0; i < c1.mConponents.size(); ++i )
	{
		if ( c1.mConponents[i] != c2.mConponents[i] )
		{
			return FALSE;
		}
	}
	return TRUE;
}

HE_BOOL IsExtGStateEqual( const CHE_PDF_ExtGState * pExtGS1, const CHE_PDF_ExtGState * pExtGS2 )
{
	if ( pExtGS1 == pExtGS2 )
	{
		return TRUE;
	}

	if ( pExtGS1 == NULL || pExtGS2 == NULL )
	{
		return FALSE;
	}

	if (	! IsFloatEqual( pExtGS1->GetFillAlpha(), pExtGS2->GetFillAlpha() )		|| 
			! IsFloatEqual( pExtGS1->GetStrokeAlpha(), pExtGS2->GetStrokeAlpha() )	||
			pExtGS1->mExtDictNameList.size() != pExtGS2->mExtDictNameList.size() )
	{
		return FALSE;
	}

	std::list<CHE_ByteString>::const_iterator it1, it2;
	it1 = pExtGS1->mExtDictNameList.begin();
	it2 = pExtGS2->mExtDictNameList.begin();

	for ( ; it1 != pExtGS1->mExtDictNameList.end() && it2 != pExtGS2->mExtDictNameList.end(); ++it1, ++it2 )
	{
		if ( *it1 != *it2 )
		{
			return FALSE;
		}
	}
	return TRUE;
}

HE_BOOL IsExtGStateContinue( const CHE_PDF_ExtGState * pExtGS1, const CHE_PDF_ExtGState * pExtGS2 )
{
	if ( pExtGS1 == NULL && pExtGS2 != NULL )
	{
		return TRUE;
	}

	if ( pExtGS1 != NULL && pExtGS2 == NULL )
	{
		return FALSE;
	}

	if ( pExtGS1 == NULL && pExtGS2 == NULL )
	{
		return FALSE;
	}

	if ( pExtGS2->mExtDictNameList.size() <= pExtGS1->mExtDictNameList.size() )
	{
		return FALSE;
	}

	std::list<CHE_ByteString>::const_iterator it1, it2;
	it1 = pExtGS1->mExtDictNameList.begin();
	it2 = pExtGS2->mExtDictNameList.begin();
	for ( ; it1 != pExtGS1->mExtDictNameList.end(); ++it1, ++it2 )
	{
		if ( *it1 != *it2 )
		{
			return FALSE;
		}
	}

	return TRUE;
}

HE_BOOL IsClipStateEqual( const CHE_PDF_ClipState * pClipGS1, const CHE_PDF_ClipState * pClipGS2 )
{
	if ( pClipGS1 == pClipGS2 )
	{
		return TRUE;
	}

	if ( pClipGS1 == NULL || pClipGS2 == NULL )
	{
		return FALSE;
	}

	if ( pClipGS1->mClipElementList.size() != pClipGS2->mClipElementList.size() )
	{
		return FALSE;
	}

	std::list<CHE_PDF_ClipStateItem*>::const_iterator it1, it2;
	CHE_PDF_ContentObject * pElement1 = NULL, * pElement2 = NULL;
	it1 = pClipGS1->mClipElementList.begin();
	it2 = pClipGS2->mClipElementList.begin();

	for ( ; it1 != pClipGS1->mClipElementList.end() && it2 != pClipGS2->mClipElementList.end(); ++it1, ++it2 )
	{
        //zctodo
		//if ( (*it1)->GetMatrix() != (*it2)->GetMatrix() )
		//{
		//	return FALSE;
		//}
		pElement1 = (*it1)->GetElement();
		pElement2 = (*it2)->GetElement();
		if ( pElement1->GetType() != pElement2->GetType() )
		{
			return FALSE;
		}
		if ( pElement1->GetExtMatrix() != pElement2->GetExtMatrix() )
		{
			return FALSE;
		}
//		zctodo 判断clip对应的图形状态是不是相等
// 		if ( pElement1->GetGState() != pElement2->GetGState() )
// 		{
// 			return FALSE;
// 		}
		if ( pElement1->GetType() == ContentType_Text && pElement2->GetType() == ContentType_Text )
		{
			CHE_PDF_Text * pText1 = (CHE_PDF_Text*)( pElement1 );
			CHE_PDF_Text * pText2 = (CHE_PDF_Text*)( pElement2 );
			//zctodo
		}else if ( pElement1->GetType() == ContentType_Path && pElement2->GetType() == ContentType_Path )
		{
			CHE_PDF_Path * pPath1 = (CHE_PDF_Path*)( pElement1 );
			CHE_PDF_Path * pPath2 = (CHE_PDF_Path*)( pElement2 );
//			zctodo
// 			if (	pPath1->GetGState() != pPath2->GetGState() )
// 			{
// 				return FALSE;
// 			}
			if (	pPath1->GetExtMatrix() != pPath2->GetExtMatrix() ||
					pPath1->GetFillMode() != pPath2->GetFillMode() ||
					pPath1->GetPaintType() != pPath2->GetPaintType() )
			{
				return FALSE;
			}
			if ( pPath1->mItems.size() != pPath2->mItems.size() )
			{
				return FALSE;
			}
			for ( HE_ULONG i = 0; i != pPath1->mItems.size(); ++i )
			{
				if ( pPath1->mItems[i].type != pPath2->mItems[i].type ||
					 pPath1->mItems[i].value != pPath2->mItems[i].value )
				{
					return FALSE;
				}
			}
		}else{
			return FALSE;
		}
	}
	return TRUE;
}

HE_BOOL IsClipStateContinue( const CHE_PDF_ClipState * pClipGS1, const CHE_PDF_ClipState * pClipGS2 )
{
	return FALSE;
}