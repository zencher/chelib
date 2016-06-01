#ifndef _CHE_CRYPT_AES_H_
#define _CHE_CRYPT_AES_H_

#include "../../include/che_base.h"

// Error codes
#define RIJNDAEL_SUCCESS 0
#define RIJNDAEL_UNSUPPORTED_MODE -1
#define RIJNDAEL_UNSUPPORTED_DIRECTION -2
#define RIJNDAEL_UNSUPPORTED_KEY_LENGTH -3
#define RIJNDAEL_BAD_KEY -4
#define RIJNDAEL_NOT_INITIALIZED -5
#define RIJNDAEL_BAD_DIRECTION -6
#define RIJNDAEL_CORRUPTED_DATA -7


#define _MAX_KEY_COLUMNS (256/32)
#define _MAX_ROUNDS      14
#define MAX_IV_SIZE      16


class  CHE_CRYPT_Rijndael
{	
public:
	enum Direction { Encrypt , Decrypt };
	enum Mode { ECB , CBC , CFB1 };
	enum KeyLength { Key16Bytes , Key24Bytes , Key32Bytes };
	
	CHE_CRYPT_Rijndael();
	~CHE_CRYPT_Rijndael();

protected:
	enum State { Valid , Invalid };
	State      m_state;
	Mode       m_mode;
	Direction  m_direction;
	HE_BYTE  m_initVector[MAX_IV_SIZE];
	HE_UINT32 m_uRounds;
	HE_BYTE  m_expandedKey[_MAX_ROUNDS+1][4][4];

public:
	HE_INT32 init( Mode mode, Direction dir, const HE_BYTE *key, KeyLength keyLen, HE_BYTE * initVector = 0 );
	HE_INT32 blockEncrypt( const HE_BYTE *input, HE_INT32 inputLen, HE_BYTE *outBuffer );
	HE_INT32 padEncrypt( const HE_BYTE *input, HE_INT32 inputOctets, HE_BYTE *outBuffer );
	HE_INT32 blockDecrypt( const HE_BYTE *input, HE_INT32 inputLen, HE_BYTE *outBuffer );
	HE_INT32 padDecrypt( const HE_BYTE *input, HE_INT32 inputOctets, HE_BYTE *outBuffer );

protected:
	HE_VOID keySched( HE_BYTE key[_MAX_KEY_COLUMNS][4] );
	HE_VOID keyEncToDec();
	HE_VOID encrypt( const HE_BYTE a[16], HE_BYTE b[16] );
	HE_VOID decrypt( const HE_BYTE a[16], HE_BYTE b[16] );
};

#endif