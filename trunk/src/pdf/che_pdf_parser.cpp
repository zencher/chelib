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
	
	void Clear();
	bool IsEmpty();
	bool Pop( CHE_PDF_Object ** pObj );
	bool Push( CHE_PDF_Object * pObjRet );

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
 			
// HE_BYTE	HE_PDF_StringCheck( CHE_ByteString & str )
// {
// 	if ( str.GetLength() == 0 )
// 	{
// 		return HE_PDF_PURE_STR;
// 	}else{
// 		HE_BYTE byteRet = HE_PDF_INTEGER_STR;
// 		HE_BOOL bBegin = TRUE;
// 		HE_BOOL bPoint = FALSE;
// 		HE_BOOL bSign = FALSE;
// 		for ( HE_DWORD i = 0; i < str.GetLength(); i++ )
// 		{
// 			if ( bBegin && i > 0 )
// 			{
// 				bBegin = FALSE;
// 			}
// 			switch( str[i] )
// 			{
// 			case '+':
// 				if ( bSign == FALSE && bBegin == TRUE )
// 				{
// 					bSign = TRUE;
// 					byteRet = HE_PDF_INTEGER_STR;
// 				}else{
// 					return HE_PDF_PURE_STR;
// 				}
// 				break;
// 			case '-':
// 				if ( bSign == FALSE && bBegin == TRUE )
// 				{
// 					bSign = TRUE;
// 					byteRet = HE_PDF_INTEGER_STR;
// 				}else{
// 					return HE_PDF_PURE_STR;
// 				}
// 				break;
// 			case '.':
// 				if ( bPoint == FALSE )
// 				{
// 					byteRet = HE_PDF_FLOAT_STR;
// 				}else{
// 					return HE_PDF_PURE_STR;
// 				}
// 				break;
// 			default:
// 				if ( '0' > str[i] || str[i] > '9' )
// 				{
// 					return HE_PDF_PURE_STR;
// 				}
// 				break;
// 			}
// 		}
// 		return byteRet;
// 	}
// }

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

	m_bBegin = TRUE;
	m_bPoint = FALSE;
	m_bSign = FALSE;
	
	m_byteType = PDFPARSER_WORD_UNKNOWN;
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

HE_VOID CHE_PDF_SyntaxParser::SeekToNextLine()
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

