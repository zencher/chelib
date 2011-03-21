#include "../../include/pdf/che_pdf_parser.h"
#include "../../include/che_datastructure.h"
#include <string.h>

HE_INT32 HE_PDF_StringToInteger( CHE_ByteString & str )
{
	if ( str.GetLength() == 0 )
	{
		return 0;
	}else{
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
				}else{
					return 0;
				}
				break;
			case '-':
				if ( bSign == FALSE && bBegin == TRUE )
				{
					bSign = TRUE;
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
				}else{
					return 0;
				}
				break;
			case '-':
				if ( bSign == FALSE && bBegin == TRUE )
				{
					bSign = TRUE;
					bNegative = TRUE;
				}else{
					return 0;
				}
				break;
			case '.':
				if ( bPoint == FALSE )
				{
					bPoint = TRUE;
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
	
	m_lWordOffset = 0;

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

HE_BOOL CHE_PDF_SyntaxParser::InitParser( IHE_Read* pFileAccess )
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
		if ( m_lFilePos + bytes <= m_lFileSize )
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

HE_DWORD CHE_PDF_SyntaxParser::ReadBytes( HE_LPBYTE pBuffer, HE_DWORD length )
{
// 	if ( pBuffer == NULL )
// 	{
// 		return 0;
// 	}
	return m_pFileAccess->ReadBlock( pBuffer, m_lFilePos, length );
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
						m_lWordOffset = m_lFilePos;
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
									if ( m_lBufferPos == 0 )
									{
										m_lWordOffset = m_lFilePos;
									}
									m_WordBuffer[m_lBufferPos++] = (HE_BYTE)tmpValue;
									tmpCount = 0;
									tmpValue = 0;
								}
								if ( byte == '>' )
								{
									if ( tmpCount == 1 )
									{
										tmpValue = tmpValue * 16;
										if ( m_lBufferPos == 0 )
										{
											m_lWordOffset = m_lFilePos;
										}
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
								if ( m_lBufferPos == 0 )
								{
									m_lWordOffset = m_lFilePos;
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
								if ( m_lBufferPos == 0 )
								{
									m_lWordOffset = m_lFilePos;
								}
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
													if ( m_lBufferPos == 0 )
													{
														m_lWordOffset = m_lFilePos;
													}
													m_WordBuffer[m_lBufferPos++] = (HE_BYTE)tmpValue;
													tmpCount = 0;
												}
											}else{
												break;
											}
										}
										if ( tmpCount != 0 )
										{
											if ( m_lBufferPos == 0 )
											{
												m_lWordOffset = m_lFilePos;
											}
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
								if ( m_lBufferPos == 0 )
								{
									m_lWordOffset = m_lFilePos;
								}
								m_WordBuffer[m_lBufferPos++] = byte;
								m_lFilePos++;
							}
						}
						m_lFilePos++;
						return SubmitBufferStr();
					}
					break;
				}
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
					if ( m_lBufferPos == 0 )
					{
						m_lWordOffset = m_lFilePos;
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
					if ( m_lBufferPos == 0 )
					{
						m_lWordOffset = m_lFilePos;
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
					if ( m_lBufferPos == 0 )
					{
						m_lWordOffset = m_lFilePos;
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
					if ( m_lBufferPos == 0 )
					{
						m_lWordOffset = m_lFilePos;
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
	CHE_PtrStack stack;
	CHE_PtrStack stackName;
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
					stack.Pop( (HE_VOID**)&pCurObj );
					if ( pCurObj->GetType() == PDFOBJ_ARRAY )
					{
						((CHE_PDF_Array*)pCurObj)->Append( pDict );
					}else if ( pCurObj->GetType() == PDFOBJ_DICTIONARY )
					{
						CHE_ByteString * pTmpStr = NULL;
						stackName.Pop(  (HE_VOID**)&pTmpStr );
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
					stack.Pop( (HE_VOID**)&pCurObj );
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
				stack.Pop( (HE_VOID**)&pCurObj );
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
	CHE_PtrStack stack;
	CHE_PtrStack stackName;
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
					stack.Pop( (HE_VOID**)&pCurObj );
					if ( pCurObj->GetType() == PDFOBJ_ARRAY )
					{
						((CHE_PDF_Array*)pCurObj)->Append( pDict );
					}else if ( pCurObj->GetType() == PDFOBJ_DICTIONARY )
					{
						CHE_ByteString * pTmpStr = NULL;
						stackName.Pop(  (HE_VOID**)&pTmpStr );
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
					stack.Pop( (HE_VOID**)&pCurObj );
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
				stack.Pop( (HE_VOID**)&pCurObj );
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

// HE_VOID	CHE_PDF_SyntaxParser::FillXRefTable( CHE_PDF_XREF_Table & table )
// {
// 	CHE_ByteString str;
// 	HE_BYTE type = 0;
// 	HE_DWORD lBeginNum = 0, lCount = 0;
// 	HE_DWORD lPos = 0;
// 	while ( true )
// 	{
// 		lPos = GetPos();
// 		str = GetWord();
// 		type = GetType();
// 		if( type == PDFPARSER_WORD_INTEGER )
// 		{
// 			lBeginNum = HE_PDF_StringToInteger( str );
// 		}else{
// 			SetPos( lPos );
// 			return;
// 		}
// 		str = GetWord();
// 		type = GetType();
// 		if( type == PDFPARSER_WORD_INTEGER )
// 		{
// 			lCount = HE_PDF_StringToInteger( str );
// 		}else{
// 			SetPos( lPos );
// 			return;
// 		}
// 		SeekToNextLine();
// 		
// 		HE_BYTE tmpByte[20];
// 		HE_DWORD objNum = 0, objGenNum = 0, objOffset = 0;
// 		for ( HE_DWORD i = lBeginNum; i < lBeginNum + lCount; i++ )
// 		{
// 			m_pFileAccess->ReadBlock( tmpByte, m_lFilePos, 20 );
// 			m_lFilePos+= 20;
// 			objOffset =	  (tmpByte[0] - 48) * 1000000000 + (tmpByte[1] - 48) * 100000000 + (tmpByte[2] - 48) * 10000000
// 						+ (tmpByte[3] - 48) * 1000000 + (tmpByte[4] - 48) * 100000 + (tmpByte[5] - 48) * 10000
// 						+ (tmpByte[6] - 48) * 1000 + (tmpByte[7] - 48) * 100 + (tmpByte[8] - 48) * 10 + (tmpByte[9] - 48); 
// 			objGenNum =	(tmpByte[10] - 48) * 10000 + (tmpByte[11] - 48) * 1000 + (tmpByte[12] - 48) * 100 + (tmpByte[13] - 48) * 10 + tmpByte[14] - 48;
// 			if ( tmpByte[17] == 'f' )
// 			{
// 				table.Append( CHE_PDF_XREF_Entry( objOffset, i, objGenNum, 'f' ) );
// 			}else{
// 				table.Append( CHE_PDF_XREF_Entry( objOffset, i, objGenNum, 'n' ) );
// 			}
// 		}
// 	}
// }

CHE_PDF_Parser::CHE_PDF_Parser()
{
	m_pTrailerDict = NULL;
	m_pIHE_FileRead = NULL;
	m_lstartxref = 0;
}

HE_BOOL CHE_PDF_Parser::StartParse( IHE_Read * file )
{
	if ( file == NULL )
	{
		return FALSE;
	}else{
		m_pIHE_FileRead = file;
		m_sParser.InitParser( file );
		return TRUE;
	}
}

HE_VOID CHE_PDF_Parser::CloseParser()
{
	if ( m_pIHE_FileRead )
	{
		m_pIHE_FileRead->Release();
		m_pIHE_FileRead = NULL;
	}
	m_objCollector.Release();
	m_objCollector.Clear();
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
		GetStartxrefOffset( 1024 * 4 );
		if ( m_lstartxref == 0 )
		{
			//需要分析整个文件来获取对象信息
			m_sParser.SetPos( 0 );
			CHE_ByteString	str;
			HE_BYTE			type = PDFPARSER_WORD_UNKNOWN;
			HE_DWORD		offset = 0;
			HE_DWORD		objNum = 0;
			while ( true )
			{
				if ( m_sParser.GetPos() == m_sParser.GetFileSize() )
				{
					break;
				}
				str = m_sParser.GetWord();
				if ( str == "trailer" )
				{
					if ( m_pTrailerDict == NULL )
					{
						m_pTrailerDict = m_sParser.GetDictionary();
					}
					continue;
				}
				type = m_sParser.GetType();
				offset = m_sParser.GetWordOffset();
				if ( type != PDFPARSER_WORD_INTEGER )
				{
					continue;
				}
				objNum = HE_PDF_StringToInteger( str );
				str = m_sParser.GetWord();
				type = m_sParser.GetType();
				if ( type != PDFPARSER_WORD_INTEGER )
				{
					continue;
				}
				str = m_sParser.GetWord();
				if ( str != "obj" )
				{
					continue;
				}
				m_xrefTable.Append( CHE_PDF_XREF_Entry( offset, objNum, 0, 'n' ) );
				while( true )
				{
					m_sParser.SeekToNextLine();
					str = m_sParser.GetWord();
					if ( str == "endobj" )
					{
						break;
					}
				}
			}
			return TRUE;
		}
	}else{
		m_sParser.SetPos( m_lstartxref );
		CHE_ByteString	str;
		HE_BYTE			type;
		str = m_sParser.GetWord();
		if ( str != "xref" )
		{  
			//处理交叉索引表流。待测试
			m_sParser.SetPos( m_lstartxref );
			HE_DWORD objNum = 0, genNum = 0;
			while ( true )
			{
				HE_DWORD lBeginNum = 0, lCount = 0, lSize = 0, lW1 = 0, lW2 = 0, lW3 = 0;

				CHE_PDF_IndirectObject * pIndObj = GetIndirectObject();
				if ( pIndObj == NULL || pIndObj->GetType() != PDFOBJ_STREAM )
				{
					return FALSE;
				}
				CHE_PDF_Dictionary * pDict = pIndObj->GetDict();
				if ( pDict == NULL )
				{
					return FALSE;
				}
				CHE_PDF_Object * pElement =  pDict->GetElement( CHE_ByteString("Type") );
				if ( pElement == NULL || pElement->GetType() != PDFOBJ_NAME )
				{
					return FALSE;
				}
				if( ((CHE_PDF_Name*)pElement)->GetString() != "XRef" )
				{
					return FALSE;
				}
				pElement = pDict->GetElement( CHE_ByteString("Size") );
				if ( pElement == NULL || pElement->GetType() != PDFOBJ_NUMBER )
				{
					return FALSE;
				}
				lSize = ((CHE_PDF_Number*)pElement)->GetInteger();

				pElement = pDict->GetElement( CHE_ByteString("Index") );
				if ( pElement != NULL && pElement->GetType() == PDFOBJ_ARRAY )
				{
					lBeginNum = 0;
					lCount = lSize;
				}else{
					if ( ((CHE_PDF_Array*)pElement)->GetCount() < 2 )
					{
						return FALSE;
					}
					if ( ((CHE_PDF_Array*)pElement)->GetElement( 0 )->GetType() != PDFOBJ_NUMBER 
						|| ((CHE_PDF_Array*)pElement)->GetElement( 1 )->GetType() != PDFOBJ_NUMBER )
					{
						return FALSE;
					}
					lBeginNum = ((CHE_PDF_Number*)((CHE_PDF_Array*)pElement)->GetElement( 0 ))->GetInteger();
					lCount	= ((CHE_PDF_Number*)((CHE_PDF_Array*)pElement)->GetElement( 1 ))->GetInteger();
				}

				pElement = pDict->GetElement( CHE_ByteString("W") );
				if ( pElement == NULL || pElement->GetType() != PDFOBJ_ARRAY )
				{
					return FALSE;
				}
				if ( ((CHE_PDF_Array*)pElement)->GetElement( 0 )->GetType() != PDFOBJ_NUMBER 
					|| ((CHE_PDF_Array*)pElement)->GetElement( 1 )->GetType() != PDFOBJ_NUMBER
					|| ((CHE_PDF_Array*)pElement)->GetElement( 2 )->GetType() != PDFOBJ_NUMBER )
				{
					return FALSE;
				}
				lW1 = ((CHE_PDF_Number*)((CHE_PDF_Array*)pElement)->GetElement( 0 ))->GetInteger();
				lW2	= ((CHE_PDF_Number*)((CHE_PDF_Array*)pElement)->GetElement( 1 ))->GetInteger();
				lW3	= ((CHE_PDF_Number*)((CHE_PDF_Array*)pElement)->GetElement( 2 ))->GetInteger();

				CHE_PDF_StreamAcc streamAcc;
				if ( streamAcc.Attach( pIndObj->GetStream() ) == FALSE )
				{
					return FALSE;
				}

				HE_DWORD lsize = streamAcc.GetSize();
				HE_LPCBYTE lpByte = streamAcc.GetData();
				HE_DWORD field1 = 0, field2 = 0, field3 = 0;
				HE_DWORD lcount = 0;
				HE_DWORD lentrySize = lW1 + lW2 + lW3;
				while ( TRUE )
				{
					if ( ( lcount + 1 ) * lentrySize > lsize )
					{
						break;
					}
					lcount++;

					field1 = 0, field2 = 0, field3 = 0;
					if ( lW1 == 0 )
					{
						field1 = 1;
					}else if ( lW1 == 1 )
					{
						field1 = *lpByte;
						lpByte++;
					}else{
						for ( HE_DWORD i = 0; i < lW1; i++ )
						{
							field1 = field1 << 8;
							field1 += *lpByte;
							lpByte++;
						}
					}
					for ( HE_DWORD j = 0; j < lW2; j++ )
					{
						field2 = field2 << 8;
						field2 += *lpByte;
						lpByte++;
					}
					for ( HE_DWORD k = 0; k < lW2; k++ )
					{
						field3 = field3 << 8;
						field3 += *lpByte;
						lpByte++;
					}

					switch( field1 )
					{
					case 0:
						m_xrefTable.Append( CHE_PDF_XREF_Entry( 0, field2, field3, 'f') );
						break;
					case 1:
						m_xrefTable.Append( CHE_PDF_XREF_Entry( field2, field3, 0, 'n' ) );
						break;
					case 2:
						m_xrefTable.Append( CHE_PDF_XREF_Entry( field2, field3 ) );
						break;
					default:
						break;
					}
				}
				streamAcc.Detach();

				pElement = pDict->GetElement( CHE_ByteString("Prev") );
				if ( pElement == NULL || pElement->GetType() != PDFOBJ_NUMBER )
				{
					break;
				}else{
					m_sParser.SetPos( ((CHE_PDF_Number*)pElement)->GetInteger() );
				}
			}
			return TRUE;
		}else if ( str == "xref" ){
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
				if( type == PDFPARSER_WORD_INTEGER )
				{
					lBeginNum = HE_PDF_StringToInteger( str );
				}else{
					return FALSE;
				}
				str = m_sParser.GetWord();
				type = m_sParser.GetType();
				if( type == PDFPARSER_WORD_INTEGER )
				{
					lCount = HE_PDF_StringToInteger( str );
				}else{
					return FALSE;
				}
				m_sParser.SeekToNextLine();

				HE_DWORD objNum = 0, objGenNum = 0, objOffset;
				HE_BYTE tmpBytes[20];
				for ( HE_DWORD i = lBeginNum; i < lBeginNum + lCount; i++ )
				{
					m_sParser.ReadBytes( tmpBytes, 20 );
					m_sParser.Seek( 20 );

					objOffset =	  (tmpBytes[0] - 48) * 1000000000 + (tmpBytes[1] - 48) * 100000000 + (tmpBytes[2] - 48) * 10000000
						+ (tmpBytes[3] - 48) * 1000000 + (tmpBytes[4] - 48) * 100000 + (tmpBytes[5] - 48) * 10000
						+ (tmpBytes[6] - 48) * 1000 + (tmpBytes[7] - 48) * 100 + (tmpBytes[8] - 48) * 10 + (tmpBytes[9] - 48); 
					objGenNum =	(tmpBytes[11] - 48) * 10000 + (tmpBytes[12] - 48) * 1000 + (tmpBytes[13] - 48) * 100 + (tmpBytes[14] - 48) * 10 + tmpBytes[15] - 48;
					if ( tmpBytes[17] == 'f' )
					{
						m_xrefTable.Append( CHE_PDF_XREF_Entry( objOffset, i, objGenNum, 'f' ) );
					}else{
						m_xrefTable.Append( CHE_PDF_XREF_Entry( objOffset, i, objGenNum, 'n' ) );
					}
				}
			}
		}
		return FALSE;
	}
	return FALSE;
}

CHE_PDF_Dictionary* CHE_PDF_Parser::GetRootDict()
{
	if ( m_pTrailerDict )
	{
		CHE_PDF_Object * pObj = m_pTrailerDict->GetElement( CHE_ByteString("Root") );
		if ( pObj == NULL )
		{
			return NULL;
		}
		if ( pObj->GetType() == PDFOBJ_DICTIONARY )
		{
			return (CHE_PDF_Dictionary*)pObj;
		}else if ( pObj->GetType() == PDFOBJ_REFERENCE )
		{
			CHE_PDF_IndirectObject * pIndirectObj = GetIndirectObject( ((CHE_PDF_Reference*)pObj)->GetRefNuml() );
			if ( pIndirectObj == NULL )
			{
				return NULL;
			}
			CHE_PDF_Object * pObj = pIndirectObj->GetObject();
			if ( pObj == NULL )
			{
				return NULL;
			}
			if ( pObj->GetType() == PDFOBJ_DICTIONARY )
			{
				return (CHE_PDF_Dictionary*)pObj;
			}else{
				return NULL;
			}
		}
	}
	return NULL;
}

CHE_PDF_Dictionary* CHE_PDF_Parser::GetInfoDict()
{
	if ( m_pTrailerDict )
	{
		CHE_PDF_Object * pObj = m_pTrailerDict->GetElement( CHE_ByteString("Info") );
		if ( pObj == NULL )
		{
			return NULL;
		}
		if ( pObj->GetType() == PDFOBJ_DICTIONARY )
		{
			return (CHE_PDF_Dictionary*)pObj;
		}else if ( pObj->GetType() == PDFOBJ_REFERENCE )
		{
			CHE_PDF_IndirectObject * pIndirectObj = GetIndirectObject( ((CHE_PDF_Reference*)pObj)->GetObjNum() );
			if ( pIndirectObj->GetType() == PDFOBJ_DICTIONARY )
			{
				CHE_PDF_Dictionary * pDict = pIndirectObj->GetDict();
				pIndirectObj->Release();
				return pDict;
			}else{
				pIndirectObj->Release();
				return NULL;
			}
		}
	}
	return NULL;
}

CHE_PDF_Array* CHE_PDF_Parser::GetIDArray()
{
	if ( m_pTrailerDict )
	{
		CHE_PDF_Array * pRootObj = (CHE_PDF_Array*)m_pTrailerDict->GetElement( CHE_ByteString("ID") );
		return pRootObj;
	}
	return NULL;
}

HE_DWORD CHE_PDF_Parser::GetPageCount()
{
	CHE_PDF_Dictionary * pDict = GetRootDict();
	if ( pDict == NULL )
	{
		return 0;
	}
	CHE_PDF_Object * pPagesRef = pDict->GetElement( CHE_ByteString("Pages") );
	if ( pPagesRef == NULL || pPagesRef->GetType() != PDFOBJ_REFERENCE )
	{
		return 0;
	}
	CHE_PDF_IndirectObject * pInObj = GetIndirectObject( ((CHE_PDF_Reference*)pPagesRef)->GetRefNuml() );
	if ( pInObj == NULL )
	{
		return 0;
	}
//////////////////////////////////////////////////////////////////////////
	HE_DWORD lPageCount = 0;
	CHE_PDF_Dictionary * pDictInObj = NULL;

	pDictInObj = pInObj->GetDict();
	if ( pDictInObj == NULL )
	{
		return 0;
	}

	CHE_PDF_Object * pObj =  pDictInObj->GetElement( CHE_ByteString("Count") );
	if ( pObj == NULL || pObj->GetType() != PDFOBJ_NUMBER )
	{
		return 0;
	}
	lPageCount = ((CHE_PDF_Number*)pObj)->GetInteger();

	return lPageCount;

// 	HE_DWORD lPageCount = 0;
// 	CHE_PtrStack stack;
// 	CHE_PDF_Dictionary * pDictInObj = NULL;
// 	stack.Push( pInObj );
// 
// 	while ( stack.IsEmpty() == FALSE )
// 	{
// 		if ( stack.Pop( (HE_LPVOID*)&pInObj ) == false || pInObj == NULL )
// 		{
// 			return 0;
// 		}
// 		pDictInObj = pInObj->GetDict();
// 		if ( pDictInObj == NULL )
// 		{
// 			return 0;
// 		}
// 
// 		HE_DWORD lNodeCount = 0;
// 		CHE_PDF_Object * pObj =  pDictInObj->GetElement( CHE_ByteString("Count") );
// 		if ( pObj == NULL || pObj->GetType() != PDFOBJ_NUMBER )
// 		{
// 			return 0;
// 		}
// 		lNodeCount = ((CHE_PDF_Number*)pObj)->GetInteger();
// 
// 		pObj = pDictInObj->GetElement( CHE_ByteString("Kids") );
// 		if ( pObj == NULL || pObj->GetType() != PDFOBJ_ARRAY )
// 		{
// 			return 0;
// 		}
// 		//if ( ((CHE_PDF_Array*)pObj)->GetCount() != lNodeCount )
// 		//{
// 		//	return 0;
// 		//}
// 		CHE_PDF_Object * pArrayElement = NULL;
// 		for ( HE_DWORD i = 0; i < ((CHE_PDF_Array*)pObj)->GetCount(); i++ )
// 		{
// 			pArrayElement = ((CHE_PDF_Array*)pObj)->GetElement( i );
// 			if ( pArrayElement == NULL || pArrayElement->GetType() != PDFOBJ_REFERENCE )
// 			{
// 				return 0;
// 			}
// 			pInObj = GetIndirectObject( ((CHE_PDF_Reference*)pArrayElement)->GetRefNuml() );
// 			if ( pInObj == NULL )
// 			{
// 				return 0;
// 			}
// 			pDictInObj = pInObj->GetDict();
// 			if ( pDictInObj == NULL )
// 			{
// 				return 0;
// 			}
// 			CHE_PDF_Object * pType = pDictInObj->GetElement( CHE_ByteString("Type") );
// 			if ( pType == NULL || pType->GetType() != PDFOBJ_NAME )
// 			{
// 				return 0;
// 			}
// 			CHE_ByteString str = ((CHE_PDF_Name*)pType)->GetString();
// 			if ( str == "Pages" )
// 			{
// 				stack.Push( pInObj );
// 			}else if ( str == "Page" )
// 			{
// 				lPageCount++;
// 			}
// 		}
// 	}
// 	return lPageCount;
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
				pCurObj->Release();
				return NULL;
			}else if ( pObj->GetType() != PDFOBJ_NUMBER )
			{
				pCurObj->Release();
				return NULL;
			}else{
				HE_DWORD length = ((CHE_PDF_Number*)pObj)->GetInteger();
				if ( length == 0 )
				{
					pCurObj->Release();
					return NULL;
				}else{
					m_sParser.Seek( length );
					str = m_sParser.GetWord();
					if ( str == "endstream" )
					{
						CHE_PDF_Stream * pStream = CHE_PDF_Stream::Create( m_pIHE_FileRead, offset, length, (CHE_PDF_Dictionary*)pCurObj );
						if ( pStream == NULL )
						{
							pCurObj->Release();
							return NULL;
						}
						CHE_PDF_IndirectObject * pObj = CHE_PDF_IndirectObject::Create( objNum, pStream );
						if ( pObj )
						{
							m_objCollector.Add( pObj );
							return pObj;
						}else{
							pStream->Release();
							return NULL;
						}
					}
				}
			}
		}else if ( str == "endobj" )
		{
			//return dictionary obj
			CHE_PDF_IndirectObject * pObj = CHE_PDF_IndirectObject::Create( objNum, pCurObj );
			if ( pObj )
			{
				m_objCollector.Add( pObj );
				return pObj;
			}else{
				pCurObj->Release();
				return NULL;
			}
		}else{
			pCurObj->Release();
			return NULL;
		}
	}else if ( type == PDFPARSER_WORD_FLOAT )
	{
		pCurObj = CHE_PDF_Number::Create( HE_PDF_StringToFloat( str ) );
	}else if ( type == PDFPARSER_WORD_INTEGER )
	{
		HE_DWORD objNum = HE_PDF_StringToInteger( str );
		HE_DWORD pos = m_sParser.GetPos();
		str = m_sParser.GetWord();
		type = m_sParser.GetType();
		if ( type == PDFPARSER_WORD_INTEGER )
		{
			str = m_sParser.GetWord();
			if ( str == "R" )
			{
				pCurObj = CHE_PDF_Reference::Create( objNum );
			}else{
				pCurObj = CHE_PDF_Number::Create( (HE_INT32)objNum );
			}
		}else{
			pCurObj = CHE_PDF_Number::Create( (HE_INT32)objNum );
		}
		m_sParser.SetPos( pos );
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
	CHE_PDF_IndirectObject * pObj = CHE_PDF_IndirectObject::Create( objNum, pCurObj );
	if ( pObj )
	{
		m_objCollector.Add( pObj );
		return pObj;
	}else{
		pCurObj->Release();
		return NULL;
	}
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
