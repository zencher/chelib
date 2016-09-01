#include "../include/che_drawer_macosx.h"

CHE_GraphicsDrawer::CHE_GraphicsDrawer( CGContextRef context )
{
    mContentRef = context;
}

CHE_GraphicsDrawer::CHE_GraphicsDrawer( uint32 width, uint32 height )
    : mContentRef( NULL ), mWidth( width ), mHeight( height ),
      mExtMatrix( CHE_Matrix() ), mFillMode( FillMode_Nonzero ), mPathRef( NULL )
{
    mColorSpaceRef = CGColorSpaceCreateDeviceRGB();
    
    /*mContentRef = CGBitmapContextCreate( NULL, width, height, 8, width * 4, mColorSpaceRef, kCGImageAlphaNoneSkipLast | kCGBitmapByteOrder32Host*kCGImageAlphaPremultipliedFirst );
    CGContextSetShouldAntialias( mContentRef, true );
    CGContextSetAllowsAntialiasing( mContentRef, true );
    CGContextSetShouldSmoothFonts( mContentRef , true );
    CGContextSetAllowsFontSmoothing( mContentRef, true );
    CGContextSetShouldSubpixelPositionFonts( mContentRef , true );
    CGContextSetAllowsFontSubpixelPositioning( mContentRef, true );
    CGContextSetShouldSubpixelQuantizeFonts( mContentRef , true );
    CGContextSetAllowsFontSubpixelQuantization( mContentRef, true );*/
    mAffineTransform.a = 1;
    mAffineTransform.b = 0;
    mAffineTransform.c = 0;
    mAffineTransform.d = 1;
    mAffineTransform.tx = 0;
    mAffineTransform.ty = 0;
}

CHE_GraphicsDrawer::~CHE_GraphicsDrawer()
{
    /*if ( mContentRef ) {
        CGContextRelease( mContentRef );
        mContentRef = NULL;
    }
    if ( mColorSpaceRef )
    {
        CGColorSpaceRelease( mColorSpaceRef );
        mColorSpaceRef = NULL;
    }*/
}

void CHE_GraphicsDrawer::Resize( uint32 width, uint32 height )
{
    /*if ( mContentRef )
    {
        CGContextRelease( mContentRef );
        mContentRef = NULL;
    }
    if ( mColorSpaceRef )
    {
        CGColorSpaceRelease( mColorSpaceRef );
        mColorSpaceRef = NULL;
    }
    mColorSpaceRef = CGColorSpaceCreateDeviceRGB();
    mWidth = width;
    mHeight = height;
    mContentRef = CGBitmapContextCreate( NULL, mWidth, mHeight, 8, mWidth * 4, mColorSpaceRef, kCGImageAlphaNoneSkipLast | kCGBitmapByteOrder32Host );
    //CGContextSetShouldAntialias( mContentRef, true );
    //CGContextSetAllowsAntialiasing( mContentRef, true );
    //CGContextSetShouldSubpixelPositionFonts( mContentRef , true );
    //CGContextSetShouldSubpixelQuantizeFonts( mContentRef , true );
    
    //CGContextSetShouldAntialias( mContentRef, false );
    //CGContextSetAllowsAntialiasing( mContentRef, false );
    //CGContextSetShouldSmoothFonts( mContentRef , false );
    //CGContextSetAllowsFontSmoothing( mContentRef, false );
    CGContextSetShouldSubpixelPositionFonts( mContentRef , false );
    CGContextSetAllowsFontSubpixelPositioning( mContentRef, false );
    CGContextSetShouldSubpixelQuantizeFonts( mContentRef , false );
    CGContextSetAllowsFontSubpixelQuantization( mContentRef, false );*/
}

uint32 CHE_GraphicsDrawer::GetWidth() const
{
    return mWidth;
}

uint32 CHE_GraphicsDrawer::GetHeight() const
{
    return mHeight;
}

void CHE_GraphicsDrawer::Clear()
{
    if ( mContentRef )
    {
        CGContextSetRGBFillColor( mContentRef, 1, 1, 1, 1 );
        CGContextFillRect( mContentRef , CGRectMake( 0, 0, mWidth, mHeight ) );
    }
}

