#ifndef _CHELIB_BITMAP_H_
#define _CHELIB_BITMAP_H_

/*******************************************************************
 *  A simple bitmap class in CHELIB
 *  Copyright(C) 2010-2016 Frank Zeng
 *  All rights reserved.
 *
 *  @file CBitmap.h
 *  @brief A simple RGB bitmap class.
 *
 *  @version 1.0
 *  @author FrankZeng
 *  @email zch3156@163.com
 *  @date 2010-06-01
 *
 *  #update
 *    [2016-09-10]
 *      remove the support for 1, 2, 4, 8, 16 bit bitmap.
 *
 ******************************************************************/


#include "base.h"

#include <vector>

namespace chelib
{
    
///Bitmap depth.
enum BITMAP_DEPTH
{
    DEPTH_24BPP = 24,
    DEPTH_32BPP = 32
};

///Bitmap data direction.
enum BITMAP_DIRECTION
{
    DIRECTION_UP,
    DIRECTION_DOWN
};

///Bitmap channels.
enum BITMAP_CHANNEL
{
    CHANNEL_Red = 1,
    CHANNEL_Green,
    CHANNEL_Blue,
    CHANNEL_Alpha
};
    
///Rectangle for Bitmap clip or pixels select.
struct BITMAP_RECT
{
    uint32 left;
    uint32 top;
    uint32 width;
    uint32 height;
};
    
/** A simple RGB bitmap handle class.
 *
 * Only Support 24bits RGB and 32bits RGBA bitmap.
 *
 */

class CBitmap : public CObject
{
public:
    CBitmap( CAllocator * pAllocator = nullptr );
    CBitmap( const CBitmap & bitmap );
    ~CBitmap();

    CBitmap & operator=( const CBitmap & bitmap );

    bool Create( uint32 width, uint32 height, BITMAP_DEPTH depth,
                 BITMAP_DIRECTION direction, size_t dataSize = 0,
                 PCBYTE data = nullptr );

    CBitmap * Clip( BITMAP_RECT rect ) const;
    CBitmap * Clone() const;

    void Clean();

    ///access data
    PBYTE GetBuffer() const { return mpBits; } ;

    ///file Operation
    bool Load( char const * );
    bool Save( char const * );
    bool SaveToMem( PBYTE buffer, size_t size );

    size_t GetMemBitmapDataSize();
    bool GetMemBitmapData( PBYTE buffer, size_t size );

    ///bitmap basic information
    uint32 Width() const { return mWidth; } ;
    uint32 Height() const { return mHeight; } ;
    uint32 Pitch() const { return ( ( ( mWidth * mDepth ) + 31 ) & ~31 ) >> 3; };
    BITMAP_DEPTH Depth() const { return mDepth; };
    BITMAP_DIRECTION Direction() const { return mDirection; } ;

    ///pixel operation
    bool GetPixelColor( uint32 x, uint32 y, ARGB & colorRet ) const;
    bool SetPixelColor( uint32 x, uint32 y, ARGB color );
    bool SetPixelColor( uint32 x, uint32 y, std::vector<ARGB> & colors );
    bool SetPixelColor( uint32 x, uint32 y, ARGB * pColors, size_t count );

    ///channel operation
    bool SetChannel( BITMAP_CHANNEL channel, BYTE vlaue );
    bool SetChannelByAlpha( BITMAP_CHANNEL channel, BYTE alpha );
    bool ExchangeChannel( BITMAP_CHANNEL channel1, BITMAP_CHANNEL channel2 );
    bool CopyChannel( BITMAP_CHANNEL channelDes, BITMAP_CHANNEL channelSrc );

    ///area operation
    bool Fill( ARGB color );
    bool Fill( ARGB color, BITMAP_RECT rect );
    void DrawLine( uint32 lineIndex, uint32 xIndex, uint32 pixelCount, ARGB color );
    void DrawLine( uint32 lineIndex, uint32 xIndex, uint32 pixelCount, PBYTE pData, size_t dataSize );

    //	//bitmap operation, for 24bit and 32bit only
    //	bool CompositeMask( ARGB color, uint32 x, uint32 y, CBitmap & maskBitmap );
    //	bool Insert( const CBitmap & bitmap, uint32 x, uint32 y );
    //	bool Insert( const CBitmap & bitmap, size_t x, size_t y, CBitmap & maskBitmap );
    //	CBitmap * StretchTo( uint32 desWidth, uint32 desHeight, uint32 flag, BITMAP_RECT * pRect = nullptr );
    //	CBitmap * Translate( FLOAT a, FLOAT b, FLOAT c, FLOAT d, FLOAT e, FLOAT f );

private:
    size_t GetPixelByteIndex( uint32 x, uint32 y ) const;

    uint32 mWidth;
    uint32 mHeight;
    BITMAP_DEPTH mDepth;
    BITMAP_DIRECTION mDirection;
    PBYTE mpBits;
};

}//namespace

#endif