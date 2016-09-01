#include "../../include/pdf/che_pdf_numbertree.h"

using namespace std;

CHE_PDF_ObjectPtr CHE_PDF_NumberTree::GetObject(int32 num)
{
	CHE_PDF_ObjectPtr objRet;
	if (!mDictPtr)
	{
		return objRet;
	}

	map<int32, CHE_PDF_ObjectPtr>::iterator it;
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

bool CHE_PDF_NumberTree::Find(int32 num, const CHE_PDF_DictionaryPtr & dict, CHE_PDF_ObjectPtr & objRet)
{
	CHE_PDF_ObjectPtr obj;
	CHE_PDF_ArrayPtr arr;
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
						mMap.insert( make_pair( num1, obj ) ); 
					}
				}
			}
			map<int32,CHE_PDF_ObjectPtr>::iterator it;
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

bool CHE_PDF_NumberTree::Parse(const CHE_PDF_DictionaryPtr & dict)
{
	int32 num;
	CHE_PDF_ObjectPtr obj;
	CHE_PDF_ArrayPtr arr;
	
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
				mMap.insert(make_pair(num, obj));
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

CHE_PDF_ObjectPtr CHE_PDF_NumberTree::First(int32 & ret)
{
	CHE_PDF_ObjectPtr obj;
	mIt = mMap.begin();
	if ( mIt != mMap.end() )
	{
		obj = mIt->second;
		ret = mIt->first;
	}
	return obj;
}

CHE_PDF_ObjectPtr CHE_PDF_NumberTree::Next(int32 & ret)
{
	CHE_PDF_ObjectPtr obj;
	++mIt;
	if (mIt != mMap.end())
	{
		obj = mIt->second;
		ret = mIt->first;
	}
	return obj;
}