#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

#include "../include/che_bitmap.h"


CHE_Palette::CHE_Palette( HE_BITMAP_DEPTH depth, const HE_ARGB * const pPalette )
{
	if (pPalette)
	{
		switch (depth)
		{
		case BITMAP_DEPTH_1BPP:
			m_nPaletteSize = 2;
			break;
		case BITMAP_DEPTH_4BPP:
			m_nPaletteSize = 16;
			break;
		case BITMAP_DEPTH_8BPP:
			m_nPaletteSize = 256;
			break;
		case BITMAP_DEPTH_24BPP:
		case BITMAP_DEPTH_32BPP:
		default:
			m_pPalette		= NULL;
			m_nPaletteSize	= 0;
			break;
		}
		if (m_nPaletteSize)
		{
			m_pPalette = new HE_ARGB[m_nPaletteSize];
			memset( m_pPalette, 0, sizeof(HE_ARGB) * m_nPaletteSize );
			memcpy( m_pPalette, pPalette, sizeof(HE_ARGB) * m_nPaletteSize );
		}
	}else{
		switch (depth)
		{
// 		case BITMAP_FORMAT_1BPPMask:
// 			{
// 				m_format = format;
// 				m_nPaletteSize = 2;
// 				m_pPalette = new HE_ARGB[2];
// 				m_pPalette[0] = 0x00FFFFFF;
// 				m_pPalette[1] = 0x00000000;
// 				break;
// 			}
		case BITMAP_DEPTH_1BPP:
			{
				m_nPaletteSize = 2;
				m_pPalette = new HE_ARGB[2];
				m_pPalette[0] = 0x00000000;
				m_pPalette[1] = 0x00FFFFFF;
				break;
			}
		case BITMAP_DEPTH_4BPP:
			{
				m_nPaletteSize = 16;
				m_pPalette = new HE_ARGB[16];
				m_pPalette[0] = 0x00FFFFFF;
				m_pPalette[1] = 0x00FF00FF;
				m_pPalette[2] = 0x00FFFF00;
				m_pPalette[3] = 0x0000FFFF;
				m_pPalette[4] = 0x000000FF;
				m_pPalette[5] = 0x0000FF00;
				m_pPalette[6] = 0x00FF0000;
				m_pPalette[7] = 0x00C0C0C0;
				m_pPalette[8] = 0x00808080;
				m_pPalette[9] = 0x00800080;
				m_pPalette[10] = 0x00808000;
				m_pPalette[11] = 0x00008080;
				m_pPalette[12] = 0x00000080;
				m_pPalette[13] = 0x00008000;
				m_pPalette[14] = 0x00800000;
				m_pPalette[15] = 0x00000000;
				break;
			}
// 		case BITMAP_DEPTH_8BPPMask:
// 			{
// 				m_format = format;
// 				m_nPaletteSize = 256;
// 				m_pPalette = new HE_ARGB[256];
// 				for ( HE_INT32 i = 0; i < 256; i++ )
// 				{
// 					m_pPalette[i] = i<<16 & i<<8 & i;
// 				}
// 				break;
// 			}
		case BITMAP_DEPTH_8BPP:
			{
				m_nPaletteSize = 256;
				m_pPalette = new HE_ARGB[256];
				HE_ARGB tempPalette[256] = {	0x00000000, 0x00800000, 0x00008000, 0x00808000, 0x00000080, 0x00800080, 0x00008080, 0x00C0C0C0,
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
				memcpy( m_pPalette, tempPalette, m_nPaletteSize * sizeof(HE_ARGB) );
				break;
			}
		case BITMAP_DEPTH_24BPP:
		case BITMAP_DEPTH_32BPP:
		default:
			{
				m_pPalette = NULL;
				m_nPaletteSize = 0;
				break;
			}
		}
	}
}

CHE_Palette::CHE_Palette( const CHE_Palette& palette )
{
	m_nPaletteSize = palette.m_nPaletteSize;
	if (m_nPaletteSize > 0 && palette.m_pPalette)
	{
		m_pPalette = new HE_ARGB[m_nPaletteSize];
		memcpy( m_pPalette, palette.m_pPalette, m_nPaletteSize * sizeof(HE_ARGB) );
	}else{
		m_pPalette = NULL;
	}
}

CHE_Palette::~CHE_Palette()
{
	if (m_pPalette)
	{
		delete m_pPalette;
		m_pPalette = NULL;
	}
}

CHE_Palette & CHE_Palette::operator=( const CHE_Palette& palette )
{
	m_nPaletteSize = palette.m_nPaletteSize;
	if (m_pPalette)
	{
		delete m_pPalette;
		m_pPalette = NULL;
	}
	if (palette.m_pPalette)
	{
		m_pPalette = new HE_ARGB[m_nPaletteSize];
		memcpy( m_pPalette, palette.m_pPalette, m_nPaletteSize * sizeof( HE_ARGB ) );
	}
	return *this;
}

HE_DWORD	CHE_Palette::GetColorCount() const
{
	return m_nPaletteSize;
}

HE_BOOL		CHE_Palette::GetNearColorIndex( HE_ARGB color, HE_DWORD & indexRet ) const
{
	if ( m_nPaletteSize == 0 )
	{
		return FALSE;
	}
	HE_DWORD index = 0;
	HE_DWORD lessCY = 0xFFFF;
	HE_DWORD tempLessCY = 0;
	HE_ARGB srcColor;
	
	for ( HE_DWORD i = 0; i < m_nPaletteSize; i++ )
	{
		srcColor = m_pPalette[i];
		
		tempLessCY = abs( ( color & 0x000000FF ) - ( srcColor & 0x000000FF )  );
		tempLessCY += abs( ( color >> 8 & 0x000000FF ) - ( srcColor >> 8 & 0x000000FF ) );
		tempLessCY += abs( ( color >> 16 & 0x000000FF ) - ( srcColor >> 16 & 0x000000FF ) );
		tempLessCY += abs( ( color >> 24 & 0x000000FF ) - ( srcColor >> 24 & 0x000000FF ) );
		if ( tempLessCY < lessCY )
		{
			lessCY = tempLessCY;
			index = i;
		}
	}
	indexRet = index;
	return TRUE;
}

HE_BOOL	CHE_Palette::GetColor( HE_DWORD index, HE_ARGB & colorRet ) const
{
	if (index >= m_nPaletteSize)
	{
		return FALSE;
	}
	colorRet = m_pPalette[index];
	return TRUE;
}

HE_BOOL	CHE_Palette::SetColor( HE_DWORD index, HE_ARGB color )
{
	if (index >= m_nPaletteSize)
	{
		return FALSE;
	}
	m_pPalette[index] = color;
	return TRUE;
}

HE_BOOL CHE_Palette::GetColorIndex( HE_ARGB color, HE_DWORD & indexRet ) const
{
	if ( m_nPaletteSize == 0 )
	{
		return FALSE;
	}

	for ( HE_DWORD i = 0; i < m_nPaletteSize; i++ )
	{
		if ( m_pPalette[i] == color )
		{
			indexRet = i;
			return TRUE;
		}
	}
	return FALSE;
}

HE_BOOL	CHE_Palette::IsColorExist( HE_ARGB color ) const
{
	for ( HE_DWORD i = 0; i < m_nPaletteSize; i++)
	{
		if ( m_pPalette[i] == color )
		{
			return TRUE;
		}
	}
	return FALSE;
}

CHE_Bitmap::CHE_Bitmap()
{
	m_lpBits = NULL;
	m_lpPalette = NULL;
	m_Depth = BITMAP_DEPTH_24BPP;
	m_Direction = BITMAP_DIRECTION_UP;
	memset( &m_InfoHeader, 0, sizeof(HE_BITMAPINFOHEADER) );
}

CHE_Bitmap::~CHE_Bitmap()
{
	if ( m_lpBits )
	{
		delete [] m_lpBits;
		m_lpBits = NULL;
	}
	if ( m_lpPalette )
	{
		delete m_lpPalette;
		m_lpBits = NULL;
	}
}

HE_BOOL CHE_Bitmap::Load( HE_LPCSTR filePath )
{
	if ( NULL == filePath )
	{
		return FALSE;
	}

	FILE* pfile = fopen( filePath, "rb+" );
	if ( NULL == pfile )
	{
		return FALSE;
	}
	HE_BITMAPFILEHEADER bfHeader;
	size_t lBytesReaded = fread( &(bfHeader.bfType), 1, sizeof(bfHeader.bfType), pfile );
	if ( lBytesReaded != sizeof(bfHeader.bfType) )
	{
		fclose( pfile );
		return FALSE;
	}
	lBytesReaded = fread( &(bfHeader.bfSize), 1, sizeof(bfHeader.bfSize), pfile );
	if ( lBytesReaded != sizeof(bfHeader.bfSize) )
	{
		fclose( pfile );
		return FALSE;
	}
	lBytesReaded = fread( &(bfHeader.bfReserved1), 1, sizeof(bfHeader.bfReserved1), pfile );
	if ( lBytesReaded != sizeof(bfHeader.bfReserved1) )
	{
		fclose( pfile );
		return FALSE;
	}
	lBytesReaded = fread( &(bfHeader.bfReserved2), 1, sizeof(bfHeader.bfReserved2), pfile );
	if ( lBytesReaded != sizeof(bfHeader.bfReserved2) )
	{
		fclose( pfile );
		return FALSE;
	}
	lBytesReaded = fread( &(bfHeader.bfOffBits), 1, sizeof(bfHeader.bfOffBits), pfile );
	if ( lBytesReaded != sizeof(bfHeader.bfOffBits) )
	{
		fclose( pfile );
		return FALSE;
	}

	if( bfHeader.bfType != ( (HE_WORD)( 'M' << 8 ) | 'B' ) )
	{   
		fclose( pfile );
		return FALSE;
	}

	HE_BITMAPINFOHEADER biHeader;
	lBytesReaded = fread( &(biHeader.biSize), 1, sizeof(biHeader.biSize), pfile );
	if ( lBytesReaded != sizeof(biHeader.biSize) )
	{
		fclose( pfile );
		return FALSE;
	}
	lBytesReaded = fread( &(biHeader.biWidth), 1, sizeof(biHeader.biWidth), pfile );
	if ( lBytesReaded != sizeof(biHeader.biWidth) )
	{
		fclose( pfile );
		return FALSE;
	}
	lBytesReaded = fread( &(biHeader.biHeight), 1, sizeof(biHeader.biHeight), pfile );
	if ( lBytesReaded != sizeof(biHeader.biHeight ) )
	{
		fclose( pfile );
		return FALSE;
	}
	lBytesReaded = fread( &(biHeader.biPlanes), 1, sizeof(biHeader.biPlanes), pfile );
	if ( lBytesReaded != sizeof(biHeader.biPlanes) )
	{
		fclose( pfile );
		return FALSE;
	}
	lBytesReaded = fread( &(biHeader.biBitCount), 1, sizeof(biHeader.biBitCount), pfile );
	if ( lBytesReaded != sizeof(biHeader.biBitCount) )
	{
		fclose( pfile );
		return FALSE;
	}
	lBytesReaded = fread( &(biHeader.biCompression), 1, sizeof(biHeader.biCompression), pfile );
	if ( lBytesReaded != sizeof(biHeader.biCompression) )
	{
		fclose( pfile );
		return FALSE;
	}
	lBytesReaded = fread( &(biHeader.biSizeImage), 1, sizeof(biHeader.biSizeImage), pfile );
	if ( lBytesReaded != sizeof(biHeader.biSizeImage) )
	{
		fclose( pfile );
		return FALSE;
	}
	lBytesReaded = fread( &(biHeader.biXPelsPerMeter), 1, sizeof(biHeader.biXPelsPerMeter), pfile );
	if ( lBytesReaded != sizeof(biHeader.biXPelsPerMeter) )
	{
		fclose( pfile );
		return FALSE;
	}
	lBytesReaded = fread( &(biHeader.biYPelsPerMeter), 1, sizeof(biHeader.biYPelsPerMeter), pfile );
	if ( lBytesReaded != sizeof(biHeader.biYPelsPerMeter) )
	{
		fclose( pfile );
		return FALSE;
	}
	lBytesReaded = fread( &(biHeader.biClrUsed), 1, sizeof(biHeader.biClrUsed), pfile );
	if ( lBytesReaded != sizeof(biHeader.biClrUsed) )
	{
		fclose( pfile );
		return FALSE;
	}
	lBytesReaded = fread( &(biHeader.biClrImportant), 1, sizeof(biHeader.biClrImportant), pfile );
	if ( lBytesReaded != sizeof(biHeader.biClrImportant) )
	{
		fclose( pfile );
		return FALSE;
	}
	if ( biHeader.biSize != 40 )
	{
		fclose( pfile );
		return FALSE;
	}

	HE_DWORD dwBitlen = 0;
	HE_DWORD dwPlatteSize = 0;
	HE_ARGB* lpPalette = NULL;
	HE_LPBYTE lpBits = NULL;

	fseek( pfile, 0, SEEK_END );
	dwBitlen = ftell( pfile );
	if ( dwBitlen < biHeader.biSizeImage )
	{
		fclose( pfile );
		return FALSE;
	}
	dwBitlen = biHeader.biSizeImage;
	if ( biHeader.biBitCount == 8 )
	{
		dwPlatteSize = 256*4;
	}else if ( biHeader.biBitCount == 4 )
	{
		dwPlatteSize = 16*4;
	}else if ( biHeader.biBitCount == 1 )
	{
		dwPlatteSize = 2*4;
	}

	if ( dwPlatteSize > 0 )
	{
		lpPalette = new HE_ARGB[dwPlatteSize];
		fseek( pfile, 54, SEEK_SET );
		lBytesReaded = fread( lpPalette, 1, dwPlatteSize, pfile );
		if ( lBytesReaded != dwPlatteSize )
		{
			delete [] lpPalette;
			lpPalette = NULL;
			fclose( pfile );
			return FALSE;
		}
	}

	lpBits = new HE_BYTE[dwBitlen];
	fseek( pfile, bfHeader.bfOffBits, SEEK_SET );
	lBytesReaded = fread( lpBits, 1, dwBitlen, pfile );
	if ( lBytesReaded != dwBitlen )
	{
		delete[] lpBits;
		lpBits = NULL;
		fclose( pfile );
		return FALSE;
	}
	fclose( pfile );
	pfile = NULL;

	Clean();

	memcpy( &m_InfoHeader, &biHeader, 40 );
	m_lpBits = lpBits;

	switch ( biHeader.biBitCount )
	{
	case 1:
		m_Depth = BITMAP_DEPTH_1BPP;
		break;
	case 4:
		m_Depth = BITMAP_DEPTH_4BPP;
		break;
	case 8:
		m_Depth = BITMAP_DEPTH_8BPP;
		break;
	case 24:
		m_Depth = BITMAP_DEPTH_24BPP;
		break;
	case 32:
		m_Depth = BITMAP_DEPTH_32BPP;
		break;
	default:
		m_Depth = BITMAP_DEPTH_24BPP;
	}
	m_lpPalette = new CHE_Palette( m_Depth, lpPalette );

	if ( m_InfoHeader.biHeight > 0 )
	{
		m_Direction = BITMAP_DIRECTION_UP;
	}else
	{
		m_Direction = BITMAP_DIRECTION_DOWN;
	}

	return TRUE;
}

HE_BOOL CHE_Bitmap::Save( HE_LPCSTR filePath )
{
	if ( NULL == filePath )
	{
		return FALSE;
	}
	if ( m_lpBits == NULL )
	{
		return FALSE;
	}
	
	FILE* pfile = fopen( filePath, "wb+" );
	if ( NULL == pfile )
	{
		return FALSE;
	}
	HE_BITMAPFILEHEADER bfHeader;
	bfHeader.bfSize = 54 + m_lpPalette->GetColorCount() * 4 + Pitch() * Height();
	bfHeader.bfType = 0x4D42;
	bfHeader.bfReserved2 = 0;
	bfHeader.bfReserved1 = 0;
	bfHeader.bfOffBits = 54 + m_lpPalette->GetColorCount() * 4;

	size_t lBytesReaded = fwrite( &(bfHeader.bfType), 1, sizeof(bfHeader.bfType), pfile );
	lBytesReaded += fwrite( &(bfHeader.bfSize), 1, sizeof(bfHeader.bfSize), pfile );
	lBytesReaded += fwrite( &(bfHeader.bfReserved1), 1, sizeof(bfHeader.bfReserved1), pfile );
	lBytesReaded += fwrite( &(bfHeader.bfReserved2), 1, sizeof(bfHeader.bfReserved2), pfile );
	lBytesReaded += fwrite( &(bfHeader.bfOffBits), 1, sizeof(bfHeader.bfOffBits), pfile );
	
	 if ( lBytesReaded != 14 )
	 {
	 		fclose( pfile );
	 		return FALSE;
	 }
	
	lBytesReaded = fwrite( &(m_InfoHeader.biSize), 1, sizeof(m_InfoHeader.biSize), pfile );
	lBytesReaded += fwrite( &(m_InfoHeader.biWidth), 1, sizeof(m_InfoHeader.biWidth), pfile );
	lBytesReaded += fwrite( &(m_InfoHeader.biHeight), 1, sizeof(m_InfoHeader.biHeight), pfile );
	lBytesReaded += fwrite( &(m_InfoHeader.biPlanes), 1, sizeof(m_InfoHeader.biPlanes), pfile );
	lBytesReaded += fwrite( &(m_InfoHeader.biBitCount), 1, sizeof(m_InfoHeader.biBitCount), pfile );
	lBytesReaded += fwrite( &(m_InfoHeader.biCompression), 1, sizeof(m_InfoHeader.biCompression), pfile );
	lBytesReaded += fwrite( &(m_InfoHeader.biSizeImage), 1, sizeof(m_InfoHeader.biSizeImage), pfile );
	lBytesReaded += fwrite( &(m_InfoHeader.biXPelsPerMeter), 1, sizeof(m_InfoHeader.biXPelsPerMeter), pfile );
	lBytesReaded += fwrite( &(m_InfoHeader.biYPelsPerMeter), 1, sizeof(m_InfoHeader.biYPelsPerMeter), pfile );
	lBytesReaded += fwrite( &(m_InfoHeader.biClrUsed), 1, sizeof(m_InfoHeader.biClrUsed), pfile );
	lBytesReaded += fwrite( &(m_InfoHeader.biClrImportant), 1, sizeof(m_InfoHeader.biClrImportant), pfile );
	
	if ( lBytesReaded != 40 )
	{
		fclose( pfile );
		return FALSE;
	}

	if ( m_lpPalette->GetColorCount() > 0 )
	{
		lBytesReaded = fwrite( m_lpPalette->m_pPalette, 1, m_lpPalette->GetColorCount() * 4, pfile );
		if ( lBytesReaded >> 2 != m_lpPalette->GetColorCount() )
		{
			fclose( pfile );
			return FALSE;
		}
	}
	lBytesReaded = fwrite( m_lpBits, 1, Pitch() * Height(), pfile );
	if ( lBytesReaded != Pitch() * Height()  )
	{
		fclose( pfile );
		return FALSE;
	}
	fclose( pfile );
	return TRUE;
}

HE_BOOL CHE_Bitmap::Create( HE_DWORD width, HE_DWORD height, HE_BITMAP_DEPTH depth, HE_BITMAP_DIRECTION direction, HE_DWORD bufferSize, HE_LPCBYTE buffer, CHE_Palette* pPalette )
{
	HE_BITMAPINFOHEADER biHeader;
	biHeader.biSize = 40;
	biHeader.biWidth = width;
	HE_LONG lHeight = height;
	biHeader.biHeight = ( direction == BITMAP_DIRECTION_UP ) ? (lHeight) : (-lHeight);
	biHeader.biPlanes = 1;
	biHeader.biCompression = BITMAP_COMPRESSION_RGB;
	biHeader.biClrImportant = 0;
	biHeader.biClrUsed = 0;
	biHeader.biXPelsPerMeter = 0;
	biHeader.biYPelsPerMeter = 0;
	biHeader.biBitCount = 0;
	biHeader.biSizeImage = 0;

	switch ( depth )
	{
	case BITMAP_DEPTH_1BPP:
		{
			biHeader.biBitCount = 1;
			biHeader.biSizeImage = ( ( ( width + 31 ) & ~31 ) >> 3 ) * height ;
			break;
		}

	case BITMAP_DEPTH_4BPP:
		{
			biHeader.biBitCount = 4;
			biHeader.biSizeImage = ( ( ( width * 4 + 31 ) & ~31 ) >> 3 ) * height;
			break;
		}
	case BITMAP_DEPTH_8BPP:
		{
			biHeader.biBitCount = 8;
			biHeader.biSizeImage = ( ( ( width * 8 + 31 ) & ~31 ) >> 3 ) * height;
			break;
		}
	case BITMAP_DEPTH_24BPP:
		{
			biHeader.biBitCount = 24;
			biHeader.biSizeImage = ( ( ( width * 24 + 31 ) & ~31 ) >> 3 ) * height;
			break;
		}
	case BITMAP_DEPTH_32BPP:
		{
			biHeader.biBitCount = 32;
			biHeader.biSizeImage = ( ( ( width * 32 + 31 ) & ~31 ) >> 3 ) * height;
			break;
		}
	default:
		break;
	}

	if ( biHeader.biBitCount == 0 || biHeader.biSizeImage == 0 )
	{
		return FALSE;
	}

	HE_LPBYTE lpBit = new HE_BYTE[biHeader.biSizeImage];
	memset( lpBit, 0, biHeader.biSizeImage );

	if ( bufferSize != 0 && buffer != NULL )
	{
		if ( bufferSize > biHeader.biSizeImage )
		{
			bufferSize = biHeader.biSizeImage;
		}
		memcpy( lpBit, buffer, bufferSize );
	}

	Clean();

	memcpy( &m_InfoHeader, &biHeader, 40 );
	m_lpBits = lpBit;
	m_Direction = direction;
	m_Depth = depth;

	if ( pPalette == NULL )
	{
		m_lpPalette = new CHE_Palette( depth );
	}else{
		m_lpPalette = new CHE_Palette( *pPalette );
	}

	return TRUE;
}

HE_VOID	CHE_Bitmap::Clean()
{
	if ( m_lpBits )
	{
		delete [] m_lpBits;
		m_lpBits = NULL;
	}
	if ( m_lpPalette )
	{
		delete m_lpPalette;
		m_lpPalette = NULL;
	}
	memset( &m_InfoHeader, 0, sizeof(HE_BITMAPINFOHEADER) );
	m_Depth = BITMAP_DEPTH_24BPP;
}

HE_BOOL	CHE_Bitmap::GetPixelColor( HE_DWORD x, HE_DWORD y, HE_ARGB & clrRet ) const
{
	if ( x >= Width() )
	{
		return FALSE;
	}
	if ( y >= Height() )
	{
		return FALSE;
	}

	HE_DWORD index = GetPixelByteIndex( x, y );
	HE_BYTE platteIndex = 0;
	if ( Depth() == 32 )
	{
		clrRet = m_lpBits[index] + ( m_lpBits[index+1] << 8 ) + ( m_lpBits[index+2] << 16 ) + ( m_lpBits[index+3] << 24 );
	}else if ( Depth() == 24 )
	{
		clrRet = m_lpBits[index] + ( m_lpBits[index+1] << 8 ) + ( m_lpBits[index+2] << 16 );
	}else if ( Depth() == 8 )
	{
		platteIndex = m_lpBits[index];
		clrRet = m_lpPalette->m_pPalette[platteIndex];
	}else if ( Depth() == 4)
	{
		platteIndex = m_lpBits[index];
		if ( x % 2 == 0 )
		{
			platteIndex &= 0xF0;
		}else{
			platteIndex &= 0x0F;
		}
		clrRet = m_lpPalette->m_pPalette[platteIndex];
	}else if ( Depth() == 1 )
	{
		platteIndex = m_lpBits[index];
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
		clrRet = m_lpPalette->m_pPalette[ ( platteIndex? 1 : 0 ) ];
	}
	return TRUE;
}

HE_BOOL CHE_Bitmap::SetPixelColor( HE_DWORD x, HE_DWORD y, HE_ARGB color )
{
	if ( x >= Width() )
	{
		return FALSE;
	}
	if ( y >= Height() )
	{
		return FALSE;
	}

	HE_DWORD index = GetPixelByteIndex( x, y );
	HE_BYTE platteIndex = 0;
	HE_DWORD tempIndex = 0;;
	if ( Depth() == 32 )
	{
		m_lpBits[index] = (HE_BYTE)( color & 0xFF );
		m_lpBits[index+1] = (HE_BYTE)( ( color >> 8 ) & 0xFF );
		m_lpBits[index+2] = (HE_BYTE)( ( color >> 16 ) & 0xFF );
		m_lpBits[index+3] = (HE_BYTE)( ( color >> 24 ) & 0xFF );
	}else if ( Depth() == 24 )
	{
		m_lpBits[index] = (HE_BYTE)( color & 0xFF );
		m_lpBits[index+1] = (HE_BYTE)( ( color >> 8 ) & 0xFF );
		m_lpBits[index+2] = (HE_BYTE)( ( color >> 16 ) & 0xFF );
	}else if ( Depth() == 8 )
	{
		if ( m_lpPalette->GetNearColorIndex( color, tempIndex ) )
		{
			m_lpBits[index] = (HE_BYTE)tempIndex;
		}
		
	}else if ( Depth() == 4 )
	{
		platteIndex = m_lpBits[index];
		m_lpPalette->GetNearColorIndex( color, tempIndex );
		if ( x % 2 == 0 )
		{
			tempIndex <<= 4;
			platteIndex &= 0x0F;
		}else{
			platteIndex &= 0xF0;
		}
		m_lpBits[index] = (HE_BYTE)tempIndex + platteIndex;
	}else if ( Depth() == 1 )
	{
		platteIndex = m_lpBits[index];
		m_lpPalette->GetNearColorIndex( color, tempIndex );
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
		m_lpBits[index] = (HE_BYTE)tempIndex + platteIndex;
	}
	return TRUE;
}

HE_BOOL	CHE_Bitmap::GetPixelIndex( HE_DWORD x, HE_DWORD y, HE_BYTE & indexRet ) const
{
	if ( x >= Width() )
	{
		return FALSE;
	}
	if ( y >= Height() )
	{
		return FALSE;
	}
	if ( Depth() > 8 )
	{
		return FALSE;
	}
	
	HE_DWORD index = GetPixelByteIndex( x, y );
	if ( Depth() == 8 )
	{
		indexRet = m_lpBits[index];
	}else if ( Depth() == 4)
	{
		indexRet = m_lpBits[index];
		if ( x % 2 == 0 )
		{
			indexRet &= 0xF0;
		}else{
			indexRet &= 0x0F;
		}
	}else if ( Depth() == 1 )
	{
		indexRet = m_lpBits[index];
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

HE_BOOL	CHE_Bitmap::SetPixelIndex( HE_DWORD x, HE_DWORD y, HE_BYTE clrIndex )
{
	if ( x >= Width() )
	{
		return FALSE;
	}
	if ( y >= Height() )
	{
		return FALSE;
	}
	if ( Depth() > 8 )
	{
		return FALSE;
	}

	HE_DWORD index = GetPixelByteIndex( x, y );
	HE_BYTE platteIndex = 0;
	if ( Depth() == 8 )
	{
		m_lpBits[index] = clrIndex;
	}else if ( Depth() == 4 )
	{
		platteIndex = m_lpBits[index];
		if ( x % 2 == 0 )
		{
			clrIndex <<= 4;
			platteIndex &= 0x0F;
		}else{
			platteIndex &= 0xF0;
		}
		m_lpBits[index] = clrIndex + platteIndex;
	}else if ( Depth() == 1 )
	{
		platteIndex = m_lpBits[index];
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
		m_lpBits[index] = clrIndex + platteIndex;
	}
	return TRUE;
}

HE_BOOL	CHE_Bitmap::Fill( HE_ARGB color )
{
	if ( m_lpBits == NULL )
	{
		return FALSE;
	}

	HE_DWORD index;
	if ( m_lpPalette->GetNearColorIndex( color, index ) )
	{
		m_lpPalette->GetColor( (HE_BYTE)index, color );
	}

	switch ( Depth() )
	{
	case 1:
		{
			if ( index == 0 )
			{
				for ( HE_DWORD i = 0; i < Pitch() * Height(); i++ )
				{
					m_lpBits[i] = 0x00;
				}
			}else{
				for ( HE_DWORD i = 0; i < Pitch() * Height(); i++ )
				{
					m_lpBits[i] = 0xFF;
				}
			}
			break;
		}
	case 4:
		{
			for ( HE_DWORD i = 0; i < Pitch() * Height(); i++ )
			{
				index = ( index << 4 ) + ( index & 0x0F );
				m_lpBits[i] = (HE_BYTE)index;
			}
			break;
		}
	case 8:
		{
			for ( HE_DWORD i = 0; i < Pitch() * Height(); i++ )
			{
				m_lpBits[i] = (HE_BYTE)(index & 0xFF );
			}
			break;
		}
	case 24:
		{
			for ( HE_DWORD i = 0; i+2 < Pitch() * Height(); i+=3 )
			{
				m_lpBits[i] = (HE_BYTE)( color & 0xFF );
				m_lpBits[i+1] = (HE_BYTE)( ( color >> 8 ) & 0xFF );
				m_lpBits[i+2] = (HE_BYTE)( ( color >> 16 ) & 0xFF );
			}
			break;
		}
	case 32:
		{
			for ( HE_DWORD i = 0; i+3 < Pitch() * Height(); i+=4 )
			{
				*((HE_LPDWORD)(m_lpBits+i)) = color;
			}
			break;
		}
	default:
		return FALSE;
	}
	return TRUE;
}

HE_BOOL	CHE_Bitmap::Fill( HE_ARGB color, const HE_RECT* pRect )
{
	HE_RECT rect;
	rect.top = pRect->top;
	rect.left = pRect->left;
	rect.width = pRect->width;
	rect.height = pRect->height;
	
	if ( rect.height <= 0 || rect.width <= 0 )
	{
		return FALSE;
	}
	if ( rect.top < 0 )
	{
		rect.height += rect.top;
		rect.top = 0;
	}
	if ( rect.left < 0 )
	{
		rect.width += rect.left;
		rect.left = 0;
	}
	
	for ( HE_DWORD i = rect.top; i < rect.height + rect.top; i++ )
	{
		DrawLine( i, rect.left, rect.width, color );
	}
	return TRUE;
}

HE_VOID CHE_Bitmap::DrawLine( HE_DWORD nLine, HE_DWORD nStart, HE_DWORD nLength, HE_ARGB color )
{ 
	if ( m_lpBits == NULL || nLine >= Height() || nStart >= Width() || nLength == 0 )
	{
		return;
	}

	if ( nStart + nLength > Width() )
	{
		nLength = Width() - nStart;
	}
	
	HE_DWORD indexB = 0, indexE = 0;
	indexB = GetPixelByteIndex( nStart, nLine );
	indexE = GetPixelByteIndex( nStart + nLength, nLine );
	
	HE_DWORD indexColor;
	if ( m_lpPalette->GetNearColorIndex( color, indexColor ) )
	{
		m_lpPalette->GetColor( (HE_BYTE)indexColor, color );
	}
	
	switch ( Depth() )
	{
	case 1:
		{
			HE_DWORD index = 0;
			HE_BYTE oriByte = 0;
			HE_BYTE newByte = 0;
			for ( HE_DWORD x = nStart; x < nStart + nLength; x++ )
			{
				index = GetPixelByteIndex( x, nLine );
				oriByte = m_lpBits[index];
				newByte = (HE_BYTE)indexColor;
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
				m_lpBits[index] = newByte | oriByte;
			}
			break;
		}
	case 4:
		{
			HE_DWORD index = 0;
			HE_BYTE oriByte = 0;
			HE_BYTE desByte = 0;
			for ( HE_DWORD x = nStart; x < nStart + nLength; x++ )
			{
				index = GetPixelByteIndex( x, nLine );
				oriByte = m_lpBits[index];
				desByte = (HE_BYTE)indexColor;
				if ( x % 2 == 0 )
				{
					desByte <<= 4;
					oriByte &= 0x0F;
				}else{
					oriByte &= 0xF0;
				}
				m_lpBits[index] = desByte | oriByte;
			}
			break;
		}
	case 8:
		{
			for ( HE_DWORD i = indexB; i < indexE; i++ )
			{
				m_lpBits[i] = (HE_BYTE)indexColor;
			}
			break;
		}
	case 24:
		{
			for ( HE_DWORD i = indexB; i < indexE; i+=3 )
			{
				m_lpBits[i] = (HE_BYTE)( color & 0xFF );
				m_lpBits[i+1] = (HE_BYTE)( ( color >> 8 ) & 0xFF );
				m_lpBits[i+2] = (HE_BYTE)( ( color >> 16 ) & 0xFF );
			}
			break;
		}
	case 32:
		{
			for ( HE_DWORD i = indexB; i < indexE; i+=4 )
			{
				*((HE_LPDWORD)(m_lpBits+i)) = color;
			}
			break;
		}
	default:
		return;
	}
}

HE_VOID CHE_Bitmap::DrawLine( HE_DWORD nLine, HE_DWORD nStart, HE_DWORD nLength, HE_LPBYTE lpDataBuf, HE_DWORD nBufSize )
{ 
	if ( m_lpBits == NULL || nLine >= Height() || nStart >= Width() || nLength == 0 || lpDataBuf == NULL || nBufSize == 0 )
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

	HE_DWORD indexB = 0, indexE = 0;
	indexB = GetPixelByteIndex( nStart, nLine );
	indexE = GetPixelByteIndex( nStart + nLength, nLine );
	
	switch ( Depth() )
	{
	case 1:
		{
			HE_DWORD index = 0;
			HE_BYTE oriByte = 0;
			HE_BYTE newByte = 0;

			for ( HE_DWORD x = nStart; x < nStart + nLength; x++ )
			{
				index = GetPixelByteIndex( x, nLine );
				oriByte = m_lpBits[index];
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
				m_lpBits[index] = newByte | oriByte;
			}
			break;
		}
	case 4:
		{
			HE_DWORD index = 0;
			HE_BYTE oriByte = 0;
			HE_BYTE desByte = 0;
			for ( HE_DWORD x = nStart; x < nStart + nLength; x++ )
			{
				index = GetPixelByteIndex( x, nLine );
				desByte = m_lpBits[index];
				oriByte = lpDataBuf[index-indexB];
				if ( x % 2 == 0 )
				{
					desByte &= 0x0F;
					oriByte &= 0xF0;
				}else{
					desByte &= 0xF0;
					oriByte &= 0x0F; 
				}
				m_lpBits[index] = desByte | oriByte;
			}
			break;
		}
	case 8:
		{
			for ( HE_DWORD i = indexB, j = 0; i < indexE; i++, j++ )
			{
				m_lpBits[i] = lpDataBuf[j];
			}
			break;
		}
	case 24:
		{
			for ( HE_DWORD i = indexB, j = 0; i < indexE; i+=3, j+=3 )
			{
				m_lpBits[i] = lpDataBuf[j];
				m_lpBits[i+1] = lpDataBuf[j+1];
				m_lpBits[i+2] = lpDataBuf[j+2];
			}
			break;
		}
	case 32:
		{
			for ( HE_DWORD i = indexB, j = 0; i < indexE; i+=4, j+=4 )
			{
				*((HE_LPDWORD)(m_lpBits+i)) = *((HE_LPDWORD)(lpDataBuf+j));
			}
			break;
		}
	default:
		return;
	}
}

HE_BOOL CHE_Bitmap::SetChannel( HE_BITMAP_CHANNEL channel, HE_BYTE value )
{
	if ( ( Depth() < 24 ) || ( Depth() == 24 && channel == BITMAP_CHANNEL_Alpha ) )
	{
		return FALSE;
	}

	if ( m_lpBits == NULL )
	{
		return FALSE;
	}

	HE_INT32 iByteOffset = 0;
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
		return FALSE;
	}

	HE_DWORD iByteIndex = 0;
	HE_DWORD iPixelByteCount = Depth()>>3;
	for ( HE_DWORD iY = 0; iY < Height(); iY++ )
	{
		iByteIndex = GetPixelByteIndex( 0, iY );
		for ( HE_DWORD iX = 0; iX < Width(); iX++ )
		{
			m_lpBits[iByteIndex + ( iX * iPixelByteCount ) + iByteOffset] = value;
		}
	}
	return TRUE;
}

HE_BOOL CHE_Bitmap::SetChannelByAlpha( HE_BITMAP_CHANNEL channel, HE_BYTE alpha )
{
	if ( ( Depth() < 24 ) || ( Depth() == 24 && channel == BITMAP_CHANNEL_Alpha ) )
	{
		return FALSE;
	}
	
	if ( m_lpBits == NULL )
	{
		return FALSE;
	}
	
	HE_INT32 iByteOffset = 0;
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
		return FALSE;
	}
	
	HE_DWORD iByteIndex = 0;
	HE_DWORD iPixelByteCount = Depth()>>3;
	HE_FLOAT fTemp = 0.0;
	HE_FLOAT fAlpha = alpha;
	for ( HE_DWORD iY = 0; iY < Height(); iY++ )
	{
		iByteIndex = GetPixelByteIndex( 0, iY );
		for ( HE_DWORD iX = 0; iX < Width(); iX++ )
		{
			fTemp = m_lpBits[iByteIndex + ( iX * iPixelByteCount ) + iByteOffset];
			m_lpBits[iByteIndex + ( iX * iPixelByteCount ) + iByteOffset] = (unsigned char)(fTemp*(fAlpha/255)) ;
		}
	}
	return TRUE;
}

HE_BOOL CHE_Bitmap::ExchangeChannel( HE_BITMAP_CHANNEL channel1, HE_BITMAP_CHANNEL channel2 )
{
	if ( ( Depth() < 24 ) || 
		 ( Depth() == 24 && ( channel1 == BITMAP_CHANNEL_Alpha || channel2 == BITMAP_CHANNEL_Alpha ) ) ||
		 channel1 == channel2 )
	{
		return FALSE;
	}
	
	if ( m_lpBits == NULL )
	{
		return FALSE;
	}
	
	HE_INT32 iByteOffset1 = 0;
	HE_INT32 iByteOffset2 = 0;
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
		return FALSE;
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
		return FALSE;
	}
	
	HE_DWORD iByteIndex = 0;
	HE_DWORD iPixelByteCount = Depth()>>3;
	HE_BYTE iTemp = 0;
	for ( HE_DWORD iY = 0; iY < Height(); iY++ )
	{
		iByteIndex = GetPixelByteIndex( 0, iY );
		for ( HE_DWORD iX = 0; iX < Width(); iX++ )
		{
			iTemp = m_lpBits[iByteIndex + ( iX * iPixelByteCount ) + iByteOffset2];
			m_lpBits[iByteIndex + ( iX * iPixelByteCount ) + iByteOffset2] \
				= m_lpBits[iByteIndex + ( iX * iPixelByteCount ) + iByteOffset1];
			m_lpBits[iByteIndex + ( iX * iPixelByteCount ) + iByteOffset1] = iTemp;
		}
	}
	return TRUE;
}

