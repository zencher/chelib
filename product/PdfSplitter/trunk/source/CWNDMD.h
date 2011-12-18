#ifndef _CWNDMD_H_
#define _CWNDMD_H_

#include <vector>
#include <list>

enum APPEAR_ITEM_TYPE
{
	APPEAR_TYPE_IMAGE,
	APPEAR_TYPE_TEXT,
	APPEAR_TYPE_PATH
};

class CHE_WD_AppearItem
{
public:
	CHE_WD_AppearItem(): mAlpha(1), mScaleX(1), mScaleY(1),
			mPositionX(0), mPositionY(0), mExtData(NULL) {}

	~CHE_WD_AppearItem() {}

	virtual APPEAR_ITEM_TYPE	GetType() = 0;
	float						GetAlpha() { return mAlpha; }
	void						SetAlpha( float alpha ) { mAlpha = alpha; }
	float						GetScaleX() { return mScaleX; }
	void						SetScaleX( float scaleX ) { mScaleX = scaleX; }
	float						GetScaleY() { return mScaleY; }
	void						SetScaleY( float scaleY ) { mScaleY = scaleY; }
	int							GetPositionX() { return mPositionX; }
	void						SetPositionX( int x ) { mPositionX = x; }
	int							GetPositionY() { return mPositionY; }
	void						SetPositionY( int y ) { mPositionY = y; }
	void						SetExtData( void * pExtData ) { mExtData = pExtData; }
	void *						GetExtData() { return mExtData; }

protected:
	float						mAlpha;
	float						mScaleX;
	float						mScaleY;
	int							mPositionX;
	int							mPositionY;
	void *						mExtData;
};

enum APPEAR_IMAGE_STYLE
{
	APPEAR_IMAGE_STYLE_SINGLE,
	APPEAR_IMAGE_STYLE_TILTING,
	APPEAR_IMAGE_STYLE_FIT
};

class CHE_WD_AppearImage : public CHE_WD_AppearItem
{
public:
	CHE_WD_AppearImage() : mFile(NULL) {}
	~CHE_WD_AppearImage() {}

	APPEAR_ITEM_TYPE	GetType() { return APPEAR_TYPE_IMAGE; }

	void				SetStyle( APPEAR_IMAGE_STYLE style ) { mStyle = style; }

	APPEAR_IMAGE_STYLE	GetStyle() { return mStyle; }

	void				SetImageFile( wchar_t * pFile )
	{
		if ( mFile )
		{
			delete [] mFile;
			mFile = NULL;
		}
		if ( pFile )
		{
			mFile = new wchar_t[wcslen(pFile)+1];
			wcscpy( mFile, pFile );
		}
	}
	wchar_t *			 GetFile() { return mFile; }

protected:
	APPEAR_IMAGE_STYLE	mStyle;
	wchar_t *			mFile;
};

enum APPEAR_TEXT_HALIGNMENT
{
	APPEAR_TEXT_HALIGNMENT_LEFT,
	APPEAR_TEXT_HALIGNMENT_CENTER,
	APPEAR_TEXT_HALIGNMENT_RIGHT
};

enum APPEAR_TEXT_VALIGNMENT
{
	APPEAR_TEXT_VALIGNMENT_TOP,
	APPEAR_TEXT_VALIGNMENT_CENTER,
	APPEAR_TEXT_VALIGNMENT_BOTTOM
};

class CHE_WD_AppearText : public CHE_WD_AppearItem
{
public:
	CHE_WD_AppearText() : mText(NULL), mFontFile(NULL), mSize(1), mColor( 0xFF000000 ),
		mVertAlign(APPEAR_TEXT_VALIGNMENT_CENTER), mHoriAlign(APPEAR_TEXT_HALIGNMENT_CENTER),
		mWidth(0), mHeight(0) {}
	~CHE_WD_AppearText() {}

	APPEAR_ITEM_TYPE GetType() { return APPEAR_TYPE_TEXT; }

