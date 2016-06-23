#ifndef _CHE_PDF_RENDERER_H_
#define _CHE_PDF_RENDERER_H_

#include "che_pdf_contentobjs.h"
#include "che_pdf_xobject.h"
#include "che_pdf_pattern.h"
#include "che_pdf_gstate.h"
#include <ApplicationServices/ApplicationServices.h>
#include <CoreGraphics/CGPattern.h>
#include <CoreGraphics/CoreGraphics.h>

class CHE_PDF_Renderer
{
public:
    CHE_PDF_Renderer( CGContextRef cgContext );
    ~CHE_PDF_Renderer();
    
    CHE_PDF_FunctionPtr shadingFunc;
    
    CHE_PDF_ColorSpacePtr shadingCS;
    
    CHE_PDF_ShadingPtr mShading;
    CHE_Matrix         mShadingMatrix;
    
    CHE_PDF_FormXObjectPtr mSoftMask;
    
    HE_VOID     SetPosition( HE_FLOAT x, HE_FLOAT y );
    
    HE_VOID     Render( CHE_PDF_ContentObjectList & content, CHE_Rect pageRect, HE_UINT32 rotate, HE_FLOAT scale, HE_FLOAT dpix, HE_FLOAT dpiy );
    
    HE_VOID     RenderTiling( CHE_PDF_ContentObjectList & context, HE_BOOL bColored );
    
public:
    
    HE_VOID     SetNoColor( HE_BOOL bVal ) { mbNoColor = bVal; }
    HE_BOOL     IsNoColor() const { return mbNoColor; }
    
    CHE_Matrix GetExtMatrix() const { return mExtMatrix; }
    
    HE_VOID		SetMatrix( const CHE_Matrix & matrix );
    HE_VOID		SetExtMatrix( const CHE_Matrix & matrix );
    HE_VOID		SetLineWidth( const HE_FLOAT & lineWidth );
    HE_VOID		SetMiterLimit( const HE_FLOAT & miterLimit );
    HE_VOID		SetLineCap( const GRAPHICS_STATE_LINECAP & lineCap );
    HE_VOID		SetLineJoin( const GRAPHICS_STATE_LINEJOIN & lineJion );
    HE_VOID		SetLineDash( const GRAPHICS_STATE_DASHPATTERN & dashPattern );
    HE_VOID		SetFillMode( GRAPHICS_STATE_FILLMODE mode );
    HE_VOID     SetBlendMode( GRAPHICS_STATE_BLENDMODE mode );
    HE_VOID     SetTextFont( CGFontRef font );
    HE_VOID     SetTextMatrix( CHE_Matrix textMatrix );
    
    HE_VOID		SetFillColor( const HE_ULONG & color );
    HE_VOID		SetStrokeColor( const HE_ULONG & color );
    HE_VOID     SetFillColor( const CHE_PDF_Color & color );
    HE_VOID     SetStrokeColor( const CHE_PDF_Color & color );
    HE_VOID     SetFillColorSpace( const CHE_PDF_ColorSpacePtr & cs );
    HE_VOID     SetStrokeColorSpace( const CHE_PDF_ColorSpacePtr & cs );
    HE_VOID     SetImageColorSpace( const CHE_PDF_ColorSpacePtr & cs );
    CGColorSpaceRef CreateColorSpace( const CHE_PDF_ColorSpacePtr & cs );
    CGImageRef      CreateImage( const CHE_PDF_ImageXObjectPtr & imagePtr );
    CGImageRef      CreateImage( CHE_PDF_InlineImage * image );
    
    //path & clip operations
    HE_VOID     ResetPath();
    HE_VOID		MoveTo( HE_FLOAT x, HE_FLOAT y );
    HE_VOID		LineTo( HE_FLOAT x, HE_FLOAT y );
    HE_VOID		CurveTo( HE_FLOAT x1, HE_FLOAT y1, HE_FLOAT x2, HE_FLOAT y2, HE_FLOAT x3, HE_FLOAT y3 );
    HE_VOID     Rectangle( HE_FLOAT x, HE_FLOAT y, HE_FLOAT width, HE_FLOAT height );
    HE_VOID		ClosePath();
    HE_VOID		FillPath();
    HE_VOID		StrokePath();
    HE_VOID		FillStrokePath();
    HE_VOID		ClipPath();
    HE_VOID		FillClipPath();
    HE_VOID		StrokeClipPath();
    HE_VOID		FillStrokeClipPath();
    
    HE_VOID     StoreGState();
    HE_VOID     RestoreGState();
    
    HE_VOID     SetCommonGState( CHE_PDF_GState * pGState, HE_BOOL bColore = TRUE, HE_BOOL bMatrix = TRUE );
    HE_VOID     SetExtGState( CHE_PDF_ExtGStateStack * pExtGState );
    HE_VOID     SetClipState( CHE_PDF_ClipState * pClipState );
    
    HE_VOID     DrawPath( CHE_PDF_Path * pPath );
    HE_VOID     DrawText( CHE_PDF_Text * pText );
    HE_VOID     DrawTextAsPath( CHE_PDF_Text * pText );
    HE_VOID     DrawTextGlyph( CGGlyph gid );
    HE_VOID     DrawInlineImage( CHE_PDF_InlineImage * pImage );
    HE_VOID     DrawComponentRef( CHE_PDF_ComponentRef * cmptRef );
    HE_VOID     DrawRefImage( const CHE_PDF_ImageXObjectPtr & image );
    HE_VOID     DrawForm( const CHE_PDF_FormXObjectPtr & form );
    HE_VOID     DrawShading( const CHE_PDF_ShadingPtr & shading );
    
    HE_VOID     SetPatternOffset( HE_FLOAT x, HE_FLOAT y ) {  mPatternOffsetX = x; mPatternOffsetY = y; }
    
public:
    HE_VOID     DrawContentObjectList( CHE_PDF_ContentObjectList & list );
    
private:
    CHE_Matrix                  mMatrix;
    CHE_Matrix					mExtMatrix;
    CHE_Matrix                  mTextMatrix;
    CHE_Matrix                  mTmpMatrix;
    GRAPHICS_STATE_FILLMODE     mFillMode;
    
    HE_BOOL                     mbNoColor;
    
    HE_FLOAT                    mPosiX;
    HE_FLOAT                    mPosiY;
    
    HE_FLOAT                    mFillAlpha;
    HE_FLOAT                    mStrokeAlpha;
    
    CGContextRef                mContextRef;
    CGMutablePathRef            mPathRef;
    CGColorSpaceRef             mFillColorSpace;
    CGColorSpaceRef             mStrokeColorSpace;
    CGColorSpaceRef             mImageColorSpace;
    //CGPatternRef                mStrokePattern;
    CGPatternRef                mFillPattern;
    //HE_BOOL                     mStrokePatternColored;
    HE_BOOL                     mFillPatternColored;
    
    
    HE_FLOAT                    mPatternOffsetX;
    HE_FLOAT                    mPatternOffsetY;
    
    HE_FLOAT                    mDpix;
    HE_FLOAT                    mDpiy;
    HE_FLOAT                    mScale;
    HE_UINT32                   mRotate;
    CHE_Rect                    mPageRect;
};

#endif
