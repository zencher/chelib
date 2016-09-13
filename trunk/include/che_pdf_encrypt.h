#ifndef _CHELIB_PDF_ENCRYPT_H_
#define _CHELIB_PDF_ENCRYPT_H_

#include "che_base_string.h"

namespace chelib {

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

class CPDF_Creator;

class CPDF_Encrypt : public CObject
{
public:
	CPDF_Encrypt( const CByteString id, BYTE O[32], BYTE U[32], BYTE algorithm,
                  BYTE version, BYTE revision, BYTE keyLength, bool bMetaData, size_t pValue,
                  CAllocator * pAllocator = nullptr );

	CPDF_Encrypt( const CByteString id, BYTE algorithm, BYTE version, BYTE revision,
                  BYTE keyLength, bool bMetaData, size_t pValue, CAllocator * pAllocator = nullptr );

	void Init( const CByteString userPassword, const CByteString ownerPassword );

	bool Authenticate( const CByteString & password );

	bool IsPasswordOK() { return m_bPasswordOk; }

	size_t Encrypt( CByteString & str, size_t objNum, size_t genNum );
	size_t Encrypt( PBYTE pData, size_t length, size_t objNum, size_t genNum );
	size_t Encrypt( CByteString & str, BYTE objKey[16], size_t objKeyLen );
	size_t Encrypt( PBYTE pData, size_t length, BYTE objKey[16], size_t objKeyLen );
	size_t Decrypt( CByteString & str, size_t objNum, size_t genNum );
	size_t Decrypt( PBYTE pData, size_t length, size_t objNum, size_t genNum );
	size_t Decrypt( CByteString & str, BYTE objKey[16], size_t objKeyLen );
	size_t Decrypt( PBYTE pData, size_t length, BYTE objKey[16], size_t objKeyLen );

private:
	void ComputeEncryptionKey( BYTE userPad[32], BYTE encryptionKeyRet[16] );
	void ComputeOwnerKey( BYTE userPad[32], BYTE ownerPad[32], BYTE ownerKeyRet[32], bool bAuth = false );
	void ComputeUserKey( BYTE encryptionKey[16], BYTE userKeyRet[32] );
	void CreateObjKey( size_t objNum, size_t genNum, BYTE objkey[16], size_t* pObjKeyLengthRet );
	void PadPassword( const CByteString & password, BYTE pswd[32] );
	void RC4( PBYTE key, size_t keyLength, PBYTE data, size_t dataLength, PBYTE dataRet );
	
	size_t AESEncrypt( PBYTE key, size_t keyLength, PBYTE data, size_t dataLength, PBYTE dataRet );
	size_t AESDecrypt( PBYTE key, size_t keyLength, PBYTE data, size_t dataLength, PBYTE dataRet );

private:
    bool        m_bPasswordOk;
	bool        m_bMetaData;
	BYTE        m_algorithm;
	BYTE        m_version;
	BYTE        m_revision;
	BYTE        m_keyLength;
	size_t      m_PValue;
	BYTE        m_OValue[32];
	BYTE        m_UValue[32];
	BYTE        m_EncryptionKey[16];
    CByteString m_ID;

	friend class CPDF_Creator;
};
    
}//namespace

#endif
