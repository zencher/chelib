// dllmain.h : Declaration of module class.

class CPDFReaderHandlersModule : public ATL::CAtlDllModuleT< CPDFReaderHandlersModule >
{
public :
	DECLARE_LIBID(LIBID_PDFReaderHandlersLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_PDFREADERHANDLERS, "{C5815802-E8CD-459E-87D0-55F8407D115E}")
};

extern class CPDFReaderHandlersModule _AtlModule;
