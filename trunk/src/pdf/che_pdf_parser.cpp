#include "../../include/pdf/che_pdf_parser.h"
#include "../../include/pdf/che_pdf_encrypt.h"

#include <unordered_set>

CHE_PDF_SyntaxParser::CHE_PDF_SyntaxParser( CHE_PDF_File * pFile, CHE_Allocator * pAllocator )
	: CHE_Object( pAllocator ), mpFile( pFile ), mpIFileRead( NULL ), mpStrEncrypt( NULL ), mlFilePos( 0 ),
	mlFileSize( 0 ), mlBufferSize( 0 ), mlBufferPos( 0 ), mlSubmitSize( 0 ), mCurObjNum( 0 ), mCurGenNum( 0 ), 
	mbBegin( TRUE ), mbPoint( FALSE ), mbSign( FALSE )
{
	memset( mWordBuffer, 0, 255 );
}

CHE_PDF_SyntaxParser::~CHE_PDF_SyntaxParser()
{
}

HE_BOOL CHE_PDF_SyntaxParser::InitParser( IHE_Read* pFileAccess )
{
	if ( pFileAccess )
	{
		mpIFileRead = pFileAccess;
		mlFileSize = mpIFileRead->GetSize();
		mlFilePos = 0;
		return TRUE;
	}else{
		return FALSE;
	}
}

HE_ULONG CHE_PDF_SyntaxParser::Seek( HE_ULONG bytes )
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

