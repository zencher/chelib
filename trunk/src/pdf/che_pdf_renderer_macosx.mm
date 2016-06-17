#include "../../include/pdf/che_pdf_renderer_macosx.h"
#include <CoreGraphics/CGPattern.h>

#include <CoreText/CoreText.h>
#include <AppKit/AppKit.h>
#include <Foundation/Foundation.h>
#include <CoreText/CoreText.h>


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
mFillColorSpace(NULL), mStrokeColorSpace(NULL), mImageColorSpace(NULL), mPosiX(0), mPosiY(0), mStrokeAlpha(1), mFillAlpha(1) {}

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
        CGContextSetRGBFillColor( mContextRef , (color>>16&0xFF)/255.0f, (color>>8&0xFF)/255.0f, (color&0xFF)/255.0f, (color>>24&0xFF)/255.0f );
    }
}

HE_VOID	CHE_PDF_Renderer::SetStrokeColor( const HE_ULONG & color )
{
    if ( mContextRef )
    {
        CGContextSetRGBStrokeColor( mContextRef , (color>>16&0xFF)/255.0f, (color>>8&0xFF)/255.0f, (color&0xFF)/255.0f, (color>>24&0xFF)/255.0f );
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
    val[i] = mFillAlpha;//1.0f;
    if (mFillAlpha < 1)
    {
        int x = 0;
    }
    
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
    val[i] = mStrokeAlpha;//1.0f;
    
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
        if ( pattern && pattern->mPattern )
        {
            if ( pattern->mPattern->GetType() == COMPONENT_TYPE_Tiling )
            {
                CHE_PDF_TilingPtr tilingPtr = CHE_PDF_Tiling::Convert( pattern->mPattern );
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
            }else if ( pattern->mPattern->GetType() == COMPONENT_TYPE_Shading )
            {
                mShading = CHE_PDF_Shading::Convert(pattern->mPattern);
                mShadingMatrix = pattern->mPatternMatrix;
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
                    CHE_PDF_ColorSpacePtr baseCS = indexcs->mBaseColorSpace;
                    if ( !baseCS )
                    {
                        //error!!!
                        assert(false);
                    }
                    
                    PDF_COLORSPACE_TYPE type = baseCS->GetColorSpaceType();
                    if ( type == COLORSPACE_SPECIAL_INDEXED || type == COLORSPACE_SPECIAL_PATTERN )
                    {
                        //error!!!
                    }
                    
                    if ( type == COLORSPACE_SPECIAL_SEPARATION )
                    {
                        CHE_PDF_FunctionPtr func = baseCS->GetSeparationPtr()->mFunction;
                        indexcs->mNewTableSize = baseCS->GetComponentCount() * (indexcs->mIndexCount + 1);
                        HE_UINT32 components = indexcs->mIndexTableSize / (indexcs->mIndexCount + 1);
                        std::vector<HE_FLOAT> input;
                        std::vector<HE_FLOAT> output;
                        HE_LPBYTE tmpByte= indexcs->mpIndexTable;
                        HE_LPBYTE tmpOutByte = indexcs->mpNewTable;
                        for (HE_UINT32 i = 0; i <= indexcs->mIndexCount; ++i)
                        {
                            input.clear();
                            output.clear();
                            for (HE_UINT32 c = 0; c < components; ++c)
                            {
                                input.push_back( *tmpByte / 255.0 );
                                tmpByte++;
                            }
                            
                            func->Calculate(input, output);
                            for (HE_UINT32 j = 0; j < components; ++j)
                            {
                                *tmpOutByte = input[j] * 255;
                                tmpOutByte++;
                            }
                            *tmpOutByte = output[3] * 255;
                            tmpOutByte++;
                        }
                        CGColorSpaceRef baseColorSpaceRef = CreateColorSpace( baseCS->GetSeparationPtr()->mBaseColorSpace );
                        if ( baseColorSpaceRef )
                        {
                            csRef = CGColorSpaceCreateIndexed( baseColorSpaceRef, indexcs->mIndexCount, indexcs->mpNewTable );
                            CGColorSpaceRelease( baseColorSpaceRef );
                        }
                    }else if ( type == COLORSPACE_SPECIAL_DEVICEN )
                    {
                        CHE_PDF_FunctionPtr func = baseCS->GetDeviceNPtr()->mFunction;
                        indexcs->mNewTableSize = 4 * (indexcs->mIndexCount + 1);
                        indexcs->mpNewTable = new unsigned char [indexcs->mNewTableSize];
                        HE_UINT32 components = indexcs->mIndexTableSize / (indexcs->mIndexCount + 1);
                        std::vector<HE_FLOAT> input;
                        std::vector<HE_FLOAT> output;
                        HE_LPBYTE tmpByte= indexcs->mpIndexTable;
                        HE_LPBYTE tmpOutByte = indexcs->mpNewTable;
                        for (HE_UINT32 i = 0; i <= indexcs->mIndexCount; ++i)
                        {
                            input.clear();
                            output.clear();
                            for (HE_UINT32 c = 0; c < components; ++c)
                            {
                                input.push_back( *tmpByte / 255.0f );
                                tmpByte++;
                            }
                            
                            func->Calculate(input, output);
                            
                            for (HE_LONG j = 0; j < output.size(); ++j)
                            {
                                *tmpOutByte = output[j] * 255;
                                tmpOutByte++;
                            }
                        }
                        
                        CGColorSpaceRef baseColorSpaceRef = CreateColorSpace( baseCS->GetDeviceNPtr()->mBaseColorSpace );
                        if ( baseColorSpaceRef )
                        {
                            csRef = CGColorSpaceCreateIndexed( baseColorSpaceRef, indexcs->mIndexCount, indexcs->mpNewTable );
                            CGColorSpaceRelease( baseColorSpaceRef );
                        }
                    }else{
                        CGColorSpaceRef baseColorSpaceRef = CreateColorSpace( indexcs->mBaseColorSpace );
                        if ( baseColorSpaceRef )
                        {
                            csRef = CGColorSpaceCreateIndexed( baseColorSpaceRef, indexcs->mIndexCount, indexcs->mpIndexTable );
                            CGColorSpaceRelease( baseColorSpaceRef );
                        }
                    }
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
        if ( 0 && arrayPtr )
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
                                       (imagePtr->GetWidth() * imagePtr->GetBPC() + 7)/8, dataRef, pDecode, imagePtr->IsInterpolate() );
            CGDataProviderRelease( dataRef );
                return imgRef;
        }
        
        CHE_PDF_ColorSpacePtr csPtr = imagePtr->GetColorspace();
        
        CGColorRenderingIntent ri = kCGRenderingIntentPerceptual;
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
        
        CGContextSetBlendMode(mContextRef, kCGBlendModeMultiply);

        
        HE_ULONG bpc = imagePtr->GetBPC();
        if ( bpc == 0 )
        {
            bpc = imagePtr->GetSize() * 8 / ( imagePtr->GetWidth() * imagePtr->GetHeight() * csPtr->GetComponentCount() );
        }
        CGColorSpaceRef csRef = CreateColorSpace( csPtr );
        
        imgRef = CGImageCreate( imagePtr->GetWidth(), imagePtr->GetHeight(), bpc, bpc*csPtr->GetComponentCount(),
                               (imagePtr->GetWidth() * csPtr->GetComponentCount() * bpc + 7)/8, csRef,
                               kCGBitmapByteOrderDefault|kCGImageAlphaNone, dataRef, pDecode, imagePtr->IsInterpolate(), ri );
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
        
        if ( image->IsMask() )
        {
            imgRef = CGImageMaskCreate( image->GetWidth(), image->GetHeight(), image->GetBpc(), image->GetBpc(), (image->GetWidth() * image->GetBpc() + 7)/8, dataRef, pDecode, false );
            CGDataProviderRelease( dataRef );
            return imgRef;
        }
            
        CHE_PDF_ColorSpacePtr csPtr = image->GetColorspace();
        
        CGColorRenderingIntent ri = kCGRenderingIntentAbsoluteColorimetric;
        switch ( image->GetRenderIntent() )
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
        
        
        HE_ULONG bpc = image->GetBpc();
        if ( bpc == 0 )
        {
            bpc = image->GetDataSize() * 8 / ( image->GetWidth() * image->GetHeight() * csPtr->GetComponentCount() );
        }
        CGColorSpaceRef csRef = CreateColorSpace( csPtr );
        if (csRef==NULL)
        {
            abort();
            
        }
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

HE_VOID CHE_PDF_Renderer::ResetPath()
{
    if ( mPathRef )
    {
        CGPathRelease( mPathRef );
    }
    mPathRef = CGPathCreateMutable();
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
        if (mShading)
        {
            StoreGState();
            
            ClipPath();
            CGContextConcatCTM(mContextRef, CGAffineTransformMake(mShadingMatrix.a, mShadingMatrix.b, mShadingMatrix.c, mShadingMatrix.d, mShadingMatrix.e, mShadingMatrix.f));
            DrawShading(mShading);
            mShading.Reset();
            
            RestoreGState();
        }else{
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

HE_VOID CHE_PDF_Renderer::SetCommonGState( CHE_PDF_GState * pGState, HE_BOOL bColor, HE_BOOL bMatrix )
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
    
    if ( bMatrix )
    {
        matrix = pGState->GetMatrix();
        SetMatrix( matrix );
        
        mTmpMatrix = mMatrix;
        mTmpMatrix.Concat( mExtMatrix );
        CGContextConcatCTM( mContextRef, CGAffineTransformMake( mTmpMatrix.a, mTmpMatrix.b, mTmpMatrix.c, mTmpMatrix.d, mTmpMatrix.e, mTmpMatrix.f) );
    }

    if ( bColor )
    {
        pGState->GetFillColor( fillColor );
        pGState->GetStrokeColor( strokeColor );
        pGState->GetFillColorSpace( fillColorSpace );
        pGState->GetStrokeColorSpace( strokeColorSpace );
        
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
                SetFillColorSpace( cs->mBaseColorSpace );
            }
        }else if ( fillColorSpace->GetColorSpaceType() == COLORSPACE_SPECIAL_DEVICEN )
        {
            CHE_PDF_CS_DeviceNPtr cs = fillColorSpace->GetDeviceNPtr();
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
                SetFillColorSpace( cs->mBaseColorSpace );
            }
        }else{
            SetFillColorSpace( fillColorSpace );
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
                SetStrokeColorSpace( cs->mBaseColorSpace );
            }
        }else if ( strokeColorSpace->GetColorSpaceType() == COLORSPACE_SPECIAL_DEVICEN )
        {
            CHE_PDF_CS_DeviceNPtr cs = strokeColorSpace->GetDeviceNPtr();
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
                SetStrokeColorSpace( cs->mBaseColorSpace );
            }
        }else{
            SetStrokeColorSpace( strokeColorSpace );
        }
        
        SetFillColor( fillColor );
        SetStrokeColor( strokeColor );
        
        
    }
    CGContextSetRenderingIntent(mContextRef, kCGRenderingIntentPerceptual);
}

