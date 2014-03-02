#include "../../include/pdf/che_pdf_renderer_macosx.h"
#include "../../include/che_bitmap.h"

CHE_PDF_Renderer::CHE_PDF_Renderer( CGContextRef cgContext )
    : mScale(1.0f), mDipx(96.0f), mDipy(96.0f), mPosiX(0.0f), mPosiY(0.0f), mFillMode(FillMode_Nonzero),
    mContextRef(cgContext), mPathRef(NULL), mFillColorSpace(NULL), mStrokeColorSpace(NULL), mImageColorSpace(NULL)
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

HE_VOID CHE_PDF_Renderer::SetDIP( HE_FLOAT dipx, HE_FLOAT dipy )
{
    mDipx = dipx;
    mDipy = dipy;
}

HE_VOID CHE_PDF_Renderer::SetScale( HE_FLOAT scale )
{
    mScale = scale;
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
    CGContextSetFillColor( mContextRef, val );
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
    if ( mFillColorSpace )
    {
        CGColorSpaceRelease( mFillColorSpace );
        mFillColorSpace = NULL;
    }
    
    mFillColorSpace = CreateColorSpace( cs );
    CGContextSetFillColorSpace( mContextRef , mFillColorSpace );
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
            csRef = CGColorSpaceCreateCalibratedGray( cs->mWhitePoint, cs->mBlackPoint, cs->mGamma[0]);
            break;
        case COLORSPACE_CIEBASE_CALRGB:
            csRef = CGColorSpaceCreateCalibratedRGB( cs->mWhitePoint , cs->mBlackPoint, cs->mGamma, cs->mMatrix );
            break;
        case COLORSPACE_CIEBASE_CALLAB:
            csRef = CGColorSpaceCreateLab( cs->mWhitePoint, cs->mBlackPoint, cs->mRange );
            break;
        case COLORSPACE_CIEBASE_ICCBASED:
            {
                CGColorSpaceRef baseColorSpaceRef = CreateColorSpace( cs->mBaseColorspace );
                if ( baseColorSpaceRef )
                {
                    CGDataProviderRef dataRef = CGDataProviderCreateWithData( NULL, cs->mStmAcc.GetData(), cs->mStmAcc.GetSize(), NULL );
                    csRef = CGColorSpaceCreateICCBased( cs->GetComponentCount(), cs->mRange, dataRef, baseColorSpaceRef );
                    CGDataProviderRelease( dataRef );
                    CGColorSpaceRelease( baseColorSpaceRef );
                }
                break;
            }
        case COLORSPACE_SPECIAL_INDEXED:
            {
                CGColorSpaceRef baseColorSpaceRef = CreateColorSpace( cs->mBaseColorspace );
                if ( baseColorSpaceRef )
                {
                    csRef = CGColorSpaceCreateIndexed( baseColorSpaceRef, cs->mIndexCount - 1, cs->mpIndexTable );
                    CGColorSpaceRelease( baseColorSpaceRef );
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
        
        CHE_PDF_ColorSpacePtr csPtr = imagePtr->GetColorspace();
        if ( !csPtr )
        {
            if ( imagePtr->IsMask() )
            {
                CHE_PDF_ObjectPtr objPtr;
                CHE_PDF_ArrayPtr arrayPtr = imagePtr->GetDecodeArray();
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
                
                imgRef = CGImageMaskCreate( imagePtr->GetWidth(), imagePtr->GetHeight(), imagePtr->GetBPC(), imagePtr->GetBPC(), (imagePtr->GetWidth() * imagePtr->GetBPC() + 7)/8, dataRef, pDecode, imagePtr->IsInterpolate() );
                CGDataProviderRelease( dataRef );
            }
            return imgRef;
        }
        
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
    CHE_Matrix tmpMatrix;
    tmpMatrix = mMatrix;
    tmpMatrix.Concat( mExtMatrix );
    CGAffineTransform affine = CGAffineTransformMake( tmpMatrix.a, tmpMatrix.b, tmpMatrix.c, tmpMatrix.d, tmpMatrix.e, tmpMatrix.f);
    CGPathMoveToPoint( mPathRef, &affine, x, y );
}

HE_VOID	CHE_PDF_Renderer::LineTo( HE_FLOAT x, HE_FLOAT y )
{
    if ( mPathRef )
    {
        CHE_Matrix tmpMatrix;
        tmpMatrix = mMatrix;
        tmpMatrix.Concat( mExtMatrix );
        CGAffineTransform affine = CGAffineTransformMake( tmpMatrix.a, tmpMatrix.b, tmpMatrix.c, tmpMatrix.d, tmpMatrix.e, tmpMatrix.f);
        CGPathAddLineToPoint( mPathRef, &affine, x, y );
    }
}

HE_VOID	CHE_PDF_Renderer::CurveTo( HE_FLOAT x1, HE_FLOAT y1, HE_FLOAT x2, HE_FLOAT y2, HE_FLOAT x3, HE_FLOAT y3 )
{
    if ( mPathRef )
    {
        CHE_Matrix tmpMatrix;
        tmpMatrix = mMatrix;
        tmpMatrix.Concat( mExtMatrix );
        CGAffineTransform affine = CGAffineTransformMake( tmpMatrix.a, tmpMatrix.b, tmpMatrix.c, tmpMatrix.d, tmpMatrix.e, tmpMatrix.f);
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
        CHE_Matrix tmpMatrix;
        tmpMatrix = mMatrix;
        tmpMatrix.Concat( mExtMatrix );
        CGAffineTransform affine = CGAffineTransformMake( tmpMatrix.a, tmpMatrix.b, tmpMatrix.c, tmpMatrix.d, tmpMatrix.e, tmpMatrix.f);
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

HE_VOID CHE_PDF_Renderer::SetCommonGState( CHE_PDF_GState * pGState )
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
	static HE_UINT32 fillColorVal = 0xFF000000;
	static HE_UINT32 strokeColorVal = 0xFF000000;
    
	pGState->GetLineWidth( val );
	SetLineWidth( val );
    
	pGState->GetLineCap( linCap );
	SetLineCap( linCap );
    
	pGState->GetLineJoin( lineJoin );
	SetLineJoin( lineJoin );
    
	pGState->GetLineDash( lineDash );
	SetLineDash( lineDash );
    
	matrix = pGState->GetMatrix();
	SetMatrix( matrix );
    
	pGState->GetFillColor( fillColor );
	pGState->GetStrokeColor( strokeColor );
	pGState->GetFillColorSpace( fillColorSpace );
	pGState->GetStrokeColorSpace( strokeColorSpace );
    
    SetFillColorSpace( fillColorSpace );
    SetStrokeColorSpace( strokeColorSpace );
    SetFillColor( fillColor );
    SetStrokeColor( strokeColor );
    
	SetFillMode( FillMode_Nonzero );
	//ResetClip();
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
        
		switch ( pObj->GetType() )
		{
            case ContentType_Path:
			{
				CHE_PDF_Path * pPath = (CHE_PDF_Path*)(pObj);
				CHE_Point p1, p2, p3;
				for ( size_t i = 0; i < pPath->mItems.size(); ++i )
				{
					switch ( pPath->mItems[i].type )
					{
                        case PathItem_MoveTo:
						{
							p1.x = pPath->mItems[i+1].value;
							p1.y = pPath->mItems[i+2].value;
							MoveTo( p1.x, p1.y );
							i+=2;
							break;
						}
                        case PathItem_LineTo:
						{
							p1.x = pPath->mItems[i+1].value;
							p1.y = pPath->mItems[i+2].value;
							LineTo( p1.x, p1.y );
							i+=2;
							break;
						}
                        case PathItem_CurveTo:
						{
							p1.x = pPath->mItems[i+1].value;
							p1.y = pPath->mItems[i+2].value;
							p2.x = pPath->mItems[i+3].value;
							p2.y = pPath->mItems[i+4].value;
							p3.x = pPath->mItems[i+5].value;
							p3.y = pPath->mItems[i+6].value;
							CurveTo( p1.x, p1.y, p2.x, p2.y, p3.x, p3.y );
							i+=6;
							break;
						}
                        case PathItem_Rectangle:
						{
							p1.x = pPath->mItems[i+1].value;
							p1.y = pPath->mItems[i+2].value;
							MoveTo( p1.x, p1.y );
							LineTo( p1.x + pPath->mItems[i+3].value, p1.y );
							LineTo( p1.x + pPath->mItems[i+3].value, p1.y + pPath->mItems[i+4].value );
							LineTo( p1.x, p1.y + pPath->mItems[i+4].value );
							ClosePath();
							i+=4;
							break;
						}
                        case PathItem_Close:
						{
							ClosePath();
							break;
						}
                        default:
                            break;
					}
				}
				break;
			}
            case ContentType_Text:
			{
				SetMatrix( CHE_Matrix() );
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
								{
									MoveTo( pPath->mItems[i+1].value, pPath->mItems[i+2].value );
									i+=2;
									break;
								}
                                case PathItem_LineTo:
								{
									LineTo( pPath->mItems[i+1].value, pPath->mItems[i+2].value );
									i+=2;
									break;
								}
                                case PathItem_CurveTo:
								{
									CurveTo(    pPath->mItems[i+1].value, pPath->mItems[i+2].value,
                                                pPath->mItems[i+3].value, pPath->mItems[i+4].value,
                                                pPath->mItems[i+5].value, pPath->mItems[i+6].value );
									i+=6;
									break;
								}
                                case PathItem_Rectangle:
								{
									MoveTo( pPath->mItems[i+1].value, pPath->mItems[i+2].value );
									LineTo( pPath->mItems[i+1].value + pPath->mItems[i+3].value, pPath->mItems[i+2].value );
									LineTo( pPath->mItems[i+1].value + pPath->mItems[i+3].value, pPath->mItems[i+2].value + pPath->mItems[i+4].value );
									LineTo( pPath->mItems[i+1].value, pPath->mItems[i+2].value + pPath->mItems[i+4].value );
									ClosePath();
									i+=4;
									break;
								}
                                case PathItem_Close:
								{
									ClosePath();
									i+=1;
									break;
								}
                                default:
                                    break;
							}
						}
						ClosePath();
						pPath->GetAllocator()->Delete( pPath );
					}
				}
				break;
			}
            default:
                break;
		}
	}
    
	ClipPath();
}

