#ifndef _CHE_PDF_GSTATE_H_
#define _CHE_PDF_GSTATE_H_

#include "../che_graphics.h"
#include "che_pdf_colorspace.h"
#include "che_pdf_extgstate.h"
#include "che_pdf_font.h"
#include <list>

class CHE_PDF_ContentObject;

class CHE_PDF_StrokeState : public CHE_Object
{
public:
	CHE_PDF_StrokeState( CHE_Allocator * pAllocator = nullptr )
		: CHE_Object( pAllocator ), mLineCap(LineCap_Butt), mLineJoin(LineJoin_Miter), mLineWidth(1), mMiterLimit(10) {}

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

	CHE_PDF_StrokeState *		Clone() const;

private:
	FLOAT                       mLineWidth;
	FLOAT                       mMiterLimit;
	GRAPHICS_STATE_LINECAP		mLineCap;
	GRAPHICS_STATE_LINEJOIN		mLineJoin;
	GRAPHICS_STATE_DASHPATTERN	mLineDash;
};

class CHE_PDF_TextGState : public CHE_Object
{
public:
	CHE_PDF_TextGState( CHE_Allocator * pAllocator = nullptr )
		: CHE_Object(pAllocator), mRMode(TextRenderMode_Fill), mpFont(nullptr), mFontSize(1),
		  mCharSpace(0), mWordSpace(0), mLeading(0), mScaling(100), mRise(0) {}

	CHE_Matrix GetMatrix() const { return mMatrix; }
	FLOAT GetFontSize() const { return mFontSize; }
	CHE_PDF_Font * GetFont() const { return mpFont; }
	FLOAT GetCharSpace() const { return mCharSpace; }
	FLOAT GetWordSpace() const { return mWordSpace; }
	FLOAT GetScaling() const { return mScaling; }
	FLOAT GetRise() const { return mRise; }
	CHE_ByteString GetFontResName() const { return mFontName; }
	GRAPHICS_STATE_TEXTRENDERMODE GetRenderMode() const { return mRMode; }

	void SetMatirx( const CHE_Matrix & matrix  ) { mMatrix = matrix; }
	void SetFontSize( FLOAT size ) { mFontSize = size; }
	void SetFont( CHE_PDF_Font * pFont ) { mpFont = pFont; }
	void SetCharSpace( FLOAT charSpace ) { mCharSpace = charSpace; }
	void SetWordSpace( FLOAT wordSpace ) { mWordSpace = wordSpace; }
	void SetScaling( FLOAT scaling ) { mScaling = scaling; }
	void SetRise( FLOAT rise ) { mRise = rise; }
	void SetFontResName( const CHE_ByteString & resName ) { mFontName = resName; }
	void SetRenderMode( GRAPHICS_STATE_TEXTRENDERMODE rm ) { mRMode = rm; }

	CHE_PDF_TextGState * Clone() const;

private:
	CHE_Matrix						mMatrix;
	FLOAT                           mFontSize;
	FLOAT                           mCharSpace;
	FLOAT                           mWordSpace;
	FLOAT                           mLeading;
	FLOAT                           mScaling;
	FLOAT                           mRise;
	CHE_ByteString					mFontName;
	CHE_PDF_Font *					mpFont;
	GRAPHICS_STATE_TEXTRENDERMODE	mRMode;
};

class CHE_PDF_ClipStateItem : public CHE_Object
{
public:
	CHE_PDF_ClipStateItem( CHE_Allocator * pAllocator = nullptr ) : CHE_Object(pAllocator) {}
	~CHE_PDF_ClipStateItem();

	CHE_PDF_ContentObject * GetElement() const { return mpClipElement; }

	bool SetElement( CHE_PDF_ContentObject * pElement );


	CHE_PDF_ClipStateItem * Clone() const;

private:
	CHE_PDF_ContentObject *		mpClipElement;
};

class CHE_PDF_ClipState : public CHE_Object
{
public:
	CHE_PDF_ClipState( CHE_Allocator * pAllocator = nullptr ) : CHE_Object(pAllocator) {}
	~CHE_PDF_ClipState();

	bool Append( CHE_PDF_ClipStateItem * pElement );

	CHE_PDF_ClipState * Clone() const;

	std::list<CHE_PDF_ClipStateItem*> mClipElementList;

private:
};

