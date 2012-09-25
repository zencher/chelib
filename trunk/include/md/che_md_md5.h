#ifndef _CHE_MD_MD5_H_
#define _CHE_MD_MD5_H_

class CHE_MD_MD5_Content
{
public:
	CHE_MD_MD5_Content();

	void				Init();
	void				Update( unsigned char const * buf, unsigned int len );
	void				Final( unsigned char digest[16] );

private:
	void				Transform( unsigned int buf[4], unsigned int const in[16] );

	unsigned int		mBuf[4];
	unsigned int		mBits[2];
	unsigned char		mIn[64];
};

#endif