#include "../include/che_bitmap.h"

#include <cstdlib>
#include <cstdio>
#include <memory>


typedef struct tagHE_BITMAPFILEHEADER
{
	uint16	bfType;
	uint32	bfSize;
	uint16	bfReserved1;
	uint16	bfReserved2;
	uint32	bfOffBits;
}HE_BITMAPFILEHEADER, * HE_LPBITMAPFILEHEADER;

typedef struct tagHE_BITMAPINFOHEADER
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
}HE_BITMAPINFOHEADER, *HE_LPBITMAPINFOHEADER;


CHE_Palette::CHE_Palette( HE_BITMAP_DEPTH depth, const ARGB * const pPalette,
                          CHE_Allocator * pAllocator ) : CHE_Object( pAllocator )
{
	if (pPalette)
	{
		switch (depth)
		{
		case BITMAP_DEPTH_1BPP:
			mPaletteSize = 2;
			break;
		case BITMAP_DEPTH_4BPP:
			mPaletteSize = 16;
			break;
		case BITMAP_DEPTH_8BPP:
			mPaletteSize = 256;
			break;
		case BITMAP_DEPTH_24BPP:
		case BITMAP_DEPTH_32BPP:
		default:
			mpPalette = nullptr;
			mPaletteSize = 0;
			break;
		}
		if (mPaletteSize)
		{
			mpPalette = GetAllocator()->NewArray<ARGB>( mPaletteSize );
			memset( mpPalette, 0, sizeof(ARGB) * mPaletteSize );
			memcpy( mpPalette, pPalette, sizeof(ARGB) * mPaletteSize );
		}
	}else{
		switch (depth)
		{
		case BITMAP_DEPTH_1BPP:
			{
				mPaletteSize = 2;
				mpPalette = GetAllocator()->NewArray<ARGB>( 2 );
				mpPalette[0] = 0xFFFFFFFF;
				mpPalette[1] = 0xFF000000;
				break;
			}
		case BITMAP_DEPTH_4BPP:
			{
				mPaletteSize = 16;
				mpPalette = GetAllocator()->NewArray<ARGB>( 16 );
				mpPalette[0] = 0xFFFFFFFF;
				mpPalette[1] = 0xFFFF00FF;
				mpPalette[2] = 0xFFFFFF00;
				mpPalette[3] = 0xFF00FFFF;
				mpPalette[4] = 0xFF0000FF;
				mpPalette[5] = 0xFF00FF00;
				mpPalette[6] = 0xFFFF0000;
				mpPalette[7] = 0xFFC0C0C0;
				mpPalette[8] = 0xFF808080;
				mpPalette[9] = 0xFF800080;
				mpPalette[10] = 0xFF808000;
				mpPalette[11] = 0xFF008080;
				mpPalette[12] = 0xFF000080;
				mpPalette[13] = 0xFF008000;
				mpPalette[14] = 0xFF800000;
				mpPalette[15] = 0xFF000000;
				break;
			}
		case BITMAP_DEPTH_8BPP:
			{
				mPaletteSize = 256;
				mpPalette = GetAllocator()->NewArray<ARGB>( 256 );
				ARGB tempPalette[256] = {
                    0x00000000, 0x00800000, 0x00008000, 0x00808000, 0x00000080, 0x00800080, 0x00008080, 0x00C0C0C0,
                    0x00C0DCC0, 0x00A6CAF0, 0x00402000, 0x00602000, 0x00802000, 0x00A02000, 0x00C02000, 0x00E02000,
                    0x00004000, 0x00204000, 0x00404000, 0x00604000, 0x00804000, 0x00A04000, 0x00C04000, 0x00E04000,
                    0x00006000, 0x00206000, 0x00406000, 0x00606000, 0x00806000, 0x00A06000, 0x00C06000, 0x00E06000,
                    0x00008000, 0x00208000, 0x00408000, 0x00608000, 0x00808000, 0x00A08000, 0x00C08000, 0x00E08000,
                    0x0000A000, 0x0020A000, 0x0040A000, 0x0060A000, 0x0080A000, 0x00A0A000, 0x00C0A000, 0x00E0A000,
                    0x0000C000, 0x0020C000, 0x0040C000, 0x0060C000, 0x0080C000, 0x00A0C000, 0x00C0C000, 0x00E0C000,
                    0x0000E000, 0x0020E000, 0x0040E000, 0x0060E000, 0x0080E000, 0x00A0E000, 0x00C0E000, 0x00E0E000,
                    0x00000040, 0x00200040, 0x00400040, 0x00600040, 0x00800040, 0x00A00040, 0x00C00040, 0x00E00040,
                    0x00002040, 0x00202040, 0x00402040, 0x00602040, 0x00802040, 0x00A02040, 0x00C02040, 0x00E02040,
                    0x00004040, 0x00204040, 0x00404040, 0x00604040, 0x00804040, 0x00A04040, 0x00C04040, 0x00E04040,
                    0x00006040, 0x00206040, 0x00406040, 0x00606040, 0x00806040, 0x00A06040, 0x00C06040, 0x00E06040,
                    0x00008040, 0x00208040, 0x00408040, 0x00608040, 0x00808040, 0x00A08040, 0x00C08040, 0x00E08040,
                    0x0000A040, 0x0020A040, 0x0040A040, 0x0060A040, 0x0080A040, 0x00A0A040, 0x00C0A040, 0x00E0A040,
                    0x0000C040, 0x0020C040, 0x0040C040, 0x0060C040, 0x0080C040, 0x00A0C040, 0x00C0C040, 0x00E0C040,
                    0x0000E040, 0x0020E040, 0x0040E040, 0x0060E040, 0x0080E040, 0x00A0E040, 0x00C0E040, 0x00E0E040,
                    0x00000080, 0x00200080, 0x00400080, 0x00600080, 0x00800080, 0x00A00080, 0x00C00080, 0x00E00080,
                    0x00002080, 0x00202080, 0x00402080, 0x00602080, 0x00802080, 0x00A02080, 0x00C02080, 0x00E02080,
                    0x00004080, 0x00204080, 0x00404080, 0x00604080, 0x00804080, 0x00A04080, 0x00C04080, 0x00E04080,
                    0x00006080, 0x00206080, 0x00406080, 0x00606080, 0x00806080, 0x00A06080, 0x00C06080, 0x00E06080,
                    0x00008080, 0x00208080, 0x00408080, 0x00608080, 0x00808080, 0x00A08080, 0x00C08080, 0x00E08080,
                    0x0000A080, 0x0020A080, 0x0040A080, 0x0060A080, 0x0080A080, 0x00A0A080, 0x00C0A080, 0x00E0A080,
                    0x0000C080, 0x0020C080, 0x0040C080, 0x0060C080, 0x0080C080, 0x00A0C080, 0x00C0C080, 0x00E0C080,
                    0x0000E080, 0x0020E080, 0x0040E080, 0x0060E080, 0x0080E080, 0x00A0E080, 0x00C0E080, 0x00E0E080,
                    0x000000C0, 0x002000C0, 0x004000C0, 0x006000C0, 0x008000C0, 0x00A000C0, 0x00C000C0, 0x00E000C0,
                    0x000020C0, 0x002020C0, 0x004020C0, 0x006020C0, 0x008020C0, 0x00A020C0, 0x00C020C0, 0x00E020C0,
                    0x000040C0, 0x002040C0, 0x004040C0, 0x006040C0, 0x008040C0, 0x00A040C0, 0x00C040C0, 0x00E040C0,
                    0x000060C0, 0x002060C0, 0x004060C0, 0x006060C0, 0x008060C0, 0x00A060C0, 0x00C060C0, 0x00E060C0,
                    0x000080C0, 0x002080C0, 0x004080C0, 0x006080C0, 0x008080C0, 0x00A080C0, 0x00C080C0, 0x00E080C0,
                    0x0000A0C0, 0x0020A0C0, 0x0040A0C0, 0x0060A0C0, 0x0080A0C0, 0x00A0A0C0, 0x00C0A0C0, 0x00E0A0C0,
                    0x0000C0C0, 0x0020C0C0, 0x0040C0C0, 0x0060C0C0, 0x0080C0C0, 0x00A0C0C0, 0x00FFFBF0, 0x00A0A0A4,
                    0x00808080, 0x00FF0000, 0x0000FF00, 0x00FFFF00, 0x000000FF, 0x00FF00FF, 0x0000FFFF, 0x00FFFFFF };
				memcpy( mpPalette, tempPalette, mPaletteSize * sizeof(ARGB) );
				break;
			}
		case BITMAP_DEPTH_24BPP:
		case BITMAP_DEPTH_32BPP:
		default:
			{
				mpPalette = nullptr;
				mPaletteSize = 0;
				break;
			}
		}
	}
}

CHE_Palette::CHE_Palette( const CHE_Palette & palette ) : CHE_Object( palette.GetAllocator() )
{
	mPaletteSize = palette.mPaletteSize;
	if ( mPaletteSize > 0 && palette.mpPalette )
	{
		mpPalette = GetAllocator()->NewArray<ARGB>( mPaletteSize );
		memcpy( mpPalette, palette.mpPalette, mPaletteSize * sizeof(ARGB) );
	}else{
		mpPalette = nullptr;
	}
}

