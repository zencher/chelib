#ifndef _CHE_PDF_GRAPHICS_STATE_H_
#define _CHE_PDF_GRAPHICS_STATE_H_

#include "che_pdf_matrix.h"
#include "che_pdf_colorspace.h"
#include "che_pdf_font.h"
#include <list>

class CHE_PDF_ContentObject;

enum PDF_GSTATE_LINECAP
{
	LineCap_Butt = 0,
	LineCap_Round = 1,
	LineCap_Square = 2
};

enum PDF_GSTATE_LINEJOIN
{
	LineJoin_Miter = 0,
	LineJoin_Round = 1,
	LineJoin_Bevel = 2
};

struct PDF_GSTATE_DASHPATTERN
{
	PDF_GSTATE_DASHPATTERN()
	{
		dashPhase = 0;
	}
	HE_FLOAT dashPhase;
	std::vector<float> dashArray;
};

enum PDF_GSTATE_RENDERINTENTS
{
	RI_AbsoluteColorimetric = 0,
	RI_RelativeColorimetric = 1,
	RI_Saturation = 2,
	RI_Perceptual = 3
};

enum PDF_GSTATE_TEXTRENDERMODE
{
	TextRenderMode_Fill = 0,
	TextRenderMode_Stroke = 1,
	TextRenderMode_FillStroke = 2,
	TextRenderMode_Invisible = 3,
	TextRenderMode_FillClip = 4,
	TextRenderMode_StrokeClip = 5,
	TextRenderMode_FillStrokeClip = 6,
	TextRenderMode_Clip = 7
};

class CHE_PDF_StrokeState : public CHE_Object
{
public:
	CHE_PDF_StrokeState( CHE_Allocator * pAllocator = NULL )
		: CHE_Object( pAllocator ), mLineCap(LineCap_Butt), mLineJoin(LineJoin_Miter), mLineWidth(1), mMiterLimit(10) {}

	HE_FLOAT				GetLineWidth() const { return mLineWidth; }
	HE_FLOAT				GetMiterLimit() const { return mMiterLimit; }
	PDF_GSTATE_LINECAP		GetLineCap() const { return mLineCap; }
	PDF_GSTATE_LINEJOIN		GetLineJoin() const { return mLineJoin; }
	PDF_GSTATE_DASHPATTERN	GetLineDash() const { return mLineDash; }

	HE_VOID					SetLineWidth( HE_FLOAT lineWidth ) { mLineWidth = lineWidth; }
	HE_VOID					SetMiterLimit( HE_FLOAT miterLimit ) { mMiterLimit = miterLimit; }
	HE_VOID					SetLineCap( PDF_GSTATE_LINECAP lineCap ) { mLineCap = lineCap; }
	HE_VOID					SetLineJoin( PDF_GSTATE_LINEJOIN lineJion ) { mLineJoin = lineJion; }
	HE_VOID					SetLineDash( PDF_GSTATE_DASHPATTERN dashPattern ) { mLineDash = dashPattern; }

	CHE_PDF_StrokeState *	Clone() const;

private:
	HE_FLOAT				mLineWidth;
	HE_FLOAT				mMiterLimit;
	PDF_GSTATE_LINECAP		mLineCap;
	PDF_GSTATE_LINEJOIN		mLineJoin;
	PDF_GSTATE_DASHPATTERN	mLineDash;
};

class CHE_PDF_TextGState : public CHE_Object
{
public:
	CHE_PDF_TextGState( CHE_Allocator * pAllocator = NULL )
		: CHE_Object(pAllocator), mRMode(TextRenderMode_Fill), mpFont(NULL), mFontSize(1),
		  mCharSpace(0), mWordSpace(0), mLeading(0), mScaling(100), mRise(0) {}

	CHE_PDF_Matrix GetMatrix() const { return mMatrix; }
	HE_FLOAT GetFontSize() const { return mFontSize; }
	CHE_PDF_Font * GetFont() const { return mpFont; }
	HE_FLOAT GetCharSpace() const { return mCharSpace; }
	HE_FLOAT GetWordSpace() const { return mWordSpace; }
	//HE_FLOAT GetLeading() const { return mLeading; }
	HE_FLOAT GetScaling() const { return mScaling; }
	HE_FLOAT GetRise() const { return mRise; }
	CHE_ByteString GetFontResName() const { return mFontName; }
	PDF_GSTATE_TEXTRENDERMODE GetRenderMode() const { return mRMode; }

