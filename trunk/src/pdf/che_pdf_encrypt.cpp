#include "../../include/pdf/che_pdf_encrypt.h"

#include "../../include/hash/che_hash_md5.h"
#include "../../include/crypt/che_crypt_rc4.h"
#include "../../include/crypt/che_crypt_aes.h"

#include <cstdlib>
#include <cstdio>
#include <cstring>

static unsigned char padding[] ="\x28\xBF\x4E\x5E\x4E\x75\x8A\x41\x64\x00\x4E\x56\xFF\xFA\x01\x08\x2E\x2E\x00\xB6\xD0\x68\x3E\x80\x2F\x0C\xA9\xFE\x64\x53\x69\x7A";

CHE_PDF_Encrypt::CHE_PDF_Encrypt(	const CHE_ByteString id, HE_BYTE O[32], HE_BYTE U[32], HE_BYTE algorithm,
									HE_BYTE version, HE_BYTE revision, HE_BYTE keyLength, HE_BOOL bMetaData, HE_ULONG pValue,
									CHE_Allocator * pAllocator ) : CHE_Object( pAllocator ), m_ID( pAllocator )
{
	m_ID = id;
	for ( HE_ULONG i = 0; i < 32; i++ )
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
	m_bPasswordOk = FALSE;
}

CHE_PDF_Encrypt::CHE_PDF_Encrypt(	const CHE_ByteString id, /*const CHE_ByteString userPassword, const CHE_ByteString ownerPassword,*/
									HE_BYTE algorithm, HE_BYTE version, HE_BYTE revision, HE_BYTE keyLength,  HE_BOOL bMetaData, HE_ULONG pValue,
									CHE_Allocator * pAllocator )
								 : CHE_Object( pAllocator ), m_ID( pAllocator )
{
	m_ID = id;
	m_algorithm = algorithm;
	m_version = version;
	m_revision = revision;
	m_keyLength = keyLength;
	m_bMetaData = bMetaData;
	m_PValue = pValue;
	m_bPasswordOk = FALSE;
}

HE_VOID CHE_PDF_Encrypt::Init( const CHE_ByteString userPassword, const CHE_ByteString ownerPassword )
{
	HE_BYTE userPad[32];
	HE_BYTE ownerPad[32];
	HE_BYTE encryptionKey[16];
	PadPassword( userPassword, userPad );
	PadPassword( ownerPassword, ownerPad );
	if ( ownerPassword.GetLength() == 0 )
	{
		ComputeOwnerKey( userPad, userPad, m_OValue, FALSE );
	}else{
		ComputeOwnerKey( userPad, ownerPad, m_OValue, FALSE );
	}
	ComputeEncryptionKey( userPad, encryptionKey );
	ComputeUserKey( encryptionKey, m_UValue );
	m_bPasswordOk = TRUE;
}

void CHE_PDF_Encrypt::PadPassword( const CHE_ByteString & password, unsigned char pswd[32] )
{
	HE_ULONG m = password.GetLength();

	if (m > 32) m = 32;
		
	HE_ULONG j = 0, p = 0;
	for ( j = 0; j < m; j++ )
	{
		pswd[p++] = password[j];
	}
	for (j = 0; p < 32 && j < 32; j++)
	{
		pswd[p++] = padding[j];
	}
}

