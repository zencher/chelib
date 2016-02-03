#import "../../include/pdf/che_pdf_renderer_macosx.h"
#import <CoreGraphics/CGPattern.h>
#include "../../include/che_bitmap.h"

void TilingDrawCallBack( void *info, CGContextRef c )
{
    if ( c == NULL )
    {
        return;
    }
    
    CHE_PDF_Tiling * pTiling = (CHE_PDF_Tiling*)( info );
    if ( pTiling )
    {
        CHE_PDF_Renderer render( c );
        render.RenderTiling( pTiling->GetList(), pTiling->IsColored() );
    }
}


CHE_PDF_Renderer::CHE_PDF_Renderer( CGContextRef cgContext )
    : mFillMode(FillMode_Nonzero),mContextRef(cgContext), mPathRef(NULL),
mFillColorSpace(NULL), mStrokeColorSpace(NULL), mImageColorSpace(NULL), mPosiX(0), mPosiY(0)
{
}

CHE_PDF_Renderer::~CHE_PDF_Renderer()
{
    if ( mPathRef )
    {
        CGPathRelease( mPathRef );
        mPathRef = NULL;
    }
    if ( mFillColorSpace )
    {
        CGColorSpaceRelease( mFillColorSpace );
        mFillColorSpace = NULL;
    }
    if ( mStrokeColorSpace )
    {
        CGColorSpaceRelease( mStrokeColorSpace );
        mStrokeColorSpace = NULL;
    }
    if ( mImageColorSpace )
    {
        CGColorSpaceRelease( mImageColorSpace );
        mImageColorSpace = NULL;
    }
}

HE_VOID CHE_PDF_Renderer::SetPosition( HE_FLOAT x, HE_FLOAT y )
{
    mPosiX = x;
    mPosiY = y;
}

HE_VOID	CHE_PDF_Renderer::SetMatrix( const CHE_Matrix & matrix )
{
    mMatrix = matrix;
}

HE_VOID	CHE_PDF_Renderer::SetExtMatrix( const CHE_Matrix & matrix )
{
    mExtMatrix = matrix;
}

HE_VOID	CHE_PDF_Renderer::SetLineWidth( const HE_FLOAT & lineWidth )
{
    if ( mContextRef )
    {
        CGContextSetLineWidth( mContextRef, lineWidth );
    }
}

HE_VOID	CHE_PDF_Renderer::SetMiterLimit( const HE_FLOAT & miterLimit )
{
    if ( mContextRef )
    {
        CGContextSetMiterLimit( mContextRef, miterLimit );
    }
}

HE_VOID	CHE_PDF_Renderer::SetLineCap( const GRAPHICS_STATE_LINECAP & lineCap )
{
    if ( mContextRef )
    {
        switch ( lineCap )
		{
            case LineCap_Butt:
                CGContextSetLineCap( mContextRef, kCGLineCapButt );
                break;
            case LineCap_Round:
                CGContextSetLineCap( mContextRef, kCGLineCapRound );
                break;
            case LineCap_Square:
                CGContextSetLineCap( mContextRef, kCGLineCapSquare );
                break;
            default:
                break;
		}
    }

}

HE_VOID	CHE_PDF_Renderer::SetLineJoin( const GRAPHICS_STATE_LINEJOIN & lineJion )
{
    if ( mContextRef )
	{
		switch ( lineJion )
		{
            case LineJoin_Miter:
                CGContextSetLineJoin( mContextRef, kCGLineJoinMiter );
                break;
            case LineJoin_Round:
                CGContextSetLineJoin( mContextRef, kCGLineJoinRound );
                break;
            case LineJoin_Bevel:
                CGContextSetLineJoin( mContextRef, kCGLineJoinBevel );
                break;
            default:
                break;
		}
	}
}

HE_VOID	CHE_PDF_Renderer::SetLineDash( const GRAPHICS_STATE_DASHPATTERN & dashPattern )
{
    if ( dashPattern.dashArray.size() > 0 )
    {
        CGFloat * pCGFloatArray = new CGFloat[dashPattern.dashArray.size()];
        for ( size_t i = 0 ; i < dashPattern.dashArray.size(); ++i )
        {
            pCGFloatArray[i] = dashPattern.dashArray[i];
        }
        if ( mContextRef )
        {
            CGContextSetLineDash( mContextRef, dashPattern.dashPhase, pCGFloatArray, dashPattern.dashArray.size() );
        }
        delete [] pCGFloatArray;
    }
}

HE_VOID	CHE_PDF_Renderer::SetFillMode( GRAPHICS_STATE_FILLMODE mode )
{
    mFillMode = mode;
}

HE_VOID CHE_PDF_Renderer::SetBlendMode( GRAPHICS_STATE_BLENDMODE mode )
{
    switch ( mode )
    {
        case BlendMode_Normal:
            CGContextSetBlendMode( mContextRef, kCGBlendModeNormal );
            break;
        case BlendMode_Multiply:
            CGContextSetBlendMode( mContextRef, kCGBlendModeMultiply );
            break;
        case BlendMode_Screen:
            CGContextSetBlendMode( mContextRef, kCGBlendModeScreen );
            break;
        case BlendMode_Overlay:
            CGContextSetBlendMode( mContextRef, kCGBlendModeOverlay );
            break;
        case BlendMode_Darken:
            CGContextSetBlendMode( mContextRef, kCGBlendModeDarken );
            break;
        case BlendMode_Lighten:
            CGContextSetBlendMode( mContextRef, kCGBlendModeLighten );
            break;
        case BlendMode_ColorDodge:
            CGContextSetBlendMode( mContextRef, kCGBlendModeColorDodge );
            break;
        case BlendMode_ColorBurn:
            CGContextSetBlendMode( mContextRef, kCGBlendModeColorBurn );
            break;
        case BlendMode_HardLight:
            CGContextSetBlendMode( mContextRef, kCGBlendModeHardLight );
            break;
        case BlendMode_SoftLight:
            CGContextSetBlendMode( mContextRef, kCGBlendModeSoftLight );
            break;
        case BlendMode_Difference:
            CGContextSetBlendMode( mContextRef, kCGBlendModeDifference );
            break;
        case BlendMode_Exclusion:
            CGContextSetBlendMode( mContextRef, kCGBlendModeExclusion );
            break;
        case BlendMode_Hue:
            CGContextSetBlendMode( mContextRef, kCGBlendModeHue );
            break;
        case BlendMode_Saturation:
            CGContextSetBlendMode( mContextRef, kCGBlendModeSaturation );
            break;
        case BlendMode_Color:
            CGContextSetBlendMode( mContextRef, kCGBlendModeColor );
            break;
        case BlendMode_Luminosity:
            CGContextSetBlendMode( mContextRef, kCGBlendModeLuminosity );
            break;
        default:
            break;
    }
}

HE_VOID CHE_PDF_Renderer::SetTextFont( CGFontRef font )
{
    if ( mContextRef )
    {
        CGContextSetFont( mContextRef, font );
    }
}

HE_VOID CHE_PDF_Renderer::SetTextMatrix( CHE_Matrix textMatrix )
{
    mTextMatrix = textMatrix;
}

HE_VOID	CHE_PDF_Renderer::SetFillColor( const HE_ULONG & color )
{
    if ( mContextRef )
    {
        CGContextSetRGBFillColor( mContextRef , (color>>16&0xFF)/256.0, (color>>8&0xFF)/256.0, (color&0xFF)/256.0, (color>>24&0xFF)/256.0 );
    }
}

HE_VOID	CHE_PDF_Renderer::SetStrokeColor( const HE_ULONG & color )
{
    if ( mContextRef )
    {
        CGContextSetRGBStrokeColor( mContextRef , (color>>16&0xFF)/256.0, (color>>8&0xFF)/256.0, (color&0xFF)/256.0, (color>>24&0xFF)/256.0 );
    }
}

