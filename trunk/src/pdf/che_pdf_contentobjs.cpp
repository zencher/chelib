#include "../../include/pdf/che_pdf_contentobjs.h"

HE_BOOL CHE_PDF_ContentObject::SetGState( CHE_PDF_GState * pGSatae )
{
	if ( mpGState )
	{
		mpGState->GetAllocator()->Delete( mpGState );
		mpGState = NULL;
	}
	if ( pGSatae )
	{
		mpGState = pGSatae;
		return TRUE;
	}
	return FALSE;
}

CHE_PDF_ContentObjectList::~CHE_PDF_ContentObjectList()
{
	Clear();
}

CHE_PDF_ContentObjectList * CHE_PDF_ContentObjectList::Clone()
{
	CHE_PDF_ContentObjectList * pObjList = GetAllocator()->New<CHE_PDF_ContentObjectList>();

	ContentObjectList::iterator it = Begin();
	for ( ; it != End(); ++it )
	{
		pObjList->Append( *it );
	}
	return pObjList;
}

HE_VOID CHE_PDF_ContentObjectList::Clear()
{
	ContentObjectList::iterator it = Begin();
	CHE_PDF_ContentObject * pObj = NULL;
	for ( ; it != End(); ++it )
	{
		pObj = *it;
		if ( pObj )
		{
			pObj->GetAllocator()->Delete( pObj );
		}
	}
	mList.clear();
}


bool CHE_PDF_ContentObjectList::Append( CHE_PDF_ContentObject * pObj )
{
	if ( pObj )
	{
		mList.push_back( pObj );
		return true;
	}
	return false;
}

ContentObjectList::iterator CHE_PDF_ContentObjectList::Begin()
{
	return mList.begin();
}

ContentObjectList::iterator CHE_PDF_ContentObjectList::End()
{
	return mList.end();
}

HE_BOOL CHE_PDF_Text::SetTextObject( const CHE_PDF_ObjectPtr & pObj )
{
	if ( pObj )
	{
		mpObj = pObj;

		//»ñÈ¡×ÖÌå
		CHE_PDF_Font * pFont = GetGState()->GetTextFont();
		if ( pFont == NULL )
		{
			return FALSE;
		}

		std::vector<CHE_PDF_ObjectPtr> tmpArray; 
		if ( pObj->GetType() == OBJ_TYPE_STRING )
		{
			tmpArray.push_back( pObj );
		}
		else if ( pObj->GetType() == OBJ_TYPE_ARRAY )
		{
			CHE_PDF_ArrayPtr pArray = pObj.GetArrayPtr();
			CHE_PDF_ObjectPtr pObj;
			for ( HE_DWORD i = 0; i < pArray->GetCount(); ++i )
			{
				pObj = pArray->GetElement( i );
				if ( pObj && ( pObj->GetType() == OBJ_TYPE_STRING || pObj->GetType() == OBJ_TYPE_NUMBER ) )
				{
					tmpArray.push_back( pObj );
				}
			}
		}
		CHE_ByteString tmpStr;
		HE_INT32 charCode = 0;
		HE_INT32 kerning = 0;
		CHE_PDF_TextItem item;
		if ( pFont->IsSimpleFont() )
		{
			for ( HE_DWORD i = 0; i < tmpArray.size(); ++i )
			{
				if ( tmpArray[i]->GetType() == OBJ_TYPE_STRING )
				{
					tmpStr = tmpArray[i]->GetString()->GetString();
					for ( HE_DWORD index = 0; index < tmpStr.GetLength(); ++index )
					{
						charCode = HE_BYTE( tmpStr[index] );
						item.cid = charCode;
						item.ucs = pFont->GetUnicode( charCode );
						item.kerning = kerning;
						item.offsetX = 0;
						item.offsetY = 0;
						mItems.push_back( item );
						kerning = 0;
					}
				}
				else if ( tmpArray[i]->GetType() == OBJ_TYPE_NUMBER )
				{
					kerning = tmpArray[i]->GetNumber()->GetInteger();
				}
			}
		}
		else
		{
			for ( HE_DWORD i = 0; i < tmpArray.size(); ++i )
			{
				if ( tmpArray[i]->GetType() == OBJ_TYPE_STRING )
				{
					tmpStr = tmpArray[i]->GetString()->GetString();
					for ( HE_DWORD index = 0; index < tmpStr.GetLength(); index+=2 )
					{
						charCode = HE_BYTE( tmpStr[index] );
						charCode = ( charCode << 8 ) + HE_BYTE( tmpStr[index+1] );
						if ( pFont->GetEncodingType() == FONT_ENCODING_SELFDEF )
						{
							item.cid = charCode;
						}else{
							item.cid = pFont->GetCID( charCode );
						}
						item.ucs = pFont->GetUnicode( item.cid );
						item.kerning = kerning;
						item.offsetX = 0;
						item.offsetY = 0;
						mItems.push_back( item );
						kerning = 0;
					}
				}
				else if ( tmpArray[i]->GetType() == OBJ_TYPE_NUMBER )
				{
					kerning = tmpArray[i]->GetNumber()->GetInteger();
				}
			}
		}
		return TRUE;
	}
	return FALSE;
}


CHE_PDF_Path::~CHE_PDF_Path()
{
	mItems.clear();
}

CHE_PDF_Form::~CHE_PDF_Form()
{

}