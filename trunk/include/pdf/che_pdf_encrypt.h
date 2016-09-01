#ifndef _CHE_PDF_ENCRYPT_H_
#define _CHE_PDF_ENCRYPT_H_

#include "../che_base.h"
#include "../che_string.h"

#define PDFENCRYPT_KEYLENGTH_40		40
#define PDFENCRYPT_KEYLENGTH_56		56
#define PDFENCRYPT_KEYLENGTH_80		80
#define PDFENCRYPT_KEYLENGTH_96		96
#define PDFENCRYPT_KEYLENGTH_128	128

#define PDFENCRYPT_PERMISSIONS_PRINT		0x00000004
#define PDFENCRYPT_PERMISSIONS_EDIT			0x00000008
#define PDFENCRYPT_PERMISSIONS_COPY			0x00000010
#define PDFENCRYPT_PERMISSIONS_EDITNOTES	0x00000020
#define PDFENCRYPT_PERMISSIONS_FILLANDSIGN	0x00000100
#define PDFENCRYPT_PERMISSIONS_ACCESSIBLE	0x00000200
#define PDFENCRYPT_PERMISSIONS_DOCACCEMBLY	0x00000400
#define PDFENCRYPT_PERMISSIONS_HighPrint	0x00000800

#define PDFENCRYPT_ALGORITHM_RC4	1
#define PDFENCRYPT_ALGORITHM_AESV2	2

class CHE_PDF_Creator;

class CHE_PDF_Encrypt : public CHE_Object
{
public:
	CHE_PDF_Encrypt(	const CHE_ByteString id, BYTE O[32], BYTE U[32], BYTE algorithm, BYTE version,
						BYTE revision, BYTE keyLength, bool bMetaData, size_t pValue, CHE_Allocator * pAllocator = nullptr );

	CHE_PDF_Encrypt(	const CHE_ByteString id, /*const CHE_ByteString userPassword, const CHE_ByteString ownerPassword,*/
						BYTE algorithm, BYTE version, BYTE revision, BYTE keyLength, bool bMetaData, size_t pValue,
						CHE_Allocator * pAllocator = nullptr );

	void Init( const CHE_ByteString userPassword, const CHE_ByteString ownerPassword );

	bool Authenticate( const CHE_ByteString & password );

	bool	IsPasswordOK() { return m_bPasswordOk; }

	size_t Encrypt( CHE_ByteString & str, size_t objNum, size_t genNum );

	size_t Encrypt( PBYTE pData, size_t length, size_t objNum, size_t genNum );
    
	size_t Encrypt( CHE_ByteString & str, BYTE objKey[16], size_t objKeyLen );

	size_t Encrypt( PBYTE pData, size_t length, BYTE objKey[16], size_t objKeyLen );

	size_t Decrypt( CHE_ByteString & str, size_t objNum, size_t genNum );

	size_t Decrypt( PBYTE pData, size_t length, size_t objNum, size_t genNum );

	size_t Decrypt( CHE_ByteString & str, BYTE objKey[16], size_t objKeyLen );
	
	size_t Decrypt( PBYTE pData, size_t length, BYTE objKey[16], size_t objKeyLen );

private:
	void ComputeEncryptionKey( BYTE userPad[32], BYTE encryptionKeyRet[16] );
	
	void ComputeOwnerKey( BYTE userPad[32], BYTE ownerPad[32], BYTE ownerKeyRet[32], bool bAuth = false );
	
	void ComputeUserKey( BYTE encryptionKey[16], BYTE userKeyRet[32] );
	
	void CreateObjKey( size_t objNum, size_t genNum, BYTE objkey[16], size_t* pObjKeyLengthRet );
	
	void PadPassword( const CHE_ByteString & password, BYTE pswd[32] );

	void RC4( PBYTE key, size_t keyLength, PBYTE data, size_t dataLength, PBYTE dataRet );
	
	size_t AESEncrypt( PBYTE key, size_t keyLength, PBYTE data, size_t dataLength, PBYTE dataRet );
	
	size_t AESDecrypt( PBYTE key, size_t keyLength, PBYTE data, size_t dataLength, PBYTE dataRet );

private:
	bool				m_bPasswordOk;

	bool				m_bMetaData;
	BYTE				m_algorithm;
	BYTE				m_version;
	BYTE				m_revision;
	BYTE				m_keyLength;
	size_t			m_PValue;
	BYTE				m_OValue[32];
	BYTE				m_UValue[32];
	CHE_ByteString		m_ID;

	BYTE				m_EncryptionKey[16];

	friend class CHE_PDF_Creator;
};

#endif