	void SetText( const wchar_t * text )
	{
		if ( text )
		{
			if ( mText )
			{
				delete [] mText;
				mText = NULL;
			}
			mText = new wchar_t[wcslen(text)+1];
			wcscpy( mText, text );
		}
	}

	wchar_t * GetText() { return mText; }

	unsigned int GetColor() { return mColor; }

	void SetColor( unsigned int val ) { mColor = val; }

	void SetFontFile( wchar_t * file )
	{
		if ( mFontFile )
		{
			delete [] mFontFile;
			mFontFile = NULL;
		}
		if ( file )
		{
			mFontFile = new wchar_t[wcslen(file)+1];
			wcscpy( mFontFile, file );
		}
	}

	wchar_t * GetFontFile() { return mFontFile; }

	void SetVertAlign( APPEAR_TEXT_VALIGNMENT align ) { mVertAlign = align; }

	APPEAR_TEXT_VALIGNMENT GetVertAlign() { return mVertAlign; }

	void SetHoriAlign( APPEAR_TEXT_HALIGNMENT align ) { mHoriAlign = align; }

	APPEAR_TEXT_HALIGNMENT GetHoriAlign() { return mHoriAlign; }

	void SetSize( unsigned int size ) { mSize = size; }

	unsigned int GetSize() { return mSize; }

	void SetWidth( unsigned int width ) { mWidth = width; }

	unsigned int GetWidth() { return mWidth; }

	void SetHeight( unsigned int height ) { mHeight = height; }

	unsigned int GetHeight() { return mHeight; }

protected:
	wchar_t *				mText;
	wchar_t *				mFontFile;
	unsigned int			mSize;
	unsigned int			mColor;
	APPEAR_TEXT_VALIGNMENT	mVertAlign;
	APPEAR_TEXT_HALIGNMENT	mHoriAlign;
	unsigned int			mWidth;
	unsigned int			mHeight;
};

enum APPEAR_PATH_OPERATOR
{
	APPEAR_PATH_FILL,
	APPEAR_PATH_STROKE,
	APPEAR_PATH_FILL_STROKE
};

enum APPEAR_PATH_FILL_MODE
{
	APPEAR_PATH_FILL_NONZERO,
	APPEAR_PATH_FILL_EVENODD
};

enum APPEAR_PATH_TYPE
{
	APPEAR_PATH_ITEM_LINE,		//4
	APPEAR_PATH_ITEM_RECT,		//4
	APPEAR_PATH_ITEM_CURVE		//6
};

class CHE_WD_AppearPathItem
{
public:
	CHE_WD_AppearPathItem() : mValue(0) {}
	~CHE_WD_AppearPathItem() {}

	void SetType( APPEAR_PATH_TYPE type ) { mType = type; }
	APPEAR_PATH_TYPE GetType() const { return mType; }
	void SetValue( float val ) { mValue = val; }
	float GetValue() const { return mValue; }

private:
	union
	{
		APPEAR_PATH_TYPE		mType;
		float					mValue;
	};
};

class CHE_WD_AppearPath : public CHE_WD_AppearItem
{
public:
	CHE_WD_AppearPath() : mLineWidth(1), mFillColor(0xFF000000), mStrokeColor(0xFF000000), 
		mOpt(APPEAR_PATH_FILL_STROKE), mFillMode(APPEAR_PATH_FILL_NONZERO) {};
	~CHE_WD_AppearPath() {};

	APPEAR_ITEM_TYPE GetType() { return APPEAR_TYPE_PATH; }