	HE_VOID SetMatirx( const CHE_PDF_Matrix & matrix  ) { mMatrix = matrix; }
	HE_VOID SetFontSize( HE_FLOAT size ) { mFontSize = size; }
	HE_VOID SetFont( CHE_PDF_Font * pFont ) { mpFont = pFont; }
	HE_VOID SetCharSpace( HE_FLOAT charSpace ) { mCharSpace = charSpace; }
	HE_VOID SetWordSpace( HE_FLOAT wordSpace ) { mWordSpace = wordSpace; }
	//HE_VOID SetLeading( HE_FLOAT leading ) { mLeading = leading; }
	HE_VOID SetScaling( HE_FLOAT scaling ) { mScaling = scaling; }
	HE_VOID SetRise( HE_FLOAT rise ) { mRise = rise; }
	HE_VOID SetFontResName( const CHE_ByteString & resName ) { mFontName = resName; }
	HE_VOID SetRenderMode( PDF_GSTATE_TEXTRENDERMODE rm ) { mRMode = rm; }

	CHE_PDF_TextGState * Clone() const;

private:
	CHE_PDF_Matrix mMatrix;
	HE_FLOAT mFontSize;
	HE_FLOAT mCharSpace;
	HE_FLOAT mWordSpace;
	HE_FLOAT mLeading;
	HE_FLOAT mScaling;
	HE_FLOAT mRise;
	CHE_ByteString mFontName;
	CHE_PDF_Font * mpFont;
	PDF_GSTATE_TEXTRENDERMODE mRMode;
};

class CHE_PDF_ClipStateItem : public CHE_Object
{
public:
	CHE_PDF_ClipStateItem( CHE_Allocator * pAllocator = NULL ) : CHE_Object(pAllocator) {}
	~CHE_PDF_ClipStateItem();

	CHE_PDF_ContentObject * GetElement() const { return mpClipElement; }

	HE_BOOL SetElement( CHE_PDF_ContentObject * pElement );

	CHE_PDF_Matrix GetMatrix() const { return mMatrix; }

	HE_VOID SetMatrix( const CHE_PDF_Matrix & matrix ) { mMatrix = matrix; }

	CHE_PDF_ClipStateItem * Clone() const;

private:
	CHE_PDF_ContentObject * mpClipElement;
	CHE_PDF_Matrix			mMatrix;
};

class CHE_PDF_ClipState : public CHE_Object
{
public:
	CHE_PDF_ClipState( CHE_Allocator * pAllocator = NULL ) : CHE_Object(pAllocator) {}
	~CHE_PDF_ClipState();

	HE_BOOL Append( CHE_PDF_ClipStateItem * pElement );

	CHE_PDF_ClipState * Clone() const;

	std::list<CHE_PDF_ClipStateItem*> mClipElementList;

private:
};

class CHE_PDF_ExtGState : public CHE_Object
{
public:
	CHE_PDF_ExtGState( CHE_Allocator * pAllocator = NULL )
		: CHE_Object(pAllocator), mStrokeAlpha(1), mFillAlpha(1) {}

	HE_BOOL PushExtStateName( const CHE_ByteString & name, CHE_PDF_DictionaryPtr dictPtr );

	HE_FLOAT GetStrokeAlpha() const { return mStrokeAlpha; }

	HE_FLOAT GetFillAlpha() const { return mFillAlpha; }

	CHE_PDF_ExtGState * Clone() const;

	bool operator == ( const CHE_PDF_ExtGState & gs ) const;

	bool operator != ( const CHE_PDF_ExtGState & gs ) const;

	std::list<CHE_ByteString> mExtDictNameList;

private:
	HE_FLOAT mStrokeAlpha;
	HE_FLOAT mFillAlpha;
};

#define GSTATE_FLAG_FillColor			0x00000001
#define GSTATE_FLAG_StrokeColor			0x00000002
#define GSTATE_FLAG_FillColorSpace		0x00000004
#define GSTATE_FLAG_StrokeColorSpace	0x00000008
#define GSTATE_FLAG_LineWidth			0x00000010
#define GSTATE_FLAG_MiterLimit			0x00000020
#define GSTATE_FLAG_LineCap				0x00000040
#define GSTATE_FLAG_LineJoin			0x00000080
#define GSTATE_FLAG_LineDash			0x00000100
#define GSTATE_FLAG_Font				0x00000200
#define GSTATE_FLAG_TextMatirx			0x00000400
#define GSTATE_FLAG_CharSpace			0x00000800
#define GSTATE_FLAG_WordSpace			0x00001000
#define GSTATE_FLAG_Scaling				0x00002000
#define GSTATE_FLAG_Rise				0x00004000
#define GSTATE_FLAG_RenderMode			0x00008000
#define GSTATE_FLAG_FillAlpha			0x00010000
#define GSTATE_FLAG_StrokeAlpha			0x00020000
#define GSTATE_FLAG_RenderIntents		0x00040000
#define GSTATE_FLAG_Flatness			0x00080000

