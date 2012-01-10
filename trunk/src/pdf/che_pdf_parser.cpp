#include "../../include/pdf/che_pdf_parser.h"
#include "../../include/pdf/che_pdf_encrypt.h"
#include "../../include/che_datastructure.h"

CHE_PDF_SyntaxParser::CHE_PDF_SyntaxParser( CHE_PDF_Parser * pParser, CHE_Allocator * pAllocator ) : CHE_Object( pAllocator )
{
	m_lFilePos = 0;
	m_pFileAccess = NULL;
	m_lFileSize = 0;

	m_pStrEncrypt = NULL;
	m_dwCurObjNum = 0;
	m_dwCurGenNum = 0;

	m_bBegin = TRUE;
	m_bPoint = FALSE;
	m_bSign = FALSE;

	memset( m_WordBuffer, 0, 255 );
	m_lBufferSize = 0;
	m_lBufferPos = 0;

	m_pParser = pParser;
}

CHE_PDF_SyntaxParser::~CHE_PDF_SyntaxParser()
{
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

HE_VOID CHE_PDF_SyntaxParser::SeekToPrevLine()
{
	if ( m_pFileAccess )
	{
		while( m_lFilePos >= 0 )
		{
			HE_BYTE byte = m_pFileAccess->ReadByte( m_lFilePos );
			switch( byte )
			{
			case 0x0A:
				{
					byte = m_pFileAccess->ReadByte( m_lFilePos-1 );
					if ( byte == 0x0D )
					{
						m_lFilePos--;
					}
				}
			case 0x0D:
				m_lFilePos--;
				return;
			}
			m_lFilePos--;
		}
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

HE_VOID	CHE_PDF_SyntaxParser::SeekToPrevWord()
{
	if ( m_pFileAccess )
	{
		HE_BOOL bWordFound = FALSE;
		while( m_lFilePos >= 0 )
		{
			HE_BYTE byte = m_pFileAccess->ReadByte( m_lFilePos );
			switch( byte )
			{
			case 0x0A:
				{
					byte = m_pFileAccess->ReadByte( m_lFilePos-1 );
					if ( byte == 0x0D )
					{
						m_lFilePos--;
					}
				}
			case 0x0D:
			case 0x00:
			case 0x09:
			case 0x0C:
			case 0x20:
				if ( bWordFound == TRUE )
				{
					return;
				}
				break;
			default:
				bWordFound = TRUE;
				break;
			}
			m_lFilePos--;
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

HE_VOID	CHE_PDF_SyntaxParser::SeekToEndStream()
{
	HE_CHAR tmpStr[8];
	if ( m_pFileAccess )
	{
		while( m_lFilePos < m_lFileSize )
		{
			HE_BYTE byte = m_pFileAccess->ReadByte( m_lFilePos++ );
			switch( byte )
			{
			case 'e':
				{
					if ( m_lFilePos + 8 < m_lFileSize )
					{
						m_pFileAccess->ReadBlock( tmpStr, m_lFilePos, 8 );
						if (	tmpStr[0] == 'n' && tmpStr[1] == 'd' && tmpStr[2] == 's' 
							&&	tmpStr[3] == 't' && tmpStr[4] == 'r' && tmpStr[5] == 'e'
							&&	tmpStr[6] == 'a' && tmpStr[7] == 'm' )
						{
							m_lFilePos--;
							m_lFilePos--;
							byte = m_pFileAccess->ReadByte( m_lFilePos );
							if ( byte == '\n' )
							{
								m_lFilePos--;
								byte = m_pFileAccess->ReadByte( m_lFilePos );
								if ( byte == '\r' )
								{
									m_lFilePos--;
								}
							}else if ( byte == '\r' )
							{
								m_lFilePos--;
							}
							return;
						}
					}
				}
			default:
				break;
			}
		}
	}
}

HE_VOID CHE_PDF_SyntaxParser::SeekToEndobj()
{
	HE_CHAR tmpStr[5];
	if ( m_pFileAccess )
	{
		while( m_lFilePos < m_lFileSize )
		{
			HE_BYTE byte = m_pFileAccess->ReadByte( m_lFilePos++ );
			switch( byte )
			{
			case 'e':
				{
					if ( m_lFilePos + 4 < m_lFileSize )
					{
						m_pFileAccess->ReadBlock( tmpStr, m_lFilePos, 5 );
						if (	tmpStr[0] == 'n' && tmpStr[1] == 'd' && tmpStr[2] == 'o' 
							&&	tmpStr[3] == 'b' && tmpStr[4] == 'j' )
						{
							m_lFilePos--;
							return;
						}
					}
				}
			default:
				break;
			}
		}
	}	
}

HE_VOID CHE_PDF_SyntaxParser::SeekToMark( CHE_ByteString markStr )
{
	if ( markStr.GetLength() == 0 )
	{
		return;
	}else if ( markStr.GetLength() == 1 )
	{
		if ( m_pFileAccess )
		{
			HE_BYTE byteTmp = markStr[0], byte = 0;
			while( m_lFilePos < m_lFileSize )
			{
				byte = m_pFileAccess->ReadByte( m_lFilePos++ );
				if ( byte == byteTmp )
				{
					m_lFilePos--;
					return;
				}
			}
		}else{
			return;
		}
	}else{
		HE_CHAR * pTmpStr = GetAllocator()->NewArray<HE_CHAR>( markStr.GetLength()+1 );
		pTmpStr[markStr.GetLength()] = 0;
		if ( m_pFileAccess )
		{
			HE_BYTE byteTmp = markStr[0], byte = 0;
			while( m_lFilePos < m_lFileSize )
			{
				byte = m_pFileAccess->ReadByte( m_lFilePos++ );
				if ( byte == byteTmp )
				{
					if ( m_lFilePos + markStr.GetLength() - 1 < m_lFileSize )
					{
						m_pFileAccess->ReadBlock( pTmpStr, m_lFilePos-1, markStr.GetLength() );
						pTmpStr[markStr.GetLength()] = 0;
						if ( markStr == CHE_ByteString(pTmpStr) )
						{
							m_lFilePos--;
							GetAllocator()->DeleteArray<HE_CHAR>( pTmpStr );
							return;
						}
					}
				}
			}
		}
		GetAllocator()->DeleteArray<HE_CHAR>( pTmpStr );
	}
}

HE_DWORD CHE_PDF_SyntaxParser::ReadBytes( HE_LPBYTE pBuffer, HE_DWORD length )
{
	return m_pFileAccess->ReadBlock( pBuffer, m_lFilePos, length );
}

HE_BOOL CHE_PDF_SyntaxParser::GetWord( CHE_PDF_ParseWordDes & des )
{
	if ( m_pFileAccess )
	{
		des.offset = 0;
		des.type = PARSE_WORD_UNKNOWN;

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
						SubmitBufferStr( des.str );
						return TRUE;
					}else{
						byte = m_pFileAccess->ReadByte( m_lFilePos+1 );
						if ( byte == '<' )
						{
							//返回表明字典开始的词
							des.offset = m_lFilePos;
							des.type = PARSE_WORD_DICT_B;
							des.str = "<<";
							m_lFilePos+=2;
							return TRUE;
						}else{
							//字符串处理
							des.offset = m_lFilePos;
							des.type = PARSE_WORD_STRING;
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
										tmpValue = tmpValue * 16 + (byte - 0x37);
										break;
									}
								case 'a':
								case 'b':
								case 'c':
								case 'd':
								case 'e':
								case 'f':
									{
										tmpValue = tmpValue * 16 + (byte - 0x57);
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
							SubmitBufferStr( des.str );
							if ( m_pStrEncrypt && m_pStrEncrypt->IsPasswordOK() == TRUE )
							{
								m_pStrEncrypt->Decrypt( des.str, m_dwCurObjNum, m_dwCurGenNum );
							}
							return TRUE;
						}
					}
					break;
				}
			case '>':	//字典结束，因为字符串的已经在其他位置处理了，这里不可能是字符串了
				{
					if ( m_lBufferPos > 0 )
					{
						SubmitBufferStr( des.str );
						return TRUE;
					}else{
						byte = m_pFileAccess->ReadByte( m_lFilePos+1 );
						if ( byte == '>' )
						{
							des.offset = m_lFilePos;
							des.type = PARSE_WORD_DICT_E;
							des.str = ">>";
							m_lFilePos += 2;
							return TRUE;
						}
					}
					break;
				}
			case '[':
				{
					if ( m_lBufferPos > 0 )
					{
						SubmitBufferStr( des.str );
						return TRUE;
					}else{
						des.offset = m_lFilePos;
						des.type = PARSE_WORD_ARRAY_B;
						des.str = "[";
						m_lFilePos++;
						return TRUE;
					}
					break;
				}
			case ']':
				{
					if ( m_lBufferPos > 0 )
					{
						SubmitBufferStr( des.str );
						return TRUE;
					}else{
						des.offset = m_lFilePos;
						des.type = PARSE_WORD_ARRAY_E;
						des.str = "]";
						m_lFilePos++;
						return TRUE;
					}
					break;
				}
			case '/':
				{
					if ( m_lBufferPos > 0 )
					{
						SubmitBufferStr( des.str );
						return TRUE;
					}else{
						des.offset = m_lFilePos;
						des.type = PARSE_WORD_NAME;
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
										tmpValue = tmpValue * 16 + (byte - 0x37);
										break;
									}
								case 'a':
								case 'b':
								case 'c':
								case 'd':
								case 'e':
								case 'f':
									{
										tmpValue = tmpValue * 16 + (byte - 0x57);
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
										tmpValue = tmpValue * 16 + (byte - 0x37);
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
						SubmitBufferStr( des.str );
						return TRUE;
					}
					break;
				}
			case '(':	//字符串
				{
					if ( m_lBufferPos > 0 )
					{
						SubmitBufferStr( des.str );
						return TRUE;
					}else{
						des.type = PARSE_WORD_STRING;
						des.offset = m_lFilePos;
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
											tmpByte = m_pFileAccess->ReadByte( ++m_lFilePos );
											if ( tmpByte >= '0' && tmpByte <= '7' )
											{
												tmpValue = tmpValue * 8 + (tmpByte-'0');
												tmpCount++;
												if ( tmpCount == 3 )
												{
													m_WordBuffer[m_lBufferPos++] = (HE_BYTE)tmpValue;
													tmpCount = 0;
													++m_lFilePos;
													break;
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
						m_lFilePos++;
						SubmitBufferStr( des.str );
						if ( m_pStrEncrypt && m_pStrEncrypt->IsPasswordOK() == TRUE )
						{
							m_pStrEncrypt->Decrypt( des.str, m_dwCurObjNum, m_dwCurGenNum );
						}
						return TRUE;
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
						SubmitBufferStr( des.str );
						return TRUE;
					}
					break;
				}
			case '+':
			case '-':
				{
					if ( m_bBegin && m_bSign == FALSE )
					{
						des.type = PARSE_WORD_INTEGER;
					}else{
						des.type = PARSE_WORD_UNKNOWN;
					}
					if ( des.offset == 0 )
					{
						des.offset = m_lFilePos;
					}
					m_WordBuffer[m_lBufferPos++] = byte;
					break;
				}
			case '.':
				{
					if ( m_bPoint == FALSE )
					{
						m_bPoint = TRUE;
						des.type = PARSE_WORD_FLOAT;
					}else{
						des.type = PARSE_WORD_UNKNOWN;
					}
					if ( des.offset == 0 )
					{
						des.offset = m_lFilePos;
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
						des.type = PARSE_WORD_INTEGER;
					}
					if ( des.offset == 0 )
					{
						des.offset = m_lFilePos;
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
					if ( des.type != PARSE_WORD_UNKNOWN )
					{
						des.type = PARSE_WORD_UNKNOWN;
					}
					if ( des.offset == 0 )
					{
						des.offset = m_lFilePos;
					}
					if ( m_lBufferPos > 32770 )
					{
						SubmitBufferStr( des.str );
						return TRUE;
					}
					m_WordBuffer[m_lBufferPos++] = byte;
					break;
				}
			}
			m_lFilePos++;
		}

		if ( m_lBufferPos != 0 )
		{
			SubmitBufferStr( des.str );
			return TRUE;
		}else{
			return FALSE;
		}
	}else{
		return FALSE;
	}
}

CHE_PDF_Array *	CHE_PDF_SyntaxParser::GetArray()
{
	if ( m_pFileAccess == NULL )
	{
		return NULL;
	}

	CHE_PDF_ParseWordDes wordDes( GetAllocator() );

	if ( !GetWord( wordDes ) )
	{
		return NULL;
	}
	if ( wordDes.type != PARSE_WORD_ARRAY_B )
	{
		return NULL;
	}

	HE_BOOL bKey = FALSE;
	CHE_ByteString key( GetAllocator() );
	CHE_Stack<CHE_PDF_Object*> stack( GetAllocator() );
	CHE_Stack<CHE_ByteString> stackName( GetAllocator() );
	CHE_PDF_Object * pCurObj = CHE_PDF_Array::Create( /*m_dwCurObjNum, m_dwCurGenNum, GetParser(),*/ GetAllocator() );

	while ( TRUE )
	{
		if ( bKey )
		{
			if ( !GetWord( wordDes ) )
			{
				return NULL;
			}
			if ( wordDes.type == PARSE_WORD_NAME )
			{
				key = wordDes.str;
			}else if ( wordDes.type == PARSE_WORD_DICT_E  )
			{
				if ( stack.IsEmpty() == FALSE )
				{
					CHE_PDF_Dictionary * pDict = (CHE_PDF_Dictionary*)pCurObj;
					stack.Pop( pCurObj );
					if ( pCurObj->GetType() == OBJ_TYPE_ARRAY )
					{
						((CHE_PDF_Array*)pCurObj)->Append( pDict );
					}else if ( pCurObj->GetType() == OBJ_TYPE_DICTIONARY )
					{
						stackName.Pop( key );
						((CHE_PDF_Dictionary*)pCurObj)->SetAtDictionary( key, pDict );
					}
				}
				if ( pCurObj->GetType() == OBJ_TYPE_ARRAY )
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
			if ( !GetWord( wordDes ) )
			{
				return NULL;
			}
			if ( wordDes.type == PARSE_WORD_INTEGER )	//整数和引用
			{
				HE_DWORD objNum = 0, genNum = 0;
				objNum = wordDes.str.GetInteger();

				HE_DWORD lPos = GetPos();
				if ( !GetWord( wordDes ) )
				{
					return NULL;
				}
				if ( wordDes.type == PARSE_WORD_INTEGER )
				{
					genNum = wordDes.str.GetInteger();
					if ( !GetWord( wordDes ) )
					{
						return NULL;
					}
					if ( wordDes.str == "R" )
					{
						if ( pCurObj->GetType() == OBJ_TYPE_DICTIONARY )
						{
							((CHE_PDF_Dictionary*)pCurObj)->SetAtReference( key, objNum, GetParser() );
						}else{
							CHE_PDF_Reference * pTmp = CHE_PDF_Reference::Create( objNum, /*m_dwCurObjNum, m_dwCurGenNum,*/ GetParser(), GetAllocator() );
							((CHE_PDF_Array*)pCurObj)->Append( pTmp );
						}
					}else{
						SetPos( lPos );
						if ( pCurObj->GetType() == OBJ_TYPE_DICTIONARY )
						{
							((CHE_PDF_Dictionary*)pCurObj)->SetAtInteger( key, objNum );
						}else{
							CHE_PDF_Number * pTmp = CHE_PDF_Number::Create( (HE_INT32)objNum, /*m_dwCurObjNum, m_dwCurGenNum, GetParser(),*/ GetAllocator() );
							((CHE_PDF_Array*)pCurObj)->Append( pTmp );
						}
					}
				}else{
					SetPos( lPos );
					if ( pCurObj->GetType() == OBJ_TYPE_DICTIONARY )
					{
						((CHE_PDF_Dictionary*)pCurObj)->SetAtInteger( key, objNum );
					}else{
						CHE_PDF_Number * pTmp = CHE_PDF_Number::Create( (HE_INT32)objNum, /*m_dwCurObjNum, m_dwCurGenNum, GetParser(),*/ GetAllocator() );
						((CHE_PDF_Array*)pCurObj)->Append( pTmp );
					}
				}
			}else if ( wordDes.type == PARSE_WORD_FLOAT )	//浮点
			{
				if ( pCurObj->GetType() == OBJ_TYPE_DICTIONARY )
				{
					((CHE_PDF_Dictionary*)pCurObj)->SetAtFloatNumber( key, wordDes.str.GetFloat() );
				}else{
					CHE_PDF_Number * pTmp = CHE_PDF_Number::Create( wordDes.str.GetFloat(), /*m_dwCurObjNum, m_dwCurGenNum, GetParser(),*/ GetAllocator() );
					((CHE_PDF_Array*)pCurObj)->Append( pTmp );
				}
			}else if ( wordDes.type == PARSE_WORD_STRING )	//字符串
			{
				if ( pCurObj->GetType() == OBJ_TYPE_DICTIONARY )
				{
					((CHE_PDF_Dictionary*)pCurObj)->SetAtString( key, wordDes.str );
				}else{
					CHE_PDF_String * pTmp = CHE_PDF_String::Create( wordDes.str, /*m_dwCurObjNum, m_dwCurGenNum, GetParser(),*/ GetAllocator() );
					((CHE_PDF_Array*)pCurObj)->Append( pTmp );
				}
			}else if ( wordDes.type == PARSE_WORD_NAME )
			{
				if ( pCurObj->GetType() == OBJ_TYPE_DICTIONARY )
				{
					((CHE_PDF_Dictionary*)pCurObj)->SetAtName( key, wordDes.str );
				}else{
					CHE_PDF_Name * pTmp = CHE_PDF_Name::Create( wordDes.str, /*m_dwCurObjNum, m_dwCurGenNum, GetParser(),*/ GetAllocator() );
					((CHE_PDF_Array*)pCurObj)->Append( pTmp );
				}
			}else if ( wordDes.type == PARSE_WORD_ARRAY_B )	//数组
			{
				if ( pCurObj->GetType() == OBJ_TYPE_DICTIONARY )
				{
					stackName.Push( key );
				}
				stack.Push( pCurObj );
				pCurObj = CHE_PDF_Array::Create( /*m_dwCurObjNum, m_dwCurGenNum, GetParser(),*/ GetAllocator() );
			}else if ( wordDes.type == PARSE_WORD_ARRAY_E )	//数组
			{
				if ( stack.IsEmpty() == FALSE )
				{
					CHE_PDF_Object * pTmp = pCurObj;
					stack.Pop( pCurObj );
					if ( pCurObj->GetType() == OBJ_TYPE_DICTIONARY )
					{
						stackName.Pop( key );
						((CHE_PDF_Dictionary*)pCurObj)->SetAtArray( key, (CHE_PDF_Array*)pTmp );
					}else{
						((CHE_PDF_Array*)pCurObj)->Append( pTmp );
					}
				}else{
					return (CHE_PDF_Array*)pCurObj;
				}
			}else if ( wordDes.type == PARSE_WORD_DICT_B )	//字典
			{
				if ( pCurObj->GetType() == OBJ_TYPE_DICTIONARY )
				{
					stackName.Push( key );
				}
				stack.Push( pCurObj );
				pCurObj = CHE_PDF_Dictionary::Create( /*m_dwCurObjNum, m_dwCurGenNum, GetParser(),*/ GetAllocator() );
			}else if ( wordDes.type == PARSE_WORD_DICT_E )	//字典	只出现在array的情况
			{
				CHE_PDF_Object * pTmp = pCurObj;
				stack.Pop( pCurObj );
				if ( pCurObj->GetType() == OBJ_TYPE_DICTIONARY )
				{
					stackName.Pop( key );
					pCurObj->ToDict()->SetAtDictionary( key, pTmp->ToDict() );
				}else{
					((CHE_PDF_Array*)pCurObj)->Append( pTmp );
				}
			}else{	//布尔和其他
				if ( wordDes.str == "false" )
				{
					if ( pCurObj->GetType() == OBJ_TYPE_DICTIONARY )
					{
						((CHE_PDF_Dictionary*)pCurObj)->SetAtBoolean( key, false );
					}else{
						CHE_PDF_Boolean * pTmp = CHE_PDF_Boolean::Create( FALSE, /*m_dwCurObjNum, m_dwCurGenNum, GetParser(),*/ GetAllocator() );
						((CHE_PDF_Array*)pCurObj)->Append( pTmp );
					}
				}else if ( wordDes.str == "true" )
				{
					if ( pCurObj->GetType() == OBJ_TYPE_DICTIONARY )
					{
						((CHE_PDF_Dictionary*)pCurObj)->SetAtBoolean( key, true );
					}else{
						CHE_PDF_Boolean * pTmp = CHE_PDF_Boolean::Create( TRUE, /*m_dwCurObjNum, m_dwCurGenNum, GetParser(),*/ GetAllocator() );
						((CHE_PDF_Array*)pCurObj)->Append( pTmp );
					}
				}else{
					return NULL;
				}
			}
		}
		if ( pCurObj->GetType() == OBJ_TYPE_DICTIONARY )
		{
			if ( bKey == TRUE )
			{
				bKey = FALSE;
			}else{
				bKey = TRUE;
			}
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
	CHE_PDF_ParseWordDes wordDes( GetAllocator() );

	GetWord( wordDes );
	if ( wordDes.type != PARSE_WORD_DICT_B )
	{
		return NULL;
	}

	HE_BOOL bKey = TRUE;
	CHE_ByteString key( GetAllocator() );
	CHE_Stack<CHE_PDF_Object*> stack( GetAllocator() );
	CHE_Stack<CHE_ByteString> stackName( GetAllocator() );
	CHE_PDF_Object * pCurObj = CHE_PDF_Dictionary::Create( /*m_dwCurObjNum, m_dwCurGenNum, GetParser(),*/ GetAllocator() );

	while ( TRUE )
	{
		if ( bKey )
		{
			if ( !GetWord( wordDes ) )
			{
				return NULL;
			}
			if ( wordDes.type == PARSE_WORD_NAME )
			{
				key = wordDes.str;
			}else if (  wordDes.type == PARSE_WORD_DICT_E  )
			{
				if ( stack.IsEmpty() == FALSE )
				{
					CHE_PDF_Dictionary * pDict = (CHE_PDF_Dictionary*)pCurObj;
					stack.Pop( pCurObj );
					if ( pCurObj->GetType() == OBJ_TYPE_ARRAY )
					{
						((CHE_PDF_Array*)pCurObj)->Append( pDict );
					}else if ( pCurObj->GetType() == OBJ_TYPE_DICTIONARY )
					{
						stackName.Pop( key );
						((CHE_PDF_Dictionary*)pCurObj)->SetAtDictionary( key, pDict );
					}
				}else{
					return (CHE_PDF_Dictionary*)pCurObj;
				}
				if ( pCurObj->GetType() == OBJ_TYPE_ARRAY )
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
			if ( !GetWord( wordDes ) )
			{
				return NULL;
			}
			if ( wordDes.type == PARSE_WORD_INTEGER )	//整数和引用
			{
				HE_DWORD objNum = 0, genNum = 0;
				objNum = wordDes.str.GetInteger();

				HE_DWORD lPos = GetPos();
				GetWord( wordDes );
				if ( wordDes.type == PARSE_WORD_INTEGER )
				{
					genNum = wordDes.str.GetInteger();
					GetWord( wordDes );
					if ( wordDes.str == "R" )
					{
						if ( pCurObj->GetType() == OBJ_TYPE_DICTIONARY )
						{
							((CHE_PDF_Dictionary*)pCurObj)->SetAtReference( key, objNum, GetParser() );
						}else{
							CHE_PDF_Reference * pTmp = CHE_PDF_Reference::Create( objNum, /*m_dwCurObjNum, m_dwCurGenNum,*/ GetParser(), GetAllocator() );
							((CHE_PDF_Array*)pCurObj)->Append( pTmp );
						}
					}else{
						if ( pCurObj->GetType() == OBJ_TYPE_DICTIONARY )
						{
							return NULL;
						}else{
							CHE_PDF_Number * pTmp = CHE_PDF_Number::Create( (HE_INT32)objNum, /*m_dwCurObjNum, m_dwCurGenNum, GetParser(),*/ GetAllocator() );
							((CHE_PDF_Array*)pCurObj)->Append( pTmp );
							SetPos( lPos );
						}
					}
				}else{
					SetPos( lPos );
					if ( pCurObj->GetType() == OBJ_TYPE_DICTIONARY )
					{
						((CHE_PDF_Dictionary*)pCurObj)->SetAtInteger( key, objNum );
					}else{
						CHE_PDF_Number * pTmp = CHE_PDF_Number::Create( (HE_INT32)objNum, /*m_dwCurObjNum, m_dwCurGenNum, GetParser(),*/ GetAllocator() );
						((CHE_PDF_Array*)pCurObj)->Append( pTmp );
					}
				}
			}else if ( wordDes.type == PARSE_WORD_FLOAT )	//浮点
			{
				if ( pCurObj->GetType() == OBJ_TYPE_DICTIONARY )
				{
					((CHE_PDF_Dictionary*)pCurObj)->SetAtFloatNumber( key, wordDes.str.GetFloat() );
				}else{
					CHE_PDF_Number * pTmp = CHE_PDF_Number::Create( wordDes.str.GetFloat(), /*m_dwCurObjNum, m_dwCurGenNum, GetParser(),*/ GetAllocator() );
					((CHE_PDF_Array*)pCurObj)->Append( pTmp );
				}
			}else if ( wordDes.type == PARSE_WORD_STRING )	//字符串
			{
				if ( pCurObj->GetType() == OBJ_TYPE_DICTIONARY )
				{
					((CHE_PDF_Dictionary*)pCurObj)->SetAtString( key, wordDes.str );
				}else{
					CHE_PDF_String * pTmp = CHE_PDF_String::Create( wordDes.str, /*m_dwCurObjNum, m_dwCurGenNum, GetParser(),*/ GetAllocator() );
					((CHE_PDF_Array*)pCurObj)->Append( pTmp );
				}
			}else if ( wordDes.type == PARSE_WORD_NAME )
			{
				if ( pCurObj->GetType() == OBJ_TYPE_DICTIONARY )
				{
					((CHE_PDF_Dictionary*)pCurObj)->SetAtName( key, wordDes.str );
				}else{
					CHE_PDF_Name * pTmp = CHE_PDF_Name::Create( wordDes.str, /*m_dwCurObjNum, m_dwCurGenNum, GetParser(),*/ GetAllocator() );
					((CHE_PDF_Array*)pCurObj)->Append( pTmp );
				}
			}else if ( wordDes.type == PARSE_WORD_ARRAY_B )	//数组
			{
				if ( pCurObj->GetType() == OBJ_TYPE_DICTIONARY )
				{
					stackName.Push( key );
				}
				stack.Push( pCurObj );
				pCurObj = CHE_PDF_Array::Create( /*m_dwCurObjNum, m_dwCurGenNum, GetParser(),*/ GetAllocator() );
			}else if ( wordDes.type == PARSE_WORD_ARRAY_E )	//数组
			{
				CHE_PDF_Object * pTmp = pCurObj;
				stack.Pop( pCurObj );
				if ( pCurObj->GetType() == OBJ_TYPE_DICTIONARY )
				{
					stackName.Pop( key );
					pCurObj->ToDict()->SetAtArray( key, pTmp->ToArray() );
				}else{
					pCurObj->ToArray()->Append( pTmp );
				}
			}else if ( wordDes.type == PARSE_WORD_DICT_B )	//字典
			{
				if ( pCurObj->GetType() == OBJ_TYPE_DICTIONARY )
				{
					stackName.Push( key );
				}
				stack.Push( pCurObj );
				pCurObj = CHE_PDF_Dictionary::Create( /*m_dwCurObjNum, m_dwCurGenNum, GetParser(),*/ GetAllocator() );
			}else if ( wordDes.type == PARSE_WORD_DICT_E )	//字典	只出现在array的情况
			{
				CHE_PDF_Object * pTmp = pCurObj;
				stack.Pop( pCurObj );
				if ( pCurObj->GetType() == OBJ_TYPE_DICTIONARY )
				{
					stackName.Pop( key );
					pCurObj->ToDict()->SetAtArray( key, pTmp->ToArray() );
				}else{
					pCurObj->ToArray()->Append( pTmp );
				}
			}else{	//布尔和其他
				if ( wordDes.str == "false" )
				{
					if ( pCurObj->GetType() == OBJ_TYPE_DICTIONARY )
					{
						((CHE_PDF_Dictionary*)pCurObj)->SetAtBoolean( key, false );
					}else{
						CHE_PDF_Boolean * pTmp = CHE_PDF_Boolean::Create( FALSE, /*m_dwCurObjNum, m_dwCurGenNum, GetParser(),*/ GetAllocator() );
						((CHE_PDF_Array*)pCurObj)->Append( pTmp );
					}
				}else if ( wordDes.str == "true" )
				{
					if ( pCurObj->GetType() == OBJ_TYPE_DICTIONARY )
					{
						((CHE_PDF_Dictionary*)pCurObj)->SetAtBoolean( key, true );
					}else{
						CHE_PDF_Boolean * pTmp = CHE_PDF_Boolean::Create( TRUE, /*m_dwCurObjNum, m_dwCurGenNum, GetParser(),*/ GetAllocator() );
						((CHE_PDF_Array*)pCurObj)->Append( pTmp );
					}
				}else if ( wordDes.str == "null" ){
					if ( pCurObj->GetType() == OBJ_TYPE_DICTIONARY )
					{
						((CHE_PDF_Dictionary*)pCurObj)->SetAtNull( key );
					}else{
						CHE_PDF_Null * pTmp = CHE_PDF_Null::Create( /*m_dwCurObjNum, m_dwCurGenNum, GetParser(),*/ GetAllocator() );
						((CHE_PDF_Array*)pCurObj)->Append( pTmp );
					}
				}
			}
		}
		if ( pCurObj->GetType() == OBJ_TYPE_DICTIONARY )
		{
			if ( bKey == TRUE )
			{
				bKey = FALSE;
			}else{
				bKey = TRUE;
			}
		}else{
			bKey = FALSE;
		}
	}
	return (CHE_PDF_Dictionary*)pCurObj;
}

HE_VOID CHE_PDF_SyntaxParser::SubmitBufferStr( CHE_ByteString & str )
{
	m_WordBuffer[m_lBufferPos] = '\0';
	str.SetData( m_WordBuffer, m_lBufferPos );
	m_WordBuffer[m_lBufferPos=0] = '\0';
	m_bBegin = TRUE;
	m_bPoint = FALSE;
	m_bSign = FALSE;
}

CHE_PDF_Parser::CHE_PDF_Parser( CHE_Allocator * pAllocator )
	: CHE_Object( pAllocator ), m_sParser( NULL, pAllocator ), m_xrefTable( pAllocator ), /*m_arrObjStm( pAllocator ),*/
	m_objCollector( pAllocator ), m_UpdateObjCollector( pAllocator ) 
{
	m_sParser.SetParser( this );
	m_pIHE_FileRead = NULL;
	m_pStrEncrypt = NULL;
	m_pStmEncrypt = NULL;
	m_pEefEncrypt = NULL;
	m_lStartxref = 0;
	m_lPageCount = 0;
	m_lCurPageIndex = 0;
}

HE_BOOL CHE_PDF_Parser::Open( IHE_Read * file )
{
	if ( file == NULL )
	{
		return FALSE;
	}else{
		m_pIHE_FileRead = file;
		m_sParser.InitParser( file );
		GetStartxref( 1024 );
		ParseXRef();
		m_xrefTable.BuildIndex();
		return TRUE;
	}
}

HE_VOID CHE_PDF_Parser::Close()
{
	m_objCollector.ReleaseObj();
	m_objCollector.Clear();
	m_UpdateObjCollector.Clear();
	m_xrefTable.Clear();
	if ( m_pStmEncrypt )
	{
		GetAllocator()->Delete( m_pStmEncrypt );
		m_pStmEncrypt = NULL;
	}
	if ( m_pIHE_FileRead )
	{
		m_pIHE_FileRead->Release();
		m_pIHE_FileRead = NULL;
	}
	m_lStartxref = 0;
	m_lPageCount = 0;
	m_lCurPageIndex = 0;
	m_pPageObjList.clear();
	m_PageNodeStack.Clear();
}

HE_DWORD CHE_PDF_Parser::GetFileSize() const
{
	if ( m_pIHE_FileRead )
	{
		return m_pIHE_FileRead->GetSize();
	}else{
		return 0;
	}
}

PDF_VERSION CHE_PDF_Parser::GetPDFVersion() const
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
				return (PDF_VERSION)(10 + buffer[7] - '0');
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


CHE_PDF_Dictionary* CHE_PDF_Parser::GetRootDict()
{
	if ( m_xrefTable.GetTrailer() )
	{
		CHE_PDF_Object * pObj =m_xrefTable.GetTrailer()->GetElement( "Root" );
		if ( pObj == NULL )
		{
			return NULL;
		}
		if ( pObj->GetType() == OBJ_TYPE_DICTIONARY )
		{
			return pObj->ToDict();
		}else if ( pObj->GetType() == OBJ_TYPE_REFERENCE )
		{
			pObj = pObj->ToReference()->GetRefObj( OBJ_TYPE_DICTIONARY );
			return pObj->ToDict();
		}
	}
	return NULL;
}

CHE_PDF_Dictionary* CHE_PDF_Parser::GetInfoDict()
{
	if ( m_xrefTable.GetTrailer() )
	{
		CHE_PDF_Object * pObj = m_xrefTable.GetTrailer()->GetElement( "Info" );
		if ( pObj == NULL )
		{
			return NULL;
		}
		if ( pObj->GetType() == OBJ_TYPE_DICTIONARY )
		{
			return pObj->ToDict();
		}else if ( pObj->GetType() == OBJ_TYPE_REFERENCE )
		{
			pObj = pObj->ToReference()->GetRefObj( OBJ_TYPE_DICTIONARY );
			return pObj->ToDict();
		}
	}
	return NULL;
}

CHE_PDF_Array* CHE_PDF_Parser::GetIDArray()
{
	if ( m_xrefTable.GetTrailer() )
	{
		CHE_PDF_Object * pObj = m_xrefTable.GetTrailer()->GetElement( "ID" );
		if ( pObj == NULL )
		{
			return NULL;
		}
		if ( pObj->GetType() == OBJ_TYPE_ARRAY )
		{
			return pObj->ToArray();
		}else if ( pObj->GetType() == OBJ_TYPE_REFERENCE )
		{
			return (CHE_PDF_Array*)( pObj->ToReference()->GetRefObj( OBJ_TYPE_ARRAY ) );
		}
	}
	return NULL;
}

HE_DWORD CHE_PDF_Parser::GetStartxref( HE_DWORD range )
{
	if ( m_pIHE_FileRead )
	{
		unsigned char * pBuffer = GetAllocator()->NewArray<unsigned char>( range );
		if ( range >= m_pIHE_FileRead->GetSize() )
		{
			m_pIHE_FileRead->ReadBlock( pBuffer, 0, m_pIHE_FileRead->GetSize() );
		}else{
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
		if ( pBuffer )
		{
			GetAllocator()->DeleteArray<HE_BYTE>( pBuffer );
			pBuffer = NULL;
		}
		m_lStartxref = iStartXref;
		return iStartXref;
	}
	return 0;
}

HE_DWORD CHE_PDF_Parser::ParseXRef()
{
	if ( m_pIHE_FileRead == NULL )
	{
		return 0;
	}
	if ( m_lStartxref == 0 )
	{
		GetStartxref( m_sParser.GetFileSize() );
		if ( m_lStartxref == 0 )
		{
			return FullParseForXRef();
		}
	}

	CHE_PDF_Dictionary * pDict = NULL;
	HE_DWORD offset = m_lStartxref;
	HE_DWORD xrefEntryCount = 0;
	HE_DWORD entryCount = 0;
	while ( TRUE )
	{
		pDict = NULL;
		entryCount = ParseXRefTable( offset, &pDict );
		xrefEntryCount += entryCount;
		if ( pDict )
		{
			CHE_PDF_Object * pObj = pDict->GetElement( "Encrypt" );
			if ( pObj != NULL && pObj->GetType() == OBJ_TYPE_REFERENCE )
			{
				pObj = pObj->ToReference()->GetRefObj( OBJ_TYPE_DICTIONARY  );
				if ( pObj != NULL )
				{
					ParseEncrypt( pObj->ToDict() );
				}
			}
			pObj = pDict->GetElement( "Prev" );
			if ( pObj != NULL && pObj->GetType() == OBJ_TYPE_NUMBER )
			{
				offset = pObj->ToNumber()->GetInteger();
				continue;
			}
		}
		if ( entryCount == 0 )
		{
			pDict = NULL;
			entryCount = ParseXRefStream( offset, &pDict );
			xrefEntryCount += entryCount;
			if ( pDict )
			{
				CHE_PDF_Object * pObj = pDict->GetElement( "Encrypt" );
				if ( pObj != NULL )
				{
					pObj = pObj->ToReference()->GetRefObj( OBJ_TYPE_DICTIONARY );
					if ( pObj != NULL )
					{
						ParseEncrypt( pObj->ToDict() );
					}
				}
				pObj = pDict->GetElement( "Prev" );
				if ( pObj != NULL && pObj->GetType() == OBJ_TYPE_NUMBER )
				{
					offset = pObj->ToNumber()->GetInteger();
					continue;
				}
			}
		}
		if ( xrefEntryCount == 0 )
		{
			return FullParseForXRef();
		}else{
			break;
		} 
	}
	return xrefEntryCount;
}

HE_BOOL CHE_PDF_Parser::ParseEncrypt( CHE_PDF_Dictionary * pEncryptDict )
{
	if ( pEncryptDict != NULL )
	{
		CHE_PDF_Object * pObj = NULL;
		pObj = pEncryptDict->GetElement( "Filter" );
		if ( pObj == NULL || pObj->GetType() != OBJ_TYPE_NAME )
		{
			return FALSE;
		}
		if ( pObj->ToName()->GetString() == "Standard" )
		{
			CHE_ByteString id1( GetAllocator() );
			HE_BYTE O[32];
			HE_BYTE U[32];
			HE_BYTE keyLength = 40;
			HE_BYTE revision = 2;
			HE_BOOL bMetaData = TRUE;
			HE_DWORD pValue = 0;
			CHE_PDF_Array * pIDArray = GetIDArray();
			if ( pIDArray != NULL )
			{
				pObj = pIDArray->GetElement( 0 );
				if ( pObj != NULL && pObj->GetType() == OBJ_TYPE_STRING )
				{
					id1 = pObj->ToString()->GetString();
				}
			}
			//获取Length
			pObj = pEncryptDict->GetElement( "Length" );
			if ( pObj != NULL && pObj->GetType() == OBJ_TYPE_NUMBER )
			{
				keyLength = pObj->ToNumber()->GetInteger();
			}
			//获取R
			pObj = pEncryptDict->GetElement( "R" );
			if ( pObj != NULL && pObj->GetType() == OBJ_TYPE_NUMBER )
			{
				revision = pObj->ToNumber()->GetInteger();
			}
			//获取P
			pObj = pEncryptDict->GetElement( "P" );
			if ( pObj != NULL && pObj->GetType() == OBJ_TYPE_NUMBER )
			{
				pValue = pObj->ToNumber()->GetInteger();
			}
			//获取EncryptMetadata
			pObj = pEncryptDict->GetElement( "EncryptMetadata" );
			if ( pObj != NULL && pObj->GetType() == OBJ_TYPE_BOOLEAN )
			{
				bMetaData = pObj->ToBoolean()->GetValue();
			}
			//获取U
			pObj = pEncryptDict->GetElement( "U" );
			if ( pObj != NULL && pObj->GetType() == OBJ_TYPE_STRING )
			{
				CHE_ByteString str = pObj->ToString()->GetString();
				for ( HE_DWORD i = 0; i < str.GetLength(); i++ )
				{
					U[i] = str[i];
				}
			}
			//获取O
			pObj = pEncryptDict->GetElement( "O" );
			if ( pObj != NULL && pObj->GetType() == OBJ_TYPE_STRING )
			{
				CHE_ByteString str = pObj->ToString()->GetString();
				for ( HE_DWORD i = 0; i < str.GetLength(); i++ )
				{
					O[i] = str[i];
				}
			}
			pObj = pEncryptDict->GetElement( "V" );
			if ( pObj != NULL && pObj->GetType() == OBJ_TYPE_NUMBER )
			{
				if ( pObj->ToNumber()->GetInteger() == 4 )
				{
					m_pStrEncrypt = GetAllocator()->New<CHE_PDF_Encrypt>( id1, O, U, PDFENCRYPT_ALGORITHM_AESV2, keyLength, revision, bMetaData, pValue, GetAllocator() );
					m_pStmEncrypt = m_pStrEncrypt;
					m_pEefEncrypt = m_pStrEncrypt;
				}else{
					m_pStrEncrypt = GetAllocator()->New<CHE_PDF_Encrypt>( id1, O, U, PDFENCRYPT_ALGORITHM_RC4V1, keyLength, revision, bMetaData, pValue, GetAllocator() );
					m_pStmEncrypt = m_pStrEncrypt;
					m_pEefEncrypt = m_pStrEncrypt;
				}
				m_sParser.SetEncrypt( m_pStrEncrypt );
				m_pStrEncrypt->Authenticate( "" );
			}
			return TRUE;
		}
		//非标准的加密处理，未支持
	}
	return FALSE;
};

HE_DWORD CHE_PDF_Parser::ParseXRefTable( HE_DWORD offset, CHE_PDF_Dictionary ** pTrailerDictRet )
{
	HE_DWORD orgOffset = m_sParser.GetPos();
	m_sParser.SetPos( offset );

	CHE_PDF_ParseWordDes wordDes( GetAllocator() );
	HE_DWORD xrefEntryCount = 0;
	if ( m_sParser.GetWord( wordDes ) == FALSE || wordDes.str != "xref" )
	{
		m_sParser.SetPos( orgOffset );
		return 0; 
	}

	HE_DWORD lBeginNum = 0, lCount = 0;
	HE_DWORD objNum = 0, objGenNum = 0, objOffset = 0;
	HE_BYTE tmpBytes[20];
	while ( true )
	{
		lBeginNum = 0;
		lCount = 0;
		if ( m_sParser.GetWord( wordDes ) == FALSE ) break;
		if( wordDes.type == PARSE_WORD_INTEGER )
		{
			lBeginNum = wordDes.str.GetInteger();
		}else if ( wordDes.type == PARSE_WORD_UNKNOWN && wordDes.str == "trailer" )
		{
			*pTrailerDictRet = m_sParser.GetDictionary();
			m_xrefTable.NewTrailer( *pTrailerDictRet, TRUE );
		}else{
			break;
		}

		if ( m_sParser.GetWord( wordDes ) == FALSE ) break;
		if( wordDes.type == PARSE_WORD_INTEGER )
		{
			lCount = wordDes.str.GetInteger();
		}else{
			break;
		}

		m_sParser.SeekToNextLine();
		m_xrefTable.NewSection( lBeginNum );
		objNum = 0, objGenNum = 0, objOffset = 0;
		for ( HE_DWORD i = lBeginNum; i < lBeginNum + lCount; i++ )
		{
			m_sParser.ReadBytes( tmpBytes, 20 );
			m_sParser.Seek( 20 );

			if ( i == 0 )
			{
				CHE_PDF_XREF_Entry tmpEntry( XREF_ENTRY_TYPE_FREE, 0, 0 );
				m_xrefTable.NewNode( tmpEntry );
				xrefEntryCount++;
			}else{
				if ( tmpBytes[17] != 'f' )
				{
					objOffset =	  (tmpBytes[0] - 48) * 1000000000 + (tmpBytes[1] - 48) * 100000000 + (tmpBytes[2] - 48) * 10000000
						+ (tmpBytes[3] - 48) * 1000000 + (tmpBytes[4] - 48) * 100000 + (tmpBytes[5] - 48) * 10000
						+ (tmpBytes[6] - 48) * 1000 + (tmpBytes[7] - 48) * 100 + (tmpBytes[8] - 48) * 10 + (tmpBytes[9] - 48); 
					objGenNum =	  (tmpBytes[11] - 48) * 10000 + (tmpBytes[12] - 48) * 1000 + (tmpBytes[13] - 48) * 100 + (tmpBytes[14] - 48) * 10 + tmpBytes[15] - 48;
					if ( objOffset != 0 )
					{
						CHE_PDF_XREF_Entry tmpEntry( XREF_ENTRY_TYPE_COMMON, objOffset, objGenNum );
						m_xrefTable.NewNode( tmpEntry );
						xrefEntryCount++;
					}else
					{
						m_xrefTable.SkipNode();
					}
				}else{
					m_xrefTable.SkipNode();
				}
			}
		}
	}
	m_sParser.SetPos( orgOffset );
	return xrefEntryCount;
}

HE_DWORD  CHE_PDF_Parser::ParseXRefStream( HE_DWORD offset, CHE_PDF_Dictionary ** pTrailerDictRet )
{
	HE_DWORD orgOffset = m_sParser.GetPos();
	m_sParser.SetPos( offset );

	CHE_PDF_ParseWordDes wordDes( GetAllocator() );
	HE_DWORD xrefEntryCount = 0;
	HE_DWORD objNum = 0, genNum = 0, lSecBeginNum = 0, lSecObjCount = 0,  lSecCount = 0, lSize = 0, lW1 = 0, lW2 = 0, lW3 = 0;

	CHE_PDF_Object * pStream = GetObject();
	if ( pStream == NULL || pStream->GetType() != OBJ_TYPE_STREAM )
	{
		m_sParser.SetPos( orgOffset );
		return 0;
	}
	CHE_PDF_Dictionary * pDict = pStream->ToStream()->GetDict();
	if ( pDict == NULL )
	{
		m_sParser.SetPos( orgOffset );
		return 0;
	}else{
		*pTrailerDictRet = pDict;
		m_xrefTable.NewTrailer( pDict ); 
	}
	CHE_PDF_Object * pElement =  pDict->GetElement( "Type" );
	if ( pElement == NULL || pElement->GetType() != OBJ_TYPE_NAME || pElement->ToName()->GetString() != "XRef" )
	{
		m_sParser.SetPos( orgOffset );
		return 0;
	}
	pElement = pDict->GetElement( "Size" );
	if ( pElement == NULL || pElement->GetType() != OBJ_TYPE_NUMBER )
	{
		m_sParser.SetPos( orgOffset );
		return 0;
	}
	lSize = pElement->ToNumber()->GetInteger();

	pElement = pDict->GetElement( "W" );
	if ( pElement == NULL || pElement->GetType() != OBJ_TYPE_ARRAY || pElement->ToArray()->GetCount() < 3 )
	{
		m_sParser.SetPos( orgOffset );
		return 0;
	}
	if (	pElement->ToArray()->GetElement( 0 )->GetType() != OBJ_TYPE_NUMBER ||
		pElement->ToArray()->GetElement( 1 )->GetType() != OBJ_TYPE_NUMBER ||
		pElement->ToArray()->GetElement( 2 )->GetType() != OBJ_TYPE_NUMBER )
	{
		m_sParser.SetPos( orgOffset );
		return 0;
	}
	lW1 = pElement->ToArray()->GetElement( 0 )->ToNumber()->GetInteger();
	lW2	= pElement->ToArray()->GetElement( 1 )->ToNumber()->GetInteger();
	lW3	= pElement->ToArray()->GetElement( 2 )->ToNumber()->GetInteger();

	pElement = pDict->GetElement( "Index" );
	if ( pElement == NULL || pElement->GetType() != OBJ_TYPE_ARRAY )
	{
		lSecCount = 1;
	}else{
		if ( pElement->ToArray()->GetCount() < 2 )
		{
			m_sParser.SetPos( orgOffset );
			return 0;
		}
		lSecCount = pElement->ToArray()->GetCount() / 2; 
	}

	CHE_PDF_StreamAcc streamAcc( GetAllocator() );
	if ( streamAcc.Attach( pStream->ToStream() ) == FALSE )
	{
		m_sParser.SetPos( orgOffset );
		return 0;
	}
	HE_DWORD streamSize = streamAcc.GetSize();
	HE_LPCBYTE lpByte = streamAcc.GetData();
	HE_DWORD field1 = 0, field2 = 0, field3 = 0;
	HE_DWORD lBlockCount = 0;
	HE_DWORD lEntrySize = lW1 + lW2 + lW3;
	HE_DWORD lItemOfSecCount = 0;

//  	std::vector<HE_DWORD> XrefVerify1;
//  	std::vector<HE_DWORD> XrefVerify2;

	CHE_SkipList<HE_DWORD> XrefVerify1;
	CHE_SkipList<HE_DWORD> XrefVerify2;

	HE_DWORD lEntryToCheck = 0;

	HE_DWORD *lSecBeginArray = GetAllocator()->NewArray<HE_DWORD>( lSecCount );
	HE_DWORD *lSecObjCountArray = GetAllocator()->NewArray<HE_DWORD>( lSecCount );
	HE_DWORD lEntryCount = 0;
	if ( pElement == NULL )
	{
		lSecBeginArray[0] = 0;
		lSecObjCountArray[0] = lSize;
		lEntryCount = lSize;
	}else{
		CHE_PDF_Array * pIndexArray = pElement->ToArray();
		for ( HE_DWORD i = 0; i < lSecCount; i++ )
		{
			lSecBeginArray[i] = pIndexArray->GetElement( i * 2 )->ToNumber()->GetInteger();
			lSecObjCountArray[i] = pIndexArray->GetElement( i * 2 + 1 )->ToNumber()->GetInteger();
			lEntryCount += lSecObjCountArray[i];
		}
	}

	if ( streamSize / lEntrySize > lEntryCount )
	{
		lEntryToCheck = streamSize / lEntrySize - lEntryCount;
	}else{
		lEntryToCheck = 0;
	}


	for ( HE_DWORD i = 0; i < lSecCount; i++ )
	{
		lSecBeginNum = lSecBeginArray[i];
		lSecObjCount = lSecObjCountArray[i] + lEntryToCheck;

		m_xrefTable.NewSection( lSecBeginNum );
		lItemOfSecCount = lSecObjCount;
		HE_DWORD indexInSec = 0;
		while ( lItemOfSecCount > 0 )
		{
			lItemOfSecCount--;
			if ( ( lBlockCount + 1 ) * lEntrySize > streamSize )
			{
				break;
			}
			lBlockCount++;
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
			for ( HE_DWORD k = 0; k < lW3; k++ )
			{
				field3 = field3 << 8;
				field3 += *lpByte;
				lpByte++;
			}
			switch( field1 )
			{
			case 2:
				{
					HE_DWORD tmpValue = ( field2 << 9 ) + field3;
					if ( XrefVerify2.IsExist( tmpValue ) )
					{
						lEntryToCheck--;
						break;
					}else{
						XrefVerify2.Append( tmpValue );
					}
//					HE_DWORD tmpValue = ( field2 << 9 ) + field3;
// 					bool bRepeat = false;
// 					std::vector<HE_DWORD>::iterator it;
// 					for ( it = XrefVerify2.begin(); it != XrefVerify2.end(); it++ )
// 					{
// 						if ( *it == tmpValue )
// 						{
// 							bRepeat = true;
// 							break;
// 						}
// 					}
// 					if ( bRepeat )
// 					{
// 						lEntryToCheck--;
// 						break;
// 					}else{
// 						XrefVerify2.push_back( tmpValue );
// 					}
					CHE_PDF_XREF_Entry tmpEntry( XREF_ENTRY_TYPE_COMPRESSED, field2, field3 );
					m_xrefTable.NewNode( tmpEntry );
					indexInSec++;
					xrefEntryCount++;
					break;
				}
			case 1:
				{	
					if ( XrefVerify1.IsExist( field2 ) )
					{
						lEntryToCheck--;
						break;
					}else{
						XrefVerify1.Append( field2 );
					}
// 					bool bRepeat = false;
// 					std::vector<HE_DWORD>::iterator it;
// 					for ( it = XrefVerify1.begin(); it != XrefVerify1.end(); it++ )
// 					{
// 						if ( *it == field2 )
// 						{
// 							bRepeat = true;
// 							break;
// 						}
// 					}
// 					if ( bRepeat )
// 					{
// 						lEntryToCheck--;
// 						break;
// 					}else{
// 						XrefVerify1.push_back( field2 );
// 					}
					if ( lEntryToCheck > 0 )
					{
						HE_DWORD offsetSave = m_sParser.GetPos();
						m_sParser.SetPos( field2 );
						m_sParser.GetWord( wordDes );
						m_sParser.SetPos( offsetSave );
						if ( wordDes.type != PARSE_WORD_INTEGER )
						{
							lEntryToCheck--;
							break;
						}else if (	wordDes.type == PARSE_WORD_INTEGER && 
									wordDes.str.GetInteger() != (HE_INT32)(lSecBeginNum + indexInSec)  )
						{
							CHE_PDF_XREF_Entry tmpEntry( XREF_ENTRY_TYPE_COMMON, field2, field3 );
							m_xrefTable.UpdateNode( lSecBeginNum + xrefEntryCount, tmpEntry );
							lEntryToCheck--;
							break;
						}
					}
					CHE_PDF_XREF_Entry tmpEntry( XREF_ENTRY_TYPE_COMMON, field2, field3 );
					m_xrefTable.NewNode( tmpEntry );
					xrefEntryCount++;
					indexInSec++;
					break;
				}
			case 0:
				{
					CHE_PDF_XREF_Entry tmpEntry( XREF_ENTRY_TYPE_FREE, 0, 0 );
					m_xrefTable.NewNode( tmpEntry );
					xrefEntryCount++;
					indexInSec++;
					break;
				}
			}
		}
	}

	GetAllocator()->DeleteArray<HE_DWORD>( lSecBeginArray );
	GetAllocator()->DeleteArray<HE_DWORD>( lSecObjCountArray );

	lSecBeginArray = NULL;
	lSecObjCountArray = NULL;
	streamAcc.Detach();
	m_sParser.SetPos( orgOffset );
	return xrefEntryCount;
}


HE_DWORD CHE_PDF_Parser::FullParseForXRef()	//分析整个文件来获取对象信息 // 还需要添加搜索xref的功能，针对某些具有xref但startxref错误的pdf文件
{
	CHE_PDF_ParseWordDes	wordDes( GetAllocator() );
	HE_DWORD				xrefEntryCount = 0;
	HE_DWORD				offset = 0;
	HE_DWORD				objNumOffset = 0;
	HE_DWORD				objNum = 0;

	m_sParser.SetPos( 0 );
	while ( true )
	{
		if ( m_sParser.GetPos() == m_sParser.GetFileSize() )
		{
			break;
		}
		if ( m_sParser.GetWord( wordDes ) == FALSE )
		{
			break;
		}
		if ( wordDes.str == "trailer" )
		{
			m_xrefTable.NewTrailer( m_sParser.GetDictionary() ); 
			continue;
		}
		if ( wordDes.str == "obj" )
		{
			offset = m_sParser.GetPos();
			m_sParser.SeekToPrevWord();
			m_sParser.SeekToPrevWord();
			m_sParser.SeekToPrevWord();

			objNumOffset = m_sParser.GetPos();
			m_sParser.GetWord( wordDes );
			if ( wordDes.type != PARSE_WORD_INTEGER )
			{
				m_sParser.SetPos( offset );
				continue;
			}
			objNum = wordDes.str.GetInteger();
			m_sParser.GetWord( wordDes );
			if ( wordDes.type != PARSE_WORD_INTEGER )
			{
				m_sParser.SetPos( offset );
				continue;
			}
			m_xrefTable.NewSection( objNum );
			CHE_PDF_XREF_Entry tmpEntry( XREF_ENTRY_TYPE_COMMON, objNumOffset, 0 );
			m_xrefTable.NewNode( tmpEntry );
			xrefEntryCount++;
			
			//seek到下一个obj
			//某些错误文件缺少endobj
			m_sParser.GetWord( wordDes );
			while( m_sParser.GetWord( wordDes ) )
			{
				if ( wordDes.str == "stream" )
				{
					while( TRUE )
					{
						m_sParser.SeekToNextLine();
						HE_BYTE tmp[9];
						tmp[8] = '\0';
						if ( m_sParser.ReadBytes( tmp, 9 ) == 0 )
						{
							return xrefEntryCount;
						}else{
							if (	tmp[0] == 'e' && tmp[1] == 'n' && tmp[2] == 'd' && tmp[3] == 's' && tmp[4] == 't'
								 && tmp[5] == 'r' && tmp[6] == 'e' && tmp[7] == 'a' && tmp[8] == 'm')
							{
								break;
							}
						}
					}
					break;
				}else if ( wordDes.str == "<<" )
				{
					m_sParser.SeekToPrevWord();
					CHE_PDF_Dictionary * pDict = m_sParser.GetDictionary();
					if ( pDict != NULL )
					{
						pDict->Release();
					}
					if ( m_sParser.GetWord( wordDes ) == FALSE )
					{
						return xrefEntryCount;
					}else if ( wordDes.str == "stream" )
					{
						m_sParser.SeekToPrevWord();
						continue;
					}else{
						break;
					}
				}
				break;
			}
		}
	}
	m_xrefTable.BuildIndex();
	return xrefEntryCount;
}


HE_DWORD CHE_PDF_Parser::GetPageCount()
{
	if ( m_lPageCount != 0 )
	{
		return m_lPageCount;
	}

	CHE_PDF_Dictionary * pDict = GetRootDict();
	if ( pDict == NULL )
	{
		return 0;
	}
	CHE_PDF_Object * pPagesRef = pDict->GetElement( "Pages" );
	if ( pPagesRef == NULL || pPagesRef->GetType() != OBJ_TYPE_REFERENCE )
	{
		return 0;
	}
	CHE_PDF_Object * pObj = pPagesRef->ToReference()->GetRefObj( OBJ_TYPE_DICTIONARY );
	if ( pObj == NULL )
	{
		return 0;
	}
	pDict = pObj->ToDict();
	if ( pDict == NULL )
	{
		return 0;
	}
	pObj =  pDict->GetElement( "Count", OBJ_TYPE_NUMBER );
	if ( pObj == NULL )
	{
		return 0;
	}
	m_lPageCount = pObj->ToNumber()->GetInteger();
	
	pObj = pDict->GetElement( "Kids", OBJ_TYPE_ARRAY );
	if ( pObj == NULL )
	{
		return 0;
	}
	CHE_PDF_Array * pKidArray = pObj->ToArray();
	if ( pKidArray != NULL )
	{
		if ( pKidArray->GetCount() == m_lPageCount )
		{
			CHE_PDF_Reference * pRef = NULL;
			for ( HE_DWORD i = 0; i < m_lPageCount; i++ )
			{
				pObj = pKidArray->GetElement( i );
				if ( pObj && pObj->GetType() == OBJ_TYPE_REFERENCE )
				{
					pRef = pObj->ToReference();
					m_pPageObjList.push_back( pRef->GetRefNum() );
				}
			}
			m_lCurPageIndex = m_lPageCount-1;
		}else{
			m_PageNodeStack.Push( pPagesRef->ToReference() );
			m_lCurPageIndex = 0;
		}
	}
	return m_lPageCount;
}

HE_DWORD CHE_PDF_Parser::GetPageObjNum( HE_DWORD pageIndex )
{
	if ( pageIndex >= m_lPageCount )
	{
		return 0;
	}
	if ( m_pPageObjList.size() > pageIndex && m_pPageObjList[pageIndex] != 0 )
	{
		return m_pPageObjList[pageIndex];
	}else{
		CHE_PDF_Object * pObj = NULL;
		CHE_PDF_Reference * pRef = NULL;
		CHE_PDF_Dictionary * pDict = NULL;
		CHE_PDF_Array * pArray = NULL;
		CHE_PDF_Name * pName = NULL;
		while ( m_PageNodeStack.Pop( pRef ) == TRUE )
		{
			pObj = pRef->GetRefObj( OBJ_TYPE_DICTIONARY );
			if ( pObj )
			{ 
				pDict = pObj->ToDict();
				pObj = pDict->GetElement( "Type", OBJ_TYPE_NAME );
				if ( pObj == NULL )
				{
					return 0;
				}
				pName = pObj->ToName();
				if ( pName->GetString() == "Page" )
				{
					m_pPageObjList.push_back( pRef->GetRefNum() );
					m_lCurPageIndex++;
					if ( m_lCurPageIndex-1 == pageIndex )
					{
						return pRef->GetRefNum();
					}
				}else if ( pName->GetString() == "Pages" )
				{
					pObj = pDict->GetElement( "Kids", OBJ_TYPE_ARRAY );
					if ( pObj )
					{
						pArray = pObj->ToArray();
						for ( HE_DWORD i = pArray->GetCount(); i > 0; i-- )
						{
							pObj = pArray->GetElement( i-1, OBJ_TYPE_REFERENCE );
							if ( pObj )
							{
								m_PageNodeStack.Push( pObj->ToReference() );
							}
						}
					}
				}
			}
		}
	}
	return 0;
}

CHE_PDF_Object * CHE_PDF_Parser::GetObject()
{
	if ( m_pIHE_FileRead == NULL )
	{
		return NULL;
	}

	HE_DWORD offset = m_sParser.GetPos();
	CHE_PDF_ParseWordDes wordDes( GetAllocator() );
	HE_DWORD objNum = 0, genNum = 0;
	m_sParser.GetWord( wordDes );
	if ( wordDes.type == PARSE_WORD_INTEGER )
	{
		objNum = wordDes.str.GetInteger();
	}else{
		return NULL;
	}
	m_sParser.GetWord( wordDes );
	if ( wordDes.type == PARSE_WORD_INTEGER )
	{
		genNum = wordDes.str.GetInteger();
	}else{
		return NULL;
	}
	m_sParser.GetWord( wordDes );
	if ( wordDes.str != "obj" )
	{
		return NULL;
	}

	m_sParser.SetCurObjNum( objNum );
	m_sParser.SetCurGenNum( genNum );

	HE_DWORD pos = m_sParser.GetPos();
	m_sParser.GetWord( wordDes );
	CHE_PDF_Object * pCurObj = NULL;

	switch ( wordDes.type )
	{
	case PARSE_WORD_INTEGER:
		{
			HE_DWORD refNum = wordDes.str.GetInteger();
			HE_DWORD tmpPos = m_sParser.GetPos();
			m_sParser.GetWord( wordDes );
			if ( wordDes.type == PARSE_WORD_INTEGER )
			{
				m_sParser.GetWord( wordDes );
				if ( wordDes.str == "R" )
				{
					pCurObj = CHE_PDF_Reference::Create( refNum, this, GetAllocator() );
					break;
				}
			}
			m_sParser.SetPos( tmpPos );
			pCurObj = CHE_PDF_Number::Create( (HE_INT32)refNum, /*objNum, genNum, this,*/ GetAllocator() );
			break;
		}
	case PARSE_WORD_FLOAT:
		{
			pCurObj = CHE_PDF_Number::Create( wordDes.str.GetFloat(), GetAllocator() );
			break;
		}
	case PARSE_WORD_STRING:
		{
			pCurObj = CHE_PDF_String::Create( wordDes.str, GetAllocator() );
			break;
		}
	case PARSE_WORD_NAME:
		{
			pCurObj = CHE_PDF_Name::Create( wordDes.str, GetAllocator() );
			break;
		}
	case PARSE_WORD_ARRAY_B:
		{
			m_sParser.SetPos( pos );
			pCurObj = m_sParser.GetArray();
			break;
		}
	case PARSE_WORD_DICT_B:
		{
			m_sParser.SetPos( pos );
			pCurObj = m_sParser.GetDictionary();
			if ( pCurObj == NULL )
			{
				break;
			}
			HE_DWORD tmpPos = m_sParser.GetPos();
			m_sParser.GetWord( wordDes );
			if ( wordDes.str == "stream" )
			{
				HE_DWORD length = 0;
				CHE_PDF_Object * pObj = pCurObj->ToDict()->GetElement( "Length" );
				if ( pObj == NULL )
				{
					length = 0;
				}else if ( pObj->GetType() == OBJ_TYPE_NUMBER || pObj->GetType() == OBJ_TYPE_REFERENCE )
				{
					if ( pObj->GetType() == OBJ_TYPE_NUMBER )
					{
						length = pObj->ToNumber()->GetInteger();
					}else{
						HE_DWORD objNum = pObj->ToReference()->GetRefNum();
						HE_DWORD offset = m_sParser.GetPos();
						CHE_PDF_XREF_Entry entry;
						m_xrefTable.GetEntry( objNum, entry );
						m_sParser.SetPos( entry.GetOffset() );
						CHE_PDF_Object * pObj = GetObject();
						if ( pObj )
						{
							if ( pObj->GetType() == OBJ_TYPE_NUMBER )
							{
								length = pObj->ToNumber()->GetInteger();
							}
						}
						m_sParser.SetPos( offset );
					}
					m_sParser.SeekToNextLine();
					offset = m_sParser.GetPos();
					if ( length == 0 )
					{
						m_sParser.SeekToEndStream();
						if ( m_sParser.GetPos() > offset )
						{
							length = m_sParser.GetPos() - offset + 1;
						}
						m_sParser.SeekToNextLine();
					}else{	
						m_sParser.Seek( length );
					}
					m_sParser.GetWord( wordDes );
					if ( wordDes.str == "endstream" )
					{
						pCurObj = CHE_PDF_Stream::Create( m_pIHE_FileRead, offset, length, pCurObj->ToDict(), objNum, genNum, m_pStmEncrypt, /*this,*/ GetAllocator() );
					}
				}
			}else if (	wordDes.str == "endobj" || 
						(	wordDes.str[0] == 'e' && wordDes.str[1] == 'n' && wordDes.str[2] == 'd' &&
							wordDes.str[3] == 'o' && wordDes.str[4] == 'b' && wordDes.str[5] == 'j' ) )
			{
				if ( pCurObj )
				{
					CHE_PDF_IndirectObject * pInObj = GetAllocator()->New<CHE_PDF_IndirectObject>( objNum, genNum, pCurObj, this, GetAllocator() );
					m_objCollector.Add( pInObj );
					return pCurObj;
				}
			}
			break;
		}
	case PARSE_WORD_UNKNOWN:
		{
			if ( wordDes.str == "false" )
			{
				pCurObj = CHE_PDF_Boolean::Create( FALSE, GetAllocator() );
			}else if ( wordDes.str == "true" )
			{
				pCurObj = CHE_PDF_Boolean::Create( TRUE, GetAllocator() );
			}else if ( wordDes.str == "null" )
			{
				pCurObj = CHE_PDF_Null::Create( GetAllocator() );
			}else if ( wordDes.str == "endobj" )
			{
				pCurObj = CHE_PDF_Null::Create( GetAllocator() );
				CHE_PDF_IndirectObject * pInObj = GetAllocator()->New<CHE_PDF_IndirectObject>( objNum, genNum, pCurObj, this, GetAllocator() );
				m_objCollector.Add( pInObj );
				return pCurObj;
			}
			break;
		}
	}

	m_sParser.GetWord( wordDes );
	if (	wordDes.str == "endobj" || 
			(	wordDes.str[0] == 'e' && wordDes.str[1] == 'n' && wordDes.str[2] == 'd' &&
				wordDes.str[3] == 'o' && wordDes.str[4] == 'b' && wordDes.str[5] == 'j' ) )
	{
		if ( pCurObj )
		{
			CHE_PDF_IndirectObject * pInObj = GetAllocator()->New<CHE_PDF_IndirectObject>( objNum, genNum, pCurObj, this, GetAllocator() );
			m_objCollector.Add( pInObj );
			return pCurObj;
		}else{
			return NULL;
		}
	}
	if ( pCurObj )
	{
		pCurObj->Release();
	}
	return NULL;
}

CHE_PDF_Object * CHE_PDF_Parser::GetObject( HE_DWORD objNum )
{
	if ( m_pIHE_FileRead == NULL )
	{
		return NULL;
	}
	if ( m_objCollector.IsExist( objNum ) )
	{
		CHE_PDF_IndirectObject * pTmpInObj = m_objCollector.GetInObj( objNum );
		if ( pTmpInObj )
		{
			return pTmpInObj->GetObj();
		}
	}
	
	CHE_PDF_XREF_Entry entry;
	if ( m_xrefTable.GetEntry( objNum, entry ) )
	{
		if ( entry.GetType() == XREF_ENTRY_TYPE_COMMON )
		{
			m_sParser.SetPos( entry.GetOffset() );
			return GetObject();
		}else{
			return GetObjectInObjStm( entry.GetParentObjNum(), objNum, entry.GetIndex() );
		}
	}else{
		return NULL;
	}
}

CHE_PDF_Object * CHE_PDF_Parser::GetObjectInObjStm( HE_DWORD stmObjNum, HE_DWORD objNum, HE_DWORD index )
{
	if ( m_pIHE_FileRead == NULL )
	{
		return NULL;
	}
	if ( m_objCollector.IsExist( objNum ) )
	{
		CHE_PDF_IndirectObject * pTmpInObj = m_objCollector.GetInObj( objNum );
		if ( pTmpInObj )
		{
			return pTmpInObj->GetObj();
		}
	}
	CHE_PDF_XREF_Entry entry;
	if ( m_xrefTable.GetEntry( stmObjNum, entry ) )
	{
		CHE_PDF_Object * pObj = NULL;
		if ( entry.GetType() == XREF_ENTRY_TYPE_COMMON )
		{
			m_sParser.SetPos( entry.GetOffset() ); 
			pObj = GetObject();
		}else{
			return NULL;
		}
		if ( pObj == NULL )
		{
			return NULL;
		}

		if ( pObj->GetType() != OBJ_TYPE_STREAM )
		{
			return NULL;
		}else{
			CHE_PDF_StreamAcc stmAcc( GetAllocator() );
			stmAcc.Attach( pObj->ToStream() );
			HE_LPCBYTE pData = stmAcc.GetData();
			HE_DWORD lDataSize = stmAcc.GetSize();
			IHE_Read * pIHE_Read = HE_CreateMemBufRead( (HE_LPBYTE)pData, lDataSize, GetAllocator() );
			if ( pIHE_Read == NULL )
			{
				stmAcc.Detach();
				return NULL;
			}
			CHE_PDF_SyntaxParser sParser( this, GetAllocator() );
			sParser.InitParser( pIHE_Read );
			sParser.SetCurObjNum( objNum );
			sParser.SetCurGenNum( 0 );
			
			CHE_PDF_ParseWordDes	wordDes( GetAllocator() );
			HE_DWORD				first = 0;
			HE_DWORD				offset = 0;
			HE_DWORD				objNumOffset = 0;
			HE_DWORD				objNumTmp = 0;
			sParser.SetPos( 0 );

			first = pObj->ToStream()->GetDict()->GetElement( "First" )->ToNumber()->GetInteger();

			for ( HE_DWORD i = 0; i < index; i++ )
			{
				if ( !sParser.GetWord( wordDes ) )
				{
					return NULL;
				}
				if ( !sParser.GetWord( wordDes ) )
				{
					return NULL;
				}
			}
			while ( true )
			{
				if ( sParser.GetPos() == sParser.GetFileSize() )
				{
					break;
				}
				if ( sParser.GetWord( wordDes ) == FALSE )
				{
					break;
				}
				if ( wordDes.type == PARSE_WORD_INTEGER )
				{
					offset = sParser.GetPos();

					objNumTmp = wordDes.str.GetInteger();

					sParser.GetWord( wordDes );
					if ( wordDes.type != PARSE_WORD_INTEGER )
					{
						sParser.SetPos( offset );
						continue;
					}
					first += wordDes.str.GetInteger();
					if ( objNumTmp == objNum )
					{
						sParser.SetPos( first );
						HE_DWORD pos = sParser.GetPos();
						sParser.GetWord( wordDes );
						CHE_PDF_Object * pCurObj = NULL;
						if ( wordDes.type == PARSE_WORD_DICT_B )
						{
							sParser.SetPos( pos );
							pCurObj = sParser.GetDictionary();
						}else if ( wordDes.type == PARSE_WORD_FLOAT )
						{
							pCurObj = CHE_PDF_Number::Create( wordDes.str.GetInteger(), GetAllocator() );
						}else if ( wordDes.type == PARSE_WORD_INTEGER )
						{
							HE_DWORD refNum = wordDes.str.GetInteger();
							HE_DWORD pos = sParser.GetPos();
							sParser.GetWord( wordDes );
							if ( wordDes.type == PARSE_WORD_INTEGER )
							{
								sParser.GetWord( wordDes );
								if ( wordDes.str == "R" )
								{
									pCurObj = CHE_PDF_Reference::Create( refNum, this, GetAllocator() );
								}else{
									pCurObj = CHE_PDF_Number::Create( (HE_INT32)refNum, GetAllocator() );
								}
							}else{
								pCurObj = CHE_PDF_Number::Create( (HE_INT32)refNum, GetAllocator() );
							}
							sParser.SetPos( pos );
						}else if ( wordDes.type == PARSE_WORD_STRING )
						{
							pCurObj = CHE_PDF_String::Create( wordDes.str, GetAllocator() );
						}else if ( wordDes.type == PARSE_WORD_NAME )
						{
							pCurObj = CHE_PDF_Name::Create( wordDes.str, GetAllocator() );
						}else if ( wordDes.type == PARSE_WORD_ARRAY_B )
						{
							sParser.SetPos( pos );
							pCurObj = sParser.GetArray();
						}else if ( wordDes.str == "false" )
						{
							pCurObj = CHE_PDF_Boolean::Create( FALSE, GetAllocator() );
						}else if ( wordDes.str == "true" )
						{
							pCurObj = CHE_PDF_Boolean::Create( TRUE, GetAllocator() );
						}else if ( wordDes.str == "null" )
						{
							pCurObj = CHE_PDF_Null::Create( GetAllocator() );
						}else{
							pCurObj = NULL;
						}

						if ( pCurObj != NULL )
						{
							CHE_PDF_IndirectObject * pInObj = GetAllocator()->New<CHE_PDF_IndirectObject>( objNum, 0, pCurObj, this, GetAllocator() );
							m_objCollector.Add( pInObj );
						}
						return pCurObj;
					}else{
						return NULL;
					}
				}
			}
		}
	}
	return NULL;
}

PDF_OBJ_STATUS	CHE_PDF_Parser::GetObjectStatus( HE_DWORD objNum )
{
	CHE_PDF_IndirectObject * pInObj = m_UpdateObjCollector.GetInObj( objNum );
	if ( pInObj )
	{
		return OBJ_STATUS_UPDATE;
	}
	pInObj = m_objCollector.GetInObj( objNum );
	if ( pInObj )
	{
		return OBJ_STATUS_LOAD;
	}
	return OBJ_STATUS_UNLOAD;
}

CHE_PDF_IndirectObject * CHE_PDF_Parser::CreateInObject( PDF_OBJ_TYPE type )
{
	CHE_PDF_IndirectObject * pInObj = NULL;
	CHE_PDF_Object * pObj = NULL;
	switch ( type )
	{
	case OBJ_TYPE_INVALID:
		{
			return NULL;
		}
	case OBJ_TYPE_NULL:
		{
			pObj = CHE_PDF_Null::Create( GetAllocator() );
			pInObj = GetAllocator()->New<CHE_PDF_IndirectObject>( GetFreeObjNum(), 0, pObj, this, GetAllocator() );
			break;
		}
	case OBJ_TYPE_BOOLEAN:
		{
			pObj = CHE_PDF_Boolean::Create( FALSE, GetAllocator() );
			pInObj = GetAllocator()->New<CHE_PDF_IndirectObject>( GetFreeObjNum(), 0, pObj, this,GetAllocator() );
			break;
		}
	case OBJ_TYPE_NUMBER:
		{
			pObj = CHE_PDF_Number::Create( (HE_INT32)0, GetAllocator() );
			pInObj = GetAllocator()->New<CHE_PDF_IndirectObject>( GetFreeObjNum(), 0, pObj, this, GetAllocator() );
			break;
		}
	case OBJ_TYPE_STRING:
		{
			pObj = CHE_PDF_String::Create( "", GetAllocator() );
			pInObj = GetAllocator()->New<CHE_PDF_IndirectObject>( GetFreeObjNum(), 0, pObj, this, GetAllocator() );
			break;
		}
	case OBJ_TYPE_NAME:
		{
			pObj = CHE_PDF_Name::Create( "", GetAllocator() );
			pInObj = GetAllocator()->New<CHE_PDF_IndirectObject>( GetFreeObjNum(), 0, pObj, this, GetAllocator() );
			break;
		}
	case OBJ_TYPE_REFERENCE:
		{
			pObj = CHE_PDF_Reference::Create( 0, this, GetAllocator() );
			pInObj = GetAllocator()->New<CHE_PDF_IndirectObject>( GetFreeObjNum(), 0, pObj, this, GetAllocator() );
			break;
		}
	case OBJ_TYPE_ARRAY:
		{
			pObj = CHE_PDF_Array::Create( GetAllocator() );
			pInObj = GetAllocator()->New<CHE_PDF_IndirectObject>( GetFreeObjNum(), 0, pObj, this, GetAllocator() );
			break;
		}
	case OBJ_TYPE_DICTIONARY:
		{
			pObj = CHE_PDF_Dictionary::Create( GetAllocator() );
			pInObj = GetAllocator()->New<CHE_PDF_IndirectObject>( GetFreeObjNum(), 0, pObj, this, GetAllocator() );
			break;
		}
	case OBJ_TYPE_STREAM:
		{
			HE_DWORD tmpObjNum = GetFreeObjNum();
			pObj = CHE_PDF_Stream::Create( tmpObjNum, 0, NULL, GetAllocator() );
			pInObj = GetAllocator()->New<CHE_PDF_IndirectObject>( tmpObjNum, 0, pObj, this, GetAllocator() );
			break;
		}
	default:
		{
			return NULL;
		}
	}
	if ( pInObj )
	{
		m_UpdateObjCollector.Add( pInObj );
		m_objCollector.Add( pInObj );
	}
	return pInObj;
}


HE_BOOL CHE_PDF_Parser::SetInObjUpdated( HE_DWORD objNum )
{
	CHE_PDF_IndirectObject * pInObj = m_objCollector.GetInObj( objNum );
	if ( pInObj )
	{
		m_UpdateObjCollector.Add( pInObj );
		return TRUE;
	}else{
		CHE_PDF_Object * pObj = GetObject( objNum );
		if ( pObj == NULL )
		{
			return FALSE;
		}
		return TRUE;
	}
	return FALSE;
}

HE_DWORD CHE_PDF_Parser::GetUpdatedInObjCount()
{
	return m_UpdateObjCollector.GetCount();
}

CHE_PDF_IndirectObject * CHE_PDF_Parser::GetUpdatedInObj( HE_DWORD index )
{
	return m_UpdateObjCollector.GetObjByIndex( index );
}

HE_DWORD CHE_PDF_Parser::GetFreeObjNum()
{
	m_xrefTable.m_lMaxObjNum++;
	return m_xrefTable.m_lMaxObjNum;
}