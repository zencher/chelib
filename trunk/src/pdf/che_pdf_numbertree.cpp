#include "../../include/pdf/che_pdf_numbertree.h"

using namespace std;

HE_VOID CHE_PDF_NumberTree::Parse(const CHE_PDF_ReferencePtr & refPtr)
{
	if ( !refPtr )
	{
		return;
	}
	mRefPtr = refPtr;
}

HE_BOOL CHE_PDF_NumberTree::Find(HE_INT32 num, const CHE_PDF_DictionaryPtr & dict, CHE_PDF_ObjectPtr & objRet)
{
	CHE_PDF_ObjectPtr obj;
	CHE_PDF_ArrayPtr arr;
	HE_INT32 num1, num2;
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
				for ( HE_UINT32 i = 0; i < arr->GetCount(); ++i )
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

			obj = dict->GetElement( "Names", OBJ_TYPE_ARRAY );
			if ( obj )
			{
				arr = obj->GetArrayPtr();
				for ( HE_UINT32 i = 0; i < arr->GetCount(); i+=2 )
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
			unordered_map<HE_INT32,CHE_PDF_ObjectPtr>::iterator it;
			it = mMap.find( num );
			if ( it != mMap.end() )
			{
				objRet = it->second;
				return TRUE;
			}
		}
		return FALSE;
	}
		
	obj = dict->GetElement( "Kids", OBJ_TYPE_ARRAY );
	if ( obj )
	{
		arr = obj->GetArrayPtr();
		for ( HE_UINT32 i = 0; i < arr->GetCount(); ++i )
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
	return FALSE;
}

CHE_PDF_ObjectPtr CHE_PDF_NumberTree::GetObject( HE_INT32 num )
{
	CHE_PDF_ObjectPtr objRet;
	if ( !mRefPtr )
	{
		return objRet;
	}

	unordered_map<HE_INT32,CHE_PDF_ObjectPtr>::iterator it;
	it = mMap.find( num );
	if ( it != mMap.end() )
	{
		objRet = it->second;
	}
	else
	{
		CHE_PDF_ObjectPtr obj;
		obj = mRefPtr->GetRefObj( OBJ_TYPE_DICTIONARY );
		if ( obj )
		{
			Find( num, obj->GetDictPtr(), objRet );
		}
	}
	return objRet;
}