#ifndef _CHE_WDM_TEXTBOX_H_
#define _CHE_WDM_TEXTBOX_H_

#include "che_wdm.h"

class CHE_WDM_TextBox : public CHE_WDM_Area
{
public:
	static CHE_WDM_TextBox * Create( HE_DWORD width, HE_DWORD height, IHE_WDM_InterActive * pInterActive, CHE_Allocator * pAllocator = NULL );

	HE_BOOL	OnKeyDown( HE_INT32 keyCode )
	{
		switch ( keyCode )
		{
		case KEYCODE_LEFT:
			if ( mCaretPosi != 0 )
			{
				mCaretPosi--;
				mbKeyDown = TRUE;
				Refresh();
			}
			break;
		case KEYCODE_RIGHT:
			if ( mCaretPosi != mText.size() )
			{
				mCaretPosi++;
				mbKeyDown = TRUE;
				Refresh();
			}
			break;
		case KEYCODE_BACK_SPACE:
			{
				if ( mCaretPosi != 0 )
				{
					if ( mCaretPosi == mText.size() )
					{
						std::wstring tmpStr = mText;
						tmpStr[tmpStr.size()-1] = L'\0';
						mText = tmpStr.c_str();
					}else{
						std::wstring afterStr = mText.c_str() + mCaretPosi + 1;
						std::wstring tmpStr = mText;
						tmpStr[mCaretPosi] = L'\0';
						mText = tmpStr.c_str();
						mText += afterStr.c_str();
					}
					mTextAppearPtr->SetText( mText.c_str() );
					--mCaretPosi;
					mbKeyDown = TRUE;
					Refresh();
				}
			}
		default:;
		}
		return TRUE;
	}

	HE_BOOL OnKeyUp( HE_INT32 keycode )
	{
		switch ( keycode )
		{
		case KEYCODE_LEFT:
		case KEYCODE_RIGHT:
		case KEYCODE_DELETE:
			mbKeyDown = FALSE;
			break;
		default:;
		}
		return TRUE;
	}

	HE_BOOL	OnChar( HE_WCHAR charCode )
	{
		if ( mCaretPosi == mText.size() )
		{
			mText += charCode;
		}else{
			std::wstring afterStr = mText.c_str() + mCaretPosi;
			std::wstring tmpStr = mText;
			tmpStr[mCaretPosi] = L'\0';
			mText = tmpStr.c_str();
			mText += charCode;
			mText += afterStr.c_str();
		}
		mTextAppearPtr->SetText( mText.c_str() );
		++mCaretPosi;
		Refresh();
		return TRUE;
	}

	HE_BOOL	OnTimer()
	{
		if ( ! mbFocus )
		{
			return FALSE;
		}
		if ( mbCaret )
		{
			mbCaret = false;
		}else{
			mbCaret = true;
		}
		this->Refresh();
		return TRUE;
	}

	HE_BOOL OnMouseLBDown( HE_INT32 x, HE_INT32 y )
	{
		mbFocus = TRUE;
		mbMouseLBDown = TRUE;
		mCaretPosi = GetCharIndexByPosi( x, y );
		mHighListPosi = mCaretPosi;
		if ( mbHighlight )
		{
			mbHighlight = FALSE;
		}
		return TRUE;
	}

	HE_BOOL OnMouseLBUp( HE_INT32 x, HE_INT32 y )
	{
		mbMouseLBDown = FALSE;
		return TRUE;
	}

	HE_BOOL OnMouseMove( HE_INT32 x, HE_INT32 y )
	{
		if ( mbMouseLBDown )
		{
			mbHighlight = TRUE;
			mCaretPosi = GetCharIndexByPosi( x, y );
			Refresh();
		}
		return TRUE;
	}

	HE_BOOL OnMouseLDBClick( HE_INT32 x, HE_INT32 y )
	{
		mbFocus = TRUE;
		return TRUE;
	}

	HE_BOOL	OnDraw()
	{
		CHE_WDM_Area::OnDraw();
		if ( IsClipEnable() )
		{
			GetInterActive()->SetClip( this );
		}
		if ( GetInterActive() )
		{
			if ( mbHighlight )
			{
				HE_DWORD width1 = 0, width2 = 0;
				HE_DWORD height1 = 0, height2 = 0;
				GetInterActive()->MeasureChars( mTextAppearPtr, mHighListPosi, width1, height1 );
				GetInterActive()->MeasureChars( mTextAppearPtr, mCaretPosi, width2, height2 );

				HE_FLOAT left = (width1>width2) ? width2+1 : width1+1;
				HE_FLOAT width = (width1>width2) ? (width1-width2)+1 : (width2-width1)+1;
				HE_FLOAT height = GetInterActive()->GetFontHeight( mTextAppearPtr );
				mHighlightPtr = CHE_WDM_AppearPath::Create();
				mHighlightPtr->AddRect( left, 0, width, height );
				mHighlightPtr->SetOperator( APPEAR_PATH_FILL );
				mHighlightPtr->SetFillColor( 0xFF0000FF );
				GetInterActive()->Draw( this, mHighlightPtr );
			}
			mTextAppearPtr->SetWidth( this->GetWidth() );
			mTextAppearPtr->SetHeight( this->GetHeight() );
			GetInterActive()->Draw( this, mTextAppearPtr );
			if ( mbCaret || mbKeyDown )
			{
				static HE_DWORD width = 0;
				static HE_DWORD height = 0;
				static HE_DWORD count = mCaretPosi;
				GetInterActive()->MeasureChars( mTextAppearPtr, count, width, height );
				mCaretPtr->SetPosiX( width + 1 ); 
				GetInterActive()->Draw( this, mCaretPtr );
			}
			if ( mbBorder )
			{
				GetInterActive()->Draw( this, mBorderPtr );
			}
		}
		if ( IsClipEnable() )
		{
			GetInterActive()->ResetClip();
		}
		return TRUE;
	}

