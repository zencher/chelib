#include "../../include/pdf/che_pdf_imageraster.h"

#include "../../extlib/libjpeg/jpeglib.h"
#include "../../extlib/openjpeg/openjpeg.h"
#include "../../extlib/jbig2dec/jbig2.h"

CHE_PDF_ImageRaster::CHE_PDF_ImageRaster( CHE_Allocator * pAllocator/*= NULL*/ )
	: CHE_Object( pAllocator ) {}

CHE_Bitmap * CHE_PDF_ImageRaster::GetBitmap( const CHE_PDF_ImageXObjectPtr & imgPtr )
{
	return GetBitmapSimpleImp( imgPtr );
}

CHE_Bitmap * CHE_PDF_ImageRaster::GetBitmapImp( const CHE_PDF_ImageXObjectPtr & imgPtr )
{
	//todo
	return NULL;
}

CHE_Bitmap * CHE_PDF_ImageRaster::GetBitmapSimpleImp( const CHE_PDF_ImageXObjectPtr & imgPtr )
{
	CHE_Bitmap * pBitmapRet = NULL;
	if ( imgPtr )
	{
		if ( imgPtr->IsMask() )
		{
			return GetBitmapAsMask( imgPtr );
		}

		if ( imgPtr->GetMaskImagePtr() )
		{
			return GetBitmapWithImageMask( imgPtr, imgPtr->GetMaskImagePtr() );
		}
		if ( imgPtr->GetSoftMaskImagePtr() )
		{
			return GetBitmapWithImageMask( imgPtr, imgPtr->GetSoftMaskImagePtr() );
		}

		CHE_PDF_Color color;
		CHE_PDF_ColorSpacePtr csPtr = imgPtr->GetColorspace();
		HE_ULONG width = imgPtr->GetWidth();
		HE_ULONG height = imgPtr->GetHeight();
		HE_ULONG bpc = imgPtr->GetBPC();
		HE_LPBYTE pByte = imgPtr->GetData();
		HE_ULONG byteSize = imgPtr->GetSize();

		HE_ULONG cscc = 0;
		PDF_COLORSPACE_TYPE type = COLORSPACE_DEVICE_GRAY;
		if ( !csPtr )
		{
			if ( imgPtr->IsMask() )
			{
			}else{
				cscc = byteSize / (width * height * (bpc / 8));
				if ( cscc == 4 )
				{
					csPtr = CHE_PDF_ColorSpace::CreateDeviceCMYK();
				}else if ( cscc == 3 )
				{
					csPtr = CHE_PDF_ColorSpace::CreateDeviceRGB();
				}else if ( cscc == 1 )
				{
					csPtr = CHE_PDF_ColorSpace::CreateDeviceGray();
				}else{
					//error
					return pBitmapRet;
				}
			}
		}

		type = csPtr->GetColorSpaceType();
		cscc = csPtr->GetComponentCount();

		if ( bpc == 0 )
		{
			bpc = byteSize * 8 / (width * height * cscc);
		}

		pBitmapRet = GetAllocator()->New<CHE_Bitmap>( GetAllocator() );
		pBitmapRet->Create( width, height, BITMAP_DEPTH_32BPP, BITMAP_DIRECTION_DOWN );
		
		HE_ULONG stride = (width * cscc * bpc + 7)/8;
		HE_ULONG x = 0;
		HE_ULONG y = 0;
		HE_ULONG lineOffset = 0;
		HE_ULONG byteOffset = 0;
		HE_ULONG bitOffset = 0;
		HE_BYTE byte = 0;
		HE_BYTE tmpByte = 0;
		HE_BYTE crange = ((1<<bpc) - 1);
		HE_FLOAT cf = 0.0f;
		HE_ARGB argb = 0;

		for ( y = 0; y < height; ++y )
		{
			x = 0;
			lineOffset = y * stride;
			for ( byteOffset = 0; byteOffset < stride; ++byteOffset )
			{
				byte = *(pByte + lineOffset + byteOffset);
				for ( bitOffset = 0; bitOffset < (8/bpc); ++bitOffset )
				{
					tmpByte = (byte>>(8-bpc*(bitOffset+1))) & crange;
					if ( type == COLORSPACE_SPECIAL_INDEXED )
					{
						cf = tmpByte;
					} else{
						cf = tmpByte * 1.0 / crange;
					}
					color.Push( cf );
					if ( color.GetComponentCount() == cscc )
					{
						argb = csPtr->GetARGBValue( color );
						color.Clear();
						pBitmapRet->SetPixelColor( x++, y, argb );
					}
					if ( x == width )
					{
						break;
					}
				}
			}
		}
	}
	return pBitmapRet;
}

