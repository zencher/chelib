#include "../../include/pdf/che_pdf_parser.h"
#include <string.h>





class CPtrStackNode
{
public:
	CPtrStackNode();
	~CPtrStackNode();
	
	CHE_PDF_Object * pObj;
	CPtrStackNode * pNext;
};

class CPtrStack
{
public:
	CPtrStack();
	~CPtrStack();
	
	bool IsEmpty();
	
	bool Pop( CHE_PDF_Object ** pObj );
	bool Push( CHE_PDF_Object * pObjRet );
	
	void Clear();
	
private:
	CPtrStackNode * m_pTop;
};

CPtrStackNode::CPtrStackNode()
{
	pObj = NULL;
	pNext = NULL;
}

CPtrStackNode::~CPtrStackNode()
{
//这里不需要销毁子节点的对象，而是由Stack来负责释放
}

CPtrStack::CPtrStack()
{
	m_pTop = NULL;
}

CPtrStack::~CPtrStack()
{
	CPtrStackNode * pTmp = m_pTop;
	while ( pTmp )
	{
		m_pTop = m_pTop->pNext;
		delete pTmp;
		pTmp = m_pTop;
	}
}

bool CPtrStack::IsEmpty()
{
	if ( m_pTop == NULL )
	{
		return true;
	}
	return false;
}

bool CPtrStack::Pop( CHE_PDF_Object ** ppObjRet )
{
	if ( m_pTop == NULL )
	{
		return false;
	}
	
	if ( m_pTop->pObj == NULL )
	{
		return false;
	}else{
		*ppObjRet = m_pTop->pObj;
		
		CPtrStackNode * pTmp = m_pTop;
		m_pTop = m_pTop->pNext;
		delete pTmp;
		pTmp = NULL;
		return true;
	}
}

bool CPtrStack::Push( CHE_PDF_Object * pObj )
{
	if ( m_pTop == NULL )
	{
		m_pTop = new CPtrStackNode;
		m_pTop->pNext = NULL;
		m_pTop->pObj = pObj;
	}else{
		CPtrStackNode * pTmp = new CPtrStackNode;
		pTmp->pNext = m_pTop;
		pTmp->pObj = pObj;
		m_pTop = pTmp;
	}
	return true;
}

void CPtrStack::Clear()
{
	if ( m_pTop )
	{
		CPtrStackNode * pTmp = m_pTop;
		while( pTmp )
		{
			m_pTop = m_pTop->pNext;
			delete pTmp;
			pTmp = m_pTop;
		}
	}
}	

#define HE_PDF_INTEGER_STR	0
#define HE_PDF_FLOAT_STR	1
#define HE_PDF_PURE_STR		2
			
HE_BYTE	HE_PDF_StringCheck( CHE_ByteString & str )
{
	if ( str.GetLength() == 0 )
	{
		return HE_PDF_PURE_STR;
	}else{
		HE_BYTE byteRet = HE_PDF_INTEGER_STR;
		HE_BOOL bBegin = TRUE;
		HE_BOOL bPoint = FALSE;
		HE_BOOL bSign = FALSE;
		for ( HE_DWORD i = 0; i < str.GetLength(); i++ )
		{
			if ( bBegin && i > 0 )
			{
				bBegin = FALSE;
			}
			switch( str[i] )
			{
			case '+':
				if ( bSign == FALSE && bBegin == TRUE )
				{
					bSign = TRUE;
					byteRet = HE_PDF_INTEGER_STR;
				}else{
					return HE_PDF_PURE_STR;
				}
				break;
			case '-':
				if ( bSign == FALSE && bBegin == TRUE )
				{
					bSign = TRUE;
					byteRet = HE_PDF_INTEGER_STR;
				}else{
					return HE_PDF_PURE_STR;
				}
				break;
			case '.':
				if ( bPoint == FALSE )
				{
					byteRet = HE_PDF_FLOAT_STR;
				}else{
					return HE_PDF_PURE_STR;
				}
				break;
			default:
				if ( '0' > str[i] || str[i] > '9' )
				{
					return HE_PDF_PURE_STR;
				}
				break;
			}
		}
		return byteRet;
	}
}

