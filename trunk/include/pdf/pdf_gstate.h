#ifndef _CHELIB_PDF_GSTATE_H_
#define _CHELIB_PDF_GSTATE_H_

#include "pdf_colorspace.h"
#include "pdf_extgstate.h"
#include "pdf_font.h"

#include <list>

namespace chelib {

enum GRAPHICS_STATE_LINECAP
{
    LineCap_Butt = 0,
    LineCap_Round = 1,
    LineCap_Square = 2
};

enum GRAPHICS_STATE_LINEJOIN
{
    LineJoin_Miter = 0,
    LineJoin_Round = 1,
    LineJoin_Bevel = 2
};

struct GRAPHICS_STATE_DASHPATTERN
{
    GRAPHICS_STATE_DASHPATTERN()
    {
        dashPhase = 0;
    }
    FLOAT dashPhase;
    std::vector<float> dashArray;
};

enum GRAPHICS_STATE_RENDERINTENTS
{
    RI_AbsoluteColorimetric = 0,
    RI_RelativeColorimetric = 1,
    RI_Saturation = 2,
    RI_Perceptual = 3
};

enum GRAPHICS_STATE_TEXTRENDERMODE
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

enum GRAPHICS_STATE_BLENDMODE
{
    BlendMode_Normal		= 0,
    BlendMode_Multiply		= 1,
    BlendMode_Screen		= 2,
    BlendMode_Overlay		= 3,
    BlendMode_Darken		= 4,
    BlendMode_Lighten		= 5,
    BlendMode_ColorDodge	= 6,
    BlendMode_ColorBurn		= 7,
    BlendMode_HardLight		= 8,
    BlendMode_SoftLight		= 9,
    BlendMode_Difference	= 10,
    BlendMode_Exclusion		= 11,
    BlendMode_Hue           = 12,
    BlendMode_Saturation    = 13,
    BlendMode_Color         = 14,
    BlendMode_Luminosity    = 15
};

enum GRAPHICS_STATE_FILLMODE
{
    FillMode_Nonzero = 0,
    FillMode_EvenOdd = 1
};

class CPDF_ContentObject;

class CPDF_StrokeState : public CObject
{
public:
	CPDF_StrokeState( CAllocator * pAllocator = nullptr )
		: CObject( pAllocator ), mLineCap(LineCap_Butt), mLineJoin(LineJoin_Miter), mLineWidth(1), mMiterLimit(10) {}

	FLOAT                       GetLineWidth() const { return mLineWidth; }
	FLOAT                       GetMiterLimit() const { return mMiterLimit; }
	GRAPHICS_STATE_LINECAP		GetLineCap() const { return mLineCap; }
	GRAPHICS_STATE_LINEJOIN		GetLineJoin() const { return mLineJoin; }
	GRAPHICS_STATE_DASHPATTERN	GetLineDash() const { return mLineDash; }

	void						SetLineWidth( FLOAT lineWidth ) { mLineWidth = lineWidth; }
	void						SetMiterLimit( FLOAT miterLimit ) { mMiterLimit = miterLimit; }
	void						SetLineCap( GRAPHICS_STATE_LINECAP lineCap ) { mLineCap = lineCap; }
	void						SetLineJoin( GRAPHICS_STATE_LINEJOIN lineJion ) { mLineJoin = lineJion; }
	void						SetLineDash( GRAPHICS_STATE_DASHPATTERN dashPattern ) { mLineDash = dashPattern; }

	CPDF_StrokeState *		Clone() const;

private:
	FLOAT                       mLineWidth;
	FLOAT                       mMiterLimit;
	GRAPHICS_STATE_LINECAP		mLineCap;
	GRAPHICS_STATE_LINEJOIN		mLineJoin;
	GRAPHICS_STATE_DASHPATTERN	mLineDash;
};

class CPDF_TextGState : public CObject
{
public:
	CPDF_TextGState( CAllocator * pAllocator = nullptr )
		: CObject(pAllocator), mRMode(TextRenderMode_Fill), mpFont(nullptr), mFontSize(1),
		  mCharSpace(0), mWordSpace(0), mLeading(0), mScaling(100), mRise(0) {}

	CMatrix GetMatrix() const { return mMatrix; }
	FLOAT GetFontSize() const { return mFontSize; }
	CPDF_Font * GetFont() const { return mpFont; }
	FLOAT GetCharSpace() const { return mCharSpace; }
	FLOAT GetWordSpace() const { return mWordSpace; }
	FLOAT GetScaling() const { return mScaling; }
	FLOAT GetRise() const { return mRise; }
	CByteString GetFontResName() const { return mFontName; }
	GRAPHICS_STATE_TEXTRENDERMODE GetRenderMode() const { return mRMode; }