HE_VOID CHE_PDF_Encrypt::ComputeOwnerKey( HE_BYTE userPad[32], HE_BYTE ownerPad[32], HE_BYTE ownerKeyRet[32], HE_BOOL bAuth )
{
	HE_BYTE mkey[16];
	HE_BYTE digest[16];
	HE_UINT32 lengthInByte = m_keyLength / 8;

	CHE_HASH_MD5_Content md5;
	md5.Init();
	md5.Update( ownerPad, 32 );
	md5.Final( digest );
	
	if ( (m_revision == 3) || (m_revision == 4) )
	{
		for ( HE_ULONG i = 0; i < 50; i++ )
		{
			md5.Init();
			md5.Update( digest, lengthInByte );
			md5.Final( digest );
		}
		memcpy( ownerKeyRet, userPad, 32 );
		
		for ( HE_ULONG j = 0; j < 20; j++ )
		{
			for ( HE_ULONG k = 0; k < lengthInByte ; k++ )
			{
				if ( bAuth )
				{
					mkey[k] = (HE_BYTE)( digest[k] ^ (19-j) );
				}else
				{
					mkey[k] = (HE_BYTE)( digest[k] ^ j );
				}
			}
			RC4( mkey, lengthInByte, ownerKeyRet, 32, ownerKeyRet );
		}
	}else{
		RC4( digest, 5, userPad, 32, ownerKeyRet );
	}
}

HE_VOID CHE_PDF_Encrypt::CreateObjKey( HE_ULONG objNum, HE_ULONG genNum, HE_BYTE objkey[16], HE_ULONG* pObjKeyLengthRet )
{
	HE_UINT32 keyLengthInByte = m_keyLength / 8;
	HE_UINT32 objKeyLength = keyLengthInByte + 5;
	HE_BYTE	tmpkey[16+5+4];

	for ( HE_ULONG j = 0; j < keyLengthInByte; j++)
	{
		tmpkey[j] = m_EncryptionKey[j];
	}
	tmpkey[keyLengthInByte+0] = (HE_BYTE)( 0xff &  objNum );
	tmpkey[keyLengthInByte+1] = (HE_BYTE)( 0xff & (objNum >> 8) );
	tmpkey[keyLengthInByte+2] = (HE_BYTE)( 0xff & (objNum >> 16) );
	tmpkey[keyLengthInByte+3] = (HE_BYTE)( 0xff &  genNum );
	tmpkey[keyLengthInByte+4] = (HE_BYTE)( 0xff & (genNum >> 8) );

	if ( m_algorithm == PDFENCRYPT_ALGORITHM_AESV2 )
	{
		objKeyLength += 4;
		tmpkey[keyLengthInByte+5] = 0x73;
		tmpkey[keyLengthInByte+6] = 0x41;
		tmpkey[keyLengthInByte+7] = 0x6c;
		tmpkey[keyLengthInByte+8] = 0x54;
	}
	
	CHE_HASH_MD5_Content md5;
	md5.Init();
	md5.Update( tmpkey, objKeyLength );
	md5.Final( objkey );
	
	*pObjKeyLengthRet = ( keyLengthInByte < 11 ) ? keyLengthInByte + 5 : 16;  
}

HE_BOOL CHE_PDF_Encrypt::Authenticate( const CHE_ByteString & password )
{
    HE_BOOL bRet = TRUE;
    HE_BYTE padpswd[32];
	HE_BYTE userKey[32];
	HE_BYTE encrypt[16];

    PadPassword( password, padpswd );
    ComputeEncryptionKey( padpswd, encrypt );
	ComputeUserKey( encrypt, userKey );

	HE_ULONG kmax = ( m_revision == 2 ) ? 32 : 16;
	for ( HE_ULONG k = 0; bRet && k < kmax; k++)
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
		for ( HE_ULONG k = 0; bRet && k < kmax; k++)
		{
			bRet = bRet && ( userKey[k] == m_UValue[k] );
		}
    }
	if ( bRet == TRUE )
	{
		m_bPasswordOk = TRUE;
		for ( HE_ULONG i = 0; i < 16; i++ )
		{
			m_EncryptionKey[i] = encrypt[i];
		}
	}
    return bRet;
}

