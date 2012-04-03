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
	pRet->SetMatrix( mMatrix );
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

HE_BOOL CHE_PDF_ExtGState::PushExtStateName( const CHE_ByteString & name, CHE_PDF_Dictionary * pDict )
{
	if ( !pDict )
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
	mExtDictNameList.push_front( name );
	CHE_PDF_ObjectPtr pTmpObj = pDict->GetElement( "CA", OBJ_TYPE_NUMBER );
	if ( pTmpObj )
	{
		mStrokeAlpha = pTmpObj->GetNumber()->GetFloat();
	}
	pTmpObj = pDict->GetElement( "ca", OBJ_TYPE_NUMBER );
	if ( pTmpObj )
	{
		mFillAlpha = pTmpObj->GetNumber()->GetFloat();
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

HE_BOOL	CHE_PDF_GState::GetFillColor( CHE_PDF_Color & colorRet ) const
{
	if ( mFlag & GSTATE_FLAG_FillColor )
	{
		colorRet = *mpFillColor;
		return TRUE;
	}
	return FALSE;
}

HE_BOOL CHE_PDF_GState::GetStrokeColor( CHE_PDF_Color & colorRet ) const
{
	if ( mFlag & GSTATE_FLAG_StrokeColor )
	{
		colorRet = *mpStrokeColor;
		return TRUE;
	}
	return FALSE;
}
HE_BOOL CHE_PDF_GState::GetFillColorSpace( CHE_PDF_ColorSpace & colorSpaceRet ) const
{
	if ( mFlag & GSTATE_FLAG_FillColorSpace )
	{
		colorSpaceRet = *mpFillColorSpace;
		return TRUE;
	}
	return FALSE;
}

HE_BOOL CHE_PDF_GState::GetStrokeColorSpace( CHE_PDF_ColorSpace & colorSpaceRet ) const
{
	if ( mFlag & GSTATE_FLAG_StrokeColorSpace )
	{
		colorSpaceRet = *mpFillColorSpace;
		return TRUE;
	}
	return FALSE;
}

HE_BOOL	CHE_PDF_GState::GetLineWidth( HE_FLOAT & lineWidthRet ) const
{
	if ( mFlag & GSTATE_FLAG_LineWidth )
	{
		lineWidthRet = mpStrokeState->GetLineWidth();
		return TRUE;
	}
	return FALSE;
}

HE_BOOL CHE_PDF_GState::GetMiterLimit( HE_FLOAT & miterLimitRet ) const
{
	if ( mFlag & GSTATE_FLAG_MiterLimit )
	{
		miterLimitRet = mpStrokeState->GetMiterLimit();
		return TRUE;
	}
	return FALSE;
}

HE_BOOL CHE_PDF_GState::GetLineCap( PDF_GSTATE_LINECAP & lineCapRet ) const
{
	if ( mFlag & GSTATE_FLAG_LineCap )
	{
		lineCapRet = mpStrokeState->GetLineCap();
		return TRUE;
	}
	return FALSE;
}

HE_BOOL CHE_PDF_GState::GetLineJoin( PDF_GSTATE_LINEJOIN & lineJoinRet ) const
{
	if ( mFlag & GSTATE_FLAG_LineJoin )
	{
		lineJoinRet = mpStrokeState->GetLineJoin();
		return TRUE;
	}
	return FALSE;
}

HE_BOOL CHE_PDF_GState::GetLineDash( PDF_GSTATE_DASHPATTERN & lineDash ) const
{
	if ( mFlag & GSTATE_FLAG_LineDash )
	{
		lineDash = mpStrokeState->GetLineDash();
		return TRUE;
	}
	return FALSE;
}

HE_BOOL CHE_PDF_GState::GetTextMatrix( CHE_PDF_Matrix & matrixRet ) const
{
	if ( mFlag & GSTATE_FLAG_TextMatirx )
	{
		matrixRet = mpTextState->GetMatrix();
		return TRUE;
	}
	return FALSE;
}

HE_BOOL CHE_PDF_GState::GetTextFontSize( HE_FLOAT & fontSizeRet ) const
{
	if ( mFlag & GSTATE_FLAG_Font )
	{
		fontSizeRet = mpTextState->GetFontSize();
		return TRUE;
	}
	return FALSE;
}

HE_BOOL CHE_PDF_GState::GetTextCharSpace( HE_FLOAT & charSpaceRet ) const
{
	if ( mFlag & GSTATE_FLAG_CharSpace )
	{
		charSpaceRet = mpTextState->GetCharSpace();
		return TRUE;
	}
	return FALSE;
}

HE_BOOL CHE_PDF_GState::GetTextWordSpace( HE_FLOAT & wordSpaceRet ) const
{
	if ( mFlag & GSTATE_FLAG_WordSpace )
	{
		wordSpaceRet = mpTextState->GetWordSpace();
		return TRUE;
	}
	return FALSE;
}

HE_BOOL CHE_PDF_GState::GetTextLeading( HE_FLOAT & leadingRet ) const
{
	if ( mFlag & GSTATE_FLAG_Leading )
	{
		leadingRet = mpTextState->GetLeading();
		return TRUE;
	}
	return FALSE;
}

HE_BOOL CHE_PDF_GState::GetTextScaling( HE_FLOAT & scalingRet ) const
{
	if ( mFlag & GSTATE_FLAG_Scaling )
	{
		scalingRet = mpTextState->GetScaling();
		return TRUE;
	}
	return FALSE;
}

HE_BOOL CHE_PDF_GState::GetTextRise( HE_FLOAT & riseRet ) const
{
	if ( mFlag & GSTATE_FLAG_Rise )
	{
		riseRet = mpTextState->GetRise();
		return TRUE;
	}
	return FALSE;
}

CHE_PDF_Font * CHE_PDF_GState::GetTextFont() const
{
	if ( mFlag & GSTATE_FLAG_Font  )
	{
		return mpTextState->GetFont();
	}
	return NULL;
}

HE_BOOL CHE_PDF_GState::GetTextFontResName( CHE_ByteString & resNameRet ) const
{
	if ( mFlag & GSTATE_FLAG_Font )
	{
		resNameRet = mpTextState->GetFontResName();
		return TRUE;
	}
	return FALSE;
}

HE_BOOL CHE_PDF_GState::GetTextRenderMode( PDF_GSTATE_TEXTRENDERMODE & rm ) const
{
	if ( mFlag & GSTATE_FLAG_RenderMode )
	{
		rm = mpTextState->GetRenderMode();
		return TRUE;
	}
	return FALSE;
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

HE_VOID	CHE_PDF_GState::SetLineCap( const PDF_GSTATE_LINECAP & lineCap )
{
	MakeStrokeState()->SetLineCap( lineCap );
	mFlag |= GSTATE_FLAG_LineCap;
}

HE_VOID	CHE_PDF_GState::SetLineJoin( const PDF_GSTATE_LINEJOIN & lineJion )
{
	MakeStrokeState()->SetLineJoin( lineJion );
	mFlag |= GSTATE_FLAG_LineJoin;
}

HE_VOID	CHE_PDF_GState::SetLineDash( const PDF_GSTATE_DASHPATTERN & dashPattern )
{
	MakeStrokeState()->SetLineDash( dashPattern );
	mFlag |= GSTATE_FLAG_LineDash;
}

HE_VOID CHE_PDF_GState::SetTextMatrix( const CHE_PDF_Matrix & matrix )
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

HE_VOID CHE_PDF_GState::SetTextLeading( const HE_FLOAT & leading )
{
	MakeTextState()->SetLeading( leading );
	mFlag |= GSTATE_FLAG_Leading;
}

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

HE_VOID CHE_PDF_GState::SetTextRenderMode( const PDF_GSTATE_TEXTRENDERMODE & rm )
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
	pItem->SetMatrix( mMatrix );
	MakeClipState()->Append( pItem );
	return TRUE;
}

HE_BOOL CHE_PDF_GState::PushExtGState( const CHE_ByteString & resName, CHE_PDF_Dictionary * pDict )
{
	return FALSE;
}