#include "../../include/che_pdf_renderer_macosx.h"

#include <CoreGraphics/CGPattern.h>
#include <CoreText/CoreText.h>
#include <AppKit/AppKit.h>
#include <Foundation/Foundation.h>
#include <CoreText/CoreText.h>

namespace chelib {

void TilingDrawCallBack( void *info, CGContextRef c )
{
    if ( c == NULL )
    {
        return;
    }
    
    CPDF_Tiling * pTiling = (CPDF_Tiling*)( info );
    if ( pTiling )
    {
        CPDF_Renderer render( c );
        render.RenderTiling( pTiling->GetList(), pTiling->IsColored() );
    }
}


CPDF_Renderer::CPDF_Renderer( CGContextRef cgContext )
    : mFillMode(FillMode_Nonzero),mContextRef(cgContext), mPathRef(NULL), mFillColorSpace(NULL),
mStrokeColorSpace(NULL), mImageColorSpace(NULL), mPosiX(0), mPosiY(0),mFillAlpha(1), mStrokeAlpha(1) {}

CPDF_Renderer::~CPDF_Renderer()
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

void CPDF_Renderer::SetPosition( FLOAT x, FLOAT y )
{
    mPosiX = x;
    mPosiY = y;
}

void	CPDF_Renderer::SetMatrix( const CMatrix & matrix )
{
    mMatrix = matrix;
}

void	CPDF_Renderer::SetExtMatrix( const CMatrix & matrix )
{
    mExtMatrix = matrix;
}

void	CPDF_Renderer::SetLineWidth( const FLOAT & lineWidth )
{
    if ( mContextRef )
    {
        CGContextSetLineWidth( mContextRef, lineWidth );
    }
}

void	CPDF_Renderer::SetMiterLimit( const FLOAT & miterLimit )
{
    if ( mContextRef )
    {
        CGContextSetMiterLimit( mContextRef, miterLimit );
    }
}

void	CPDF_Renderer::SetLineCap( const GRAPHICS_STATE_LINECAP & lineCap )
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

void	CPDF_Renderer::SetLineJoin( const GRAPHICS_STATE_LINEJOIN & lineJion )
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

void	CPDF_Renderer::SetLineDash( const GRAPHICS_STATE_DASHPATTERN & dashPattern )
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

void	CPDF_Renderer::SetFillMode( GRAPHICS_STATE_FILLMODE mode )
{
    mFillMode = mode;
}

void CPDF_Renderer::SetBlendMode( GRAPHICS_STATE_BLENDMODE mode )
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

void CPDF_Renderer::SetTextFont( CGFontRef font )
{
    if ( mContextRef )
    {
        CGContextSetFont( mContextRef, font );
    }
}

void CPDF_Renderer::SetTextMatrix( CMatrix textMatrix )
{
    mTextMatrix = textMatrix;
}

void	CPDF_Renderer::SetFillColor( const ARGB & color )
{
    if ( mContextRef )
    {
        CGContextSetRGBFillColor( mContextRef , (color>>16&0xFF)/255.0f, (color>>8&0xFF)/255.0f, (color&0xFF)/255.0f, (color>>24&0xFF)/255.0f );
    }
}

void	CPDF_Renderer::SetStrokeColor( const ARGB & color )
{
    if ( mContextRef )
    {
        CGContextSetRGBStrokeColor( mContextRef , (color>>16&0xFF)/255.0f, (color>>8&0xFF)/255.0f, (color&0xFF)/255.0f, (color>>24&0xFF)/255.0f );
    }
}

void CPDF_Renderer::SetFillColor( const CPDF_Color & color )
{
    // why i do this?
    /*if (mShading)
    {
        return;
    }*/
    
    CGFloat val[5];
    int i = 0;
    for ( ; i < color.GetComponentCount(); ++i )
    {
        val[i] = color.GetComponent( i );
    }
    val[i] = mFillAlpha;
    
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

void CPDF_Renderer::SetStrokeColor( const CPDF_Color & color )
{
    CGFloat val[5];
    int i = 0;
    for ( ; i < color.GetComponentCount(); ++i )
    {
        val[i] = color.GetComponent( i );
    }
    val[i] = mStrokeAlpha;
    
    CGContextSetStrokeColor( mContextRef, val );
}

void CPDF_Renderer::SetFillColorSpace( const CPDF_ColorSpacePtr & cs )
{
    mFillPattern = NULL;
    if ( mFillColorSpace )
    {
        CGColorSpaceRelease( mFillColorSpace );
        mFillColorSpace = NULL;
    }
    
    if ( cs->GetColorSpaceType() == COLORSPACE_SPECIAL_PATTERN )
    {
        CPDF_CS_PatternPtr pattern = cs->GetPatternPtr();
        if ( pattern && pattern->mPattern )
        {
            if ( pattern->mPattern->GetType() == COMPONENT_TYPE_Tiling )
            {
                CPDF_TilingPtr tilingPtr = CPDF_Tiling::Convert( pattern->mPattern );
                if ( tilingPtr )
                {
                    CRect rect = tilingPtr->GetBBox();
                    CGRect bounds = CGRectMake( rect.left, rect.bottom, rect.width, rect.height );
                    
                    CMatrix tilingMatrix = tilingPtr->GetMatrix();
                    CMatrix matrix;
                    if ( mRotate == 90 )
                    {
                        CMatrix rotateMatrix = CMatrix::RotateMatrix( mRotate );
                        CMatrix transformMatrix = CMatrix::TranslateMatrix( mPageRect.height, 0 );
                        rotateMatrix.Concat( transformMatrix );
                        matrix.Concat( rotateMatrix );
                    }else if ( mRotate == 180 )
                    {
                        CMatrix rotateMatrix = CMatrix::RotateMatrix( mRotate );
                        CMatrix transformMatrix = CMatrix::TranslateMatrix( mPageRect.width, mPageRect.height );
                        rotateMatrix.Concat( transformMatrix );
                        matrix.Concat( rotateMatrix );
                    }else if ( mRotate == 270 )
                    {
                        CMatrix rotateMatrix = CMatrix::RotateMatrix( mRotate );
                        CMatrix transformMatrix = CMatrix::TranslateMatrix( 0, mPageRect.width );
                        rotateMatrix.Concat( transformMatrix );
                        matrix.Concat( rotateMatrix );
                    }
                    
                    CRect newPageRect = matrix.Transform( mPageRect );
                    
                    CMatrix bboxClipMatrix;
                    bboxClipMatrix.a = 1;
                    bboxClipMatrix.b = 0;
                    bboxClipMatrix.c = 0;
                    bboxClipMatrix.d = 1;
                    bboxClipMatrix.e = -newPageRect.left;
                    bboxClipMatrix.f = -newPageRect.bottom;
                    matrix.Concat( bboxClipMatrix );
                    
                    /*CMatrix flipMatrix;
                     flipMatrix.a = 1;
                     flipMatrix.b = 0;
                     flipMatrix.c = 0;
                     flipMatrix.d = -1;
                     flipMatrix.e = 0;
                     flipMatrix.f = newPageRect.height;
                     matrix.Concat( flipMatrix );*/
                    
                    CMatrix scaletMatrix;
                    scaletMatrix.a = mDpix * mScale / 72;
                    scaletMatrix.b = 0;
                    scaletMatrix.c = 0;
                    scaletMatrix.d = mDpiy * mScale / 72;
                    scaletMatrix.e = 0;
                    scaletMatrix.f = 0;
                    matrix.Concat( scaletMatrix );
                    
                    CMatrix offsetMatrix;
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
                mShading = CPDF_Shading::Convert(pattern->mPattern);
                mShadingMatrix = pattern->mPatternMatrix;
            }
        }
    }else{
        mFillColorSpace = CreateColorSpace( cs );
        CGContextSetFillColorSpace( mContextRef , mFillColorSpace );
    }
}

void CPDF_Renderer::SetStrokeColorSpace( const CPDF_ColorSpacePtr & cs )
{
    if ( mStrokeColorSpace )
    {
        CGColorSpaceRelease( mStrokeColorSpace );
        mStrokeColorSpace = NULL;
    }
    
    mStrokeColorSpace = CreateColorSpace( cs );
    CGContextSetStrokeColorSpace( mContextRef , mStrokeColorSpace );
}

CGColorSpaceRef CPDF_Renderer::CreateColorSpace( const CPDF_ColorSpacePtr & cs )
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
                CPDF_CS_CalGrayPtr calgraycs = cs->GetCalGrayPtr();
                if ( calgraycs )
                {
                    csRef = CGColorSpaceCreateCalibratedGray( calgraycs->mWhitePoint, calgraycs->mBlackPoint, calgraycs->mGamma );
                }
                break;
            }
        case COLORSPACE_CIEBASE_CALRGB:
            {
                CPDF_CS_CalRGBPtr calrgbcs = cs->GetCalRGBPtr();
                if ( calrgbcs )
                {
                    csRef = CGColorSpaceCreateCalibratedRGB( calrgbcs->mWhitePoint , calrgbcs->mBlackPoint, calrgbcs->mGamma, calrgbcs->mMatrix );
                }
                break;
            }
        case COLORSPACE_CIEBASE_CALLAB:
            {
                CPDF_CS_CalLabPtr labcs = cs->GetCalLabPtr();
                if ( labcs )
                {
                    csRef = CGColorSpaceCreateLab( labcs->mWhitePoint, labcs->mBlackPoint, labcs->mRange );
                }
                break;
            }
        case COLORSPACE_CIEBASE_ICCBASED:
            {
                CPDF_CS_ICCBasedPtr icccs = cs->GetICCBasedPtr();
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
                CPDF_CS_IndexedPtr indexcs = cs->GetIndexedPtr();
                if ( indexcs )
                {
                    CPDF_ColorSpacePtr baseCS = indexcs->mBaseColorSpace;
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
                        CPDF_FunctionPtr func = baseCS->GetSeparationPtr()->mFunction;
                        indexcs->mNewTableSize = baseCS->GetComponentCount() * (indexcs->mIndexCount + 1);
                        size_t components = indexcs->mIndexTableSize / (indexcs->mIndexCount + 1);
                        std::vector<FLOAT> input;
                        std::vector<FLOAT> output;
                        PBYTE tmpByte= indexcs->mpIndexTable;
                        PBYTE tmpOutByte = indexcs->mpNewTable;
                        for (uint32 i = 0; i <= indexcs->mIndexCount; ++i)
                        {
                            input.clear();
                            output.clear();
                            for (uint32 c = 0; c < components; ++c)
                            {
                                input.push_back( *tmpByte / 255.0 );
                                tmpByte++;
                            }
                            
                            func->Calculate(input, output);
                            for (uint32 j = 0; j < components; ++j)
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
                        CPDF_FunctionPtr func = baseCS->GetDeviceNPtr()->mFunction;
                        indexcs->mNewTableSize = 4 * (indexcs->mIndexCount + 1);
                        indexcs->mpNewTable = new unsigned char [indexcs->mNewTableSize];
                        size_t components = indexcs->mIndexTableSize / (indexcs->mIndexCount + 1);
                        std::vector<FLOAT> input;
                        std::vector<FLOAT> output;
                        PBYTE tmpByte= indexcs->mpIndexTable;
                        PBYTE tmpOutByte = indexcs->mpNewTable;
                        for (uint32 i = 0; i <= indexcs->mIndexCount; ++i)
                        {
                            input.clear();
                            output.clear();
                            for (uint32 c = 0; c < components; ++c)
                            {
                                input.push_back( *tmpByte / 255.0f );
                                tmpByte++;
                            }
                            
                            func->Calculate(input, output);
                            
                            for (size_t j = 0; j < output.size(); ++j)
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

CGImageRef CPDF_Renderer::CreateImage( const CPDF_ImageXObjectPtr & imagePtr )
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
        double decode[] = { 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f };
        
        CPDF_ArrayPtr arrayPtr = imagePtr->GetDecodeArray();
        if ( arrayPtr )
        {
            CPDF_ObjectPtr objPtr;
            for ( unsigned int i = 0; i < arrayPtr->GetCount() && i < 10; ++i )
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
        
        CPDF_ColorSpacePtr csPtr = imagePtr->GetColorspace();
        
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
        
        size_t bpc = imagePtr->GetBPC();
        if ( bpc == 0 )
        {
            bpc = imagePtr->GetSize() * 8 / ( imagePtr->GetWidth() * imagePtr->GetHeight() * csPtr->GetComponentCount() );
        }
        CGColorSpaceRef csRef = CreateColorSpace( csPtr );
        
        CGBitmapInfo bitmapInfo = kCGBitmapByteOrderDefault;
        
        imgRef = CGImageCreate( imagePtr->GetWidth(), imagePtr->GetHeight(), bpc, bpc*csPtr->GetComponentCount(),
                                (imagePtr->GetWidth() * csPtr->GetComponentCount() * bpc + 7)/8, csRef,
                                bitmapInfo, dataRef, pDecode, imagePtr->IsInterpolate(), ri );
        CGDataProviderRelease( dataRef );
        CGColorSpaceRelease( csRef );
        
        CPDF_ArrayPtr arrPtr = imagePtr->GetColorKeyMask();
        if ( arrPtr ) {
            CGFloat val[8];
            for (size_t i = 0; i < arrPtr->GetCount(); ++i) {
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

CGImageRef CPDF_Renderer::CreateImage( CPDF_InlineImage * image )
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
        double decode[] = { 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f };
        
        CPDF_ArrayPtr arrayPtr;
        CPDF_ObjectPtr objPtr = image->GetDecode();
        if ( objPtr ) {
            arrayPtr = objPtr->GetArrayPtr();
        }
        if ( arrayPtr )
        {
            for ( unsigned int i = 0; i < arrayPtr->GetCount() && i < 10; ++i )
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
            
        CPDF_ColorSpacePtr csPtr = image->GetColorspace();
        
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
        
        
        size_t bpc = image->GetBpc();
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

void CPDF_Renderer::SetImageColorSpace( const CPDF_ColorSpacePtr & cs )
{
    if ( mImageColorSpace )
    {
        CGColorSpaceRelease( mImageColorSpace );
        mImageColorSpace = NULL;
    }
    mImageColorSpace = CreateColorSpace( cs );
}

void CPDF_Renderer::ResetPath()
{
    if ( mPathRef )
    {
        CGPathRelease( mPathRef );
    }
    mPathRef = CGPathCreateMutable();
}

void	CPDF_Renderer::MoveTo( FLOAT x, FLOAT y )
{
    if ( mPathRef == NULL )
    {
        mPathRef = CGPathCreateMutable();
    }
    CGAffineTransform affine = CGAffineTransformIdentity;
    CGPathMoveToPoint( mPathRef, &affine, x, y );
}

void	CPDF_Renderer::LineTo( FLOAT x, FLOAT y )
{
    if ( mPathRef )
    {
        CGAffineTransform affine = CGAffineTransformIdentity;
        CGPathAddLineToPoint( mPathRef, &affine, x, y );
    }
}

void	CPDF_Renderer::CurveTo( FLOAT x1, FLOAT y1, FLOAT x2, FLOAT y2, FLOAT x3, FLOAT y3 )
{
    if ( mPathRef )
    {
        CGAffineTransform affine = CGAffineTransformIdentity;
        CGPathAddCurveToPoint( mPathRef, &affine, x1, y1, x2, y2, x3, y3 );
    }
}

void CPDF_Renderer::Rectangle( FLOAT x, FLOAT y, FLOAT width, FLOAT height )
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

void	CPDF_Renderer::ClosePath()
{
    if ( mPathRef && !CGPathIsEmpty( mPathRef ) )
    {
        CGPathCloseSubpath( mPathRef );
    }
}

void	CPDF_Renderer::FillPath()
{
    if ( mContextRef && mPathRef && !CGPathIsEmpty( mPathRef ) )
    {
        /*bool bSoft = FALSE;
        if (mSoftMask)
        {
            CGContextSetAlpha(mContextRef, 0.6);
            CGContextBeginTransparencyLayer(mContextRef, nullptr);
            CGContextSetBlendMode(mContextRef, kCGBlendModeLuminosity);
            bSoft = TRUE;
        }*/
        
        if (mShading)
        {
            StoreGState();
            
            CGContextAddPath( mContextRef, mPathRef );
            CGContextClip( mContextRef );
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
        }
        
        ResetPath();
        
        /*if (bSoft)
        {
            CPDF_FormXObjectPtr tmpForm = mSoftMask;
            mSoftMask.Reset();
            
            StoreGState();
            
            CMatrix t1 = mMatrix;
            t1.Concat(mExtMatrix);
            
            CMatrix revertMatrix;
            revertMatrix.Invert(t1);
            CGContextConcatCTM( mContextRef, CGAffineTransformMake( revertMatrix.a, revertMatrix.b, revertMatrix.c, revertMatrix.d, revertMatrix.e, revertMatrix.f) );
            
            CMatrix tmpMatrix = mMatrix;
            CMatrix tmpExtMatrix = mExtMatrix;
            CMatrix newExtMatrix = tmpForm->GetMatrix();
            newExtMatrix.Concat( tmpExtMatrix );
            SetExtMatrix( newExtMatrix );
            DrawForm(tmpForm);
            SetExtMatrix( tmpExtMatrix );
            SetMatrix( tmpMatrix );
            
            RestoreGState();
            
            CGContextEndTransparencyLayer(mContextRef);
        }*/
    }
}

void	CPDF_Renderer::StrokePath()
{
    if ( mContextRef && mPathRef && !CGPathIsEmpty( mPathRef ) )
    {
        CGContextAddPath( mContextRef, mPathRef );
        CGContextStrokePath( mContextRef );
        ResetPath();
    }
}

void	CPDF_Renderer::FillStrokePath()
{
    if ( mContextRef && mPathRef && !CGPathIsEmpty( mPathRef ) )
    {
        if (mShading)
        {
            StoreGState();
            CGContextAddPath( mContextRef, mPathRef );
            CGContextClip( mContextRef );
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
        }
        
        CGContextAddPath( mContextRef, mPathRef );
        CGContextStrokePath( mContextRef );
        ResetPath();
    }
}

void	CPDF_Renderer::ClipPath()
{
    if ( mContextRef && mPathRef && !CGPathIsEmpty( mPathRef ) )
    {
        CGContextAddPath( mContextRef, mPathRef );
        CGContextClip( mContextRef );
        ResetPath();
    }
}

void	CPDF_Renderer::FillClipPath()
{
    if ( mContextRef && mPathRef && !CGPathIsEmpty( mPathRef ) )
    {
        if (mShading)
        {
            StoreGState();
            
            CGContextAddPath( mContextRef, mPathRef );
            CGContextClip( mContextRef );
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
        }

        CGContextAddPath( mContextRef, mPathRef );
        CGContextClip( mContextRef );
        ResetPath();
    }
}

void	CPDF_Renderer::StrokeClipPath()
{
    if ( mContextRef && mPathRef && !CGPathIsEmpty( mPathRef ) )
    {
        CGContextAddPath( mContextRef, mPathRef );
        CGContextStrokePath( mContextRef );
        CGContextAddPath( mContextRef, mPathRef );
        CGContextClip( mContextRef );
        ResetPath();
    }
}

void	CPDF_Renderer::FillStrokeClipPath()
{
    if ( mContextRef && mPathRef && !CGPathIsEmpty( mPathRef ) )
    {
        if (mShading)
        {
            StoreGState();
            
            CGContextAddPath( mContextRef, mPathRef );
            CGContextClip( mContextRef );
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
        }
        
        CGContextAddPath( mContextRef, mPathRef );
        CGContextStrokePath( mContextRef );
        CGContextAddPath( mContextRef, mPathRef );
        CGContextClip( mContextRef );
        ResetPath();
    }
}

void CPDF_Renderer::StoreGState()
{
    CGContextSaveGState( mContextRef );
}

void CPDF_Renderer::RestoreGState()
{
    CGContextRestoreGState( mContextRef );
}

void CPDF_Renderer::SetCommonGState( CPDF_GState * pGState, bool bColor, bool bMatrix )
{
    if ( pGState == NULL )
    {
        return;
    }
    
    static FLOAT val = 0;
	static CMatrix matrix;
	static GRAPHICS_STATE_LINECAP linCap = LineCap_Butt;
	static GRAPHICS_STATE_LINEJOIN lineJoin = LineJoin_Miter;
	static GRAPHICS_STATE_DASHPATTERN lineDash;
	static CPDF_Color fillColor;
	static CPDF_Color strokeColor;
	static CPDF_ColorSpacePtr fillColorSpace;
	static CPDF_ColorSpacePtr strokeColorSpace;
    
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
            CPDF_CS_SeparationPtr cs = fillColorSpace->GetSeparationPtr();
            if ( cs )
            {
                CPDF_FunctionPtr funcPtr = cs->mFunction;
                if ( funcPtr )
                {
                    std::vector<FLOAT> input;
                    std::vector<FLOAT> output;
                    for ( uint32 i = 0 ; i < fillColor.GetComponentCount(); ++i )
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
            CPDF_CS_DeviceNPtr cs = fillColorSpace->GetDeviceNPtr();
            if ( cs )
            {
                CPDF_FunctionPtr funcPtr = cs->mFunction;
                if ( funcPtr )
                {
                    std::vector<FLOAT> input;
                    std::vector<FLOAT> output;
                    for ( uint32 i = 0 ; i < fillColor.GetComponentCount(); ++i )
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
            CPDF_CS_SeparationPtr cs = strokeColorSpace->GetSeparationPtr();
            if ( cs )
            {
                CPDF_FunctionPtr funcPtr = cs->mFunction;
                if ( funcPtr )
                {
                    std::vector<FLOAT> input;
                    std::vector<FLOAT> output;
                    for ( uint32 i = 0 ; i < strokeColor.GetComponentCount(); ++i )
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
            CPDF_CS_DeviceNPtr cs = strokeColorSpace->GetDeviceNPtr();
            if ( cs )
            {
                CPDF_FunctionPtr funcPtr = cs->mFunction;
                if ( funcPtr )
                {
                    std::vector<FLOAT> input;
                    std::vector<FLOAT> output;
                    for ( uint32 i = 0 ; i < strokeColor.GetComponentCount(); ++i )
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
}

void CPDF_Renderer::SetExtGState( CPDF_ExtGStateStack * pExtGState )
{
    if ( pExtGState )
    {
        GRAPHICS_STATE_BLENDMODE blendMode = pExtGState->GetBlendMode();
        SetBlendMode( blendMode );
        
        mFillAlpha = pExtGState->GetFillAlpha();
        mStrokeAlpha = pExtGState->GetStrokeAlpha();
        
        CPDF_DictionaryPtr dict = pExtGState->GetSoftMaskDict();
        if (dict)
        {
            CPDF_ObjectPtr obj = dict->GetElement("G", OBJ_TYPE_REFERENCE);
            if ( obj )
            {
                CPDF_ComponentMgr mgr;
                mSoftMask = CPDF_FormXObject::Create(obj->GetRefPtr(), &mgr);
            }
        }
    }
}

void CPDF_Renderer::SetClipState( CPDF_ClipState * pClipState )
{
    if ( pClipState == NULL )
    {
        return;
    }
    CPDF_ContentObject * pObj = NULL;
	std::list<CPDF_ClipStateItem*>::iterator it = pClipState->mClipElementList.begin();
	for ( ; it != pClipState->mClipElementList.end(); it++ )
	{
		pObj = (*it)->GetElement();
		if ( !pObj )
		{
			continue;
		}
        CPDF_GState * pGState = pObj->GetGState();
        if ( pGState )
        {
            SetCommonGState( pGState );
        }
		if ( pObj->GetType() == ContentType_Path )
		{
            CPDF_Path * pPath = (CPDF_Path*)(pObj);
            CPoint p1, p2, p3;
            
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
            CMatrix matrix;
            CPDF_Text * pText = (CPDF_Text*)(pObj);
            CPDF_Font * pFont = pGState->GetTextFont();
            
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
		
        CMatrix invertMatrix;
        invertMatrix.Invert( mTmpMatrix );
        CGContextConcatCTM( mContextRef, CGAffineTransformMake( invertMatrix.a, invertMatrix.b, invertMatrix.c, invertMatrix.d, invertMatrix.e, invertMatrix.f) );
	}
}

void CGFontCleanCallBack( void * info )
{
    if ( info )
    {
        CGFontRef fontRef = (CGFontRef)info;
        CGFontRelease( fontRef );
    }
}

void CPDF_Renderer::DrawPath( CPDF_Path * pPath )
{
    if ( pPath->GetFillMode() == Mode_Nonzero )
    {
        SetFillMode( FillMode_Nonzero );
    }else{
        SetFillMode( FillMode_EvenOdd );
    }
    
    CPoint p1, p2, p3;
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

void CPDF_Renderer::DrawTextGlyph( CGGlyph gid )
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



void CPDF_Renderer::DrawText( CPDF_Text * pText )
{
    GRAPHICS_STATE_TEXTRENDERMODE rm = TextRenderMode_Fill;
    CPDF_GState * pGState = pText->GetGState();
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
            CPDF_Font * pFont = pText->GetGState()->GetTextFont();
            if ( pFont->GetFontType() == FONT_TYPE3 )
            {
                StoreGState();
                
                CMatrix inverstMatrix;
                inverstMatrix.Invert( mTmpMatrix );
                CGContextConcatCTM( mContextRef, CGAffineTransformMake( inverstMatrix.a, inverstMatrix.b, inverstMatrix.c, inverstMatrix.d, inverstMatrix.e, inverstMatrix.f) );
                
                CPDF_Type3_Font * pFontType3 = (CPDF_Type3_Font*)pFont;
                for ( size_t i = 0; i < pText->mItems.size(); ++i )
                {
                    StoreGState();
                    
                    CMatrix textMatirx = pText->GetCharMatrix( i );
                    textMatirx.Concat( mMatrix );
                    textMatirx.Concat( mExtMatrix );
                    CGContextConcatCTM( mContextRef, CGAffineTransformMake( textMatirx.a, textMatirx.b, textMatirx.c, textMatirx.d, textMatirx.e, textMatirx.f) );
                    CPDF_ContentObjectList * pList = pFontType3->GetGlyph( pText->mItems[i].charCode );
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
            CMatrix textMatirx;
            for ( size_t i = 0; i < pText->mItems.size(); ++i )
            {
                textMatirx = pText->GetCharMatrix( i );
                SetTextMatrix( textMatirx );
                
                sprintf( glyphName, "cid%u", pText->mItems[i].gid );
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

void CPDF_Renderer::DrawTextAsPath( CPDF_Text * pText )
{
    GRAPHICS_STATE_TEXTRENDERMODE rm = TextRenderMode_Fill;
    CPDF_GState * pGState = pText->GetGState();
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
        for ( size_t j = 0; j < pText->mItems.size(); ++j )
        {
            CPDF_Path * pPath = pText->GetGraphPath( j );
            if ( pPath )
            {
                for ( size_t i = 0; i < pPath->mItems.size(); ++i )
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
}

void CPDF_Renderer::DrawComponentRef( CPDF_ComponentRef * cmptRef )
{
    if ( cmptRef == NULL )
	{
		return;
	}
    
	CPDF_ComponentPtr componentPtr = cmptRef->GetComponentPtr();
    
	switch( componentPtr->GetType() )
	{
        case COMPONENT_TYPE_ImageXObject:
		{
			DrawRefImage( CPDF_ImageXObject::Convert( componentPtr ) );
			break;
		}
        case COMPONENT_TYPE_FormXObject:
		{
            StoreGState();
            
            CPDF_FormXObjectPtr formXObject = CPDF_FormXObject::Convert( componentPtr );
            
            FLOAT tmpFillAlpha = mFillAlpha;
            FLOAT tmpStrokeAlpha = mStrokeAlpha;
            
            if ( formXObject->IsGroup() )
            {
                CGContextSetAlpha(mContextRef, mFillAlpha);
                mFillAlpha = 1.0f;
                mStrokeAlpha = 1.0f;
                CGContextBeginTransparencyLayer(mContextRef, nullptr);
            }
            
            CMatrix t1 = mMatrix;
            t1.Concat(mExtMatrix);
            
            CMatrix revertMatrix;
            revertMatrix.Invert(t1);
            CGContextConcatCTM( mContextRef, CGAffineTransformMake( revertMatrix.a, revertMatrix.b, revertMatrix.c, revertMatrix.d, revertMatrix.e, revertMatrix.f) );
            
            CMatrix tmpMatrix = mMatrix;
			CMatrix tmpExtMatrix = mExtMatrix;
            CMatrix newExtMatrix = formXObject->GetMatrix();
			CPDF_GState * pGState = cmptRef->GetGState();
			if ( pGState )
			{
                newExtMatrix.Concat(pGState->GetMatrix());
			}
			newExtMatrix.Concat( tmpExtMatrix );
			SetExtMatrix( newExtMatrix );
            DrawForm( formXObject );
			SetExtMatrix( tmpExtMatrix );
            SetMatrix( tmpMatrix );
            
            if ( formXObject->IsGroup() )
            {
                mFillAlpha = tmpFillAlpha;
                mStrokeAlpha = tmpStrokeAlpha;
                CGContextEndTransparencyLayer(mContextRef);
            }
            
            RestoreGState();
			break;
		}
        case COMPONENT_TYPE_Shading:
		{
            DrawShading( CPDF_Shading::Convert(componentPtr) );
			break;
		}
        default:
            break;
	}
}

void CPDF_Renderer::DrawRefImage( const CPDF_ImageXObjectPtr & image )
{
    if ( image )
    {
        CGImageRef imgRef = CreateImage( image );
        
        CPDF_ImageXObjectPtr maskImagePtr = image->GetMaskImagePtr();
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

void CPDF_Renderer::DrawInlineImage( CPDF_InlineImage * pImage )
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
    CPDF_Renderer * pRender = (CPDF_Renderer*)info;
    std::vector<FLOAT> input;
    std::vector<FLOAT> output;
    input.push_back( in[0] );
    pRender->shadingFunc->Calculate( input, output );
    
    if ( pRender->shadingCS->GetColorSpaceType() == COLORSPACE_SPECIAL_SEPARATION )
        
    {
        std::vector<FLOAT> routput;
        pRender->shadingCS->GetSeparationPtr()->mFunction->Calculate(output, routput);
        output = routput;
    }else if ( pRender->shadingCS->GetColorSpaceType() == COLORSPACE_SPECIAL_DEVICEN )
    {
        std::vector<FLOAT> routput;
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

void FunctionReleaseInfoCallback(void * __nullable info)
{
    
}

void CPDF_Renderer::DrawShading( const CPDF_ShadingPtr & shading )
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
        
        CPDF_AxialShadingPtr ashading = shading->GetAixalShadingPtr();
        
        CPDF_FunctionPtr fun = ashading->GetFunction();
        
        if (!fun )
        {
            return ;
        }
        shadingFunc = fun;
        
        CPDF_ColorSpacePtr cs = shading->GetColorSpace();
        
        if ( !cs )
        {
            return;
        }
        
        CPDF_Color color = shading->GetBackgroundColor();
        if ( color.GetComponentCount() )
        {
            CGRect rc = CGContextGetClipBoundingBox(mContextRef);
            SetFillColorSpace(cs);
            SetFillColor(color);
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
        CPDF_ColorSpacePtr cs = shading->GetColorSpace();
        
        CPDF_RadialShadingPtr ashading = shading->GetRadialShadingPtr();
        
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
        
        CPDF_FunctionPtr fun = ashading->GetFunction();
        
        if (!fun )
        {
            return ;
        }
        shadingFunc = fun;
        
        if ( !cs )
        {
            return;
        }
        
        CPDF_Color color = shading->GetBackgroundColor();
        if ( color.GetComponentCount() )
        {
            CGRect rc = CGContextGetClipBoundingBox(mContextRef);
            SetFillColorSpace(cs);
            SetFillColor(color);
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
        CGShadingRef shadingRef = CGShadingCreateRadial(csref, CGPointMake(ashading->GetStartPoint().x, ashading->GetStartPoint().y), ashading->GetStartRadius(), CGPointMake(ashading->GetEndPoint().x, ashading->GetEndPoint().y), ashading->GetEndRadius(), funcRef, ashading->GetStartExtend(), ashading->GetEndExtend());
        CGContextDrawShading(mContextRef, shadingRef);
    }
}

void CPDF_Renderer::DrawForm( const CPDF_FormXObjectPtr & form )
{
    
    CPDF_ContentObjectList & list = form->GetList();
    ContentObjectList::iterator it = list.Begin();
    CPDF_GState * pGState = NULL;
    CPDF_ClipState * pClipState = NULL;
    CPDF_ExtGStateStack * pExtGStateStack = NULL;
    
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
				DrawPath( (CPDF_Path*)(*it) );
				break;
			}
            case ContentType_Text:
			{
                DrawText( (CPDF_Text*)(*it) );
				break;
			}
            case ContentType_InlineImage:
			{
				DrawInlineImage( (CPDF_InlineImage*)(*it) );
				break;
			}
            case ContentType_Component:
			{
				DrawComponentRef( (CPDF_ComponentRef*)(*it) );
				break;
			}
            default:
                break;
		}
        RestoreGState();
	}
}

void CPDF_Renderer::DrawContentObjectList( CPDF_ContentObjectList & list )
{
    ContentObjectList::iterator it = list.Begin();
    CPDF_GState * pGState = NULL;
    CPDF_ClipState * pClipState = NULL;
    CPDF_ExtGStateStack * pExtGStateStack = NULL;
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
				DrawPath( (CPDF_Path*)(*it) );
				break;
			}
            case ContentType_Text:
			{
                DrawText( (CPDF_Text*)(*it) );
				break;
			}
            case ContentType_InlineImage:
			{
				DrawInlineImage( (CPDF_InlineImage*)(*it) );
				break;
			}
            case ContentType_Component:
			{
				DrawComponentRef( (CPDF_ComponentRef*)(*it) );
				break;
			}
            default:
                break;
		}
        RestoreGState();
	}
}

void CPDF_Renderer::Render( CPDF_ContentObjectList & content, CRect pageRect, uint32 rotate, FLOAT scale, FLOAT dpix, FLOAT dpiy )
{
    mbNoColor = FALSE;
    mDpix = dpix;
    mDpiy = dpiy;
    mScale = scale;
    mRotate = rotate;
    mPageRect = pageRect;
    
    //计算extMatrix
    CMatrix matrix;
    if ( rotate == 90 )
    {
        CMatrix rotateMatrix = CMatrix::RotateMatrix( rotate );
        CMatrix transformMatrix = CMatrix::TranslateMatrix( pageRect.height, 0 );
        rotateMatrix.Concat( transformMatrix );
        matrix.Concat( rotateMatrix );
    }else if ( rotate == 180 )
    {
        CMatrix rotateMatrix = CMatrix::RotateMatrix( rotate );
        CMatrix transformMatrix = CMatrix::TranslateMatrix( pageRect.width, pageRect.height );
        rotateMatrix.Concat( transformMatrix );
        matrix.Concat( rotateMatrix );
    }else if ( rotate == 270 )
    {
        CMatrix rotateMatrix = CMatrix::RotateMatrix( rotate );
        CMatrix transformMatrix = CMatrix::TranslateMatrix( 0, pageRect.width );
        rotateMatrix.Concat( transformMatrix );
        matrix.Concat( rotateMatrix );
    }
    
    CRect newPageRect = matrix.Transform( pageRect );
    
    CMatrix bboxClipMatrix;
    bboxClipMatrix.a = 1;
    bboxClipMatrix.b = 0;
    bboxClipMatrix.c = 0;
    bboxClipMatrix.d = 1;
    bboxClipMatrix.e = -newPageRect.left;
    bboxClipMatrix.f = -newPageRect.bottom;
    matrix.Concat( bboxClipMatrix );
    
    CMatrix flipMatrix;
    flipMatrix.a = 1;
    flipMatrix.b = 0;
    flipMatrix.c = 0;
    flipMatrix.d = -1;
    flipMatrix.e = 0;
    flipMatrix.f = newPageRect.height;
    matrix.Concat( flipMatrix );
    
	CMatrix scaletMatrix;
	scaletMatrix.a = dpix * scale / 72;
	scaletMatrix.b = 0;
	scaletMatrix.c = 0;
	scaletMatrix.d = dpiy * scale / 72;
	scaletMatrix.e = 0;
    scaletMatrix.f = 0;
    matrix.Concat( scaletMatrix );
    
	CMatrix offsetMatrix;
    offsetMatrix.e = mPosiX;
    offsetMatrix.f = mPosiY;
    
    matrix.Concat( offsetMatrix );
    SetExtMatrix( matrix );
    
    StoreGState();
    CGContextBeginTransparencyLayer(mContextRef, NULL);
    
    //clip当前页面绘制的区域
    newPageRect = matrix.Transform( pageRect );
    CGContextAddRect( mContextRef, CGRectMake( newPageRect.left, newPageRect.bottom, newPageRect.width, newPageRect.height ) );
    CGContextClip( mContextRef );
    
	CPDF_GState * pGState = NULL;
	CPDF_ClipState * pClipState = NULL;
    CPDF_ExtGStateStack * pExtGStateStack = NULL;
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
            }else{
                SetBlendMode(BlendMode_Normal);
                mFillAlpha = 1.0f;
                mStrokeAlpha = 1.0f;
            }
            SetCommonGState( pGState );
		}
		switch ( (*it)->GetType() )
		{
            case ContentType_Path:
				DrawPath( (CPDF_Path*)(*it) );
				break;
            case ContentType_Text:
                DrawText( (CPDF_Text*)(*it) );
                break;
            case ContentType_InlineImage:
				DrawInlineImage( (CPDF_InlineImage*)(*it) );
				break;
            case ContentType_Component:
				DrawComponentRef( (CPDF_ComponentRef*)(*it) );
                break;
            default: break;
		}
        RestoreGState();
	}
    RestoreGState();
    CGContextEndTransparencyLayer(mContextRef);
}

void CPDF_Renderer::RenderTiling( CPDF_ContentObjectList & content, bool bColored )
{
	CPDF_GState * pGState = NULL;
	CPDF_ClipState * pClipState = NULL;
    CPDF_ExtGStateStack * pExtGStateStack = NULL;
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
				DrawPath( (CPDF_Path*)(*it) );
				break;
            case ContentType_Text:
                DrawText( (CPDF_Text*)(*it) );
                break;
            case ContentType_InlineImage:
				DrawInlineImage( (CPDF_InlineImage*)(*it) );
				break;
            case ContentType_Component:
				DrawComponentRef( (CPDF_ComponentRef*)(*it) );
				break;
            default: break;
		}
        RestoreGState();
	}
}

}//namespace