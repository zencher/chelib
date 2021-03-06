#include "../../include/wdm/che_wdm.h"
#include <cstring>

CHE_WDM_AppearImagePtr CHE_WDM_AppearImage::Create( CHE_Allocator * pAllocator )
{
	if ( pAllocator == NULL )
	{
		pAllocator = GetDefaultAllocator();
	}
	CHE_WDM_AppearImagePtr ptr;
	CHE_WDM_AppearImage * pImage =  pAllocator->New<CHE_WDM_AppearImage>( pAllocator );
	ptr.reset( pImage );
	return ptr;
}

CHE_WDM_AppearImage::~CHE_WDM_AppearImage()
{
	if ( mFile )
	{
		GetAllocator()->DeleteArray( mFile );
		mFile = NULL;
	}
}

void CHE_WDM_AppearImage::SetImageFile( PWSTR pFile )
{
	if ( mFile )
	{
		GetAllocator()->DeleteArray( mFile );
		mFile = NULL;
	}
	if ( pFile )
	{
		mFile = GetAllocator()->NewArray<WCHAR>( wcslen(pFile)+1 );
		wcscpy( mFile, pFile );
	}
}

CHE_WDM_AppearTextPtr CHE_WDM_AppearText::Create( CHE_Allocator * pAllocator )
{
	if ( pAllocator == NULL )
	{
		pAllocator = GetDefaultAllocator();
	}
	CHE_WDM_AppearTextPtr ptr;
	CHE_WDM_AppearText * pImage = pAllocator->New<CHE_WDM_AppearText>( pAllocator );
	ptr.reset( pImage );
	return ptr;
}

CHE_WDM_AppearText::~CHE_WDM_AppearText()
{
	if ( mText )
	{
		GetAllocator()->DeleteArray( mText );
		mText = NULL;
	}
	if ( mFontFile )
	{
		GetAllocator()->DeleteArray( mFontFile );
		mFontFile = NULL;
	}
}

void CHE_WDM_AppearText::SetText( PCWSTR text )
{
	if ( mText )
	{
		GetAllocator()->DeleteArray( mText );
		mText = NULL;
	}
	if ( text )
	{
		mText = GetAllocator()->NewArray<WCHAR>( wcslen( text ) + 1 );
		wcscpy( mText, text );
	}
}

void CHE_WDM_AppearText::SetFontFile( PCWSTR file )
{
	if ( mFontFile )
	{
		GetAllocator()->DeleteArray( mFontFile );
		mFontFile = NULL;
	}
	if ( file )
	{
		mFontFile = GetAllocator()->NewArray<WCHAR>( wcslen(file) + 1 );
		wcscpy( mFontFile, file );
	}
}

CHE_WDM_AppearPathPtr CHE_WDM_AppearPath::Create( CHE_Allocator * pAllocator )
{
	if ( pAllocator == NULL )
	{
		pAllocator = GetDefaultAllocator();
	}
	CHE_WDM_AppearPathPtr ptr;
	CHE_WDM_AppearPath * pPath =  pAllocator->New<CHE_WDM_AppearPath>( pAllocator );
	ptr.reset( pPath );
	return ptr;
}

void CHE_WDM_AppearPath::AddLine( FLOAT xStart, FLOAT yStart, FLOAT xEnd, FLOAT yEnd )
{
	CHE_WDM_AppearPathItem item;
	item.SetType( APPEAR_PATH_ITEM_LINE );
	mItems.push_back( item );
	item.SetValue( xStart );
	mItems.push_back( item );
	item.SetValue( yStart );
	mItems.push_back( item );
	item.SetValue( xEnd );
	mItems.push_back( item );
	item.SetValue( yEnd );
	mItems.push_back( item );
}

void CHE_WDM_AppearPath::AddRect( FLOAT left, FLOAT top, FLOAT width, FLOAT hight )
{
	CHE_WDM_AppearPathItem item;
	item.SetType( APPEAR_PATH_ITEM_RECT );
	mItems.push_back( item );
	item.SetValue( left );
	mItems.push_back( item );
	item.SetValue( top );
	mItems.push_back( item );
	item.SetValue( width );
	mItems.push_back( item );
	item.SetValue( hight );
	mItems.push_back( item );
}

void CHE_WDM_AppearPath::AddCurve( FLOAT x0, FLOAT y0, FLOAT x1, FLOAT y1, FLOAT x2, FLOAT y2 )
{
	CHE_WDM_AppearPathItem item;
	item.SetType( APPEAR_PATH_ITEM_CURVE );
	mItems.push_back( item );
	item.SetValue( x0 );
	mItems.push_back( item );
	item.SetValue( y0 );
	mItems.push_back( item );
	item.SetValue( x1 );
	mItems.push_back( item );
	item.SetValue( y1 );
	mItems.push_back( item );
	item.SetValue( x2 );
	mItems.push_back( item );
	item.SetValue( y2 );
	mItems.push_back( item );
}

size_t CHE_WDM_AppearPath::GetItemCount()
{
	return mItems.size();
}

bool CHE_WDM_AppearPath::GetItem( size_t index, CHE_WDM_AppearPathItem & itemRet )
{
	if ( index >= mItems.size() )
	{
		return FALSE;
	}
	itemRet = mItems[index];
	return TRUE;
}

CHE_WDM_Area* CHE_WDM_Area::Create( IHE_WDM_InterActive * pInterActive, CHE_Allocator * pAllocator )
{
	if ( pAllocator == NULL )
	{
		pAllocator =  GetDefaultAllocator();
	}
	CHE_WDM_Area * pArea = pAllocator->New<CHE_WDM_Area>( pInterActive, pAllocator );
	return pArea;
}