CHE_Palette::~CHE_Palette()
{
	if ( mpPalette )
	{
		GetAllocator()->DeleteArray<ARGB>( mpPalette );
		mpPalette = nullptr;
	}
}

CHE_Palette & CHE_Palette::operator=( const CHE_Palette& palette )
{
	mPaletteSize = palette.mPaletteSize;
	if ( mpPalette )
	{
		GetAllocator()->DeleteArray<ARGB>( mpPalette );
		mpPalette = nullptr;
	}
	if ( palette.mpPalette && mPaletteSize > 0 )
	{
		mpPalette = new ARGB[palette.mPaletteSize];
		memcpy( mpPalette, palette.mpPalette, palette.mPaletteSize * sizeof( ARGB ) );
	}
	return *this;
}

size_t CHE_Palette::GetCount() const
{
	return mPaletteSize;
}

bool CHE_Palette::GetNearColorIndex( ARGB color, size_t & indexRet ) const
{
	if ( mPaletteSize == 0 )
	{
		return false;
	}
	size_t index = 0;
	size_t lessCY = 0xFFFF;
	size_t tempLessCY = 0;
	ARGB srcColor;
	
	for ( size_t i = 0; i < mPaletteSize; i++ )
	{
		srcColor = mpPalette[i];
		tempLessCY =  ( color & 0x000000FF ) - ( srcColor & 0x000000FF );
		tempLessCY += ( color >> 8 & 0x000000FF ) - ( srcColor >> 8 & 0x000000FF );
		tempLessCY += ( color >> 16 & 0x000000FF ) - ( srcColor >> 16 & 0x000000FF );
		tempLessCY += ( color >> 24 & 0x000000FF ) - ( srcColor >> 24 & 0x000000FF );
		if ( tempLessCY < lessCY )
		{
			lessCY = tempLessCY;
			index = i;
		}
	}
	indexRet = index;
	return TRUE;
}

bool CHE_Palette::GetColor( size_t index, ARGB & colorRet ) const
{
	if (index >= mPaletteSize)
	{
		return false;
	}
	colorRet = mpPalette[index];
	return TRUE;
}

bool CHE_Palette::SetColor( size_t index, ARGB color )
{
	if (index >= mPaletteSize)
	{
		return false;
	}
	mpPalette[index] = color;
	return TRUE;
}

bool CHE_Palette::GetColorIndex( ARGB color, size_t & indexRet ) const
{
	if ( mPaletteSize == 0 )
	{
		return false;
	}

	for ( size_t i = 0; i < mPaletteSize; i++ )
	{
		if ( mpPalette[i] == color )
		{
			indexRet = i;
			return TRUE;
		}
	}
	return false;
}

bool CHE_Palette::IsColorExist( ARGB color ) const
{
	for ( size_t i = 0; i < mPaletteSize; i++)
	{
		if ( mpPalette[i] == color )
		{
			return TRUE;
		}
	}
	return false;
}


CHE_Bitmap::CHE_Bitmap( CHE_Allocator * pAllocator ) : CHE_Object( pAllocator ),
    mWidth(0), mHeight(0), mDepth(BITMAP_DEPTH_24BPP), mDirection(BITMAP_DIRECTION_UP),
    mCompression(BITMAP_COMPRESSION_RGB), mpPalette(nullptr), mpBits(nullptr) {}

CHE_Bitmap::CHE_Bitmap( const CHE_Bitmap & bitmap ) : CHE_Object( bitmap.GetAllocator() ),
    mWidth(bitmap.Width()), mHeight(bitmap.Height()), mDepth(bitmap.Depth()),
    mDirection(bitmap.Direction()), mCompression(bitmap.GetCompressionType()),
    mpPalette(nullptr), mpBits(nullptr)
{
	if ( bitmap.mpPalette != nullptr )
	{
		mpPalette = GetAllocator()->New<CHE_Palette>( *(bitmap.mpPalette) );
	}
	if ( bitmap.mpBits != nullptr )
	{
		mpBits = GetAllocator()->NewArray<BYTE>( bitmap.Pitch() * bitmap.Height() );
		memcpy( mpBits, bitmap.mpBits, bitmap.Pitch() * bitmap.Height() );
	}
}

CHE_Bitmap & CHE_Bitmap::operator =( const CHE_Bitmap & bitmap )
{
	if ( this == &bitmap )
	{
		return *this;
	}
	mWidth = bitmap.mWidth;
	mHeight = bitmap.mHeight;
	mDepth = bitmap.mDepth;
	mDirection = bitmap.mDirection;
	mCompression = bitmap.mCompression;
	if ( bitmap.mpPalette != nullptr )
	{
		mpPalette = GetAllocator()->New<CHE_Palette>( *(bitmap.mpPalette) );
	}
	if ( bitmap.mpBits != nullptr )
	{
		mpBits = GetAllocator()->NewArray<BYTE>( bitmap.Pitch() * bitmap.Height() );
		memcpy( mpBits, bitmap.mpBits, bitmap.Pitch() * bitmap.Height() );
	}
	return *this;
}

CHE_Bitmap::~CHE_Bitmap()
{
	if ( mpBits )
	{
		GetAllocator()->DeleteArray( mpBits );
		mpBits = nullptr;
	}
	if ( mpPalette )
	{
		GetAllocator()->Delete<CHE_Palette>( mpPalette );
		mpPalette = nullptr;
		mpBits = nullptr;
	}
}

bool CHE_Bitmap::Load( char const * filePath )
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
	HE_BITMAPFILEHEADER bfHeader;
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

	HE_BITMAPINFOHEADER biHeader;
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
	size_t platteSize = 0;
	ARGB* pPalette = nullptr;
	PBYTE pBits = nullptr;

	fseek( pfile, 0, SEEK_END );
	fileSize = ftell( pfile );
	if ( fileSize < biHeader.biSizeImage )
	{
		fclose( pfile );
		return false;
	}
	fileSize = biHeader.biSizeImage;
	if ( biHeader.biBitCount == 8 )
	{
		platteSize = 256*4;
	}else if ( biHeader.biBitCount == 4 )
	{
		platteSize = 16*4;
	}else if ( biHeader.biBitCount == 1 )
	{
		platteSize = 2*4;
	}

	if ( platteSize > 0 )
	{
		pPalette = GetAllocator()->NewArray<ARGB>( platteSize );
		fseek( pfile, 54, SEEK_SET );
		bytesReaded = fread( pPalette, 1, platteSize, pfile );
		if ( bytesReaded != platteSize )
		{
			GetAllocator()->DeleteArray( pPalette );
			pPalette = nullptr;
			fclose( pfile );
			return false;
		}
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
	mDepth = (HE_BITMAP_DEPTH)(biHeader.biBitCount);
	mCompression = (HE_BITMAP_COMPRESSION)(biHeader.biCompression);
	mDirection = (biHeader.biHeight > 0) ? BITMAP_DIRECTION_UP : BITMAP_DIRECTION_DOWN;
	mpBits = pBits;

	switch ( biHeader.biBitCount )
	{
	case 1:
		mDepth = BITMAP_DEPTH_1BPP;
		break;
	case 4:
		mDepth = BITMAP_DEPTH_4BPP;
		break;
	case 8:
		mDepth = BITMAP_DEPTH_8BPP;
		break;
	case 16:
		mDepth = BITMAP_DEPTH_16BPP;
		break;
	case 24:
		mDepth = BITMAP_DEPTH_24BPP;
		break;
	case 32:
		mDepth = BITMAP_DEPTH_32BPP;
		break;
	default:
		mDepth = BITMAP_DEPTH_24BPP;
	}
	mpPalette = GetAllocator()->New<CHE_Palette>( mDepth, pPalette, GetAllocator() );

	if ( biHeader.biHeight > 0 )
	{
		mDirection = BITMAP_DIRECTION_UP;
	}else
	{
		mDirection = BITMAP_DIRECTION_DOWN;
	}
	return TRUE;
}

bool CHE_Bitmap::Save( char const * filePath )
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
	HE_BITMAPFILEHEADER bfHeader;
	bfHeader.bfSize = (uint32)( 54 + mpPalette->GetCount() * 4 + Pitch() * Height() );
	bfHeader.bfType = 0x4D42;
	bfHeader.bfReserved2 = 0;
	bfHeader.bfReserved1 = 0;
	bfHeader.bfOffBits = (uint32)( 54 + mpPalette->GetCount() * 4 );

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
	HE_BITMAPINFOHEADER biHeader;
	biHeader.biSize = 40;
	biHeader.biWidth = (int32)mWidth;
	size_t lHeight = mHeight;
	biHeader.biHeight = ( mDirection == BITMAP_DIRECTION_UP ) ? (int32)(lHeight) : (int32)(-lHeight);
	biHeader.biPlanes = 1;
	biHeader.biBitCount = mDepth;
	biHeader.biCompression = mCompression;
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

	if ( mpPalette->GetCount() > 0 )
	{
		lBytesReaded = fwrite( mpPalette->mpPalette, 1, mpPalette->GetCount() * 4, pfile );
		if ( lBytesReaded >> 2 != mpPalette->GetCount() )
		{
			fclose( pfile );
			return false;
		}
	}
	lBytesReaded = fwrite( mpBits, 1, Pitch() * Height(), pfile );
	if ( lBytesReaded != Pitch() * Height()  )
	{
		fclose( pfile );
		return false;
	}
	fclose( pfile );
	return TRUE;
}

