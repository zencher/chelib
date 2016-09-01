#ifndef _CHE_BITMAP_H_
#define _CHE_BITMAP_H_

#include <vector>

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

struct BITMAP_RECT
{
	uint32 left;
	uint32 top;
	uint32 width;
	uint32 height;
};


class CHE_Palette : public CHE_Object
{
public:
	CHE_Palette( HE_BITMAP_DEPTH depth, const ARGB * const pPalette = nullptr, CHE_Allocator * pAllocator = nullptr );
	CHE_Palette( const CHE_Palette & palette );
	~CHE_Palette();

	CHE_Palette & operator = ( const CHE_Palette & palette );

	uint32  GetCount() const;
	bool    GetColor( uint32 index, ARGB & colorRet ) const;
	bool    GetColorIndex( ARGB color, uint32 & indexRet ) const;
	bool    GetNearColorIndex( ARGB color, uint32 & indexRet ) const;
	bool    SetColor( uint32 index, ARGB color );
	bool    IsColorExist( ARGB color ) const;
private:
	ARGB *	mpPalette;
	uint32	mPaletteSize;
    
    friend class CHE_Bitmap;
};


class CHE_Bitmap : public CHE_Object
{
public:
	CHE_Bitmap( CHE_Allocator * pAllocator = nullptr );
	CHE_Bitmap( const CHE_Bitmap & bitmap );
	~CHE_Bitmap();

	CHE_Bitmap & operator = ( const CHE_Bitmap & bitmap );

	//file Operation
	bool		Load( char const * );
	bool		Save( char const * );
	bool		SaveToMem( PBYTE buffer, size_t size );

	size_t      GetMemBitmapDataSize();
	bool		GetMemBitmapData( PBYTE buffer, size_t size );

	//bitmap basic information
	uint32				Width() const { return mWidth; } ;
	uint32				Height() const { return mHeight; } ;
	size_t				Pitch() const { return ( ( ( mWidth * mDepth ) + 31 ) & ~31 ) >> 3; } ;
	HE_BITMAP_DEPTH			Depth() const { return mDepth; };
	HE_BITMAP_DIRECTION		Direction() const { return mDirection; } ;
	bool					IsCompression() const { return ( mCompression > 0 ) ? TRUE : false; } ;
	HE_BITMAP_COMPRESSION	GetCompressionType() const { return mCompression; } ;

	//palette
	CHE_Palette *			GetPalette() const { return mpPalette; };

	//access data	
	PBYTE				GetBuffer() const { return mpBits; } ;
	
	//pixel operation
	bool				GetPixelColor( uint32 x, uint32 y, ARGB & colorRet ) const;
	bool				SetPixelColor( uint32 x, uint32 y, ARGB color );
	bool				SetPixelColor( uint32 x, uint32 y, std::vector<ARGB> & colors );
    bool                SetPixelColor( uint32 x, uint32 y, ARGB * pColors, size_t count );
	bool				GetPixelIndex( uint32 x, uint32 y, BYTE & indexRet ) const;
	bool				SetPixelIndex( uint32 x, uint32 y, BYTE index );

	//channel operation
	bool				SetChannel( HE_BITMAP_CHANNEL channel, BYTE vlaue );
	bool				SetChannelByAlpha( HE_BITMAP_CHANNEL channel, BYTE alpha );
	//CHE_Bitmap &	SaveChannelAsBitmap( HE_BITMAP_CHANNEL channel, bool bMask = false );
	bool				ExchangeChannel( HE_BITMAP_CHANNEL channel1, HE_BITMAP_CHANNEL channel2 );
	bool				CopyChannel( HE_BITMAP_CHANNEL channelDes, HE_BITMAP_CHANNEL channelSrc );

	//area operation
	bool				Fill( ARGB color );
	bool				Fill( ARGB color, const BITMAP_RECT* rect );
	void				DrawLine( uint32 lineIndex, uint32 xIndex, uint32 pixelCount, ARGB color );
	void				DrawLine( uint32 lineIndex, uint32 xIndex, uint32 pixelCount, PBYTE pData, size_t dataSize );

	//bitmap operation
	bool                Create( uint32 width, uint32 height, HE_BITMAP_DEPTH depth, HE_BITMAP_DIRECTION direction, size_t dataSize = 0,
						PCBYTE data = nullptr, CHE_Palette* pPalette = nullptr );
	CHE_Bitmap *        Clone( const BITMAP_RECT* pRect = nullptr ) const;
	void				Clean();

	bool				ConvertDetph( HE_BITMAP_DEPTH depth );

	//bitmap operation, for 24bit and 32bit only
	bool				CompositeMask( ARGB color, uint32 x, uint32 y, CHE_Bitmap & maskBitmap );
	bool				Insert( const CHE_Bitmap & bitmap, uint32 x, uint32 y );
	//bool			Insert( const CHE_Bitmap & bitmap, size_t x, size_t y, CHE_Bitmap & maskBitmap );
    
	CHE_Bitmap*			StretchTo( uint32 desWidth, uint32 desHeight, uint32 flag, BITMAP_RECT * pRect = nullptr );
	CHE_Bitmap*			Translate( FLOAT a, FLOAT b, FLOAT c, FLOAT d, FLOAT e, FLOAT f );


private:
	size_t				GetPixelByteIndex( uint32 x, uint32 y ) const;

    uint32				mWidth;
	uint32				mHeight;
	HE_BITMAP_DEPTH			mDepth;
	HE_BITMAP_DIRECTION		mDirection;
	HE_BITMAP_COMPRESSION	mCompression;
	CHE_Palette *			mpPalette;
	PBYTE                   mpBits;
};

#endif