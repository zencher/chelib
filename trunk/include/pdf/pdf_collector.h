#ifndef _CHELIB_PDF_COLLECTOR_H_
#define _CHELIB_PDF_COLLECTOR_H_

#include "pdf_object.h"

#include <unordered_map>

namespace chelib {

class CPDF_Collector : public CObject
{
public:
	CPDF_Collector( CAllocator * pAllocator = nullptr ) : CObject( pAllocator ) {}
	~CPDF_Collector() { Clear(); }

	size_t GetCount() { return mMap.size(); }

	bool Add( size_t objNum, const CPDF_ObjectPtr & ObjPtr )
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

	CPDF_ObjectPtr GetObj( size_t objNum )
	{
        std::unordered_map<size_t,CPDF_ObjectPtr>::iterator it;
		it = mMap.find( objNum );
		if ( it != mMap.end() )
		{
			return it->second;
		}
		return CPDF_ObjectPtr(); 
	}

	void Clear()
	{
		mMap.clear();
	}

private:
    std::unordered_map<size_t,CPDF_ObjectPtr> mMap;
};

}//namespace

#endif