bool CHE_Bitmap::SaveToMem( PBYTE buffer, size_t size )
{
	if ( buffer == nullptr || size < GetMemBitmapDataSize() + 14 )
	{
		return false;
	}

	size_t index = 0;

	HE_BITMAPFILEHEADER bfHeader;
	bfHeader.bfSize = (uint32)( 54 + mpPalette->GetCount() * 4 + Pitch() * Height() );
	bfHeader.bfType = 0x4D42;
	bfHeader.bfReserved2 = 0;
	bfHeader.bfReserved1 = 0;
	bfHeader.bfOffBits = (uint32)( 54 + mpPalette->GetCount() * 4 );

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

	HE_BITMAPINFOHEADER biHeader;
	biHeader.biSize = 40;
	biHeader.biWidth = (int32)mWidth;
	size_t lHeight = mHeight;
	biHeader.biHeight = ( mDirection == BITMAP_DIRECTION_UP ) ? (int32)(lHeight) : (int32)(-lHeight);
	biHeader.biPlanes = 1;
	biHeader.biBitCount = mDepth;
	biHeader.biCompression = mCompression;
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

	if ( mpPalette && mpPalette->GetCount() > 0 )
	{
		memcpy( buffer + index, mpPalette->mpPalette, mpPalette->GetCount() * 4 );
		index += mpPalette->GetCount() * 4;
	}
	memcpy( buffer + index, mpBits, Pitch() * Height() );
	return TRUE;
}

size_t CHE_Bitmap::GetMemBitmapDataSize()
{
	size_t dwRet = 0;

	dwRet += 40; //bitmapInfo size;

	if ( mpPalette &&  mpPalette->GetCount() > 0 )
	{
		dwRet += mpPalette->GetCount() * 4;
	}

	dwRet += Pitch() * Height();

	return dwRet;
}

bool CHE_Bitmap::GetMemBitmapData( PBYTE buffer, size_t size )
{
	if ( buffer == nullptr || size < GetMemBitmapDataSize() )
	{
		return false;
	}

	size_t index = 0;

	HE_BITMAPINFOHEADER biHeader;
	biHeader.biSize = 40;
	biHeader.biWidth = (int32)mWidth;
	size_t lHeight = mHeight;
	biHeader.biHeight = ( mDirection == BITMAP_DIRECTION_UP ) ? (int32)(lHeight) : (int32)(-lHeight);
	biHeader.biPlanes = 1;
	biHeader.biBitCount = mDepth;
	biHeader.biCompression = mCompression;
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

	if ( mpPalette && mpPalette->GetCount() > 0 )
	{
		memcpy( buffer + index, mpPalette->mpPalette, mpPalette->GetCount() * 4 );
		index += mpPalette->GetCount() * 4;
	}
	memcpy( buffer + index, mpBits, Pitch() * Height() );
	return TRUE;
}

bool CHE_Bitmap::Create( size_t width, size_t height, HE_BITMAP_DEPTH depth, HE_BITMAP_DIRECTION direction, size_t bufferSize, PCBYTE buffer, CHE_Palette* pPalette )
{
	size_t bitcount = 0, imageSize = 0;
	switch ( depth )
	{
	case BITMAP_DEPTH_1BPP:
		{
			bitcount = 1;
			imageSize = ( ( ( width + 31 ) & ~31 ) >> 3 ) * height ;
			break;
		}
	case BITMAP_DEPTH_4BPP:
		{
			bitcount = 4;
			imageSize = ( ( ( width * 4 + 31 ) & ~31 ) >> 3 ) * height;
			break;
		}
	case BITMAP_DEPTH_8BPP:
		{
			bitcount = 8;
			imageSize = ( ( ( width * 8 + 31 ) & ~31 ) >> 3 ) * height;
			break;
		}
	case BITMAP_DEPTH_24BPP:
		{
			bitcount = 24;
			imageSize = ( ( ( width * 24 + 31 ) & ~31 ) >> 3 ) * height;
			break;
		}
	case BITMAP_DEPTH_32BPP:
		{
			bitcount = 32;
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
	mCompression = BITMAP_COMPRESSION_RGB;
	mpBits = GetAllocator()->NewArray<BYTE>( imageSize );
	memset( mpBits, 0, imageSize );
	if ( pPalette == nullptr )
	{
		mpPalette = GetAllocator()->New<CHE_Palette>( depth, (const ARGB * const)nullptr, GetAllocator() );
	}else{
		mpPalette = GetAllocator()->New<CHE_Palette>( *pPalette );
	}
	if ( bufferSize != 0 && buffer != nullptr )
	{
		if ( bufferSize > imageSize )
		{
			bufferSize = imageSize;	
		}
		memcpy( mpBits, buffer, bufferSize );
	}else{
		Fill( 0xFFFFFFFF, nullptr );
	}
	return TRUE;
}

void CHE_Bitmap::Clean()
{
	if ( mpBits )
	{
		GetAllocator()->DeleteArray( mpBits );
		mpBits = nullptr;
	}
	if ( mpPalette )
	{
		GetAllocator()->Delete<CHE_Palette>( mpPalette );
		mpPalette = nullptr;
	}
	mWidth = 0;
	mHeight = 0;
	mDepth = BITMAP_DEPTH_24BPP;
	mDirection = BITMAP_DIRECTION_UP;
	mCompression = BITMAP_COMPRESSION_RGB;
}

bool CHE_Bitmap::GetPixelColor( size_t x, size_t y, ARGB & clrRet ) const
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
	BYTE platteIndex = 0;
	if ( Depth() == 32 )
	{
		clrRet = mpBits[index] + ( mpBits[index+1] << 8 ) + ( mpBits[index+2] << 16 ) + ( mpBits[index+3] << 24 );
	}else if ( Depth() == 24 )
	{
		clrRet = mpBits[index] + ( mpBits[index+1] << 8 ) + ( mpBits[index+2] << 16 );
	}else if ( Depth() == 8 )
	{
		platteIndex = mpBits[index];
		clrRet = mpPalette->mpPalette[platteIndex];
	}else if ( Depth() == 4)
	{
		platteIndex = mpBits[index];
		if ( x % 2 == 0 )
		{
			platteIndex &= 0xF0;
		}else{
			platteIndex &= 0x0F;
		}
		clrRet = mpPalette->mpPalette[platteIndex];
	}else if ( Depth() == 1 )
	{
		platteIndex = mpBits[index];
		switch ( x % 8 )
		{
		case 0:
			platteIndex &= 0x80;
			break;
		case 1:
			platteIndex &= 0x40;
			break;
		case 2:
			platteIndex &= 0x20;
			break;
		case 3:
			platteIndex &= 0x10;
			break;
		case 4:
			platteIndex &= 0x8;
			break;
		case 5:
			platteIndex &= 0x4;
			break;
		case 6:
			platteIndex &= 0x2;
			break;
		case 7:
			platteIndex &= 0x1;
			break;
		default:
			break;
		}
		clrRet = mpPalette->mpPalette[ ( platteIndex? 1 : 0 ) ];
	}
	return TRUE;
}

bool CHE_Bitmap::SetPixelColor( size_t x, size_t y, ARGB color )
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
	BYTE platteIndex = 0;
	size_t tempIndex = 0;;
	if ( Depth() == 32 )
	{
		mpBits[index] = (BYTE)( color & 0xFF );
		mpBits[index+1] = (BYTE)( ( color >> 8 ) & 0xFF );
		mpBits[index+2] = (BYTE)( ( color >> 16 ) & 0xFF );
		mpBits[index+3] = (BYTE)( ( color >> 24 ) & 0xFF );
	}else if ( Depth() == 24 )
	{
		mpBits[index] = (BYTE)( color & 0xFF );
		mpBits[index+1] = (BYTE)( ( color >> 8 ) & 0xFF );
		mpBits[index+2] = (BYTE)( ( color >> 16 ) & 0xFF );
	}else if ( Depth() == 8 )
	{
		if ( mpPalette->GetNearColorIndex( color, tempIndex ) )
		{
			mpBits[index] = (BYTE)tempIndex;
		}
		
	}else if ( Depth() == 4 )
	{
		platteIndex = mpBits[index];
		mpPalette->GetNearColorIndex( color, tempIndex );
		if ( x % 2 == 0 )
		{
			tempIndex <<= 4;
			platteIndex &= 0x0F;
		}else{
			platteIndex &= 0xF0;
		}
		mpBits[index] = (BYTE)tempIndex + platteIndex;
	}else if ( Depth() == 1 )
	{
		platteIndex = mpBits[index];
		mpPalette->GetNearColorIndex( color, tempIndex );
		switch ( x % 8 )
		{
		case 0:
			platteIndex &= 0x7F;
			tempIndex <<= 7;
			tempIndex &= 0x80;
			break;
		case 1:
			platteIndex &= 0xBF;
			tempIndex <<= 6;
			tempIndex &= 0x40;
			break;
		case 2:
			platteIndex &= 0xDF;
			tempIndex <<= 5;
			tempIndex &= 0x20;
			break;
		case 3:
			platteIndex &= 0xEF;
			tempIndex <<= 4;
			tempIndex &= 0x10;
			break;
		case 4:
			platteIndex &= 0xF7;
			tempIndex <<= 3;
			tempIndex &= 0x08;
			break;
		case 5:
			platteIndex &= 0xFB;
			tempIndex <<= 2;
			tempIndex &= 0x04;
			break;
		case 6:
			platteIndex &= 0xFD;
			tempIndex <<= 1;
			tempIndex &= 0x02;
			break;
		case 7:
			platteIndex &= 0xFE;
			tempIndex &= 0x01;
			break;
		default:
			break;
		}
		mpBits[index] = (BYTE)tempIndex + platteIndex;
	}
	return TRUE;
}

