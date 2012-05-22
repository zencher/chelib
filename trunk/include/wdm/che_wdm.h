#ifndef _CHE_WDM_H_
#define _CHE_WDM_H_

#include <vector>
#include <list>

#include "../che_base.h"

enum WDM_Layout_Type
{
	LAYOUT_FIX,
	LAYOUT_FILL,
	LAYOUT_ALIGN_LEFT_OR_TOP,
	LAYOUT_ALIGN_CENTER,
	LAYOUT_ALIGN_RIGHT_OR_BOTTOM
};

class CHE_WDM_Layout : public CHE_Object
{
public:
	CHE_WDM_Layout()
		: CHE_Object( NULL ), mVertLayout( LAYOUT_FIX ), mHoriLayout( LAYOUT_FIX ) {}

	CHE_WDM_Layout( const WDM_Layout_Type & VertLayout, const WDM_Layout_Type & HoriLayout )
		: CHE_Object( NULL ), mVertLayout( VertLayout ), mHoriLayout( HoriLayout ) {}

	HE_VOID								SetVertLayout( const WDM_Layout_Type & layout ) { mVertLayout = layout; }
	WDM_Layout_Type						GetVertLayout() const { return mVertLayout; }
	
	HE_VOID								SetHoriLayout( const WDM_Layout_Type & layout ) { mHoriLayout = layout; }
	WDM_Layout_Type						GetHoriLayout() const { return mHoriLayout; }

private:
	WDM_Layout_Type						mVertLayout;
	WDM_Layout_Type						mHoriLayout;
};

enum WDM_APPEAR_ITEM_TYPE
{
	APPEAR_ITEM_IMAGE,
	APPEAR_ITEM_TEXT,
	APPEAR_ITEM_PATH
};

class CHE_WDM_AppearItem;
class CHE_WDM_AppearImage;
class CHE_WDM_AppearText;
class CHE_WDM_AppearPath;

class CHE_WDM_AppearItemPtr;
class CHE_WDM_AppearImagePtr;
class CHE_WDM_AppearTextPtr;
class CHE_WDM_AppearPathPtr;

class CHE_WDM_AppearItemPtr
{
public:
	CHE_WDM_AppearItemPtr() : mpItem( NULL ) {}

	CHE_WDM_AppearItemPtr( const CHE_WDM_AppearItemPtr & ptr );

	~CHE_WDM_AppearItemPtr();

	CHE_WDM_AppearItemPtr operator = ( const CHE_WDM_AppearItemPtr & ptr );

	bool operator!() const { return mpItem ? FALSE : TRUE ; }

	operator HE_BOOL() const { return mpItem ? TRUE : FALSE ; }

	inline CHE_WDM_AppearItem *	operator->() const { return mpItem; }

	HE_VOID	reset( CHE_WDM_AppearItem * pItem = NULL );

	CHE_WDM_AppearImagePtr GetImagePtr();

	CHE_WDM_AppearPathPtr GetPathPtr();

	CHE_WDM_AppearTextPtr GetTextPtr();

protected:
	CHE_WDM_AppearItem * mpItem;
};

class CHE_WDM_AppearItem : public CHE_Object
{
public:
	virtual WDM_APPEAR_ITEM_TYPE		GetType() = 0;

	HE_VOID								SetAlpha( HE_FLOAT alpha ) { mAlpha = alpha; }
	HE_FLOAT							GetAlpha() const { return mAlpha; }
	
	HE_VOID								SetScaleX( HE_FLOAT scaleX ) { mScaleX = scaleX; }
	HE_FLOAT							GetScaleX() const { return mScaleX; }
	
	HE_VOID								SetScaleY( HE_FLOAT scaleY ) { mScaleY = scaleY; }
	HE_FLOAT							GetScaleY() const { return mScaleY; }
	
	HE_VOID								SetPositX( HE_INT32 x ) { mPosiX = x; }
	HE_INT32							GetPosiX() const { return mPosiX; }
	
	HE_VOID								SetPositY( HE_INT32 y ) { mPosiY = y; }
	HE_INT32							GetPosiY() const { return mPosiY; }
	
