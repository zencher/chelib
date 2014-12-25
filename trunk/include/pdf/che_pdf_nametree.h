#ifndef _CHE_PDF_NAMETREE_H_
#define _CHE_PDF_NAMETREE_H_

#include "../che_base.h"
#include "che_pdf_objects.h"

#include <string>
#include <unordered_map>

class CHE_PDF_NameTree : public CHE_Object
{
public:
	CHE_PDF_NameTree( CHE_Allocator * pAllocator = NULL ) : CHE_Object( pAllocator ) {};

	HE_VOID Parse( const CHE_PDF_ReferencePtr & refPtr );

	CHE_PDF_ObjectPtr GetObject( const CHE_ByteString & name );

private:
	HE_BOOL Find( string & name, const CHE_PDF_DictionaryPtr & dict, CHE_PDF_ObjectPtr & objRet );

	CHE_PDF_ReferencePtr								mRefPtr;
	std::unordered_map<std::string,CHE_PDF_ObjectPtr>	mMap;
};

class CHE_PDF_NameDict : public CHE_Object
{
public:
	CHE_PDF_NameDict( CHE_Allocator * pAllocator = NULL );

	~CHE_PDF_NameDict();

	HE_VOID Parse( const CHE_PDF_DictionaryPtr & dict );

	CHE_PDF_ObjectPtr GetDest( const CHE_ByteString & name );

	CHE_PDF_ObjectPtr GetJavaScript( const CHE_ByteString & name );

	//todo

private:
	CHE_PDF_NameTree *	mpDestNameTree;
	CHE_PDF_NameTree *	mpAPNameTree;
	CHE_PDF_NameTree *	mpJSNameTree;
	CHE_PDF_NameTree *	mpPagesNameTree;
	CHE_PDF_NameTree *	mpTemplatesNameTree;
	CHE_PDF_NameTree *	mpIDSNameTree;
	CHE_PDF_NameTree *	mpURLSNameTree;
	CHE_PDF_NameTree *	mpEFNameTree;
	CHE_PDF_NameTree *	mpAlPrNameTree;
	CHE_PDF_NameTree *	mpRendNameTree;
};

#endif