HE_VOID CHE_PDF_Encrypt::ComputeEncryptionKey( HE_BYTE userPad[32], HE_BYTE encryptionKeyRet[16] )
{
	HE_ULONG keyLengthInByte = m_keyLength / 8;

	CHE_HASH_MD5_Content md5;
	md5.Init();
	md5.Update( userPad, 32 );
	md5.Update( m_OValue, 32 );
	
	HE_BYTE ext[4];
	ext[0] = (HE_BYTE)(  m_PValue        & 0xff );
	ext[1] = (HE_BYTE)( (m_PValue >>  8) & 0xff );
	ext[2] = (HE_BYTE)( (m_PValue >> 16) & 0xff );
	ext[3] = (HE_BYTE)( (m_PValue >> 24) & 0xff );
	
	md5.Update( ext, 4 );
	
	HE_BYTE * docId = NULL;
	if ( m_ID.GetLength() > 0 )
	{
		docId = GetAllocator()->NewArray<HE_BYTE>( m_ID.GetLength() );
		HE_ULONG j;
		for ( j = 0; j < m_ID.GetLength(); j++ )
		{
			docId[j] = static_cast<unsigned char>( m_ID[j] );
		}
		md5.Update( docId, m_ID.GetLength() );
	}

	if ( m_bMetaData == FALSE && m_revision >= 4 )
	{
		HE_BYTE ext[4];
		ext[0] = 0xFF;
		ext[1] = 0xFF;
		ext[2] = 0xFF;
		ext[3] = 0xFF;
		md5.Update( ext, 4 );
	}
	md5.Final( encryptionKeyRet );

	if ( m_revision >= 3 )
	{
		for ( HE_ULONG k = 0; k < 50; k++ )
		{
			md5.Init();
			md5.Update( encryptionKeyRet, keyLengthInByte );
			md5.Final( encryptionKeyRet );
		}
	}
}

