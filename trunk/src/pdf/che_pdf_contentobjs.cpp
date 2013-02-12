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

HE_VOID CHE_PDF_ContentObjectList::SetType3BBox( const HE_INT32 type, std::vector<HE_FLOAT> & param )
{
	mType3DType = type;
	mType3Param = param;
}

HE_BOOL CHE_PDF_ContentObjectList::GetType3BBox( HE_INT32 & type, std::vector<HE_FLOAT> & param )
{
	type = mType3DType;
	param = mType3Param;
	return TRUE;
}

HE_BOOL CHE_PDF_Text::SetTextObject( const CHE_PDF_ObjectPtr & pObj )
{
	if ( pObj )
	{
		mpObj = pObj;

		//获取字体
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
			CHE_PDF_ArrayPtr pArray = pObj->GetArrayPtr();
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
		HE_INT32 kerning = 0;
		CHE_PDF_TextItem item;
		if ( pFont->GetType() != FONT_TYPE0 )
		{
			for ( HE_DWORD i = 0; i < tmpArray.size(); ++i )
			{
				if ( tmpArray[i]->GetType() == OBJ_TYPE_STRING )
				{
					tmpStr = tmpArray[i]->GetStringPtr()->GetString();
					for ( HE_DWORD index = 0; index < tmpStr.GetLength(); ++index )
					{
						item.charCode = HE_BYTE( tmpStr[index] );
						item.gid = 0;
						item.cid = 0;
						item.ucs = 0;
						if ( pFont->GetEncodingType() == FONT_ENCODING_NONE )
						{
							//对于简单字体而言，没有编码信息的时候
							pFont->GetGlyphId( item.charCode, item.gid );
							pFont->GetUnicode( item.charCode, item.ucs );
						}
						else if ( pFont->GetEncodingType() == FONT_ENCODING_BUILDINCMAP )
						{
							//error
						}else if ( pFont->GetEncodingType() == FONT_ENCODING_IDENTITY )
						{
							item.gid = item.charCode;
							pFont->GetUnicode( item.charCode, item.ucs );
						}else{
							pFont->GetGlyphId( item.charCode, item.gid );
							pFont->GetUnicode( item.charCode, item.ucs );
						}
						item.kerning = kerning;
						item.width = pFont->GetWidth( item.gid );
						item.height = 1; //font height??
						mItems.push_back( item );
						kerning = 0;
					}
				}
				else if ( tmpArray[i]->GetType() == OBJ_TYPE_NUMBER )
				{
					kerning = tmpArray[i]->GetNumberPtr()->GetInteger();
				}
			}
		}
		else
		{
			CHE_PDF_Type0_Font * pType0Font = (CHE_PDF_Type0_Font *)( pFont );
			for ( HE_DWORD i = 0; i < tmpArray.size(); ++i )
			{
				if ( tmpArray[i]->GetType() == OBJ_TYPE_STRING )
				{
					tmpStr = tmpArray[i]->GetStringPtr()->GetString();
					for ( HE_DWORD index = 0; index < tmpStr.GetLength(); index+=2 )
					{
						item.charCode = HE_BYTE( tmpStr[index] );
						item.charCode = ( item.charCode << 8 ) + HE_BYTE( tmpStr[index+1] );
						item.gid = 0;
						item.cid = 0;
						item.ucs = 0;
						if ( pFont->GetEncodingType() == FONT_ENCODING_NONE )
						{
							item.gid = item.charCode;
							pFont->GetUnicode( item.charCode, item.ucs );
						}
						else if ( pFont->GetEncodingType() == FONT_ENCODING_BUILDINCMAP )
						{
							if ( pType0Font->GetCID( item.charCode, item.cid ) )
							{
								if ( ! pFont->GetUnicode( item.cid, item.ucs ) )
								{
									item.ucs = item.cid;
									pFont->GetGlyphId( item.ucs, item.gid );
								}
							}
						}else if ( pFont->GetEncodingType() == FONT_ENCODING_IDENTITY )
						{
							item.gid = item.charCode;
							pFont->GetUnicode( item.charCode, item.ucs );
						}else{
							pFont->GetGlyphId( item.charCode, item.gid );
							pFont->GetUnicode( item.charCode, item.ucs );
						}
						item.kerning = kerning;
						item.width = pFont->GetWidth( item.gid );
						item.height = 0; //font height??
						mItems.push_back( item );
						kerning = 0;
					}
				}
				else if ( tmpArray[i]->GetType() == OBJ_TYPE_NUMBER )
				{
					kerning = tmpArray[i]->GetNumberPtr()->GetInteger();
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