HE_VOID CHE_PDF_Renderer::SetFillColor( const CHE_PDF_Color & color )
{
    CGFloat val[5];
    int i = 0;
    for ( ; i < color.GetComponentCount(); ++i )
    {
        val[i] = color.GetComponent( i );
    }
    val[i] = 1.0f;
    
    if ( mFillPattern ) {
        if ( mFillPatternColored )
        {
            val[0] = 1.0f;
        }
        CGContextSetFillPattern( mContextRef, mFillPattern, val );
        CGPatternRelease(mFillPattern);
    }else{
        CGContextSetFillColor( mContextRef, val );
    }
}

HE_VOID CHE_PDF_Renderer::SetStrokeColor( const CHE_PDF_Color & color )
{
    CGFloat val[5];
    int i = 0;
    for ( ; i < color.GetComponentCount(); ++i )
    {
        val[i] = color.GetComponent( i );
    }
    val[i] = 1.0f;
    CGContextSetStrokeColor( mContextRef, val );
}

HE_VOID CHE_PDF_Renderer::SetFillColorSpace( const CHE_PDF_ColorSpacePtr & cs )
{
    mFillPattern = NULL;
    if ( mFillColorSpace )
    {
        CGColorSpaceRelease( mFillColorSpace );
        mFillColorSpace = NULL;
    }
    
    if ( cs->GetColorSpaceType() == COLORSPACE_SPECIAL_PATTERN )
    {
        CHE_PDF_CS_PatternPtr pattern = cs->GetPatternPtr();
        if ( pattern )
        {
            CHE_PDF_TilingPtr tilingPtr = pattern->mTiling;
            if ( tilingPtr )
            {
                CHE_Rect rect = tilingPtr->GetBBox();
                CGRect bounds = CGRectMake( rect.left, rect.bottom, rect.width, rect.height );
                
                CHE_Matrix tilingMatrix = tilingPtr->GetMatrix();
                CHE_Matrix matrix;
                if ( mRotate == 90 )
                {
                    CHE_Matrix rotateMatrix = CHE_Matrix::RotateMatrix( mRotate );
                    CHE_Matrix transformMatrix = CHE_Matrix::TranslateMatrix( mPageRect.height, 0 );
                    rotateMatrix.Concat( transformMatrix );
                    matrix.Concat( rotateMatrix );
                }else if ( mRotate == 180 )
                {
                    CHE_Matrix rotateMatrix = CHE_Matrix::RotateMatrix( mRotate );
                    CHE_Matrix transformMatrix = CHE_Matrix::TranslateMatrix( mPageRect.width, mPageRect.height );
                    rotateMatrix.Concat( transformMatrix );
                    matrix.Concat( rotateMatrix );
                }else if ( mRotate == 270 )
                {
                    CHE_Matrix rotateMatrix = CHE_Matrix::RotateMatrix( mRotate );
                    CHE_Matrix transformMatrix = CHE_Matrix::TranslateMatrix( 0, mPageRect.width );
                    rotateMatrix.Concat( transformMatrix );
                    matrix.Concat( rotateMatrix );
                }
                
                CHE_Rect newPageRect = matrix.Transform( mPageRect );
                
                CHE_Matrix bboxClipMatrix;
                bboxClipMatrix.a = 1;
                bboxClipMatrix.b = 0;
                bboxClipMatrix.c = 0;
                bboxClipMatrix.d = 1;
                bboxClipMatrix.e = -newPageRect.left;
                bboxClipMatrix.f = -newPageRect.bottom;
                matrix.Concat( bboxClipMatrix );
                
                /*CHE_Matrix flipMatrix;
                flipMatrix.a = 1;
                flipMatrix.b = 0;
                flipMatrix.c = 0;
                flipMatrix.d = -1;
                flipMatrix.e = 0;
                flipMatrix.f = newPageRect.height;
                matrix.Concat( flipMatrix );*/
                
                CHE_Matrix scaletMatrix;
                scaletMatrix.a = mDpix * mScale / 72;
                scaletMatrix.b = 0;
                scaletMatrix.c = 0;
                scaletMatrix.d = mDpiy * mScale / 72;
                scaletMatrix.e = 0;
                scaletMatrix.f = 0;
                matrix.Concat( scaletMatrix );
                
                CHE_Matrix offsetMatrix;
                offsetMatrix.a = 1;
                offsetMatrix.b = 0;
                offsetMatrix.c = 0;
                offsetMatrix.d = 1;
                offsetMatrix.e = mPosiX + mPatternOffsetX;
                offsetMatrix.f = mPosiY + mPatternOffsetY;
                matrix.Concat( offsetMatrix );
                
                tilingMatrix.Concat( matrix );
                CGAffineTransform m = CGAffineTransformMake( tilingMatrix.a, tilingMatrix.b, tilingMatrix.c, tilingMatrix.d, tilingMatrix.e, tilingMatrix.f );
                
                bool bColored = tilingPtr->IsColored();
                CGColorSpaceRef baseColorSpaceRef = CreateColorSpace( pattern->mUnderLyingColorspace );
                CGColorSpaceRef patternColorSpaceRef = CGColorSpaceCreatePattern( baseColorSpaceRef );
                CGContextSetFillColorSpace( mContextRef, patternColorSpaceRef );
                CGColorSpaceRelease( patternColorSpaceRef );
                if ( baseColorSpaceRef ) {
                    CGColorSpaceRelease( baseColorSpaceRef );
                }
                CGPatternCallbacks callbacks;
                callbacks.version = 0;
                callbacks.drawPattern = TilingDrawCallBack;
                callbacks.releaseInfo = NULL;
                    
                CGPatternTiling type = kCGPatternTilingNoDistortion;
                if ( tilingPtr->GetTilingType() == 1 ) {
                    type = kCGPatternTilingConstantSpacing;
                }else if ( tilingPtr->GetTilingType() == 2 )
                {
                    type = kCGPatternTilingNoDistortion;
                }else if ( tilingPtr->GetTilingType() == 3 )
                {
                    type = kCGPatternTilingConstantSpacingMinimalDistortion;
                }
                    
                mFillPattern = CGPatternCreate( tilingPtr.GetPointer(), bounds, m, tilingPtr->GetXStep(),
                                                tilingPtr->GetYStep(), type, bColored, &callbacks );
                mFillPatternColored = bColored;
            }
        }
    }else{
        mFillColorSpace = CreateColorSpace( cs );
        CGContextSetFillColorSpace( mContextRef , mFillColorSpace );
    }
}

HE_VOID CHE_PDF_Renderer::SetStrokeColorSpace( const CHE_PDF_ColorSpacePtr & cs )
{
    if ( mStrokeColorSpace )
    {
        CGColorSpaceRelease( mStrokeColorSpace );
        mStrokeColorSpace = NULL;
    }
    
    mStrokeColorSpace = CreateColorSpace( cs );
    CGContextSetStrokeColorSpace( mContextRef , mStrokeColorSpace );
}