bool CHE_Bitmap::SetPixelColor( size_t x, size_t y, std::vector<ARGB> & colors )
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
	BYTE platteIndex = 0;
	size_t tempIndex = 0;
	ARGB color;
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
			mpBits[index] = (BYTE)( color & 0xFF );
			mpBits[index+1] = (BYTE)( ( color >> 8 ) & 0xFF );
			mpBits[index+2] = (BYTE)( ( color >> 16 ) & 0xFF );
			mpBits[index+3] = (BYTE)( ( color >> 24 ) & 0xFF );
		}else if ( Depth() == 24 )
		{
			mpBits[index] = (BYTE)( color & 0xFF );
			mpBits[index+1] = (BYTE)( ( color >> 8 ) & 0xFF );
			mpBits[index+2] = (BYTE)( ( color >> 16 ) & 0xFF );
		}else if ( Depth() == 8 )
		{
			if ( mpPalette->GetNearColorIndex( color, tempIndex ) )
			{
				mpBits[index] = (BYTE)tempIndex;
			}

		}else if ( Depth() == 4 )
		{
			platteIndex = mpBits[index];
			mpPalette->GetNearColorIndex( color, tempIndex );
			if ( x % 2 == 0 )
			{
				tempIndex <<= 4;
				platteIndex &= 0x0F;
			}else{
				platteIndex &= 0xF0;
			}
			mpBits[index] = (BYTE)tempIndex + platteIndex;
		}else if ( Depth() == 1 )
		{
			platteIndex = mpBits[index];
			mpPalette->GetNearColorIndex( color, tempIndex );
			switch ( x % 8 )
			{
			case 0:
				platteIndex &= 0x7F;
				tempIndex <<= 7;
				tempIndex &= 0x80;
				break;
			case 1:
				platteIndex &= 0xBF;
				tempIndex <<= 6;
				tempIndex &= 0x40;
				break;
			case 2:
				platteIndex &= 0xDF;
				tempIndex <<= 5;
				tempIndex &= 0x20;
				break;
			case 3:
				platteIndex &= 0xEF;
				tempIndex <<= 4;
				tempIndex &= 0x10;
				break;
			case 4:
				platteIndex &= 0xF7;
				tempIndex <<= 3;
				tempIndex &= 0x08;
				break;
			case 5:
				platteIndex &= 0xFB;
				tempIndex <<= 2;
				tempIndex &= 0x04;
				break;
			case 6:
				platteIndex &= 0xFD;
				tempIndex <<= 1;
				tempIndex &= 0x02;
				break;
			case 7:
				platteIndex &= 0xFE;
				tempIndex &= 0x01;
				break;
			default:
				break;
			}
			mpBits[index] = (BYTE)tempIndex + platteIndex;
		}
	}
	return TRUE;
}

bool CHE_Bitmap::SetPixelColor( size_t x, size_t y, ARGB * pColors, size_t count )
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
	BYTE platteIndex = 0;
	size_t tempIndex = 0;
	ARGB color;
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
            mpBits[index] = (BYTE)( color & 0xFF );
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
            mpBits[index] = (BYTE)( color & 0xFF );
            mpBits[index+1] = (BYTE)( ( color >> 8 ) & 0xFF );
            mpBits[index+2] = (BYTE)( ( color >> 16 ) & 0xFF );
        }
    }else if ( Depth() == 8 )
    {
        for ( size_t i = 0; i < count; ++i )
        {
            index = GetPixelByteIndex( x++, y );
            if ( x == mWidth )
            {
                x = 0;
                y += 1;
            }
            color = *(pColors+i);
            if ( mpPalette->GetNearColorIndex( color, tempIndex ) )
            {
                mpBits[index] = (BYTE)tempIndex;
            }
        }
    }else if (  Depth() == 4 )
    {
        for ( size_t i = 0; i < count; ++i )
        {
            index = GetPixelByteIndex( x++, y );
            if ( x == mWidth )
            {
                x = 0;
                y += 1;
            }
            color = *(pColors+i);

            platteIndex = mpBits[index];
            mpPalette->GetNearColorIndex( color, tempIndex );
            if ( x % 2 == 0 )
            {
                tempIndex <<= 4;
                platteIndex &= 0x0F;
            }else{
                platteIndex &= 0xF0;
            }
            mpBits[index] = (BYTE)tempIndex + platteIndex;
        }
    }else if ( Depth() == 1 )
    {
        for ( size_t i = 0; i < count; ++i )
        {
            index = GetPixelByteIndex( x++, y );
            if ( x == mWidth )
            {
                x = 0;
                y += 1;
            }
            color = *(pColors+i);
            
            platteIndex = mpBits[index];
            mpPalette->GetNearColorIndex( color, tempIndex );
            switch ( x % 8 )
            {
            case 0:
                platteIndex &= 0x7F;
                tempIndex <<= 7;
                tempIndex &= 0x80;
                break;
            case 1:
                platteIndex &= 0xBF;
                tempIndex <<= 6;
                tempIndex &= 0x40;
                break;
            case 2:
                platteIndex &= 0xDF;
                tempIndex <<= 5;
                tempIndex &= 0x20;
                break;
            case 3:
                platteIndex &= 0xEF;
                tempIndex <<= 4;
                tempIndex &= 0x10;
                break;
            case 4:
                platteIndex &= 0xF7;
                tempIndex <<= 3;
                tempIndex &= 0x08;
                break;
            case 5:
                platteIndex &= 0xFB;
                tempIndex <<= 2;
                tempIndex &= 0x04;
                break;
            case 6:
                platteIndex &= 0xFD;
                tempIndex <<= 1;
                tempIndex &= 0x02;
                break;
            case 7:
                platteIndex &= 0xFE;
                tempIndex &= 0x01;
                break;
            default:
                break;
            }
            mpBits[index] = (BYTE)tempIndex + platteIndex;
        }
    }
	return TRUE;
}

bool	CHE_Bitmap::GetPixelIndex( size_t x, size_t y, BYTE & indexRet ) const
{
	if ( x >= Width() )
	{
		return false;
	}
	if ( y >= Height() )
	{
		return false;
	}
	if ( Depth() > 8 )
	{
		return false;
	}
	
	size_t index = GetPixelByteIndex( x, y );
	if ( Depth() == 8 )
	{
		indexRet = mpBits[index];
	}else if ( Depth() == 4)
	{
		indexRet = mpBits[index];
		if ( x % 2 == 0 )
		{
			indexRet &= 0xF0;
		}else{
			indexRet &= 0x0F;
		}
	}else if ( Depth() == 1 )
	{
		indexRet = mpBits[index];
		switch ( x % 8 )
		{
		case 0:
			indexRet &= 0x80;
			break;
		case 1:
			indexRet &= 0x40;
			break;
		case 2:
			indexRet &= 0x20;
			break;
		case 3:
			indexRet &= 0x10;
			break;
		case 4:
			indexRet &= 0x8;
			break;
		case 5:
			indexRet &= 0x4;
			break;
		case 6:
			indexRet &= 0x2;
			break;
		case 7:
			indexRet &= 0x1;
			break;
		default:
			break;
		}
	}
	return TRUE;
}

