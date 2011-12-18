#include <stdafx.h>
#include "CWNDMD.h"


void CHE_WD_Area::ExecuteFrame()
{
	if ( mCurAnimation )
	{
		if ( mCurAnimation->IsOver() )
		{
			mCurAnimation = NULL;
		}else{
			mCurAnimation->Execute();
		}
	}else if ( mpDefaultAnimation )
	{
		mCurAnimation = mpDefaultAnimation;
		mCurAnimation->Init();
		mCurAnimation->Execute();
	}
	if ( mInterActive )
	{
		mInterActive->Invalidate();
	}
	CHE_WD_Area * pTmp = NULL;
	for ( size_t i = 0; i < mChildren.size(); ++i )
	{
		pTmp = mChildren[i];
		if ( pTmp )
		{
			pTmp->ExecuteFrame();
		}
	}
}

void CHE_WD_Area::OnMouseMove( int x, int y )
{
	if ( !mbEnable )
	{
		return;
	}
	if ( mpCaptureChild )
	{
 		mpCaptureChild->OnMouseMove( x, y );
		return;
	}
	CHE_WD_Area * pTmp = NULL;
	bool bChildrenGet = false;
	for ( size_t i = mChildren.size(); i > 0; --i )
	{
		pTmp = mChildren[i-1];
		if ( pTmp )
		{
			if (	x >= pTmp->GetPositionX() && x < pTmp->GetPositionX()+pTmp->GetWidth() &&
					y >= pTmp->GetPositionY() && y < pTmp->GetPositionY()+pTmp->GetHeight() )
			{
				if ( mpMouseOverArea == NULL )
				{
					pTmp->OnMouseOver();
				}else if ( mpMouseOverArea != pTmp )
				{
					mpMouseOverArea->OnMouseOut();
					pTmp->OnMouseOver();
				}
				pTmp->OnMouseMove( x, y );
				bChildrenGet = true;
				break;
			}
		}
	}
	if ( !bChildrenGet )
	{
		mbMO = true;
		if ( mpMouseOverArea != NULL )
		{
			mpMouseOverArea->OnMouseOut();
			OnMouseOver();
		}
	}else{
		mbMO = false;
	}
}

void CHE_WD_Area::OnMouseOver()
{
	if ( !mbEnable )
	{
		return;
	}
	if ( GetParent() )
	{
		GetParent()->SetFocus( this );
	}
	if ( mpMouseOverAnimation )
	{
		mCurAnimation = mpMouseOverAnimation;
		mpMouseOverAnimation->Init();
	}
	mbMO = true;
	if ( mInterActive )
	{
		mInterActive->InvalidateRect( GetPositionX(), GetPositionY(), GetPositionX() + GetWidth(), GetPositionY() + GetHeight() );
	}
}

void CHE_WD_Area::OnMouseOut()
{
	if ( !mbEnable )
	{
		return;
	}
	if ( GetParent() )
	{
		GetParent()->ReleaseFocus();
	}
	mbLBD = false;
	mbRBD = false;
	mbMO = false;
	mpMouseOverArea = NULL;
	if ( mInterActive )
	{
		mInterActive->InvalidateRect( GetPositionX(), GetPositionY(), GetPositionX() + GetWidth(), GetPositionY() + GetHeight() );
	}
}

void CHE_WD_Area::OnMouseLButtonDown( int x, int y )
{
	if ( !mbEnable )
	{
		return;
	}
	if ( mpCaptureChild )
	{
		mpCaptureChild->OnMouseLButtonDown( x, y );
		return;
	}
	CHE_WD_Area * pTmp = NULL;
	for ( size_t i = mChildren.size(); i > 0; --i )
	{
		pTmp = mChildren[i-1];
		if ( pTmp )
		{
			if ( x >= pTmp->GetPositionX() && x < pTmp->GetPositionX()+pTmp->GetWidth() &&
				 y >= pTmp->GetPositionY() && y < pTmp->GetPositionY()+pTmp->GetHeight() )
			{
				pTmp->OnMouseLButtonDown( x, y );
				return;
			}
		}
	}
	if ( mpMouseLButtonDownAnimation )
	{
		mCurAnimation = mpMouseLButtonDownAnimation;
		mpMouseLButtonDownAnimation->Init();
	}
	mbLBD = true;
	if ( mInterActive )
	{
		mInterActive->InvalidateRect( GetPositionX(), GetPositionY(), GetPositionX() + GetWidth(), GetPositionY() + GetHeight() );
	}
}