CGColorSpaceRef CHE_PDF_Renderer::CreateColorSpace( const CHE_PDF_ColorSpacePtr & cs )
{
    CGColorSpaceRef csRef = NULL;
    if ( cs )
    {
        switch ( cs->GetColorSpaceType() )
        {
        case COLORSPACE_DEVICE_GRAY:
            csRef = CGColorSpaceCreateDeviceGray();
            break;
        case COLORSPACE_DEVICE_RGB:
            csRef = CGColorSpaceCreateDeviceRGB();
            break;
        case COLORSPACE_DEVICE_CMYK:
            csRef = CGColorSpaceCreateDeviceCMYK();
            break;
        case COLORSPACE_CIEBASE_CALGRAY:
            {
                CHE_PDF_CS_CalGrayPtr calgraycs = cs->GetCalGrayPtr();
                if ( calgraycs )
                {
                    csRef = CGColorSpaceCreateCalibratedGray( calgraycs->mWhitePoint, calgraycs->mBlackPoint, calgraycs->mGamma );
                }
                break;
            }
        case COLORSPACE_CIEBASE_CALRGB:
            {
                CHE_PDF_CS_CalRGBPtr calrgbcs = cs->GetCalRGBPtr();
                if ( calrgbcs )
                {
                    csRef = CGColorSpaceCreateCalibratedRGB( calrgbcs->mWhitePoint , calrgbcs->mBlackPoint, calrgbcs->mGamma, calrgbcs->mMatrix );
                }
                break;
            }
        case COLORSPACE_CIEBASE_CALLAB:
            {
                CHE_PDF_CS_CalLabPtr labcs = cs->GetCalLabPtr();
                if ( labcs )
                {
                    csRef = CGColorSpaceCreateLab( labcs->mWhitePoint, labcs->mBlackPoint, labcs->mRange );
                }
                break;
            }
        case COLORSPACE_CIEBASE_ICCBASED:
            {
                CHE_PDF_CS_ICCBasedPtr icccs = cs->GetICCBasedPtr();
                if ( icccs )
                {
                    CGColorSpaceRef baseColorSpaceRef = CreateColorSpace( icccs->mAlternate );
                    if ( baseColorSpaceRef )
                    {
                        CGDataProviderRef dataRef = CGDataProviderCreateWithData( NULL, icccs->mStmAcc.GetData(), icccs->mStmAcc.GetSize(), NULL );
                        csRef = CGColorSpaceCreateICCBased( cs->GetComponentCount(), icccs->mRange, dataRef, baseColorSpaceRef );
                        CGDataProviderRelease( dataRef );
                        CGColorSpaceRelease( baseColorSpaceRef );
                    }
                }
                break;
            }
        case COLORSPACE_SPECIAL_INDEXED:
            {
                CHE_PDF_CS_IndexedPtr indexcs = cs->GetIndexedPtr();
                if ( indexcs )
                {
                    CGColorSpaceRef baseColorSpaceRef = CreateColorSpace( indexcs->mBaseColorSpace );
                    if ( baseColorSpaceRef )
                    {
                        csRef = CGColorSpaceCreateIndexed( baseColorSpaceRef, indexcs->mIndexCount, indexcs->mpIndexTable );
                        //CGColorSpaceRelease( baseColorSpaceRef );
                    }
                }
                break;
            }
        case COLORSPACE_SPECIAL_SEPARATION:
            {
                CHE_PDF_CS_SeparationPtr separation = cs->GetSeparationPtr();
                if ( separation )
                {
                    csRef = CreateColorSpace( separation->mBaseColorSpace );
                }
                break;
            }
        default:
            break;
        }

    }
    return csRef;
}

CGImageRef CHE_PDF_Renderer::CreateImage( const CHE_PDF_ImageXObjectPtr & imagePtr )
{
    CGImageRef imgRef = NULL;
    if ( imagePtr )
    {
        CGDataProviderRef dataRef = CGDataProviderCreateWithData( NULL, imagePtr->GetData(), imagePtr->GetSize(), NULL );
        if ( dataRef == NULL )
        {
            return imgRef;
        }
        
        double * pDecode = NULL;
        double decode[] = { 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f };
        
        CHE_PDF_ArrayPtr arrayPtr = imagePtr->GetDecodeArray();
        if ( arrayPtr )
        {
            CHE_PDF_ObjectPtr objPtr;
            for ( unsigned int i = 0; i < arrayPtr->GetCount() && i < 4; ++i )
            {
                objPtr = arrayPtr->GetElement( i, OBJ_TYPE_NUMBER );
                if ( objPtr )
                {
                    decode[i] = objPtr->GetNumberPtr()->GetFloat();
                }
            }
            pDecode = decode;
        }
        
        if ( imagePtr->IsMask() )
        {
            imgRef = CGImageMaskCreate( imagePtr->GetWidth(), imagePtr->GetHeight(), imagePtr->GetBPC(), imagePtr->GetBPC(),
                                       (imagePtr->GetWidth() * imagePtr->GetBPC() + 7)/8,dataRef, pDecode, imagePtr->IsInterpolate() );
            CGDataProviderRelease( dataRef );
                return imgRef;
        }
        
        CHE_PDF_ColorSpacePtr csPtr = imagePtr->GetColorspace();
        
        CGColorRenderingIntent ri = kCGRenderingIntentAbsoluteColorimetric;
        switch ( imagePtr->GetRI() )
        {
            case RI_AbsoluteColorimetric:
                ri = kCGRenderingIntentAbsoluteColorimetric;
                break;
            case RI_RelativeColorimetric:
                ri = kCGRenderingIntentRelativeColorimetric;
                break;
            case RI_Saturation:
                ri = kCGRenderingIntentSaturation;
                break;
            case RI_Perceptual:
                ri = kCGRenderingIntentPerceptual;
                break;
            default:
                break;
        }

        
        HE_ULONG bpc = imagePtr->GetBPC();
        if ( bpc == 0 )
        {
            bpc = imagePtr->GetSize() * 8 / ( imagePtr->GetWidth() * imagePtr->GetHeight() * csPtr->GetComponentCount() );
        }
        CGColorSpaceRef csRef = CreateColorSpace( csPtr );
        imgRef = CGImageCreate( imagePtr->GetWidth(), imagePtr->GetHeight(), bpc, bpc*csPtr->GetComponentCount(),
                               (imagePtr->GetWidth() * csPtr->GetComponentCount() * bpc + 7)/8, csRef,
                               kCGBitmapByteOrderDefault, dataRef, pDecode, imagePtr->IsInterpolate(), ri );
        CGDataProviderRelease( dataRef );
        CGColorSpaceRelease( csRef );
        
        CHE_PDF_ArrayPtr arrPtr = imagePtr->GetColorKeyMask();
        if ( arrPtr ) {
            CGFloat val[8];
            for (HE_ULONG i = 0; i < arrPtr->GetCount(); ++i) {
                val[i] = arrPtr->GetElement(i)->GetNumberPtr()->GetInteger();
            }
            CGImageRef newImgRef = CGImageCreateWithMaskingColors(imgRef, val);
            if ( newImgRef )
            {
                CGImageRelease(imgRef);
                imgRef = newImgRef;
            }
        }
    }
    return imgRef;
}

CGImageRef CHE_PDF_Renderer::CreateImage( CHE_PDF_InlineImage * image )
{
    CGImageRef imgRef = NULL;
    if ( image )
    {
        CGDataProviderRef dataRef = CGDataProviderCreateWithData( NULL, image->GetData(), image->GetDataSize(), NULL );
        if ( dataRef == NULL )
        {
            return imgRef;
        }
        
        double * pDecode = NULL;
        double decode[] = { 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f };
        
        CHE_PDF_ArrayPtr arrayPtr;
        CHE_PDF_ObjectPtr objPtr = image->GetDecode();
        if ( objPtr ) {
            arrayPtr = objPtr->GetArrayPtr();
        }
        if ( arrayPtr )
        {
            for ( unsigned int i = 0; i < arrayPtr->GetCount() && i < 4; ++i )
            {
                objPtr = arrayPtr->GetElement( i, OBJ_TYPE_NUMBER );
                if ( objPtr )
                {
                    decode[i] = objPtr->GetNumberPtr()->GetFloat();
                }
            }
            pDecode = decode;
        }
        
        CHE_PDF_ColorSpacePtr csPtr = image->GetColorspace();
        if ( !csPtr )
        {
            if ( image->IsMask() )
            {
                imgRef = CGImageMaskCreate( image->GetWidth(), image->GetHeight(), image->GetBpc(), image->GetBpc(), (image->GetWidth() * image->GetBpc() + 7)/8, dataRef, pDecode, false );
                CGDataProviderRelease( dataRef );
            }
            return imgRef;
        }
        
        CGColorRenderingIntent ri = kCGRenderingIntentAbsoluteColorimetric;
        /*switch ( imagePtr->GetRI() )
        {
            case RI_AbsoluteColorimetric:
                ri = kCGRenderingIntentAbsoluteColorimetric;
                break;
            case RI_RelativeColorimetric:
                ri = kCGRenderingIntentRelativeColorimetric;
                break;
            case RI_Saturation:
                ri = kCGRenderingIntentSaturation;
                break;
            case RI_Perceptual:
                ri = kCGRenderingIntentPerceptual;
                break;
            default:
                break;
        }*/
        
        
        HE_ULONG bpc = image->GetBpc();
        if ( bpc == 0 )
        {
            bpc = image->GetDataSize() * 8 / ( image->GetWidth() * image->GetHeight() * csPtr->GetComponentCount() );
        }
        CGColorSpaceRef csRef = CreateColorSpace( csPtr );
        imgRef = CGImageCreate( image->GetWidth(), image->GetHeight(), bpc, bpc * csPtr->GetComponentCount(),
                               (image->GetWidth() * csPtr->GetComponentCount() * bpc + 7)/8, csRef,
                               kCGBitmapByteOrderDefault, dataRef, pDecode, false, ri );
        CGDataProviderRelease( dataRef );
        CGColorSpaceRelease( csRef );
    }
    return imgRef;
}

