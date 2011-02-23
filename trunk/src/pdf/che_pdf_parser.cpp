#include "../../include/pdf/che_pdf_parser.h"

#include <string.h>

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
							m_lFilePos++;
							m_WordBuffer[m_lBufferPos++] = '(';

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
									m_WordBuffer[m_lBufferPos++] = ')';
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
						m_WordBuffer[m_lBufferPos=0] = '/';
						m_lBufferPos++;
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
						m_WordBuffer[m_lBufferPos++] = '(';
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

							m_WordBuffer[m_lBufferPos++] = byte;
							m_lFilePos++;
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

CHE_PDF_Parser::CHE_PDF_Parser()
{
	m_pIHE_FileRead = NULL;
}

HE_BOOL CHE_PDF_Parser::StartParse( IHE_FileRead * file )
{
	if ( file == NULL )
	{
		return false;
	}else{
		m_pIHE_FileRead = file;
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

			HE_DWORD iStartXref = 0;

			for ( unsigned int m = m_pIHE_FileRead->GetSize(); m >= 8; m-- )
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
			return iStartXref;
		}else{
			return 0;
		}
	}
	return 0;
}