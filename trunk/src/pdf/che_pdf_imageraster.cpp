#include "../../include/pdf/che_pdf_imageraster.h"


CHE_PDF_ImageRaster::CHE_PDF_ImageRaster( CHE_Allocator * pAllocator/*= nullptr*/ )
	: CHE_Object( pAllocator ) {}

CHE_Bitmap * CHE_PDF_ImageRaster::GetBitmap( const CHE_PDF_ImageXObjectPtr & imgPtr )
{
	return GetBitmapSimpleImp( imgPtr );
}

CHE_Bitmap * CHE_PDF_ImageRaster::GetBitmapImp( const CHE_PDF_ImageXObjectPtr & imgPtr )
{
	//todo
	return nullptr;
}

CHE_Bitmap * CHE_PDF_ImageRaster::GetBitmapSimpleImp( const CHE_PDF_ImageXObjectPtr & imgPtr )
{
	CHE_Bitmap * pBitmapRet = nullptr;
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
			return GetBitmapWithImageMask( imgPtr, imgPtr->GetSoftMaskImagePtr(), TRUE );
		}

		CHE_PDF_Color color;
		CHE_PDF_ColorSpacePtr csPtr = imgPtr->GetColorspace();
		size_t width = imgPtr->GetWidth();
		size_t height = imgPtr->GetHeight();
		size_t bpc = imgPtr->GetBPC();
		PBYTE pByte = imgPtr->GetData();
		size_t byteSize = imgPtr->GetSize();

		size_t cscc = 0;
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
		
		size_t stride = (width * cscc * bpc + 7)/8;
		size_t x = 0;
		size_t y = 0;
		size_t lineOffset = 0;
		size_t byteOffset = 0;
		size_t bitOffset = 0;
		BYTE byte = 0;
		BYTE tmpByte = 0;
		BYTE crange = ((1<<bpc) - 1);
		FLOAT cf = 0.0f;
		ARGB argb = 0;

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
	CHE_Bitmap * pBitmapRet = nullptr;
	if ( imgPtr )
	{
		CHE_PDF_Color color;
		CHE_PDF_ColorSpacePtr csPtr = imgPtr->GetColorspace();
		size_t width = imgPtr->GetWidth();
		size_t height = imgPtr->GetHeight();
		size_t bpc = imgPtr->GetBPC();
		PBYTE pByte = imgPtr->GetData();
		size_t byteSize = imgPtr->GetSize();

		size_t cscc = 0;
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

		size_t stride = (width * cscc * bpc + 7)/8;
		size_t x = 0;
		size_t y = 0;
		size_t lineOffset = 0;
		size_t byteOffset = 0;
		size_t bitOffset = 0;
		BYTE byte = 0;
		BYTE tmpByte = 0;
		BYTE crange = ((1<<bpc) - 1);
		FLOAT cf = 0.0f;
		ARGB argb = 0;

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
	CHE_Bitmap * pBitmapRet = nullptr;
	if ( imgPtr )
	{
		if ( ! imgPtr->IsMask() )
		{
			return pBitmapRet;
		}

		CHE_PDF_Color color;
		CHE_PDF_ColorSpacePtr csPtr = imgPtr->GetColorspace();
		size_t width = imgPtr->GetWidth();
		size_t height = imgPtr->GetHeight();
		size_t bpc = imgPtr->GetBPC();
		PBYTE pByte = imgPtr->GetData();
		size_t byteSize = imgPtr->GetSize();
		size_t stride = (width * bpc + 7)/8;
		ARGB	colorARGB1 = 0xFF000000;
		ARGB	colorARGB2 = 0xFF000000;
		BOOL bMaskDecode = false;
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

		size_t x = 0;
		size_t y = 0;
		size_t lineOffset = 0;
		size_t byteOffset = 0;
		size_t bitOffset = 0;
		BYTE byte = 0;
		BYTE crange = ((1<<bpc) - 1);
		ARGB * pColors = GetAllocator()->NewArray<ARGB>( width );

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

CHE_Bitmap * CHE_PDF_ImageRaster::GetBitmapWithImageMask(const CHE_PDF_ImageXObjectPtr & imgPtr, const CHE_PDF_ImageXObjectPtr & maskPtr, bool bExplicit/*= false*/)
{
	CHE_Bitmap * pBitmapRet = nullptr;
	if ( imgPtr && maskPtr )
	{
		CHE_Bitmap * pBitmapS = GetBitmapNoMask( imgPtr );
		if ( pBitmapS == nullptr )
		{
			return pBitmapRet;
		}else{
			pBitmapRet = pBitmapS;
		}

		
		//获得maskPtr的数据并作为bitmaps的alpha通道
		size_t width = maskPtr->GetWidth();
		size_t height = maskPtr->GetHeight();

		if ( width != imgPtr->GetWidth() || height != imgPtr->GetHeight() )
		{
			pBitmapS = pBitmapRet->StretchTo( width, height, 0 );
			pBitmapRet->GetAllocator()->Delete( pBitmapRet );
			pBitmapRet = pBitmapS;
		}

		size_t bpc = maskPtr->GetBPC();
		PBYTE pByte = maskPtr->GetData();
		size_t byteSize = maskPtr->GetSize();
		size_t stride = (width * bpc + 7)/8;
		ARGB * pColors = GetAllocator()->NewArray<ARGB>( width );

		BOOL bMaskDecode = false;
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

		if ( bExplicit == false )
		{
			bMaskDecode = !bMaskDecode;
		}

		size_t x = 0;
		size_t y = 0;
		size_t lineOffset = 0;
		size_t byteOffset = 0;
		size_t bitOffset = 0;
		BYTE byte = 0;
		BYTE tmpByte = 0;
		BYTE crange = ((1<<bpc) - 1);
		ARGB argb = 0xFF000000;
		ARGB tmpArgb = 0xFF000000;

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
					for ( size_t j = 0; j < 2; ++j )
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
					for ( size_t j = 0; j < 4; ++j )
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
					for ( size_t j = 0; j < 8; ++j )
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