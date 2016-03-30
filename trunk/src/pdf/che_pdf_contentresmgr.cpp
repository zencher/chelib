#include "../../include/pdf/che_pdf_contentresmgr.h"

CHE_ByteString CHE_PDF_ContentResMgr::CreateName( PDF_CONTENTRES_TYPE type, const CHE_PDF_ObjectPtr & pObj )
{
	CHE_PDF_DictionaryPtr pSubDict;
	pSubDict = GetSubDict( type );
	if ( !pSubDict )
	{
		return "";
	}
	CHE_ByteString nameRet;
	switch( type )
	{
	case CONTENTRES_EXTGSTATE:
		{
			nameRet = RequestName( pSubDict, "GS" );
			break;
		}
	case CONTENTRES_COLORSPACE:
		{
			nameRet = RequestName( pSubDict, "CS" );
			break;
		}
	case CONTENTRES_PATTERN:
		{
			nameRet = RequestName( pSubDict, "P" );
			break;
		}
	case CONTENTRES_SHADING:
		{
			nameRet = RequestName( pSubDict, "S" );
			break;
		}
	case CONTENTRES_XOBJECT:
		{
			nameRet = RequestName( pSubDict, "XOBJ" );
			break;
		}
	case CONTENTRES_FONT:
		{
			nameRet = RequestName( pSubDict, "F" );
			break;
		}
	case CONTENTRES_PROPERTIES:
		{
			nameRet = RequestName( pSubDict, "MP" );
			break;
		}
	default:
		break;
	}
	if ( nameRet.GetLength() > 0 )
	{
		pSubDict->SetObject( nameRet, pObj );
	}
	return nameRet;
}

CHE_PDF_ObjectPtr CHE_PDF_ContentResMgr::GetResObj( PDF_CONTENTRES_TYPE type, const CHE_ByteString & name )
{
	CHE_PDF_DictionaryPtr pSubDict = GetSubDict( type );
	if ( pSubDict )
	{
		return pSubDict->GetElement( name );
	}
	return CHE_PDF_ObjectPtr();
}

CHE_PDF_ObjectPtr CHE_PDF_ContentResMgr::GetResObj( const CHE_ByteString & name )
{
	CHE_PDF_ObjectPtr pRet = GetResObj( CONTENTRES_EXTGSTATE, name );
	if ( pRet )
	{
		return pRet;	
	}
	pRet = GetResObj( CONTENTRES_COLORSPACE, name );
	if ( pRet )
	{
		return pRet;	
	}
	pRet = GetResObj( CONTENTRES_PATTERN, name );
	if ( pRet )
	{
		return pRet;	
	}
	pRet = GetResObj( CONTENTRES_SHADING, name );
	if ( pRet )
	{
		return pRet;	
	}
	pRet = GetResObj( CONTENTRES_XOBJECT, name );
	if ( pRet )
	{
		return pRet;	
	}
	pRet = GetResObj( CONTENTRES_FONT, name );
	if ( pRet )
	{
		return pRet;	
	}
	pRet = GetResObj( CONTENTRES_PROPERTIES, name );
	return pRet;
}

CHE_PDF_DictionaryPtr CHE_PDF_ContentResMgr::GetSubDict( PDF_CONTENTRES_TYPE type )
{
	if ( !mpResDict )
	{
		return CHE_PDF_DictionaryPtr();
	}
	CHE_PDF_ObjectPtr pTmp;
	switch ( type )
	{
	case CONTENTRES_EXTGSTATE:
		{
			pTmp = mpResDict->GetElement( "ExtGState", OBJ_TYPE_DICTIONARY );
			break;
		}
	case CONTENTRES_COLORSPACE:
		{
			pTmp = mpResDict->GetElement( "ColorSpace", OBJ_TYPE_DICTIONARY );
			break;
		}
	case CONTENTRES_PATTERN:
		{
			pTmp = mpResDict->GetElement( "Pattern", OBJ_TYPE_DICTIONARY );
			break;
		}
	case CONTENTRES_SHADING:
		{
			pTmp = mpResDict->GetElement( "Shading", OBJ_TYPE_DICTIONARY );
			break;
		}
	case CONTENTRES_XOBJECT:
		{
			pTmp = mpResDict->GetElement( "XObject", OBJ_TYPE_DICTIONARY );
			break;
		}
	case CONTENTRES_FONT:
		{
			pTmp = mpResDict->GetElement( "Font", OBJ_TYPE_DICTIONARY );
			break;
		}
	case CONTENTRES_PROPERTIES:
		{
			pTmp = mpResDict->GetElement( "Properties", OBJ_TYPE_DICTIONARY );
			break;
		}
	default:
		break;
	}
	if ( pTmp )
	{
		return pTmp->GetDictPtr();
	}else{
		return CreateSubDict( type );
	}
}


