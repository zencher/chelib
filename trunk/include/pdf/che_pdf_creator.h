#ifndef _CHE_PDF_CREATOR_H_
#define _CHE_PDF_CREATOR_H_

#include "../che_base.h"
#include "che_pdf_objects.h"
#include "che_pdf_parser.h"
#include "che_pdf_collector.h"

class CHE_PDF_Creator : public CHE_Object
{
public:
	static CHE_PDF_Creator *	Create( IHE_Write * pWrite, CHE_Allocator * pAllocator = NULL );

    HE_VOID				OutPutObject( IHE_Write * pWrite, const PDF_RefInfo refInfo, const CHE_PDF_ObjectPtr & pObj, CHE_PDF_Encrypt * pEncrypt = NULL );

	~CHE_PDF_Creator();

	HE_BOOL						SetEncrypt( CHE_PDF_Encrypt * pEncrypt ) { mpEncrypt = pEncrypt; return TRUE; }

	HE_BOOL						SetEncryption(	const CHE_ByteString id, const CHE_ByteString userPassword, const CHE_ByteString ownerPassword,
											HE_BYTE algorithm, HE_BYTE keyLength, HE_BYTE revision,  HE_BOOL bMetaData, HE_ULONG pValue );
    
    HE_VOID                     SetCompress( HE_BOOL bEnable ) { mbCompress = bEnable; }

	HE_VOID						OutPutFileHead( PDF_VERSION version );

	HE_ULONG					OutPutObject( const PDF_RefInfo refInfo, const CHE_PDF_ObjectPtr & pObj );

	HE_ULONG					OutPutInObject( const PDF_RefInfo & refInfo, const CHE_PDF_ObjectPtr & objPtr, HE_BOOL bEncrypt = FALSE );

	HE_ULONG					OutPutXRefTable( CHE_PDF_XRefTable & xref );

	HE_VOID						OutPutTailerDict( const CHE_PDF_DictionaryPtr & pDcit );

	HE_VOID						OutPutFileTailer( HE_ULONG startxref );

private:
	CHE_PDF_Creator( IHE_Write * pWrite, CHE_Allocator * pAllocator = NULL );

	IHE_Write *					mpWrite;

	CHE_PDF_Encrypt *			mpEncrypt;
    
    HE_BOOL                     mbCompress;

	friend class CHE_Allocator;
};

#endif