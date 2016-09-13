#ifndef _CHE_CRYPT_AES_H_
#define _CHE_CRYPT_AES_H_

#include "../../include/che_base_object.h"

namespace chelib {

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


class  CCryptRijndael
{	
public:
	enum Direction { Encrypt , Decrypt };
	enum Mode { ECB , CBC , CFB1 };
	enum KeyLength { Key16Bytes , Key24Bytes , Key32Bytes };
	
	CCryptRijndael();
	~CCryptRijndael();

protected:
	enum State { Valid , Invalid };
	State       m_state;
	Mode        m_mode;
	Direction   m_direction;
	BYTE        m_initVector[MAX_IV_SIZE];
	uint32      m_uRounds;
	BYTE        m_expandedKey[_MAX_ROUNDS+1][4][4];

public:
	int32 init( Mode mode, Direction dir, const BYTE *key, KeyLength keyLen, BYTE * initVector = 0 );
	int32 blockEncrypt( const BYTE *input, int32 inputLen, BYTE *outBuffer );
	int32 padEncrypt( const BYTE *input, int32 inputOctets, BYTE *outBuffer );
	int32 blockDecrypt( const BYTE *input, int32 inputLen, BYTE *outBuffer );
	int32 padDecrypt( const BYTE *input, int32 inputOctets, BYTE *outBuffer );

protected:
	void keySched( BYTE key[_MAX_KEY_COLUMNS][4] );
	void keyEncToDec();
	void encrypt( const BYTE a[16], BYTE b[16] );
	void decrypt( const BYTE a[16], BYTE b[16] );
};

}//namespace

#endif