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
	HE_BOOL					SaveUpdate( IHE_Write * pWrite );

	HE_DWORD				GetFileSize() const;
	PDF_VERSION				GetPDFVersion() const;
	CHE_PDF_Dictionary *	GetTrailerDict() const;
	CHE_PDF_Dictionary *	GetRootDict();
	CHE_PDF_Dictionary *	GetInfoDict();
	CHE_PDF_Array *			GetIDArray();

	HE_BOOL					Authenticate( const CHE_ByteString & password ) const;

	CHE_PDF_Object *		GetObject( const HE_PDF_RefInfo & refInfo );

	HE_BOOL					ChangeObject( const HE_PDF_RefInfo & refInfo, CHE_PDF_Object * pObj );

	CHE_PDF_IndirectObject*	CreateInObj_Null();
	CHE_PDF_IndirectObject*	CreateInObj_Boolean( HE_BOOL value );
	CHE_PDF_IndirectObject*	CreateInObj_Number( HE_INT32 value );
	CHE_PDF_IndirectObject*	CreateInObj_Number( HE_FLOAT value );
	CHE_PDF_IndirectObject*	CreateInObj_String( const CHE_ByteString & str );
	CHE_PDF_IndirectObject*	CreateInObj_Name( const CHE_ByteString & str );
	CHE_PDF_IndirectObject*	CreateInObj_Array();
	CHE_PDF_IndirectObject*	CreateInObj_Dict();
	CHE_PDF_IndirectObject*	CreateInObj_Stream();

private:
	HE_PDF_RefInfo			TakeFreeRefIno();
	HE_VOID					ReleaseRefInfo( const HE_PDF_RefInfo & refInfo );

private:
	CHE_PDF_Parser *		mpParser;
	CHE_PDF_Collector		mObjCollector;
	CHE_PDF_XREF_Table		mXRefTable;
};

#endif