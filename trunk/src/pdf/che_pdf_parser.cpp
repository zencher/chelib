#include "../../include/pdf/che_pdf_parser.h"
#include "../../include/pdf/che_pdf_encrypt.h"
#include "../../include/che_datastructure.h"

CHE_PDF_SyntaxParser::CHE_PDF_SyntaxParser( CHE_PDF_File * pFile, CHE_Allocator * pAllocator ) : CHE_Object( pAllocator )
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

	mpFile = pFile;
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

CHE_PDF_ArrayPtr CHE_PDF_SyntaxParser::GetArray()
{
	if ( m_pFileAccess == NULL )
	{
		return CHE_PDF_ArrayPtr();
	}

	CHE_PDF_ParseWordDes wordDes( GetAllocator() );

	if ( !GetWord( wordDes ) )
	{
		return CHE_PDF_ArrayPtr();
	}
	if ( wordDes.type != PARSE_WORD_ARRAY_B )
	{
		return CHE_PDF_ArrayPtr();
	}

	HE_BOOL bKey = FALSE;
	CHE_ByteString key( GetAllocator() );
	CHE_Stack<CHE_PDF_ObjectPtr> stack( GetAllocator() );
	CHE_Stack<CHE_ByteString> stackName( GetAllocator() );
	CHE_PDF_ObjectPtr pCurObj = CHE_PDF_Array::Create( GetAllocator() );
	
	while ( TRUE )
	{
		if ( bKey )
		{
			if ( !GetWord( wordDes ) )
			{
				return CHE_PDF_ArrayPtr();
			}
			if ( wordDes.type == PARSE_WORD_NAME )
			{
				key = wordDes.str;
			}else if ( wordDes.type == PARSE_WORD_DICT_E  )
			{
				if ( stack.IsEmpty() == FALSE )
				{
					CHE_PDF_DictionaryPtr pDict = pCurObj.GetDictPtr();
					stack.Pop( pCurObj );
					if ( pCurObj->GetType() == OBJ_TYPE_ARRAY )
					{
						pCurObj->GetArray()->Append( pDict );
						//((CHE_PDF_Array*)pCurObj)->Append( pDict );
					}else if ( pCurObj->GetType() == OBJ_TYPE_DICTIONARY )
					{
						stackName.Pop( key );
						pCurObj->GetDict()->SetAtDictionary( key, pDict );
						//((CHE_PDF_Dictionary*)pCurObj)->SetAtDictionary( key, pDict );
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
				return CHE_PDF_ArrayPtr();
			}
			if ( wordDes.type == PARSE_WORD_INTEGER )	//整数和引用
			{
				HE_DWORD objNum = 0, genNum = 0;
				objNum = wordDes.str.GetInteger();

				HE_DWORD lPos = GetPos();
				if ( !GetWord( wordDes ) )
				{
					return CHE_PDF_ArrayPtr();
				}
				if ( wordDes.type == PARSE_WORD_INTEGER )
				{
					genNum = wordDes.str.GetInteger();
					if ( !GetWord( wordDes ) )
					{
						return CHE_PDF_ArrayPtr();
					}
					if ( wordDes.str == "R" )
					{
						if ( pCurObj->GetType() == OBJ_TYPE_DICTIONARY )
						{
							pCurObj->GetDict()->SetAtReference( key, objNum, genNum, GetFile() );
						}else{
							CHE_PDF_ReferencePtr pTmp = CHE_PDF_Reference::Create( objNum, genNum, GetFile(), GetAllocator() );
							pCurObj->GetArray()->Append( pTmp );
						}
					}else{
						SetPos( lPos );
						if ( pCurObj->GetType() == OBJ_TYPE_DICTIONARY )
						{
							pCurObj->GetDict()->SetAtInteger( key, objNum );
						}else{
							CHE_PDF_NumberPtr pTmp = CHE_PDF_Number::Create( (HE_INT32)objNum, GetAllocator() );
							pCurObj->GetArray()->Append( pTmp );
						}
					}
				}else{
					SetPos( lPos );
					if ( pCurObj->GetType() == OBJ_TYPE_DICTIONARY )
					{
						pCurObj->GetDict()->SetAtInteger( key, objNum );
					}else{
						CHE_PDF_NumberPtr pTmp = CHE_PDF_Number::Create( (HE_INT32)objNum, GetAllocator() );
						pCurObj->GetArray()->Append( pTmp );
					}
				}
			}else if ( wordDes.type == PARSE_WORD_FLOAT )	//浮点
			{
				if ( pCurObj->GetType() == OBJ_TYPE_DICTIONARY )
				{
					pCurObj.GetDictPtr()->SetAtFloatNumber( key, wordDes.str.GetFloat() );
				}else{
					CHE_PDF_NumberPtr pTmp = CHE_PDF_Number::Create( wordDes.str.GetFloat(), GetAllocator() );
					pCurObj->GetArray()->Append( pTmp );
				}
			}else if ( wordDes.type == PARSE_WORD_STRING )	//字符串
			{
				if ( pCurObj->GetType() == OBJ_TYPE_DICTIONARY )
				{
					pCurObj->GetDict()->SetAtString( key, wordDes.str );
				}else{
					CHE_PDF_StringPtr pTmp = CHE_PDF_String::Create( wordDes.str,  GetAllocator() );
					pCurObj->GetArray()->Append( pTmp );
				}
			}else if ( wordDes.type == PARSE_WORD_NAME )
			{
				if ( pCurObj->GetType() == OBJ_TYPE_DICTIONARY )
				{
					pCurObj->GetDict()->SetAtName( key, wordDes.str );
				}else{
					CHE_PDF_NamePtr pTmp = CHE_PDF_Name::Create( wordDes.str, GetAllocator() );
					pCurObj->GetArray()->Append( pTmp );
				}
			}else if ( wordDes.type == PARSE_WORD_ARRAY_B )	//数组
			{
				if ( pCurObj->GetType() == OBJ_TYPE_DICTIONARY )
				{
					stackName.Push( key );
				}
				stack.Push( pCurObj );
				pCurObj = CHE_PDF_Array::Create( GetAllocator() );
			}else if ( wordDes.type == PARSE_WORD_ARRAY_E )	//数组
			{
				if ( stack.IsEmpty() == FALSE )
				{
					CHE_PDF_ObjectPtr pTmp = pCurObj;
					stack.Pop( pCurObj );
					if ( pCurObj->GetType() == OBJ_TYPE_DICTIONARY )
					{
						stackName.Pop( key );
						pCurObj->GetDict()->SetAtObj( key, pTmp );
					}else{
						pCurObj->GetArray()->Append( pTmp );
					}
				}else{
					return pCurObj.GetArrayPtr();
				}
			}else if ( wordDes.type == PARSE_WORD_DICT_B )	//字典
			{
				if ( pCurObj->GetType() == OBJ_TYPE_DICTIONARY )
				{
					stackName.Push( key );
				}
				stack.Push( pCurObj );
				pCurObj = CHE_PDF_Dictionary::Create( GetAllocator() );
			}else if ( wordDes.type == PARSE_WORD_DICT_E )	//字典	只出现在array的情况
			{
				CHE_PDF_ObjectPtr pTmp = pCurObj;
				stack.Pop( pCurObj );
				if ( pCurObj->GetType() == OBJ_TYPE_DICTIONARY )
				{
					stackName.Pop( key );
					pCurObj->GetDict()->SetAtObj( key, pTmp );
				}else{
					pCurObj->GetArray()->Append( pTmp );
				}
			}else{	//布尔和其他
				if ( wordDes.str == "false" )
				{
					if ( pCurObj->GetType() == OBJ_TYPE_DICTIONARY )
					{
						pCurObj->GetDict()->SetAtBoolean( key, false );
					}else{
						CHE_PDF_BooleanPtr pTmp = CHE_PDF_Boolean::Create( FALSE, GetAllocator() );
						pCurObj->GetArray()->Append( pTmp );
					}
				}else if ( wordDes.str == "true" )
				{
					if ( pCurObj->GetType() == OBJ_TYPE_DICTIONARY )
					{
						pCurObj->GetDict()->SetAtBoolean( key, true );
					}else{
						CHE_PDF_BooleanPtr pTmp = CHE_PDF_Boolean::Create( TRUE, GetAllocator() );
						pCurObj->GetArray()->Append( pTmp );
					}
				}else{
					return CHE_PDF_ArrayPtr();
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
	return pCurObj.GetArrayPtr();
}

CHE_PDF_DictionaryPtr CHE_PDF_SyntaxParser::GetDictionary()
{
	if ( m_pFileAccess == NULL )
	{
		return CHE_PDF_DictionaryPtr();
	}
	CHE_PDF_ParseWordDes wordDes( GetAllocator() );

	GetWord( wordDes );
	if ( wordDes.type != PARSE_WORD_DICT_B )
	{
		return CHE_PDF_DictionaryPtr();
	}

	HE_BOOL bKey = TRUE;
	CHE_ByteString key( GetAllocator() );
	CHE_Stack<CHE_PDF_ObjectPtr> stack( GetAllocator() );
	CHE_Stack<CHE_ByteString> stackName( GetAllocator() );
	CHE_PDF_ObjectPtr pCurObj = CHE_PDF_Dictionary::Create( GetAllocator() );

	while ( TRUE )
	{
		if ( bKey )
		{
			if ( !GetWord( wordDes ) )
			{
				return CHE_PDF_DictionaryPtr();
			}
			if ( wordDes.type == PARSE_WORD_NAME )
			{
				key = wordDes.str;
			}else if (  wordDes.type == PARSE_WORD_DICT_E  )
			{
				if ( stack.IsEmpty() == FALSE )
				{
					CHE_PDF_DictionaryPtr pDict = pCurObj.GetDictPtr();
					stack.Pop( pCurObj );
					if ( pCurObj->GetType() == OBJ_TYPE_ARRAY )
					{
						pCurObj->GetArray()->Append( pDict );
					}else if ( pCurObj->GetType() == OBJ_TYPE_DICTIONARY )
					{
						stackName.Pop( key );
						pCurObj->GetDict()->SetAtDictionary( key, pDict );
					}
				}else{
					return pCurObj.GetDictPtr();
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
				return CHE_PDF_DictionaryPtr();
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
							pCurObj->GetDict()->SetAtReference( key, objNum, genNum, GetFile() );
						}else{
							CHE_PDF_ReferencePtr pTmp = CHE_PDF_Reference::Create( objNum, genNum, GetFile(), GetAllocator() );
							pCurObj->GetArray()->Append( pTmp );
						}
					}else{
						if ( pCurObj->GetType() == OBJ_TYPE_DICTIONARY )
						{
							return CHE_PDF_DictionaryPtr();
						}else{
							CHE_PDF_NumberPtr pTmp = CHE_PDF_Number::Create( (HE_INT32)objNum, GetAllocator() );
							pCurObj->GetArray()->Append( pTmp );
							SetPos( lPos );
						}
					}
				}else{
					SetPos( lPos );
					if ( pCurObj->GetType() == OBJ_TYPE_DICTIONARY )
					{
						pCurObj->GetDict()->SetAtInteger( key, objNum );
					}else{
						CHE_PDF_NumberPtr pTmp = CHE_PDF_Number::Create( (HE_INT32)objNum, GetAllocator() );
						pCurObj->GetArray()->Append( pTmp );
					}
				}
			}else if ( wordDes.type == PARSE_WORD_FLOAT )	//浮点
			{
				if ( pCurObj->GetType() == OBJ_TYPE_DICTIONARY )
				{
					pCurObj->GetDict()->SetAtFloatNumber( key, wordDes.str.GetFloat() );
				}else{
					CHE_PDF_NumberPtr pTmp = CHE_PDF_Number::Create( wordDes.str.GetFloat(), GetAllocator() );
					pCurObj->GetArray()->Append( pTmp );
				}
			}else if ( wordDes.type == PARSE_WORD_STRING )	//字符串
			{
				if ( pCurObj->GetType() == OBJ_TYPE_DICTIONARY )
				{
					pCurObj->GetDict()->SetAtString( key, wordDes.str );
				}else{
					CHE_PDF_StringPtr pTmp = CHE_PDF_String::Create( wordDes.str, GetAllocator() );
					pCurObj->GetArray()->Append( pTmp );
				}
			}else if ( wordDes.type == PARSE_WORD_NAME )
			{
				if ( pCurObj->GetType() == OBJ_TYPE_DICTIONARY )
				{
					pCurObj->GetDict()->SetAtName( key, wordDes.str );
				}else{
					CHE_PDF_NamePtr pTmp = CHE_PDF_Name::Create( wordDes.str, GetAllocator() );
					pCurObj->GetArray()->Append( pTmp );
				}
			}else if ( wordDes.type == PARSE_WORD_ARRAY_B )	//数组
			{
				if ( pCurObj->GetType() == OBJ_TYPE_DICTIONARY )
				{
					stackName.Push( key );
				}
				stack.Push( pCurObj );
				pCurObj = CHE_PDF_Array::Create( GetAllocator() );
			}else if ( wordDes.type == PARSE_WORD_ARRAY_E )	//数组
			{
				CHE_PDF_ObjectPtr pTmp = pCurObj;
				stack.Pop( pCurObj );
				if ( pCurObj->GetType() == OBJ_TYPE_DICTIONARY )
				{
					stackName.Pop( key );
					pCurObj->GetDict()->SetAtObj( key, pTmp );
				}else{
					pCurObj->GetArray()->Append( pTmp );
				}
			}else if ( wordDes.type == PARSE_WORD_DICT_B )	//字典
			{
				if ( pCurObj->GetType() == OBJ_TYPE_DICTIONARY )
				{
					stackName.Push( key );
				}
				stack.Push( pCurObj );
				pCurObj = CHE_PDF_Dictionary::Create( GetAllocator() );
			}else if ( wordDes.type == PARSE_WORD_DICT_E )	//字典	只出现在array的情况
			{
				CHE_PDF_ObjectPtr pTmp = pCurObj;
				stack.Pop( pCurObj );
				if ( pCurObj->GetType() == OBJ_TYPE_DICTIONARY )
				{
					stackName.Pop( key );
					pCurObj->GetDict()->SetAtObj( key, pTmp );
				}else{
					pCurObj->GetArray()->Append( pTmp );
				}
			}else{	//布尔和其他
				if ( wordDes.str == "false" )
				{
					if ( pCurObj->GetType() == OBJ_TYPE_DICTIONARY )
					{
						pCurObj->GetDict()->SetAtBoolean( key, false );
					}else{
						CHE_PDF_BooleanPtr pTmp = CHE_PDF_Boolean::Create( FALSE, GetAllocator() );
						pCurObj->GetArray()->Append( pTmp );
					}
				}else if ( wordDes.str == "true" )
				{
					if ( pCurObj->GetType() == OBJ_TYPE_DICTIONARY )
					{
						pCurObj->GetDict()->SetAtBoolean( key, true );
					}else{
						CHE_PDF_BooleanPtr pTmp = CHE_PDF_Boolean::Create( TRUE, GetAllocator() );
						pCurObj->GetArray()->Append( pTmp );
					}
				}else if ( wordDes.str == "null" ){
					if ( pCurObj->GetType() == OBJ_TYPE_DICTIONARY )
					{
						pCurObj->GetDict()->SetAtNull( key );
					}else{
						CHE_PDF_NullPtr pTmp = CHE_PDF_Null::Create( GetAllocator() );
						pCurObj->GetArray()->Append( pTmp );
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
	return pCurObj.GetDictPtr();
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

CHE_PDF_Parser * CHE_PDF_Parser::Create( CHE_PDF_File * pFile, IHE_Read * pRead, CHE_PDF_XREF_Table * pXrefTable, CHE_Allocator * pAllocator )
{
	if ( pFile == NULL || pRead == NULL || pXrefTable == NULL )
	{
		return NULL;
	}
	CHE_PDF_Parser * pParserRet = NULL;
	if ( pAllocator == NULL )
	{
		pAllocator = GetDefaultAllocator();
	}
	pParserRet = pAllocator->New<CHE_PDF_Parser>( pFile, pRead, pXrefTable, pAllocator );
	if ( pParserRet )
	{
		pParserRet->m_sParser.InitParser( pParserRet->m_pIHE_FileRead );
		pParserRet->GetStartxref( 1024 );
		pParserRet->ParseXRef();
	}
	return pParserRet;
}

CHE_PDF_Parser::CHE_PDF_Parser( CHE_PDF_File * pFile, IHE_Read * pRead, CHE_PDF_XREF_Table * pXrefTable, CHE_Allocator * pAllocator )
	: CHE_Object( pAllocator ), mpFile( pFile ), m_pIHE_FileRead( pRead ), mpXRefTable( pXrefTable ), m_sParser( pFile, pAllocator )
{
	m_pStrEncrypt = NULL;
	m_pStmEncrypt = NULL;
	m_pEefEncrypt = NULL;
	m_lStartxref = 0;
}

CHE_PDF_Parser::~CHE_PDF_Parser()
{
	mpXRefTable = NULL;
	m_lStartxref = 0;
	if ( m_pStmEncrypt )
	{
		GetAllocator()->Delete( m_pStmEncrypt );
		m_pStmEncrypt = NULL;
	}
	if ( m_pIHE_FileRead )
	{
		m_pIHE_FileRead = NULL;
	}
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

	CHE_PDF_DictionaryPtr pDict;
	CHE_PDF_ArrayPtr pIdArray;
	CHE_PDF_ObjectPtr pObj;
	HE_DWORD offset = m_lStartxref;
	HE_DWORD xrefEntryCount = 0;
	HE_DWORD entryCount = 0;
	while ( TRUE )
	{
		pDict.reset();
		entryCount = ParseXRefTable( offset, pDict );
		xrefEntryCount += entryCount;
		if ( pDict )
		{
			pObj = pDict->GetElement( "ID", OBJ_TYPE_ARRAY );
			if ( pObj )
			{
				pIdArray = pObj.GetArrayPtr();
			}
			pObj = pDict->GetElement( "Encrypt", OBJ_TYPE_DICTIONARY );
			if ( pObj )
			{
				ParseEncrypt( pObj.GetDictPtr(), pIdArray );
			}
			pObj = pDict->GetElement( "Prev", OBJ_TYPE_NUMBER );
			if ( pObj )
			{
				offset = pObj->GetNumber()->GetInteger();
				continue;
			}
		}
		if ( entryCount == 0 )
		{
			pDict.reset();
			entryCount = ParseXRefStream( offset, pDict );
			xrefEntryCount += entryCount;
			if ( pDict )
			{
				pObj = pDict->GetElement( "ID", OBJ_TYPE_ARRAY );
				if ( pObj )
				{
					pIdArray = pObj.GetArrayPtr();
				}
				pObj = pDict->GetElement( "Encrypt", OBJ_TYPE_DICTIONARY );
				if ( pObj )
				{
					ParseEncrypt( pObj.GetDictPtr(), pIdArray );
				}
				pObj = pDict->GetElement( "Prev", OBJ_TYPE_NUMBER );
				if ( pObj )
				{
					offset = pObj->GetNumber()->GetInteger();
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

HE_BOOL CHE_PDF_Parser::ParseEncrypt( const CHE_PDF_DictionaryPtr & pEncryptDict, const CHE_PDF_ArrayPtr & pIDArray )
{
	if ( pEncryptDict != NULL )
	{
		CHE_PDF_ObjectPtr pObj;
		pObj = pEncryptDict->GetElement( "Filter" );
		if ( !pObj || pObj->GetType() != OBJ_TYPE_NAME )
		{
			return FALSE;
		}
		if ( pObj->GetName()->GetString() == "Standard" )
		{
			CHE_ByteString id1( GetAllocator() );
			HE_BYTE O[32];
			HE_BYTE U[32];
			HE_BYTE keyLength = 40;
			HE_BYTE revision = 2;
			HE_BOOL bMetaData = TRUE;
			HE_DWORD pValue = 0;
			if ( pIDArray != NULL )
			{
				pObj = pIDArray->GetElement( 0 );
				if ( pObj != NULL && pObj->GetType() == OBJ_TYPE_STRING )
				{
					id1 = pObj->GetString()->GetString();
				}
			}
			//获取Length
			pObj = pEncryptDict->GetElement( "Length" );
			if ( pObj != NULL && pObj->GetType() == OBJ_TYPE_NUMBER )
			{
				keyLength = pObj->GetNumber()->GetInteger();
			}
			//获取R
			pObj = pEncryptDict->GetElement( "R" );
			if ( pObj != NULL && pObj->GetType() == OBJ_TYPE_NUMBER )
			{
				revision = pObj->GetNumber()->GetInteger();
			}
			//获取P
			pObj = pEncryptDict->GetElement( "P" );
			if ( pObj != NULL && pObj->GetType() == OBJ_TYPE_NUMBER )
			{
				pValue = pObj->GetNumber()->GetInteger();
			}
			//获取EncryptMetadata
			pObj = pEncryptDict->GetElement( "EncryptMetadata" );
			if ( pObj != NULL && pObj->GetType() == OBJ_TYPE_BOOLEAN )
			{
				bMetaData = pObj->GetBoolean()->GetValue();
			}
			//获取U
			pObj = pEncryptDict->GetElement( "U" );
			if ( pObj != NULL && pObj->GetType() == OBJ_TYPE_STRING )
			{
				CHE_ByteString str = pObj->GetString()->GetString();
				for ( HE_DWORD i = 0; i < str.GetLength(); i++ )
				{
					U[i] = str[i];
				}
			}
			//获取O
			pObj = pEncryptDict->GetElement( "O" );
			if ( pObj != NULL && pObj->GetType() == OBJ_TYPE_STRING )
			{
				CHE_ByteString str = pObj->GetString()->GetString();
				for ( HE_DWORD i = 0; i < str.GetLength(); i++ )
				{
					O[i] = str[i];
				}
			}
			pObj = pEncryptDict->GetElement( "V" );
			if ( pObj != NULL && pObj->GetType() == OBJ_TYPE_NUMBER )
			{
				if ( pObj->GetNumber()->GetInteger() == 4 )
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

HE_DWORD CHE_PDF_Parser::ParseXRefTable( HE_DWORD offset, CHE_PDF_DictionaryPtr & pTrailerDictRet )
{
	if ( mpXRefTable == NULL )
	{
		return 0;
	}
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
			pTrailerDictRet = m_sParser.GetDictionary();
 			mpXRefTable->AddTrailerDict( pTrailerDictRet );
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
		objNum = 0, objGenNum = 0, objOffset = 0;
		for ( HE_DWORD i = lBeginNum; i < lBeginNum + lCount; i++ )
		{
			m_sParser.ReadBytes( tmpBytes, 20 );
			m_sParser.Seek( 20 );

			if ( i == 0 )
			{
				CHE_PDF_XREF_Entry tmpEntry( XREF_ENTRY_TYPE_FREE, 0, 0, 0 );
				mpXRefTable->Add( tmpEntry );
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
						CHE_PDF_XREF_Entry tmpEntry( XREF_ENTRY_TYPE_COMMON, i, objOffset, objGenNum );
						mpXRefTable->Add( tmpEntry );
						xrefEntryCount++;
					}
				}
			}
		}
	}
	m_sParser.SetPos( orgOffset );
	return xrefEntryCount;
}

HE_DWORD  CHE_PDF_Parser::ParseXRefStream( HE_DWORD offset, CHE_PDF_DictionaryPtr & pTrailerDictRet )
{
	if ( mpXRefTable == NULL )
	{
		return 0;
	}

	HE_DWORD orgOffset = m_sParser.GetPos();
	m_sParser.SetPos( offset );

	CHE_PDF_ParseWordDes wordDes( GetAllocator() );
	HE_DWORD xrefEntryCount = 0;
	HE_DWORD objNum = 0, genNum = 0, lSecBeginNum = 0, lSecObjCount = 0,  lSecCount = 0, lSize = 0, lW1 = 0, lW2 = 0, lW3 = 0;

	CHE_PDF_ObjectPtr pStream = GetObject();
	if ( !pStream || pStream->GetType() != OBJ_TYPE_STREAM )
	{
		m_sParser.SetPos( orgOffset );
		return 0;
	}
	CHE_PDF_DictionaryPtr pDict = pStream.GetStreamPtr()->GetDict();
	if ( pDict == NULL )
	{
		m_sParser.SetPos( orgOffset );
		return 0;
	}else{
		pTrailerDictRet = pDict;
		mpXRefTable->AddTrailerDict( pDict ); 
	}
	CHE_PDF_ObjectPtr pElement =  pDict->GetElement( "Type" );
	if ( !pElement || pElement->GetType() != OBJ_TYPE_NAME || pElement->GetName()->GetString() != "XRef" )
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
	lSize = pElement->GetNumber()->GetInteger();

	pElement = pDict->GetElement( "W" );
	if ( pElement == NULL || pElement->GetType() != OBJ_TYPE_ARRAY || pElement->GetArray()->GetCount() < 3 )
	{
		m_sParser.SetPos( orgOffset );
		return 0;
	}
	if (	pElement->GetArray()->GetElement( 0 )->GetType() != OBJ_TYPE_NUMBER ||
			pElement->GetArray()->GetElement( 1 )->GetType() != OBJ_TYPE_NUMBER ||
			pElement->GetArray()->GetElement( 2 )->GetType() != OBJ_TYPE_NUMBER )
	{
		m_sParser.SetPos( orgOffset );
		return 0;
	}
	lW1 = pElement->GetArray()->GetElement( 0 )->GetNumber()->GetInteger();
	lW2	= pElement->GetArray()->GetElement( 1 )->GetNumber()->GetInteger();
	lW3	= pElement->GetArray()->GetElement( 2 )->GetNumber()->GetInteger();

	pElement = pDict->GetElement( "Index" );
	if ( !pElement || pElement->GetType() != OBJ_TYPE_ARRAY )
	{
		lSecCount = 1;
	}else{
		if ( pElement->GetArray()->GetCount() < 2 )
		{
			m_sParser.SetPos( orgOffset );
			return 0;
		}
		lSecCount = pElement->GetArray()->GetCount() / 2; 
	}

	CHE_PDF_StreamAcc streamAcc( GetAllocator() );
	if ( streamAcc.Attach( pStream.GetStreamPtr() ) == FALSE )
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
		CHE_PDF_Array * pIndexArray = pElement->GetArray();
		for ( HE_DWORD i = 0; i < lSecCount; i++ )
		{
			lSecBeginArray[i] = pIndexArray->GetElement( i * 2 )->GetNumber()->GetInteger();
			lSecObjCountArray[i] = pIndexArray->GetElement( i * 2 + 1 )->GetNumber()->GetInteger();
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
					CHE_PDF_XREF_Entry tmpEntry( XREF_ENTRY_TYPE_COMPRESSED, lSecBeginNum + indexInSec, field2, field3 );
					mpXRefTable->Add( tmpEntry );
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
							CHE_PDF_XREF_Entry tmpEntry( XREF_ENTRY_TYPE_COMMON, lSecBeginNum + indexInSec, field2, field3 );
							mpXRefTable->Update( lSecBeginNum + xrefEntryCount, tmpEntry );
							lEntryToCheck--;
							break;
						}
					}
					CHE_PDF_XREF_Entry tmpEntry( XREF_ENTRY_TYPE_COMMON, lSecBeginNum + indexInSec, field2, field3 );
					mpXRefTable->Add( tmpEntry );
					xrefEntryCount++;
					indexInSec++;
					break;
				}
			case 0:
				{
					CHE_PDF_XREF_Entry tmpEntry( XREF_ENTRY_TYPE_FREE, lSecBeginNum + indexInSec, 0, 0 );
					mpXRefTable->Add( tmpEntry );
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
	if ( mpXRefTable == NULL )
	{
		return 0;
	}
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
			mpXRefTable->AddTrailerDict( m_sParser.GetDictionary() ); 
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
			CHE_PDF_XREF_Entry tmpEntry( XREF_ENTRY_TYPE_COMMON, objNum, objNumOffset, 0 );
			mpXRefTable->Add( tmpEntry );
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
					CHE_PDF_DictionaryPtr pDict = m_sParser.GetDictionary();
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
	return xrefEntryCount;
}

CHE_PDF_ObjectPtr CHE_PDF_Parser::GetObject()
{
	if ( m_pIHE_FileRead == NULL )
	{
		return CHE_PDF_ObjectPtr();
	}

	HE_DWORD offset = m_sParser.GetPos();
	CHE_PDF_ParseWordDes wordDes( GetAllocator() );
	HE_DWORD objNum = 0, genNum = 0;
	m_sParser.GetWord( wordDes );
	if ( wordDes.type == PARSE_WORD_INTEGER )
	{
		objNum = wordDes.str.GetInteger();
	}else{
		return CHE_PDF_ObjectPtr();
	}
	m_sParser.GetWord( wordDes );
	if ( wordDes.type == PARSE_WORD_INTEGER )
	{
		genNum = wordDes.str.GetInteger();
	}else{
		return CHE_PDF_ObjectPtr();
	}
	m_sParser.GetWord( wordDes );
	if ( wordDes.str != "obj" )
	{
		return CHE_PDF_ObjectPtr();
	}

	m_sParser.SetCurObjNum( objNum );
	m_sParser.SetCurGenNum( genNum );

	HE_DWORD pos = m_sParser.GetPos();
	m_sParser.GetWord( wordDes );
	CHE_PDF_ObjectPtr pCurObj;

	switch ( wordDes.type )
	{
	case PARSE_WORD_INTEGER:
		{
			HE_DWORD refObjNum = 0;
			HE_DWORD refGenNum = 0;
			HE_DWORD tmpPos = 0;
			refObjNum = wordDes.str.GetInteger();
			tmpPos = m_sParser.GetPos();
			m_sParser.GetWord( wordDes );
			if ( wordDes.type == PARSE_WORD_INTEGER )
			{
				refGenNum = wordDes.str.GetInteger();
				m_sParser.GetWord( wordDes );
				if ( wordDes.str == "R" )
				{
					pCurObj = CHE_PDF_Reference::Create( refObjNum, refGenNum, mpFile, GetAllocator() );
					break;
				}
			}
			m_sParser.SetPos( tmpPos );
			pCurObj = CHE_PDF_Number::Create( (HE_INT32)refObjNum, GetAllocator() );
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
			if ( ! pCurObj )
			{
				break;
			}
			HE_DWORD tmpPos = m_sParser.GetPos();
			m_sParser.GetWord( wordDes );
			if ( wordDes.str == "stream" )
			{
				HE_DWORD length = 0;
				CHE_PDF_ObjectPtr pObj = pCurObj->GetDict()->GetElement( "Length" );
				if ( ! pObj )
				{
					length = 0;
				}else if ( pObj->GetType() == OBJ_TYPE_NUMBER || pObj->GetType() == OBJ_TYPE_REFERENCE )
				{
					if ( pObj->GetType() == OBJ_TYPE_NUMBER )
					{
						length = pObj->GetNumber()->GetInteger();
					}else{
						HE_DWORD objNum = pObj->GetReference()->GetRefNum();
						HE_DWORD offset = m_sParser.GetPos();
						CHE_PDF_XREF_Entry entry;
						mpXRefTable->Get( objNum, entry );
						m_sParser.SetPos( entry.GetOffset() );
						CHE_PDF_ObjectPtr pObj = GetObject();
						if ( pObj )
						{
							if ( pObj->GetType() == OBJ_TYPE_NUMBER )
							{
								length = pObj->GetNumber()->GetInteger();
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
						pCurObj = CHE_PDF_Stream::Create( m_pIHE_FileRead, offset, length, pCurObj.GetDictPtr(), objNum, genNum, m_pStmEncrypt, GetAllocator() );
					}
				}
			}else if (	wordDes.str == "endobj" || 
						(	wordDes.str[0] == 'e' && wordDes.str[1] == 'n' && wordDes.str[2] == 'd' &&
							wordDes.str[3] == 'o' && wordDes.str[4] == 'b' && wordDes.str[5] == 'j' ) )
			{
				if ( pCurObj )
				{
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
			return pCurObj;
		}else{
			return CHE_PDF_ObjectPtr();
		}
	}
	return CHE_PDF_ObjectPtr();
}
CHE_PDF_ObjectPtr CHE_PDF_Parser::GetObjectInObjStm( CHE_PDF_StreamPtr & pStream, const HE_PDF_RefInfo & objRefInfo, HE_DWORD index )
{
	if ( !pStream )
	{
		return CHE_PDF_ObjectPtr();
	}

	CHE_PDF_StreamAcc stmAcc( GetAllocator() );
	stmAcc.Attach( pStream );
	HE_LPCBYTE pData = stmAcc.GetData();
	HE_DWORD lDataSize = stmAcc.GetSize();
	IHE_Read * pIHE_Read = HE_CreateMemBufRead( (HE_LPBYTE)pData, lDataSize, GetAllocator() );
	if ( pIHE_Read == NULL )
	{
		stmAcc.Detach();
		return CHE_PDF_ObjectPtr();
	}
	CHE_PDF_SyntaxParser sParser( mpFile, GetAllocator() );
	sParser.InitParser( pIHE_Read );
	sParser.SetCurObjNum( objRefInfo.objNum );
	sParser.SetCurGenNum( objRefInfo.genNum );

	CHE_PDF_ParseWordDes	wordDes( GetAllocator() );
	HE_DWORD				first = 0;
	HE_DWORD				offset = 0;
	HE_DWORD				objNumOffset = 0;
	HE_DWORD				objNumTmp = 0;
	sParser.SetPos( 0 );

	first = pStream->GetDict()->GetElement( "First" )->GetNumber()->GetInteger();

	for ( HE_DWORD i = 0; i < index; i++ )
	{
		if ( !sParser.GetWord( wordDes ) )
		{
			return CHE_PDF_ObjectPtr();
		}
		if ( !sParser.GetWord( wordDes ) )
		{
			return CHE_PDF_ObjectPtr();
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
			if ( objNumTmp == objRefInfo.objNum )
			{
				sParser.SetPos( first );
				HE_DWORD pos = sParser.GetPos();
				sParser.GetWord( wordDes );
				CHE_PDF_ObjectPtr pCurObj;
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
						HE_DWORD refGenNum = wordDes.str.GetInteger();
						sParser.GetWord( wordDes );
						if ( wordDes.str == "R" )
						{
							pCurObj = CHE_PDF_Reference::Create( refNum, refGenNum, mpFile, GetAllocator() );
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
					pCurObj.reset();
				}
				return pCurObj;
			}
		}
	}
	return CHE_PDF_ObjectPtr();
}