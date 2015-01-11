#include "../../include/pdf/che_pdf_xobject.h"
#include "../../include/pdf/che_pdf_contentlistbuilder.h"

#include "../../extlib/libjpeg/jpeglib.h"
#include "../../extlib/openjpeg/openjpeg.h"
#include "../../extlib/jbig2dec/jbig2.h"

CHE_PDF_ImageXObjectPtr CHE_PDF_ImageXObject::Create( const CHE_PDF_ReferencePtr & refPtr, CHE_Allocator * pAllocator/*= NULL*/ )
{
	CHE_PDF_ImageXObjectPtr ptr;
	if ( ! refPtr || ! refPtr->GetRefObj( OBJ_TYPE_STREAM ) )
	{
		return ptr;
	}
	if ( pAllocator == NULL )
	{
		pAllocator = GetDefaultAllocator();
	}
	ptr.Reset( pAllocator->New<CHE_PDF_ImageXObject>( refPtr, pAllocator ) );
	return ptr;
}

CHE_PDF_ImageXObjectPtr CHE_PDF_ImageXObject::Convert( const CHE_PDF_ComponentPtr & componentPtr )
{
	CHE_PDF_ImageXObjectPtr ptr;
	if ( componentPtr && componentPtr->GetType() == COMPONENT_TYPE_ImageXObject )
	{
		ptr.Reset( componentPtr.GetPointer() );
	}
	return ptr;
}


CHE_PDF_ImageXObject::CHE_PDF_ImageXObject( const CHE_PDF_ReferencePtr & refPtr, CHE_Allocator * pAllocator/*= NULL*/ )
	: CHE_PDF_Component( COMPONENT_TYPE_ImageXObject, refPtr, pAllocator ), mWidth(0), mHeight(0), mBpc(0),
mbInterpolate(FALSE), mStmAcc(pAllocator), mbMask(FALSE), mpBitmapCache(NULL), mRI(RI_AbsoluteColorimetric)
{
	if ( refPtr )
	{
		CHE_PDF_ObjectPtr objPtr = refPtr->GetRefObj( OBJ_TYPE_STREAM );
		if ( objPtr )
		{
			mStmPtr = objPtr->GetStreamPtr();
			CHE_PDF_DictionaryPtr dictPtr = mStmPtr->GetDictPtr();
			if ( dictPtr )
			{
				objPtr = dictPtr->GetElement( "Width", OBJ_TYPE_NUMBER );
				if ( objPtr )
				{
					mWidth = objPtr->GetNumberPtr()->GetInteger();
				}
				objPtr = dictPtr->GetElement( "Height", OBJ_TYPE_NUMBER );
				if ( objPtr )
				{
					mHeight = objPtr->GetNumberPtr()->GetInteger();
				}
				objPtr = dictPtr->GetElement( "BitsPerComponent", OBJ_TYPE_NUMBER );
				if ( objPtr )
				{
					mBpc = objPtr->GetNumberPtr()->GetInteger();
				}
				objPtr = dictPtr->GetElement( "ColorSpace" );
				if ( objPtr )
				{
                    if ( objPtr->GetType() == OBJ_TYPE_REFERENCE )
                    {
                        objPtr = objPtr->GetRefPtr()->GetRefObj();
                    }
					mColorspace = CHE_PDF_ColorSpace::Create( objPtr, GetAllocator() );
				}
				objPtr = dictPtr->GetElement( "Interpolate", OBJ_TYPE_BOOLEAN );
				if ( objPtr )
				{
					mbInterpolate = objPtr->GetBooleanPtr()->GetValue();
				}
				objPtr = dictPtr->GetElement( "ImageMask", OBJ_TYPE_BOOLEAN );
				if ( objPtr )
				{
					if ( objPtr->GetBooleanPtr()->GetValue() == TRUE )
					{
						mbMask = TRUE;
					}
				}
				objPtr = dictPtr->GetElement( "Decode", OBJ_TYPE_ARRAY );
				if ( objPtr )
				{
                    mDecodeArray = objPtr->GetArrayPtr();
					/*CHE_PDF_ArrayPtr arrayPtr = objPtr->GetArrayPtr();
					if ( arrayPtr->GetCount() == 2 )
					{
						objPtr = arrayPtr->GetElement( 0, OBJ_TYPE_NUMBER );
						if ( objPtr )
						{
							if ( objPtr->GetNumberPtr()->GetInteger() == 1 )
							{
								mMaskDecode = 1;
							}else{
								mMaskDecode = 0;
							}
							mbMask = TRUE;
						}
					}*/
				}
                objPtr = dictPtr->GetElement( "Intent", OBJ_TYPE_NAME );
                if ( objPtr )
                {
                    CHE_ByteString str = objPtr->GetNamePtr()->GetString();
                    if ( str == "AbsoluteColorimetric" )
                    {
                        mRI = RI_AbsoluteColorimetric;
                    }else if ( str == "RelativeColorimetric" )
                    {
                        mRI = RI_RelativeColorimetric;
                    }else if ( str == "Saturation" )
                    {
                        mRI = RI_Saturation;
                    }else{
                        mRI = RI_Perceptual;
                    }
                }
				if ( mbMask == FALSE )
				{
					objPtr = dictPtr->GetElement( "Mask" );
					if ( objPtr )
					{
						mMaskPtr = objPtr;
                        if ( mMaskPtr->GetType() == OBJ_TYPE_REFERENCE )
                        {
                            if ( mMaskPtr->GetRefPtr()->GetRefObj( OBJ_TYPE_STREAM ) )
                            {
                                mMaskImagePtr = CHE_PDF_ImageXObject::Create( mMaskPtr->GetRefPtr() );
                            }
                        }
					}
                    else
                    {
						objPtr = dictPtr->GetElement( "SMask" );
                        if ( objPtr && objPtr->GetType() == OBJ_TYPE_REFERENCE )
                        {
                            mSoftMaskImagePtr = CHE_PDF_ImageXObject::Create( objPtr->GetRefPtr() );
                        }
					}
				}
			}
		}
	}
}


CHE_PDF_ImageXObject::~CHE_PDF_ImageXObject()
{
	if ( mpBitmapCache )
	{
		mpBitmapCache->GetAllocator()->Delete( mpBitmapCache );
		mpBitmapCache = NULL;
	}
}

HE_LPBYTE CHE_PDF_ImageXObject::GetData()
{
    HE_LPBYTE lpByte = mStmAcc.GetData();
    if ( lpByte == NULL )
    {
        mStmAcc.Attach( mStmPtr );
        lpByte = mStmAcc.GetData();
    }
    return lpByte;
}

HE_ULONG CHE_PDF_ImageXObject::GetSize()
{
    HE_LPBYTE lpByte = mStmAcc.GetData();
    if ( lpByte == NULL )
    {
        mStmAcc.Attach( mStmPtr );
    }
    return mStmAcc.GetSize();
}