	HE_VOID								SetExtData( HE_LPVOID pExtData ) { mExtData = pExtData; }
	HE_LPVOID							GetExtData() const { return mExtData; }

protected:
	CHE_WDM_AppearItem( CHE_Allocator * pAllocator = NULL )
		: CHE_Object( pAllocator ), mAlpha( 1 ), mScaleX( 1 ), mScaleY( 1 ), mPosiX( 0 ), mPosiY( 0 ), mExtData( NULL ) {}

	virtual ~CHE_WDM_AppearItem() {}

	HE_FLOAT							mAlpha;
	HE_FLOAT							mScaleX;
	HE_FLOAT							mScaleY;
	HE_INT32							mPosiX;
	HE_INT32							mPosiY;
	HE_LPVOID							mExtData;

	CHE_RefCount						mRefs;

	friend class CHE_Allocator;
	friend class CHE_WDM_AppearItemPtr;
};

class CHE_WDM_AppearImagePtr : public CHE_WDM_AppearItemPtr
{
public:
	CHE_WDM_AppearImage * operator->() const;
};

enum WDM_APPEAR_IMAGE_STYLE
{
	APPEAR_IMAGE_STYLE_SINGLE,
	APPEAR_IMAGE_STYLE_TILTING,
	APPEAR_IMAGE_STYLE_FIT
};

class CHE_WDM_AppearImage : public CHE_WDM_AppearItem
{
public:
	static CHE_WDM_AppearImagePtr		Create( CHE_Allocator * pAllocator = NULL );

	~CHE_WDM_AppearImage();

	WDM_APPEAR_ITEM_TYPE				GetType() { return APPEAR_ITEM_IMAGE; }

	HE_VOID								SetStyle( WDM_APPEAR_IMAGE_STYLE style ) { mStyle = style; }
	WDM_APPEAR_IMAGE_STYLE				GetStyle() { return mStyle; }

	HE_VOID								SetImageFile( HE_LPWSTR pFile );
	HE_LPWSTR							GetImageFile() { return mFile; }

protected:
	CHE_WDM_AppearImage( CHE_Allocator * pAllocator = NULL )
		: CHE_WDM_AppearItem( pAllocator ), mFile( NULL ), mStyle( APPEAR_IMAGE_STYLE_SINGLE ) {}

	WDM_APPEAR_IMAGE_STYLE				mStyle;
	HE_LPWSTR							mFile;

	friend class CHE_Allocator;
	friend class CHE_WDM_AppearImagePtr;
};

class CHE_WDM_AppearTextPtr : public CHE_WDM_AppearItemPtr
{
public:
	CHE_WDM_AppearText *	operator->() const;
};

class CHE_WDM_AppearText : public CHE_WDM_AppearItem
{
public:
	static CHE_WDM_AppearTextPtr		Create( CHE_Allocator * pAllocator = NULL );

	~CHE_WDM_AppearText();

	WDM_APPEAR_ITEM_TYPE				GetType() { return APPEAR_ITEM_TEXT; }

	HE_VOID								SetText( HE_LPCWSTR text );
	HE_LPCWSTR							GetText() const { return mText; }

	HE_VOID								SetColor( HE_DWORD val ) { mColor = val; }
	HE_DWORD							GetColor() const { return mColor; }

	HE_VOID								SetFontFile( HE_LPCWSTR file );
	HE_LPWSTR							GetFontFile() const { return mFontFile; }

	HE_VOID								SetLayout( CHE_WDM_Layout layout ) { mLayout = layout; }
	CHE_WDM_Layout						GetLayout() const { return mLayout; }

	HE_VOID								SetSize( HE_DWORD size ) { mSize = size; }
	HE_DWORD							GetSize() const { return mSize; }

	HE_VOID								SetWidth( HE_DWORD width ) { mWidth = width; }
	HE_DWORD							GetWidth() const { return mWidth; }

