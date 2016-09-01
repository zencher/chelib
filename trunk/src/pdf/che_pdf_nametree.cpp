#include "../../include/pdf/che_pdf_nametree.h"

using namespace std;

bool CHE_PDF_NameTree::Find( const string & name, const CHE_PDF_DictionaryPtr & dict, CHE_PDF_ObjectPtr & objRet )
{
	CHE_PDF_ObjectPtr obj;
	CHE_PDF_ArrayPtr arr;
	obj = dict->GetElement( "Limits", OBJ_TYPE_ARRAY );
	if ( obj )
	{
		arr = obj->GetArrayPtr();
		obj = arr->GetElement( 0, OBJ_TYPE_STRING );
		CHE_ByteString l1 = obj->GetStringPtr()->GetString();
		obj = arr->GetElement( 1, OBJ_TYPE_STRING );
		CHE_ByteString l2 = obj->GetStringPtr()->GetString();
		string str1( l1.GetData() );
		string str2( l2.GetData() );
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
			unordered_map<string,CHE_PDF_ObjectPtr>::iterator it;
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

CHE_PDF_ObjectPtr CHE_PDF_NameTree::GetObject( const CHE_ByteString & name )
{
	CHE_PDF_ObjectPtr objRet;
	if ( !mDictPtr )
	{
		return objRet;
	}

	string str = name.GetData();
	unordered_map<string,CHE_PDF_ObjectPtr>::iterator it;
	it = mMap.find( str );
	if ( it != mMap.end() )
	{
		objRet = it->second;
	}else{
		Find( str, mDictPtr, objRet );
	}
	return objRet;
}