CHE_WDM_Area::CHE_WDM_Area( IHE_WDM_InterActive * pInteractive, CHE_Allocator * pAllocator )
	: CHE_Object( pAllocator ), mbLBD( FALSE ), mbRBD( FALSE ), mbMO( FALSE ), mbClip( FALSE ),
	mbVisable( TRUE ), mbEnable( TRUE ), mWidth( 0 ), mHeight( 0 ), mPosX( 0 ), mPosY( 0 ),
	mParent( NULL ), mInterActive( pInteractive ), mpCaptureChild( NULL ), mpMouseOverArea( NULL ),
	mName( pAllocator ) {}

CHE_WDM_Area::~CHE_WDM_Area()
{
	CHE_WDM_Area * pArea = NULL;
	for ( size_t i = 0; i < mChildren.size(); ++i )
	{
		pArea = mChildren[i];
		if ( pArea )
		{
			pArea->GetAllocator()->Delete( pArea );
			pArea = NULL;
		}
	}
}

void CHE_WDM_Area::SetWidth( int32 width )
{
	mWidth = width;

	CHE_WDM_Layout layout;
	CHE_WDM_Area * pTmp = NULL;
	for ( size_t i = 0; i < mChildren.size(); ++i )
	{
		pTmp = mChildren[i];
		if ( pTmp )
		{
			layout = pTmp->GetLayout();
			switch( layout.GetVertLayout() )
			{
			case LAYOUT_FILL:
				pTmp->SetWidth( width );
				break;
			case LAYOUT_ALIGN_LEFT_OR_TOP:
				pTmp->SetPosiX( mPosX );
				break;
			case LAYOUT_ALIGN_CENTER:
				pTmp->SetPosiX( mPosX + (mWidth/2) - pTmp->GetWidth()/2 );
				break;
			case LAYOUT_ALIGN_RIGHT_OR_BOTTOM:
				pTmp->SetPosiX( mPosX + mWidth - pTmp->GetWidth() );
				break;
			case LAYOUT_FIX:
			default:
				break;
			}
		}
	}
}

void CHE_WDM_Area::SetHeight( int32 height )
{
	mHeight = height;

	CHE_WDM_Layout layout;
	CHE_WDM_Area * pTmp = NULL;

	for ( size_t i = 0; i < mChildren.size(); ++i )
	{
		pTmp = mChildren[i];
		if ( pTmp )
		{
			layout = pTmp->GetLayout();

			switch( layout.GetHoriLayout() )
			{
			case LAYOUT_FILL:
				pTmp->SetHeight( height );
				break;
			case LAYOUT_ALIGN_LEFT_OR_TOP:
				pTmp->SetPosiY( mPosY );
				break;
			case LAYOUT_ALIGN_CENTER:
				pTmp->SetPosiY( mPosY + (mHeight/2) - (pTmp->GetHeight()/2) );
				break;
			case LAYOUT_ALIGN_RIGHT_OR_BOTTOM:
				pTmp->SetPosiY( mPosY + mHeight - pTmp->GetHeight() );
				break;
			case LAYOUT_FIX:
			default:
				break;
			}
		}
	}
}

void CHE_WDM_Area::SetPosiX( int32 x )
{
	int32 offset = x - mPosX;
	mPosX = x;

	CHE_WDM_Area * pTmp = NULL;
	for ( size_t i = 0; i < mChildren.size(); ++i )
	{
		pTmp = mChildren[i];
		if ( pTmp )
		{
			pTmp->SetPosiX( pTmp->GetPosiX() + offset );
		}
	}
}

void CHE_WDM_Area::SetPosiY( int32 y )
{
	int32 offset = y - mPosY;
	mPosY = y;
	CHE_WDM_Area * pTmp = NULL;
	for ( size_t i = 0; i < mChildren.size(); ++i )
	{
		pTmp = mChildren[i];
		if ( pTmp )
		{
			pTmp->SetPosiY( pTmp->GetPosiY() + offset );
		}
	}
}

void CHE_WDM_Area::SetEnable( bool bEnable = TRUE )
{
	mbEnable = bEnable;
	if ( ! mbEnable )
	{
		ReleaseFocus();
		ReleaseCapture();

		mbLBD = FALSE;
		mbRBD = FALSE;
		mbMO = FALSE;
	}
}

void CHE_WDM_Area::SetFocus( CHE_WDM_Area * pChild )
{
	if ( pChild )
	{
		mpMouseOverArea = pChild;
	}
	if ( GetParent() )
	{
		GetParent()->SetFocus( pChild );
	}
}

void CHE_WDM_Area::ReleaseFocus()
{
	if ( mpMouseOverArea )
	{
		mpMouseOverArea = NULL;
	}
	if ( GetParent() )
	{
		GetParent()->ReleaseFocus();
	}
}

void CHE_WDM_Area::SetCapture( CHE_WDM_Area * pChild )
{
	if ( pChild )
	{
		mpCaptureChild = pChild;
	}
	if ( GetParent() )
	{
		GetParent()->SetCapture( this );
	}
}

void CHE_WDM_Area::ReleaseCapture()
{
	mpCaptureChild = NULL;
	if ( GetParent() )
	{
		GetParent()->ReleaseCapture();
	}
}

void CHE_WDM_Area::Refresh()
{
	if ( mInterActive )
	{
		mInterActive->InvalidateRect( GetPosiX(), GetPosiY(), GetPosiX() + GetWidth(), GetPosiY() + GetHeight() );
	}
}

