#ifndef _CHE_PDF_CREATOR_H_
#define _CHE_PDF_CREATOR_H_

#include "../che_base.h"
#include "che_pdf_objects.h"
#include "che_pdf_parser.h"
#include "che_pdf_collector.h"

class CHE_PDF_Creator : public CHE_Object
{
public:
	static CHE_PDF_Creator *	Create( IHE_Write * pWrite, CHE_Allocator * pAllocator = nullptr );

    void				OutPutObject( IHE_Write * pWrite, const PDF_RefInfo refInfo, const CHE_PDF_ObjectPtr & pObj, CHE_PDF_Encrypt * pEncrypt = nullptr );

	~CHE_PDF_Creator();

	bool						SetEncrypt( CHE_PDF_Encrypt * pEncrypt ) { mpEncrypt = pEncrypt; return TRUE; }

	bool						SetEncryption(	const CHE_ByteString id, const CHE_ByteString userPassword, const CHE_ByteString ownerPassword,
											BYTE algorithm, BYTE keyLength, BYTE revision,  bool bMetaData, size_t pValue );
    
    void                     SetCompress( bool bEnable ) { mbCompress = bEnable; }

	void						OutPutFileHead( PDF_VERSION version );

	size_t					OutPutObject( const PDF_RefInfo refInfo, const CHE_PDF_ObjectPtr & pObj );

	size_t					OutPutInObject( const PDF_RefInfo & refInfo, const CHE_PDF_ObjectPtr & objPtr, bool bEncrypt = false );

	size_t					OutPutXRefTable( CHE_PDF_XRefTable & xref );

	void						OutPutTailerDict( const CHE_PDF_DictionaryPtr & pDcit );

	void						OutPutFileTailer( size_t startxref );

private:
	CHE_PDF_Creator( IHE_Write * pWrite, CHE_Allocator * pAllocator = nullptr );

	IHE_Write *					mpWrite;

	CHE_PDF_Encrypt *			mpEncrypt;
    
    bool                     mbCompress;

	friend class CHE_Allocator;
};

#endif