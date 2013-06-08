#ifndef _CHE_CRYPT_RC4_H_
#define _CHE_CRYPT_RC4_H_

#include "../../include/che_base.h"

class CHE_CRYPT_RC4
{
public:
	static HE_VOID Encrypt( HE_LPBYTE key, HE_ULONG keyLength, HE_LPBYTE data, HE_ULONG dataLength, HE_LPBYTE dataRet );
};

#endif