	HE_VOID								SetHeight( HE_DWORD height ) { mHeight = height; }
	HE_DWORD							GetHeight() const { return mHeight; }

protected:
	CHE_WDM_AppearText( CHE_Allocator * pAllocator = NULL )
		: CHE_WDM_AppearItem( pAllocator ), mText( NULL ), mFontFile( NULL ), mSize( 1 ), mColor( 0xFF000000 ) {}

	HE_LPWSTR							mText;
	HE_LPWSTR							mFontFile;
	HE_DWORD							mSize;
	HE_DWORD							mColor;
	CHE_WDM_Layout						mLayout;
 	HE_DWORD							mWidth;
 	HE_DWORD							mHeight;

	friend class CHE_Allocator;
	friend class CHE_WDM_AppearTextPtr;
};

enum WDM_APPEAR_PATH_OPERATOR
{
	APPEAR_PATH_FILL,
	APPEAR_PATH_STROKE,
	APPEAR_PATH_FILL_STROKE
};

enum WDM_APPEAR_PATH_TYPE
{
	APPEAR_PATH_ITEM_LINE,
	APPEAR_PATH_ITEM_RECT,
	APPEAR_PATH_ITEM_CURVE
};

enum WDM_APPEAR_PATH_FILL_MODE
{
	APPEAR_PATH_FILL_NONZERO,
	APPEAR_PATH_FILL_EVENODD
};

class CHE_WDM_AppearPathItem : public CHE_Object
{
public:
	CHE_WDM_AppearPathItem() : CHE_Object( NULL ), mValue( 0 ) {}

	HE_VOID								SetType( WDM_APPEAR_PATH_TYPE type ) { mType = type; }
	WDM_APPEAR_PATH_TYPE				GetType() const { return mType; }

	HE_VOID								SetValue( float val ) { mValue = val; }
	HE_FLOAT							GetValue() const { return mValue; }

private:
	union
	{
		WDM_APPEAR_PATH_TYPE			mType;
		HE_FLOAT						mValue;
	};
};

class CHE_WDM_AppearPathPtr : public CHE_WDM_AppearItemPtr
{
public:
	CHE_WDM_AppearPath *	operator->() const;
};

class CHE_WDM_AppearPath : public CHE_WDM_AppearItem
{
public:
	static CHE_WDM_AppearPathPtr		Create( CHE_Allocator * pAllocator = NULL );

	WDM_APPEAR_ITEM_TYPE				GetType() { return APPEAR_ITEM_PATH; }

	HE_VOID								AddLine( HE_FLOAT xStart, HE_FLOAT yStart, HE_FLOAT xEnd, HE_FLOAT yEnd );
	HE_VOID								AddRect( HE_FLOAT left, HE_FLOAT top, HE_FLOAT width, HE_FLOAT hight );
	HE_VOID								AddCurve( HE_FLOAT x0, HE_FLOAT y0, HE_FLOAT x1, HE_FLOAT y1, HE_FLOAT x2, HE_FLOAT y2 );

	HE_DWORD							GetItemCount();
	HE_BOOL								GetItem( HE_DWORD index, CHE_WDM_AppearPathItem & itemRet );

	HE_VOID								SetLineWidth( HE_FLOAT width ) { mLineWidth = width; }
	HE_FLOAT							GetLineWidth() const { return mLineWidth; }

	HE_VOID								SetFillColor( HE_DWORD color ) { mFillColor = color; }
	HE_DWORD							GetFillColor() const { return mFillColor; }

	HE_VOID								SetStrokeColor( HE_DWORD color ) { mStrokeColor = color; }
	HE_DWORD							GetStrokeColor() const { return mStrokeColor; }

	HE_VOID								SetOperator( WDM_APPEAR_PATH_OPERATOR opt ) { mOperator = opt; }
	WDM_APPEAR_PATH_OPERATOR			GetOperator() const { return mOperator; }

