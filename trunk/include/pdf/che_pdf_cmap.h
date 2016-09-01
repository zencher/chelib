#ifndef _CHE_PDF_CMAP_H_
#define _CHE_PDF_CMAP_H_

#include "../che_base.h"
#include "../che_string.h"
#include "che_pdf_cmap_res.h"

class CHE_PDF_CMap : public CHE_Object
{
public:
	static CHE_PDF_CMap * LoadBuildinCMap( const CHE_ByteString & cmapName,  CHE_Allocator * pAllocator = nullptr );

	~CHE_PDF_CMap();

	bool LookupCode( uint32 cpt, uint32 & codeRet ) const;

	bool IsCode( uint32 cpt, BYTE byteCount ) const;

private:
	CHE_PDF_CMap( PDF_CMAP * pCmap, bool bNeedClear = false, CHE_Allocator * pAllocator = nullptr )
		: CHE_Object(pAllocator), mbNeedClear(false) { mpCMap = pCmap; }

	bool		mbNeedClear; 
	PDF_CMAP *	mpCMap;

	friend CHE_Allocator;
};

#endif