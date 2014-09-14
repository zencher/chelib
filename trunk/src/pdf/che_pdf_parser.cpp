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

HE_ULONG CHE_PDF_SyntaxParser::Seek( HE_ULONG bytes )
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
		while( m_lFilePos > 0 )
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
		while( m_lFilePos > 0 )
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

HE_ULONG CHE_PDF_SyntaxParser::ReadBytes( HE_LPBYTE pBuffer, HE_ULONG length )
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
			case '%':	//ע�ͣ�ֱ���ƶ�����һ��
				{
					SeekToNextLine();
					continue;
				}
			case '<':	//�������ֵ䣬Ҳ������ʮ�����Ʊ�ʾ���ַ�������Ҫ�����ж�
				{
					if ( m_lBufferPos > 0 )
					{
						//�����Ѿ���ȡ�Ĵ�
						SubmitBufferStr( /*des.str*/ );
						return TRUE;
					}else{
						byte = m_pFileAccess->ReadByte( m_lFilePos+1 );
						if ( byte == '<' )
						{
							//���ر����ֵ俪ʼ�Ĵ�
							des.offset = m_lFilePos;
							des.type = PARSE_WORD_DICT_B;
							//des.str = "<<";
							m_lFilePos+=2;
							return TRUE;
						}else{
							//�ַ�������
							des.offset = m_lFilePos;
							des.type = PARSE_WORD_STRING;
							m_lFilePos++;
							HE_ULONG tmpValue = 0;
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
							SubmitBufferStr( /*des.str*/ );
							if ( m_pStrEncrypt && m_pStrEncrypt->IsPasswordOK() == TRUE )
							{
								m_pStrEncrypt->Decrypt( CHE_ByteString((const char*)&m_WordBuffer[0])/*des.str*/, m_dwCurObjNum, m_dwCurGenNum );
							}
							return TRUE;
						}
					}
					break;
				}
			case '>':	//�ֵ��������Ϊ�ַ������Ѿ�������λ�ô����ˣ����ﲻ�������ַ�����
				{
					if ( m_lBufferPos > 0 )
					{
						SubmitBufferStr( /*des.str*/ );
						return TRUE;
					}else{
						byte = m_pFileAccess->ReadByte( m_lFilePos+1 );
						if ( byte == '>' )
						{
							des.offset = m_lFilePos;
							des.type = PARSE_WORD_DICT_E;
							//des.str = ">>";
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
						SubmitBufferStr( /*des.str*/ );
						return TRUE;
					}else{
						des.offset = m_lFilePos;
						des.type = PARSE_WORD_ARRAY_B;
						//des.str = "[";
						m_lFilePos++;
						return TRUE;
					}
					break;
				}
			case ']':
				{
					if ( m_lBufferPos > 0 )
					{
						SubmitBufferStr( /*des.str*/ );
						return TRUE;
					}else{
						des.offset = m_lFilePos;
						des.type = PARSE_WORD_ARRAY_E;
						//des.str = "]";
						m_lFilePos++;
						return TRUE;
					}
					break;
				}
			case '/':
				{
					if ( m_lBufferPos > 0 )
					{
						SubmitBufferStr( /*des.str*/ );
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
								HE_ULONG tmpValue = 0;
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
										tmpValue = tmpValue * 16 + (byte - 0x57);
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
						SubmitBufferStr( /*des.str*/ );
						return TRUE;
					}
					break;
				}
			case '(':	//�ַ���
				{
					if ( m_lBufferPos > 0 )
					{
						SubmitBufferStr( /*des.str*/ );
						return TRUE;
					}else{
						des.type = PARSE_WORD_STRING;
						des.offset = m_lFilePos;
						HE_ULONG lCount = 1;
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
										HE_ULONG tmpValue = byte - '0';
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
						SubmitBufferStr( /*des.str*/ );
						if ( m_pStrEncrypt && m_pStrEncrypt->IsPasswordOK() == TRUE )
						{
							m_pStrEncrypt->Decrypt( CHE_ByteString((const char*)&m_WordBuffer[0])/*des.str*/, m_dwCurObjNum, m_dwCurGenNum );
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
						SubmitBufferStr( /*des.str*/ );
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
					if ( m_bBegin )
					{
						m_bBegin = FALSE;
						m_WordBuffer[m_lBufferPos++] = '0';
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
						SubmitBufferStr( /*des.str*/ );
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
			SubmitBufferStr( /*des.str*/ );
			return TRUE;
		}else{
			return FALSE;
		}
	}else{
		return FALSE;
	}
}

CHE_PDF_ArrayPtr CHE_PDF_SyntaxParser::GetArrayPtr()
{
	if ( m_pFileAccess == NULL )
	{
		return CHE_PDF_ArrayPtr();
	}

	CHE_PDF_ParseWordDes wordDes( GetAllocator() );

	if ( ! GetWord( wordDes ) || wordDes.type != PARSE_WORD_ARRAY_B )
	{
		return CHE_PDF_ArrayPtr();
	}

	CHE_PDF_ArrayPtr arrayPtrRet = CHE_PDF_Array::Create( GetAllocator() );

	HE_ULONG preOffset = 0;
	HE_BOOL	bOver = FALSE;
	
	while ( ! bOver )
	{
		preOffset = GetPos();

		if ( ! GetWord( wordDes ) )
		{
			bOver = TRUE;

			continue;
		}

		switch ( wordDes.type )
		{
		case PARSE_WORD_INTEGER:
			{
				HE_INT32 integer = 0;
				HE_ULONG objNum = 0, genNum = 0;
				HE_ULONG tmpOffset = GetPos();

				integer = atoi((const char*)&m_WordBuffer[0]);//wordDes.str.GetInteger();
				objNum = (HE_ULONG)( integer );

				if ( GetWord( wordDes ) && wordDes.type == PARSE_WORD_INTEGER )
				{
					genNum = (HE_ULONG)( atoi((const char*)&m_WordBuffer[0])/*wordDes.str.GetInteger()*/ );

					if ( GetWord( wordDes ) && strcmp( (const char*)&m_WordBuffer[0], "R" ) == 0 /*wordDes.str == "R"*/ )
					{
						CHE_PDF_ReferencePtr pTmp = CHE_PDF_Reference::Create( objNum, genNum, GetFile(), GetAllocator() );

						arrayPtrRet->Append( pTmp );

						break;
					}
				}

				SetPos( tmpOffset );

				CHE_PDF_NumberPtr pTmp = CHE_PDF_Number::Create( integer, GetAllocator() );

				arrayPtrRet->Append( pTmp );

				break;
			}
		case PARSE_WORD_FLOAT:
			{
				CHE_PDF_NumberPtr pTmp = CHE_PDF_Number::Create( (HE_FLOAT)atof((const char*)&m_WordBuffer[0])/*wordDes.str.GetFloat()*/, GetAllocator() );

				arrayPtrRet->Append( pTmp );

				break;
			}
		case PARSE_WORD_STRING:
			{
				CHE_PDF_StringPtr pTmp = CHE_PDF_String::Create( CHE_ByteString((const char*)&m_WordBuffer[0])/*wordDes.str*/,  GetAllocator() );

				arrayPtrRet->Append( pTmp );

				break;
			}
		case PARSE_WORD_NAME:
			{
				CHE_PDF_NamePtr pTmp = CHE_PDF_Name::Create( CHE_ByteString((const char*)&m_WordBuffer[0])/*wordDes.str*/, GetAllocator() );

				arrayPtrRet->Append( pTmp );

				break;
			}
		case PARSE_WORD_ARRAY_B:
			{
				SetPos( preOffset );

				CHE_PDF_ArrayPtr tmpPtr = GetArrayPtr();

				arrayPtrRet->Append( tmpPtr );

				break;
			}
		case PARSE_WORD_ARRAY_E:
			{
				bOver = TRUE;

				break;
			}
		case PARSE_WORD_DICT_B:
			{
				SetPos( preOffset );

				CHE_PDF_DictionaryPtr tmpPtr = GetDictionaryPtr();

				arrayPtrRet->Append( tmpPtr );

				break;
			}
		case  PARSE_WORD_UNKNOWN:
		default:
			{
				if ( strcmp( (const char*)&m_WordBuffer[0], "false" ) == 0 /*wordDes.str == "false"*/ )
				{
					CHE_PDF_BooleanPtr tmpPtr = CHE_PDF_Boolean::Create( FALSE, GetAllocator() );
					
					arrayPtrRet->Append( tmpPtr );
				}
				else if ( strcmp( (const char*)&m_WordBuffer[0], "true" ) == 0 /*wordDes.str == "true"*/ )
				{
					CHE_PDF_BooleanPtr tmpPtr = CHE_PDF_Boolean::Create( TRUE, GetAllocator() );
				
					arrayPtrRet->Append( tmpPtr );	
				}
				else if ( strcmp( (const char*)&m_WordBuffer[0], "null" ) == 0 /*wordDes.str == "null"*/ )
				{
					CHE_PDF_NullPtr tmpPtr = CHE_PDF_Null::Create( GetAllocator() );

					arrayPtrRet->Append( tmpPtr );
				}
			}
		}
	}

	arrayPtrRet->SetModified( FALSE );

	return arrayPtrRet;
}

CHE_PDF_DictionaryPtr CHE_PDF_SyntaxParser::GetDictionaryPtr()
{
	if ( m_pFileAccess == NULL )
	{
		return CHE_PDF_DictionaryPtr();
	}

	CHE_PDF_ParseWordDes wordDes( GetAllocator() );

	if ( ! GetWord( wordDes ) || wordDes.type != PARSE_WORD_DICT_B )
	{
		return CHE_PDF_DictionaryPtr();
	}

	HE_BOOL bKey = TRUE;
	HE_BOOL bOver = FALSE;
	HE_ULONG preOffset = 0;
	CHE_ByteString key( GetAllocator() );
	CHE_PDF_DictionaryPtr dictRet = CHE_PDF_Dictionary::Create( GetAllocator() );

	while ( ! bOver )
	{
		preOffset = GetPos();

		if ( ! GetWord( wordDes ) )
		{
			bOver = TRUE;
			
			continue;
		}

		if ( bKey )
		{
			if ( wordDes.type != PARSE_WORD_NAME )
			{
				bOver = TRUE;
				
				continue;
			}
			else
			{
				key = (const char*)&m_WordBuffer[0];//wordDes.str;
				
				bKey = FALSE;

				continue;
			}
		}

		switch ( wordDes.type )
		{
		case PARSE_WORD_INTEGER:
			{
				HE_INT32 integer = 0;
				HE_ULONG objNum = 0, genNum = 0;
				HE_ULONG tmpOffset = GetPos();

				integer = atoi((const char*)&m_WordBuffer[0]); //wordDes.str.GetInteger();
				objNum = (HE_ULONG)( integer );
				
				if ( GetWord( wordDes ) && wordDes.type == PARSE_WORD_INTEGER )
				{
					genNum = (HE_ULONG)( atoi((const char*)&m_WordBuffer[0])/*wordDes.str.GetInteger()*/ );

					if ( GetWord( wordDes ) && strcmp((const char*)&m_WordBuffer[0], "R")==0 /*wordDes.str == "R"*/ )
					{
						dictRet->SetAtReference( key, objNum, genNum, GetFile() );

						break;
					}
				}

				SetPos( tmpOffset );

				dictRet->SetAtInteger( key, integer );

				break;
			}
		case PARSE_WORD_FLOAT:
			{
				dictRet->SetAtFloatNumber( key, atof((const char*)&m_WordBuffer[0])/*wordDes.str.GetFloat()*/ );

				break;
			}
		case PARSE_WORD_STRING:
			{
				dictRet->SetAtString( key, CHE_ByteString((const char*)&m_WordBuffer[0])/*wordDes.str*/ );

				break;
			}
		case PARSE_WORD_NAME:
			{
				dictRet->SetAtName( key, CHE_ByteString((const char*)&m_WordBuffer[0])/*wordDes.str*/ );

				break;
			}
		case PARSE_WORD_ARRAY_B:
			{
				SetPos( preOffset );

				CHE_PDF_ArrayPtr tmpPtr = GetArrayPtr();

				dictRet->SetAtArray( key, tmpPtr );

				break;
			}
		case PARSE_WORD_DICT_B:
			{
				SetPos( preOffset );

				CHE_PDF_DictionaryPtr tmpPtr = GetDictionaryPtr();

				dictRet->SetAtDictionary( key, tmpPtr );

				break;
			}
		case PARSE_WORD_DICT_E:
			{
				bOver = TRUE;

				break;
			}
		case PARSE_WORD_UNKNOWN:
		default:
			{
				if ( strcmp( (const char*)&m_WordBuffer[0], "false" ) == 0 /*wordDes.str == "false"*/ )
				{
					dictRet->SetAtBoolean( key, FALSE );
				}
				else if ( strcmp( (const char*)&m_WordBuffer[0], "true" ) == 0 /*wordDes.str == "true"*/ )
				{
					dictRet->SetAtBoolean( key, TRUE );
				}
				else if ( strcmp( (const char*)&m_WordBuffer[0], "null" ) == 0 /*wordDes.str == "null"*/ )
				{
					dictRet->SetAtNull( key ); 
				}
				break;
			}
		}
		
		bKey = TRUE;
	}

	dictRet->SetModified( FALSE );

	return dictRet;
}

HE_BOOL	CHE_PDF_SyntaxParser::IsWord( const char * words )
{
	if ( strcmp( (const char*)&m_WordBuffer[0], words ) == 0 )
	{
		return TRUE;
	}
	return FALSE;
}

HE_INT32 CHE_PDF_SyntaxParser::GetInteger()
{
	return atoi( (const char*)&m_WordBuffer[0] );
}

HE_FLOAT CHE_PDF_SyntaxParser::GetFloat()
{
	return (HE_FLOAT)atof( (const char*)&m_WordBuffer[0] );
}

CHE_ByteString CHE_PDF_SyntaxParser::GetString()
{
	return CHE_ByteString( (const char *)&m_WordBuffer[0] );
}

HE_VOID CHE_PDF_SyntaxParser::SubmitBufferStr( /*CHE_ByteString & str*/ )
{
	m_WordBuffer[m_lBufferPos] = '\0';
	//str.SetData( m_WordBuffer, m_lBufferPos );
	//m_WordBuffer[m_lBufferPos=0] = '\0';
	m_lBufferPos = 0;
	m_bBegin = TRUE;
	m_bPoint = FALSE;
	m_bSign = FALSE;
}

CHE_PDF_Parser * CHE_PDF_Parser::Create( CHE_PDF_File * pFile, IHE_Read * pRead, CHE_PDF_XRefTable * pXrefTable, CHE_Allocator * pAllocator )
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
	}
	return pParserRet;
}