void CHE_WDM_Area::AppendAppearItem( const CHE_WDM_AppearItemPtr & ptr, WDM_AREA_APPEAR_TYPE type )
{
	switch ( type )
	{
	case AREA_APPEAR_NORMAL:
		mAppearance.mNormal.push_back( ptr );
		break;
	case AREA_APPEAR_DISABLE:
		mAppearance.mDisable.push_back( ptr );
		break;
	case AREA_APPEAR_BACKGROUND:
		mAppearance.mBackground.push_back( ptr );
		break;
	case AREA_APPEAR_MOUSEOVER:
		mAppearance.mMouseOver.push_back( ptr );
		break;
	case AREA_APPEAR_MOUSELBDOWN:
		mAppearance.mMouseLButtonDown.push_back( ptr );
		break;
	case AREA_APPEAR_MOUSERBDOWN:
		mAppearance.mMouseRButtonDown.push_back( ptr );
		break;
	default:
		break;
	}
}

void CHE_WDM_Area::AppendChild( CHE_WDM_Area * pChlid )
{
	if( pChlid )
	{
		mChildren.push_back( pChlid );
		pChlid->SetParent( this );
	}
}

CHE_WDM_Area * CHE_WDM_Area::PopChild( size_t index )
{
	std::vector<CHE_WDM_Area*>::iterator it = mChildren.begin();
	
	CHE_WDM_Area * pTmp = NULL;

	for ( size_t i = 0; it != mChildren.end(); ++it, ++i )
	{
		if ( index == i )
		{
			pTmp = *it;
			mChildren.erase( it );
			pTmp->SetParent( NULL );
			return pTmp;
		}
	}
	return NULL;
}

bool CHE_WDM_Area::ChildToLower( size_t index )
{
	if ( index == 0 )
	{
		return FALSE;
	}

	CHE_WDM_Area * pTmp = mChildren[index];
	mChildren[index] = mChildren[index-1];
	mChildren[index-1] = pTmp;
	return TRUE;
}

bool CHE_WDM_Area::ChildToUpper( size_t index )
{
	if ( index >= mChildren.size() - 1 )
	{
		return FALSE;
	}

	CHE_WDM_Area * pTmp = mChildren[index];
	mChildren[index] = mChildren[index+1];
	mChildren[index+1] = pTmp;
	return TRUE;
}

size_t CHE_WDM_Area::GetChildrenCount() const
{
	return mChildren.size();
}

CHE_WDM_Area * CHE_WDM_Area::GetChild( size_t index ) const
{
	return ( index < GetChildrenCount() ) ? mChildren[index] : NULL ;
}

void CHE_WDM_Area::ClearChild()
{
	mChildren.clear();
}

bool CHE_WDM_Area::OnMouseMove( int32 x, int32 y )
{
	if ( !mbEnable )
	{
		return FALSE;
	}

	if ( mpCaptureChild )
	{
		return mpCaptureChild->OnMouseMove( x, y );
	}

	bool bHandled = FALSE;
	bool bChildrenGet = FALSE;

	CHE_WDM_Area * pTmp = NULL;
	
	for ( size_t i = mChildren.size(); i > 0; --i )
	{
		pTmp = mChildren[i-1];
		if ( pTmp && pTmp->IsEnable() )
		{
			if (	x >= pTmp->GetPosiX() && x < pTmp->GetPosiX() + pTmp->GetWidth() &&
					y >= pTmp->GetPosiY() && y < pTmp->GetPosiY() + pTmp->GetHeight() )
			{
				if ( mpMouseOverArea == NULL )
				{
					pTmp->OnMouseOver();
				}else if ( mpMouseOverArea != pTmp )
				{
					mpMouseOverArea->OnMouseOut();
					pTmp->OnMouseOver();
				}
				bHandled = pTmp->OnMouseMove( x, y );
				bChildrenGet = TRUE;
				break;
			}
		}
	}

	mbMO = TRUE;
	if ( !bChildrenGet )
	{	
		if ( mpMouseOverArea != NULL )
		{
			mpMouseOverArea->OnMouseOut();
			OnMouseOver();
		}
	}

// 	if ( !bChildrenGet )
// 	{
// 		mbMO = TRUE;
// 		if ( mpMouseOverArea != NULL )
// 		{
// 			mpMouseOverArea->OnMouseOut();
// 			OnMouseOver();
// 		}
// 	}else{
// 		mbMO = FALSE;
// 	}

	return bHandled;
}

bool CHE_WDM_Area::OnMouseOver()
{
	if ( !mbEnable )
	{
		return FALSE;
	}

	if ( GetParent() )
	{
		GetParent()->SetFocus( this );
	}
	mbMO = TRUE;
	if ( mInterActive )
	{
		mInterActive->InvalidateRect( GetPosiX(), GetPosiY(), GetPosiX() + GetWidth(), GetPosiY() + GetHeight() );
	}

	return FALSE;
}

bool CHE_WDM_Area::OnMouseOut()
{
	if ( GetParent() )
	{
		GetParent()->ReleaseFocus();
	}
	mbLBD = FALSE;
	mbRBD = FALSE;
	mbMO = FALSE;
	mpMouseOverArea = NULL;
	if ( mInterActive )
	{
		mInterActive->InvalidateRect( GetPosiX(), GetPosiY(), GetPosiX() + GetWidth(), GetPosiY() + GetHeight() );
	}
	return FALSE;
}

