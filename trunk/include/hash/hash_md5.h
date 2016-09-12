#ifndef _CHE_HASH_MD5_H_
#define _CHE_HASH_MD5_H_

namespace chelib {

class CHashMD5
{
public:
    CHashMD5();

    void Init();
    void Update( unsigned char const * buf, unsigned int len );
    void Final( unsigned char digest[16] );

private:
    void Transform( unsigned int buf[4], unsigned int const in[16] );

    unsigned int mBuf[4];
    unsigned int mBits[2];
    unsigned char mIn[64];
};

}//namespace

#endif