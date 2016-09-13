#include "../include/che_base_bitmap.h"

#include <cstdlib>
#include <cstdio>
#include <memory>

namespace chelib {

typedef struct
{
	uint16	bfType;
	uint32	bfSize;
	uint16	bfReserved1;
	uint16	bfReserved2;
	uint32	bfOffBits;
}tagBITMAPFILEHEADER;

typedef struct
{	
	uint32	biSize;
	int32	biWidth;
	int32	biHeight;
	uint16	biPlanes;
	uint16 	biBitCount;
	uint32	biCompression;
	uint32	biSizeImage;
	int32	biXPelsPerMeter;
	int32	biYPelsPerMeter;
	uint32	biClrUsed;
	uint32	biClrImportant;
}tagBITMAPINFOHEADER;


CBitmap::CBitmap( CAllocator * pAllocator )
  : CObject(pAllocator), mWidth(0), mHeight(0), mDepth(DEPTH_24BPP),
    mDirection(DIRECTION_UP), mpBits(nullptr) {}

CBitmap::CBitmap( const CBitmap & bitmap )
  : CObject(bitmap.GetAllocator()), mWidth(bitmap.Width()),
    mHeight(bitmap.Height()), mDepth(bitmap.Depth()),
    mDirection(bitmap.Direction()), mpBits(nullptr)
{
	if ( bitmap.mpBits != nullptr )
	{
		mpBits = GetAllocator()->NewArray<BYTE>( bitmap.Pitch() * bitmap.Height() );
		memcpy( mpBits, bitmap.mpBits, bitmap.Pitch() * bitmap.Height() );
	}
}

CBitmap & CBitmap::operator=( const CBitmap & bitmap )
{
	if ( this == &bitmap )
	{
		return *this;
	}
	mWidth = bitmap.mWidth;
	mHeight = bitmap.mHeight;
	mDepth = bitmap.mDepth;
	mDirection = bitmap.mDirection;
	if ( bitmap.mpBits != nullptr )
	{
		mpBits = GetAllocator()->NewArray<BYTE>( bitmap.Pitch() * bitmap.Height() );
		memcpy( mpBits, bitmap.mpBits, bitmap.Pitch() * bitmap.Height() );
	}
	return *this;
}

CBitmap::~CBitmap()
{
	if ( mpBits )
	{
		GetAllocator()->DeleteArray( mpBits );
		mpBits = nullptr;
	}
}

bool CBitmap::Load( char const * filePath )
{
	if ( nullptr == filePath )
	{
		return false;
	}

	FILE * pfile = fopen( filePath, "rb+" );
	if ( nullptr == pfile )
	{
		return false;
	}
	tagBITMAPFILEHEADER bfHeader;
	size_t bytesReaded = fread( &(bfHeader.bfType), 1, sizeof(bfHeader.bfType), pfile );
	if ( bytesReaded != sizeof(bfHeader.bfType) )
	{
		fclose( pfile );
		return false;
	}
	bytesReaded = fread( &(bfHeader.bfSize), 1, sizeof(bfHeader.bfSize), pfile );
	if ( bytesReaded != sizeof(bfHeader.bfSize) )
	{
		fclose( pfile );
		return false;
	}
	bytesReaded = fread( &(bfHeader.bfReserved1), 1, sizeof(bfHeader.bfReserved1), pfile );
	if ( bytesReaded != sizeof(bfHeader.bfReserved1) )
	{
		fclose( pfile );
		return false;
	}
	bytesReaded = fread( &(bfHeader.bfReserved2), 1, sizeof(bfHeader.bfReserved2), pfile );
	if ( bytesReaded != sizeof(bfHeader.bfReserved2) )
	{
		fclose( pfile );
		return false;
	}
	bytesReaded = fread( &(bfHeader.bfOffBits), 1, sizeof(bfHeader.bfOffBits), pfile );
	if ( bytesReaded != sizeof(bfHeader.bfOffBits) )
	{
		fclose( pfile );
		return false;
	}

	if( bfHeader.bfType != ( (uint16)( 'M' << 8 ) | 'B' ) )
	{   
		fclose( pfile );
		return false;
	}

	tagBITMAPINFOHEADER biHeader;
	bytesReaded = fread( &(biHeader.biSize), 1, sizeof(biHeader.biSize), pfile );
	if ( bytesReaded != sizeof(biHeader.biSize) )
	{
		fclose( pfile );
		return false;
	}
	bytesReaded = fread( &(biHeader.biWidth), 1, sizeof(biHeader.biWidth), pfile );
	if ( bytesReaded != sizeof(biHeader.biWidth) )
	{
		fclose( pfile );
		return false;
	}
	bytesReaded = fread( &(biHeader.biHeight), 1, sizeof(biHeader.biHeight), pfile );
	if ( bytesReaded != sizeof(biHeader.biHeight ) )
	{
		fclose( pfile );
		return false;
	}
	bytesReaded = fread( &(biHeader.biPlanes), 1, sizeof(biHeader.biPlanes), pfile );
	if ( bytesReaded != sizeof(biHeader.biPlanes) )
	{
		fclose( pfile );
		return false;
	}
	bytesReaded = fread( &(biHeader.biBitCount), 1, sizeof(biHeader.biBitCount), pfile );
	if ( bytesReaded != sizeof(biHeader.biBitCount) )
	{
		fclose( pfile );
		return false;
	}
	bytesReaded = fread( &(biHeader.biCompression), 1, sizeof(biHeader.biCompression), pfile );
	if ( bytesReaded != sizeof(biHeader.biCompression) )
	{
		fclose( pfile );
		return false;
	}
	bytesReaded = fread( &(biHeader.biSizeImage), 1, sizeof(biHeader.biSizeImage), pfile );
	if ( bytesReaded != sizeof(biHeader.biSizeImage) )
	{
		fclose( pfile );
		return false;
	}
	bytesReaded = fread( &(biHeader.biXPelsPerMeter), 1, sizeof(biHeader.biXPelsPerMeter), pfile );
	if ( bytesReaded != sizeof(biHeader.biXPelsPerMeter) )
	{
		fclose( pfile );
		return false;
	}
	bytesReaded = fread( &(biHeader.biYPelsPerMeter), 1, sizeof(biHeader.biYPelsPerMeter), pfile );
	if ( bytesReaded != sizeof(biHeader.biYPelsPerMeter) )
	{
		fclose( pfile );
		return false;
	}
	bytesReaded = fread( &(biHeader.biClrUsed), 1, sizeof(biHeader.biClrUsed), pfile );
	if ( bytesReaded != sizeof(biHeader.biClrUsed) )
	{
		fclose( pfile );
		return false;
	}
	bytesReaded = fread( &(biHeader.biClrImportant), 1, sizeof(biHeader.biClrImportant), pfile );
	if ( bytesReaded != sizeof(biHeader.biClrImportant) )
	{
		fclose( pfile );
		return false;
	}
	if ( biHeader.biSize != 40 )
	{
		fclose( pfile );
		return false;
	}

	size_t fileSize = 0;
	PBYTE pBits = nullptr;

	fseek( pfile, 0, SEEK_END );
	fileSize = ftell( pfile );
	if ( fileSize < biHeader.biSizeImage )
	{
		fclose( pfile );
		return false;
	}
	fileSize = biHeader.biSizeImage;
	if ( biHeader.biBitCount != 24 || biHeader.biBitCount != 32 )
	{
        fclose( pfile );
        return false;
	}
    
	pBits = GetAllocator()->NewArray<BYTE>( fileSize );
	fseek( pfile, bfHeader.bfOffBits, SEEK_SET );
	bytesReaded = fread( pBits, 1, fileSize, pfile );
	if ( bytesReaded != fileSize )
	{
		GetAllocator()->DeleteArray<BYTE>( pBits );
		pBits = nullptr;
		fclose( pfile );
		return false;
	}
	fclose( pfile );
	pfile = nullptr;

	Clean();

	mWidth = biHeader.biWidth;
	mHeight = (biHeader.biHeight > 0) ? biHeader.biHeight : -biHeader.biHeight;
	mDepth = (biHeader.biBitCount == 32) ? DEPTH_32BPP : DEPTH_24BPP;
	mDirection = (biHeader.biHeight > 0) ? DIRECTION_UP : DIRECTION_DOWN;
	mpBits = pBits;
	return TRUE;
}

bool CBitmap::Save( char const * filePath )
{
	if ( nullptr == filePath )
	{
		return false;
	}
	if ( mpBits == nullptr )
	{
		return false;
	}
	
	FILE * pfile = fopen( filePath, "wb+" );
	if ( nullptr == pfile )
	{
		return false;
	}
    
	tagBITMAPFILEHEADER bfHeader;
	bfHeader.bfSize = (uint32)( 54 + Pitch() * Height() );
	bfHeader.bfType = 0x4D42;
	bfHeader.bfReserved2 = 0;
	bfHeader.bfReserved1 = 0;
	bfHeader.bfOffBits = 54;

	size_t lBytesReaded = fwrite( &(bfHeader.bfType), 1, sizeof(bfHeader.bfType), pfile );
	lBytesReaded += fwrite( &(bfHeader.bfSize), 1, sizeof(bfHeader.bfSize), pfile );
	lBytesReaded += fwrite( &(bfHeader.bfReserved1), 1, sizeof(bfHeader.bfReserved1), pfile );
	lBytesReaded += fwrite( &(bfHeader.bfReserved2), 1, sizeof(bfHeader.bfReserved2), pfile );
	lBytesReaded += fwrite( &(bfHeader.bfOffBits), 1, sizeof(bfHeader.bfOffBits), pfile );
	
	if ( lBytesReaded != 14 )
	{
		fclose( pfile );
		return false;
	}
	tagBITMAPINFOHEADER biHeader;
	biHeader.biSize = 40;
	biHeader.biWidth = (int32)mWidth;
	size_t lHeight = mHeight;
	biHeader.biHeight = ( mDirection == DIRECTION_UP ) ? (int32)(lHeight) : (int32)(-lHeight);
	biHeader.biPlanes = 1;
	biHeader.biBitCount = mDepth;
	biHeader.biCompression = 0;
	biHeader.biSizeImage = (uint32)( Pitch() * Height() );
	biHeader.biXPelsPerMeter = 0;
	biHeader.biYPelsPerMeter = 0;
	biHeader.biClrUsed = 0;
	biHeader.biClrImportant = 0;
	lBytesReaded = fwrite( &(biHeader.biSize), 1, sizeof(biHeader.biSize), pfile );
	lBytesReaded += fwrite( &(biHeader.biWidth), 1, sizeof(biHeader.biWidth), pfile );
	lBytesReaded += fwrite( &(biHeader.biHeight), 1, sizeof(biHeader.biHeight), pfile );
	lBytesReaded += fwrite( &(biHeader.biPlanes), 1, sizeof(biHeader.biPlanes), pfile );
	lBytesReaded += fwrite( &(biHeader.biBitCount), 1, sizeof(biHeader.biBitCount), pfile );
	lBytesReaded += fwrite( &(biHeader.biCompression), 1, sizeof(biHeader.biCompression), pfile );
	lBytesReaded += fwrite( &(biHeader.biSizeImage), 1, sizeof(biHeader.biSizeImage), pfile );
	lBytesReaded += fwrite( &(biHeader.biXPelsPerMeter), 1, sizeof(biHeader.biXPelsPerMeter), pfile );
	lBytesReaded += fwrite( &(biHeader.biYPelsPerMeter), 1, sizeof(biHeader.biYPelsPerMeter), pfile );
	lBytesReaded += fwrite( &(biHeader.biClrUsed), 1, sizeof(biHeader.biClrUsed), pfile );
	lBytesReaded += fwrite( &(biHeader.biClrImportant), 1, sizeof(biHeader.biClrImportant), pfile );	
	if ( lBytesReaded != 40 )
	{
		fclose( pfile );
		return false;
	}

	lBytesReaded = fwrite( mpBits, 1, Pitch() * Height(), pfile );
	if ( lBytesReaded != Pitch() * Height()  )
	{
		fclose( pfile );
		return false;
	}
	fclose( pfile );
	return true;
}

bool CBitmap::SaveToMem( PBYTE buffer, size_t size )
{
	if ( buffer == nullptr || size < GetMemBitmapDataSize() + 14 )
	{
		return false;
	}

	size_t index = 0;

	tagBITMAPFILEHEADER bfHeader;
	bfHeader.bfSize = (uint32)( 54 + Pitch() * Height() );
	bfHeader.bfType = 0x4D42;
	bfHeader.bfReserved2 = 0;
	bfHeader.bfReserved1 = 0;
	bfHeader.bfOffBits = 54;

	memcpy( buffer + index, &(bfHeader.bfType), sizeof(bfHeader.bfType) );
	index += sizeof(bfHeader.bfType);
	memcpy( buffer + index, &(bfHeader.bfSize), sizeof(bfHeader.bfSize) );
	index += sizeof(bfHeader.bfSize);
	memcpy( buffer + index, &(bfHeader.bfReserved2), sizeof(bfHeader.bfReserved2) );
	index += sizeof(bfHeader.bfReserved2);
	memcpy( buffer + index, &(bfHeader.bfReserved1), sizeof(bfHeader.bfReserved1) );
	index += sizeof(bfHeader.bfReserved1);
	memcpy( buffer + index, &(bfHeader.bfOffBits), sizeof(bfHeader.bfOffBits) );
	index += sizeof(bfHeader.bfOffBits);

	tagBITMAPINFOHEADER biHeader;
	biHeader.biSize = 40;
	biHeader.biWidth = (int32)mWidth;
	size_t lHeight = mHeight;
	biHeader.biHeight = ( mDirection == DIRECTION_UP ) ? (int32)(lHeight) : (int32)(-lHeight);
	biHeader.biPlanes = 1;
	biHeader.biBitCount = mDepth;
	biHeader.biCompression = 0;
	biHeader.biSizeImage = (uint32)( Pitch() * Height() );
	biHeader.biXPelsPerMeter = 0;
	biHeader.biYPelsPerMeter = 0;
	biHeader.biClrUsed = 0;
	biHeader.biClrImportant = 0;
	memcpy( buffer + index, &(biHeader.biSize), sizeof(biHeader.biSize) );
	index += sizeof(biHeader.biSize);
	memcpy( buffer + index, &(biHeader.biWidth), sizeof(biHeader.biWidth) );
	index += sizeof(biHeader.biWidth);
	memcpy( buffer + index, &(biHeader.biHeight), sizeof(biHeader.biHeight) );
	index += sizeof(biHeader.biHeight);
	memcpy( buffer + index, &(biHeader.biPlanes), sizeof(biHeader.biPlanes) );
	index += sizeof(biHeader.biPlanes);
	memcpy( buffer + index, &(biHeader.biBitCount), sizeof(biHeader.biBitCount) );
	index += sizeof(biHeader.biBitCount);
	memcpy( buffer + index, &(biHeader.biCompression), sizeof(biHeader.biCompression) );
	index += sizeof(biHeader.biCompression);
	memcpy( buffer + index, &(biHeader.biSizeImage), sizeof(biHeader.biSizeImage) );
	index += sizeof(biHeader.biSizeImage);
	memcpy( buffer + index, &(biHeader.biXPelsPerMeter), sizeof(biHeader.biXPelsPerMeter) );
	index += sizeof(biHeader.biXPelsPerMeter);
	memcpy( buffer + index, &(biHeader.biYPelsPerMeter), sizeof(biHeader.biYPelsPerMeter) );
	index += sizeof(biHeader.biYPelsPerMeter);
	memcpy( buffer + index, &(biHeader.biClrUsed), sizeof(biHeader.biClrUsed) );
	index += sizeof(biHeader.biClrUsed);
	memcpy( buffer + index, &(biHeader.biClrImportant), sizeof(biHeader.biClrImportant) );
	index += sizeof(biHeader.biClrImportant);
	memcpy( buffer + index, mpBits, Pitch() * Height() );
	return true;
}

size_t CBitmap::GetMemBitmapDataSize()
{
	return Pitch() * Height() + 40;
}

bool CBitmap::GetMemBitmapData( PBYTE buffer, size_t size )
{
	if ( buffer == nullptr || size < GetMemBitmapDataSize() )
	{
		return false;
	}

	size_t index = 0;
	tagBITMAPINFOHEADER biHeader;
	biHeader.biSize = 40;
	biHeader.biWidth = (int32)mWidth;
	size_t lHeight = mHeight;
	biHeader.biHeight = ( mDirection == DIRECTION_UP ) ? (int32)(lHeight) : (int32)(-lHeight);
	biHeader.biPlanes = 1;
	biHeader.biBitCount = mDepth;
	biHeader.biCompression = 0;
	biHeader.biSizeImage = (uint32)( Pitch() * Height() );
	biHeader.biXPelsPerMeter = 0;
	biHeader.biYPelsPerMeter = 0;
	biHeader.biClrUsed = 0;
	biHeader.biClrImportant = 0;
	memcpy( buffer + index, &(biHeader.biSize), sizeof(biHeader.biSize) );
	index += sizeof(biHeader.biSize);
	memcpy( buffer + index, &(biHeader.biWidth), sizeof(biHeader.biWidth) );
	index += sizeof(biHeader.biWidth);
	memcpy( buffer + index, &(biHeader.biHeight), sizeof(biHeader.biHeight) );
	index += sizeof(biHeader.biHeight);
	memcpy( buffer + index, &(biHeader.biPlanes), sizeof(biHeader.biPlanes) );
	index += sizeof(biHeader.biPlanes);
	memcpy( buffer + index, &(biHeader.biBitCount), sizeof(biHeader.biBitCount) );
	index += sizeof(biHeader.biBitCount);
	memcpy( buffer + index, &(biHeader.biCompression), sizeof(biHeader.biCompression) );
	index += sizeof(biHeader.biCompression);
	memcpy( buffer + index, &(biHeader.biSizeImage), sizeof(biHeader.biSizeImage) );
	index += sizeof(biHeader.biSizeImage);
	memcpy( buffer + index, &(biHeader.biXPelsPerMeter), sizeof(biHeader.biXPelsPerMeter) );
	index += sizeof(biHeader.biXPelsPerMeter);
	memcpy( buffer + index, &(biHeader.biYPelsPerMeter), sizeof(biHeader.biYPelsPerMeter) );
	index += sizeof(biHeader.biYPelsPerMeter);
	memcpy( buffer + index, &(biHeader.biClrUsed), sizeof(biHeader.biClrUsed) );
	index += sizeof(biHeader.biClrUsed);
	memcpy( buffer + index, &(biHeader.biClrImportant), sizeof(biHeader.biClrImportant) );
	index += sizeof(biHeader.biClrImportant);
	memcpy( buffer + index, mpBits, Pitch() * Height() );
	return TRUE;
}

bool CBitmap::Create( uint32 width, uint32 height, BITMAP_DEPTH depth, BITMAP_DIRECTION direction, size_t bufferSize, PCBYTE buffer )
{
	size_t bitcount = 0, imageSize = 0;
	switch ( depth )
	{
	case DEPTH_24BPP:
		{
			imageSize = ( ( ( width * 24 + 31 ) & ~31 ) >> 3 ) * height;
			break;
		}
	case DEPTH_32BPP:
		{
			imageSize = ( ( ( width * 32 + 31 ) & ~31 ) >> 3 ) * height;
			break;
		}
	default:
		break;
	}
	if ( bitcount == 0 || imageSize == 0 )
	{
		return false;
	}

	mWidth = width;
	mHeight = height;
	mDepth = depth;
	mDirection = direction;
	mpBits = GetAllocator()->NewArray<BYTE>( imageSize );
	memset( mpBits, 0, imageSize );
    if ( bufferSize != 0 && buffer != nullptr )
	{
		if ( bufferSize > imageSize )
		{
			bufferSize = imageSize;	
		}
		memcpy( mpBits, buffer, bufferSize );
	}else{
		Fill( 0xFFFFFFFF );
	}
	return true;
}

void CBitmap::Clean()
{
	if ( mpBits )
	{
		GetAllocator()->DeleteArray( mpBits );
		mpBits = nullptr;
	}
	mWidth = 0;
	mHeight = 0;
	mDepth = DEPTH_24BPP;
	mDirection = DIRECTION_UP;
}

bool CBitmap::GetPixelColor( uint32 x, uint32 y, ARGB & clrRet ) const
{
	if ( x >= Width() )
	{
		return false;
	}
	if ( y >= Height() )
	{
		return false;
	}
	size_t index = GetPixelByteIndex( x, y );
	if ( Depth() == 32 )
	{
		clrRet = mpBits[index] + ( mpBits[index+1] << 8 ) +
            ( mpBits[index+2] << 16 ) + ( mpBits[index+3] << 24 );
	}else if ( Depth() == 24 )
	{
		clrRet = mpBits[index] + ( mpBits[index+1] << 8 ) + ( mpBits[index+2] << 16 );
	}
	return true;
}

bool CBitmap::SetPixelColor( uint32 x, uint32 y, ARGB color )
{
	if ( x >= Width() )
	{
		return false;
	}
	if ( y >= Height() )
	{
		return false;
	}

	size_t index = GetPixelByteIndex( x, y );
	if ( Depth() == 32 )
	{
		mpBits[index]   = (BYTE)( color & 0xFF );
		mpBits[index+1] = (BYTE)( ( color >> 8 ) & 0xFF );
		mpBits[index+2] = (BYTE)( ( color >> 16 ) & 0xFF );
		mpBits[index+3] = (BYTE)( ( color >> 24 ) & 0xFF );
	}else if ( Depth() == 24 )
	{
		mpBits[index]   = (BYTE)( color & 0xFF );
		mpBits[index+1] = (BYTE)( ( color >> 8 ) & 0xFF );
		mpBits[index+2] = (BYTE)( ( color >> 16 ) & 0xFF );
	}
	return true;
}

bool CBitmap::SetPixelColor( uint32 x, uint32 y, std::vector<ARGB> & colors )
{
	if ( x >= Width() )
	{
		return false;
	}
	if ( y >= Height() )
	{
		return false;
	}

	size_t index = 0;
	ARGB color = 0xFFFFFFFF;
	for ( size_t i = 0; i < colors.size(); ++i )
	{
		index = GetPixelByteIndex( x++, y );
		if ( x == mWidth )
		{
			x = 0;
			y += 1;
		}
		color = colors[i];
		if ( Depth() == 32 )
		{
			mpBits[index]   = (BYTE)( color & 0xFF );
			mpBits[index+1] = (BYTE)( ( color >> 8 ) & 0xFF );
			mpBits[index+2] = (BYTE)( ( color >> 16 ) & 0xFF );
			mpBits[index+3] = (BYTE)( ( color >> 24 ) & 0xFF );
		}else if ( Depth() == 24 )
		{
			mpBits[index]   = (BYTE)( color & 0xFF );
			mpBits[index+1] = (BYTE)( ( color >> 8 ) & 0xFF );
			mpBits[index+2] = (BYTE)( ( color >> 16 ) & 0xFF );
		}
	}
	return true;
}

bool CBitmap::SetPixelColor( uint32 x, uint32 y, ARGB * pColors, size_t count )
{
	if ( x >= Width() )
	{
		return false;
	}
	if ( y >= Height() )
	{
		return false;
	}
    if ( pColors == nullptr || count == 0 )
    {
        return false;
    }

	size_t index = 0;
	ARGB color = 0xFFFFFFFF;
    if ( Depth() == 32 )
    {
        for ( size_t i = 0; i < count; ++i )
        {
            index = GetPixelByteIndex( x, y );
            if ( ++x == mWidth )
            {
                x = 0;
                ++y;
            }
            color = *(pColors+i);
            mpBits[index]   = (BYTE)( color & 0xFF );
            mpBits[index+1] = (BYTE)( ( color >> 8 ) & 0xFF );
            mpBits[index+2] = (BYTE)( ( color >> 16 ) & 0xFF );
            mpBits[index+3] = (BYTE)( ( color >> 24 ) & 0xFF );
        }
    }else if ( Depth() == 24 )
    {
        for ( size_t i = 0; i < count; ++i )
        {
            index = GetPixelByteIndex( x, y );
            if ( ++x == mWidth )
            {
                x = 0;
                ++y;
            }
            color = *(pColors+i);
            mpBits[index]   = (BYTE)( color & 0xFF );
            mpBits[index+1] = (BYTE)( ( color >> 8 ) & 0xFF );
            mpBits[index+2] = (BYTE)( ( color >> 16 ) & 0xFF );
        }
    }
	return true;
}

bool CBitmap::Fill( ARGB color )
{
	if ( mpBits == nullptr )
	{
		return false;
	}
	if ( Depth() == 24 )
	{
        for ( size_t i = 0; i+2 < Pitch() * Height(); i+=3 )
        {
            mpBits[i]   = (BYTE)( color & 0xFF );
            mpBits[i+1] = (BYTE)( ( color >> 8 ) & 0xFF );
            mpBits[i+2] = (BYTE)( ( color >> 16 ) & 0xFF );
        }
    }else if ( Depth() == 32 )
    {
        for ( size_t i = 0; i+3 < Pitch() * Height(); i+=4 )
        {
            *((uint32*)(mpBits+i)) = color;
        }
    }
	return true;
}

bool CBitmap::Fill( ARGB color, BITMAP_RECT rect )
{
	if ( rect.height == 0 || rect.width == 0 )
	{
		return false;
	}
	for ( uint32 i = rect.top; i < rect.height + rect.top; i++ )
	{
		DrawLine( i, rect.left, rect.width, color );
	}
	return true;
}

void CBitmap::DrawLine( uint32 lineIndex, uint32 xIndex, uint32 pixelCount, ARGB color )
{ 
	if ( mpBits == nullptr || lineIndex >= Height() || xIndex >= Width() || pixelCount == 0 )
	{
		return;
	}

	if ( xIndex + pixelCount > Width() )
	{
		pixelCount = Width() - xIndex;
	}
	
	size_t indexB = 0, indexE = 0;
	indexB = GetPixelByteIndex( xIndex, lineIndex );
	indexE = GetPixelByteIndex( xIndex + pixelCount, lineIndex );
	if ( Depth() == 24 )
	{
        for ( size_t i = indexB; i < indexE; i+=3 )
        {
            mpBits[i] = (BYTE)( color & 0xFF );
            mpBits[i+1] = (BYTE)( ( color >> 8 ) & 0xFF );
            mpBits[i+2] = (BYTE)( ( color >> 16 ) & 0xFF );
        }
    }else if ( Depth() == 32 )
    {
        for ( size_t i = indexB; i < indexE; i+=4 )
        {
            *((uint32*)(mpBits+i)) = color;
        }
    }
}

void CBitmap::DrawLine( uint32 lineIndex, uint32 xIndex, uint32 pixelCount, PBYTE pData, size_t dataSize )
{ 
	if ( mpBits == nullptr || lineIndex >= Height() || xIndex >= Width() || pixelCount == 0 || pData == nullptr || dataSize == 0 )
	{
		return;
	}

	if ( xIndex + pixelCount > Width() )
	{
		pixelCount = Width() - xIndex;
	}

	if ( ( pixelCount * Depth()) > (dataSize << 3) )
	{
		pixelCount = (uint32)( ( dataSize << 3 ) / Depth() );
	}

	size_t indexB = 0, indexE = 0;
    indexB = GetPixelByteIndex( xIndex, lineIndex );
	indexE = GetPixelByteIndex( xIndex + pixelCount, lineIndex );
	
	if ( Depth() == 24 )
	{
        for ( size_t i = indexB, j = 0; i < indexE; i+=3, j+=3 )
        {
            mpBits[i]   = pData[j];
            mpBits[i+1] = pData[j+1];
            mpBits[i+2] = pData[j+2];
        }
    }else if ( Depth() == 32 )
    {
        for ( size_t i = indexB, j = 0; i < indexE; i+=4, j+=4 )
        {
            *((uint32*)(mpBits+i)) = *((uint32*)(pData+j));
        }
    }
}

bool CBitmap::SetChannel( BITMAP_CHANNEL channel, BYTE value )
{
	if ( ( Depth() < 24 ) || ( Depth() == 24 && channel == CHANNEL_Alpha ) )
	{
		return false;
	}

	if ( mpBits == nullptr )
	{
		return false;
	}

	int32 iByteOffset = 0;
	switch ( channel  )
	{
	case CHANNEL_Alpha:
		iByteOffset = 3;
		break;
	case CHANNEL_Red:
		iByteOffset = 2;
		break;
	case CHANNEL_Green:
		iByteOffset = 1;
		break;
	case CHANNEL_Blue:
		iByteOffset = 0;
		break;
	default:
		return false;
	}

	size_t iByteIndex = 0;
	uint32 iPixelByteCount = Depth()>>3;
	for ( uint32 iY = 0; iY < Height(); iY++ )
	{
		iByteIndex = GetPixelByteIndex( 0, iY );
		for ( uint32 iX = 0; iX < Width(); iX++ )
		{
			mpBits[iByteIndex + ( iX * iPixelByteCount ) + iByteOffset] = value;
		}
	}
	return TRUE;
}

bool CBitmap::SetChannelByAlpha( BITMAP_CHANNEL channel, BYTE alpha )
{
	if ( ( Depth() < 24 ) || ( Depth() == 24 && channel == CHANNEL_Alpha ) )
	{
		return false;
	}
	
	if ( mpBits == nullptr )
	{
		return false;
	}
	
	int32 iByteOffset = 0;
	switch ( channel  )
	{
	case CHANNEL_Alpha:
		iByteOffset = 3;
		break;
	case CHANNEL_Red:
		iByteOffset = 2;
		break;
	case CHANNEL_Green:
		iByteOffset = 1;
		break;
	case CHANNEL_Blue:
		iByteOffset = 0;
		break;
	default:
		return false;
	}
	
	size_t iByteIndex = 0;
	size_t iPixelByteCount = Depth()>>3;
	FLOAT fTemp = 0.0;
	FLOAT fAlpha = alpha;
	for ( uint32 iY = 0; iY < Height(); iY++ )
	{
		iByteIndex = GetPixelByteIndex( 0, iY );
		for ( uint32 iX = 0; iX < Width(); iX++ )
		{
			fTemp = mpBits[iByteIndex + ( iX * iPixelByteCount ) + iByteOffset];
			mpBits[iByteIndex + ( iX * iPixelByteCount ) + iByteOffset] = (BYTE)(fTemp*(fAlpha/255)) ;
		}
	}
	return true;
}

bool CBitmap::ExchangeChannel( BITMAP_CHANNEL channel1, BITMAP_CHANNEL channel2 )
{
	if ( ( Depth() == 24 && ( channel1 == CHANNEL_Alpha || channel2 == CHANNEL_Alpha ) ) ||
		 channel1 == channel2 )
	{
		return false;
	}
	
	if ( mpBits == nullptr )
	{
		return false;
	}
	
	int32 iByteOffset1 = 0;
	int32 iByteOffset2 = 0;
	switch ( channel1 )
	{
	case CHANNEL_Alpha:
		iByteOffset1 = 3;
		break;
	case CHANNEL_Red:
		iByteOffset1 = 2;
		break;
	case CHANNEL_Green:
		iByteOffset1 = 1;
		break;
	case CHANNEL_Blue:
		iByteOffset1 = 0;
		break;
	default:
		return false;
	}

	switch ( channel2 )
	{
	case CHANNEL_Alpha:
		iByteOffset2 = 3;
		break;
	case CHANNEL_Red:
		iByteOffset2 = 2;
		break;
	case CHANNEL_Green:
		iByteOffset2 = 1;
		break;
	case CHANNEL_Blue:
		iByteOffset2 = 0;
		break;
	default:
		return false;
	}
	
	size_t iByteIndex = 0;
	uint32 iPixelByteCount = Depth()>>3;
	BYTE iTemp = 0;
	for ( uint32 iY = 0; iY < Height(); iY++ )
	{
		iByteIndex = GetPixelByteIndex( 0, iY );
		for ( uint32 iX = 0; iX < Width(); iX++ )
		{
			iTemp = mpBits[iByteIndex + ( iX * iPixelByteCount ) + iByteOffset2];
			mpBits[iByteIndex + ( iX * iPixelByteCount ) + iByteOffset2] \
				= mpBits[iByteIndex + ( iX * iPixelByteCount ) + iByteOffset1];
			mpBits[iByteIndex + ( iX * iPixelByteCount ) + iByteOffset1] = iTemp;
		}
	}
	return true;
}

bool CBitmap::CopyChannel( BITMAP_CHANNEL channelDes, BITMAP_CHANNEL channelSrc )
{
	if ( ( Depth() == 24 && ( channelDes == CHANNEL_Alpha || channelSrc == CHANNEL_Alpha ) ) ||
		 channelDes == channelSrc )
	{
		return false;
	}
	
	if ( mpBits == nullptr )
	{
		return false;
	}
	int32 iByteOffsetDes = 0;
	int32 iByteOffsetSrc = 0;
	switch ( channelSrc )
	{
	case CHANNEL_Alpha:
		iByteOffsetSrc = 3;
		break;
	case CHANNEL_Red:
		iByteOffsetSrc = 2;
		break;
	case CHANNEL_Green:
		iByteOffsetSrc = 1;
		break;
	case CHANNEL_Blue:
		iByteOffsetSrc = 0;
		break;
	default:
		return false;
	}
	switch ( channelDes )
	{
	case CHANNEL_Alpha:
		iByteOffsetDes = 3;
		break;
	case CHANNEL_Red:
		iByteOffsetDes = 2;
		break;
	case CHANNEL_Green:
		iByteOffsetDes = 1;
		break;
	case CHANNEL_Blue:
		iByteOffsetDes = 0;
		break;
	default:
		return false;
	}
	size_t iByteIndex = 0;
	uint32 iPixelByteCount = Depth()>>3;
	for ( uint32 iY = 0; iY < Height(); iY++ )
	{
		iByteIndex = GetPixelByteIndex( 0, iY );
		for ( uint32 iX = 0; iX < Width(); iX++ )
		{
			mpBits[iByteIndex + ( iX * iPixelByteCount ) + iByteOffsetDes] \
				= mpBits[iByteIndex + ( iX * iPixelByteCount ) + iByteOffsetSrc];
		}
	}
	return TRUE;
}

CBitmap * CBitmap::Clip(BITMAP_RECT rect) const
{
    if ( rect.width == 0 || rect.height == 0 )
    {
        return nullptr;
    }
    if ( rect.width > Width() - rect.left )
    {
        rect.width = Width() - rect.left;
    }
    if ( rect.height > Height() - rect.top )
    {
        rect.height = Height() - rect.top;
    }
    if ( Depth() == 24 )
    {
        CBitmap * bitmap = GetAllocator()->New<CBitmap>( GetAllocator() );
        bitmap->Create( rect.width, rect.height, Depth(), Direction(), ( ( ( rect.width * Depth() ) + 31 ) & ~31 ) >> 3 * rect.height );
        PBYTE lpLineData = GetAllocator()->NewArray<BYTE>( ( ( ( rect.width * Depth() ) + 31 ) & ~31 ) >> 3 );
        size_t indexB = 0, indexE = 0;
        for ( uint32 y = rect.top, desY = 0; y < rect.top + rect.height; y++, desY++ )
        {
            if ( Direction() == DIRECTION_UP )
            {
                indexB = ( Height() - 1 - y ) * Pitch();
                indexE = indexB;
                indexB += ( rect.left * Depth() ) >> 3;
                indexE += ( ( ( rect.left + rect.width - 1 ) * Depth() ) >> 3 ) + 2;
            }else{
                indexB = y * Pitch();
                indexE = indexB;
                indexB += ( rect.left * Depth() ) >> 3;
                indexE += ( ( ( rect.left + rect.width - 1 ) * Depth() ) >> 3 ) + 2;
            }
            for ( size_t oriIndex = indexB, desIndex = 0; oriIndex <= indexE; oriIndex++, desIndex++ )
            {
                lpLineData[desIndex] = mpBits[oriIndex];
            }
            bitmap->DrawLine( desY, 0, rect.width, lpLineData, ( ( ( rect.width * Depth() ) + 31 ) & ~31 ) >> 3 );
        }
        GetAllocator()->DeleteArray<BYTE>( lpLineData );
        return bitmap;
    }else if ( Depth() == 32 )
    {
        CBitmap * bitmap = GetAllocator()->New<CBitmap>( GetAllocator() );
        bitmap->Create( rect.width, rect.height, Depth(), Direction(), ( ( ( rect.width * Depth() ) + 31 ) & ~31 ) >> 3 * rect.height );
        PBYTE lpLineData = GetAllocator()->NewArray<BYTE>( ( ( ( rect.width * Depth() ) + 31 ) & ~31 ) >> 3 );
        size_t indexB = 0, indexE = 0;
        for ( uint32 y = rect.top, desY = 0; y < rect.top + rect.height; y++, desY++ )
        {
            if ( Direction() == DIRECTION_UP )
            {
                indexB = ( Height() - 1 - y ) * Pitch();
                indexE = indexB;
                indexB += ( rect.left * Depth() ) >> 3;
                indexE += ( ( ( rect.left + rect.width - 1 ) * Depth() ) >> 3 ) + 3;
            }else{
                indexB = y * Pitch();
                indexE = indexB;
                indexB += ( rect.left * Depth() ) >> 3;
                indexE += ( ( ( rect.left + rect.width - 1 ) * Depth() ) >> 3 ) + 3;
            }
            for ( size_t oriIndex = indexB, desIndex = 0; oriIndex <= indexE; oriIndex++, desIndex++ )
            {
                lpLineData[desIndex] = mpBits[oriIndex];
            }
            bitmap->DrawLine( desY, 0, rect.width, lpLineData, ( ( ( rect.width * Depth() ) + 31 ) & ~31 ) >> 3 );
        }
        GetAllocator()->DeleteArray<BYTE>( lpLineData );
        return bitmap;
    }
    return nullptr;
}

CBitmap * CBitmap::Clone() const
{
	if ( mpBits == nullptr )
	{
		return nullptr;
	}
    CBitmap * bitmap = GetAllocator()->New<CBitmap>( GetAllocator() );
    bitmap->Create( Width(), Height(), Depth(), Direction(), Pitch() * Height(), GetBuffer() );
    return bitmap;
}

/*bool CBitmap::CompositeMask( ARGB color, uint32 x, uint32 y, CBitmap & maskBitmap )
{
	if ( Depth() < 24 || maskBitmap.GetBuffer() == nullptr ||
		 ( maskBitmap.Depth() != BITMAP_DEPTH_1BPP && maskBitmap.Depth() != BITMAP_DEPTH_8BPP ) )
	{
		return false;
	}

	uint32 nWidth = maskBitmap.Width(), nHeight = maskBitmap.Height();
	if ( Width() - x < nWidth )
	{
		nWidth = Width() - x;
	}
	if ( Height() - y < nHeight )
	{
		nHeight = Height() - y;
	}
	
	switch ( Depth() )
	{
	case 24:
		{
			BYTE nMaskValue = 0;
			size_t byteIndex = 0;
			FLOAT fTemp = 0.0;
			if ( maskBitmap.Depth() == 8 )
			{
				for ( uint32 nY = 0; nY < nHeight; nY++ )
				{
					for ( uint32 nX = 0; nX < nWidth; nX++ )
					{
						if ( maskBitmap.GetPixelIndex( nX, nY, nMaskValue ) )
						{
							fTemp = nMaskValue;
							fTemp /= 255;
							byteIndex = GetPixelByteIndex( x + nX, y + nY );
							mpBits[byteIndex] = (BYTE)( (1-fTemp)*mpBits[byteIndex] + fTemp* (color&0xFF) );
							mpBits[byteIndex+1] = (BYTE)( (1-fTemp)*mpBits[byteIndex+1] + fTemp* ((color>>8)&0xFF) );
							mpBits[byteIndex+2] = (BYTE)( (1-fTemp)*mpBits[byteIndex+2] + fTemp* ((color>>16)&0xFF) );
						}
					}
				}
			}else{
				for ( uint32 nY = 0; nY < nHeight; nY++ )
				{
					for ( uint32 nX = 0; nX < nWidth; nX++ )
					{
						if ( maskBitmap.GetPixelIndex( nX, nY, nMaskValue ) )
						{
							byteIndex = GetPixelByteIndex( x + nX, y + nY );
							if ( nMaskValue == 0 )
							{
								mpBits[byteIndex] = (BYTE)( color & 0xFF );
								mpBits[byteIndex+1] = (BYTE)( ( color >> 8 ) & 0xFF );
								mpBits[byteIndex+2] = (BYTE)( ( color >> 16 ) & 0xFF );
							}
						}
					}
				}
			}
			break;
		}
	case 32:
		{
			BYTE nMaskValue = 0;
			size_t byteIndex = 0;
			FLOAT fTemp = 0.0;
			if ( maskBitmap.Depth() == 8 )
			{
				for ( uint32 nY = 0; nY < nHeight; nY++ )
				{
					for ( uint32 nX = 0; nX < nWidth; nX++ )
					{
						if ( maskBitmap.GetPixelIndex( nX, nY, nMaskValue ) )
						{
							fTemp = nMaskValue;
							fTemp /= 255;
							byteIndex = GetPixelByteIndex( x + nX, y + nY );
							mpBits[byteIndex] = (BYTE)( (1-fTemp)*mpBits[byteIndex] + fTemp* (color&0xFF) );
							mpBits[byteIndex+1] = (BYTE)( (1-fTemp)*mpBits[byteIndex+1] + fTemp* ((color>>8)&0xFF) );
							mpBits[byteIndex+2] = (BYTE)( (1-fTemp)*mpBits[byteIndex+2] + fTemp* ((color>>16)&0xFF) );
							mpBits[byteIndex+3] = (BYTE)( (1-fTemp)*mpBits[byteIndex+3] + fTemp* ((color>>24)&0xFF) );
						}
					}
				}
			}else{
				for ( uint32 nY = 0; nY < nHeight; nY++ )
				{
					for ( uint32 nX = 0; nX < nWidth; nX++ )
					{
						if ( maskBitmap.GetPixelIndex( nX, nY, nMaskValue ) )
						{
							byteIndex = GetPixelByteIndex( x + nX, y + nY );
							if ( nMaskValue == 0 )
							{
								mpBits[byteIndex] = (BYTE)( color & 0xFF );
								mpBits[byteIndex+1] = (BYTE)( ( color >> 8 ) & 0xFF );
								mpBits[byteIndex+2] = (BYTE)( ( color >> 16 ) & 0xFF );
								mpBits[byteIndex+3] = (BYTE)( ( color >> 24 ) & 0xFF );
							}
						}
					}
				}
			}
			break;
		}
	default:
		return false;
	}
	return TRUE;
		
}

bool CBitmap::Insert( const CBitmap & bitmap, uint32 x, uint32 y )
{
	if ( this->Depth() != bitmap.Depth() )
	{
		return false;
	}
	
	if ( bitmap.mpBits == nullptr || x >= this->Width() || y >= this->Height() )
	{
		return false;
	}
	
	BITMAP_RECT rtSrcBitmap;
	rtSrcBitmap.left = 0;
	rtSrcBitmap.top = 0;
	rtSrcBitmap.height = ((Height() - y) > bitmap.Height()) ? (bitmap.Height()) : (Height() - y);
	rtSrcBitmap.width = ((Width() - x) > bitmap.Width()) ? (bitmap.Width()) : (Width() - x);
	
	CBitmap * tempBitmap = bitmap.Clone( &rtSrcBitmap );
	if ( tempBitmap == nullptr )
	{
		return false;
	}
	
	size_t index = 0, lineDataSize = tempBitmap->Pitch();
	for ( uint32 lineIndex = 0; lineIndex < rtSrcBitmap.height; lineIndex++ )
	{
		index = tempBitmap->GetPixelByteIndex( 0, lineIndex );
		DrawLine( lineIndex + y, x, tempBitmap->Width(), &(tempBitmap->mpBits[index]), lineDataSize );
	}
	
	GetAllocator()->Delete( tempBitmap );
	
	return TRUE;
}

CBitmap* CBitmap::StretchTo( uint32 desWidth, uint32 desHeight, uint32 flag, BITMAP_RECT * pRect )
{
	if ( desWidth == 0 || desHeight == 0 )
	{
		return nullptr;
	}

	CBitmap * pBitmapRet = GetAllocator()->New<CBitmap>( GetAllocator() );

	if ( !pBitmapRet->Create( desWidth, desHeight, this->Depth(), this->Direction() ) )
	{
		return nullptr;	
	}

	FLOAT fWidthScale = 0.0, fHeightScale = 0.0;
	fWidthScale = ((FLOAT)Width()) / desWidth;
	fHeightScale = ((FLOAT)Height()) / desHeight;

	switch ( flag )
	{
	case 0:
		{
			ARGB color = 0xFFFFFFFF;
			for ( uint32 iY = 0; iY < desHeight; iY++ )
			{
				for ( uint32 iX = 0; iX < desWidth; iX++ )
				{
					if ( GetPixelColor( (uint32)(iX * fWidthScale), (uint32)(iY * fHeightScale), color ) )
					{
						pBitmapRet->SetPixelColor( iX, iY, color );
					}
				}
			}
			break;
		}
	case 1:
		{
			ARGB color = 0xFFFFFFFF;
			BYTE red = 0, green = 0, blue = 0;
			FLOAT fX = 0, fY = 0;
			uint32 XS = 0, XB = 0, YS = 0, YB = 0;
			BYTE clrYS = 0, clrYB = 0, clrTmp1 = 0, clrTmp2;
			ARGB tmpClr = 0xFFFFFFFF;
			for ( uint32 iY = 0; iY < desHeight; iY++ )
			{
				for ( uint32 iX = 0; iX < desWidth; iX++ )
				{
					fX = iX * fWidthScale;
					fY = iY * fHeightScale;

					XS = (uint32)fX;
					XB = (uint32)(fX + 1);
					YS = (uint32)fY;
					YB = (uint32)(fY + 1);

					GetPixelColor( XS, YS, tmpClr );
					clrYS = (BYTE)(tmpClr >> 16);
					GetPixelColor( XS, YB, tmpClr );
					clrYB = (BYTE)(tmpClr >> 16);
					clrTmp1 = (BYTE)(clrYB - (clrYB-clrYS)*(YB-fY)/(YB-YS));

					GetPixelColor( XB, YS, tmpClr );
					clrYS = (BYTE)(tmpClr >> 16);
					GetPixelColor( XB, YB, tmpClr );
					clrYB = (BYTE)(tmpClr >> 16);
					clrTmp2 = (BYTE)(clrYB - (clrYB-clrYS)*(YB-fY)/(YB-YS));

					red = (BYTE)(clrTmp2 - (clrTmp2-clrTmp1)*(XB-fX)/(XB-XS));

					GetPixelColor( XS, YS, tmpClr );
					clrYS = (BYTE)(tmpClr >> 8);
					GetPixelColor( XS, YB, tmpClr );
					clrYB = (BYTE)(tmpClr >> 8);
					clrTmp1 = (BYTE)(clrYB - (clrYB-clrYS)*(YB-fY)/(YB-YS));
					
					GetPixelColor( XB, YS, tmpClr );
					clrYS = (BYTE)(tmpClr >> 8);
					GetPixelColor( XB, YB, tmpClr );
					clrYB = (BYTE)(tmpClr >> 8);
					clrTmp2 = (BYTE)(clrYB - (clrYB-clrYS)*(YB-fY)/(YB-YS));
					
					green = (BYTE)(clrTmp2 - (clrTmp2-clrTmp1)*(XB-fX)/(XB-XS));

					GetPixelColor( XS, YS, tmpClr );
					clrYS = (BYTE)tmpClr;
					GetPixelColor( XS, YB, tmpClr );
					clrYB = (BYTE)tmpClr;
					clrTmp1 = (BYTE)(clrYB - (clrYB-clrYS)*(YB-fY)/(YB-YS));
					
					GetPixelColor( XB, YS, tmpClr );
					clrYS = (BYTE)tmpClr;
					GetPixelColor( XB, YB, tmpClr );
					clrYB = (BYTE)tmpClr;
					clrTmp2 = (BYTE)(clrYB - (clrYB-clrYS)*(YB-fY)/(YB-YS));
					
					blue = (BYTE)(clrTmp2 - (clrTmp2-clrTmp1)*(XB-fX)/(XB-XS));

					color = (red << 16) + (green << 8) + blue;

					pBitmapRet->SetPixelColor( iX, iY, color );
				}
			}
			break;
		}
	default:
		{
			pBitmapRet->Clean();
			GetAllocator()->Delete<CBitmap>( pBitmapRet );
			return nullptr;
		}
	}
	return pBitmapRet;
}

CBitmap*	CBitmap::Translate( FLOAT a, FLOAT b, FLOAT c, FLOAT d, FLOAT e, FLOAT f )
{
	CBitmap * pBitmapRet = GetAllocator()->New<CBitmap>( GetAllocator() );
	if ( !pBitmapRet->Create( mWidth, mHeight, this->Depth(), this->Direction() ) )
	{
		return nullptr;	
	}
	pBitmapRet->Fill( 0xFF000000 );
	ARGB color = 0xFFFFFFFF;
	BYTE red = 0, green = 0, blue = 0;
	FLOAT fX = 0, fY = 0;
	uint32 XS = 0, XB = 0, YS = 0, YB = 0;
	BYTE clrYS = 0, clrYB = 0, clrTmp1 = 0, clrTmp2;
	ARGB tmpClr = 0xFFFFFFFF;
	for ( uint32 iY = 0; iY < mHeight; iY++ )
	{
		for ( uint32 iX = 0; iX < mWidth; iX++ )
		{
			fX = a * iX + c * iY - e;
			fY = b * iX + d * iY - f;

			XS = (uint32)fX;
			YS = (uint32)fY;
			XB = XS + 1;
			YB = YS + 1;
			
			if ( GetPixelColor( XS, YS, tmpClr ) )
			{
				clrYS = (BYTE)(tmpClr >> 16);
			}else{
				clrYS = 0x00;
			}
			if ( GetPixelColor( XS, YB, tmpClr ) )
			{
				clrYB = (BYTE)(tmpClr >> 16);
			}else{
				clrYB = 0x00;
			}
			clrTmp1 = (BYTE)(clrYB - (clrYB-clrYS)*(YB-fY)/(YB-YS));
			
			if( GetPixelColor( XB, YS, tmpClr ) )
			{
				clrYS = (BYTE)(tmpClr >> 16);
			}else{
				clrYS = 0x00;
			}
			if( GetPixelColor( XB, YB, tmpClr ) )
			{
				clrYB = (BYTE)(tmpClr >> 16);
			}else{
				clrYB = 0x00;
			}
			clrTmp2 = (BYTE)(clrYB - (clrYB-clrYS)*(YB-fY)/(YB-YS));
		
			red = (BYTE)(clrTmp2 - (clrTmp2-clrTmp1)*(XB-fX)/(XB-XS));
			
			if ( GetPixelColor( XS, YS, tmpClr ) )
			{
				clrYS = (BYTE)(tmpClr >> 8);
			}else{
				clrYS = 0x00;
			}
			if ( GetPixelColor( XS, YB, tmpClr ) )
			{
				clrYB = (BYTE)(tmpClr >> 8);
			}else{
				clrYB = 0x00;
			}
			clrTmp1 = (BYTE)(clrYB - (clrYB-clrYS)*(YB-fY)/(YB-YS));
			
			if( GetPixelColor( XB, YS, tmpClr ) )
			{
				clrYS = (BYTE)(tmpClr >> 8);
			}else{
				clrYS = 0x00;
			}
			if( GetPixelColor( XB, YB, tmpClr ) )
			{
				clrYB = (BYTE)(tmpClr >> 8);
			}else{
				clrYB = 0x00;
			}
			clrTmp2 = (BYTE)(clrYB - (clrYB-clrYS)*(YB-fY)/(YB-YS));
			
			green = (BYTE)(clrTmp2 - (clrTmp2-clrTmp1)*(XB-fX)/(XB-XS));
			
			if ( GetPixelColor( XS, YS, tmpClr ) )
			{
				clrYS = (BYTE)(tmpClr >> 8);
			}else{
				clrYS = 0x00;
			}
			if ( GetPixelColor( XS, YB, tmpClr ) )
			{
				clrYB = (BYTE)(tmpClr);
			}else{
				clrYB = 0x00;
			}
			clrTmp1 = (BYTE)(clrYB - (clrYB-clrYS)*(YB-fY)/(YB-YS));
			
			if( GetPixelColor( XB, YS, tmpClr ) )
			{
				clrYS = (BYTE)(tmpClr);
			}else{
				clrYS = 0x00;
			}
			if( GetPixelColor( XB, YB, tmpClr ) )
			{
				clrYB = (BYTE)(tmpClr);
			}else{
				clrYB = 0x00;
			}
			clrTmp2 = (BYTE)(clrYB - (clrYB-clrYS)*(YB-fY)/(YB-YS));
			
			blue = (BYTE)(clrTmp2 - (clrTmp2-clrTmp1)*(XB-fX)/(XB-XS));
			
			color = (red << 16) + (green << 8) + blue;
			
			pBitmapRet->SetPixelColor( iX, iY, color );
		}
	}
	return pBitmapRet;
}*/


size_t CBitmap::GetPixelByteIndex( uint32 x, uint32 y ) const
{
	size_t index = 0;
	if ( Direction() == DIRECTION_UP )
	{
		index = ( Height() - 1 - y ) * Pitch();
		index += ( x * Depth() ) >> 3;
	}else{
		index = y * Pitch();
		index += ( x * Depth() ) >> 3;
	}
	return index;
}

}//namespace