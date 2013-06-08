#include "../include/che_drawer_macosx.h"

CHE_GraphicsDrawer::CHE_GraphicsDrawer( HE_ULONG width, HE_ULONG height )
    : mContentRef( NULL ), mWidth( width ), mHeight( height ),
      mExtMatrix( CHE_Matrix() ), mFillMode( FillMode_Nonzero ), mPathRef( NULL )
{
    mColorSpaceRef = CGColorSpaceCreateWithName(kCGColorSpaceGenericRGB);
    mContentRef = CGBitmapContextCreate( NULL, width, height, 8, width * 4, mColorSpaceRef, kCGImageAlphaPremultipliedLast );
    mAffineTransform.a = 1;
    mAffineTransform.b = 0;
    mAffineTransform.c = 0;
    mAffineTransform.d = 1;
    mAffineTransform.tx = 0;
    mAffineTransform.ty = 0;
}

CHE_GraphicsDrawer::~CHE_GraphicsDrawer()
{
    if ( mContentRef ) {
        CGContextRelease( mContentRef );
        mContentRef = NULL;
    }
    if ( mContentRef )
    {
        CGColorSpaceRelease( mColorSpaceRef );
        mContentRef = NULL;
    }
}

HE_VOID CHE_GraphicsDrawer::Resize( HE_ULONG width, HE_ULONG height )
{
    if ( mContentRef ) {
        CGContextRelease( mContentRef );
        mContentRef = NULL;
    }
    if ( mContentRef )
    {
        CGColorSpaceRelease( mColorSpaceRef );
        mContentRef = NULL;
    }
    mColorSpaceRef = CGColorSpaceCreateWithName(kCGColorSpaceGenericRGB);
    mContentRef = CGBitmapContextCreate( NULL, width, height, 8, width * 4, mColorSpaceRef, kCGImageAlphaPremultipliedLast );
}

HE_ULONG CHE_GraphicsDrawer::GetWidth() const
{
    return mWidth;
}

HE_ULONG CHE_GraphicsDrawer::GetHeight() const
{
    return mHeight;
}

HE_VOID CHE_GraphicsDrawer::Clear()
{
    if ( mContentRef )
    {
        CGContextSetRGBFillColor( mContentRef, 1, 1, 1, 1 );
        CGContextFillRect( mContentRef , CGRectMake( 0, 0, mWidth, mHeight ) );
    }
}

HE_VOID	CHE_GraphicsDrawer::SetMatrix( const CHE_Matrix & matrix )
{
    mMatrix = matrix;
}

HE_VOID	CHE_GraphicsDrawer::SetExtMatrix( const CHE_Matrix & matrix )
{
    mExtMatrix = matrix;
}

HE_VOID	CHE_GraphicsDrawer::SetLineWidth( const HE_FLOAT & lineWidth )
{
    if ( mContentRef )
    {
        CGContextSetLineWidth( mContentRef, lineWidth );
    }
}

HE_VOID	CHE_GraphicsDrawer::SetMiterLimit( const HE_FLOAT & miterLimit )
{
    if ( mContentRef )
    {
        CGContextSetMiterLimit( mContentRef, miterLimit );
    }
}

HE_VOID	CHE_GraphicsDrawer::SetFillColor( const HE_ULONG & color )
{
    //todo
    if ( mContentRef )
    {
        CGContextSetRGBFillColor( mContentRef , 0, 0, 0, 1 );
    }
}

HE_VOID	CHE_GraphicsDrawer::SetStrokeColor( const HE_ULONG & color )
{
    //todo
    if ( mContentRef )
    {
        CGContextSetRGBStrokeColor( mContentRef, 0, 0, 0, 1 );
    }
}

HE_VOID	CHE_GraphicsDrawer::SetLineCap( const GRAPHICS_STATE_LINECAP & lineCap )
{
    if ( mContentRef )
    {
        switch ( lineCap )
		{
        case LineCap_Butt:
            CGContextSetLineCap( mContentRef, kCGLineCapButt );
            break;
        case LineCap_Round:
            CGContextSetLineCap( mContentRef, kCGLineCapRound );
            break;
        case LineCap_Square:
            CGContextSetLineCap( mContentRef, kCGLineCapSquare );
            break;
        default:
            break;
		}
    }
}

