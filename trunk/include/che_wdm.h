#ifndef _CHE_WDM_H_
#define _CHE_WDM_H_

#include <vector>
#include <list>

#include "../che_base.h"
#include "../che_string.h"

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

	void								SetVertLayout( const WDM_Layout_Type & layout ) { mVertLayout = layout; }
	WDM_Layout_Type						GetVertLayout() const { return mVertLayout; }
	
	void								SetHoriLayout( const WDM_Layout_Type & layout ) { mHoriLayout = layout; }
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

	operator bool() const { return mpItem ? TRUE : FALSE ; }

	inline CHE_WDM_AppearItem *	operator->() const { return mpItem; }

	void	reset( CHE_WDM_AppearItem * pItem = NULL );

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

	void								SetAlpha( FLOAT alpha ) { mAlpha = alpha; }
	FLOAT							GetAlpha() const { return mAlpha; }
	
	void								SetScaleX( FLOAT scaleX ) { mScaleX = scaleX; }
	FLOAT							GetScaleX() const { return mScaleX; }
	
	void								SetScaleY( FLOAT scaleY ) { mScaleY = scaleY; }
	FLOAT							GetScaleY() const { return mScaleY; }
	
	void								SetPosiX( int32 x ) { mPosiX = x; }
	int32							GetPosiX() const { return mPosiX; }
	
	void								SetPosiY( int32 y ) { mPosiY = y; }
	int32							GetPosiY() const { return mPosiY; }
	
	void								SetExtData( void * pExtData ) { mExtData = pExtData; }
	void *							GetExtData() const { return mExtData; }

protected:
	CHE_WDM_AppearItem( CHE_Allocator * pAllocator = NULL )
		: CHE_Object( pAllocator ), mAlpha( 1 ), mScaleX( 1 ), mScaleY( 1 ), mPosiX( 0 ), mPosiY( 0 ), mExtData( NULL ) {}

	virtual ~CHE_WDM_AppearItem() {}

	FLOAT							mAlpha;
	FLOAT							mScaleX;
	FLOAT							mScaleY;
	int32							mPosiX;
	int32							mPosiY;
	void *							mExtData;

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

	WDM_APPEAR_ITEM_TYPE				GetType() { return APPEAR_ITEM_IMAGE; }

	void								SetStyle( WDM_APPEAR_IMAGE_STYLE style ) { mStyle = style; }
	WDM_APPEAR_IMAGE_STYLE				GetStyle() { return mStyle; }

	void								SetImageFile( PWSTR pFile );
	PWSTR							GetImageFile() { return mFile; }

protected:
	CHE_WDM_AppearImage( CHE_Allocator * pAllocator = NULL )
		: CHE_WDM_AppearItem( pAllocator ), mFile( NULL ), mStyle( APPEAR_IMAGE_STYLE_SINGLE ) {}

	~CHE_WDM_AppearImage();

	WDM_APPEAR_IMAGE_STYLE				mStyle;
	PWSTR							mFile;

	friend class CHE_Allocator;
	friend class CHE_WDM_AppearImagePtr;
};

class CHE_WDM_AppearTextPtr : public CHE_WDM_AppearItemPtr
{
public:
	CHE_WDM_AppearText * operator->() const;
};

class CHE_WDM_AppearText : public CHE_WDM_AppearItem
{
public:
	static CHE_WDM_AppearTextPtr		Create( CHE_Allocator * pAllocator = NULL );

	WDM_APPEAR_ITEM_TYPE				GetType() { return APPEAR_ITEM_TEXT; }

	void								SetText( PCWSTR text );
	PCWSTR							GetText() const { return mText; }

	void								SetColor( size_t val ) { mColor = val; }
	size_t							GetColor() const { return mColor; }

	void								SetFontFile( PCWSTR file );
	PWSTR							GetFontFile() const { return mFontFile; }