HE_VOID CHE_PDF_Encrypt::ComputeUserKey( HE_BYTE encryptionKey[16], HE_BYTE userKeyRet[32] )
{
	HE_ULONG keyLengthInByte = m_keyLength / 8;

	if ( m_revision >= 3 )
	{
		CHE_HASH_MD5_Content md5;

		md5.Init();
		md5.Update( padding, 32 );

		if ( m_ID.GetLength() > 0 )
		{
			HE_BYTE * docId = GetAllocator()->NewArray<HE_BYTE>( m_ID.GetLength() );
			for ( HE_ULONG j = 0; j < m_ID.GetLength(); j++ )
			{
				docId[j] = static_cast<unsigned char>( m_ID[j] );
			}
			md5.Update( docId, m_ID.GetLength() );
			GetAllocator()->DeleteArray<HE_BYTE>( docId );
		}

		HE_BYTE digest[16];
		md5.Final( digest );
		HE_ULONG k;
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
			for ( HE_ULONG j = 0; j < keyLengthInByte; j++ )
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

HE_ULONG CHE_PDF_Encrypt::Encrypt( CHE_ByteString & str, HE_ULONG objNum, HE_ULONG genNum )
{
	HE_BYTE objKey[16];
	HE_ULONG objKeyLength = 0;
	CreateObjKey( objNum, genNum, objKey, &objKeyLength );
	return Encrypt( str, objKey, objKeyLength );
}

HE_ULONG CHE_PDF_Encrypt::Encrypt( HE_LPBYTE pData, HE_ULONG length, HE_ULONG objNum, HE_ULONG genNum )
{
	HE_BYTE objKey[16];
	HE_ULONG objKeyLength = 0;
	CreateObjKey( objNum, genNum, objKey, &objKeyLength );
	return Encrypt( pData, length, objKey, objKeyLength );
}

HE_ULONG CHE_PDF_Encrypt::Encrypt( CHE_ByteString & str, HE_BYTE objKey[16], HE_ULONG objKeyLen )
{
	HE_ULONG length = str.GetLength();
	HE_BYTE * pData = GetAllocator()->NewArray<HE_BYTE>(length+16);
	for ( HE_ULONG i = 0; i < length; i++ )
	{
		pData[i] = (HE_BYTE)( str[i] );
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
	GetAllocator()->DeleteArray<HE_BYTE>( pData );
	return str.GetLength();
}

HE_ULONG CHE_PDF_Encrypt::Encrypt( HE_LPBYTE pData, HE_ULONG length, HE_BYTE objKey[16], HE_ULONG objKeyLen )
{
	if ( pData == NULL || length == 0 )
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

HE_ULONG CHE_PDF_Encrypt::Decrypt( CHE_ByteString & str, HE_ULONG objNum, HE_ULONG genNum )
{
	HE_BYTE objKey[16];
	HE_ULONG objKeyLength = 0;
	CreateObjKey( objNum, genNum, objKey, &objKeyLength );
	return Decrypt( str, objKey, objKeyLength );
}

HE_ULONG CHE_PDF_Encrypt::Decrypt( HE_LPBYTE pData, HE_ULONG length, HE_ULONG objNum, HE_ULONG genNum )
{
	HE_BYTE objKey[16];
	HE_ULONG objKeyLength = 0;
	CreateObjKey( objNum, genNum, objKey, &objKeyLength );
	return Decrypt( pData, length, objKey, objKeyLength );
}

HE_ULONG CHE_PDF_Encrypt::Decrypt( CHE_ByteString & str, HE_BYTE objKey[16], HE_ULONG objKeyLen )
{
	HE_ULONG length = str.GetLength();
	HE_BYTE * pData = GetAllocator()->NewArray<HE_BYTE>(length);
	for ( HE_ULONG i = 0; i < length; i++ )
	{
		pData[i] = (HE_BYTE)( str[i] );
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
	GetAllocator()->DeleteArray<HE_BYTE>(pData);
	return length;
}

HE_ULONG CHE_PDF_Encrypt::Decrypt( HE_LPBYTE pData, HE_ULONG length, HE_BYTE objKey[16], HE_ULONG objKeyLen )
{
	if ( pData == NULL || length == 0 )
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

HE_VOID CHE_PDF_Encrypt::RC4( HE_LPBYTE key, HE_ULONG keyLength, HE_LPBYTE data, HE_ULONG dataLength, HE_LPBYTE dataRet )
{
	CHE_CRYPT_RC4::Encrypt( key, keyLength, data, dataLength, dataRet );
}

HE_ULONG CHE_PDF_Encrypt::AESEncrypt( HE_LPBYTE key, HE_ULONG keyLength, HE_LPBYTE data, HE_ULONG dataLength, HE_LPBYTE dataRet )
{
	CHE_CRYPT_Rijndael aes;
	for ( HE_ULONG i = 0; i < 16; i++ )
	{
		dataRet[i] = key[i];
	}
	aes.init( CHE_CRYPT_Rijndael::CBC, CHE_CRYPT_Rijndael::Encrypt, key, CHE_CRYPT_Rijndael::Key16Bytes, dataRet );
	HE_INT32 len = aes.padEncrypt( data, dataLength, &dataRet[16] );
	if ( len < 0 )
	{
		return 0;
	}else{
		return len;
	}
}

HE_ULONG CHE_PDF_Encrypt::AESDecrypt( HE_LPBYTE key, HE_ULONG keyLength, HE_LPBYTE data, HE_ULONG dataLength, HE_LPBYTE dataRet )
{
	CHE_CRYPT_Rijndael aes;
	HE_BYTE vector[16];
	for ( HE_ULONG i = 0; i < 16; i++ )
	{
		vector[i] = data[i];
	}
	aes.init( CHE_CRYPT_Rijndael::CBC, CHE_CRYPT_Rijndael::Decrypt, key, CHE_CRYPT_Rijndael::Key16Bytes, vector );
	HE_INT32 len = aes.padDecrypt( &data[16], dataLength-16, dataRet );
	if ( len < 0 )
	{
		return 0;
	}else{
		return len;
	}
}
