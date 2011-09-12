#include "../../include/pdf/che_pdf_pages.h"
#include <memory>

class IHE_DefaultGetPDFFont : public IHE_PDF_GetFont
{
public:
	IHE_DefaultGetPDFFont( CHE_PDF_Document * pDocument, CHE_Allocator * pAllocator ) : IHE_PDF_GetFont( pAllocator )
		{ m_pDoc = pDocument; }
	~IHE_DefaultGetPDFFont() {};
	
	CHE_PDF_Font * GetFont( HE_DWORD objNum ) { return (m_pDoc!= NULL)?m_pDoc->GetFont(objNum):NULL; }
	
private:
	CHE_PDF_Document * m_pDoc;
};

CHE_PDF_Document::CHE_PDF_Document( CHE_Allocator * pAllocator ) 
: CHE_Object( pAllocator ), m_ID1( pAllocator ), m_ID2( pAllocator ), m_FontMap( pAllocator )
{
	m_pInfoDict = NULL;
	m_pRootDict = NULL;
	m_pParser = NULL;
	m_pIHE_GetPDFFont = GetAllocator()->New<IHE_DefaultGetPDFFont>( this, GetAllocator() );
}

CHE_PDF_Document::CHE_PDF_Document( IHE_Read * pFileRead, CHE_Allocator * pAllocator )
: CHE_Object( pAllocator ), m_ID1( pAllocator ), m_ID2( pAllocator ), m_FontMap( pAllocator )
{
	if ( pFileRead == NULL )
	{
		m_ID1 = "";
		m_ID2 = "";
		m_pInfoDict = NULL;
		m_pRootDict = NULL;
		m_pParser = NULL;
		m_pIHE_GetPDFFont = GetAllocator()->New<IHE_DefaultGetPDFFont>( this, GetAllocator() );
	}else{
		m_pParser = GetAllocator()->New<CHE_PDF_Parser>( GetAllocator() );
		m_pParser->Open( pFileRead );
		m_pRootDict = m_pParser->GetRootDict();
		m_pInfoDict = m_pParser->GetInfoDict();
		HE_DWORD pageCount = m_pParser->GetPageCount();
		CHE_PDF_Object * pIDArray = m_pParser->GetIDArray();
		if ( pIDArray && pIDArray->GetType() != OBJ_TYPE_ARRAY )
		{
			CHE_PDF_String * pStrObj = (CHE_PDF_String*)((CHE_PDF_Array*)pIDArray)->GetElement( 0 );
			m_ID1 = pStrObj->GetString();
			pStrObj = (CHE_PDF_String*)((CHE_PDF_Array*)pIDArray)->GetElement( 1 );
			m_ID2 = pStrObj->GetString();
		}
		m_pIHE_GetPDFFont = GetAllocator()->New<IHE_DefaultGetPDFFont>( this, GetAllocator() );
	}
}

CHE_PDF_Document::~CHE_PDF_Document()
{
	if ( m_pIHE_GetPDFFont )
	{
		GetAllocator()->Delete<IHE_PDF_GetFont>( m_pIHE_GetPDFFont );
	}
	if ( m_pParser )
	{
		m_pParser->Close();
		GetAllocator()->Delete<CHE_PDF_Parser>( m_pParser );
	}
	if ( m_FontMap.GetCount() > 0 )
	{
		CHE_PDF_Font * pTmpFont = NULL;
		for ( HE_DWORD i = 0; i < m_FontMap.GetCount(); i++ )
		{
			pTmpFont = (CHE_PDF_Font*)m_FontMap.GetItemByIndex( i );
			if ( pTmpFont )
			{
				GetAllocator()->Delete<CHE_PDF_Font>( pTmpFont );
			}
		}
		m_FontMap.Clear();
	}
}

HE_BOOL CHE_PDF_Document::Load( IHE_Read * pFileRead )
{
	if ( pFileRead == NULL )
	{
		return FALSE;
	}
	m_pParser = GetAllocator()->New<CHE_PDF_Parser>( GetAllocator() );
	m_pParser->Open( pFileRead );
	m_pRootDict = m_pParser->GetRootDict();
	m_pInfoDict = m_pParser->GetInfoDict();
	HE_DWORD pageCount = m_pParser->GetPageCount();
	CHE_PDF_Object * pIDArray = m_pParser->GetIDArray();
	if ( pIDArray && pIDArray->GetType() == OBJ_TYPE_ARRAY )
	{
		CHE_PDF_String * pStrObj = (CHE_PDF_String*)((CHE_PDF_Array*)pIDArray)->GetElement( 0 );
		m_ID1 = pStrObj->GetString();
		pStrObj = (CHE_PDF_String*)((CHE_PDF_Array*)pIDArray)->GetElement( 1 );
		m_ID2 = pStrObj->GetString();
	}
	return TRUE;
}

HE_VOID CHE_PDF_Document::Unload()
{
	m_pParser->Close();
	GetAllocator()->Delete<CHE_PDF_Parser>( m_pParser );
	m_pParser = NULL;
	if ( m_pIHE_GetPDFFont )
	{
		GetAllocator()->Delete<IHE_PDF_GetFont>( m_pIHE_GetPDFFont );
		m_pIHE_GetPDFFont = NULL;
	}
	if ( m_FontMap.GetCount() > 0 )
	{
		CHE_PDF_Font * pTmpFont = NULL;
		for ( HE_DWORD i = 0; i < m_FontMap.GetCount(); i++ )
		{
			pTmpFont = (CHE_PDF_Font*)m_FontMap.GetItemByIndex( i );
			if ( pTmpFont )
			{
				GetAllocator()->Delete<CHE_PDF_Font>( pTmpFont );
			}
		}
		m_FontMap.Clear();
	}
}

HE_DWORD CHE_PDF_Document::GetPageCount() const
{
	if ( m_pParser )
	{
		return m_pParser->GetPageCount();
	}
	return 0;
}
 
CHE_PDF_Page* CHE_PDF_Document::GetPage( HE_DWORD iPageIndex )
{
	if ( m_pParser == NULL )
	{
		return NULL;
	}
	if ( iPageIndex >= GetPageCount() )
	{
		return NULL;
	}
	HE_DWORD objNum = m_pParser->GetPageObjNum( iPageIndex );
	if ( objNum == 0 )
	{
		return NULL;
	}
	CHE_PDF_Object * pPageObj =  m_pParser->GetObject( objNum );
	if ( pPageObj == NULL || pPageObj->GetType() != OBJ_TYPE_DICTIONARY )
	{
		return NULL;
	}
	return GetAllocator()->New<CHE_PDF_Page>( iPageIndex, pPageObj->ToDict(), this, GetAllocator() );
}

HE_VOID	CHE_PDF_Document::ClosePage( CHE_PDF_Page * pPage )
{
	if ( pPage )
	{
		GetAllocator()->Delete<CHE_PDF_Page>( pPage );
	}
}

HE_BOOL CHE_PDF_Document::IsEncrypted()
{
	if ( m_pParser == NULL )
	{
		return FALSE;
	}
	if ( m_pParser->GetTrailerDict() == NULL )
	{
		return FALSE;
	}
	CHE_PDF_Object * pObj = m_pParser->GetTrailerDict()->GetElement( "Encrypt" );
	if ( pObj == NULL )
	{
		return FALSE;
	}else{
		return TRUE;
	}
}
 
CHE_PDF_Array * CHE_PDF_Document::GetPageMediaBox( CHE_PDF_Dictionary * pPageDict )
{
	if ( pPageDict == NULL )
	{
		return NULL;
	}
	CHE_PDF_Object * pObj = pPageDict->GetElement( "MediaBox" );
	if ( pObj == NULL )
	{
		CHE_PDF_Dictionary * pCurDict = pPageDict;
		while ( pCurDict )
		{
			pObj = pCurDict->GetElement( "Parent" );
			if ( pObj == NULL )
			{
				return NULL;
			}
			if ( pObj->GetType() != OBJ_TYPE_REFERENCE )
			{
				return NULL;
			}else{
				pObj = m_pParser->GetObject( ((CHE_PDF_Reference*)pObj)->GetRefNum() );
				if ( pObj == NULL || pObj->GetType() != OBJ_TYPE_DICTIONARY )
				{
					return NULL;
				}
				pCurDict = pObj->ToDict();
				pObj = GetPageMediaBox( pCurDict );
				if ( pObj != NULL )
				{
					return pObj->ToArray();
				}
			}
		}
	}
	if ( pObj->GetType() == OBJ_TYPE_ARRAY )
	{
		return pObj->ToArray();
	}else if ( pObj->GetType() == OBJ_TYPE_REFERENCE )
	{
		pObj = m_pParser->GetObject( pObj->ToReference()->GetRefNum() );
		if ( pObj != NULL && pObj->GetType() == OBJ_TYPE_ARRAY )
		{
			return pObj->ToArray();
		}else{
			return NULL;
		}
	}else{
		return NULL;
	}
}

CHE_PDF_Dictionary * CHE_PDF_Document::GetPageResources( CHE_PDF_Dictionary * pPageDict )
{
	if ( pPageDict == NULL )
	{
		return NULL;
	}
	CHE_PDF_Object * pObj = pPageDict->GetElement( "Resources" );
	if ( pObj == NULL )
	{
		CHE_PDF_Dictionary * pCurDict = pPageDict;
		while ( pCurDict )
		{
			pObj = pCurDict->GetElement( "Parent" );
			if ( pObj == NULL )
			{
				return NULL;
			}
			if ( pObj->GetType() != OBJ_TYPE_REFERENCE )
			{
				return NULL;
			}else{
				pObj = pObj->ToReference()->GetRefObj( OBJ_TYPE_DICTIONARY );
				if ( pObj == NULL )
				{
					return NULL;
				}
				pCurDict = pObj->ToDict();
				pObj = GetPageResources( pCurDict );
				if ( pObj != NULL )
				{
					return pObj->ToDict();
				}				
			}
		}
	}else if ( pObj->GetType() == OBJ_TYPE_DICTIONARY )
	{
		return pObj->ToDict();
	}else if ( pObj->GetType() == OBJ_TYPE_REFERENCE )
	{
		pObj = pObj->ToReference()->GetRefObj( OBJ_TYPE_DICTIONARY );
		if ( pObj == NULL )
		{
			return NULL;
		}
		return pObj->ToDict();
	}else{
		return NULL;
	}
}

CHE_PDF_Font *	CHE_PDF_Document::GetFont( HE_DWORD objNum )
{
	CHE_PDF_Font * pFont = (CHE_PDF_Font *)m_FontMap.GetItem( objNum );
	if ( pFont == NULL )
	{
		CHE_PDF_Object * pObj = m_pParser->GetObject( objNum );
		if ( pObj == NULL )
		{
			return NULL;
		}
		CHE_PDF_Dictionary * pFontDict = NULL;
		if ( pObj->GetType() == OBJ_TYPE_REFERENCE )
		{
			pObj = pObj->ToReference()->GetRefObj( OBJ_TYPE_DICTIONARY );
			if ( pObj->GetType() == OBJ_TYPE_DICTIONARY )
			{
				pFontDict = pObj->ToDict();
			}
		}else if ( pObj->GetType() == OBJ_TYPE_DICTIONARY )
		{
			pFontDict = pObj->ToDict();
		}else if ( pObj->GetType() == OBJ_TYPE_ARRAY )
		{
			pFontDict = pObj->ToArray()->GetElement( 0, OBJ_TYPE_DICTIONARY )->ToDict();
		}
		if ( pFontDict == NULL )
		{
			return NULL;
		}
		pFont = GetAllocator()->New<CHE_PDF_Font>( pFontDict, GetAllocator() );
		m_FontMap.Append( objNum, pFont );
		return pFont;
	}
	return pFont;
}

CHE_PDF_Page::CHE_PDF_Page( HE_DWORD lPageIndex, CHE_PDF_Dictionary * pDict, CHE_PDF_Document * pDoc, CHE_Allocator * pAllocator )
: CHE_Object( pAllocator ), m_arrContentObj( pAllocator )
{
	m_lPageIndex = lPageIndex;
	m_pPageObj = NULL;
	m_fPageHeight = 0.0;
	m_fPageWidth = 0.0;
	m_pDoc = pDoc;
	if ( pDict )
	{
		m_pPageObj = pDict;
		CHE_PDF_Object * pObj = NULL;
		if ( m_pDoc )
		{
			pObj = m_pDoc->GetPageMediaBox( m_pPageObj );
		}
		if ( pObj )
		{
			if ( pObj->GetType() == OBJ_TYPE_ARRAY )
			{
				CHE_PDF_Number * pNum1 = (CHE_PDF_Number*)((CHE_PDF_Array*)pObj)->GetElement( 0 );
				CHE_PDF_Number * pNum2 = (CHE_PDF_Number*)((CHE_PDF_Array*)pObj)->GetElement( 1 );
				CHE_PDF_Number * pNum3 = (CHE_PDF_Number*)((CHE_PDF_Array*)pObj)->GetElement( 2 );
				CHE_PDF_Number * pNum4 = (CHE_PDF_Number*)((CHE_PDF_Array*)pObj)->GetElement( 3 );
				m_fPageWidth = pNum3->GetFloat() - pNum1->GetFloat();
				m_fPageHeight = pNum4->GetFloat() - pNum2->GetFloat();
			}
		}
	}
}

