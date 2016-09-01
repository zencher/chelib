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
    
    CHE_PDF_FunctionPtr         shadingFunc;
    CHE_PDF_ColorSpacePtr       shadingCS;
    CHE_PDF_ShadingPtr          mShading;
    CHE_Matrix                  mShadingMatrix;
    CHE_PDF_FormXObjectPtr      mSoftMask;
    
    void SetPosition( FLOAT x, FLOAT y );
    void SetPatternOffset( FLOAT x, FLOAT y ) {  mPatternOffsetX = x; mPatternOffsetY = y; }
    void Render( CHE_PDF_ContentObjectList & content, CHE_Rect pageRect, uint32 rotate, FLOAT scale, FLOAT dpix, FLOAT dpiy );
    void RenderTiling( CHE_PDF_ContentObjectList & context, bool bColored );
    
private:
    void SetNoColor( bool bVal ) { mbNoColor = bVal; }
    bool IsNoColor() const { return mbNoColor; }
    
    CHE_Matrix GetExtMatrix() const { return mExtMatrix; }
    
    void SetMatrix( const CHE_Matrix & matrix );
    void SetExtMatrix( const CHE_Matrix & matrix );
    void SetLineWidth( const FLOAT & lineWidth );
    void SetMiterLimit( const FLOAT & miterLimit );
    void SetLineCap( const GRAPHICS_STATE_LINECAP & lineCap );
    void SetLineJoin( const GRAPHICS_STATE_LINEJOIN & lineJion );
    void SetLineDash( const GRAPHICS_STATE_DASHPATTERN & dashPattern );
    void SetFillMode( GRAPHICS_STATE_FILLMODE mode );
    void SetBlendMode( GRAPHICS_STATE_BLENDMODE mode );
    void SetTextFont( CGFontRef font );
    void SetTextMatrix( CHE_Matrix textMatrix );
    
    void SetFillColor( const ARGB & color );
    void SetStrokeColor( const ARGB & color );
    void SetFillColor( const CHE_PDF_Color & color );
    void SetStrokeColor( const CHE_PDF_Color & color );
    void SetFillColorSpace( const CHE_PDF_ColorSpacePtr & cs );
    void SetStrokeColorSpace( const CHE_PDF_ColorSpacePtr & cs );
    void SetImageColorSpace( const CHE_PDF_ColorSpacePtr & cs );
    CGColorSpaceRef CreateColorSpace( const CHE_PDF_ColorSpacePtr & cs );
    CGImageRef CreateImage( const CHE_PDF_ImageXObjectPtr & imagePtr );
    CGImageRef CreateImage( CHE_PDF_InlineImage * image );
    
    //path & clip operations
    void ResetPath();
    void MoveTo( FLOAT x, FLOAT y );
    void LineTo( FLOAT x, FLOAT y );
    void CurveTo( FLOAT x1, FLOAT y1, FLOAT x2, FLOAT y2, FLOAT x3, FLOAT y3 );
    void Rectangle( FLOAT x, FLOAT y, FLOAT width, FLOAT height );
    void ClosePath();
    void FillPath();
    void StrokePath();
    void FillStrokePath();
    void ClipPath();
    void FillClipPath();
    void StrokeClipPath();
    void FillStrokeClipPath();
    
    void StoreGState();
    void RestoreGState();
    
    void SetCommonGState( CHE_PDF_GState * pGState, bool bColore = TRUE, bool bMatrix = TRUE );
    void SetExtGState( CHE_PDF_ExtGStateStack * pExtGState );
    void SetClipState( CHE_PDF_ClipState * pClipState );
    
    void DrawPath( CHE_PDF_Path * pPath );
    void DrawText( CHE_PDF_Text * pText );
    void DrawTextAsPath( CHE_PDF_Text * pText );
    void DrawTextGlyph( CGGlyph gid );
    void DrawInlineImage( CHE_PDF_InlineImage * pImage );
    void DrawComponentRef( CHE_PDF_ComponentRef * cmptRef );
    void DrawRefImage( const CHE_PDF_ImageXObjectPtr & image );
    void DrawForm( const CHE_PDF_FormXObjectPtr & form );
    void DrawShading( const CHE_PDF_ShadingPtr & shading );
    void DrawContentObjectList( CHE_PDF_ContentObjectList & list );
    
private:
    CHE_Matrix                  mMatrix;
    CHE_Matrix					mExtMatrix;
    CHE_Matrix                  mTextMatrix;
    CHE_Matrix                  mTmpMatrix;
    GRAPHICS_STATE_FILLMODE     mFillMode;
    
    bool                        mbNoColor;
    
    FLOAT                       mPosiX;
    FLOAT                       mPosiY;
    
    FLOAT                       mFillAlpha;
    FLOAT                       mStrokeAlpha;
    
    CGContextRef                mContextRef;
    CGMutablePathRef            mPathRef;
    CGColorSpaceRef             mFillColorSpace;
    CGColorSpaceRef             mStrokeColorSpace;
    CGColorSpaceRef             mImageColorSpace;
    //CGPatternRef              mStrokePattern;
    CGPatternRef                mFillPattern;
    //bool                      mStrokePatternColored;
    bool                        mFillPatternColored;
    
    FLOAT                       mPatternOffsetX;
    FLOAT                       mPatternOffsetY;
    
    FLOAT                       mDpix;
    FLOAT                       mDpiy;
    FLOAT                       mScale;
    uint32                      mRotate;
    CHE_Rect                    mPageRect;
};

#endif