class CHE_PDF_GState : public CHE_Object
{
public:
	CHE_PDF_GState( CHE_Allocator * pAllocator = NULL )
		: CHE_Object(pAllocator), mFlag(0), mRenderIntents(RI_AbsoluteColorimetric), mFlatness(0),
		mpFillColor(NULL), mpStrokeColor(NULL),mpFillColorSpace(NULL), mpStrokeColorSpace(NULL),
		mpStrokeState(NULL), mpTextState(NULL), mpClipState(NULL), mpExtState(NULL) {}

	~CHE_PDF_GState();

	CHE_PDF_GState * Clone() const;

	CHE_PDF_Matrix GetMatrix() const { return mMatrix; }; 
	PDF_GSTATE_RENDERINTENTS GetRenderIntents() const { return mRenderIntents; }
	HE_FLOAT GetFlatness() const { return mFlatness; }

	/*
	*	@breif	获取图形状态的填充颜色
	*	@param	用于接收返回值的颜色对象的引用
	*	@return HE_VOID
	*	@remark	如果该图形状态中没有包含填充颜色的信息，则返回一个当前颜色空间下面的默认颜色
	*/
	HE_VOID	GetFillColor( CHE_PDF_Color & colorRet ) const;
	HE_VOID GetStrokeColor( CHE_PDF_Color & colorRet ) const;
	HE_VOID GetFillColorSpace( CHE_PDF_ColorSpace & colorSpaceRet ) const;
	HE_VOID GetStrokeColorSpace( CHE_PDF_ColorSpace & colorSpaceRet ) const;
	HE_VOID	GetLineWidth( HE_FLOAT & lineWidthRet ) const;
	HE_VOID GetMiterLimit( HE_FLOAT & miterLimitRet ) const;
	HE_VOID GetLineCap( PDF_GSTATE_LINECAP & lineCapRet ) const;
	HE_VOID GetLineJoin( PDF_GSTATE_LINEJOIN & lineJoinRet ) const;
	HE_VOID GetLineDash( PDF_GSTATE_DASHPATTERN & lineDash ) const;
	HE_VOID GetTextMatrix( CHE_PDF_Matrix & matrixRet ) const;
	HE_VOID GetTextFontSize( HE_FLOAT & fontSizeRet ) const;
	HE_VOID GetTextCharSpace( HE_FLOAT & charSpaceRet ) const;
	HE_VOID GetTextWordSpace( HE_FLOAT & wordSpaceRet ) const;
	HE_VOID GetTextScaling( HE_FLOAT & scalingRet ) const;
	HE_VOID GetTextRise( HE_FLOAT & riseRet ) const;
	CHE_PDF_Font * GetTextFont() const;
	HE_VOID GetTextFontResName( CHE_ByteString & resNameRet ) const;
	HE_VOID GetTextRenderMode( PDF_GSTATE_TEXTRENDERMODE & rm ) const;
	CHE_PDF_ClipState * GetClipState() { return mpClipState; }
	CHE_PDF_ExtGState * GetExtGState() { return mpExtState; }

