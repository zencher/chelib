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

#define PDFENCRYPT_ALGORITHM_RC4V1	1
#define PDFENCRYPT_ALGORITHM_RC4V2	2
#define PDFENCRYPT_ALGORITHM_AESV2	4

#define _MAX_KEY_COLUMNS (256/32)
#define _MAX_ROUNDS      14
#define MAX_IV_SIZE      16

// Error codes
#define RIJNDAEL_SUCCESS 0
#define RIJNDAEL_UNSUPPORTED_MODE -1
#define RIJNDAEL_UNSUPPORTED_DIRECTION -2
#define RIJNDAEL_UNSUPPORTED_KEY_LENGTH -3
#define RIJNDAEL_BAD_KEY -4
#define RIJNDAEL_NOT_INITIALIZED -5
#define RIJNDAEL_BAD_DIRECTION -6
#define RIJNDAEL_CORRUPTED_DATA -7

class CHE_PDF_Creator;

class  CHE_Rijndael
{	
public:
	enum Direction { Encrypt , Decrypt };
	enum Mode { ECB , CBC , CFB1 };
	enum KeyLength { Key16Bytes , Key24Bytes , Key32Bytes };
	CHE_Rijndael();
	~CHE_Rijndael();
protected:
	enum State { Valid , Invalid };
	State      m_state;
	Mode       m_mode;
	Direction  m_direction;
	HE_BYTE  m_initVector[MAX_IV_SIZE];
	HE_DWORD m_uRounds;
	HE_BYTE  m_expandedKey[_MAX_ROUNDS+1][4][4];
public:
	int init(Mode mode,Direction dir,const HE_BYTE *key,KeyLength keyLen,HE_BYTE * initVector = 0);
	int blockEncrypt(const HE_BYTE *input, int inputLen, HE_BYTE *outBuffer);
	HE_INT32 padEncrypt(const HE_BYTE *input, HE_INT32 inputOctets, HE_BYTE *outBuffer);
	int blockDecrypt(const HE_BYTE *input, int inputLen, HE_BYTE *outBuffer);
	int padDecrypt(const HE_BYTE *input, int inputOctets, HE_BYTE *outBuffer);
protected:
	void keySched(HE_BYTE key[_MAX_KEY_COLUMNS][4]);
	void keyEncToDec();
	void encrypt(const HE_BYTE a[16], HE_BYTE b[16]);
	void decrypt(const HE_BYTE a[16], HE_BYTE b[16]);
};


class CHE_PDF_Encrypt : public CHE_Object
{
public:
	CHE_PDF_Encrypt( const CHE_ByteString id, HE_BYTE O[32], HE_BYTE U[32], HE_BYTE algorithm, HE_BYTE keyLength,
						HE_BYTE revision,  HE_BOOL bMetaData, HE_DWORD pValue, CHE_Allocator * pAllocator = NULL );

	CHE_PDF_Encrypt( const CHE_ByteString id, /*const CHE_ByteString userPassword, const CHE_ByteString ownerPassword,*/
						HE_BYTE algorithm, HE_BYTE keyLength, HE_BYTE revision,  HE_BOOL bMetaData, HE_DWORD pValue,
						CHE_Allocator * pAllocator = NULL );

	HE_VOID Init( const CHE_ByteString userPassword, const CHE_ByteString ownerPassword );

	HE_BOOL Authenticate( const CHE_ByteString & password );

	HE_BOOL	IsPasswordOK() { return m_bPasswordOk; }

	HE_DWORD Encrypt( CHE_ByteString & str, HE_DWORD objNum, HE_DWORD genNum );

	HE_DWORD Encrypt( HE_LPBYTE pData, HE_DWORD length, HE_DWORD objNum, HE_DWORD genNum );
    
	HE_DWORD Encrypt( CHE_ByteString & str, HE_BYTE objKey[16], HE_DWORD objKeyLen );

	HE_DWORD Encrypt( HE_LPBYTE pData, HE_DWORD length, HE_BYTE objKey[16], HE_DWORD objKeyLen );

	HE_DWORD Decrypt( CHE_ByteString & str, HE_DWORD objNum, HE_DWORD genNum );

	HE_DWORD Decrypt( HE_LPBYTE pData, HE_DWORD length, HE_DWORD objNum, HE_DWORD genNum );

	HE_DWORD Decrypt( CHE_ByteString & str, HE_BYTE objKey[16], HE_DWORD objKeyLen );
	
	HE_DWORD Decrypt( HE_LPBYTE pData, HE_DWORD length, HE_BYTE objKey[16], HE_DWORD objKeyLen );

private:
	HE_VOID ComputeEncryptionKey( HE_BYTE userPad[32], HE_BYTE encryptionKeyRet[16] );
	
	HE_VOID ComputeOwnerKey( HE_BYTE userPad[32], HE_BYTE ownerPad[32], HE_BYTE ownerKeyRet[32], HE_BOOL bAuth = FALSE );
	
	HE_VOID ComputeUserKey( HE_BYTE encryptionKey[16], HE_BYTE userKeyRet[32] );
	
	HE_VOID CreateObjKey( HE_DWORD objNum, HE_DWORD genNum, HE_BYTE objkey[16], HE_DWORD* pObjKeyLengthRet );
	
	HE_VOID PadPassword( const CHE_ByteString & password, HE_BYTE pswd[32] );

	HE_VOID RC4( HE_LPBYTE key, HE_DWORD keyLength, HE_LPBYTE data, HE_DWORD dataLength, HE_LPBYTE dataRet );
	
	HE_DWORD AESEncrypt( HE_LPBYTE key, HE_DWORD keyLength, HE_LPBYTE data, HE_DWORD dataLength, HE_LPBYTE dataRet );
	
	HE_DWORD AESDecrypt( HE_LPBYTE key, HE_DWORD keyLength, HE_LPBYTE data, HE_DWORD dataLength, HE_LPBYTE dataRet );

private:
	HE_BOOL				m_bPasswordOk;

	HE_BOOL				m_bMetaData;
	HE_BYTE				m_algorithm;
	HE_BYTE				m_revision;
	HE_BYTE				m_keyLength;
	HE_DWORD			m_PValue;
	HE_BYTE				m_OValue[32];
	HE_BYTE				m_UValue[32];
	CHE_ByteString		m_ID;

	HE_BYTE				m_EncryptionKey[16];

	friend class CHE_PDF_Creator;
};

#endif