CHE_Bitmap * CHE_PDF_ImageXObject::GetBitmap()
{
	if ( mpBitmapCache )
	{
		return mpBitmapCache;
	}

	CHE_Bitmap * pBitmapRet = NULL;
	if( mStmPtr )
	{
		HE_BOOL bMultiFilter = FALSE;
		CHE_PDF_ObjectPtr objPtr;
		CHE_ByteString filterName( GetAllocator() );
		CHE_PDF_DictionaryPtr dictPtr = mStmPtr->GetDictPtr();
		if ( dictPtr )
		{
			objPtr = dictPtr->GetElement( "Filter", OBJ_TYPE_NAME );
			if ( objPtr )
			{
				filterName = objPtr->GetNamePtr()->GetString();
			}else{
				objPtr = dictPtr->GetElement( "Filter", OBJ_TYPE_ARRAY );
				if ( objPtr )
				{
					CHE_PDF_ArrayPtr arrayPtr = objPtr->GetArrayPtr();
					if ( arrayPtr->GetCount() > 1 )
					{
						bMultiFilter = TRUE;
					}
					objPtr = arrayPtr->GetElement( arrayPtr->GetCount()-1, OBJ_TYPE_NAME );
					if ( objPtr )
					{
						filterName = objPtr->GetNamePtr()->GetString();
					}
				}
			}
		}
		if ( filterName.GetLength() > 0 )
		{
			HE_ULONG ulSize = 0;
			HE_LPBYTE pData = NULL;
			if ( filterName == "JPXDecode" )
			{
				if ( bMultiFilter )
				{
					CHE_PDF_StreamAcc stmAcc( GetAllocator() );
					if ( stmAcc.Attach( mStmPtr, STREAM_DECODE_NOTLASTFILTER ) )
					{
						pData = stmAcc.GetData();
						ulSize = stmAcc.GetSize();
						pBitmapRet = JPXStreamToBitmap( pData, ulSize );
					}
				}else{
					ulSize = mStmPtr->GetRawSize();
					pData = GetAllocator()->NewArray<HE_BYTE>( ulSize );
					mStmPtr->GetRawData( 0, pData, ulSize );
					pBitmapRet = JPXStreamToBitmap( pData, ulSize );
					GetAllocator()->DeleteArray( pData );
				}
			}else if ( filterName == "DCTDecode" )
			{
				if ( bMultiFilter )
				{
					CHE_PDF_StreamAcc stmAcc( GetAllocator() );
					if ( stmAcc.Attach( mStmPtr, STREAM_DECODE_NOTLASTFILTER ) )
					{
						pData = stmAcc.GetData();
						ulSize = stmAcc.GetSize();
						pBitmapRet = JpegStreamToBitmap( pData, ulSize );
					}
				}else{
					ulSize = mStmPtr->GetRawSize();
					pData = GetAllocator()->NewArray<HE_BYTE>( ulSize );
					mStmPtr->GetRawData( 0, pData, ulSize );
					pBitmapRet = JpegStreamToBitmap( pData, ulSize );
					GetAllocator()->DeleteArray( pData );
				}
			}else if ( filterName == "JBIG2Decode" )
			{
				HE_LPBYTE pParam = NULL;
				HE_ULONG paramSize = 0;
				objPtr = dictPtr->GetElement( "JBIG2Globals", OBJ_TYPE_STREAM );
				if ( objPtr )
				{
					CHE_PDF_StreamPtr paramStrPtr = objPtr->GetStreamPtr();
					paramSize = paramStrPtr->GetRawSize();
					pParam = GetAllocator()->NewArray<HE_BYTE>( paramSize );
					paramStrPtr->GetRawData( 0, pParam, paramSize );
					//似乎有些不对，这个流要不要解码呢？
				}
				if ( bMultiFilter )
				{
					CHE_PDF_StreamAcc stmAcc( GetAllocator() );
					if ( stmAcc.Attach( mStmPtr, STREAM_DECODE_NOTLASTFILTER ) )
					{
						pData = stmAcc.GetData();
						ulSize = stmAcc.GetSize();
						pBitmapRet = JBig2StreamToBitmap( pData, ulSize, pParam, paramSize );
					}
				}else{
					ulSize = mStmPtr->GetRawSize();
					pData = GetAllocator()->NewArray<HE_BYTE>( ulSize );
					mStmPtr->GetRawData( 0, pData, ulSize );
					pBitmapRet = pBitmapRet = JBig2StreamToBitmap( pData, ulSize, pParam, paramSize );
					GetAllocator()->DeleteArray( pData );
				}
				if ( pParam )
				{
					GetAllocator()->DeleteArray( pParam );
				}
			}else{	
				pBitmapRet = StreamToBitmap();
			}
		}else{
			pBitmapRet = StreamToBitmap();
		}
	}

	mpBitmapCache = pBitmapRet;

	return pBitmapRet;
}