HE_INT32 HE_PDF_StringToInteger( CHE_ByteString & str )
{
	if ( str.GetLength() == 0 )
	{
		return 0;
	}else{
		HE_BYTE byteRet = HE_PDF_PURE_STR;
		HE_BOOL bBegin = TRUE;
		HE_BOOL	bNegative = FALSE;
		HE_INT32 iValue = 0;
		HE_BOOL bPoint = FALSE;
		HE_BOOL bSign = FALSE;
		for ( HE_DWORD i = 0; i < str.GetLength(); i++ )
		{
			if ( bBegin && i > 0 )
			{
				bBegin = FALSE;
			}
			switch( str[i] )
			{
			case '+':
				if ( bSign == FALSE && bBegin == TRUE )
				{
					bSign = TRUE;
					bNegative = FALSE;
					byteRet = HE_PDF_INTEGER_STR;
				}else{
					return 0;
				}
				break;
			case '-':
				if ( bSign == FALSE && bBegin == TRUE )
				{
					bSign = TRUE;
					byteRet = HE_PDF_INTEGER_STR;
					bNegative = TRUE;
				}else{
					return 0;
				}
				break;
			default:
				if ( '0' > str[i] || str[i] > '9' )
				{
					return 0;
				}else{
					iValue = iValue * 10 + ( str[i] - '0' ); 
				}
				break;
			}
		}
		if( bNegative == TRUE )
		{
			return 0 - iValue;
		}else{
			return iValue;
		}
	}
}

HE_FLOAT HE_PDF_StringToFloat( CHE_ByteString & str )
{
	if ( str.GetLength() == 0 )
	{
		return 0;
	}else{
		HE_BYTE byteRet = HE_PDF_PURE_STR;
		HE_BOOL	bNegative = FALSE;
		HE_BOOL bBegin = TRUE;
		HE_DWORD lPointBit = 1;
		HE_FLOAT fValue = 0;
		HE_BOOL bPoint = FALSE;
		HE_BOOL bSign = FALSE;
		for ( HE_DWORD i = 0; i < str.GetLength(); i++ )
		{
			if ( bBegin && i > 0 )
			{
				bBegin = FALSE;
			}
			switch( str[i] )
			{
			case '+':
				if ( bSign == FALSE && bBegin == TRUE )
				{
					bSign = TRUE;
					bNegative = FALSE;
					byteRet = HE_PDF_INTEGER_STR;
				}else{
					return 0;
				}
				break;
			case '-':
				if ( bSign == FALSE && bBegin == TRUE )
				{
					bSign = TRUE;
					byteRet = HE_PDF_INTEGER_STR;
					bNegative = TRUE;
				}else{
					return 0;
				}
				break;
			case '.':
				if ( bPoint == FALSE )
				{
					bPoint = TRUE;
					byteRet = HE_PDF_FLOAT_STR;
				}else{
					return 0;
				}
				break;
			default:
				if ( '0' > str[i] || str[i] > '9' )
				{
					return 0;
				}else{
					if ( bPoint == FALSE )
					{
						fValue = fValue * 10 + ( str[i] - '0' );
					}else{
						fValue = fValue + ( str[i] - '0' ) / ( lPointBit * 10 );
						lPointBit++;
					}
				}
				break;
			}
		}
		if ( bNegative == TRUE )
		{
			return 0 - fValue;
		}else{
			return fValue;
		}
	}
}

CHE_PDF_SyntaxParser::CHE_PDF_SyntaxParser()
{
	m_lFilePos = 0;
	m_pFileAccess = NULL;
	m_lFileSize = 0;
	
	memset( m_WordBuffer, 0, 255 );
	m_lBufferSize = 0;
	m_lBufferPos = 0;
}

CHE_PDF_SyntaxParser::~CHE_PDF_SyntaxParser()
{
	if ( m_pFileAccess )
	{
		m_pFileAccess->Release();
		m_pFileAccess = NULL;
	}
}

HE_BOOL CHE_PDF_SyntaxParser::InitParser( IHE_FileRead* pFileAccess )
{
	if ( pFileAccess )
	{
		m_pFileAccess = pFileAccess;
		m_lFileSize = m_pFileAccess->GetSize();
		m_lFilePos = 0;
		return TRUE;
	}else{
		return FALSE;
	}
}

HE_DWORD CHE_PDF_SyntaxParser::Seek( HE_DWORD bytes )
{
	if ( m_pFileAccess )
	{
		if ( m_lFilePos + bytes <= 0 )
		{
			m_lFilePos+= bytes;
			return bytes;
		}else{
			bytes = m_lFileSize - m_lFilePos;
			m_lFilePos = m_lFileSize;
			return bytes;
		}
	}else{
		return 0;
	}
}