void CHE_WD_Area::OnMouseLButtonUp( int x, int y )
{
	if ( !mbEnable )
	{
		return;
	}
	if ( mpCaptureChild )
	{
		mpCaptureChild->OnMouseLButtonUp( x, y );
		return;
	}
	CHE_WD_Area * pTmp = NULL;
	for ( size_t i = mChildren.size(); i > 0; --i )
	{
		pTmp = mChildren[i-1];
		if ( pTmp )
		{
			if (	x >= pTmp->GetPositionX() && x < pTmp->GetPositionX()+pTmp->GetWidth() &&
					y >= pTmp->GetPositionY() && y < pTmp->GetPositionY()+pTmp->GetHeight() )
			{
				pTmp->OnMouseLButtonUp( x, y );
				return;
			}
		}
	}
	mbLBD = false;
	if ( mInterActive )
	{
		mInterActive->InvalidateRect( GetPositionX(), GetPositionY(), GetPositionX() + GetWidth(), GetPositionY() + GetHeight() );
	}
}

void CHE_WD_Area::OnMouseRButtonDown( int x, int y )
{
	if ( !mbEnable )
	{
		return;
	}
	if ( mpCaptureChild )
	{
		mpCaptureChild->OnMouseRButtonDown( x, y );
	}
	CHE_WD_Area * pTmp = NULL;
	for ( size_t i = mChildren.size(); i > 0; --i )
	{
		pTmp = mChildren[i-1];
		if ( pTmp )
		{
			if (	x >= pTmp->GetPositionX() && x < pTmp->GetPositionX()+pTmp->GetWidth() &&
					y >= pTmp->GetPositionY() && y < pTmp->GetPositionY()+pTmp->GetHeight() )
			{
				pTmp->OnMouseRButtonDown( x, y );
				return;
			}
		}
	}
	if ( mpMouseRButtonDownAnimation )
	{
		mCurAnimation = mpMouseRButtonDownAnimation;
		mpMouseRButtonDownAnimation->Init();
	}
	mbRBD = true;
	if ( mInterActive )
	{
		mInterActive->InvalidateRect( GetPositionX(), GetPositionY(), GetPositionX() + GetWidth(), GetPositionY() + GetHeight() );
	}
}

void CHE_WD_Area::OnMouseRButtonUp( int x, int y )
{
	if ( !mbEnable )
	{
		return;
	}
	if ( mpCaptureChild )
	{
		mpCaptureChild->OnMouseRButtonUp( x, y );
	}
	CHE_WD_Area * pTmp = NULL;
	for ( size_t i = mChildren.size(); i > 0; --i )
	{
		pTmp = mChildren[i-1];
		if ( pTmp )
		{
			if (	x >= pTmp->GetPositionX() && x < pTmp->GetPositionX()+pTmp->GetWidth() &&
					y >= pTmp->GetPositionY() && y < pTmp->GetPositionY()+pTmp->GetHeight() )
			{
				pTmp->OnMouseRButtonUp( x, y );
				return;
			}
		}
	}
	mbRBD = false;
	if ( mInterActive )
	{
		mInterActive->InvalidateRect( GetPositionX(), GetPositionY(), GetPositionX() + GetWidth(), GetPositionY() + GetHeight() );
	}
}

void CHE_WD_Area::OnDraw()
{
	if ( !mbVisable )
	{
		return;
	}
	if ( mInterActive )
	{
		if ( mbClip )
		{
			mInterActive->SetClip( this );
		}
		if ( mpBackgroundAppear )
		{
			mInterActive->Draw( this, mpBackgroundAppear );
		}
		if ( mpMouseLButtonDownAppear && mbLBD )
		{
			mInterActive->Draw( this, mpMouseLButtonDownAppear );
		}else if ( mpMouseRButtonDownAppear && mbRBD )
		{
			mInterActive->Draw( this, mpMouseRButtonDownAppear );
		}else if ( mpMouseOverAppear && mbMO )
		{
			mInterActive->Draw( this, mpMouseOverAppear );
		}else if ( mpNormalAppear )
		{
			mInterActive->Draw( this, mpNormalAppear );
		}
		if ( mbClip )
		{
			mInterActive->ResetClip();
		}

		CHE_WD_Area * pTmp = NULL;
		for ( size_t i = 0; i < mChildren.size(); ++i )
		{
			pTmp = mChildren[i];
			if ( pTmp )
			{
				pTmp->OnDraw();
			}
		}
	}
}