HE_VOID CHE_PDF_Renderer::SetImageColorSpace( const CHE_PDF_ColorSpacePtr & cs )
{
    if ( mImageColorSpace )
    {
        CGColorSpaceRelease( mImageColorSpace );
        mImageColorSpace = NULL;
    }
    mImageColorSpace = CreateColorSpace( cs );
}

HE_VOID	CHE_PDF_Renderer::MoveTo( HE_FLOAT x, HE_FLOAT y )
{
    if ( mPathRef == NULL )
    {
        mPathRef = CGPathCreateMutable();
    }
    CGAffineTransform affine = CGAffineTransformIdentity;
    CGPathMoveToPoint( mPathRef, &affine, x, y );
}

HE_VOID	CHE_PDF_Renderer::LineTo( HE_FLOAT x, HE_FLOAT y )
{
    if ( mPathRef )
    {
        CGAffineTransform affine = CGAffineTransformIdentity;
        CGPathAddLineToPoint( mPathRef, &affine, x, y );
    }
}

HE_VOID	CHE_PDF_Renderer::CurveTo( HE_FLOAT x1, HE_FLOAT y1, HE_FLOAT x2, HE_FLOAT y2, HE_FLOAT x3, HE_FLOAT y3 )
{
    if ( mPathRef )
    {
        CGAffineTransform affine = CGAffineTransformIdentity;
        CGPathAddCurveToPoint( mPathRef, &affine, x1, y1, x2, y2, x3, y3 );
    }
}

HE_VOID CHE_PDF_Renderer::Rectangle( HE_FLOAT x, HE_FLOAT y, HE_FLOAT width, HE_FLOAT height )
{
    if ( mPathRef == NULL )
    {
        mPathRef = CGPathCreateMutable();
    }
    if ( mPathRef )
    {
        CGAffineTransform affine = CGAffineTransformIdentity;
        CGPathAddRect( mPathRef, &affine, CGRectMake( x, y, width, height ) );
    }
}

HE_VOID	CHE_PDF_Renderer::ClosePath()
{
    if ( mPathRef && !CGPathIsEmpty( mPathRef ) )
    {
        CGPathCloseSubpath( mPathRef );
    }
}

HE_VOID	CHE_PDF_Renderer::FillPath()
{
    if ( mContextRef && mPathRef && !CGPathIsEmpty( mPathRef ) )
    {
        CGContextAddPath( mContextRef, mPathRef );
        if ( mFillMode == FillMode_Nonzero )
        {
            CGContextFillPath( mContextRef );
        }else{
            CGContextEOFillPath( mContextRef );
        }
        CGPathRelease( mPathRef );
        mPathRef = NULL;
    }
}

HE_VOID	CHE_PDF_Renderer::StrokePath()
{
    if ( mContextRef && mPathRef && !CGPathIsEmpty( mPathRef ) )
    {
        CGContextAddPath( mContextRef, mPathRef );
        CGContextStrokePath( mContextRef );
        CGPathRelease( mPathRef );
        mPathRef = NULL;
    }
}

HE_VOID	CHE_PDF_Renderer::FillStrokePath()
{
    if ( mContextRef && mPathRef && !CGPathIsEmpty( mPathRef ) )
    {
        CGContextAddPath( mContextRef, mPathRef );
        if ( mFillMode == FillMode_Nonzero )
        {
            CGContextFillPath( mContextRef );
        }else{
            CGContextEOFillPath( mContextRef );
        }
        CGContextAddPath( mContextRef, mPathRef );
        CGContextStrokePath( mContextRef );
        CGPathRelease( mPathRef );
        mPathRef = NULL;
    }
}

HE_VOID	CHE_PDF_Renderer::ClipPath()
{
    if ( mContextRef && mPathRef && !CGPathIsEmpty( mPathRef ) )
    {
        CGContextAddPath( mContextRef, mPathRef );
        CGContextClip( mContextRef );
        CGPathRelease( mPathRef );
        mPathRef = NULL;
    }
}

HE_VOID	CHE_PDF_Renderer::FillClipPath()
{
    if ( mContextRef && mPathRef && !CGPathIsEmpty( mPathRef ) )
    {
        CGContextAddPath( mContextRef, mPathRef );
        if ( mFillMode == FillMode_Nonzero )
        {
            CGContextFillPath( mContextRef );
        }else{
            CGContextEOFillPath( mContextRef );
        }
        CGContextAddPath( mContextRef, mPathRef );
        CGContextClip( mContextRef );
        CGPathRelease( mPathRef );
        mPathRef = NULL;
    }
}

HE_VOID	CHE_PDF_Renderer::StrokeClipPath()
{
    if ( mContextRef && mPathRef && !CGPathIsEmpty( mPathRef ) )
    {
        CGContextAddPath( mContextRef, mPathRef );
        CGContextStrokePath( mContextRef );
        CGContextAddPath( mContextRef, mPathRef );
        CGContextClip( mContextRef );
        CGPathRelease( mPathRef );
        mPathRef = NULL;
    }
}

HE_VOID	CHE_PDF_Renderer::FillStrokeClipPath()
{
    if ( mContextRef && mPathRef && !CGPathIsEmpty( mPathRef ) )
    {
        CGContextAddPath( mContextRef, mPathRef );
        if ( mFillMode == FillMode_Nonzero )
        {
            CGContextFillPath( mContextRef );
        }else{
            CGContextEOFillPath( mContextRef );
        }
        CGContextAddPath( mContextRef, mPathRef );
        CGContextStrokePath( mContextRef );
        CGContextAddPath( mContextRef, mPathRef );
        CGContextClip( mContextRef );
        CGPathRelease( mPathRef );
        mPathRef = NULL;
    }
}

HE_VOID CHE_PDF_Renderer::StoreGState()
{
    CGContextSaveGState( mContextRef );
}

HE_VOID CHE_PDF_Renderer::RestoreGState()
{
    CGContextRestoreGState( mContextRef );
}

