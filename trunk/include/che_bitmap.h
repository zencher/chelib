#ifndef _CHE_BITMAP_H_
#define _CHE_BITMAP_H_

#include "che_base.h"

typedef struct tagHE_BITMAPFILEHEADER
{
	HE_WORD		bfType;
	HE_DWORD	bfSize;
	HE_WORD		bfReserved1;
	HE_WORD		bfReserved2;
	HE_DWORD	bfOffBits;
}HE_BITMAPFILEHEADER, * HE_LPBITMAPFILEHEADER;

typedef struct tagHE_BITMAPINFOHEADER
{
	HE_DWORD	biSize; 
	HE_LONG		biWidth; 
	HE_LONG		biHeight; 
	HE_WORD		biPlanes; 
	HE_WORD 	biBitCount; 
	HE_DWORD	biCompression; 
	HE_DWORD	biSizeImage; 
	HE_LONG		biXPelsPerMeter; 
	HE_LONG		biYPelsPerMeter; 
	HE_DWORD	biClrUsed; 
	HE_DWORD	biClrImportant;
}HE_BITMAPINFOHEADER, *HE_LPBITMAPINFOHEADER;

enum HE_BITMAP_FORMAT
{
	BITMAP_FORMAT_Invaild,
	BITMAP_FORMAT_1BPP,
	BITMAP_FORMAT_1BPPMask,
	BITMAP_FORMAT_4BPP,
	BITMAP_FORMAT_8BPP,
	BITMAP_FORMAT_8BPPMask,
	BITMAP_FORMAT_24BPP,
	BITMAP_FORMAT_32BPP
};

enum HE_BITMAP_ORIG
{
	BITMAP_ORIG_BOTTOM,	//一般情况下，height为负时
	BITMAP_ORIG_TOP
};

enum HE_BITMAP_CHANNEL
{
	BITMAP_CHANNEL_Red = 1,
	BITMAP_CHANNEL_Green,
	BITMAP_CHANNEL_Blue,
	BITMAP_CHANNEL_Alpha
};

enum HE_BITMAP_COMPRESSION
{
	BITMAP_COMPRESSION_RGB = 0,
	BITMAP_COMPRESSION_RLE8 = 1,
	BITMAP_COMPRESSION_RLE4 = 2,
	BITMAP_COMPRESSION_BITFIELDS =3
};

typedef HE_DWORD HE_ARGB;

class CHE_Palette : public CHE_Object
{
public:
	~CHE_Palette();

	CHE_Palette & operator=( const CHE_Palette& palette );

	HE_BITMAP_FORMAT	Format() const;

	HE_DWORD	GetColorCount() const;
	HE_BOOL		GetColor( HE_DWORD index, HE_ARGB & colorRet ) const;
	HE_BOOL		GetColorIndex( HE_ARGB color, HE_DWORD & indexRet ) const;
	HE_BOOL		GetNearColorIndex( HE_ARGB color, HE_DWORD & indexRet ) const;
	HE_BOOL		SetColor( HE_DWORD index, HE_ARGB color );
	HE_BOOL		IsColorExist( HE_ARGB color ) const;

private:
	friend class CHE_Bitmap;

	CHE_Palette( HE_BITMAP_FORMAT format, const HE_ARGB * const pPalette = NULL );
	CHE_Palette( const CHE_Palette& palette );

	HE_ARGB *		m_pPalette;
	HE_BITMAP_FORMAT m_format;
	HE_DWORD		m_nPaletteSize;			
};


class CHE_Bitmap : public CHE_Object
{
public:
	CHE_Bitmap();
	~CHE_Bitmap();

	HE_BOOL		Load( HE_LPCSTR );
	HE_BOOL		Save( HE_LPCSTR );

	//bitmap basic information
	HE_DWORD	Width() const { return m_InfoHeader.biWidth; } ;
	HE_DWORD	Height() const { return ( m_InfoHeader.biHeight > 0 ) ? (m_InfoHeader.biHeight) : (-m_InfoHeader.biHeight); } ;
	HE_WORD		Depth() const { return m_InfoHeader.biBitCount; } ;
	HE_DWORD	Pitch() const { return ( ( ( m_InfoHeader.biWidth * m_InfoHeader.biBitCount ) + 31 ) & ~31 ) >> 3; } ;
	HE_BITMAP_FORMAT		Format() const;
	HE_BITMAP_ORIG			Orig() const { return m_Orig; } ;
	HE_BOOL					IsCompression() const { return m_InfoHeader.biCompression; } ;
	HE_BITMAP_COMPRESSION	GetCompressionType() const { return (HE_BITMAP_COMPRESSION)(m_InfoHeader.biCompression); } ;

	//palette
	CHE_Palette*	GetPalette() const { return m_lpPalette; };

	//access data	
	const HE_LPBITMAPINFOHEADER		GetInfoHeader() { return &m_InfoHeader; } ;
	HE_LPCBYTE						GetBuffer() const { return m_lpBits; } ;
	
	//pixel operation
	HE_ARGB		GetPixel( HE_DWORD x, HE_DWORD y ) const;
	HE_BOOL		SetPixel( HE_DWORD x, HE_DWORD y, HE_ARGB color );

	//channel operation
	HE_BOOL			SetChannel( HE_BITMAP_CHANNEL channel, HE_BYTE vlaue );
	HE_BOOL			SetChannelByAlpha( HE_BITMAP_CHANNEL channel, HE_BYTE alpha );
	//CHE_Bitmap &	SaveChannelAsBitmap( HE_BITMAP_CHANNEL channel, HE_BOOL bMask = FALSE );
	HE_BOOL			ExchangeChannel( HE_BITMAP_CHANNEL channel1, HE_BITMAP_CHANNEL channel2 );
	HE_BOOL			CopyChannel( HE_BITMAP_CHANNEL channelDes, HE_BITMAP_CHANNEL channelSrc );

	//area operation
	HE_BOOL		Fill( HE_ARGB color );
	HE_BOOL		Fill( HE_ARGB color, const HE_RECT* rect );
	HE_VOID		DrawLine( HE_DWORD nLine, HE_DWORD nStart, HE_DWORD nLength, HE_ARGB color );
	HE_VOID		DrawLine( HE_DWORD nLine, HE_DWORD nStrat, HE_DWORD nLength, HE_LPBYTE lpDatabuf, HE_DWORD nBufSize );

	//bitmap operation
	HE_BOOL		Create( HE_DWORD width, HE_DWORD height, HE_BITMAP_FORMAT format, HE_BITMAP_ORIG flowOrig, HE_DWORD bufferSize = 0,
						HE_LPCBYTE buffer = NULL, const HE_ARGB* pPalette = NULL );
	CHE_Bitmap* Clone( const HE_RECT* pRect = NULL ) const;
	HE_VOID		Clean();
	HE_BOOL		Insert( const CHE_Bitmap & bitmap, HE_DWORD x, HE_DWORD y );

private:

	HE_DWORD	GetByteIndex( HE_DWORD x, HE_DWORD y );

	HE_LPBYTE			m_lpBits;
	CHE_Palette*		m_lpPalette;
	HE_BITMAPINFOHEADER	m_InfoHeader;
	HE_BITMAP_ORIG		m_Orig;
};



class CHE_FontBitmap : public CHE_Bitmap
{
public:
	HE_BOOL		Load( HE_DWORD width, HE_DWORD height, HE_DWORD pitch, HE_BITMAP_FORMAT format, HE_BITMAP_ORIG flowOrig, HE_LPBYTE buffer );
};

#endif