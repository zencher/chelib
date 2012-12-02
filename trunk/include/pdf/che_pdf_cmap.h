#ifndef _CHE_PDF_CMAP_H_
#define _CHE_PDF_CMAP_H_

#include "../che_base.h"
#include "../che_string.h"
#include "che_pdf_cmap_res.h"

class CHE_PDF_CMap : public CHE_Object
{
public:
	static CHE_PDF_CMap * LoadBuildinCMap( const CHE_ByteString & cmapName,  CHE_Allocator * pAllocator = NULL );

	~CHE_PDF_CMap();

	HE_BOOL LookupCode( HE_DWORD cpt, HE_DWORD & codeRet ) const;

private:
	CHE_PDF_CMap( PDF_CMAP * pCmap, HE_BOOL bNeedClear = FALSE, CHE_Allocator * pAllocator = NULL )
		: CHE_Object(pAllocator), mbNeedClear(FALSE) { mpCMap = pCmap; }

	HE_BOOL		mbNeedClear; 
	PDF_CMAP *	mpCMap;

	friend CHE_Allocator;
};

#endif