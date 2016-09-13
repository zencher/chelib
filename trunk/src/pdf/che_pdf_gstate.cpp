#include "../../include/pdf/che_pdf_gstate.h"
#include "../../include/pdf/che_pdf_contentobject.h"

namespace chelib {

CPDF_StrokeState * CPDF_StrokeState::Clone() const
{
	CPDF_StrokeState * pRet = GetAllocator()->New<CPDF_StrokeState>( GetAllocator() );
	pRet->mLineCap = mLineCap;
	pRet->mLineDash = mLineDash;
	pRet->mLineJoin = mLineJoin;
	pRet->mLineWidth = mLineWidth;
	pRet->mMiterLimit = mMiterLimit;
	pRet->mLineDash.dashPhase = mLineDash.dashPhase;
	pRet->mLineDash.dashArray = mLineDash.dashArray;
	return pRet;
}

CPDF_TextGState * CPDF_TextGState::Clone() const
{
	CPDF_TextGState * pRet = GetAllocator()->New<CPDF_TextGState>( GetAllocator() );
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

CPDF_ClipStateItem::~CPDF_ClipStateItem()
{
	if ( mpClipElement )
	{
		mpClipElement->GetAllocator()->Delete( mpClipElement );
	}
}

bool CPDF_ClipStateItem::SetElement( CPDF_ContentObject * pElement )
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

CPDF_ClipStateItem * CPDF_ClipStateItem::Clone() const
{
	CPDF_ClipStateItem * pRet = GetAllocator()->New<CPDF_ClipStateItem>( GetAllocator() );
	pRet->SetElement( mpClipElement->Clone() );
	return pRet;
}

CPDF_ClipState::~CPDF_ClipState()
{
	if ( mClipElementList.size() )
	{
		std::list<CPDF_ClipStateItem*>::iterator it = mClipElementList.begin();
		for ( ; it != mClipElementList.end(); ++it )
		{
			if ( *it )
			{
				(*it)->GetAllocator()->Delete( *it );
			}
		}
	}
}

bool CPDF_ClipState::Append( CPDF_ClipStateItem * pItem )
{
	if ( pItem == nullptr )
	{
		return false;
	}

	mClipElementList.push_back( pItem );

	return TRUE;
}

CPDF_ClipState * CPDF_ClipState::Clone() const
{
	CPDF_ClipState * pRet = GetAllocator()->New<CPDF_ClipState>( GetAllocator() );
	std::list<CPDF_ClipStateItem*>::const_iterator it = mClipElementList.begin();
	for ( ; it != mClipElementList.end(); ++it )
	{
		pRet->Append( (*it)->Clone() );
	}
	return pRet;
}

bool CPDF_ExtGStateStack::PushExtStateName( const CByteString & name, const CPDF_ExtGStatePtr & extgstate )
{
	if ( ! extgstate )
	{
		return false;
	}
	std::list<CByteString>::iterator itn = mExtGStateName.begin();
	for ( ; itn != mExtGStateName.end(); ++itn )
	{
		if ( *itn == name )
		{
			mExtGStateName.erase( itn );
			break;
		}
	}
	mExtGStateName.push_back( name );

	CPDF_ObjectPtr objPtr;
	std::vector<CPDF_ExtGStateItem>::iterator iti = extgstate->mStateItems.begin();
	for ( ; iti != extgstate->mStateItems.end(); ++iti )
	{
		switch ( iti->type )
		{
		case PDF_EXTGSTATE_BM:
			{
				CByteString blendName = iti->objPtr->GetNamePtr()->GetString();
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
                    /*CPDF_DictionaryPtr dict = objPtr->GetDictPtr();
                    CPDF_ObjectPtr obj = dict->GetElement("G", OBJ_TYPE_STREAM);
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

CPDF_ExtGStateStack * CPDF_ExtGStateStack::Clone() const
{
	CPDF_ExtGStateStack * pRet = GetAllocator()->New<CPDF_ExtGStateStack>( GetAllocator() );
	pRet->mExtGStateName = mExtGStateName;
	pRet->mFillAlpha = mFillAlpha;
	pRet->mStrokeAlpha = mStrokeAlpha;
    pRet->mBlendMode = mBlendMode;
    pRet->mSMask = mSMask;
	return pRet;
}

bool CPDF_ExtGStateStack::operator == ( const CPDF_ExtGStateStack & gs ) const
{
	if ( ! IsFloatEqual( mFillAlpha, gs.mFillAlpha ) ||
		 ! IsFloatEqual( mStrokeAlpha, gs.mFillAlpha ) )
	{
		return false;
	}

	std::list<CByteString>::const_iterator it = mExtGStateName.begin();
	std::list<CByteString>::const_iterator tmpIt = gs.mExtGStateName.begin();

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

bool CPDF_ExtGStateStack::operator != ( const CPDF_ExtGStateStack & gs ) const
{
	return ! operator==( gs );
}

CPDF_GState::~CPDF_GState()
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

CPDF_StrokeState * CPDF_GState::MakeStrokeState()
{
	if ( !mpStrokeState )
	{
		mpStrokeState = GetAllocator()->New<CPDF_StrokeState>( GetAllocator() );
	}
	return mpStrokeState;
}

CPDF_TextGState * CPDF_GState::MakeTextState()
{
	if ( !mpTextState )
	{
		mpTextState = GetAllocator()->New<CPDF_TextGState>( GetAllocator() );
	}
	return mpTextState;
}

CPDF_ClipState *	CPDF_GState::MakeClipState()
{
	if ( !mpClipState )
	{
		mpClipState = GetAllocator()->New<CPDF_ClipState>( GetAllocator() );
	}
	return mpClipState;
}

CPDF_ExtGStateStack *	CPDF_GState::MakeExtGState()
{
	if ( !mpExtState )
	{
		mpExtState = GetAllocator()->New<CPDF_ExtGStateStack>( GetAllocator() );
	}
	return mpExtState;
}

CPDF_GState * CPDF_GState::Clone() const
{
	CPDF_GState * pRet = GetAllocator()->New<CPDF_GState>( GetAllocator() );
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

void	CPDF_GState::GetFillColor( CPDF_Color & colorRet ) const
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

void CPDF_GState::GetStrokeColor( CPDF_Color & colorRet ) const
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

void CPDF_GState::GetFillColorSpace( CPDF_ColorSpacePtr & colorSpaceRet ) const
{
	if ( mFlag & GSTATE_FLAG_FillColorSpace )
	{
		colorSpaceRet = mFillColorSpace;
		return;
	}
	colorSpaceRet = CPDF_ColorSpace::CreateDeviceGray();
}

void CPDF_GState::GetStrokeColorSpace( CPDF_ColorSpacePtr & colorSpaceRet ) const
{
	if ( mFlag & GSTATE_FLAG_StrokeColorSpace )
	{
		colorSpaceRet = mStrokeColorSpace;
		return;
	}
	colorSpaceRet = CPDF_ColorSpace::CreateDeviceGray();
}

void	CPDF_GState::GetLineWidth( FLOAT & lineWidthRet ) const
{
	if ( mFlag & GSTATE_FLAG_LineWidth )
	{
		lineWidthRet = mpStrokeState->GetLineWidth();
		return;
	}
	lineWidthRet = 1;
}

void CPDF_GState::GetMiterLimit( FLOAT & miterLimitRet ) const
{
	if ( mFlag & GSTATE_FLAG_MiterLimit )
	{
		miterLimitRet = mpStrokeState->GetMiterLimit();
		return;
	}
	miterLimitRet = 10;
}

void CPDF_GState::GetLineCap( GRAPHICS_STATE_LINECAP & lineCapRet ) const
{
	if ( mFlag & GSTATE_FLAG_LineCap )
	{
		lineCapRet = mpStrokeState->GetLineCap();
		return;
	}
	lineCapRet = LineCap_Butt;
}

void CPDF_GState::GetLineJoin( GRAPHICS_STATE_LINEJOIN & lineJoinRet ) const
{
	if ( mFlag & GSTATE_FLAG_LineJoin )
	{
		lineJoinRet = mpStrokeState->GetLineJoin();
		return;
	}
	lineJoinRet = LineJoin_Miter;
}

void CPDF_GState::GetLineDash( GRAPHICS_STATE_DASHPATTERN & lineDash ) const
{
	if ( mFlag & GSTATE_FLAG_LineDash )
	{
		lineDash = mpStrokeState->GetLineDash();
		return;
	}
	lineDash.dashPhase = 0;
	lineDash.dashArray.clear();
}

void CPDF_GState::GetTextMatrix( CMatrix & matrixRet ) const
{
	if ( mFlag & GSTATE_FLAG_TextMatirx )
	{
		matrixRet = mpTextState->GetMatrix();
		return;
	}
	matrixRet = CMatrix();
}

void CPDF_GState::GetTextFontSize( FLOAT & fontSizeRet ) const
{
	if ( mFlag & GSTATE_FLAG_Font )
	{
		fontSizeRet = mpTextState->GetFontSize();
		return;
	}
	fontSizeRet = 1;
}

void CPDF_GState::GetTextCharSpace( FLOAT & charSpaceRet ) const
{
	if ( mFlag & GSTATE_FLAG_CharSpace )
	{
		charSpaceRet = mpTextState->GetCharSpace();
		return;
	}
	charSpaceRet = 0;
}

void CPDF_GState::GetTextWordSpace( FLOAT & wordSpaceRet ) const
{
	if ( mFlag & GSTATE_FLAG_WordSpace )
	{
		wordSpaceRet = mpTextState->GetWordSpace();
		return;
	}
	wordSpaceRet = 0;
}

void CPDF_GState::GetTextScaling( FLOAT & scalingRet ) const
{
	if ( mFlag & GSTATE_FLAG_Scaling )
	{
		scalingRet = mpTextState->GetScaling();
		return;
	}
	scalingRet = 100;
}

void CPDF_GState::GetTextRise( FLOAT & riseRet ) const
{
	if ( mFlag & GSTATE_FLAG_Rise )
	{
		riseRet = mpTextState->GetRise();
		return;
	}
	riseRet = 0;
}

CPDF_Font * CPDF_GState::GetTextFont() const
{
	if ( mFlag & GSTATE_FLAG_Font )
	{
		return mpTextState->GetFont();
	}
	return nullptr;
}

void CPDF_GState::GetTextFontResName( CByteString & resNameRet ) const
{
	if ( mFlag & GSTATE_FLAG_Font )
	{
		resNameRet = mpTextState->GetFontResName();
		return;
	}
	resNameRet.Clear();
}

void CPDF_GState::GetTextRenderMode( GRAPHICS_STATE_TEXTRENDERMODE & rm ) const
{
	if ( mFlag & GSTATE_FLAG_RenderMode )
	{
		rm = mpTextState->GetRenderMode();
		return;
	}
	rm = TextRenderMode_Fill;
}

void CPDF_GState::SetMatrix( const CMatrix & matirx )
{
	mMatrix = matirx;
}

void CPDF_GState::SetRenderIntents( const GRAPHICS_STATE_RENDERINTENTS & ri )
{
	mFlag |= GSTATE_FLAG_RenderIntents;
	mRenderIntents = ri;
}
void CPDF_GState::SetFlatness( const FLOAT & flatness )
{
	mFlag |= GSTATE_FLAG_Flatness;
	mFlatness = flatness;
}

bool CPDF_GState::SetFillColor( const CPDF_Color & color )
{
	mFillColor = color;
	mFlag |= GSTATE_FLAG_FillColor;
	return TRUE;
}

bool CPDF_GState::SetStrokeColor( const CPDF_Color & color )
{
	mStrokeColor = color;
	mFlag |= GSTATE_FLAG_StrokeColor;
	return TRUE;
}

bool CPDF_GState::SetFillColorSpace( CPDF_ColorSpacePtr colorSpace )
{
	mFillColorSpace = colorSpace;
	mFlag |= GSTATE_FLAG_FillColorSpace;
	return TRUE;
}

bool CPDF_GState::SetStrokeColorSpace( CPDF_ColorSpacePtr colorSpace )
{
	mStrokeColorSpace = colorSpace;
	mFlag |= GSTATE_FLAG_StrokeColorSpace;
	return TRUE;
}

void	CPDF_GState::SetLineWidth( const FLOAT & lineWidth )
{
	MakeStrokeState()->SetLineWidth( lineWidth );
	mFlag |= GSTATE_FLAG_LineWidth;
}

void	CPDF_GState::SetMiterLimit( const FLOAT & miterLimit )
{
	MakeStrokeState()->SetMiterLimit( miterLimit );
	mFlag |= GSTATE_FLAG_MiterLimit;
}

void	CPDF_GState::SetLineCap( const GRAPHICS_STATE_LINECAP & lineCap )
{
	MakeStrokeState()->SetLineCap( lineCap );
	mFlag |= GSTATE_FLAG_LineCap;
}

void	CPDF_GState::SetLineJoin( const GRAPHICS_STATE_LINEJOIN & lineJion )
{
	MakeStrokeState()->SetLineJoin( lineJion );
	mFlag |= GSTATE_FLAG_LineJoin;
}

void	CPDF_GState::SetLineDash( const GRAPHICS_STATE_DASHPATTERN & dashPattern )
{
	MakeStrokeState()->SetLineDash( dashPattern );
	mFlag |= GSTATE_FLAG_LineDash;
}

void CPDF_GState::SetTextMatrix( const CMatrix & matrix )
{
	MakeTextState()->SetMatirx( matrix );
	mFlag |= GSTATE_FLAG_TextMatirx;
}

void CPDF_GState::SetTextFont( CPDF_Font * pFont )
{
	MakeTextState()->SetFont( pFont );
	mFlag |= GSTATE_FLAG_Font;
}

void CPDF_GState::SetTextFontSize( const FLOAT & size )
{
	MakeTextState()->SetFontSize( size );
	mFlag |= GSTATE_FLAG_Font;
}

void CPDF_GState::SetTextFontResName( const CByteString & resName )
{
	MakeTextState()->SetFontResName( resName );
	mFlag |= GSTATE_FLAG_Font;
}

void CPDF_GState::SetTextCharSpace( const FLOAT & charSpace )
{
	MakeTextState()->SetCharSpace( charSpace );
	mFlag |= GSTATE_FLAG_CharSpace;
}

void CPDF_GState::SetTextWordSpace( const FLOAT & wordSpace )
{
	MakeTextState()->SetWordSpace( wordSpace );
	mFlag |= GSTATE_FLAG_WordSpace;
}

// void CPDF_GState::SetTextLeading( const FLOAT & leading )
// {
// 	MakeTextState()->SetLeading( leading );
// 	mFlag |= GSTATE_FLAG_Leading;
// }

void CPDF_GState::SetTextScaling( const FLOAT & scaling )
{
	MakeTextState()->SetScaling( scaling );
	mFlag |= GSTATE_FLAG_Scaling;
}

void CPDF_GState::SetTextRise( const FLOAT & rise )
{
	MakeTextState()->SetRise( rise );
	mFlag |= GSTATE_FLAG_Rise;
}

void CPDF_GState::SetTextRenderMode( const GRAPHICS_STATE_TEXTRENDERMODE & rm )
{
	MakeTextState()->SetRenderMode( rm );
	mFlag |= GSTATE_FLAG_RenderMode;
}

bool CPDF_GState::PushClipElement( CPDF_ContentObject * pElement )
{
	if ( !pElement )
	{
		return false;
	}
	CPDF_ClipStateItem * pItem = GetAllocator()->New<CPDF_ClipStateItem>( GetAllocator() );
	pItem->SetElement( pElement );
	MakeClipState()->Append( pItem );
	return TRUE;
}

bool CPDF_GState::PushExtGState( const CByteString & resName, const CPDF_ExtGStatePtr & extgstate )
{
	MakeExtGState()->PushExtStateName( resName, extgstate );

	CPDF_ObjectPtr objPtr;
	std::vector<CPDF_ExtGStateItem>::iterator iti = extgstate->mStateItems.begin();
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

void	CPDF_GState::CopyTextState( CPDF_GState * pGState )
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

bool IsDefMatrix( const CMatrix & textMatrix )
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

bool IsDefColorSpace( const CPDF_ColorSpacePtr & colorSpace )
{
	if ( colorSpace->GetColorSpaceType() == COLORSPACE_DEVICE_GRAY )
	{
		return TRUE;
	}
	return false;
}

bool IsDefColor( const CPDF_Color & color )
{
	if ( color.GetComponentCount() == 1 && IsFloatEqual( color.GetComponent(0), 0 ) )
	{
		return TRUE;
	}
	return false;
}

bool	IsColorSpaceEqual( const CPDF_ColorSpacePtr & cs1, const CPDF_ColorSpacePtr & cs2 )
{
	//zctodo
	if ( cs1->GetColorSpaceType() == cs2->GetColorSpaceType() /*&& cs1.GetResName() == cs2.GetResName()*/ )
	{
		return TRUE;
	}
	return false;
}

bool IsColorEqual( const CPDF_Color & c1, const CPDF_Color & c2 )
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

bool IsExtGStateEqual( const CPDF_ExtGStateStack * pExtGS1, const CPDF_ExtGStateStack * pExtGS2 )
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

	std::list<CByteString>::const_iterator it1, it2;
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

bool IsExtGStateContinue( const CPDF_ExtGStateStack * pExtGS1, const CPDF_ExtGStateStack * pExtGS2 )
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

	std::list<CByteString>::const_iterator it1, it2;
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

bool IsClipStateEqual( const CPDF_ClipState * pClipGS1, const CPDF_ClipState * pClipGS2 )
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

	std::list<CPDF_ClipStateItem*>::const_iterator it1, it2;
	CPDF_ContentObject * pElement1 = nullptr, * pElement2 = nullptr;
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
			//CPDF_Text * pText1 = (CPDF_Text*)( pElement1 );
			//CPDF_Text * pText2 = (CPDF_Text*)( pElement2 );
			//zctodo
		}else if ( pElement1->GetType() == ContentType_Path && pElement2->GetType() == ContentType_Path )
		{
			CPDF_Path * pPath1 = (CPDF_Path*)( pElement1 );
			CPDF_Path * pPath2 = (CPDF_Path*)( pElement2 );
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

bool IsClipStateContinue( const CPDF_ClipState * pClipGS1, const CPDF_ClipState * pClipGS2 )
{
	return false;
}

}//namespace