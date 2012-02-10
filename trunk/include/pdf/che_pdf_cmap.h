#ifndef _CHE_PDF_CMAP_H_
#define _CHE_PDF_CMAP_H_

#include "../che_base.h"

class CHE_PDF_CMap : public CHE_Object
{
public:
	CHE_PDF_CMap( CHE_Allocator * pAllocator = NULL ) : CHE_Object( pAllocator ) {}

	~CHE_PDF_CMap() { }

private:
};

#endif