bool CHE_WDM_Area::OnMouseLBDown( int32 x, int32 y )
{
	if ( !mbEnable )
	{
		return FALSE;
	}
	if ( mpCaptureChild )
	{
		return mpCaptureChild->OnMouseLBDown( x, y );
	}
	bool bHandled = FALSE;
	CHE_WDM_Area * pTmp = NULL;
	for ( size_t i = mChildren.size(); i > 0; --i )
	{
		pTmp = mChildren[i-1];
		if ( pTmp && pTmp->IsEnable() )
		{
			if ( x >= pTmp->GetPosiX() && x < pTmp->GetPosiX() + pTmp->GetWidth() &&
				 y >= pTmp->GetPosiY() && y < pTmp->GetPosiY() + pTmp->GetHeight() )
			{
				bHandled = pTmp->OnMouseLBDown( x, y );
				break;
			}
		}
	}
	if ( ! bHandled )
	{
		mbLBD = TRUE;
		if ( mInterActive )
		{
			mInterActive->InvalidateRect( GetPosiX(), GetPosiY(), GetPosiX() + GetWidth(), GetPosiY() + GetHeight() );
		}
	}
	return bHandled;
}

bool CHE_WDM_Area::OnMouseLBUp( int32 x, int32 y )
{
	if ( !mbEnable )
	{
		return FALSE;
	}
	
	if ( mpCaptureChild )
	{
		return mpCaptureChild->OnMouseLBUp( x, y );
	}

	bool bHandled = FALSE;
	CHE_WDM_Area * pTmp = NULL;
	for ( size_t i = mChildren.size(); i > 0; --i )
	{
		pTmp = mChildren[i-1];
		if ( pTmp && pTmp->IsEnable() )
		{
			if (	x >= pTmp->GetPosiX() && x < pTmp->GetPosiX() + pTmp->GetWidth() &&
					y >= pTmp->GetPosiY() && y < pTmp->GetPosiY() + pTmp->GetHeight() )
			{
				bHandled = pTmp->OnMouseLBUp( x, y );
				break;
			}
		}
	}
	if ( ! bHandled )
	{
		mbLBD = FALSE;
// 		if ( mInterActive )
// 		{
// 			mInterActive->InvalidateRect( GetPosiX(), GetPosiY(), GetPosiX() + GetWidth(), GetPosiY() + GetHeight() );
// 		}
	}
	return bHandled;
}

bool CHE_WDM_Area::OnMouseRBDown( int32 x, int32 y )
{
	if ( !mbEnable )
	{
		return FALSE;
	}
	if ( mpCaptureChild )
	{
		return mpCaptureChild->OnMouseRBDown( x, y );
	}
	bool bHandled = FALSE;
	CHE_WDM_Area * pTmp = NULL;
	for ( size_t i = mChildren.size(); i > 0; --i )
	{
		pTmp = mChildren[i-1];
		if ( pTmp && pTmp->IsEnable() )
		{
			if (	x >= pTmp->GetPosiX() && x < pTmp->GetPosiX() + pTmp->GetWidth() &&
					y >= pTmp->GetPosiY() && y < pTmp->GetPosiY() + pTmp->GetHeight() )
			{
				bHandled = pTmp->OnMouseRBDown( x, y );
				break;
			}
		}
	}
	if ( !bHandled )
	{
		mbRBD = TRUE;
		if ( mInterActive )
		{
			mInterActive->InvalidateRect( GetPosiX(), GetPosiY(), GetPosiX() + GetWidth(), GetPosiY() + GetHeight() );
		}
	}
	return bHandled;
}

bool CHE_WDM_Area::OnMouseRBUp( int32 x, int32 y )
{
	if ( !mbEnable )
	{
		return FALSE;
	}
	if ( mpCaptureChild )
	{
		return mpCaptureChild->OnMouseRBUp( x, y );
	}
	bool bHandled = FALSE;
	CHE_WDM_Area * pTmp = NULL;
	for ( size_t i = mChildren.size(); i > 0; --i )
	{
		pTmp = mChildren[i-1];
		if ( pTmp && pTmp->IsEnable() )
		{
			if (	x >= pTmp->GetPosiX() && x < pTmp->GetPosiX() + pTmp->GetWidth() &&
					y >= pTmp->GetPosiY() && y < pTmp->GetPosiY() + pTmp->GetHeight() )
			{
				bHandled = pTmp->OnMouseRBUp( x, y );
				break;
			}
		}
	}
	if ( bHandled )
	{
		mbRBD = FALSE;
		if ( mInterActive )
		{
			mInterActive->InvalidateRect( GetPosiX(), GetPosiY(), GetPosiX() + GetWidth(), GetPosiY() + GetHeight() );
		}
	}
	return bHandled;
}

bool CHE_WDM_Area::OnMouseLDBClick( int32 x, int32 y )
{
	if ( !mbEnable )
	{
		return FALSE;
	}
	bool bHandled = FALSE;
	CHE_WDM_Area * pTmp = NULL;
	for ( size_t i = mChildren.size(); i > 0; --i )
	{
		pTmp = mChildren[i-1];
		if ( pTmp && pTmp->IsEnable() )
		{
			if (	x >= pTmp->GetPosiX() && x < pTmp->GetPosiX() + pTmp->GetWidth() &&
					y >= pTmp->GetPosiY() && y < pTmp->GetPosiY() + pTmp->GetHeight() )
			{
				bHandled = pTmp->OnMouseLDBClick( x, y );
				break;
			}
		}
	}
	return bHandled;
}