CHE_Bitmap * CHE_PDF_ImageXObject::StreamToBitmap()
{
	CHE_Bitmap * pBitmapRet = NULL;
	if ( mStmPtr )
	{
		CHE_PDF_StreamAcc stmAcc( GetAllocator() ) ;
		if ( stmAcc.Attach( mStmPtr ) )
		{
			if ( mbMask )
			{
				return GetStencilMaskingBitmap( stmAcc.GetData(), stmAcc.GetSize() );
			}

			CHE_PDF_Color	color;
			HE_LPBYTE		pData = stmAcc.GetData();
			HE_LPBYTE		pTmpByte = NULL;
			HE_BYTE			tmpByte1 = 0xFF;
			HE_BYTE			tmpByte2 = 0xFF;
			HE_ARGB			colorARGB1 = 0xFF000000;
			HE_ARGB			colorARGB2 = 0xFF000000;
			HE_ARGB *		pColors = GetAllocator()->NewArray<HE_ARGB>( mWidth );
			HE_ULONG		colorIndex = 0;
			HE_ULONG		componentCount = 1;
			HE_ULONG		stride = 0;
			HE_BITMAP_DEPTH	targetDepth = BITMAP_DEPTH_24BPP;

			if ( mColorspace == NULL )
			{
				mColorspace = CHE_PDF_ColorSpace::Create( "DeviceGray", GetAllocator() );
			}else{
				if ( mColorspace->GetColorSpaceType() == COLORSPACE_SPECIAL_INDEXED )
				{
					componentCount = 1;
				}else{
					componentCount = mColorspace->GetComponentCount();
				}
			}

			stride = (mWidth * componentCount * mBpc + 7)/8;

			if ( mMaskPtr )
			{
				targetDepth = BITMAP_DEPTH_32BPP;
			}

			switch ( mColorspace->GetColorSpaceType() )
			{
			case COLORSPACE_DEVICE_GRAY:
			case COLORSPACE_CIEBASE_CALGRAY:
				{
					pBitmapRet = GetAllocator()->New<CHE_Bitmap>( GetAllocator() );
					pBitmapRet->Create( mWidth, mHeight, targetDepth, BITMAP_DIRECTION_DOWN );
					if ( mBpc == 8 )
					{
						for ( HE_ULONG y = 0; y < mHeight; ++y )
						{
							colorIndex = 0;
							for ( HE_ULONG x = 0; x < mWidth; ++x )
							{
								pTmpByte = pData + ( ( y * mWidth + x )  );
								colorARGB1 = 0xFF000000 + *pTmpByte + (*pTmpByte << 8) + (*pTmpByte << 16); 
								*(pColors+colorIndex++) = colorARGB1;
							}
							pBitmapRet->SetPixelColor( 0, y, pColors, mWidth );
						}
					}else if ( mBpc == 4 )
					{
						for ( HE_ULONG y = 0; y < mHeight; ++y )
						{
							pTmpByte = pData + ( y * stride );
							colorIndex = 0;
							for ( HE_ULONG i = 0; i < stride; ++i )
							{
								tmpByte1 = *(pTmpByte + i);
								for ( HE_ULONG j = 0; j < 2; ++j )
								{
									tmpByte2 = ((tmpByte1>>(1-j)*4)&0x0F)*16.0f;
									colorARGB1 = 0xFF000000 + tmpByte2 + (tmpByte2 << 8) + (tmpByte2 << 16);
									*(pColors+colorIndex++) = colorARGB1;
									if ( colorIndex == mWidth )
									{
										break;
									}
								}
							}
							pBitmapRet->SetPixelColor( 0, y, pColors, mWidth );
						}
					}else if ( mBpc == 2 )
					{
						for ( HE_ULONG y = 0; y < mHeight; ++y )
						{
							pTmpByte = pData + ( y * stride );
							colorIndex = 0;
							for ( HE_ULONG i = 0; i < stride; ++i )
							{
								tmpByte1 = *(pTmpByte + i);
								for ( HE_ULONG j = 0; j < 4; ++j )
								{	
									tmpByte2 = ((tmpByte1>>((3-j)*2))&0x03)*64.0f;
									colorARGB1 = 0xFF000000 + tmpByte2 + (tmpByte2 << 8) + (tmpByte2 << 16);
									*(pColors+colorIndex++) = colorARGB1;
									if ( colorIndex == mWidth )
									{
										break;
									}
								}
							}
							pBitmapRet->SetPixelColor( 0, y, pColors, mWidth );
						}
					}else if ( mBpc == 1 )
					{
						for ( HE_ULONG y = 0; y < mHeight; ++y )
						{
							colorIndex = 0;
							pTmpByte = pData + ( y * stride );
							for ( HE_ULONG i = 0; i < stride; ++i )
							{
								tmpByte1 = *(pTmpByte + i);
								for ( HE_ULONG j = 0; j < 8; ++j )
								{
									tmpByte2 = ((tmpByte1>>(7-j))&0x01)*255.0f;
									colorARGB1 = 0xFF000000 + tmpByte2 + (tmpByte2 << 8) + (tmpByte2 << 16);
									*(pColors+colorIndex++) = colorARGB1;
									if ( colorIndex == mWidth )
									{
										break;
									}
								}
							}
							pBitmapRet->SetPixelColor( 0, y, pColors, mWidth );
						}
					}
					break;
				}
			case COLORSPACE_DEVICE_RGB:
			case COLORSPACE_CIEBASE_CALRGB:
				{
					pBitmapRet = GetAllocator()->New<CHE_Bitmap>( GetAllocator() );
					pBitmapRet->Create( mWidth, mHeight, targetDepth, BITMAP_DIRECTION_DOWN );
					for ( HE_ULONG y = 0; y < mHeight; ++y  )
					{
						colorIndex = 0;
						for ( HE_ULONG x = 0; x < mWidth; ++x )
						{
							pTmpByte = pData + ( ( y * mWidth + x ) * 3 );
							colorARGB1 = 0xFF000000 + *(pTmpByte+2) + (*(pTmpByte+1) << 8) + (*pTmpByte << 16);
							*(pColors+colorIndex++) = colorARGB1;
						}
						pBitmapRet->SetPixelColor( 0, y, pColors, mWidth );
					}
					break;
				}
			case COLORSPACE_CIEBASE_ICCBASED:
				{
					if ( componentCount == 1 )
					{
						pBitmapRet = GetAllocator()->New<CHE_Bitmap>( GetAllocator() );
						pBitmapRet->Create( mWidth, mHeight, targetDepth, BITMAP_DIRECTION_DOWN );
						for ( HE_ULONG y = 0; y < mHeight; ++y  )
						{
							colorIndex = 0;
							for ( HE_ULONG x = 0; x < mWidth; ++x )
							{
								pTmpByte = pData + ( ( y * mWidth + x ) );
								colorARGB1 = *pTmpByte + *pTmpByte << 8 + *pTmpByte << 16; 
								*(pColors+colorIndex++) = colorARGB1;
							}
							pBitmapRet->SetPixelColor( 0, y, pColors, mWidth );
						}
					}else if ( componentCount == 3 )
					{
						pBitmapRet = GetAllocator()->New<CHE_Bitmap>( GetAllocator() );
						pBitmapRet->Create( mWidth, mHeight, targetDepth, BITMAP_DIRECTION_DOWN );
						for ( HE_ULONG y = 0; y < mHeight; ++y  )
						{
							colorIndex = 0;
							for ( HE_ULONG x = 0; x < mWidth; ++x )
							{
								pTmpByte = pData + ( ( y * mWidth + x ) * 3 );
								colorARGB1 = 0xFF000000 + *(pTmpByte+2) + (*(pTmpByte+1) << 8) + (*pTmpByte << 16);
								*(pColors+colorIndex++) = colorARGB1;
							}
							pBitmapRet->SetPixelColor( 0, y, pColors, mWidth );
						}
					}else if ( componentCount == 4 )
					{
						pBitmapRet = GetAllocator()->New<CHE_Bitmap>( GetAllocator() );
						pBitmapRet->Create( mWidth, mHeight, targetDepth, BITMAP_DIRECTION_DOWN );
						for ( HE_ULONG y = 0; y < mHeight; ++y  )
						{
							colorIndex = 0;
							for ( HE_ULONG x = 0; x < mWidth; ++x )
							{
								color.Clear();
								pTmpByte = pData + ( ( y * mWidth + x ) * 4 );
								for ( HE_ULONG k = 0; k < componentCount; ++k )
								{
									color.Push( ( *pTmpByte ) / 255.0f );
									pTmpByte++;
								}
								colorARGB1 = mColorspace->GetARGBValue( color );
								*(pColors+colorIndex++) = colorARGB1;
							}
							pBitmapRet->SetPixelColor( 0, y, pColors, mWidth );
						}
					}
					break;
				}
			case COLORSPACE_DEVICE_CMYK:
			case COLORSPACE_CIEBASE_CALCMYK:
				{
					pBitmapRet = GetAllocator()->New<CHE_Bitmap>( GetAllocator() );
					pBitmapRet->Create( mWidth, mHeight, targetDepth, BITMAP_DIRECTION_DOWN );
					for ( HE_ULONG y = 0; y < mHeight; ++y  )
					{
						colorIndex = 0;
						for ( HE_ULONG x = 0; x < mWidth; ++x )
						{
							color.Clear();
							pTmpByte = pData + ( ( y * mWidth + x ) * 4 );
							for ( HE_ULONG k = 0; k < componentCount; ++k )
							{
								color.Push( ( *pTmpByte ) / 255.0f );
								pTmpByte++;
							}
							colorARGB1 = mColorspace->GetARGBValue( color );
							*(pColors+colorIndex++) = colorARGB1;
						}
						pBitmapRet->SetPixelColor( 0, y, pColors, mWidth );
					}
					break;
				}
			case COLORSPACE_SPECIAL_INDEXED:
			case COLORSPACE_SPECIAL_SEPARATION:
			case COLORSPACE_SPECIAL_DEVICEN:
				{
					pBitmapRet = GetAllocator()->New<CHE_Bitmap>( GetAllocator() );
					pBitmapRet->Create( mWidth, mHeight, targetDepth, BITMAP_DIRECTION_DOWN );
					if ( mBpc == 8 )
					{
						color.Clear();
						for ( HE_ULONG y = 0; y < mHeight; ++y )
						{
							colorIndex = 0;
							for ( HE_ULONG x = 0 ; x < mWidth; ++x )
							{
								pTmpByte = pData + ( ( y * mWidth + x ) * componentCount );
								for ( HE_ULONG k = 0; k < componentCount; ++k )
								{
									if ( mColorspace->GetColorSpaceType() == COLORSPACE_SPECIAL_INDEXED )
									{
										color.Push( *( pTmpByte ) );
									}else{
										color.Push( *( pTmpByte ) / 255.0f );
									}
									pTmpByte++;
								}
								colorARGB1 = mColorspace->GetARGBValue( color );
								color.Clear();
								*(pColors+colorIndex++) = colorARGB1;
							}
							pBitmapRet->SetPixelColor( 0, y, pColors, mWidth );
						}
					}else if ( mBpc == 4 )
					{
						color.Clear();
						for ( HE_ULONG y = 0; y < mHeight; ++y )
						{
							pTmpByte = pData + ( y * stride );
							colorIndex = 0;
							for ( HE_ULONG i = 0; i < stride; ++i )
							{
								tmpByte1 = *(pTmpByte + i);
								for ( HE_ULONG j = 0; j < 2; ++j )
								{
									if ( mColorspace->GetColorSpaceType() == COLORSPACE_SPECIAL_INDEXED )
									{
										color.Push( (tmpByte1>>(1-j)*4)&0x0F );
									}else{
										color.Push( ((tmpByte1>>(1-j)*4)&0x0F)/16.0f );
									}
									if ( color.GetComponentCount() == componentCount )
									{
										colorARGB1 = mColorspace->GetARGBValue( color );
										color.Clear();
										*(pColors+colorIndex++) = colorARGB1;
										if ( colorIndex == mWidth )
										{
											break;
										}
									}
								}
							}
							pBitmapRet->SetPixelColor( 0, y, pColors, mWidth );
						}
					}else if ( mBpc == 2 )
					{
						color.Clear();
						for ( HE_ULONG y = 0; y < mHeight; ++y )
						{
							colorIndex = 0;
							pTmpByte = pData + ( y * stride );
							for ( HE_ULONG i = 0; i < stride; ++i )
							{
								tmpByte1 = *(pTmpByte + i);
								for ( HE_ULONG j = 0; j < 4; ++j )
								{
									if ( mColorspace->GetColorSpaceType() == COLORSPACE_SPECIAL_INDEXED )
									{
										color.Push( tmpByte1>>((3-j)*2) & 0x03 );
									}else{
										color.Push( ((tmpByte1>>((3-j)*2)) & 0x03) /4.0f );
									}
									if ( color.GetComponentCount() == componentCount )
									{
										colorARGB1 = mColorspace->GetARGBValue( color );
										color.Clear();
										*(pColors+colorIndex++) = colorARGB1;
										if ( colorIndex == mWidth )
										{
											break;
										}
									}
								}
							}
							pBitmapRet->SetPixelColor( 0, y, pColors, mWidth );
						}
					}else if ( mBpc == 1 )
					{
						color.Clear();
						for ( HE_ULONG y = 0; y < mHeight; ++y )
						{
							colorIndex = 0;
							pTmpByte = pData + ( y * stride );
							for ( HE_ULONG i = 0; i < stride; ++i )
							{
								tmpByte1 = *(pTmpByte + i);
								for ( HE_ULONG j = 0; j < 8; ++j )
								{
									color.Push( ( tmpByte1 >> (7-j) ) & 0x01  );
									if ( color.GetComponentCount() == componentCount )
									{
										colorARGB1 = mColorspace->GetARGBValue( color );
										color.Clear();
										*(pColors+colorIndex++) = colorARGB1;
										if ( colorIndex == mWidth )
										{
											break;
										}
									}
								}
							}
							pBitmapRet->SetPixelColor( 0, y, pColors, mWidth );
						}
					}
				}
			default:
				break;
			}

			if ( mMaskPtr )
			{
				CHE_PDF_ObjectPtr objPtr;
				CHE_PDF_ReferencePtr refPtr;
				if ( mMaskPtr->GetType() == OBJ_TYPE_REFERENCE )
				{
					refPtr = mMaskPtr->GetRefPtr();
					objPtr = refPtr->GetRefObj( OBJ_TYPE_STREAM );
					if ( objPtr )
					{
						CHE_PDF_StreamPtr stmPtr = objPtr->GetStreamPtr();
						pBitmapRet = GetExplicitMaskingBitmap( pBitmapRet, stmPtr );
					}
				}
			}

			GetAllocator()->DeleteArray( pColors );
			stmAcc.Detach();
		}
	}
	return pBitmapRet;
}

