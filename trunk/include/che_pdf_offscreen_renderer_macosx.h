#ifndef _CHELIB_PDF_OFFSCREEN_RENDERER_H_
#define _CHELIB_PDF_OFFSCREEN_RENDERER_H_

#include "che_pdf_contentobject.h"
#include "che_pdf_xobject.h"
#include "che_pdf_pattern.h"
#include "che_pdf_gstate.h"

#include <ApplicationServices/ApplicationServices.h>
#include <CoreGraphics/CGPattern.h>
#include <CoreGraphics/CoreGraphics.h>

namespace chelib {

class CPDF_Offscreen_Renderer
{
public:
    CPDF_Offscreen_Renderer( CGContextRef cgContext );
    CPDF_Offscreen_Renderer( CGContextRef cgContext, CRect pageRect, uint32 rotate, FLOAT scale, FLOAT dpix, FLOAT dpiy );
    ~CPDF_Offscreen_Renderer();
    
    CPDF_FunctionPtr         shadingFunc;
    CPDF_ColorSpacePtr       shadingCS;
    CPDF_ShadingPtr          mShading;
    CMatrix                  mShadingMatrix;
    CPDF_FormXObjectPtr      mSoftMask;
    
    void Render( CPDF_ContentObjectList & content );
    void RenderTiling( CPDF_ContentObjectList & context, bool bColored );
    
    CGLayerRef GetLayer() const { return mLayerRef; }
    
private:
    void SetNoColor( bool bVal ) { mbNoColor = bVal; }
    bool IsNoColor() const { return mbNoColor; }
    
    CMatrix GetExtMatrix() const { return mExtMatrix; }
    
    void SetMatrix( const CMatrix & matrix );
    void SetExtMatrix( const CMatrix & matrix );
    void SetLineWidth( const FLOAT & lineWidth );
    void SetMiterLimit( const FLOAT & miterLimit );
    void SetLineCap( const GRAPHICS_STATE_LINECAP & lineCap );
    void SetLineJoin( const GRAPHICS_STATE_LINEJOIN & lineJion );
    void SetLineDash( const GRAPHICS_STATE_DASHPATTERN & dashPattern );
    void SetFillMode( GRAPHICS_STATE_FILLMODE mode );
    void SetBlendMode( GRAPHICS_STATE_BLENDMODE mode );
    void SetTextFont( CGFontRef font );
    void SetTextMatrix( CMatrix textMatrix );
    
    void SetFillColor( const ARGB & color );
    void SetStrokeColor( const ARGB & color );
    void SetFillColor( const CPDF_Color & color );
    void SetStrokeColor( const CPDF_Color & color );
    void SetFillColorSpace( const CPDF_ColorSpacePtr & cs );
    void SetStrokeColorSpace( const CPDF_ColorSpacePtr & cs );
    void SetImageColorSpace( const CPDF_ColorSpacePtr & cs );
    CGColorSpaceRef CreateColorSpace( const CPDF_ColorSpacePtr & cs );
    CGImageRef CreateImage( const CPDF_ImageXObjectPtr & imagePtr );
    CGImageRef CreateImage( CPDF_InlineImage * image );
    
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
    
    void SetCommonGState( CPDF_GState * pGState, bool bColore = TRUE, bool bMatrix = TRUE );
    void SetExtGState( CPDF_ExtGStateStack * pExtGState );
    void SetClipState( CPDF_ClipState * pClipState );
    
    void DrawPath( CPDF_Path * pPath );
    void DrawText( CPDF_Text * pText );
    void DrawTextAsPath( CPDF_Text * pText );
    void DrawTextGlyph( CGGlyph gid );
    void DrawInlineImage( CPDF_InlineImage * pImage );
    void DrawComponentRef( CPDF_ComponentRef * cmptRef );
    void DrawRefImage( const CPDF_ImageXObjectPtr & image );
    void DrawForm( const CPDF_FormXObjectPtr & form );
    void DrawShading( const CPDF_ShadingPtr & shading );
    void DrawContentObjectList( CPDF_ContentObjectList & list );
    
private:
    CMatrix                     mMatrix;
    CMatrix                     mExtMatrix;
    CMatrix                     mTextMatrix;
    CMatrix                     mTmpMatrix;
    GRAPHICS_STATE_FILLMODE     mFillMode;
    
    bool                        mbNoColor;

    FLOAT                       mFillAlpha;
    FLOAT                       mStrokeAlpha;
    
    CGContextRef                mContextRef;
    CGLayerRef                  mLayerRef;
    CGMutablePathRef            mPathRef;
    CGColorSpaceRef             mFillColorSpace;
    CGColorSpaceRef             mStrokeColorSpace;
    CGColorSpaceRef             mImageColorSpace;
    //CGPatternRef              mStrokePattern;
    CGPatternRef                mFillPattern;
    //bool                      mStrokePatternColored;
    bool                        mFillPatternColored;

    
    FLOAT                       mDpix;
    FLOAT                       mDpiy;
    FLOAT                       mScale;
    uint32                      mRotate;
    CRect                       mPageRect;
};

}//namespace

#endif
