#ifndef _CHELIB_PDF_FILE_H_
#define _CHELIB_PDF_FILE_H_

#include "../base.h"
#include "pdf_parser.h"
#include "pdf_collector.h"

namespace chelib {

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


class CPDF_File : public CObject
{
public:
	CPDF_File( CAllocator * pAllocator = nullptr );
	~CPDF_File();

	bool Open( IRead * pRead );
	void Close();

	bool Authenticate( const CByteString & password ) const;
	
	size_t GetFileSize() const;
	PDF_VERSION GetPDFVersion() const;
	CPDF_DictionaryPtr GetTrailerDict() const;
	CPDF_DictionaryPtr GetRootDict();
	CPDF_DictionaryPtr GetInfoDict();
	CPDF_ArrayPtr GetIDArray();
	CPDF_XRefTable * GetXRefTable() { return &mXRefTable; }
	CPDF_ObjectPtr GetObject( const PDF_RefInfo & refInfo );

	void SetPDFVersion( PDF_VERSION version );
	bool SetInfo( PDF_DOCUMENT_INFO infoType, const CByteString & str );

	bool Save( IWrite * pWrite, bool bCompress = TRUE );
	bool SaveCompact( IWrite * pWrite );
	bool SaveUpdate( IWrite * pWrite );

	void CreateTrailerDict();
	void CreateCatalogDict();
    
	PDF_RefInfo CreateNullObject( CPDF_NullPtr & ptrRet );
	PDF_RefInfo CreateBooleanObject( CPDF_BooleanPtr & ptrRet );
	PDF_RefInfo CreateNumberObject( CPDF_NumberPtr & ptrRet );
	PDF_RefInfo CreateStringObject( CPDF_StringPtr & ptrRet );
	PDF_RefInfo CreateNameObject( CPDF_NamePtr & ptrRet );
	PDF_RefInfo CreateArrayObject( CPDF_ArrayPtr & ptrRet );
	PDF_RefInfo CreateDictObject( CPDF_DictionaryPtr & ptrRet );
	PDF_RefInfo CreateStreamObject( CPDF_StreamPtr & ptrRet );

private:
	CLock				mLock;
	PDF_VERSION         mVersion;
	CPDF_Parser *		mpParser;
	CPDF_Collector		mObjCollector;
	CPDF_XRefTable		mXRefTable;
};

}//namespace

#endif