	HE_VOID SetMatrix( const CHE_PDF_Matrix & matirx ) { mMatrix = matirx; }
	HE_VOID SetRenderIntents( const PDF_GSTATE_RENDERINTENTS & ri ) { mFlag &= GSTATE_FLAG_RenderIntents; mRenderIntents = ri; }
	HE_VOID SetFlatness( const HE_FLOAT & flatness ) { mFlag &= GSTATE_FLAG_Flatness; mFlatness = flatness; }
	HE_BOOL SetFillColor( CHE_PDF_Color * pColor );
	HE_BOOL SetStrokeColor( CHE_PDF_Color * pColor );
	HE_BOOL SetFillColorSpace( CHE_PDF_ColorSpace * pColorSpace );
	HE_BOOL SetStrokeColorSpace( CHE_PDF_ColorSpace * pColorSpace );
	HE_VOID	SetLineWidth( const HE_FLOAT & lineWidth );
	HE_VOID	SetMiterLimit( const HE_FLOAT & miterLimit );
	HE_VOID	SetLineCap( const PDF_GSTATE_LINECAP & lineCap );
	HE_VOID	SetLineJoin( const PDF_GSTATE_LINEJOIN & lineJion );
	HE_VOID	SetLineDash( const PDF_GSTATE_DASHPATTERN & dashPattern );
	HE_VOID SetTextMatrix( const CHE_PDF_Matrix & matrix );
	HE_VOID SetTextFontSize( const HE_FLOAT & size );
	HE_VOID SetTextFont( CHE_PDF_Font * pFont );
	HE_VOID SetTextFontResName( const CHE_ByteString & resName );
	HE_VOID SetTextCharSpace( const HE_FLOAT & charSpace );
	HE_VOID SetTextWordSpace( const HE_FLOAT & wordSpace );
	//HE_VOID SetTextLeading( const HE_FLOAT & leading );
	HE_VOID SetTextScaling( const HE_FLOAT & scaling );
	HE_VOID SetTextRise( const HE_FLOAT & rise );
	HE_VOID SetTextRenderMode( const PDF_GSTATE_TEXTRENDERMODE & rm );
	HE_BOOL PushClipElement( CHE_PDF_ContentObject * pElement );
	HE_BOOL PushExtGState( const CHE_ByteString & resName, CHE_PDF_DictionaryPtr dictPtr );

private:
	CHE_PDF_StrokeState *		MakeStrokeState();
	CHE_PDF_TextGState *		MakeTextState();
	CHE_PDF_ClipState *			MakeClipState();
	CHE_PDF_ExtGState *			MakeExtGState();

	HE_UINT32					mFlag;
	HE_FLOAT					mFlatness;
	PDF_GSTATE_RENDERINTENTS	mRenderIntents;
	CHE_PDF_Matrix				mMatrix;
	CHE_PDF_Color *				mpFillColor;
	CHE_PDF_Color *				mpStrokeColor;
	CHE_PDF_ColorSpace *		mpFillColorSpace;
	CHE_PDF_ColorSpace *		mpStrokeColorSpace;
	CHE_PDF_StrokeState *		mpStrokeState;
	CHE_PDF_TextGState *		mpTextState;
	CHE_PDF_ClipState *			mpClipState;
	CHE_PDF_ExtGState *			mpExtState;
};

HE_BOOL	IsFloatEqual( const HE_FLOAT &, const HE_FLOAT & );

HE_BOOL IsDefLineWidth( const HE_FLOAT & lineWidth );
HE_BOOL IsDefLineCap( const PDF_GSTATE_LINECAP & lineCap );
HE_BOOL IsDefLineJoin( const PDF_GSTATE_LINEJOIN & lineJoin );
HE_BOOL IsDefMiterLimit( const HE_FLOAT & miterLimit );
HE_BOOL IsDefLineDash( const PDF_GSTATE_DASHPATTERN & lineDash );

HE_BOOL IsDefMatrix( const CHE_PDF_Matrix & textMatrix );

HE_BOOL IsDefTextCharSpace( const HE_FLOAT & charSpace );
HE_BOOL IsDefTextWrodSpace( const HE_FLOAT & wordSpace );
HE_BOOL IsDefTextRise( const HE_FLOAT & textRise );
HE_BOOL IsDefTextScaling( const HE_FLOAT & textScaling );
HE_BOOL IsDefTextRenderMode( const PDF_GSTATE_TEXTRENDERMODE & rm );

HE_BOOL IsDefFlatness( const HE_FLOAT & flatness );
HE_BOOL IsDefRenderIntents( const PDF_GSTATE_RENDERINTENTS & ri );

HE_BOOL IsDefColorSpace( const CHE_PDF_ColorSpace & colorSpace );
HE_BOOL IsDefColor( const CHE_PDF_Color & color );

HE_BOOL	IsColorSpaceEqual( const CHE_PDF_ColorSpace & cs1, const CHE_PDF_ColorSpace & cs2 );
HE_BOOL IsColorEqual( const CHE_PDF_Color & c1, const CHE_PDF_Color & c2 );

HE_BOOL IsExtGStateEqual( const CHE_PDF_ExtGState * pExtGS1, const CHE_PDF_ExtGState * pExtGS2 );
HE_BOOL IsExtGStateContinue( const CHE_PDF_ExtGState * pExtGS1, const CHE_PDF_ExtGState * pExtGS2 );

HE_BOOL IsClipStateEqual( const CHE_PDF_ClipState * pClipGS1, const CHE_PDF_ClipState * pClipGS2 );
HE_BOOL IsClipStateContinue( const CHE_PDF_ClipState * pClipGS1, const CHE_PDF_ClipState * pClipGS2 );

#endif