	HE_VOID								SetFillMode( WDM_APPEAR_PATH_FILL_MODE fillMode ) { mFillMode = fillMode; }
	WDM_APPEAR_PATH_FILL_MODE			GetFillMode() const { return mFillMode; }

private:
	CHE_WDM_AppearPath( CHE_Allocator * pAllocator = NULL )
		: CHE_WDM_AppearItem( pAllocator ), mLineWidth( 1 ), mFillColor( 0xFF000000 ), mStrokeColor( 0xFF000000 ), 
		mOperator( APPEAR_PATH_FILL_STROKE ), mFillMode( APPEAR_PATH_FILL_NONZERO ) {};

	HE_FLOAT							mLineWidth;
	HE_DWORD							mFillColor;
	HE_DWORD							mStrokeColor;
	WDM_APPEAR_PATH_OPERATOR			mOperator;
	WDM_APPEAR_PATH_FILL_MODE			mFillMode;
	std::vector<CHE_WDM_AppearPathItem>	mItems;

	friend class CHE_Allocator;
	friend class CHE_WDM_AppearPathPtr;
};

class CHE_WDM_Appearance : public CHE_Object
{
public:
	CHE_WDM_Appearance() : CHE_Object( NULL ) {}

	std::vector<CHE_WDM_AppearItemPtr>	mNormal;
	std::vector<CHE_WDM_AppearItemPtr>	mDisable;
	std::vector<CHE_WDM_AppearItemPtr>	mMouseOver;
	std::vector<CHE_WDM_AppearItemPtr>	mMouseLButtonDown;
	std::vector<CHE_WDM_AppearItemPtr>	mMouseRButtonDown;
	std::vector<CHE_WDM_AppearItemPtr>	mBackground;
	
};

class CHE_WDM_Area;

enum WDM_AREA_APPEAR_TYPE
{
	AREA_APPEAR_NORMAL,
	AREA_APPEAR_DISABLE,
	AREA_APPEAR_BACKGROUND,
	AREA_APPEAR_MOUSEOVER,
	AREA_APPEAR_MOUSELBDOWN,
	AREA_APPEAR_MOUSERBDOWN
};

class IHE_WDM_InterActive
{
public:
	virtual HE_VOID						Invalidate() = 0;
	virtual HE_VOID						InvalidateRect( HE_INT32 left, HE_INT32 top, HE_INT32 right, HE_INT32 bottom ) = 0;

	virtual HE_VOID						SetClip( CHE_WDM_Area * pArea ) = 0;
	virtual HE_VOID						SetClip( HE_INT32 left, HE_INT32 top, HE_INT32 right, HE_INT32 bottom ) = 0;
	virtual HE_VOID						ResetClip() = 0;

	virtual HE_VOID						Draw( CHE_WDM_Area * pArea, WDM_AREA_APPEAR_TYPE ) = 0;

	virtual HE_VOID						SetTimer(HE_DWORD ) = 0;
	virtual HE_VOID						KillTimer() = 0;
};

// class CHE_WDM_Area;
// 
// class CHE_WDM_AreaPtr
// {
// public:
// 	CHE_WDM_AreaPtr() : mpArea( NULL ) {}
// 
// 	CHE_WDM_AreaPtr( const CHE_WDM_AreaPtr & ptr );
// 
// 	~CHE_WDM_AreaPtr();
// 
// 	CHE_WDM_AreaPtr operator = ( const CHE_WDM_AreaPtr & ptr );
// 
// 	bool operator!() const { return mpArea ? FALSE : TRUE ; }
// 
// 	operator HE_BOOL() const { return mpArea ? TRUE : FALSE ; }
// 
// 	inline CHE_WDM_Area * operator->() const { return mpArea; }
// 
// 	HE_VOID	reset( CHE_WDM_Area * pArea = NULL );
// 
// private:
// 	CHE_WDM_Area * mpArea;
// };

class CHE_WDM_Area : public CHE_Object
{
public:
	static CHE_WDM_Area*				Create( IHE_WDM_InterActive * pInterActive, CHE_Allocator * pAllocator = NULL );

	virtual	~CHE_WDM_Area();

	HE_VOID								SetWidth( HE_INT32 width );
	HE_INT32							GetWidth() const { return mWidth; }

	HE_VOID								SetHeight( HE_INT32 height );
	HE_INT32							GetHeight() const {return mHeight; }

