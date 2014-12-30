#ifndef _CHE_PDF_IMAGERASTER_H_
#define _CHE_PDF_IMAGERASTER_H_

#include "../che_bitmap.h"
#include "che_pdf_xobject.h"

class CHE_PDF_ImageRaster : public CHE_Object
{
public:
	CHE_PDF_ImageRaster( CHE_Allocator * pAllocator = NULL );

	CHE_Bitmap * GetBitmap( const CHE_PDF_ImageXObjectPtr & imgPtr );

private: 
	CHE_Bitmap * GetBitmapImp( const CHE_PDF_ImageXObjectPtr & imgPtr );

	CHE_Bitmap * GetBitmapSimpleImp( const CHE_PDF_ImageXObjectPtr & imgPtr );

	CHE_Bitmap * GetBitmapNoMask( const CHE_PDF_ImageXObjectPtr & imgPtr );

	CHE_Bitmap * GetBitmapAsMask( const CHE_PDF_ImageXObjectPtr & imgPtr );

	CHE_Bitmap * GetBitmapWithImageMask( const CHE_PDF_ImageXObjectPtr & imgPtr, const CHE_PDF_ImageXObjectPtr & maskPtr, HE_BOOL bExplicit = false );
};

#endif