bool	CHE_Bitmap::SetPixelIndex( size_t x, size_t y, BYTE clrIndex )
{
	if ( x >= Width() )
	{
		return false;
	}
	if ( y >= Height() )
	{
		return false;
	}
	if ( Depth() > 8 )
	{
		return false;
	}

	size_t index = GetPixelByteIndex( x, y );
	BYTE platteIndex = 0;
	if ( Depth() == 8 )
	{
		mpBits[index] = clrIndex;
	}else if ( Depth() == 4 )
	{
		platteIndex = mpBits[index];
		if ( x % 2 == 0 )
		{
			clrIndex <<= 4;
			platteIndex &= 0x0F;
		}else{
			platteIndex &= 0xF0;
		}
		mpBits[index] = clrIndex + platteIndex;
	}else if ( Depth() == 1 )
	{
		platteIndex = mpBits[index];
		switch ( x % 8 )
		{
		case 0:
			platteIndex &= 0x7F;
			clrIndex <<= 7;
			clrIndex &= 0x80;
			break;
		case 1:
			platteIndex &= 0xBF;
			clrIndex <<= 6;
			clrIndex &= 0x40;
			break;
		case 2:
			platteIndex &= 0xDF;
			clrIndex <<= 5;
			clrIndex &= 0x20;
			break;
		case 3:
			platteIndex &= 0xEF;
			clrIndex <<= 4;
			clrIndex &= 0x10;
			break;
		case 4:
			platteIndex &= 0xF7;
			clrIndex <<= 3;
			clrIndex &= 0x08;
			break;
		case 5:
			platteIndex &= 0xFB;
			clrIndex <<= 2;
			clrIndex &= 0x04;
			break;
		case 6:
			platteIndex &= 0xFD;
			clrIndex <<= 1;
			clrIndex &= 0x02;
			break;
		case 7:
			platteIndex &= 0xFE;
			clrIndex &= 0x01;
			break;
		default:
			break;
		}
		mpBits[index] = clrIndex + platteIndex;
	}
	return TRUE;
}

bool	CHE_Bitmap::Fill( ARGB color )
{
	if ( mpBits == nullptr )
	{
		return false;
	}

	size_t index;
	if ( mpPalette->GetNearColorIndex( color, index ) )
	{
		mpPalette->GetColor( (BYTE)index, color );
	}

	switch ( Depth() )
	{
	case 1:
		{
			if ( index == 0 )
			{
				for ( size_t i = 0; i < Pitch() * Height(); i++ )
				{
					mpBits[i] = 0x00;
				}
			}else{
				for ( size_t i = 0; i < Pitch() * Height(); i++ )
				{
					mpBits[i] = 0xFF;
				}
			}
			break;
		}
	case 4:
		{
			for ( size_t i = 0; i < Pitch() * Height(); i++ )
			{
				index = ( index << 4 ) + ( index & 0x0F );
				mpBits[i] = (BYTE)index;
			}
			break;
		}
	case 8:
		{
			for ( size_t i = 0; i < Pitch() * Height(); i++ )
			{
				mpBits[i] = (BYTE)(index & 0xFF );
			}
			break;
		}
	case 24:
		{
			for ( size_t i = 0; i+2 < Pitch() * Height(); i+=3 )
			{
				mpBits[i] = (BYTE)( color & 0xFF );
				mpBits[i+1] = (BYTE)( ( color >> 8 ) & 0xFF );
				mpBits[i+2] = (BYTE)( ( color >> 16 ) & 0xFF );
			}
			break;
		}
	case 32:
		{
			for ( size_t i = 0; i+3 < Pitch() * Height(); i+=4 )
			{
				*((uint32*)(mpBits+i)) = color;
			}
			break;
		}
	default:
		return false;
	}
	return TRUE;
}

bool	CHE_Bitmap::Fill( ARGB color, const BITMAP_RECT* pRect )
{
	BITMAP_RECT rect;
	if ( pRect == nullptr )
	{
		Fill( color );
		return TRUE;
	}else{
		rect.top = pRect->top;
		rect.left = pRect->left;
		rect.width = pRect->width;
		rect.height = pRect->height;
	}
	if ( rect.height <= 0 || rect.width <= 0 )
	{
		return false;
	}
	//if ( rect.top < 0 )
	//{
	//	rect.height += rect.top;
	//	rect.top = 0;
	//}
	//if ( rect.left < 0 )
	//{
	//	rect.width += rect.left;
	//	rect.left = 0;
	//}
	
	for ( size_t i = rect.top; i < rect.height + rect.top; i++ )
	{
		DrawLine( i, rect.left, rect.width, color );
	}
	return TRUE;
}

void CHE_Bitmap::DrawLine( size_t nLine, size_t nStart, size_t nLength, ARGB color )
{ 
	if ( mpBits == nullptr || nLine >= Height() || nStart >= Width() || nLength == 0 )
	{
		return;
	}

	if ( nStart + nLength > Width() )
	{
		nLength = Width() - nStart;
	}
	
	size_t indexB = 0, indexE = 0;
	indexB = GetPixelByteIndex( nStart, nLine );
	indexE = GetPixelByteIndex( nStart + nLength, nLine );
	
	size_t indexColor;
	if ( mpPalette->GetNearColorIndex( color, indexColor ) )
	{
		mpPalette->GetColor( (BYTE)indexColor, color );
	}
	
	switch ( Depth() )
	{
	case 1:
		{
			size_t index = 0;
			BYTE oriByte = 0;
			BYTE newByte = 0;
			for ( size_t x = nStart; x < nStart + nLength; x++ )
			{
				index = GetPixelByteIndex( x, nLine );
				oriByte = mpBits[index];
				newByte = (BYTE)indexColor;
				switch ( x % 8 )
				{
				case 0:
					oriByte &= 0x7F;
					newByte &= 0x80;
					break;
				case 1:
					oriByte &= 0xBF;
					newByte &= 0x40;
					break;
				case 2:
					oriByte &= 0xDF;
					newByte &= 0x20;
					break;
				case 3:
					oriByte &= 0xEF;
					newByte &= 0x10;
					break;
				case 4:
					oriByte &= 0xF7;
					newByte &= 0x08;
					break;
				case 5:
					oriByte &= 0xFB;
					newByte &= 0x04;
					break;
				case 6:
					oriByte &= 0xFD;
					newByte &= 0x02;
					break;
				case 7:
					oriByte &= 0xFE;
					newByte &= 0x01;
					break;
				default:
					break;
				}
				mpBits[index] = newByte | oriByte;
			}
			break;
		}
	case 4:
		{
			size_t index = 0;
			BYTE oriByte = 0;
			BYTE desByte = 0;
			for ( size_t x = nStart; x < nStart + nLength; x++ )
			{
				index = GetPixelByteIndex( x, nLine );
				oriByte = mpBits[index];
				desByte = (BYTE)indexColor;
				if ( x % 2 == 0 )
				{
					desByte <<= 4;
					oriByte &= 0x0F;
				}else{
					oriByte &= 0xF0;
				}
				mpBits[index] = desByte | oriByte;
			}
			break;
		}
	case 8:
		{
			for ( size_t i = indexB; i < indexE; i++ )
			{
				mpBits[i] = (BYTE)indexColor;
			}
			break;
		}
	case 24:
		{
			for ( size_t i = indexB; i < indexE; i+=3 )
			{
				mpBits[i] = (BYTE)( color & 0xFF );
				mpBits[i+1] = (BYTE)( ( color >> 8 ) & 0xFF );
				mpBits[i+2] = (BYTE)( ( color >> 16 ) & 0xFF );
			}
			break;
		}
	case 32:
		{
			for ( size_t i = indexB; i < indexE; i+=4 )
			{
				*((uint32*)(mpBits+i)) = color;
			}
			break;
		}
	default:
		return;
	}
}

void CHE_Bitmap::DrawLine( size_t nLine, size_t nStart, size_t nLength, PBYTE lpDataBuf, size_t nBufSize )
{ 
	if ( mpBits == nullptr || nLine >= Height() || nStart >= Width() || nLength == 0 || lpDataBuf == nullptr || nBufSize == 0 )
	{
		return;
	}

	if ( nStart + nLength > Width() )
	{
		nLength = Width() - nStart;
	}

	if ( ( nLength * Depth()) > (nBufSize << 3) )
	{
		nLength = ( nBufSize << 3 ) / Depth();
	}

	size_t indexB = 0, indexE = 0;
	indexB = GetPixelByteIndex( nStart, nLine );
	indexE = GetPixelByteIndex( nStart + nLength, nLine );
	
	switch ( Depth() )
	{
	case 1:
		{
			size_t index = 0;
			BYTE oriByte = 0;
			BYTE newByte = 0;

			for ( size_t x = nStart; x < nStart + nLength; x++ )
			{
				index = GetPixelByteIndex( x, nLine );
				oriByte = mpBits[index];
				newByte = lpDataBuf[index-indexB];
				switch ( x % 8 )
				{
				case 0:
					oriByte &= 0x7F;
					newByte &= 0x80;
					break;
				case 1:
					oriByte &= 0xBF;
					newByte &= 0x40;
					break;
				case 2:
					oriByte &= 0xDF;
					newByte &= 0x20;
					break;
				case 3:
					oriByte &= 0xEF;
					newByte &= 0x10;
					break;
				case 4:
					oriByte &= 0xF7;
					newByte &= 0x08;
					break;
				case 5:
					oriByte &= 0xFB;
					newByte &= 0x04;
					break;
				case 6:
					oriByte &= 0xFD;
					newByte &= 0x02;
					break;
				case 7:
					oriByte &= 0xFE;
					newByte &= 0x01;
					break;
				default:
					break;
				}
				mpBits[index] = newByte | oriByte;
			}
			break;
		}
	case 4:
		{
			size_t index = 0;
			BYTE oriByte = 0;
			BYTE desByte = 0;
			for ( size_t x = nStart; x < nStart + nLength; x++ )
			{
				index = GetPixelByteIndex( x, nLine );
				desByte = mpBits[index];
				oriByte = lpDataBuf[index-indexB];
				if ( x % 2 == 0 )
				{
					desByte &= 0x0F;
					oriByte &= 0xF0;
				}else{
					desByte &= 0xF0;
					oriByte &= 0x0F; 
				}
				mpBits[index] = desByte | oriByte;
			}
			break;
		}
	case 8:
		{
			for ( size_t i = indexB, j = 0; i < indexE; i++, j++ )
			{
				mpBits[i] = lpDataBuf[j];
			}
			break;
		}
	case 24:
		{
			for ( size_t i = indexB, j = 0; i < indexE; i+=3, j+=3 )
			{
				mpBits[i] = lpDataBuf[j];
				mpBits[i+1] = lpDataBuf[j+1];
				mpBits[i+2] = lpDataBuf[j+2];
			}
			break;
		}
	case 32:
		{
			for ( size_t i = indexB, j = 0; i < indexE; i+=4, j+=4 )
			{
				*((uint32*)(mpBits+i)) = *((uint32*)(lpDataBuf+j));
			}
			break;
		}
	default:
		return;
	}
}