HE_VOID CHE_PDF_SyntaxParser::NextLine()
{
	if ( m_pFileAccess )
	{
		while( m_lFilePos < m_lFileSize )
		{
			HE_BYTE byte = m_pFileAccess->ReadByte( m_lFilePos );
			switch( byte )
			{
			case 0x0D:
				byte = m_pFileAccess->ReadByte( m_lFilePos+1 );
				if ( byte == 0x0A )
				{
					m_lFilePos++;
				}
			case 0x0A:
				m_lFilePos++;
				return;
			default:
				 break;
			}
			m_lFilePos++;
		}
	}	
}

HE_VOID CHE_PDF_SyntaxParser::NextWord()
{
	if ( m_pFileAccess )
	{
		HE_BOOL bNewBegin = FALSE;
		while( m_lFilePos < m_lFileSize )
		{
			HE_BYTE byte = m_pFileAccess->ReadByte( m_lFilePos );
			switch( byte )
			{
			case 0x0D:
				{
					byte = m_pFileAccess->ReadByte( m_lFilePos+1 );
					if ( byte == 0x0A )
					{
						m_lFilePos++;
					}
				}
			case 0x00:
			case 0x09:
			case 0x0A:
			case 0x0C:
			case 0x20:
				bNewBegin = TRUE;
				break;
			default:
				if ( bNewBegin == TRUE )
				{
					return;
				}
				break;
			}
			m_lFilePos++;
		}
	}
}

