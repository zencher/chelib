#ifndef _CHE_PDF_FILE_H_
#define _CHE_PDF_FILE_H_

#include "../che_base.h"
#include "che_pdf_parser.h"
#include "che_pdf_collector.h"


enum PDF_DOCUMENT_INFO
{
	DOCUMENT_INFO_TITLE			= 0x00,
	DOCUMENT_INFO_AUTHOR		= 0x01,
	DOCUMENT_INFO_SUBJECT		= 0x02,
	DOCUMENT_INFO_KEYWORDS		= 0x03,
	DOCUMENT_INFO_CREATOR		= 0x04,
	DOCUMENT_INFO_PRODUCER		= 0x05,
	DOCUMENT_INFO_CREATIONDATE	= 0x06,
	DOCUMENT_INFO_MODDATE		= 0x07,
	DOCUMENT_INFO_TRAPPED		= 0x08
};


class CHE_PDF_File : public CHE_Object
{
public:
	CHE_PDF_File( CHE_Allocator * pAllocator = nullptr );
	~CHE_PDF_File();

	bool					Open( IHE_Read * pRead );
	void					Close();

	bool					Authenticate( const CHE_ByteString & password ) const;
	
	size_t                  GetFileSize() const;
	PDF_VERSION				GetPDFVersion() const;
	CHE_PDF_DictionaryPtr	GetTrailerDict() const;
	CHE_PDF_DictionaryPtr	GetRootDict();
	CHE_PDF_DictionaryPtr	GetInfoDict();
	CHE_PDF_ArrayPtr		GetIDArray();
	CHE_PDF_XRefTable *		GetXRefTable() { return &mXRefTable; }
	CHE_PDF_ObjectPtr		GetObject( const PDF_RefInfo & refInfo );

	void					SetPDFVersion( PDF_VERSION version );
	bool					SetInfo( PDF_DOCUMENT_INFO infoType, const CHE_ByteString & str );

	bool					Save( IHE_Write * pWrite, bool bCompress = TRUE );
	bool					SaveCompact( IHE_Write * pWrite );
	bool					SaveUpdate( IHE_Write * pWrite );

	void					CreateTrailerDict();
	void					CreateCatalogDict();
	PDF_RefInfo				CreateNullObject( CHE_PDF_NullPtr & ptrRet );
	PDF_RefInfo				CreateBooleanObject( CHE_PDF_BooleanPtr & ptrRet );
	PDF_RefInfo				CreateNumberObject( CHE_PDF_NumberPtr & ptrRet );
	PDF_RefInfo				CreateStringObject( CHE_PDF_StringPtr & ptrRet );
	PDF_RefInfo				CreateNameObject( CHE_PDF_NamePtr & ptrRet );
	PDF_RefInfo				CreateArrayObject( CHE_PDF_ArrayPtr & ptrRet );
	PDF_RefInfo				CreateDictObject( CHE_PDF_DictionaryPtr & ptrRet );
	PDF_RefInfo				CreateStreamObject( CHE_PDF_StreamPtr & ptrRet );

private:
	CHE_Lock				mLock;
	PDF_VERSION				mVersion;
	CHE_PDF_Parser *		mpParser;
	CHE_PDF_Collector		mObjCollector;
	CHE_PDF_XRefTable		mXRefTable;
};

#endif