HE_BOOL CHE_Bitmap::CopyChannel( HE_BITMAP_CHANNEL channelDes, HE_BITMAP_CHANNEL channelSrc )
{
	if ( ( Depth() < 24 ) || 
		 ( Depth() == 24 && ( channelDes == BITMAP_CHANNEL_Alpha || channelSrc == BITMAP_CHANNEL_Alpha ) ) ||
		 channelDes == channelSrc )
	{
		return FALSE;
	}
	
	if ( m_lpBits == NULL )
	{
		return FALSE;
	}
	
	HE_INT32 iByteOffsetDes = 0;
	HE_INT32 iByteOffsetSrc = 0;
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
		return FALSE;
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
		return FALSE;
	}
	
	HE_DWORD iByteIndex = 0;
	HE_DWORD iPixelByteCount = Depth()>>3;
	for ( HE_DWORD iY = 0; iY < Height(); iY++ )
	{
		iByteIndex = GetPixelByteIndex( 0, iY );
		for ( HE_DWORD iX = 0; iX < Width(); iX++ )
		{
			m_lpBits[iByteIndex + ( iX * iPixelByteCount ) + iByteOffsetDes] \
				= m_lpBits[iByteIndex + ( iX * iPixelByteCount ) + iByteOffsetSrc];
		}
	}
	return TRUE;
}

