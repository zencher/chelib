#ifndef _CHE_PDF_COLLECTOR_H_
#define _CHE_PDF_COLLECTOR_H_

#include "../che_base.h"
#include "che_pdf_objects.h"

#include <unordered_map>

using namespace std;

class CHE_PDF_Collector : public CHE_Object
{
public:
	CHE_PDF_Collector( CHE_Allocator * pAllocator = nullptr ) : CHE_Object( pAllocator ) {}
	~CHE_PDF_Collector() { Clear(); }

	size_t GetCount() { return mMap.size(); }

	bool Add( size_t objNum, const CHE_PDF_ObjectPtr & ObjPtr )
	{
		if ( ! ObjPtr )
		{
			return false;
		}
		mMap[objNum] = ObjPtr;
		return TRUE;
	}

	bool IsExist( size_t objNum )
	{
		if ( mMap.count( objNum ) )
		{
			return TRUE;
		}
		return false;
	}

	CHE_PDF_ObjectPtr GetObj( size_t objNum )
	{
		unordered_map<size_t,CHE_PDF_ObjectPtr>::iterator it;
		it = mMap.find( objNum );
		if ( it != mMap.end() )
		{
			return it->second;
		}
		return CHE_PDF_ObjectPtr(); 
	}

	void Clear()
	{
		mMap.clear();
	}

private:
	unordered_map<size_t,CHE_PDF_ObjectPtr> mMap;
};

#endif