HE_VOID CHE_PDF_SyntaxParser::SeekToNextWord()
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
		HE_BYTE byte = 0;
		while ( m_lFilePos < m_lFileSize )
		{
			byte = m_pFileAccess->ReadByte( m_lFilePos );
			switch ( byte )
			{
			case '%':	//注释，直接移动到下一行
				{
					SeekToNextLine();
					continue;
				}
			case '<':	//可能是字典，也可能是十六进制表示的字符串，需要更多判断
				{
					if ( m_lBufferPos > 0 )
					{
						//返回已经获取的词
						return SubmitBufferStr();
					}else{
						byte = m_pFileAccess->ReadByte( m_lFilePos+1 );
						if ( byte == '<' )
						{
							//返回表明字典开始的词
							m_lFilePos+=2;
							m_byteType = PDFPARSER_WORD_DICT_B;
							return "<<";
						}else{
							//字符串处理
							m_byteType = PDFPARSER_WORD_STRING;
							m_lFilePos++;
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
									break;
								}
							}
							return SubmitBufferStr();
						}
					}
					break;
				}
			case '>':	//字典结束，因为字符串的已经在其他位置处理了，这里不可能是字符串了
				{
					if ( m_lBufferPos > 0 )
					{
						return SubmitBufferStr();
					}else{
						byte = m_pFileAccess->ReadByte( m_lFilePos+1 );
						if ( byte == '>' )
						{
							m_byteType = PDFPARSER_WORD_DICT_E;
							m_lFilePos+=2;
							return ">>";
						}
					}
					break;
				}
			case '[':	//数组
				{
					if ( m_lBufferPos > 0 )
					{
						return SubmitBufferStr();
					}else{
						m_lFilePos++;
						m_byteType = PDFPARSER_WORD_ARRAY_B;
						return byte;
					}
					break;
				}
			case ']':
				{
					if ( m_lBufferPos > 0 )
					{
						return SubmitBufferStr();
					}else{
						m_lFilePos++;
						m_byteType = PDFPARSER_WORD_ARRAY_E;
						return byte;
					}
					break;
				}
			case '/':	//名称
				{
					if ( m_lBufferPos > 0 )
					{
						return SubmitBufferStr();
					}else{
						m_byteType = PDFPARSER_WORD_NAME;
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
							return SubmitBufferStr();
						}
					}
					break;
				}
			case '(':	//字符串
				{
					if ( m_lBufferPos > 0 )
					{
						return SubmitBufferStr();
					}else{
						m_byteType = PDFPARSER_WORD_STRING;
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
						return SubmitBufferStr();
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
						return SubmitBufferStr();
					}
					break;
				}
			case '+':
			case '-':
				{
					if ( m_bBegin && m_bSign == FALSE )
					{
						m_byteType = PDFPARSER_WORD_INTEGER;
					}else{
						m_byteType = PDFPARSER_WORD_UNKNOWN;
					}
					m_WordBuffer[m_lBufferPos++] = byte;
					break;
				}
			case '.':
				{
					if ( m_bPoint == FALSE )
					{
						m_bPoint = TRUE;
						m_byteType = PDFPARSER_WORD_FLOAT;
					}else{
						m_byteType = PDFPARSER_WORD_UNKNOWN;
					}
					m_WordBuffer[m_lBufferPos++] = byte;
					break;
				}
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
					if ( m_bBegin == TRUE )
					{
						m_bBegin = FALSE;
						m_byteType = PDFPARSER_WORD_INTEGER;
					}
					m_WordBuffer[m_lBufferPos++] = byte;
					break;
				}
			default:
				{
					if ( m_bBegin )
					{
						m_bBegin = FALSE;
					}
					if ( m_byteType != PDFPARSER_WORD_UNKNOWN )
					{
						m_byteType = PDFPARSER_WORD_UNKNOWN;
					}
					m_WordBuffer[m_lBufferPos++] = byte;
					break;
				}
			}
			m_lFilePos++;
		}

		if ( m_lBufferPos != 0 )
		{
			return SubmitBufferStr();
		}else{
			return "";
		}
	}else{
		return "";
	}
}