HE_VOID CHE_PDF_Renderer::SetCommonGState( CHE_PDF_GState * pGState, HE_BOOL bColored )
{
    if ( pGState == NULL )
    {
        return;
    }
    
    static HE_FLOAT val = 0;
	static CHE_Matrix matrix;
	static GRAPHICS_STATE_LINECAP linCap = LineCap_Butt;
	static GRAPHICS_STATE_LINEJOIN lineJoin = LineJoin_Miter;
	static GRAPHICS_STATE_DASHPATTERN lineDash;
	static CHE_PDF_Color fillColor;
	static CHE_PDF_Color strokeColor;
	static CHE_PDF_ColorSpacePtr fillColorSpace;
	static CHE_PDF_ColorSpacePtr strokeColorSpace;
    
	pGState->GetLineWidth( val );
    SetLineWidth( val );
    
	pGState->GetLineCap( linCap );
	SetLineCap( linCap );
    
	pGState->GetLineJoin( lineJoin );
	SetLineJoin( lineJoin );
    
	pGState->GetLineDash( lineDash );
	SetLineDash( lineDash );
    
    SetFillMode( FillMode_Nonzero );
    
	matrix = pGState->GetMatrix();
	SetMatrix( matrix );
    
    mTmpMatrix = mMatrix;
    mTmpMatrix.Concat( mExtMatrix );
    CGContextConcatCTM( mContextRef, CGAffineTransformMake( mTmpMatrix.a, mTmpMatrix.b, mTmpMatrix.c, mTmpMatrix.d, mTmpMatrix.e, mTmpMatrix.f) );

    if ( !bColored )
    {
        return;
    }
	pGState->GetFillColor( fillColor );
	pGState->GetStrokeColor( strokeColor );
	pGState->GetFillColorSpace( fillColorSpace );
	pGState->GetStrokeColorSpace( strokeColorSpace );
    
    SetFillColorSpace( fillColorSpace );
    SetStrokeColorSpace( strokeColorSpace );
    
    if ( fillColorSpace->GetColorSpaceType() == COLORSPACE_SPECIAL_SEPARATION )
    {
        CHE_PDF_CS_SeparationPtr cs = fillColorSpace->GetSeparationPtr();
        if ( cs )
        {
            CHE_PDF_FunctionPtr funcPtr = cs->mFunction;
            if ( funcPtr )
            {
                std::vector<HE_FLOAT> input;
                std::vector<HE_FLOAT> output;
                for ( size_t i = 0 ; i < fillColor.GetComponentCount(); ++i )
                {
                    input.push_back( fillColor.GetComponent( i ) );
                }
                funcPtr->Calculate( input, output );
                fillColor.Clear();
                for ( size_t j = 0; j < output.size(); ++j )
                {
                    fillColor.Push( output[j] );
                }
            }
        }
    }
    
    if ( strokeColorSpace->GetColorSpaceType() == COLORSPACE_SPECIAL_SEPARATION )
    {
        CHE_PDF_CS_SeparationPtr cs = strokeColorSpace->GetSeparationPtr();
        if ( cs )
        {
            CHE_PDF_FunctionPtr funcPtr = cs->mFunction;
            if ( funcPtr )
            {
                std::vector<HE_FLOAT> input;
                std::vector<HE_FLOAT> output;
                for ( size_t i = 0 ; i < strokeColor.GetComponentCount(); ++i )
                {
                    input.push_back( strokeColor.GetComponent( i ) );
                }
                funcPtr->Calculate( input, output );
                strokeColor.Clear();
                for ( size_t j = 0; j < output.size(); ++j )
                {
                    strokeColor.Push( output[j] );
                }
            }
        }
    }
    
    SetFillColor( fillColor );
    SetStrokeColor( strokeColor );
}

HE_VOID CHE_PDF_Renderer::SetExtGState( CHE_PDF_ExtGStateStack * pExtGState )
{
    if ( pExtGState )
    {
        GRAPHICS_STATE_BLENDMODE blendMode = pExtGState->GetBlendMode();
        SetBlendMode( blendMode );
    }
}

HE_VOID CHE_PDF_Renderer::SetClipState( CHE_PDF_ClipState * pClipState )
{
    if ( pClipState == NULL )
    {
        return;
    }
    CHE_PDF_ContentObject * pObj = NULL;
	std::list<CHE_PDF_ClipStateItem*>::iterator it = pClipState->mClipElementList.begin();
	for ( ; it != pClipState->mClipElementList.end(); it++ )
	{
		pObj = (*it)->GetElement();
		if ( !pObj )
		{
			continue;
		}
        CHE_PDF_GState * pGState = pObj->GetGState();
        if ( pGState )
        {
            SetCommonGState( pGState );
        }
		if ( pObj->GetType() == ContentType_Path )
		{
            CHE_PDF_Path * pPath = (CHE_PDF_Path*)(pObj);
            CHE_Point p1, p2, p3;
            for ( size_t i = 0; i < pPath->mItems.size(); ++i )
            {
                switch ( pPath->mItems[i].type )
                {
                case PathItem_MoveTo:
                    p1.x = pPath->mItems[i+1].value;
                    p1.y = pPath->mItems[i+2].value;
                    MoveTo( p1.x, p1.y );
                    i+=2;
                    break;
                case PathItem_LineTo:
                    p1.x = pPath->mItems[i+1].value;
                    p1.y = pPath->mItems[i+2].value;
                    LineTo( p1.x, p1.y );
                    i+=2;
                    break;
                case PathItem_CurveTo:
                    p1.x = pPath->mItems[i+1].value;
                    p1.y = pPath->mItems[i+2].value;
                    p2.x = pPath->mItems[i+3].value;
                    p2.y = pPath->mItems[i+4].value;
                    p3.x = pPath->mItems[i+5].value;
                    p3.y = pPath->mItems[i+6].value;
                    CurveTo( p1.x, p1.y, p2.x, p2.y, p3.x, p3.y );
                    i+=6;
                    break;
                case PathItem_Rectangle:
                    p1.x = pPath->mItems[i+1].value;
                    p1.y = pPath->mItems[i+2].value;
                    MoveTo( p1.x, p1.y );
                    LineTo( p1.x + pPath->mItems[i+3].value, p1.y );
                    LineTo( p1.x + pPath->mItems[i+3].value, p1.y + pPath->mItems[i+4].value );
                    LineTo( p1.x, p1.y + pPath->mItems[i+4].value );
                    ClosePath();
                    i+=4;
                    break;
                case PathItem_Close:
                    ClosePath();
                    break;
                default:
                    break;
				}
			}
        }
        else if ( pObj->GetType() == ContentType_Text )
        {
            CHE_PDF_Text * pText = (CHE_PDF_Text*)(pObj);
            for ( size_t i = 0; i < pText->mItems.size(); ++i )
            {
                CHE_PDF_Path * pPath = pText->GetGraphPath( i );
                if ( pPath )
                {
                    for ( HE_INT32 i = 0; i < pPath->mItems.size(); ++i )
                    {
                        switch ( pPath->mItems[i].type )
                        {
                        case PathItem_MoveTo:
                            MoveTo( pPath->mItems[i+1].value, pPath->mItems[i+2].value );
                            i+=2;
                            break;
                        case PathItem_LineTo:
                            LineTo( pPath->mItems[i+1].value, pPath->mItems[i+2].value );
                            i+=2;
                            break;
                        case PathItem_CurveTo:
                            CurveTo(   pPath->mItems[i+1].value, pPath->mItems[i+2].value,
                                        pPath->mItems[i+3].value, pPath->mItems[i+4].value,
                                        pPath->mItems[i+5].value, pPath->mItems[i+6].value );
                            i+=6;
                            break;
                        case PathItem_Rectangle:
                            MoveTo( pPath->mItems[i+1].value, pPath->mItems[i+2].value );
                            LineTo( pPath->mItems[i+1].value + pPath->mItems[i+3].value, pPath->mItems[i+2].value );
                            LineTo( pPath->mItems[i+1].value + pPath->mItems[i+3].value, pPath->mItems[i+2].value + pPath->mItems[i+4].value );
                            LineTo( pPath->mItems[i+1].value, pPath->mItems[i+2].value + pPath->mItems[i+4].value );
                            ClosePath();
                            i+=4;
                            break;
                        case PathItem_Close:
                            ClosePath();
                            i+=1;
                            break;
                        default:
                            break;
                        }
                    }
                    ClosePath();
                    pPath->GetAllocator()->Delete( pPath );
                }
            }
        }
        ClipPath();
				        
        CHE_Matrix invertMatrix;
        invertMatrix.Invert( mTmpMatrix );
        CGContextConcatCTM( mContextRef, CGAffineTransformMake( invertMatrix.a, invertMatrix.b, invertMatrix.c, invertMatrix.d, invertMatrix.e, invertMatrix.f) );
	}
}

