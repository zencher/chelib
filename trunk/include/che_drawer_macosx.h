#ifndef _CHE_DRAWER_MACOSX_H_
#define _CHE_DRAWER_MACOSX_H_

#include <ApplicationServices/ApplicationServices.h>

#include "che_define.h"
#include "che_graphics.h"
#include "che_bitmap.h"

class CHE_GraphicsDrawer
{
public:
    CHE_GraphicsDrawer( CGContextRef context );
    CHE_GraphicsDrawer( uint32 width, uint32 height );
    ~CHE_GraphicsDrawer();

    void		Resize( uint32 width, uint32 height );
    uint32      GetWidth() const;
    uint32      GetHeight() const;
    void		Clear();

    //properties setting
    void		SetMatrix( const CHE_Matrix & matrix );
    void		SetExtMatrix( const CHE_Matrix & matrix );
    void		SetLineWidth( const FLOAT & lineWidth );
    void		SetMiterLimit( const FLOAT & miterLimit );
    void		SetFillColor( const ARGB & color );
    void		SetStrokeColor( const ARGB & color );
    void		SetLineCap( const GRAPHICS_STATE_LINECAP & lineCap );
    void		SetLineJoin( const GRAPHICS_STATE_LINEJOIN & lineJion );
    void		SetLineDash( const GRAPHICS_STATE_DASHPATTERN & dashPattern );
    void		SetFillMode( GRAPHICS_STATE_FILLMODE mode );


    //path & clip operations
    void		MoveTo( FLOAT x, FLOAT y );
    void		LineTo( FLOAT x, FLOAT y );
    void		CurveTo( FLOAT x1, FLOAT y1, FLOAT x2, FLOAT y2, FLOAT x3, FLOAT y3 );
    void     Rectangle( FLOAT x, FLOAT y, FLOAT width, FLOAT height );
    void		ClosePath();
    void		FillPath();
    void		StrokePath();
    void		FillStrokePath();
    void		ClipPath();
    void		FillClipPath();
    void		StrokeClipPath();
    void		FillStrokeClipPath();
    void		ResetClip();
    
    void     DrawBitmap( CHE_Bitmap * pBitmap );
    
    void     SetTextFont( PBYTE fontData, size_t dataSize );
    void     SetTextMatrix( CHE_Matrix textMatrix );
    void     DrawText( unsigned short gid );
    
    //todo
    void         SaveToFile( const char * pPath );
    CGContextRef    GetContextRef() const { return mContentRef; }

private:
    
    CGContextRef                mContentRef;
    CGColorSpaceRef             mColorSpaceRef;
    uint32                      mWidth;
    uint32                      mHeight;
    CHE_Matrix                  mMatrix;
    CHE_Matrix					mExtMatrix;
    CHE_Matrix                  mTextMatrix;
    CGAffineTransform           mAffineTransform;
    GRAPHICS_STATE_FILLMODE     mFillMode;
    CGMutablePathRef            mPathRef;
};

#endif