CHE_PDF_Array *	CHE_PDF_SyntaxParser::GetArray()
{
	if ( m_pFileAccess == NULL )
	{
		return NULL;
	}

	CHE_ByteString	str;
	HE_BYTE			type = PDFPARSER_WORD_UNKNOWN;
	str = GetWord();
	type = GetType();
	if ( type != PDFPARSER_WORD_ARRAY_B )
	{
		return NULL;
	}

	HE_BOOL bKey = FALSE;
	HE_BOOL bOver = FALSE;
	CHE_ByteString key;
	CPtrStack stack;
	CPtrStack stackName;
	CHE_PDF_Object * pCurObj = CHE_PDF_Array::Create();

	while ( bOver == FALSE )
	{
		if ( bKey )
		{
			key = GetWord();
			type = GetType();
			if ( type == PDFPARSER_WORD_DICT_E  )
			{
				if ( stack.IsEmpty() == FALSE )
				{
					CHE_PDF_Dictionary * pDict = (CHE_PDF_Dictionary*)pCurObj;
					stack.Pop( &pCurObj );
					if ( pCurObj->GetType() == PDFOBJ_ARRAY )
					{
						((CHE_PDF_Array*)pCurObj)->Append( pDict );
					}else if ( pCurObj->GetType() == PDFOBJ_DICTIONARY )
					{
						CHE_ByteString * pTmpStr = NULL;
						stackName.Pop(  (CHE_PDF_Object**)&pTmpStr );
						key = *pTmpStr;
						delete pTmpStr;
						((CHE_PDF_Dictionary*)pCurObj)->SetAtDictionary( key, pDict );
					}
				}
				if ( pCurObj->GetType() == PDFOBJ_ARRAY )
				{
					bKey = FALSE;
				}else{
					bKey = TRUE;
				}
				continue;
			}
		}
		if ( bKey == FALSE )
		{
			str = GetWord();
			type = GetType();
			if ( type == PDFPARSER_WORD_INTEGER )	//整数和引用
			{
				HE_DWORD objNum = 0, genNum = 0;
				objNum = HE_PDF_StringToInteger( str );

				HE_DWORD lPos = GetPos();
				str = GetWord();
				type = GetType();
				if ( type == PDFPARSER_WORD_INTEGER )
				{
					genNum = HE_PDF_StringToInteger( str );
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
						if ( pCurObj->GetType() == PDFOBJ_DICTIONARY )
						{
							return NULL;
						}else{
							CHE_PDF_Number * pTmp = CHE_PDF_Number::Create( (HE_INT32)objNum );
							((CHE_PDF_Array*)pCurObj)->Append( pTmp );
							pTmp = CHE_PDF_Number::Create( (HE_INT32)genNum );
							((CHE_PDF_Array*)pCurObj)->Append( pTmp );
						}
					}
				}else{
					SetPos( lPos );
					if ( pCurObj->GetType() == PDFOBJ_DICTIONARY )
					{
						((CHE_PDF_Dictionary*)pCurObj)->SetAtInteger( key, objNum );
					}else{
						CHE_PDF_Number * pTmp = CHE_PDF_Number::Create( (HE_INT32)objNum );
						((CHE_PDF_Array*)pCurObj)->Append( pTmp );
					}
				}
			}else if ( type == PDFPARSER_WORD_FLOAT )	//浮点
			{
				if ( pCurObj->GetType() == PDFOBJ_DICTIONARY )
				{
					((CHE_PDF_Dictionary*)pCurObj)->SetAtFloatNumber( key, HE_PDF_StringToFloat( str ) );
				}else{
					CHE_PDF_Number * pTmp = CHE_PDF_Number::Create( HE_PDF_StringToFloat( str ) );
					((CHE_PDF_Array*)pCurObj)->Append( pTmp );
				}
			}else if ( type == PDFPARSER_WORD_STRING )	//字符串
			{
				if ( pCurObj->GetType() == PDFOBJ_DICTIONARY )
				{
					((CHE_PDF_Dictionary*)pCurObj)->SetAtString( key, str );
				}else{
					CHE_PDF_String * pTmp = CHE_PDF_String::Create( str );
					((CHE_PDF_Array*)pCurObj)->Append( pTmp );
				}
			}else if ( type == PDFPARSER_WORD_NAME )	//名称
			{
				if ( pCurObj->GetType() == PDFOBJ_DICTIONARY )
				{
					((CHE_PDF_Dictionary*)pCurObj)->SetAtName( key, str );
				}else{
					CHE_PDF_Name * pTmp = CHE_PDF_Name::Create( str );
					((CHE_PDF_Array*)pCurObj)->Append( pTmp );
				}
			}else if ( type == PDFPARSER_WORD_ARRAY_B )	//数组
			{
				stack.Push( pCurObj );
				pCurObj = CHE_PDF_Array::Create();
			}else if ( type == PDFPARSER_WORD_ARRAY_E )	//数组
			{
				if ( stack.IsEmpty() == FALSE )
				{
					CHE_PDF_Object * pTmp = pCurObj;
					stack.Pop( &pCurObj );
					if ( pCurObj->GetType() == PDFOBJ_DICTIONARY )
					{
						((CHE_PDF_Dictionary*)pCurObj)->SetAtArray( key, (CHE_PDF_Array*)pTmp );
					}else{
						((CHE_PDF_Array*)pCurObj)->Append( pTmp );
					}
				}else{
					return (CHE_PDF_Array*)pCurObj;
				}
			}else if ( type == PDFPARSER_WORD_DICT_B )	//字典
			{
				if ( pCurObj->GetType() == PDFOBJ_DICTIONARY )
				{
					CHE_ByteString * pStr = new CHE_ByteString( key );
					stackName.Push( (CHE_PDF_Object*)pStr );
				}
				stack.Push( pCurObj );
				pCurObj = CHE_PDF_Dictionary::Create();
			}else if ( type == PDFPARSER_WORD_DICT_E )	//字典	只出现在array的情况
			{
				CHE_PDF_Object * pTmp = pCurObj;
				stack.Pop( &pCurObj );
				((CHE_PDF_Array*)pCurObj)->Append( pTmp );
			}else{	//布尔和其他
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
					return NULL;
				}
			}
		}
		if ( pCurObj->GetType() == PDFOBJ_DICTIONARY )
		{
			bKey = TRUE;
		}else{
			bKey = FALSE;
		}
	}
	return (CHE_PDF_Array*)pCurObj;
}