HE_VOID CHE_PDF_Renderer::DrawPath( CHE_PDF_Path * pPath )
{
    if ( pPath->GetFillMode() == Mode_Nonzero )
    {
        SetFillMode( FillMode_Nonzero );
    }
    else{
        SetFillMode( FillMode_EvenOdd );
    }
    CHE_Point p1, p2, p3;
    for ( size_t i = 0; i < pPath->mItems.size(); ++i )
    {
        switch ( pPath->mItems[i].type )
        {
            case PathItem_MoveTo:
            {
                p1.x = pPath->mItems[i+1].value;
                p1.y = pPath->mItems[i+2].value;
                MoveTo( p1.x, p1.y );
                i+=2;
				break;
            }
            case PathItem_LineTo:
            {
                p1.x = pPath->mItems[i+1].value;
                p1.y = pPath->mItems[i+2].value;
                LineTo( p1.x, p1.y );
                i+=2;
                break;
            }
            case PathItem_CurveTo:
            {
                p1.x = pPath->mItems[i+1].value;
                p1.y = pPath->mItems[i+2].value;
                p2.x = pPath->mItems[i+3].value;
                p2.y = pPath->mItems[i+4].value;
                p3.x = pPath->mItems[i+5].value;
                p3.y = pPath->mItems[i+6].value;
                CurveTo( p1.x, p1.y, p2.x, p2.y, p3.x, p3.y );
				i+=6;
                break;
            }
            case PathItem_Rectangle:
            {
                p1.x = pPath->mItems[i+1].value;
                p1.y = pPath->mItems[i+2].value;
                MoveTo( p1.x, p1.y );
                LineTo( p1.x + pPath->mItems[i+3].value, p1.y );
                LineTo( p1.x + pPath->mItems[i+3].value, p1.y + pPath->mItems[i+4].value );
                LineTo( p1.x, p1.y + pPath->mItems[i+4].value );
                ClosePath();
                i+=4;
                break;
            }
            case PathItem_Close:
            {
                ClosePath();
                break;
            }
            default:
                break;
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
    CGPoint position;
    position.x = 0;
    position.y = 0;
    CGContextSetFontSize( mContextRef , 1 );
    
    CHE_Matrix tmpMatrix;
    tmpMatrix = mTextMatrix;
    tmpMatrix.Concat( mExtMatrix );
    CGContextSetTextMatrix( mContextRef, CGAffineTransformMake( tmpMatrix.a, tmpMatrix.b, tmpMatrix.c, tmpMatrix.d, tmpMatrix.e, tmpMatrix.f) );
    CGContextShowGlyphsAtPositions( mContextRef, &gid, &position, 1 );
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
            SetMatrix( CHE_Matrix() );
            //对于Fill类型的文本输出，可以使用系统原生文本输出接口，以获得次像素支持
            
            CHE_PDF_Font * pFont = pText->GetGState()->GetTextFont();
            if ( pFont->GetPlatformFontInfo() )
            {
                CGFontRef fontRef = (CGFontRef)pFont->GetPlatformFontInfo();
                SetTextFont( fontRef );
            }else{
                CFDataRef dataRef = CFDataCreateWithBytesNoCopy( kCFAllocatorDefault, pFont->GetEmbededFont(), pFont->GetEmbededFontSize(), kCFAllocatorNull );
                if ( dataRef )
                {
                    CGDataProviderRef dataProviderRef = CGDataProviderCreateWithCFData( dataRef );
                    CGFontRef fontRef = CGFontCreateWithDataProvider( dataProviderRef );
                    if ( fontRef )
                    {
                        pFont->SetPlatformFontInfo( fontRef );
                        pFont->SetPlatformFontInfoCleanCallBack( CGFontCleanCallBack );
                        SetTextFont( fontRef );
                    }
                }
            }
            
            CHE_Matrix textMatirx;
            for ( HE_ULONG i = 0; i < pText->mItems.size(); ++i )
            {
                textMatirx = pText->GetCharMatrix( i );
                SetTextMatrix( textMatirx );
                DrawTextGlyph( pText->mItems[i].gid );
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
        {
            SetMatrix( CHE_Matrix() );
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
                            {
                                MoveTo( pPath->mItems[i+1].value, pPath->mItems[i+2].value );
                                i+=2;
                                break;
                            }
                            case PathItem_LineTo:
                            {
                                LineTo( pPath->mItems[i+1].value, pPath->mItems[i+2].value );
                                i+=2;
                                break;
                            }
                            case PathItem_CurveTo:
                            {
                                CurveTo(    pPath->mItems[i+1].value, pPath->mItems[i+2].value,
                                            pPath->mItems[i+3].value, pPath->mItems[i+4].value,
                                            pPath->mItems[i+5].value, pPath->mItems[i+6].value );
                                i+=6;
                                break;
                            }
                            case PathItem_Rectangle:
                            {
                                MoveTo( pPath->mItems[i+1].value, pPath->mItems[i+2].value );
                                LineTo( pPath->mItems[i+1].value + pPath->mItems[i+3].value, pPath->mItems[i+2].value );
                                LineTo( pPath->mItems[i+1].value + pPath->mItems[i+3].value, pPath->mItems[i+2].value + pPath->mItems[i+4].value );
                                LineTo( pPath->mItems[i+1].value, pPath->mItems[i+2].value + pPath->mItems[i+4].value );
                                ClosePath();
                                i+=4;
                                break;
                            }
                            case PathItem_Close:
                            {
                                ClosePath();
                                i+=1;
                                break;
                            }
                            default:
                                break;
                        }
                    }
                    pPath->GetAllocator()->Delete( pPath );
                }
            }
            break;
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
        default:break;
    }
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
			CHE_Matrix matrix;
			CHE_PDF_GState * pGState = cmptRef->GetGState();
			if ( pGState )
			{
				matrix = pGState->GetMatrix();
			}
			DrawRefImage( CHE_PDF_ImageXObject::Convert( componentPtr ) );
			break;
		}
        case COMPONENT_TYPE_FormXObject:
		{
			//CHE_Matrix tmpExtMatrix = extMatrix;
			//CHE_Matrix newExtMatrix;
			//CHE_PDF_GState * pGState = cmptRef->GetGState();
			//if ( pGState )
			//{
			//	newExtMatrix = pGState->GetMatrix();
            //
			//}
			///*newExtMatrix.Concat( pForm->GetExtMatrix() );*/
			//newExtMatrix.Concat( tmpExtMatrix );
            //
			//drawer.SetExtMatrix( newExtMatrix );
            //
			//OutputForm( CHE_PDF_FormXObject::Convert( componentPtr ), extMatrix, drawer );
            //
			//drawer.SetExtMatrix( extMatrix );
			break;
		}
        case COMPONENT_TYPE_Shading:
		{
            //DrawShading( )
			//OutputShading( pComponentRef->GetComponentPtr()->GetShadingPtr(), drawer );
			break;
		}
        default:
            break;
	}
}