CHE_Bitmap * CHE_PDF_ImageRaster::GetBitmapNoMask( const CHE_PDF_ImageXObjectPtr & imgPtr )
{
	CHE_Bitmap * pBitmapRet = NULL;
	if ( imgPtr )
	{
		CHE_PDF_Color color;
		CHE_PDF_ColorSpacePtr csPtr = imgPtr->GetColorspace();
		HE_ULONG width = imgPtr->GetWidth();
		HE_ULONG height = imgPtr->GetHeight();
		HE_ULONG bpc = imgPtr->GetBPC();
		HE_LPBYTE pByte = imgPtr->GetData();
		HE_ULONG byteSize = imgPtr->GetSize();

		HE_ULONG cscc = 0;
		PDF_COLORSPACE_TYPE type = COLORSPACE_DEVICE_GRAY;
		if ( !csPtr )
		{
			cscc = byteSize / (width * height * bpc);
			if ( cscc == 4 )
			{
				csPtr = CHE_PDF_ColorSpace::CreateDeviceCMYK();
			}else if ( cscc == 3 )
			{
				csPtr = CHE_PDF_ColorSpace::CreateDeviceRGB();
			}else if ( cscc == 1 )
			{
				csPtr = CHE_PDF_ColorSpace::CreateDeviceGray();
			}else{
				//error
				return pBitmapRet;
			}
		}

		type = csPtr->GetColorSpaceType();
		cscc = csPtr->GetComponentCount();

		if ( bpc == 0 )
		{
			bpc = byteSize * 8 / (width * height * cscc);
		}

		pBitmapRet = GetAllocator()->New<CHE_Bitmap>( GetAllocator() );
		pBitmapRet->Create( width, height, BITMAP_DEPTH_32BPP, BITMAP_DIRECTION_DOWN );

		HE_ULONG stride = (width * cscc * bpc + 7)/8;
		HE_ULONG x = 0;
		HE_ULONG y = 0;
		HE_ULONG lineOffset = 0;
		HE_ULONG byteOffset = 0;
		HE_ULONG bitOffset = 0;
		HE_BYTE byte = 0;
		HE_BYTE tmpByte = 0;
		HE_BYTE crange = ((1<<bpc) - 1);
		HE_FLOAT cf = 0.0f;
		HE_ARGB argb = 0;

		for ( y = 0; y < height; ++y )
		{
			x = 0;
			lineOffset = y * stride;
			for ( byteOffset = 0; byteOffset < stride; ++byteOffset )
			{
				byte = *(pByte + lineOffset + byteOffset);
				for ( bitOffset = 0; bitOffset < (8/bpc); ++bitOffset )
				{
					tmpByte = (byte>>(8-bpc*(bitOffset+1))) & crange;
					if ( type == COLORSPACE_SPECIAL_INDEXED )
					{
						cf = tmpByte;
					} else{
						cf = tmpByte * 1.0 / crange;
					}
					color.Push( cf );
					if ( color.GetComponentCount() == cscc )
					{
						argb = csPtr->GetARGBValue( color );
						color.Clear();
						pBitmapRet->SetPixelColor( x++, y, argb );
					}
					if ( x == width )
					{
						break;
					}
				}
			}
		}
	}
	return pBitmapRet;
}

