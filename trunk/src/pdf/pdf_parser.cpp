#include "../../include/pdf/pdf_parser.h"
#include "../../include/pdf/pdf_encrypt.h"

#include <unordered_set>
#include <cstdlib>

namespace chelib {

CPDF_SyntaxParser::CPDF_SyntaxParser( CPDF_File * pFile, CAllocator * pAllocator )
	: CObject( pAllocator ), mpFile( pFile ), mpIFileRead( nullptr ), mpStrEncrypt( nullptr ), mlFilePos( 0 ),
	mlFileSize( 0 ), mlBufferSize( 0 ), mlBufferPos( 0 ), mlSubmitSize( 0 ), mCurObjNum( 0 ), mCurGenNum( 0 ), 
	mbBegin( TRUE ), mbPoint( false ), mbSign( false )
{
	memset( mWordBuffer, 0, 255 );
}

CPDF_SyntaxParser::~CPDF_SyntaxParser()
{
}

bool CPDF_SyntaxParser::InitParser( IRead* pFileAccess )
{
	if ( pFileAccess )
	{
		mpIFileRead = pFileAccess;
		mlFileSize = mpIFileRead->GetSize();
		mlFilePos = 0;
		return TRUE;
	}else{
		return false;
	}
}

size_t CPDF_SyntaxParser::Seek( size_t bytes )
{
	if ( mpIFileRead )
	{
		if ( mlFilePos + bytes <= mlFileSize )
		{
			mlFilePos+= bytes;
			return bytes;
		}else{
			bytes = mlFileSize - mlFilePos;
			mlFilePos = mlFileSize;
			return bytes;
		}
	}else{
		return 0;
	}
}

void CPDF_SyntaxParser::SeekToPrevLine()
{
	if ( mpIFileRead )
	{
		while( mlFilePos > 0 )
		{
			BYTE byte = mpIFileRead->ReadByte( mlFilePos );
			switch( byte )
			{
			case 0x0A:
				{
					byte = mpIFileRead->ReadByte( mlFilePos-1 );
					if ( byte == 0x0D )
					{
						mlFilePos--;
					}
				}
			case 0x0D:
				mlFilePos--;
				return;
			}
			mlFilePos--;
		}
	}
}

void CPDF_SyntaxParser::SeekToNextLine()
{
	if ( mpIFileRead )
	{
		while( mlFilePos < mlFileSize )
		{
			BYTE byte = mpIFileRead->ReadByte( mlFilePos );
			switch( byte )
			{
			case 0x0D:
				byte = mpIFileRead->ReadByte( mlFilePos+1 );
				if ( byte == 0x0A )
				{
					mlFilePos++;
				}
			case 0x0A:
				mlFilePos++;
				return;
			default:
				 break;
			}
			mlFilePos++;
		}
	}	
}

void CPDF_SyntaxParser::SeekToPrevWord()
{
	if ( mpIFileRead )
	{
		bool bWordFound = false;
		while( mlFilePos > 0 )
		{
			BYTE byte = mpIFileRead->ReadByte( mlFilePos );
			switch( byte )
			{
			case 0x0A:
				{
					byte = mpIFileRead->ReadByte( mlFilePos-1 );
					if ( byte == 0x0D )
					{
						mlFilePos--;
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
			mlFilePos--;
		}
	}
}

void CPDF_SyntaxParser::SeekToNextWord()
{
	if ( mpIFileRead )
	{
		bool bNewBegin = false;
		while( mlFilePos < mlFileSize )
		{
			BYTE byte = mpIFileRead->ReadByte( mlFilePos );
			switch( byte )
			{
			case 0x0D:
				{
					byte = mpIFileRead->ReadByte( mlFilePos+1 );
					if ( byte == 0x0A )
					{
						mlFilePos++;
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
			mlFilePos++;
		}
	}
}

void	CPDF_SyntaxParser::SeekToEndStream()
{
	char tmpStr[8];
	if ( mpIFileRead )
	{
		while( mlFilePos < mlFileSize )
		{
			BYTE byte = mpIFileRead->ReadByte( mlFilePos++ );
			switch( byte )
			{
			case 'e':
				{
					if ( mlFilePos + 8 < mlFileSize )
					{
						mpIFileRead->ReadBlock( tmpStr, mlFilePos, 8 );
						if (	tmpStr[0] == 'n' && tmpStr[1] == 'd' && tmpStr[2] == 's' 
							&&	tmpStr[3] == 't' && tmpStr[4] == 'r' && tmpStr[5] == 'e'
							&&	tmpStr[6] == 'a' && tmpStr[7] == 'm' )
						{
							mlFilePos--;
							mlFilePos--;
							byte = mpIFileRead->ReadByte( mlFilePos );
							if ( byte == '\n' )
							{
								mlFilePos--;
								byte = mpIFileRead->ReadByte( mlFilePos );
								if ( byte == '\r' )
								{
									mlFilePos--;
								}
							}else if ( byte == '\r' )
							{
								mlFilePos--;
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

void CPDF_SyntaxParser::SeekToEndobj()
{
	char tmpStr[5];
	if ( mpIFileRead )
	{
		while( mlFilePos < mlFileSize )
		{
			BYTE byte = mpIFileRead->ReadByte( mlFilePos++ );
			switch( byte )
			{
			case 'e':
				{
					if ( mlFilePos + 4 < mlFileSize )
					{
						mpIFileRead->ReadBlock( tmpStr, mlFilePos, 5 );
						if (	tmpStr[0] == 'n' && tmpStr[1] == 'd' && tmpStr[2] == 'o' 
							&&	tmpStr[3] == 'b' && tmpStr[4] == 'j' )
						{
							mlFilePos--;
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

void CPDF_SyntaxParser::SeekToMark( CByteString markStr )
{
	if ( markStr.GetLength() == 0 )
	{
		return;
	}else if ( markStr.GetLength() == 1 )
	{
		if ( mpIFileRead )
		{
			BYTE byteTmp = markStr[0], byte = 0;
			while( mlFilePos < mlFileSize )
			{
				byte = mpIFileRead->ReadByte( mlFilePos++ );
				if ( byte == byteTmp )
				{
					mlFilePos--;
					return;
				}
			}
		}else{
			return;
		}
	}else{
		char * pTmpStr = GetAllocator()->NewArray<char>( markStr.GetLength()+1 );
		pTmpStr[markStr.GetLength()] = 0;
		if ( mpIFileRead )
		{
			BYTE byteTmp = markStr[0], byte = 0;
			while( mlFilePos < mlFileSize )
			{
				byte = mpIFileRead->ReadByte( mlFilePos++ );
				if ( byte == byteTmp )
				{
					if ( mlFilePos + markStr.GetLength() - 1 < mlFileSize )
					{
						mpIFileRead->ReadBlock( pTmpStr, mlFilePos-1, markStr.GetLength() );
						pTmpStr[markStr.GetLength()] = 0;
						if ( markStr == CByteString(pTmpStr) )
						{
							mlFilePos--;
							GetAllocator()->DeleteArray<char>( pTmpStr );
							return;
						}
					}
				}
			}
		}
		GetAllocator()->DeleteArray<char>( pTmpStr );
	}
}

size_t CPDF_SyntaxParser::ReadBytes( PBYTE pBuffer, size_t length )
{
	return mpIFileRead->ReadBlock( pBuffer, mlFilePos, length );
}

bool CPDF_SyntaxParser::GetWord( CPDF_ParseWordDes & des )
{
	if ( mpIFileRead )
	{
		des.offset = 0;
		des.type = PARSE_WORD_UNKNOWN;

		BYTE byte = 0;
		while ( mlFilePos < mlFileSize )
		{
			byte = mpIFileRead->ReadByte( mlFilePos );
			switch ( byte )
			{
			case '%':	//注释，直接移动到下一行
				{
					SeekToNextLine();
					continue;
				}
			case '<':	//可能是字典，也可能是十六进制表示的字符串，需要更多判断
				{
					if ( mlBufferPos > 0 )
					{
						//返回已经获取的词
						SubmitBufferStr();
						return TRUE;
					}else{
						byte = mpIFileRead->ReadByte( mlFilePos+1 );
						if ( byte == '<' )
						{
							//返回表明字典开始的词
							des.offset = mlFilePos;
							des.type = PARSE_WORD_DICT_B;
							mlFilePos+=2;
							return TRUE;
						}else{
							//字符串处理
							des.offset = mlFilePos;
							des.type = PARSE_WORD_STRING;
							mlFilePos++;
							size_t tmpValue = 0;
							BYTE	tmpCount = 0;
							while ( mlFilePos < mlFileSize )
							{
								byte = mpIFileRead->ReadByte( mlFilePos++ );
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
									mWordBuffer[mlBufferPos++] = (BYTE)tmpValue;
									tmpCount = 0;
									tmpValue = 0;
								}
								if ( byte == '>' )
								{
									if ( tmpCount == 1 )
									{
										tmpValue = tmpValue * 16;
										mWordBuffer[mlBufferPos++] = (BYTE)tmpValue;
									}
									break;
								}
							}
							SubmitBufferStr();
							if ( mpStrEncrypt && mpStrEncrypt->IsPasswordOK() == TRUE )
							{
								mpStrEncrypt->Decrypt( mWordBuffer, mlSubmitSize, mCurObjNum, mCurGenNum );
							}
							return TRUE;
						}
					}
					break;
				}
			case '>':	//字典结束，因为字符串的已经在其他位置处理了，这里不可能是字符串了
				{
					if ( mlBufferPos > 0 )
					{
						SubmitBufferStr();
						return TRUE;
					}else{
						byte = mpIFileRead->ReadByte( mlFilePos+1 );
						if ( byte == '>' )
						{
							des.offset = mlFilePos;
							des.type = PARSE_WORD_DICT_E;
							mlFilePos += 2;
							return TRUE;
						}
					}
					break;
				}
			case '[':
				{
					if ( mlBufferPos > 0 )
					{
						SubmitBufferStr();
						return TRUE;
					}else{
						des.offset = mlFilePos;
						des.type = PARSE_WORD_ARRAY_B;
						mlFilePos++;
						return TRUE;
					}
					break;
				}
			case ']':
				{
					if ( mlBufferPos > 0 )
					{
						SubmitBufferStr();
						return TRUE;
					}else{
						des.offset = mlFilePos;
						des.type = PARSE_WORD_ARRAY_E;
						mlFilePos++;
						return TRUE;
					}
					break;
				}
			case '{':
				{
					if ( mlBufferPos > 0 )
					{
						SubmitBufferStr();
						return TRUE;
					}else{
						des.offset = mlFilePos;
						des.type = PARSE_WORD_BLOCK_B;
						mlFilePos++;
						return TRUE;
					}
					break;
				}
			case '}':
				{
					if (mlBufferPos > 0)
					{
						SubmitBufferStr();
						return TRUE;
					}else{
						des.offset = mlFilePos;
						des.type = PARSE_WORD_BLOCK_E;
						mlFilePos++;
						return TRUE;
					}
					break;
				}
			case '/':
				{
					if ( mlBufferPos > 0 )
					{
						SubmitBufferStr();
						return TRUE;
					}else{
						des.offset = mlFilePos;
						des.type = PARSE_WORD_NAME;
						mlFilePos++;
						while ( mlFilePos < mlFileSize )
						{
							byte = mpIFileRead->ReadByte( mlFilePos );
							if ( byte == '#' )
							{
								size_t tmpValue = 0;
								byte = mpIFileRead->ReadByte( ++mlFilePos );
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
								byte = mpIFileRead->ReadByte( ++mlFilePos );
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
								mWordBuffer[mlBufferPos++] = (BYTE)tmpValue;
								mlFilePos++;
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
								mWordBuffer[mlBufferPos++] = byte;
								mlFilePos++;
								continue;
							}
							break;
						}
						SubmitBufferStr();
						return TRUE;
					}
					break;
				}
			case '(':	//字符串
				{
					if ( mlBufferPos > 0 )
					{
						SubmitBufferStr();
						return TRUE;
					}else{
						des.type = PARSE_WORD_STRING;
						des.offset = mlFilePos;
						size_t lCount = 1;
						mlFilePos++;
						while ( mlFilePos < mlFileSize )
						{
							byte = mpIFileRead->ReadByte( mlFilePos );
							if ( byte == '\\' )
							{
								byte = mpIFileRead->ReadByte( ++mlFilePos );
								switch ( byte )
								{
								case 0x0D:
									{
										byte = mpIFileRead->ReadByte( mlFilePos+1 );
										if ( byte == 0x0A )
										{
											mlFilePos+=2;
                                        }else{
                                            mlFilePos++;
                                        }
                                        continue;
									}
								case 0x0A:
                                    mlFilePos++;
									continue;
								case 'n':
									mWordBuffer[mlBufferPos++] = 0x0A;
									mlFilePos++;
									continue;
								case 'r':
									mWordBuffer[mlBufferPos++] = 0x0D;
									mlFilePos++;
									continue;
								case 't':
									mWordBuffer[mlBufferPos++] = 0x09;
									mlFilePos++;
									continue;
								case 'b':
									mWordBuffer[mlBufferPos++] = 0x20;
									mlFilePos++;
									continue;
								case 'f':
									mWordBuffer[mlBufferPos++] = 0x0C;
									mlFilePos++;
									continue;
								case '(':
									mWordBuffer[mlBufferPos++] = '(';
									mlFilePos++;
									continue;
								case ')':
									mWordBuffer[mlBufferPos++] = ')';
									mlFilePos++;
									continue;
								case '\\':
									mWordBuffer[mlBufferPos++] = '\\';
									mlFilePos++;
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
										size_t tmpValue = byte - '0';
										BYTE tmpByte = 0;
										BYTE tmpCount = 1;
										while ( true )
										{
											tmpByte = mpIFileRead->ReadByte( ++mlFilePos );
											if ( tmpByte >= '0' && tmpByte <= '7' )
											{
												tmpValue = tmpValue * 8 + (tmpByte-'0');
												tmpCount++;
												if ( tmpCount == 3 )
												{
													mWordBuffer[mlBufferPos++] = (BYTE)tmpValue;
													tmpCount = 0;
													++mlFilePos;
													break;
												}
											}else{
												break;
											}
										}
										if ( tmpCount != 0 )
										{
											mWordBuffer[mlBufferPos++] = (BYTE)tmpValue;
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
								mWordBuffer[mlBufferPos++] = byte;
								mlFilePos++;
							}
						}
						mlFilePos++;
						SubmitBufferStr();
						if ( mpStrEncrypt && mpStrEncrypt->IsPasswordOK() == TRUE )
						{
							mpStrEncrypt->Decrypt( mWordBuffer, mlSubmitSize, mCurObjNum, mCurGenNum );
						}
						return TRUE;
					}
					break;
				}
			case 0x0D:
				{
					byte = mpIFileRead->ReadByte( mlFilePos+1 );
					if ( byte == 0x0A )
					{
						mlFilePos++;
					}
				}
			case 0x00:
			case 0x09:
			case 0x0A:
			case 0x0C:
			case 0x20:
				{
					if ( mlBufferPos > 0 )
					{
						SubmitBufferStr( /*des.str*/ );
						return TRUE;
					}
					break;
				}
			case '+':
			case '-':
				{
					if ( mbBegin && mbSign == false )
					{
						des.type = PARSE_WORD_INTEGER;
					}else{
						des.type = PARSE_WORD_UNKNOWN;
					}
					if ( des.offset == 0 )
					{
						des.offset = mlFilePos;
					}
					mWordBuffer[mlBufferPos++] = byte;
					break;
				}
			case '.':
				{
					if ( mbPoint == false )
					{
						mbPoint = TRUE;
						des.type = PARSE_WORD_FLOAT;
					}else{
						des.type = PARSE_WORD_UNKNOWN;
					}
					if ( des.offset == 0 )
					{
						des.offset = mlFilePos;
					}
					if ( mbBegin )
					{
						mbBegin = false;
						mWordBuffer[mlBufferPos++] = '0';
					}
					mWordBuffer[mlBufferPos++] = byte;
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
					if ( mbBegin == TRUE )
					{
						mbBegin = false;
						des.type = PARSE_WORD_INTEGER;
					}
					if ( des.offset == 0 )
					{
						des.offset = mlFilePos;
					}
					mWordBuffer[mlBufferPos++] = byte;
					break;
				}
			default:
				{
					if ( mbBegin )
					{
						mbBegin = false;
					}
					if ( des.type != PARSE_WORD_UNKNOWN )
					{
						des.type = PARSE_WORD_UNKNOWN;
					}
					if ( des.offset == 0 )
					{
						des.offset = mlFilePos;
					}
					if ( mlBufferPos > 32770 )
					{
						SubmitBufferStr();
						return TRUE;
					}
					mWordBuffer[mlBufferPos++] = byte;
					break;
				}
			}
			mlFilePos++;
		}

		if ( mlBufferPos != 0 )
		{
			SubmitBufferStr();
			return TRUE;
		}else{
			return false;
		}
	}else{
		return false;
	}
}

CPDF_ArrayPtr CPDF_SyntaxParser::GetArrayPtr()
{
	if ( mpIFileRead == nullptr )
	{
		return CPDF_ArrayPtr();
	}

	CPDF_ParseWordDes wordDes;

	if ( ! GetWord( wordDes ) || wordDes.type != PARSE_WORD_ARRAY_B )
	{
		return CPDF_ArrayPtr();
	}

	CPDF_ArrayPtr arrayPtrRet = CPDF_Array::Create( GetAllocator() );

	size_t preOffset = 0;
	bool	bOver = false;
	
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
				int32 integer = 0;
				size_t objNum = 0, genNum = 0;
				size_t tmpOffset = GetPos();

				integer = GetString().GetInteger();
				objNum = (size_t)( integer );

				if ( GetWord( wordDes ) && wordDes.type == PARSE_WORD_INTEGER )
				{
					genNum = (size_t)( GetString().GetInteger() );

					if ( GetWord( wordDes ) && IsWord( "R" ) )
					{
						CPDF_ReferencePtr pTmp = CPDF_Reference::Create( objNum, genNum, GetFile(), GetAllocator() );

						arrayPtrRet->Append( pTmp );

						break;
					}
				}

				SetPos( tmpOffset );

				CPDF_NumberPtr pTmp = CPDF_Number::Create( integer, GetAllocator() );

				arrayPtrRet->Append( pTmp );

				break;
			}
		case PARSE_WORD_FLOAT:
			{
				CPDF_NumberPtr pTmp = CPDF_Number::Create( GetString().GetFloat(), GetAllocator() );

				arrayPtrRet->Append( pTmp );

				break;
			}
		case PARSE_WORD_STRING:
			{
				CPDF_StringPtr pTmp = CPDF_String::Create( GetString(),  GetAllocator() );

				arrayPtrRet->Append( pTmp );

				break;
			}
		case PARSE_WORD_NAME:
			{
				CPDF_NamePtr pTmp = CPDF_Name::Create( GetString(), GetAllocator() );

				arrayPtrRet->Append( pTmp );

				break;
			}
		case PARSE_WORD_ARRAY_B:
			{
				SetPos( preOffset );

				CPDF_ArrayPtr tmpPtr = GetArrayPtr();

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

				CPDF_DictionaryPtr tmpPtr = GetDictionaryPtr();

				arrayPtrRet->Append( tmpPtr );

				break;
			}
		case  PARSE_WORD_UNKNOWN:
		default:
			{
				if ( IsWord( "false" ) )
				{
					CPDF_BooleanPtr tmpPtr = CPDF_Boolean::Create( false, GetAllocator() );
					
					arrayPtrRet->Append( tmpPtr );
				}
				else if ( IsWord( "true" ) )
				{
					CPDF_BooleanPtr tmpPtr = CPDF_Boolean::Create( TRUE, GetAllocator() );
				
					arrayPtrRet->Append( tmpPtr );	
				}
				else if ( IsWord( "null" ) )
				{
					CPDF_NullPtr tmpPtr = CPDF_Null::Create( GetAllocator() );

					arrayPtrRet->Append( tmpPtr );
				}
			}
		}
	}

	arrayPtrRet->SetModified( false );

	return arrayPtrRet;
}

CPDF_DictionaryPtr CPDF_SyntaxParser::GetDictionaryPtr()
{
	if ( mpIFileRead == nullptr )
	{
		return CPDF_DictionaryPtr();
	}

	CPDF_ParseWordDes wordDes;

	if ( ! GetWord( wordDes ) || wordDes.type != PARSE_WORD_DICT_B )
	{
		return CPDF_DictionaryPtr();
	}

	bool bKey = TRUE;
	bool bOver = false;
	size_t preOffset = 0;
	CByteString key( GetAllocator() );
	CPDF_DictionaryPtr dictRet = CPDF_Dictionary::Create( GetAllocator() );

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
				GetString( key );
				bKey = false;

				continue;
			}
		}

		switch ( wordDes.type )
		{
		case PARSE_WORD_INTEGER:
			{
				int32 integer = 0;
				size_t objNum = 0, genNum = 0;
				size_t tmpOffset = GetPos();

				integer = GetString().GetInteger();
				objNum = (size_t)( integer );
				
				if ( GetWord( wordDes ) && wordDes.type == PARSE_WORD_INTEGER )
				{
					genNum = GetString().GetInteger();

					if ( GetWord( wordDes ) && IsWord( "R" ) )
					{
						dictRet->SetReference( key, objNum, genNum, GetFile() );

						break;
					}
				}

				SetPos( tmpOffset );

				dictRet->SetInteger( key, integer );

				break;
			}
		case PARSE_WORD_FLOAT:
			{
				dictRet->SetFloatNumber( key, GetString().GetFloat() );

				break;
			}
		case PARSE_WORD_STRING:
			{
				dictRet->SetString( key, GetString() );

				break;
			}
		case PARSE_WORD_NAME:
			{
				dictRet->SetName( key, GetString() );

				break;
			}
		case PARSE_WORD_ARRAY_B:
			{
				SetPos( preOffset );

				CPDF_ArrayPtr tmpPtr = GetArrayPtr();

				dictRet->SetArray( key, tmpPtr );

				break;
			}
		case PARSE_WORD_DICT_B:
			{
				SetPos( preOffset );

				CPDF_DictionaryPtr tmpPtr = GetDictionaryPtr();

				dictRet->SetDictionary( key, tmpPtr );

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
				if ( IsWord( "false" ) )
				{
					dictRet->SetBoolean( key, false );
				}
				else if ( IsWord( "true" ) )
				{
					dictRet->SetBoolean( key, TRUE );
				}
				else if ( IsWord( "null" ) )
				{
					dictRet->SetNull( key );
				}
				break;
			}
		}
		
		bKey = TRUE;
	}

	dictRet->SetModified( false );

	return dictRet;
}

bool	CPDF_SyntaxParser::IsWord( const char * words )
{
	if ( strcmp( (const char*)&mWordBuffer[0], words ) == 0 )
	{
		return TRUE;
	}
	return false;
}

int32 CPDF_SyntaxParser::GetInteger()
{
	return atoi( (const char*)&mWordBuffer[0] );
}

FLOAT CPDF_SyntaxParser::GetFloat()
{
	return (FLOAT)atof( (const char*)&mWordBuffer[0] );
}

CByteString CPDF_SyntaxParser::GetString()
{
	return CByteString( (char const *)(mWordBuffer), mlSubmitSize );
}

void CPDF_SyntaxParser::GetString( CByteString & str )
{
	str.SetData( mWordBuffer, mlSubmitSize );
}

void CPDF_SyntaxParser::SubmitBufferStr( )
{
	mWordBuffer[mlBufferPos] = '\0';
	mlSubmitSize = mlBufferPos;
	mlBufferPos = 0;
	mbBegin = TRUE;
	mbPoint = false;
	mbSign = false;
}

CPDF_Parser * CPDF_Parser::Create( CPDF_File * pFile, IRead * pRead, CPDF_XRefTable * pXrefTable, CAllocator * pAllocator )
{
	if ( pFile == nullptr || pRead == nullptr || pXrefTable == nullptr )
	{
		return nullptr;
	}
	CPDF_Parser * pParserRet = nullptr;
	if ( pAllocator == nullptr )
	{
		pAllocator = GetDefaultAllocator();
	}
	pParserRet = pAllocator->New<CPDF_Parser>( pFile, pRead, pXrefTable, pAllocator );
	if ( pParserRet )
	{
		pParserRet->mSyntaxParser.InitParser( pParserRet->mpIFileRead );
	}
	return pParserRet;
}

CPDF_Parser::CPDF_Parser( CPDF_File * pFile, IRead * pRead, CPDF_XRefTable * pXrefTable, CAllocator * pAllocator )
	: CObject( pAllocator ), mpFile( pFile ), mpIFileRead( pRead ), mlStartxref(0), mpXRefTable( pXrefTable ),
	mSyntaxParser( pFile, pAllocator ), mpStrEncrypt(nullptr), mpStmEncrypt(nullptr), mpEefEncrypt(nullptr) { }

CPDF_Parser::~CPDF_Parser()
{
	mpXRefTable = nullptr;
	mlStartxref = 0;
	if ( mpStmEncrypt )
	{
		GetAllocator()->Delete( mpStmEncrypt );
		mpStmEncrypt = nullptr;
	}
	if ( mpIFileRead )
	{
		mpIFileRead = nullptr;
	}
}

size_t CPDF_Parser::GetFileSize() const
{
	if ( mpIFileRead )
	{
		return mpIFileRead->GetSize();
	}else{
		return 0;
	}
}

PDF_VERSION CPDF_Parser::GetPDFVersion() const
{
	size_t offset = 0;
	size_t fileSize = mpIFileRead->GetSize();
	bool bHeadFound = false;

	while( offset < 1024 && offset < fileSize )
	{
		if ( mpIFileRead->ReadByte( offset ) == '%' )
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
	if ( mpIFileRead->ReadBlock( buffer, offset, 8 ) )
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

bool CPDF_Parser::Authenticate( const CByteString & password ) 
{
	bool bRet = false;

	if ( mpStrEncrypt )
	{
		bRet = mpStmEncrypt->Authenticate( password );
		if ( mpStrEncrypt )
		{
			mSyntaxParser.SetEncrypt( mpStrEncrypt );
		}
	}
	return bRet;
}

size_t CPDF_Parser::GetStartxref( size_t range )
{
	if ( mpIFileRead )
	{
		unsigned char * pBuffer = GetAllocator()->NewArray<unsigned char>( range );
		if ( range >= mpIFileRead->GetSize() )
		{
			mpIFileRead->ReadBlock( pBuffer, 0, mpIFileRead->GetSize() );
		}else{
			mpIFileRead->ReadBlock( pBuffer, mpIFileRead->GetSize()-range, range );
		}
		size_t iStartXref = 0;
		for ( size_t m = range; m >= 8; m-- )
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
				size_t iBufferIndex = m+1;
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
			GetAllocator()->DeleteArray<BYTE>( pBuffer );
			pBuffer = nullptr;
		}
		mlStartxref = iStartXref;
		return iStartXref;
	}
	return 0;
}

size_t CPDF_Parser::ParseXRef()
{
	if ( mpIFileRead == nullptr )
	{
		return 0;
	}
	if ( mlStartxref == 0 )
	{
		GetStartxref( mSyntaxParser.GetFileSize() );
		if ( mlStartxref == 0 )
		{
			return FullParseForXRef();
		}
	}

	CPDF_DictionaryPtr pDict;
	CPDF_ArrayPtr pIdArray;
	CPDF_ObjectPtr pObj;
	size_t offset = mlStartxref;
	size_t xrefEntryCount = 0;
	size_t entryCount = 0;
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
			if ( mpStrEncrypt == nullptr )
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

bool CPDF_Parser::ParseEncrypt( const CPDF_DictionaryPtr & pEncryptDict, const CPDF_ArrayPtr & pIDArray )
{
	if ( pEncryptDict )
	{
		CPDF_ObjectPtr pObj;
		pObj = pEncryptDict->GetElement( "Filter" );
		if ( !pObj || pObj->GetType() != OBJ_TYPE_NAME )
		{
			return false;
		}
		if ( pObj->GetNamePtr()->GetString() == "Standard" )
		{
			CByteString id1( GetAllocator() );
			BYTE O[32];
			BYTE U[32];
			BYTE keyLength = 40;
			BYTE revision = 2;
			BYTE version = 0;
			bool bMetaData = TRUE;
			size_t pValue = 0;
			if ( pIDArray )
			{
				pObj = pIDArray->GetElement( 0 );
				if ( pObj && pObj->GetType() == OBJ_TYPE_STRING )
				{
					id1 = pObj->GetStringPtr()->GetString();
				}
			}
			//获取Length
			pObj = pEncryptDict->GetElement( "Length" );
			if ( pObj && pObj->GetType() == OBJ_TYPE_NUMBER )
			{
				keyLength = pObj->GetNumberPtr()->GetInteger();
			}
			//获取R
			pObj = pEncryptDict->GetElement( "R" );
			if ( pObj && pObj->GetType() == OBJ_TYPE_NUMBER )
			{
				revision = pObj->GetNumberPtr()->GetInteger();
			}
			//获取P
			pObj = pEncryptDict->GetElement( "P" );
			if ( pObj && pObj->GetType() == OBJ_TYPE_NUMBER )
			{
				pValue = pObj->GetNumberPtr()->GetInteger();
			}
			//获取EncryptMetadata
			pObj = pEncryptDict->GetElement( "EncryptMetadata" );
			if ( pObj && pObj->GetType() == OBJ_TYPE_BOOLEAN )
			{
				bMetaData = pObj->GetBooleanPtr()->GetValue();
			}
			//获取U
			pObj = pEncryptDict->GetElement( "U" );
			if ( pObj && pObj->GetType() == OBJ_TYPE_STRING )
			{
				CByteString str = pObj->GetStringPtr()->GetString();
				for ( size_t i = 0; i < str.GetLength(); i++ )
				{
					U[i] = str[i];
				}
			}
			//获取O
			pObj = pEncryptDict->GetElement( "O" );
			if ( pObj && pObj->GetType() == OBJ_TYPE_STRING )
			{
				CByteString str = pObj->GetStringPtr()->GetString();
				for ( size_t i = 0; i < str.GetLength(); i++ )
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
					//判断是否是AESV2算法
					pObj = pEncryptDict->GetElement( "CF" );
					if ( pObj  && pObj->GetType() == OBJ_TYPE_DICTIONARY )
					{
						pObj = pObj->GetDictPtr()->GetElement( "StdCF", OBJ_TYPE_DICTIONARY );
						if ( pObj && pObj->GetType() == OBJ_TYPE_DICTIONARY )
						{
							pObj = pObj->GetDictPtr()->GetElement( "CFM", OBJ_TYPE_NAME );
							if ( pObj && pObj->GetType() == OBJ_TYPE_NAME )
							{
								CByteString cfm = pObj->GetNamePtr()->GetString();
								if ( cfm == "AESV2" )
								{
									mpStrEncrypt = GetAllocator()->New<CPDF_Encrypt>( id1, O, U, PDFENCRYPT_ALGORITHM_AESV2, version, revision, keyLength, bMetaData, pValue, GetAllocator() );
									mpStmEncrypt = mpStrEncrypt;
									mpEefEncrypt = mpStrEncrypt;

									mSyntaxParser.SetEncrypt( mpStrEncrypt );

									mpStrEncrypt->Authenticate( "" );

									return TRUE;
								}
							}
						}
					}
				}
			}
			
			mpStrEncrypt = GetAllocator()->New<CPDF_Encrypt>( id1, O, U, PDFENCRYPT_ALGORITHM_RC4, version, revision, keyLength, bMetaData, pValue, GetAllocator() );
			mpStmEncrypt = mpStrEncrypt;
			mpEefEncrypt = mpStrEncrypt;
			mSyntaxParser.SetEncrypt( mpStrEncrypt );

			mpStrEncrypt->Authenticate( "" );

			return TRUE;
		}
		//非标准的加密处理，未支持
	}
	return false;
};

size_t CPDF_Parser::ParseXRefTable( size_t offset, CPDF_DictionaryPtr & pTrailerDictRet )
{
	if ( mpXRefTable == nullptr )
	{
		return 0;
	}
	size_t orgOffset = mSyntaxParser.GetPos();
	mSyntaxParser.SetPos( offset );

	CPDF_ParseWordDes wordDes;
	size_t xrefEntryCount = 0;
	if ( mSyntaxParser.GetWord( wordDes ) == false || ! mSyntaxParser.IsWord( "xref" ) )
	{
		mSyntaxParser.SetPos( orgOffset );
		return 0; 
	}

	size_t lBeginNum = 0, lCount = 0;
	size_t objNum = 0, objGenNum = 0, objOffset = 0;
	BYTE tmpBytes[20];
	while ( true )
	{
		lBeginNum = 0;
		lCount = 0;
		if ( mSyntaxParser.GetWord( wordDes ) == false ) break;
		if( wordDes.type == PARSE_WORD_INTEGER )
		{
			lBeginNum = mSyntaxParser.GetInteger();
		}else if ( wordDes.type == PARSE_WORD_UNKNOWN && mSyntaxParser.IsWord( "trailer" ) )
		{
			pTrailerDictRet = mSyntaxParser.GetDictionaryPtr();
 			mpXRefTable->AddTrailerDict( pTrailerDictRet );
		}else{
			break;
		}

		if ( mSyntaxParser.GetWord( wordDes ) == false ) break;
		if( wordDes.type == PARSE_WORD_INTEGER )
		{
			lCount = mSyntaxParser.GetInteger();
		}else{
			break;
		}

		mSyntaxParser.SeekToNextLine();
		objNum = 0, objGenNum = 0, objOffset = 0;
		for ( size_t i = lBeginNum; i < lBeginNum + lCount; i++ )
		{
			mSyntaxParser.ReadBytes( tmpBytes, 20 );
			mSyntaxParser.Seek( 20 );

			if ( i == 0 )
			{
				CPDF_XRefEntry tmpEntry( XREF_ENTRY_TYPE_FREE, 0, 0, 0 );
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
						CPDF_XRefEntry tmpEntry( XREF_ENTRY_TYPE_COMMON, i, objOffset, objGenNum );
						mpXRefTable->Add( tmpEntry );
						xrefEntryCount++;
					}
				}
			}
		}
	}
	mSyntaxParser.SetPos( orgOffset );
	return xrefEntryCount;
}

size_t  CPDF_Parser::ParseXRefStream( size_t offset, CPDF_DictionaryPtr & pTrailerDictRet )
{
	if ( mpXRefTable == nullptr )
	{
		return 0;
	}

	size_t orgOffset = mSyntaxParser.GetPos();
	mSyntaxParser.SetPos( offset );

	CPDF_ParseWordDes wordDes;
	size_t xrefEntryCount = 0;
	size_t lSecBeginNum = 0, lSecObjCount = 0,  lSecCount = 0, lSize = 0, lW1 = 0, lW2 = 0, lW3 = 0;

	CPDF_ObjectPtr pStream = GetObject();
	if ( !pStream || pStream->GetType() != OBJ_TYPE_STREAM )
	{
		mSyntaxParser.SetPos( orgOffset );
		return 0;
	}
	CPDF_DictionaryPtr pDict = pStream->GetStreamPtr()->GetDictPtr();
	if ( !pDict )
	{
		mSyntaxParser.SetPos( orgOffset );
		return 0;
	}else{
		pTrailerDictRet = pDict;
		mpXRefTable->AddTrailerDict( pDict ); 
	}
	CPDF_ObjectPtr pElement =  pDict->GetElement( "Type" );
	if ( !pElement || pElement->GetType() != OBJ_TYPE_NAME || pElement->GetNamePtr()->GetString() != "XRef" )
	{
		mSyntaxParser.SetPos( orgOffset );
		return 0;
	}
	pElement = pDict->GetElement( "Size" );
	if ( !pElement || pElement->GetType() != OBJ_TYPE_NUMBER )
	{
		mSyntaxParser.SetPos( orgOffset );
		return 0;
	}
	lSize = pElement->GetNumberPtr()->GetInteger();

	pElement = pDict->GetElement( "W" );
	if ( !pElement || pElement->GetType() != OBJ_TYPE_ARRAY || pElement->GetArrayPtr()->GetCount() < 3 )
	{
		mSyntaxParser.SetPos( orgOffset );
		return 0;
	}
	if (	pElement->GetArrayPtr()->GetElement( 0 )->GetType() != OBJ_TYPE_NUMBER ||
			pElement->GetArrayPtr()->GetElement( 1 )->GetType() != OBJ_TYPE_NUMBER ||
			pElement->GetArrayPtr()->GetElement( 2 )->GetType() != OBJ_TYPE_NUMBER )
	{
		mSyntaxParser.SetPos( orgOffset );
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
			mSyntaxParser.SetPos( orgOffset );
			return 0;
		}
		lSecCount = pElement->GetArrayPtr()->GetCount() / 2; 
	}

	CPDF_StreamAcc streamAcc( GetAllocator() );
	if ( streamAcc.Attach( pStream->GetStreamPtr() ) == false )
	{
		mSyntaxParser.SetPos( orgOffset );
		return 0;
	}
	size_t streamSize = streamAcc.GetSize();
	PCBYTE PBYTE = streamAcc.GetData();
	size_t field1 = 0, field2 = 0, field3 = 0;
	size_t lBlockCount = 0;
	size_t lEntrySize = lW1 + lW2 + lW3;
	size_t lItemOfSecCount = 0;

// 	FILE * pFile = fopen( "d:\\xrefstream.txt", "wb+" );
// 	PBYTE tmpByte = (PBYTE)( streamAcc.GetData() );
// 	for ( size_t offset = 0; offset < streamSize; offset+=lEntrySize )
// 	{
// 		for ( size_t index = 0; index < lEntrySize; ++index )
// 		{
// 			fprintf( pFile, "%02X", *(tmpByte + offset + index) );
// 		}
// 		fprintf( pFile, "\n" );
// 	}
// 	fclose( pFile );

	std::unordered_set<size_t> XrefVerify1;
	std::unordered_set<size_t> XrefVerify2;

	size_t lEntryToCheck = 0;

	size_t *lSecBeginArray = GetAllocator()->NewArray<size_t>( lSecCount );
	size_t *lSecObjCountArray = GetAllocator()->NewArray<size_t>( lSecCount );
	size_t lEntryCount = 0;
	if ( !pElement )
	{
		lSecBeginArray[0] = 0;
		lSecObjCountArray[0] = lSize;
		lEntryCount = lSize;
	}else{
		CPDF_ArrayPtr pIndexArray = pElement->GetArrayPtr();
		for ( size_t i = 0; i < lSecCount; i++ )
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


	for ( size_t i = 0; i < lSecCount; i++ )
	{
		lSecBeginNum = lSecBeginArray[i];
		lSecObjCount = lSecObjCountArray[i] + lEntryToCheck;

		lItemOfSecCount = lSecObjCount;
		size_t indexInSec = 0;
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
				field1 = *PBYTE;
				PBYTE++;
			}else{
				for ( size_t i = 0; i < lW1; i++ )
				{
					field1 = field1 << 8;
					field1 += *PBYTE;
					PBYTE++;
				}
			}
			for ( size_t j = 0; j < lW2; j++ )
			{
				field2 = field2 << 8;
				field2 += *PBYTE;
				PBYTE++;
			}
			for ( size_t k = 0; k < lW3; k++ )
			{
				field3 = field3 << 8;
				field3 += *PBYTE;
				PBYTE++;
			}
			switch( field1 )
			{
			case 2:
				{
					size_t tmpValue = ( field2 << 9 ) + field3;
					if ( XrefVerify2.count( tmpValue ) )
					{
						lEntryToCheck--;
						break;
					}else{
						XrefVerify2.insert( tmpValue );
					}
//					size_t tmpValue = ( field2 << 9 ) + field3;
// 					bool bRepeat = false;
// 					std::vector<size_t>::iterator it;
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
					CPDF_XRefEntry tmpEntry( XREF_ENTRY_TYPE_COMPRESSED, lSecBeginNum + indexInSec, field2, field3 );
					mpXRefTable->Add( tmpEntry );
					indexInSec++;
					xrefEntryCount++;
					break;
				}
			case 1:
				{	
					if ( XrefVerify1.count( field2 ) )
					{
						lEntryToCheck--;
						break;
					}else{
						XrefVerify1.insert( field2 );
					}
// 					bool bRepeat = false;
// 					std::vector<size_t>::iterator it;
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
						size_t offsetSave = mSyntaxParser.GetPos();
						mSyntaxParser.SetPos( field2 );
						mSyntaxParser.GetWord( wordDes );
						mSyntaxParser.SetPos( offsetSave );
						if ( wordDes.type != PARSE_WORD_INTEGER )
						{
							lEntryToCheck--;
							break;
						}else if (	wordDes.type == PARSE_WORD_INTEGER && mSyntaxParser.GetInteger() != (int32)(lSecBeginNum + indexInSec)  )
						{
							CPDF_XRefEntry tmpEntry( XREF_ENTRY_TYPE_COMMON, lSecBeginNum + indexInSec, field2, field3 );
							mpXRefTable->Update( lSecBeginNum + xrefEntryCount, tmpEntry );
							lEntryToCheck--;
							break;
						}
					}
					CPDF_XRefEntry tmpEntry( XREF_ENTRY_TYPE_COMMON, lSecBeginNum + indexInSec, field2, field3 );
					mpXRefTable->Add( tmpEntry );
					xrefEntryCount++;
					indexInSec++;
					break;
				}
			case 0:
				{
					CPDF_XRefEntry tmpEntry( XREF_ENTRY_TYPE_FREE, lSecBeginNum + indexInSec, 0, 0 );
					mpXRefTable->Add( tmpEntry );
					xrefEntryCount++;
					indexInSec++;
					break;
				}
			}
		}
	}

	GetAllocator()->DeleteArray<size_t>( lSecBeginArray );
	GetAllocator()->DeleteArray<size_t>( lSecObjCountArray );

	lSecBeginArray = nullptr;
	lSecObjCountArray = nullptr;
	streamAcc.Detach();
	mSyntaxParser.SetPos( orgOffset );
	return xrefEntryCount;
}


size_t CPDF_Parser::FullParseForXRef()	//分析整个文件来获取对象信息 // 还需要添加搜索xref的功能，针对某些具有xref但startxref错误的pdf文件
{
	if ( mpXRefTable == nullptr )
	{
		return 0;
	}
	CPDF_ParseWordDes	wordDes;
	size_t				xrefEntryCount = 0;
	size_t				offset = 0;
	size_t				objNumOffset = 0;
	size_t				objNum = 0;

	mSyntaxParser.SetPos( 0 );
	while ( true )
	{
		if ( mSyntaxParser.GetPos() == mSyntaxParser.GetFileSize() )
		{
			break;
		}
		if ( mSyntaxParser.GetWord( wordDes ) == false )
		{
			break;
		}
		if ( mSyntaxParser.IsWord( "trailer" ) )
		{
			mpXRefTable->AddTrailerDict( mSyntaxParser.GetDictionaryPtr() ); 
			continue;
		}
		if ( mSyntaxParser.IsWord( "obj" ) )
		{
			offset = mSyntaxParser.GetPos();
			mSyntaxParser.SetPos( wordDes.offset );
			mSyntaxParser.SeekToPrevWord();
			mSyntaxParser.SeekToPrevWord();
			mSyntaxParser.SeekToPrevWord();

			objNumOffset = mSyntaxParser.GetPos();
			mSyntaxParser.GetWord( wordDes );
			if ( wordDes.type != PARSE_WORD_INTEGER )
			{
				mSyntaxParser.SetPos( offset );
				continue;
			}
			objNum = mSyntaxParser.GetInteger();
			mSyntaxParser.GetWord( wordDes );
			if ( wordDes.type != PARSE_WORD_INTEGER )
			{
				mSyntaxParser.SetPos( offset );
				continue;
			}
			CPDF_XRefEntry tmpEntry( XREF_ENTRY_TYPE_COMMON, objNum, objNumOffset, 0 );
			mpXRefTable->Add( tmpEntry );
			xrefEntryCount++;
			
			//seek到下一个obj
			//某些错误文件缺少endobj
			mSyntaxParser.GetWord( wordDes );
			while( mSyntaxParser.GetWord( wordDes ) )
			{
				if ( mSyntaxParser.IsWord( "stream" ) )
				{
					while( TRUE )
					{
						mSyntaxParser.SeekToNextLine();
						BYTE tmp[9];
						tmp[8] = '\0';
						if ( mSyntaxParser.ReadBytes( tmp, 9 ) == 0 )
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
				}else if ( wordDes.type == PARSE_WORD_DICT_B )
				{
					mSyntaxParser.SeekToPrevWord();
					CPDF_DictionaryPtr pDict = mSyntaxParser.GetDictionaryPtr();
					if ( mSyntaxParser.GetWord( wordDes ) == false )
					{
						return xrefEntryCount;
					}else if ( mSyntaxParser.IsWord( "stream" ) )
					{
						mSyntaxParser.SeekToPrevWord();
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

CPDF_ObjectPtr CPDF_Parser::GetObject()
{
	if ( mpIFileRead == nullptr )
	{
		return CPDF_ObjectPtr();
	}

	size_t offset = mSyntaxParser.GetPos();
	CPDF_ParseWordDes wordDes;
	size_t objNum = 0, genNum = 0;
	mSyntaxParser.GetWord( wordDes );
	if ( wordDes.type == PARSE_WORD_INTEGER )
	{
		objNum = mSyntaxParser.GetInteger();
	}else{
		return CPDF_ObjectPtr();
	}
	mSyntaxParser.GetWord( wordDes );
	if ( wordDes.type == PARSE_WORD_INTEGER )
	{
		genNum = mSyntaxParser.GetInteger();
	}else{
		return CPDF_ObjectPtr();
	}
	mSyntaxParser.GetWord( wordDes );
	if ( !mSyntaxParser.IsWord( "obj" ) )
	{
		return CPDF_ObjectPtr();
	}

	mSyntaxParser.SetCurObjNum( objNum );
	mSyntaxParser.SetCurGenNum( genNum );

	size_t pos = mSyntaxParser.GetPos();
	mSyntaxParser.GetWord( wordDes );
	CPDF_ObjectPtr pCurObj;

	switch ( wordDes.type )
	{
	case PARSE_WORD_INTEGER:
		{
			int32 integer = 0;
			size_t refObjNum = 0;
			size_t refGenNum = 0;
			size_t tmpPos = 0;
			integer = mSyntaxParser.GetInteger();
			refObjNum = (size_t)( integer );
			tmpPos = mSyntaxParser.GetPos();
			mSyntaxParser.GetWord( wordDes );
			if ( wordDes.type == PARSE_WORD_INTEGER )
			{
				refGenNum = (size_t)( mSyntaxParser.GetInteger() );
				mSyntaxParser.GetWord( wordDes );
				if ( mSyntaxParser.IsWord( "R" ) )
				{
					pCurObj = CPDF_Reference::Create( refObjNum, refGenNum, mpFile, GetAllocator() );
					break;
				}
			}
			mSyntaxParser.SetPos( tmpPos );
			pCurObj = CPDF_Number::Create( integer, GetAllocator() );
			break;
		}
	case PARSE_WORD_FLOAT:
		{
			pCurObj = CPDF_Number::Create( mSyntaxParser.GetFloat(), GetAllocator() );
			break;
		}
	case PARSE_WORD_STRING:
		{
			CByteString str = mSyntaxParser.GetString();
			if ( mpStrEncrypt )
			{	
				mpStrEncrypt->Decrypt( str, objNum, genNum );
			}
			pCurObj = CPDF_String::Create( str, GetAllocator() );
			break;
		}
	case PARSE_WORD_NAME:
		{
			pCurObj = CPDF_Name::Create( mSyntaxParser.GetString(), GetAllocator() );
			break;
		}
	case PARSE_WORD_ARRAY_B:
		{
			mSyntaxParser.SetPos( pos );
			pCurObj = mSyntaxParser.GetArrayPtr();
			break;
		}
	case PARSE_WORD_DICT_B:
		{
			mSyntaxParser.SetPos( pos );
			pCurObj = mSyntaxParser.GetDictionaryPtr();
			if ( ! pCurObj )
			{
				break;
			}
			mSyntaxParser.GetWord( wordDes );
			if ( mSyntaxParser.IsWord( "stream" ) )
			{
				size_t length = 0;
				CPDF_ObjectPtr pObj = pCurObj->GetDictPtr()->GetElement( "Length" );
				if ( ! pObj )
				{
					length = 0;
				}else if ( pObj->GetType() == OBJ_TYPE_NUMBER || pObj->GetType() == OBJ_TYPE_REFERENCE )
				{
					if ( pObj->GetType() == OBJ_TYPE_NUMBER )
					{
						length = pObj->GetNumberPtr()->GetInteger();
					}else{
						size_t objNum = pObj->GetRefPtr()->GetRefNum();
						size_t offset = mSyntaxParser.GetPos();
						CPDF_XRefEntry entry;
						mpXRefTable->Get( objNum, entry );
						mSyntaxParser.SetPos( entry.GetOffset() );
						CPDF_ObjectPtr pObj = GetObject();
						if ( pObj )
						{
							if ( pObj->GetType() == OBJ_TYPE_NUMBER )
							{
								length = pObj->GetNumberPtr()->GetInteger();
							}
						}
						mSyntaxParser.SetPos( offset );
					}
					mSyntaxParser.SeekToNextLine();
					offset = mSyntaxParser.GetPos();
					if ( length == 0 )
					{
						mSyntaxParser.SeekToEndStream();
						if ( mSyntaxParser.GetPos() > offset )
						{
							length = mSyntaxParser.GetPos() - offset + 1;
						}
						mSyntaxParser.SeekToNextLine();
					}else{	
						mSyntaxParser.Seek( length );
					}
					mSyntaxParser.GetWord( wordDes );
					if ( mSyntaxParser.IsWord( "endstream" ) )
					{
						pCurObj = CPDF_Stream::Create( mpIFileRead, offset, length, pCurObj->GetDictPtr(), objNum, genNum, mpStmEncrypt, GetAllocator() );
					}
				}
			}
			break;
		}
	case PARSE_WORD_UNKNOWN:
		{
			if ( mSyntaxParser.IsWord( "false" ) )
			{
				pCurObj = CPDF_Boolean::Create( false, GetAllocator() );
			}else if ( mSyntaxParser.IsWord( "true" ) )
			{
				pCurObj = CPDF_Boolean::Create( TRUE, GetAllocator() );
			}else if ( mSyntaxParser.IsWord( "null" ) )
			{
				pCurObj = CPDF_Null::Create( GetAllocator() );
			}else if ( mSyntaxParser.IsWord( "endobj" ) )
			{
				pCurObj = CPDF_Null::Create( GetAllocator() );
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
	return CPDF_ObjectPtr();
}
CPDF_ObjectPtr CPDF_Parser::GetObjectInObjStm( CPDF_StreamPtr & pStream, const PDF_RefInfo & objRefInfo, size_t index )
{
	if ( !pStream )
	{
		return CPDF_ObjectPtr();
	}

	CPDF_StreamAcc stmAcc( GetAllocator() );
	stmAcc.Attach( pStream );
	PCBYTE pData = stmAcc.GetData();
	size_t lDataSize = stmAcc.GetSize();
	IRead * pIRead = CreateMemBufIRead( (PBYTE)pData, lDataSize, GetAllocator() );
	if ( pIRead == nullptr )
	{
		stmAcc.Detach();
		return CPDF_ObjectPtr();
	}
	CPDF_SyntaxParser sParser( mpFile, GetAllocator() );
	sParser.InitParser( pIRead );
	sParser.SetCurObjNum( objRefInfo.objNum );
	sParser.SetCurGenNum( objRefInfo.genNum );

	CPDF_ParseWordDes	wordDes;
	size_t				first = 0;
	size_t				offset = 0;
	size_t				objNumTmp = 0;
	sParser.SetPos( 0 );

	first = pStream->GetDictPtr()->GetElement( "First" )->GetNumberPtr()->GetInteger();

	for ( size_t i = 0; i < index; i++ )
	{
		if ( !sParser.GetWord( wordDes ) )
		{
			return CPDF_ObjectPtr();
		}
		if ( !sParser.GetWord( wordDes ) )
		{
			return CPDF_ObjectPtr();
		}
	}
	while ( true )
	{
		if ( sParser.GetPos() == sParser.GetFileSize() )
		{
			break;
		}
		if ( sParser.GetWord( wordDes ) == false )
		{
			break;
		}
		if ( wordDes.type == PARSE_WORD_INTEGER )
		{
			offset = sParser.GetPos();

			objNumTmp = sParser.GetInteger();

			sParser.GetWord( wordDes );
			if ( wordDes.type != PARSE_WORD_INTEGER )
			{
				sParser.SetPos( offset );
				continue;
			}
			first += mSyntaxParser.GetInteger();
			if ( objNumTmp == objRefInfo.objNum )
			{
				sParser.SetPos( first );
				size_t pos = sParser.GetPos();
				sParser.GetWord( wordDes );
				CPDF_ObjectPtr pCurObj;
				if ( wordDes.type == PARSE_WORD_DICT_B )
				{
					sParser.SetPos( pos );
					pCurObj = sParser.GetDictionaryPtr();
				}else if ( wordDes.type == PARSE_WORD_FLOAT )
				{
					pCurObj = CPDF_Number::Create( sParser.GetInteger(), GetAllocator() );
				}else if ( wordDes.type == PARSE_WORD_INTEGER )
				{
					size_t refNum = sParser.GetInteger();
					size_t pos = sParser.GetPos();
					sParser.GetWord( wordDes );
					if ( wordDes.type == PARSE_WORD_INTEGER )
					{
						size_t refGenNum = sParser.GetInteger();
						sParser.GetWord( wordDes );
						if ( sParser.IsWord( "R" ) )
						{
							pCurObj = CPDF_Reference::Create( refNum, refGenNum, mpFile, GetAllocator() );
						}else{
							pCurObj = CPDF_Number::Create( (int32)refNum, GetAllocator() );
						}
					}else{
						pCurObj = CPDF_Number::Create( (int32)refNum, GetAllocator() );
					}
					sParser.SetPos( pos );
				}else if ( wordDes.type == PARSE_WORD_STRING )
				{
					pCurObj = CPDF_String::Create( sParser.GetString(), GetAllocator() );
				}else if ( wordDes.type == PARSE_WORD_NAME )
				{
					pCurObj = CPDF_Name::Create( sParser.GetString(), GetAllocator() );
				}else if ( wordDes.type == PARSE_WORD_ARRAY_B )
				{
					sParser.SetPos( pos );
					pCurObj = sParser.GetArrayPtr();
				}else if ( sParser.IsWord( "false" ) )
				{
					pCurObj = CPDF_Boolean::Create( false, GetAllocator() );
				}else if ( sParser.IsWord( "true" ) )
				{
					pCurObj = CPDF_Boolean::Create( TRUE, GetAllocator() );
				}else if ( sParser.IsWord( "null" ) )
				{
					pCurObj = CPDF_Null::Create( GetAllocator() );
				}else{
					pCurObj.Reset();
				}
				return pCurObj;
			}
		}
	}
	return CPDF_ObjectPtr();
}

bool CPDF_Parser::GetAllObjectsInObjStm( CPDF_StreamPtr & pStream, CPDF_Collector * pCollector )
{
	if ( pCollector == nullptr )
	{
		return false;
	}
	if ( !pStream )
	{
		return false;
	}

	CPDF_StreamAcc stmAcc( GetAllocator() );
	stmAcc.Attach( pStream );
	PCBYTE pData = stmAcc.GetData();
	size_t lDataSize = stmAcc.GetSize();
	IRead * pIRead = CreateMemBufIRead( (PBYTE)pData, lDataSize, GetAllocator() );
	if ( pIRead == nullptr )
	{
		stmAcc.Detach();
		return false;
	}

	CPDF_SyntaxParser sParser( mpFile, GetAllocator() );
	sParser.InitParser( pIRead );
	sParser.SetPos( 0 );

	CPDF_ParseWordDes	wordDes;
	size_t				first = 0;
	size_t				count = 0;
	size_t				offset = 0;
	size_t				pposi = 0;
	size_t				objNum = 0;

	first = pStream->GetDictPtr()->GetElement( "First" )->GetNumberPtr()->GetInteger();
	count = pStream->GetDictPtr()->GetElement( "N" )->GetNumberPtr()->GetInteger();

	for ( size_t i = 0; i < count; i++ )
	{
// 		if ( sParser.GetPos() == sParser.GetFileSize() )
// 		{
// 			//error
// 			return false;
// 		}
		sParser.GetWord( wordDes );

		if ( wordDes.type == PARSE_WORD_INTEGER )
		{
			objNum = sParser.GetInteger();

			sParser.SetCurObjNum( objNum );
			sParser.SetCurGenNum( 0 );

			sParser.GetWord( wordDes );
			if ( wordDes.type == PARSE_WORD_INTEGER )
			{
				offset = sParser.GetInteger();

				pposi = sParser.GetPos();

				sParser.SetPos( first + offset );

				size_t pos = sParser.GetPos();

				sParser.GetWord( wordDes );

				CPDF_ObjectPtr pCurObj;
				if ( wordDes.type == PARSE_WORD_DICT_B )
				{
					sParser.SetPos( pos );
					pCurObj = sParser.GetDictionaryPtr();
				}else if ( wordDes.type == PARSE_WORD_FLOAT )
				{
					pCurObj = CPDF_Number::Create( sParser.GetInteger(), GetAllocator() );
				}else if ( wordDes.type == PARSE_WORD_INTEGER )
				{
					int32 refNum = sParser.GetInteger();
					size_t pos = sParser.GetPos();
					sParser.GetWord( wordDes );
					if ( wordDes.type == PARSE_WORD_INTEGER )
					{
						int32 refGenNum = sParser.GetInteger();
						sParser.GetWord( wordDes );
						if ( sParser.IsWord( "R" ) )
						{
							pCurObj = CPDF_Reference::Create( (size_t)refNum, (size_t)refGenNum, mpFile, GetAllocator() );
						}else{
							pCurObj = CPDF_Number::Create( refNum, GetAllocator() );
						}
					}else{
						pCurObj = CPDF_Number::Create( (int32)refNum, GetAllocator() );
					}
					sParser.SetPos( pos );
				}else if ( wordDes.type == PARSE_WORD_STRING )
				{
					pCurObj = CPDF_String::Create( sParser.GetString(), GetAllocator() );
				}else if ( wordDes.type == PARSE_WORD_NAME )
				{
					pCurObj = CPDF_Name::Create( sParser.GetString(), GetAllocator() );
				}else if ( wordDes.type == PARSE_WORD_ARRAY_B )
				{
					sParser.SetPos( pos );
					pCurObj = sParser.GetArrayPtr();
				}else if ( sParser.GetString() == "false" )
				{
					pCurObj = CPDF_Boolean::Create( false, GetAllocator() );
				}else if ( sParser.GetString() == "true" )
				{
					pCurObj = CPDF_Boolean::Create( TRUE, GetAllocator() );
				}else if ( sParser.GetString() == "null" )
				{
					pCurObj = CPDF_Null::Create( GetAllocator() );
				}else{
					pCurObj.Reset();
				}
				pCollector->Add( objNum, pCurObj );
			}
			sParser.SetPos( pposi );
		}
 	}
	return true;
}

}//namespace