	HE_VOID								SetPosiX( HE_INT32 x );
	HE_VOID								SetPosiY( HE_INT32 y );
	HE_INT32							GetPosiX() const { return mPosX; }
	HE_INT32							GetPosiY() const { return mPosY; }

	HE_VOID								SetLayout( const CHE_WDM_Layout & layout ) { mLayout = layout; }
	CHE_WDM_Layout						GetLayout() const { return mLayout; }

	HE_BOOL								IsMouseOver() const { return mbMO; }
	HE_BOOL								IsMouseLBDown() const { return mbLBD; }
	HE_BOOL								IsMouseRBDown() const { return mbRBD; }

	HE_VOID								EnableClip() { mbClip = TRUE; }
	HE_VOID								DisableClip() { mbClip = FALSE; }
	HE_BOOL								IsClipEnable() const { return mbClip; }

	HE_BOOL								SetVisable( HE_BOOL bVisable ) { mbVisable = bVisable; };
	HE_BOOL								IsVisable() const { return mbVisable; }

	HE_VOID								SetEnable( HE_BOOL bEnable );
	HE_BOOL								IsEnable() const { return mbEnable; }

	HE_VOID								Refresh();

	CHE_WDM_Appearance &				GetAppear() { return mAppearance; }
	HE_VOID								AppendAppearItem( const CHE_WDM_AppearItemPtr & ptr, WDM_AREA_APPEAR_TYPE type );

	HE_DWORD							GetChildrenCount() const;

	HE_VOID								AppendChild( CHE_WDM_Area * pChlid );
	CHE_WDM_Area *						PopChild( HE_DWORD index );
	CHE_WDM_Area *						GetChild( HE_DWORD index ) const;
	HE_VOID								ClearChild();

	HE_BOOL								ChildToLower( HE_DWORD index );
	HE_BOOL								ChildToUpper( HE_DWORD index );

	virtual HE_VOID						OnMouseMove( HE_INT32 x, HE_INT32 y );
	virtual HE_VOID						OnMouseOver();
	virtual HE_VOID						OnMouseOut();
	virtual HE_VOID						OnMouseLBDown( HE_INT32 x, HE_INT32 y );
	virtual HE_VOID						OnMouseLBUp( HE_INT32 x, HE_INT32 y );
	virtual HE_VOID						OnMouseRBDown( HE_INT32 x, HE_INT32 y );
	virtual HE_VOID						OnMouseRBUp( HE_INT32 x, HE_INT32 y );
	virtual HE_VOID						OnDraw();
	virtual HE_VOID						OnDraw( HE_INT32 left, HE_INT32 top, HE_INT32 right, HE_INT32 bottom );

	HE_VOID								SetFocus( CHE_WDM_Area * pChild );
	HE_VOID								ReleaseFocus();

	HE_VOID								SetCapture( CHE_WDM_Area * pChild );
	HE_VOID								ReleaseCapture();

protected:
	CHE_WDM_Area( IHE_WDM_InterActive * pInteractive, CHE_Allocator * pAllocator = NULL );

	HE_VOID								SetParent( CHE_WDM_Area * pParent ) { mParent = pParent; }
	CHE_WDM_Area *						GetParent() const { return mParent; }

	HE_VOID								SetInterActive( IHE_WDM_InterActive * pInterActive ) { mInterActive = pInterActive; }
	IHE_WDM_InterActive *				GetInterActive() { return mInterActive; }

private:
	HE_BOOL								mbLBD;
	HE_BOOL								mbRBD;
	HE_BOOL								mbMO;

	HE_BOOL								mbClip;
	HE_BOOL								mbVisable;
	HE_BOOL								mbEnable;

	HE_DWORD							mWidth;
	HE_DWORD							mHeight;
	HE_INT32							mPosX;
	HE_INT32							mPosY;

	CHE_WDM_Layout						mLayout;
	CHE_WDM_Area *						mParent;
	IHE_WDM_InterActive *				mInterActive;
	CHE_WDM_Appearance					mAppearance;
	std::vector<CHE_WDM_Area*>			mChildren;
	CHE_WDM_Area *						mpCaptureChild;
	CHE_WDM_Area *						mpMouseOverArea;

