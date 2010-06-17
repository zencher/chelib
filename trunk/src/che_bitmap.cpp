#include <memory.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "../include/che_bitmap.h"


CHE_Palette::CHE_Palette( HE_BITMAP_TYPE type, const HE_ARGB * const pPalette )
{
	if (pPalette)
	{
		switch (type)
		{
		case HE_BITMAP_1BPPMask:
		case HE_BITMAP_1BPP:
			m_nPaletteSize = 2;
			break;
		case HE_BITMAP_4BPP:
			m_nPaletteSize = 16;
			break;
		case HE_BITMAP_8BPPMask:
		case HE_BITMAP_8BPP:
			m_nPaletteSize = 256;
			break;
		case HE_BITMAP_24BPP:
		case HE_BITMAP_32BPP:
		default:
			m_pPalette		= NULL;
			m_type			= HE_BITMAP_Invaild;
			m_nPaletteSize	= 0;
			break;
		}
		if (m_nPaletteSize)
		{
			m_pPalette = (HE_ARGB*)pPalette;/*new HE_ARGB[m_nPaletteSize];*/
			//memcpy( m_pPalette, pPalette, m_nPaletteSize );
			m_type = type;
		}
	}else{
		switch (type)
		{
		case HE_BITMAP_1BPPMask:
			{
				m_type = type;
				m_nPaletteSize = 2;
				m_pPalette = new HE_ARGB[2];
				m_pPalette[0] = 0x00FFFFFF;
				m_pPalette[1] = 0x00000000;
				break;
			}

		case HE_BITMAP_1BPP:
			{
				m_type = type;
				m_nPaletteSize = 2;
				m_pPalette = new HE_ARGB[2];
				m_pPalette[0] = 0x00000000;
				m_pPalette[1] = 0x00FFFFFF;
				break;
			}

		case HE_BITMAP_4BPP:
			{
				m_type = type;
				m_nPaletteSize = 16;
				m_pPalette = new HE_ARGB[16];
				m_pPalette[15] = 0x00FFFFFF;
				m_pPalette[14] = 0x00FF00FF;
				m_pPalette[13] = 0x00FFFF00;
				m_pPalette[12] = 0x0000FFFF;
				m_pPalette[11] = 0x000000FF;
				m_pPalette[10] = 0x0000FF00;
				m_pPalette[9] = 0x00FF0000;
				m_pPalette[8] = 0x00C0C0C0;
				m_pPalette[7] = 0x00808080;
				m_pPalette[6] = 0x00800080;
				m_pPalette[5] = 0x00808000;
				m_pPalette[4] = 0x00008080;
				m_pPalette[3] = 0x00000080;
				m_pPalette[2] = 0x00008000;
				m_pPalette[1] = 0x00800000;
				m_pPalette[0] = 0x00000000;
				break;
			}

		case HE_BITMAP_8BPPMask:
			{
				m_type = type;
				m_nPaletteSize = 256;
				m_pPalette = new HE_ARGB[256];
				for ( HE_INT32 i = 0; i < 256; i++ )
				{
					m_pPalette[i] = i<<16 & i<<8 & i;
				}
				break;
			}
		case HE_BITMAP_8BPP:
			{
				m_type = type;
				m_nPaletteSize = 256;
				m_pPalette = new HE_ARGB[256];
				static HE_ARGB tempPalette[256] = {	0x00000000, 0x00800000, 0x00008000, 0x00808000, 0x00000080, 0x00800080, 0x00008080, 0x00C0C0C0,
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
				memcpy( m_pPalette, pPalette, m_nPaletteSize );
				break;
			}
		case HE_BITMAP_24BPP:
		case HE_BITMAP_32BPP:
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
	m_type = palette.m_type;
	m_nPaletteSize = palette.m_nPaletteSize;
	if (m_nPaletteSize > 0 && palette.m_pPalette)
	{
		m_pPalette = new HE_ARGB[m_nPaletteSize];
		for (HE_DWORD i = 0; i < m_nPaletteSize; i++)
		{
			memcpy( m_pPalette, palette.m_pPalette, m_nPaletteSize );
		}
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
	m_type = palette.m_type;
	m_nPaletteSize = palette.m_nPaletteSize;
	if (m_pPalette)
	{
		delete m_pPalette;
		m_pPalette = NULL;
	}
	if (palette.m_pPalette)
	{
		m_pPalette = new HE_ARGB[m_nPaletteSize];
		memcpy( m_pPalette, palette.m_pPalette, m_nPaletteSize );
	}
	return *this;
}

HE_DWORD	CHE_Palette::ColorCount() const
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
		
		tempLessCY = abs( ( color & 0x000000FF ) - ( srcColor & 0x000000FF ) );
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

HE_BOOL	CHE_Palette::GetColor( HE_BYTE index, HE_ARGB & colorRet ) const
{
	if (index >= m_nPaletteSize)
	{
		return FALSE;
	}
	colorRet = m_pPalette[index];
	return TRUE;
}

HE_BOOL	CHE_Palette::SetColor( HE_BYTE index, HE_ARGB color )
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
	m_flowOrig = HE_BITMAP_FLOWORIG_BOTTOM;
	memset( &m_InfoHeader, 0, sizeof(HE_BITMAPINFOHEADER) );
}

CHE_Bitmap::~CHE_Bitmap()
{
	if ( m_lpBits )
	{
		delete m_lpBits;
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
	size_t lBytesReaded = fread( &bfHeader, 1, 14/*sizeof(HE_BITMAPFILEHEADER)*/, pfile );
	if ( lBytesReaded != 14/*sizeof(HE_BITMAPFILEHEADER)*/ )
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
	lBytesReaded = fread( &biHeader, 1, 40/*sizeof(HE_BITMAPINFOHEADER)*/, pfile );
	if ( lBytesReaded != 40/*sizeof(HE_BITMAPINFOHEADER)*/ )
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
	dwBitlen = biHeader.biSizeImage/*sizeof(HE_BITMAPINFOHEADER)+sizeof(HE_BITMAPFILEHEADER)*/;
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
		fseek( pfile, 54, SEEK_SET ); /*sizeof(HE_BITMAPINFOHEADER)+sizeof(HE_BITMAPFILEHEADER)*/
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

	HE_BITMAP_TYPE type;
	switch ( biHeader.biBitCount )
	{
	case 1:
		type = HE_BITMAP_1BPP;
		break;
	case 4:
		type = HE_BITMAP_4BPP;
		break;
	case 8:
		type = HE_BITMAP_8BPP;
		break;
	case 24:
		type = HE_BITMAP_24BPP;
		break;
	case 32:
		type = HE_BITMAP_32BPP;
		break;
	default:
		type = HE_BITMAP_Invaild;
	}
	m_lpPalette = new CHE_Palette( type, lpPalette );

	if ( m_InfoHeader.biHeight > 0 )
	{
		m_flowOrig = HE_BITMAP_FLOWORIG_BOTTOM;
	}else
	{
		m_flowOrig = HE_BITMAP_FLOWORIG_UP;
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
	bfHeader.bfSize = 54 + m_lpPalette->ColorCount() * 4 + Pitch() * Height();
	bfHeader.bfType = 0x4D42;
	bfHeader.bfReserved2 = 0;
	bfHeader.bfReserved1 = 0;
	bfHeader.bfOffBits = 54 + m_lpPalette->ColorCount() * 4;

	HE_DWORD lBytesReaded = fwrite( &bfHeader, 1, 14/*sizeof(HE_BITMAPFILEHEADER)*/, pfile );
	if ( lBytesReaded != 14/*sizeof(HE_BITMAPFILEHEADER)*/ )
	{
		fclose( pfile );
		return FALSE;
	}
	lBytesReaded = fwrite( &m_InfoHeader, 1,40 /*sizeof(HE_BITMAPINFOHEADER)*/, pfile );
	if ( lBytesReaded != 40/*sizeof(HE_BITMAPINFOHEADER)*/ )
	{
		fclose( pfile );
		return FALSE;
	}
	if ( m_lpPalette->ColorCount() > 0 )
	{
		lBytesReaded = fwrite( m_lpPalette->m_pPalette, 1, m_lpPalette->ColorCount() * 4, pfile );
		if ( lBytesReaded >> 2 != m_lpPalette->ColorCount() )
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

HE_BOOL CHE_Bitmap::Create( HE_DWORD width, HE_DWORD height, HE_BITMAP_TYPE type, HE_BITMAP_FLOWORIG flowOrig, HE_DWORD bufferSize, HE_LPCBYTE buffer, const HE_ARGB* Palette )
{
	HE_BITMAPINFOHEADER biHeader;
	biHeader.biSize = 40/*sizeof(HE_BITMAPINFOHEADER)*/;
	biHeader.biWidth = width;
	HE_LONG lHeight = height;
	biHeader.biHeight = ( flowOrig == HE_BITMAP_FLOWORIG_BOTTOM ) ? (height) : (-lHeight);
	biHeader.biPlanes = 1;
	biHeader.biCompression = HE_BIBITMAP_RGB;
	biHeader.biClrImportant = 0;
	biHeader.biClrUsed = 0;
	biHeader.biXPelsPerMeter = 0;
	biHeader.biYPelsPerMeter = 0;
	biHeader.biBitCount = 0;
	biHeader.biSizeImage = 0;

	HE_SHORT iPaletteSize = 0;
	HE_ARGB* lpPalette = NULL;

	switch ( type )
	{
	case HE_BITMAP_1BPP:
		{
			biHeader.biBitCount = 1;
			biHeader.biSizeImage = ( ( ( width + 31 ) & ~31 ) >> 3 ) * height ;
			iPaletteSize = 2;
			lpPalette = new HE_ARGB[2];
			if ( Palette == NULL )
			{
				lpPalette[0] = 0x00FFFFFF;
				lpPalette[1] = 0x00000000;
			}else{
				lpPalette[0] = Palette[0];
				lpPalette[1] = Palette[1];
			}
			break;
		}
	case HE_BITMAP_4BPP:
		{
			biHeader.biBitCount = 4;
			biHeader.biSizeImage = ( ( ( width * 4 + 31 ) & ~31 ) >> 3 ) * height;
			iPaletteSize = 16;
			lpPalette = new HE_ARGB[16];
			if ( Palette == NULL )
			{
				lpPalette[0] = 0x00FFFFFF;
				lpPalette[1] = 0x00FF00FF;
				lpPalette[2] = 0x00FFFF00;
				lpPalette[3] = 0x0000FFFF;
				lpPalette[4] = 0x000000FF;
				lpPalette[5] = 0x0000FF00;
				lpPalette[6] = 0x00FF0000;
				lpPalette[7] = 0x00C0C0C0;
				lpPalette[8] = 0x00808080;
				lpPalette[9] = 0x00800080;
				lpPalette[10] = 0x00808000;
				lpPalette[11] = 0x00008080;
				lpPalette[12] = 0x00000080;
				lpPalette[13] = 0x00008000;
				lpPalette[14] = 0x00800000;
				lpPalette[15] = 0x00000000;
			}else{
				for ( HE_BYTE i = 0; i < 16; i++ )
				{
					lpPalette[i] = Palette[i];
				}
			}
			break;
		}
	case HE_BITMAP_8BPP:
		{
			biHeader.biBitCount = 8;
			biHeader.biSizeImage = ( ( ( width * 8 + 31 ) & ~31 ) >> 3 ) * height;
			iPaletteSize = 256;
			lpPalette = new HE_ARGB[256];
			if ( Palette == NULL )
			{
				static HE_ARGB tempPalette[256] = {	0x00000000, 0x00800000, 0x00008000, 0x00808000, 0x00000080, 0x00800080, 0x00008080, 0x00C0C0C0,
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

				for ( HE_SHORT i = 0; i <= 255; i++ )
				{
					lpPalette[i] = tempPalette[i];
				}	
			}else
			{
				for ( HE_SHORT i = 0; i <= 255; i++ )
				{
					lpPalette[i] = Palette[i];
				}			
			}
			break;
		}
	case HE_BITMAP_24BPP:
		{
			biHeader.biBitCount = 24;
			biHeader.biSizeImage = ( ( ( width * 24 + 31 ) & ~31 ) >> 3 ) * height;
			break;
		}
	case HE_BITMAP_32BPP:
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
	m_lpPalette = new CHE_Palette( type, lpPalette );

	return TRUE;
}

HE_BITMAP_TYPE CHE_Bitmap::Type() const
{
	if ( m_lpBits == NULL )
	{
		return HE_BITMAP_Invaild;
	}

	switch ( m_InfoHeader.biBitCount )
	{
	case 1:
		return HE_BITMAP_1BPP;
	case 4:
		return HE_BITMAP_4BPP;
	case 8:
		return HE_BITMAP_8BPP;
	case 24:
		return HE_BITMAP_24BPP;
	case 32:
		return HE_BITMAP_32BPP;
	default:
		return HE_BITMAP_Invaild;
	}
}

HE_VOID	CHE_Bitmap::Clean()
{
	if ( m_lpBits )
	{
		delete m_lpBits;
		m_lpBits = NULL;
	}
	if ( m_lpPalette )
	{
		delete m_lpPalette;
		m_lpPalette = NULL;
	}
	memset( &m_InfoHeader, 0, sizeof(HE_BITMAPINFOHEADER) );
}

// HE_WORD CHE_Bitmap::GetPaletteSize() const
// {
// 	if ( m_InfoHeader.biBitCount == 8 )
// 	{
// 		return 256;
// 	}else if ( m_InfoHeader.biBitCount == 4 )
// 	{
// 		return 16;
// 	}else if ( m_InfoHeader.biBitCount == 1 )
// 	{
// 		return 2;
// 	}else{
// 		return 0;
// 	}
// }

// HE_ARGB	CHE_Bitmap::GetPaletteArgb( HE_WORD index ) const
// {
// 	HE_DWORD iPaletteSize = GetPaletteSize();
// 	if ( ( index >= iPaletteSize ) || index < 0 )
// 	{
// 		return 0;
// 	}
// 	return m_lpPalette[index];
// }

// HE_SHORT CHE_Bitmap::GetNearArgbInPalette( HE_ARGB color ) const
// {
// 	if ( GetPaletteSize() == 0 )
// 	{
// 		return -1;
// 	}
// 	HE_SHORT index = 0;
// 	HE_WORD lessCY = 0xFFFF;
// 	HE_WORD tempLessCY = 0;
// 	HE_ARGB srcColor;
// 
// 	for ( HE_SHORT i = 0; i < GetPaletteSize(); i++ )
// 	{
// 		srcColor = m_lpPalette[i];
// 
// 		tempLessCY = abs( ( color & 0x000000FF ) - ( srcColor & 0x000000FF ) );
// 		tempLessCY += abs( ( color >> 8 & 0x000000FF ) - ( srcColor >> 8 & 0x000000FF ) );
// 		tempLessCY += abs( ( color >> 16 & 0x000000FF ) - ( srcColor >> 16 & 0x000000FF ) );
// 		tempLessCY += abs( ( color >> 24 & 0x000000FF ) - ( srcColor >> 24 & 0x000000FF ) );
// 		if ( tempLessCY < lessCY )
// 		{
// 			lessCY = tempLessCY;
// 			index = i;
// 		}
// 	}
// 	return index;
// }

// HE_BOOL	CHE_Bitmap::FindPaletteArgb( HE_ARGB argb, HE_LPBYTE pIndexRet ) const
// {
// 	if ( GetPaletteSize() == 0 )
// 	{
// 		return FALSE;
// 	}
// 	for ( HE_BYTE i = 0; i < GetPaletteSize(); i++ )
// 	{
// 		if ( argb == m_lpPalette[i] )
// 		{
// 			if ( pIndexRet )
// 			{
// 				*pIndexRet = i;
// 			}
// 			return TRUE;
// 		}
// 	}
// 	return FALSE;
// }

// HE_VOID CHE_Bitmap::SetPaletteArgb( HE_BYTE index, HE_ARGB argb )
// {
// 	if ( index >= GetPaletteSize() || index < 0 )
// 	{
// 		return;
// 	}
// 	m_lpPalette[index] = argb;
// }

HE_ARGB	CHE_Bitmap::GetPixel( HE_DWORD x, HE_DWORD y ) const
{
	if ( x >= Width() )
	{
		return 0;
	}
	if ( y >= Height() )
	{
		return 0;
	}

	HE_DWORD index = 0;
	if ( FlowOrig() == HE_BITMAP_FLOWORIG_BOTTOM )
	{
		index = ( Height() - y - 1 ) * Pitch();
		index += ( x * Depth() ) >> 3;
	}else{
		index = y * Pitch();
		index += ( x * Depth() ) >> 3;
	}

	HE_ARGB clrRet = 0;
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
	return clrRet;
}

HE_BOOL CHE_Bitmap::SetPixel( HE_DWORD x, HE_DWORD y, HE_ARGB color )
{
	if ( x >= Width() )
	{
		return FALSE;
	}
	if ( y >= Height() )
	{
		return FALSE;
	}

	HE_DWORD index = 0;	
	if ( FlowOrig() == HE_BITMAP_FLOWORIG_BOTTOM )
	{
		index = ( Height() - 1 - y ) * Pitch();
		index += ( x * Depth() ) >> 3;
	}else{
		index = y * Pitch();
		index += ( x * Depth() ) >> 3;
	}

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
		//tempIndex = GetNearArgbInPalette( color );
		switch ( x % 8 )
		{
		case 0:
			platteIndex &= 0x7F;
			tempIndex <<= 7;
			break;
		case 1:
			platteIndex &= 0xBF;
			tempIndex <<= 6;
			break;
		case 2:
			platteIndex &= 0xDF;
			tempIndex <<= 5;
			break;
		case 3:
			platteIndex &= 0xEF;
			tempIndex <<= 4;
			break;
		case 4:
			platteIndex &= 0xF7;
			tempIndex <<= 3;
			break;
		case 5:
			platteIndex &= 0xFB;
			tempIndex <<= 2;
			break;
		case 6:
			platteIndex &= 0xFD;
			tempIndex <<= 1;
			break;
		case 7:
			platteIndex &= 0xFE;
			break;
		default:
			break;
		}
		m_lpBits[index] = (HE_BYTE)tempIndex + platteIndex;
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
			for ( HE_DWORD i = 0; i < Pitch() * Height(); i+=3 )
			{
				m_lpBits[i] = (HE_BYTE)( color & 0xFF );
				m_lpBits[i+1] = (HE_BYTE)( ( color >> 8 ) & 0xFF );
				m_lpBits[i+2] = (HE_BYTE)( ( color >> 16 ) & 0xFF );
			}
			break;
		}
	case 32:
		{
			for ( HE_DWORD i = 0; i < Pitch() * Height(); i+=4 )
			{
				((HE_LPDWORD)(m_lpBits))[i] = color;
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
	if ( m_lpBits == NULL )
	{
		return;
	}
	
	HE_DWORD indexB = 0, indexE = 0;
	if ( FlowOrig() == HE_BITMAP_FLOWORIG_BOTTOM )
	{
		indexB = ( Height() - 1 - nLine ) * Pitch();
		indexE = indexB;
		indexB += ( nStart * Depth() ) >> 3;
		indexE += ( ( nStart + nLength - 1 ) * Depth() ) >> 3;
	}else{
		indexB = nLine * Pitch();
		indexE = indexB;
		indexB += ( nStart * Depth() ) >> 3;
		indexE += ( ( nStart + nLength - 1 ) * Depth() ) >> 3;
	}
	
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
			HE_BYTE desByte = 0;
			for ( HE_DWORD x = nStart; x < nStart + nLength; x++ )
			{
				if ( FlowOrig() == HE_BITMAP_FLOWORIG_BOTTOM )
				{
					index = ( Height() - 1 - nLine ) * Pitch();
					index += ( x * Depth() ) >> 3;
				}else{
					index = nLine * Pitch();
					index += ( x * Depth() ) >> 3;
				}

				oriByte = m_lpBits[index];
				desByte = (HE_BYTE)indexColor;
				switch ( x % 8 )
				{
				case 0:
					oriByte &= 0x7F;
					desByte <<= 7;
					break;
				case 1:
					oriByte &= 0xBF;
					desByte <<= 6;
					break;
				case 2:
					oriByte &= 0xDF;
					desByte <<= 5;
					break;
				case 3:
					oriByte &= 0xEF;
					desByte <<= 4;
					break;
				case 4:
					oriByte &= 0xF7;
					desByte <<= 3;
					break;
				case 5:
					oriByte &= 0xFB;
					desByte <<= 2;
					break;
				case 6:
					oriByte &= 0xFD;
					desByte <<= 1;
					break;
				case 7:
					oriByte &= 0xFE;
					break;
				default:
					break;
				}
				m_lpBits[index] = desByte + oriByte;
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
				
				if ( FlowOrig() == HE_BITMAP_FLOWORIG_BOTTOM )
				{
					index = ( Height() - 1 - nLine ) * Pitch();
					index += ( x * Depth() ) >> 3;
				}else{
					index = nLine * Pitch();
					index += ( x * Depth() ) >> 3;
				}

				oriByte = m_lpBits[index];
				desByte = (HE_BYTE)indexColor;
				if ( x % 2 == 0 )
				{
					desByte <<= 4;
					oriByte &= 0x0F;
				}else{
					oriByte &= 0xF0;
				}
				m_lpBits[index] = desByte+oriByte;
			}
			break;
		}
	case 8:
		{
			for ( HE_DWORD i = indexB; i <= indexE; i++ )
			{
				m_lpBits[i] = (HE_BYTE)indexColor;
			}
			break;
		}
	case 24:
		{
			for ( HE_DWORD i = indexB; i <= indexE; i+=3 )
			{
				m_lpBits[i] = (HE_BYTE)( color & 0xFF );
				m_lpBits[i+1] = (HE_BYTE)( ( color >> 8 ) & 0xFF );
				m_lpBits[i+2] = (HE_BYTE)( ( color >> 16 ) & 0xFF );
			}
			break;
		}
	case 32:
		{
			for ( HE_DWORD i = indexB; i <= indexE; i+=4 )
			{
				m_lpBits[i] = (HE_BYTE)( color & 0xFF );
				m_lpBits[i+1] = (HE_BYTE)( ( color >> 8 ) & 0xFF );
				m_lpBits[i+2] = (HE_BYTE)( ( color >> 16 ) & 0xFF );
				m_lpBits[i+3] = (HE_BYTE)( ( color >> 24 ) & 0xFF );
			}
			break;
		}
	default:
		return;
	}
}

HE_VOID CHE_Bitmap::DrawLine( HE_DWORD nLine, HE_DWORD nStart, HE_DWORD nLength, HE_LPBYTE lpDataBuf, HE_DWORD nBufSize )
{ 
	if ( m_lpBits == NULL || lpDataBuf == NULL || nBufSize == 0 )
	{
		return;
	}

	if ( ( nLength * Depth()) > (nBufSize << 3) )
	{
		nLength = ( nBufSize << 3 ) / Depth();
	}

	HE_DWORD indexB = 0, indexE = 0;
	if ( FlowOrig() == HE_BITMAP_FLOWORIG_BOTTOM )
	{
		indexB = ( Height() - 1 - nLine ) * Pitch();
		indexE = indexB;
		indexB += ( nStart * Depth() ) >> 3;
		indexE += ( ( nStart + nLength - 1 ) * Depth() ) >> 3;
	}else{
		indexB = nLine * Pitch();
		indexE = indexB;
		indexB += ( nStart * Depth() ) >> 3;
		indexE += ( ( nStart + nLength - 1 ) * Depth() ) >> 3;
	}
	
	switch ( Depth() )
	{
	case 1:
		{
			HE_DWORD index = 0;
			HE_BYTE oriByte = 0;
			HE_BYTE desByte = 0;

			for ( HE_DWORD x = nStart; x < nStart + nLength; x++ )
			{
				if ( FlowOrig() == HE_BITMAP_FLOWORIG_BOTTOM )
				{
					index = ( Height() - 1 - nLine ) * Pitch();
					index += ( x * Depth() ) >> 3;
				}else{
					index = nLine * Pitch();
					index += ( x * Depth() ) >> 3;
				}
				
				oriByte = m_lpBits[index];
				desByte = lpDataBuf[index-indexB];
				switch ( x % 8 )
				{
				case 0:
					oriByte &= 0x7F;
					desByte &= 0x80;
					break;
				case 1:
					oriByte &= 0xBF;
					desByte &= 0x40;
					break;
				case 2:
					oriByte &= 0xDF;
					desByte &= 0x20;
					break;
				case 3:
					oriByte &= 0xEF;
					desByte &= 0x10;
					break;
				case 4:
					oriByte &= 0xF7;
					desByte &= 0x08;
					break;
				case 5:
					oriByte &= 0xFB;
					desByte &= 0x04;
					break;
				case 6:
					oriByte &= 0xFD;
					desByte &= 0x02;
					break;
				case 7:
					oriByte &= 0xFE;
					desByte &= 0x01;
					break;
				default:
					break;
				}
				m_lpBits[index] = desByte + oriByte;
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
				
				if ( FlowOrig() == HE_BITMAP_FLOWORIG_BOTTOM )
				{
					index = ( Height() - 1 - nLine ) * Pitch();
					index += ( x * Depth() ) >> 3;
				}else{
					index = nLine * Pitch();
					index += ( x * Depth() ) >> 3;
				}
				
				oriByte = m_lpBits[index];
				desByte = lpDataBuf[index-indexB];
				if ( x % 2 == 0 )
				{
					desByte &= 0xF0;
					oriByte &= 0x0F; 
				}else{
					oriByte &= 0xF0;
					desByte &= 0x0F;
				}
				m_lpBits[index] = desByte+oriByte;
			}
			break;
		}
	case 8:
		{
			for ( HE_DWORD i = indexB, j = 0; i <= indexE; i++, j++ )
			{
				m_lpBits[i] = lpDataBuf[j];
			}
			break;
		}
	case 24:
		{
			for ( HE_DWORD i = indexB, j = 0; i <= indexE; i+=3, j+=3 )
			{
				m_lpBits[i] = lpDataBuf[j];
				m_lpBits[i+1] = lpDataBuf[j+1];
				m_lpBits[i+2] = lpDataBuf[j+2];
			}
			break;
		}
	case 32:
		{
			for ( HE_DWORD i = indexB, j = 0; i <= indexE; i+=4, j+=4 )
			{
				m_lpBits[i] = lpDataBuf[j];
				m_lpBits[i+1] = lpDataBuf[j+1];
				m_lpBits[i+2] = lpDataBuf[j+2];
				m_lpBits[i+3] = lpDataBuf[j+3];
			}
			break;
		}
	default:
		return;
	}
}

CHE_Bitmap* CHE_Bitmap::Clone( const HE_RECT* pRect ) const
{
	if ( Type() == HE_BITMAP_Invaild || m_lpBits == NULL )
	{
		return NULL;
	}

	if ( pRect  == NULL )
	{
		CHE_Bitmap * pNewBitmap = new CHE_Bitmap;
		pNewBitmap->Create( Width(), Height(), Type(), FlowOrig(), Pitch() * Height(), GetBuffer(), m_lpPalette->m_pPalette );
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
			pNewBitmap->Create( Width(), Height(), Type(), FlowOrig(), Pitch() * Height(), GetBuffer(), m_lpPalette->m_pPalette );
			return pNewBitmap;
		}

		switch ( Depth() )
		{
		case 1:
			{
				CHE_Bitmap * pNewBitmap = new CHE_Bitmap;
				pNewBitmap->Create( rect.width, rect.height, Type(), FlowOrig(), ( ( ( rect.width * Depth() ) + 31 ) & ~31 ) >> 3 * rect.height, NULL, m_lpPalette->m_pPalette );
				HE_DWORD buffsize = ( ( ( rect.width * Depth() ) + 31 ) & ~31 ) >> 3;
				HE_LPBYTE lpLineData = new HE_BYTE[buffsize];
				memset( lpLineData, 0, buffsize );

				HE_DWORD indexB = 0, indexE = 0;
				for ( HE_DWORD y = rect.top, desY = 0; y < rect.top + rect.height; y++, desY++ )
				{
					if ( FlowOrig() == HE_BITMAP_FLOWORIG_BOTTOM )
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

					HE_LONG nOffset = 8 - rect.left % 8;
					HE_DWORD tempDByte = 0;

					for ( HE_DWORD oriIndex = indexB, desIndex = 0; oriIndex <= indexE; oriIndex++, desIndex++ )
					{
						tempDByte = m_lpBits[oriIndex];
						tempDByte <<= 8;
						tempDByte += m_lpBits[oriIndex+1];
						tempDByte >>= nOffset;
						lpLineData[desIndex] = (HE_BYTE)tempDByte;
					}
					pNewBitmap->DrawLine( desY, 0, rect.width, lpLineData, ( ( ( rect.width * Depth() ) + 31 ) & ~31 ) >> 3 );
				}
				delete [] lpLineData;
				return pNewBitmap;
				break;
			}
		case 4:
			{
				CHE_Bitmap * pNewBitmap = new CHE_Bitmap;
				pNewBitmap->Create( rect.width, rect.height, Type(), FlowOrig(), ( ( ( rect.width * Depth() ) + 31 ) & ~31 ) >> 3 * rect.height, NULL, m_lpPalette->m_pPalette );
				HE_DWORD buffsize = ( ( ( rect.width * Depth() ) + 31 ) & ~31 ) >> 3;
				HE_LPBYTE lpLineData = new HE_BYTE[buffsize];
				memset( lpLineData, 0, buffsize );

				HE_DWORD oriIndex = 0, desIndex = 0;
				HE_BYTE oriByte = 0;
				HE_BYTE desByte = 0;
				for ( HE_DWORD y = rect.top, desY = 0; y < rect.top + rect.height; y++, desY++ )
				{
					desIndex = 0;
					HE_DWORD rset = 0;
					for ( HE_DWORD x = rect.left; x <= rect.left + rect.width; x++ )
					{
						rset++;
						if ( FlowOrig() == HE_BITMAP_FLOWORIG_BOTTOM )
						{
							oriIndex = ( Height() - 1 - y ) * Pitch();
							oriIndex += ( x * Depth() ) >> 3;
						}else{
							oriIndex = y * Pitch();
							oriIndex += ( x * Depth() ) >> 3;
						}
						
						oriByte = m_lpBits[oriIndex];
						desByte = lpLineData[desIndex];
						if ( x % 2 == 0 )
						{
							oriByte &= 0x0F;
							desByte &= 0xF0;
						}else{
							oriByte &= 0xF0;
							desByte &= 0x0F;
						}
						lpLineData[desIndex] = desByte | oriByte;
						if ( rset == 2 )
						{
							rset = 0;
							desIndex++;
						}
					}
					pNewBitmap->DrawLine( desY, 0, rect.width, lpLineData, ( ( ( rect.width * Depth() ) + 31 ) & ~31 ) >> 3 );
					memset( lpLineData, 0, ( ( ( rect.width * Depth() ) + 31 ) & ~31 ) >> 3 );
				}
				delete [] lpLineData;
				return pNewBitmap;
				break;
			}
		case 8:
			{
				CHE_Bitmap * pNewBitmap = new CHE_Bitmap;
				pNewBitmap->Create( rect.width, rect.height, Type(), FlowOrig(), ( ( ( rect.width * Depth() ) + 31 ) & ~31 ) >> 3 * rect.height, NULL, m_lpPalette->m_pPalette );
				HE_LPBYTE lpLineData = new HE_BYTE[( ( ( rect.width * Depth() ) + 31 ) & ~31 ) >> 3];

				HE_DWORD indexB = 0, indexE = 0;
				for ( HE_DWORD y = rect.top, desY = 0; y < rect.top + rect.height; y++, desY++ )
				{
					if ( FlowOrig() == HE_BITMAP_FLOWORIG_BOTTOM )
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
				pNewBitmap->Create( rect.width, rect.height, Type(), FlowOrig(), ( ( ( rect.width * Depth() ) + 31 ) & ~31 ) >> 3 * rect.height, NULL, m_lpPalette->m_pPalette );
				HE_LPBYTE lpLineData = new HE_BYTE[( ( ( rect.width * Depth() ) + 31 ) & ~31 ) >> 3];
				
				HE_DWORD indexB = 0, indexE = 0;
				for ( HE_DWORD y = rect.top, desY = 0; y < rect.top + rect.height; y++, desY++ )
				{
					if ( FlowOrig() == HE_BITMAP_FLOWORIG_BOTTOM )
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
				pNewBitmap->Create( rect.width, rect.height, Type(), FlowOrig(), ( ( ( rect.width * Depth() ) + 31 ) & ~31 ) >> 3 * rect.height, NULL, m_lpPalette->m_pPalette );
				HE_LPBYTE lpLineData = new HE_BYTE[( ( ( rect.width * Depth() ) + 31 ) & ~31 ) >> 3];
				
				HE_DWORD indexB = 0, indexE = 0;
				for ( HE_DWORD y = rect.top, desY = 0; y < rect.top + rect.height; y++, desY++ )
				{
					if ( FlowOrig() == HE_BITMAP_FLOWORIG_BOTTOM )
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

HE_BOOL CHE_FontBitmap::Load( HE_DWORD width, HE_DWORD height, HE_DWORD pitch, HE_BITMAP_TYPE format, HE_BITMAP_FLOWORIG flowOrig, HE_LPBYTE buffer )
{
	if ( buffer == NULL || width <= 0 || height <= 0 )
	{
		return FALSE;
	}

	HE_DWORD TurePitch;
	HE_BYTE depth;
	HE_BITMAP_TYPE desFormat;
	HE_DWORD bitStremSize = 0;
	HE_ARGB* pPlatte = NULL;

	if ( format == HE_BITMAP_1BPPMask )
	{
		desFormat = HE_BITMAP_1BPP;
		depth = 1;
	}else if ( format == HE_BITMAP_8BPPMask )
	{
		desFormat = HE_BITMAP_8BPP;
		depth = 8;
	}else{
		return FALSE;
	}
	TurePitch = ( ( ( width * depth ) + 31 ) & ~31 ) >> 3;
	bitStremSize = height*TurePitch;

	HE_LPBYTE bitStrem = new unsigned char[bitStremSize];

	if ( format == HE_BITMAP_1BPPMask )
	{
		HE_DWORD irow = 0;
		for ( HE_DWORD j = 0; j < bitStremSize; j++ )
		 {
			irow = j/pitch;
			bitStrem[ j % pitch + ( height - irow - 1 ) * TurePitch ] = buffer[j];
		}
		pPlatte = new HE_ARGB[2];
		pPlatte[0] = 0xFFFFFFFF;
		pPlatte[1] = 0xFF000000;
	}else{
		for( HE_DWORD j = 0; j < height; j++ )
		{
			for( HE_DWORD i = 0; i < width; i++ )
			{
				bitStrem[ i + ( height - j - 1 ) * TurePitch ] = buffer[i + width * j];
			}
		}
		pPlatte = new HE_ARGB[256];
		
		for ( HE_LONG iPlatteIndex = 255, iPlatteValue = 0; iPlatteIndex >= 0; iPlatteIndex--, iPlatteValue++ )
		{
			pPlatte[iPlatteIndex] = (0xFF<< 24) + (iPlatteValue<<16) + (iPlatteValue<<8) + iPlatteValue;
		}
	}

	Create( width, height, desFormat, HE_BITMAP_FLOWORIG_BOTTOM, bitStremSize, bitStrem, pPlatte );


	delete [] pPlatte;
	pPlatte = NULL;

	return TRUE;
}

HE_BOOL	CHE_Bitmap::Insert( const CHE_Bitmap & bitmap, HE_DWORD x, HE_DWORD y )
{
	if ( bitmap.m_lpBits == NULL || x >= this->Width() || y >= this->Height() )
	{
		return FALSE;
	}
	
	if ( this->Depth() != bitmap.Depth() )
	{
		//目前还没有不同深度的位图间的转换方法，所以暂时返回假，等待以后添加转换处理。
		return FALSE;
	}
	
	HE_RECT rtSrcBitmap;
	rtSrcBitmap.left = 0;
	rtSrcBitmap.top = 0;
	rtSrcBitmap.height = ((Height() - y ) > bitmap.Height()) ? (bitmap.Height()) : (Height() - y );
	rtSrcBitmap.width = ((Width() - x ) > bitmap.Width()) ? (bitmap.Width()) : (Width() - x );
	
	CHE_Bitmap * tempBitmap = bitmap.Clone( &rtSrcBitmap );
	if ( tempBitmap == NULL )
	{
		return FALSE;
	}

	if ( Depth() <= 8 )
	{
		tempBitmap->ChangePalette( *m_lpPalette );
	}
	
	HE_DWORD index = 0, lineDataSize = ( ( ( rtSrcBitmap.width * tempBitmap->Depth() ) + 31 ) & ~31 ) >> 3 ;
	for ( HE_DWORD nLine = 0; nLine < rtSrcBitmap.height; nLine++ )
	{
		if ( tempBitmap->FlowOrig() == HE_BITMAP_FLOWORIG_BOTTOM )
		{
			index = ( tempBitmap->Height() - 1 - nLine ) * tempBitmap->Pitch();
		}else{
			index = nLine * tempBitmap->Pitch();
		}
		DrawLine( nLine + y, x, rtSrcBitmap.width, &(tempBitmap->m_lpBits[index]), lineDataSize );
	}
	
	delete tempBitmap;
	
	return TRUE;
}

HE_VOID	CHE_Bitmap::ChangePalette( const CHE_Palette & palette )
{
	switch ( Depth() )
	{
	case 1:
		{

			break;
		}
	case 4:
		{

			break;
		}
	case 8:
		{
			CHE_Palette * OriPalette = GetPalette();
			HE_DWORD Colorindex = 0;
			HE_DWORD indexB = 0, indexE = 0;
			HE_ARGB tempColor;
			HE_DWORD colorIndex;
			for ( HE_DWORD y = 0; y < Height(); y++ )
			{
				indexB = GetByteIndexB( 0, y );
				indexE = GetByteIndexE( 0, y, Width() );
				for ( HE_DWORD i = indexB; i <= indexE; i++ )
				{
					if ( m_lpPalette->GetColor( m_lpBits[i], tempColor ) )
					{
						if ( palette.GetNearColorIndex( tempColor, colorIndex ) )
						{
							m_lpBits[i] = (HE_BYTE)colorIndex;	
						}
					}
				}
			}
			if ( m_lpPalette->m_nPaletteSize == palette.m_nPaletteSize )
			{
				m_lpPalette->m_nPaletteSize = palette.m_nPaletteSize;
				m_lpPalette->m_type = palette.m_type;
				memcpy( m_lpPalette->m_pPalette, palette.m_pPalette, m_lpPalette->m_nPaletteSize );
			}else{
				m_lpPalette->m_nPaletteSize = palette.m_nPaletteSize;
				m_lpPalette->m_type = palette.m_type;
				if ( m_lpPalette->m_pPalette )
				{
					delete m_lpPalette->m_pPalette;
				}
				m_lpPalette->m_pPalette = new HE_ARGB[m_lpPalette->m_nPaletteSize];
				memcpy( m_lpPalette->m_pPalette, palette.m_pPalette, m_lpPalette->m_nPaletteSize );
			}
			break;
		}
	default:
		return;
	}
}

HE_DWORD CHE_Bitmap::GetByteIndexB( HE_DWORD x, HE_DWORD y )
{
	HE_DWORD index;
	if ( FlowOrig() == HE_BITMAP_FLOWORIG_BOTTOM )
	{
		index = ( Height() - 1 - y ) * Pitch();
		index += ( x * Depth() ) >> 3;
	}else{
		index = y * Pitch();
		index += ( x * Depth() ) >> 3;
	}
	return index;
}

HE_DWORD CHE_Bitmap::GetByteIndexE( HE_DWORD x, HE_DWORD y, HE_DWORD length )
{
	HE_DWORD index;
	if ( FlowOrig() == HE_BITMAP_FLOWORIG_BOTTOM )
	{
		index = ( Height() - 1 - y ) * Pitch();
		index += ( ( x + length - 1 ) * Depth() ) >> 3;
	}else{
		index = y * Pitch();
		index += ( ( x + length - 1 ) * Depth() ) >> 3;
	}
	return index;
}