HE_VOID CHE_PDF_Renderer::DrawPath( CHE_PDF_Path * pPath )
{
    if ( pPath->GetFillMode() == Mode_Nonzero )
    {
        SetFillMode( FillMode_Nonzero );
    }else{
        SetFillMode( FillMode_EvenOdd );
    }
    
    CHE_Point p1, p2, p3;
    for ( size_t i = 0; i < pPath->mItems.size(); ++i )
    {
        switch ( pPath->mItems[i].type )
        {
        case PathItem_MoveTo:
            p1.x = pPath->mItems[i+1].value;
            p1.y = pPath->mItems[i+2].value;
            MoveTo( p1.x, p1.y );
            i+=2;
            break;
        case PathItem_LineTo:
            p1.x = pPath->mItems[i+1].value;
            p1.y = pPath->mItems[i+2].value;
            LineTo( p1.x, p1.y );
            i+=2;
            break;
        case PathItem_CurveTo:
            p1.x = pPath->mItems[i+1].value;
            p1.y = pPath->mItems[i+2].value;
            p2.x = pPath->mItems[i+3].value;
            p2.y = pPath->mItems[i+4].value;
            p3.x = pPath->mItems[i+5].value;
            p3.y = pPath->mItems[i+6].value;
            CurveTo( p1.x, p1.y, p2.x, p2.y, p3.x, p3.y );
            i+=6;
            break;
        case PathItem_Rectangle:
            p1.x = pPath->mItems[i+1].value;
            p1.y = pPath->mItems[i+2].value;
            MoveTo( p1.x, p1.y );
            LineTo( p1.x + pPath->mItems[i+3].value, p1.y );
            LineTo( p1.x + pPath->mItems[i+3].value, p1.y + pPath->mItems[i+4].value );
            LineTo( p1.x, p1.y + pPath->mItems[i+4].value );
            ClosePath();
            i+=4;
            break;
        case PathItem_Close:
            ClosePath();
            break;
        default: break;
        }
    }
    switch ( pPath->GetPaintType() )
    {
    case Paint_Fill:
        FillPath();
        break;
    case Paint_Stroke:
        StrokePath();
        break;
    case Paint_FillStroke:
        FillStrokePath();
        break;
    case Paint_None:
    default:
        break;
    }
}

HE_VOID CHE_PDF_Renderer::DrawTextGlyph( CGGlyph gid )
{
    StoreGState();
    CGPoint position;
    position.x = 0;
    position.y = 0;
    CGContextSetFontSize( mContextRef , 1 );
    CGContextSetTextMatrix( mContextRef, CGAffineTransformMake( mTextMatrix.a, mTextMatrix.b, mTextMatrix.c, mTextMatrix.d, mTextMatrix.e, mTextMatrix.f) );
    CGContextShowGlyphsAtPositions( mContextRef, &gid, &position, 1 );
    RestoreGState();
}

HE_VOID CGFontCleanCallBack( HE_LPVOID info )
{
    if ( info )
    {
        CGFontRef fontRef = (CGFontRef)info;
        CGFontRelease( fontRef );
    }
}

HE_VOID CHE_PDF_Renderer::DrawText( CHE_PDF_Text * pText )
{
    GRAPHICS_STATE_TEXTRENDERMODE rm = TextRenderMode_Fill;
    CHE_PDF_GState * pGState = pText->GetGState();
    if ( pGState )
    {
        pGState->GetTextRenderMode( rm );
    }
    switch( rm )
    {
        case TextRenderMode_Fill:
            CGContextSetTextDrawingMode( mContextRef, kCGTextFill );
            break;
        case TextRenderMode_Stroke:
            CGContextSetTextDrawingMode( mContextRef, kCGTextStroke );
            break;
        case TextRenderMode_FillStroke:
            CGContextSetTextDrawingMode( mContextRef, kCGTextFillStroke );
            break;
        case TextRenderMode_Invisible:
            CGContextSetTextDrawingMode( mContextRef, kCGTextInvisible );
            break;
        case TextRenderMode_FillClip:
            CGContextSetTextDrawingMode( mContextRef, kCGTextFillClip );
            break;
        case TextRenderMode_StrokeClip:
            CGContextSetTextDrawingMode( mContextRef, kCGTextStrokeClip );
            break;
        case TextRenderMode_FillStrokeClip:
            CGContextSetTextDrawingMode( mContextRef, kCGTextFillStrokeClip );
            break;
        case TextRenderMode_Clip:
            CGContextSetTextDrawingMode( mContextRef, kCGTextClip );
            break;
        default:break;
    }
    switch( rm )
    {
        case TextRenderMode_Fill:
        case TextRenderMode_FillClip:
        case TextRenderMode_FillStroke:
        case TextRenderMode_Stroke:
        case TextRenderMode_StrokeClip:
        case TextRenderMode_FillStrokeClip:
        {
            HE_ULONG cgGlyphCount;
            HE_ULONG ftGlyphCount;
            CGFontRef cgfontRef = NULL;
            
            //对于Fill类型的文本输出，可以使用系统原生文本输出接口，以获得次像素支
            CHE_PDF_Font * pFont = pText->GetGState()->GetTextFont();
            if ( pFont->GetPlatformFontInfo() != nullptr )
            {
                cgfontRef = (CGFontRef)pFont->GetPlatformFontInfo();
                SetTextFont( cgfontRef );
                cgGlyphCount = CGFontGetNumberOfGlyphs(cgfontRef);
                ftGlyphCount = pFont->GetFTFaceGlyphCount();
            }else{
                if ( pFont->GetEmbededFontSize() )
                {
                    CFDataRef dataRef = CFDataCreateWithBytesNoCopy( kCFAllocatorDefault, pFont->GetEmbededFont(), pFont->GetEmbededFontSize(), kCFAllocatorNull );
                    if ( dataRef )
                    {
                        CGDataProviderRef dataProviderRef = CGDataProviderCreateWithCFData( dataRef );
                        cgfontRef = CGFontCreateWithDataProvider( dataProviderRef );
                        if ( cgfontRef )
                        {
                            cgGlyphCount = CGFontGetNumberOfGlyphs(cgfontRef);
                            ftGlyphCount = pFont->GetFTFaceGlyphCount();
                            pFont->SetPlatformFontInfo( cgfontRef );
                            pFont->SetPlatformFontInfoCleanCallBack( CGFontCleanCallBack );
                            SetTextFont( cgfontRef );
                        }else{
                            //assert(false);
                            return DrawTextAsPath( pText );
                        }
                    }
                }else{
                    CHE_ByteString fontPath = pFont->GetFontPath();
                    if ( fontPath.GetLength() > 0 )
                    {
                        FILE * pFile = fopen( fontPath.GetData(), "rb" );
                        if ( pFile )
                        {
                            fseek( pFile, 0, SEEK_END );
                            long posi = ftell( pFile );
                            unsigned char * data = new unsigned char[posi];
                            fseek( pFile, 0, SEEK_SET);
                            fread( data, 1, posi, pFile);
                            
                            CFDataRef dataRef = CFDataCreateWithBytesNoCopy( kCFAllocatorDefault, data, posi, kCFAllocatorNull );
                            if ( dataRef )
                            {
                                CGDataProviderRef dataProviderRef = CGDataProviderCreateWithCFData( dataRef );
                                cgfontRef = CGFontCreateWithDataProvider( dataProviderRef );
                                if ( cgfontRef )
                                {
                                    cgGlyphCount = CGFontGetNumberOfGlyphs(cgfontRef);
                                    ftGlyphCount = pFont->GetFTFaceGlyphCount();
                                    pFont->SetPlatformFontInfo( cgfontRef );
                                    pFont->SetPlatformFontInfoCleanCallBack( CGFontCleanCallBack );
                                    SetTextFont( cgfontRef );
                                }else{
                                    assert(false);
                                    return ;//DrawTextAsPath( pText );
                                }
                            }
                        }
                    }
                }
            }
            
            char glyphName[128];
            CHE_Matrix textMatirx;
            for ( HE_ULONG i = 0; i < pText->mItems.size(); ++i )
            {
                textMatirx = pText->GetCharMatrix( i );
                SetTextMatrix( textMatirx );
                
                if ( cgGlyphCount == ftGlyphCount )
                {
                    DrawTextGlyph( pText->mItems[i].gid );
                }else{
                    sprintf( glyphName, "cid%ld", pText->mItems[i].gid );
                    CFStringRef glyphNameStrRef = CFStringCreateWithCString( kCFAllocatorDefault, glyphName, kCFStringEncodingASCII );
                    GlyphID gid = CGFontGetGlyphWithGlyphName( cgfontRef, glyphNameStrRef );
                    DrawTextGlyph( gid );
                }
            }
            break;
        }
        default:
            break;
    }
}

