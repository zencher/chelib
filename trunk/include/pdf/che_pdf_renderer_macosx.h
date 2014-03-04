#ifndef _CHE_PDF_RENDERER_H_
#define _CHE_PDF_RENDERER_H_

#include "che_pdf_contentobjs.h"
#include "che_pdf_xobject.h"
#include "che_pdf_pattern.h"
#include "che_pdf_gstate.h"
#include <ApplicationServices/ApplicationServices.h>

class CHE_PDF_Renderer
{
public:
    CHE_PDF_Renderer( CGContextRef cgContext );
    ~CHE_PDF_Renderer();
    
    HE_VOID     SetDIP( HE_FLOAT dipx, HE_FLOAT dipy );
    HE_VOID     SetScale( HE_FLOAT scale );
    HE_VOID     SetPosition( HE_FLOAT x, HE_FLOAT y );
    HE_VOID     Render( CHE_PDF_ContentObjectList & content );
    
private:
    HE_VOID		SetMatrix( const CHE_Matrix & matrix );
    HE_VOID		SetExtMatrix( const CHE_Matrix & matrix );
    HE_VOID		SetLineWidth( const HE_FLOAT & lineWidth );
    HE_VOID		SetMiterLimit( const HE_FLOAT & miterLimit );
    HE_VOID		SetLineCap( const GRAPHICS_STATE_LINECAP & lineCap );
    HE_VOID		SetLineJoin( const GRAPHICS_STATE_LINEJOIN & lineJion );
    HE_VOID		SetLineDash( const GRAPHICS_STATE_DASHPATTERN & dashPattern );
    HE_VOID		SetFillMode( GRAPHICS_STATE_FILLMODE mode );
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
    
    //path & clip operations
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
    
    HE_VOID     SetCommonGState( CHE_PDF_GState * pGState );
    HE_VOID     SetClipState( CHE_PDF_ClipState * pClipState );
    
    HE_VOID     DrawPath( CHE_PDF_Path * pPath );
    HE_VOID     DrawText( CHE_PDF_Text * pText );
    HE_VOID     DrawTextAsPath( CHE_PDF_Text * pText );
    HE_VOID     DrawTextGlyph( CGGlyph gid );
    HE_VOID     DrawInlineImage( CHE_PDF_InlineImage * pImage );
    HE_VOID     DrawComponentRef( CHE_PDF_ComponentRef * cmptRef, const CHE_Matrix & extMatrix );
    HE_VOID     DrawRefImage( const CHE_PDF_ImageXObjectPtr & image );
    HE_VOID     DrawForm( const CHE_PDF_FormXObjectPtr & form, const CHE_Matrix & extMatrix );
    HE_VOID     DrawShading( const CHE_PDF_ShadingPtr & shading );
    
private:
    HE_FLOAT                    mScale;
    HE_FLOAT                    mDipx;
    HE_FLOAT                    mDipy;
    HE_FLOAT                    mPosiX;
    HE_FLOAT                    mPosiY;
    CHE_Matrix                  mMatrix;
    CHE_Matrix					mExtMatrix;
    CHE_Matrix                  mTextMatrix;
    GRAPHICS_STATE_FILLMODE     mFillMode;
    
    CGContextRef                mContextRef;
    CGMutablePathRef            mPathRef;
    CGColorSpaceRef             mFillColorSpace;
    CGColorSpaceRef             mStrokeColorSpace;
    CGColorSpaceRef             mImageColorSpace;
};

#endif