	void								SetLayout( CHE_WDM_Layout layout ) { mLayout = layout; }
	CHE_WDM_Layout						GetLayout() const { return mLayout; }

	void								SetSize( size_t size ) { mSize = size; }
	size_t							GetSize() const { return mSize; }

	void								SetWidth( size_t width ) { mWidth = width; }
	size_t							GetWidth() const { return mWidth; }

	void								SetHeight( size_t height ) { mHeight = height; }
	size_t							GetHeight() const { return mHeight; }

protected:
	CHE_WDM_AppearText( CHE_Allocator * pAllocator = NULL )
		: CHE_WDM_AppearItem( pAllocator ), mText( NULL ), mFontFile( NULL ), mSize( 1 ), mColor( 0xFF000000 ) {}

	~CHE_WDM_AppearText();

	PWSTR							mText;
	PWSTR							mFontFile;
	size_t							mSize;
	size_t							mColor;
	CHE_WDM_Layout						mLayout;
 	size_t							mWidth;
 	size_t							mHeight;

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

	void								SetType( WDM_APPEAR_PATH_TYPE type ) { mType = type; }
	WDM_APPEAR_PATH_TYPE				GetType() const { return mType; }

	void								SetValue( float val ) { mValue = val; }
	FLOAT							GetValue() const { return mValue; }

private:
	union
	{
		WDM_APPEAR_PATH_TYPE			mType;
		FLOAT						mValue;
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

	void								AddLine( FLOAT xStart, FLOAT yStart, FLOAT xEnd, FLOAT yEnd );
	void								AddRect( FLOAT left, FLOAT top, FLOAT width, FLOAT hight );
	void								AddCurve( FLOAT x0, FLOAT y0, FLOAT x1, FLOAT y1, FLOAT x2, FLOAT y2 );

	size_t							GetItemCount();
	bool								GetItem( size_t index, CHE_WDM_AppearPathItem & itemRet );

	void								SetLineWidth( FLOAT width ) { mLineWidth = width; }
	FLOAT							GetLineWidth() const { return mLineWidth; }

	void								SetFillColor(ARGB color) { mFillColor = color; }
	ARGB							GetFillColor() const { return mFillColor; }

	void								SetStrokeColor(ARGB color) { mStrokeColor = color; }
	ARGB							GetStrokeColor() const { return mStrokeColor; }

	void								SetOperator( WDM_APPEAR_PATH_OPERATOR opt ) { mOperator = opt; }
	WDM_APPEAR_PATH_OPERATOR			GetOperator() const { return mOperator; }

	void								SetFillMode( WDM_APPEAR_PATH_FILL_MODE fillMode ) { mFillMode = fillMode; }
	WDM_APPEAR_PATH_FILL_MODE			GetFillMode() const { return mFillMode; }

private:
	CHE_WDM_AppearPath( CHE_Allocator * pAllocator = NULL )
		: CHE_WDM_AppearItem( pAllocator ), mLineWidth( 1 ), mFillColor( 0xFF000000 ), mStrokeColor( 0xFF000000 ), 
		mOperator( APPEAR_PATH_FILL_STROKE ), mFillMode( APPEAR_PATH_FILL_NONZERO ) {};

	~CHE_WDM_AppearPath() {}

	FLOAT							mLineWidth;
	ARGB							mFillColor;
	ARGB							mStrokeColor;
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
	virtual void						Invalidate() = 0;
	virtual void						InvalidateRect( int32 left, int32 top, int32 right, int32 bottom ) = 0;

	virtual void						SetClip( CHE_WDM_Area * pArea ) = 0;
	virtual void						SetClip( int32 left, int32 top, int32 right, int32 bottom ) = 0;
	virtual void						ResetClip() = 0;

	virtual void						Draw( CHE_WDM_Area * pArea, WDM_AREA_APPEAR_TYPE ) = 0;
	virtual void						Draw( CHE_WDM_Area * pArea, CHE_WDM_AppearItemPtr appearPtr ) = 0;

