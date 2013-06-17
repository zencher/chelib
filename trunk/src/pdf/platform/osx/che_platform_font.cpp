#include "../../../../include/pdf/che_pdf_fontmgr.h"

class CHE_MacOSXFontMgr : public IHE_SystemFontMgr
{
public:
	CHE_MacOSXFontMgr( CHE_Allocator * pAllocator = NULL );
	~CHE_MacOSXFontMgr();

	CHE_ByteString GetFontFilePath( const CHE_ByteString & fontName );

	CHE_ByteString GetFontFilePath( HE_FLOAT ascent, HE_FLOAT descent );
};


IHE_SystemFontMgr * IHE_SystemFontMgr::Create( CHE_Allocator * pAllocator /*= NULL*/ )
{
    if ( pAllocator == NULL )
	{
		pAllocator = GetDefaultAllocator();
	}

	IHE_SystemFontMgr * pSystemFontMgr = pAllocator->New<CHE_MacOSXFontMgr>( pAllocator );
	
	return pSystemFontMgr;
}


HE_VOID IHE_SystemFontMgr::Destroy( IHE_SystemFontMgr * pSystemFontMgr )
{
	if ( pSystemFontMgr )
	{
		pSystemFontMgr->GetAllocator()->Delete<IHE_SystemFontMgr>( pSystemFontMgr );
	}
}

CHE_MacOSXFontMgr::CHE_MacOSXFontMgr( CHE_Allocator * pAllocator /*= NULL*/ )
    : IHE_SystemFontMgr( pAllocator )
{
    
}

CHE_MacOSXFontMgr::~CHE_MacOSXFontMgr()
{
}


CHE_ByteString CHE_MacOSXFontMgr::GetFontFilePath( const CHE_ByteString & fontName )
{
	return "";
}


CHE_ByteString CHE_MacOSXFontMgr::GetFontFilePath( HE_FLOAT ascent, HE_FLOAT descent )
{
	return "";
}