HE_VOID CHE_PDF_Renderer::SetExtGState( CHE_PDF_ExtGStateStack * pExtGState )
{
    if ( pExtGState )
    {
        GRAPHICS_STATE_BLENDMODE blendMode = pExtGState->GetBlendMode();
        SetBlendMode( blendMode );
        
        mFillAlpha = pExtGState->GetFillAlpha();
        mStrokeAlpha = pExtGState->GetStrokeAlpha();
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
            
            ResetPath();
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
            CHE_Matrix matrix;
            CHE_PDF_Text * pText = (CHE_PDF_Text*)(pObj);
            CHE_PDF_Font * pFont = pGState->GetTextFont();
            
            CGGlyph glyph = 0;
            CTFontRef ctFontRef  = CTFontCreateWithGraphicsFont((CGFontRef)pFont->GetPlatformFontInfo(), 1, nil, nil);
            
            ResetPath();
            for (size_t i = 0; i < pText->mItems.size(); ++i)
            {
                glyph = pText->mItems[i].gid;
                matrix = pText->GetCharMatrix(i);
                CGAffineTransform tm = CGAffineTransformMake(matrix.a, matrix.b, matrix.c, matrix.d, matrix.e, matrix.f);
                CGPathRef path =  CTFontCreatePathForGlyph(ctFontRef, glyph, &tm);
                CGPathAddPath(mPathRef, NULL, path);
            }
            CFRelease(ctFontRef);
        }
        ClipPath();
		
        CHE_Matrix invertMatrix;
        invertMatrix.Invert( mTmpMatrix );
        CGContextConcatCTM( mContextRef, CGAffineTransformMake( invertMatrix.a, invertMatrix.b, invertMatrix.c, invertMatrix.d, invertMatrix.e, invertMatrix.f) );
	}
}

