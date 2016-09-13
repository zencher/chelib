#include "../../include/pdf/che_pdf_encrypt.h"

#include "../../include/hash/che_hash_md5.h"
#include "../../include/crypt/che_crypt_rc4.h"
#include "../../include/crypt/che_crypt_aes.h"

#include <cstdlib>
#include <cstdio>
#include <cstring>

namespace chelib {

static unsigned char padding[] ="\x28\xBF\x4E\x5E\x4E\x75\x8A\x41\x64\x00\x4E\x56\xFF\xFA\x01\x08\x2E\x2E\x00\xB6\xD0\x68\x3E\x80\x2F\x0C\xA9\xFE\x64\x53\x69\x7A";

CPDF_Encrypt::CPDF_Encrypt(	const CByteString id, BYTE O[32], BYTE U[32], BYTE algorithm,
									BYTE version, BYTE revision, BYTE keyLength, bool bMetaData, size_t pValue,
									CAllocator * pAllocator ) : CObject( pAllocator ), m_ID( pAllocator )
{
	m_ID = id;
	for ( size_t i = 0; i < 32; i++ )
	{
		m_OValue[i] = O[i];
		m_UValue[i] = U[i];
	}
	m_algorithm = algorithm;
	m_version = version;
	m_revision = revision;
	m_keyLength = keyLength;
	m_bMetaData = bMetaData;
	m_PValue = pValue;
	m_bPasswordOk = false;
}

CPDF_Encrypt::CPDF_Encrypt(	const CByteString id, /*const CByteString userPassword, const CByteString ownerPassword,*/
									BYTE algorithm, BYTE version, BYTE revision, BYTE keyLength,  bool bMetaData, size_t pValue,
									CAllocator * pAllocator )
								 : CObject( pAllocator ), m_ID( pAllocator )
{
	m_ID = id;
	m_algorithm = algorithm;
	m_version = version;
	m_revision = revision;
	m_keyLength = keyLength;
	m_bMetaData = bMetaData;
	m_PValue = pValue;
	m_bPasswordOk = false;
}

void CPDF_Encrypt::Init( const CByteString userPassword, const CByteString ownerPassword )
{
	BYTE userPad[32];
	BYTE ownerPad[32];
	BYTE encryptionKey[16];
	PadPassword( userPassword, userPad );
	PadPassword( ownerPassword, ownerPad );
	if ( ownerPassword.GetLength() == 0 )
	{
		ComputeOwnerKey( userPad, userPad, m_OValue, false );
	}else{
		ComputeOwnerKey( userPad, ownerPad, m_OValue, false );
	}
	ComputeEncryptionKey( userPad, encryptionKey );
	ComputeUserKey( encryptionKey, m_UValue );
	m_bPasswordOk = TRUE;
}

void CPDF_Encrypt::PadPassword( const CByteString & password, unsigned char pswd[32] )
{
	size_t m = password.GetLength();

	if (m > 32) m = 32;
		
	size_t j = 0, p = 0;
	for ( j = 0; j < m; j++ )
	{
		pswd[p++] = password[j];
	}
	for (j = 0; p < 32 && j < 32; j++)
	{
		pswd[p++] = padding[j];
	}
}

void CPDF_Encrypt::ComputeOwnerKey( BYTE userPad[32], BYTE ownerPad[32], BYTE ownerKeyRet[32], bool bAuth )
{
	BYTE mkey[16];
	BYTE digest[16];
	uint32 lengthInByte = m_keyLength / 8;

	CHashMD5 md5;
	md5.Init();
	md5.Update( ownerPad, 32 );
	md5.Final( digest );
	
	if ( (m_revision == 3) || (m_revision == 4) )
	{
		for ( size_t i = 0; i < 50; i++ )
		{
			md5.Init();
			md5.Update( digest, lengthInByte );
			md5.Final( digest );
		}
		memcpy( ownerKeyRet, userPad, 32 );
		
		for ( size_t j = 0; j < 20; j++ )
		{
			for ( size_t k = 0; k < lengthInByte ; k++ )
			{
				if ( bAuth )
				{
					mkey[k] = (BYTE)( digest[k] ^ (19-j) );
				}else
				{
					mkey[k] = (BYTE)( digest[k] ^ j );
				}
			}
			RC4( mkey, lengthInByte, ownerKeyRet, 32, ownerKeyRet );
		}
	}else{
		RC4( digest, 5, userPad, 32, ownerKeyRet );
	}
}

void CPDF_Encrypt::CreateObjKey( size_t objNum, size_t genNum, BYTE objkey[16], size_t* pObjKeyLengthRet )
{
	uint32 keyLengthInByte = m_keyLength / 8;
	uint32 objKeyLength = keyLengthInByte + 5;
	BYTE	tmpkey[16+5+4];

	for ( size_t j = 0; j < keyLengthInByte; j++)
	{
		tmpkey[j] = m_EncryptionKey[j];
	}
	tmpkey[keyLengthInByte+0] = (BYTE)( 0xff &  objNum );
	tmpkey[keyLengthInByte+1] = (BYTE)( 0xff & (objNum >> 8) );
	tmpkey[keyLengthInByte+2] = (BYTE)( 0xff & (objNum >> 16) );
	tmpkey[keyLengthInByte+3] = (BYTE)( 0xff &  genNum );
	tmpkey[keyLengthInByte+4] = (BYTE)( 0xff & (genNum >> 8) );

	if ( m_algorithm == PDFENCRYPT_ALGORITHM_AESV2 )
	{
		objKeyLength += 4;
		tmpkey[keyLengthInByte+5] = 0x73;
		tmpkey[keyLengthInByte+6] = 0x41;
		tmpkey[keyLengthInByte+7] = 0x6c;
		tmpkey[keyLengthInByte+8] = 0x54;
	}
	
	CHashMD5 md5;
	md5.Init();
	md5.Update( tmpkey, objKeyLength );
	md5.Final( objkey );
	
	*pObjKeyLengthRet = ( keyLengthInByte < 11 ) ? keyLengthInByte + 5 : 16;  
}

bool CPDF_Encrypt::Authenticate( const CByteString & password )
{
    bool bRet = TRUE;
    BYTE padpswd[32];
	BYTE userKey[32];
	BYTE encrypt[16];

    PadPassword( password, padpswd );
    ComputeEncryptionKey( padpswd, encrypt );
	ComputeUserKey( encrypt, userKey );

	size_t kmax = ( m_revision == 2 ) ? 32 : 16;
	for ( size_t k = 0; bRet && k < kmax; k++)
	{
		bRet = bRet && ( userKey[k] == m_UValue[k] );
	}
    if ( !bRet )
    {
		bRet = TRUE;
        unsigned char userpswd[32];
        ComputeOwnerKey( m_OValue, padpswd, userpswd, TRUE );
        ComputeEncryptionKey( userpswd, encrypt );
		ComputeUserKey( encrypt, userKey );
		kmax = 32;
		for ( size_t k = 0; bRet && k < kmax; k++)
		{
			bRet = bRet && ( userKey[k] == m_UValue[k] );
		}
    }
	if ( bRet == TRUE )
	{
		m_bPasswordOk = TRUE;
		for ( size_t i = 0; i < 16; i++ )
		{
			m_EncryptionKey[i] = encrypt[i];
		}
	}
    return bRet;
}

void CPDF_Encrypt::ComputeEncryptionKey( BYTE userPad[32], BYTE encryptionKeyRet[16] )
{
	size_t keyLengthInByte = m_keyLength / 8;

	CHashMD5 md5;
	md5.Init();
	md5.Update( userPad, 32 );
	md5.Update( m_OValue, 32 );
	
	BYTE ext[4];
	ext[0] = (BYTE)(  m_PValue        & 0xff );
	ext[1] = (BYTE)( (m_PValue >>  8) & 0xff );
	ext[2] = (BYTE)( (m_PValue >> 16) & 0xff );
	ext[3] = (BYTE)( (m_PValue >> 24) & 0xff );
	
	md5.Update( ext, 4 );
	
	BYTE * docId = nullptr;
	if ( m_ID.GetLength() > 0 )
	{
		docId = GetAllocator()->NewArray<BYTE>( m_ID.GetLength() );
		size_t j;
		for ( j = 0; j < m_ID.GetLength(); j++ )
		{
			docId[j] = static_cast<unsigned char>( m_ID[j] );
		}
		md5.Update( docId, (int32)m_ID.GetLength() );
	}

	if ( m_bMetaData == false && m_revision >= 4 )
	{
		BYTE ext[4];
		ext[0] = 0xFF;
		ext[1] = 0xFF;
		ext[2] = 0xFF;
		ext[3] = 0xFF;
		md5.Update( ext, 4 );
	}
	md5.Final( encryptionKeyRet );

	if ( m_revision >= 3 )
	{
		for ( size_t k = 0; k < 50; k++ )
		{
			md5.Init();
			md5.Update( encryptionKeyRet, (int32)keyLengthInByte );
			md5.Final( encryptionKeyRet );
		}
	}
}

void CPDF_Encrypt::ComputeUserKey( BYTE encryptionKey[16], BYTE userKeyRet[32] )
{
	size_t keyLengthInByte = m_keyLength / 8;

	if ( m_revision >= 3 )
	{
		CHashMD5 md5;
		md5.Init();
		md5.Update( padding, 32 );

		if ( m_ID.GetLength() > 0 )
		{
			BYTE * docId = GetAllocator()->NewArray<BYTE>( m_ID.GetLength() );
			for ( size_t j = 0; j < m_ID.GetLength(); j++ )
			{
				docId[j] = static_cast<unsigned char>( m_ID[j] );
			}
			md5.Update( docId, (int32)m_ID.GetLength() );
			GetAllocator()->DeleteArray<BYTE>( docId );
		}

		BYTE digest[16];
		md5.Final( digest );
		size_t k;
		for ( k = 0; k < 16; k++ )
		{
			userKeyRet[k] = digest[k];
		}
		for ( k = 16; k < 32; k++ )
		{
			userKeyRet[k] = 0;
		}
		for ( k = 0; k < 20; k++ )
		{
			for ( size_t j = 0; j < keyLengthInByte; j++ )
			{
				digest[j] = static_cast<unsigned char>( encryptionKey[j] ^ k );
			}
			RC4( digest, keyLengthInByte, userKeyRet, 16, userKeyRet );
		}
	}
	else
	{
		RC4( encryptionKey, keyLengthInByte, padding, 32, userKeyRet );
	}
}

size_t CPDF_Encrypt::Encrypt( CByteString & str, size_t objNum, size_t genNum )
{
	BYTE objKey[16];
	size_t objKeyLength = 0;
	CreateObjKey( objNum, genNum, objKey, &objKeyLength );
	return Encrypt( str, objKey, objKeyLength );
}

size_t CPDF_Encrypt::Encrypt( PBYTE pData, size_t length, size_t objNum, size_t genNum )
{
	BYTE objKey[16];
	size_t objKeyLength = 0;
	CreateObjKey( objNum, genNum, objKey, &objKeyLength );
	return Encrypt( pData, length, objKey, objKeyLength );
}

size_t CPDF_Encrypt::Encrypt( CByteString & str, BYTE objKey[16], size_t objKeyLen )
{
	size_t length = str.GetLength();
	BYTE * pData = GetAllocator()->NewArray<BYTE>(length+16);
	for ( size_t i = 0; i < length; i++ )
	{
		pData[i] = (BYTE)( str[i] );
	}
	if ( m_algorithm == PDFENCRYPT_ALGORITHM_RC4 )
	{
		RC4( objKey, objKeyLen, pData, length, pData );
		str.SetData( pData, length );
	}else if ( m_algorithm == PDFENCRYPT_ALGORITHM_AESV2 )
	{
		AESEncrypt( objKey, objKeyLen, pData, length, pData );
		str.SetData( pData, length+16 );
	}
	GetAllocator()->DeleteArray<BYTE>( pData );
	return str.GetLength();
}

size_t CPDF_Encrypt::Encrypt( PBYTE pData, size_t length, BYTE objKey[16], size_t objKeyLen )
{
	if ( pData == nullptr || length == 0 )
	{
		return 0;
	}
	if ( m_algorithm == PDFENCRYPT_ALGORITHM_RC4 )
	{
		RC4( objKey, objKeyLen, pData, length, pData );
	}else if ( m_algorithm == PDFENCRYPT_ALGORITHM_AESV2 )
	{
		length = AESEncrypt( objKey, objKeyLen, pData, length, pData );
	}
	return length;
}

size_t CPDF_Encrypt::Decrypt( CByteString & str, size_t objNum, size_t genNum )
{
	BYTE objKey[16];
	size_t objKeyLength = 0;
	CreateObjKey( objNum, genNum, objKey, &objKeyLength );
	return Decrypt( str, objKey, objKeyLength );
}

size_t CPDF_Encrypt::Decrypt( PBYTE pData, size_t length, size_t objNum, size_t genNum )
{
	BYTE objKey[16];
	size_t objKeyLength = 0;
	CreateObjKey( objNum, genNum, objKey, &objKeyLength );
	return Decrypt( pData, length, objKey, objKeyLength );
}

size_t CPDF_Encrypt::Decrypt( CByteString & str, BYTE objKey[16], size_t objKeyLen )
{
	size_t length = str.GetLength();
	BYTE * pData = GetAllocator()->NewArray<BYTE>(length);
	for ( size_t i = 0; i < length; i++ )
	{
		pData[i] = (BYTE)( str[i] );
	}
	if ( m_algorithm == PDFENCRYPT_ALGORITHM_RC4 )
	{
		RC4( objKey, objKeyLen, pData, length, pData );
		str.SetData( pData, length );
	}else if ( m_algorithm == PDFENCRYPT_ALGORITHM_AESV2 )
	{
		length = AESDecrypt( objKey, objKeyLen, pData, length, pData );
		if( length > 0 )
		{
			str.SetData( pData, length );
		}
	}
	GetAllocator()->DeleteArray<BYTE>(pData);
	return length;
}

size_t CPDF_Encrypt::Decrypt( PBYTE pData, size_t length, BYTE objKey[16], size_t objKeyLen )
{
	if ( pData == nullptr || length == 0 )
	{
		return 0;
	}
	if ( m_algorithm == PDFENCRYPT_ALGORITHM_RC4 )
	{
		RC4( objKey, objKeyLen, pData, length, pData );
	}else if ( m_algorithm == PDFENCRYPT_ALGORITHM_AESV2 )
	{
		length = AESDecrypt( objKey, objKeyLen, pData, length, pData );
	}
	return length;
}

void CPDF_Encrypt::RC4( PBYTE key, size_t keyLength, PBYTE data, size_t dataLength, PBYTE dataRet )
{
	CCryptRC4::Encrypt( key, keyLength, data, dataLength, dataRet );
}

size_t CPDF_Encrypt::AESEncrypt( PBYTE key, size_t keyLength, PBYTE data, size_t dataLength, PBYTE dataRet )
{
	CCryptRijndael aes;
	for ( size_t i = 0; i < 16; i++ )
	{
		dataRet[i] = key[i];
	}
	aes.init( CCryptRijndael::CBC, CCryptRijndael::Encrypt, key, CCryptRijndael::Key16Bytes, dataRet );
	int32 len = aes.padEncrypt( data, (int32)dataLength, &dataRet[16] );
	if ( len < 0 )
	{
		return 0;
	}else{
		return len;
	}
}

size_t CPDF_Encrypt::AESDecrypt( PBYTE key, size_t keyLength, PBYTE data, size_t dataLength, PBYTE dataRet )
{
	CCryptRijndael aes;
	BYTE vector[16];
	for ( size_t i = 0; i < 16; i++ )
	{
		vector[i] = data[i];
	}
	aes.init( CCryptRijndael::CBC, CCryptRijndael::Decrypt, key, CCryptRijndael::Key16Bytes, vector );
	int32 len = aes.padDecrypt( &data[16], (int32)(dataLength-16), dataRet );
	if ( len < 0 )
	{
		return 0;
	}else{
		return len;
	}
}
    
}//namespace