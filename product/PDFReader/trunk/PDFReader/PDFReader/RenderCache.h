#pragma once

#include <vector>
#include <Windows.h>

#include "../../../../../trunk/include/che_bitmap.h"
#include "../../../../../trunk/include/che_graphics.h"

struct RENDERBITMAP
{
	bool			bInvalid;
	int				index;
	int				rotate;
	float			scale;
	CHE_Rect		rect;
	CHE_Bitmap *	pBitmap;
};

class CRenderCache
{
public:
	CRenderCache( size_t size );
	~CRenderCache();

	void PushItem( int index, int rotate, float scale, CHE_Rect rect, CHE_Bitmap * pBitmap );

	void Update( int indexFirst, int indexLast, int rotate, float scale );

	CHE_Bitmap * GetRenderBitmap( int index, int rotate, float scale );

private:
	std::vector<RENDERBITMAP>	mCache;
};

