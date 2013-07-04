#ifndef _CHE_DRAWER_MACOSX_H_
#define _CHE_DRAWER_MACOSX_H_

#include <ApplicationServices/ApplicationServices.h>

#include "che_define.h"
#include "che_graphics.h"

class CHE_GraphicsDrawer
{
public:
    CHE_GraphicsDrawer( HE_ULONG width, HE_ULONG height );
    ~CHE_GraphicsDrawer();

    HE_VOID		Resize( HE_ULONG width, HE_ULONG height );
    HE_ULONG	GetWidth() const;
    HE_ULONG	GetHeight() const;
    HE_VOID		Clear();

    //properties setting
    HE_VOID		SetMatrix( const CHE_Matrix & matrix );
    HE_VOID		SetExtMatrix( const CHE_Matrix & matrix );
    HE_VOID		SetLineWidth( const HE_FLOAT & lineWidth );
    HE_VOID		SetMiterLimit( const HE_FLOAT & miterLimit );
    HE_VOID		SetFillColor( const HE_ULONG & color );
    HE_VOID		SetStrokeColor( const HE_ULONG & color );
    HE_VOID		SetLineCap( const GRAPHICS_STATE_LINECAP & lineCap );
    HE_VOID		SetLineJoin( const GRAPHICS_STATE_LINEJOIN & lineJion );
    HE_VOID		SetLineDash( const GRAPHICS_STATE_DASHPATTERN & dashPattern );
    HE_VOID		SetFillMode( GRAPHICS_STATE_FILLMODE mode );


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
    HE_VOID		ResetClip();
    
    //todo
    HE_VOID     SaveToFile( const char * pPath );
    CGContextRef    GetContextRef() const { return mContentRef; }

private:
    
    CGContextRef                mContentRef;
    CGColorSpaceRef             mColorSpaceRef;
    HE_ULONG					mWidth;
    HE_ULONG					mHeight;
    CHE_Matrix                  mMatrix;
    CHE_Matrix					mExtMatrix;
    CGAffineTransform           mAffineTransform;
    GRAPHICS_STATE_FILLMODE     mFillMode;
    CGMutablePathRef            mPathRef;
};

#endif
