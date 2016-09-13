#include "../../include/che_pdf_numbertree.h"

namespace chelib {

//using namespace std;

CPDF_ObjectPtr CPDF_NumberTree::GetObject(int32 num)
{
	CPDF_ObjectPtr objRet;
	if (!mDictPtr)
	{
		return objRet;
	}

    std::map<int32, CPDF_ObjectPtr>::iterator it;
	it = mMap.find(num);
	if (it != mMap.end())
	{
		objRet = it->second;
	}
	else{
		Find(num, mDictPtr, objRet);
	}
	return objRet;
}

bool CPDF_NumberTree::Find(int32 num, const CPDF_DictionaryPtr & dict, CPDF_ObjectPtr & objRet)
{
	CPDF_ObjectPtr obj;
	CPDF_ArrayPtr arr;
	int32 num1, num2;
	obj = dict->GetElement( "Limits", OBJ_TYPE_ARRAY );
	if ( obj )
	{
		arr = obj->GetArrayPtr();
		obj = arr->GetElement( 0, OBJ_TYPE_NUMBER );
		num1 = obj->GetNumberPtr()->GetInteger();
		obj = arr->GetElement(1, OBJ_TYPE_NUMBER);
		num2 = obj->GetNumberPtr()->GetInteger();
		if ( num >= num1 && num <= num2 )
		{
			obj = dict->GetElement( "Kids", OBJ_TYPE_ARRAY );
			if ( obj )
			{
				arr = obj->GetArrayPtr();
				for ( uint32 i = 0; i < arr->GetCount(); ++i )
				{
					obj = arr->GetElement( i, OBJ_TYPE_DICTIONARY );
					if ( obj )
					{
						if ( Find( num, obj->GetDictPtr(), objRet ) )
						{
							return TRUE;
						}
					}
				}
			}

			obj = dict->GetElement( "Nums", OBJ_TYPE_ARRAY );
			if ( obj )
			{
				arr = obj->GetArrayPtr();
				for ( uint32 i = 0; i < arr->GetCount(); i+=2 )
				{
					obj = arr->GetElement( i, OBJ_TYPE_NUMBER );
					if ( obj )
					{
						num1 = obj->GetNumberPtr()->GetInteger();
					}
					obj = arr->GetElement( i+1 );
					if ( obj )
					{
                        mMap.insert( std::make_pair( num1, obj ) );
					}
				}
			}
            std::map<int32,CPDF_ObjectPtr>::iterator it;
			it = mMap.find( num );
			if ( it != mMap.end() )
			{
				objRet = it->second;
				return TRUE;
			}
		}
		return false;
	}
		
	obj = dict->GetElement( "Kids", OBJ_TYPE_ARRAY );
	if ( obj )
	{
		arr = obj->GetArrayPtr();
		for ( uint32 i = 0; i < arr->GetCount(); ++i )
		{
			obj = arr->GetElement( i, OBJ_TYPE_DICTIONARY );
			if ( obj )
			{
				if ( Find( num, obj->GetDictPtr(), objRet ) )
				{
					return TRUE;
				}
			}
		}
	}
	return false;
}

bool CPDF_NumberTree::Parse(const CPDF_DictionaryPtr & dict)
{
	int32 num;
	CPDF_ObjectPtr obj;
	CPDF_ArrayPtr arr;
	
	obj = dict->GetElement("Nums", OBJ_TYPE_ARRAY);
	if (obj)
	{
		arr = obj->GetArrayPtr();
		for (uint32 i = 0; i < arr->GetCount(); i += 2)
		{
			obj = arr->GetElement(i, OBJ_TYPE_NUMBER);
			if (obj)
			{
				num = obj->GetNumberPtr()->GetInteger();
			}
			obj = arr->GetElement(i + 1);
			if (obj)
			{
				mMap.insert(std::make_pair(num, obj));
			}
		}
	}
	obj = dict->GetElement("Kids", OBJ_TYPE_ARRAY);
	if (obj)
	{
		arr = obj->GetArrayPtr();
		for (uint32 i = 0; i < arr->GetCount(); ++i)
		{
			obj = arr->GetElement(i, OBJ_TYPE_DICTIONARY);
			if (obj)
			{
				Parse(obj->GetDictPtr());
			}
		}
	}
	return true;
}

CPDF_ObjectPtr CPDF_NumberTree::First(int32 & ret)
{
	CPDF_ObjectPtr obj;
	mIt = mMap.begin();
	if ( mIt != mMap.end() )
	{
		obj = mIt->second;
		ret = mIt->first;
	}
	return obj;
}

CPDF_ObjectPtr CPDF_NumberTree::Next(int32 & ret)
{
	CPDF_ObjectPtr obj;
	++mIt;
	if (mIt != mMap.end())
	{
		obj = mIt->second;
		ret = mIt->first;
	}
	return obj;
}

}//namesp