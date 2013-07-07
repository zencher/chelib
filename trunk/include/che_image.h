#ifndef _CHE_IMAGE_H_
#define _CHE_IMAGE_H_

#include "che_base.h"
#include "che_bitmap.h"

enum HE_IMAGE_TYPE
{
	IMAGE_TYPE_JPEG,
	IMAGE_TYPE_JPX,
	IMAGE_TYPE_JBIG2,
	IMAGE_TYPE_TIFF,
	IMAGE_TYPE_PNG,
	IMAGE_TYPE_GIF
};

class CHE_ImageDecoder;

class CHE_ImageDescriptor : CHE_Object
{
public:
	CHE_ImageDescriptor( CHE_Allocator * pAllocator = NULL );
	~CHE_ImageDescriptor();

	HE_ULONG	GetWidth() const { return mWidth; }
	HE_ULONG	GetHeight() const { return mHeight; }
	HE_ULONG	GetComponentCount() const { return mComponent; }
	HE_ULONG	GetBitsPerComponent() const { return mBitpc; }
	HE_LPBYTE	GetData() const { return mData; }
	HE_ULONG	GetSize() const { return mSize; }

	HE_VOID		SetWidth( HE_ULONG width ) { mWidth = width; }
	HE_VOID		SetHeight( HE_ULONG height ) { mHeight = height; }
	HE_VOID		SetComponentCount( HE_ULONG component ) { mComponent = component; }
	HE_VOID		SetBitsPerComponent( HE_ULONG bitpc ) { mBitpc = bitpc; }
	HE_VOID		AllocData( HE_ULONG size );

private:
	HE_ULONG	mWidth;
	HE_ULONG	mHeight;
	HE_ULONG	mComponent;
	HE_ULONG	mBitpc;
	HE_LPBYTE	mData;
	HE_ULONG	mSize;
};

class CHE_ImageDecoder : public CHE_Object
{
public:
	CHE_ImageDecoder( CHE_Allocator * pAllocator = NULL );

	HE_VOID			SetDecodeParam( HE_LPBYTE data, HE_ULONG size ); //not take over the data, just ref the data

	HE_BOOL			Decode( HE_IMAGE_TYPE type, HE_LPBYTE data, HE_ULONG size, CHE_ImageDescriptor & desRet );
	CHE_Bitmap *	Decode( HE_IMAGE_TYPE type, HE_LPBYTE data, HE_ULONG size );

private:
	HE_LPBYTE		mDecodeParam;
	HE_ULONG		mParamSize;
};


#endif