	HE_BOOL	OnDraw( HE_INT32 left, HE_INT32 top, HE_INT32 right, HE_INT32 bottom )
	{
		CHE_WDM_Area::OnDraw( left, top, right, bottom );
		GetInterActive()->SetClip( left, top, right, bottom );
		if ( IsClipEnable() )
		{
			GetInterActive()->SetClip( this );
		}
		if ( GetInterActive() )
		{
			if ( mbHighlight )
			{
				HE_DWORD width1 = 0, width2 = 0;
				HE_DWORD height1 = 0, height2 = 0;
				GetInterActive()->MeasureChars( mTextAppearPtr, mHighListPosi, width1, height1 );
				GetInterActive()->MeasureChars( mTextAppearPtr, mCaretPosi, width2, height2 );

				HE_FLOAT left = (width1>width2) ? width2+1 : width1+1;
				HE_FLOAT width = (width1>width2) ? (width1-width2)+1 : (width2-width1)+1;
				HE_FLOAT height = GetInterActive()->GetFontHeight( mTextAppearPtr );
				mHighlightPtr = CHE_WDM_AppearPath::Create();
				mHighlightPtr->AddRect( left, 0, width, height );
				mHighlightPtr->SetOperator( APPEAR_PATH_FILL );
				mHighlightPtr->SetFillColor( 0xFF0000FF );
				GetInterActive()->Draw( this, mHighlightPtr );
			}
			mTextAppearPtr->SetWidth( this->GetWidth() );
			mTextAppearPtr->SetHeight( this->GetHeight() );
			GetInterActive()->Draw( this, mTextAppearPtr );
			if ( mbCaret || mbKeyDown )
			{
				static HE_DWORD width = 0;
				static HE_DWORD height = 0;
				static HE_DWORD count = 0;
				count = mCaretPosi;
				GetInterActive()->MeasureChars( mTextAppearPtr, count, width, height );
				mCaretPtr->SetPosiX( width + 1 );
				GetInterActive()->Draw( this, mCaretPtr );
			}
			if ( mbBorder )
			{
				GetInterActive()->Draw( this, mBorderPtr );
			}
		}
		if ( IsClipEnable() )
		{
			GetInterActive()->ResetClip();
		}
		GetInterActive()->ResetClip();

		return TRUE;
	}

private:
	CHE_WDM_TextBox( HE_DWORD width, HE_DWORD height, IHE_WDM_InterActive * pInterActive, CHE_Allocator * pAllocator = NULL )
		: CHE_WDM_Area( pInterActive, pAllocator )
	{
		SetWidth( width );
		SetHeight( height );
		EnableClip();

		mTextAppearPtr = CHE_WDM_AppearText::Create( GetAllocator() );
		mTextAppearPtr->SetSize( 12 );
		mTextAppearPtr->SetColor( 0xFF000000 );
		mTextAppearPtr->SetLayout( CHE_WDM_Layout( LAYOUT_ALIGN_LEFT_OR_TOP, LAYOUT_ALIGN_LEFT_OR_TOP ) );

		mCaretPtr = CHE_WDM_AppearPath::Create();
		mCaretPtr->AddLine( 0, 0, 0, GetInterActive()->GetFontHeight( mTextAppearPtr ) );
		mCaretPtr->SetPosiX( 1 );
		mCaretPtr->SetPosiY( 0 );
		mCaretPtr->SetStrokeColor( 0xFF000000 );
		mCaretPtr->SetOperator( APPEAR_PATH_STROKE );

		mBorderPtr = CHE_WDM_AppearPath::Create();
		mBorderPtr->AddRect( 0, 0, GetWidth()-1, GetHeight()-1 );
		mBorderPtr->SetStrokeColor( 0xFF000000 );
		mBorderPtr->SetOperator( APPEAR_PATH_STROKE );

		mbFocus = FALSE;
		mbCaret = FALSE;
		mbBorder = TRUE;
		mbHighlight = FALSE;

		mbKeyDown = FALSE;
		mbMouseLBDown = FALSE;

		mCaretPosi = 0;
		mHighListPosi = 0;

		pInterActive->SetTimer( this, 500 );
	}

	HE_DWORD GetCharIndexByPosi( HE_INT32 x, HE_INT32 y )
	{
		HE_INT32 realPosiX = x - GetPosiX();
		HE_INT32 realPosiY = y - GetPosiY();
		if ( GetInterActive() && mTextAppearPtr->GetText() )
		{
			HE_DWORD index = 1;
			HE_DWORD count = wcslen( mTextAppearPtr->GetText() );
			HE_DWORD width = 0, height = 0;
			for ( ; index <= count; ++index )
			{
				if ( GetInterActive()->MeasureChars( mTextAppearPtr, index, width, height ) )
				{
					if ( (HE_INT32)width > realPosiX )
					{
						return index-1;
					}
				}
			}
			return count;
		}
		return 0;
	}

private:
	std::wstring			mText;
	CHE_WDM_AppearTextPtr	mTextAppearPtr;
	CHE_WDM_AppearPathPtr	mCaretPtr;
	CHE_WDM_AppearPathPtr	mHighlightPtr;
	CHE_WDM_AppearPathPtr	mBorderPtr;

	HE_DWORD				mCaretPosi;
	HE_DWORD				mHighListPosi;

	HE_BOOL					mbFocus;
	HE_BOOL					mbCaret;
	HE_BOOL					mbBorder;
	HE_BOOL					mbHighlight;

	HE_BOOL					mbKeyDown;
	HE_BOOL					mbMouseLBDown;
	
	friend class CHE_Allocator;
};

#endif