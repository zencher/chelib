#include "../../include/che_pdf_contentresmgr.h"

namespace chelib {

CByteString CPDF_ContentResMgr::CreateName( PDF_CONTENTRES_TYPE type, const CPDF_ObjectPtr & pObj )
{
	CPDF_DictionaryPtr pSubDict;
	pSubDict = GetSubDict( type );
	if ( !pSubDict )
	{
		return "";
	}
	CByteString nameRet;
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

CPDF_ObjectPtr CPDF_ContentResMgr::GetResObj( PDF_CONTENTRES_TYPE type, const CByteString & name )
{
	CPDF_DictionaryPtr pSubDict = GetSubDict( type );
	if ( pSubDict )
	{
		return pSubDict->GetElement( name );
	}
	return CPDF_ObjectPtr();
}

CPDF_ObjectPtr CPDF_ContentResMgr::GetResObj( const CByteString & name )
{
	CPDF_ObjectPtr pRet = GetResObj( CONTENTRES_EXTGSTATE, name );
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

CPDF_DictionaryPtr CPDF_ContentResMgr::GetSubDict( PDF_CONTENTRES_TYPE type )
{
	if ( !mpResDict )
	{
		return CPDF_DictionaryPtr();
	}
	CPDF_ObjectPtr pTmp;
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


CPDF_DictionaryPtr CPDF_ContentResMgr::CreateSubDict( PDF_CONTENTRES_TYPE type )
{
	CPDF_DictionaryPtr pTmpDict = CPDF_Dictionary::Create( mpResDict->GetAllocator() );
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

CByteString CPDF_ContentResMgr::RequestName( const CPDF_DictionaryPtr & pSubDict, const CByteString & name )
{
	if ( !pSubDict )
	{
		return "";
	}
	CPDF_ObjectPtr pTmpObj = pSubDict->GetElement( name );
	if ( !pTmpObj )
	{
		return name;
	}else
	{
		char tmpStr[128];
		size_t tmpInt = 0;
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

CPDF_ComponentPtr CPDF_ContentResMgr::GetComponent( const CByteString & name )
{
	CPDF_ComponentPtr ptr;
	std::unordered_map<std::string,CPDF_ComponentPtr>::iterator it;
	it = mCompontentsMap.find( std::string( name.GetData() ) );
	if ( it != mCompontentsMap.end() )
	{
		ptr = it->second;
	}
	return ptr;
}

CPDF_ComponentPtr CPDF_ContentResMgr::GetComponent( const CByteString & name, PDF_COMPONENT_TYPE type )
{
	CPDF_ComponentPtr ptr = GetComponent( name );
	if ( ptr && ptr->GetType() != type )
	{
		ptr.Reset();
	}
	return ptr;
}

bool	CPDF_ContentResMgr::PushComponent( const CByteString & name, const CPDF_ComponentPtr & component )
{
	if ( name.GetLength() == 0 || !component )
	{
		return false;
	}
    mCompontentsMap.insert( std::pair<std::string,CPDF_ComponentPtr>( std::string( name.GetData() ), component ) );
	return true;
}

}//namespace