#ifndef _CHE_PDF_COLLECTOR_H_
#define _CHE_PDF_COLLECTOR_H_

#include "../che_base.h"
#include "che_pdf_objects.h"

#include <unordered_map>

using namespace std;

class CHE_PDF_Collector : public CHE_Object
{
public:
	CHE_PDF_Collector( CHE_Allocator * pAllocator = NULL ) : CHE_Object( pAllocator ) {}
	~CHE_PDF_Collector() { Clear(); }

	HE_ULONG GetCount() { return mMap.size(); }

	HE_BOOL Add( HE_ULONG objNum, const CHE_PDF_ObjectPtr & ObjPtr )
	{
		if ( ! ObjPtr )
		{
			return FALSE;
		}
		mMap[objNum] = ObjPtr;
		return TRUE;
	}

	HE_BOOL IsExist( HE_ULONG objNum )
	{
		if ( mMap.count( objNum ) )
		{
			return TRUE;
		}
		return FALSE;
	}

	CHE_PDF_ObjectPtr GetObj( HE_ULONG objNum )
	{
		unordered_map<HE_ULONG,CHE_PDF_ObjectPtr>::iterator it;
		it = mMap.find( objNum );
		if ( it != mMap.end() )
		{
			return it->second;
		}
		return CHE_PDF_ObjectPtr(); 
	}

	HE_VOID Clear()
	{
		mMap.clear();
	}

private:
	unordered_map<HE_ULONG,CHE_PDF_ObjectPtr> mMap;
};

#endif