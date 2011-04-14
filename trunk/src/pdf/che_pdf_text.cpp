#include "../../include/pdf/che_pdf_text.h"
#include "../../include/pdf/che_pdf_resource.h"
#include "../../include/che_datastructure.h"

HE_DWORD CHE_PDF_TextExtractor::Extract( CHE_PDF_Page * page, HE_WCHAR* buffer, HE_DWORD size )
{
	if ( page == NULL || buffer == NULL || size == 0 )
	{
		return 0;
	}
	CHE_DynBuffer contentBuf;
	if ( !page->GetPageContent( contentBuf ) )
	{
		return 0;
	}
	if ( contentBuf.GetByteCount() < 4 )
	{
		return 0;
	}

	CHE_PDF_FontCharCodeMgr * pCurFontCharCodeMgr = NULL;
	CHE_PDF_Dictionary * pFontDict = page->GetPageDictionary();
	if ( pFontDict == NULL )
	{
		return 0;
	}
	pFontDict = page->GetPageResources();
	if ( pFontDict == NULL || pFontDict->GetType() != PDFOBJ_DICTIONARY )
	{
		return 0;
	}else if ( pFontDict->GetType() == PDFOBJ_REFERENCE )
	{
		CHE_PDF_IndirectObject * pInObj = page->GetDocument()->GetParser()->GetIndirectObject( ((CHE_PDF_Reference*)pFontDict)->GetRefNuml() );
		pFontDict = pInObj->GetDict();
	} 

	pFontDict = (CHE_PDF_Dictionary*)pFontDict->GetElement( CHE_ByteString("Font") );
	if ( pFontDict == NULL )
	{
		return 0;
	}else if ( pFontDict->GetType() == PDFOBJ_REFERENCE )
	{
		CHE_PDF_IndirectObject * pInObj = page->GetDocument()->GetParser()->GetIndirectObject( ((CHE_PDF_Reference*)pFontDict)->GetRefNuml() );
		pFontDict = pInObj->GetDict();
		if ( pFontDict == NULL )
		{
			return 0;
		}
	} 
	IHE_Read* pContentRead = HE_CreateMemBufRead( contentBuf.GetData(), contentBuf.GetByteCount() );
	if (  pContentRead == NULL )
	{
		return 0;
	}
	CHE_PDF_SyntaxParser sParser;
	if ( sParser.InitParser( pContentRead ) == FALSE )
	{
		pContentRead->Release();
		return 0;
	}

	HE_DWORD lCharCount = 0;
	CHE_PtrStack OpdStack;
	PDFPARSER_WORD_DES wordDes;
	CHE_PDF_Object * pTmpNode = NULL;
	while( sParser.GetWord( wordDes ) == TRUE )
	{
		if ( wordDes.str == "BI" )
		{
			sParser.SeekToMark( CHE_ByteString("EI") );
			continue;
		}
		if ( wordDes.str != "BT" )
			 continue;
		HE_BOOL	bOpd = TRUE;
		while ( sParser.GetWord( wordDes ) == TRUE )
		{
			bOpd = TRUE;
			if ( wordDes.str == "ET" )
			{
				break;
			}
			switch ( wordDes.type )
			{
			case PDFPARSER_WORD_INTEGER:
				pTmpNode = CHE_PDF_Number::Create( HE_PDF_StringToInteger(wordDes.str) );
				OpdStack.Push( pTmpNode );
				break;
			case PDFPARSER_WORD_FLOAT:
				pTmpNode = CHE_PDF_Number::Create( HE_PDF_StringToFloat(wordDes.str) );
				OpdStack.Push( pTmpNode );
				break;
			case PDFPARSER_WORD_NAME:
				pTmpNode = CHE_PDF_Name::Create( wordDes.str );
				OpdStack.Push( pTmpNode );
				break;
			case PDFPARSER_WORD_ARRAY_B:
				sParser.SetPos( wordDes.offset );
				pTmpNode = sParser.GetArray();
				OpdStack.Push( pTmpNode );
				break;
			case PDFPARSER_WORD_DICT_B:
				sParser.SetPos( wordDes.offset );
				pTmpNode = sParser.GetDictionary();
				OpdStack.Push( pTmpNode );
				break;
			case PDFPARSER_WORD_STRING:
				pTmpNode = CHE_PDF_String::Create( wordDes.str );
				OpdStack.Push( pTmpNode );
				break;
			case PDFPARSER_WORD_UNKNOWN:
				{
					if ( wordDes.str == "null" )
					{
						pTmpNode = CHE_PDF_Null::Create();
						OpdStack.Push( pTmpNode );
						break;
					}else if ( wordDes.str == "false" )
					{
						pTmpNode = CHE_PDF_Boolean::Create( FALSE );
						OpdStack.Push( pTmpNode );
						break;
					}else if ( wordDes.str == "true" )
					{
						pTmpNode = CHE_PDF_Boolean::Create( TRUE );
						OpdStack.Push( pTmpNode );
						break;
					}
				}
			default:
				bOpd = FALSE;
				break;
			}
			if ( bOpd == TRUE )
			{
				continue;
			}

			//处理指令
			if ( wordDes.str == "Tf" )
			{
				OpdStack.Pop( (HE_LPVOID*)&pTmpNode );
				pTmpNode->Release();
				OpdStack.Pop( (HE_LPVOID*)&pTmpNode );

				CHE_PDF_Reference * pFontRef =  (CHE_PDF_Reference *)pFontDict->GetElement( ((CHE_PDF_Name*)pTmpNode)->GetString() );
				if ( pFontRef == NULL || pFontRef->GetType() != PDFOBJ_REFERENCE )
				{
					pCurFontCharCodeMgr = NULL;
				}else{
					HE_DWORD x = pFontRef->GetRefNuml();
					pCurFontCharCodeMgr = page->GetDocument()->GetFontCodeMgr( pFontRef->GetRefNuml() );
				}
				if ( pTmpNode )
				{
					pTmpNode->Release();
				}
				continue;
			}else if ( wordDes.str == "Tj" || wordDes.str == "'" || wordDes.str == "\"" ) 
			{
				OpdStack.Pop( (HE_LPVOID*)&pTmpNode );
				CHE_ByteString str = ((CHE_PDF_String*)pTmpNode)->GetString();
				if ( pCurFontCharCodeMgr == NULL )
				{
					continue;
				}
				if ( pCurFontCharCodeMgr->GetFontType() == PDFFONT_TYPE0 )
				{
					HE_WCHAR wch, wch1, wch2;
					for ( HE_DWORD i = 0; i < str.GetLength(); i+=2 )
					{
						wch1 = str[i] & 0x00FF;
						wch2 = str[i+1] & 0x00FF;
						wch1 = wch1 << 8;
						wch = wch1 + wch2;
						buffer[lCharCount] = pCurFontCharCodeMgr->GetUnicode( wch );
						lCharCount++;
						if ( lCharCount == size )
						{
							if ( pTmpNode )
							{
								pTmpNode->Release();
							}
							//清除数据
							CHE_PDF_Object * pTmpObj = NULL;
							while ( OpdStack.Pop( (HE_LPVOID*)&pTmpObj ) == TRUE )
							{
								if ( pTmpObj != NULL )
								{
									pTmpObj->Release();
								}
							}
							pContentRead->Release();
							delete pContentRead; 
							return lCharCount;
						}
					}
				}else{
					for ( HE_DWORD i = 0; i < str.GetLength(); i++ )
					{
						buffer[lCharCount] = pCurFontCharCodeMgr->GetUnicode( str[i] );
						lCharCount++;
						if ( lCharCount == size )
						{
							if ( pTmpNode )
							{
								pTmpNode->Release();
							}
							//清除数据
							CHE_PDF_Object * pTmpObj = NULL;
							while ( OpdStack.Pop( (HE_LPVOID*)&pTmpObj ) == TRUE )
							{
								if ( pTmpObj != NULL )
								{
									pTmpObj->Release();
								}
							}
							pContentRead->Release();
							delete pContentRead; 
							return lCharCount;
						}
					}
				}
				if ( pTmpNode )
				{
					pTmpNode->Release();
				}
			}else if ( wordDes.str == "TJ" )
			{
				OpdStack.Pop( (HE_LPVOID*)&pTmpNode );
				if ( pCurFontCharCodeMgr == NULL )
				{
					continue;
				}
				CHE_PDF_Array * pArray = (CHE_PDF_Array*)pTmpNode;
				for ( HE_DWORD i = 0; i < pArray->GetCount(); i++ )
				{
					pTmpNode = pArray->GetElement( i );
					if ( pTmpNode->GetType() != PDFOBJ_STRING )
					{
						continue;
					}
					CHE_ByteString str = ((CHE_PDF_String*)pTmpNode)->GetString();
					if ( pCurFontCharCodeMgr->GetFontType() == PDFFONT_TYPE0 )
					{
						HE_WCHAR wch, wch1, wch2;
						for ( HE_DWORD i = 0; i < str.GetLength(); i+= 2 )
						{
							wch1 = str[i] & 0x00FF;
							wch2 = str[i+1] & 0x00FF;
							wch1 = wch1 << 8;
							wch = wch1 + wch2;
							buffer[lCharCount] = pCurFontCharCodeMgr->GetUnicode( wch );
							lCharCount++;
							if ( lCharCount == size )
							{
								//清除数据
								if ( pArray )
								{
									pArray->Release();
								}
								CHE_PDF_Object * pTmpObj = NULL;
								while ( OpdStack.Pop( (HE_LPVOID*)&pTmpObj ) == TRUE )
								{
									if ( pTmpObj != NULL )
									{
										pTmpObj->Release();
									}
								}
								pContentRead->Release();
								delete pContentRead; 
								return lCharCount;
							}
						}
					}else{
						for ( HE_DWORD i = 0; i < str.GetLength(); i++ )
						{
							buffer[lCharCount] = pCurFontCharCodeMgr->GetUnicode( str[i] );
							lCharCount++;
							if ( lCharCount == size )
							{
								//清除数据
								if ( pArray )
								{
									pArray->Release();
								}
								CHE_PDF_Object * pTmpObj = NULL;
								while ( OpdStack.Pop( (HE_LPVOID*)&pTmpObj ) == TRUE )
								{
									if ( pTmpObj != NULL )
									{
										pTmpObj->Release();
									}
								}
								pContentRead->Release();
								delete pContentRead; 
								return lCharCount;
							}
						}
					}
				}
				if ( pArray )
				{
					pArray->Release();
				}
			}
			//清除无用的操作数
			CHE_PDF_Object * pTmpObj = NULL;
			while ( OpdStack.Pop( (HE_LPVOID*)&pTmpObj ) == TRUE )
			{
				if ( pTmpObj != NULL )
				{
					pTmpObj->Release();
				}
			}
		}
	}

	pContentRead->Release();
	delete pContentRead; 

	CHE_PDF_Object * pTmpObj = NULL;
	while ( OpdStack.Pop( (HE_LPVOID*)&pTmpObj ) == TRUE )
	{
		if ( pTmpObj != NULL )
		{
			pTmpObj->Release();
		}
	}
	return lCharCount;
}