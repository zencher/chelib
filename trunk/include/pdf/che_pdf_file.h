#ifndef _CHE_PDF_FILE_H_
#define _CHE_PDF_FILE_H_

#include "../che_base.h"
#include "che_pdf_parser.h"
#include "che_pdf_collector.h"

class CHE_PDF_File : public CHE_Object
{
public:

	CHE_PDF_File( CHE_Allocator * pAllocator = NULL );

	~CHE_PDF_File();

	HE_BOOL					Open( IHE_Read * pRead );

	HE_VOID					Close();

	HE_BOOL					Save( IHE_Write * pWrite );

	HE_BOOL					SaveCompact( IHE_Write * pWrite );

	HE_BOOL					SaveUpdate( IHE_Write * pWrite );

	HE_DWORD				GetFileSize() const;

	PDF_VERSION				GetPDFVersion() const;

	CHE_PDF_DictionaryPtr	GetTrailerDict() const;

	CHE_PDF_DictionaryPtr	GetRootDict();

	CHE_PDF_DictionaryPtr	GetInfoDict();

	CHE_PDF_ArrayPtr		GetIDArray();

	HE_VOID					CreateTrailerDict();

	HE_VOID					CreateCatalogDict();

	HE_BOOL					Authenticate( const CHE_ByteString & password ) const;

	CHE_PDF_ObjectPtr		GetObject( const PDF_RefInfo & refInfo );

	PDF_RefInfo			CreateNullObject( CHE_PDF_NullPtr & ptrRet );
	
	PDF_RefInfo			CreateBooleanObject( CHE_PDF_BooleanPtr & ptrRet );

	PDF_RefInfo			CreateNumberObject( CHE_PDF_NumberPtr & ptrRet );

	PDF_RefInfo			CreateStringObject( CHE_PDF_StringPtr & ptrRet );

	PDF_RefInfo			CreateNameObject( CHE_PDF_NamePtr & ptrRet );

	PDF_RefInfo			CreateArrayObject( CHE_PDF_ArrayPtr & ptrRet );

	PDF_RefInfo			CreateDictObject( CHE_PDF_DictionaryPtr & ptrRet );

	PDF_RefInfo			CreateStreamObject( CHE_PDF_StreamPtr & ptrRet );

private:

	PDF_VERSION				mVersion;

	CHE_PDF_Parser *		mpParser;
	CHE_PDF_Collector		mObjCollector;
	CHE_PDF_XREF_Table		mXRefTable;
};

#endif