CHE_Bitmap* CHE_Bitmap::Clone( const HE_RECT* pRect ) const
{
	if ( m_lpBits == NULL )
	{
		return NULL;
	}

	if ( pRect  == NULL )
	{
		CHE_Bitmap * pNewBitmap = new CHE_Bitmap;
		pNewBitmap->Create( Width(), Height(), Depth(), Direction(), Pitch() * Height(), GetBuffer(), m_lpPalette );
		return pNewBitmap;
	}else{
		HE_RECT rect;
		rect.top = pRect->top;
		rect.left = pRect->left;
		rect.width = pRect->width;
		rect.height = pRect->height;
		
		if ( rect.top < 0 )
		{
			rect.top = 0;
		}
		if ( rect.left < 0 )
		{
			rect.left = 0;
		}
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
			CHE_Bitmap * pNewBitmap = new CHE_Bitmap;
			pNewBitmap->Create( Width(), Height(), Depth(), Direction(), Pitch() * Height(), GetBuffer(), m_lpPalette );
			return pNewBitmap;
		}

		switch ( Depth() )
		{
		case 1:
			{
				HE_DWORD lineBuffsize = ( ( ( rect.width * Depth() ) + 31 ) & ~31 ) >> 3;
				HE_LPBYTE lpLineData = new HE_BYTE[lineBuffsize];
				memset( lpLineData, 0, lineBuffsize );

				CHE_Bitmap * pNewBitmap = new CHE_Bitmap;
				pNewBitmap->Create( rect.width, rect.height, Depth(), Direction(), lineBuffsize * rect.height, NULL, m_lpPalette );

				HE_DWORD indexB = 0, indexE = 0;
				for ( HE_DWORD y = rect.top, desY = 0; y < rect.top + rect.height; y++, desY++ )
				{
					indexB = GetPixelByteIndex( rect.left, y );
					indexE = GetPixelByteIndex( rect.left + rect.width, y );

					HE_LONG nOffset = 8 - rect.left % 8;
					HE_DWORD tempDByte = 0;

					for ( HE_DWORD oriIndex = indexB, desIndex = 0; oriIndex <= indexE; oriIndex++, desIndex++ )
					{
						tempDByte = m_lpBits[oriIndex];
						tempDByte <<= 8;
						if ( oriIndex+1 <= indexE )
						{
							tempDByte += m_lpBits[oriIndex+1];
						}
						tempDByte >>= nOffset;
						lpLineData[desIndex] = (HE_BYTE)tempDByte;
					}
					pNewBitmap->DrawLine( desY, 0, rect.width, lpLineData, lineBuffsize );
					memset( lpLineData, 0, lineBuffsize );
				}
				delete [] lpLineData;
				return pNewBitmap;
				break;
			}
		case 4:
			{
				HE_DWORD lineBuffsize = ( ( ( rect.width * Depth() ) + 31 ) & ~31 ) >> 3;
				HE_LPBYTE lpLineData = new HE_BYTE[lineBuffsize];
				memset( lpLineData, 0, lineBuffsize );

				CHE_Bitmap * pNewBitmap = new CHE_Bitmap;
				pNewBitmap->Create( rect.width, rect.height, Depth(), Direction(), lineBuffsize * rect.height, NULL, m_lpPalette );

				HE_DWORD oriIndex = 0, desIndex = 0;
				HE_BYTE oriByte = 0;
				HE_BYTE newByte = 0;
				for ( HE_DWORD y = rect.top, desY = 0; y < rect.top + rect.height; y++, desY++ )
				{

					for ( HE_DWORD x = rect.left, desX = 0; x < rect.left + rect.width; x++, desX++ )
					{
						oriIndex = GetPixelByteIndex( x, y );
						desIndex = desX/2;
						
						oriByte = m_lpBits[oriIndex];
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
				delete [] lpLineData;
				return pNewBitmap;
				break;
			}
		case 8:
			{
				CHE_Bitmap * pNewBitmap = new CHE_Bitmap;
				pNewBitmap->Create( rect.width, rect.height, Depth(), Direction(), ( ( ( rect.width * Depth() ) + 31 ) & ~31 ) >> 3 * rect.height, NULL, m_lpPalette );
				HE_LPBYTE lpLineData = new HE_BYTE[( ( ( rect.width * Depth() ) + 31 ) & ~31 ) >> 3];

				HE_DWORD indexB = 0, indexE = 0;
				for ( HE_DWORD y = rect.top, desY = 0; y < rect.top + rect.height; y++, desY++ )
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
					for ( HE_DWORD oriIndex = indexB, desIndex = 0; oriIndex <= indexE; oriIndex++, desIndex++ )
					{
						lpLineData[desIndex] = m_lpBits[oriIndex];
					}
					pNewBitmap->DrawLine( desY, 0, rect.width, lpLineData, ( ( ( rect.width * Depth() ) + 31 ) & ~31 ) >> 3 );
				}
				delete [] lpLineData;
				return pNewBitmap;
				break;
			}
		case 24:
			{
				CHE_Bitmap * pNewBitmap = new CHE_Bitmap;
				pNewBitmap->Create( rect.width, rect.height, Depth(), Direction(), ( ( ( rect.width * Depth() ) + 31 ) & ~31 ) >> 3 * rect.height, NULL, m_lpPalette );
				HE_LPBYTE lpLineData = new HE_BYTE[( ( ( rect.width * Depth() ) + 31 ) & ~31 ) >> 3];
				
				HE_DWORD indexB = 0, indexE = 0;
				for ( HE_DWORD y = rect.top, desY = 0; y < rect.top + rect.height; y++, desY++ )
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
					for ( HE_DWORD oriIndex = indexB, desIndex = 0; oriIndex <= indexE; oriIndex++, desIndex++ )
					{
						lpLineData[desIndex] = m_lpBits[oriIndex];
					}
					pNewBitmap->DrawLine( desY, 0, rect.width, lpLineData, ( ( ( rect.width * Depth() ) + 31 ) & ~31 ) >> 3 );
				}
				delete [] lpLineData;
				return pNewBitmap;
				break;
			}
		case 32:
			{
				CHE_Bitmap * pNewBitmap = new CHE_Bitmap;
				pNewBitmap->Create( rect.width, rect.height, Depth(), Direction(), ( ( ( rect.width * Depth() ) + 31 ) & ~31 ) >> 3 * rect.height, NULL, m_lpPalette );
				HE_LPBYTE lpLineData = new HE_BYTE[( ( ( rect.width * Depth() ) + 31 ) & ~31 ) >> 3];
				
				HE_DWORD indexB = 0, indexE = 0;
				for ( HE_DWORD y = rect.top, desY = 0; y < rect.top + rect.height; y++, desY++ )
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
					for ( HE_DWORD oriIndex = indexB, desIndex = 0; oriIndex <= indexE; oriIndex++, desIndex++ )
					{
						lpLineData[desIndex] = m_lpBits[oriIndex];
					}
					pNewBitmap->DrawLine( desY, 0, rect.width, lpLineData, ( ( ( rect.width * Depth() ) + 31 ) & ~31 ) >> 3 );
				}
				delete [] lpLineData;
				return pNewBitmap;
				break;
			}
		default:
			return NULL;
		}
	}
	return NULL;
}