CHE_ByteString CHE_PDF_SyntaxParser::GetWord()
{
	if ( m_pFileAccess )
	{
		m_byteType = PDFPARSER_WORD_UNKNOWN;
		CHE_ByteString strTmp;
		while ( m_lFilePos < m_lFileSize )
		{
			HE_BYTE byte = m_pFileAccess->ReadByte( m_lFilePos );
			switch ( byte )
			{
			case '%':	//注释，直接移动到下一行
				{
					NextLine();
					continue;
				}
			case '<':	//可能是字典，也可能是十六进制表示的字符串，需要更多判断
				{
					if ( m_lBufferPos > 0 )
					{
						m_WordBuffer[m_lBufferPos] = '\0';
						strTmp = (char const *)(&(m_WordBuffer[0]));
						m_WordBuffer[m_lBufferPos=0] = '\0';
						return strTmp;
					}else{
						byte = m_pFileAccess->ReadByte( m_lFilePos+1 );
						if ( byte == '<' )
						{
							m_lFilePos+=2;
							return "<<";
						}else{
							//字符串处理
							m_byteType = PDFPARSER_WORD_STRING;
							m_lFilePos++;
							//m_WordBuffer[m_lBufferPos++] = '(';

							HE_DWORD tmpValue = 0;
							HE_BYTE	tmpCount = 0;
							while ( m_lFilePos < m_lFileSize )
							{
								byte = m_pFileAccess->ReadByte( m_lFilePos++ );
								tmpCount++;
								switch( byte )
								{
								case '0':
								case '1':
								case '2':
								case '3':
								case '4':
								case '5':
								case '6':
								case '7':
								case '8':
								case '9':
									{
										tmpValue = tmpValue * 16 + (byte - '0');
										break;
									}
								case 'A':
								case 'B':
								case 'C':
								case 'D':
								case 'E':
								case 'F':
									{
										tmpValue = tmpValue * 16 + (byte - 0x31);
										break;
									}
								case 'a':
								case 'b':
								case 'c':
								case 'd':
								case 'e':
								case 'f':
									{
										tmpValue = tmpValue * 16 + (byte - 0x51);
										break;
									}
								default:
									tmpCount--;
									break;
								}
								if ( tmpCount == 2 )
								{
									m_WordBuffer[m_lBufferPos++] = (HE_BYTE)tmpValue;
									tmpCount = 0;
									tmpValue = 0;
								}
								if ( byte == '>' )
								{
									if ( tmpCount == 1 )
									{
										tmpValue = tmpValue * 16;
										m_WordBuffer[m_lBufferPos++] = (HE_BYTE)tmpValue;
									}
									//m_WordBuffer[m_lBufferPos++] = ')';
									break;
								}
							}
							m_WordBuffer[m_lBufferPos] = '\0';
							strTmp = (char const *)(&(m_WordBuffer[0]));
							m_WordBuffer[m_lBufferPos=0] = '\0';
							return strTmp;
						}
					}
					break;
				}
			case '>':	//字典结束，因为字符串的已经在其他位置处理了，这里不可能是字符串了
				{
					if ( m_lBufferPos > 0 )
					{
						m_WordBuffer[m_lBufferPos] = '\0';
						strTmp = (char const *)(&(m_WordBuffer[0]));
						m_WordBuffer[m_lBufferPos=0] = '\0';
						return strTmp;
					}else{
						byte = m_pFileAccess->ReadByte( m_lFilePos+1 );
						if ( byte == '>' )
						{
							m_lFilePos+=2;
							return ">>";
						}
					}
					break;
				}
			case '[':	//数组
			case ']':
				{
					if ( m_lBufferPos > 0 )
					{
						m_WordBuffer[m_lBufferPos] = '\0';
						strTmp = (char const *)(&(m_WordBuffer[0]));
						m_WordBuffer[m_lBufferPos=0] = '\0';
						return strTmp;
					}else{
						m_lFilePos++;
						return byte;
					}
					break;
				}
			case '/':	//名称
				{
					if ( m_lBufferPos > 0 )
					{
						m_WordBuffer[m_lBufferPos] = '\0';
						strTmp = (char const *)(&(m_WordBuffer[0]));
						m_WordBuffer[++m_lBufferPos=0] = '\0';
						return strTmp;
					}else{
						m_byteType = PDFPARSER_WORD_NAME;
						//m_WordBuffer[m_lBufferPos=0] = '/';
						//m_lBufferPos++;
						m_lFilePos++;
						while ( m_lFilePos < m_lFileSize )
						{
							byte = m_pFileAccess->ReadByte( m_lFilePos );
							if ( byte == '#' )
							{
								HE_DWORD tmpValue = 0;
								byte = m_pFileAccess->ReadByte( ++m_lFilePos );
								switch( byte )
								{
								case '0':
								case '1':
								case '2':
								case '3':
								case '4':
								case '5':
								case '6':
								case '7':
								case '8':
								case '9':
									{
										tmpValue = tmpValue * 16 + (byte - '0');
										break;
									}
								case 'A':
								case 'B':
								case 'C':
								case 'D':
								case 'E':
								case 'F':
									{
										tmpValue = tmpValue * 16 + (byte - 0x31);
										break;
									}
								case 'a':
								case 'b':
								case 'c':
								case 'd':
								case 'e':
								case 'f':
									{
										tmpValue = tmpValue * 16 + (byte - 0x51);
										break;
									}
								default:
									break;
								}
								byte = m_pFileAccess->ReadByte( ++m_lFilePos );
								switch( byte )
								{
								case '0':
								case '1':
								case '2':
								case '3':
								case '4':
								case '5':
								case '6':
								case '7':
								case '8':
								case '9':
									{
										tmpValue = tmpValue * 16 + (byte - '0');
										break;
									}
								case 'A':
								case 'B':
								case 'C':
								case 'D':
								case 'E':
								case 'F':
									{
										tmpValue = tmpValue * 16 + (byte - 0x31);
										break;
									}
								case 'a':
								case 'b':
								case 'c':
								case 'd':
								case 'e':
								case 'f':
									{
										tmpValue = tmpValue * 16 + (byte - 0x51);
										break;
									}
								default:
									break;
								}
								m_WordBuffer[m_lBufferPos++] = (HE_BYTE)tmpValue;
								m_lFilePos++;
								continue;
							}
							switch ( byte )
							{
							case 0x00:
							case 0x09:
							case 0x0A:
							case 0x0C:
							case 0x0D:
							case 0x20:
							case '/':
							case '(':
							case ')':
							case '[':
							case ']':
							case '<':
							case '>':
								break;
							default:
								m_WordBuffer[m_lBufferPos++] = byte;
								m_lFilePos++;
								continue;
							}
							break;
						}
						if ( m_lBufferPos > 0 )
						{
							m_WordBuffer[m_lBufferPos] = '\0';
							strTmp = (char const *)(&(m_WordBuffer[0]));
							m_WordBuffer[++m_lBufferPos=0] = '\0';
							return strTmp;
						}
					}
					break;
				}
			case '(':	//字符串
				{
					if ( m_lBufferPos > 0 )
					{
						m_WordBuffer[m_lBufferPos] = '\0';
						strTmp = (char const *)(&(m_WordBuffer[0]));
						m_WordBuffer[m_lBufferPos=0] = '\0';
						return strTmp;
					}else{
						m_byteType = PDFPARSER_WORD_STRING;
						//m_WordBuffer[m_lBufferPos++] = '(';
						HE_DWORD lCount = 1;
						m_lFilePos++;
						while ( m_lFilePos < m_lFileSize )
						{
							byte = m_pFileAccess->ReadByte( m_lFilePos );
							if ( byte == '\\' )
							{
								byte = m_pFileAccess->ReadByte( ++m_lFilePos );
								switch ( byte )
								{
								case 0x0D:
									{
										byte = m_pFileAccess->ReadByte( m_lFilePos+1 );
										if ( byte == 0x0A )
										{
											m_lFilePos+=2;
										}
									}
								case 0x0A:
									continue;
								case 'n':
									m_WordBuffer[m_lBufferPos++] = 0x0A;
									m_lFilePos++;
									continue;
								case 'r':
									m_WordBuffer[m_lBufferPos++] = 0x0D;
									m_lFilePos++;
									continue;
								case 't':
									m_WordBuffer[m_lBufferPos++] = 0x09;
									m_lFilePos++;
									continue;
								case 'b':
									m_WordBuffer[m_lBufferPos++] = 0x20;
									m_lFilePos++;
									continue;
								case 'f':
									m_WordBuffer[m_lBufferPos++] = 0x0C;
									m_lFilePos++;
									continue;
								case '(':
									m_WordBuffer[m_lBufferPos++] = '(';
									m_lFilePos++;
									continue;
								case ')':
									m_WordBuffer[m_lBufferPos++] = ')';
									m_lFilePos++;
									continue;
								case '\\':
									m_WordBuffer[m_lBufferPos++] = '\\';
									m_lFilePos++;
									continue;
								case '0':
								case '1':
								case '2':
								case '3':
								case '4':
								case '5':
								case '6':
								case '7':
								case '8':
								case '9':
									{
										HE_DWORD tmpValue = byte - '0';
										HE_BYTE tmpByte = 0;
										HE_BYTE tmpCount = 1;
										while ( true )
										{
											tmpByte = m_pFileAccess->ReadByte( m_lFilePos+1 );
											if ( tmpByte >= '0' && tmpByte <= '7' )
											{
												tmpValue = tmpValue * 8 + (tmpByte-'0');
												m_lFilePos++;
												tmpCount++;
												if ( tmpCount == 3 )
												{
													m_WordBuffer[m_lBufferPos++] = (HE_BYTE)tmpValue;
													tmpCount = 0;
												}
											}else{
												break;
											}
										}
										if ( tmpCount != 0 )
										{
											m_WordBuffer[m_lBufferPos++] = (HE_BYTE)tmpValue;
										}
										continue;
									}
								default:
									continue;
								}
							}
							if ( byte == ')' )
							{
								lCount--;
							}else if ( byte == '(' )
							{
								lCount++;
							}
							if ( lCount == 0 )
							{
								break;
							}else{
								m_WordBuffer[m_lBufferPos++] = byte;
								m_lFilePos++;
							}
						}
						m_WordBuffer[m_lBufferPos] = '\0';
						strTmp = (char const *)(&(m_WordBuffer[0]));
						m_WordBuffer[m_lBufferPos=0] = '\0';
						return strTmp;
					}
					break;
				}
			case 0x0D:
				{
					byte = m_pFileAccess->ReadByte( m_lFilePos+1 );
					if ( byte == 0x0A )
					{
						m_lFilePos+=2;
					}
				}
			case 0x00:
			case 0x09:
			case 0x0A:
			case 0x0C:
			case 0x20:
				{
					if ( m_lBufferPos > 0 )
					{
						m_WordBuffer[m_lBufferPos] = '\0';
						strTmp = (char const *)(&(m_WordBuffer[0]));
						m_WordBuffer[m_lBufferPos=0] = '\0';
						return strTmp;
					}
					break;
				}
			default:
				m_WordBuffer[m_lBufferPos++] = byte;
			}
			m_lFilePos++;
		}

		if ( m_lBufferPos != 0 )
		{
			strTmp = (char const *)(&(m_WordBuffer[0]));
			m_WordBuffer[m_lBufferPos=0] = '\0';
			return strTmp;
		}else{
			return "";
		}
	}else{
		return "";
	}
}