class CHE_PDF_ExtGStateStack : public CHE_Object
{
public:
	CHE_PDF_ExtGStateStack( CHE_Allocator * pAllocator = nullptr )
		: CHE_Object(pAllocator), mStrokeAlpha(1), mFillAlpha(1), mBlendMode(BlendMode_Normal) {}

	bool PushExtStateName( const CHE_ByteString & name, const CHE_PDF_ExtGStatePtr & extgstate );

	FLOAT GetStrokeAlpha() const { return mStrokeAlpha; }

	FLOAT GetFillAlpha() const { return mFillAlpha; }

	GRAPHICS_STATE_BLENDMODE GetBlendMode() const { return mBlendMode; }
    
    CHE_PDF_DictionaryPtr GetSoftMaskDict () const { return mSMask; }

	CHE_PDF_ExtGStateStack * Clone() const;

	bool operator==( const CHE_PDF_ExtGStateStack & gs ) const;

	bool operator!=( const CHE_PDF_ExtGStateStack & gs ) const;

	std::list<CHE_ByteString>			mExtGStateName;
	std::list<CHE_PDF_ExtGStatePtr>		mExtGStateStack;

private:
	FLOAT                               mStrokeAlpha;
	FLOAT                               mFillAlpha;
	GRAPHICS_STATE_BLENDMODE			mBlendMode;
    CHE_PDF_DictionaryPtr               mSMask;
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

class CHE_PDF_GState : public CHE_Object
{
public:
	CHE_PDF_GState( CHE_Allocator * pAllocator = nullptr )
		: CHE_Object(pAllocator), mFlag(0), mRenderIntents(RI_AbsoluteColorimetric), mFlatness(0),
		mpStrokeState(nullptr), mpTextState(nullptr), mpClipState(nullptr), mpExtState(nullptr) {}

	~CHE_PDF_GState();

	CHE_PDF_GState * Clone() const;

	CHE_Matrix GetMatrix() const { return mMatrix; }; 
	GRAPHICS_STATE_RENDERINTENTS GetRenderIntents() const { return mRenderIntents; }
	FLOAT GetFlatness() const { return mFlatness; }

	void GetFillColor( CHE_PDF_Color & colorRet ) const;
	void GetStrokeColor( CHE_PDF_Color & colorRet ) const;
	void GetFillColorSpace( CHE_PDF_ColorSpacePtr & colorSpaceRet ) const;
	void GetStrokeColorSpace( CHE_PDF_ColorSpacePtr & colorSpaceRet ) const;
	void GetLineWidth( FLOAT & lineWidthRet ) const;
	void GetMiterLimit( FLOAT & miterLimitRet ) const;
	void GetLineCap( GRAPHICS_STATE_LINECAP & lineCapRet ) const;
	void GetLineJoin( GRAPHICS_STATE_LINEJOIN & lineJoinRet ) const;
	void GetLineDash( GRAPHICS_STATE_DASHPATTERN & lineDash ) const;
	void GetTextMatrix( CHE_Matrix & matrixRet ) const;
	void GetTextFontSize( FLOAT & fontSizeRet ) const;
	void GetTextCharSpace( FLOAT & charSpaceRet ) const;
	void GetTextWordSpace( FLOAT & wordSpaceRet ) const;
	void GetTextScaling( FLOAT & scalingRet ) const;
	void GetTextRise( FLOAT & riseRet ) const;
	CHE_PDF_Font * GetTextFont() const;
	void GetTextFontResName( CHE_ByteString & resNameRet ) const;
	void GetTextRenderMode( GRAPHICS_STATE_TEXTRENDERMODE & rm ) const;
	CHE_PDF_ClipState * GetClipState() const { return mpClipState; }
	CHE_PDF_ExtGStateStack * GetExtGState() const { return mpExtState; }

