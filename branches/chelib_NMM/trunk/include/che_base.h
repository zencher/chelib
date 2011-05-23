#ifndef _CHE_BASE_H_
#define _CHE_BASE_H_

#include "che_define.h"

class CHE_Object {};

class IHE_Pause
{
public:
	virtual HE_BOOL	NeedToPauseNow() = 0;
};

class IHE_Write : public CHE_Object
{
public:
	virtual HE_DWORD	GetSize() = 0;

	virtual HE_DWORD	Flush() = 0;

	virtual	HE_BOOL		WriteBlock( const HE_LPVOID pData, HE_DWORD offset, HE_DWORD size ) = 0;
	
	virtual	HE_BOOL		WriteBlock( const HE_LPVOID pData, HE_DWORD size )
	{
		return WriteBlock( pData, GetSize(), size );
	}

	virtual HE_BOOL		WriteByte( HE_BYTE byte )
	{
		return WriteBlock( &byte, 1 );
	}

	virtual HE_VOID		Release() = 0;
};

IHE_Write*	HE_CreateFileWrite( HE_LPCSTR filename );

HE_VOID		HE_DestoryIHEWrite( IHE_Write * pIHEWrite );

class IHE_Read : public CHE_Object
{
public:
	virtual ~IHE_Read() {};

	virtual HE_DWORD	GetSize() = 0;
	
	virtual HE_DWORD	ReadBlock( HE_LPVOID buffer, HE_DWORD offset, HE_DWORD size ) = 0;

	virtual HE_BYTE		ReadByte( HE_DWORD offset ) = 0;
	
	virtual HE_VOID		Release() = 0;
};

IHE_Read*	HE_CreateMemBufRead( HE_LPCBYTE pBuf, HE_DWORD lSize );

HE_VOID		HE_DestoryIHERead( IHE_Read * pIHERead );

#define FILEREAD_MODE_DEFAULT		0
#define FILEREAD_MODE_MEMCOPY		1
#define FILEREAD_MODE_BUFFER		2
#define FILEREAD_MODE_BLOCKLINK		3

IHE_Read* HE_CreateFileRead( HE_LPCSTR filename, HE_BYTE mode = 0, HE_DWORD param = 4096 );

#endif