CHE_PDF_Dictionary * CHE_PDF_SyntaxParser::GetDictionary()
{
	CHE_ByteString	str;
	HE_BYTE			type = PDFPARSER_WORD_UNKNOWN;
	str = GetWord();
	if ( str != "<<" )
	{
		return NULL;
	}

	HE_BOOL bKey = TRUE;
	HE_BOOL bOver = FALSE;
	CHE_ByteString key;
	CPtrStack stack;
	CHE_PDF_Object * pCurObj = CHE_PDF_Dictionary::Create();

	while ( true )
	{
		if ( bKey )
		{
			key = GetWord();
			//type = GetType();
			if ( key == ">>" )
			{
				bOver = TRUE;
			}else{
				bKey = FALSE;
			}
		}
		if ( bKey == FALSE )
		{
			str = GetWord();
			type = GetType();
			if ( type == PDFPARSER_WORD_STRING )
			{
				if ( pCurObj->GetType() == PDFOBJ_DICTIONARY )
				{
					((CHE_PDF_Dictionary*)pCurObj)->SetAtString( key, str );
				}else{
					CHE_PDF_String * pTmp = CHE_PDF_String::Create( str );
					((CHE_PDF_Array*)pCurObj)->Append( pTmp );
				}
				continue;
			}else if ( type == PDFPARSER_WORD_NAME ){

				if ( pCurObj->GetType() == PDFOBJ_DICTIONARY )
				{
					((CHE_PDF_Dictionary*)pCurObj)->SetAtName( key, str );
				}else{
					CHE_PDF_Name * pTmp = CHE_PDF_Name::Create( str );
					((CHE_PDF_Array*)pCurObj)->Append( pTmp );
				}
				continue;
			}
			switch ( str[0] )
			{
// 			case '(':
// 				if ( pCurObj->GetType() == PDFOBJ_DICTIONARY )
// 				{
// 					((CHE_PDF_Dictionary*)pCurObj)->SetAtString( key, str );
// 				}else{
// 					CHE_PDF_String * pTmp = CHE_PDF_String::Create( str );
// 					((CHE_PDF_Array*)pCurObj)->Append( pTmp );
// 				}
// 				break;
// 			case '/':
// 				if ( pCurObj->GetType() == PDFOBJ_DICTIONARY )
// 				{
// 					((CHE_PDF_Dictionary*)pCurObj)->SetAtName( key, str );
// 				}else{
// 					CHE_PDF_Name * pTmp = CHE_PDF_Name::Create( str );
// 					((CHE_PDF_Array*)pCurObj)->Append( pTmp );
// 				}
// 				break;
			case '[':
				{
					stack.Push( pCurObj );
					pCurObj = CHE_PDF_Array::Create();
					break;
				}
			case ']':
				{
					CHE_PDF_Object * pTmp = pCurObj;
					stack.Pop( &pCurObj );
					if ( pCurObj->GetType() == PDFOBJ_DICTIONARY )
					{
						((CHE_PDF_Dictionary*)pCurObj)->SetAtArray( key, (CHE_PDF_Array*)pTmp );
					}else{
						((CHE_PDF_Array*)pCurObj)->Append( pTmp );
					}
					break;
				}
			case '<':
				{
					stack.Push( pCurObj );
					pCurObj = CHE_PDF_Dictionary::Create();
					break;
				}
			case '>':
				{
					CHE_PDF_Object * pTmp = pCurObj;
					stack.Pop( &pCurObj );
					if ( pCurObj->GetType() == PDFOBJ_DICTIONARY )
					{
						((CHE_PDF_Dictionary*)pCurObj)->SetAtDictionary( key, (CHE_PDF_Dictionary*)pTmp );
					}else{
						((CHE_PDF_Array*)pCurObj)->Append( pTmp );
					}
				break;
				}
			default:
				{
					HE_DWORD objNum = 0, genNum = 0;
					if ( HE_PDF_StringCheck( str ) == HE_PDF_INTEGER_STR )
					{
						objNum = HE_PDF_StringToInteger( str );
					}else if ( HE_PDF_StringCheck( str ) == HE_PDF_FLOAT_STR )
					{
						if ( pCurObj->GetType() == PDFOBJ_DICTIONARY )
						{
							((CHE_PDF_Dictionary*)pCurObj)->SetAtFloatNumber( key, HE_PDF_StringToFloat( str ) );
						}else{
							CHE_PDF_Number * pTmp = CHE_PDF_Number::Create( HE_PDF_StringToFloat( str ) );
							((CHE_PDF_Array*)pCurObj)->Append( pTmp );
						}
						break;
					}else{
						if ( str == "false" )
						{
							if ( pCurObj->GetType() == PDFOBJ_DICTIONARY )
							{
								((CHE_PDF_Dictionary*)pCurObj)->SetAtBoolean( key, false );
							}else{
								CHE_PDF_Boolean * pTmp = CHE_PDF_Boolean::Create( FALSE );
								((CHE_PDF_Array*)pCurObj)->Append( pTmp );
							}
						}else if ( str == "true" )
						{
							if ( pCurObj->GetType() == PDFOBJ_DICTIONARY )
							{
								((CHE_PDF_Dictionary*)pCurObj)->SetAtBoolean( key, true );
							}else{
								CHE_PDF_Boolean * pTmp = CHE_PDF_Boolean::Create( TRUE );
								((CHE_PDF_Array*)pCurObj)->Append( pTmp );
							}
						}else{
							return FALSE;
						}
					}
					HE_DWORD lPos = GetPos();
					str = GetWord();
					type = GetType();
					if ( type == PDFPARSER_WORD_NAME || str[0] == '>' )
					{
						SetPos( lPos );
						if ( pCurObj->GetType() == PDFOBJ_DICTIONARY )
						{
							((CHE_PDF_Dictionary*)pCurObj)->SetAtInteger( key, objNum );
						}else{
							CHE_PDF_Number * pTmp = CHE_PDF_Number::Create( (HE_INT32)objNum );
							((CHE_PDF_Array*)pCurObj)->Append( pTmp );
						}
						break;
					}else{
						if ( HE_PDF_StringCheck( str ) == HE_PDF_INTEGER_STR )
						{
							genNum = HE_PDF_StringToInteger( str );
						}else{
							return FALSE;
						}
					}
					str = GetWord();
					if ( str == "R" )
					{
						if ( pCurObj->GetType() == PDFOBJ_DICTIONARY )
						{
							((CHE_PDF_Dictionary*)pCurObj)->SetAtReference( key, objNum );
						}else{
							CHE_PDF_Reference * pTmp = CHE_PDF_Reference::Create( objNum );
							((CHE_PDF_Array*)pCurObj)->Append( pTmp );
						}
					}else{
						return NULL;
					}
					break;
				}
			}
		}
		if ( stack.IsEmpty() == true )
		{
			bKey = TRUE;
		}
		if ( bOver == TRUE )
		{
			break;
		}
	}
	return (CHE_PDF_Dictionary*)pCurObj;
}