bool CHE_Bitmap::SetChannel( HE_BITMAP_CHANNEL channel, BYTE value )
{
	if ( ( Depth() < 24 ) || ( Depth() == 24 && channel == BITMAP_CHANNEL_Alpha ) )
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
	case BITMAP_CHANNEL_Alpha:
		iByteOffset = 3;
		break;
	case BITMAP_CHANNEL_Red:
		iByteOffset = 2;
		break;
	case BITMAP_CHANNEL_Green:
		iByteOffset = 1;
		break;
	case BITMAP_CHANNEL_Blue:
		iByteOffset = 0;
		break;
	default:
		return false;
	}

	size_t iByteIndex = 0;
	size_t iPixelByteCount = Depth()>>3;
	for ( size_t iY = 0; iY < Height(); iY++ )
	{
		iByteIndex = GetPixelByteIndex( 0, iY );
		for ( size_t iX = 0; iX < Width(); iX++ )
		{
			mpBits[iByteIndex + ( iX * iPixelByteCount ) + iByteOffset] = value;
		}
	}
	return TRUE;
}

bool CHE_Bitmap::SetChannelByAlpha( HE_BITMAP_CHANNEL channel, BYTE alpha )
{
	if ( ( Depth() < 24 ) || ( Depth() == 24 && channel == BITMAP_CHANNEL_Alpha ) )
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
	case BITMAP_CHANNEL_Alpha:
		iByteOffset = 3;
		break;
	case BITMAP_CHANNEL_Red:
		iByteOffset = 2;
		break;
	case BITMAP_CHANNEL_Green:
		iByteOffset = 1;
		break;
	case BITMAP_CHANNEL_Blue:
		iByteOffset = 0;
		break;
	default:
		return false;
	}
	
	size_t iByteIndex = 0;
	size_t iPixelByteCount = Depth()>>3;
	FLOAT fTemp = 0.0;
	FLOAT fAlpha = alpha;
	for ( size_t iY = 0; iY < Height(); iY++ )
	{
		iByteIndex = GetPixelByteIndex( 0, iY );
		for ( size_t iX = 0; iX < Width(); iX++ )
		{
			fTemp = mpBits[iByteIndex + ( iX * iPixelByteCount ) + iByteOffset];
			mpBits[iByteIndex + ( iX * iPixelByteCount ) + iByteOffset] = (unsigned char)(fTemp*(fAlpha/255)) ;
		}
	}
	return TRUE;
}

bool CHE_Bitmap::ExchangeChannel( HE_BITMAP_CHANNEL channel1, HE_BITMAP_CHANNEL channel2 )
{
	if ( ( Depth() < 24 ) || 
		 ( Depth() == 24 && ( channel1 == BITMAP_CHANNEL_Alpha || channel2 == BITMAP_CHANNEL_Alpha ) ) ||
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
	case BITMAP_CHANNEL_Alpha:
		iByteOffset1 = 3;
		break;
	case BITMAP_CHANNEL_Red:
		iByteOffset1 = 2;
		break;
	case BITMAP_CHANNEL_Green:
		iByteOffset1 = 1;
		break;
	case BITMAP_CHANNEL_Blue:
		iByteOffset1 = 0;
		break;
	default:
		return false;
	}

	switch ( channel2 )
	{
	case BITMAP_CHANNEL_Alpha:
		iByteOffset2 = 3;
		break;
	case BITMAP_CHANNEL_Red:
		iByteOffset2 = 2;
		break;
	case BITMAP_CHANNEL_Green:
		iByteOffset2 = 1;
		break;
	case BITMAP_CHANNEL_Blue:
		iByteOffset2 = 0;
		break;
	default:
		return false;
	}
	
	size_t iByteIndex = 0;
	size_t iPixelByteCount = Depth()>>3;
	BYTE iTemp = 0;
	for ( size_t iY = 0; iY < Height(); iY++ )
	{
		iByteIndex = GetPixelByteIndex( 0, iY );
		for ( size_t iX = 0; iX < Width(); iX++ )
		{
			iTemp = mpBits[iByteIndex + ( iX * iPixelByteCount ) + iByteOffset2];
			mpBits[iByteIndex + ( iX * iPixelByteCount ) + iByteOffset2] \
				= mpBits[iByteIndex + ( iX * iPixelByteCount ) + iByteOffset1];
			mpBits[iByteIndex + ( iX * iPixelByteCount ) + iByteOffset1] = iTemp;
		}
	}
	return TRUE;
}

bool CHE_Bitmap::CopyChannel( HE_BITMAP_CHANNEL channelDes, HE_BITMAP_CHANNEL channelSrc )
{
	if ( ( Depth() < 24 ) || 
		 ( Depth() == 24 && ( channelDes == BITMAP_CHANNEL_Alpha || channelSrc == BITMAP_CHANNEL_Alpha ) ) ||
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
	case BITMAP_CHANNEL_Alpha:
		iByteOffsetSrc = 3;
		break;
	case BITMAP_CHANNEL_Red:
		iByteOffsetSrc = 2;
		break;
	case BITMAP_CHANNEL_Green:
		iByteOffsetSrc = 1;
		break;
	case BITMAP_CHANNEL_Blue:
		iByteOffsetSrc = 0;
		break;
	default:
		return false;
	}
	
	switch ( channelDes )
	{
	case BITMAP_CHANNEL_Alpha:
		iByteOffsetDes = 3;
		break;
	case BITMAP_CHANNEL_Red:
		iByteOffsetDes = 2;
		break;
	case BITMAP_CHANNEL_Green:
		iByteOffsetDes = 1;
		break;
	case BITMAP_CHANNEL_Blue:
		iByteOffsetDes = 0;
		break;
	default:
		return false;
	}
	
	size_t iByteIndex = 0;
	size_t iPixelByteCount = Depth()>>3;
	for ( size_t iY = 0; iY < Height(); iY++ )
	{
		iByteIndex = GetPixelByteIndex( 0, iY );
		for ( size_t iX = 0; iX < Width(); iX++ )
		{
			mpBits[iByteIndex + ( iX * iPixelByteCount ) + iByteOffsetDes] \
				= mpBits[iByteIndex + ( iX * iPixelByteCount ) + iByteOffsetSrc];
		}
	}
	return TRUE;
}