CHE_PDF_Dictionary * CHE_PDF_SyntaxParser::GetDictionary()
{
	if ( m_pFileAccess == NULL )
	{
		return NULL;
	}

	CHE_ByteString	str;
	HE_BYTE			type = PDFPARSER_WORD_UNKNOWN;
	str = GetWord();
	type = GetType();
	if ( type != PDFPARSER_WORD_DICT_B )
	{
		return NULL;
	}

	HE_BOOL bKey = TRUE;
	HE_BOOL bOver = FALSE;
	CHE_ByteString key;
	CPtrStack stack;
	CPtrStack stackName;
	CHE_PDF_Object * pCurObj = CHE_PDF_Dictionary::Create();

	while ( bOver == FALSE )
	{
		if ( bKey )
		{
			key = GetWord();
			type = GetType();
			if ( type == PDFPARSER_WORD_NAME )
			{
				bKey = FALSE;
			}else if ( type == PDFPARSER_WORD_DICT_E  )
			{
				if ( stack.IsEmpty() == FALSE )
				{
					CHE_PDF_Dictionary * pDict = (CHE_PDF_Dictionary*)pCurObj;
					stack.Pop( &pCurObj );
					if ( pCurObj->GetType() == PDFOBJ_ARRAY )
					{
						((CHE_PDF_Array*)pCurObj)->Append( pDict );
					}else if ( pCurObj->GetType() == PDFOBJ_DICTIONARY )
					{
						CHE_ByteString * pTmpStr = NULL;
						stackName.Pop(  (CHE_PDF_Object**)&pTmpStr );
						key = *pTmpStr;
						delete pTmpStr;
						((CHE_PDF_Dictionary*)pCurObj)->SetAtDictionary( key, pDict );
					}
				}else{
					return (CHE_PDF_Dictionary*)pCurObj;
				}
				if ( pCurObj->GetType() == PDFOBJ_ARRAY )
				{
					bKey = FALSE;
				}else{
					bKey = TRUE;
				}
				continue;
			}
		}
		if ( bKey == FALSE )
		{
			str = GetWord();
			type = GetType();
			if ( type == PDFPARSER_WORD_INTEGER )	//整数和引用
			{
				HE_DWORD objNum = 0, genNum = 0;
				objNum = HE_PDF_StringToInteger( str );

				HE_DWORD lPos = GetPos();
				str = GetWord();
				type = GetType();
				if ( type == PDFPARSER_WORD_INTEGER )
				{
					genNum = HE_PDF_StringToInteger( str );
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
						if ( pCurObj->GetType() == PDFOBJ_DICTIONARY )
						{
							return NULL;
						}else{
							CHE_PDF_Number * pTmp = CHE_PDF_Number::Create( (HE_INT32)objNum );
							((CHE_PDF_Array*)pCurObj)->Append( pTmp );
							pTmp = CHE_PDF_Number::Create( (HE_INT32)genNum );
							((CHE_PDF_Array*)pCurObj)->Append( pTmp );
						}
					}
				}else{
					SetPos( lPos );
					if ( pCurObj->GetType() == PDFOBJ_DICTIONARY )
					{
						((CHE_PDF_Dictionary*)pCurObj)->SetAtInteger( key, objNum );
					}else{
						CHE_PDF_Number * pTmp = CHE_PDF_Number::Create( (HE_INT32)objNum );
						((CHE_PDF_Array*)pCurObj)->Append( pTmp );
					}
				}
			}else if ( type == PDFPARSER_WORD_FLOAT )	//浮点
			{
				if ( pCurObj->GetType() == PDFOBJ_DICTIONARY )
				{
					((CHE_PDF_Dictionary*)pCurObj)->SetAtFloatNumber( key, HE_PDF_StringToFloat( str ) );
				}else{
					CHE_PDF_Number * pTmp = CHE_PDF_Number::Create( HE_PDF_StringToFloat( str ) );
					((CHE_PDF_Array*)pCurObj)->Append( pTmp );
				}
			}else if ( type == PDFPARSER_WORD_STRING )	//字符串
			{
				if ( pCurObj->GetType() == PDFOBJ_DICTIONARY )
				{
					((CHE_PDF_Dictionary*)pCurObj)->SetAtString( key, str );
				}else{
					CHE_PDF_String * pTmp = CHE_PDF_String::Create( str );
					((CHE_PDF_Array*)pCurObj)->Append( pTmp );
				}
			}else if ( type == PDFPARSER_WORD_NAME )	//名称
			{
				if ( pCurObj->GetType() == PDFOBJ_DICTIONARY )
				{
					((CHE_PDF_Dictionary*)pCurObj)->SetAtName( key, str );
				}else{
					CHE_PDF_Name * pTmp = CHE_PDF_Name::Create( str );
					((CHE_PDF_Array*)pCurObj)->Append( pTmp );
				}
			}else if ( type == PDFPARSER_WORD_ARRAY_B )	//数组
			{
				stack.Push( pCurObj );
				pCurObj = CHE_PDF_Array::Create();
			}else if ( type == PDFPARSER_WORD_ARRAY_E )	//数组
			{
				if ( stack.IsEmpty() == FALSE )
				{
					CHE_PDF_Object * pTmp = pCurObj;
					stack.Pop( &pCurObj );
					if ( pCurObj->GetType() == PDFOBJ_DICTIONARY )
					{
						((CHE_PDF_Dictionary*)pCurObj)->SetAtArray( key, (CHE_PDF_Array*)pTmp );
					}else{
						((CHE_PDF_Array*)pCurObj)->Append( pTmp );
					}
				}
			}else if ( type == PDFPARSER_WORD_DICT_B )	//字典
			{
				if ( pCurObj->GetType() == PDFOBJ_DICTIONARY )
				{
					CHE_ByteString * pStr = new CHE_ByteString( key );
					stackName.Push( (CHE_PDF_Object*)pStr );
				}
				stack.Push( pCurObj );
				pCurObj = CHE_PDF_Dictionary::Create();
			}else if ( type == PDFPARSER_WORD_DICT_E )	//字典	只出现在array的情况
			{
				CHE_PDF_Object * pTmp = pCurObj;
				stack.Pop( &pCurObj );
				((CHE_PDF_Array*)pCurObj)->Append( pTmp );
			}else{	//布尔和其他
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
					return NULL;
				}
			}
		}
		if ( pCurObj->GetType() == PDFOBJ_DICTIONARY )
		{
			bKey = TRUE;
		}else{
			bKey = FALSE;
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
		return TRUE;
	}else{
		m_pIHE_FileRead = file;
		m_sParser.InitParser( file );
		return FALSE;
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
		CHE_ByteString	str;
		HE_BYTE			type;
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
				type = m_sParser.GetType();
				if ( str == "trailer" )
				{
					CHE_PDF_Dictionary * pDict = NULL;
					if ( m_pTrailerDict == NULL )
					{
						pDict = m_pTrailerDict = m_sParser.GetDictionary();
					}else{
						pDict = m_sParser.GetDictionary();
					}
					if ( pDict != NULL )
					{
						CHE_PDF_Object * pObj = pDict->GetElement( CHE_ByteString("Prev") );
						if ( pObj )
						{
							HE_INT32 offset = ((CHE_PDF_Number*)pObj)->GetInteger();
							m_sParser.SetPos( offset );
							str = m_sParser.GetWord();
							if ( str != "xref" )
							{
								return TRUE;
							}else{
								str = m_sParser.GetWord();
								type = m_sParser.GetType();
							}
						}else{
							return TRUE;
						}
					}else{
						return TRUE;
					}
				}
				if( type = PDFPARSER_WORD_INTEGER )
				{
					lBeginNum = HE_PDF_StringToInteger( str );
				}else{
					return FALSE;
				}
				str = m_sParser.GetWord();
				type = m_sParser.GetType();
				if( type = PDFPARSER_WORD_INTEGER )
				{
					lCount = HE_PDF_StringToInteger( str );
				}else{
					return FALSE;
				}

				HE_DWORD objNum = 0, objGenNum = 0, objOffset;
				for ( HE_DWORD i = lBeginNum; i < lBeginNum + lCount; i++ )
				{
					str = m_sParser.GetWord();
					type = m_sParser.GetType();
					if( type = PDFPARSER_WORD_INTEGER )
					{
						objOffset = HE_PDF_StringToInteger( str );
					}else{
						return FALSE;
					}
					str = m_sParser.GetWord();
					type = m_sParser.GetType();
					if( type = PDFPARSER_WORD_INTEGER )
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

CHE_PDF_IndirectObject * CHE_PDF_Parser::GetIndirectObject()
{
	if ( m_pIHE_FileRead == NULL )
	{
		return NULL;
	}

	HE_DWORD offset = m_sParser.GetPos();

	CHE_ByteString str;
	HE_BYTE type;
	HE_DWORD objNum = 0, genNum = 0;
	str = m_sParser.GetWord();
	type = m_sParser.GetType();
	if ( type == PDFPARSER_WORD_INTEGER )
	{
		objNum = HE_PDF_StringToInteger( str );
	}else{
		return NULL;
	}
	str = m_sParser.GetWord();
	type = m_sParser.GetType();
	if ( type == PDFPARSER_WORD_INTEGER )
	{
		genNum = HE_PDF_StringToInteger( str );
	}else{
		return NULL;
	}
	str = m_sParser.GetWord();
	if ( str != "obj" )
	{
		return NULL;
	}

	HE_DWORD pos = m_sParser.GetPos();
	str = m_sParser.GetWord();
	type = m_sParser.GetType();
	CHE_PDF_Object * pCurObj = NULL;
	if ( type == PDFPARSER_WORD_DICT_B )
	{
		m_sParser.SetPos( pos );
		pCurObj = m_sParser.GetDictionary();
		if ( pCurObj == NULL )
		{
			return NULL;
		}

		str = m_sParser.GetWord();
		if ( str == "stream" )
		{
			CHE_PDF_Object * pObj = ((CHE_PDF_Dictionary*)pCurObj)->GetElement( CHE_ByteString("Length") );
			if ( pObj == NULL )
			{
				//销毁之前已经建立的对象
				//待添加
				return NULL;
			}else if ( pObj->GetType() != PDFOBJ_NUMBER )
			{
				return NULL;
			}else{
				HE_DWORD length = ((CHE_PDF_Number*)pObj)->GetInteger();
				if ( length == 0 )
				{
					return NULL;
				}else{
					m_sParser.Seek( length );
					str = m_sParser.GetWord();
					if ( str == "endstream" )
					{
						CHE_PDF_Stream * pStream = CHE_PDF_Stream::Create( m_pIHE_FileRead, offset, length, (CHE_PDF_Dictionary*)pCurObj );
						return CHE_PDF_IndirectObject::Create( objNum, pStream );
					}
				}
			}
		}else if ( str != "endobj" )
		{
			return NULL;
		}
	}else if ( type == PDFPARSER_WORD_FLOAT )
	{
		pCurObj = CHE_PDF_Number::Create( HE_PDF_StringToFloat( str ) );
	}else if ( type == PDFPARSER_WORD_INTEGER )
	{
		pCurObj = CHE_PDF_Number::Create( HE_PDF_StringToInteger( str ) );
	}else if ( type == PDFPARSER_WORD_STRING )
	{
		pCurObj = CHE_PDF_String::Create( str );
	}else if ( type == PDFPARSER_WORD_NAME )
	{
		pCurObj = CHE_PDF_Name::Create( str );
	}else if ( type == PDFPARSER_WORD_ARRAY_B )
	{
		pCurObj = m_sParser.GetArray();
	}else if ( str == "false" )
	{
		pCurObj = CHE_PDF_Boolean::Create( FALSE );
	}else if ( str == "true" )
	{
		pCurObj = CHE_PDF_Boolean::Create( TRUE );
	}else{
		return NULL;
	}
	return CHE_PDF_IndirectObject::Create( objNum, pCurObj );
}

CHE_PDF_IndirectObject * CHE_PDF_Parser::GetIndirectObject( HE_DWORD objNum )
{
	if ( m_pIHE_FileRead == NULL )
	{
		return NULL;
	}

	CHE_PDF_XREF_Entry entry;
	if ( m_xrefTable.GetEntry( objNum, entry ) )
	{
		m_sParser.SetPos( entry.GetOffset() );
		return GetIndirectObject();
	}else{
		return NULL;
	}
}