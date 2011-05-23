#include "../../include/pdf/che_pdf_parser.h"
#include "../../include/pdf/che_pdf_encrypt.h"
#include "../../include/che_datastructure.h"
#include <cstring>
//#include <cstdio>

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

class IHE_DefaultGetPDFInObj : public IHE_PDF_GetInObj
{
public:
	IHE_DefaultGetPDFInObj( CHE_PDF_Parser * pParser, CHE_Allocator * pAllocator ) : IHE_PDF_GetInObj( pAllocator )
	{ m_pParser = pParser; }

	~IHE_DefaultGetPDFInObj() {};

	CHE_PDF_IndirectObject * GetInObj( HE_DWORD objNum ) { return ( m_pParser!= NULL) ? m_pParser->GetIndirectObject(objNum) : NULL; }

	CHE_PDF_Object * GetObj( HE_DWORD objNum, HE_BYTE objType = 0 );

private:
	CHE_PDF_Parser * m_pParser;
};

CHE_PDF_Object * IHE_DefaultGetPDFInObj::GetObj( HE_DWORD objNum, HE_BYTE objType /*= 0*/ )
{
	if ( objNum == 0 )
	{
		return NULL;
	}
	if ( m_pParser == NULL )
	{
		return NULL;
	}

	CHE_PDF_IndirectObject * pInObj = m_pParser->GetIndirectObject( objNum );
	if ( pInObj == NULL )
	{
		return NULL;
	}
	if ( pInObj->GetObject() == NULL )
	{
		return NULL;
	}
	CHE_PDF_Object * pCurObj = pInObj->GetObject();
	if ( objType == PDFOBJ_INVALID )
	{
		return pCurObj;
	}
	if ( objType == pCurObj->GetType() )
	{
		return pCurObj;
	}
	if( pCurObj->GetType() != PDFOBJ_ARRAY && pCurObj->GetType() != PDFOBJ_REFERENCE )
	{
		return NULL;
	}
	while ( TRUE )
	{
		if ( pCurObj->GetType() == PDFOBJ_ARRAY )
		{
			//不处理数组下面的数组，只获取第一个可能，也即是可能忽略掉一些数组元素
			CHE_PDF_Object * pTmpObj = NULL;
			HE_BOOL	bContinue = FALSE;
			HE_DWORD lCount = ((CHE_PDF_Array*)pCurObj)->GetCount();
			for ( HE_DWORD i = 0; i < lCount; i++ )
			{
				pTmpObj = ((CHE_PDF_Array*)pCurObj)->GetElement( i );
				if ( pTmpObj == NULL )
				{
					break;
				}
				if ( pTmpObj->GetType() == objType )
				{
					return pTmpObj;
				}else if ( pTmpObj->GetType() == PDFOBJ_REFERENCE )
				{
					pCurObj = pTmpObj;
					bContinue = TRUE;
				}
			}
			if ( bContinue == TRUE )
			{
				continue;
			}else{
				return NULL;
			}
		}else if ( pCurObj->GetType() == PDFOBJ_REFERENCE )
		{
			pInObj = m_pParser->GetIndirectObject( ((CHE_PDF_Reference*)pCurObj)->GetRefNuml() );
			if (pInObj == NULL )
			{
				return NULL;
			}
			pCurObj = pInObj->GetObject();
			if ( pCurObj == NULL )
			{
				return NULL;
			}
			if ( pCurObj->GetType() == objType )
			{
				return pCurObj;
			}
			if ( pCurObj->GetType() == PDFOBJ_REFERENCE || pCurObj->GetType() == PDFOBJ_ARRAY )
			{
				continue;
			}
			return NULL;
		}else if ( pCurObj->GetType() == objType )
		{
			return pCurObj;
		}else{
			return NULL;
		}
	}
	return pCurObj;
}

CHE_PDF_SyntaxParser::CHE_PDF_SyntaxParser( CHE_Allocator * pAllocator ) : CHE_Object( pAllocator )
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