CHE_Bitmap* CHE_Bitmap::Clone( const BITMAP_RECT* pRect ) const
{
	if ( mpBits == nullptr )
	{
		return nullptr;
	}

	if ( pRect  == nullptr )
	{
		CHE_Bitmap * pNewBitmap = GetAllocator()->New<CHE_Bitmap>( GetAllocator() );
		pNewBitmap->Create( Width(), Height(), Depth(), Direction(), Pitch() * Height(), GetBuffer(), mpPalette );
		return pNewBitmap;
	}else{
		BITMAP_RECT rect;
		rect.top = pRect->top;
		rect.left = pRect->left;
		rect.width = pRect->width;
		rect.height = pRect->height;
		
		//if ( rect.top < 0 )
		//{
		//	rect.top = 0;
		//}
		//if ( rect.left < 0 )
		//{
		//	rect.left = 0;
		//}
		if ( rect.width > Width() - rect.left )
		{
			rect.width = Width() - rect.left;
		}
		if ( rect.height > Height() - rect.top )
		{
			rect.height = Height() - rect.top;
		}

		if ( rect.top == 0 && rect.width == Width() && rect.left == 0 && rect.height == Height() )
		{
			CHE_Bitmap * pNewBitmap = GetAllocator()->New<CHE_Bitmap>( GetAllocator() );
			pNewBitmap->Create( Width(), Height(), Depth(), Direction(), Pitch() * Height(), GetBuffer(), mpPalette );
			return pNewBitmap;
		}

		switch ( Depth() )
		{
		case 1:
			{
				size_t lineBuffsize = ( ( ( rect.width * Depth() ) + 31 ) & ~31 ) >> 3;
				PBYTE lpLineData = GetAllocator()->NewArray<BYTE>( lineBuffsize );
				memset( lpLineData, 0, lineBuffsize );

				CHE_Bitmap * pNewBitmap = GetAllocator()->New<CHE_Bitmap>( GetAllocator() );
				pNewBitmap->Create( rect.width, rect.height, Depth(), Direction(), lineBuffsize * rect.height, nullptr, mpPalette );

				size_t indexB = 0, indexE = 0;
				for ( size_t y = rect.top, desY = 0; y < rect.top + rect.height; y++, desY++ )
				{
					indexB = GetPixelByteIndex( rect.left, y );
					indexE = GetPixelByteIndex( rect.left + rect.width, y );

					size_t nOffset = 8 - rect.left % 8;
					size_t tempDByte = 0;

					for ( size_t oriIndex = indexB, desIndex = 0; oriIndex <= indexE; oriIndex++, desIndex++ )
					{
						tempDByte = mpBits[oriIndex];
						tempDByte <<= 8;
						if ( oriIndex+1 <= indexE )
						{
							tempDByte += mpBits[oriIndex+1];
						}
						tempDByte >>= nOffset;
						lpLineData[desIndex] = (BYTE)tempDByte;
					}
					pNewBitmap->DrawLine( desY, 0, rect.width, lpLineData, lineBuffsize );
					memset( lpLineData, 0, lineBuffsize );
				}
				GetAllocator()->DeleteArray<BYTE>( lpLineData );
				return pNewBitmap;
				break;
			}
		case 4:
			{
				size_t lineBuffsize = ( ( ( rect.width * Depth() ) + 31 ) & ~31 ) >> 3;
				PBYTE lpLineData = GetAllocator()->NewArray<BYTE>( lineBuffsize );
				memset( lpLineData, 0, lineBuffsize );

				CHE_Bitmap * pNewBitmap = GetAllocator()->New<CHE_Bitmap>( GetAllocator() );
				pNewBitmap->Create( rect.width, rect.height, Depth(), Direction(), lineBuffsize * rect.height, nullptr, mpPalette );

				size_t oriIndex = 0, desIndex = 0;
				BYTE oriByte = 0;
				BYTE newByte = 0;
				for ( size_t y = rect.top, desY = 0; y < rect.top + rect.height; y++, desY++ )
				{

					for ( size_t x = rect.left, desX = 0; x < rect.left + rect.width; x++, desX++ )
					{
						oriIndex = GetPixelByteIndex( x, y );
						desIndex = desX/2;
						
						oriByte = mpBits[oriIndex];
						newByte = lpLineData[desIndex];
						if ( rect.left % 2 == 0 )
						{
							if ( x % 2 == 0 )
							{
								newByte &= 0x0F;
								oriByte &= 0xF0;
							}else{
								newByte &= 0xF0;
								oriByte &= 0x0F;
							}
						}else{
							if ( x % 2 == 0 )
							{
								oriByte &= 0x0F;
								oriByte = oriByte<<4;
								newByte &= 0x0F;
							}else{
								oriByte &= 0xF0;
								oriByte = oriByte>>4;
								newByte &= 0xF0;
							}
						}
	
						lpLineData[desIndex] = newByte | oriByte;
					}
					pNewBitmap->DrawLine( desY, 0, rect.width, lpLineData, lineBuffsize );
					memset( lpLineData, 0, lineBuffsize );
				}
				GetAllocator()->DeleteArray<BYTE>( lpLineData );
				return pNewBitmap;
				break;
			}
		case 8:
			{
				CHE_Bitmap * pNewBitmap = GetAllocator()->New<CHE_Bitmap>( GetAllocator() );
				pNewBitmap->Create( rect.width, rect.height, Depth(), Direction(), ( ( ( rect.width * Depth() ) + 31 ) & ~31 ) >> 3 * rect.height, nullptr, mpPalette );
				PBYTE lpLineData = GetAllocator()->NewArray<BYTE>( ( ( ( rect.width * Depth() ) + 31 ) & ~31 ) >> 3 );

				size_t indexB = 0, indexE = 0;
				for ( size_t y = rect.top, desY = 0; y < rect.top + rect.height; y++, desY++ )
				{
					if ( Direction() == BITMAP_DIRECTION_UP )
					{
						indexB = ( Height() - 1 - y ) * Pitch();
						indexE = indexB;
						indexB += ( rect.left * Depth() ) >> 3;
						indexE += ( ( rect.left + rect.width - 1 ) * Depth() ) >> 3;
					}else{
						indexB = y * Pitch();
						indexE = indexB;
						indexB += ( rect.left * Depth() ) >> 3;
						indexE += ( ( rect.left + rect.width - 1 ) * Depth() ) >> 3;
					}
					for ( size_t oriIndex = indexB, desIndex = 0; oriIndex <= indexE; oriIndex++, desIndex++ )
					{
						lpLineData[desIndex] = mpBits[oriIndex];
					}
					pNewBitmap->DrawLine( desY, 0, rect.width, lpLineData, ( ( ( rect.width * Depth() ) + 31 ) & ~31 ) >> 3 );
				}
				GetAllocator()->DeleteArray<BYTE>( lpLineData );
				return pNewBitmap;
				break;
			}
		case 24:
			{
				CHE_Bitmap * pNewBitmap = GetAllocator()->New<CHE_Bitmap>( GetAllocator() );
				pNewBitmap->Create( rect.width, rect.height, Depth(), Direction(), ( ( ( rect.width * Depth() ) + 31 ) & ~31 ) >> 3 * rect.height, nullptr, mpPalette );
				PBYTE lpLineData = GetAllocator()->NewArray<BYTE>( ( ( ( rect.width * Depth() ) + 31 ) & ~31 ) >> 3 );
				
				size_t indexB = 0, indexE = 0;
				for ( size_t y = rect.top, desY = 0; y < rect.top + rect.height; y++, desY++ )
				{
					if ( Direction() == BITMAP_DIRECTION_UP )
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
					pNewBitmap->DrawLine( desY, 0, rect.width, lpLineData, ( ( ( rect.width * Depth() ) + 31 ) & ~31 ) >> 3 );
				}
				GetAllocator()->DeleteArray<BYTE>( lpLineData );
				return pNewBitmap;
				break;
			}
		case 32:
			{
				CHE_Bitmap * pNewBitmap = GetAllocator()->New<CHE_Bitmap>( GetAllocator() );
				pNewBitmap->Create( rect.width, rect.height, Depth(), Direction(), ( ( ( rect.width * Depth() ) + 31 ) & ~31 ) >> 3 * rect.height, nullptr, mpPalette );
				PBYTE lpLineData = GetAllocator()->NewArray<BYTE>( ( ( ( rect.width * Depth() ) + 31 ) & ~31 ) >> 3 );
				
				size_t indexB = 0, indexE = 0;
				for ( size_t y = rect.top, desY = 0; y < rect.top + rect.height; y++, desY++ )
				{
					if ( Direction() == BITMAP_DIRECTION_UP )
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
					pNewBitmap->DrawLine( desY, 0, rect.width, lpLineData, ( ( ( rect.width * Depth() ) + 31 ) & ~31 ) >> 3 );
				}
				GetAllocator()->DeleteArray<BYTE>( lpLineData );
				return pNewBitmap;
				break;
			}
		default:
			return nullptr;
		}
	}
	return nullptr;
}