HE_VOID CHE_PDF_Renderer::DrawTextAsPath( CHE_PDF_Text * pText )
{
    GRAPHICS_STATE_TEXTRENDERMODE rm = TextRenderMode_Fill;
    CHE_PDF_GState * pGState = pText->GetGState();
    if ( pGState )
    {
        pGState->GetTextRenderMode( rm );
    }
    switch( rm )
    {
    case TextRenderMode_Fill:
    case TextRenderMode_FillClip:
    case TextRenderMode_FillStroke:
    case TextRenderMode_Stroke:
    case TextRenderMode_StrokeClip:
    case TextRenderMode_FillStrokeClip:
        for ( HE_ULONG j = 0; j < pText->mItems.size(); ++j )
        {
            CHE_PDF_Path * pPath = pText->GetGraphPath( j );
            if ( pPath )
            {
                for ( HE_ULONG i = 0; i < pPath->mItems.size(); ++i )
                {
                    switch ( pPath->mItems[i].type )
                    {
                    case PathItem_MoveTo:
                        MoveTo( pPath->mItems[i+1].value, pPath->mItems[i+2].value );
                        i+=2;
                        break;
                    case PathItem_LineTo:
                        LineTo( pPath->mItems[i+1].value, pPath->mItems[i+2].value );
                        i+=2;
                        break;
                    case PathItem_CurveTo:
                        CurveTo(   pPath->mItems[i+1].value, pPath->mItems[i+2].value,
                                    pPath->mItems[i+3].value, pPath->mItems[i+4].value,
                                    pPath->mItems[i+5].value, pPath->mItems[i+6].value );
                        i+=6;
                        break;
                    case PathItem_Rectangle:
                        MoveTo( pPath->mItems[i+1].value, pPath->mItems[i+2].value );
                        LineTo( pPath->mItems[i+1].value + pPath->mItems[i+3].value, pPath->mItems[i+2].value );
                        LineTo( pPath->mItems[i+1].value + pPath->mItems[i+3].value, pPath->mItems[i+2].value + pPath->mItems[i+4].value );
                        LineTo( pPath->mItems[i+1].value, pPath->mItems[i+2].value + pPath->mItems[i+4].value );
                        ClosePath();
                        i+=4;
                        break;
                    case PathItem_Close:
                        ClosePath();
                        i+=1;
                        break;
                    default:
                        break;
                    }
                    pPath->GetAllocator()->Delete( pPath );
                }
            }
        }
    default:
        break;
    }
    switch( rm )
    {
    case TextRenderMode_Fill:
        FillPath();
        break;
    case TextRenderMode_Stroke:
        StrokePath();
        break;
    case TextRenderMode_FillStroke:
        FillStrokePath();
        break;
    case TextRenderMode_Invisible:
        break;
    case TextRenderMode_FillClip:
        FillPath();
        break;
    case TextRenderMode_StrokeClip:
        StrokePath();
        break;
    case TextRenderMode_FillStrokeClip:
        StrokePath();
        break;
    case TextRenderMode_Clip:
        break;
    default:
        break;
    }
}

HE_VOID CHE_PDF_Renderer::DrawComponentRef( CHE_PDF_ComponentRef * cmptRef, const CHE_Matrix & extMatrix )
{
    if ( cmptRef == NULL )
	{
		return;
	}
    
	CHE_PDF_ComponentPtr componentPtr = cmptRef->GetComponentPtr();
    
	switch( componentPtr->GetType() )
	{
        case COMPONENT_TYPE_ImageXObject:
		{
			DrawRefImage( CHE_PDF_ImageXObject::Convert( componentPtr ) );
			break;
		}
        case COMPONENT_TYPE_FormXObject:
		{
            StoreGState();
            
			CHE_Matrix tmpExtMatrix = extMatrix;
			CHE_Matrix newExtMatrix;
			CHE_PDF_GState * pGState = cmptRef->GetGState();
			if ( pGState )
			{
                newExtMatrix = pGState->GetMatrix();
			}
			newExtMatrix.Concat( tmpExtMatrix );
			SetExtMatrix( newExtMatrix );
            DrawForm( CHE_PDF_FormXObject::Convert( componentPtr ), mExtMatrix );
			SetExtMatrix( extMatrix );
            
            RestoreGState();
			break;
		}
        case COMPONENT_TYPE_Shading:
		{
            //DrawShading( )
			break;
		}
        default:
            break;
	}
}

HE_VOID CHE_PDF_Renderer::DrawRefImage( const CHE_PDF_ImageXObjectPtr & image )
{
    if ( image )
    {
        CGImageRef imgRef = CreateImage( image );
        
        CHE_PDF_ImageXObjectPtr maskImagePtr = image->GetMaskImagePtr();
        if ( !maskImagePtr )
        {
            maskImagePtr = image->GetSoftMaskImagePtr();
        }
        if ( maskImagePtr )
        {
            CGImageRef sImgRef = imgRef;
            CGImageRef mImgRef = CreateImage( maskImagePtr );
            imgRef = CGImageCreateWithMask( sImgRef, mImgRef );
            CGImageRelease( sImgRef );
            CGImageRelease( mImgRef );
        }
        
        if ( imgRef )
        {
            CGContextDrawImage( mContextRef, CGRectMake(0, 0, 1, 1), imgRef );
            CGImageRelease( imgRef );
        }
    }
}

HE_VOID CHE_PDF_Renderer::DrawInlineImage( CHE_PDF_InlineImage * pImage )
{
    CGImageRef imgRef = CreateImage( pImage );
    if ( imgRef )
    {
        CGContextDrawImage( mContextRef, CGRectMake(0, 0, 1, 1), imgRef );
        CGImageRelease( imgRef );
    }
}


HE_VOID CHE_PDF_Renderer::DrawShading( const CHE_PDF_ShadingPtr & shading )
{
    
}

HE_VOID CHE_PDF_Renderer::DrawForm( const CHE_PDF_FormXObjectPtr & form, const CHE_Matrix & extMatrix )
{
    CHE_PDF_ContentObjectList & list = form->GetList();
    ContentObjectList::iterator it = list.Begin();
    CHE_PDF_GState * pGState = NULL;
    CHE_PDF_ClipState * pClipState = NULL;
	for ( ; it != list.End(); ++it )
	{
        StoreGState();
		pGState = (*it)->GetGState();
		if ( pGState )
		{
			pClipState = pGState->GetClipState();
			if ( pClipState )
			{
				SetClipState( pClipState );
			}
			SetCommonGState( pGState );
		}
        
		switch ( (*it)->GetType() )
		{
            case ContentType_Path:
			{
				DrawPath( (CHE_PDF_Path*)(*it) );
				break;
			}
            case ContentType_Text:
			{
                DrawText( (CHE_PDF_Text*)(*it) );
				break;
			}
            case ContentType_InlineImage:
			{
				DrawInlineImage( (CHE_PDF_InlineImage*)(*it) );
				break;
			}
            case ContentType_Component:
			{
				DrawComponentRef( (CHE_PDF_ComponentRef*)(*it), mExtMatrix );
				break;
			}
            default:
                break;
		}
        RestoreGState();
	}
}