CHE_Bitmap * CHE_PDF_ImageRaster::GetBitmapAsMask( const CHE_PDF_ImageXObjectPtr & imgPtr )
{
	CHE_Bitmap * pBitmapRet = NULL;
	if ( imgPtr )
	{
		if ( ! imgPtr->IsMask() )
		{
			return pBitmapRet;
		}

		CHE_PDF_Color color;
		CHE_PDF_ColorSpacePtr csPtr = imgPtr->GetColorspace();
		HE_ULONG width = imgPtr->GetWidth();
		HE_ULONG height = imgPtr->GetHeight();
		HE_ULONG bpc = imgPtr->GetBPC();
		HE_LPBYTE pByte = imgPtr->GetData();
		HE_ULONG byteSize = imgPtr->GetSize();
		HE_ULONG stride = (width * bpc + 7)/8;
		HE_ARGB	colorARGB1 = 0xFF000000;
		HE_ARGB	colorARGB2 = 0xFF000000;
		BOOL bMaskDecode = FALSE;
		if ( imgPtr->GetDecodeArray() )
		{
			CHE_PDF_ObjectPtr objPtr;
			CHE_PDF_ArrayPtr arrayPtr = imgPtr->GetDecodeArray();
			objPtr = arrayPtr->GetElement( 0, OBJ_TYPE_NUMBER );
			if ( objPtr )
			{
				if ( objPtr->GetNumberPtr()->GetFloat() != 0.0f )
				{
					bMaskDecode = TRUE;
				}
			}
		}
		if ( byteSize < height * stride  )
		{
			return pBitmapRet;
		}

		// 这里需要获得当前的绘制颜色吗？
		// 	CHE_PDF_GState * pGStae = GetGState();
		// 	CHE_PDF_Color color;
		// 	CHE_PDF_ColorSpace colorspace( COLORSPACE_DEVICE_GRAY, GetAllocator() );
		// 	if ( pGStae )
		// 	{
		// 		pGStae->GetFillColor( color );
		// 		pGStae->GetFillColorSpace( colorspace );
		// 		colorARGB1 = colorspace.GetARGBValue( color );
		// 	}

		pBitmapRet = GetAllocator()->New<CHE_Bitmap>( GetAllocator() );
		pBitmapRet->Create( width, height, BITMAP_DEPTH_32BPP, BITMAP_DIRECTION_DOWN );

		HE_ULONG x = 0;
		HE_ULONG y = 0;
		HE_ULONG lineOffset = 0;
		HE_ULONG byteOffset = 0;
		HE_ULONG bitOffset = 0;
		HE_BYTE byte = 0;
		HE_BYTE crange = ((1<<bpc) - 1);
		HE_ARGB * pColors = GetAllocator()->NewArray<HE_ARGB>( width );

		if ( bpc == 8 )
		{
			for ( y = 0; y < height; ++y )
			{
				lineOffset = y * stride;
				x = 0;
				for ( byteOffset = 0; byteOffset < stride; ++byteOffset )
				{
					byte = *(pByte + lineOffset + byteOffset);
					if ( ! bMaskDecode )
					{
						colorARGB2 = 255 - byte;
					}else{
						colorARGB2 = byte;
					}
					colorARGB2 = colorARGB2 << 24;
					colorARGB2 = colorARGB1 & 0x00FFFFFF + colorARGB2;
					*(pColors+x) = colorARGB2;
					if ( ++x == width )
					{
						break;
					}
				}
				pBitmapRet->SetPixelColor( 0, y, pColors, width );
			}
		}else if ( bpc == 1 )
		{
			for ( y = 0; y < height; ++y )
			{
				lineOffset = y * stride;
				x = 0;
				for ( byteOffset = 0; byteOffset < stride; ++byteOffset )
				{
					byte = *(pByte + lineOffset + byteOffset);
					for ( bitOffset = 0; bitOffset < 8; ++bitOffset )
					{
						if ( ! bMaskDecode )
						{
							colorARGB2 = 255.0f - ((byte>>(7-bitOffset))&0x01)*255.0f;
						}else{
							colorARGB2 = ((byte>>(7-bitOffset))&0x01)*255.0f;
						}
						colorARGB2 = colorARGB2 << 24;
						colorARGB2 = colorARGB1 & 0x00FFFFFF + colorARGB2;
						*(pColors+x) = colorARGB2;
						if ( ++x == width )
						{
							break;
						}
					}
				}
				pBitmapRet->SetPixelColor( 0, y, pColors, width );
			}
		}

		GetAllocator()->DeleteArray( pColors );
		return pBitmapRet;
	}
	return pBitmapRet;
}