HE_VOID CGFontCleanCallBack( HE_LPVOID info )
{
    if ( info )
    {
        CGFontRef fontRef = (CGFontRef)info;
        CGFontRelease( fontRef );
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
    CGGlyph ggid = gid;
    CGContextSetFontSize( mContextRef , 1 );
    CGContextSetTextMatrix( mContextRef, CGAffineTransformMake( mTextMatrix.a, mTextMatrix.b, mTextMatrix.c, mTextMatrix.d, mTextMatrix.e, mTextMatrix.f) );
    CGContextShowGlyphsAtPositions( mContextRef, &ggid, &position, 1 );
    RestoreGState();
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
            CGFontRef cgfontRef = NULL;
            
            //对于Fill类型的文本输出，可以使用系统原生文本输出接口，以获得次像素支
            CHE_PDF_Font * pFont = pText->GetGState()->GetTextFont();
            if ( pFont->GetFontType() == FONT_TYPE3 )
            {
                StoreGState();
                
                CHE_Matrix inverstMatrix;
                inverstMatrix.Invert( mTmpMatrix );
                CGContextConcatCTM( mContextRef, CGAffineTransformMake( inverstMatrix.a, inverstMatrix.b, inverstMatrix.c, inverstMatrix.d, inverstMatrix.e, inverstMatrix.f) );
                
                CHE_PDF_Type3_Font * pFontType3 = (CHE_PDF_Type3_Font*)pFont;
                for ( HE_ULONG i = 0; i < pText->mItems.size(); ++i )
                {
                    StoreGState();
                    
                    CHE_Matrix textMatirx = pText->GetCharMatrix( i );
                    textMatirx.Concat( mMatrix );
                    textMatirx.Concat( mExtMatrix );
                    CGContextConcatCTM( mContextRef, CGAffineTransformMake( textMatirx.a, textMatirx.b, textMatirx.c, textMatirx.d, textMatirx.e, textMatirx.f) );
                    CHE_PDF_ContentObjectList * pList = pFontType3->GetGlyph( pText->mItems[i].charCode );
                    if ( pList ) {
                        DrawContentObjectList( *pList );
                    }
                    
                    RestoreGState();
                }
                
                RestoreGState();
                return;
            }
            
            if (  pFont->GetPlatformFontInfo() != nullptr )
            {
                cgfontRef = (CGFontRef)pFont->GetPlatformFontInfo();
                SetTextFont( cgfontRef );
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
                            pFont->SetPlatformFontInfo( cgfontRef );
                            pFont->SetPlatformFontInfoCleanCallBack( CGFontCleanCallBack );
                            SetTextFont( cgfontRef );
                        }else{
                            CFRelease(dataProviderRef);
                            CFRelease(dataRef);
                            return DrawTextAsPath( pText );
                        }
                        CFRelease(dataProviderRef);
                        CFRelease(dataRef);
                    }
                }
            }
            
            char glyphName[128];
            CHE_Matrix textMatirx;
            for ( HE_ULONG i = 0; i < pText->mItems.size(); ++i )
            {
                textMatirx = pText->GetCharMatrix( i );
                SetTextMatrix( textMatirx );
                
                sprintf( glyphName, "cid%ld", pText->mItems[i].gid );
                CFStringRef glyphNameStrRef = CFStringCreateWithCString( kCFAllocatorDefault, glyphName, kCFStringEncodingASCII );
                GlyphID gid = CGFontGetGlyphWithGlyphName( cgfontRef, glyphNameStrRef );
                CFRelease(glyphNameStrRef);
                if ( gid )
                {
                    DrawTextGlyph( gid );
                }else{
                    DrawTextGlyph( pText->mItems[i].gid );
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
                        CurveTo(    pPath->mItems[i+1].value, pPath->mItems[i+2].value,
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
    
    //RestoreGState();
}

HE_VOID CHE_PDF_Renderer::DrawComponentRef( CHE_PDF_ComponentRef * cmptRef )
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
            
            CHE_Matrix t1 = mMatrix;
            t1.Concat(mExtMatrix);
            
            CHE_Matrix revertMatrix;
            revertMatrix.Invert(t1);
            CGContextConcatCTM( mContextRef, CGAffineTransformMake( revertMatrix.a, revertMatrix.b, revertMatrix.c, revertMatrix.d, revertMatrix.e, revertMatrix.f) );
            
            CHE_PDF_FormXObjectPtr formPtr = CHE_PDF_FormXObject::Convert( componentPtr );
            
            CHE_Matrix tmpMatrix = mMatrix;
			CHE_Matrix tmpExtMatrix = mExtMatrix;
            CHE_Matrix newExtMatrix = formPtr->GetMatrix();
			CHE_PDF_GState * pGState = cmptRef->GetGState();
			if ( pGState )
			{
                newExtMatrix.Concat(pGState->GetMatrix());
			}
			newExtMatrix.Concat( tmpExtMatrix );
			SetExtMatrix( newExtMatrix );
            DrawForm( CHE_PDF_FormXObject::Convert( componentPtr ) );
			SetExtMatrix( tmpExtMatrix );
            SetMatrix( tmpMatrix );
            
            RestoreGState();
			break;
		}
        case COMPONENT_TYPE_Shading:
		{
            //StoreGState();
            
            //CHE_Matrix t1 = mMatrix;
            //t1.Concat(mExtMatrix);
            
            //CHE_Matrix revertMatrix;
            //revertMatrix.Invert(t1);
            //CGContextConcatCTM( mContextRef, CGAffineTransformMake( revertMatrix.a, revertMatrix.b, revertMatrix.c, revertMatrix.d, revertMatrix.e, revertMatrix.f) );
            
            DrawShading( CHE_PDF_Shading::Convert(componentPtr) );
            
            //RestoreGState();
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

void FunctionEvaluateCallback(void * __nullable info, const CGFloat *  in, CGFloat *  out)
{
    CHE_PDF_Renderer * pRender = (CHE_PDF_Renderer*)info;
    std::vector<HE_FLOAT> input;
    std::vector<HE_FLOAT> output;
    input.push_back( in[0] );
    pRender->shadingFunc->Calculate( input, output );
    
    if ( pRender->shadingCS->GetColorSpaceType() == COLORSPACE_SPECIAL_SEPARATION )
        
    {
        std::vector<HE_FLOAT> routput;
        pRender->shadingCS->GetSeparationPtr()->mFunction->Calculate(output, routput);
        output = routput;
    }else if ( pRender->shadingCS->GetColorSpaceType() == COLORSPACE_SPECIAL_DEVICEN )
    {
        std::vector<HE_FLOAT> routput;
        pRender->shadingCS->GetDeviceNPtr()->mFunction->Calculate(output, routput);
        output = routput;
    }
    int i = 0;
    for( ; i < output.size(); ++i)
    {
        out[i] = output[i];
    }
    out[i] = 1.0f;
}

void FunctionEvaluateCallback1(void * __nullable info, const CGFloat *  in, CGFloat *  out)
{
    CHE_PDF_Renderer * pRender = (CHE_PDF_Renderer*)info;
    int c = 0;
    if ( pRender->shadingCS->GetColorSpaceType() == COLORSPACE_SPECIAL_SEPARATION )
    {
        c = pRender->shadingCS->GetSeparationPtr()->mBaseColorSpace->GetComponentCount() + 1;
    }else if ( pRender->shadingCS->GetColorSpaceType() == COLORSPACE_SPECIAL_DEVICEN )
    {
        c = pRender->shadingCS->GetDeviceNPtr()->mBaseColorSpace->GetComponentCount() + 1;
    }else{
        c = pRender->shadingCS->GetComponentCount();
    }
    int i = 0;
    for( ; i < c; ++i)
    {
        out[i] = in[0];
    }
    out[i] = 1.0f;
}

void FunctionReleaseInfoCallback(void * __nullable info)
{
    
}

static void myCalculateShadingValues (void *info,
                                      const CGFloat *in,
                                      CGFloat *out)
{
    CGFloat v;
    size_t k, components;
    static const CGFloat c[] = {1, 0, .5, 0 };
    
    components = (size_t)info;
    
    v = *in;
    for (k = 0; k < components -1; k++)
        *out++ = c[k] * v;
    *out++ = 1;
}

static CGFunctionRef myGetFunction (CGColorSpaceRef colorspace)// 1
{
    size_t numComponents;
    static const CGFloat input_value_range [2] = { 0, 1 };
    static const CGFloat output_value_ranges [8] = { 0, 1, 0, 1, 0, 1, 0, 1 };
    static const CGFunctionCallbacks callbacks = { 0,// 2
        &myCalculateShadingValues,
        NULL };
    
    numComponents = 1 + CGColorSpaceGetNumberOfComponents (colorspace);// 3
    return CGFunctionCreate ((void *) numComponents, // 4
                             1, // 5
                             input_value_range, // 6
                             numComponents, // 7
                             output_value_ranges, // 8
                             &callbacks);// 9
}


HE_VOID CHE_PDF_Renderer::DrawShading( const CHE_PDF_ShadingPtr & shading )
{
    /*CGRect rc = CGContextGetClipBoundingBox(mContextRef);
    CGContextSetRGBFillColor(mContextRef, 0.5, 0.5, 0, 1);
    CGContextFillRect(mContextRef, rc);*/
    
    if ( shading->GetShadingType() == SHADING_TYPE_Axial )
    {
        CGFloat domain[2];
        domain[0] = 0;
        domain[1] = 1;
        CGFloat range[10];
        range[0] = 0;
        range[1] = 1;
        range[2] = 0;
        range[3] = 1;
        range[4] = 0;
        range[5] = 1;
        range[6] = 0;
        range[7] = 1;
        range[8] = 0;
        range[9] = 1;
        CGFunctionCallbacks callbacks;
        callbacks.version = 0;
        callbacks.evaluate = FunctionEvaluateCallback;
        callbacks.releaseInfo = FunctionReleaseInfoCallback;
        
        CHE_PDF_AxialShadingPtr ashading = shading->GetAixalShadingPtr();
        
        CHE_PDF_FunctionPtr fun = ashading->GetFunction();
        
        if (!fun )
        {
            return ;
        }
        shadingFunc = fun;
        
        CHE_PDF_ColorSpacePtr cs = shading->GetColorSpace();
        
        if ( !cs )
        {
            return;
        }
        
        CHE_PDF_Color color = shading->GetBackgroundColor();
        if ( color.GetComponentCount() )
        {
            CGRect rc = CGContextGetClipBoundingBox(mContextRef);
            SetFillColorSpace(cs);
            SetFillColor(color);
            //CGContextAddRect(mContextRef, rc);
            CGContextFillRect(mContextRef, rc);
        }
        
        
        
        
        
        CGColorSpaceRef csref = CreateColorSpace(cs);
        if ( csref == NULL )
        {
            if (cs->GetDeviceNPtr())
            {
                csref = CreateColorSpace(cs->GetDeviceNPtr()->mBaseColorSpace);
            }else{
                csref = CreateColorSpace(cs->GetSeparationPtr()->mBaseColorSpace);
            }
        }
        shadingCS = cs;
        size_t c = CGColorSpaceGetNumberOfComponents(csref) + 1;
        CGFunctionRef funcRef = CGFunctionCreate( this, 1, domain, c, range, &callbacks );
        CGShadingRef shadingRef = CGShadingCreateAxial( csref, CGPointMake(ashading->GetStartPoint().x, ashading->GetStartPoint().y),
                                                       CGPointMake(ashading->GetEndPoint().x, ashading->GetEndPoint().y), funcRef,
                                                       ashading->GetStartExtend(), ashading->GetEndExtend() );
        CGContextDrawShading(mContextRef, shadingRef);
    }else if ( shading->GetShadingType() == SHADING_TYPE_Radial )
    {
        /*CGRect rc = CGContextGetClipBoundingBox(mContextRef);
         CGContextSetRGBFillColor(mContextRef, 0.5, 0.5, 0, 1);
         CGContextFillRect(mContextRef, rc);*/
        
        CHE_PDF_ColorSpacePtr cs = shading->GetColorSpace();
        
        CHE_PDF_RadialShadingPtr ashading = shading->GetRadialShadingPtr();
        
        CGFloat domain[2];
        domain[0] = 0;
        domain[1] = 1;
        CGFloat range[10];
        range[0] = 0;
        range[1] = 1;
        range[2] = 0;
        range[3] = 1;
        range[4] = 0;
        range[5] = 1;
        range[6] = 0;
        range[7] = 1;
        range[8] = 0;
        range[9] = 1;
        CGFunctionCallbacks callbacks;
        callbacks.version = 0;
        callbacks.evaluate = FunctionEvaluateCallback;
        callbacks.releaseInfo = FunctionReleaseInfoCallback;
        
        CHE_PDF_FunctionPtr fun = ashading->GetFunction();
        
        if (!fun )
        {
            return ;
        }
        shadingFunc = fun;
        
        if ( !cs )
        {
            return;
        }
        
        CHE_PDF_Color color = shading->GetBackgroundColor();
        if ( color.GetComponentCount() )
        {
            CGRect rc = CGContextGetClipBoundingBox(mContextRef);
            SetFillColorSpace(cs);
            SetFillColor(color);
            //CGContextAddRect(mContextRef, rc);
            CGContextFillRect(mContextRef, rc);
        }
        
        
        CGColorSpaceRef csref = CreateColorSpace(cs);
        if ( csref == NULL )
        {
            if (cs->GetDeviceNPtr())
            {
                csref = CreateColorSpace(cs->GetDeviceNPtr()->mBaseColorSpace);
            }else{
                csref = CreateColorSpace(cs->GetSeparationPtr()->mBaseColorSpace);
            }
        }
                size_t c = CGColorSpaceGetNumberOfComponents(csref) + 1;
        CGFunctionRef funcRef = CGFunctionCreate( this, 1, domain, c, range, &callbacks );
        CGShadingRef shadingRef = CGShadingCreateRadial(csref, CGPointMake(ashading->GetStartPoint().x, ashading->GetStartPoint().y), ashading->GetStartRadius(), CGPointMake(ashading->GetEndPoint().x, ashading->GetEndPoint().y), ashading->GetEndRadius(), funcRef, ashading->GetStartExtend(), ashading->GetEndExtend());
        CGContextDrawShading(mContextRef, shadingRef);
    }
    
    /*if ( shading->GetShadingType() == SHADING_TYPE_Axial )
    {
        CGFloat domain[2];
        domain[0] = 0;
        domain[1] = 1;
        CGFloat range[8];
        range[0] = 0;
        range[1] = 1;
        range[2] = 0;
        range[3] = 1;
        range[4] = 0;
        range[5] = 1;
        range[6] = 0;
        range[7] = 1;
        CGFunctionCallbacks callbacks;
        callbacks.version = 0;
        callbacks.evaluate = myCalculateShadingValues;//FunctionEvaluateCallback;
        callbacks.releaseInfo = FunctionReleaseInfoCallback;
        CHE_PDF_FunctionPtr fun = shading->GetFunction();
        shadingFunc = fun;
        
        CHE_PDF_ColorSpacePtr cs = shading->GetColorSpace();
        
        if ( !cs )
        {
            return;
        }
        
        
        CGRect rc = CGContextGetClipBoundingBox(mContextRef);
        
        
        CGColorSpaceRef csref = CreateColorSpace(cs);
        if ( csref == NULL )
        {
            if (cs->GetDeviceNPtr())
            {
                csref = CreateColorSpace(cs->GetDeviceNPtr()->mBaseColorSpace);
            }else{
                csref = CreateColorSpace(cs->GetSeparationPtr()->mBaseColorSpace);
            }
        }
        shadingCS = cs;
        CGFunctionRef funcRef = CGFunctionCreate( (void *)4, 1, domain, 4, range, &callbacks );
        CGShadingRef shadingRef = CGShadingCreateAxial( CGColorSpaceCreateDeviceRGB(), CGPointMake(0, 0), CGPointMake(1, 0), funcRef, true, true);
        CGContextDrawShading(mContextRef, shadingRef);
    }*/
    
    /*CGPoint     startPoint, endPoint;
    CGFunctionRef myFunctionObject;
    CGShadingRef myShading;
    CGColorSpaceRef colorspace;
    
    CGContextRef context = (CGContextRef)[[NSGraphicsContext currentContext] graphicsPort];
    

    startPoint = CGPointMake(0,0);
    endPoint = CGPointMake(1,0);
    colorspace = CGColorSpaceCreateDeviceRGB();
    myFunctionObject = myGetFunction (colorspace);
    
    myShading = CGShadingCreateAxial (colorspace,
                                      startPoint, endPoint,
                                      myFunctionObject,
                                      false, false);
    
    CGContextDrawShading (context, myShading);
    
    CGShadingRelease (myShading);
    CGColorSpaceRelease (colorspace);
    CGFunctionRelease (myFunctionObject);*/
}

HE_VOID CHE_PDF_Renderer::DrawForm( const CHE_PDF_FormXObjectPtr & form )
{
    CHE_PDF_ContentObjectList & list = form->GetList();
    ContentObjectList::iterator it = list.Begin();
    CHE_PDF_GState * pGState = NULL;
    CHE_PDF_ClipState * pClipState = NULL;
    CHE_PDF_ExtGStateStack * pExtGStateStack = NULL;
    
    
    CGContextBeginTransparencyLayer(mContextRef, NULL);
    
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
            pExtGStateStack = pGState->GetExtGState();
            if ( pExtGStateStack )
            {
                SetExtGState( pExtGStateStack );
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
                //RestoreGState();
				DrawComponentRef( (CHE_PDF_ComponentRef*)(*it) );
                //StoreGState();
				break;
			}
            default:
                break;
		}
        RestoreGState();
	}
    
    CGContextEndTransparencyLayer(mContextRef);
}

HE_VOID CHE_PDF_Renderer::DrawContentObjectList( CHE_PDF_ContentObjectList & list )
{
    ContentObjectList::iterator it = list.Begin();
    CHE_PDF_GState * pGState = NULL;
    CHE_PDF_ClipState * pClipState = NULL;
    CHE_PDF_ExtGStateStack * pExtGStateStack = NULL;
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
            pExtGStateStack = pGState->GetExtGState();
            if ( pExtGStateStack )
            {
                SetExtGState( pExtGStateStack );
            }
			SetCommonGState( pGState, TRUE, FALSE );
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
				DrawComponentRef( (CHE_PDF_ComponentRef*)(*it) );
				break;
			}
            default:
                break;
		}
        RestoreGState();
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
            pExtGStateStack = pGState->GetExtGState();
            if ( pExtGStateStack )
            {
                SetExtGState( pExtGStateStack );
            }
            SetCommonGState( pGState );
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
				DrawComponentRef( (CHE_PDF_ComponentRef*)(*it) );
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
				DrawComponentRef( (CHE_PDF_ComponentRef*)(*it) );
				break;
            default: break;
		}
        RestoreGState();
	}
}