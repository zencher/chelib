#ifndef _CHE_BASE_H_
#define _CHE_BASE_H_

#include "che_define.h"

class CHE_Object
{
// public:
// 	HE_LPVOID operator new( size_t );
// 
// 	HE_LPVOID operator new( size_t, HE_LPCSTR, HE_INT32 );
// 
// 	HE_VOID  operator delete( HE_LPVOID );
// 
// 	HE_VOID  operator delete( HE_LPVOID, HE_LPCSTR, HE_INT32 );
};

/** Simple pause interface. */
class IHE_Pause
{
public:
	/**
	* Do we need to pause now ?
	*
	* return Non-zero means we need, otherwise we need not.
	*/
	virtual HE_BOOL	NeedToPauseNow() = 0;
};

/** stream writing interface. */
class IHE_StreamWrite : public CHE_Object
{
public:
	/**
	* Write a block data. 
	*
	* pData			The block data.
	* size			The length in bytes of the block data.
	* Return FALSE for failure;
	*/
	virtual	HE_BOOL		WriteBlock( const void* pData, HE_DWORD size) = 0;
	
	/** Called when to release everything */
	virtual void		Release() = 0;
	
};

/** stream writing interface. */
class IHE_FileWrite : public IHE_StreamWrite
{
public:
	virtual HE_DWORD	GetSize() = 0;
	virtual HE_DWORD	Flush() = 0;
	/**
	* Write a block data. 
	*
	* pData			The block data.
	* offset		Byte offset for the block, from beginning of the file
	* size			The length in bytes of the block data.
	* Return FALSE for failure;
	*/
	virtual	HE_BOOL		WriteBlock(const void* pData, HE_DWORD offset, HE_DWORD size) = 0;
	
	virtual	HE_BOOL		WriteBlock(const void* pData, HE_DWORD size)
	{
		return WriteBlock(pData, GetSize(), size);
	}
};

/** Create a CRT based file writing interface. Call Release() to destroy it. 
Using MBCS file name. Available only for platforms supporting CRT file access (fopen, ...)
*/
IHE_FileWrite* HE_CreateFileWrite(HE_LPCSTR filename);

/** Create a CRT based file writing interface. Call Release() to destroy it. 
Using unicode file name. Available only for platforms supporting CRT file access (fopen, ...)
*/
IHE_FileWrite* HE_CreateFileWrite(HE_LPCWSTR filename);

class IHE_FileRead : public CHE_Object
{
public:
	/**
	* Interface: GetSize
	*		Get total size of the file
	*
	* @return File size, in bytes. Implementation can return 0 for any error.
	*/   
	virtual HE_DWORD	GetSize() = 0;
	
	/**
	* Interface: ReadBlock
	*				Read a data block from the file
	*
	* buffer		Pointer to a buffer receiving read data
	* offset		Byte offset for the block, from beginning of the file
	* size			Number of bytes for the block.
	* TRUE for success, FALSE for failure.
	*/
	virtual HE_DWORD	ReadBlock(void* buffer, HE_DWORD offset, HE_DWORD size) = 0;

	virtual HE_BYTE		ReadByte( HE_DWORD offset ) = 0;
	
	/** Called when to release everything */
	virtual void		Release() = 0;
};

/** Create a CRT based file reading interface. Call Release() to destroy it. 
Using MBCS file name. Available only for platforms supporting CRT file access
*/
IHE_FileRead* HE_CreateFileRead(HE_LPCSTR filename);

/** Create a CRT based file reading interface. Call Release() to destroy it. 
Using Unicode file name. Available only for platforms supporting CRT file access
*/
IHE_FileRead* HE_CreateFileRead(HE_LPCWSTR filename);


class CHE_PtrArray : public CHE_Object
{
public:
	CHE_PtrArray();
	~CHE_PtrArray();

	HE_BOOL		Append( HE_LPVOID ptr );

	HE_LPVOID	GetItem( HE_DWORD index ) const;

	HE_VOID		Clear();
	
	HE_DWORD	GetCount() const { return m_lCount; }
 
private:
	HE_LPVOID *	m_pData;
	HE_DWORD	m_lSize;
	HE_DWORD	m_lCount;
};

#endif