HE_VOID CHE_PDF_Renderer::DrawContentObjectList( CHE_PDF_ContentObjectList & list, const CHE_Matrix & extMatrix )
{
    ContentObjectList::iterator it = list.Begin();
    //CHE_PDF_GState * pGState = NULL;
    //CHE_PDF_ClipState * pClipState = NULL;
	for ( ; it != list.End(); ++it )
	{
        //StoreGState();
		//pGState = (*it)->GetGState();
		//if ( pGState )
		//{
		//	pClipState = pGState->GetClipState();
		//	if ( pClipState )
		//	{
		//		SetClipState( pClipState );
		//	}
		//	SetCommonGState( pGState );
		//}
        
		switch ( (*it)->GetType() )
		{
            case ContentType_Path:
			{
				DrawPath( (CHE_PDF_Path*)(*it) );
				break;
			}
            case ContentType_Text:
			{
                DrawText( (CHE_PDF_Text*)(*it) );
				break;
			}
            case ContentType_InlineImage:
			{
				DrawInlineImage( (CHE_PDF_InlineImage*)(*it) );
				break;
			}
            case ContentType_Component:
			{
				DrawComponentRef( (CHE_PDF_ComponentRef*)(*it), mExtMatrix );
				break;
			}
            default:
                break;
		}
        //RestoreGState();
	}
}

HE_VOID CHE_PDF_Renderer::Render( CHE_PDF_ContentObjectList & content, CHE_Rect pageRect, HE_UINT32 rotate, HE_FLOAT scale, HE_FLOAT dpix, HE_FLOAT dpiy )
{
    mbNoColor = FALSE;
    mDpix = dpix;
    mDpiy = dpiy;
    mScale = scale;
    mRotate = rotate;
    mPageRect = pageRect;
    
    //计算extMatrix
    CHE_Matrix matrix;
    if ( rotate == 90 )
    {
        CHE_Matrix rotateMatrix = CHE_Matrix::RotateMatrix( rotate );
        CHE_Matrix transformMatrix = CHE_Matrix::TranslateMatrix( pageRect.height, 0 );
        rotateMatrix.Concat( transformMatrix );
        matrix.Concat( rotateMatrix );
    }else if ( rotate == 180 )
    {
        CHE_Matrix rotateMatrix = CHE_Matrix::RotateMatrix( rotate );
        CHE_Matrix transformMatrix = CHE_Matrix::TranslateMatrix( pageRect.width, pageRect.height );
        rotateMatrix.Concat( transformMatrix );
        matrix.Concat( rotateMatrix );
    }else if ( rotate == 270 )
    {
        CHE_Matrix rotateMatrix = CHE_Matrix::RotateMatrix( rotate );
        CHE_Matrix transformMatrix = CHE_Matrix::TranslateMatrix( 0, pageRect.width );
        rotateMatrix.Concat( transformMatrix );
        matrix.Concat( rotateMatrix );
    }
    
    CHE_Rect newPageRect = matrix.Transform( pageRect );
    
    CHE_Matrix bboxClipMatrix;
    bboxClipMatrix.a = 1;
    bboxClipMatrix.b = 0;
    bboxClipMatrix.c = 0;
    bboxClipMatrix.d = 1;
    bboxClipMatrix.e = -newPageRect.left;
    bboxClipMatrix.f = -newPageRect.bottom;
    matrix.Concat( bboxClipMatrix );
    
    CHE_Matrix flipMatrix;
    flipMatrix.a = 1;
    flipMatrix.b = 0;
    flipMatrix.c = 0;
    flipMatrix.d = -1;
    flipMatrix.e = 0;
    flipMatrix.f = newPageRect.height;
    matrix.Concat( flipMatrix );
    
	CHE_Matrix scaletMatrix;
	scaletMatrix.a = dpix * scale / 72;
	scaletMatrix.b = 0;
	scaletMatrix.c = 0;
	scaletMatrix.d = dpiy * scale / 72;
	scaletMatrix.e = 0;
    scaletMatrix.f = 0;
    matrix.Concat( scaletMatrix );
    
	CHE_Matrix offsetMatrix;
    offsetMatrix.e = mPosiX;
    offsetMatrix.f = mPosiY;
    
    matrix.Concat( offsetMatrix );
    SetExtMatrix( matrix );
    
    StoreGState();
    
    //clip当前页面绘制的区域
    newPageRect = matrix.Transform( pageRect );
    CGContextAddRect( mContextRef, CGRectMake( newPageRect.left, newPageRect.bottom, newPageRect.width, newPageRect.height ) );
    CGContextClip( mContextRef );
    
	CHE_PDF_GState * pGState = NULL;
	CHE_PDF_ClipState * pClipState = NULL;
    CHE_PDF_ExtGStateStack * pExtGStateStack = NULL;
	ContentObjectList::iterator it = content.Begin();
	for ( ; it != content.End(); ++it )
	{
        StoreGState();
		pGState = (*it)->GetGState();
		if ( pGState )
		{
			pClipState = pGState->GetClipState();
			if ( pClipState )
			{
				SetClipState( pClipState );
			}
            SetCommonGState( pGState );
            pExtGStateStack = pGState->GetExtGState();
            if ( pExtGStateStack )
            {
                SetExtGState( pExtGStateStack );
            }
		}
		switch ( (*it)->GetType() )
		{
            case ContentType_Path:
				DrawPath( (CHE_PDF_Path*)(*it) );
				break;
            case ContentType_Text:
                DrawText( (CHE_PDF_Text*)(*it) );
                break;
            case ContentType_InlineImage:
				DrawInlineImage( (CHE_PDF_InlineImage*)(*it) );
				break;
            case ContentType_Component:
				DrawComponentRef( (CHE_PDF_ComponentRef*)(*it), mExtMatrix );
                break;
            default: break;
		}
        RestoreGState();
	}
    RestoreGState();
}

HE_VOID CHE_PDF_Renderer::RenderTiling( CHE_PDF_ContentObjectList & content, HE_BOOL bColored )
{
	CHE_PDF_GState * pGState = NULL;
	CHE_PDF_ClipState * pClipState = NULL;
    CHE_PDF_ExtGStateStack * pExtGStateStack = NULL;
	ContentObjectList::iterator it = content.Begin();
	for ( ; it != content.End(); ++it )
	{
        StoreGState();
		pGState = (*it)->GetGState();
		if ( pGState )
		{
			pClipState = pGState->GetClipState();
			if ( pClipState )
			{
				SetClipState( pClipState );
			}
            SetCommonGState( pGState, bColored );
            pExtGStateStack = pGState->GetExtGState();
            if ( pExtGStateStack )
            {
                SetExtGState( pExtGStateStack );
            }
		}
		switch ( (*it)->GetType() )
		{
            case ContentType_Path:
				DrawPath( (CHE_PDF_Path*)(*it) );
				break;
            case ContentType_Text:
                DrawText( (CHE_PDF_Text*)(*it) );
                break;
            case ContentType_InlineImage:
				DrawInlineImage( (CHE_PDF_InlineImage*)(*it) );
				break;
            case ContentType_Component:
				DrawComponentRef( (CHE_PDF_ComponentRef*)(*it), mExtMatrix );
				break;
            default: break;
		}
        RestoreGState();
	}
}