	virtual void						SetTimer( CHE_WDM_Area * pArea, size_t elapse ) = 0;
	virtual void						KillTimer( CHE_WDM_Area * pArea ) = 0;

	virtual bool						MeasureString( CHE_WDM_AppearTextPtr ptr, size_t & width, size_t & height ) = 0;
	virtual bool						MeasureChars( CHE_WDM_AppearTextPtr ptr, size_t count, size_t & width, size_t & height ) = 0;
	virtual FLOAT					GetFontHeight( CHE_WDM_AppearTextPtr ptr ) = 0;
};


// #define KEYCODE_0		0
// #define KEYCODE_1		1
// #define KEYCODE_2		2
// #define KEYCODE_3		3
// #define KEYCODE_4		4
// #define KEYCODE_5		5
// #define KEYCODE_6		6
// #define KEYCODE_7		7
// #define KEYCODE_8		8
// #define KEYCODE_9		9
#define KEYCODE_ADD				10	//表示数字键盘上的“ + ”键。
#define KEYCODE_ALT				11	//表示键盘上 “ ALT ”键。
#define KEYCODE_BACK_QUOTE		12	//表示键盘上 “ ` ”
#define KEYCODE_BACK_SLASH		13	//表示键盘上 “ \ ”键。 
#define KEYCODE_BACK_SPACE		14	//表示键盘上 “ BACKSPACE “键。 
#define KEYCODE_CAPS_LOCK		15	//表示键盘上 “ CAPS LOCK ”键。 
#define KEYCODE_CLOSE_BRACKET	16	//表示键盘上 “ ] ”键。 
#define KEYCODE_COMMAN			17	//表示键盘上 “ , ”键。 
#define KEYCODE_CONTROL			18	//表示键盘上 “ CTRL ”键。 
#define KEYCODE_DECIMAL			19	//表示数字键盘上 “ . ”键。 
#define KEYCODE_DELETE			20	//表示键盘上 “ DELETE ”键。 
#define KEYCODE_DIVIDE			21	//表示键盘上 “ / ”键。 
#define KEYCODE_DOWN			22	//表示键盘上 “向下箭头”键。 
#define KEYCODE_END				23	//表示键盘上 “ END ”键。 
#define KEYCODE_ENTER			24	//表示键盘上 “ ENTER ”键。 
#define KEYCODE_EQUALS			25	//表示键盘上 “＝”键。 
#define KEYCODE_ESCAPE			26	//表示键盘上 “ ESC ”键。 
#define KEYCODE_F1				27				
#define KEYCODE_F2				28
#define KEYCODE_F3				29
#define KEYCODE_F4				30
#define KEYCODE_F5				31
#define KEYCODE_F6				32
#define KEYCODE_F7				33
#define KEYCODE_F8				34
#define KEYCODE_F9				35
#define KEYCODE_F10				36
#define KEYCODE_F11				37
#define KEYCODE_F12				38
#define KEYCODE_HOME			39	//表示键盘上 “ HOME ”键。 
#define KEYCODE_INSERT			40	//表示键盘上 “ INSTERT ”键。 
#define KEYCODE_LEFT			41	//表示键盘上 “向左箭头”键。 
#define KEYCODE_MULTIPLY		42	//表示键盘上 “ * ”键。 
//#define KEYCODE_NUMPAD0 到 #define KEYCODE_NUMPAD9     表示数字键盘上 “ 0 ”到“ 9 ”键。 
#define KEYCODE_OPEN_BRACKET	52	//表示键盘上 “ [ ”键。 
#define KEYCODE_PAGE_DOWN		53	//表示键盘上 “ PAGE DOWN ”键。 
#define KEYCODE_PAGE_UP			54	//表示键盘上 “ PAGE UP ”键。 
#define KEYCODE_PAUSE			55	//表示键盘上 “ PAUSE ”键。 
#define KEYCODE_PRINTSCREEN		56	//表示键盘上 “ PRINT SCREEN ”键。 
#define KEYCODE_RIGHT			57	//表示键盘上 “向右箭头”键。 
#define KEYCODE_SCROLL_LOCK		58	//表示键盘上 “ SCROLL LOCK ”键。 
#define KEYCODE_SEMICOLON		59	//表示键盘上 “；”键。 
#define KEYCODE_SHIFT			60	//表示键盘上 “ SHIFT ”键。 
#define KEYCODE_SPACE			61	//表示键盘上 “ SPACEBAR ”键。 
#define KEYCODE_SUBTRACT		62	//表示键盘上 “－”键。 
#define KEYCODE_TAB				63	//表示键盘上 “ TAB ”键。 
#define KEYCODE_UP				64	//表示键盘上 “向上箭头”键。


class CHE_WDM_Area : public CHE_Object
{
public:
	static CHE_WDM_Area*				Create( IHE_WDM_InterActive * pInterActive, CHE_Allocator * pAllocator = NULL );

