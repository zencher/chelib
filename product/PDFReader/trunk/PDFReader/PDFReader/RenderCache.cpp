#include "StdAfx.h"
#include <float.h>
#include "RenderCache.h"


CRenderCache::CRenderCache( size_t size )
{
	mCache.resize( size );
	for ( size_t i = 0; i < size; ++i )
	{
		mCache[i].bInvalid = true;
		mCache[i].pBitmap = NULL;
	}
}


CRenderCache::~CRenderCache(void)
{
	size_t count = mCache.size();
	for ( size_t i = 0; i < count; ++i )
	{
		if ( mCache[i].pBitmap )
		{
			mCache[i].pBitmap->GetAllocator()->Delete( mCache[i].pBitmap );
			mCache[i].pBitmap = NULL;
			mCache[i].bInvalid = true;
		}
	}
}


void CRenderCache::PushItem( int index, int rotate, float scale, CHE_Rect rect, CHE_Bitmap * pBitmap )
{
	size_t count = mCache.size();
	for ( size_t i = 0; i < count; ++i )
	{
		if ( mCache[i].bInvalid == true )
		{
			mCache[i].index = index;
			mCache[i].rotate = rotate;
			mCache[i].scale = scale;
			mCache[i].rect = rect;
			mCache[i].pBitmap = pBitmap;
			mCache[i].bInvalid = false;
			break;
		}
	}
}


void CRenderCache::Update( int indexFirst, int indexLast, int rotate, float scale )
{
// 	size_t count = mCache.size();
// 	for ( size_t i = 0; i < count; ++i )
// 	{
// 		if ( mCache[i].bInvalid != true )
// 		{
// 			if (	(mCache[i].index < indexFirst || mCache[i].index > indexLast) && 
// 					(mCache[i].rotate == rotate) && (mCache[i].scale - scale <= FLT_EPSILON) )
// 			{
// 				mCache[i].pBitmap->GetAllocator()->Delete( mCache[i].pBitmap );
// 				mCache[i].pBitmap = NULL;
// 				mCache[i].bInvalid = true;
// 			}
// 		}
// 	}
}


CHE_Bitmap * CRenderCache::GetRenderBitmap( int index, int rotate, float scale )
{
	size_t count = mCache.size();
	for ( size_t i = 0; i < count; ++i )
	{
		if ( mCache[i].bInvalid != true )
		{
			if ( mCache[i].index == index && mCache[i].rotate == rotate && mCache[i].scale - scale <= FLT_EPSILON )
			{
				return mCache[i].pBitmap;
			}
		}
	}
	return NULL;
}