#include "../../include/pdf/che_pdf_file.h"

CHE_PDF_File::CHE_PDF_File( CHE_Allocator * pAllocator )
	: CHE_Object( pAllocator ), mParser( pAllocator ), mObjCollector( pAllocator ) {}

CHE_PDF_File::~CHE_PDF_File()
{
	Close();
}

HE_BOOL CHE_PDF_File::Open( IHE_Read * pRead )
{
	if ( pRead == NULL )
	{
		return FALSE;
	}
	return mParser.Open( pRead );
}

HE_VOID CHE_PDF_File::Close()
{
	mObjCollector.ReleaseObj();
	mObjCollector.Clear();
	mParser.Close();
}

HE_BOOL CHE_PDF_File::Authenticate( const CHE_ByteString & password ) const
{
	return mParser.Authenticate( password );
}

CHE_PDF_Object * CHE_PDF_File::GetObject( const HE_PDF_RefInfo & refInfo )
{
	return mParser.GetObject( refInfo );
}

HE_DWORD CHE_PDF_File::GetFileSize() const
{
	return mParser.GetFileSize();
}

PDF_VERSION CHE_PDF_File::GetPDFVersion() const
{
	return mParser.GetPDFVersion();
}

CHE_PDF_Dictionary * CHE_PDF_File::GetTrailerDict() const
{
	return mParser.GetTrailerDict();
}

CHE_PDF_Dictionary * CHE_PDF_File::GetRootDict()
{
	return mParser.GetRootDict();
}

CHE_PDF_Dictionary * CHE_PDF_File::GetInfoDict()
{
	return mParser.GetRootDict();
}

CHE_PDF_Array * CHE_PDF_File::GetIDArray()
{
	return mParser.GetIDArray();
}