CHE_PDF_Parser::CHE_PDF_Parser()
{
	m_pTrailerDict = NULL;
	m_pIHE_FileRead = NULL;
	m_lstartxref = 0;
}

HE_BOOL CHE_PDF_Parser::StartParse( IHE_FileRead * file )
{
	if ( file == NULL )
	{
		return false;
	}else{
		m_pIHE_FileRead = file;
		m_sParser.InitParser( file );
		return true;
	}
}

HE_VOID CHE_PDF_Parser::CloseParser()
{
	if ( m_pIHE_FileRead )
	{
		m_pIHE_FileRead->Release();
		m_pIHE_FileRead = NULL;
	}
}

HE_DWORD CHE_PDF_Parser::GetFileSize()
{
	if ( m_pIHE_FileRead )
	{
		return m_pIHE_FileRead->GetSize();
	}else{
		return 0;
	}
}

HE_PDF_VERSION CHE_PDF_Parser::GetPDFVersion()
{
	char buffer[8];
	memset( buffer, 0, 8 );
	if ( m_pIHE_FileRead->ReadBlock( buffer, 0, 8 ) )
	{
		if ( buffer[0] == '%'
			 && buffer[1] == 'P'
			 && buffer[2] == 'D'
			 && buffer[3] == 'F'
			 && buffer[4] == '-'
			 && buffer[5] == '1'
			 && buffer[6] == '.' )
		{
			if (  '0' <= buffer[7] && buffer[7] <= '7'  )
			{
				return (HE_PDF_VERSION)(10 + buffer[7] - '0');
			}else{
				return PDF_VERSION_UNKNOWN;
			}	
		}else{
			return PDF_VERSION_UNKNOWN;
		}
	}else{
		return PDF_VERSION_UNKNOWN;
	}
}