	void SetMatirx( const CMatrix & matrix  ) { mMatrix = matrix; }
	void SetFontSize( FLOAT size ) { mFontSize = size; }
	void SetFont( CPDF_Font * pFont ) { mpFont = pFont; }
	void SetCharSpace( FLOAT charSpace ) { mCharSpace = charSpace; }
	void SetWordSpace( FLOAT wordSpace ) { mWordSpace = wordSpace; }
	void SetScaling( FLOAT scaling ) { mScaling = scaling; }
	void SetRise( FLOAT rise ) { mRise = rise; }
	void SetFontResName( const CByteString & resName ) { mFontName = resName; }
	void SetRenderMode( GRAPHICS_STATE_TEXTRENDERMODE rm ) { mRMode = rm; }

	CPDF_TextGState * Clone() const;

private:
	CMatrix						mMatrix;
	FLOAT                           mFontSize;
	FLOAT                           mCharSpace;
	FLOAT                           mWordSpace;
	FLOAT                           mLeading;
	FLOAT                           mScaling;
	FLOAT                           mRise;
	CByteString					mFontName;
	CPDF_Font *					mpFont;
	GRAPHICS_STATE_TEXTRENDERMODE	mRMode;
};

class CPDF_ClipStateItem : public CObject
{
public:
	CPDF_ClipStateItem( CAllocator * pAllocator = nullptr ) : CObject(pAllocator) {}
	~CPDF_ClipStateItem();

	CPDF_ContentObject * GetElement() const { return mpClipElement; }

	bool SetElement( CPDF_ContentObject * pElement );


	CPDF_ClipStateItem * Clone() const;

private:
	CPDF_ContentObject *		mpClipElement;
};

class CPDF_ClipState : public CObject
{
public:
	CPDF_ClipState( CAllocator * pAllocator = nullptr ) : CObject(pAllocator) {}
	~CPDF_ClipState();

	bool Append( CPDF_ClipStateItem * pElement );

	CPDF_ClipState * Clone() const;

	std::list<CPDF_ClipStateItem*> mClipElementList;

private:
};

class CPDF_ExtGStateStack : public CObject
{
public:
	CPDF_ExtGStateStack( CAllocator * pAllocator = nullptr )
		: CObject(pAllocator), mStrokeAlpha(1), mFillAlpha(1), mBlendMode(BlendMode_Normal) {}

	bool PushExtStateName( const CByteString & name, const CPDF_ExtGStatePtr & extgstate );

	FLOAT GetStrokeAlpha() const { return mStrokeAlpha; }

	FLOAT GetFillAlpha() const { return mFillAlpha; }

	GRAPHICS_STATE_BLENDMODE GetBlendMode() const { return mBlendMode; }
    
    CPDF_DictionaryPtr GetSoftMaskDict () const { return mSMask; }

	CPDF_ExtGStateStack * Clone() const;

	bool operator==( const CPDF_ExtGStateStack & gs ) const;

	bool operator!=( const CPDF_ExtGStateStack & gs ) const;
    
	std::list<CByteString>			mExtGStateName;
	std::list<CPDF_ExtGStatePtr>		mExtGStateStack;

private:
	FLOAT                               mStrokeAlpha;
	FLOAT                               mFillAlpha;
	GRAPHICS_STATE_BLENDMODE			mBlendMode;
    CPDF_DictionaryPtr               mSMask;
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
#define GSTATE_FLAG_BlendMode			0x00100000

class CPDF_GState : public CObject
{
public:
	CPDF_GState( CAllocator * pAllocator = nullptr )
		: CObject(pAllocator), mFlag(0), mRenderIntents(RI_AbsoluteColorimetric), mFlatness(0),
		mpStrokeState(nullptr), mpTextState(nullptr), mpClipState(nullptr), mpExtState(nullptr) {}

	~CPDF_GState();

	CPDF_GState * Clone() const;

	CMatrix GetMatrix() const { return mMatrix; }; 
	GRAPHICS_STATE_RENDERINTENTS GetRenderIntents() const { return mRenderIntents; }
	FLOAT GetFlatness() const { return mFlatness; }

	void GetFillColor( CPDF_Color & colorRet ) const;
	void GetStrokeColor( CPDF_Color & colorRet ) const;
	void GetFillColorSpace( CPDF_ColorSpacePtr & colorSpaceRet ) const;
	void GetStrokeColorSpace( CPDF_ColorSpacePtr & colorSpaceRet ) const;
	void GetLineWidth( FLOAT & lineWidthRet ) const;
	void GetMiterLimit( FLOAT & miterLimitRet ) const;
	void GetLineCap( GRAPHICS_STATE_LINECAP & lineCapRet ) const;
	void GetLineJoin( GRAPHICS_STATE_LINEJOIN & lineJoinRet ) const;
	void GetLineDash( GRAPHICS_STATE_DASHPATTERN & lineDash ) const;
	void GetTextMatrix( CMatrix & matrixRet ) const;
	void GetTextFontSize( FLOAT & fontSizeRet ) const;
	void GetTextCharSpace( FLOAT & charSpaceRet ) const;
	void GetTextWordSpace( FLOAT & wordSpaceRet ) const;
	void GetTextScaling( FLOAT & scalingRet ) const;
	void GetTextRise( FLOAT & riseRet ) const;
	CPDF_Font * GetTextFont() const;
	void GetTextFontResName( CByteString & resNameRet ) const;
	void GetTextRenderMode( GRAPHICS_STATE_TEXTRENDERMODE & rm ) const;
	CPDF_ClipState * GetClipState() const { return mpClipState; }
	CPDF_ExtGStateStack * GetExtGState() const { return mpExtState; }

