#ifndef _CHE_CRYPT_RC4_H_
#define _CHE_CRYPT_RC4_H_

#include "../../include/che_base_object.h"

namespace chelib {

class CCryptRC4
{
public:
	static void Encrypt( PBYTE key, size_t keyLength, PBYTE data, size_t dataLength, PBYTE dataRet );
};
    
}//namespace

#endif