	friend class CHE_Allocator;

// 	CHE_RefCount						mRefs;
// 
// 	friend class CHE_Allocator;
// 	friend class CHE_WDM_AreaPtr;
};

// class CHE_WD_AnimationData
// {
// public:
// 	CHE_WD_AnimationData(): mAlpha(1), mScaleX(1), mScaleY(1), mPositionX(0), mPositionY(0) {}
// 
// 	float					mAlpha;
// 	float					mScaleX;
// 	float					mScaleY;
// 	int						mPositionX;
// 	int						mPositionY;
// };
//  
// class CHE_WD_Animation
// {
// public:
// 	CHE_WD_Animation() {};
// 	~CHE_WD_Animation() {};
// 
// 	bool SetTarget( CHE_WD_AppearItem * pAppear );
// 	void SetBeginState( const CHE_WD_AnimationData & state ) { mBeginState = state; }
// 	void SetEndState( const CHE_WD_AnimationData & state ) { mEndState = state; }
// 	void SetFrames( unsigned int frames ) { mFramesCount = frames; }
// 	void Init();
// 	void Execute();
// 	bool IsOver();
// 
// protected:
// 	unsigned int					mFramesCount;
// 	unsigned int					mCurFrame;
// 	CHE_WD_AppearItem *				mAppearItem;
// 	CHE_WD_AnimationData			mEndState;
// 	CHE_WD_AnimationData			mBeginState;
// 	CHE_WD_AnimationData			mStateOffset;
// };
// 
// class CHE_WD_Appearance
// {
// public:
// 	std::vector<CHE_WD_AppearItem*> mItems;
// };
//


// enum AREA_POSITION_X_TYPE
// {
// 	AREA_POSITION_X_FIX,
// 	AREA_POSITION_X_LEFT,
// 	AREA_POSITION_X_CENTER,
// 	AREA_POSITION_X_RIGHT
// };
// 
// enum AREA_POSITION_Y_TYPE
// {
// 	AREA_POSITION_Y_FIX,
// 	AREA_POSITION_Y_TOP,
// 	AREA_POSITION_Y_CENTER,
// 	AREA_POSITION_Y_BOTTOM
// };
// 
// enum AREA_WIDTH_TYPE
// {
// 	AREA_WIDTH_FIX,
// 	AREA_WIDTH_FILL
// };
// 
// enum AREA_HEIGHT_TYPE
// {
// 	AREA_HEIGHT_FIX,
// 	AREA_HEIGHT_FILL
// };
// 

// class CHE_WD_Area
// {
// public:
// 
// 	//外观设置
// 	void SetNormalAppear( CHE_WD_Appearance * appr ) { mpNormalAppear = appr; }
// 	CHE_WD_Appearance * GetNormalAppear() { return mpNormalAppear; }
// 	void SetBackGroundAppear( CHE_WD_Appearance * appr ) { mpBackgroundAppear = appr; }
// 	CHE_WD_Appearance * GetBackGroundAppear() { return mpBackgroundAppear; }
// 	void SetMouseOverAppear( CHE_WD_Appearance * appr ) { mpMouseOverAppear = appr; }
// 	CHE_WD_Appearance * GetMouseOverAppear() { return mpMouseOverAppear; }
// 	void SetMouseLButtonDownAppear( CHE_WD_Appearance * appr ) { mpMouseLButtonDownAppear = appr; }
// 	CHE_WD_Appearance * GetMouseLButtonDownAppear() { return mpMouseLButtonDownAppear; }
// 	void SetMouseRButtonDownAppear( CHE_WD_Appearance * appr ) { mpMouseRButtonDownAppear = appr; }
// 	CHE_WD_Appearance * GetMouseRButtonDownAppear() { return mpMouseRButtonDownAppear; }
// 
// 
// 	//动画过程
// 	void SetDefaultAnimation( CHE_WD_Animation * pAnimation ) { mpDefaultAnimation = pAnimation; }
// 	void SetMouseOverAnimation( CHE_WD_Animation * pAnimation ) { mpMouseOverAnimation = pAnimation; }
// 	void SetMouseLButtonDownAnimation( CHE_WD_Animation * pAnimation ) { mpMouseLButtonDownAnimation = pAnimation; }
// 	void SetMouseRButtonDownAnimation( CHE_WD_Animation * pAnimation ) { mpMouseRButtonDownAnimation = pAnimation; }
// 
// 	//帧执行
// 	void ExecuteFrame();
// 
// 	//事件接口
// };