	void SetMatrix( const CHE_Matrix & matirx );
	void SetRenderIntents( const GRAPHICS_STATE_RENDERINTENTS & ri );
	void SetFlatness( const FLOAT & flatness );
	bool SetFillColor( const CHE_PDF_Color & color );
	bool SetStrokeColor( const CHE_PDF_Color & color );
	bool SetFillColorSpace( CHE_PDF_ColorSpacePtr ColorSpace );
	bool SetStrokeColorSpace( CHE_PDF_ColorSpacePtr ColorSpace );
	void SetLineWidth( const FLOAT & lineWidth );
	void SetMiterLimit( const FLOAT & miterLimit );
	void SetLineCap( const GRAPHICS_STATE_LINECAP & lineCap );
	void SetLineJoin( const GRAPHICS_STATE_LINEJOIN & lineJion );
	void SetLineDash( const GRAPHICS_STATE_DASHPATTERN & dashPattern );
	void SetTextMatrix( const CHE_Matrix & matrix );
	void SetTextFontSize( const FLOAT & size );
	void SetTextFont( CHE_PDF_Font * pFont );
	void SetTextFontResName( const CHE_ByteString & resName );
	void SetTextCharSpace( const FLOAT & charSpace );
	void SetTextWordSpace( const FLOAT & wordSpace );
	void SetTextScaling( const FLOAT & scaling );
	void SetTextRise( const FLOAT & rise );
	void SetTextRenderMode( const GRAPHICS_STATE_TEXTRENDERMODE & rm );
	bool PushClipElement( CHE_PDF_ContentObject * pElement );
	bool PushExtGState( const CHE_ByteString & resName, const CHE_PDF_ExtGStatePtr & extgstate );

	void	CopyTextState( CHE_PDF_GState * pGState );

private:
	CHE_PDF_StrokeState *			MakeStrokeState();
	CHE_PDF_TextGState *			MakeTextState();
	CHE_PDF_ClipState *				MakeClipState();
	CHE_PDF_ExtGStateStack *		MakeExtGState();

    uint32                          mFlag;
	FLOAT                           mFlatness;
	GRAPHICS_STATE_RENDERINTENTS	mRenderIntents;
	CHE_Matrix						mMatrix;
	CHE_PDF_Color					mFillColor;
	CHE_PDF_Color					mStrokeColor;
	CHE_PDF_ColorSpacePtr			mFillColorSpace;
	CHE_PDF_ColorSpacePtr			mStrokeColorSpace;
	CHE_PDF_StrokeState *			mpStrokeState;
	CHE_PDF_TextGState *			mpTextState;
	CHE_PDF_ClipState *				mpClipState;
	CHE_PDF_ExtGStateStack *		mpExtState;
};

bool IsFloatEqual( const FLOAT &, const FLOAT & );

bool IsDefLineWidth( const FLOAT & lineWidth );
bool IsDefLineCap( const GRAPHICS_STATE_LINECAP & lineCap );
bool IsDefLineJoin( const GRAPHICS_STATE_LINEJOIN & lineJoin );
bool IsDefMiterLimit( const FLOAT & miterLimit );
bool IsDefLineDash( const GRAPHICS_STATE_DASHPATTERN & lineDash );

bool IsDefMatrix( const CHE_Matrix & textMatrix );

bool IsDefTextCharSpace( const FLOAT & charSpace );
bool IsDefTextWrodSpace( const FLOAT & wordSpace );
bool IsDefTextRise( const FLOAT & textRise );
bool IsDefTextScaling( const FLOAT & textScaling );
bool IsDefTextRenderMode( const GRAPHICS_STATE_TEXTRENDERMODE & rm );

bool IsDefFlatness( const FLOAT & flatness );
bool IsDefRenderIntents( const GRAPHICS_STATE_RENDERINTENTS & ri );

bool IsDefColorSpace( const CHE_PDF_ColorSpacePtr & colorSpace );
bool IsDefColor( const CHE_PDF_Color & color );

bool IsColorSpaceEqual( const CHE_PDF_ColorSpacePtr & cs1, const CHE_PDF_ColorSpacePtr & cs2 );
bool IsColorEqual( const CHE_PDF_Color & c1, const CHE_PDF_Color & c2 );

bool IsExtGStateEqual( const CHE_PDF_ExtGStateStack * pExtGS1, const CHE_PDF_ExtGStateStack * pExtGS2 );
bool IsExtGStateContinue( const CHE_PDF_ExtGStateStack * pExtGS1, const CHE_PDF_ExtGStateStack * pExtGS2 );

bool IsClipStateEqual( const CHE_PDF_ClipState * pClipGS1, const CHE_PDF_ClipState * pClipGS2 );
bool IsClipStateContinue( const CHE_PDF_ClipState * pClipGS1, const CHE_PDF_ClipState * pClipGS2 );

#endif