HE_VOID	CHE_GraphicsDrawer::SetLineJoin( const GRAPHICS_STATE_LINEJOIN & lineJion )
{
    if ( mContentRef )
	{
		switch ( lineJion )
		{
        case LineJoin_Miter:
            CGContextSetLineJoin( mContentRef, kCGLineJoinMiter );
            break;
        case LineJoin_Round:
            CGContextSetLineJoin( mContentRef, kCGLineJoinRound );
            break;
        case LineJoin_Bevel:
            CGContextSetLineJoin( mContentRef, kCGLineJoinBevel );
            break;
        default:
            break;
		}
	}
}

HE_VOID	CHE_GraphicsDrawer::SetLineDash( const GRAPHICS_STATE_DASHPATTERN & dashPattern )
{
    CGFloat * pCGFloatArray = NULL;
    if ( dashPattern.dashArray.size() > 0 )
    {
        pCGFloatArray = new CGFloat[dashPattern.dashArray.size()];
    }
    if ( mContentRef )
    {
        CGContextSetLineDash( mContentRef, dashPattern.dashPhase, pCGFloatArray, dashPattern.dashArray.size() );
    }
    if ( pCGFloatArray ) {
        delete [] pCGFloatArray;
    }
}

HE_VOID	CHE_GraphicsDrawer::SetFillMode( GRAPHICS_STATE_FILLMODE mode )
{
    mFillMode = mode;
}

HE_VOID	CHE_GraphicsDrawer::MoveTo( HE_FLOAT x, HE_FLOAT y )
{
    if ( mPathRef == NULL )
    {
        mPathRef = CGPathCreateMutable();
    }
    CHE_Matrix tmpMatrix;
    tmpMatrix = mMatrix;
    tmpMatrix.Concat( mExtMatrix );
    mAffineTransform.a = tmpMatrix.a;
    mAffineTransform.b = tmpMatrix.b;
    mAffineTransform.c = tmpMatrix.c;
    mAffineTransform.d = tmpMatrix.d;
    mAffineTransform.tx = tmpMatrix.e;
    mAffineTransform.ty = tmpMatrix.f;
    CGPathMoveToPoint( mPathRef, &mAffineTransform, x, y );
}

HE_VOID	CHE_GraphicsDrawer::LineTo( HE_FLOAT x, HE_FLOAT y )
{
    if ( mPathRef )
    {
        CHE_Matrix tmpMatrix;
        tmpMatrix = mMatrix;
        tmpMatrix.Concat( mExtMatrix );
        mAffineTransform.a = tmpMatrix.a;
        mAffineTransform.b = tmpMatrix.b;
        mAffineTransform.c = tmpMatrix.c;
        mAffineTransform.d = tmpMatrix.d;
        mAffineTransform.tx = tmpMatrix.e;
        mAffineTransform.ty = tmpMatrix.f;
        CGPathAddLineToPoint( mPathRef, &mAffineTransform, x, y );
    }
}

HE_VOID	CHE_GraphicsDrawer::CurveTo( HE_FLOAT x1, HE_FLOAT y1, HE_FLOAT x2, HE_FLOAT y2, HE_FLOAT x3, HE_FLOAT y3 )
{
    if ( mPathRef )
    {
        CHE_Matrix tmpMatrix;
        tmpMatrix = mMatrix;
        tmpMatrix.Concat( mExtMatrix );
        mAffineTransform.a = tmpMatrix.a;
        mAffineTransform.b = tmpMatrix.b;
        mAffineTransform.c = tmpMatrix.c;
        mAffineTransform.d = tmpMatrix.d;
        mAffineTransform.tx = tmpMatrix.e;
        mAffineTransform.ty = tmpMatrix.f;
        CGPathAddCurveToPoint( mPathRef, &mAffineTransform, x1, y1, x2, y2, x3, y3 );
    }
}

HE_VOID CHE_GraphicsDrawer::Rectangle( HE_FLOAT x, HE_FLOAT y, HE_FLOAT width, HE_FLOAT height )
{
    if ( mPathRef == NULL )
    {
        mPathRef = CGPathCreateMutable();
    }
    if ( mPathRef )
    {
        CHE_Matrix tmpMatrix;
        tmpMatrix = mMatrix;
        tmpMatrix.Concat( mExtMatrix );
        mAffineTransform.a = tmpMatrix.a;
        mAffineTransform.b = tmpMatrix.b;
        mAffineTransform.c = tmpMatrix.c;
        mAffineTransform.d = tmpMatrix.d;
        mAffineTransform.tx = tmpMatrix.e;
        mAffineTransform.ty = tmpMatrix.f;
        CGPathAddRect( mPathRef, &mAffineTransform, CGRectMake( x, y, width, height ) );
    }
}