typedef void (*EventFunction)( CHE_WDM_Area * pArea );

class CHE_WDM_Button : public CHE_WDM_Area
{
public:
	static CHE_WDM_Button * Create( IHE_WDM_InterActive * pInterActive, CHE_Allocator * pAllocator = NULL );

	HE_VOID SetClickEvent( EventFunction eventFunc ) { mClickEventFunc = eventFunc; }

	virtual HE_VOID OnMouseLBUp( HE_INT32 x, HE_INT32 y );

protected:
	CHE_WDM_Button( IHE_WDM_InterActive * pInteractive, CHE_Allocator * pAllocator )
		: CHE_WDM_Area( pInteractive, pAllocator ), mClickEventFunc(NULL) {};

	EventFunction mClickEventFunc;

	friend class CHE_Allocator;
};

// class CHE_WD_MouseEventBtn : public CHE_WD_Button
// {
// public:
// 	CHE_WD_MouseEventBtn( IHE_WDM_InterActive * pInteractive )
// 		: CHE_WD_Button( pInteractive ), mMouseOverEventFunc(NULL), mMouseOutEventFunc(NULL) {};
// 	~CHE_WD_MouseEventBtn() {};
// 
// 	void SetMouseOverEvent( EventFunction eventFunc ) { mMouseOverEventFunc = eventFunc; }
// 	void SetMouseOutEvent( EventFunction eventFunc ) { mMouseOutEventFunc = eventFunc; }
// 
// 	void OnMouseOver()
// 	{
// 		if ( mMouseOverEventFunc )
// 		{
// 			mMouseOverEventFunc( this );
// 		}
// 		CHE_WD_Button::OnMouseOver();
// 	}
// 
// 	void OnMouseOut()
// 	{
// 		if ( mMouseOutEventFunc )
// 		{
// 			mMouseOutEventFunc( this );
// 		}
// 		CHE_WD_Button::OnMouseOut();
// 	}
// 
// protected:
// 	EventFunction mMouseOverEventFunc;
// 	EventFunction mMouseOutEventFunc;
// };

class CHE_WDM_DragArea : public CHE_WDM_Area
{
public:
	static CHE_WDM_DragArea * Create( IHE_WDM_InterActive * pInterActive, CHE_Allocator * pAllocator = NULL );

	HE_VOID OnMouseLBDown( HE_INT32 x, HE_INT32 y );

	HE_VOID OnMouseLBUp( HE_INT32 x, HE_INT32 y );

	HE_VOID SetRange( HE_INT32 left, HE_INT32 top, HE_INT32 right, HE_INT32 bottom );

	HE_VOID OnMouseMove( HE_INT32 x, HE_INT32 y );

	HE_VOID SetDragEvent( EventFunction eventFunc ) { mEventFunc = eventFunc; }

protected:
	CHE_WDM_DragArea( IHE_WDM_InterActive * pInteractive, CHE_Allocator * pAllocator ) : CHE_WDM_Area( pInteractive ),
		mOffsetX( 0 ), mOffsetY( 0 ), mRangeLeft( 0 ), mRangeTop( 0 ), mRangeRight( 0 ),
		mRangeBottom( 0 ), mEventFunc( NULL ) {};

	HE_INT32 mOffsetX;
	HE_INT32 mOffsetY;
	HE_INT32 mRangeLeft;
	HE_INT32 mRangeTop;
	HE_INT32 mRangeRight;
	HE_INT32 mRangeBottom;

	EventFunction mEventFunc;

	friend class CHE_Allocator;
};

#endif