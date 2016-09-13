#ifndef _CHELIB_PDF_CREATOR_H_
#define _CHELIB_PDF_CREATOR_H_

#include "che_pdf_parser.h"
#include "che_pdf_collector.h"

namespace chelib {

class CPDF_Creator : public CObject
{
public:
	static CPDF_Creator *	Create( IWrite * pWrite, CAllocator * pAllocator = nullptr );

    void				OutPutObject( IWrite * pWrite, const PDF_RefInfo refInfo, const CPDF_ObjectPtr & pObj, CPDF_Encrypt * pEncrypt = nullptr );

	~CPDF_Creator();

	bool						SetEncrypt( CPDF_Encrypt * pEncrypt ) { mpEncrypt = pEncrypt; return TRUE; }

	bool						SetEncryption(	const CByteString id, const CByteString userPassword, const CByteString ownerPassword,
											BYTE algorithm, BYTE keyLength, BYTE revision,  bool bMetaData, size_t pValue );
    
    void                     SetCompress( bool bEnable ) { mbCompress = bEnable; }

	void						OutPutFileHead( PDF_VERSION version );

	size_t					OutPutObject( const PDF_RefInfo refInfo, const CPDF_ObjectPtr & pObj );

	size_t					OutPutInObject( const PDF_RefInfo & refInfo, const CPDF_ObjectPtr & objPtr, bool bEncrypt = false );

	size_t					OutPutXRefTable( CPDF_XRefTable & xref );

	void						OutPutTailerDict( const CPDF_DictionaryPtr & pDcit );

	void						OutPutFileTailer( size_t startxref );

private:
	CPDF_Creator( IWrite * pWrite, CAllocator * pAllocator = nullptr );

	IWrite *					mpWrite;

	CPDF_Encrypt *			mpEncrypt;
    
    bool                     mbCompress;

	friend class CAllocator;
};

}//namespace

#endif