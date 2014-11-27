#include "../../include/pdf/che_pdf_nametree.h"

using namespace std;

HE_VOID CHE_PDF_NameTree::Parse( CHE_PDF_ReferencePtr & refPtr )
{
	if ( !refPtr )
	{
		return;
	}
	mRefPtr = refPtr;
}

HE_BOOL CHE_PDF_NameTree::Find( string & name, CHE_PDF_DictionaryPtr & dict, CHE_PDF_ObjectPtr & objRet )
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
				for ( HE_UINT32 i = 0; i < arr->GetCount(); ++i )
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
				for ( HE_UINT32 i = 0; i < arr->GetCount(); i+=2 )
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
				if ( Find( name, obj->GetDictPtr(), objRet ) )
				{
					return TRUE;
				}
			}
		}
	}
	return FALSE;
}

CHE_PDF_ObjectPtr CHE_PDF_NameTree::GetObject( CHE_ByteString & name )
{
	CHE_PDF_ObjectPtr objRet;
	if ( !mRefPtr )
	{
		return objRet;
	}

	string str = name.GetData();
	unordered_map<string,CHE_PDF_ObjectPtr>::iterator it;
	it = mMap.find( str );
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
			Find( str, obj->GetDictPtr(), objRet );
		}
	}
	return objRet;
}

CHE_PDF_NameDict::CHE_PDF_NameDict( CHE_Allocator * pAllocator /*= NULL*/ )
	: CHE_Object( pAllocator ), mpDestNameTree( NULL ), mpAPNameTree( NULL ),
	mpJSNameTree( NULL ), mpPagesNameTree( NULL ), mpTemplatesNameTree( NULL ),
	mpIDSNameTree( NULL ), mpURLSNameTree( NULL ), mpEFNameTree( NULL ),
	mpAlPrNameTree( NULL ), mpRendNameTree( NULL ) {}

CHE_PDF_NameDict::~CHE_PDF_NameDict()
{
	if ( mpDestNameTree )
	{
		mpDestNameTree->GetAllocator()->Delete( mpDestNameTree );
	}
	if ( mpAPNameTree )
	{
		mpAPNameTree->GetAllocator()->Delete( mpAPNameTree );
	}
	if ( mpJSNameTree )
	{
		mpJSNameTree->GetAllocator()->Delete( mpJSNameTree );
	}
	if ( mpPagesNameTree )
	{
		mpPagesNameTree->GetAllocator()->Delete( mpPagesNameTree );
	}
	if ( mpTemplatesNameTree )
	{
		mpTemplatesNameTree->GetAllocator()->Delete( mpTemplatesNameTree );
	}
	if ( mpIDSNameTree )
	{
		mpIDSNameTree->GetAllocator()->Delete( mpIDSNameTree );
	}
	if ( mpURLSNameTree )
	{
		mpURLSNameTree->GetAllocator()->Delete( mpURLSNameTree );
	}
	if ( mpEFNameTree )
	{
		mpEFNameTree->GetAllocator()->Delete( mpEFNameTree );
	}
	if ( mpAlPrNameTree )
	{
		mpAlPrNameTree->GetAllocator()->Delete( mpAlPrNameTree );
	}
	if ( mpRendNameTree )
	{
		mpRendNameTree->GetAllocator()->Delete( mpRendNameTree );
	}
}

HE_VOID CHE_PDF_NameDict::Parse( CHE_PDF_DictionaryPtr & dict )
{
	if ( !dict )
	{
		return;
	}

	CHE_PDF_ObjectPtr objPtr = dict->GetElement( "Dests", OBJ_TYPE_REFERENCE );
	if ( objPtr )
	{
		mpDestNameTree = GetAllocator()->New<CHE_PDF_NameTree>( GetAllocator() );
		mpDestNameTree->Parse( objPtr->GetRefPtr() );
	}

	objPtr = dict->GetElement( "AP", OBJ_TYPE_REFERENCE );
	if ( objPtr )
	{
		mpAPNameTree = GetAllocator()->New<CHE_PDF_NameTree>( GetAllocator() );
		mpAPNameTree->Parse( objPtr->GetRefPtr() );
	}

	objPtr = dict->GetElement( "JavaScript", OBJ_TYPE_REFERENCE );
	if ( objPtr )
	{
		mpJSNameTree = GetAllocator()->New<CHE_PDF_NameTree>( GetAllocator() );
		mpJSNameTree->Parse( objPtr->GetRefPtr() );
	}

	objPtr = dict->GetElement( "Pages", OBJ_TYPE_REFERENCE );
	if ( objPtr )
	{
		mpPagesNameTree = GetAllocator()->New<CHE_PDF_NameTree>( GetAllocator() );
		mpPagesNameTree->Parse( objPtr->GetRefPtr() );
	}

	objPtr = dict->GetElement( "Templates", OBJ_TYPE_REFERENCE );
	if ( objPtr )
	{
		mpTemplatesNameTree = GetAllocator()->New<CHE_PDF_NameTree>( GetAllocator() );
		mpTemplatesNameTree->Parse( objPtr->GetRefPtr() );
	}

	objPtr = dict->GetElement( "IDS", OBJ_TYPE_REFERENCE );
	if ( objPtr )
	{
		mpIDSNameTree = GetAllocator()->New<CHE_PDF_NameTree>( GetAllocator() );
		mpIDSNameTree->Parse( objPtr->GetRefPtr() );
	}

	objPtr = dict->GetElement( "URLS", OBJ_TYPE_REFERENCE );
	if ( objPtr )
	{
		mpURLSNameTree = GetAllocator()->New<CHE_PDF_NameTree>( GetAllocator() );
		mpURLSNameTree->Parse( objPtr->GetRefPtr() );
	}

	objPtr = dict->GetElement( "EmbeddedFile", OBJ_TYPE_REFERENCE );
	if ( objPtr )
	{
		mpEFNameTree = GetAllocator()->New<CHE_PDF_NameTree>( GetAllocator() );
		mpEFNameTree->Parse( objPtr->GetRefPtr() );
	}

	objPtr = dict->GetElement( "AlternatePresentations", OBJ_TYPE_REFERENCE );
	if ( objPtr )
	{
		mpAlPrNameTree = GetAllocator()->New<CHE_PDF_NameTree>( GetAllocator() );
		mpAlPrNameTree->Parse( objPtr->GetRefPtr() );
	}

	objPtr = dict->GetElement( "Renditions", OBJ_TYPE_REFERENCE );
	if ( objPtr )
	{
		mpRendNameTree = GetAllocator()->New<CHE_PDF_NameTree>( GetAllocator() );
		mpRendNameTree->Parse( objPtr->GetRefPtr() );
	}
}

CHE_PDF_ObjectPtr CHE_PDF_NameDict::GetDest( CHE_ByteString & name )
{
	CHE_PDF_ObjectPtr objPtr;
	if ( mpDestNameTree )
	{
		objPtr = mpDestNameTree->GetObject( name );
	}
	return objPtr;
}

CHE_PDF_ObjectPtr CHE_PDF_NameDict::GetJavaScript( CHE_ByteString & name )
{
	CHE_PDF_ObjectPtr objPtr;
	if ( mpDestNameTree )
	{
		objPtr = mpJSNameTree->GetObject( name );
	}
	return objPtr;
}