	virtual	~CHE_WDM_Area();

	void								SetName( const CHE_ByteString & name ) { mName = name; }
	CHE_ByteString &					GetName() { return mName; }

	void								SetWidth( int32 width );
	int32							GetWidth() const { return mWidth; }

	void								SetHeight( int32 height );
	int32							GetHeight() const {return mHeight; }

	void								SetPosiX( int32 x );
	void								SetPosiY( int32 y );
	int32							GetPosiX() const { return mPosX; }
	int32							GetPosiY() const { return mPosY; }

	void								SetLayout( const CHE_WDM_Layout & layout ) { mLayout = layout; }
	CHE_WDM_Layout						GetLayout() const { return mLayout; }

	bool								IsMouseOver() const { return mbMO; }
	bool								IsMouseLBDown() const { return mbLBD; }
	bool								IsMouseRBDown() const { return mbRBD; }

	void								EnableClip() { mbClip = TRUE; }
	void								DisableClip() { mbClip = FALSE; }
	bool								IsClipEnable() const { return mbClip; }

	void								SetVisable( bool bVisable ) { mbVisable = bVisable; };
	bool								IsVisable() const { return mbVisable; }

	void								SetEnable( bool bEnable );
	bool								IsEnable() const { return mbEnable; }

	void								Refresh();

	CHE_WDM_Appearance &				GetAppear() { return mAppearance; }
	void								AppendAppearItem( const CHE_WDM_AppearItemPtr & ptr, WDM_AREA_APPEAR_TYPE type );

	size_t							GetChildrenCount() const;

	void								AppendChild( CHE_WDM_Area * pChlid );
	CHE_WDM_Area *						PopChild( size_t index );
	CHE_WDM_Area *						GetChild( size_t index ) const;
	void								ClearChild();

	bool								ChildToLower( size_t index );
	bool								ChildToUpper( size_t index );

	virtual bool						OnMouseMove( int32 x, int32 y );
	virtual bool						OnMouseOver();
	virtual bool						OnMouseOut();
	virtual bool						OnMouseLBDown( int32 x, int32 y );
	virtual bool						OnMouseLBUp( int32 x, int32 y );
	virtual bool						OnMouseRBDown( int32 x, int32 y );
	virtual bool						OnMouseRBUp( int32 x, int32 y );
	virtual bool						OnMouseLDBClick( int32 x, int32 y );
	virtual bool						OnMouseRDBClick( int32 x, int32 y );
	virtual bool						OnKeyDown( int32 /*keyCode*/ ) { return FALSE; }
	virtual bool						OnKeyUp( int32 /*keyCode*/ ) { return FALSE; }
	virtual bool						OnChar( WCHAR /*charCode*/ ) { return FALSE; }
	virtual bool						OnTimer() { return FALSE; }
	virtual bool						OnDraw();
	virtual bool						OnDraw( int32 left, int32 top, int32 right, int32 bottom );

