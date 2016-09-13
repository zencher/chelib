#ifndef _CHELIB_PDF_CMAP_H_
#define _CHELIB_PDF_CMAP_H_

#include "../che_base_string.h"
#include "che_pdf_cmap_res.h"

namespace chelib {

class CPDF_CMap : public CObject
{
public:
	static CPDF_CMap * LoadBuildinCMap( const CByteString & cmapName,  CAllocator * pAllocator = nullptr );

	~CPDF_CMap();

	bool LookupCode( uint32 cpt, uint32 & codeRet ) const;

	bool IsCode( uint32 cpt, BYTE byteCount ) const;

private:
	CPDF_CMap( PDF_CMAP * pCmap, bool bNeedClear = false, CAllocator * pAllocator = nullptr )
		: CObject(pAllocator), mbNeedClear(false) { mpCMap = pCmap; }

	bool		mbNeedClear; 
	PDF_CMAP *	mpCMap;

	friend CAllocator;
};

}//namespce

#endif