/*CHE_Bitmap * pBitmap = image->GetBitmap();
 if ( pBitmap )
 {
 CGDataProviderRef dataRef = CGDataProviderCreateWithData( NULL, pBitmap->GetBuffer(), pBitmap->GetMemBitmapDataSize(), NULL );
 CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
 CGImageRef imageRef = CGImageCreate( pBitmap->Width(), pBitmap->Height(), 8, pBitmap->Depth(),
 pBitmap->Pitch(), colorSpace, kCGBitmapByteOrder32Little | kCGImageAlphaNoneSkipFirst,
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
 CGContextDrawImage( mContextRef, CGRectMake( rect.left, rect.bottom, rect.width, rect.height), imageRef );
 CGImageRelease( imageRef );
 CGColorSpaceRelease( colorSpace );
 CGDataProviderRelease( dataRef );
 }*/

//return;


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
            CHE_Matrix tmpMatrix;
            tmpMatrix = mMatrix;
            tmpMatrix.Concat( mExtMatrix );
            CHE_Rect rect;
            rect.left = 0;
            rect.bottom = 0;
            rect.width = 1;
            rect.height = 1;
            rect = tmpMatrix.Transform( rect );
            CGContextDrawImage( mContextRef, CGRectMake( rect.left, rect.bottom, rect.width, rect.height), imgRef );
            CGImageRelease( imgRef );
        }
    }
}