HE_DWORD CHE_PDF_Parser::GetStartxrefOffset( HE_DWORD range )
{
	if ( m_pIHE_FileRead )
	{
		unsigned char * pBuffer = NULL;
		if ( range >= m_pIHE_FileRead->GetSize() )
		{
			pBuffer = new unsigned char[range];
			m_pIHE_FileRead->ReadBlock( pBuffer, 0, m_pIHE_FileRead->GetSize() );
		}else{
			pBuffer = new unsigned char[range];
			m_pIHE_FileRead->ReadBlock( pBuffer, m_pIHE_FileRead->GetSize()-range, range );
		}
		
		HE_DWORD iStartXref = 0;
		for ( unsigned int m = range; m >= 8; m-- )
		{
			if (	pBuffer[m] == 'f'
				&& pBuffer[m-1] == 'e'
				&& pBuffer[m-2] == 'r'
				&& pBuffer[m-3] == 'x'
				&& pBuffer[m-4] == 't'
				&& pBuffer[m-5] == 'r'
				&& pBuffer[m-6] == 'a'
				&& pBuffer[m-7] == 't'
				&& pBuffer[m-8] == 's' )
			{
				HE_DWORD iBufferIndex = m+1;
				while ( pBuffer[iBufferIndex] < '0' || pBuffer[iBufferIndex] > '9' )
				{
					iBufferIndex++;
				}
				
				while ( pBuffer[iBufferIndex] >= '0' && pBuffer[iBufferIndex] <= '9' )
				{
					iStartXref *= 10;
					iStartXref += pBuffer[iBufferIndex] - '0';
					iBufferIndex++;
				}
				break;
			}
		}
		m_lstartxref = iStartXref;
		return iStartXref;
	}
	return 0;
}