	void SetMatrix( const CMatrix & matirx );
	void SetRenderIntents( const GRAPHICS_STATE_RENDERINTENTS & ri );
	void SetFlatness( const FLOAT & flatness );
	bool SetFillColor( const CPDF_Color & color );
	bool SetStrokeColor( const CPDF_Color & color );
	bool SetFillColorSpace( CPDF_ColorSpacePtr ColorSpace );
	bool SetStrokeColorSpace( CPDF_ColorSpacePtr ColorSpace );
	void SetLineWidth( const FLOAT & lineWidth );
	void SetMiterLimit( const FLOAT & miterLimit );
	void SetLineCap( const GRAPHICS_STATE_LINECAP & lineCap );
	void SetLineJoin( const GRAPHICS_STATE_LINEJOIN & lineJion );
	void SetLineDash( const GRAPHICS_STATE_DASHPATTERN & dashPattern );
	void SetTextMatrix( const CMatrix & matrix );
	void SetTextFontSize( const FLOAT & size );
	void SetTextFont( CPDF_Font * pFont );
	void SetTextFontResName( const CByteString & resName );
	void SetTextCharSpace( const FLOAT & charSpace );
	void SetTextWordSpace( const FLOAT & wordSpace );
	void SetTextScaling( const FLOAT & scaling );
	void SetTextRise( const FLOAT & rise );
	void SetTextRenderMode( const GRAPHICS_STATE_TEXTRENDERMODE & rm );
	bool PushClipElement( CPDF_ContentObject * pElement );
	bool PushExtGState( const CByteString & resName, const CPDF_ExtGStatePtr & extgstate );

	void	CopyTextState( CPDF_GState * pGState );

private:
	CPDF_StrokeState *			MakeStrokeState();
	CPDF_TextGState *			MakeTextState();
	CPDF_ClipState *				MakeClipState();
	CPDF_ExtGStateStack *		MakeExtGState();

    uint32                          mFlag;
	FLOAT                           mFlatness;
	GRAPHICS_STATE_RENDERINTENTS	mRenderIntents;
	CMatrix						mMatrix;
	CPDF_Color					mFillColor;
	CPDF_Color					mStrokeColor;
	CPDF_ColorSpacePtr			mFillColorSpace;
	CPDF_ColorSpacePtr			mStrokeColorSpace;
	CPDF_StrokeState *			mpStrokeState;
	CPDF_TextGState *			mpTextState;
	CPDF_ClipState *				mpClipState;
	CPDF_ExtGStateStack *		mpExtState;
};


bool IsFloatEqual( const FLOAT &, const FLOAT & );

bool IsDefLineWidth( const FLOAT & lineWidth );
bool IsDefLineCap( const GRAPHICS_STATE_LINECAP & lineCap );
bool IsDefLineJoin( const GRAPHICS_STATE_LINEJOIN & lineJoin );
bool IsDefMiterLimit( const FLOAT & miterLimit );
bool IsDefLineDash( const GRAPHICS_STATE_DASHPATTERN & lineDash );

bool IsDefMatrix( const CMatrix & textMatrix );

bool IsDefTextCharSpace( const FLOAT & charSpace );
bool IsDefTextWrodSpace( const FLOAT & wordSpace );
bool IsDefTextRise( const FLOAT & textRise );
bool IsDefTextScaling( const FLOAT & textScaling );
bool IsDefTextRenderMode( const GRAPHICS_STATE_TEXTRENDERMODE & rm );

bool IsDefFlatness( const FLOAT & flatness );
bool IsDefRenderIntents( const GRAPHICS_STATE_RENDERINTENTS & ri );

bool IsDefColorSpace( const CPDF_ColorSpacePtr & colorSpace );
bool IsDefColor( const CPDF_Color & color );
bool IsColorSpaceEqual( const CPDF_ColorSpacePtr & cs1, const CPDF_ColorSpacePtr & cs2 );
bool IsColorEqual( const CPDF_Color & c1, const CPDF_Color & c2 );

bool IsExtGStateEqual( const CPDF_ExtGStateStack * pExtGS1, const CPDF_ExtGStateStack * pExtGS2 );
bool IsExtGStateContinue( const CPDF_ExtGStateStack * pExtGS1, const CPDF_ExtGStateStack * pExtGS2 );

bool IsClipStateEqual( const CPDF_ClipState * pClipGS1, const CPDF_ClipState * pClipGS2 );
bool IsClipStateContinue( const CPDF_ClipState * pClipGS1, const CPDF_ClipState * pClipGS2 );

}//namespace

#endif