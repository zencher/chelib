#include "../../include/crypt/che_crypt_rc4.h"

HE_VOID CHE_CRYPT_RC4::Encrypt( HE_LPBYTE key, HE_ULONG keyLength, HE_LPBYTE data, HE_ULONG dataLength, HE_LPBYTE dataRet )
{
	HE_BYTE rc4[256];
	HE_BYTE t = 0;
	HE_UINT32 j = 0;

	for ( size_t i = 0; i < 256; ++i )
	{
		rc4[i] = static_cast<HE_BYTE>(i);
	}

	for ( size_t l = 0; l < 256; ++l )
	{
		t = rc4[l];
		j = (j + static_cast<HE_UINT32>(t) + static_cast<HE_UINT32>(key[l % keyLength])) % 256;
		rc4[l] = rc4[j];
		rc4[j] = t;
	}
	size_t a = 0;
	size_t b = 0;
	HE_BYTE k = 0;
	for ( HE_ULONG m = 0; m < dataLength; ++m )
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