	void								SetFocus( CHE_WDM_Area * pChild );
	void								ReleaseFocus();

	void								SetCapture( CHE_WDM_Area * pChild );
	void								ReleaseCapture();

	CHE_WDM_Area *						GetParent() const { return mParent; }

protected:
	CHE_WDM_Area( IHE_WDM_InterActive * pInteractive, CHE_Allocator * pAllocator = NULL );

	void								SetParent( CHE_WDM_Area * pParent ) { mParent = pParent; }

	void								SetInterActive( IHE_WDM_InterActive * pInterActive ) { mInterActive = pInterActive; }
	IHE_WDM_InterActive *				GetInterActive() { return mInterActive; }

private:
	bool								mbLBD;
	bool								mbRBD;
	bool								mbMO;

	bool								mbClip;
	bool								mbVisable;
	bool								mbEnable;

	size_t							mWidth;
	size_t							mHeight;
	int32							mPosX;
	int32							mPosY;

	CHE_WDM_Layout						mLayout;
	CHE_WDM_Area *						mParent;
	IHE_WDM_InterActive *				mInterActive;
	CHE_WDM_Appearance					mAppearance;
	std::vector<CHE_WDM_Area*>			mChildren;
	CHE_WDM_Area *						mpCaptureChild;
	CHE_WDM_Area *						mpMouseOverArea;

	CHE_ByteString						mName;

	friend class CHE_Allocator;
};

class CHE_WDM_AnimationData
{
public:
	CHE_WDM_AnimationData() : mAlpha(1), mScaleX(1), mScaleY(1), mPosiX(0), mPosiY(0) {}

	FLOAT							mAlpha;
	FLOAT							mScaleX;
	FLOAT							mScaleY;
	int32							mPosiX;
	int32							mPosiY;
};

typedef void (*Function)();

class CHE_WDM_AreaAnimation
{
public:
	CHE_WDM_AreaAnimation()
		: mbLoop( FALSE ), mFramesCount( 0 ), mCurFrame( 0 ), mIndex( 0 ), mFramesToGo( 0 ), mpArea( NULL ), mpOverFunction( NULL ) {};

	~CHE_WDM_AreaAnimation() {};

	bool								SetTarget( CHE_WDM_Area * pArea );
	void								SetState( const CHE_WDM_AnimationData & state ) { mState = state; }
	void								SetLoop( bool bLoop ) { mbLoop = bLoop; }
	void								SetOverFunction( Function pFunction ) { mpOverFunction = pFunction; }
	void								CallFunction();
	void								InsertFrames( size_t frames, const CHE_WDM_AnimationData & state );

	void								Init();
	void								Execute();

	bool								IsOver();
	bool								IsLoop() const { return mbLoop; }

protected:
	bool								mbLoop;

	size_t							mFramesCount;
	size_t							mCurFrame;
	size_t							mIndex;
	size_t							mFramesToGo;

	CHE_WDM_Area	*					mpArea;
	CHE_WDM_AnimationData				mState;
	CHE_WDM_AnimationData				mStateOffset;
	Function							mpOverFunction;

	std::vector<CHE_WDM_AnimationData>	mAnimations;
	std::vector<size_t>				mAnimationFrames;
};

class CHE_WDM_AppearAnimation : public CHE_WDM_AreaAnimation
{
public:
	void							SetAppear( const CHE_WDM_AppearItemPtr & appearPtr ) { mAppearPtr = appearPtr; }

	void							Init();
	void							Execute();

protected:
	CHE_WDM_AppearItemPtr			mAppearPtr;
};

class CHE_WDM_AnimationMgr
{
public:
	void Execute();

	void	StartAreaAnimation( const CHE_WDM_AreaAnimation & animation );

