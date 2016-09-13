#include "../../include/che_pdf_nametree.h"

namespace chelib {

//using namespace std;

    bool CPDF_NameTree::Find( const std::string & name, const CPDF_DictionaryPtr & dict, CPDF_ObjectPtr & objRet )
{
	CPDF_ObjectPtr obj;
	CPDF_ArrayPtr arr;
	obj = dict->GetElement( "Limits", OBJ_TYPE_ARRAY );
	if ( obj )
	{
		arr = obj->GetArrayPtr();
		obj = arr->GetElement( 0, OBJ_TYPE_STRING );
		CByteString l1 = obj->GetStringPtr()->GetString();
		obj = arr->GetElement( 1, OBJ_TYPE_STRING );
		CByteString l2 = obj->GetStringPtr()->GetString();
		std::string str1( l1.GetData() );
		std::string str2( l2.GetData() );
		if ( name >= str1 && name <= str2 )
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
						if ( Find( name, obj->GetDictPtr(), objRet ) )
						{
							return TRUE;
						}
					}
				}
			}

			obj = dict->GetElement( "Names", OBJ_TYPE_ARRAY );
			if ( obj )
			{
				arr = obj->GetArrayPtr();
				for ( uint32 i = 0; i < arr->GetCount(); i+=2 )
				{
					obj = arr->GetElement( i, OBJ_TYPE_STRING );
					if ( obj )
					{
						str1 = obj->GetStringPtr()->GetString().GetData();
					}
					obj = arr->GetElement( i+1 );
					if ( obj )
					{
						mMap.insert( make_pair( str1, obj ) ); 
					}
				}
			}
            std::unordered_map<std::string,CPDF_ObjectPtr>::iterator it;
			it = mMap.find( name );
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
				if ( Find( name, obj->GetDictPtr(), objRet ) )
				{
					return TRUE;
				}
			}
		}
	}
	return false;
}

CPDF_ObjectPtr CPDF_NameTree::GetObject( const CByteString & name )
{
	CPDF_ObjectPtr objRet;
	if ( !mDictPtr )
	{
		return objRet;
	}

	std::string str = name.GetData();
    std::unordered_map<std::string,CPDF_ObjectPtr>::iterator it;
	it = mMap.find( str );
	if ( it != mMap.end() )
	{
		objRet = it->second;
	}else{
		Find( str, mDictPtr, objRet );
	}
	return objRet;
}

}//namespace