HE_BOOL CHE_Bitmap::CompositeMask( HE_ARGB color, HE_DWORD x, HE_DWORD y, CHE_Bitmap & maskBitmap )
{
	if ( Depth() < 24 || maskBitmap.GetBuffer() == NULL ||
		 ( maskBitmap.Depth() != BITMAP_DEPTH_1BPP && maskBitmap.Depth() != BITMAP_DEPTH_8BPP ) )
	{
		return FALSE;
	}

	HE_DWORD nWidth = maskBitmap.Width(), nHeight = maskBitmap.Height();
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
			HE_BYTE nMaskValue = 0;
			HE_DWORD byteIndex = 0;
			HE_FLOAT fTemp = 0.0;
			if ( maskBitmap.Depth() == 8 )
			{
				for ( HE_DWORD nY = 0; nY < nHeight; nY++ )
				{
					for ( HE_DWORD nX = 0; nX < nWidth; nX++ )
					{
						if ( maskBitmap.GetPixelIndex( nX, nY, nMaskValue ) )
						{
							fTemp = nMaskValue;
							fTemp /= 255;
							byteIndex = GetPixelByteIndex( x + nX, y + nY );
							m_lpBits[byteIndex] = (HE_BYTE)( (1-fTemp)*m_lpBits[byteIndex] + fTemp* (color&0xFF) );
							m_lpBits[byteIndex+1] = (HE_BYTE)( (1-fTemp)*m_lpBits[byteIndex+1] + fTemp* ((color>>8)&0xFF) );
							m_lpBits[byteIndex+2] = (HE_BYTE)( (1-fTemp)*m_lpBits[byteIndex+2] + fTemp* ((color>>16)&0xFF) );
						}
					}
				}
			}else{
				for ( HE_DWORD nY = 0; nY < nHeight; nY++ )
				{
					for ( HE_DWORD nX = 0; nX < nWidth; nX++ )
					{
						if ( maskBitmap.GetPixelIndex( nX, nY, nMaskValue ) )
						{
							byteIndex = GetPixelByteIndex( x + nX, y + nY );
							if ( nMaskValue == 0 )
							{
								m_lpBits[byteIndex] = (HE_BYTE)( color & 0xFF );
								m_lpBits[byteIndex+1] = (HE_BYTE)( ( color >> 8 ) & 0xFF );
								m_lpBits[byteIndex+2] = (HE_BYTE)( ( color >> 16 ) & 0xFF );
							}
						}
					}
				}
			}
			break;
		}
	case 32:
		{
			HE_BYTE nMaskValue = 0;
			HE_DWORD byteIndex = 0;
			HE_FLOAT fTemp = 0.0;
			if ( maskBitmap.Depth() == 8 )
			{
				for ( HE_DWORD nY = 0; nY < nHeight; nY++ )
				{
					for ( HE_DWORD nX = 0; nX < nWidth; nX++ )
					{
						if ( maskBitmap.GetPixelIndex( nX, nY, nMaskValue ) )
						{
							fTemp = nMaskValue;
							fTemp /= 255;
							byteIndex = GetPixelByteIndex( x + nX, y + nY );
							m_lpBits[byteIndex] = (HE_BYTE)( (1-fTemp)*m_lpBits[byteIndex] + fTemp* (color&0xFF) );
							m_lpBits[byteIndex+1] = (HE_BYTE)( (1-fTemp)*m_lpBits[byteIndex+1] + fTemp* ((color>>8)&0xFF) );
							m_lpBits[byteIndex+2] = (HE_BYTE)( (1-fTemp)*m_lpBits[byteIndex+2] + fTemp* ((color>>16)&0xFF) );
							m_lpBits[byteIndex+3] = (HE_BYTE)( (1-fTemp)*m_lpBits[byteIndex+3] + fTemp* ((color>>24)&0xFF) );
						}
					}
				}
			}else{
				for ( HE_DWORD nY = 0; nY < nHeight; nY++ )
				{
					for ( HE_DWORD nX = 0; nX < nWidth; nX++ )
					{
						if ( maskBitmap.GetPixelIndex( nX, nY, nMaskValue ) )
						{
							byteIndex = GetPixelByteIndex( x + nX, y + nY );
							if ( nMaskValue == 0 )
							{
								m_lpBits[byteIndex] = (HE_BYTE)( color & 0xFF );
								m_lpBits[byteIndex+1] = (HE_BYTE)( ( color >> 8 ) & 0xFF );
								m_lpBits[byteIndex+2] = (HE_BYTE)( ( color >> 16 ) & 0xFF );
								m_lpBits[byteIndex+3] = (HE_BYTE)( ( color >> 24 ) & 0xFF );
							}
						}
					}
				}
			}
			break;
		}
	default:
		return FALSE;
	}
	return TRUE;
		
}