CHE_Bitmap * CHE_PDF_ImageRaster::GetBitmapWithImageMask( const CHE_PDF_ImageXObjectPtr & imgPtr, const CHE_PDF_ImageXObjectPtr & maskPtr )
{
	CHE_Bitmap * pBitmapRet = NULL;
	if ( imgPtr && maskPtr )
	{
		CHE_Bitmap * pBitmapS = GetBitmapNoMask( imgPtr );
		if ( pBitmapS == NULL )
		{
			return pBitmapRet;
		}else{
			pBitmapRet = pBitmapS;
		}

		
		//获得maskPtr的数据并作为bitmaps的alpha通道
		HE_ULONG width = maskPtr->GetWidth();
		HE_ULONG height = maskPtr->GetHeight();

		if ( width != imgPtr->GetWidth() || height != imgPtr->GetHeight() )
		{
			pBitmapS = pBitmapRet->StretchTo( width, height, 0 );
			pBitmapRet->GetAllocator()->Delete( pBitmapRet );
			pBitmapRet = pBitmapS;
		}

		HE_ULONG bpc = maskPtr->GetBPC();
		HE_LPBYTE pByte = maskPtr->GetData();
		HE_ULONG byteSize = maskPtr->GetSize();
		HE_ULONG stride = (width * bpc + 7)/8;
		HE_ARGB * pColors = GetAllocator()->NewArray<HE_ARGB>( width );

		BOOL bMaskDecode = FALSE;
		if ( maskPtr->GetDecodeArray() )
		{
			CHE_PDF_ObjectPtr objPtr;
			CHE_PDF_ArrayPtr arrayPtr = maskPtr->GetDecodeArray();
			objPtr = arrayPtr->GetElement( 0, OBJ_TYPE_NUMBER );
			if ( objPtr )
			{
				if ( objPtr->GetNumberPtr()->GetFloat() != 0.0f )
				{
					bMaskDecode = TRUE;
				}
			}
		}

		HE_ULONG x = 0;
		HE_ULONG y = 0;
		HE_ULONG lineOffset = 0;
		HE_ULONG byteOffset = 0;
		HE_ULONG bitOffset = 0;
		HE_BYTE byte = 0;
		HE_BYTE tmpByte = 0;
		HE_BYTE crange = ((1<<bpc) - 1);
		HE_ARGB argb = 0xFF000000;
		HE_ARGB tmpArgb = 0xFF000000;

		if ( bpc == 8 )
		{
			for ( y = 0; y < height; ++y )
			{
				lineOffset = y * stride;
				x = 0;
				for ( byteOffset = 0; byteOffset < stride; ++byteOffset, ++x )
				{
					byte = *(pByte + lineOffset + byteOffset);
					if ( bMaskDecode )
					{
						byte = 0xFF - byte;
					}
					tmpArgb = byte;
					tmpArgb = tmpArgb<<24;
					pBitmapS->GetPixelColor( x, y, argb );
					tmpArgb = argb & 0x00FFFFFF + tmpArgb;
					*(pColors+x) = tmpArgb;
				}
				pBitmapS->SetPixelColor( 0, y, pColors, width );
			}
		}
		else if ( bpc == 4 )
		{
			for ( y = 0; y < height; ++y )
			{
				lineOffset = y * stride;
				x = 0;
				for ( byteOffset = 0; byteOffset < stride; ++byteOffset )
				{
					byte = *(pByte + lineOffset + byteOffset);
					for ( HE_ULONG j = 0; j < 2; ++j )
					{
						tmpByte = ((byte>>(1-j)*4)&0x0F)*255.0f;
						if ( bMaskDecode )
						{
							tmpByte = 0xFF - tmpByte;
						}
						tmpArgb = tmpByte;
						tmpArgb = tmpArgb<<24;
						pBitmapS->GetPixelColor( x, y, argb );
						tmpArgb = argb & 0x00FFFFFF + tmpArgb;
						*(pColors+x) = tmpArgb;
						if ( ++x == width )
						{
							break;
						}
					}
				}
				pBitmapS->SetPixelColor( 0, y, pColors, width );
			}
		}
		else if ( bpc == 2 )
		{
			for ( y = 0, x = 0; y < height; ++y )
			{
				lineOffset = y * stride;
				x = 0;
				for ( byteOffset = 0; byteOffset < stride; ++byteOffset )
				{
					byte = *(pByte + lineOffset + byteOffset);
					for ( HE_ULONG j = 0; j < 4; ++j )
					{	
						tmpByte = ((byte>>((3-j)*2))&0x03)*255.0f;
						if ( bMaskDecode )
						{
							tmpByte = 0xFF - tmpByte;
						}
						tmpArgb = tmpByte;
						tmpArgb = tmpArgb<<24;
						pBitmapS->GetPixelColor( x, y, argb );
						tmpArgb = argb & 0x00FFFFFF + tmpArgb;
						*(pColors+x) = tmpArgb;
						if ( ++x == width )
						{
							break;
						}
					}
				}
				pBitmapS->SetPixelColor( 0, y, pColors, width );
			}
		}
		else if ( bpc == 1 )
		{
			for ( y = 0, x = 0; y < height; ++y )
			{
				lineOffset = y * stride;
				x = 0;
				for ( byteOffset = 0; byteOffset < stride; ++byteOffset )
				{
					byte = *(pByte + lineOffset + byteOffset);
					for ( HE_ULONG j = 0; j < 8; ++j )
					{
						tmpByte = ((byte>>(7-j)) & 0x01) * 0xFF;
						if ( bMaskDecode )
						{
							tmpByte = 0xFF - tmpByte;
						}
						tmpArgb = tmpByte;
						tmpArgb = tmpArgb<<24;
						pBitmapRet->GetPixelColor( x, y, argb );
						tmpArgb = argb & 0x00FFFFFF + tmpArgb;
						pBitmapRet->SetPixelColor( x, y, tmpArgb );
						*(pColors+x) = tmpArgb;
						if ( ++x == width )
						{
							break;
						}
					}
				}
				pBitmapRet->SetPixelColor( 0, y, pColors, width );
			}
		}

		GetAllocator()->DeleteArray( pColors );
	}
	return pBitmapRet;
}