bool CHE_WDM_Area::OnMouseRDBClick( int32 x, int32 y )
{
	if ( !mbEnable )
	{
		return FALSE;
	}
	bool bHandled = FALSE;
	CHE_WDM_Area * pTmp = NULL;
	for ( size_t i = mChildren.size(); i > 0; --i )
	{
		pTmp = mChildren[i-1];
		if ( pTmp && pTmp->IsEnable() )
		{
			if (	x >= pTmp->GetPosiX() && x < pTmp->GetPosiX() + pTmp->GetWidth() &&
					y >= pTmp->GetPosiY() && y < pTmp->GetPosiY() + pTmp->GetHeight() )
			{
				bHandled = pTmp->OnMouseRDBClick( x, y );
				break;
			}
		}
	}
	return bHandled;
}

bool CHE_WDM_Area::OnDraw()
{
	if ( ! mbVisable )
	{
		return FALSE;
	}
	if ( mInterActive )
	{
		if ( mbClip )
		{
			mInterActive->SetClip( this );
		}
		if ( mAppearance.mBackground.size() > 0 )
		{
			mInterActive->Draw( this, AREA_APPEAR_BACKGROUND );
		}
		if ( ! mbEnable && mAppearance.mDisable.size() > 0 )
		{
			mInterActive->Draw( this, AREA_APPEAR_DISABLE );
		}else if ( mAppearance.mMouseLButtonDown.size() > 0 && mbLBD )
		{
			mInterActive->Draw( this, AREA_APPEAR_MOUSELBDOWN );
		}else if ( mAppearance.mMouseRButtonDown.size() > 0 && mbRBD )
		{
			mInterActive->Draw( this, AREA_APPEAR_MOUSERBDOWN );
		}else if ( mAppearance.mMouseOver.size() > 0 && mbMO )
		{
			mInterActive->Draw( this, AREA_APPEAR_MOUSEOVER );
		}else if ( mAppearance.mNormal.size() > 0 )
		{
			mInterActive->Draw( this, AREA_APPEAR_NORMAL );
		}
		CHE_WDM_Area * pTmp = NULL;
		for ( size_t i = 0; i < mChildren.size(); ++i )
		{
			pTmp = mChildren[i];
			if ( pTmp )
			{
				pTmp->OnDraw();
			}
		}
		if ( mbClip )
		{
			mInterActive->ResetClip();
		}
	}
	return TRUE;
}

bool CHE_WDM_Area::OnDraw( int32 left, int32 top, int32 right, int32 bottom )
{
	if ( !mbVisable )
	{
		return FALSE;
	}
	if ( mInterActive )
	{
		mInterActive->SetClip( left, top, right, bottom );
		if ( mbClip )
		{
			mInterActive->SetClip( this );
		}
		if (	( ( GetPosiX() < left ) && ( ( GetPosiX() + GetWidth() ) <left ) )
			||
			( ( GetPosiX() > right ) && ( ( GetPosiX() + GetWidth() ) > right ) )
			||
			( ( GetPosiY() < top) && ( ( GetPosiY() + GetHeight() )<top ) )
			||
			( ( GetPosiY() > bottom ) && ( ( GetPosiY() + GetHeight() ) > bottom ) )
			)
		{
		}else{
			if ( mAppearance.mBackground.size() > 0 )
			{
				mInterActive->Draw( this, AREA_APPEAR_BACKGROUND );
			}
			if ( ! mbEnable && mAppearance.mDisable.size() > 0 )
			{
				mInterActive->Draw( this, AREA_APPEAR_DISABLE );
			}else if ( mAppearance.mMouseLButtonDown.size() > 0 && mbLBD )
			{
				mInterActive->Draw( this, AREA_APPEAR_MOUSELBDOWN );
			}else if ( mAppearance.mMouseRButtonDown.size() > 0 && mbRBD )
			{
				mInterActive->Draw( this, AREA_APPEAR_MOUSERBDOWN );
			}else if ( mAppearance.mMouseOver.size() > 0 && mbMO )
			{
				mInterActive->Draw( this, AREA_APPEAR_MOUSEOVER );
			}else if ( mAppearance.mNormal.size() > 0 )
			{
				mInterActive->Draw( this, AREA_APPEAR_NORMAL );
			}
		}
		CHE_WDM_Area * pTmp = NULL;
		for ( size_t i = 0; i < mChildren.size(); ++i )
		{
			pTmp = mChildren[i];
			if ( pTmp )
			{
				pTmp->OnDraw( left, top, right, bottom );
			}
		}
		if ( mbClip )
		{
			mInterActive->ResetClip();
		}
		mInterActive->ResetClip();
	}
	return TRUE;
}

CHE_WDM_Button * CHE_WDM_Button::Create( IHE_WDM_InterActive * pInterActive, CHE_Allocator * pAllocator )
{
	if ( pAllocator == NULL )
	{
		pAllocator = GetDefaultAllocator();
	}
	CHE_WDM_Button * pButtun = pAllocator->New<CHE_WDM_Button>( pInterActive, pAllocator );
	return pButtun;
}

bool CHE_WDM_Button::OnMouseLBDown( int32 x, int32 y )
{
	if ( IsEnable() )
	{
		if ( CHE_WDM_Area::OnMouseLBDown( x, y ) )
		{
			return TRUE;
		}
		if ( mLBDEventFunc )
		{
			mLBDEventFunc( this );
		}
		return TRUE;
	}
	return FALSE;
}