static void fz_opj_error_callback(const char *msg, void *client_data)
{
}

static void fz_opj_warning_callback(const char *msg, void *client_data)
{
}

static void fz_opj_info_callback(const char *msg, void *client_data)
{
}

CHE_Bitmap * CHE_PDF_ImageXObject::JPXStreamToBitmap( HE_LPBYTE data, HE_ULONG size )
{
	opj_event_mgr_t     evtmgr;
	opj_dparameters_t   params;
	opj_dinfo_t *       info = NULL;
	opj_cio_t *         cio = NULL;
	opj_image_t *       jpx = NULL;
	CODEC_FORMAT        format;

	if ( data[0] == 0xFF && data[1] == 0x4F )
	{
		format = CODEC_J2K;
	}else{
		format = CODEC_JP2;
	}

	memset( &evtmgr, 0, sizeof(evtmgr) );
	evtmgr.error_handler	= fz_opj_error_callback;
	evtmgr.warning_handler	= fz_opj_warning_callback;
	evtmgr.info_handler		= fz_opj_info_callback;
	opj_set_default_decoder_parameters( &params );
	info = opj_create_decompress( format );
	opj_set_event_mgr( (opj_common_ptr)info, &evtmgr, NULL );
	opj_setup_decoder( info, &params );
	cio = opj_cio_open( (opj_common_ptr)info, data, size );
	jpx = opj_decode( info, cio );
	opj_cio_close( cio );
	opj_destroy_decompress( info );

	if ( !jpx )
	{
		return NULL;
	}

	for ( HE_ULONG i = 1; i < jpx->numcomps; ++i )
	{
		if ( jpx->comps[i].w != jpx->comps[0].w )
		{
			opj_image_destroy(jpx);
			return NULL;
		}
		if ( jpx->comps[i].h != jpx->comps[0].h )
		{
			opj_image_destroy(jpx);
			return NULL;
		}
		if (jpx->comps[i].prec != jpx->comps[0].prec )
		{
			opj_image_destroy(jpx);
			return NULL;
		}
	}

	CHE_Bitmap * pBitmapRet = NULL;
	HE_ARGB * pColors = NULL;
	HE_ULONG colorsIndex = 0;
	HE_ARGB colorARGB = 0xFF000000;
	HE_ULONG nComps, nWidth, nHeight, nAlpha, nDepth, nSgnd, k, x, y;
	int v;
	nComps = jpx->numcomps;
	nWidth = jpx->comps[0].w;
	nHeight = jpx->comps[0].h;
	nDepth = jpx->comps[0].prec;
	nSgnd = jpx->comps[0].sgnd;

	if ( jpx->color_space == CLRSPC_SRGB && nComps == 4 )
	{
		nComps = 3; nAlpha = 1;
	}
	else if ( jpx->color_space == CLRSPC_SYCC && nComps == 4 )
	{
		nComps = 3; nAlpha = 1;
	}
	else if ( nComps == 2 )
	{ 
		nComps = 1; nAlpha = 1;
	}
	else if ( nComps > 4 )
	{
		nComps = 4; nAlpha = 1;
	}else{
		nAlpha = 0;
	}

	pBitmapRet = GetAllocator()->New<CHE_Bitmap>( GetAllocator() );
	pBitmapRet->Create( nWidth, nHeight, BITMAP_DEPTH_32BPP, BITMAP_DIRECTION_DOWN );
	pColors = GetAllocator()->NewArray<HE_ARGB>( nWidth );

	for ( y = 0; y < nHeight; ++y )
	{
		colorsIndex = 0;
		for ( x = 0; x < nWidth; ++x )
		{
			colorARGB = 0x000000FF;
			for ( k = 0; k < nComps + nAlpha; ++k )
			{
				v = jpx->comps[k].data[y * nWidth + x];
				if (nSgnd)
					v = v + (1 << (nDepth - 1));
				if (nDepth > 8)
					v = v >> (nDepth - 8);

				colorARGB = (colorARGB << 8) + v;
			}
			*(pColors+colorsIndex++) = colorARGB;
		}
		pBitmapRet->SetPixelColor( 0, y, pColors, mWidth );
	}

	opj_image_destroy( jpx );

	if ( mMaskPtr )
	{
		CHE_PDF_ObjectPtr objPtr;
		CHE_PDF_ReferencePtr refPtr;
		if ( mMaskPtr->GetType() == OBJ_TYPE_REFERENCE )
		{
			refPtr = mMaskPtr->GetRefPtr();
			objPtr = refPtr->GetRefObj( OBJ_TYPE_STREAM );
			if ( objPtr )
			{
				CHE_PDF_StreamPtr stmPtr = objPtr->GetStreamPtr();
				pBitmapRet = GetExplicitMaskingBitmap( pBitmapRet, stmPtr );
			}
		}
	}

	GetAllocator()->DeleteArray( pColors );

	return pBitmapRet;
}

