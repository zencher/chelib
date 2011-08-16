#include "../../include/pdf/che_pdf_collector.h"

HE_BOOL CHE_PDF_Collector::Add( CHE_PDF_Object * pObj )
{
	if ( pObj == NULL )
	{
		return FALSE;
	}
	return m_map.Append( pObj->GetObjNum(), (HE_LPVOID)pObj );
}

HE_VOID CHE_PDF_Collector::ReleaseObj()
{
	HE_DWORD count = m_map.GetCount();
	CHE_PDF_Object * pObj = NULL;
	for ( HE_DWORD i = 0; i < count; i++ )
	{
		pObj = (CHE_PDF_Object*)m_map.GetItemByIndex( i );
		if ( pObj )
		{
			pObj->Release();
		}
	}
}