CHE_PDF_DictionaryPtr CHE_PDF_ContentResMgr::CreateSubDict( PDF_CONTENTRES_TYPE type )
{
	CHE_PDF_DictionaryPtr pTmpDict = CHE_PDF_Dictionary::Create( mpResDict->GetAllocator() );
	switch ( type )
	{
	case CONTENTRES_EXTGSTATE:
		{
			mpResDict->SetDictionary( "ExtGState", pTmpDict );
			break;
		}
	case CONTENTRES_COLORSPACE:
		{
			mpResDict->SetDictionary( "ColorSpace", pTmpDict );
			break;
		}
	case CONTENTRES_PATTERN:
		{
			mpResDict->SetDictionary( "Pattern", pTmpDict );
			break;
		}
	case CONTENTRES_SHADING:
		{
			mpResDict->SetDictionary( "Shading", pTmpDict );
			break;
		}
	case CONTENTRES_XOBJECT:
		{
			mpResDict->SetDictionary( "XObject", pTmpDict );
			break;
		}
	case CONTENTRES_FONT:
		{
			mpResDict->SetDictionary( "Font", pTmpDict );
			break;
		}
	case CONTENTRES_PROPERTIES:
		{
			mpResDict->SetDictionary( "Properties", pTmpDict );
			break;
		}
	default:
		break;
	}
	return pTmpDict;
}

CHE_ByteString CHE_PDF_ContentResMgr::RequestName( const CHE_PDF_DictionaryPtr & pSubDict, const CHE_ByteString & name )
{
	if ( !pSubDict )
	{
		return "";
	}
	CHE_PDF_ObjectPtr pTmpObj = pSubDict->GetElement( name );
	if ( !pTmpObj )
	{
		return name;
	}else
	{
		HE_CHAR tmpStr[128];
		HE_ULONG tmpInt = 0;
		bool bContinue = true;
		while ( bContinue )
		{
			sprintf( tmpStr, "%s%ld", name.GetData(), tmpInt );
			pTmpObj = pSubDict->GetElement( tmpStr );
			if ( !pTmpObj )
			{
				bContinue = false;
				continue;;
			}
			++tmpInt;
		}
		return tmpStr;
	}
	return "";
}

CHE_PDF_ComponentPtr CHE_PDF_ContentResMgr::GetComponent( const CHE_ByteString & name )
{
	CHE_PDF_ComponentPtr ptr;
	std::unordered_map<std::string,CHE_PDF_ComponentPtr>::iterator it;
	it = mCompontentsMap.find( std::string( name.GetData() ) );
	if ( it != mCompontentsMap.end() )
	{
		ptr = it->second;
	}
	return ptr;
}

CHE_PDF_ComponentPtr CHE_PDF_ContentResMgr::GetComponent( const CHE_ByteString & name, PDF_COMPONENT_TYPE type )
{
	CHE_PDF_ComponentPtr ptr = GetComponent( name );
	if ( ptr && ptr->GetType() != type )
	{
		ptr.Reset();
	}
	return ptr;
}

HE_BOOL	CHE_PDF_ContentResMgr::PushComponent( const CHE_ByteString & name, const CHE_PDF_ComponentPtr & component )
{
	if ( name.GetLength() == 0 || !component )
	{
		return FALSE;
	}
	mCompontentsMap.insert( pair<std::string,CHE_PDF_ComponentPtr>( std::string( name.GetData() ), component ) );
	return TRUE;
}