// #include <jpeglib.h>
#include <setjmp.h>

struct jpeg_error_mgr_jmp
{
	struct jpeg_error_mgr super;
	jmp_buf env;
	char msg[JMSG_LENGTH_MAX];
};

static void error_exit(j_common_ptr cinfo)
{
	struct jpeg_error_mgr_jmp *err = (struct jpeg_error_mgr_jmp *)cinfo->err;
	cinfo->err->format_message(cinfo, err->msg);
	longjmp(err->env, 1);
}

static void init_source(j_decompress_ptr cinfo)
{
	
}

static void term_source(j_decompress_ptr cinfo)
{
 
}

static boolean fill_input_buffer(j_decompress_ptr cinfo)
{
	static unsigned char eoi[2] = { 0xFF, JPEG_EOI };
	struct jpeg_source_mgr *src = cinfo->src;
	src->next_input_byte = eoi;
	src->bytes_in_buffer = 2;
	return 1;
}

static void skip_input_data(j_decompress_ptr cinfo, long num_bytes)
{
	struct jpeg_source_mgr *src = cinfo->src;
	if (num_bytes > 0)
	{
		src->next_input_byte += num_bytes;
		src->bytes_in_buffer -= num_bytes;
	}
}

CHE_Bitmap * CHE_PDF_ImageXObject::JpegStreamToBitmap( HE_LPBYTE data, HE_ULONG size )
{
	struct jpeg_decompress_struct	cinfo;
	struct jpeg_source_mgr			src;
	struct jpeg_error_mgr			err;

	HE_LPBYTE row[1], sp, dp;

	cinfo.err = &err;
	jpeg_std_error(cinfo.err);
	cinfo.err->error_exit = error_exit;
	cinfo.client_data = NULL;

	jpeg_create_decompress(&cinfo);

	cinfo.src = &src;
	src.init_source = init_source;
	src.fill_input_buffer = fill_input_buffer;
	src.skip_input_data = skip_input_data;
	src.resync_to_restart = jpeg_resync_to_restart;
	src.term_source = term_source;
	src.next_input_byte = data;
	src.bytes_in_buffer = size;

	jpeg_read_header( &cinfo, 1 );

	cinfo.dct_method = JDCT_FASTEST;
	cinfo.do_fancy_upsampling = FALSE;

	jpeg_start_decompress(&cinfo);

	CHE_Bitmap *	pBitmapRet = NULL;
	CHE_PDF_Color	color;
	HE_ARGB *		pColors = NULL;
	HE_ULONG		colorsIndex = 0;
	HE_ARGB			colorARGB = 0xFF000000;
	HE_ULONG		componentCount = 1;
	HE_ULONG		stride = (mWidth * componentCount * mBpc + 7)/8;
	HE_BYTE			tmpByte = 0;
	HE_BITMAP_DEPTH	targetDepth = BITMAP_DEPTH_32BPP;

	if ( mbMask )
	{
//		CHE_PDF_GState * pGStae = GetGState();
// 		CHE_PDF_Color color;
// 		CHE_PDF_ColorSpace * pColorspace = CHE_PDF_ColorSpace::Create( "DeviceGray", GetAllocator() );
		HE_ARGB colorARGB = 0xFF000000;
		HE_ARGB colorTmpARGB = 0xFF00000;

		pBitmapRet = GetAllocator()->New<CHE_Bitmap>( GetAllocator() );
		pBitmapRet->Create( mWidth, mHeight, BITMAP_DEPTH_32BPP, BITMAP_DIRECTION_DOWN );
// 		if ( pGStae )
// 		{
// 			pGStae->GetFillColor( color );
// 			pGStae->GetFillColorSpace( *pColorspace );
// 			if ( pColorspace )
// 			{
// 				colorARGB = pColorspace->GetARGBValue( color );
// 			}
// 		}

		row[0] = GetAllocator()->NewArray<HE_BYTE>( cinfo.output_components * cinfo.output_width );

		if ( mBpc == 8 )
		{
			for ( HE_ULONG y = 0; y < mHeight; ++y )
			{
				jpeg_read_scanlines( &cinfo, row,  1 );
				sp = row[0];
				colorsIndex = 0;
				for ( HE_ULONG x = 0; x < mWidth; ++x )
				{
					colorTmpARGB = *sp++;
					colorTmpARGB = colorTmpARGB<<24;
					colorTmpARGB = colorARGB & 0x00FFFFFF + colorTmpARGB;
					*(pColors+colorsIndex++) = colorTmpARGB;
				}
				pBitmapRet->SetPixelColor( 0, y, pColors, mWidth );
			}
		}else if ( mBpc == 4 )
		{
			for ( HE_ULONG y = 0, x = 0; y < mHeight; ++y )
			{
				jpeg_read_scanlines( &cinfo, row,  1 );
				sp = row[0];
				colorsIndex = 0;
				x = 0;
				for ( HE_ULONG i = 0; i < stride; ++i )
				{
					tmpByte = *(sp + i);
					for ( HE_ULONG j = 0; j < 2; ++j )
					{
						colorTmpARGB = ((tmpByte>>(1-j)*4)&0x0F)*255.0f;
						colorTmpARGB = colorTmpARGB<<24;
						colorTmpARGB = colorARGB & 0x00FFFFFF + colorTmpARGB;
						*(pColors+colorsIndex++) = colorTmpARGB;
						if ( ++x == mWidth )
						{
							x = 0;
							break;
						}
					}
				}
				pBitmapRet->SetPixelColor( 0, y, pColors, mWidth );
			}
		}else if ( mBpc == 2 )
		{
			for ( HE_ULONG y = 0, x = 0; y < mHeight; ++y )
			{
				jpeg_read_scanlines( &cinfo, row,  1 );
				sp = row[0];
				colorsIndex = 0;
				x = 0;
				for ( HE_ULONG i = 0; i < stride; ++i )
				{
					tmpByte = *(sp + i);
					for ( HE_ULONG j = 0; j < 4; ++j )
					{	
						colorTmpARGB = ((tmpByte>>((3-j)*2))&0x03)*255.0f;
						colorTmpARGB = colorTmpARGB<<24;
						colorTmpARGB = colorARGB & 0x00FFFFFF + colorTmpARGB;
						*(pColors+colorsIndex++) = colorTmpARGB;
						if ( ++x == mWidth )
						{
							x = 0;
							break;
						}
					}
				}
				pBitmapRet->SetPixelColor( 0, y, pColors, mWidth );
			}
		}else if ( mBpc == 1 )
		{
			for ( HE_ULONG y = 0, x = 0; y < mHeight; ++y )
			{
				jpeg_read_scanlines( &cinfo, row,  1 );
				sp = row[0];
				colorsIndex = 0;
				x = 0;
				for ( HE_ULONG i = 0; i < stride; ++i )
				{
					tmpByte = *(sp + i);
					for ( HE_ULONG j = 0; j < 8; ++j )
					{
						colorTmpARGB = ((tmpByte>>(7-j))&0x01)*255.0f;
						colorTmpARGB = colorTmpARGB<<24;
						colorTmpARGB = colorARGB & 0x00FFFFFF + colorTmpARGB;
						*(pColors+colorsIndex++) = colorTmpARGB;
						if ( ++x == mWidth )
						{
							x = 0;
							break;
						}
					}
				}
				pBitmapRet->SetPixelColor( 0, y, pColors, mWidth );
			}
		}
		GetAllocator()->DeleteArray( pColors );
		GetAllocator()->DeleteArray( row[0] );
		return pBitmapRet;
	}

	if ( mMaskPtr )
	{
		targetDepth = BITMAP_DEPTH_32BPP;
	}

	if ( mColorspace == NULL )
	{
		if ( cinfo.output_components == 4 )
		{
			mColorspace = CHE_PDF_ColorSpace::Create( "DeviceCMYK", GetAllocator() );
		}else if ( cinfo.output_components == 3 )
		{
			mColorspace = CHE_PDF_ColorSpace::Create( "DeviceRGB", GetAllocator() );
		}else if ( cinfo.output_components == 1 )
		{
			mColorspace = CHE_PDF_ColorSpace::Create( "DeviceGray", GetAllocator() );
		}
	}

	switch ( mColorspace->GetColorSpaceType() )
	{
	case COLORSPACE_DEVICE_GRAY:
	case COLORSPACE_CIEBASE_CALGRAY:
		{
			pBitmapRet = GetAllocator()->New<CHE_Bitmap>( GetAllocator() );
			pBitmapRet->Create( cinfo.output_width, cinfo.output_height, targetDepth, BITMAP_DIRECTION_DOWN );
			row[0] = GetAllocator()->NewArray<HE_BYTE>( cinfo.output_components * cinfo.output_width );
			pColors = GetAllocator()->NewArray<HE_ARGB>( cinfo.output_width );
			colorsIndex = 0;
			colorARGB = 0xFF000000;
			for ( HE_ULONG y = 0; y < cinfo.output_height; ++y  )
			{
				jpeg_read_scanlines( &cinfo, row,  1 );
				sp = row[0];
				colorsIndex = 0;
				for ( HE_ULONG x = 0; x < cinfo.output_width; x++)
				{
					colorARGB = 0xFF000000 + *sp + (*sp << 8) + (*sp << 16);
					sp++;
					*(pColors+colorsIndex++) = colorARGB;
				}
				pBitmapRet->SetPixelColor( 0, y, pColors, cinfo.output_width );
			}
			GetAllocator()->DeleteArray( row[0] );
			GetAllocator()->DeleteArray( pColors );
			break;
		}
	case COLORSPACE_DEVICE_RGB:
	case COLORSPACE_CIEBASE_CALRGB:
		{
			pBitmapRet = GetAllocator()->New<CHE_Bitmap>( GetAllocator() );
			pBitmapRet->Create( cinfo.output_width, cinfo.output_height, targetDepth, BITMAP_DIRECTION_DOWN );
			row[0] = GetAllocator()->NewArray<HE_BYTE>( cinfo.output_components * cinfo.output_width );
			pColors = GetAllocator()->NewArray<HE_ARGB>( cinfo.output_width );
			for ( HE_ULONG y = 0; y < cinfo.output_height; ++y  )
			{
				jpeg_read_scanlines( &cinfo, row, 1 );
				sp = row[0];
				colorsIndex = 0;
				for ( HE_ULONG x = 0; x < cinfo.output_width; x++)
				{
					colorARGB = 0xFF000000 + *(sp+2) + (*(sp+1) << 8) + (*sp << 16);
					sp += 3;
					*(pColors+colorsIndex++) = colorARGB;
				}
				pBitmapRet->SetPixelColor( 0, y, pColors, cinfo.output_width );
			}
			GetAllocator()->DeleteArray( row[0] );
			GetAllocator()->DeleteArray( pColors );
			break;
		}
	case COLORSPACE_CIEBASE_ICCBASED:
		{
			componentCount = mColorspace->GetComponentCount();
			if ( componentCount == 1 )
			{
				pBitmapRet = GetAllocator()->New<CHE_Bitmap>( GetAllocator() );
				pBitmapRet->Create( cinfo.output_width, cinfo.output_height, targetDepth, BITMAP_DIRECTION_DOWN );
				row[0] = GetAllocator()->NewArray<HE_BYTE>( cinfo.output_components * cinfo.output_width );
				pColors = GetAllocator()->NewArray<HE_ARGB>( cinfo.output_width );
				colorsIndex = 0;
				colorARGB = 0xFF000000;
				for ( HE_ULONG y = 0; y < cinfo.output_height; ++y )
				{
					jpeg_read_scanlines( &cinfo, row,  1 );
					sp = row[0];
					colorsIndex = 0;
					for ( HE_ULONG x = 0; x < cinfo.output_width; ++x )
					{
						colorARGB = 0xFF000000 + *sp + (*sp << 8) + (*sp << 16);
						sp++;
						*(pColors+colorsIndex++) = colorARGB;
					}
					pBitmapRet->SetPixelColor( 0, y, pColors, cinfo.output_width );
				}
				GetAllocator()->DeleteArray( row[0] );
				GetAllocator()->DeleteArray( pColors );
			}else if ( componentCount == 3 )
			{
				pBitmapRet = GetAllocator()->New<CHE_Bitmap>( GetAllocator() );
				pBitmapRet->Create( cinfo.output_width, cinfo.output_height, targetDepth, BITMAP_DIRECTION_DOWN );
				row[0] = GetAllocator()->NewArray<HE_BYTE>( cinfo.output_components * cinfo.output_width );
				pColors = GetAllocator()->NewArray<HE_ARGB>( cinfo.output_width );
				for ( HE_ULONG y = 0; y < cinfo.output_height; ++y  )
				{
					jpeg_read_scanlines( &cinfo, row, 1 );
					sp = row[0];
					colorsIndex = 0;
					for ( HE_ULONG x = 0; x < cinfo.output_width; x++)
					{
						colorARGB = 0xFF000000 + *(sp+2) + (*(sp+1) << 8) + (*sp << 16);
						sp += 3;
						*(pColors+colorsIndex++) = colorARGB;
					}
					pBitmapRet->SetPixelColor( 0, y, pColors, cinfo.output_width );
				}
				GetAllocator()->DeleteArray( row[0] );
				GetAllocator()->DeleteArray( pColors );
			}else if ( componentCount == 4 )
			{
				pBitmapRet = GetAllocator()->New<CHE_Bitmap>( GetAllocator() );
				pBitmapRet->Create( cinfo.output_width, cinfo.output_height, targetDepth, BITMAP_DIRECTION_DOWN );
				row[0] = GetAllocator()->NewArray<HE_BYTE>( cinfo.output_components * cinfo.output_width );
				pColors = GetAllocator()->NewArray<HE_ARGB>( cinfo.output_width );
				colorsIndex = 0;
				colorARGB = 0xFF000000;
				componentCount = mColorspace->GetComponentCount();
				for ( HE_ULONG y = 0; y < cinfo.output_height; ++y  )
				{
					jpeg_read_scanlines( &cinfo, row, 1 );
					sp = row[0];
					colorsIndex = 0;
					for ( HE_ULONG x = 0; x < cinfo.output_width; ++x )
					{
						color.Clear();
						for ( HE_ULONG k = 0; k < componentCount; ++k )
						{
							color.Push( ( *sp ) / 255.0f );
							sp++;
						}
						colorARGB = mColorspace->GetARGBValue( color );
						*(pColors+colorsIndex++) = colorARGB;
					}
					pBitmapRet->SetPixelColor( 0, y, pColors, cinfo.output_width );
				}
				GetAllocator()->DeleteArray( row[0] );
				GetAllocator()->DeleteArray( pColors );
			}
			break;
		}
	case COLORSPACE_DEVICE_CMYK:
	case COLORSPACE_CIEBASE_CALCMYK:
	case COLORSPACE_SPECIAL_INDEXED:
	case COLORSPACE_SPECIAL_SEPARATION:
	case COLORSPACE_SPECIAL_DEVICEN:
		{
			pBitmapRet = GetAllocator()->New<CHE_Bitmap>( GetAllocator() );
			pBitmapRet->Create( cinfo.output_width, cinfo.output_height, targetDepth, BITMAP_DIRECTION_DOWN );
			row[0] = GetAllocator()->NewArray<HE_BYTE>( cinfo.output_components * cinfo.output_width );
			pColors = GetAllocator()->NewArray<HE_ARGB>( cinfo.output_width );
			colorsIndex = 0;
			colorARGB = 0xFF000000;
			componentCount = mColorspace->GetComponentCount();
			for ( HE_ULONG y = 0; y < cinfo.output_height; ++y  )
			{
				jpeg_read_scanlines( &cinfo, row, 1 );
				sp = row[0];
				colorsIndex = 0;
				for ( HE_ULONG x = 0; x < cinfo.output_width; ++x )
				{
					color.Clear();
					for ( HE_ULONG k = 0; k < componentCount; ++k )
					{
						color.Push( ( *sp ) / 255.0f );
						sp++;
					}
					colorARGB = mColorspace->GetARGBValue( color );
					*(pColors+colorsIndex++) = colorARGB;
				}
				pBitmapRet->SetPixelColor( 0, y, pColors, cinfo.output_width );
			}
			GetAllocator()->DeleteArray( row[0] );
			GetAllocator()->DeleteArray( pColors );
			break;
		}
	default:
		break;
	}

	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);

	if ( mMaskPtr )
	{
		CHE_PDF_ObjectPtr objPtr;
		CHE_PDF_ReferencePtr refPtr;
		if ( mMaskPtr->GetType() == OBJ_TYPE_REFERENCE )
		{
			refPtr = mMaskPtr->GetRefPtr();
			objPtr = refPtr->GetRefObj( OBJ_TYPE_STREAM );
			if ( objPtr )
			{
				CHE_PDF_StreamPtr stmPtr = objPtr->GetStreamPtr();
				pBitmapRet = GetExplicitMaskingBitmap( pBitmapRet, stmPtr );
			}
		}
	}

	return pBitmapRet;
}