HE_BOOL CHE_PDF_SyntaxParser::GetWord( CHE_PDF_PARSER_WORD_DES & des )
{
	if ( m_pFileAccess )
	{
		des.offset = 0;
		des.type = PDFPARSER_WORD_UNKNOWN;

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
							des.type = PDFPARSER_WORD_DICT_B;
							des.str = "<<";
							m_lFilePos+=2;
							return TRUE;
						}else{
							//字符串处理
							des.offset = m_lFilePos;
							des.type = PDFPARSER_WORD_STRING;
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
							des.type = PDFPARSER_WORD_DICT_E;
							des.str = ">>";
							m_lFilePos += 2;
							return TRUE;
						}
					}
					break;
				}
			case '[':	//数组
				{
					if ( m_lBufferPos > 0 )
					{
						SubmitBufferStr( des.str );
						return TRUE;
					}else{
						des.offset = m_lFilePos;
						des.type = PDFPARSER_WORD_ARRAY_B;
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
						des.type = PDFPARSER_WORD_ARRAY_E;
						des.str = "]";
						m_lFilePos++;
						return TRUE;
					}
					break;
				}
			case '/':	//名称
				{
					if ( m_lBufferPos > 0 )
					{
						SubmitBufferStr( des.str );
						return TRUE;
					}else{
						des.offset = m_lFilePos;
						des.type = PDFPARSER_WORD_NAME;
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
						des.type = PDFPARSER_WORD_STRING;
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
						des.type = PDFPARSER_WORD_INTEGER;
					}else{
						des.type = PDFPARSER_WORD_UNKNOWN;
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
						des.type = PDFPARSER_WORD_FLOAT;
					}else{
						des.type = PDFPARSER_WORD_UNKNOWN;
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
						des.type = PDFPARSER_WORD_INTEGER;
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
					if ( des.type != PDFPARSER_WORD_UNKNOWN )
					{
						des.type = PDFPARSER_WORD_UNKNOWN;
					}
					if ( des.offset == 0 )
					{
						des.offset = m_lFilePos;
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

	CHE_PDF_PARSER_WORD_DES wordDes( GetAllocator() );

	if ( !GetWord( wordDes ) )
	{
		return NULL;
	}
	if ( wordDes.type != PDFPARSER_WORD_ARRAY_B )
	{
		return NULL;
	}

	HE_BOOL bKey = FALSE;
	HE_BOOL bOver = FALSE;
	CHE_ByteString key( GetAllocator() );
	CHE_PtrStack stack( GetAllocator() );
	CHE_PtrStack stackName( GetAllocator() );
	CHE_PDF_Object * pCurObj = CHE_PDF_Array::Create( m_dwCurObjNum, m_dwCurGenNum, GetAllocator() );

	while ( bOver == FALSE )
	{
		if ( bKey )
		{
			if ( !GetWord( wordDes ) )
			{
				return NULL;
			}
			if ( wordDes.type == PDFPARSER_WORD_DICT_E  )
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
						GetAllocator()->Delete<CHE_ByteString>( pTmpStr );
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
			if ( !GetWord( wordDes ) )
			{
				return NULL;
			}
			if ( wordDes.type == PDFPARSER_WORD_INTEGER )	//整数和引用
			{
				HE_DWORD objNum = 0, genNum = 0;
				objNum = HE_PDF_StringToInteger( wordDes.str );

				HE_DWORD lPos = GetPos();
				if ( !GetWord( wordDes ) )
				{
					return NULL;
				}
				if ( wordDes.type == PDFPARSER_WORD_INTEGER )
				{
					genNum = HE_PDF_StringToInteger( wordDes.str );
					if ( !GetWord( wordDes ) )
					{
						return NULL;
					}
					if ( wordDes.str == "R" )
					{
						if ( pCurObj->GetType() == PDFOBJ_DICTIONARY )
						{
							((CHE_PDF_Dictionary*)pCurObj)->SetAtReference( key, objNum );
						}else{
							CHE_PDF_Reference * pTmp = CHE_PDF_Reference::Create( objNum, m_dwCurObjNum, m_dwCurGenNum, GetAllocator() );
							((CHE_PDF_Array*)pCurObj)->Append( pTmp );
						}
					}else{
						if ( pCurObj->GetType() == PDFOBJ_DICTIONARY )
						{
							return NULL;
						}else{
							CHE_PDF_Number * pTmp = CHE_PDF_Number::Create( (HE_INT32)objNum, m_dwCurObjNum, m_dwCurGenNum, GetAllocator() );
							((CHE_PDF_Array*)pCurObj)->Append( pTmp );
							pTmp = CHE_PDF_Number::Create( (HE_INT32)genNum, m_dwCurObjNum, m_dwCurGenNum, GetAllocator() );
							((CHE_PDF_Array*)pCurObj)->Append( pTmp );
						}
					}
				}else{
					SetPos( lPos );
					if ( pCurObj->GetType() == PDFOBJ_DICTIONARY )
					{
						((CHE_PDF_Dictionary*)pCurObj)->SetAtInteger( key, objNum );
					}else{
						CHE_PDF_Number * pTmp = CHE_PDF_Number::Create( (HE_INT32)objNum, m_dwCurObjNum, m_dwCurGenNum, GetAllocator() );
						((CHE_PDF_Array*)pCurObj)->Append( pTmp );
					}
				}
			}else if ( wordDes.type == PDFPARSER_WORD_FLOAT )	//浮点
			{
				if ( pCurObj->GetType() == PDFOBJ_DICTIONARY )
				{
					((CHE_PDF_Dictionary*)pCurObj)->SetAtFloatNumber( key, HE_PDF_StringToFloat( wordDes.str ) );
				}else{
					CHE_PDF_Number * pTmp = CHE_PDF_Number::Create( HE_PDF_StringToFloat( wordDes.str ), m_dwCurObjNum, m_dwCurGenNum, GetAllocator() );
					((CHE_PDF_Array*)pCurObj)->Append( pTmp );
				}
			}else if ( wordDes.type == PDFPARSER_WORD_STRING )	//字符串
			{
				if ( pCurObj->GetType() == PDFOBJ_DICTIONARY )
				{
					((CHE_PDF_Dictionary*)pCurObj)->SetAtString( key, wordDes.str );
				}else{
					CHE_PDF_String * pTmp = CHE_PDF_String::Create( wordDes.str, m_dwCurObjNum, m_dwCurGenNum, GetAllocator() );
					((CHE_PDF_Array*)pCurObj)->Append( pTmp );
				}
			}else if ( wordDes.type == PDFPARSER_WORD_NAME )	//名称
			{
				if ( pCurObj->GetType() == PDFOBJ_DICTIONARY )
				{
					((CHE_PDF_Dictionary*)pCurObj)->SetAtName( key, wordDes.str );
				}else{
					CHE_PDF_Name * pTmp = CHE_PDF_Name::Create( wordDes.str, m_dwCurObjNum, m_dwCurGenNum, GetAllocator() );
					((CHE_PDF_Array*)pCurObj)->Append( pTmp );
				}
			}else if ( wordDes.type == PDFPARSER_WORD_ARRAY_B )	//数组
			{
				stack.Push( pCurObj );
				pCurObj = CHE_PDF_Array::Create( m_dwCurObjNum, m_dwCurGenNum, GetAllocator() );
			}else if ( wordDes.type == PDFPARSER_WORD_ARRAY_E )	//数组
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
			}else if ( wordDes.type == PDFPARSER_WORD_DICT_B )	//字典
			{
				if ( pCurObj->GetType() == PDFOBJ_DICTIONARY )
				{
					CHE_ByteString * pStr = GetAllocator()->New<CHE_ByteString>( key );
					stackName.Push( (CHE_PDF_Object*)pStr );
				}
				stack.Push( pCurObj );
				pCurObj = CHE_PDF_Dictionary::Create( m_dwCurObjNum, m_dwCurGenNum, GetAllocator() );
			}else if ( wordDes.type == PDFPARSER_WORD_DICT_E )	//字典	只出现在array的情况
			{
				CHE_PDF_Object * pTmp = pCurObj;
				stack.Pop( (HE_VOID**)&pCurObj );
				((CHE_PDF_Array*)pCurObj)->Append( pTmp );
			}else{	//布尔和其他
				if ( wordDes.str == "false" )
				{
					if ( pCurObj->GetType() == PDFOBJ_DICTIONARY )
					{
						((CHE_PDF_Dictionary*)pCurObj)->SetAtBoolean( key, false );
					}else{
						CHE_PDF_Boolean * pTmp = CHE_PDF_Boolean::Create( FALSE, m_dwCurObjNum, m_dwCurGenNum, GetAllocator() );
						((CHE_PDF_Array*)pCurObj)->Append( pTmp );
					}
				}else if ( wordDes.str == "true" )
				{
					if ( pCurObj->GetType() == PDFOBJ_DICTIONARY )
					{
						((CHE_PDF_Dictionary*)pCurObj)->SetAtBoolean( key, true );
					}else{
						CHE_PDF_Boolean * pTmp = CHE_PDF_Boolean::Create( TRUE, m_dwCurObjNum, m_dwCurGenNum, GetAllocator() );
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
	CHE_PDF_PARSER_WORD_DES wordDes( GetAllocator() );
	CHE_PDF_PARSER_WORD_DES wordDesKey( GetAllocator() );
	GetWord( wordDes );
	if ( wordDes.type != PDFPARSER_WORD_DICT_B )
	{
		return NULL;
	}

	HE_BOOL bKey = TRUE;
	HE_BOOL bOver = FALSE;
	CHE_PtrStack stack( GetAllocator() );
	CHE_PtrStack stackName( GetAllocator() );
	CHE_PDF_Object * pCurObj = CHE_PDF_Dictionary::Create( m_dwCurObjNum, m_dwCurGenNum, GetAllocator() );

	while ( bOver == FALSE )
	{
		if ( bKey )
		{
			GetWord( wordDesKey );
			if ( wordDesKey.type == PDFPARSER_WORD_NAME )
			{
				bKey = FALSE;
			}else if (  wordDesKey.type == PDFPARSER_WORD_DICT_E  )
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
						wordDesKey.str = *pTmpStr;
						GetAllocator()->Delete<CHE_ByteString>( pTmpStr );
						((CHE_PDF_Dictionary*)pCurObj)->SetAtDictionary( wordDesKey.str, pDict );
						
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
			GetWord( wordDes );
			if ( wordDes.type == PDFPARSER_WORD_INTEGER )	//整数和引用
			{
				HE_DWORD objNum = 0, genNum = 0;
				objNum = HE_PDF_StringToInteger( wordDes.str );

				HE_DWORD lPos = GetPos();
				GetWord( wordDes );
				if ( wordDes.type == PDFPARSER_WORD_INTEGER )
				{
					genNum = HE_PDF_StringToInteger( wordDes.str );
					GetWord( wordDes );
					if ( wordDes.str == "R" )
					{
						if ( pCurObj->GetType() == PDFOBJ_DICTIONARY )
						{
							((CHE_PDF_Dictionary*)pCurObj)->SetAtReference( wordDesKey.str, objNum );
						}else{
							CHE_PDF_Reference * pTmp = CHE_PDF_Reference::Create( objNum, m_dwCurObjNum, m_dwCurGenNum, GetAllocator() );
							((CHE_PDF_Array*)pCurObj)->Append( pTmp );
						}
					}else{
						if ( pCurObj->GetType() == PDFOBJ_DICTIONARY )
						{
							return NULL;
						}else{
							CHE_PDF_Number * pTmp = CHE_PDF_Number::Create( (HE_INT32)objNum, m_dwCurObjNum, m_dwCurGenNum, GetAllocator() );
							((CHE_PDF_Array*)pCurObj)->Append( pTmp );
							SetPos( lPos );
						}
					}
				}else{
					SetPos( lPos );
					if ( pCurObj->GetType() == PDFOBJ_DICTIONARY )
					{
						((CHE_PDF_Dictionary*)pCurObj)->SetAtInteger( wordDesKey.str, objNum );
					}else{
						CHE_PDF_Number * pTmp = CHE_PDF_Number::Create( (HE_INT32)objNum, m_dwCurObjNum, m_dwCurGenNum, GetAllocator() );
						((CHE_PDF_Array*)pCurObj)->Append( pTmp );
					}
				}
			}else if ( wordDes.type == PDFPARSER_WORD_FLOAT )	//浮点
			{
				if ( pCurObj->GetType() == PDFOBJ_DICTIONARY )
				{
					((CHE_PDF_Dictionary*)pCurObj)->SetAtFloatNumber( wordDesKey.str, HE_PDF_StringToFloat( wordDes.str ) );
				}else{
					CHE_PDF_Number * pTmp = CHE_PDF_Number::Create( HE_PDF_StringToFloat( wordDes.str ), m_dwCurObjNum, m_dwCurGenNum, GetAllocator() );
					((CHE_PDF_Array*)pCurObj)->Append( pTmp );
				}
			}else if ( wordDes.type == PDFPARSER_WORD_STRING )	//字符串
			{
				if ( pCurObj->GetType() == PDFOBJ_DICTIONARY )
				{
					((CHE_PDF_Dictionary*)pCurObj)->SetAtString( wordDesKey.str, wordDes.str );
				}else{
					CHE_PDF_String * pTmp = CHE_PDF_String::Create( wordDes.str, m_dwCurObjNum, m_dwCurGenNum, GetAllocator() );
					((CHE_PDF_Array*)pCurObj)->Append( pTmp );
				}
			}else if ( wordDes.type == PDFPARSER_WORD_NAME )	//名称
			{
				if ( pCurObj->GetType() == PDFOBJ_DICTIONARY )
				{
					((CHE_PDF_Dictionary*)pCurObj)->SetAtName( wordDesKey.str, wordDes.str );
				}else{
					CHE_PDF_Name * pTmp = CHE_PDF_Name::Create( wordDes.str, m_dwCurObjNum, m_dwCurGenNum, GetAllocator() );
					((CHE_PDF_Array*)pCurObj)->Append( pTmp );
				}
			}else if ( wordDes.type == PDFPARSER_WORD_ARRAY_B )	//数组
			{
				stack.Push( pCurObj );
				pCurObj = CHE_PDF_Array::Create( m_dwCurObjNum, m_dwCurGenNum, GetAllocator() );
			}else if ( wordDes.type == PDFPARSER_WORD_ARRAY_E )	//数组
			{
				if ( stack.IsEmpty() == FALSE )
				{
					CHE_PDF_Object * pTmp = pCurObj;
					stack.Pop( (HE_VOID**)&pCurObj );
					if ( pCurObj->GetType() == PDFOBJ_DICTIONARY )
					{
						((CHE_PDF_Dictionary*)pCurObj)->SetAtArray( wordDesKey.str, (CHE_PDF_Array*)pTmp );
					}else{
						((CHE_PDF_Array*)pCurObj)->Append( pTmp );
					}
				}
			}else if ( wordDes.type == PDFPARSER_WORD_DICT_B )	//字典
			{
				if ( pCurObj->GetType() == PDFOBJ_DICTIONARY )
				{
					CHE_ByteString * pStr = GetAllocator()->New<CHE_ByteString>( wordDesKey.str );
					stackName.Push( (CHE_PDF_Object*)pStr );
				}
				stack.Push( pCurObj );
				pCurObj = CHE_PDF_Dictionary::Create( m_dwCurObjNum, m_dwCurGenNum, GetAllocator() );
			}else if ( wordDes.type == PDFPARSER_WORD_DICT_E )	//字典	只出现在array的情况
			{
				CHE_PDF_Object * pTmp = pCurObj;
				stack.Pop( (HE_VOID**)&pCurObj );
				((CHE_PDF_Array*)pCurObj)->Append( pTmp );
			}else{	//布尔和其他
				if ( wordDes.str == "false" )
				{
					if ( pCurObj->GetType() == PDFOBJ_DICTIONARY )
					{
						((CHE_PDF_Dictionary*)pCurObj)->SetAtBoolean( wordDesKey.str, false );
					}else{
						CHE_PDF_Boolean * pTmp = CHE_PDF_Boolean::Create( FALSE, m_dwCurObjNum, m_dwCurGenNum, GetAllocator() );
						((CHE_PDF_Array*)pCurObj)->Append( pTmp );
					}
				}else if ( wordDes.str == "true" )
				{
					if ( pCurObj->GetType() == PDFOBJ_DICTIONARY )
					{
						((CHE_PDF_Dictionary*)pCurObj)->SetAtBoolean( wordDesKey.str, true );
					}else{
						CHE_PDF_Boolean * pTmp = CHE_PDF_Boolean::Create( TRUE, m_dwCurObjNum, m_dwCurGenNum, GetAllocator() );
						((CHE_PDF_Array*)pCurObj)->Append( pTmp );
					}
				}else if ( wordDes.str == "null" ){
					if ( pCurObj->GetType() == PDFOBJ_DICTIONARY )
					{
						((CHE_PDF_Dictionary*)pCurObj)->SetAtNull( wordDesKey.str );
					}else{
						CHE_PDF_Null * pTmp = CHE_PDF_Null::Create( m_dwCurObjNum, m_dwCurGenNum, GetAllocator() );
						((CHE_PDF_Array*)pCurObj)->Append( pTmp );
					}
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
: CHE_Object( pAllocator ), m_arrObjStm( pAllocator ), m_XrefVerifyMap1( pAllocator ), 
m_XrefVerifyMap2( pAllocator ), m_sParser( pAllocator ), m_xrefTable( pAllocator ), m_objCollector( pAllocator ) 
{
	m_pTrailerDict = NULL;
	m_pIHE_FileRead = NULL;
	m_pStrEncrypt = NULL;
	m_pStmEncrypt = NULL;
	m_pEefEncrypt = NULL;
	m_lstartxref = 0;
	m_lPageCount = 0;
	m_bTrailerDictNeedDestory = TRUE;
	m_pIHE_GetPDFInObj = GetAllocator()->New<IHE_DefaultGetPDFInObj>( this, pAllocator );
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
	m_objCollector.ReleaseObj();
	m_objCollector.Clear();
	if ( m_pIHE_FileRead )
	{
		m_pIHE_FileRead->Release();
		m_pIHE_FileRead = NULL;
	}
	if ( m_pIHE_GetPDFInObj )
	{
		GetAllocator()->Delete<IHE_PDF_GetInObj>( m_pIHE_GetPDFInObj );
		m_pIHE_GetPDFInObj = NULL;
	}
 	if ( m_pTrailerDict && m_bTrailerDictNeedDestory == TRUE )
 	{
 		m_pTrailerDict->Release();
		m_pTrailerDict = NULL;
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
		unsigned char * pBuffer = GetAllocator()->NewArray<unsigned char>( range );;
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
		m_lstartxref = iStartXref;
		return iStartXref;
	}
	return 0;
}

HE_DWORD CHE_PDF_Parser::FullParseForXRef()	//分析整个文件来获取对象信息 // 还需要添加搜索xref的功能，针对某些具有xref但startxref错误的pdf文件
{
	HE_DWORD			xrefEntryCount = 0;
	CHE_PDF_PARSER_WORD_DES	wordDes( GetAllocator() );
	HE_DWORD			offset = 0;
	HE_DWORD			objNumOffset = 0;
	HE_DWORD			objNum = 0;
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
			if ( m_pTrailerDict == NULL )
			{
				m_pTrailerDict = m_sParser.GetDictionary();
			}
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
			if ( wordDes.type != PDFPARSER_WORD_INTEGER )
			{
				m_sParser.SetPos( offset );
				continue;
			}
			objNum = HE_PDF_StringToInteger( wordDes.str );
			m_sParser.GetWord( wordDes );
			if ( wordDes.type != PDFPARSER_WORD_INTEGER )
			{
				m_sParser.SetPos( offset );
				continue;
			}
			m_xrefTable.NewSection( objNum );
			CHE_PDF_XREF_Entry tmpEntry( 1, objNumOffset, 0, objNum );
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
						if ( m_sParser.ReadBytes( /*(HE_BYTE*)*/tmp, 9 ) == 0 )
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

HE_DWORD CHE_PDF_Parser::ParseXRefTable( HE_DWORD offset, CHE_PDF_Dictionary ** pTrailerDictRet )
{
	HE_DWORD orgOffset = m_sParser.GetPos();
	m_sParser.SetPos( offset );
	*pTrailerDictRet = NULL;

	CHE_PDF_PARSER_WORD_DES wordDes( GetAllocator() );
	HE_DWORD xrefEntryCount = 0;
	if ( m_sParser.GetWord( wordDes ) == FALSE )
	{
		m_sParser.SetPos( orgOffset );
		return 0; 
	}
	if ( wordDes.str != "xref" )
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
		if ( wordDes.str == "trailer" )
		{
			*pTrailerDictRet = m_sParser.GetDictionary();
		}
		if( wordDes.type == PDFPARSER_WORD_INTEGER )
		{
			lBeginNum = HE_PDF_StringToInteger( wordDes.str );
		}else{
			break;
		}
		if ( m_sParser.GetWord( wordDes ) == FALSE ) break;
		if( wordDes.type == PDFPARSER_WORD_INTEGER )
		{
			lCount = HE_PDF_StringToInteger( wordDes.str );
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

			objOffset =	  (tmpBytes[0] - 48) * 1000000000 + (tmpBytes[1] - 48) * 100000000 + (tmpBytes[2] - 48) * 10000000
				+ (tmpBytes[3] - 48) * 1000000 + (tmpBytes[4] - 48) * 100000 + (tmpBytes[5] - 48) * 10000
				+ (tmpBytes[6] - 48) * 1000 + (tmpBytes[7] - 48) * 100 + (tmpBytes[8] - 48) * 10 + (tmpBytes[9] - 48); 
			objGenNum =	(tmpBytes[11] - 48) * 10000 + (tmpBytes[12] - 48) * 1000 + (tmpBytes[13] - 48) * 100 + (tmpBytes[14] - 48) * 10 + tmpBytes[15] - 48;
			if ( tmpBytes[17] == 'f' )
			{
				CHE_PDF_XREF_Entry tmpEntry( 0, objOffset, objGenNum, i );
				m_xrefTable.NewNode( tmpEntry );
				xrefEntryCount++;
			}else{
				CHE_PDF_XREF_Entry tmpEntry( 1, objOffset, objGenNum, i );
				m_xrefTable.NewNode( tmpEntry );
				xrefEntryCount++;
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
	*pTrailerDictRet = NULL;
	
	CHE_PDF_PARSER_WORD_DES wordDes( GetAllocator() );
	HE_DWORD xrefEntryCount = 0;
	HE_DWORD objNum = 0, genNum = 0, lBeginNum = 0, lObjCount = 0, lSize = 0, lW1 = 0, lW2 = 0, lW3 = 0, lSecCount = 0;

	CHE_PDF_IndirectObject * pIndObj = GetIndirectObject();
	if ( pIndObj == NULL || pIndObj->GetType() != PDFOBJ_INDIRECTOBJ )
	{
		m_sParser.SetPos( orgOffset );
		return 0;
	}
	CHE_PDF_Dictionary * pDict = pIndObj->GetDict();
	if ( pDict == NULL )
	{
		m_sParser.SetPos( orgOffset );
		return 0;
	}else{
		*pTrailerDictRet = pDict;
	}
	CHE_PDF_Object * pElement =  pDict->GetElement( "Type" );
	if ( pElement == NULL || pElement->GetType() != PDFOBJ_NAME )
	{
		m_sParser.SetPos( orgOffset );
		return 0;
	}
	if( ((CHE_PDF_Name*)pElement)->GetString() != "XRef" )
	{
		m_sParser.SetPos( orgOffset );
		return 0;
	}
	pElement = pDict->GetElement( "Size" );
	if ( pElement == NULL || pElement->GetType() != PDFOBJ_NUMBER )
	{
		m_sParser.SetPos( orgOffset );
		return 0;
	}
	lSize = ((CHE_PDF_Number*)pElement)->GetInteger();

	pElement = pDict->GetElement( "W" );
	if ( pElement == NULL || pElement->GetType() != PDFOBJ_ARRAY )
	{
		m_sParser.SetPos( orgOffset );
		return 0;
	}
	if ( ((CHE_PDF_Array*)pElement)->GetElement( 0 )->GetType() != PDFOBJ_NUMBER 
		|| ((CHE_PDF_Array*)pElement)->GetElement( 1 )->GetType() != PDFOBJ_NUMBER
		|| ((CHE_PDF_Array*)pElement)->GetElement( 2 )->GetType() != PDFOBJ_NUMBER )
	{
		m_sParser.SetPos( orgOffset );
		return 0;
	}
	lW1 = ((CHE_PDF_Number*)((CHE_PDF_Array*)pElement)->GetElement( 0 ))->GetInteger();
	lW2	= ((CHE_PDF_Number*)((CHE_PDF_Array*)pElement)->GetElement( 1 ))->GetInteger();
	lW3	= ((CHE_PDF_Number*)((CHE_PDF_Array*)pElement)->GetElement( 2 ))->GetInteger();

	pElement = pDict->GetElement( "Index" );
	if ( pElement == NULL || pElement->GetType() != PDFOBJ_ARRAY )
	{
		lSecCount = 1;
	}else{
		if ( ((CHE_PDF_Array*)pElement)->GetCount() < 2 )
		{
			m_sParser.SetPos( orgOffset );
			return 0;
		}
		lSecCount = ((CHE_PDF_Array*)pElement)->GetCount() / 2; 
	}

	CHE_PDF_StreamAcc streamAcc( GetAllocator() );
	if ( streamAcc.Attach( pIndObj->GetStream() ) == FALSE )
	{
		m_sParser.SetPos( orgOffset );
		return 0;
	}
	HE_DWORD streamSize = streamAcc.GetSize();
	HE_LPCBYTE lpByte = streamAcc.GetData();
//    FILE * pFile = fopen( "c:\\11.txt", "wb+" );
//    fwrite( lpByte, 1, streamSize, pFile );
//	fclose( pFile );
	HE_DWORD field1 = 0, field2 = 0, field3 = 0;
	HE_DWORD lBlockCount = 0;
	HE_DWORD lentrySize = lW1 + lW2 + lW3;
	HE_DWORD lItemOfSetCount = 0; 

	for ( HE_DWORD i = 0; i < lSecCount; i++ )
	{
		if ( pElement == NULL )
		{
			lBeginNum = 0;
			if ( streamSize / lentrySize > lSize )
			{
				lObjCount = streamSize / lentrySize;
			}else{
				lObjCount = lSize;
			}
		}else{
			lBeginNum = ((CHE_PDF_Number*)((CHE_PDF_Array*)pElement)->GetElement( i * 2 ))->GetInteger();
			lObjCount	= ((CHE_PDF_Number*)((CHE_PDF_Array*)pElement)->GetElement( i * 2 + 1 ))->GetInteger();
		}
		m_xrefTable.NewSection( lBeginNum );
		lItemOfSetCount = lObjCount;
		while ( lItemOfSetCount > 0 )
		{
			lItemOfSetCount--;
			if ( ( lBlockCount + 1 ) * lentrySize > streamSize )
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
					HE_LPVOID pSubMap = m_XrefVerifyMap2.GetItem( field2 );
					if ( pSubMap == NULL )
					{
						CHE_NumToPtrMap * pNewMap = GetAllocator()->New<CHE_NumToPtrMap>( GetAllocator() ) ;
						pNewMap->Append( field3, (HE_LPVOID)1 );
						m_XrefVerifyMap2.Append( field2, pNewMap );
						m_arrObjStm.Append( (HE_LPVOID)field2 );
					}else{
						HE_LPVOID pIndex = ((CHE_NumToPtrMap *)pSubMap)->GetItem( field3 );
						if ( pIndex != NULL )
						{
							break;
						}else{
							((CHE_NumToPtrMap *)pSubMap)->Append( field3, (HE_LPVOID)1 );
						}
					}
					CHE_PDF_XREF_Entry tmpEntry( field1, field2, field3, lBeginNum + xrefEntryCount );
					m_xrefTable.NewNode( tmpEntry );
					xrefEntryCount++;
					break;
				}
			case 1:
				{
					HE_LPVOID pSubMap = m_XrefVerifyMap1.GetItem( field2 );
					if ( pSubMap == NULL )
					{
						m_XrefVerifyMap1.Append( field2, (HE_LPVOID)1 );
					}else{
						break;
					}
					HE_DWORD offsetSave = m_sParser.GetPos();
					m_sParser.SetPos( field2 );
					m_sParser.GetWord( wordDes );
					m_sParser.SetPos( offsetSave );
					if ( wordDes.type != PDFPARSER_WORD_INTEGER ||
						( wordDes.type == PDFPARSER_WORD_INTEGER && 
						HE_PDF_StringToInteger( wordDes.str ) != (HE_INT32)(lBeginNum + xrefEntryCount) )  )
					{
						break;
					}
					CHE_PDF_XREF_Entry tmpEntry( field1, field2, field3, lBeginNum + xrefEntryCount );
					m_xrefTable.NewNode( tmpEntry );
					xrefEntryCount++;
					break;
				}
			case 0:
				{
					CHE_PDF_XREF_Entry tmpEntry( field1, field2, field3, lBeginNum + xrefEntryCount );
					m_xrefTable.NewNode( tmpEntry );
					xrefEntryCount++;
					break;
				}
				break;
			default:
				break;
			}
		}
	}
	streamAcc.Detach();
	m_sParser.SetPos( orgOffset );
	return xrefEntryCount;
}

HE_DWORD CHE_PDF_Parser::ParseXRef()
{
	if ( m_pIHE_FileRead == NULL )
	{
		return 0;
	}
	if ( m_lstartxref == 0 )
	{
		GetStartxrefOffset( m_sParser.GetFileSize() );
		if ( m_lstartxref == 0 )
		{
			return FullParseForXRef();
		}else{
			return ParseXRef();
		}
	}else{
		HE_DWORD xrefEntryCount = 0;
		HE_DWORD offset = m_lstartxref;
		CHE_PDF_Dictionary * pDict = NULL;
		HE_DWORD entryCount = 0;

		while ( TRUE )
		{
			entryCount = ParseXRefTable( offset, &pDict );
			xrefEntryCount += entryCount;
			if ( pDict )
			{
				if ( m_pTrailerDict == NULL )
				{
					m_pTrailerDict = pDict;
				}
				CHE_PDF_Dictionary * pEncryptDict = NULL;
				CHE_PDF_Object * pObj = pDict->GetElement( "Encrypt" );
				if ( pObj != NULL )
				{
					CHE_PDF_IndirectObject * pInEncryptObj = NULL;
					pInEncryptObj = GetIndirectObject( ((CHE_PDF_Reference*)pObj)->GetRefNuml() );
					if ( pInEncryptObj != NULL )
					{
						pObj = pInEncryptObj->GetObject();
						if ( pObj != NULL && pObj->GetType() == PDFOBJ_DICTIONARY )
						{
							pEncryptDict = (CHE_PDF_Dictionary*)pObj;						
						}
					}
				}
				if ( pEncryptDict != NULL )
				{
					pObj = pEncryptDict->GetElement( "Filter" );
					if (	pObj != NULL 
							&& pObj->GetType() == PDFOBJ_NAME 
							&& ((CHE_PDF_Name*)pObj)->GetString() == "Standard" )
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
							if ( pObj != NULL && pObj->GetType() == PDFOBJ_STRING )
							{
								id1 = ((CHE_PDF_String*)pObj)->GetString();
							}
						}
						//获取Length
						pObj = pEncryptDict->GetElement( "Length" );
						if (	pObj != NULL 
							&& pObj->GetType() == PDFOBJ_NUMBER )
						{
							keyLength = ((CHE_PDF_Number*)pObj)->GetInteger();
						}
						//获取R
						pObj = pEncryptDict->GetElement( "R" );
						if (	pObj != NULL 
							&& pObj->GetType() == PDFOBJ_NUMBER )
						{
							revision = ((CHE_PDF_Number*)pObj)->GetInteger();
						}
						//获取P
						pObj = pEncryptDict->GetElement( "P" );
						if (	pObj != NULL 
							&& pObj->GetType() == PDFOBJ_NUMBER )
						{
							pValue = ((CHE_PDF_Number*)pObj)->GetInteger();
						}
						//获取EncryptMetadata
						pObj = pEncryptDict->GetElement( "EncryptMetadata" );
						if (	pObj != NULL 
							&& pObj->GetType() == PDFOBJ_BOOLEAN )
						{
							bMetaData = ((CHE_PDF_Boolean*)pObj)->GetValue();
						}
						//获取U
						pObj = pEncryptDict->GetElement( "U" );
						if (	pObj != NULL 
							&& pObj->GetType() == PDFOBJ_STRING )
						{
							CHE_ByteString str = ((CHE_PDF_String*)pObj)->GetString();
							for ( HE_DWORD i = 0; i < str.GetLength(); i++ )
							{
								U[i] = str[i];
							}
						}
						//获取O
						pObj = pEncryptDict->GetElement( "O" );
						if (	pObj != NULL 
							&& pObj->GetType() == PDFOBJ_STRING )
						{
							CHE_ByteString str = ((CHE_PDF_String*)pObj)->GetString();
							for ( HE_DWORD i = 0; i < str.GetLength(); i++ )
							{
								O[i] = str[i];
							}
						}
						pObj = pEncryptDict->GetElement( "V" );
						if (	pObj != NULL 
								&& pObj->GetType() == PDFOBJ_NUMBER )
						{
							if ( ((CHE_PDF_Number*)pObj)->GetInteger() == 4 )
							{
								m_pStrEncrypt = GetAllocator()->New<CHE_PDF_Encrypt>( id1, O, U, PDFENCRYPT_ALGORITHM_AESV2, keyLength, revision, bMetaData, pValue, GetAllocator() );
								m_pStmEncrypt = m_pStrEncrypt;
								m_pEefEncrypt = m_pStrEncrypt;
								m_pStrEncrypt->Authenticate( "" );
								//m_pStmEncrypt = NULL;
							}else{
								m_pStrEncrypt = GetAllocator()->New<CHE_PDF_Encrypt>( id1, O, U, PDFENCRYPT_ALGORITHM_RC4V1, keyLength, revision, bMetaData, pValue, GetAllocator() );
								m_pStmEncrypt = m_pStrEncrypt;
								m_pEefEncrypt = m_pStrEncrypt;
								m_pStrEncrypt->Authenticate( "" );
								//m_pStmEncrypt = NULL;
							}
							m_sParser.SetEncrypt( m_pStrEncrypt );
						}
					}else{
						//非标准的加密处理，未支持
					}
				}

				pObj = pDict->GetElement( "Prev" );
				if ( pObj != NULL && pObj->GetType() == PDFOBJ_NUMBER )
				{
					offset = ((CHE_PDF_Number*)pObj)->GetInteger();
					if ( pDict != m_pTrailerDict )
					{
						pDict->Release();	//由于该对象没有被间接对象容器管理，所以必须手动释放
					}
					continue;
				}
				if ( pDict != m_pTrailerDict )
				{
					pDict->Release();	//由于该对象没有被间接对象容器管理，所以必须手动释放
				}
			}
			if ( entryCount == 0 )
			{
				entryCount = ParseXRefStream( offset, &pDict );
				xrefEntryCount += entryCount;
				if ( pDict )
				{
					if ( m_pTrailerDict == NULL )
					{
						m_pTrailerDict = pDict;
						m_bTrailerDictNeedDestory = FALSE;
					}
					CHE_PDF_Dictionary * pEncryptDict = NULL;
					CHE_PDF_Object * pObj = pDict->GetElement( "Encrypt" );
					if ( pObj != NULL )
					{
						CHE_PDF_IndirectObject * pInEncryptObj = NULL;
						pInEncryptObj = GetIndirectObject( ((CHE_PDF_Reference*)pObj)->GetRefNuml() );
						if ( pInEncryptObj != NULL )
						{
							pObj = pInEncryptObj->GetObject();
							if ( pObj != NULL && pObj->GetType() == PDFOBJ_DICTIONARY )
							{
								pEncryptDict = (CHE_PDF_Dictionary*)pObj;						
							}
						}
					}
					if ( pEncryptDict != NULL )
					{
						pObj = pEncryptDict->GetElement( "Filter" );
						if (	pObj != NULL 
								&& pObj->GetType() == PDFOBJ_NAME 
								&& ((CHE_PDF_Name*)pObj)->GetString() == "Standard" )
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
								if ( pObj != NULL && pObj->GetType() == PDFOBJ_STRING )
								{
									id1 = ((CHE_PDF_String*)pObj)->GetString();
								}
							}
							//获取Length
							pObj = pEncryptDict->GetElement( "Length" );
							if (	pObj != NULL 
								&& pObj->GetType() == PDFOBJ_NUMBER )
							{
								keyLength = ((CHE_PDF_Number*)pObj)->GetInteger();
							}
							//获取R
							pObj = pEncryptDict->GetElement( "R" );
							if (	pObj != NULL 
								&& pObj->GetType() == PDFOBJ_NUMBER )
							{
								revision = ((CHE_PDF_Number*)pObj)->GetInteger();
							}
							//获取P
							pObj = pEncryptDict->GetElement( "P" );
							if (	pObj != NULL 
								&& pObj->GetType() == PDFOBJ_NUMBER )
							{
								pValue = ((CHE_PDF_Number*)pObj)->GetInteger();
							}
							//获取EncryptMetadata
							pObj = pEncryptDict->GetElement( "EncryptMetadata" );
							if (	pObj != NULL 
								&& pObj->GetType() == PDFOBJ_BOOLEAN )
							{
								bMetaData = ((CHE_PDF_Boolean*)pObj)->GetValue();
							}
							//获取U
							pObj = pEncryptDict->GetElement( "U" );
							if (	pObj != NULL 
								&& pObj->GetType() == PDFOBJ_STRING )
							{
								CHE_ByteString str = ((CHE_PDF_String*)pObj)->GetString();
								for ( HE_DWORD i = 0; i < str.GetLength(); i++ )
								{
									U[i] = str[i];
								}
							}
							//获取O
							pObj = pEncryptDict->GetElement( "O" );
							if (	pObj != NULL 
								&& pObj->GetType() == PDFOBJ_STRING )
							{
								CHE_ByteString str = ((CHE_PDF_String*)pObj)->GetString();
								for ( HE_DWORD i = 0; i < str.GetLength(); i++ )
								{
									O[i] = str[i];
								}
							}
							pObj = pEncryptDict->GetElement( "V" );
							if (	pObj != NULL 
									&& pObj->GetType() == PDFOBJ_NUMBER )
							{
								if ( ((CHE_PDF_Number*)pObj)->GetInteger() == 4 )
								{
// 									CHE_PDF_Dictionary * pCFDict = (CHE_PDF_Dictionary*)(pEncryptDict->GetElement( "CF" ));
// 									if ( pCFDict != NULL )
// 									{
// 										pObj = pEncryptDict->GetElement( "StrF" );
// 										((CHE_PDF_Dictionary*)pObj)->GetElement(  )
// 									}

									m_pStrEncrypt = GetAllocator()->New<CHE_PDF_Encrypt>( id1, O, U, PDFENCRYPT_ALGORITHM_AESV2, keyLength, revision, bMetaData, pValue, GetAllocator() );
									m_pStmEncrypt = m_pStrEncrypt;
									m_pEefEncrypt = m_pStrEncrypt;
									m_pStrEncrypt->Authenticate( "" );
									//m_pStmEncrypt = NULL;
								}else{
									m_pStrEncrypt = GetAllocator()->New<CHE_PDF_Encrypt>( id1, O, U, PDFENCRYPT_ALGORITHM_RC4V1, keyLength, revision, bMetaData, pValue, GetAllocator() );
									m_pStmEncrypt = m_pStrEncrypt;
									m_pEefEncrypt = m_pStrEncrypt;
									m_pStrEncrypt->Authenticate( "" );
									//m_pStmEncrypt = NULL;
								}
								m_sParser.SetEncrypt( m_pStrEncrypt );
							}
						}else{
							//非标准的加密处理，未支持
						}
					}
					pObj = pDict->GetElement( "Prev" );
					if ( pObj != NULL && pObj->GetType() == PDFOBJ_NUMBER )
					{
						offset = ((CHE_PDF_Number*)pObj)->GetInteger();
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
		m_xrefTable.BuildIndex();
		HE_BOOL bNeedVerify = FALSE;
		if ( m_arrObjStm.GetCount() > 0 )
		{
			HE_DWORD objNum = 0;
			for ( HE_DWORD i = 0; i < m_arrObjStm.GetCount(); i++ )
			{
				objNum = (HE_DWORD)m_arrObjStm.GetItem( i );
				if ( objNum == 0 )
				{
					continue;
				}else{
					CHE_PDF_IndirectObject * pInObj = GetIndirectObject( objNum );
					if ( pInObj == NULL )
					{
						bNeedVerify = TRUE;
						break;
					}else{
						CHE_PDF_Dictionary * pDict = pInObj->GetDict();
						if ( pDict == NULL )
						{
							bNeedVerify = TRUE;
							break;
						}else{
							CHE_PDF_Name * pDictName = (CHE_PDF_Name *)pDict->GetElement( "Type" );
							if ( pDictName == NULL )
							{
								bNeedVerify = TRUE;
								break;
							}else{
								if ( pDictName->GetString() != "ObjStm" )
								{
									bNeedVerify = TRUE;
									break;
								}
							}
						}
						CHE_PDF_Stream * pStream = pInObj->GetStream();
						if ( pStream == NULL )
						{
							bNeedVerify = TRUE;
							break;
						}
					}
				}
			}
		}
		if ( bNeedVerify )
		{
			VerifyObjInStm();
		}
// 		FILE * pFile = fopen( "c:\\XRefIndex.txt", "wb+" );
//    		HE_CHAR tmpStr[128];
// 		PDF_XREF_ENTRY_NODE * pTmpNode = NULL;
// 		for ( HE_DWORD i = 0; i < m_xrefTable.m_lMaxObjNum; i++ )
// 		{
// 			pTmpNode = m_xrefTable.m_pFastAccessArr[i];
// 			if ( pTmpNode )
// 			{
// 				sprintf( tmpStr, "obj:%04d - %d %08X %d\r\n", pTmpNode->entry.objNum, pTmpNode->entry.field1, pTmpNode->entry.GetParentObjNum(), pTmpNode->entry.GetIndex() );
// 				fwrite( tmpStr, 1, strlen(tmpStr), pFile );
// 			}
// 		}
// 		fclose( pFile );
		return xrefEntryCount;
	}
	return 0;
}

HE_VOID CHE_PDF_Parser::VerifyXRef()
{
	PDF_XREF_ENTRY_NODE * pTmpNode = NULL;
	CHE_PDF_PARSER_WORD_DES wordDes( GetAllocator() );
	HE_DWORD index = 0;
	while( TRUE )
	{
		if ( index > m_xrefTable.m_lMaxObjNum )
		{
			break;
		}
		pTmpNode = m_xrefTable.m_pFastAccessArr[index];
 		if ( pTmpNode )
 		{
//  			if ( pTmpNode->entry.GetType() == OBJTYPE_COMMON )
//  			{
//  				m_sParser.SetPos( pTmpNode->entry.GetOffset() );
//  				m_sParser.GetWord( wordDes );
//  				if ( wordDes.type != PDFPARSER_WORD_INTEGER ||
//  					( wordDes.type == PDFPARSER_WORD_INTEGER && 
//  					HE_PDF_StringToInteger( wordDes.str ) != (HE_INT32)index ) )
//  				{
//  					if ( pTmpNode->pPrv )
//  					{
//  						pTmpNode->pPrv->pNext = pTmpNode->pNext;
//  						m_xrefTable.m_lMaxObjNum--;
//  						m_xrefTable.m_lCount--;
//  					}
//  					delete pTmpNode;
//  					m_xrefTable.BuildIndex();
// 					continue;
//  				}
//  			}else
			if ( pTmpNode->entry.GetType() == OBJTYPE_COMPRESSED )
			{
				CHE_PDF_IndirectObject * pInObj = GetIndirectObject( pTmpNode->entry.GetParentObjNum() );
				if (	pInObj == NULL || pInObj->GetDict() == NULL ||
						pInObj->GetDict()->GetElement( "Type" ) == NULL ||
						((CHE_PDF_Name*)pInObj->GetDict()->GetElement( "Type" ))->GetString() != "ObjStm" )
				{
					if ( pTmpNode->pPrv )
					{
						pTmpNode->pPrv->pNext = pTmpNode->pNext;
						//m_xrefTable.m_lMaxObjNum;
						m_xrefTable.m_lCount--;
					}
					GetAllocator()->Delete( pTmpNode );
					m_xrefTable.BuildIndex();
					continue;
				}
			}
		}
		index++;
	}
}

HE_VOID	CHE_PDF_Parser::VerifyObjInStm()
{
	if ( m_arrObjStm.GetCount() > 0 )
	{
		HE_DWORD objNum = 0;
		HE_DWORD n = 0;
		for ( HE_DWORD i = 0; i < m_arrObjStm.GetCount(); i++ )
		{
			objNum = (HE_DWORD)m_arrObjStm.GetItem( i );
			if ( objNum == 0 )
			{
				continue;
			}else{
				CHE_PDF_IndirectObject * pInObj = GetIndirectObject( objNum );
				if ( pInObj == NULL )
				{
					continue;
				}else{
					CHE_PDF_Dictionary * pDict = pInObj->GetDict();
					if ( pDict == NULL )
					{
						continue;
					}else{
						CHE_PDF_Object * pTmpObj = pDict->GetElement( "N" );
						if ( pTmpObj == NULL )
						{
							continue;
						}else{
							n = ((CHE_PDF_Number*)pTmpObj)->GetInteger();
						}
						
						CHE_PDF_Name * pDictName = (CHE_PDF_Name *)pDict->GetElement( "Type" );
						if ( pDictName == NULL )
						{
							continue;
						}else{
							if ( pDictName->GetString() != "ObjStm" )
							{
								continue;
							}
						}
					}
					CHE_PDF_Stream * pStream = pInObj->GetStream();
					if ( pStream == NULL )
					{
						continue;
					}
					CHE_PDF_StreamAcc stmAcc( GetAllocator() );
					if ( stmAcc.Attach( pStream ) == FALSE )
					{
						continue;
					}
					CHE_PDF_SyntaxParser sParser( GetAllocator() );
					CHE_PDF_PARSER_WORD_DES wordDes( GetAllocator() );
					IHE_Read * pIHE_Read = HE_CreateMemBufRead( stmAcc.GetData(), stmAcc.GetSize(), GetAllocator() );
					if ( pIHE_Read == NULL )
					{
						continue;
					}
					sParser.InitParser( pIHE_Read );
					for ( HE_DWORD i = 0; i < n; i++ )
					{

						if ( sParser.GetWord( wordDes ) == FALSE )
						{
							break;
						}
						if ( wordDes.type == PDFPARSER_WORD_INTEGER )
						{
							HE_DWORD iObjNum = HE_PDF_StringToInteger( wordDes.str );
							m_xrefTable.m_pFastAccessArr[iObjNum]->entry.field2 = objNum;
							m_xrefTable.m_pFastAccessArr[iObjNum]->entry.field3 = i;
						}
						if ( sParser.GetWord( wordDes ) == FALSE )
						{
							break;
						}
					}
					pIHE_Read->Release();
					HE_DestoryIHERead( pIHE_Read );
					stmAcc.Detach();
				}
			}
		}
	}
}

CHE_PDF_Dictionary* CHE_PDF_Parser::GetRootDict()
{
	if ( m_pTrailerDict )
	{
		CHE_PDF_Object * pObj = m_pTrailerDict->GetElement( "Root" );
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
		CHE_PDF_Object * pObj = m_pTrailerDict->GetElement( "Info" );
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
				CHE_PDF_Dictionary * pDict = pIndirectObj->GetDict();
				return pDict;
			}else{
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
		CHE_PDF_Array * pRootObj = (CHE_PDF_Array*)m_pTrailerDict->GetElement( "ID" );
		return pRootObj;
	}
	return NULL;
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
	if ( pPagesRef == NULL || pPagesRef->GetType() != PDFOBJ_REFERENCE )
	{
		return 0;
	}
	CHE_PDF_IndirectObject * pInObj = GetIndirectObject( ((CHE_PDF_Reference*)pPagesRef)->GetRefNuml() );
	if ( pInObj == NULL )
	{
		return 0;
	}
	CHE_PDF_Dictionary * pDictInObj = NULL;

	pDictInObj = pInObj->GetDict();
	if ( pDictInObj == NULL )
	{
		return 0;
	}

	CHE_PDF_Object * pObj =  pDictInObj->GetElement( "Count" );
	if ( pObj == NULL || pObj->GetType() != PDFOBJ_NUMBER )
	{
		return 0;
	}
	m_lPageCount = ((CHE_PDF_Number*)pObj)->GetInteger();

	return m_lPageCount;
}

HE_DWORD CHE_PDF_Parser::GetPageObjList( HE_DWORD* pList )
{
	if ( pList == NULL )
	{
		return 0;
	}
	CHE_PDF_Dictionary * pDict = GetRootDict();
	if ( pDict == NULL )
	{
		return 0;
	}
	CHE_PDF_Object * pPagesRef = pDict->GetElement( "Pages" );
	if ( pPagesRef == NULL || pPagesRef->GetType() != PDFOBJ_REFERENCE )
	{
		return 0;
	}
	CHE_PDF_IndirectObject * pInObj = GetIndirectObject( ((CHE_PDF_Reference*)pPagesRef)->GetRefNuml() );
	if ( pInObj == NULL )
	{
		return 0;
	}

	HE_DWORD lPageCount = 0;
	CHE_PtrQueue queue( GetAllocator() );
	CHE_PDF_Dictionary * pDictInObj = NULL;
	queue.Push( pInObj );
	while ( queue.IsEmpty() == FALSE )
	{
		if ( queue.Pop( (HE_LPVOID*)&pInObj ) == FALSE || pInObj == NULL )
		{
			return 0;
		}
		pDictInObj = pInObj->GetDict();
		if ( pDictInObj == NULL )
		{
			return 0;
		}

		CHE_PDF_Object * pObj =  pDictInObj->GetElement( "Count" );
		if ( pObj->GetType() == PDFOBJ_REFERENCE )
		{
			CHE_PDF_IndirectObject * pInObj = NULL;
			pInObj = GetIndirectObject( ((CHE_PDF_Reference*)pObj)->GetRefNuml() );
			if ( pInObj == NULL )
			{
				return 0;
			}
			pObj = pInObj->GetObject();
		}else if ( pObj == NULL || pObj->GetType() != PDFOBJ_NUMBER )
		{
			return 0;
		}

		pObj = pDictInObj->GetElement( "Kids" );
		if ( pObj->GetType() == PDFOBJ_REFERENCE )
		{
			CHE_PDF_IndirectObject * pInObj = NULL;
			pInObj = GetIndirectObject( ((CHE_PDF_Reference*)pObj)->GetRefNuml() );
			if ( pInObj == NULL )
			{
				return 0;
			}
			pObj = pInObj->GetObject();
		}else if ( pObj == NULL || pObj->GetType() != PDFOBJ_ARRAY )
		{
			return 0;
		}

		CHE_PDF_Object * pArrayElement = NULL;
		for ( HE_DWORD i = 0; i < ((CHE_PDF_Array*)pObj)->GetCount(); i++ )
		{
			pArrayElement = ((CHE_PDF_Array*)pObj)->GetElement( i );
			if ( pArrayElement == NULL || pArrayElement->GetType() != PDFOBJ_REFERENCE )
			{
				continue;
			}
			pInObj = GetIndirectObject( ((CHE_PDF_Reference*)pArrayElement)->GetRefNuml() );
			if ( pInObj == NULL )
			{
				continue;
			}
			pDictInObj = pInObj->GetDict();
			if ( pDictInObj == NULL )
			{
				continue;
			}
			CHE_PDF_Object * pType = pDictInObj->GetElement( "Type" );
			if ( pType == NULL || pType->GetType() != PDFOBJ_NAME )
			{
				continue;
			}
			CHE_ByteString str = ((CHE_PDF_Name*)pType)->GetString();
			if ( str == "Pages" )
			{
				queue.Push( pInObj );
			}else if ( str == "Page" )
			{
				pList[lPageCount] = ((CHE_PDF_Reference*)pArrayElement)->GetRefNuml();
				lPageCount++;
			}
		}
	}
	return lPageCount;
}

CHE_PDF_IndirectObject * CHE_PDF_Parser::GetIndirectObject()
{
	if ( m_pIHE_FileRead == NULL )
	{
		return NULL;
	}

	HE_DWORD offset = m_sParser.GetPos();
	CHE_PDF_PARSER_WORD_DES wordDes( GetAllocator() );
	HE_DWORD objNum = 0, genNum = 0;
	m_sParser.GetWord( wordDes );
	if ( wordDes.type == PDFPARSER_WORD_INTEGER )
	{
		objNum = HE_PDF_StringToInteger( wordDes.str );
	}else{
		return NULL;
	}
	m_sParser.GetWord( wordDes );
	if ( wordDes.type == PDFPARSER_WORD_INTEGER )
	{
		genNum = HE_PDF_StringToInteger( wordDes.str );
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
	if ( wordDes.type == PDFPARSER_WORD_DICT_B )
	{
		m_sParser.SetPos( pos );
		pCurObj = m_sParser.GetDictionary();
		if ( pCurObj == NULL )
		{
			return NULL;
		}
		m_sParser.GetWord( wordDes );
		if ( wordDes.str == "stream" )
		{
			HE_DWORD length = 0;
			CHE_PDF_Object * pObj = ((CHE_PDF_Dictionary*)pCurObj)->GetElement( "Length" );
			if ( pObj == NULL )
			{
				length = 0;
			}else if ( pObj->GetType() == PDFOBJ_NUMBER || pObj->GetType() == PDFOBJ_REFERENCE )
			{
				if ( pObj->GetType() == PDFOBJ_NUMBER )
				{
					length = ((CHE_PDF_Number*)pObj)->GetInteger();
				}else{
					HE_DWORD objNum = ((CHE_PDF_Reference*)pObj)->GetRefNuml();
					HE_DWORD offset = m_sParser.GetPos();
					CHE_PDF_XREF_Entry entry;
					m_xrefTable.GetEntry( objNum, entry );
					m_sParser.SetPos( entry.GetOffset() );
					CHE_PDF_IndirectObject * pObj = GetIndirectObject();
					if ( pObj && pObj->GetObject() )
					{
						if ( pObj->GetObject()->GetType() == PDFOBJ_NUMBER )
						{
							length = ((CHE_PDF_Number*)pObj->GetObject())->GetInteger();
						}
					}
					m_sParser.SetPos( offset );
				}
				m_sParser.SeekToNextLine();
				offset = m_sParser.GetPos();
				if ( length == 0 )
				{
					m_sParser.SeekToEndStream();
					length = m_sParser.GetPos() - offset;
				}else{	
					m_sParser.Seek( length );
				}
				m_sParser.GetWord( wordDes );
				if ( wordDes.str == "endstream" )
				{
					CHE_PDF_Stream * pStream = CHE_PDF_Stream::Create( m_pIHE_FileRead, offset, length, (CHE_PDF_Dictionary*)pCurObj, objNum, genNum, m_pStmEncrypt );
					if ( pStream == NULL )
					{
						pCurObj->Release();
						return NULL;
					}
					CHE_PDF_IndirectObject * pObj = CHE_PDF_IndirectObject::Create( objNum, genNum, pStream, GetAllocator() );
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
		}else if ( wordDes.str == "endobj" )
		{
			CHE_PDF_IndirectObject * pObj = CHE_PDF_IndirectObject::Create( objNum, genNum, pCurObj, GetAllocator() );
			if ( pObj )
			{
				m_objCollector.Add( pObj );
				return pObj;
			}else{
				pCurObj->Release();
				return NULL;
			}
		}else if (	wordDes.str[0] == 'e' && wordDes.str[1] == 'n' && wordDes.str[2] == 'd'
				&&  wordDes.str[3] == 'o' && wordDes.str[4] == 'b' && wordDes.str[5] == 'j' )
		{
			CHE_PDF_IndirectObject * pObj = CHE_PDF_IndirectObject::Create( objNum, genNum, pCurObj, GetAllocator() );
			if ( pObj )
			{
				m_objCollector.Add( pObj );
				return pObj;
			}else{
				pCurObj->Release();
				return NULL;
			}
		}else{
			CHE_PDF_IndirectObject * pObj = CHE_PDF_IndirectObject::Create( objNum, genNum, pCurObj, GetAllocator() );
			if ( pObj )
			{
				m_objCollector.Add( pObj );
				return pObj;
			}else{
				pCurObj->Release();
				return NULL;
			}
		}
	}else if ( wordDes.type == PDFPARSER_WORD_FLOAT )
	{
		pCurObj = CHE_PDF_Number::Create( HE_PDF_StringToFloat( wordDes.str ), objNum, genNum, GetAllocator() );
	}else if ( wordDes.type == PDFPARSER_WORD_INTEGER )
	{
		HE_DWORD refNum = HE_PDF_StringToInteger( wordDes.str );
		HE_DWORD pos = m_sParser.GetPos();
		m_sParser.GetWord( wordDes );
		if ( wordDes.type == PDFPARSER_WORD_INTEGER )
		{
			m_sParser.GetWord( wordDes );
			if ( wordDes.str == "R" )
			{
				pCurObj = CHE_PDF_Reference::Create( refNum, objNum, genNum, GetAllocator() );
			}else{
				pCurObj = CHE_PDF_Number::Create( (HE_INT32)refNum, objNum, genNum, GetAllocator() );
			}
		}else{
			pCurObj = CHE_PDF_Number::Create( (HE_INT32)refNum, objNum, genNum, GetAllocator() );
		}
		m_sParser.SetPos( pos );
	}else if ( wordDes.type == PDFPARSER_WORD_STRING )
	{
		pCurObj = CHE_PDF_String::Create( wordDes.str, objNum, genNum, GetAllocator() );
	}else if ( wordDes.type == PDFPARSER_WORD_NAME )
	{
		pCurObj = CHE_PDF_Name::Create( wordDes.str, objNum, genNum, GetAllocator() );
	}else if ( wordDes.type == PDFPARSER_WORD_ARRAY_B )
	{
		m_sParser.SetPos( pos );
		pCurObj = m_sParser.GetArray();
		if ( pCurObj == NULL )
		{
		}
	}else if ( wordDes.str == "false" )
	{
		pCurObj = CHE_PDF_Boolean::Create( FALSE, objNum, genNum, GetAllocator() );
	}else if ( wordDes.str == "true" )
	{
		pCurObj = CHE_PDF_Boolean::Create( TRUE, objNum, genNum, GetAllocator() );
	}else if ( wordDes.str == "null" || wordDes.str == "endobj" )
	{
		pCurObj = CHE_PDF_Null::Create( objNum, genNum, GetAllocator() );
	}else{
		return NULL;
	}
	CHE_PDF_IndirectObject * pObj = CHE_PDF_IndirectObject::Create( objNum, genNum, pCurObj, GetAllocator() );
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
	if ( m_objCollector.IsExist( objNum ) )
	{
		return m_objCollector.GetObj( objNum );
	}
	
	CHE_PDF_XREF_Entry entry;
	if ( m_xrefTable.GetEntry( objNum, entry ) )
	{
		if ( entry.GetType() == OBJTYPE_COMMON )
		{
			m_sParser.SetPos( entry.GetOffset() );
			return GetIndirectObject();
		}else{
			return GetIndirectObjectInObjStm( entry.GetParentObjNum(), entry.GetObjNum(), entry.GetIndex() );
		}
	}else{
		return NULL;
	}
}

CHE_PDF_IndirectObject * CHE_PDF_Parser::GetIndirectObjectInObjStm( HE_DWORD stmObjNum, HE_DWORD objNum, HE_DWORD index )
{
	if ( m_pIHE_FileRead == NULL )
	{
		return NULL;
	}
	if ( m_objCollector.IsExist( objNum ) )
	{
		return m_objCollector.GetObj( objNum );
	}
	CHE_PDF_XREF_Entry entry;
	if ( m_xrefTable.GetEntry( stmObjNum, entry ) )
	{
		CHE_PDF_IndirectObject * pInObj = NULL;
		if ( entry.GetType() == OBJTYPE_COMMON )
		{
			m_sParser.SetPos( entry.GetOffset() ); 
			pInObj = GetIndirectObject();
		}else{
			return NULL;
			//pInObj = GetIndirectObjectInObjStm( entry.GetParentObjNum(), entry.GetObjNum(), entry.GetIndex() );
		}
		if ( pInObj == NULL )
		{
			return NULL;
		}
		CHE_PDF_Object * pObj = NULL;
		if ( pObj = (CHE_PDF_Object *)pInObj->GetObject() )
		{
			if ( pObj->GetType() != PDFOBJ_STREAM )
			{
				return NULL;
			}else{
				CHE_PDF_StreamAcc stmAcc( GetAllocator() );
				stmAcc.Attach( (CHE_PDF_Stream*)pObj );
				HE_LPCBYTE pData = stmAcc.GetData();
				HE_DWORD lDataSize = stmAcc.GetSize();
// 				FILE * pFile  = fopen( "c:\\33.txt", "wb+" );
// 				fwrite( pData, 1, lDataSize, pFile );
// 				fclose( pFile );
				IHE_Read * pIHE_Read = HE_CreateMemBufRead( (HE_LPBYTE)pData, lDataSize, GetAllocator() );
				if ( pIHE_Read == NULL )
				{
					stmAcc.Detach();
					return NULL;
				}
				CHE_PDF_SyntaxParser sParser( GetAllocator() );
				sParser.InitParser( pIHE_Read );
				sParser.SetCurObjNum( stmObjNum );
				sParser.SetCurGenNum( 0 );
				
				HE_DWORD first = 0;
				CHE_PDF_PARSER_WORD_DES	wordDes( GetAllocator() );
				HE_DWORD			offset = 0;
				HE_DWORD			objNumOffset = 0;
				HE_DWORD			objNumTmp = 0;
				sParser.SetPos( 0 );

				first = ((CHE_PDF_Number*)(((CHE_PDF_Stream*)pObj)->GetDict()->GetElement( "First" )))->GetInteger();

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
					if ( wordDes.type == PDFPARSER_WORD_INTEGER )
					{
						offset = sParser.GetPos();

						objNumTmp = HE_PDF_StringToInteger( wordDes.str );

						sParser.GetWord( wordDes );
						if ( wordDes.type != PDFPARSER_WORD_INTEGER )
						{
							sParser.SetPos( offset );
							continue;
						}
						first += HE_PDF_StringToInteger( wordDes.str );
						if ( objNumTmp == objNum )
						{
							sParser.SetPos( first );
							HE_DWORD pos = sParser.GetPos();
							sParser.GetWord( wordDes );
							CHE_PDF_Object * pCurObj = NULL;
							if ( wordDes.type == PDFPARSER_WORD_DICT_B )
							{
								sParser.SetPos( pos );
								pCurObj = sParser.GetDictionary();
							}else if ( wordDes.type == PDFPARSER_WORD_FLOAT )
							{
								pCurObj = CHE_PDF_Number::Create( HE_PDF_StringToFloat( wordDes.str ), objNum, 0, GetAllocator() );
							}else if ( wordDes.type == PDFPARSER_WORD_INTEGER )
							{
								HE_DWORD refNum = HE_PDF_StringToInteger( wordDes.str );
								HE_DWORD pos = sParser.GetPos();
								sParser.GetWord( wordDes );
								if ( wordDes.type == PDFPARSER_WORD_INTEGER )
								{
									sParser.GetWord( wordDes );
									if ( wordDes.str == "R" )
									{
										pCurObj = CHE_PDF_Reference::Create( refNum, objNum, 0, GetAllocator() );
									}else{
										pCurObj = CHE_PDF_Number::Create( (HE_INT32)refNum, objNum, 0, GetAllocator() );
									}
								}else{
									pCurObj = CHE_PDF_Number::Create( (HE_INT32)refNum, objNum, 0, GetAllocator() );
								}
								sParser.SetPos( pos );
							}else if ( wordDes.type == PDFPARSER_WORD_STRING )
							{
								pCurObj = CHE_PDF_String::Create( wordDes.str, objNum, 0, GetAllocator() );
							}else if ( wordDes.type == PDFPARSER_WORD_NAME )
							{
								pCurObj = CHE_PDF_Name::Create( wordDes.str, objNum, 0, GetAllocator() );
							}else if ( wordDes.type == PDFPARSER_WORD_ARRAY_B )
							{
								sParser.SetPos( pos );
								pCurObj = sParser.GetArray();
							}else if ( wordDes.str == "false" )
							{
								pCurObj = CHE_PDF_Boolean::Create( FALSE, objNum, 0, GetAllocator() );
							}else if ( wordDes.str == "true" )
							{
								pCurObj = CHE_PDF_Boolean::Create( TRUE, objNum, 0, GetAllocator() );
							}else if ( wordDes.str == "null" )
							{
								pCurObj = CHE_PDF_Null::Create( objNum, 0, GetAllocator() );
							}else{
								pCurObj = NULL;
							}

							if ( pCurObj != NULL )
							{
								CHE_PDF_IndirectObject * pObj = CHE_PDF_IndirectObject::Create( objNum, 0, pCurObj, GetAllocator() );
								if ( pObj )
								{
									m_objCollector.Add( pObj );
									return pObj;
								}else{
									pCurObj->Release();
									return NULL;
								}
							}
						}else{
							return NULL;
						}
					}
				}
			}
		}
	}
	return NULL;
}