HE_BOOL CHE_PDF_Parser::GetXRefTable()
{
	if ( m_pIHE_FileRead == NULL )
	{
		return FALSE;
	}
	if ( m_lstartxref == 0 )
	{
		GetStartxrefOffset( 512 );
		if ( m_lstartxref == 0 )
		{
			//需要分析整个文件来获取对象信息
			return FALSE;
		}
	}else{
		m_sParser.SetPos( m_lstartxref );
		CHE_ByteString str;
		str = m_sParser.GetWord();
		if ( str != "xref" )
		{
			return FALSE;
		}else{
			//开始解析xref段头
			while ( true )
			{
				HE_DWORD lBeginNum = 0, lCount = 0; 
				str = m_sParser.GetWord();
				if ( str == "trailer" )
				{
					m_pTrailerDict = m_sParser.GetDictionary();
					return TRUE;
				}
				if( HE_PDF_StringCheck( str ) == HE_PDF_INTEGER_STR )
				{
					lBeginNum = HE_PDF_StringToInteger( str );
				}else{
					return FALSE;
				}
				str = m_sParser.GetWord();
				if( HE_PDF_StringCheck( str ) == HE_PDF_INTEGER_STR )
				{
					lCount = HE_PDF_StringToInteger( str );
				}else{
					return FALSE;
				}

				HE_DWORD objNum = 0, objGenNum = 0, objOffset;
				for ( HE_DWORD i = lBeginNum; i < lBeginNum + lCount; i++ )
				{
					str = m_sParser.GetWord();
					if( HE_PDF_StringCheck( str ) == HE_PDF_INTEGER_STR )
					{
						objOffset = HE_PDF_StringToInteger( str );
					}else{
						return FALSE;
					}
					str = m_sParser.GetWord();
					if( HE_PDF_StringCheck( str ) == HE_PDF_INTEGER_STR )
					{
						objGenNum = HE_PDF_StringToInteger( str );
					}else{
						return FALSE;
					}
					str = m_sParser.GetWord();
					if ( str == 'f' )
					{
						m_xrefTable.Append( objOffset, i, objGenNum, 'f' );
					}else{
						m_xrefTable.Append( objOffset, i, objGenNum, 'n' );
					}
				}
			}
		}
		return FALSE;
	}
	return FALSE;
}