HE_VOID CHE_PDF_SyntaxParser::SeekToPrevLine()
{
	if ( mpIFileRead )
	{
		while( mlFilePos > 0 )
		{
			HE_BYTE byte = mpIFileRead->ReadByte( mlFilePos );
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

HE_VOID CHE_PDF_SyntaxParser::SeekToNextLine()
{
	if ( mpIFileRead )
	{
		while( mlFilePos < mlFileSize )
		{
			HE_BYTE byte = mpIFileRead->ReadByte( mlFilePos );
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

HE_VOID	CHE_PDF_SyntaxParser::SeekToPrevWord()
{
	if ( mpIFileRead )
	{
		HE_BOOL bWordFound = FALSE;
		while( mlFilePos > 0 )
		{
			HE_BYTE byte = mpIFileRead->ReadByte( mlFilePos );
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

HE_VOID CHE_PDF_SyntaxParser::SeekToNextWord()
{
	if ( mpIFileRead )
	{
		HE_BOOL bNewBegin = FALSE;
		while( mlFilePos < mlFileSize )
		{
			HE_BYTE byte = mpIFileRead->ReadByte( mlFilePos );
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

HE_VOID	CHE_PDF_SyntaxParser::SeekToEndStream()
{
	HE_CHAR tmpStr[8];
	if ( mpIFileRead )
	{
		while( mlFilePos < mlFileSize )
		{
			HE_BYTE byte = mpIFileRead->ReadByte( mlFilePos++ );
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

HE_VOID CHE_PDF_SyntaxParser::SeekToEndobj()
{
	HE_CHAR tmpStr[5];
	if ( mpIFileRead )
	{
		while( mlFilePos < mlFileSize )
		{
			HE_BYTE byte = mpIFileRead->ReadByte( mlFilePos++ );
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

HE_VOID CHE_PDF_SyntaxParser::SeekToMark( CHE_ByteString markStr )
{
	if ( markStr.GetLength() == 0 )
	{
		return;
	}else if ( markStr.GetLength() == 1 )
	{
		if ( mpIFileRead )
		{
			HE_BYTE byteTmp = markStr[0], byte = 0;
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
		HE_CHAR * pTmpStr = GetAllocator()->NewArray<HE_CHAR>( markStr.GetLength()+1 );
		pTmpStr[markStr.GetLength()] = 0;
		if ( mpIFileRead )
		{
			HE_BYTE byteTmp = markStr[0], byte = 0;
			while( mlFilePos < mlFileSize )
			{
				byte = mpIFileRead->ReadByte( mlFilePos++ );
				if ( byte == byteTmp )
				{
					if ( mlFilePos + markStr.GetLength() - 1 < mlFileSize )
					{
						mpIFileRead->ReadBlock( pTmpStr, mlFilePos-1, markStr.GetLength() );
						pTmpStr[markStr.GetLength()] = 0;
						if ( markStr == CHE_ByteString(pTmpStr) )
						{
							mlFilePos--;
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
	return mpIFileRead->ReadBlock( pBuffer, mlFilePos, length );
}

HE_BOOL CHE_PDF_SyntaxParser::GetWord( CHE_PDF_ParseWordDes & des )
{
	if ( mpIFileRead )
	{
		des.offset = 0;
		des.type = PARSE_WORD_UNKNOWN;

		HE_BYTE byte = 0;
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
							HE_ULONG tmpValue = 0;
							HE_BYTE	tmpCount = 0;
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
									mWordBuffer[mlBufferPos++] = (HE_BYTE)tmpValue;
									tmpCount = 0;
									tmpValue = 0;
								}
								if ( byte == '>' )
								{
									if ( tmpCount == 1 )
									{
										tmpValue = tmpValue * 16;
										mWordBuffer[mlBufferPos++] = (HE_BYTE)tmpValue;
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
								HE_ULONG tmpValue = 0;
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
								mWordBuffer[mlBufferPos++] = (HE_BYTE)tmpValue;
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
						HE_ULONG lCount = 1;
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
										HE_ULONG tmpValue = byte - '0';
										HE_BYTE tmpByte = 0;
										HE_BYTE tmpCount = 1;
										while ( true )
										{
											tmpByte = mpIFileRead->ReadByte( ++mlFilePos );
											if ( tmpByte >= '0' && tmpByte <= '7' )
											{
												tmpValue = tmpValue * 8 + (tmpByte-'0');
												tmpCount++;
												if ( tmpCount == 3 )
												{
													mWordBuffer[mlBufferPos++] = (HE_BYTE)tmpValue;
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
											mWordBuffer[mlBufferPos++] = (HE_BYTE)tmpValue;
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
					if ( mbBegin && mbSign == FALSE )
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
					if ( mbPoint == FALSE )
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
						mbBegin = FALSE;
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
						mbBegin = FALSE;
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
						mbBegin = FALSE;
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
			return FALSE;
		}
	}else{
		return FALSE;
	}
}

CHE_PDF_ArrayPtr CHE_PDF_SyntaxParser::GetArrayPtr()
{
	if ( mpIFileRead == NULL )
	{
		return CHE_PDF_ArrayPtr();
	}

	CHE_PDF_ParseWordDes wordDes;

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

				integer = GetString().GetInteger();
				objNum = (HE_ULONG)( integer );

				if ( GetWord( wordDes ) && wordDes.type == PARSE_WORD_INTEGER )
				{
					genNum = (HE_ULONG)( GetString().GetInteger() );

					if ( GetWord( wordDes ) && IsWord( "R" ) )
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
				CHE_PDF_NumberPtr pTmp = CHE_PDF_Number::Create( GetString().GetFloat(), GetAllocator() );

				arrayPtrRet->Append( pTmp );

				break;
			}
		case PARSE_WORD_STRING:
			{
				CHE_PDF_StringPtr pTmp = CHE_PDF_String::Create( GetString(),  GetAllocator() );

				arrayPtrRet->Append( pTmp );

				break;
			}
		case PARSE_WORD_NAME:
			{
				CHE_PDF_NamePtr pTmp = CHE_PDF_Name::Create( GetString(), GetAllocator() );

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
				if ( IsWord( "false" ) )
				{
					CHE_PDF_BooleanPtr tmpPtr = CHE_PDF_Boolean::Create( FALSE, GetAllocator() );
					
					arrayPtrRet->Append( tmpPtr );
				}
				else if ( IsWord( "true" ) )
				{
					CHE_PDF_BooleanPtr tmpPtr = CHE_PDF_Boolean::Create( TRUE, GetAllocator() );
				
					arrayPtrRet->Append( tmpPtr );	
				}
				else if ( IsWord( "null" ) )
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
	if ( mpIFileRead == NULL )
	{
		return CHE_PDF_DictionaryPtr();
	}

	CHE_PDF_ParseWordDes wordDes;

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
				GetString( key );
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

				integer = GetString().GetInteger();
				objNum = (HE_ULONG)( integer );
				
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

				CHE_PDF_ArrayPtr tmpPtr = GetArrayPtr();

				dictRet->SetArray( key, tmpPtr );

				break;
			}
		case PARSE_WORD_DICT_B:
			{
				SetPos( preOffset );

				CHE_PDF_DictionaryPtr tmpPtr = GetDictionaryPtr();

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
					dictRet->SetBoolean( key, FALSE );
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

	dictRet->SetModified( FALSE );

	return dictRet;
}

HE_BOOL	CHE_PDF_SyntaxParser::IsWord( const char * words )
{
	if ( strcmp( (const char*)&mWordBuffer[0], words ) == 0 )
	{
		return TRUE;
	}
	return FALSE;
}

HE_INT32 CHE_PDF_SyntaxParser::GetInteger()
{
	return atoi( (const char*)&mWordBuffer[0] );
}

HE_FLOAT CHE_PDF_SyntaxParser::GetFloat()
{
	return (HE_FLOAT)atof( (const char*)&mWordBuffer[0] );
}

CHE_ByteString CHE_PDF_SyntaxParser::GetString()
{
	return CHE_ByteString( HE_LPCSTR(mWordBuffer), mlSubmitSize );
}

void CHE_PDF_SyntaxParser::GetString( CHE_ByteString & str )
{
	str.SetData( mWordBuffer, mlSubmitSize );
}

HE_VOID CHE_PDF_SyntaxParser::SubmitBufferStr( )
{
	mWordBuffer[mlBufferPos] = '\0';
	mlSubmitSize = mlBufferPos;
	mlBufferPos = 0;
	mbBegin = TRUE;
	mbPoint = FALSE;
	mbSign = FALSE;
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
		pParserRet->mSyntaxParser.InitParser( pParserRet->mpIFileRead );
	}
	return pParserRet;
}

CHE_PDF_Parser::CHE_PDF_Parser( CHE_PDF_File * pFile, IHE_Read * pRead, CHE_PDF_XRefTable * pXrefTable, CHE_Allocator * pAllocator )
	: CHE_Object( pAllocator ), mpFile( pFile ), mpIFileRead( pRead ), mlStartxref(0), mpXRefTable( pXrefTable ),
	mSyntaxParser( pFile, pAllocator ), mpStrEncrypt(NULL), mpStmEncrypt(NULL), mpEefEncrypt(NULL) { }

CHE_PDF_Parser::~CHE_PDF_Parser()
{
	mpXRefTable = NULL;
	mlStartxref = 0;
	if ( mpStmEncrypt )
	{
		GetAllocator()->Delete( mpStmEncrypt );
		mpStmEncrypt = NULL;
	}
	if ( mpIFileRead )
	{
		mpIFileRead = NULL;
	}
}

HE_ULONG CHE_PDF_Parser::GetFileSize() const
{
	if ( mpIFileRead )
	{
		return mpIFileRead->GetSize();
	}else{
		return 0;
	}
}

PDF_VERSION CHE_PDF_Parser::GetPDFVersion() const
{
	HE_ULONG offset = 0;
	HE_ULONG fileSize = mpIFileRead->GetSize();
	HE_BOOL bHeadFound = FALSE;

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

HE_BOOL CHE_PDF_Parser::Authenticate( const CHE_ByteString & password ) 
{
	HE_BOOL bRet = FALSE;

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

HE_ULONG CHE_PDF_Parser::GetStartxref( HE_ULONG range )
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
		mlStartxref = iStartXref;
		return iStartXref;
	}
	return 0;
}

HE_ULONG CHE_PDF_Parser::ParseXRef()
{
	if ( mpIFileRead == NULL )
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

	CHE_PDF_DictionaryPtr pDict;
	CHE_PDF_ArrayPtr pIdArray;
	CHE_PDF_ObjectPtr pObj;
	HE_ULONG offset = mlStartxref;
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
			if ( mpStrEncrypt == NULL )
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
				CHE_ByteString str = pObj->GetStringPtr()->GetString();
				for ( HE_ULONG i = 0; i < str.GetLength(); i++ )
				{
					U[i] = str[i];
				}
			}
			//获取O
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
								CHE_ByteString cfm = pObj->GetNamePtr()->GetString();
								if ( cfm == "AESV2" )
								{
									mpStrEncrypt = GetAllocator()->New<CHE_PDF_Encrypt>( id1, O, U, PDFENCRYPT_ALGORITHM_AESV2, version, revision, keyLength, bMetaData, pValue, GetAllocator() );
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
			
			mpStrEncrypt = GetAllocator()->New<CHE_PDF_Encrypt>( id1, O, U, PDFENCRYPT_ALGORITHM_RC4, version, revision, keyLength, bMetaData, pValue, GetAllocator() );
			mpStmEncrypt = mpStrEncrypt;
			mpEefEncrypt = mpStrEncrypt;
			mSyntaxParser.SetEncrypt( mpStrEncrypt );

			mpStrEncrypt->Authenticate( "" );

			return TRUE;
		}
		//非标准的加密处理，未支持
	}
	return FALSE;
};

HE_ULONG CHE_PDF_Parser::ParseXRefTable( HE_ULONG offset, CHE_PDF_DictionaryPtr & pTrailerDictRet )
{
	if ( mpXRefTable == NULL )
	{
		return 0;
	}
	HE_ULONG orgOffset = mSyntaxParser.GetPos();
	mSyntaxParser.SetPos( offset );

	CHE_PDF_ParseWordDes wordDes;
	HE_ULONG xrefEntryCount = 0;
	if ( mSyntaxParser.GetWord( wordDes ) == FALSE || ! mSyntaxParser.IsWord( "xref" ) )
	{
		mSyntaxParser.SetPos( orgOffset );
		return 0; 
	}

	HE_ULONG lBeginNum = 0, lCount = 0;
	HE_ULONG objNum = 0, objGenNum = 0, objOffset = 0;
	HE_BYTE tmpBytes[20];
	while ( true )
	{
		lBeginNum = 0;
		lCount = 0;
		if ( mSyntaxParser.GetWord( wordDes ) == FALSE ) break;
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

		if ( mSyntaxParser.GetWord( wordDes ) == FALSE ) break;
		if( wordDes.type == PARSE_WORD_INTEGER )
		{
			lCount = mSyntaxParser.GetInteger();
		}else{
			break;
		}

		mSyntaxParser.SeekToNextLine();
		objNum = 0, objGenNum = 0, objOffset = 0;
		for ( HE_ULONG i = lBeginNum; i < lBeginNum + lCount; i++ )
		{
			mSyntaxParser.ReadBytes( tmpBytes, 20 );
			mSyntaxParser.Seek( 20 );

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
	mSyntaxParser.SetPos( orgOffset );
	return xrefEntryCount;
}

HE_ULONG  CHE_PDF_Parser::ParseXRefStream( HE_ULONG offset, CHE_PDF_DictionaryPtr & pTrailerDictRet )
{
	if ( mpXRefTable == NULL )
	{
		return 0;
	}

	HE_ULONG orgOffset = mSyntaxParser.GetPos();
	mSyntaxParser.SetPos( offset );

	CHE_PDF_ParseWordDes wordDes;
	HE_ULONG xrefEntryCount = 0;
	HE_ULONG lSecBeginNum = 0, lSecObjCount = 0,  lSecCount = 0, lSize = 0, lW1 = 0, lW2 = 0, lW3 = 0;

	CHE_PDF_ObjectPtr pStream = GetObject();
	if ( !pStream || pStream->GetType() != OBJ_TYPE_STREAM )
	{
		mSyntaxParser.SetPos( orgOffset );
		return 0;
	}
	CHE_PDF_DictionaryPtr pDict = pStream->GetStreamPtr()->GetDictPtr();
	if ( !pDict )
	{
		mSyntaxParser.SetPos( orgOffset );
		return 0;
	}else{
		pTrailerDictRet = pDict;
		mpXRefTable->AddTrailerDict( pDict ); 
	}
	CHE_PDF_ObjectPtr pElement =  pDict->GetElement( "Type" );
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

	CHE_PDF_StreamAcc streamAcc( GetAllocator() );
	if ( streamAcc.Attach( pStream->GetStreamPtr() ) == FALSE )
	{
		mSyntaxParser.SetPos( orgOffset );
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

	std::unordered_set<HE_ULONG> XrefVerify1;
	std::unordered_set<HE_ULONG> XrefVerify2;

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
					if ( XrefVerify2.count( tmpValue ) )
					{
						lEntryToCheck--;
						break;
					}else{
						XrefVerify2.insert( tmpValue );
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
					if ( XrefVerify1.count( field2 ) )
					{
						lEntryToCheck--;
						break;
					}else{
						XrefVerify1.insert( field2 );
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
						HE_ULONG offsetSave = mSyntaxParser.GetPos();
						mSyntaxParser.SetPos( field2 );
						mSyntaxParser.GetWord( wordDes );
						mSyntaxParser.SetPos( offsetSave );
						if ( wordDes.type != PARSE_WORD_INTEGER )
						{
							lEntryToCheck--;
							break;
						}else if (	wordDes.type == PARSE_WORD_INTEGER && mSyntaxParser.GetInteger() != (HE_INT32)(lSecBeginNum + indexInSec)  )
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
	mSyntaxParser.SetPos( orgOffset );
	return xrefEntryCount;
}


HE_ULONG CHE_PDF_Parser::FullParseForXRef()	//分析整个文件来获取对象信息 // 还需要添加搜索xref的功能，针对某些具有xref但startxref错误的pdf文件
{
	if ( mpXRefTable == NULL )
	{
		return 0;
	}
	CHE_PDF_ParseWordDes	wordDes;
	HE_ULONG				xrefEntryCount = 0;
	HE_ULONG				offset = 0;
	HE_ULONG				objNumOffset = 0;
	HE_ULONG				objNum = 0;

	mSyntaxParser.SetPos( 0 );
	while ( true )
	{
		if ( mSyntaxParser.GetPos() == mSyntaxParser.GetFileSize() )
		{
			break;
		}
		if ( mSyntaxParser.GetWord( wordDes ) == FALSE )
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
			CHE_PDF_XRefEntry tmpEntry( XREF_ENTRY_TYPE_COMMON, objNum, objNumOffset, 0 );
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
						HE_BYTE tmp[9];
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
					CHE_PDF_DictionaryPtr pDict = mSyntaxParser.GetDictionaryPtr();
					if ( mSyntaxParser.GetWord( wordDes ) == FALSE )
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

CHE_PDF_ObjectPtr CHE_PDF_Parser::GetObject()
{
	if ( mpIFileRead == NULL )
	{
		return CHE_PDF_ObjectPtr();
	}

	HE_ULONG offset = mSyntaxParser.GetPos();
	CHE_PDF_ParseWordDes wordDes;
	HE_ULONG objNum = 0, genNum = 0;
	mSyntaxParser.GetWord( wordDes );
	if ( wordDes.type == PARSE_WORD_INTEGER )
	{
		objNum = mSyntaxParser.GetInteger();
	}else{
		return CHE_PDF_ObjectPtr();
	}
	mSyntaxParser.GetWord( wordDes );
	if ( wordDes.type == PARSE_WORD_INTEGER )
	{
		genNum = mSyntaxParser.GetInteger();
	}else{
		return CHE_PDF_ObjectPtr();
	}
	mSyntaxParser.GetWord( wordDes );
	if ( !mSyntaxParser.IsWord( "obj" ) )
	{
		return CHE_PDF_ObjectPtr();
	}

	mSyntaxParser.SetCurObjNum( objNum );
	mSyntaxParser.SetCurGenNum( genNum );

	HE_ULONG pos = mSyntaxParser.GetPos();
	mSyntaxParser.GetWord( wordDes );
	CHE_PDF_ObjectPtr pCurObj;

	switch ( wordDes.type )
	{
	case PARSE_WORD_INTEGER:
		{
			HE_INT32 integer = 0;
			HE_ULONG refObjNum = 0;
			HE_ULONG refGenNum = 0;
			HE_ULONG tmpPos = 0;
			integer = mSyntaxParser.GetInteger();
			refObjNum = (HE_ULONG)( integer );
			tmpPos = mSyntaxParser.GetPos();
			mSyntaxParser.GetWord( wordDes );
			if ( wordDes.type == PARSE_WORD_INTEGER )
			{
				refGenNum = (HE_ULONG)( mSyntaxParser.GetInteger() );
				mSyntaxParser.GetWord( wordDes );
				if ( mSyntaxParser.IsWord( "R" ) )
				{
					pCurObj = CHE_PDF_Reference::Create( refObjNum, refGenNum, mpFile, GetAllocator() );
					break;
				}
			}
			mSyntaxParser.SetPos( tmpPos );
			pCurObj = CHE_PDF_Number::Create( integer, GetAllocator() );
			break;
		}
	case PARSE_WORD_FLOAT:
		{
			pCurObj = CHE_PDF_Number::Create( mSyntaxParser.GetFloat(), GetAllocator() );
			break;
		}
	case PARSE_WORD_STRING:
		{
			CHE_ByteString str = mSyntaxParser.GetString();
			if ( mpStrEncrypt )
			{	
				mpStrEncrypt->Decrypt( str, objNum, genNum );
			}
			pCurObj = CHE_PDF_String::Create( str, GetAllocator() );
			break;
		}
	case PARSE_WORD_NAME:
		{
			pCurObj = CHE_PDF_Name::Create( mSyntaxParser.GetString(), GetAllocator() );
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
						HE_ULONG offset = mSyntaxParser.GetPos();
						CHE_PDF_XRefEntry entry;
						mpXRefTable->Get( objNum, entry );
						mSyntaxParser.SetPos( entry.GetOffset() );
						CHE_PDF_ObjectPtr pObj = GetObject();
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
						pCurObj = CHE_PDF_Stream::Create( mpIFileRead, offset, length, pCurObj->GetDictPtr(), objNum, genNum, mpStmEncrypt, GetAllocator() );
					}
				}
			}
			break;
		}
	case PARSE_WORD_UNKNOWN:
		{
			if ( mSyntaxParser.IsWord( "false" ) )
			{
				pCurObj = CHE_PDF_Boolean::Create( FALSE, GetAllocator() );
			}else if ( mSyntaxParser.IsWord( "true" ) )
			{
				pCurObj = CHE_PDF_Boolean::Create( TRUE, GetAllocator() );
			}else if ( mSyntaxParser.IsWord( "null" ) )
			{
				pCurObj = CHE_PDF_Null::Create( GetAllocator() );
			}else if ( mSyntaxParser.IsWord( "endobj" ) )
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

	CHE_PDF_ParseWordDes	wordDes;
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
				HE_ULONG pos = sParser.GetPos();
				sParser.GetWord( wordDes );
				CHE_PDF_ObjectPtr pCurObj;
				if ( wordDes.type == PARSE_WORD_DICT_B )
				{
					sParser.SetPos( pos );
					pCurObj = sParser.GetDictionaryPtr();
				}else if ( wordDes.type == PARSE_WORD_FLOAT )
				{
					pCurObj = CHE_PDF_Number::Create( sParser.GetInteger(), GetAllocator() );
				}else if ( wordDes.type == PARSE_WORD_INTEGER )
				{
					HE_ULONG refNum = sParser.GetInteger();
					HE_ULONG pos = sParser.GetPos();
					sParser.GetWord( wordDes );
					if ( wordDes.type == PARSE_WORD_INTEGER )
					{
						HE_ULONG refGenNum = sParser.GetInteger();
						sParser.GetWord( wordDes );
						if ( sParser.IsWord( "R" ) )
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
					pCurObj = CHE_PDF_String::Create( sParser.GetString(), GetAllocator() );
				}else if ( wordDes.type == PARSE_WORD_NAME )
				{
					pCurObj = CHE_PDF_Name::Create( sParser.GetString(), GetAllocator() );
				}else if ( wordDes.type == PARSE_WORD_ARRAY_B )
				{
					sParser.SetPos( pos );
					pCurObj = sParser.GetArrayPtr();
				}else if ( sParser.IsWord( "false" ) )
				{
					pCurObj = CHE_PDF_Boolean::Create( FALSE, GetAllocator() );
				}else if ( sParser.IsWord( "true" ) )
				{
					pCurObj = CHE_PDF_Boolean::Create( TRUE, GetAllocator() );
				}else if ( sParser.IsWord( "null" ) )
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

	CHE_PDF_ParseWordDes	wordDes;
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
			objNum = sParser.GetInteger();

			sParser.SetCurObjNum( objNum );
			sParser.SetCurGenNum( 0 );

			sParser.GetWord( wordDes );
			if ( wordDes.type == PARSE_WORD_INTEGER )
			{
				offset = sParser.GetInteger();

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
					pCurObj = CHE_PDF_Number::Create( sParser.GetInteger(), GetAllocator() );
				}else if ( wordDes.type == PARSE_WORD_INTEGER )
				{
					HE_INT32 refNum = sParser.GetInteger();
					HE_ULONG pos = sParser.GetPos();
					sParser.GetWord( wordDes );
					if ( wordDes.type == PARSE_WORD_INTEGER )
					{
						HE_INT32 refGenNum = sParser.GetInteger();
						sParser.GetWord( wordDes );
						if ( sParser.IsWord( "R" ) )
						{
							pCurObj = CHE_PDF_Reference::Create( (HE_ULONG)refNum, (HE_ULONG)refGenNum, mpFile, GetAllocator() );
						}else{
							pCurObj = CHE_PDF_Number::Create( refNum, GetAllocator() );
						}
					}else{
						pCurObj = CHE_PDF_Number::Create( (HE_INT32)refNum, GetAllocator() );
					}
					sParser.SetPos( pos );
				}else if ( wordDes.type == PARSE_WORD_STRING )
				{
					pCurObj = CHE_PDF_String::Create( sParser.GetString(), GetAllocator() );
				}else if ( wordDes.type == PARSE_WORD_NAME )
				{
					pCurObj = CHE_PDF_Name::Create( sParser.GetString(), GetAllocator() );
				}else if ( wordDes.type == PARSE_WORD_ARRAY_B )
				{
					sParser.SetPos( pos );
					pCurObj = sParser.GetArrayPtr();
				}else if ( sParser.GetString() == "false" )
				{
					pCurObj = CHE_PDF_Boolean::Create( FALSE, GetAllocator() );
				}else if ( sParser.GetString() == "true" )
				{
					pCurObj = CHE_PDF_Boolean::Create( TRUE, GetAllocator() );
				}else if ( sParser.GetString() == "null" )
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
}