CHE_Bitmap * CHE_PDF_ImageXObject::JBig2StreamToBitmap( HE_LPBYTE data, HE_ULONG size, HE_LPBYTE globals, HE_ULONG globalsSize )
{
	Jbig2Ctx *ctx = jbig2_ctx_new( NULL, JBIG2_OPTIONS_EMBEDDED, NULL, NULL, NULL );
	Jbig2GlobalCtx *gctx = NULL;
	Jbig2Image *page = NULL;

	if ( globals )
	{
		jbig2_data_in( ctx, globals, globalsSize );
		gctx = jbig2_make_global_ctx( ctx );
		ctx = jbig2_ctx_new( NULL, JBIG2_OPTIONS_EMBEDDED, gctx, NULL, NULL );
	}

	if ( !page )
	{
		jbig2_data_in( ctx, data, size );
		jbig2_complete_page( ctx );
		page = jbig2_page_out( ctx );
	}

	HE_LPBYTE s = page->data;
	HE_INT32 w = page->height * page->stride;
	HE_INT32 index = 0;
	for ( HE_ULONG i = 0; i < w; ++i )
	{
		s[i] = s[i] ^ 0xff;
	}

	if ( mbMask )
	{
		return GetStencilMaskingBitmap( s, w );
	}

	CHE_Bitmap * pBitmap = GetDefaultAllocator()->New<CHE_Bitmap>( GetDefaultAllocator() );
	pBitmap->Create( page->width, page->height, BITMAP_DEPTH_1BPP, BITMAP_DIRECTION_DOWN, w, s );

	if ( page )
	{
		jbig2_release_page( ctx, page );
	}
	if ( gctx )
	{
		jbig2_global_ctx_free( gctx );
	}
	if ( ctx )
	{
		jbig2_ctx_free( ctx );
	}
	return pBitmap;
}