void CHE_WD_Area::OnDraw( int left, int top, int right, int bottom )
{
	if ( !mbVisable )
	{
		return;
	}
	if ( mInterActive )
	{
		if ( ( ( left < GetPositionX() && right < GetPositionX() ) ||
			( left > ( GetPositionX() + GetWidth() ) && ( right > ( GetPositionX() + GetWidth() ) ) ) &&
			( ( top < GetPositionY() && bottom < GetPositionX() ) || 
			( top > ( GetPositionY() + GetHeight() ) && ( bottom > ( GetPositionY() + GetHeight() ) ) ) ) ) )
		{

		}else{
			if ( mpBackgroundAppear )
			{
				mInterActive->SetClip( left, top, right, bottom );
				mInterActive->Draw( this, mpBackgroundAppear );
				mInterActive->ResetClip();
			}
			if ( mpMouseLButtonDownAppear && mbLBD )
			{
				mInterActive->SetClip( left, top, right, bottom );
				mInterActive->Draw( this, mpMouseLButtonDownAppear );
				mInterActive->ResetClip();
			}else if ( mpMouseRButtonDownAppear && mbRBD )
			{
				mInterActive->SetClip( left, top, right, bottom );
				mInterActive->Draw( this, mpMouseRButtonDownAppear );
				mInterActive->ResetClip();
			}else if ( mpMouseOverAppear && mbMO )
			{
				mInterActive->SetClip( left, top, right, bottom );
				mInterActive->Draw( this, mpMouseOverAppear );
				mInterActive->ResetClip();
			}else if ( mpNormalAppear )
			{
				mInterActive->SetClip( left, top, right, bottom );
				mInterActive->Draw( this, mpNormalAppear );
				mInterActive->ResetClip();
			}
		}
		CHE_WD_Area * pTmp = NULL;
		for ( size_t i = 0; i < mChildren.size(); ++i )
		{
			pTmp = mChildren[i];
			if ( pTmp )
			{
				pTmp->OnDraw( left, top, right, bottom );
			}
		}
	}
}

bool CHE_WD_Animation::SetTarget( CHE_WD_AppearItem * pAppear )
{
	if ( pAppear )
	{
		mAppearItem = pAppear;
		return true;
	}
	return false;
}

void CHE_WD_Animation::Init()
{
	mCurFrame = 0;
	if ( mAppearItem )
	{
		mAppearItem->SetAlpha( mBeginState.mAlpha );
		mAppearItem->SetPositionX( mBeginState.mPositionX );
		mAppearItem->SetPositionY( mBeginState.mPositionY );
		mAppearItem->SetScaleX( mBeginState.mScaleX );
		mAppearItem->SetScaleY( mBeginState.mScaleY );
		mStateOffset.mAlpha = (mEndState.mAlpha-mBeginState.mAlpha)/mFramesCount;
		mStateOffset.mPositionX = (mEndState.mPositionX-mBeginState.mPositionX) * 1.0 / mFramesCount;
		mStateOffset.mPositionY = (mEndState.mPositionY-mBeginState.mPositionY) * 1.0 / mFramesCount;
		mStateOffset.mScaleX = (mEndState.mScaleX-mBeginState.mScaleX)/mFramesCount;
		mStateOffset.mScaleY = (mEndState.mScaleY-mBeginState.mScaleY)/mFramesCount;
	}
}

void CHE_WD_Animation::Execute()
{
	if ( mCurFrame < mFramesCount )
	{
		if ( mAppearItem )
		{
			mAppearItem->SetAlpha( mAppearItem->GetAlpha() + mStateOffset.mAlpha );
			mAppearItem->SetPositionX( mAppearItem->GetPositionX() + mStateOffset.mPositionX );
			mAppearItem->SetPositionY( mAppearItem->GetPositionY() + mStateOffset.mPositionY );
			mAppearItem->SetScaleX( mAppearItem->GetScaleX() + mStateOffset.mScaleX );
			mAppearItem->SetScaleY( mAppearItem->GetScaleY() + mStateOffset.mScaleY );
		}
		mCurFrame++;
	}
}

bool CHE_WD_Animation::IsOver()
{
	if( mCurFrame == mFramesCount )
	{
		return true;
	}
	return false;
}