bool CHE_WDM_Button::OnMouseLBUp( int32 x, int32 y )
{
	if ( IsEnable() )
	{
		/*bool bLBD = IsMouseLBDown();*/
		if ( CHE_WDM_Area::OnMouseLBUp( x, y ) )
		{
			return TRUE;
		}
		if ( /*bLBD &&*/ mLBUEventFunc )
		{
			mLBUEventFunc( this );
		}
		return TRUE;
	}
	return FALSE;
}

bool	CHE_WDM_Button::OnMouseLDBClick( int32 x, int32 y )
{
	if ( IsEnable() )
	{
		if ( CHE_WDM_Area::OnMouseLDBClick( x, y ) )
		{
			return TRUE;
		}
		if ( mDBClickEventFunc )
		{
			mDBClickEventFunc( this );
		}
		return TRUE;
	}
	return FALSE;
}

bool	CHE_WDM_MouseEventBtn::OnMouseOver()
{
	if ( IsEnable() )
	{
		if ( mMouseOverEventFunc )
		{
			mMouseOverEventFunc( this );
		}
		CHE_WDM_Button::OnMouseOver();
		return TRUE;
	}
	return FALSE;
}

bool	CHE_WDM_MouseEventBtn::OnMouseOut()
{
	if ( IsEnable() )
	{
		if ( mMouseOutEventFunc )
		{
			mMouseOutEventFunc( this );
		}
		CHE_WDM_Button::OnMouseOut();
		return TRUE;
	}
	return FALSE;
}

CHE_WDM_MouseEventBtn * CHE_WDM_MouseEventBtn::Create( IHE_WDM_InterActive * pInterActive, CHE_Allocator * pAllocator )
{
	if ( pAllocator == NULL )
	{
		pAllocator = GetDefaultAllocator();
	}
	return pAllocator->New<CHE_WDM_MouseEventBtn>( pInterActive, pAllocator );
}

CHE_WDM_DragArea * CHE_WDM_DragArea::Create( IHE_WDM_InterActive * pInterActive, CHE_Allocator * pAllocator )
{
	if ( pAllocator == NULL )
	{
		pAllocator = GetDefaultAllocator();
	}
	CHE_WDM_DragArea * pArea = pAllocator->New<CHE_WDM_DragArea>( pInterActive, pAllocator );
	return pArea;
}

bool CHE_WDM_DragArea::OnMouseLBDown( int32 x, int32 y )
{
	mOffsetX = x - GetPosiX();
	mOffsetY = y - GetPosiY();
	GetParent()->SetCapture( this );
	CHE_WDM_Area::OnMouseLBDown( x, y );
	return TRUE;
}

bool CHE_WDM_DragArea::OnMouseLBUp( int32 x, int32 y )
{
	GetParent()->ReleaseCapture();
	CHE_WDM_Area::OnMouseLBUp( x, y );
	return TRUE;
}

void CHE_WDM_DragArea::SetRange( int32 left, int32 top, int32 right, int32 bottom )
{
	mRangeLeft = left;
	mRangeTop = top;
	mRangeRight = right;
	mRangeBottom = bottom;
}

bool CHE_WDM_DragArea::OnMouseMove( int32 x, int32 y )
{
	if ( IsMouseLBDown() )
	{
		int32 tmpX = x - mOffsetX;
		if ( tmpX < mRangeLeft )
		{
			SetPosiX( mRangeLeft );
		}else if ( tmpX > mRangeRight )
		{
			SetPosiX( mRangeRight );
		}
		else
		{
			SetPosiX( tmpX );
		}
		int32 tmpY = y - mOffsetY;
		if ( tmpY < mRangeTop )
		{
			SetPosiY( mRangeTop );
		}else if ( tmpY > mRangeBottom )
		{
			SetPosiY( mRangeBottom );
		}
		else
		{
			SetPosiY( tmpY );
		}
		if ( mEventFunc )
		{
			mEventFunc( this );
		}

		//CHE_WDM_Area::OnMouseMove( x, y );
		if ( GetInterActive() )
		{
			GetInterActive()->InvalidateRect( GetPosiX(), GetPosiY(), GetPosiX() + GetWidth(), GetPosiY() + GetHeight() );
		}
	}
	return TRUE;
}

bool CHE_WDM_AreaAnimation::SetTarget( CHE_WDM_Area * pArea )
{
	if ( pArea )
	{
		mpArea = pArea;
		return TRUE;
	}
	return FALSE;
}

void CHE_WDM_AreaAnimation::Init()
{
	mCurFrame = 0;
	mIndex = 0;
	mFramesToGo = 0;
	if ( mAnimationFrames.size() > 0 )
	{
		mFramesToGo = mAnimationFrames[0];
		mStateOffset.mPosiX = ( mAnimations[0].mPosiX - mState.mPosiX ) * 1.0 / mFramesToGo;
		mStateOffset.mPosiY = ( mAnimations[0].mPosiY - mState.mPosiY ) * 1.0 / mFramesToGo;
		//mStateOffset.mAlpha = ( mAnimations[0].mAlpha - mState.mAlpha ) / mFramesToGo;
		//mStateOffset.mScaleX = ( mAnimations[0].mScaleX - mState.mScaleX ) / mFramesToGo;
		//mStateOffset.mScaleY = ( mAnimations[0].mScaleY - mState.mScaleY ) / mFramesToGo;
	}
	if ( mpArea )
	{
		mpArea->SetPosiX( mState.mPosiX );
		mpArea->SetPosiY( mState.mPosiY );
		mpArea->Refresh();
		//mpArea->SetAlpha( mState.mAlpha );
		//mpArea->SetScaleX( mState.mScaleX );
		//mpArea->SetScaleY( mState.mScaleY );
	}
}

