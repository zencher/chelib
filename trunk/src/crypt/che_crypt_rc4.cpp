#include "../../include/crypt/che_crypt_rc4.h"

void CHE_CRYPT_RC4::Encrypt( PBYTE key, size_t keyLength, PBYTE data, size_t dataLength, PBYTE dataRet )
{
	BYTE rc4[256];
	BYTE t = 0;
	uint32 j = 0;

	for ( size_t i = 0; i < 256; ++i )
	{
		rc4[i] = static_cast<BYTE>(i);
	}

	for ( size_t l = 0; l < 256; ++l )
	{
		t = rc4[l];
		j = (j + static_cast<uint32>(t) + static_cast<uint32>(key[l % keyLength])) % 256;
		rc4[l] = rc4[j];
		rc4[j] = t;
	}
	size_t a = 0;
	size_t b = 0;
	BYTE k = 0;
	for ( size_t m = 0; m < dataLength; ++m )
	{
		a = (a + 1) % 256;
		t = rc4[a];
		b = (b + static_cast<size_t>(t)) % 256;
		rc4[a] = rc4[b];
		rc4[b] = t;
		k = rc4[(static_cast<size_t>(rc4[a]) + static_cast<size_t>(rc4[b])) % 256];
		dataRet[m] = data[m] ^ k;
	}
}