CHE_Bitmap * CHE_PDF_ImageXObject::GetStencilMaskingBitmap( HE_LPBYTE pData, HE_ULONG size )
{
	if ( pData == NULL )
	{
		return NULL;
	}

	HE_ULONG stride = (mWidth * mBpc + 7)/8;

	if ( size < mHeight * stride  )
	{
		return NULL;
	}

// 	CHE_PDF_GState * pGStae = GetGState();
// 	CHE_PDF_Color color;
// 	CHE_PDF_ColorSpace colorspace( COLORSPACE_DEVICE_GRAY, GetAllocator() );
	HE_ARGB	colorARGB1 = 0xFF000000;
	HE_ARGB	colorARGB2 = 0xFF000000;
	CHE_Bitmap * pBitmapRet = GetAllocator()->New<CHE_Bitmap>( GetAllocator() );

// 	if ( pGStae )
// 	{
// 		pGStae->GetFillColor( color );
// 		pGStae->GetFillColorSpace( colorspace );
// 		colorARGB1 = colorspace.GetARGBValue( color );
// 	}

	pBitmapRet->Create( mWidth, mHeight, BITMAP_DEPTH_32BPP, BITMAP_DIRECTION_DOWN );

	HE_ULONG colorIndex = 0;
	HE_ULONG byteIndex = 0;
	HE_ULONG bitIndex = 0;
	HE_LPBYTE pTmpByte = NULL;
	HE_BYTE tmpByte = 0xFF;
	HE_ARGB * pColors = GetAllocator()->NewArray<HE_ARGB>( mWidth );

	if ( mBpc == 8 )
	{
		for ( HE_ULONG y = 0; y < mHeight; ++y )
		{
			pTmpByte = pData + ( y * stride );
			colorIndex = 0;
			for ( byteIndex = 0; byteIndex < mWidth; ++byteIndex )
			{
				tmpByte = *(pTmpByte + byteIndex);
				//if ( mMaskDecode == 0 )
				//{
				//	colorARGB2 = 255 - tmpByte;
				//}else{
					colorARGB2 = tmpByte;
				//}
				colorARGB2 = colorARGB2 << 24;
				colorARGB2 = colorARGB1 & 0x00FFFFFF + colorARGB2;
				*(pColors+colorIndex++) = colorARGB2;
				if ( colorIndex == mWidth )
				{
					break;
				}
			}
			pBitmapRet->SetPixelColor( 0, y, pColors, mWidth );
		}
	}else if ( mBpc == 1 )
	{
		for ( HE_ULONG y = 0; y < mHeight; ++y )
		{
			pTmpByte = pData + ( y * stride );
			colorIndex = 0;
			for ( byteIndex = 0; byteIndex < stride; ++byteIndex )
			{
				tmpByte = *(pTmpByte + byteIndex);
				for ( bitIndex = 0; bitIndex < 8; ++bitIndex )
				{
					//if ( mMaskDecode == 0 )
					//{
					//	colorARGB2 = 255 - ((tmpByte>>(7-bitIndex))&0x01)*255.0f;
					//}else{
						colorARGB2 = ((tmpByte>>(7-bitIndex))&0x01)*255.0f;
					//}
					colorARGB2 = colorARGB2 << 24;
					colorARGB2 = colorARGB1 & 0x00FFFFFF + colorARGB2;
					*(pColors+colorIndex++) = colorARGB2;
					if ( colorIndex == mWidth )
					{
						break;
					}
				}
			}
			pBitmapRet->SetPixelColor( 0, y, pColors, mWidth );
		}
	}

	GetAllocator()->DeleteArray( pColors );
	return pBitmapRet;
}

