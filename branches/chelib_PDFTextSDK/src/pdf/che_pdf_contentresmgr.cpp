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
		pSubDict->SetAtObj( nameRet, pObj );
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
			mpResDict->SetAtDictionary( "ExtGState", pTmpDict );
			break;
		}
	case CONTENTRES_COLORSPACE:
		{
			mpResDict->SetAtDictionary( "ColorSpace", pTmpDict );
			break;
		}
	case CONTENTRES_PATTERN:
		{
			mpResDict->SetAtDictionary( "Pattern", pTmpDict );
			break;
		}
	case CONTENTRES_SHADING:
		{
			mpResDict->SetAtDictionary( "Shading", pTmpDict );
			break;
		}
	case CONTENTRES_XOBJECT:
		{
			mpResDict->SetAtDictionary( "XObject", pTmpDict );
			break;
		}
	case CONTENTRES_FONT:
		{
			mpResDict->SetAtDictionary( "Font", pTmpDict );
			break;
		}
	case CONTENTRES_PROPERTIES:
		{
			mpResDict->SetAtDictionary( "Properties", pTmpDict );
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
		HE_DWORD tmpInt = 0;
		bool bContinue = true;
		while ( bContinue )
		{
			sprintf_s( tmpStr, "%s%d", name.GetData(), tmpInt );
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