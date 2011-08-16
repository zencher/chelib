#ifndef _CHE_PDF_COLLECTOR_H_
#define _CHE_PDF_COLLECTOR_H_

#include "../che_base.h"
#include "../che_datastructure.h"
#include "che_pdf_objects.h"

class CHE_PDF_Collector : public CHE_Object
{
public:
	CHE_PDF_Collector( CHE_Allocator * pAllocator = NULL ) : CHE_Object( pAllocator ), m_map( pAllocator ) {}
	~CHE_PDF_Collector() { ReleaseObj(); }

	HE_DWORD GetCount() { return m_map.GetCount(); }

	HE_BOOL Add( CHE_PDF_Object * pObj );

	HE_BOOL IsExist( HE_DWORD objNum ) { return m_map.GetItem( objNum ) ? TRUE : FALSE ; }

	CHE_PDF_Object * GetObj( HE_DWORD objNum ) { return (CHE_PDF_Object*)m_map.GetItem( objNum ); }

	CHE_PDF_Object * GetObjByIndex( HE_DWORD index ) { return (CHE_PDF_Object*)( m_map.GetItemByIndex( index ) ); }

	HE_VOID Clear() { m_map.Clear(); }

	HE_VOID ReleaseObj();

private:
	CHE_NumToPtrMap m_map;
};

#endif