HE_BOOL	CHE_Bitmap::Insert( const CHE_Bitmap & bitmap, HE_DWORD x, HE_DWORD y )
{
	if ( this->Depth() != bitmap.Depth() )
	{
		return FALSE;
	}
	
	if ( bitmap.m_lpBits == NULL || x >= this->Width() || y >= this->Height() )
	{
		return FALSE;
	}
	
	HE_RECT rtSrcBitmap;
	rtSrcBitmap.left = 0;
	rtSrcBitmap.top = 0;
	rtSrcBitmap.height = ((Height() - y) > bitmap.Height()) ? (bitmap.Height()) : (Height() - y);
	rtSrcBitmap.width = ((Width() - x) > bitmap.Width()) ? (bitmap.Width()) : (Width() - x);
	
	CHE_Bitmap * tempBitmap = bitmap.Clone( &rtSrcBitmap );
	if ( tempBitmap == NULL )
	{
		return FALSE;
	}
	
	HE_DWORD index = 0, lineDataSize = tempBitmap->Pitch();
	for ( HE_DWORD nLine = 0; nLine < rtSrcBitmap.height; nLine++ )
	{
		index = tempBitmap->GetPixelByteIndex( 0, nLine );
		DrawLine( nLine + y, x, tempBitmap->Width(), &(tempBitmap->m_lpBits[index]), lineDataSize );
	}
	
	delete tempBitmap;
	
	return TRUE;
}