CHE_Bitmap * CHE_PDF_ImageXObject::GetExplicitMaskingBitmap( CHE_Bitmap * pBitmapOrig, CHE_PDF_StreamPtr & stmPtr )
{
	if ( pBitmapOrig == NULL )
	{
		return NULL;
	}
	if ( pBitmapOrig->Depth() != BITMAP_DEPTH_32BPP || !stmPtr )
	{
		return pBitmapOrig;
	}

	HE_ULONG maskWidth = 0;
	HE_ULONG maskHeight = 0;
	HE_ULONG maskBPC = 0;
	CHE_Bitmap * pBitmapRet = NULL;

	CHE_PDF_ObjectPtr objPtr;
	CHE_PDF_DictionaryPtr dictPtr = stmPtr->GetDictPtr();
	if ( dictPtr )
	{
		objPtr = dictPtr->GetElement( "Width", OBJ_TYPE_NUMBER );
		if ( objPtr )
		{
			maskWidth = objPtr->GetNumberPtr()->GetInteger();
		}
		objPtr = dictPtr->GetElement( "Height", OBJ_TYPE_NUMBER );
		if ( objPtr )
		{
			maskHeight = objPtr->GetNumberPtr()->GetInteger();
		}
		objPtr = dictPtr->GetElement( "BitsPerComponent", OBJ_TYPE_NUMBER );
		if ( objPtr )
		{
			maskBPC = objPtr->GetNumberPtr()->GetInteger();
		}
		// 		objPtr = dictPtr->GetElement( "ImageMask", OBJ_TYPE_BOOLEAN );
		// 		if ( objPtr )
		// 		{
		// 			if ( objPtr->GetBooleanPtr()->GetValue() == FALSE )
		// 			{
		// 				//todo
		// 			}
		// 		}
		if ( pBitmapOrig->Width() != maskWidth || pBitmapOrig->Height() != maskHeight )
		{
			pBitmapRet = pBitmapOrig->StretchTo( maskWidth, maskHeight, 1, NULL );
			pBitmapOrig->GetAllocator()->Delete( pBitmapOrig );
		}else{
			pBitmapRet = pBitmapOrig;
		}

		CHE_PDF_StreamAcc maskStmAcc( GetAllocator() ) ;
		if ( maskStmAcc.Attach( stmPtr ) )
		{
			HE_LPBYTE pMaskData = maskStmAcc.GetData();
			HE_LPBYTE pTmpByte = NULL;
			HE_BYTE	tmpByte = 0;
			HE_BYTE tmpValue = 0;
			HE_ARGB colorARGB = 0xFF000000;
			HE_ARGB colorTmpARGB = 0xFF000000;
			CHE_PDF_Color color;
			HE_ULONG colorsIndex = 0;
			HE_ULONG stride = (maskWidth * maskBPC + 7)/8;
			HE_ARGB * pColors = GetAllocator()->NewArray<HE_ARGB>( maskWidth );

			if ( maskBPC == 8 )
			{
				for ( HE_ULONG y = 0; y < maskHeight; ++y )
				{
					colorsIndex = 0;
					for ( HE_ULONG x = 0; x < maskWidth; ++x )
					{
						pTmpByte = pMaskData+(y*maskWidth+x);
						colorTmpARGB = *pTmpByte;
						colorTmpARGB = colorTmpARGB<<24;
						pBitmapRet->GetPixelColor( x, y, colorARGB );
						colorTmpARGB = colorARGB & 0x00FFFFFF + colorTmpARGB;
						*(pColors+colorsIndex++) = colorTmpARGB;
					}
					pBitmapRet->SetPixelColor( 0, y, pColors, maskWidth );
				}
			}else if ( maskBPC == 4 )
			{
				for ( HE_ULONG y = 0, x = 0; y < maskHeight; ++y )
				{
					pTmpByte = pMaskData + ( y * stride );
					colorsIndex = 0;
					x = 0;
					for ( HE_ULONG i = 0; i < stride; ++i )
					{
						tmpByte = *(pTmpByte + i);
						for ( HE_ULONG j = 0; j < 2; ++j )
						{
							tmpValue = ((tmpByte>>(1-j)*4)&0x0F)*255.0f;
							colorTmpARGB = tmpValue;
							colorTmpARGB = colorTmpARGB<<24;
							colorTmpARGB = colorARGB & 0x00FFFFFF + colorTmpARGB;
							*(pColors+colorsIndex++) = colorTmpARGB;
							if ( ++x == maskWidth )
							{
								x = 0;
								break;
							}
						}
					}
					pBitmapRet->SetPixelColor( 0, y, pColors, maskWidth );
				}
			}else if ( maskBPC == 2 )
			{
				for ( HE_ULONG y = 0, x = 0; y < maskHeight; ++y )
				{
					colorsIndex = 0;
					pTmpByte = pMaskData + ( y * stride );
					x = 0;
					for ( HE_ULONG i = 0; i < stride; ++i )
					{
						tmpByte = *(pTmpByte + i);
						for ( HE_ULONG j = 0; j < 4; ++j )
						{	
							tmpValue = ((tmpByte>>((3-j)*2))&0x03)*255.0f;
							colorTmpARGB = tmpValue;
							colorTmpARGB = colorTmpARGB<<24;
							pBitmapRet->GetPixelColor( x, y, colorARGB );
							colorTmpARGB = colorARGB & 0x00FFFFFF + colorTmpARGB;
							*(pColors+colorsIndex++) = colorTmpARGB;
							if ( ++x == maskWidth )
							{
								x = 0;
								break;
							}
						}
					}
					pBitmapRet->SetPixelColor( 0, y, pColors, maskWidth );
				}
			}else if ( maskBPC == 1 )
			{
				for ( HE_ULONG y = 0, x = 0; y < maskHeight; ++y )
				{
					colorsIndex = 0;
					pTmpByte = pMaskData + ( y * stride );
					x = 0;
					for ( HE_ULONG i = 0; i < stride; ++i )
					{
						tmpByte = *(pTmpByte + i);
						for ( HE_ULONG j = 0; j < 8; ++j )
						{
							tmpValue = ((tmpByte>>(7-j))&0x01)*255.0f;
							colorTmpARGB = colorTmpARGB<<24;
							pBitmapRet->GetPixelColor( x, y, colorARGB );
							colorTmpARGB = colorARGB & 0x00FFFFFF + colorTmpARGB;
							*(pColors+colorsIndex++) = colorTmpARGB;
							if ( ++x == maskWidth )
							{
								x = 0;
								break;
							}
						}
					}
					pBitmapRet->SetPixelColor( 0, y, pColors, maskWidth );
				}
			}
		}
	}

	return pBitmapRet;
}

CHE_PDF_FormXObjectPtr CHE_PDF_FormXObject::Create( const CHE_PDF_ReferencePtr & refPtr, CHE_PDF_ComponentMgr * pComponentMgr, CHE_Allocator * pAllocator/*=  NULL*/ )
{
	CHE_PDF_FormXObjectPtr ptr;
	if ( !refPtr || ! refPtr->GetRefObj( OBJ_TYPE_STREAM ) || pComponentMgr == NULL )
	{
		return ptr;
	}
	if ( pAllocator == NULL )
	{
		pAllocator = GetDefaultAllocator();
	}
	CHE_PDF_FormXObject * pForm = pAllocator->New<CHE_PDF_FormXObject>( refPtr, pAllocator );
	if ( CHE_PDF_ContentListBuilder::ParseContentStream( refPtr, pForm->GetList(), pComponentMgr, pAllocator ) )
	{
		ptr.Reset( pForm );
	}else{
		pAllocator->Delete( pForm );
	}
	return ptr;
}

CHE_PDF_FormXObjectPtr CHE_PDF_FormXObject::Convert( const CHE_PDF_ComponentPtr & componentPtr )
{
	CHE_PDF_FormXObjectPtr ptr;
	if ( componentPtr && componentPtr->GetType() == COMPONENT_TYPE_FormXObject )
	{
		ptr.Reset( componentPtr.GetPointer() );
	}
	return ptr;
}

CHE_PDF_FormXObject::CHE_PDF_FormXObject( const CHE_PDF_ReferencePtr & refPtr, CHE_Allocator * pAllocator/*= NULL*/ )
	: CHE_PDF_Component( COMPONENT_TYPE_FormXObject, refPtr, pAllocator ) {}