CHE_PDF_Page::~CHE_PDF_Page()
{
	CHE_PDF_ContentObject * pContentObj = NULL;
	for ( HE_DWORD i = 0; i < m_arrContentObj.GetCount(); i++ )
	{
		pContentObj = (CHE_PDF_ContentObject *)m_arrContentObj.GetItem( i );
		if ( pContentObj )
		{
			pContentObj->Release();
		}
	}
	m_arrContentObj.Clear();
}

HE_BOOL CHE_PDF_Page::GetPageContent( CHE_DynBuffer & buffer )
{
	CHE_PDF_Dictionary * pPageDict = m_pPageObj;
	if ( pPageDict == NULL )
	{
		return FALSE;
	}
	CHE_PDF_Object * pPageContent = pPageDict->GetElement( "Contents" );
	if ( pPageContent == NULL )
	{
		return FALSE;
	}
	if ( m_pDoc == NULL )
	{
		return FALSE;
	}
	if ( m_pDoc->GetParser() == NULL )
	{
		return FALSE;
	}
	if ( pPageContent->GetType() == OBJ_TYPE_REFERENCE )
	{
		HE_DWORD objNum = pPageContent->ToReference()->GetRefNum();
		pPageContent = m_pDoc->GetParser()->GetObject( objNum );
		if ( pPageContent == NULL )
		{
			return FALSE;
		}
		if ( pPageContent->GetType() == OBJ_TYPE_STREAM )
		{
			CHE_PDF_StreamAcc stmAcc( GetAllocator() );
			stmAcc.Attach( pPageContent->ToStream() );
			buffer.Write( stmAcc.GetData(), stmAcc.GetSize() );
			stmAcc.Detach();
		}else if ( pPageContent->GetType() == OBJ_TYPE_ARRAY )
		{
			CHE_PDF_Object * pTmpObj = NULL;
			for ( HE_DWORD i = 0; i < pPageContent->ToArray()->GetCount(); i++ )
			{
				pTmpObj = ((CHE_PDF_Array*)pPageContent)->GetElement( i );
				if ( pTmpObj->GetType() == OBJ_TYPE_STREAM )
				{
					CHE_PDF_StreamAcc stmAcc( GetAllocator() );
					stmAcc.Attach( pTmpObj->ToStream() );
					buffer.Write( stmAcc.GetData(), stmAcc.GetSize() );
					stmAcc.Detach();
					
				}else if ( pTmpObj->GetType() == OBJ_TYPE_REFERENCE )
				{
					HE_DWORD objNum = pTmpObj->ToReference()->GetRefNum();
					pTmpObj = m_pDoc->GetParser()->GetObject( objNum );
					if ( pTmpObj == NULL )
					{
						continue;
					}
					if ( pTmpObj->GetType() == OBJ_TYPE_STREAM )
					{
						CHE_PDF_StreamAcc stmAcc( GetAllocator() );
						stmAcc.Attach( pTmpObj->ToStream() );
						buffer.Write( stmAcc.GetData(), stmAcc.GetSize() );
						stmAcc.Detach();
					}
				}
			}
		}
		return TRUE;
	}else if ( pPageContent->GetType() == OBJ_TYPE_ARRAY )
	{
		HE_DWORD objCount = pPageContent->ToArray()->GetCount();
		for ( HE_DWORD i = 0; i < objCount; i++ )
		{
			CHE_PDF_Object * pTmpObj = pPageContent->ToArray()->GetElement( i );
			if ( pTmpObj == NULL )
			{
				continue;
			}
			if ( pTmpObj->GetType() != OBJ_TYPE_REFERENCE )
			{
				continue;
			}
			pTmpObj = pTmpObj->ToReference()->GetRefObj( OBJ_TYPE_STREAM );
			if ( pTmpObj == NULL )
			{
				continue;
			}
			if ( pTmpObj->GetType() == OBJ_TYPE_STREAM )
			{
				CHE_PDF_StreamAcc stmAcc( GetAllocator() );
				stmAcc.Attach( pTmpObj->ToStream() );
				buffer.Write( stmAcc.GetData(), stmAcc.GetSize() );
				stmAcc.Detach();
			}else if ( pTmpObj->GetType() == OBJ_TYPE_ARRAY )
			{
				CHE_PDF_Object * pObj = NULL;
				for ( HE_DWORD i = 0; i < pTmpObj->ToArray()->GetCount(); i++ )
				{
					pObj = pTmpObj->ToArray()->GetElement( i );
					if ( pObj->GetType() == OBJ_TYPE_STREAM )
					{
						CHE_PDF_StreamAcc stmAcc( GetAllocator() );
						stmAcc.Attach( pObj->ToStream() );
						buffer.Write( stmAcc.GetData(), stmAcc.GetSize() );
						stmAcc.Detach();
						
					}else if ( pObj->GetType() == OBJ_TYPE_REFERENCE )
					{
						pObj = pObj->ToReference()->GetRefObj( OBJ_TYPE_STREAM );
						if ( pObj == NULL )
						{
							continue;
						}
						if ( pObj->GetType() == OBJ_TYPE_STREAM )
						{
							CHE_PDF_StreamAcc stmAcc( GetAllocator() );
							stmAcc.Attach( pObj->ToStream() );
							buffer.Write( stmAcc.GetData(), stmAcc.GetSize() );
							stmAcc.Detach();
						}
					}
				}
			}
		}
		return TRUE;
	}
	return FALSE;
}

