#ifndef _CHE_BITMAP_H_
#define _CHE_BITMAP_H_

#include "che_base.h"

enum HE_BITMAP_DEPTH
{
	BITMAP_DEPTH_1BPP = 1,
	BITMAP_DEPTH_4BPP = 4,
	BITMAP_DEPTH_8BPP = 8,
	BITMAP_DEPTH_16BPP = 16,
	BITMAP_DEPTH_24BPP = 24,
	BITMAP_DEPTH_32BPP = 32
};

enum HE_BITMAP_DIRECTION
{
	BITMAP_DIRECTION_UP,
	BITMAP_DIRECTION_DOWN
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

typedef HE_INT32 HE_ARGB;

class CHE_Palette : public CHE_Object
{
public:
	CHE_Palette( HE_BITMAP_DEPTH depth, const HE_ARGB * const pPalette = NULL, CHE_Allocator * pAllocator = NULL );
	CHE_Palette( const CHE_Palette& palette );
	~CHE_Palette();

	CHE_Palette & operator=( const CHE_Palette& palette );

	HE_ULONG	GetCount() const;
	HE_BOOL		GetColor( HE_ULONG index, HE_ARGB & colorRet ) const;
	HE_BOOL		GetColorIndex( HE_ARGB color, HE_ULONG & indexRet ) const;
	HE_BOOL		GetNearColorIndex( HE_ARGB color, HE_ULONG & indexRet ) const;
	HE_BOOL		SetColor( HE_ULONG index, HE_ARGB color );
	HE_BOOL		IsColorExist( HE_ARGB color ) const;
private:
	friend class CHE_Bitmap;

	HE_ARGB *	m_pPalette;
	HE_ULONG	m_nPaletteSize;			
};


class CHE_Bitmap : public CHE_Object
{
public:
	CHE_Bitmap( CHE_Allocator * pAllocator = NULL );
	CHE_Bitmap( const CHE_Bitmap & bitmap );
	~CHE_Bitmap();

	CHE_Bitmap & operator = ( const CHE_Bitmap & bitmap );

	//file Operation
	HE_BOOL		Load( HE_LPCSTR );
	HE_BOOL		Save( HE_LPCSTR );

	HE_BOOL		SaveToMem( HE_LPBYTE buffer, HE_ULONG size );

	HE_ULONG	GetMemBitmapDataSize();
	HE_BOOL		GetMemBitmapData( HE_LPBYTE buffer, HE_ULONG size );

	//bitmap basic information
	HE_ULONG				Width() const { return m_lWidth; } ;
	HE_ULONG				Height() const { return m_lHeight; } ;
	HE_ULONG				Pitch() const { return ( ( ( m_lWidth * m_Depth ) + 31 ) & ~31 ) >> 3; } ;
	HE_BITMAP_DEPTH			Depth() const { return m_Depth; };
	HE_BITMAP_DIRECTION		Direction() const { return m_Direction; } ;
	HE_BOOL					IsCompression() const { return ( m_Compression > 0 ) ? TRUE : FALSE; } ;
	HE_BITMAP_COMPRESSION	GetCompressionType() const { return m_Compression; } ;

	//palette
	CHE_Palette*			GetPalette() const { return m_lpPalette; };

	//access data	
	HE_LPCBYTE				GetBuffer() const { return m_lpBits; } ;
	
	//pixel operation
	HE_BOOL				GetPixelColor( HE_ULONG x, HE_ULONG y, HE_ARGB & colorRet ) const;
	HE_BOOL				SetPixelColor( HE_ULONG x, HE_ULONG y, HE_ARGB color );
	HE_BOOL				GetPixelIndex( HE_ULONG x, HE_ULONG y, HE_BYTE & indexRet ) const;
	HE_BOOL				SetPixelIndex( HE_ULONG x, HE_ULONG y, HE_BYTE index );

	//channel operation
	HE_BOOL				SetChannel( HE_BITMAP_CHANNEL channel, HE_BYTE vlaue );
	HE_BOOL				SetChannelByAlpha( HE_BITMAP_CHANNEL channel, HE_BYTE alpha );
	//CHE_Bitmap &	SaveChannelAsBitmap( HE_BITMAP_CHANNEL channel, HE_BOOL bMask = FALSE );
	HE_BOOL				ExchangeChannel( HE_BITMAP_CHANNEL channel1, HE_BITMAP_CHANNEL channel2 );
	HE_BOOL				CopyChannel( HE_BITMAP_CHANNEL channelDes, HE_BITMAP_CHANNEL channelSrc );

	//area operation
	HE_BOOL				Fill( HE_ARGB color );
	HE_BOOL				Fill( HE_ARGB color, const HE_RECT* rect );
	HE_VOID				DrawLine( HE_ULONG nLine, HE_ULONG nStart, HE_ULONG nLength, HE_ARGB color );
	HE_VOID				DrawLine( HE_ULONG nLine, HE_ULONG nStrat, HE_ULONG nLength, HE_LPBYTE lpDatabuf, HE_ULONG nBufSize );

	//bitmap operation
	HE_BOOL				Create( HE_ULONG width, HE_ULONG height, HE_BITMAP_DEPTH depth, HE_BITMAP_DIRECTION direction, HE_ULONG bufferSize = 0,
						HE_LPCBYTE buffer = NULL, CHE_Palette* pPalette = NULL );
	CHE_Bitmap*			Clone( const HE_RECT* pRect = NULL ) const;
	HE_VOID				Clean();

	HE_BOOL				ConvertDetph( HE_BITMAP_DEPTH depth );

	//bitmap operation, for 24bit and 32bit only
	HE_BOOL				CompositeMask( HE_ARGB color, HE_ULONG x, HE_ULONG y, CHE_Bitmap & maskBitmap );
	HE_BOOL				Insert( const CHE_Bitmap & bitmap, HE_ULONG x, HE_ULONG y );
	//HE_BOOL			Insert( const CHE_Bitmap & bitmap, HE_ULONG x, HE_ULONG y, CHE_Bitmap & maskBitmap );

	CHE_Bitmap*			StretchTo( HE_ULONG desWidth, HE_ULONG desHeight, HE_ULONG flag, HE_RECT * pRect = NULL );

	CHE_Bitmap*			Translate( HE_FLOAT a, HE_FLOAT b, HE_FLOAT c, HE_FLOAT d, HE_FLOAT e, HE_FLOAT f );


private:

	HE_ULONG				GetPixelByteIndex( HE_ULONG x, HE_ULONG y ) const;

	HE_ULONG				m_lWidth;
	HE_ULONG				m_lHeight;
	HE_BITMAP_DEPTH			m_Depth;
	HE_BITMAP_DIRECTION		m_Direction;
	HE_BITMAP_COMPRESSION	m_Compression;
	CHE_Palette*			m_lpPalette;
	HE_LPBYTE				m_lpBits;
};

#endif