	void	StartAppearAnimation( const CHE_WDM_AppearAnimation & animation );

public:
	std::vector<CHE_WDM_AreaAnimation>		mAreaAnimations;
	std::vector<CHE_WDM_AppearAnimation>	mAppearAnimations;
};

typedef void (*EventFunction)( CHE_WDM_Area * pArea );

class CHE_WDM_Button : public CHE_WDM_Area
{
public:
	static CHE_WDM_Button *			Create( IHE_WDM_InterActive * pInterActive, CHE_Allocator * pAllocator = NULL );

	void							SetMouseLBDEvent( EventFunction eventFunc ) { mLBDEventFunc = eventFunc; }

	void							SetMouseLBUEvent( EventFunction eventFunc ) { mLBUEventFunc = eventFunc; }

	void							SetDBClickEvent( EventFunction eventFunc ) { mDBClickEventFunc = eventFunc; }

	virtual bool					OnMouseLBDown( int32 x, int32 y );

	virtual bool					OnMouseLBUp( int32 x, int32 y );

	virtual bool					OnMouseLDBClick( int32 x, int32 y );

protected:
	CHE_WDM_Button( IHE_WDM_InterActive * pInteractive, CHE_Allocator * pAllocator )
		: CHE_WDM_Area( pInteractive, pAllocator ), mLBDEventFunc(NULL), mLBUEventFunc(NULL), mDBClickEventFunc(NULL) {};

	EventFunction mLBDEventFunc;
	EventFunction mLBUEventFunc;
	EventFunction mDBClickEventFunc;

	friend class CHE_Allocator;
};

class CHE_WDM_MouseEventBtn : public CHE_WDM_Button
{
public:
	static CHE_WDM_MouseEventBtn *	Create( IHE_WDM_InterActive * pInterActive, CHE_Allocator * pAllocator = NULL );

	~CHE_WDM_MouseEventBtn() {};

	void							SetMouseOverEvent( EventFunction eventFunc ) { mMouseOverEventFunc = eventFunc; }
	void							SetMouseOutEvent( EventFunction eventFunc ) { mMouseOutEventFunc = eventFunc; }

	virtual bool					OnMouseOver();
	virtual bool					OnMouseOut();

protected:
	CHE_WDM_MouseEventBtn( IHE_WDM_InterActive * pInteractive, CHE_Allocator * pAllocator )
		: CHE_WDM_Button( pInteractive, pAllocator ), mMouseOverEventFunc(NULL), mMouseOutEventFunc(NULL) {};

	EventFunction mMouseOverEventFunc;
	EventFunction mMouseOutEventFunc;

	friend class CHE_Allocator;
};

class CHE_WDM_DragArea : public CHE_WDM_Area
{
public:
	static CHE_WDM_DragArea *		Create( IHE_WDM_InterActive * pInterActive, CHE_Allocator * pAllocator = NULL );

	virtual bool					OnMouseLBDown( int32 x, int32 y );
	virtual bool					OnMouseLBUp( int32 x, int32 y );
	virtual bool					OnMouseMove( int32 x, int32 y );

	void							SetRange( int32 left, int32 top, int32 right, int32 bottom );
	void							SetDragEvent( EventFunction eventFunc ) { mEventFunc = eventFunc; }

protected:
	CHE_WDM_DragArea( IHE_WDM_InterActive * pInteractive, CHE_Allocator * pAllocator )
		: CHE_WDM_Area( pInteractive ), mOffsetX( 0 ), mOffsetY( 0 ), mRangeLeft( 0 ), 
		mRangeTop( 0 ), mRangeRight( 0 ), mRangeBottom( 0 ), mEventFunc( NULL ) {};

	int32 mOffsetX;
	int32 mOffsetY;
	int32 mRangeLeft;
	int32 mRangeTop;
	int32 mRangeRight;
	int32 mRangeBottom;

	EventFunction mEventFunc;

	friend class CHE_Allocator;
};

#endif