	void AddLine( float xStart, float yStart, float xEnd, float yEnd )
	{
		CHE_WD_AppearPathItem item;
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

	void AddRect( float left, float top, float width, float hight )
	{
		CHE_WD_AppearPathItem item;
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

	void AddCurve( float x0, float y0, float x1, float y1, float x2, float y2 )
	{
		CHE_WD_AppearPathItem item;
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

	size_t GetItemCount() { return mItems.size(); }
	bool GetItem( unsigned int index, CHE_WD_AppearPathItem & itemRet )
	{
		if ( index >= mItems.size() )
		{
			return false;
		}
		itemRet = mItems[index];
		return true;
	}

	void SetLineWidth( unsigned int width ) { mLineWidth = width; }
	unsigned int GetLineWidth() { return mLineWidth; }

	void SetFillColor( unsigned int color ) { mFillColor = color; }
	unsigned int GetFillColor() { return mFillColor; }
	void SetStrokeColor( unsigned int color ) { mStrokeColor = color; }
	unsigned int GetStrokeColor() { return mStrokeColor; }

	void SetOperator( APPEAR_PATH_OPERATOR opt ) { mOpt = opt; }
	APPEAR_PATH_OPERATOR GetOperator() { return mOpt; }

	void SetFillMode( APPEAR_PATH_FILL_MODE fillMode ) { mFillMode = fillMode; }
	APPEAR_PATH_FILL_MODE GetFillMode() { return mFillMode; }

private:
	unsigned int			mLineWidth;
	unsigned int			mFillColor;
	unsigned int			mStrokeColor;
	APPEAR_PATH_OPERATOR	mOpt;
	APPEAR_PATH_FILL_MODE	mFillMode;
	std::vector< CHE_WD_AppearPathItem > mItems;

};

class CHE_WD_AnimationData
{
public:
	CHE_WD_AnimationData(): mAlpha(1), mScaleX(1), mScaleY(1), mPositionX(0), mPositionY(0) {}

	float					mAlpha;
	float					mScaleX;
	float					mScaleY;
	int						mPositionX;
	int						mPositionY;
};
 
class CHE_WD_Animation
{
public:
	CHE_WD_Animation() {};
	~CHE_WD_Animation() {};

	bool SetTarget( CHE_WD_AppearItem * pAppear );
	void SetBeginState( const CHE_WD_AnimationData & state ) { mBeginState = state; }
	void SetEndState( const CHE_WD_AnimationData & state ) { mEndState = state; }
	void SetFrames( unsigned int frames ) { mFramesCount = frames; }
	void Init();
	void Execute();
	bool IsOver();

protected:
	unsigned int					mFramesCount;
	unsigned int					mCurFrame;
	CHE_WD_AppearItem *				mAppearItem;
	CHE_WD_AnimationData			mEndState;
	CHE_WD_AnimationData			mBeginState;
	CHE_WD_AnimationData			mStateOffset;
};

class CHE_WD_Appearance
{
public:
	std::vector<CHE_WD_AppearItem*> mItems;
};

class CHE_WD_Area;

class IHE_WD_InterActive
{
public:
	virtual void Invalidate() = 0;
	virtual void InvalidateRect( int left, int top, int right, int bottom ) = 0;
	virtual void SetClip( CHE_WD_Area * pArea ) = 0;
	virtual void SetClip( int left, int top, int right, int bottom ) = 0;
	virtual void ResetClip() = 0;
	virtual void Draw( CHE_WD_Area * pArea, CHE_WD_Appearance * pAppear ) = 0;
	virtual void SetTimer( unsigned int ) = 0;
	virtual void KillTimer() = 0;
};

enum AREA_POSITION_X_TYPE
{
	AREA_POSITION_X_FIX,
	AREA_POSITION_X_LEFT,
	AREA_POSITION_X_CENTER,
	AREA_POSITION_X_RIGHT
};

enum AREA_POSITION_Y_TYPE
{
	AREA_POSITION_Y_FIX,
	AREA_POSITION_Y_TOP,
	AREA_POSITION_Y_CENTER,
	AREA_POSITION_Y_BOTTOM
};

enum AREA_WIDTH_TYPE
{
	AREA_WIDTH_FIX,
	AREA_WIDTH_FILL
};

enum AREA_HEIGHT_TYPE
{
	AREA_HEIGHT_FIX,
	AREA_HEIGHT_FILL
};

class CHE_WD_Area
{
public:
	CHE_WD_Area() : mWidthType(AREA_WIDTH_FIX), mHightType(AREA_HEIGHT_FIX), mPositionXType(AREA_POSITION_X_FIX), mPositionYType(AREA_POSITION_Y_FIX),
		mWidth(0), mHeight(0), mX(0), mY(0), mInterActive(NULL), mbMO(false), mbLBD(false), mbRBD(false), mbClip(false), mbVisable(true), mbEnable(true),
		mParent(NULL), mpDefaultAnimation(NULL), mpMouseOverAnimation(NULL), mpMouseLButtonDownAnimation(NULL), mpMouseRButtonDownAnimation(NULL), mCurAnimation(NULL),
		mpBackgroundAppear(NULL), mpNormalAppear(NULL), mpMouseOverAppear(NULL), mpMouseOverArea(NULL), mpMouseLButtonDownAppear(NULL),
		mpMouseRButtonDownAppear(NULL), mpCaptureChild(NULL) {};

	CHE_WD_Area( unsigned int width, unsigned int height, IHE_WD_InterActive * pInterActive = NULL )
		: mWidthType(AREA_WIDTH_FIX), mHightType(AREA_HEIGHT_FIX), mPositionXType(AREA_POSITION_X_FIX), mPositionYType(AREA_POSITION_Y_FIX),
		mWidth(width), mHeight(height), mX(0), mY(0), mInterActive(pInterActive), mbMO(false), mbLBD(false), mbRBD(false), mbClip(false), mbVisable(true), mbEnable(true),
		mParent(NULL), mpDefaultAnimation(NULL), mpMouseOverAnimation(NULL), mpMouseLButtonDownAnimation(NULL), mpMouseRButtonDownAnimation(NULL), 
		mCurAnimation(NULL), mpBackgroundAppear(NULL), mpNormalAppear(NULL), mpMouseOverAppear(NULL), mpMouseOverArea(NULL), mpMouseLButtonDownAppear(NULL),
		mpMouseRButtonDownAppear(NULL), mpCaptureChild(NULL) {};

	void SetParent( CHE_WD_Area * pParent )
	{
		mParent = pParent;
	}

	CHE_WD_Area * GetParent() { return mParent; }

	//基本属性
	virtual void SetWidth( unsigned int width )
	{
		mWidth = width;
		CHE_WD_Area * pTmp = NULL;
		for ( size_t i = 0; i < mChildren.size(); ++i )
		{
			pTmp = mChildren[i];
			if ( pTmp )
			{
				switch( pTmp->GetWidthType() )
				{
				case AREA_WIDTH_FILL:
					pTmp->SetWidth( width );
					break;
				case AREA_WIDTH_FIX:
					break;
				default:;
				}
				switch( pTmp->GetPositionXType() )
				{
				case AREA_POSITION_X_FIX:
					break;
				case AREA_POSITION_X_LEFT: 
					{
						pTmp->SetPositionX( mX );
						break;
					}
				case AREA_POSITION_X_CENTER:
					{
						pTmp->SetPositionX( mX + (mWidth/2) - pTmp->GetWidth()/2 );
						break;
					}
				case AREA_POSITION_X_RIGHT:
					{
						pTmp->SetPositionX( mX + mWidth - pTmp->GetWidth() );
						break;
					}
				default:;
				}
			}
		}
	}
	virtual void SetHeight( unsigned int height )
	{
		mHeight = height;
		CHE_WD_Area * pTmp = NULL;
		for ( size_t i = 0; i < mChildren.size(); ++i )
		{
			pTmp = mChildren[i];
			if ( pTmp )
			{
				switch( pTmp->GetHeightType() )
				{
				case AREA_HEIGHT_FILL:
					pTmp->SetHeight( height );
					break;
				case AREA_WIDTH_FIX:
					break;
				default:;
				}
				switch( pTmp->GetPositionYType() )
				{
				case AREA_POSITION_Y_FIX:
					break;
				case AREA_POSITION_Y_TOP: 
					{
						pTmp->SetPositionY( mY );
						break;
					}
				case AREA_POSITION_Y_CENTER:
					{
						pTmp->SetPositionY( mY + (mHeight/2) - (pTmp->GetHeight()/2) );
						break;
					}
				case AREA_POSITION_Y_BOTTOM:
					{
						pTmp->SetPositionY( mY + mHeight - pTmp->GetHeight() );
						break;
					}
				default:;
				}
			}
		}
	}
	int GetWidth() { return mWidth; }
	int GetHeight() { return mHeight; }

	virtual void SetPositionX( int x )
	{
		int offset = x-mX;
		mX = x;
		CHE_WD_Area * pTmp = NULL;
		for ( size_t i = 0; i < mChildren.size(); ++i )
		{
			pTmp = mChildren[i];
			if ( pTmp )
			{
				pTmp->SetPositionX( pTmp->GetPositionX() + offset );
			}
		}
	}
	virtual void SetPositionY( int y )
	{
		int offset = y-mY;
		mY = y;
		CHE_WD_Area * pTmp = NULL;
		for ( size_t i = 0; i < mChildren.size(); ++i )
		{
			pTmp = mChildren[i];
			if ( pTmp )
			{
				pTmp->SetPositionY( pTmp->GetPositionY() + offset );
			}
		}
	}
	int GetPositionX() { return mX; }
	int GetPositionY() { return mY; }

	void SetWidthType( AREA_WIDTH_TYPE type ) { mWidthType = type; }
	void SetHeightType( AREA_HEIGHT_TYPE type ) { mHightType = type; }

	AREA_WIDTH_TYPE GetWidthType() { return mWidthType; }
	AREA_HEIGHT_TYPE GetHeightType() { return mHightType; }

	void SetPositionXType( AREA_POSITION_X_TYPE type ) { mPositionXType = type; }
	void SetPositionYType( AREA_POSITION_Y_TYPE type ) { mPositionYType = type; }

	AREA_POSITION_X_TYPE GetPositionXType() { return mPositionXType; }
	AREA_POSITION_Y_TYPE GetPositionYType() { return mPositionYType; }

	void EnableClip() { mbClip = true; }
	void DisableClip() { mbClip = false; }
	bool IsClipEnable() { return mbClip; }
	void SetVisable( bool bVisable ) { mbVisable = bVisable; }
	bool IsVisable() { return mbVisable; }

	void SetEnable( bool bEnable = true )
	{
		mbEnable = bEnable;
		if ( !mbEnable )
		{
			ReleaseFocus();
			ReleaseCapture();
			mbLBD = false;
			mbRBD = false;
			mbMO = false;
		}
	}

	bool IsEnable() { return mbEnable; }

	void SetFocus( CHE_WD_Area * pChild )
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

	void ReleaseFocus()
	{
		if ( mpMouseOverArea )
		{
			mpMouseOverArea->ReleaseFocus();
			mpMouseOverArea = NULL;
		}
	}

	void SetCapture( CHE_WD_Area * pChild )
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
	void ReleaseCapture()
	{
		mpCaptureChild = NULL;
		if ( GetParent() )
		{
			GetParent()->ReleaseCapture();
		}
	}

	void Refresh()
	{
		if ( mInterActive )
		{
			mInterActive->InvalidateRect( GetPositionX(), GetPositionY(), GetPositionX() + GetWidth(), GetPositionY() + GetHeight() );
		}
	}

	//外观设置
	void SetNormalAppear( CHE_WD_Appearance * appr ) { mpNormalAppear = appr; }
	CHE_WD_Appearance * GetNormalAppear() { return mpNormalAppear; }
	void SetBackGroundAppear( CHE_WD_Appearance * appr ) { mpBackgroundAppear = appr; }
	CHE_WD_Appearance * GetBackGroundAppear() { return mpBackgroundAppear; }
	void SetMouseOverAppear( CHE_WD_Appearance * appr ) { mpMouseOverAppear = appr; }
	CHE_WD_Appearance * GetMouseOverAppear() { return mpMouseOverAppear; }
	void SetMouseLButtonDownAppear( CHE_WD_Appearance * appr ) { mpMouseLButtonDownAppear = appr; }
	CHE_WD_Appearance * GetMouseLButtonDownAppear() { return mpMouseLButtonDownAppear; }
	void SetMouseRButtonDownAppear( CHE_WD_Appearance * appr ) { mpMouseRButtonDownAppear = appr; }
	CHE_WD_Appearance * GetMouseRButtonDownAppear() { return mpMouseRButtonDownAppear; }

	//子区域（窗口，控件）
	void AppendChild( CHE_WD_Area * pChlid )
	{
		if( pChlid )
		{
			mChildren.push_back( pChlid );
			pChlid->SetParent( this );
		}
	}

	CHE_WD_Area * PopChild( unsigned int index )
	{
		std::vector<CHE_WD_Area*>::iterator it = mChildren.begin();
		unsigned int i = 0;
		CHE_WD_Area * pTmp = NULL;
		for ( ; it != mChildren.end(); ++it, ++i )
		{
			if ( index == i )
			{
				pTmp = *it;
				mChildren.erase( it );
				return pTmp;
			}
		}
		return NULL;
	}

	size_t GetChildrenCount() { return mChildren.size(); }

	CHE_WD_Area * GetChild( unsigned int index ) { return ( index < GetChildrenCount() ) ? mChildren[index] : NULL ; }

	void ClearChild() { mChildren.clear(); }

	//动画过程
	void SetDefaultAnimation( CHE_WD_Animation * pAnimation ) { mpDefaultAnimation = pAnimation; }
	void SetMouseOverAnimation( CHE_WD_Animation * pAnimation ) { mpMouseOverAnimation = pAnimation; }
	void SetMouseLButtonDownAnimation( CHE_WD_Animation * pAnimation ) { mpMouseLButtonDownAnimation = pAnimation; }
	void SetMouseRButtonDownAnimation( CHE_WD_Animation * pAnimation ) { mpMouseRButtonDownAnimation = pAnimation; }

	//帧执行
	void ExecuteFrame();

	//事件接口
	virtual void OnMouseMove( int x, int y );
	virtual void OnMouseOver();
	virtual void OnMouseOut();
	virtual void OnMouseLButtonDown( int x, int y );
	virtual void OnMouseLButtonUp( int x, int y );
	virtual void OnMouseRButtonDown( int x, int y );
	virtual void OnMouseRButtonUp( int x, int y );
	virtual void OnDraw();
	virtual void OnDraw( int left, int top, int right, int bottom );

	bool IsMouseOver() { return mbMO; }
	bool IsMouseLButtonDown() { return mbLBD; }
	bool IsMouseRButtonDown() { return mbRBD; }

protected:

	AREA_HEIGHT_TYPE		mHightType;
	AREA_WIDTH_TYPE			mWidthType;
	AREA_POSITION_X_TYPE	mPositionXType;
	AREA_POSITION_Y_TYPE	mPositionYType;

	int mWidth;
	int mHeight;
	int mX;
	int mY;
	bool mbLBD;
	bool mbRBD;
	bool mbMO;
	bool mbClip;
	bool mbVisable;
	bool mbEnable;

	CHE_WD_Area *			mParent;

 	IHE_WD_InterActive *	mInterActive;
	CHE_WD_Appearance *		mpBackgroundAppear;
	CHE_WD_Appearance *		mpNormalAppear;
	CHE_WD_Appearance *		mpMouseOverAppear;
	CHE_WD_Appearance *		mpMouseLButtonDownAppear;
	CHE_WD_Appearance *		mpMouseRButtonDownAppear;

	CHE_WD_Animation *		mpDefaultAnimation;
	CHE_WD_Animation *		mpMouseOverAnimation;
	CHE_WD_Animation *		mpMouseLButtonDownAnimation;
	CHE_WD_Animation *		mpMouseRButtonDownAnimation;
	CHE_WD_Animation *		mCurAnimation;

	std::vector<CHE_WD_Area*>	mChildren;
	CHE_WD_Area *				mpCaptureChild;
	CHE_WD_Area *				mpMouseOverArea;
};

typedef void (*EventFunction)( CHE_WD_Area * pArea );

class CHE_WD_Button : public CHE_WD_Area
{
public:
	CHE_WD_Button( IHE_WD_InterActive * pInteractive )
		: CHE_WD_Area( 0, 0, pInteractive ), mClickEventFunc(NULL), mDisableAppear(NULL) {};
	~CHE_WD_Button() {};

	void SetClickEvent( EventFunction eventFunc ) { mClickEventFunc = eventFunc; }

	virtual void OnMouseLButtonUp( int x, int y )
	{
		if ( mbEnable )
		{
			if ( mClickEventFunc )
			{
				mClickEventFunc( this );
			}
			return CHE_WD_Area::OnMouseLButtonUp( x, y );
		}
	}

	virtual void OnDraw()
	{
		if ( !mbVisable )
		{
			return;
		}
		if ( mInterActive )
		{
			if ( !mbEnable && mDisableAppear )
			{
				if ( mbClip )
				{
					mInterActive->SetClip( this );
				}
				mInterActive->Draw( this, mDisableAppear );
				if ( mbClip )
				{
					mInterActive->ResetClip();
				}
			}else{
				CHE_WD_Area::OnDraw();
			}
		}
	}

	virtual void OnDraw( int left, int top, int right, int bottom )
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
				if ( !mbEnable && mDisableAppear )
				{
					mInterActive->SetClip( left, top, right, bottom );
					mInterActive->Draw( this, mDisableAppear );
					mInterActive->ResetClip();
				}else{
					CHE_WD_Area::OnDraw( left, top, right, bottom );
				}
			}
		}
	}

