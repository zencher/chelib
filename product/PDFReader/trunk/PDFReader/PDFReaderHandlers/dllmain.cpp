// dllmain.cpp : Implementation of DllMain.

#include "stdafx.h"
#include "resource.h"
#include "PDFReaderHandlers_i.h"
#include "dllmain.h"
#include "xdlldata.h"

CPDFReaderHandlersModule _AtlModule;

class CPDFReaderHandlersApp : public CWinApp
{
public:

// Overrides
	virtual BOOL InitInstance();
	virtual int ExitInstance();

	DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(CPDFReaderHandlersApp, CWinApp)
END_MESSAGE_MAP()

CPDFReaderHandlersApp theApp;

BOOL CPDFReaderHandlersApp::InitInstance()
{
	if (!PrxDllMain(m_hInstance, DLL_PROCESS_ATTACH, NULL))
		return FALSE;
	return CWinApp::InitInstance();
}

int CPDFReaderHandlersApp::ExitInstance()
{
	return CWinApp::ExitInstance();
}