CHE_PDF_Parser::CHE_PDF_Parser( CHE_PDF_File * pFile, IHE_Read * pRead, CHE_PDF_XRefTable * pXrefTable, CHE_Allocator * pAllocator )
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

HE_ULONG CHE_PDF_Parser::GetFileSize() const
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
	HE_ULONG offset = 0;
	HE_ULONG fileSize = m_pIHE_FileRead->GetSize();
	HE_BOOL bHeadFound = FALSE;

	while( offset < 1024 && offset < fileSize )
	{
		if ( m_pIHE_FileRead->ReadByte( offset ) == '%' )
		{
			bHeadFound = TRUE;
			break;
		}
		++offset;
	}
	if ( ! bHeadFound )
	{
		return PDF_VERSION_UNKNOWN;
	}

	char buffer[8];
	memset( buffer, 0, 8 );
	if ( m_pIHE_FileRead->ReadBlock( buffer, offset, 8 ) )
	{
		if ( buffer[0] == '%' &&
			 buffer[1] == 'P' &&
			 buffer[2] == 'D' &&
			 buffer[3] == 'F' &&
			 buffer[4] == '-' &&
			 buffer[5] == '1' &&
			 buffer[6] == '.' )
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

HE_BOOL CHE_PDF_Parser::Authenticate( const CHE_ByteString & password ) 
{
	HE_BOOL bRet = FALSE;

	if ( m_pStrEncrypt )
	{
		bRet = m_pStmEncrypt->Authenticate( password );
		if ( m_pStrEncrypt )
		{
			m_sParser.SetEncrypt( m_pStrEncrypt );
		}
	}
	return bRet;
}

HE_ULONG CHE_PDF_Parser::GetStartxref( HE_ULONG range )
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
		HE_ULONG iStartXref = 0;
		for ( HE_ULONG m = range; m >= 8; m-- )
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
				HE_ULONG iBufferIndex = m+1;
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

HE_ULONG CHE_PDF_Parser::ParseXRef()
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
	HE_ULONG offset = m_lStartxref;
	HE_ULONG xrefEntryCount = 0;
	HE_ULONG entryCount = 0;
	while ( TRUE )
	{
		pDict.Reset();
		entryCount = ParseXRefTable( offset, pDict );
		xrefEntryCount += entryCount;
		if ( pDict )
		{
			pObj = pDict->GetElement( "ID", OBJ_TYPE_ARRAY );
			if ( pObj )
			{
				pIdArray = pObj->GetArrayPtr();
			}
			if ( m_pStrEncrypt == NULL )
			{
				pObj = pDict->GetElement( "Encrypt", OBJ_TYPE_REFERENCE );
				if ( pObj )
				{
					mEncryptRef = pObj->GetRefPtr();
				}
				pObj = pDict->GetElement( "Encrypt", OBJ_TYPE_DICTIONARY );
				if ( pObj ) 
				{
					ParseEncrypt( pObj->GetDictPtr(), pIdArray );
				}
				pObj = pDict->GetElement( "ID", OBJ_TYPE_ARRAY );
				if ( pObj )
				{
					mIDArrayPtr = pObj->GetArrayPtr();
				}
			}
			pObj = pDict->GetElement( "Prev", OBJ_TYPE_NUMBER );
			if ( pObj )
			{
				offset = pObj->GetNumberPtr()->GetInteger();
				continue;
			}
		}
		if ( entryCount == 0 )
		{
			pDict.Reset();
			entryCount = ParseXRefStream( offset, pDict );
			xrefEntryCount += entryCount;
			if ( pDict )
			{
				pObj = pDict->GetElement( "ID", OBJ_TYPE_ARRAY );
				if ( pObj )
				{
					pIdArray = pObj->GetArrayPtr();
				}
				pObj = pDict->GetElement( "Encrypt", OBJ_TYPE_DICTIONARY );
				if ( pObj )
				{
					ParseEncrypt( pObj->GetDictPtr(), pIdArray );
				}
				pObj = pDict->GetElement( "Prev", OBJ_TYPE_NUMBER );
				if ( pObj )
				{
					offset = pObj->GetNumberPtr()->GetInteger();
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
	if ( pEncryptDict )
	{
		CHE_PDF_ObjectPtr pObj;
		pObj = pEncryptDict->GetElement( "Filter" );
		if ( !pObj || pObj->GetType() != OBJ_TYPE_NAME )
		{
			return FALSE;
		}
		if ( pObj->GetNamePtr()->GetString() == "Standard" )
		{
			CHE_ByteString id1( GetAllocator() );
			HE_BYTE O[32];
			HE_BYTE U[32];
			HE_BYTE keyLength = 40;
			HE_BYTE revision = 2;
			HE_BYTE version = 0;
			HE_BOOL bMetaData = TRUE;
			HE_ULONG pValue = 0;
			if ( pIDArray )
			{
				pObj = pIDArray->GetElement( 0 );
				if ( pObj && pObj->GetType() == OBJ_TYPE_STRING )
				{
					id1 = pObj->GetStringPtr()->GetString();
				}
			}
			//��ȡLength
			pObj = pEncryptDict->GetElement( "Length" );
			if ( pObj && pObj->GetType() == OBJ_TYPE_NUMBER )
			{
				keyLength = pObj->GetNumberPtr()->GetInteger();
			}
			//��ȡR
			pObj = pEncryptDict->GetElement( "R" );
			if ( pObj && pObj->GetType() == OBJ_TYPE_NUMBER )
			{
				revision = pObj->GetNumberPtr()->GetInteger();
			}
			//��ȡP
			pObj = pEncryptDict->GetElement( "P" );
			if ( pObj && pObj->GetType() == OBJ_TYPE_NUMBER )
			{
				pValue = pObj->GetNumberPtr()->GetInteger();
			}
			//��ȡEncryptMetadata
			pObj = pEncryptDict->GetElement( "EncryptMetadata" );
			if ( pObj && pObj->GetType() == OBJ_TYPE_BOOLEAN )
			{
				bMetaData = pObj->GetBooleanPtr()->GetValue();
			}
			//��ȡU
			pObj = pEncryptDict->GetElement( "U" );
			if ( pObj && pObj->GetType() == OBJ_TYPE_STRING )
			{
				CHE_ByteString str = pObj->GetStringPtr()->GetString();
				for ( HE_ULONG i = 0; i < str.GetLength(); i++ )
				{
					U[i] = str[i];
				}
			}
			//��ȡO
			pObj = pEncryptDict->GetElement( "O" );
			if ( pObj && pObj->GetType() == OBJ_TYPE_STRING )
			{
				CHE_ByteString str = pObj->GetStringPtr()->GetString();
				for ( HE_ULONG i = 0; i < str.GetLength(); i++ )
				{
					O[i] = str[i];
				}
			}
			pObj = pEncryptDict->GetElement( "V" );
			if ( pObj && pObj->GetType() == OBJ_TYPE_NUMBER )
			{
				version = pObj->GetNumberPtr()->GetInteger();
				if ( version == 4 )
				{
					//�ж��Ƿ���AESV2�㷨
					pObj = pEncryptDict->GetElement( "CF" );
					if ( pObj  && pObj->GetType() == OBJ_TYPE_DICTIONARY )
					{
						pObj = pObj->GetDictPtr()->GetElement( "StdCF", OBJ_TYPE_DICTIONARY );
						if ( pObj && pObj->GetType() == OBJ_TYPE_DICTIONARY )
						{
							pObj = pObj->GetDictPtr()->GetElement( "CFM", OBJ_TYPE_NAME );
							if ( pObj && pObj->GetType() == OBJ_TYPE_NAME )
							{
								CHE_ByteString cfm = pObj->GetNamePtr()->GetString();
								if ( cfm == "AESV2" )
								{
									m_pStrEncrypt = GetAllocator()->New<CHE_PDF_Encrypt>( id1, O, U, PDFENCRYPT_ALGORITHM_AESV2, version, revision, keyLength, bMetaData, pValue, GetAllocator() );
									m_pStmEncrypt = m_pStrEncrypt;
									m_pEefEncrypt = m_pStrEncrypt;

									m_sParser.SetEncrypt( m_pStrEncrypt );

									m_pStrEncrypt->Authenticate( "" );

									return TRUE;
								}
							}
						}
					}
				}
			}
			
			m_pStrEncrypt = GetAllocator()->New<CHE_PDF_Encrypt>( id1, O, U, PDFENCRYPT_ALGORITHM_RC4, version, revision, keyLength, bMetaData, pValue, GetAllocator() );
			m_pStmEncrypt = m_pStrEncrypt;
			m_pEefEncrypt = m_pStrEncrypt;
			m_sParser.SetEncrypt( m_pStrEncrypt );

			m_pStrEncrypt->Authenticate( "" );

			return TRUE;
		}
		//�Ǳ�׼�ļ��ܴ���δ֧��
	}
	return FALSE;
};

HE_ULONG CHE_PDF_Parser::ParseXRefTable( HE_ULONG offset, CHE_PDF_DictionaryPtr & pTrailerDictRet )
{
	if ( mpXRefTable == NULL )
	{
		return 0;
	}
	HE_ULONG orgOffset = m_sParser.GetPos();
	m_sParser.SetPos( offset );

	CHE_PDF_ParseWordDes wordDes( GetAllocator() );
	HE_ULONG xrefEntryCount = 0;
	if ( m_sParser.GetWord( wordDes ) == FALSE || ! m_sParser.IsWord( "xref" ) /*wordDes.str != "xref"*/ )
	{
		m_sParser.SetPos( orgOffset );
		return 0; 
	}

	HE_ULONG lBeginNum = 0, lCount = 0;
	HE_ULONG objNum = 0, objGenNum = 0, objOffset = 0;
	HE_BYTE tmpBytes[20];
	while ( true )
	{
		lBeginNum = 0;
		lCount = 0;
		if ( m_sParser.GetWord( wordDes ) == FALSE ) break;
		if( wordDes.type == PARSE_WORD_INTEGER )
		{
			lBeginNum = m_sParser.GetInteger();//wordDes.str.GetInteger();
		}else if ( wordDes.type == PARSE_WORD_UNKNOWN && m_sParser.IsWord( "trailer" )/*wordDes.str == "trailer"*/ )
		{
			pTrailerDictRet = m_sParser.GetDictionaryPtr();
 			mpXRefTable->AddTrailerDict( pTrailerDictRet );
		}else{
			break;
		}

		if ( m_sParser.GetWord( wordDes ) == FALSE ) break;
		if( wordDes.type == PARSE_WORD_INTEGER )
		{
			lCount = m_sParser.GetInteger();//wordDes.str.GetInteger();
		}else{
			break;
		}

		m_sParser.SeekToNextLine();
		objNum = 0, objGenNum = 0, objOffset = 0;
		for ( HE_ULONG i = lBeginNum; i < lBeginNum + lCount; i++ )
		{
			m_sParser.ReadBytes( tmpBytes, 20 );
			m_sParser.Seek( 20 );

			if ( i == 0 )
			{
				CHE_PDF_XRefEntry tmpEntry( XREF_ENTRY_TYPE_FREE, 0, 0, 0 );
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
						CHE_PDF_XRefEntry tmpEntry( XREF_ENTRY_TYPE_COMMON, i, objOffset, objGenNum );
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

HE_ULONG  CHE_PDF_Parser::ParseXRefStream( HE_ULONG offset, CHE_PDF_DictionaryPtr & pTrailerDictRet )
{
	if ( mpXRefTable == NULL )
	{
		return 0;
	}

	HE_ULONG orgOffset = m_sParser.GetPos();
	m_sParser.SetPos( offset );

	CHE_PDF_ParseWordDes wordDes( GetAllocator() );
	HE_ULONG xrefEntryCount = 0;
	HE_ULONG lSecBeginNum = 0, lSecObjCount = 0,  lSecCount = 0, lSize = 0, lW1 = 0, lW2 = 0, lW3 = 0;

	CHE_PDF_ObjectPtr pStream = GetObject();
	if ( !pStream || pStream->GetType() != OBJ_TYPE_STREAM )
	{
		m_sParser.SetPos( orgOffset );
		return 0;
	}
	CHE_PDF_DictionaryPtr pDict = pStream->GetStreamPtr()->GetDictPtr();
	if ( !pDict )
	{
		m_sParser.SetPos( orgOffset );
		return 0;
	}else{
		pTrailerDictRet = pDict;
		mpXRefTable->AddTrailerDict( pDict ); 
	}
	CHE_PDF_ObjectPtr pElement =  pDict->GetElement( "Type" );
	if ( !pElement || pElement->GetType() != OBJ_TYPE_NAME || pElement->GetNamePtr()->GetString() != "XRef" )
	{
		m_sParser.SetPos( orgOffset );
		return 0;
	}
	pElement = pDict->GetElement( "Size" );
	if ( !pElement || pElement->GetType() != OBJ_TYPE_NUMBER )
	{
		m_sParser.SetPos( orgOffset );
		return 0;
	}
	lSize = pElement->GetNumberPtr()->GetInteger();

	pElement = pDict->GetElement( "W" );
	if ( !pElement || pElement->GetType() != OBJ_TYPE_ARRAY || pElement->GetArrayPtr()->GetCount() < 3 )
	{
		m_sParser.SetPos( orgOffset );
		return 0;
	}
	if (	pElement->GetArrayPtr()->GetElement( 0 )->GetType() != OBJ_TYPE_NUMBER ||
			pElement->GetArrayPtr()->GetElement( 1 )->GetType() != OBJ_TYPE_NUMBER ||
			pElement->GetArrayPtr()->GetElement( 2 )->GetType() != OBJ_TYPE_NUMBER )
	{
		m_sParser.SetPos( orgOffset );
		return 0;
	}
	lW1 = pElement->GetArrayPtr()->GetElement( 0 )->GetNumberPtr()->GetInteger();
	lW2	= pElement->GetArrayPtr()->GetElement( 1 )->GetNumberPtr()->GetInteger();
	lW3	= pElement->GetArrayPtr()->GetElement( 2 )->GetNumberPtr()->GetInteger();

	pElement = pDict->GetElement( "Index" );
	if ( !pElement || pElement->GetType() != OBJ_TYPE_ARRAY )
	{
		lSecCount = 1;
	}else{
		if ( pElement->GetArrayPtr()->GetCount() < 2 )
		{
			m_sParser.SetPos( orgOffset );
			return 0;
		}
		lSecCount = pElement->GetArrayPtr()->GetCount() / 2; 
	}

	CHE_PDF_StreamAcc streamAcc( GetAllocator() );
	if ( streamAcc.Attach( pStream->GetStreamPtr() ) == FALSE )
	{
		m_sParser.SetPos( orgOffset );
		return 0;
	}
	HE_ULONG streamSize = streamAcc.GetSize();
	HE_LPCBYTE lpByte = streamAcc.GetData();
	HE_ULONG field1 = 0, field2 = 0, field3 = 0;
	HE_ULONG lBlockCount = 0;
	HE_ULONG lEntrySize = lW1 + lW2 + lW3;
	HE_ULONG lItemOfSecCount = 0;

// 	FILE * pFile = fopen( "d:\\xrefstream.txt", "wb+" );
// 	HE_LPBYTE tmpByte = (HE_LPBYTE)( streamAcc.GetData() );
// 	for ( HE_ULONG offset = 0; offset < streamSize; offset+=lEntrySize )
// 	{
// 		for ( HE_ULONG index = 0; index < lEntrySize; ++index )
// 		{
// 			fprintf( pFile, "%02X", *(tmpByte + offset + index) );
// 		}
// 		fprintf( pFile, "\n" );
// 	}
// 	fclose( pFile );



//  	std::vector<HE_ULONG> XrefVerify1;
//  	std::vector<HE_ULONG> XrefVerify2;

	CHE_SkipList<HE_ULONG> XrefVerify1;
	CHE_SkipList<HE_ULONG> XrefVerify2;

	HE_ULONG lEntryToCheck = 0;

	HE_ULONG *lSecBeginArray = GetAllocator()->NewArray<HE_ULONG>( lSecCount );
	HE_ULONG *lSecObjCountArray = GetAllocator()->NewArray<HE_ULONG>( lSecCount );
	HE_ULONG lEntryCount = 0;
	if ( !pElement )
	{
		lSecBeginArray[0] = 0;
		lSecObjCountArray[0] = lSize;
		lEntryCount = lSize;
	}else{
		CHE_PDF_ArrayPtr pIndexArray = pElement->GetArrayPtr();
		for ( HE_ULONG i = 0; i < lSecCount; i++ )
		{
			lSecBeginArray[i] = pIndexArray->GetElement( i * 2 )->GetNumberPtr()->GetInteger();
			lSecObjCountArray[i] = pIndexArray->GetElement( i * 2 + 1 )->GetNumberPtr()->GetInteger();
			lEntryCount += lSecObjCountArray[i];
		}
	}

	if ( streamSize / lEntrySize > lEntryCount )
	{
		lEntryToCheck = streamSize / lEntrySize - lEntryCount;
	}else{
		lEntryToCheck = 0;
	}


	for ( HE_ULONG i = 0; i < lSecCount; i++ )
	{
		lSecBeginNum = lSecBeginArray[i];
		lSecObjCount = lSecObjCountArray[i] + lEntryToCheck;

		lItemOfSecCount = lSecObjCount;
		HE_ULONG indexInSec = 0;
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
				for ( HE_ULONG i = 0; i < lW1; i++ )
				{
					field1 = field1 << 8;
					field1 += *lpByte;
					lpByte++;
				}
			}
			for ( HE_ULONG j = 0; j < lW2; j++ )
			{
				field2 = field2 << 8;
				field2 += *lpByte;
				lpByte++;
			}
			for ( HE_ULONG k = 0; k < lW3; k++ )
			{
				field3 = field3 << 8;
				field3 += *lpByte;
				lpByte++;
			}
			switch( field1 )
			{
			case 2:
				{
					HE_ULONG tmpValue = ( field2 << 9 ) + field3;
					if ( XrefVerify2.IsExist( tmpValue ) )
					{
						lEntryToCheck--;
						break;
					}else{
						XrefVerify2.Append( tmpValue );
					}
//					HE_ULONG tmpValue = ( field2 << 9 ) + field3;
// 					bool bRepeat = false;
// 					std::vector<HE_ULONG>::iterator it;
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
					CHE_PDF_XRefEntry tmpEntry( XREF_ENTRY_TYPE_COMPRESSED, lSecBeginNum + indexInSec, field2, field3 );
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
// 					std::vector<HE_ULONG>::iterator it;
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
						HE_ULONG offsetSave = m_sParser.GetPos();
						m_sParser.SetPos( field2 );
						m_sParser.GetWord( wordDes );
						m_sParser.SetPos( offsetSave );
						if ( wordDes.type != PARSE_WORD_INTEGER )
						{
							lEntryToCheck--;
							break;
						}else if (	wordDes.type == PARSE_WORD_INTEGER && 
									/*wordDes.str.GetInteger()*/ m_sParser.GetInteger() != (HE_INT32)(lSecBeginNum + indexInSec)  )
						{
							CHE_PDF_XRefEntry tmpEntry( XREF_ENTRY_TYPE_COMMON, lSecBeginNum + indexInSec, field2, field3 );
							mpXRefTable->Update( lSecBeginNum + xrefEntryCount, tmpEntry );
							lEntryToCheck--;
							break;
						}
					}
					CHE_PDF_XRefEntry tmpEntry( XREF_ENTRY_TYPE_COMMON, lSecBeginNum + indexInSec, field2, field3 );
					mpXRefTable->Add( tmpEntry );
					xrefEntryCount++;
					indexInSec++;
					break;
				}
			case 0:
				{
					CHE_PDF_XRefEntry tmpEntry( XREF_ENTRY_TYPE_FREE, lSecBeginNum + indexInSec, 0, 0 );
					mpXRefTable->Add( tmpEntry );
					xrefEntryCount++;
					indexInSec++;
					break;
				}
			}
		}
	}

	GetAllocator()->DeleteArray<HE_ULONG>( lSecBeginArray );
	GetAllocator()->DeleteArray<HE_ULONG>( lSecObjCountArray );

	lSecBeginArray = NULL;
	lSecObjCountArray = NULL;
	streamAcc.Detach();
	m_sParser.SetPos( orgOffset );
	return xrefEntryCount;
}


HE_ULONG CHE_PDF_Parser::FullParseForXRef()	//���������ļ�����ȡ������Ϣ // ����Ҫ�������xref�Ĺ��ܣ����ĳЩ����xref��startxref�����pdf�ļ�
{
	if ( mpXRefTable == NULL )
	{
		return 0;
	}
	CHE_PDF_ParseWordDes	wordDes( GetAllocator() );
	HE_ULONG				xrefEntryCount = 0;
	HE_ULONG				offset = 0;
	HE_ULONG				objNumOffset = 0;
	HE_ULONG				objNum = 0;

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
		if ( m_sParser.IsWord( "trailer" )/*wordDes.str == "trailer"*/ )
		{
			mpXRefTable->AddTrailerDict( m_sParser.GetDictionaryPtr() ); 
			continue;
		}
		if ( m_sParser.IsWord( "obj" )/*wordDes.str == "obj"*/ )
		{
			offset = m_sParser.GetPos();
			m_sParser.SetPos( wordDes.offset );
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
			objNum = m_sParser.GetInteger();//wordDes.str.GetInteger();
			m_sParser.GetWord( wordDes );
			if ( wordDes.type != PARSE_WORD_INTEGER )
			{
				m_sParser.SetPos( offset );
				continue;
			}
			CHE_PDF_XRefEntry tmpEntry( XREF_ENTRY_TYPE_COMMON, objNum, objNumOffset, 0 );
			mpXRefTable->Add( tmpEntry );
			xrefEntryCount++;
			
			//seek����һ��obj
			//ĳЩ�����ļ�ȱ��endobj
			m_sParser.GetWord( wordDes );
			while( m_sParser.GetWord( wordDes ) )
			{
				if ( m_sParser.IsWord( "stream" )/*wordDes.str == "stream"*/ )
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
				}else if ( wordDes.type == PARSE_WORD_DICT_B /*wordDes.str == "<<"*/ )
				{
					m_sParser.SeekToPrevWord();
					CHE_PDF_DictionaryPtr pDict = m_sParser.GetDictionaryPtr();
					if ( m_sParser.GetWord( wordDes ) == FALSE )
					{
						return xrefEntryCount;
					}else if ( m_sParser.IsWord( "stream" ) /*wordDes.str == "stream"*/ )
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

	HE_ULONG offset = m_sParser.GetPos();
	CHE_PDF_ParseWordDes wordDes( GetAllocator() );
	HE_ULONG objNum = 0, genNum = 0;
	m_sParser.GetWord( wordDes );
	if ( wordDes.type == PARSE_WORD_INTEGER )
	{
		objNum = m_sParser.GetInteger();//wordDes.str.GetInteger();
	}else{
		return CHE_PDF_ObjectPtr();
	}
	m_sParser.GetWord( wordDes );
	if ( wordDes.type == PARSE_WORD_INTEGER )
	{
		genNum = m_sParser.GetInteger();//wordDes.str.GetInteger();
	}else{
		return CHE_PDF_ObjectPtr();
	}
	m_sParser.GetWord( wordDes );
	if ( !m_sParser.IsWord( "obj" )/*wordDes.str != "obj"*/ )
	{
		return CHE_PDF_ObjectPtr();
	}

	m_sParser.SetCurObjNum( objNum );
	m_sParser.SetCurGenNum( genNum );

	HE_ULONG pos = m_sParser.GetPos();
	m_sParser.GetWord( wordDes );
	CHE_PDF_ObjectPtr pCurObj;

	switch ( wordDes.type )
	{
	case PARSE_WORD_INTEGER:
		{
			HE_INT32 integer = 0;
			HE_ULONG refObjNum = 0;
			HE_ULONG refGenNum = 0;
			HE_ULONG tmpPos = 0;
			integer = m_sParser.GetInteger();//wordDes.str.GetInteger();
			refObjNum = (HE_ULONG)( integer );
			tmpPos = m_sParser.GetPos();
			m_sParser.GetWord( wordDes );
			if ( wordDes.type == PARSE_WORD_INTEGER )
			{
				refGenNum = (HE_ULONG)( m_sParser.GetInteger()/*wordDes.str.GetInteger()*/ );
				m_sParser.GetWord( wordDes );
				if ( m_sParser.IsWord( "R" )/*wordDes.str == "R"*/ )
				{
					pCurObj = CHE_PDF_Reference::Create( refObjNum, refGenNum, mpFile, GetAllocator() );
					break;
				}
			}
			m_sParser.SetPos( tmpPos );
			pCurObj = CHE_PDF_Number::Create( integer, GetAllocator() );
			break;
		}
	case PARSE_WORD_FLOAT:
		{
			pCurObj = CHE_PDF_Number::Create( m_sParser.GetFloat()/*wordDes.str.GetFloat()*/, GetAllocator() );
			break;
		}
	case PARSE_WORD_STRING:
		{
			CHE_ByteString str = m_sParser.GetString();
			if ( m_pStrEncrypt )
			{	
				m_pStrEncrypt->Decrypt( str/*wordDes.str*/, objNum, genNum );
			}
			pCurObj = CHE_PDF_String::Create( str, GetAllocator() );
			break;
		}
	case PARSE_WORD_NAME:
		{
			pCurObj = CHE_PDF_Name::Create( m_sParser.GetString()/*wordDes.str*/, GetAllocator() );
			break;
		}
	case PARSE_WORD_ARRAY_B:
		{
			m_sParser.SetPos( pos );
			pCurObj = m_sParser.GetArrayPtr();
			break;
		}
	case PARSE_WORD_DICT_B:
		{
			m_sParser.SetPos( pos );
			pCurObj = m_sParser.GetDictionaryPtr();
			if ( ! pCurObj )
			{
				break;
			}
			m_sParser.GetWord( wordDes );
			if ( m_sParser.IsWord( "stream" ) /*wordDes.str == "stream"*/ )
			{
				HE_ULONG length = 0;
				CHE_PDF_ObjectPtr pObj = pCurObj->GetDictPtr()->GetElement( "Length" );
				if ( ! pObj )
				{
					length = 0;
				}else if ( pObj->GetType() == OBJ_TYPE_NUMBER || pObj->GetType() == OBJ_TYPE_REFERENCE )
				{
					if ( pObj->GetType() == OBJ_TYPE_NUMBER )
					{
						length = pObj->GetNumberPtr()->GetInteger();
					}else{
						HE_ULONG objNum = pObj->GetRefPtr()->GetRefNum();
						HE_ULONG offset = m_sParser.GetPos();
						CHE_PDF_XRefEntry entry;
						mpXRefTable->Get( objNum, entry );
						m_sParser.SetPos( entry.GetOffset() );
						CHE_PDF_ObjectPtr pObj = GetObject();
						if ( pObj )
						{
							if ( pObj->GetType() == OBJ_TYPE_NUMBER )
							{
								length = pObj->GetNumberPtr()->GetInteger();
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
					if ( m_sParser.IsWord( "endstream" )/*wordDes.str == "endstream"*/ )
					{
						pCurObj = CHE_PDF_Stream::Create( m_pIHE_FileRead, offset, length, pCurObj->GetDictPtr(), objNum, genNum, m_pStmEncrypt, GetAllocator() );
					}
				}
			}
			break;
		}
	case PARSE_WORD_UNKNOWN:
		{
			if ( m_sParser.IsWord( "false" )/*wordDes.str == "false"*/ )
			{
				pCurObj = CHE_PDF_Boolean::Create( FALSE, GetAllocator() );
			}else if ( m_sParser.IsWord( "true" ) /*wordDes.str == "true"*/ )
			{
				pCurObj = CHE_PDF_Boolean::Create( TRUE, GetAllocator() );
			}else if ( m_sParser.IsWord( "null" ) /*wordDes.str == "null"*/ )
			{
				pCurObj = CHE_PDF_Null::Create( GetAllocator() );
			}else if ( m_sParser.IsWord( "endobj" ) /*wordDes.str == "endobj"*/ )
			{
				pCurObj = CHE_PDF_Null::Create( GetAllocator() );
				return pCurObj;
			}
			break;
		}
    default:
        break;
	}

	if ( pCurObj )
	{
		return pCurObj;
	}
	return CHE_PDF_ObjectPtr();
}
CHE_PDF_ObjectPtr CHE_PDF_Parser::GetObjectInObjStm( CHE_PDF_StreamPtr & pStream, const PDF_RefInfo & objRefInfo, HE_ULONG index )
{
	if ( !pStream )
	{
		return CHE_PDF_ObjectPtr();
	}

	CHE_PDF_StreamAcc stmAcc( GetAllocator() );
	stmAcc.Attach( pStream );
	HE_LPCBYTE pData = stmAcc.GetData();
	HE_ULONG lDataSize = stmAcc.GetSize();
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
	HE_ULONG				first = 0;
	HE_ULONG				offset = 0;
	HE_ULONG				objNumTmp = 0;
	sParser.SetPos( 0 );

	first = pStream->GetDictPtr()->GetElement( "First" )->GetNumberPtr()->GetInteger();

	for ( HE_ULONG i = 0; i < index; i++ )
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

			objNumTmp = sParser.GetInteger();//wordDes.str.GetInteger();

			sParser.GetWord( wordDes );
			if ( wordDes.type != PARSE_WORD_INTEGER )
			{
				sParser.SetPos( offset );
				continue;
			}
			first += m_sParser.GetInteger();//wordDes.str.GetInteger();
			if ( objNumTmp == objRefInfo.objNum )
			{
				sParser.SetPos( first );
				HE_ULONG pos = sParser.GetPos();
				sParser.GetWord( wordDes );
				CHE_PDF_ObjectPtr pCurObj;
				if ( wordDes.type == PARSE_WORD_DICT_B )
				{
					sParser.SetPos( pos );
					pCurObj = sParser.GetDictionaryPtr();
				}else if ( wordDes.type == PARSE_WORD_FLOAT )
				{
					pCurObj = CHE_PDF_Number::Create( sParser.GetInteger()/*wordDes.str.GetInteger()*/, GetAllocator() );
				}else if ( wordDes.type == PARSE_WORD_INTEGER )
				{
					HE_ULONG refNum = sParser.GetInteger();//wordDes.str.GetInteger();
					HE_ULONG pos = sParser.GetPos();
					sParser.GetWord( wordDes );
					if ( wordDes.type == PARSE_WORD_INTEGER )
					{
						HE_ULONG refGenNum = sParser.GetInteger();//wordDes.str.GetInteger();
						sParser.GetWord( wordDes );
						if ( sParser.IsWord( "R" )/*wordDes.str == "R"*/ )
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
					pCurObj = CHE_PDF_String::Create( sParser.GetString()/*wordDes.str*/, GetAllocator() );
				}else if ( wordDes.type == PARSE_WORD_NAME )
				{
					pCurObj = CHE_PDF_Name::Create( sParser.GetString()/*wordDes.str*/, GetAllocator() );
				}else if ( wordDes.type == PARSE_WORD_ARRAY_B )
				{
					sParser.SetPos( pos );
					pCurObj = sParser.GetArrayPtr();
				}else if ( sParser.IsWord( "false" )/*wordDes.str == "false"*/ )
				{
					pCurObj = CHE_PDF_Boolean::Create( FALSE, GetAllocator() );
				}else if ( sParser.IsWord( "true" )/*wordDes.str == "true"*/ )
				{
					pCurObj = CHE_PDF_Boolean::Create( TRUE, GetAllocator() );
				}else if ( sParser.IsWord( "null" )/*wordDes.str == "null"*/ )
				{
					pCurObj = CHE_PDF_Null::Create( GetAllocator() );
				}else{
					pCurObj.Reset();
				}
				return pCurObj;
			}
		}
	}
	return CHE_PDF_ObjectPtr();
}

HE_BOOL CHE_PDF_Parser::GetAllObjectsInObjStm( CHE_PDF_StreamPtr & pStream, CHE_PDF_Collector * pCollector )
{
	if ( pCollector == NULL )
	{
		return FALSE;
	}
	if ( !pStream )
	{
		return FALSE;
	}

	CHE_PDF_StreamAcc stmAcc( GetAllocator() );
	stmAcc.Attach( pStream );
	HE_LPCBYTE pData = stmAcc.GetData();
	HE_ULONG lDataSize = stmAcc.GetSize();
	IHE_Read * pIHE_Read = HE_CreateMemBufRead( (HE_LPBYTE)pData, lDataSize, GetAllocator() );
	if ( pIHE_Read == NULL )
	{
		stmAcc.Detach();
		return FALSE;
	}

	CHE_PDF_SyntaxParser sParser( mpFile, GetAllocator() );
	sParser.InitParser( pIHE_Read );
	sParser.SetPos( 0 );

	CHE_PDF_ParseWordDes	wordDes( GetAllocator() );
	HE_ULONG				first = 0;
	HE_ULONG				count = 0;
	HE_ULONG				offset = 0;
	HE_ULONG				pposi = 0;
	HE_ULONG				objNum = 0;

	first = pStream->GetDictPtr()->GetElement( "First" )->GetNumberPtr()->GetInteger();
	count = pStream->GetDictPtr()->GetElement( "N" )->GetNumberPtr()->GetInteger();

	for ( HE_ULONG i = 0; i < count; i++ )
	{
// 		if ( sParser.GetPos() == sParser.GetFileSize() )
// 		{
// 			//error
// 			return FALSE;
// 		}
		sParser.GetWord( wordDes );

		if ( wordDes.type == PARSE_WORD_INTEGER )
		{
			objNum = sParser.GetInteger();//wordDes.str.GetInteger();

			sParser.SetCurObjNum( objNum );
			sParser.SetCurGenNum( 0 );

			sParser.GetWord( wordDes );
			if ( wordDes.type == PARSE_WORD_INTEGER )
			{
				offset = sParser.GetInteger();//wordDes.str.GetInteger();

				pposi = sParser.GetPos();

				sParser.SetPos( first + offset );

				HE_ULONG pos = sParser.GetPos();

				sParser.GetWord( wordDes );

				CHE_PDF_ObjectPtr pCurObj;
				if ( wordDes.type == PARSE_WORD_DICT_B )
				{
					sParser.SetPos( pos );
					pCurObj = sParser.GetDictionaryPtr();
				}else if ( wordDes.type == PARSE_WORD_FLOAT )
				{
					pCurObj = CHE_PDF_Number::Create( sParser.GetInteger()/*wordDes.str.GetInteger()*/, GetAllocator() );
				}else if ( wordDes.type == PARSE_WORD_INTEGER )
				{
					HE_ULONG refNum = sParser.GetInteger();//wordDes.str.GetInteger();
					HE_ULONG pos = sParser.GetPos();
					sParser.GetWord( wordDes );
					if ( wordDes.type == PARSE_WORD_INTEGER )
					{
						HE_ULONG refGenNum = sParser.GetInteger() ;//wordDes.str.GetInteger();
						sParser.GetWord( wordDes );
						if ( sParser.IsWord( "R" )/*wordDes.str == "R"*/ )
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
					pCurObj = CHE_PDF_String::Create( sParser.GetString()/*wordDes.str*/, GetAllocator() );
				}else if ( wordDes.type == PARSE_WORD_NAME )
				{
					pCurObj = CHE_PDF_Name::Create( sParser.GetString()/*wordDes.str*/, GetAllocator() );
				}else if ( wordDes.type == PARSE_WORD_ARRAY_B )
				{
					sParser.SetPos( pos );
					pCurObj = sParser.GetArrayPtr();
				}else if ( /*wordDes.str*/sParser.GetString() == "false" )
				{
					pCurObj = CHE_PDF_Boolean::Create( FALSE, GetAllocator() );
				}else if ( /*wordDes.str*/sParser.GetString() == "true" )
				{
					pCurObj = CHE_PDF_Boolean::Create( TRUE, GetAllocator() );
				}else if ( /*wordDes.str*/sParser.GetString() == "null" )
				{
					pCurObj = CHE_PDF_Null::Create( GetAllocator() );
				}else{
					pCurObj.Reset();
				}
				pCollector->Add( objNum, pCurObj );
			}
			sParser.SetPos( pposi );
		}



		



 	}

	return TRUE;

// 	while ( true )
// 	{
// 
// 
// 		if ( wordDes.type == PARSE_WORD_INTEGER )
// 		{
// 			
// 
// 			
// 
// 
// 			if ( objNumTmp == objRefInfo.objNum )
// 			{
// 				
// 			}
// 		}
// 	}
// 	return CHE_PDF_ObjectPtr();
}