#include "../../include/pdf/che_pdf_gstate.h"
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

bool CHE_PDF_ClipStateItem::SetElement( CHE_PDF_ContentObject * pElement )
{
	if ( pElement == nullptr )
	{
		return false;
	}
	if ( pElement->GetType() != ContentType_Text && pElement->GetType() != ContentType_Path )
	{
		return false;
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

bool CHE_PDF_ClipState::Append( CHE_PDF_ClipStateItem * pItem )
{
	if ( pItem == nullptr )
	{
		return false;
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

bool CHE_PDF_ExtGStateStack::PushExtStateName( const CHE_ByteString & name, const CHE_PDF_ExtGStatePtr & extgstate )
{
	if ( ! extgstate )
	{
		return false;
	}
	std::list<CHE_ByteString>::iterator itn = mExtGStateName.begin();
	for ( ; itn != mExtGStateName.end(); ++itn )
	{
		if ( *itn == name )
		{
			mExtGStateName.erase( itn );
			break;
		}
	}
	mExtGStateName.push_back( name );

	CHE_PDF_ObjectPtr objPtr;
	std::vector<CHE_PDF_ExtGStateItem>::iterator iti = extgstate->mStateItems.begin();
	for ( ; iti != extgstate->mStateItems.end(); ++iti )
	{
		switch ( iti->type )
		{
		case PDF_EXTGSTATE_BM:
			{
				CHE_ByteString blendName = iti->objPtr->GetNamePtr()->GetString();
				if ( blendName == "Normal" )
				{
					mBlendMode = BlendMode_Normal;
				}else if ( blendName == "ColorBurn" )
				{
					mBlendMode = BlendMode_ColorBurn;
				}else if ( blendName == "ColorDodge" )
				{
					mBlendMode = BlendMode_ColorDodge;
				}else if ( blendName == "Darken" )
				{
					mBlendMode = BlendMode_Darken;
				}else if ( blendName == "Difference" )
				{
					mBlendMode = BlendMode_Difference;
				}else if ( blendName == "Exclusion" )
				{
					mBlendMode = BlendMode_Exclusion;
				}else if ( blendName == "HardLight" )
				{
					mBlendMode = BlendMode_HardLight;
				}else if ( blendName == "Lighten" )
				{
					mBlendMode = BlendMode_Lighten;
				}else if ( blendName == "Multiply" )
				{
					mBlendMode = BlendMode_Multiply;
				}else if ( blendName == "Overlay" )
				{
					mBlendMode = BlendMode_Overlay;
				}else if ( blendName == "Screen" )
				{
					mBlendMode = BlendMode_Screen;
				}else if ( blendName == "SoftLight" )
				{
					mBlendMode = BlendMode_SoftLight;
				}else if ( blendName == "Hue" )
				{
					mBlendMode = BlendMode_Hue;
				}else if ( blendName == "Saturation" )
				{
					mBlendMode = BlendMode_Saturation;
				}else if ( blendName == "Color" )
				{
					mBlendMode = BlendMode_Color;
				}else if ( blendName == "Luminosity" )
				{
					mBlendMode = BlendMode_Luminosity;
				}
				break;
			}
		case PDF_EXTGSTATE_CA:
			mStrokeAlpha = iti->objPtr->GetNumberPtr()->GetFloat();
			break;
		case PDF_EXTGSTATE_ca:
			mFillAlpha = iti->objPtr->GetNumberPtr()->GetFloat();
            break;
        case PDF_EXTGSTATE_SMask:
            {
                if ( iti->objPtr->GetType() == OBJ_TYPE_DICTIONARY )
                {
                    mSMask = iti->objPtr->GetDictPtr();
                    /*CHE_PDF_DictionaryPtr dict = objPtr->GetDictPtr();
                    CHE_PDF_ObjectPtr obj = dict->GetElement("G", OBJ_TYPE_STREAM);
                    if ( obj )
                    {
                        
                    }*/
                }
                break;
            }
		default:break;
		}
	}
	return TRUE;
}

CHE_PDF_ExtGStateStack * CHE_PDF_ExtGStateStack::Clone() const
{
	CHE_PDF_ExtGStateStack * pRet = GetAllocator()->New<CHE_PDF_ExtGStateStack>( GetAllocator() );
	pRet->mExtGStateName = mExtGStateName;
	pRet->mFillAlpha = mFillAlpha;
	pRet->mStrokeAlpha = mStrokeAlpha;
    pRet->mBlendMode = mBlendMode;
    pRet->mSMask = mSMask;
	return pRet;
}

bool CHE_PDF_ExtGStateStack::operator == ( const CHE_PDF_ExtGStateStack & gs ) const
{
	if ( ! IsFloatEqual( mFillAlpha, gs.mFillAlpha ) ||
		 ! IsFloatEqual( mStrokeAlpha, gs.mFillAlpha ) )
	{
		return false;
	}

	std::list<CHE_ByteString>::const_iterator it = mExtGStateName.begin();
	std::list<CHE_ByteString>::const_iterator tmpIt = gs.mExtGStateName.begin();

	for ( ; it != mExtGStateName.end() && tmpIt != gs.mExtGStateName.end() ; ++it, ++tmpIt )
	{
		if ( *it != *tmpIt )
		{
			return false;
		}
	}

	if ( tmpIt != gs.mExtGStateName.end() || it != mExtGStateName.end() )
	{
		return false;
	}
	
	return true;
}

bool CHE_PDF_ExtGStateStack::operator != ( const CHE_PDF_ExtGStateStack & gs ) const
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

CHE_PDF_ExtGStateStack *	CHE_PDF_GState::MakeExtGState()
{
	if ( !mpExtState )
	{
		mpExtState = GetAllocator()->New<CHE_PDF_ExtGStateStack>( GetAllocator() );
	}
	return mpExtState;
}

CHE_PDF_GState * CHE_PDF_GState::Clone() const
{
	CHE_PDF_GState * pRet = GetAllocator()->New<CHE_PDF_GState>( GetAllocator() );
	pRet->mMatrix = mMatrix;
	pRet->mFlag = mFlag;
	pRet->mFillColor = mFillColor;
	pRet->mStrokeColor = mStrokeColor;
	pRet->mFillColorSpace = mFillColorSpace;
	pRet->mStrokeColorSpace = mStrokeColorSpace;

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

void	CHE_PDF_GState::GetFillColor( CHE_PDF_Color & colorRet ) const
{
	if ( mFlag & GSTATE_FLAG_FillColor )
	{
		colorRet = mFillColor;
		return;
	}
	
	colorRet.Clear();

	if ( mFlag & GSTATE_FLAG_FillColorSpace )
	{
		switch ( mFillColorSpace->GetColorSpaceType() )
		{
		case COLORSPACE_DEVICE_GRAY:
		case COLORSPACE_CIEBASE_CALGRAY:
		case COLORSPACE_SPECIAL_SEPARATION:
		case COLORSPACE_SPECIAL_DEVICEN:
		case COLORSPACE_SPECIAL_INDEXED:
			colorRet.Push( 0 );
			break;
		case COLORSPACE_DEVICE_RGB:
		case COLORSPACE_CIEBASE_CALRGB:
			colorRet.Push( 0 );
			colorRet.Push( 0 );
			colorRet.Push( 0 );
			break;
		case COLORSPACE_DEVICE_CMYK:
			colorRet.Push( 0 );
			colorRet.Push( 0 );
			colorRet.Push( 0 );
			colorRet.Push( 0 );
			break;
		case COLORSPACE_CIEBASE_CALLAB:
			colorRet.Push( 0 );
			colorRet.Push( 0 );
			colorRet.Push( 0 );
			break;
		case COLORSPACE_CIEBASE_ICCBASED:
            {
                uint32 c = mFillColorSpace->GetICCBasedPtr()->mAlternate->GetComponentCount();
                for (uint32 i = 0; i < c; ++i)
                {
                    colorRet.Push( 0 );
                }
            }
            break;
		case COLORSPACE_SPECIAL_PATTERN:break;
		default:break;
		}
	}else{
		colorRet.Push( 0 );
	}
}

void CHE_PDF_GState::GetStrokeColor( CHE_PDF_Color & colorRet ) const
{
	if ( mFlag & GSTATE_FLAG_StrokeColor )
	{
		colorRet = mStrokeColor;
		return;
	}

	colorRet.Clear();

	if ( mFlag & GSTATE_FLAG_StrokeColorSpace )
	{
		switch ( mStrokeColorSpace->GetColorSpaceType() )
		{
		case COLORSPACE_DEVICE_GRAY:
		case COLORSPACE_CIEBASE_CALGRAY:
		case COLORSPACE_SPECIAL_SEPARATION:
		case COLORSPACE_SPECIAL_DEVICEN:
		case COLORSPACE_SPECIAL_INDEXED:
			colorRet.Push( 0 );
			break;
		case COLORSPACE_DEVICE_RGB:
		case COLORSPACE_CIEBASE_CALRGB:
			colorRet.Push( 0 );
			colorRet.Push( 0 );
			colorRet.Push( 0 );
			break;
		case COLORSPACE_DEVICE_CMYK:
			colorRet.Push( 0 );
			colorRet.Push( 0 );
			colorRet.Push( 0 );
			colorRet.Push( 0 );
			break;
		case COLORSPACE_CIEBASE_CALLAB:
			colorRet.Push( 0 );
			colorRet.Push( 0 );
			colorRet.Push( 0 );
			break;
		case COLORSPACE_CIEBASE_ICCBASED:
            {
                uint32 c = mStrokeColorSpace->GetICCBasedPtr()->mAlternate->GetComponentCount();
                for (uint32 i = 0; i < c; ++i)
                {
                    colorRet.Push( 0 );
                }
            }
            break;
		case COLORSPACE_SPECIAL_PATTERN:break;
		default:break;
		}
	}else{
		colorRet.Push( 0 );
	}
}

void CHE_PDF_GState::GetFillColorSpace( CHE_PDF_ColorSpacePtr & colorSpaceRet ) const
{
	if ( mFlag & GSTATE_FLAG_FillColorSpace )
	{
		colorSpaceRet = mFillColorSpace;
		return;
	}
	colorSpaceRet = CHE_PDF_ColorSpace::CreateDeviceGray();
}

void CHE_PDF_GState::GetStrokeColorSpace( CHE_PDF_ColorSpacePtr & colorSpaceRet ) const
{
	if ( mFlag & GSTATE_FLAG_StrokeColorSpace )
	{
		colorSpaceRet = mStrokeColorSpace;
		return;
	}
	colorSpaceRet = CHE_PDF_ColorSpace::CreateDeviceGray();
}

void	CHE_PDF_GState::GetLineWidth( FLOAT & lineWidthRet ) const
{
	if ( mFlag & GSTATE_FLAG_LineWidth )
	{
		lineWidthRet = mpStrokeState->GetLineWidth();
		return;
	}
	lineWidthRet = 1;
}

void CHE_PDF_GState::GetMiterLimit( FLOAT & miterLimitRet ) const
{
	if ( mFlag & GSTATE_FLAG_MiterLimit )
	{
		miterLimitRet = mpStrokeState->GetMiterLimit();
		return;
	}
	miterLimitRet = 10;
}

void CHE_PDF_GState::GetLineCap( GRAPHICS_STATE_LINECAP & lineCapRet ) const
{
	if ( mFlag & GSTATE_FLAG_LineCap )
	{
		lineCapRet = mpStrokeState->GetLineCap();
		return;
	}
	lineCapRet = LineCap_Butt;
}

void CHE_PDF_GState::GetLineJoin( GRAPHICS_STATE_LINEJOIN & lineJoinRet ) const
{
	if ( mFlag & GSTATE_FLAG_LineJoin )
	{
		lineJoinRet = mpStrokeState->GetLineJoin();
		return;
	}
	lineJoinRet = LineJoin_Miter;
}

void CHE_PDF_GState::GetLineDash( GRAPHICS_STATE_DASHPATTERN & lineDash ) const
{
	if ( mFlag & GSTATE_FLAG_LineDash )
	{
		lineDash = mpStrokeState->GetLineDash();
		return;
	}
	lineDash.dashPhase = 0;
	lineDash.dashArray.clear();
}

void CHE_PDF_GState::GetTextMatrix( CHE_Matrix & matrixRet ) const
{
	if ( mFlag & GSTATE_FLAG_TextMatirx )
	{
		matrixRet = mpTextState->GetMatrix();
		return;
	}
	matrixRet = CHE_Matrix();
}

void CHE_PDF_GState::GetTextFontSize( FLOAT & fontSizeRet ) const
{
	if ( mFlag & GSTATE_FLAG_Font )
	{
		fontSizeRet = mpTextState->GetFontSize();
		return;
	}
	fontSizeRet = 1;
}

void CHE_PDF_GState::GetTextCharSpace( FLOAT & charSpaceRet ) const
{
	if ( mFlag & GSTATE_FLAG_CharSpace )
	{
		charSpaceRet = mpTextState->GetCharSpace();
		return;
	}
	charSpaceRet = 0;
}

void CHE_PDF_GState::GetTextWordSpace( FLOAT & wordSpaceRet ) const
{
	if ( mFlag & GSTATE_FLAG_WordSpace )
	{
		wordSpaceRet = mpTextState->GetWordSpace();
		return;
	}
	wordSpaceRet = 0;
}

void CHE_PDF_GState::GetTextScaling( FLOAT & scalingRet ) const
{
	if ( mFlag & GSTATE_FLAG_Scaling )
	{
		scalingRet = mpTextState->GetScaling();
		return;
	}
	scalingRet = 100;
}

void CHE_PDF_GState::GetTextRise( FLOAT & riseRet ) const
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
	return nullptr;
}

void CHE_PDF_GState::GetTextFontResName( CHE_ByteString & resNameRet ) const
{
	if ( mFlag & GSTATE_FLAG_Font )
	{
		resNameRet = mpTextState->GetFontResName();
		return;
	}
	resNameRet.Clear();
}

void CHE_PDF_GState::GetTextRenderMode( GRAPHICS_STATE_TEXTRENDERMODE & rm ) const
{
	if ( mFlag & GSTATE_FLAG_RenderMode )
	{
		rm = mpTextState->GetRenderMode();
		return;
	}
	rm = TextRenderMode_Fill;
}

void CHE_PDF_GState::SetMatrix( const CHE_Matrix & matirx )
{
	mMatrix = matirx;
}

void CHE_PDF_GState::SetRenderIntents( const GRAPHICS_STATE_RENDERINTENTS & ri )
{
	mFlag |= GSTATE_FLAG_RenderIntents;
	mRenderIntents = ri;
}
void CHE_PDF_GState::SetFlatness( const FLOAT & flatness )
{
	mFlag |= GSTATE_FLAG_Flatness;
	mFlatness = flatness;
}

bool CHE_PDF_GState::SetFillColor( const CHE_PDF_Color & color )
{
	mFillColor = color;
	mFlag |= GSTATE_FLAG_FillColor;
	return TRUE;
}

bool CHE_PDF_GState::SetStrokeColor( const CHE_PDF_Color & color )
{
	mStrokeColor = color;
	mFlag |= GSTATE_FLAG_StrokeColor;
	return TRUE;
}

bool CHE_PDF_GState::SetFillColorSpace( CHE_PDF_ColorSpacePtr colorSpace )
{
	mFillColorSpace = colorSpace;
	mFlag |= GSTATE_FLAG_FillColorSpace;
	return TRUE;
}

bool CHE_PDF_GState::SetStrokeColorSpace( CHE_PDF_ColorSpacePtr colorSpace )
{
	mStrokeColorSpace = colorSpace;
	mFlag |= GSTATE_FLAG_StrokeColorSpace;
	return TRUE;
}

void	CHE_PDF_GState::SetLineWidth( const FLOAT & lineWidth )
{
	MakeStrokeState()->SetLineWidth( lineWidth );
	mFlag |= GSTATE_FLAG_LineWidth;
}

void	CHE_PDF_GState::SetMiterLimit( const FLOAT & miterLimit )
{
	MakeStrokeState()->SetMiterLimit( miterLimit );
	mFlag |= GSTATE_FLAG_MiterLimit;
}

void	CHE_PDF_GState::SetLineCap( const GRAPHICS_STATE_LINECAP & lineCap )
{
	MakeStrokeState()->SetLineCap( lineCap );
	mFlag |= GSTATE_FLAG_LineCap;
}

void	CHE_PDF_GState::SetLineJoin( const GRAPHICS_STATE_LINEJOIN & lineJion )
{
	MakeStrokeState()->SetLineJoin( lineJion );
	mFlag |= GSTATE_FLAG_LineJoin;
}

void	CHE_PDF_GState::SetLineDash( const GRAPHICS_STATE_DASHPATTERN & dashPattern )
{
	MakeStrokeState()->SetLineDash( dashPattern );
	mFlag |= GSTATE_FLAG_LineDash;
}

void CHE_PDF_GState::SetTextMatrix( const CHE_Matrix & matrix )
{
	MakeTextState()->SetMatirx( matrix );
	mFlag |= GSTATE_FLAG_TextMatirx;
}

void CHE_PDF_GState::SetTextFont( CHE_PDF_Font * pFont )
{
	MakeTextState()->SetFont( pFont );
	mFlag |= GSTATE_FLAG_Font;
}

void CHE_PDF_GState::SetTextFontSize( const FLOAT & size )
{
	MakeTextState()->SetFontSize( size );
	mFlag |= GSTATE_FLAG_Font;
}

void CHE_PDF_GState::SetTextFontResName( const CHE_ByteString & resName )
{
	MakeTextState()->SetFontResName( resName );
	mFlag |= GSTATE_FLAG_Font;
}

void CHE_PDF_GState::SetTextCharSpace( const FLOAT & charSpace )
{
	MakeTextState()->SetCharSpace( charSpace );
	mFlag |= GSTATE_FLAG_CharSpace;
}

void CHE_PDF_GState::SetTextWordSpace( const FLOAT & wordSpace )
{
	MakeTextState()->SetWordSpace( wordSpace );
	mFlag |= GSTATE_FLAG_WordSpace;
}

// void CHE_PDF_GState::SetTextLeading( const FLOAT & leading )
// {
// 	MakeTextState()->SetLeading( leading );
// 	mFlag |= GSTATE_FLAG_Leading;
// }

void CHE_PDF_GState::SetTextScaling( const FLOAT & scaling )
{
	MakeTextState()->SetScaling( scaling );
	mFlag |= GSTATE_FLAG_Scaling;
}

void CHE_PDF_GState::SetTextRise( const FLOAT & rise )
{
	MakeTextState()->SetRise( rise );
	mFlag |= GSTATE_FLAG_Rise;
}

void CHE_PDF_GState::SetTextRenderMode( const GRAPHICS_STATE_TEXTRENDERMODE & rm )
{
	MakeTextState()->SetRenderMode( rm );
	mFlag |= GSTATE_FLAG_RenderMode;
}

bool CHE_PDF_GState::PushClipElement( CHE_PDF_ContentObject * pElement )
{
	if ( !pElement )
	{
		return false;
	}
	CHE_PDF_ClipStateItem * pItem = GetAllocator()->New<CHE_PDF_ClipStateItem>( GetAllocator() );
	pItem->SetElement( pElement );
	MakeClipState()->Append( pItem );
	return TRUE;
}

bool CHE_PDF_GState::PushExtGState( const CHE_ByteString & resName, const CHE_PDF_ExtGStatePtr & extgstate )
{
	MakeExtGState()->PushExtStateName( resName, extgstate );

	CHE_PDF_ObjectPtr objPtr;
	std::vector<CHE_PDF_ExtGStateItem>::iterator iti = extgstate->mStateItems.begin();
	for ( ; iti != extgstate->mStateItems.end(); ++iti )
	{
		switch ( iti->type )
		{
		case PDF_EXTGSTATE_LW:
			{
				SetLineWidth( iti->objPtr->GetNumberPtr()->GetFloat() );
				break;
			}
		case PDF_EXTGSTATE_LC:
			{
				switch ( iti->objPtr->GetNumberPtr()->GetInteger() )
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
				break;
			}
		case PDF_EXTGSTATE_LJ:
			{
				switch ( iti->objPtr->GetNumberPtr()->GetInteger() )
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
		case PDF_EXTGSTATE_ML:
			{
				SetMiterLimit( iti->objPtr->GetNumberPtr()->GetFloat() );
				break;
			}
		case PDF_EXTGSTATE_D:
		case PDF_EXTGSTATE_RI:
		case PDF_EXTGSTATE_FL:
		default:break;
		}
	}

	return TRUE;
}

void	CHE_PDF_GState::CopyTextState( CHE_PDF_GState * pGState )
{
	if ( mpTextState )
	{
		mpTextState->GetAllocator()->Delete( mpTextState );
		mpTextState = nullptr;
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

bool IsFloatEqual( const FLOAT & val1, const FLOAT & val2 )
{
	if ( fabsf( val1 - val2 ) <= FLT_EPSILON )
	{
		return TRUE;
	}
	return false;
}

bool IsDefLineWidth( const FLOAT & lineWidth )
{
	return IsFloatEqual( lineWidth, 1 );
}

bool IsDefLineCap( const GRAPHICS_STATE_LINECAP & lineCap )
{
	if ( lineCap == LineCap_Butt )
	{
		return TRUE;
	}
	return false;
}

bool IsDefLineJoin( const GRAPHICS_STATE_LINEJOIN & lineJoin )
{
	if ( lineJoin == LineJoin_Miter )
	{
		return TRUE;
	}
	return false;
}

bool IsDefMiterLimit( const FLOAT & miterLimit )
{
	return IsFloatEqual( miterLimit, 10 );
}

bool IsDefLineDash( const GRAPHICS_STATE_DASHPATTERN & lineDash )
{
	if ( IsFloatEqual( lineDash.dashPhase, 0 ) && ( lineDash.dashArray.size() == 0 ) )
	{
		return TRUE;
	}
	return false;
}

bool IsDefMatrix( const CHE_Matrix & textMatrix )
{
	if ( IsFloatEqual( textMatrix.a, 1 ) && IsFloatEqual( textMatrix.b, 0 ) && IsFloatEqual( textMatrix.c, 0 ) &&
		 IsFloatEqual( textMatrix.d, 1 ) && IsFloatEqual( textMatrix.e, 0 ) && IsFloatEqual( textMatrix.f, 0 ) )
	{
		return TRUE;
	}
	return false;
}

bool IsDefTextCharSpace( const FLOAT & charSpace )
{
	return IsFloatEqual( charSpace, 0 );	
}

bool IsDefTextWrodSpace( const FLOAT & wordSpace )
{
	return IsFloatEqual( wordSpace, 0 );
}

bool IsDefTextRise( const FLOAT & textRise )
{
	return IsFloatEqual( textRise, 0 );
}

bool IsDefTextScaling( const FLOAT & textScaling )
{
	return IsFloatEqual( textScaling, 100 );
}

bool IsDefTextRenderMode( const GRAPHICS_STATE_TEXTRENDERMODE & rm )
{
	if ( rm == TextRenderMode_Fill )
	{
		return TRUE;
	}
	return false;
}

bool IsDefFlatness( const FLOAT & flatness )
{
	return IsFloatEqual( flatness, 0 );
}

bool IsDefRenderIntents( const GRAPHICS_STATE_RENDERINTENTS & ri )
{
	if ( ri == RI_AbsoluteColorimetric )
	{
		return TRUE;
	}
	return false;
}

bool IsDefColorSpace( const CHE_PDF_ColorSpace & colorSpace )
{
	if ( colorSpace.GetColorSpaceType() == COLORSPACE_DEVICE_GRAY )
	{
		return TRUE;
	}
	return false;
}

bool IsDefColor( const CHE_PDF_Color & color )
{
	if ( color.GetComponentCount() == 1 && IsFloatEqual( color.GetComponent(0), 0 ) )
	{
		return TRUE;
	}
	return false;
}

bool	IsColorSpaceEqual( const CHE_PDF_ColorSpace & cs1, const CHE_PDF_ColorSpace & cs2 )
{
	//zctodo
	if ( cs1.GetType() == cs2.GetType() /*&& cs1.GetResName() == cs2.GetResName()*/ )
	{
		return TRUE;
	}
	return false;
}

bool IsColorEqual( const CHE_PDF_Color & c1, const CHE_PDF_Color & c2 )
{
	if ( c1.GetComponentCount() != c2.GetComponentCount() )
	{
		return false;
	}
	for ( uint32 i = 0; i < c1.GetComponentCount(); ++i )
	{
		if ( c1.GetComponent(i) != c2.GetComponent(i) )
		{
			return false;
		}
	}
	return TRUE;
}

bool IsExtGStateEqual( const CHE_PDF_ExtGStateStack * pExtGS1, const CHE_PDF_ExtGStateStack * pExtGS2 )
{
	if ( pExtGS1 == pExtGS2 )
	{
		return TRUE;
	}

	if ( pExtGS1 == nullptr || pExtGS2 == nullptr )
	{
		return false;
	}

	if (	! IsFloatEqual( pExtGS1->GetFillAlpha(), pExtGS2->GetFillAlpha() )		|| 
			! IsFloatEqual( pExtGS1->GetStrokeAlpha(), pExtGS2->GetStrokeAlpha() )	||
			pExtGS1->mExtGStateName.size() != pExtGS2->mExtGStateName.size() )
	{
		return false;
	}

	std::list<CHE_ByteString>::const_iterator it1, it2;
	it1 = pExtGS1->mExtGStateName.begin();
	it2 = pExtGS2->mExtGStateName.begin();

	for ( ; it1 != pExtGS1->mExtGStateName.end() && it2 != pExtGS2->mExtGStateName.end(); ++it1, ++it2 )
	{
		if ( *it1 != *it2 )
		{
			return false;
		}
	}
	return TRUE;
}

bool IsExtGStateContinue( const CHE_PDF_ExtGStateStack * pExtGS1, const CHE_PDF_ExtGStateStack * pExtGS2 )
{
	if ( pExtGS1 == nullptr && pExtGS2 != nullptr )
	{
		return TRUE;
	}

	if ( pExtGS1 != nullptr && pExtGS2 == nullptr )
	{
		return false;
	}

	if ( pExtGS1 == nullptr && pExtGS2 == nullptr )
	{
		return false;
	}

	if ( pExtGS2->mExtGStateName.size() <= pExtGS1->mExtGStateName.size() )
	{
		return false;
	}

	std::list<CHE_ByteString>::const_iterator it1, it2;
	it1 = pExtGS1->mExtGStateName.begin();
	it2 = pExtGS2->mExtGStateName.begin();
	for ( ; it1 != pExtGS1->mExtGStateName.end(); ++it1, ++it2 )
	{
		if ( *it1 != *it2 )
		{
			return false;
		}
	}

	return TRUE;
}

bool IsClipStateEqual( const CHE_PDF_ClipState * pClipGS1, const CHE_PDF_ClipState * pClipGS2 )
{
	if ( pClipGS1 == pClipGS2 )
	{
		return TRUE;
	}

	if ( pClipGS1 == nullptr || pClipGS2 == nullptr )
	{
		return false;
	}

	if ( pClipGS1->mClipElementList.size() != pClipGS2->mClipElementList.size() )
	{
		return false;
	}

	std::list<CHE_PDF_ClipStateItem*>::const_iterator it1, it2;
	CHE_PDF_ContentObject * pElement1 = nullptr, * pElement2 = nullptr;
	it1 = pClipGS1->mClipElementList.begin();
	it2 = pClipGS2->mClipElementList.begin();

	for ( ; it1 != pClipGS1->mClipElementList.end() && it2 != pClipGS2->mClipElementList.end(); ++it1, ++it2 )
	{
        //zctodo
		//if ( (*it1)->GetMatrix() != (*it2)->GetMatrix() )
		//{
		//	return false;
		//}
		pElement1 = (*it1)->GetElement();
		pElement2 = (*it2)->GetElement();
		if ( pElement1->GetType() != pElement2->GetType() )
		{
			return false;
		}
// 		if ( pElement1->GetExtMatrix() != pElement2->GetExtMatrix() )
// 		{
// 			return false;
// 		}
//		zctodo 判断clip对应的图形状态是不是相等
// 		if ( pElement1->GetGState() != pElement2->GetGState() )
// 		{
// 			return false;
// 		}
		if ( pElement1->GetType() == ContentType_Text && pElement2->GetType() == ContentType_Text )
		{
			//CHE_PDF_Text * pText1 = (CHE_PDF_Text*)( pElement1 );
			//CHE_PDF_Text * pText2 = (CHE_PDF_Text*)( pElement2 );
			//zctodo
		}else if ( pElement1->GetType() == ContentType_Path && pElement2->GetType() == ContentType_Path )
		{
			CHE_PDF_Path * pPath1 = (CHE_PDF_Path*)( pElement1 );
			CHE_PDF_Path * pPath2 = (CHE_PDF_Path*)( pElement2 );
//			zctodo
// 			if (	pPath1->GetGState() != pPath2->GetGState() )
// 			{
// 				return false;
// 			}
			if (	
// 					pPath1->GetExtMatrix() != pPath2->GetExtMatrix() ||
// 					pPath1->GetFillMode() != pPath2->GetFillMode() ||
					pPath1->GetPaintType() != pPath2->GetPaintType() )
			{
				return false;
			}
			if ( pPath1->mItems.size() != pPath2->mItems.size() )
			{
				return false;
			}
			for ( size_t i = 0; i != pPath1->mItems.size(); ++i )
			{
				if ( pPath1->mItems[i].type != pPath2->mItems[i].type ||
					 pPath1->mItems[i].value != pPath2->mItems[i].value )
				{
					return false;
				}
			}
		}else{
			return false;
		}
	}
	return TRUE;
}

bool IsClipStateContinue( const CHE_PDF_ClipState * pClipGS1, const CHE_PDF_ClipState * pClipGS2 )
{
	return false;
}