// HE_BOOL CHE_FontBitmap::Load( HE_DWORD width, HE_DWORD height, HE_DWORD pitch, HE_BITMAP_FORMAT format, HE_BITMAP_ORIG flowOrig, HE_LPBYTE buffer )
// {
// 	if ( buffer == NULL || width <= 0 || height <= 0 )
// 	{
// 		return FALSE;
// 	}
// 
// 	HE_DWORD TurePitch;
// 	HE_BYTE depth;
// 	HE_BITMAP_FORMAT desFormat;
// 	HE_DWORD bitStremSize = 0;
// 	HE_ARGB* pPlatte = NULL;
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
// 		return FALSE;
// 	}
// 	TurePitch = ( ( ( width * depth ) + 31 ) & ~31 ) >> 3;
// 	bitStremSize = height*TurePitch;
// 
// 	HE_LPBYTE bitStrem = new unsigned char[bitStremSize];
// 
// 	if ( format == BITMAP_FORMAT_1BPPMask )
// 	{
// 		HE_DWORD irow = 0;
// 		for ( HE_DWORD j = 0; j < bitStremSize; j++ )
// 		 {
// 			irow = j/pitch;
// 			bitStrem[ j % pitch + ( height - irow - 1 ) * TurePitch ] = buffer[j];
// 		}
// 		pPlatte = new HE_ARGB[2];
// 		pPlatte[0] = 0xFFFFFFFF;
// 		pPlatte[1] = 0xFF000000;
// 	}else{
// 		for( HE_DWORD j = 0; j < height; j++ )
// 		{
// 			for( HE_DWORD i = 0; i < width; i++ )
// 			{
// 				bitStrem[ i + ( height - j - 1 ) * TurePitch ] = buffer[i + width * j];
// 			}
// 		}
// 		pPlatte = new HE_ARGB[256];
// 		
// 		for ( HE_LONG iPlatteIndex = 255, iPlatteValue = 0; iPlatteIndex >= 0; iPlatteIndex--, iPlatteValue++ )
// 		{
// 			pPlatte[iPlatteIndex] = (0xFF<< 24) + (iPlatteValue<<16) + (iPlatteValue<<8) + iPlatteValue;
// 		}
// 	}
// 
// 	Create( width, height, desFormat, BITMAP_ORIG_BOTTOM, bitStremSize, bitStrem, pPlatte );
// 
// 	delete [] pPlatte;
// 	pPlatte = NULL;
// 
// 	return TRUE;
// }