void CHE_GraphicsDrawer::SetMatrix( const CHE_Matrix & matrix )
{
    mMatrix = matrix;
}

void CHE_GraphicsDrawer::SetExtMatrix( const CHE_Matrix & matrix )
{
    mExtMatrix = matrix;
}

void CHE_GraphicsDrawer::SetLineWidth( const FLOAT & lineWidth )
{
    if ( mContentRef )
    {
        CGContextSetLineWidth( mContentRef, lineWidth );
    }
}

void CHE_GraphicsDrawer::SetMiterLimit( const FLOAT & miterLimit )
{
    if ( mContentRef )
    {
        CGContextSetMiterLimit( mContentRef, miterLimit );
    }
}

void CHE_GraphicsDrawer::SetFillColor( const ARGB & color )
{
    if ( mContentRef )
    {
        CGContextSetRGBFillColor( mContentRef , (color>>16&0xFF)/256.0, (color>>8&0xFF)/256.0, (color&0xFF)/256.0, (color>>24&0xFF)/256.0 );
    }
}

void CHE_GraphicsDrawer::SetStrokeColor( const ARGB & color )
{
    if ( mContentRef )
    {
        CGContextSetRGBStrokeColor( mContentRef, (color>>16&0xFF)/256.0, (color>>8&0xFF)/256.0, (color&0xFF)/256.0, (color>>24&0xFF)/256.0 );
    }
}

void CHE_GraphicsDrawer::SetLineCap( const GRAPHICS_STATE_LINECAP & lineCap )
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

void CHE_GraphicsDrawer::SetLineJoin( const GRAPHICS_STATE_LINEJOIN & lineJion )
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

void CHE_GraphicsDrawer::SetLineDash( const GRAPHICS_STATE_DASHPATTERN & dashPattern )
{
    CGFloat * pCGFloatArray = NULL;
    if ( dashPattern.dashArray.size() > 0 )
    {
        pCGFloatArray = new CGFloat[dashPattern.dashArray.size()];
        for ( size_t i = 0 ; i < dashPattern.dashArray.size(); ++i )
        {
            pCGFloatArray[i] = dashPattern.dashArray[i];
        }
        if ( mContentRef )
        {
            CGContextSetLineDash( mContentRef, dashPattern.dashPhase, pCGFloatArray, dashPattern.dashArray.size() );
        }
        if ( pCGFloatArray ) {
            delete [] pCGFloatArray;
        }
    }
}

void CHE_GraphicsDrawer::SetFillMode( GRAPHICS_STATE_FILLMODE mode )
{
    mFillMode = mode;
}

void CHE_GraphicsDrawer::MoveTo( FLOAT x, FLOAT y )
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

void CHE_GraphicsDrawer::LineTo( FLOAT x, FLOAT y )
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

void CHE_GraphicsDrawer::CurveTo( FLOAT x1, FLOAT y1, FLOAT x2, FLOAT y2, FLOAT x3, FLOAT y3 )
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

void CHE_GraphicsDrawer::Rectangle( FLOAT x, FLOAT y, FLOAT width, FLOAT height )
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

void CHE_GraphicsDrawer::ClosePath()
{
    if ( mPathRef && !CGPathIsEmpty( mPathRef ) )
    {
        CGPathCloseSubpath( mPathRef );
    }
}

void CHE_GraphicsDrawer::FillPath()
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

void CHE_GraphicsDrawer::StrokePath()
{
    if ( mContentRef && mPathRef && !CGPathIsEmpty( mPathRef ) )
    {
        CGContextAddPath( mContentRef, mPathRef );
        CGContextStrokePath( mContentRef );
        CGPathRelease( mPathRef );
        mPathRef = NULL;
    }
}

void CHE_GraphicsDrawer::FillStrokePath()
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

void CHE_GraphicsDrawer::ClipPath()
{
    if ( mContentRef && mPathRef && !CGPathIsEmpty( mPathRef ) )
    {
        CGContextAddPath( mContentRef, mPathRef );
        CGContextClip( mContentRef );
        CGPathRelease( mPathRef );
        mPathRef = NULL;
    }
}

void CHE_GraphicsDrawer::FillClipPath()
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

void CHE_GraphicsDrawer::StrokeClipPath()
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

