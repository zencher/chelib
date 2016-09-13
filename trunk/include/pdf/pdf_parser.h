#ifndef _CHELIB_PDF_PARSER_H_
#define _CHELIB_PDF_PARSER_H_

#include "pdf_object.h"
#include "pdf_xref.h"
#include "pdf_encrypt.h"
#include "pdf_collector.h"

#include <vector>

namespace chelib {

enum PDF_VERSION 
{
	PDF_VERSION_UNKNOWN	= 0x00,
	PDF_VERSION_1_0		= 0x0A,
	PDF_VERSION_1_1		= 0x0B,
	PDF_VERSION_1_2		= 0x0C,
	PDF_VERSION_1_3		= 0x0D,
	PDF_VERSION_1_4		= 0x0E,
	PDF_VERSION_1_5		= 0x0F,
	PDF_VERSION_1_6		= 0x10,
	PDF_VERSION_1_7		= 0x11
};

enum PDF_PARSE_WORD
{
	PARSE_WORD_UNKNOWN	= 0x00,
	PARSE_WORD_INTEGER	= 0x01,
	PARSE_WORD_FLOAT	= 0x02,
	PARSE_WORD_STRING	= 0x03,
	PARSE_WORD_NAME		= 0x04,
	PARSE_WORD_DICT_B	= 0x05,
	PARSE_WORD_DICT_E	= 0x06,
	PARSE_WORD_ARRAY_B	= 0x07,
	PARSE_WORD_ARRAY_E	= 0x08,
	PARSE_WORD_BLOCK_B	= 0x09,
	PARSE_WORD_BLOCK_E	= 0x10
};


class CPDF_File;
class CPDF_Collector;

class CPDF_ParseWordDes 
{
public:
	CPDF_ParseWordDes() : type(PARSE_WORD_UNKNOWN), offset(0) {};

	PDF_PARSE_WORD  type;
	size_t offset;
};

class CPDF_SyntaxParser : public CObject
{
public:
	CPDF_SyntaxParser( CPDF_File * pFile, CAllocator * pAllocator = nullptr );
	~CPDF_SyntaxParser();

	bool InitParser( IRead* pIFileRead );
	size_t GetFileSize() { return mlFileSize; };

	void SetFile( CPDF_File * pFile ) { mpFile = pFile; }
	CPDF_File * GetFile()const { return mpFile; }

	size_t GetPos() { return mlFilePos; };
	void SetPos( size_t pos) { mlFilePos = pos; };

	size_t Seek( size_t bytes );
	void SeekToPrevLine();
	void SeekToNextLine();
	void SeekToPrevWord();
	void SeekToNextWord();
 	void SeekToEndStream();
 	void SeekToEndobj();
	void SeekToMark( CByteString markStr );

    CPDF_Encrypt * GetEncrypt() { return mpStrEncrypt; }
	void SetEncrypt( CPDF_Encrypt * pEncrypt ) { mpStrEncrypt = pEncrypt; }

	void SetCurObjNum( size_t objNum ) { mCurObjNum = objNum; }
	void SetCurGenNum( size_t genNum ) { mCurGenNum = genNum; }

	size_t ReadBytes( PBYTE pBuffer, size_t length );

    bool IsWord( const char * words );
	bool GetWord( CPDF_ParseWordDes & des );

	CPDF_ArrayPtr GetArrayPtr();
	CPDF_DictionaryPtr GetDictionaryPtr();

	int32 GetInteger();
	FLOAT GetFloat();

    CByteString GetString();
    void GetString( CByteString & str );

private:
	void SubmitBufferStr();

	CPDF_File * mpFile;
	IRead* mpIFileRead;
	CPDF_Encrypt * mpStrEncrypt;

	size_t mlFilePos;
	size_t mlFileSize;

	size_t mlBufferSize;
	size_t mlBufferPos;
	size_t mlSubmitSize;

	size_t mCurObjNum;
	size_t mCurGenNum;

	bool mbBegin;
	bool mbPoint;
	bool mbSign;

	BYTE mWordBuffer[32770];

	friend class CPDF_Parser;
};

class CPDF_Parser : public CObject
{
public:
	static CPDF_Parser * Create( CPDF_File * pFile, IRead * pRead, CPDF_XRefTable * pXrefTable, CAllocator * pAllocator = nullptr );
	~CPDF_Parser();

	//Basic information
	size_t GetFileSize() const;
	PDF_VERSION GetPDFVersion() const;

	//Encryption
	bool Authenticate( const CByteString & password );

	size_t GetReadPos() { return mSyntaxParser.GetPos(); }
	void SetReadPos( size_t pos) { mSyntaxParser.SetPos( pos ); }

	CPDF_ObjectPtr GetObject();
	CPDF_ObjectPtr GetObjectInObjStm( CPDF_StreamPtr & pStream, const PDF_RefInfo & ObjrefInfo, size_t index );
	bool GetAllObjectsInObjStm( CPDF_StreamPtr & pStream, CPDF_Collector * pCollector );

private:
	CPDF_Parser( CPDF_File * pFile, IRead * pRead, CPDF_XRefTable * pXrefTable, CAllocator * pAllocator = nullptr );

	size_t GetStartxref( size_t range );
    
	size_t ParseXRef();
	size_t ParseXRefTable( size_t offset, CPDF_DictionaryPtr & pTrailerDictRet );
	size_t ParseXRefStream( size_t offset, CPDF_DictionaryPtr & pTrailerDictRet );
	size_t FullParseForXRef();

	bool ParseEncrypt( const CPDF_DictionaryPtr & pEncryptDict, const CPDF_ArrayPtr & pIdArray );

private:
	CPDF_SyntaxParser mSyntaxParser;
	CPDF_File * mpFile;
	IRead * mpIFileRead;
	size_t mlStartxref;
	CPDF_XRefTable * mpXRefTable;
	CPDF_Encrypt * mpStrEncrypt;
	CPDF_Encrypt * mpStmEncrypt;
	CPDF_Encrypt * mpEefEncrypt;
	CPDF_ReferencePtr mEncryptRef;
	CPDF_ArrayPtr mIDArrayPtr;

	friend class CPDF_File;
	friend class CAllocator;
};
    
}//namespace

#endif