HE_VOID CHE_PDF_Renderer::DrawInlineImage( CHE_PDF_InlineImage * pImage )
{
    CHE_Bitmap * pBitmap = pImage->GetBitmap();
    if ( pBitmap )
    {
        CGDataProviderRef dataRef = CGDataProviderCreateWithData( NULL, pBitmap->GetBuffer(), pBitmap->GetMemBitmapDataSize(), NULL );
        CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
        CGImageRef imageRef = CGImageCreate(    pBitmap->Width(), pBitmap->Height(), 8, pBitmap->Depth(),
                                                pBitmap->Pitch(), colorSpace, kCGBitmapByteOrder32Little | kCGImageAlphaNoneSkipFirst,
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
        CGContextDrawImage( mContextRef, CGRectMake( rect.left, rect.bottom, rect.width, rect.height), imageRef );
        CGImageRelease( imageRef );
        CGColorSpaceRelease( colorSpace );
        CGDataProviderRelease( dataRef );
    }
}


HE_VOID CHE_PDF_Renderer::DrawShading( const CHE_PDF_ShadingPtr & shading )
{
    
}

HE_VOID CHE_PDF_Renderer::DrawForm( const CHE_PDF_FormXObjectPtr & form, const CHE_Matrix & extMatrix )
{
    /*CHE_PDF_GState * pGState = NULL;
	CHE_PDF_ClipState * pClipState = NULL;
    CHE_PDF_ContentObjectList & content = form->GetList();
	ContentObjectList::iterator it = content.Begin();
    
	CHE_Matrix tmpExtMatrix = extMatrix;
	CHE_Matrix newExtMatrix;
	pGState = form->GetGState();
	if ( pGState )
	{
		newExtMatrix = pGState->GetMatrix();
		
	}
	newExtMatrix.Concat( pForm->GetExtMatrix() );
	newExtMatrix.Concat( tmpExtMatrix );
    
	SetExtMatrix( newExtMatrix );
    
	for ( ; it != content.End(); ++it )
	{
        StoreGState();
		//ResetClip();
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
				DrawComponentRef( (CHE_PDF_ComponentRef*)(*it) );
				break;
			}
            default:
                break;
		}
        
        RestoreGState();
	}
    
	SetExtMatrix( extMatrix );*/
}

HE_VOID CHE_PDF_Renderer::Render( CHE_PDF_ContentObjectList & content )
{
	CHE_Matrix tmpMatrix;
	tmpMatrix.a = mDipx * mScale / 72;
	tmpMatrix.b = 0;
	tmpMatrix.c = 0;
	tmpMatrix.d = mDipy * mScale / 72;
	tmpMatrix.e = 0;
	tmpMatrix.f = 0;
	CHE_Matrix extMatrix;
	//if ( pClipRect != NULL )
	//{
	//	extMatrix.e = - pClipRect->left * dipx * scale / 72;
	//	extMatrix.f = ( pClipRect->height + pClipRect->bottom ) * dipy * scale / 72;
	//}else{
	//	extMatrix.e = 0;
	//	extMatrix.f = pageRect.height * dipy * scale / 72;
	//}
	tmpMatrix.Concat( extMatrix );
	SetExtMatrix( tmpMatrix );
    
    
	CHE_PDF_GState * pGState = NULL;
	CHE_PDF_ClipState * pClipState = NULL;
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