CHE_Bitmap* CHE_Bitmap::StretchTo( HE_DWORD desWidth, HE_DWORD desHeight, HE_DWORD flag, HE_RECT * pRect )
{
	CHE_Bitmap * pBitmapRet = new CHE_Bitmap;
	//ASSERT(pBitmapRet!=NULL);

	if ( !pBitmapRet->Create( desWidth, desHeight, this->Depth(), this->Direction() ) )
	{
		return NULL;	
	}

	HE_DOUBLE fWidthScale = 0.0, fHeightScale = 0.0;
	fWidthScale = ((HE_DOUBLE)desWidth) / this->Width();
	fHeightScale = ((HE_DOUBLE)desHeight) / this->Height();

	switch ( flag )
	{
	case 0:
		{
			HE_ARGB color = 0xFFFFFFFF;
			for ( HE_DWORD iY = 0; iY < desHeight; iY++ )
			{
				for ( HE_DWORD iX = 0; iX < desWidth; iX++ )
				{
					if ( GetPixelColor( (HE_DWORD)(iX/fWidthScale+0.5), (HE_DWORD)(iY/fHeightScale+0.5), color ) )
					{
						pBitmapRet->SetPixelColor( iX, iY, color );
					}
				}
			}
			break;
		}
	case 1:
		{
			HE_ARGB color = 0x00000000;
			HE_BYTE byteRed = 0x00;
			HE_BYTE byteGreen = 0x00;
			HE_BYTE byteBlue = 0x00;
			HE_BYTE bytePixelCross = 0;
			HE_BYTE bytePixelColorAlpha[4] = { 0, 0, 0, 0 };
			HE_ARGB pixelColor[4] = { 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF };
			HE_DWORD localX = 0, localY = 0;
			HE_DOUBLE fX = 0, fY = 0, fWidth = 1/fWidthScale, fHeight = 1/fHeightScale; 
			for ( HE_DWORD iY = 0; iY < desHeight; iY++ )
			{
				for ( HE_DWORD iX = 0; iX < desWidth; iX++ )
				{
					bytePixelCross = 0;

					fX = iX/fWidthScale;
					fY = iY/fHeightScale;

					localX = (HE_DWORD)(fX)+1;
					localY = (HE_DWORD)(fY)+1;

					if ( fX + fWidth <= localX )
					{
						if ( fY + fHeight <= localY )
						{
							bytePixelCross = 1;
							bytePixelColorAlpha[0] = 255;
							GetPixelColor( localX - 1, localY - 1, pixelColor[0] );
						}else{
							bytePixelCross = 2;
							bytePixelColorAlpha[0] = (fY+fHeight-localY) / (fHeight) * 255 + 0.5;
							bytePixelColorAlpha[1] = 255 - bytePixelColorAlpha[0];
							GetPixelColor( localX - 1, localY, pixelColor[0] );
							GetPixelColor( localX - 1, localY - 1, pixelColor[1] );
						}
					}else{
						if ( fY + fHeight <= localY )
						{
							bytePixelCross = 2;
							bytePixelColorAlpha[0] = (fX+fWidth-localX) / (fWidth) * 255 + 0.5;
							bytePixelColorAlpha[1] = 255 - bytePixelColorAlpha[0];
							GetPixelColor( localX, localY - 1, pixelColor[0] );
							GetPixelColor( localX - 1, localY - 1, pixelColor[1] );
						}else{
							bytePixelCross = 4;
							bytePixelColorAlpha[0] = (localX-fX)*(localY-fY) / (fWidth*fHeight) * 255 + 0.5;
							bytePixelColorAlpha[1] = (fX+fWidth-localX)*(localY-fY) / (fWidth*fHeight) * 255 + 0.5;
							bytePixelColorAlpha[2] = (localX-fX)*(fY+fHeight-localY) / (fWidth*fHeight) * 255 +0.5;
							bytePixelColorAlpha[3] = 255 - bytePixelColorAlpha[0] - bytePixelColorAlpha[1] - bytePixelColorAlpha[2];
							GetPixelColor( localX - 1, localY - 1, pixelColor[0] );
							GetPixelColor( localX, localY - 1, pixelColor[1] );
							GetPixelColor( localX - 1, localY, pixelColor[2] );
							GetPixelColor( localX, localY, pixelColor[3] );
						}
					}

					color = 0x00000000;
					byteRed = 0x00;
					byteGreen = 0x00;
					byteBlue = 0x00;
					for ( HE_BYTE i = 0; i < bytePixelCross; i++ )
					{
						byteRed += ( ( pixelColor[i] & 0x00FF0000 ) >> 16 ) * (bytePixelColorAlpha[i]/255.0) + 0.5;
						byteGreen += ( ( pixelColor[i] & 0x0000FF00 ) >> 8 ) * (bytePixelColorAlpha[i]/255.0) + 0.5;
						byteBlue += ( ( pixelColor[i] & 0x000000FF ) ) * (bytePixelColorAlpha[i]/255.0) + 0.5;
					}
					color = (byteRed << 16) + (byteGreen << 8) + byteBlue;

					pBitmapRet->SetPixelColor( iX, iY, color );
				}
			}
			break;
		}
	default:
		{
			pBitmapRet->Clean();
			delete pBitmapRet;
			return NULL;
		}
	}
	return pBitmapRet;
}

HE_DWORD CHE_Bitmap::GetPixelByteIndex( HE_DWORD x, HE_DWORD y ) const
{
	HE_DWORD index = 0;
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


