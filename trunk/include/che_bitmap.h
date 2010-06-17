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

enum HE_BITMAP_TYPE
{
	HE_BITMAP_Invaild,
	HE_BITMAP_1BPP,
	HE_BITMAP_1BPPMask,
	HE_BITMAP_4BPP,
	HE_BITMAP_8BPP,
	HE_BITMAP_8BPPMask,
	HE_BITMAP_24BPP,
	HE_BITMAP_32BPP
};

enum HE_BITMAP_FLOWORIG
{
	HE_BITMAP_FLOWORIG_BOTTOM,	//一般情况下，height为负时
	HE_BITMAP_FLOWORIG_UP		
};

enum HE_BITMAP_Channel
{
	HE_BITMAP_Red = 1,
	HE_BITMAP_Green,
	HE_BITMAP_Blue,
	HE_BITMAP_Alpha
};

enum HE_BITMAP_Compression
{
	HE_BIBITMAP_RGB = 0,
	HE_BITMAP_RLE8 = 1,
	HE_BITMAP_RLE4 = 2,
	HE_BITMAP_BITFIELDS =3
};

typedef HE_DWORD HE_ARGB;

struct HE_BITMAP_PixelsDataMark
{
	HE_LPBYTE	pByte;
	HE_DWORD	nBitCount;
	HE_DWORD	nIndex;
};

struct HE_BITMAP_PixelsData
{
	HE_LPBYTE	pData;
	HE_DWORD	nBitCount;
};

class CHE_Palette : public CHE_Object
{
public:
	~CHE_Palette();

	CHE_Palette & operator=( const CHE_Palette& palette );

	HE_BITMAP_TYPE	Type() const;

	HE_DWORD	ColorCount() const;
	HE_BOOL		GetColor( HE_BYTE index, HE_ARGB & colorRet ) const;
	HE_BOOL		GetColorIndex( HE_ARGB color, HE_DWORD & indexRet ) const;
	HE_BOOL		GetNearColorIndex( HE_ARGB color, HE_DWORD & indexRet ) const;
	HE_BOOL		SetColor( HE_BYTE index, HE_ARGB color );
	HE_BOOL		IsColorExist( HE_ARGB color ) const;

private:
	friend class CHE_Bitmap;

	CHE_Palette( HE_BITMAP_TYPE type, const HE_ARGB * const pPalette = NULL );
	CHE_Palette( const CHE_Palette& palette );

	HE_ARGB *		m_pPalette;
	HE_BITMAP_TYPE	m_type;
	HE_DWORD		m_nPaletteSize;			
};


class CHE_Bitmap : public CHE_Object
{
public:
	CHE_Bitmap();
	~CHE_Bitmap();

	HE_BOOL		Load( HE_LPCSTR );
	HE_BOOL		Save( HE_LPCSTR );
	HE_BOOL		Create( HE_DWORD width, HE_DWORD height, HE_BITMAP_TYPE type, HE_BITMAP_FLOWORIG flowOrig, HE_DWORD bufferSize = 0,
						HE_LPCBYTE buffer = NULL, const HE_ARGB* pPalette = NULL );

	HE_VOID		Clean();

	HE_DWORD	Width() const { return m_InfoHeader.biWidth; } ;
	HE_DWORD	Height() const { return ( m_InfoHeader.biHeight > 0 ) ? (m_InfoHeader.biHeight) : (-m_InfoHeader.biHeight); } ;
	HE_WORD		Depth() const { return m_InfoHeader.biBitCount; } ;
	HE_DWORD	Pitch() const { return ( ( ( m_InfoHeader.biWidth * m_InfoHeader.biBitCount ) + 31 ) & ~31 ) >> 3; } ;
	HE_BITMAP_TYPE Type() const;
	HE_BITMAP_FLOWORIG FlowOrig() const { return m_flowOrig; } ;

	//const HE_ARGB*	GetPalette() const { return m_lpPalette; } ;
	//HE_WORD		GetPaletteSize() const;
	//HE_ARGB		GetPaletteArgb( HE_WORD index ) const;
	//HE_SHORT	GetNearArgbInPalette( HE_ARGB color ) const;
	//HE_VOID		SetPaletteArgb( HE_BYTE index, HE_ARGB argb );
	//HE_BOOL		FindPaletteArgb( HE_ARGB argb, HE_LPBYTE pIndexRet ) const;

	CHE_Palette*	GetPalette() const { return m_lpPalette; };

	HE_BOOL		IsCompression() const { return m_InfoHeader.biCompression; } ;
	HE_BITMAP_Compression	GetCompressionType() const { return (HE_BITMAP_Compression)(m_InfoHeader.biCompression); } ;
	
	const HE_LPBITMAPINFOHEADER GetInfoHeader() { return &m_InfoHeader; } ;
	HE_LPCBYTE	GetBuffer() const { return m_lpBits; } ;
	
	HE_ARGB		GetPixel( HE_DWORD x, HE_DWORD y ) const;
	HE_BOOL		SetPixel( HE_DWORD x, HE_DWORD y, HE_ARGB color );

	HE_BOOL		Fill( HE_ARGB color );
	HE_BOOL		Fill( HE_ARGB color, const HE_RECT* rect );

	CHE_Bitmap* Clone( const HE_RECT* pRect = NULL ) const;

	HE_BOOL		Insert( const CHE_Bitmap & bitmap, HE_DWORD x, HE_DWORD y );

	

private:
	/*HE_ARGB*/
	HE_VOID		ChangePalette( const CHE_Palette & palette );

	HE_DWORD	GetByteIndexB( HE_DWORD x, HE_DWORD y );
	HE_DWORD	GetByteIndexE( HE_DWORD x, HE_DWORD y, HE_DWORD length );

	HE_VOID		DrawLine( HE_DWORD nLine, HE_DWORD nStart, HE_DWORD nLength, HE_ARGB color );
	HE_VOID		DrawLine( HE_DWORD nLine, HE_DWORD nStrat, HE_DWORD nLength, HE_LPBYTE lpDatabuf, HE_DWORD nBufSize );

	HE_LPBYTE			m_lpBits;
	//HE_ARGB*			m_lpPalette;
	CHE_Palette*		m_lpPalette;
	HE_BITMAPINFOHEADER	m_InfoHeader;
	HE_BITMAP_FLOWORIG	m_flowOrig;
};



class CHE_FontBitmap : public CHE_Bitmap
{
public:
	HE_BOOL		Load( HE_DWORD width, HE_DWORD height, HE_DWORD pitch, HE_BITMAP_TYPE type, HE_BITMAP_FLOWORIG flowOrig, HE_LPBYTE buffer );
};

#endif