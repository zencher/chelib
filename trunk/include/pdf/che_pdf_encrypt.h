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
	CHE_PDF_Encrypt(	const CHE_ByteString id, HE_BYTE O[32], HE_BYTE U[32], HE_BYTE algorithm, HE_BYTE version,
						HE_BYTE revision, HE_BYTE keyLength, HE_BOOL bMetaData, HE_ULONG pValue, CHE_Allocator * pAllocator = NULL );

	CHE_PDF_Encrypt(	const CHE_ByteString id, /*const CHE_ByteString userPassword, const CHE_ByteString ownerPassword,*/
						HE_BYTE algorithm, HE_BYTE version, HE_BYTE revision, HE_BYTE keyLength, HE_BOOL bMetaData, HE_ULONG pValue,
						CHE_Allocator * pAllocator = NULL );

	HE_VOID Init( const CHE_ByteString userPassword, const CHE_ByteString ownerPassword );

	HE_BOOL Authenticate( const CHE_ByteString & password );

	HE_BOOL	IsPasswordOK() { return m_bPasswordOk; }

	HE_ULONG Encrypt( CHE_ByteString & str, HE_ULONG objNum, HE_ULONG genNum );

	HE_ULONG Encrypt( HE_LPBYTE pData, HE_ULONG length, HE_ULONG objNum, HE_ULONG genNum );
    
	HE_ULONG Encrypt( CHE_ByteString & str, HE_BYTE objKey[16], HE_ULONG objKeyLen );

	HE_ULONG Encrypt( HE_LPBYTE pData, HE_ULONG length, HE_BYTE objKey[16], HE_ULONG objKeyLen );

	HE_ULONG Decrypt( CHE_ByteString & str, HE_ULONG objNum, HE_ULONG genNum );

	HE_ULONG Decrypt( HE_LPBYTE pData, HE_ULONG length, HE_ULONG objNum, HE_ULONG genNum );

	HE_ULONG Decrypt( CHE_ByteString & str, HE_BYTE objKey[16], HE_ULONG objKeyLen );
	
	HE_ULONG Decrypt( HE_LPBYTE pData, HE_ULONG length, HE_BYTE objKey[16], HE_ULONG objKeyLen );

private:
	HE_VOID ComputeEncryptionKey( HE_BYTE userPad[32], HE_BYTE encryptionKeyRet[16] );
	
	HE_VOID ComputeOwnerKey( HE_BYTE userPad[32], HE_BYTE ownerPad[32], HE_BYTE ownerKeyRet[32], HE_BOOL bAuth = FALSE );
	
	HE_VOID ComputeUserKey( HE_BYTE encryptionKey[16], HE_BYTE userKeyRet[32] );
	
	HE_VOID CreateObjKey( HE_ULONG objNum, HE_ULONG genNum, HE_BYTE objkey[16], HE_ULONG* pObjKeyLengthRet );
	
	HE_VOID PadPassword( const CHE_ByteString & password, HE_BYTE pswd[32] );

	HE_VOID RC4( HE_LPBYTE key, HE_ULONG keyLength, HE_LPBYTE data, HE_ULONG dataLength, HE_LPBYTE dataRet );
	
	HE_ULONG AESEncrypt( HE_LPBYTE key, HE_ULONG keyLength, HE_LPBYTE data, HE_ULONG dataLength, HE_LPBYTE dataRet );
	
	HE_ULONG AESDecrypt( HE_LPBYTE key, HE_ULONG keyLength, HE_LPBYTE data, HE_ULONG dataLength, HE_LPBYTE dataRet );

private:
	HE_BOOL				m_bPasswordOk;

	HE_BOOL				m_bMetaData;
	HE_BYTE				m_algorithm;
	HE_BYTE				m_version;
	HE_BYTE				m_revision;
	HE_BYTE				m_keyLength;
	HE_ULONG			m_PValue;
	HE_BYTE				m_OValue[32];
	HE_BYTE				m_UValue[32];
	CHE_ByteString		m_ID;

	HE_BYTE				m_EncryptionKey[16];

	friend class CHE_PDF_Creator;
};

#endif