void CHE_GraphicsDrawer::FillStrokeClipPath()
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

void CHE_GraphicsDrawer::ResetClip()
{
    if ( mContentRef )
    {
        //todo
        //CGPathRelease( mPathRef );
        //mPathRef = NULL;
        //CGContextClip(mContentRef)
    }
}

void CHE_GraphicsDrawer::DrawBitmap( CHE_Bitmap * pBitmap )
{
    if ( pBitmap && mContentRef )
    {
        CGDataProviderRef dataRef = CGDataProviderCreateWithData( NULL, pBitmap->GetBuffer(), pBitmap->GetMemBitmapDataSize(), NULL );
        
        CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
        
        CGImageRef imageRef = CGImageCreate( pBitmap->Width(), pBitmap->Height(), 8, pBitmap->Depth(),	
                                             pBitmap->Pitch(), colorSpace, kCGImageAlphaNone,
                                            dataRef, NULL, false, kCGRenderingIntentDefault );
        
        CHE_Matrix tmpMatrix;
        tmpMatrix = mMatrix;
        tmpMatrix.Concat( mExtMatrix );

        CHE_Rect rect;
        rect.left = 0;
        rect.bottom = 0;
        rect.width = 1;
        rect.height = 1;
        rect = tmpMatrix.Transform( rect );
        
        CGContextDrawImage( mContentRef, CGRectMake( rect.left, rect.bottom, rect.width, rect.height), imageRef );
        
        CGImageRelease( imageRef );
    }
}

void CHE_GraphicsDrawer::SetTextFont( PBYTE fontData, size_t dataSize )
{
    CFDataRef dataRef = CFDataCreateWithBytesNoCopy( kCFAllocatorDefault, fontData, dataSize, kCFAllocatorNull );
    if ( dataRef )
    {
        CGDataProviderRef dataProviderRef = CGDataProviderCreateWithCFData( dataRef );
        CGFontRef fontRef = CGFontCreateWithDataProvider( dataProviderRef );
        CGContextSetFont( mContentRef, fontRef );
    }
    
    //CFMutableStringRef fontName = CFStringCreateMutable( kCFAllocatorDefault, 100 );
    //CFStringAppendCString( fontName, "Arial", kCFStringEncodingASCII);
    //CGFontRef cgFont = CGFontCreateWithFontName( fontName );
    //CGContextSetFont( mContentRef, cgFont );
}

void CHE_GraphicsDrawer::SetTextMatrix( CHE_Matrix textMatrix )
{
    mTextMatrix = textMatrix;
}

void CHE_GraphicsDrawer::DrawText( unsigned short gid )
{
    CGPoint position;
    position.x = 0;
    position.y = 0;
    CGContextSetFontSize( mContentRef , 1 );
    
    CHE_Matrix tmpMatrix;
    tmpMatrix = mTextMatrix;
    tmpMatrix.Concat( mExtMatrix );
    
    CGContextSetTextMatrix( mContentRef, CGAffineTransformMake( tmpMatrix.a, tmpMatrix.b, tmpMatrix.c, tmpMatrix.d, tmpMatrix.e, tmpMatrix.f));
    CGContextShowGlyphsAtPositions( mContentRef, &gid, &position, 1 );
}

void CHE_GraphicsDrawer::SaveToFile( const char * pPath )
{
    CGImageRef imageRef = CGBitmapContextCreateImage( mContentRef );
    if ( imageRef == nil )
    {
        return;
    }
    
    CFStringRef strTypeRef = CFStringCreateWithCString( kCFAllocatorDefault , "public.png", kCFStringEncodingASCII );
    
    CFStringRef strPathRef = CFStringCreateWithCString( kCFAllocatorDefault , pPath, kCFStringEncodingASCII );
    
    CFURLRef urlRef = CFURLCreateWithFileSystemPath( kCFAllocatorDefault , strPathRef, kCFURLPOSIXPathStyle, FALSE );
    
    CGImageDestinationRef destRef = CGImageDestinationCreateWithURL( urlRef, strTypeRef, 1, nil );
    
    CGImageDestinationAddImage( destRef, imageRef, nil );
    
    CGImageDestinationFinalize( destRef );
    
    CFRelease( destRef );
}