void CHE_WDM_AreaAnimation::CallFunction()
{
	if ( mpOverFunction )
	{
		mpOverFunction();
	}
}

void CHE_WDM_AreaAnimation::InsertFrames( size_t frames, const CHE_WDM_AnimationData & state )
{
	if ( frames > 0 )
	{
		mAnimationFrames.push_back( frames );
		mAnimations.push_back( state );
		mFramesCount += frames;
	}
}

void CHE_WDM_AreaAnimation::Execute()
{
	if ( mCurFrame < mFramesCount && mpArea )
	{
		if ( mFramesToGo > 0 )
		{
			--mFramesToGo;
			mpArea->Refresh();
			mpArea->SetPosiX( mpArea->GetPosiX() + mStateOffset.mPosiX );
			mpArea->SetPosiY( mpArea->GetPosiY() + mStateOffset.mPosiY );
			//mpArea->SetAlpha( mpArea->GetAlpha() + mStateOffset.mAlpha );
			//mpArea->SetScaleX( mpArea->GetScaleX() + mStateOffset.mScaleX );
			//mpArea->SetScaleY( mpArea->GetScaleY() + mStateOffset.mScaleY );
			mpArea->Refresh();
		}
		if ( mFramesToGo == 0 && mIndex < mAnimations.size() - 1 )
		{
			++mIndex;
			mFramesToGo = mAnimationFrames[mIndex];
			mStateOffset.mPosiX = ( mAnimations[mIndex].mPosiX - mpArea->GetPosiX() ) * 1.0 / mFramesToGo;
			mStateOffset.mPosiY = ( mAnimations[mIndex].mPosiY - mpArea->GetPosiY() ) * 1.0 / mFramesToGo;
			//mStateOffset.mAlpha = ( mAnimations[mIndex].mAlpha - mpArea->GetAlpha() ) / mFramesToGo;
			//mStateOffset.mScaleX = ( mAnimations[mIndex].mScaleX - mpArea->GetScaleX() ) / mFramesToGo;
			//mStateOffset.mScaleY = ( mAnimations[mIndex].mScaleY - mpArea->GetScaleY() ) / mFramesToGo;
		}
		mCurFrame++;
	}
}

bool CHE_WDM_AreaAnimation::IsOver()
{
	if( mCurFrame == mFramesCount )
	{
		return TRUE;
	}
	return FALSE;
}

void CHE_WDM_AppearAnimation::Init()
{
	mCurFrame = 0;
	mIndex = 0;
	mFramesToGo = 0;
	if ( mAnimationFrames.size() > 0 )
	{
		mFramesToGo = mAnimationFrames[0];
		mStateOffset.mPosiX = ( mAnimations[0].mPosiX - mState.mPosiX ) * 1.0 / mFramesToGo;
		mStateOffset.mPosiY = ( mAnimations[0].mPosiY - mState.mPosiY ) * 1.0 / mFramesToGo;
		mStateOffset.mAlpha = ( mAnimations[0].mAlpha - mState.mAlpha ) / mFramesToGo;
		mStateOffset.mScaleX = ( mAnimations[0].mScaleX - mState.mScaleX ) / mFramesToGo;
		mStateOffset.mScaleY = ( mAnimations[0].mScaleY - mState.mScaleY ) / mFramesToGo;
	}
	if ( mAppearPtr )
	{
		mAppearPtr->SetAlpha( mState.mAlpha );
		mAppearPtr->SetPosiX( mState.mPosiX );
		mAppearPtr->SetPosiY( mState.mPosiY );
		mAppearPtr->SetScaleX( mState.mScaleX );
		mAppearPtr->SetScaleY( mState.mScaleY );
	}
}

void CHE_WDM_AppearAnimation::Execute()
{
	if ( mCurFrame < mFramesCount && mpArea )
	{
		if ( mFramesToGo > 0 )
		{
			--mFramesToGo;
			mpArea->Refresh();
			mAppearPtr->SetPosiX( mAppearPtr->GetPosiX() + mStateOffset.mPosiX );
			mAppearPtr->SetPosiY( mAppearPtr->GetPosiY() + mStateOffset.mPosiY );
			mAppearPtr->SetAlpha( mAppearPtr->GetAlpha() + mStateOffset.mAlpha );
			mAppearPtr->SetScaleX( mAppearPtr->GetScaleX() + mStateOffset.mScaleX );
			mAppearPtr->SetScaleY( mAppearPtr->GetScaleY() + mStateOffset.mScaleY );
			mpArea->Refresh();
		}
		if ( mFramesToGo == 0 && mIndex < mAnimations.size() - 1 )
		{
			++mIndex;
			mFramesToGo = mAnimationFrames[mIndex];
			mStateOffset.mPosiX = ( mAnimations[mIndex].mPosiX - mAppearPtr->GetPosiX() ) * 1.0 / mFramesToGo;
			mStateOffset.mPosiY = ( mAnimations[mIndex].mPosiY - mAppearPtr->GetPosiY() ) * 1.0 / mFramesToGo;
			mStateOffset.mAlpha = ( mAnimations[mIndex].mAlpha - mAppearPtr->GetAlpha() ) / mFramesToGo;
			mStateOffset.mScaleX = ( mAnimations[mIndex].mScaleX - mAppearPtr->GetScaleX() ) / mFramesToGo;
			mStateOffset.mScaleY = ( mAnimations[mIndex].mScaleY - mAppearPtr->GetScaleX() ) / mFramesToGo;
		}
		mCurFrame++;
	}
}

