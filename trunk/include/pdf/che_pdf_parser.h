#ifndef _CHE_PDF_PARSER_H_
#define _CHE_PDF_PARSER_H_

#include "../che_base.h"
#include "../che_string.h"
#include "che_pdf_objects.h"
#include "che_pdf_xref.h"
#include "che_pdf_encrypt.h"
#include "che_pdf_collector.h"
#include <vector>

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


class CHE_PDF_File;
class CHE_PDF_Collector;

class CHE_PDF_ParseWordDes 
{
public:
	CHE_PDF_ParseWordDes() : type(PARSE_WORD_UNKNOWN), offset(0) {};

	PDF_PARSE_WORD		type;
	size_t			offset;
};

class CHE_PDF_SyntaxParser : public CHE_Object
{
public:
	CHE_PDF_SyntaxParser( CHE_PDF_File * pFile, CHE_Allocator * pAllocator = nullptr );
	~CHE_PDF_SyntaxParser();

	bool				InitParser( IHE_Read* pIFileRead );
	size_t			GetFileSize() { return mlFileSize; };

	void				SetFile( CHE_PDF_File * pFile ) { mpFile = pFile; }
	CHE_PDF_File *		GetFile()const { return mpFile; }

	/*	当前位置移动和设置的相关操作	*/
	size_t			GetPos() { return mlFilePos; };
	void				SetPos( size_t pos) { mlFilePos = pos; };
	size_t			Seek( size_t bytes );
	void				SeekToPrevLine();
	void				SeekToNextLine();
	void				SeekToPrevWord();
	void				SeekToNextWord();
 	void				SeekToEndStream();
 	void				SeekToEndobj();

	void				SeekToMark( CHE_ByteString markStr );
	void				SetEncrypt( CHE_PDF_Encrypt * pEncrypt ) { mpStrEncrypt = pEncrypt; }
	void				SetCurObjNum( size_t objNum ) { mCurObjNum = objNum; }
	void				SetCurGenNum( size_t genNum ) { mCurGenNum = genNum; }
	CHE_PDF_Encrypt *	GetEncrypt() { return mpStrEncrypt; }
	size_t			ReadBytes( PBYTE pBuffer, size_t length );
	bool				GetWord( CHE_PDF_ParseWordDes & des );
	/* 从当前位置开始解析一个数组，如果当前位置不是一个数组，则返回空（当前位置必须是数组开始"["） */
	CHE_PDF_ArrayPtr	GetArrayPtr();
	/*	从当前位置开始解析一个字典，如果当前位置不是一个字典，则返回空（当前位置必须是字典开始"<<"）*/
	CHE_PDF_DictionaryPtr	GetDictionaryPtr();

	bool				IsWord( const char * words );

	int32			GetInteger();
	FLOAT			GetFloat();

	CHE_ByteString		GetString();
	void				GetString( CHE_ByteString & str );

private:
	void				SubmitBufferStr();

	CHE_PDF_File *		mpFile;
	IHE_Read*			mpIFileRead;
	CHE_PDF_Encrypt	*	mpStrEncrypt;

	size_t			mlFilePos;
	size_t			mlFileSize;

	size_t			mlBufferSize;
	size_t			mlBufferPos;
	size_t			mlSubmitSize;

	size_t			mCurObjNum;
	size_t			mCurGenNum;

	bool				mbBegin;
	bool				mbPoint;
	bool				mbSign;

	BYTE				mWordBuffer[32770];

	friend class CHE_PDF_Parser;
};

class CHE_PDF_Parser : public CHE_Object
{
public:
	static CHE_PDF_Parser * Create( CHE_PDF_File * pFile, IHE_Read * pRead, CHE_PDF_XRefTable * pXrefTable, CHE_Allocator * pAllocator = nullptr );

	~CHE_PDF_Parser();

	//Basic information
	size_t					GetFileSize() const;
	PDF_VERSION					GetPDFVersion() const;

	//Encryption
	bool						Authenticate( const CHE_ByteString & password );

	size_t					GetReadPos() { return mSyntaxParser.GetPos(); }
	void						SetReadPos( size_t pos) { mSyntaxParser.SetPos( pos ); }

	CHE_PDF_ObjectPtr			GetObject();
	CHE_PDF_ObjectPtr			GetObjectInObjStm( CHE_PDF_StreamPtr & pStream, const PDF_RefInfo & ObjrefInfo, size_t index );
	bool						GetAllObjectsInObjStm( CHE_PDF_StreamPtr & pStream, CHE_PDF_Collector * pCollector );

/*	CHE_PDF_ReferencePtr		GetEncryptRef() const { return mpEncryptRef; } */

private:
	CHE_PDF_Parser( CHE_PDF_File * pFile, IHE_Read * pRead, CHE_PDF_XRefTable * pXrefTable, CHE_Allocator * pAllocator = nullptr );

	size_t					GetStartxref( size_t range );

	size_t					ParseXRef();
	size_t					ParseXRefTable( size_t offset, CHE_PDF_DictionaryPtr & pTrailerDictRet );
	size_t					ParseXRefStream( size_t offset, CHE_PDF_DictionaryPtr & pTrailerDictRet );
	size_t					FullParseForXRef();

	bool						ParseEncrypt( const CHE_PDF_DictionaryPtr & pEncryptDict, const CHE_PDF_ArrayPtr & pIdArray );

private:
	CHE_PDF_SyntaxParser		mSyntaxParser;
	CHE_PDF_File *				mpFile;
	IHE_Read *					mpIFileRead;
	size_t					mlStartxref;
	CHE_PDF_XRefTable *			mpXRefTable;
	CHE_PDF_Encrypt	*			mpStrEncrypt;
	CHE_PDF_Encrypt	*			mpStmEncrypt;
	CHE_PDF_Encrypt	*			mpEefEncrypt;
	CHE_PDF_ReferencePtr		mEncryptRef;
	CHE_PDF_ArrayPtr			mIDArrayPtr;

	friend class CHE_PDF_File;
	friend class CHE_Allocator;
};

#endif