HE_VOID	CHE_GraphicsDrawer::ClosePath()
{
    if ( mPathRef && !CGPathIsEmpty( mPathRef ) )
    {
        CGPathCloseSubpath( mPathRef );
    }
}

HE_VOID	CHE_GraphicsDrawer::FillPath()
{
    if ( mContentRef && mPathRef && !CGPathIsEmpty( mPathRef ) )
    {
        CGContextAddPath( mContentRef, mPathRef );
        if ( mFillMode == FillMode_Nonzero )
        {
            CGContextFillPath( mContentRef );
        }else{
            CGContextEOFillPath( mContentRef );
        }
        CGPathRelease( mPathRef );
        mPathRef = NULL;
    }
}

HE_VOID	CHE_GraphicsDrawer::StrokePath()
{
    if ( mContentRef && mPathRef && !CGPathIsEmpty( mPathRef ) )
    {
        CGContextAddPath( mContentRef, mPathRef );
        CGContextStrokePath( mContentRef );
        CGPathRelease( mPathRef );
        mPathRef = NULL;
    }
}

HE_VOID	CHE_GraphicsDrawer::FillStrokePath()
{
    if ( mContentRef && mPathRef && !CGPathIsEmpty( mPathRef ) )
    {
        CGContextAddPath( mContentRef, mPathRef );
        if ( mFillMode == FillMode_Nonzero )
        {
            CGContextFillPath( mContentRef );
        }else{
            CGContextEOFillPath( mContentRef );
        }
        CGContextAddPath( mContentRef, mPathRef );
        CGContextStrokePath( mContentRef );
        CGPathRelease( mPathRef );
        mPathRef = NULL;
    }
}

HE_VOID	CHE_GraphicsDrawer::ClipPath()
{
    if ( mContentRef && mPathRef && !CGPathIsEmpty( mPathRef ) )
    {
        CGContextAddPath( mContentRef, mPathRef );
        CGContextClip( mContentRef );
        CGPathRelease( mPathRef );
        mPathRef = NULL;
    }
}

HE_VOID	CHE_GraphicsDrawer::FillClipPath()
{
    if ( mContentRef && mPathRef && !CGPathIsEmpty( mPathRef ) )
    {
        CGContextAddPath( mContentRef, mPathRef );
        if ( mFillMode == FillMode_Nonzero )
        {
            CGContextFillPath( mContentRef );
        }else{
            CGContextEOFillPath( mContentRef );
        }
        CGContextAddPath( mContentRef, mPathRef );
        CGContextClip( mContentRef );
        CGPathRelease( mPathRef );
        mPathRef = NULL;
    }
}

HE_VOID	CHE_GraphicsDrawer::StrokeClipPath()
{
    if ( mContentRef && mPathRef && !CGPathIsEmpty( mPathRef ) )
    {
        CGContextAddPath( mContentRef, mPathRef );
        CGContextStrokePath( mContentRef );
        CGContextAddPath( mContentRef, mPathRef );
        CGContextClip( mContentRef );
        CGPathRelease( mPathRef );
        mPathRef = NULL;
    }
}

HE_VOID	CHE_GraphicsDrawer::FillStrokeClipPath()
{
    if ( mContentRef && mPathRef && !CGPathIsEmpty( mPathRef ) )
    {
        CGContextAddPath( mContentRef, mPathRef );
        if ( mFillMode == FillMode_Nonzero )
        {
            CGContextFillPath( mContentRef );
        }else{
            CGContextEOFillPath( mContentRef );
        }
        CGContextAddPath( mContentRef, mPathRef );
        CGContextStrokePath( mContentRef );
        CGContextAddPath( mContentRef, mPathRef );
        CGContextClip( mContentRef );
        CGPathRelease( mPathRef );
        mPathRef = NULL;
    }
}

HE_VOID	CHE_GraphicsDrawer::ResetClip()
{
    if ( mContentRef )
    {
        //todo
        CGPathRelease( mPathRef );
        mPathRef = NULL;
    }
}