void	CHE_WDM_AnimationMgr::StartAreaAnimation( const CHE_WDM_AreaAnimation & animation )
{
	mAreaAnimations.push_back( animation );
	mAreaAnimations[mAreaAnimations.size()-1].Init();
}

void	CHE_WDM_AnimationMgr::StartAppearAnimation( const CHE_WDM_AppearAnimation & animation )
{
	mAppearAnimations.push_back( animation );
	mAppearAnimations[mAppearAnimations.size()-1].Init();
}

void CHE_WDM_AnimationMgr::Execute()
{
	std::vector<CHE_WDM_AreaAnimation>::iterator it;
	for ( it = mAreaAnimations.begin(); it != mAreaAnimations.end(); ++it )
	{
		it->Execute();
		if ( it->IsOver() )
		{
			it->CallFunction();

			if ( ! it->IsLoop() )
			{
				it = mAreaAnimations.erase( it );
				break;
			}
			else{
				it->Init();
			}
		}
	}

	std::vector<CHE_WDM_AppearAnimation>::iterator tmpIt;
	for ( tmpIt = mAppearAnimations.begin(); tmpIt != mAppearAnimations.end(); ++tmpIt )
	{
		tmpIt->Execute();
		if ( tmpIt->IsOver() )
		{
			if ( ! tmpIt->IsLoop() )
			{
				mAppearAnimations.erase( tmpIt );
			}
			else{
				tmpIt->Init();
			}
		}
	}
}

CHE_WDM_AppearItemPtr::CHE_WDM_AppearItemPtr( const CHE_WDM_AppearItemPtr & ptr )
	: mpItem( NULL )
{
	if ( mpItem != ptr.mpItem && ptr.mpItem != NULL )
	{
		if ( mpItem )
		{
			mpItem->mRefs.DecRef();
			if ( mpItem->mRefs == 0 )
			{
				mpItem->GetAllocator()->Delete( mpItem );
				mpItem = NULL;
			}
		}
		mpItem = ptr.mpItem;
		if ( mpItem )
		{
			mpItem->mRefs.AddRef();
		}
	}
}

CHE_WDM_AppearItemPtr::~CHE_WDM_AppearItemPtr()
{
	if ( mpItem )
	{
		mpItem->mRefs.DecRef();
		if ( mpItem->mRefs == 0 )
		{
			mpItem->GetAllocator()->Delete( mpItem );
			mpItem = NULL;
		}
	}
}

CHE_WDM_AppearItemPtr CHE_WDM_AppearItemPtr::operator = ( const CHE_WDM_AppearItemPtr & ptr )
{
	if ( mpItem )
	{
		if ( mpItem == ptr.mpItem )
		{
			return *this;
		}
		mpItem->mRefs.DecRef();
		if ( mpItem->mRefs == 0 )
		{
			mpItem->GetAllocator()->Delete( mpItem );
			mpItem = NULL;
		}
	}
	mpItem = ptr.mpItem;
	if ( mpItem )
	{
		mpItem->mRefs.AddRef();
	}
	return *this;
}

void	CHE_WDM_AppearItemPtr::reset( CHE_WDM_AppearItem * pItem )
{
	if ( mpItem != pItem )
	{
		if ( mpItem )
		{
			mpItem->mRefs.DecRef();
			if ( mpItem->mRefs == 0 )
			{
				mpItem->GetAllocator()->Delete( mpItem );
				mpItem = NULL;
			}
		}
		mpItem = pItem;
		if ( mpItem )
		{
			mpItem->mRefs.AddRef();
		}
	}
}

CHE_WDM_AppearImagePtr CHE_WDM_AppearItemPtr::GetImagePtr()
{
	CHE_WDM_AppearImagePtr ptr;
	if ( mpItem && mpItem->GetType() == APPEAR_ITEM_IMAGE )
	{
		ptr.reset( (CHE_WDM_AppearImage*)mpItem );
	}
	return ptr;
}

CHE_WDM_AppearTextPtr CHE_WDM_AppearItemPtr::GetTextPtr()
{
	CHE_WDM_AppearTextPtr ptr;
	if ( mpItem && mpItem->GetType() == APPEAR_ITEM_TEXT )
	{
		ptr.reset( (CHE_WDM_AppearText*)mpItem );
	}
	return ptr;
}

CHE_WDM_AppearPathPtr CHE_WDM_AppearItemPtr::GetPathPtr()
{
	CHE_WDM_AppearPathPtr ptr;
	if ( mpItem && mpItem->GetType() == APPEAR_ITEM_PATH )
	{
		ptr.reset( (CHE_WDM_AppearPath*)mpItem );
	}
	return ptr;
}

CHE_WDM_AppearImage * CHE_WDM_AppearImagePtr::operator->() const
{
	if ( mpItem && mpItem->GetType() == APPEAR_ITEM_IMAGE )
	{
		return (CHE_WDM_AppearImage*)( mpItem );
	}
	return NULL;
}

CHE_WDM_AppearText * CHE_WDM_AppearTextPtr::operator->() const
{
	if ( mpItem && mpItem->GetType() == APPEAR_ITEM_TEXT )
	{
		return (CHE_WDM_AppearText*)( mpItem );
	}
	return NULL;
}

CHE_WDM_AppearPath * CHE_WDM_AppearPathPtr::operator->() const
{
	if ( mpItem && mpItem->GetType() == APPEAR_ITEM_PATH )
	{
		return (CHE_WDM_AppearPath*)( mpItem );
	}
	return NULL;
}