HE_DWORD CHE_PDF_Page::ParseContent()
{
	if ( m_arrContentObj.GetCount() > 0 )
	{
		return m_arrContentObj.GetCount();
	}

	CHE_DynBuffer buf( 1024 * 4, 1024 *4, GetAllocator() );
	if ( GetPageContent( buf ) == FALSE )
	{
		return 0;
	}

	IHE_Read * pRead = HE_CreateMemBufRead( buf.GetData(), buf.GetSize(), GetAllocator() );
	if ( pRead == NULL )
	{
		return 0;
	}

	CHE_PDF_Parser * pDocParser = GetDocument()->GetParser();

	CHE_PDF_SyntaxParser sParser( pDocParser, GetAllocator() );
	if ( sParser.InitParser( pRead ) == FALSE )
	{
		pRead->Release();
		HE_DestoryIHERead( pRead );
		return 0;
	}

	CHE_PDF_Dictionary* pPageResourcesDict =  GetPageResources();
	CHE_PDF_Dictionary* pFontDict = NULL;
	CHE_PDF_Dictionary* pExtGStateDict = NULL;
	CHE_PDF_Object * pTmpObj = NULL;

	CHE_PtrStack OpdStack( GetAllocator() );
	CHE_PDF_ParseWordDes wordDes( GetAllocator() );
	CHE_PDF_Object * pTmpNode = NULL;
	HE_BOOL	bOpd = TRUE;

	CHE_PtrQueue SupPathQueue( GetAllocator() );
	HE_FLOAT fBeginX = 0, fBeginY = 0;
	HE_FLOAT fCurX = 0, fCurY = 0;
	HE_BOOL	bConnect = FALSE;
	HE_BOOL	bSubPathClosed = FALSE;

	HE_BOOL		bClipPath = FALSE;
	PDF_FILL_MODE	ClipFillMode = FILL_MODE_NOZERO;

	HE_FLOAT	fPosiX = 0;
	HE_FLOAT	fPosiY = 0;
	HE_FLOAT	fCharSpace = 0;
	HE_FLOAT	fWordSpace = 0;
	HE_DWORD	dwScale = 100;
	HE_FLOAT	fLeading = 0;
	HE_DWORD	dwSize = 0;
	HE_BYTE		byteRender = 0;
	HE_DWORD	dwRise = 0;
	HE_BOOL		bKnockout = FALSE;
	HE_FLOAT	fMatrixA = 1;
	HE_FLOAT	fMatrixB = 0;
	HE_FLOAT	fMatrixC = 0;
	HE_FLOAT	fMatrixD = 1;
	HE_FLOAT	fMatrixE = 0;
	HE_FLOAT	fMatrixF = 0;
	HE_DWORD	dwFontObjNum = 0;

	if ( pPageResourcesDict != NULL )
	{
		pFontDict = (CHE_PDF_Dictionary*)( pPageResourcesDict->GetElement( "Font", OBJ_TYPE_DICTIONARY ) );
		
		pExtGStateDict = (CHE_PDF_Dictionary*)( pPageResourcesDict->GetElement( "ExtGState", OBJ_TYPE_DICTIONARY ) );
	}

	//Init or Reset the Graph State
	CHE_PDF_OrderObject * pClipOrder = CHE_PDF_OrderObject::Create( GetAllocator() );
	m_arrContentObj.Append( pClipOrder );

	while( sParser.GetWord( wordDes ) == TRUE )
	{
		bOpd = TRUE;
		switch ( wordDes.type )
		{
		case PARSE_WORD_INTEGER:
			{
				pTmpNode = CHE_PDF_Number::Create( wordDes.str.GetInteger(), 0, 0, pDocParser, GetAllocator() );
				OpdStack.Push( pTmpNode );
				break;
			}
		case PARSE_WORD_FLOAT:
			{
				pTmpNode = CHE_PDF_Number::Create( wordDes.str.GetFloat(), 0, 0, pDocParser, GetAllocator() );
				OpdStack.Push( pTmpNode );
				break;
			}
		case PARSE_WORD_NAME:
			{
				pTmpNode = CHE_PDF_Name::Create( wordDes.str, 0, 0, pDocParser, GetAllocator() );
				OpdStack.Push( pTmpNode );
				break;
			}
		case PARSE_WORD_STRING:
			{
				pTmpNode = CHE_PDF_String::Create( wordDes.str, 0, 0, pDocParser, GetAllocator() );
				OpdStack.Push( pTmpNode );
				break;
			}
		case PARSE_WORD_ARRAY_B:
			{
				sParser.SetPos( wordDes.offset );
				pTmpNode = sParser.GetArray();
				OpdStack.Push( pTmpNode );
				break;
			}
		case PARSE_WORD_DICT_B:
			{
				sParser.SetPos( wordDes.offset );
				pTmpNode = sParser.GetDictionary();
				OpdStack.Push( pTmpNode );
				break;
			}
		default:
			{
				bOpd = FALSE;
				break;
			}
		}
		if ( bOpd == TRUE )
		{
			continue;
		}

		//text对象
		if ( wordDes.str == "BT" )
		{
			CHE_PDF_TextObject * pTmpText = CHE_PDF_TextObject::Create( GetAllocator() );
			CHE_PDF_ContentObject * pCurContentObj = pTmpText;

			fPosiX = 0;
			fPosiY = 0;
			fCharSpace = 0;
			fWordSpace = 0;
			dwScale = 100;
			fLeading = 0;
			dwSize = 0;
			byteRender = 0;
			dwRise = 0;
			bKnockout = FALSE;
			fMatrixA = 1;
			fMatrixB = 0;
			fMatrixC = 0;
			fMatrixD = 1;
			fMatrixE = 0;
			fMatrixF = 0;

			while ( sParser.GetWord( wordDes ) == TRUE )
			{
				bOpd = TRUE;
				if ( wordDes.str == "ET" )
				{
					m_arrContentObj.Append( (HE_LPVOID)pCurContentObj );
					break;
				}

				switch ( wordDes.type )
				{
				case PARSE_WORD_INTEGER:
					{
						pTmpNode = CHE_PDF_Number::Create( wordDes.str.GetInteger(), 0, 0, pDocParser, GetAllocator() );
						OpdStack.Push( pTmpNode );
						break;
					}
				case PARSE_WORD_FLOAT:
					{
						pTmpNode = CHE_PDF_Number::Create( wordDes.str.GetFloat(), 0, 0, pDocParser, GetAllocator() );
						OpdStack.Push( pTmpNode );
						break;
					}

				case PARSE_WORD_NAME:
					{
						pTmpNode = CHE_PDF_Name::Create( wordDes.str, 0, 0, pDocParser, GetAllocator() );
						OpdStack.Push( pTmpNode );
						break;
					}
				case PARSE_WORD_STRING:
					{
						pTmpNode = CHE_PDF_String::Create( wordDes.str, 0, 0, pDocParser, GetAllocator() );
						OpdStack.Push( pTmpNode );
						break;
					}
				case PARSE_WORD_ARRAY_B:
					{
						sParser.SetPos( wordDes.offset );
						pTmpNode = sParser.GetArray();
						OpdStack.Push( pTmpNode );
						break;			
					}
				case PARSE_WORD_DICT_B:
					{
						sParser.SetPos( wordDes.offset );
						pTmpNode = sParser.GetDictionary();
						OpdStack.Push( pTmpNode );
						break;
					}
				case PARSE_WORD_UNKNOWN:
					{
						if ( wordDes.str == "null" )
						{
							pTmpNode = CHE_PDF_Null::Create( 0, 0, pDocParser, GetAllocator() );
							OpdStack.Push( pTmpNode );
						}else if ( wordDes.str == "false" )
						{
							pTmpNode = CHE_PDF_Boolean::Create( FALSE, 0, 0, pDocParser, GetAllocator() );
							OpdStack.Push( pTmpNode );
						}else if ( wordDes.str == "true" )
						{
							pTmpNode = CHE_PDF_Boolean::Create( TRUE, 0, 0, pDocParser, GetAllocator() );
							OpdStack.Push( pTmpNode );
						}
						break;
					}
				default:
					{
						bOpd = FALSE;
						break;
					}
				}
				if ( bOpd == TRUE )
				{
					continue;
				}

				//处理指令
				if ( wordDes.str == "Tc" )	//char space //1 opd
				{
					if ( OpdStack.Pop( (HE_LPVOID*)&pTmpNode ) == TRUE )
					{
						if( pTmpNode != NULL && pTmpNode->GetType() == OBJ_TYPE_NUMBER )
						{
							fCharSpace = ((CHE_PDF_Number*)pTmpNode)->GetFloat();
							pTmpNode->Release();
						}
					}
				}else if ( wordDes.str == "Tw" )	//word space //1 opd
				{
					if ( OpdStack.Pop( (HE_LPVOID*)&pTmpNode ) == TRUE )
					{
						if( pTmpNode != NULL && pTmpNode->GetType() == OBJ_TYPE_NUMBER )
						{
							fWordSpace = ((CHE_PDF_Number*)pTmpNode)->GetFloat();
							pTmpNode->Release();
						}
					}
				}else if ( wordDes.str == "Tz" )	//word space //1 opd
				{
					if ( OpdStack.Pop( (HE_LPVOID*)&pTmpNode ) == TRUE )
					{
						if( pTmpNode != NULL && pTmpNode->GetType() == OBJ_TYPE_NUMBER )
						{
							dwScale = ((CHE_PDF_Number*)pTmpNode)->GetInteger();
							pTmpNode->Release();
						}
					}
				}else if ( wordDes.str == "TL" )	//word space //1 opd
				{
					if ( OpdStack.Pop( (HE_LPVOID*)&pTmpNode ) == TRUE )
					{
						if( pTmpNode != NULL && pTmpNode->GetType() == OBJ_TYPE_NUMBER )
						{
							fLeading = ((CHE_PDF_Number*)pTmpNode)->GetFloat();
							pTmpNode->Release();
						}
					}
				}else if ( wordDes.str == "Tf" )
				{
					if ( OpdStack.Pop( (HE_LPVOID*)&pTmpNode ) == TRUE )
					{
						if( pTmpNode != NULL && pTmpNode->GetType() == OBJ_TYPE_NUMBER )
						{
							dwSize = ((CHE_PDF_Number*)pTmpNode)->GetInteger();
							pTmpNode->Release();
						}
					}
					if ( OpdStack.Pop( (HE_LPVOID*)&pTmpNode ) == TRUE )
					{
						if ( pFontDict == NULL )
						{
							if ( pTmpNode != NULL )
							{
								pTmpNode->Release();
							}
						}else{
							if ( pTmpNode != NULL && pTmpNode->GetType() == OBJ_TYPE_NAME )
							{
								CHE_PDF_Reference * pFontRef =  (CHE_PDF_Reference *)( pFontDict->GetElement( ((CHE_PDF_Name*)pTmpNode)->GetString() ) );
								if ( pFontRef != NULL && pFontRef->GetType() == OBJ_TYPE_REFERENCE )
								{
									dwFontObjNum = pFontRef->GetRefNum();
								}
								pTmpNode->Release();
							}
						}
					}
				}else if ( wordDes.str == "Tr" )
				{
					if( OpdStack.Pop( (HE_LPVOID*)&pTmpNode ) == TRUE )
					{
						if( pTmpNode != NULL && pTmpNode->GetType() == OBJ_TYPE_NUMBER )
						{
							byteRender = ((CHE_PDF_Number*)pTmpNode)->GetInteger(); 
							pTmpNode->Release();
						}
					}
				}else if ( wordDes.str == "Ts" )
				{
					if ( OpdStack.Pop( (HE_LPVOID*)&pTmpNode ) == TRUE )
					{
						if( pTmpNode != NULL && pTmpNode->GetType() == OBJ_TYPE_NUMBER )
						{
							dwRise = ((CHE_PDF_Number*)pTmpNode)->GetInteger();
							pTmpNode->Release();
						}
					}
				}else if( wordDes.str == "Td" )
				{
					if ( OpdStack.Pop( (HE_LPVOID*)&pTmpNode ) == TRUE )
					{
						if( pTmpNode != NULL && pTmpNode->GetType() == OBJ_TYPE_NUMBER )
						{
							fPosiY += ((CHE_PDF_Number*)pTmpNode)->GetFloat();
							pTmpNode->Release();
						}
					}
					if ( OpdStack.Pop( (HE_LPVOID*)&pTmpNode ) == TRUE )
					{
						if( pTmpNode != NULL && pTmpNode->GetType() == OBJ_TYPE_NUMBER )
						{
							fPosiX += ((CHE_PDF_Number*)pTmpNode)->GetFloat();
							pTmpNode->Release();
						}
					}
				}else if( wordDes.str == "TD" )
				{
					if ( OpdStack.Pop( (HE_LPVOID*)&pTmpNode ) == TRUE )
					{
						if( pTmpNode != NULL && pTmpNode->GetType() == OBJ_TYPE_NUMBER )
						{
							HE_FLOAT tmpValue = ((CHE_PDF_Number*)pTmpNode)->GetFloat();
							fLeading = -tmpValue;
							fPosiY += tmpValue;
							pTmpNode->Release();
						}
					}
					if ( OpdStack.Pop( (HE_LPVOID*)&pTmpNode ) == TRUE )
					{
						if( pTmpNode != NULL && pTmpNode->GetType() == OBJ_TYPE_NUMBER )
						{
							fPosiX += ((CHE_PDF_Number*)pTmpNode)->GetFloat();
							pTmpNode->Release();
						}
					}
				}else if( wordDes.str == "Tm" )
				{
					if ( OpdStack.Pop( (HE_LPVOID*)&pTmpNode ) == TRUE )
					{
						if( pTmpNode != NULL && pTmpNode->GetType() == OBJ_TYPE_NUMBER )
						{
							fMatrixF = ((CHE_PDF_Number*)pTmpNode)->GetFloat();
							pTmpNode->Release();
						}
					}
					if ( OpdStack.Pop( (HE_LPVOID*)&pTmpNode ) == TRUE )
					{
						if( pTmpNode != NULL && pTmpNode->GetType() == OBJ_TYPE_NUMBER )
						{
							fMatrixE = ((CHE_PDF_Number*)pTmpNode)->GetFloat();
							pTmpNode->Release();
						}
					}
					if ( OpdStack.Pop( (HE_LPVOID*)&pTmpNode ) == TRUE )
					{
						if( pTmpNode != NULL && pTmpNode->GetType() == OBJ_TYPE_NUMBER )
						{
							fMatrixD = ((CHE_PDF_Number*)pTmpNode)->GetFloat();
							pTmpNode->Release();
						}
					}
					if ( OpdStack.Pop( (HE_LPVOID*)&pTmpNode ) == TRUE )
					{
						if( pTmpNode != NULL && pTmpNode->GetType() == OBJ_TYPE_NUMBER )
						{
							fMatrixC = ((CHE_PDF_Number*)pTmpNode)->GetFloat();
							pTmpNode->Release();
						}
					}
					if ( OpdStack.Pop( (HE_LPVOID*)&pTmpNode ) == TRUE )
					{
						if( pTmpNode != NULL && pTmpNode->GetType() == OBJ_TYPE_NUMBER )
						{
							fMatrixB = ((CHE_PDF_Number*)pTmpNode)->GetFloat();
							pTmpNode->Release();
						}
					}
					if ( OpdStack.Pop( (HE_LPVOID*)&pTmpNode ) == TRUE )
					{
						if( pTmpNode != NULL && pTmpNode->GetType() == OBJ_TYPE_NUMBER )
						{
							fMatrixA = ((CHE_PDF_Number*)pTmpNode)->GetFloat();
							pTmpNode->Release();
						}
					}
				}else if( wordDes.str == "T*" )
				{
					fPosiY -= fLeading;
				}else if ( wordDes.str == "Tj" || wordDes.str == "'" ) 
				{
					if ( OpdStack.Pop( (HE_LPVOID*)&pTmpNode ) == TRUE )
					{
						if ( pTmpNode != NULL )
						{
							CHE_PDF_TextObjectItem * pTextItem =  GetAllocator()->New<CHE_PDF_TextObjectItem>( GetAllocator() );
							pTextItem->SetMatrix( fMatrixA, fMatrixB, fMatrixC, fMatrixD, fMatrixE, fMatrixF );
							pTextItem->SetCharSpace( fCharSpace );
							pTextItem->SetWordSpace( fWordSpace );
							pTextItem->SetFontObj( dwFontObjNum );
							pTextItem->SetKnockout( bKnockout );
							pTextItem->SetLeading( fLeading );
							pTextItem->SetPosi( fPosiX, fPosiY );
							pTextItem->SetRender( byteRender );
							pTextItem->SetRise( dwRise );
							pTextItem->SetSize( dwSize );
							pTextItem->SetScale( dwScale );
							pTextItem->SetText( pTmpNode );
							CHE_ByteString str( GetAllocator() );
							str = ((CHE_PDF_String*)pTmpNode)->GetString();
							CHE_WideString wstr( GetAllocator() );
							CHE_PDF_Font * pFont = NULL;
							if ( GetDocument() && (pFont = GetDocument()->GetFont( dwFontObjNum )) )
							{
								if ( pFont->GetUnicodeString( str, wstr ) )
								{
									pTextItem->SetString( wstr );
								}
							}
							pTmpText->AppendItem( pTextItem );
							pTmpNode->Release();
						}
					}
				}else if ( wordDes.str == "\"" )
				{
					CHE_PDF_TextObjectItem * pTextItem =  GetAllocator()->New<CHE_PDF_TextObjectItem>( GetAllocator() );
					if ( OpdStack.Pop( (HE_LPVOID*)&pTmpNode ) == TRUE )
					{
						if ( pTmpNode != NULL )
						{
							pTextItem->SetMatrix( fMatrixA, fMatrixB, fMatrixC, fMatrixD, fMatrixE, fMatrixF );
							pTextItem->SetCharSpace( fCharSpace );
							pTextItem->SetWordSpace( fWordSpace );
							pTextItem->SetFontObj( dwFontObjNum );
							pTextItem->SetKnockout( bKnockout );
							pTextItem->SetLeading( fLeading );
							pTextItem->SetPosi( fPosiX, fPosiY );
							pTextItem->SetRender( byteRender );
							pTextItem->SetRise( dwRise );
							pTextItem->SetSize( dwSize );
							pTextItem->SetScale( dwScale );
							pTextItem->SetText( pTmpNode );
							CHE_ByteString str( GetAllocator() );
							str = ((CHE_PDF_String*)pTmpNode)->GetString();
							CHE_WideString wstr( GetAllocator() );
							CHE_PDF_Font * pFont = NULL;
							if ( GetDocument() && (pFont = GetDocument()->GetFont( dwFontObjNum )) )
							{
								if ( pFont->GetUnicodeString( str, wstr ) )
								{
									pTextItem->SetString( wstr );
								}
							}
							pTmpNode->Release();
						}
					}
					if ( OpdStack.Pop( (HE_LPVOID*)&pTmpNode ) == TRUE )
					{
						if( pTmpNode != NULL && pTmpNode->GetType() == OBJ_TYPE_NUMBER )
						{
							pTextItem->SetCharSpace( ((CHE_PDF_Number*)pTmpNode)->GetFloat() );
							pTmpNode->Release();
						}
					}
					if ( OpdStack.Pop( (HE_LPVOID*)&pTmpNode ) == TRUE )
					{
						if( pTmpNode != NULL && pTmpNode->GetType() == OBJ_TYPE_NUMBER )
						{
							pTextItem->SetWordSpace( ((CHE_PDF_Number*)pTmpNode)->GetFloat() );
							pTmpNode->Release();
						}
					}
					pTmpText->AppendItem( pTextItem );
				}else if ( wordDes.str == "TJ" )
				{
					if ( OpdStack.Pop( (HE_LPVOID*)&pTmpNode ) == TRUE )
					{
						if ( pTmpNode != NULL )
						{
							CHE_PDF_TextObjectItem * pTextItem =  GetAllocator()->New<CHE_PDF_TextObjectItem>( GetAllocator() );
							pTextItem->SetMatrix( fMatrixA, fMatrixB, fMatrixC, fMatrixD, fMatrixE, fMatrixF );
							pTextItem->SetCharSpace( fCharSpace );
							pTextItem->SetWordSpace( fWordSpace );
							pTextItem->SetFontObj( dwFontObjNum );
							pTextItem->SetKnockout( bKnockout );
							pTextItem->SetLeading( fLeading );
							pTextItem->SetPosi( fPosiX, fPosiY );
							pTextItem->SetRender( byteRender );
							pTextItem->SetRise( dwRise );
							pTextItem->SetSize( dwSize );
							pTextItem->SetScale( dwScale );
							pTextItem->SetText( pTmpNode );
							CHE_ByteString str( GetAllocator() );
							CHE_WideString wstr( GetAllocator() );
							CHE_WideString wstrTmp( GetAllocator() );
							CHE_PDF_Font * pFont = NULL;
							if ( GetDocument() && (pFont = GetDocument()->GetFont( dwFontObjNum )) )
							{
								CHE_PDF_Object * pTmpObj = NULL;
								for ( HE_DWORD i = 0; i < ((CHE_PDF_Array*)pTmpNode)->GetCount(); i++ )
								{
									pTmpObj = ((CHE_PDF_Array*)pTmpNode)->GetElement( i );
									if ( pTmpObj != NULL && pTmpObj->GetType() == OBJ_TYPE_STRING )
									{
										str = ((CHE_PDF_String*)pTmpObj)->GetString();
										if ( pFont->GetUnicodeString( str, wstr ) )
										{
											pTextItem->SetString( wstrTmp );
											wstr += wstrTmp;
										}
									}
								}
							}
							pTmpText->AppendItem( pTextItem );
							pTmpNode->Release();
						}
					}
				}else if ( wordDes.str == "w" )	//line width 
				{
					if( OpdStack.Pop( (HE_LPVOID*)&pTmpNode ) == TRUE )
					{
						if ( pTmpNode != NULL && pTmpNode->GetType() == OBJ_TYPE_NUMBER )
						{
							HE_FLOAT fTmp = ((CHE_PDF_Number*)pTmpNode)->GetFloat();
							CHE_PDF_OrderParam_LineWidth * pTmp = GetAllocator()->New<CHE_PDF_OrderParam_LineWidth>( fTmp, GetAllocator() );
							CHE_PDF_OrderObject * pOrder = CHE_PDF_OrderObject::Create( GetAllocator() );
							pOrder->SetType( ORDER_LINE_WIDTH );
							pOrder->SetParam( pTmp );
							m_arrContentObj.Append( pOrder );
							pTmpNode->Release();
						}
					}
				}else if ( wordDes.str == "J" )	//line cap
				{
					if( OpdStack.Pop( (HE_LPVOID*)&pTmpNode ) == TRUE )
					{
						if ( pTmpNode != NULL && pTmpNode->GetType() == OBJ_TYPE_NUMBER )
						{
							HE_BYTE dwByte = ((CHE_PDF_Number*)pTmpNode)->GetInteger();
							CHE_PDF_OrderParam_LineCap * pTmp = GetAllocator()->New<CHE_PDF_OrderParam_LineCap>( dwByte, GetAllocator() );
							CHE_PDF_OrderObject * pOrder = CHE_PDF_OrderObject::Create( GetAllocator() );
							pOrder->SetType( ORDER_LINE_CAP );
							pOrder->SetParam( pTmp );
							m_arrContentObj.Append( pOrder );
							pTmpNode->Release();
						}
					}
				}else if ( wordDes.str == "j" )	//line join
				{
					if( OpdStack.Pop( (HE_LPVOID*)&pTmpNode ) == TRUE )
					{
						if ( pTmpNode != NULL && pTmpNode->GetType() == OBJ_TYPE_NUMBER )
						{
							HE_BYTE dwByte = ((CHE_PDF_Number*)pTmpNode)->GetInteger();
							CHE_PDF_OrderParam_LineJoin * pTmp = GetAllocator()->New<CHE_PDF_OrderParam_LineJoin>( dwByte, GetAllocator() );
							CHE_PDF_OrderObject * pOrder = CHE_PDF_OrderObject::Create( GetAllocator() );
							pOrder->SetType( ORDER_LINE_JION );
							pOrder->SetParam( pTmp );
							m_arrContentObj.Append( pOrder );
							pTmpNode->Release();
						}
					}
				}else if ( wordDes.str == "M" )	//Miter limit
				{
					if( OpdStack.Pop( (HE_LPVOID*)&pTmpNode ) == TRUE )
					{
						if ( pTmpNode != NULL && pTmpNode->GetType() == OBJ_TYPE_NUMBER )
						{
							HE_FLOAT fTmp = ((CHE_PDF_Number*)pTmpNode)->GetFloat();
							CHE_PDF_OrderParam_MiterLimit * pTmp = GetAllocator()->New<CHE_PDF_OrderParam_MiterLimit>( fTmp, GetAllocator() );
							CHE_PDF_OrderObject * pOrder = CHE_PDF_OrderObject::Create( GetAllocator() );
							pOrder->SetType( ORDER_MITER_LIMIT );
							pOrder->SetParam( pTmp );
							m_arrContentObj.Append( pOrder );
							pTmpNode->Release();
						}
					}
				}else if ( wordDes.str == "d" )	//dash
				{
					HE_FLOAT dashPhase = 0;
					HE_FLOAT * dashArray = NULL;
					HE_DWORD dashArraySize = 0;
					if( OpdStack.Pop( (HE_LPVOID*)&pTmpNode ) == TRUE )
					{
						if ( pTmpNode != NULL && pTmpNode->GetType() == OBJ_TYPE_NUMBER )
						{
							dashPhase = ((CHE_PDF_Number*)pTmpNode)->GetFloat();
							pTmpNode->Release();
						}
					}
					if( OpdStack.Pop( (HE_LPVOID*)&pTmpNode ) == TRUE )
					{
						if ( pTmpNode != NULL && pTmpNode->GetType() == OBJ_TYPE_ARRAY )
						{
							dashArraySize = ((CHE_PDF_Array*)pTmpNode)->GetCount();
							if ( dashArraySize == 0 )
							{
								CHE_PDF_OrderParam_DashPattern * pTmp = GetAllocator()->New<CHE_PDF_OrderParam_DashPattern>( dashArray, 0, dashPhase, GetAllocator() );
								CHE_PDF_OrderObject * pOrder = CHE_PDF_OrderObject::Create( GetAllocator() );
								pOrder->SetType( ORDER_DASH_PATTERN );
								pOrder->SetParam( pTmp );
								m_arrContentObj.Append( pOrder );
							}else{
								dashArray = GetAllocator()->NewArray<HE_FLOAT>( dashArraySize );
								CHE_PDF_Object * pObj = NULL;
								for ( HE_DWORD i = 0; i < dashArraySize; i++ )
								{
									pObj = ((CHE_PDF_Array*)pTmpNode)->GetElement( i );
									if ( pObj != NULL && pObj->GetType() == OBJ_TYPE_NUMBER )
									{
										dashArray[i] = ((CHE_PDF_Number*)pObj)->GetFloat();
									}
								}
								CHE_PDF_OrderParam_DashPattern * pTmp = GetAllocator()->New<CHE_PDF_OrderParam_DashPattern>( dashArray, dashArraySize, dashPhase, GetAllocator() );
								CHE_PDF_OrderObject * pOrder = CHE_PDF_OrderObject::Create( GetAllocator() );
								pOrder->SetType( ORDER_DASH_PATTERN );
								pOrder->SetParam( pTmp );
								m_arrContentObj.Append( pOrder );
								GetAllocator()->DeleteArray<HE_FLOAT>( dashArray );
							}
							pTmpNode->Release();
						}
					}
				}else if ( wordDes.str == "ri" )
				{
					if( OpdStack.Pop( (HE_LPVOID*)&pTmpNode ) == TRUE )
					{
						if ( pTmpNode != NULL && pTmpNode->GetType() == OBJ_TYPE_NUMBER )
						{
							CHE_PDF_OrderParam_Intent * pTmp = GetAllocator()->New<CHE_PDF_OrderParam_Intent>( GetAllocator() );
							CHE_PDF_OrderObject * pOrder = CHE_PDF_OrderObject::Create( GetAllocator() );
							pOrder->SetType( ORDER_INTENT );
							pOrder->SetParam( pTmp );
							m_arrContentObj.Append( pOrder );
							pTmpNode->Release();
						}
					}
				}else if ( wordDes.str == "i" )
				{
					if( OpdStack.Pop( (HE_LPVOID*)&pTmpNode ) == TRUE )
					{
						if ( pTmpNode != NULL && pTmpNode->GetType() == OBJ_TYPE_NUMBER )
						{
							HE_BYTE byteTmp = ((CHE_PDF_Number*)pTmpNode)->GetInteger();
							CHE_PDF_OrderParam_Flatness * pTmp = GetAllocator()->New<CHE_PDF_OrderParam_Flatness>( byteTmp, GetAllocator() );
							CHE_PDF_OrderObject * pOrder = CHE_PDF_OrderObject::Create( GetAllocator() );
							pOrder->SetType( ORDER_FALTNESS );
							pOrder->SetParam( pTmp );
							m_arrContentObj.Append( pOrder );
							pTmpNode->Release();
						}
					}
				}else if ( wordDes.str == "SC" )
				{
				}else if ( wordDes.str == "sc" )
				{
				}else if ( wordDes.str == "G" )
				{
					if( OpdStack.Pop( (HE_LPVOID*)&pTmpNode ) == TRUE )
					{
						if ( pTmpNode != NULL && pTmpNode->GetType() == OBJ_TYPE_NUMBER )
						{
							HE_FLOAT fTmp = ((CHE_PDF_Number*)pTmpNode)->GetFloat();
							HE_DWORD value = 0;
							HE_DWORD bTmp = (HE_DWORD)( fTmp * 255 );
							value = 0xFF000000 | ( (bTmp<<16) | (bTmp<<8) | bTmp );
							CHE_PDF_Color color;
							color.SetValue( value );
							CHE_PDF_OrderParam_StrokeColor * pTmp = GetAllocator()->New<CHE_PDF_OrderParam_StrokeColor>( color, GetAllocator() );
							CHE_PDF_OrderObject * pOrder = CHE_PDF_OrderObject::Create( GetAllocator() );
							pOrder->SetType( ORDER_STROKE_COLOR );
							pOrder->SetParam( pTmp );
							m_arrContentObj.Append( pOrder );
							pTmpNode->Release();
						}
					}
				}else if ( wordDes.str == "g" )
				{
					if( OpdStack.Pop( (HE_LPVOID*)&pTmpNode ) == TRUE )
					{
						if ( pTmpNode != NULL && pTmpNode->GetType() == OBJ_TYPE_NUMBER )
						{
							HE_FLOAT fTmp = ((CHE_PDF_Number*)pTmpNode)->GetFloat();
							HE_DWORD value = 0;
							HE_DWORD bTmp = (HE_DWORD)( fTmp * 255 );
							value = 0xFF000000 | ( (bTmp<<16) | (bTmp<<8) | bTmp ); 
							CHE_PDF_Color color;
							color.SetValue( value );
							CHE_PDF_OrderParam_FillColor * pTmp = GetAllocator()->New<CHE_PDF_OrderParam_FillColor>( color, GetAllocator() );
							CHE_PDF_OrderObject * pOrder = CHE_PDF_OrderObject::Create( GetAllocator() );
							pOrder->SetType( ORDER_FILL_COLOR );
							pOrder->SetParam( pTmp );
							m_arrContentObj.Append( pOrder );
							pTmpNode->Release();
						}
					}
				}else if ( wordDes.str == "RG" )
				{
					HE_FLOAT fR = 0.0, fG = 0.0, fB = 0.0;
					HE_DWORD dwR = 0, dwG = 0, dwB = 0;
					HE_DWORD value = 0;
					if( OpdStack.Pop( (HE_LPVOID*)&pTmpNode ) == TRUE )
					{
						if ( pTmpNode != NULL && pTmpNode->GetType() == OBJ_TYPE_NUMBER )
						{
							fB = ((CHE_PDF_Number*)pTmpNode)->GetFloat();
							pTmpNode->Release();
						}
					}
					if( OpdStack.Pop( (HE_LPVOID*)&pTmpNode ) == TRUE )
					{
						if ( pTmpNode != NULL && pTmpNode->GetType() == OBJ_TYPE_NUMBER )
						{
							fG = ((CHE_PDF_Number*)pTmpNode)->GetFloat();
							pTmpNode->Release();
						}
					}
					if( OpdStack.Pop( (HE_LPVOID*)&pTmpNode ) == TRUE )
					{
						if ( pTmpNode != NULL && pTmpNode->GetType() == OBJ_TYPE_NUMBER )
						{
							fR = ((CHE_PDF_Number*)pTmpNode)->GetFloat();
							pTmpNode->Release();
						}
					}
					dwR = (HE_DWORD)( fR * 255 );
					dwG = (HE_DWORD)( fG * 255 );
					dwB = (HE_DWORD)( fB * 255 );
					value = 0xFF000000 | ( (dwR << 16) | (dwG << 8) | dwB );
					CHE_PDF_Color color;
					color.SetValue( value );
					CHE_PDF_OrderParam_StrokeColor * pTmp = GetAllocator()->New<CHE_PDF_OrderParam_StrokeColor>( color, GetAllocator() );
					CHE_PDF_OrderObject * pOrder = CHE_PDF_OrderObject::Create( GetAllocator() );
					pOrder->SetType( ORDER_STROKE_COLOR );
					pOrder->SetParam( pTmp );
					m_arrContentObj.Append( pOrder );
				}else if ( wordDes.str == "rg" )
				{
					HE_FLOAT fR = 0.0, fG = 0.0, fB = 0.0;
					HE_DWORD dwR = 0, dwG = 0, dwB = 0;
					HE_DWORD value = 0;
					if( OpdStack.Pop( (HE_LPVOID*)&pTmpNode ) == TRUE )
					{
						if ( pTmpNode != NULL && pTmpNode->GetType() == OBJ_TYPE_NUMBER )
						{
							fB = ((CHE_PDF_Number*)pTmpNode)->GetFloat();
							pTmpNode->Release();
						}
					}
					if( OpdStack.Pop( (HE_LPVOID*)&pTmpNode ) == TRUE )
					{
						if ( pTmpNode != NULL && pTmpNode->GetType() == OBJ_TYPE_NUMBER )
						{
							fG = ((CHE_PDF_Number*)pTmpNode)->GetFloat();
							pTmpNode->Release();
						}
					}
					if( OpdStack.Pop( (HE_LPVOID*)&pTmpNode ) == TRUE )
					{
						if ( pTmpNode != NULL && pTmpNode->GetType() == OBJ_TYPE_NUMBER )
						{
							fR = ((CHE_PDF_Number*)pTmpNode)->GetFloat();
							pTmpNode->Release();
						}
					}
					dwR = (HE_DWORD)( fR * 255 );
					dwG = (HE_DWORD)( fG * 255 );
					dwB = (HE_DWORD)( fB * 255 );
					value = 0xFF000000 | (dwR << 16) | (dwG << 8) | dwB;
					CHE_PDF_Color color;
					color.SetValue( value );
					CHE_PDF_OrderParam_FillColor * pTmp = GetAllocator()->New<CHE_PDF_OrderParam_FillColor>( color, GetAllocator() );
					CHE_PDF_OrderObject * pOrder = CHE_PDF_OrderObject::Create( GetAllocator() );
					pOrder->SetType( ORDER_FILL_COLOR );
					pOrder->SetParam( pTmp );
					m_arrContentObj.Append( pOrder );
				}else if ( wordDes.str == "K" )
				{
				}else if ( wordDes.str == "k" )
				{
				}

				//清除无用的操作数
				while ( OpdStack.Pop( (HE_LPVOID*)&pTmpNode ) == TRUE )
				{
					if ( pTmpNode != NULL )
					{
						pTmpNode->Release();
					}
				}
			}
		}

		//external object
		if ( wordDes.str == "Do" )
		{
			//清除无用的操作数
			while ( OpdStack.Pop( (HE_LPVOID*)&pTmpNode ) == TRUE )
			{
				if ( pTmpNode != NULL )
				{
					pTmpNode->Release();
				}
			}
			continue;
		}

		//inline images object
		if ( wordDes.str == "BI" )
		{
			//跳过inline images
			sParser.SeekToMark( CHE_ByteString("EI") );

			//清除无用的操作数
			while ( OpdStack.Pop( (HE_LPVOID*)&pTmpNode ) == TRUE )
			{
				if ( pTmpNode != NULL )
				{
					pTmpNode->Release();
				}
			}
			continue;
		}

		//shading object
		if ( wordDes.str == "sh" )
		{
			//清除无用的操作数
			while ( OpdStack.Pop( (HE_LPVOID*)&pTmpNode ) == TRUE )
			{
				if ( pTmpNode != NULL )
				{
					pTmpNode->Release();
				}
			}
			continue;
		}

		//其他的对象path，cliping，gs，ss，mark content，text state，color等相关对象和操作符号的处理
		if ( wordDes.str == "m" )
		{
			if( OpdStack.Pop( (HE_LPVOID*)&pTmpNode ) == TRUE )
			{
				if ( pTmpNode != NULL && pTmpNode->GetType() == OBJ_TYPE_NUMBER )
				{
					fCurY = ((CHE_PDF_Number*)pTmpNode)->GetFloat();
					pTmpNode->Release();
				}
			}
			if( OpdStack.Pop( (HE_LPVOID*)&pTmpNode ) == TRUE )
			{
				if ( pTmpNode != NULL && pTmpNode->GetType() == OBJ_TYPE_NUMBER )
				{
					fCurX = ((CHE_PDF_Number*)pTmpNode)->GetFloat();
					pTmpNode->Release();
				}
			}
			fBeginX = fCurX;
			fBeginY = fCurY;
			bSubPathClosed = FALSE;
			bConnect = FALSE;
		}else if ( wordDes.str == "l" )
		{
			HE_FLOAT x = 0, y = 0; 
			if( OpdStack.Pop( (HE_LPVOID*)&pTmpNode ) == TRUE )
			{
				if ( pTmpNode != NULL && pTmpNode->GetType() == OBJ_TYPE_NUMBER )
				{
					y = ((CHE_PDF_Number*)pTmpNode)->GetFloat();
					pTmpNode->Release();
				}
			}
			if( OpdStack.Pop( (HE_LPVOID*)&pTmpNode ) == TRUE )
			{
				if ( pTmpNode != NULL && pTmpNode->GetType() == OBJ_TYPE_NUMBER )
				{
					x = ((CHE_PDF_Number*)pTmpNode)->GetFloat();
					pTmpNode->Release();
				}
			}
			CHE_GraphicsObject * pGraph = GetAllocator()->New<CHE_Line>( fCurX, fCurY, x, y, bConnect, GetAllocator() );
			SupPathQueue.Push( (HE_LPVOID)pGraph );
			fCurX = x;
			fCurY = y;
			if ( bConnect == FALSE )
			{
				bConnect = TRUE;
			}
		}else if ( wordDes.str == "c" )
		{
			HE_FLOAT x1 = 0, y1 = 0, x2 = 0, y2 = 0, x3 = 0, y3 = 0 ; 
			if( OpdStack.Pop( (HE_LPVOID*)&pTmpNode ) == TRUE )
			{
				if ( pTmpNode != NULL && pTmpNode->GetType() == OBJ_TYPE_NUMBER )
				{
					y3 = ((CHE_PDF_Number*)pTmpNode)->GetFloat();
					pTmpNode->Release();
				}
			}
			if( OpdStack.Pop( (HE_LPVOID*)&pTmpNode ) == TRUE )
			{
				if ( pTmpNode != NULL && pTmpNode->GetType() == OBJ_TYPE_NUMBER )
				{
					x3 = ((CHE_PDF_Number*)pTmpNode)->GetFloat();
					pTmpNode->Release();
				}
			}
			if( OpdStack.Pop( (HE_LPVOID*)&pTmpNode ) == TRUE )
			{
				if ( pTmpNode != NULL && pTmpNode->GetType() == OBJ_TYPE_NUMBER )
				{
					y2 = ((CHE_PDF_Number*)pTmpNode)->GetFloat();
					pTmpNode->Release();
				}
			}
			if( OpdStack.Pop( (HE_LPVOID*)&pTmpNode ) == TRUE )
			{
				if ( pTmpNode != NULL && pTmpNode->GetType() == OBJ_TYPE_NUMBER )
				{
					x2 = ((CHE_PDF_Number*)pTmpNode)->GetFloat();
					pTmpNode->Release();
				}
			}
			if( OpdStack.Pop( (HE_LPVOID*)&pTmpNode ) == TRUE )
			{
				if ( pTmpNode != NULL && pTmpNode->GetType() == OBJ_TYPE_NUMBER )
				{
					y1 = ((CHE_PDF_Number*)pTmpNode)->GetFloat();
					pTmpNode->Release();
				}
			}
			if( OpdStack.Pop( (HE_LPVOID*)&pTmpNode ) == TRUE )
			{
				if ( pTmpNode != NULL && pTmpNode->GetType() == OBJ_TYPE_NUMBER )
				{
					x1 = ((CHE_PDF_Number*)pTmpNode)->GetFloat();
					pTmpNode->Release();
				}
			}
			CHE_GraphicsObject * pGraph = GetAllocator()->New<CHE_BCurve>( fCurX, fCurY, x1, y1, x2, y2, x3, y3, bConnect, GetAllocator() );
			SupPathQueue.Push( (HE_LPVOID)pGraph );
			fCurX = x3;
			fCurY = y3;
			if ( bConnect == FALSE )
			{
				bConnect = TRUE;
			}
		}else if ( wordDes.str == "v" )
		{
			HE_FLOAT x1 = 0, y1 = 0, x2 = 0, y2 = 0 ; 
			if( OpdStack.Pop( (HE_LPVOID*)&pTmpNode ) == TRUE )
			{
				if ( pTmpNode != NULL && pTmpNode->GetType() == OBJ_TYPE_NUMBER )
				{
					y2 = ((CHE_PDF_Number*)pTmpNode)->GetFloat();
					pTmpNode->Release();
				}
			}
			if( OpdStack.Pop( (HE_LPVOID*)&pTmpNode ) == TRUE )
			{
				if ( pTmpNode != NULL && pTmpNode->GetType() == OBJ_TYPE_NUMBER )
				{
					x2 = ((CHE_PDF_Number*)pTmpNode)->GetFloat();
					pTmpNode->Release();
				}
			}
			if( OpdStack.Pop( (HE_LPVOID*)&pTmpNode ) == TRUE )
			{
				if ( pTmpNode != NULL && pTmpNode->GetType() == OBJ_TYPE_NUMBER )
				{
					y1 = ((CHE_PDF_Number*)pTmpNode)->GetFloat();
					pTmpNode->Release();
				}
			}
			if( OpdStack.Pop( (HE_LPVOID*)&pTmpNode ) == TRUE )
			{
				if ( pTmpNode != NULL && pTmpNode->GetType() == OBJ_TYPE_NUMBER )
				{
					x1 = ((CHE_PDF_Number*)pTmpNode)->GetFloat();
					pTmpNode->Release();
				}
			}
			CHE_GraphicsObject * pGraph = GetAllocator()->New<CHE_BCurve>( fCurX, fCurY, fCurX, fCurY, x1, y1, x2, y2, bConnect, GetAllocator() );
			SupPathQueue.Push( (HE_LPVOID)pGraph );
			fCurX = x2;
			fCurY = y2;
			if ( bConnect == FALSE )
			{
				bConnect = TRUE;
			}
		}else if ( wordDes.str == "y" )
		{
			HE_FLOAT x1 = 0, y1 = 0, x2 = 0, y2 = 0 ; 
			if( OpdStack.Pop( (HE_LPVOID*)&pTmpNode ) == TRUE )
			{
				if ( pTmpNode != NULL && pTmpNode->GetType() == OBJ_TYPE_NUMBER )
				{
					y2 = ((CHE_PDF_Number*)pTmpNode)->GetFloat();
					pTmpNode->Release();
				}
			}
			if( OpdStack.Pop( (HE_LPVOID*)&pTmpNode ) == TRUE )
			{
				if ( pTmpNode != NULL && pTmpNode->GetType() == OBJ_TYPE_NUMBER )
				{
					x2 = ((CHE_PDF_Number*)pTmpNode)->GetFloat();
					pTmpNode->Release();
				}
			}
			if( OpdStack.Pop( (HE_LPVOID*)&pTmpNode ) == TRUE )
			{
				if ( pTmpNode != NULL && pTmpNode->GetType() == OBJ_TYPE_NUMBER )
				{
					y1 = ((CHE_PDF_Number*)pTmpNode)->GetFloat();
					pTmpNode->Release();
				}
			}
			if( OpdStack.Pop( (HE_LPVOID*)&pTmpNode ) == TRUE )
			{
				if ( pTmpNode != NULL && pTmpNode->GetType() == OBJ_TYPE_NUMBER )
				{
					x1 = ((CHE_PDF_Number*)pTmpNode)->GetFloat();
					pTmpNode->Release();
				}
			}
			CHE_GraphicsObject * pGraph = GetAllocator()->New<CHE_BCurve>( fCurX, fCurY, x1, y1, x2, y2, x2, y2, bConnect, GetAllocator() );
			SupPathQueue.Push( (HE_LPVOID)pGraph );
			fCurX = x2;
			fCurY = y2;
			if ( bConnect == FALSE )
			{
				bConnect = TRUE;
			}
		}else if ( wordDes.str == "h" )
		{
			if ( bSubPathClosed == FALSE )
			{
				CHE_GraphicsObject * pGraph = GetAllocator()->New<CHE_Line>( fCurX, fCurY, fBeginX, fBeginY, bConnect, GetAllocator() );
				SupPathQueue.Push( (HE_LPVOID)pGraph );
				bSubPathClosed = TRUE;
			}
			bConnect = FALSE;
			fCurX = fBeginX;
			fCurY = fBeginY;
		}else if ( wordDes.str == "re" )
		{
			HE_FLOAT x = 0, y = 0, width = 0, height = 0 ; 
			if( OpdStack.Pop( (HE_LPVOID*)&pTmpNode ) == TRUE )
			{
				if ( pTmpNode != NULL && pTmpNode->GetType() == OBJ_TYPE_NUMBER )
				{
					height = ((CHE_PDF_Number*)pTmpNode)->GetFloat();
					pTmpNode->Release();
				}
			}
			if( OpdStack.Pop( (HE_LPVOID*)&pTmpNode ) == TRUE )
			{
				if ( pTmpNode != NULL && pTmpNode->GetType() == OBJ_TYPE_NUMBER )
				{
					width = ((CHE_PDF_Number*)pTmpNode)->GetFloat();
					pTmpNode->Release();
				}
			}
			if( OpdStack.Pop( (HE_LPVOID*)&pTmpNode ) == TRUE )
			{
				if ( pTmpNode != NULL && pTmpNode->GetType() == OBJ_TYPE_NUMBER )
				{
					y = ((CHE_PDF_Number*)pTmpNode)->GetFloat();
					pTmpNode->Release();
				}
			}
			if( OpdStack.Pop( (HE_LPVOID*)&pTmpNode ) == TRUE )
			{
				if ( pTmpNode != NULL && pTmpNode->GetType() == OBJ_TYPE_NUMBER )
		   		{
					x = ((CHE_PDF_Number*)pTmpNode)->GetFloat();
					pTmpNode->Release();
				}
			}
			CHE_GraphicsObject * pGraph = GetAllocator()->New<CHE_Rectangle>( x, y, width, height, FALSE, GetAllocator() );
			SupPathQueue.Push( (HE_LPVOID)pGraph );
			fCurX = x;
			fCurY = y;
			bSubPathClosed = TRUE;
			bConnect = FALSE;
		}else if ( wordDes.str == "S" )
		{
			CHE_PDF_PathObject * pPath = GetAllocator()->New<CHE_PDF_PathObject>( GetAllocator() );
			pPath->SetType( PATH_STROKE );
			CHE_GraphicsObject * pSubPath = NULL;
			while( SupPathQueue.Pop( (HE_LPVOID*)&pSubPath ) == TRUE )
			{
				if ( pSubPath )
				{
					pPath->AppendItem( pSubPath );
				}
			}
			if ( bClipPath == TRUE )
			{
				pPath->SetType( PATH_STROKE_CLIP );
				pPath->SetClipFillMode( ClipFillMode );
				bClipPath = FALSE;
			}
			m_arrContentObj.Append( (HE_LPVOID)pPath );
		}else if ( wordDes.str == "s" )
		{
			if ( bSubPathClosed == FALSE )
			{
				CHE_GraphicsObject * pGraph = GetAllocator()->New<CHE_Line>( fCurX, fCurY, fBeginX, fBeginY, bConnect, GetAllocator() );
				SupPathQueue.Push( (HE_LPVOID)pGraph );
				bSubPathClosed = TRUE;
			}
			bConnect = FALSE;
			
			CHE_PDF_PathObject * pPath = GetAllocator()->New<CHE_PDF_PathObject>( GetAllocator() );
			pPath->SetType( PATH_STROKE );
			CHE_GraphicsObject * pSubPath = NULL;
			while( SupPathQueue.Pop( (HE_LPVOID*)&pSubPath ) == TRUE )
			{
				if ( pSubPath )
				{
					pPath->AppendItem( pSubPath );
				}
			}
			if ( bClipPath == TRUE )
			{
				pPath->SetType( PATH_STROKE_CLIP );
				pPath->SetClipFillMode( ClipFillMode );
				bClipPath = FALSE;
			}
			m_arrContentObj.Append( (HE_LPVOID)pPath );
		}else if ( wordDes.str == "f" || wordDes.str == "F" )
		{
			CHE_PDF_PathObject * pPath = GetAllocator()->New<CHE_PDF_PathObject>( GetAllocator() );
			pPath->SetType( PATH_FILL );
			CHE_GraphicsObject * pSubPath = NULL;
			while( SupPathQueue.Pop( (HE_LPVOID*)&pSubPath ) == TRUE )
			{
				if ( pSubPath )
				{
					pPath->AppendItem( pSubPath );
				}
			}
			if ( bClipPath == TRUE )
			{
				pPath->SetType( PATH_FILL_CLIP );
				pPath->SetClipFillMode( ClipFillMode );
				bClipPath = FALSE;
			}
			m_arrContentObj.Append( (HE_LPVOID)pPath );
		}else if ( wordDes.str == "f*" )
		{
			CHE_PDF_PathObject * pPath = GetAllocator()->New<CHE_PDF_PathObject>( GetAllocator() );
			pPath->SetType( PATH_FILL );
			pPath->SetFillMode( FILL_MODE_EVERODD );
			CHE_GraphicsObject * pSubPath = NULL;
			while( SupPathQueue.Pop( (HE_LPVOID*)&pSubPath ) == TRUE )
			{
				if ( pSubPath )
				{
					pPath->AppendItem( pSubPath );
				}
			}
			if ( bClipPath == TRUE )
			{
				pPath->SetType( PATH_FILL_CLIP );
				pPath->SetClipFillMode( ClipFillMode );
				bClipPath = FALSE;
			}
			m_arrContentObj.Append( (HE_LPVOID)pPath );
		}else if ( wordDes.str == "B" )
		{
			CHE_PDF_PathObject * pPath = GetAllocator()->New<CHE_PDF_PathObject>( GetAllocator() );
			CHE_GraphicsObject * pSubPath = NULL;
			while( SupPathQueue.Pop( (HE_LPVOID*)&pSubPath ) == TRUE )
			{
				if ( pSubPath )
				{
					pPath->AppendItem( pSubPath );
				}
			}
			pPath->SetType( PATH_FILLSTROKE );
			pPath->SetFillMode( FILL_MODE_NOZERO );
			if ( bClipPath == TRUE )
			{
				pPath->SetType( PATH_FILLSTROKE_CLIP );
				pPath->SetClipFillMode( ClipFillMode );
				bClipPath = FALSE;
			}
			m_arrContentObj.Append( (HE_LPVOID)pPath );
		}else if ( wordDes.str == "B*" )
		{
			CHE_PDF_PathObject * pPath = GetAllocator()->New<CHE_PDF_PathObject>( GetAllocator() );
			CHE_GraphicsObject * pSubPath = NULL;
			while( SupPathQueue.Pop( (HE_LPVOID*)&pSubPath ) == TRUE )
			{
				if ( pSubPath )
				{
					pPath->AppendItem( pSubPath );
				}
			}
			pPath->SetType( PATH_FILLSTROKE );
			pPath->SetFillMode( FILL_MODE_EVERODD );

			if ( bClipPath == TRUE )
			{
				pPath->SetType( PATH_FILLSTROKE_CLIP );
				pPath->SetClipFillMode( ClipFillMode );
				bClipPath = FALSE;
			}
			m_arrContentObj.Append( (HE_LPVOID)pPath );
		}else if ( wordDes.str == "b" )
		{
			if ( bSubPathClosed == FALSE )
			{
				CHE_GraphicsObject * pGraph = GetAllocator()->New<CHE_Line>( fCurX, fCurY, fBeginX, fBeginY, FALSE, GetAllocator() );
				SupPathQueue.Push( (HE_LPVOID)pGraph );
				bSubPathClosed = TRUE;
			}
			CHE_PDF_PathObject * pPath = GetAllocator()->New<CHE_PDF_PathObject>( GetAllocator() );
			CHE_GraphicsObject * pSubPath = NULL;
			while( SupPathQueue.Pop( (HE_LPVOID*)&pSubPath ) == TRUE )
			{
				if ( pSubPath )
				{
					pPath->AppendItem( pSubPath );
				}
			}
			pPath->SetType( PATH_FILLSTROKE );
			pPath->SetFillMode( FILL_MODE_NOZERO );
			if ( bClipPath == TRUE )
			{
				pPath->SetType( PATH_FILLSTROKE_CLIP );
				pPath->SetClipFillMode( ClipFillMode );
				bClipPath = FALSE;
			}
			m_arrContentObj.Append( (HE_LPVOID)pPath );
		}else if ( wordDes.str == "b*" )
		{
			if ( bSubPathClosed == FALSE )
			{
				CHE_GraphicsObject * pGraph = GetAllocator()->New<CHE_Line>( fCurX, fCurY, fBeginX, fBeginY, FALSE, GetAllocator() );
				SupPathQueue.Push( (HE_LPVOID)pGraph );
				bSubPathClosed = TRUE;
			}
			CHE_PDF_PathObject * pPath = GetAllocator()->New<CHE_PDF_PathObject>( GetAllocator() );
			CHE_GraphicsObject * pSubPath = NULL;
			while( SupPathQueue.Pop( (HE_LPVOID*)&pSubPath ) == TRUE )
			{
				if ( pSubPath )
				{
					pPath->AppendItem( pSubPath );
				}
			}
			pPath->SetType( PATH_FILLSTROKE );
			pPath->SetFillMode( FILL_MODE_EVERODD );
			if ( bClipPath == TRUE )
			{
				pPath->SetType( PATH_FILLSTROKE_CLIP );
				pPath->SetClipFillMode( ClipFillMode );
				bClipPath = FALSE;
			}
			m_arrContentObj.Append( (HE_LPVOID)pPath );
		}else if ( wordDes.str == "n" )
		{
			CHE_PDF_PathObject * pPath = GetAllocator()->New<CHE_PDF_PathObject>( GetAllocator() );
			CHE_GraphicsObject * pSubPath = NULL;
			while( SupPathQueue.Pop( (HE_LPVOID*)&pSubPath ) == TRUE )
			{
				if ( pSubPath )
				{
					pPath->AppendItem( pSubPath );
				}
			}
			pPath->SetType( PATH_NOOP );
			if ( bClipPath == TRUE )
			{
				pPath->SetType( PATH_NOOP_CLIP );
				pPath->SetClipFillMode( ClipFillMode );
				bClipPath = FALSE;
			}
			m_arrContentObj.Append( (HE_LPVOID)pPath );
		}else if ( wordDes.str == "W" )
		{
			bClipPath = TRUE;
			ClipFillMode = FILL_MODE_NOZERO;
		}else if ( wordDes.str == "W*" )
		{
			bClipPath = TRUE;
			ClipFillMode = FILL_MODE_EVERODD;
		}else if ( wordDes.str == "cm" )
		{
			HE_FLOAT a = 0, b = 0, c = 0, d = 0, e = 0, f = 0 ; 
			if( OpdStack.Pop( (HE_LPVOID*)&pTmpNode ) == TRUE )
			{
				if ( pTmpNode != NULL && pTmpNode->GetType() == OBJ_TYPE_NUMBER )
				{
					f = ((CHE_PDF_Number*)pTmpNode)->GetFloat();
					pTmpNode->Release();
				}
			}
			if( OpdStack.Pop( (HE_LPVOID*)&pTmpNode ) == TRUE )
			{
				if ( pTmpNode != NULL && pTmpNode->GetType() == OBJ_TYPE_NUMBER )
				{
					e = ((CHE_PDF_Number*)pTmpNode)->GetFloat();
					pTmpNode->Release();
				}
			}
			if( OpdStack.Pop( (HE_LPVOID*)&pTmpNode ) == TRUE )
			{
				if ( pTmpNode != NULL && pTmpNode->GetType() == OBJ_TYPE_NUMBER )
				{
					d = ((CHE_PDF_Number*)pTmpNode)->GetFloat();
					pTmpNode->Release();
				}
			}
			if( OpdStack.Pop( (HE_LPVOID*)&pTmpNode ) == TRUE )
			{
				if ( pTmpNode != NULL && pTmpNode->GetType() == OBJ_TYPE_NUMBER )
				{
					c = ((CHE_PDF_Number*)pTmpNode)->GetFloat();
					pTmpNode->Release();
				}
			}
			if( OpdStack.Pop( (HE_LPVOID*)&pTmpNode ) == TRUE )
			{
				if ( pTmpNode != NULL && pTmpNode->GetType() == OBJ_TYPE_NUMBER )
				{
					b = ((CHE_PDF_Number*)pTmpNode)->GetFloat();
					pTmpNode->Release();
				}
			}
			if( OpdStack.Pop( (HE_LPVOID*)&pTmpNode ) == TRUE )
			{
				if ( pTmpNode != NULL && pTmpNode->GetType() == OBJ_TYPE_NUMBER )
				{
					a = ((CHE_PDF_Number*)pTmpNode)->GetFloat();
					pTmpNode->Release();
				}
			}
		}else if ( wordDes.str == "q" )
		{
			CHE_PDF_OrderObject * pClipOrder = CHE_PDF_OrderObject::Create( GetAllocator() );
			pClipOrder->SetType( ORDER_PUSH_STATE );
			m_arrContentObj.Append( pClipOrder );
		}else if ( wordDes.str == "Q" )
		{
			CHE_PDF_OrderObject * pClipOrder = CHE_PDF_OrderObject::Create( GetAllocator() );
			pClipOrder->SetType( ORDER_POP_STATE );
			m_arrContentObj.Append( pClipOrder );
		}else if ( wordDes.str == "gs" )
		{
			if ( pExtGStateDict == NULL )
			{
				return NULL;
			}
			if ( OpdStack.Pop( (HE_LPVOID*)&pTmpNode ) == TRUE )
			{
				if ( pTmpNode != NULL && pTmpNode->GetType() == OBJ_TYPE_NAME )
				{
					CHE_PDF_Dictionary * pGraphStateDict = (CHE_PDF_Dictionary*)pExtGStateDict->GetElement( ((CHE_PDF_Name*)pTmpNode)->GetString(), OBJ_TYPE_DICTIONARY );
					if ( pGraphStateDict != NULL )
					{
						CHE_PDF_Object * pTmpObj = NULL;
						CHE_PDF_OrderObject * pOrder = CHE_PDF_OrderObject::Create( GetAllocator() );
						pOrder->SetType( ORDER_SET_STATE );
						CHE_PDF_OrderParam_GraphState * pOrderParam = GetAllocator()->New<CHE_PDF_OrderParam_GraphState>( GetAllocator() );
						pTmpObj = pGraphStateDict->GetElement( "LW", OBJ_TYPE_NUMBER );
						if ( pTmpObj != NULL )
						{
							HE_FLOAT fLineWidth = (HE_FLOAT)( ((CHE_PDF_Number*)pTmpObj)->GetFloat() );
							CHE_PDF_OrderParam_LineWidth * pParam = GetAllocator()->New<CHE_PDF_OrderParam_LineWidth>( fLineWidth, GetAllocator() );
							pOrderParam->Append( pParam );
						}

						pTmpObj = pGraphStateDict->GetElement( "LC", OBJ_TYPE_NUMBER );
						if ( pTmpObj != NULL )
						{
							HE_BYTE byteLineCap = (HE_BYTE)( ((CHE_PDF_Number*)pTmpObj)->GetInteger() );
							CHE_PDF_OrderParam_LineCap * pParam = GetAllocator()->New<CHE_PDF_OrderParam_LineCap>( byteLineCap, GetAllocator() );
							pOrderParam->Append( pParam );
						}

						pTmpObj = pGraphStateDict->GetElement( "LJ", OBJ_TYPE_NUMBER );
						if ( pTmpObj != NULL )
						{
							HE_BYTE byteLineJoin = (HE_BYTE)( ((CHE_PDF_Number*)pTmpObj)->GetInteger() );
							CHE_PDF_OrderParam_LineJoin * pParam = GetAllocator()->New<CHE_PDF_OrderParam_LineJoin>( byteLineJoin, GetAllocator() );
							pOrderParam->Append( pParam );
						}

						pTmpObj = pGraphStateDict->GetElement( "ML", OBJ_TYPE_NUMBER );
						if ( pTmpObj != NULL )
						{
							HE_FLOAT fMiterLimit = (HE_FLOAT)( ((CHE_PDF_Number*)pTmpObj)->GetFloat() );
							CHE_PDF_OrderParam_MiterLimit * pParam = GetAllocator()->New<CHE_PDF_OrderParam_MiterLimit>( fMiterLimit, GetAllocator() );
							pOrderParam->Append( pParam );
						}

						pTmpObj = pGraphStateDict->GetElement( "D", OBJ_TYPE_ARRAY );
						if ( pTmpObj != NULL )
						{
							HE_FLOAT * pDashArray = NULL;
							HE_FLOAT fDashPhase = 0;
							HE_DWORD dwDashArraySize = 0;
	
							CHE_PDF_Object * pFirstElement = ((CHE_PDF_Array*)pTmpObj)->GetElement( 0, OBJ_TYPE_ARRAY );
							CHE_PDF_Object * pSecondElement = ((CHE_PDF_Array*)pTmpObj)->GetElement( 1, OBJ_TYPE_NUMBER );
							if ( pFirstElement != NULL )
							{
								CHE_PDF_Array * pArray = (CHE_PDF_Array*)pFirstElement;
								dwDashArraySize = pArray->GetCount();
								pDashArray = GetAllocator()->NewArray<HE_FLOAT>( dwDashArraySize );
								CHE_PDF_Number * pNumber = NULL;
								for ( HE_DWORD i = 0; i < dwDashArraySize; i++ )
								{
									pNumber = (CHE_PDF_Number*)pArray->GetElement( i, OBJ_TYPE_NUMBER );
									if ( pNumber != NULL )
									{
										pDashArray[i] = pNumber->GetFloat();
									}else{
										pDashArray[i] = 0;
									}
								}
							}
							if ( pSecondElement != NULL )
							{
								fDashPhase = ((CHE_PDF_Number*)pSecondElement)->GetFloat();
							}
							CHE_PDF_OrderParam_DashPattern * pParam = GetAllocator()->New<CHE_PDF_OrderParam_DashPattern>( pDashArray, dwDashArraySize, fDashPhase, GetAllocator() );
							pOrderParam->Append( pParam );
							if ( pDashArray != NULL )
							{
								GetAllocator()->DeleteArray<HE_FLOAT>( pDashArray );
								pDashArray = NULL;
							}
						}
						pTmpNode->Release();
					}
				}
			}
		}else if ( wordDes.str == "w" )	//line width 
		{
			if( OpdStack.Pop( (HE_LPVOID*)&pTmpNode ) == TRUE )
			{
				if ( pTmpNode != NULL && pTmpNode->GetType() == OBJ_TYPE_NUMBER )
				{
					HE_FLOAT fTmp = ((CHE_PDF_Number*)pTmpNode)->GetFloat();
					CHE_PDF_OrderParam_LineWidth * pTmp = GetAllocator()->New<CHE_PDF_OrderParam_LineWidth>( fTmp, GetAllocator() );
					CHE_PDF_OrderObject * pOrder = CHE_PDF_OrderObject::Create( GetAllocator() );
					pOrder->SetType( ORDER_LINE_WIDTH );
					pOrder->SetParam( pTmp );
					m_arrContentObj.Append( pOrder );
					pTmpNode->Release();
				}
			}
		}else if ( wordDes.str == "J" )	//line cap
		{
			if( OpdStack.Pop( (HE_LPVOID*)&pTmpNode ) == TRUE )
			{
				if ( pTmpNode != NULL && pTmpNode->GetType() == OBJ_TYPE_NUMBER )
				{
					HE_BYTE dwByte = ((CHE_PDF_Number*)pTmpNode)->GetInteger();
					CHE_PDF_OrderParam_LineCap * pTmp = GetAllocator()->New<CHE_PDF_OrderParam_LineCap>( dwByte, GetAllocator() );
					CHE_PDF_OrderObject * pOrder = CHE_PDF_OrderObject::Create( GetAllocator() );
					pOrder->SetType( ORDER_LINE_CAP );
					pOrder->SetParam( pTmp );
					m_arrContentObj.Append( pOrder );
					pTmpNode->Release();
				}
			}
		}else if ( wordDes.str == "j" )	//line join
		{
			if( OpdStack.Pop( (HE_LPVOID*)&pTmpNode ) == TRUE )
			{
				if ( pTmpNode != NULL && pTmpNode->GetType() == OBJ_TYPE_NUMBER )
				{
					HE_BYTE dwByte = ((CHE_PDF_Number*)pTmpNode)->GetInteger();
					CHE_PDF_OrderParam_LineJoin * pTmp = GetAllocator()->New<CHE_PDF_OrderParam_LineJoin>( dwByte, GetAllocator() );
					CHE_PDF_OrderObject * pOrder = CHE_PDF_OrderObject::Create( GetAllocator() );
					pOrder->SetType( ORDER_LINE_JION );
					pOrder->SetParam( pTmp );
					m_arrContentObj.Append( pOrder );
					pTmpNode->Release();
				}
			}
		}else if ( wordDes.str == "M" )	//Miter limit
		{
			if( OpdStack.Pop( (HE_LPVOID*)&pTmpNode ) == TRUE )
			{
				if ( pTmpNode != NULL && pTmpNode->GetType() == OBJ_TYPE_NUMBER )
				{
					HE_FLOAT fTmp = ((CHE_PDF_Number*)pTmpNode)->GetFloat();
					CHE_PDF_OrderParam_MiterLimit * pTmp = GetAllocator()->New<CHE_PDF_OrderParam_MiterLimit>( fTmp, GetAllocator() );
					CHE_PDF_OrderObject * pOrder = CHE_PDF_OrderObject::Create( GetAllocator() );
					pOrder->SetType( ORDER_MITER_LIMIT );
					pOrder->SetParam( pTmp );
					m_arrContentObj.Append( pOrder );
					pTmpNode->Release();
				}
			}
		}else if ( wordDes.str == "d" )	//dash
		{
			HE_FLOAT dashPhase = 0;
			HE_FLOAT * dashArray = NULL;
			HE_DWORD dashArraySize = 0;
			if( OpdStack.Pop( (HE_LPVOID*)&pTmpNode ) == TRUE )
			{
				if ( pTmpNode != NULL && pTmpNode->GetType() == OBJ_TYPE_NUMBER )
				{
					dashPhase = ((CHE_PDF_Number*)pTmpNode)->GetFloat();
					pTmpNode->Release();
				}
			}
			if( OpdStack.Pop( (HE_LPVOID*)&pTmpNode ) == TRUE )
			{
				if ( pTmpNode != NULL && pTmpNode->GetType() == OBJ_TYPE_ARRAY )
				{
					dashArraySize = ((CHE_PDF_Array*)pTmpNode)->GetCount();
					if ( dashArraySize == 0 )
					{
						CHE_PDF_OrderParam_DashPattern * pTmp = GetAllocator()->New<CHE_PDF_OrderParam_DashPattern>( dashArray, 0, dashPhase, GetAllocator() );
						CHE_PDF_OrderObject * pOrder = CHE_PDF_OrderObject::Create( GetAllocator() );
						pOrder->SetType( ORDER_DASH_PATTERN );
						pOrder->SetParam( pTmp );
						m_arrContentObj.Append( pOrder );
					}else{
						dashArray = GetAllocator()->NewArray<HE_FLOAT>( dashArraySize );
						CHE_PDF_Object * pObj = NULL;
						for ( HE_DWORD i = 0; i < dashArraySize; i++ )
						{
							pObj = ((CHE_PDF_Array*)pTmpNode)->GetElement( i );
							if ( pObj != NULL && pObj->GetType() == OBJ_TYPE_NUMBER )
							{
								dashArray[i] = ((CHE_PDF_Number*)pObj)->GetFloat();
							}
						}
						CHE_PDF_OrderParam_DashPattern * pTmp = GetAllocator()->New<CHE_PDF_OrderParam_DashPattern>( dashArray, dashArraySize, dashPhase, GetAllocator() );
						CHE_PDF_OrderObject * pOrder = CHE_PDF_OrderObject::Create( GetAllocator() );
						pOrder->SetType( ORDER_DASH_PATTERN );
						pOrder->SetParam( pTmp );
						m_arrContentObj.Append( pOrder );
						GetAllocator()->DeleteArray<HE_FLOAT>( dashArray );
					}
					pTmpNode->Release();
				}
			}
		}else if ( wordDes.str == "ri" )
		{
			if( OpdStack.Pop( (HE_LPVOID*)&pTmpNode ) == TRUE )
			{
				if ( pTmpNode != NULL && pTmpNode->GetType() == OBJ_TYPE_NUMBER )
				{
					CHE_PDF_OrderParam_Intent * pTmp = GetAllocator()->New<CHE_PDF_OrderParam_Intent>( GetAllocator() );
					CHE_PDF_OrderObject * pOrder = CHE_PDF_OrderObject::Create( GetAllocator() );
					pOrder->SetType( ORDER_INTENT );
					pOrder->SetParam( pTmp );
					m_arrContentObj.Append( pOrder );
					pTmpNode->Release();
				}
			}
		}else if ( wordDes.str == "i" )
		{
			if( OpdStack.Pop( (HE_LPVOID*)&pTmpNode ) == TRUE )
			{
				if ( pTmpNode != NULL && pTmpNode->GetType() == OBJ_TYPE_NUMBER )
				{
					HE_BYTE byteTmp = ((CHE_PDF_Number*)pTmpNode)->GetInteger();
					CHE_PDF_OrderParam_Flatness * pTmp = GetAllocator()->New<CHE_PDF_OrderParam_Flatness>( byteTmp, GetAllocator() );
					CHE_PDF_OrderObject * pOrder = CHE_PDF_OrderObject::Create( GetAllocator() );
					pOrder->SetType( ORDER_FALTNESS );
					pOrder->SetParam( pTmp );
					m_arrContentObj.Append( pOrder );
					pTmpNode->Release();
				}
			}
		}else if ( wordDes.str == "gs" )
		{
			if ( pExtGStateDict == NULL )
			{
				return NULL;
			}
			if ( OpdStack.Pop( (HE_LPVOID*)&pTmpNode ) == TRUE )
			{
				if ( pTmpNode != NULL && pTmpNode->GetType() == OBJ_TYPE_NAME )
				{
					CHE_PDF_Dictionary * pGraphStateDict = (CHE_PDF_Dictionary*)pExtGStateDict->GetElement( ((CHE_PDF_Name*)pTmpNode)->GetString(), OBJ_TYPE_DICTIONARY );
					if ( pGraphStateDict != NULL )
					{
						CHE_PDF_Object * pTmpObj = NULL;
						CHE_PDF_OrderObject * pOrder = CHE_PDF_OrderObject::Create( GetAllocator() );
						pOrder->SetType( ORDER_SET_STATE );
						CHE_PDF_OrderParam_GraphState * pOrderParam = GetAllocator()->New<CHE_PDF_OrderParam_GraphState>( GetAllocator() );
						pTmpObj = pGraphStateDict->GetElement( "LW", OBJ_TYPE_NUMBER );
						if ( pTmpObj != NULL )
						{
							HE_FLOAT fLineWidth = (HE_FLOAT)( ((CHE_PDF_Number*)pTmpObj)->GetFloat() );
							CHE_PDF_OrderParam_LineWidth * pParam = GetAllocator()->New<CHE_PDF_OrderParam_LineWidth>( fLineWidth, GetAllocator() );
							pOrderParam->Append( pParam );
						}

						pTmpObj = pGraphStateDict->GetElement( "LC", OBJ_TYPE_NUMBER );
						if ( pTmpObj != NULL )
						{
							HE_BYTE byteLineCap = (HE_BYTE)( ((CHE_PDF_Number*)pTmpObj)->GetInteger() );
							CHE_PDF_OrderParam_LineCap * pParam = GetAllocator()->New<CHE_PDF_OrderParam_LineCap>( byteLineCap, GetAllocator() );
							pOrderParam->Append( pParam );
						}

						pTmpObj = pGraphStateDict->GetElement( "LJ", OBJ_TYPE_NUMBER );
						if ( pTmpObj != NULL )
						{
							HE_BYTE byteLineJoin = (HE_BYTE)( ((CHE_PDF_Number*)pTmpObj)->GetInteger() );
							CHE_PDF_OrderParam_LineJoin * pParam = GetAllocator()->New<CHE_PDF_OrderParam_LineJoin>( byteLineJoin, GetAllocator() );
							pOrderParam->Append( pParam );
						}

						pTmpObj = pGraphStateDict->GetElement( "ML", OBJ_TYPE_NUMBER );
						if ( pTmpObj != NULL )
						{
							HE_FLOAT fMiterLimit = (HE_FLOAT)( ((CHE_PDF_Number*)pTmpObj)->GetFloat() );
							CHE_PDF_OrderParam_MiterLimit * pParam = GetAllocator()->New<CHE_PDF_OrderParam_MiterLimit>( fMiterLimit, GetAllocator() );
							pOrderParam->Append( pParam );
						}

						pTmpObj = pGraphStateDict->GetElement( "D", OBJ_TYPE_ARRAY );
						if ( pTmpObj != NULL )
						{
							HE_FLOAT * pDashArray = NULL;
							HE_FLOAT fDashPhase = 0;
							HE_DWORD dwDashArraySize = 0;

							CHE_PDF_Object * pFirstElement = ((CHE_PDF_Array*)pTmpObj)->GetElement( 0, OBJ_TYPE_ARRAY );
							CHE_PDF_Object * pSecondElement = ((CHE_PDF_Array*)pTmpObj)->GetElement( 1, OBJ_TYPE_NUMBER );
							if ( pFirstElement != NULL )
							{
								CHE_PDF_Array * pArray = (CHE_PDF_Array*)pFirstElement;
								dwDashArraySize = pArray->GetCount();
								pDashArray = GetAllocator()->NewArray<HE_FLOAT>( dwDashArraySize );
								CHE_PDF_Number * pNumber = NULL;
								for ( HE_DWORD i = 0; i < dwDashArraySize; i++ )
								{
									pNumber = (CHE_PDF_Number*)pArray->GetElement( i, OBJ_TYPE_NUMBER );
									if ( pNumber != NULL )
									{
										pDashArray[i] = pNumber->GetFloat();
									}else{
										pDashArray[i] = 0;
									}
								}
							}
							if ( pSecondElement != NULL )
							{
								fDashPhase = ((CHE_PDF_Number*)pSecondElement)->GetFloat();
							}
							CHE_PDF_OrderParam_DashPattern * pParam = GetAllocator()->New<CHE_PDF_OrderParam_DashPattern>( pDashArray, dwDashArraySize, fDashPhase, GetAllocator() );
							pOrderParam->Append( pParam );
							if ( pDashArray != NULL )
							{
								GetAllocator()->DeleteArray<HE_FLOAT>( pDashArray );
								pDashArray = NULL;
							}
						}
						pTmpNode->Release();
					}
				}
			}		
		}else if ( wordDes.str == "SC" )
		{
		}else if ( wordDes.str == "sc" )
		{
		}else if ( wordDes.str == "G" )
		{
			if( OpdStack.Pop( (HE_LPVOID*)&pTmpNode ) == TRUE )
			{
				if ( pTmpNode != NULL && pTmpNode->GetType() == OBJ_TYPE_NUMBER )
				{
					HE_FLOAT fTmp = ((CHE_PDF_Number*)pTmpNode)->GetFloat();
					HE_DWORD value = 0;
					HE_DWORD bTmp = (HE_DWORD)( fTmp * 255 );
					value = 0xFF000000 | ( (bTmp<<16) | (bTmp<<8) | bTmp ); 
					CHE_PDF_Color color;
					color.SetValue( value );
					CHE_PDF_OrderParam_StrokeColor * pTmp = GetAllocator()->New<CHE_PDF_OrderParam_StrokeColor>( color, GetAllocator() );
					CHE_PDF_OrderObject * pOrder = CHE_PDF_OrderObject::Create( GetAllocator() );
					pOrder->SetType( ORDER_STROKE_COLOR );
					pOrder->SetParam( pTmp );
					m_arrContentObj.Append( pOrder );
					pTmpNode->Release();
				}
			}
		}else if ( wordDes.str == "g" )
		{
			if( OpdStack.Pop( (HE_LPVOID*)&pTmpNode ) == TRUE )
			{
				if ( pTmpNode != NULL && pTmpNode->GetType() == OBJ_TYPE_NUMBER )
				{
					HE_FLOAT fTmp = ((CHE_PDF_Number*)pTmpNode)->GetFloat();
					HE_DWORD value = 0;
					HE_DWORD bTmp = (HE_DWORD)( fTmp * 255 );
					value = 0xFF000000 | ( (bTmp<<16) | (bTmp<<8) | bTmp ); 
					CHE_PDF_Color color;
					color.SetValue( value );
					CHE_PDF_OrderParam_FillColor * pTmp = GetAllocator()->New<CHE_PDF_OrderParam_FillColor>( color, GetAllocator() );
					CHE_PDF_OrderObject * pOrder = CHE_PDF_OrderObject::Create( GetAllocator() );
					pOrder->SetType( ORDER_FILL_COLOR );
					pOrder->SetParam( pTmp );
					m_arrContentObj.Append( pOrder );
					pTmpNode->Release();
				}
			}
		}else if ( wordDes.str == "RG" )
		{
			HE_FLOAT fR = 0.0, fG = 0.0, fB = 0.0;
			HE_DWORD dwR = 0, dwG = 0, dwB = 0;
			HE_DWORD value = 0;
			if( OpdStack.Pop( (HE_LPVOID*)&pTmpNode ) == TRUE )
			{
				if ( pTmpNode != NULL && pTmpNode->GetType() == OBJ_TYPE_NUMBER )
				{
					fB = ((CHE_PDF_Number*)pTmpNode)->GetFloat();
					pTmpNode->Release();
				}
			}
			if( OpdStack.Pop( (HE_LPVOID*)&pTmpNode ) == TRUE )
			{
				if ( pTmpNode != NULL && pTmpNode->GetType() == OBJ_TYPE_NUMBER )
				{
					fG = ((CHE_PDF_Number*)pTmpNode)->GetFloat();
					pTmpNode->Release();
				}
			}
			if( OpdStack.Pop( (HE_LPVOID*)&pTmpNode ) == TRUE )
			{
				if ( pTmpNode != NULL && pTmpNode->GetType() == OBJ_TYPE_NUMBER )
				{
					fR = ((CHE_PDF_Number*)pTmpNode)->GetFloat();
					pTmpNode->Release();
				}
			}
			dwR = (HE_DWORD)( fR * 255 );
			dwG = (HE_DWORD)( fG * 255 );
			dwB = (HE_DWORD)( fB * 255 );
			value = 0xFF000000 | ( (dwR << 16) | (dwG << 8) | dwB );
			CHE_PDF_Color color;
			color.SetValue( value );
			CHE_PDF_OrderParam_StrokeColor * pTmp = GetAllocator()->New<CHE_PDF_OrderParam_StrokeColor>( color, GetAllocator() );
			CHE_PDF_OrderObject * pOrder = CHE_PDF_OrderObject::Create( GetAllocator() );
			pOrder->SetType( ORDER_STROKE_COLOR );
			pOrder->SetParam( pTmp );
			m_arrContentObj.Append( pOrder );
		}else if ( wordDes.str == "rg" )
		{
			HE_FLOAT fR = 0.0, fG = 0.0, fB = 0.0;
			HE_DWORD dwR = 0, dwG = 0, dwB = 0;
			HE_DWORD value = 0;
			if( OpdStack.Pop( (HE_LPVOID*)&pTmpNode ) == TRUE )
			{
				if ( pTmpNode != NULL && pTmpNode->GetType() == OBJ_TYPE_NUMBER )
				{
					fB = ((CHE_PDF_Number*)pTmpNode)->GetFloat();
					pTmpNode->Release();
				}
			}
			if( OpdStack.Pop( (HE_LPVOID*)&pTmpNode ) == TRUE )
			{
				if ( pTmpNode != NULL && pTmpNode->GetType() == OBJ_TYPE_NUMBER )
				{
					fG = ((CHE_PDF_Number*)pTmpNode)->GetFloat();
					pTmpNode->Release();
				}
			}
			if( OpdStack.Pop( (HE_LPVOID*)&pTmpNode ) == TRUE )
			{
				if ( pTmpNode != NULL && pTmpNode->GetType() == OBJ_TYPE_NUMBER )
				{
					fR = ((CHE_PDF_Number*)pTmpNode)->GetFloat();
					pTmpNode->Release();
				}
			}
			dwR = (HE_DWORD)( fR * 255 );
			dwG = (HE_DWORD)( fG * 255 );
			dwB = (HE_DWORD)( fB * 255 );
			value = 0xFF000000 | (dwR << 16) | (dwG << 8) | dwB;
			CHE_PDF_Color color;
			color.SetValue( value );
			CHE_PDF_OrderParam_FillColor * pTmp = GetAllocator()->New<CHE_PDF_OrderParam_FillColor>( color, GetAllocator() );
			CHE_PDF_OrderObject * pOrder = CHE_PDF_OrderObject::Create( GetAllocator() );
			pOrder->SetType( ORDER_FILL_COLOR );
			pOrder->SetParam( pTmp );
			m_arrContentObj.Append( pOrder );
		}else if ( wordDes.str == "K" )
		{
		}else if ( wordDes.str == "k" )
		{
		}
		 
		//清除无用的操作数
		while ( OpdStack.Pop( (HE_LPVOID*)&pTmpNode ) == TRUE )
		{
			if ( pTmpNode != NULL )
			{
				pTmpNode->Release();
			}
		}
	}

	pRead->Release();
	HE_DestoryIHERead( pRead );

	while ( OpdStack.Pop( (HE_LPVOID*)&pTmpNode ) == TRUE )
	{
		if ( pTmpNode != NULL )
		{
			pTmpNode->Release();
		}
	}
	//清除无用的subpath
	CHE_GraphicsObject * pTmpGraph = NULL;
	while ( SupPathQueue.Pop( (HE_LPVOID*)&pTmpGraph ) == TRUE )
	{
		if ( pTmpGraph != NULL )
		{
			pTmpGraph->Release();
		}
	}

	return m_arrContentObj.GetCount();
}