	void SetDisableAppear( CHE_WD_Appearance * pAppear ) { mDisableAppear = pAppear; }

	CHE_WD_Appearance * GetDisableAppear() { return mDisableAppear; }

protected:
	EventFunction mClickEventFunc;

	CHE_WD_Appearance * mDisableAppear;
};


class CHE_WD_DragArea : public CHE_WD_Area
{
public:
	CHE_WD_DragArea( IHE_WD_InterActive * pInteractive ) : CHE_WD_Area( 0, 0, pInteractive ), mOffsetX(0), mOffsetY(0),
		mRangeLeft(0), mRangeTop(0), mRangeRight(0), mRangeBottom(0), mEventFunc(NULL) {};
	~CHE_WD_DragArea() {};

	void OnMouseLButtonDown( int x, int y )
	{
		mOffsetX = x-mX;
		mOffsetY = y-mY;
		GetParent()->SetCapture( this );
		CHE_WD_Area::OnMouseLButtonDown( x, y );
	}

	void OnMouseLButtonUp( int x, int y )
	{
		GetParent()->ReleaseCapture();
		CHE_WD_Area::OnMouseLButtonUp( x, y );
	}

	void SetRange( int left, int top, int right, int bottom )
	{
		mRangeLeft = left;
		mRangeTop = top;
		mRangeRight = right;
		mRangeBottom = bottom;
	}

	void OnMouseMove( int x, int y )
	{
		if ( mbLBD )
		{
			int tmpX = x - mOffsetX;
			if ( tmpX < mRangeLeft )
			{
				mX = mRangeLeft;
			}else if ( tmpX > mRangeRight )
			{
				mX = mRangeRight;
			}
			else
			{
				mX = tmpX;
			}
			int tmpY = y - mOffsetY;
			if ( tmpY < mRangeTop )
			{
				mY = mRangeTop;
			}else if ( tmpY > mRangeBottom )
			{
				mY = mRangeBottom;
			}
			else
			{
				mY = tmpY;
			}
			if ( mEventFunc )
			{
				mEventFunc( this );
			}
		}
		CHE_WD_Area::OnMouseMove( x, y );
		mInterActive->Invalidate();
	}

	void SetDragEvent( EventFunction eventFunc ) { mEventFunc = eventFunc; }

private:
	int mOffsetX;
	int mOffsetY;
	int mRangeLeft;
	int mRangeTop;
	int mRangeRight;
	int mRangeBottom;
	EventFunction mEventFunc;
};

#endif