bool CHE_Bitmap::CompositeMask( ARGB color, size_t x, size_t y, CHE_Bitmap & maskBitmap )
{
	if ( Depth() < 24 || maskBitmap.GetBuffer() == nullptr ||
		 ( maskBitmap.Depth() != BITMAP_DEPTH_1BPP && maskBitmap.Depth() != BITMAP_DEPTH_8BPP ) )
	{
		return false;
	}

	size_t nWidth = maskBitmap.Width(), nHeight = maskBitmap.Height();
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
				for ( size_t nY = 0; nY < nHeight; nY++ )
				{
					for ( size_t nX = 0; nX < nWidth; nX++ )
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
				for ( size_t nY = 0; nY < nHeight; nY++ )
				{
					for ( size_t nX = 0; nX < nWidth; nX++ )
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
				for ( size_t nY = 0; nY < nHeight; nY++ )
				{
					for ( size_t nX = 0; nX < nWidth; nX++ )
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
				for ( size_t nY = 0; nY < nHeight; nY++ )
				{
					for ( size_t nX = 0; nX < nWidth; nX++ )
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

bool	CHE_Bitmap::Insert( const CHE_Bitmap & bitmap, size_t x, size_t y )
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
	
	CHE_Bitmap * tempBitmap = bitmap.Clone( &rtSrcBitmap );
	if ( tempBitmap == nullptr )
	{
		return false;
	}
	
	size_t index = 0, lineDataSize = tempBitmap->Pitch();
	for ( size_t nLine = 0; nLine < rtSrcBitmap.height; nLine++ )
	{
		index = tempBitmap->GetPixelByteIndex( 0, nLine );
		DrawLine( nLine + y, x, tempBitmap->Width(), &(tempBitmap->mpBits[index]), lineDataSize );
	}
	
	GetAllocator()->Delete( tempBitmap );
	
	return TRUE;
}

// bool CHE_FontBitmap::Load( size_t width, size_t height, size_t pitch, HE_BITMAP_FORMAT format, HE_BITMAP_ORIG flowOrig, PBYTE buffer )
// {
// 	if ( buffer == nullptr || width <= 0 || height <= 0 )
// 	{
// 		return false;
// 	}
// 
// 	size_t TurePitch;
// 	BYTE depth;
// 	HE_BITMAP_FORMAT desFormat;
// 	size_t bitStremSize = 0;
// 	ARGB* pPlatte = nullptr;
// 
// 	if ( format == BITMAP_FORMAT_1BPPMask )
// 	{
// 		desFormat = BITMAP_FORMAT_1BPP;
// 		depth = 1;
// 	}else if ( format == BITMAP_FORMAT_8BPPMask )
// 	{
// 		desFormat = BITMAP_FORMAT_8BPP;
// 		depth = 8;
// 	}else{
// 		return false;
// 	}
// 	TurePitch = ( ( ( width * depth ) + 31 ) & ~31 ) >> 3;
// 	bitStremSize = height*TurePitch;
// 
// 	PBYTE bitStrem = new unsigned char[bitStremSize];
// 
// 	if ( format == BITMAP_FORMAT_1BPPMask )
// 	{
// 		size_t irow = 0;
// 		for ( size_t j = 0; j < bitStremSize; j++ )
// 		 {
// 			irow = j/pitch;
// 			bitStrem[ j % pitch + ( height - irow - 1 ) * TurePitch ] = buffer[j];
// 		}
// 		pPlatte = new ARGB[2];
// 		pPlatte[0] = 0xFFFFFFFF;
// 		pPlatte[1] = 0xFF000000;
// 	}else{
// 		for( size_t j = 0; j < height; j++ )
// 		{
// 			for( size_t i = 0; i < width; i++ )
// 			{
// 				bitStrem[ i + ( height - j - 1 ) * TurePitch ] = buffer[i + width * j];
// 			}
// 		}
// 		pPlatte = new ARGB[256];
// 		
// 		for ( size_t iPlatteIndex = 255, iPlatteValue = 0; iPlatteIndex >= 0; iPlatteIndex--, iPlatteValue++ )
// 		{
// 			pPlatte[iPlatteIndex] = (0xFF<< 24) + (iPlatteValue<<16) + (iPlatteValue<<8) + iPlatteValue;
// 		}
// 	}
// 
// 	Create( width, height, desFormat, BITMAP_ORIG_BOTTOM, bitStremSize, bitStrem, pPlatte );
// 
// 	delete [] pPlatte;
// 	pPlatte = nullptr;
// 
// 	return TRUE;
// }

CHE_Bitmap* CHE_Bitmap::StretchTo( size_t desWidth, size_t desHeight, size_t flag, BITMAP_RECT * pRect )
{
	if ( desWidth == 0 || desHeight == 0 )
	{
		return nullptr;
	}

	CHE_Bitmap * pBitmapRet = GetAllocator()->New<CHE_Bitmap>( GetAllocator() );

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
			for ( size_t iY = 0; iY < desHeight; iY++ )
			{
				for ( size_t iX = 0; iX < desWidth; iX++ )
				{
					if ( GetPixelColor( (size_t)(iX * fWidthScale), (size_t)(iY * fHeightScale), color ) )
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
			size_t dwXS = 0, dwXB = 0, dwYS = 0, dwYB = 0;
			BYTE clrdwYS = 0, clrdwYB = 0, clrTmp1 = 0, clrTmp2;
			ARGB tmpClr = 0xFFFFFFFF;
			for ( size_t iY = 0; iY < desHeight; iY++ )
			{
				for ( size_t iX = 0; iX < desWidth; iX++ )
				{
					fX = iX * fWidthScale;
					fY = iY * fHeightScale;

					dwXS = (size_t)fX;
					dwXB = (size_t)(fX + 1);
					dwYS = (size_t)fY;
					dwYB = (size_t)(fY + 1);

					GetPixelColor( dwXS, dwYS, tmpClr );
					clrdwYS = (BYTE)(tmpClr >> 16);
					GetPixelColor( dwXS, dwYB, tmpClr );
					clrdwYB = (BYTE)(tmpClr >> 16);
					clrTmp1 = (BYTE)(clrdwYB - (clrdwYB-clrdwYS)*(dwYB-fY)/(dwYB-dwYS));

					GetPixelColor( dwXB, dwYS, tmpClr );
					clrdwYS = (BYTE)(tmpClr >> 16);
					GetPixelColor( dwXB, dwYB, tmpClr );
					clrdwYB = (BYTE)(tmpClr >> 16);
					clrTmp2 = (BYTE)(clrdwYB - (clrdwYB-clrdwYS)*(dwYB-fY)/(dwYB-dwYS));

					red = (BYTE)(clrTmp2 - (clrTmp2-clrTmp1)*(dwXB-fX)/(dwXB-dwXS));

					GetPixelColor( dwXS, dwYS, tmpClr );
					clrdwYS = (BYTE)(tmpClr >> 8);
					GetPixelColor( dwXS, dwYB, tmpClr );
					clrdwYB = (BYTE)(tmpClr >> 8);
					clrTmp1 = (BYTE)(clrdwYB - (clrdwYB-clrdwYS)*(dwYB-fY)/(dwYB-dwYS));
					
					GetPixelColor( dwXB, dwYS, tmpClr );
					clrdwYS = (BYTE)(tmpClr >> 8);
					GetPixelColor( dwXB, dwYB, tmpClr );
					clrdwYB = (BYTE)(tmpClr >> 8);
					clrTmp2 = (BYTE)(clrdwYB - (clrdwYB-clrdwYS)*(dwYB-fY)/(dwYB-dwYS));
					
					green = (BYTE)(clrTmp2 - (clrTmp2-clrTmp1)*(dwXB-fX)/(dwXB-dwXS));

					GetPixelColor( dwXS, dwYS, tmpClr );
					clrdwYS = (BYTE)tmpClr;
					GetPixelColor( dwXS, dwYB, tmpClr );
					clrdwYB = (BYTE)tmpClr;
					clrTmp1 = (BYTE)(clrdwYB - (clrdwYB-clrdwYS)*(dwYB-fY)/(dwYB-dwYS));
					
					GetPixelColor( dwXB, dwYS, tmpClr );
					clrdwYS = (BYTE)tmpClr;
					GetPixelColor( dwXB, dwYB, tmpClr );
					clrdwYB = (BYTE)tmpClr;
					clrTmp2 = (BYTE)(clrdwYB - (clrdwYB-clrdwYS)*(dwYB-fY)/(dwYB-dwYS));
					
					blue = (BYTE)(clrTmp2 - (clrTmp2-clrTmp1)*(dwXB-fX)/(dwXB-dwXS));

					color = (red << 16) + (green << 8) + blue;

					pBitmapRet->SetPixelColor( iX, iY, color );
				}
			}
			break;
		}
	default:
		{
			pBitmapRet->Clean();
			GetAllocator()->Delete<CHE_Bitmap>( pBitmapRet );
			return nullptr;
		}
	}
	return pBitmapRet;
}

CHE_Bitmap*	CHE_Bitmap::Translate( FLOAT a, FLOAT b, FLOAT c, FLOAT d, FLOAT e, FLOAT f )
{
	CHE_Bitmap * pBitmapRet = GetAllocator()->New<CHE_Bitmap>( GetAllocator() );
	if ( !pBitmapRet->Create( mWidth, mHeight, this->Depth(), this->Direction() ) )
	{
		return nullptr;	
	}
	pBitmapRet->Fill( 0xFF000000 );
	ARGB color = 0xFFFFFFFF;
	BYTE red = 0, green = 0, blue = 0;
	FLOAT fX = 0, fY = 0;
	size_t XS = 0, XB = 0, YS = 0, YB = 0;
	BYTE clrYS = 0, clrYB = 0, clrTmp1 = 0, clrTmp2;
	ARGB tmpClr = 0xFFFFFFFF;
	for ( size_t iY = 0; iY < mHeight; iY++ )
	{
		for ( size_t iX = 0; iX < mWidth; iX++ )
		{
			fX = a * iX + c * iY - e;
			fY = b * iX + d * iY - f;

			XS = (size_t)fX;
			YS = (size_t)fY;
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
}


size_t CHE_Bitmap::GetPixelByteIndex( size_t x, size_t y ) const
{
	size_t index = 0;
	if ( Direction() == BITMAP_DIRECTION_UP )
	{
		index = ( Height() - 1 - y ) * Pitch();
		index += ( x * Depth() ) >> 3;
	}else{
		index = y * Pitch();
		index += ( x * Depth() ) >> 3;
	}
	return index;
}


