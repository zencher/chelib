#include "../../include/pdf/che_pdf_filter.h"

#include "../../extlib/zlib/zlib.h"

#include <memory.h>


//===================================================================
//	解码程序用到String Table的最大长度
#define		LZW_MAX_TABLE_SIZE		4096

//	编码程序只用到Hash表, 不需要String Table, 因为它不需要知道String Table中的内容, 
//	只需要知道Old + Pixel字串是否在表中和表中的index
//	Hash表设计为  :  (Old << 8) | Pixel 其中存放的是String Table的Index
#define		LZW_MAX_HASH_SIZE		0x1000FF // (4096 << 8) + 0xFF

//	解码程序用到String Table - string结构
//	每个string可以形成一棵二叉树, 此二叉树仅有一个右节点
//	因为wPrefix总是指向String Table中的另一位置, 而wSuffix指向0 ~ (clear-1)
typedef struct tagLZW_STRING
{
	HE_WORD	wPrefix ;	// 为 Old
	HE_WORD	wSuffix ;	// 为 GetFirstChar (Old) 或 GetFirstChar (Code)
} LZW_STRING, * PLZW_STRING ;

//===================================================================
//	LZW - 压缩算法
//===================================================================
class FCLzw
{
public :
	 FCLzw () ;
	~FCLzw () ;

	/********************************************************************/
	/*	功  能 ：通用LZW编码											*/
	/*	说  明 ：OutBuffer必须预先置0									*/
	/*			 输出的OutBuffer按 8 + ...data... 格式存储				*/
	/********************************************************************/
	HE_DWORD	LZW_Encode ( HE_LPBYTE InBuffer, HE_DWORD dwLength,
						 HE_LPBYTE OutBuffer) ;

	/********************************************************************/
	/*	功  能 ：通用LZW解码											*/
	/*	说  明 ：InBuffer必须按 8 + ...data... 格式存储					*/
	/********************************************************************/
	void	LZW_Decode ( HE_LPBYTE InBuffer, HE_LPBYTE OutBuffer) ;

	/********************************************************************/
	/*	功  能 ：编码DIB为GIF											*/
	/*	参  数 ：把DIB左上角的指针传给DibBuffer							*/
	/*	返回值 ：写入OutBuffer中的字节数								*/
	/*	说  明 ：OutBuffer必须预先置0									*/
	/*			 暂时只支持4, 8位色位图编码，不支持1位色位图			*/
	/********************************************************************/
	HE_DWORD	LZW_GIF_Encode ( HE_LPBYTE DibBuffer, HE_LPBYTE OutBuffer,
							HE_DWORD dwDibWidth, HE_DWORD dwDibHeight,
							HE_WORD wColorBit) ;

	/********************************************************************/
	/*	功  能 ：解码GIF到DIB											*/
	/*	参  数 ：把DIB左上角的指针传给DibBuffer							*/
	/*	说  明 ：所有位色GIF一律解码为8位色DIB, DibBuffer为8位色		*/
	/*			 InBuffer必须按 Min_Code_Length + ...data... 格式存储	*/
	/*				所以GIF数据(InBuffer)先要解包						*/
	/********************************************************************/
	void	LZW_GIF_Decode ( HE_LPBYTE InBuffer, HE_LPBYTE DibBuffer,
							 HE_DWORD dwDibWidth, HE_DWORD dwDibHeight,
							bool bInterlace) ;

private :
	LZW_STRING	* m_pStrBegin ;	// String Table
	HE_WORD		* m_pHash ;		// Hash Table (16bit存放m_pStrBegin的Index)
	HE_WORD		m_LZW_CLEAR, m_LZW_END ; // clear和end标志

	//	交错存储用---------------------------------------------------+
	HE_BYTE		m_byInterval ;	// 用在交错存储的gif中, 作为m_Interval的索引, 0xFF为非交错存储
	HE_BYTE		m_Interval[7] ; // 交错排列 : 8 8 4 2
	//	交错存储用---------------------------------------------------+

	HE_BYTE		* m_pOrigin ;
	HE_BYTE		* m_pCurIn ;
	HE_BYTE		* m_pCurOut ;
	HE_BYTE		m_byInBit ;		// 输入缓冲Bit位置
	HE_BYTE		m_byOutBit ;	// 输出缓冲Bit位置

	HE_BYTE		m_byMinCode ;		// 最小码长度 (即原始颜色位数)
	HE_BYTE		m_byCurrBits ;		// 现阶段码长
	HE_WORD		m_CurrTableIndex ;	// 当前的String Table Index	
	HE_BYTE		m_Padding[2] ;

	HE_DWORD		m_dwDibHeight ;  // DIB的高度
	HE_DWORD		m_dwDibWidth ;   // DIB的宽度
	HE_DWORD		m_dwCurrPixel ;	 // 当前行写入的像素个数
	HE_DWORD		m_dwCurrHeight ; // 当前的高
	HE_DWORD		m_dwPitch ;		 // DIB每行的字节数

private :
	//	解码
	void	Decode_InitStringTable () ;
	HE_WORD	Decode_GetNextCode () ; // 最长12位
	bool	Decode_IsInTable (HE_WORD Code) ;
	void	Decode_AddStringToTable (HE_WORD wPrefix, HE_WORD wSuffix) ;
	HE_BYTE	Decode_GetFirstChar (HE_WORD Code) ;
	void	Decode_WriteString_to8 (HE_WORD Code) ; // 解码到目标8位色位图
	void	Decode_SwitchToFollowLine () ; // 使m_pCurOut切换到下一行
	//	编码
	void	Encode_InitStringTable () ;
	HE_BYTE	Encode_GetNextPixel () ;
	bool	Encode_IsInTable (HE_WORD Old, HE_WORD Pixel) ; // Old和Pixel都指向在m_pStrBegin中的index。
	void	Encode_AddStringToTable (HE_WORD Old, HE_WORD Pixel) ;
	void	Encode_WriteIndex (HE_DWORD wIndex) ; // 压缩时写String Index
} ;

//===================================================================
//	Implement
//===================================================================
inline  FCLzw::FCLzw () {
	m_Interval[0] = m_Interval[1] = 8 ;
	m_Interval[2] = 4 ; m_Interval[3] = 2 ; // 交错行数
}
inline  FCLzw::~FCLzw () {
}
inline void  FCLzw::Decode_InitStringTable () {
	memset (m_pStrBegin, 0xFF, LZW_MAX_TABLE_SIZE * sizeof(LZW_STRING)) ;
	for (HE_WORD i = 0 ; i < m_LZW_CLEAR ; i++) // 初始化String Table
		m_pStrBegin[i].wSuffix = i ;
	m_CurrTableIndex = m_LZW_END + 1 ;
	m_byCurrBits = m_byMinCode + 1 ;
}
inline void  FCLzw::Decode_SwitchToFollowLine () {
	m_dwCurrPixel = 0 ; // 到行开头
	if (m_byInterval == 0xFF) // 非交错存储, 直接到下一行
		m_pCurOut -= m_dwPitch ;
	else
	{
		m_dwCurrHeight += m_Interval[m_byInterval] ; // 交错存储的gif, 计算下一行
		if (m_dwCurrHeight >= m_dwDibHeight)
			if (++m_byInterval < 4)
				m_dwCurrHeight = m_Interval[m_byInterval]/2 ;
			else
				return ;
		m_pCurOut = m_pOrigin - m_dwCurrHeight * m_dwPitch ;
	}
}
inline HE_WORD  FCLzw::Decode_GetNextCode () {
	// 与Encode_WriteIndex ()是相对应的, 最长为12位(最多跨越2-BYTE)
	register HE_DWORD			dwRet = 0 ;
	register unsigned int	uiAdd = m_byInBit + m_byCurrBits ;

	if (uiAdd <= 8) // 在当前BYTE内
		dwRet |= * m_pCurIn ;
	else
		if (uiAdd <= 16) // 跨1-BYTE
			dwRet |= * (HE_WORD *) m_pCurIn ;
		else // 跨2-BYTE
		{
			dwRet |= * (m_pCurIn + 2) ;
			dwRet <<= 16 ;
			dwRet |= * (HE_WORD *) m_pCurIn ;
		}
	m_pCurIn += uiAdd / 8 ;
	m_byInBit = uiAdd % 8 ;
	dwRet <<= 32 - uiAdd ;
	dwRet >>= 32 - m_byCurrBits ; // 左右清零
	return (HE_WORD)dwRet ;
}
inline bool  FCLzw::Decode_IsInTable (HE_WORD Code) {
	return (Code < m_CurrTableIndex) ;
}
inline HE_BYTE  FCLzw::Decode_GetFirstChar (HE_WORD Code) {
	while (m_pStrBegin[Code].wPrefix != 0xFFFF)
		Code = m_pStrBegin[Code].wPrefix ;
	return (HE_BYTE) m_pStrBegin[Code].wSuffix ;
}
inline void  FCLzw::Decode_AddStringToTable (HE_WORD wPrefix, HE_WORD wSuffix) {
	m_pStrBegin[m_CurrTableIndex  ].wPrefix = wPrefix ;
	m_pStrBegin[m_CurrTableIndex++].wSuffix = wSuffix ;
	if ((m_CurrTableIndex == 0x008) || (m_CurrTableIndex == 0x010) ||
		(m_CurrTableIndex == 0x020) || (m_CurrTableIndex == 0x040) ||
		(m_CurrTableIndex == 0x080) || (m_CurrTableIndex == 0x100) ||
		(m_CurrTableIndex == 0x200) || (m_CurrTableIndex == 0x400) ||
		(m_CurrTableIndex == 0x800))
		m_byCurrBits++ ;
}
inline void  FCLzw::LZW_Decode (HE_BYTE * InBuffer, HE_BYTE * OutBuffer) {
	//	给dwDibWidth传入0则不换行
	this->LZW_GIF_Decode (InBuffer, OutBuffer, 0, 0, false) ;
}
//===================================================================
//	Encode
//===================================================================
inline void  FCLzw::Encode_InitStringTable () {
	m_CurrTableIndex = m_LZW_END + 1 ;
	m_byCurrBits = m_byMinCode + 1 ;
	memset (m_pHash, 0x00, LZW_MAX_HASH_SIZE * sizeof(HE_WORD)) ; // Hash Table置位0
}
inline HE_BYTE  FCLzw::Encode_GetNextPixel () {
	//	目前只支持1, 4, 8位色, 所以不会跨BYTE
	register HE_BYTE		byRet ;
	switch (m_byMinCode)
	{
		case 8 : byRet = m_pCurIn[m_dwCurrPixel] ; break ;
		case 4 : byRet = (m_dwCurrPixel % 2 == 0)
						 ? m_pCurIn[m_dwCurrPixel / 2] >> 4
						 : m_pCurIn[m_dwCurrPixel / 2] & 0x0F ; break ;
		case 1 : byRet = 0x01 & (m_pCurIn[m_dwCurrPixel / 8] >> (7 - (m_dwCurrPixel & 7))) ; break ;
	}
	if (++m_dwCurrPixel == m_dwDibWidth) // 行结束
	{
		m_dwCurrPixel = 0 ;
		m_dwCurrHeight++ ; // DIB下一行
		m_pCurIn = m_pOrigin - m_dwCurrHeight * m_dwPitch ;
	}
	return byRet ;
}
inline bool  FCLzw::Encode_IsInTable (HE_WORD Old, HE_WORD Pixel) {
	return (m_pHash[(Old << 8) | Pixel] != 0) ;
}
inline void  FCLzw::Encode_AddStringToTable (HE_WORD Old, HE_WORD Pixel) {
	m_pHash[(Old << 8) | Pixel] = m_CurrTableIndex++ ;
	if ((m_CurrTableIndex == 0x009) || (m_CurrTableIndex == 0x011) ||
		(m_CurrTableIndex == 0x021) || (m_CurrTableIndex == 0x041) ||
		(m_CurrTableIndex == 0x081) || (m_CurrTableIndex == 0x101) ||
		(m_CurrTableIndex == 0x201) || (m_CurrTableIndex == 0x401) ||
		(m_CurrTableIndex == 0x801))
		m_byCurrBits++ ;
}
inline void  FCLzw::Encode_WriteIndex (HE_DWORD Index) {
	// 压缩时写String Index, 最长为12位(最多跨越2-BYTE)
	// 与Decode_GetNextCode ()是相对应的
	register unsigned int	uiAdd = m_byOutBit + m_byCurrBits ;

	// 与解码不一样, 压缩时会预留一些内存, 可以用DWORD
	* (HE_DWORD *) m_pCurOut |= (Index << m_byOutBit) ;
	m_byOutBit = uiAdd % 8 ;
	m_pCurOut += uiAdd / 8 ;
}
inline HE_DWORD  FCLzw::LZW_Encode (HE_BYTE * InBuffer, HE_DWORD dwLength, HE_BYTE * OutBuffer) {
	//	给dwDibWidth传入0则不换行
	*OutBuffer++ = 8 ;
	return 1 + this->LZW_GIF_Encode (InBuffer, OutBuffer, dwLength, 1, 8) ;
}

void  FCLzw::Decode_WriteString_to8 (HE_WORD Code)
{
	if (Code < m_LZW_CLEAR)
	{
		m_pCurOut[m_dwCurrPixel++] = (HE_BYTE) m_pStrBegin[Code].wSuffix ;
		if (m_dwCurrPixel == m_dwDibWidth) // 写完一行, 切换值至下一行
			this->Decode_SwitchToFollowLine () ;
	}
	else
	{
		this->Decode_WriteString_to8 (m_pStrBegin[Code].wPrefix) ;
		this->Decode_WriteString_to8 (m_pStrBegin[Code].wSuffix) ;
	}
}
//===================================================================
void  FCLzw::LZW_GIF_Decode (HE_BYTE * InBuffer, HE_BYTE * DibBuffer,
							 HE_DWORD dwDibWidth, HE_DWORD dwDibHeight,
							 bool bInterlace)
{
	m_pStrBegin = new LZW_STRING [LZW_MAX_TABLE_SIZE + 32] ;
	if ((m_pStrBegin == NULL) || (InBuffer == NULL) || (DibBuffer == NULL))
		 return ;
	//	初始化设置-----------------------------------------+
	m_pCurIn = InBuffer + 1 ;
	m_pCurOut = DibBuffer ;
	m_byInBit = 0 ; // 输入从0开始, 输出为8位色位图
	m_byMinCode = *InBuffer ; // 获得最小码长度
	m_LZW_CLEAR = 1 << m_byMinCode ; // clear
	m_LZW_END = m_LZW_CLEAR + 1 ; // end

	m_dwDibWidth = dwDibWidth ; // DIB宽
	m_dwCurrPixel = 0 ;
	m_dwPitch = 4 * ((dwDibWidth * 8 + 31) / 32) ;

	m_byInterval = bInterlace ? 0 : 0xFF ; // 设置交错索引
	if (m_byInterval == 0) // 交错存储
	{
		m_pOrigin = m_pCurOut ;
		m_dwDibHeight = dwDibHeight ; // DIB高
		m_dwCurrHeight = 0 ; // 从最顶行开始
	}
	//	初始化设置完成-------------------------------------+

	HE_WORD		Code, Old ;
	this->Decode_InitStringTable () ;
	while ((Code = this->Decode_GetNextCode ()) != m_LZW_END)
	{
		if (Code == m_LZW_CLEAR)
		{
			this->Decode_InitStringTable () ;
			while ((Code = this->Decode_GetNextCode ()) == m_LZW_CLEAR) ;
		}
		else
			if (this->Decode_IsInTable (Code))
				this->Decode_AddStringToTable (Old, this->Decode_GetFirstChar (Code)) ;
			else
				this->Decode_AddStringToTable (Old, this->Decode_GetFirstChar (Old)) ;

		this->Decode_WriteString_to8 (Code) ;
		Old = Code ;
	}
	delete[] m_pStrBegin ;
}
//===================================================================
HE_DWORD  FCLzw::LZW_GIF_Encode (HE_BYTE * DibBuffer, HE_BYTE * OutBuffer,
							  HE_DWORD dwDibWidth, HE_DWORD dwDibHeight,
							  HE_WORD wColorBit)
{
	//	编码阶段不需要String Table，只需要m_CurrTableIndex来获知写位数
	m_pHash = new HE_WORD [LZW_MAX_HASH_SIZE] ;
	if ((m_pHash == NULL) || (DibBuffer == NULL) || (OutBuffer == NULL))
		return 0 ;
	//	初始化设置-----------------------------------------+
	m_byMinCode = (wColorBit == 1) ? 2 : wColorBit ; // 1, 4, 8位色(最小码长度)，1位色为2
	m_LZW_CLEAR = 1 << m_byMinCode ;
	m_LZW_END = m_LZW_CLEAR + 1 ;
	m_pOrigin = m_pCurIn = DibBuffer ;
	m_pCurOut = OutBuffer ;
	m_byInBit = 8 ;
	m_byOutBit = 0 ; // 输出从0开始

	m_dwDibWidth = dwDibWidth ;		// DIB宽
	m_dwDibHeight = dwDibHeight ;	// DIB高
	m_dwCurrPixel = 0 ;
	m_dwCurrHeight = 0 ; // 从最顶行开始
	m_dwPitch = 4 * ((dwDibWidth * m_byMinCode + 31) / 32) ;
	//	初始化设置完成-------------------------------------+

	HE_WORD		Old ;	// 保留字串
	HE_BYTE		Pixel ;	// 当前读入字符
	this->Encode_InitStringTable () ;
	this->Encode_WriteIndex (m_LZW_CLEAR) ; // 首先写clear
	Old = this->Encode_GetNextPixel () ;
	// 编码
	while (m_dwCurrHeight < m_dwDibHeight)
	{
		Pixel = this->Encode_GetNextPixel () ;
		if (this->Encode_IsInTable (Old, Pixel))
			Old = m_pHash[(Old << 8) | Pixel] ; // 已在表中, 取出索引, Hash Table中存放的是String Table的Index
		else
		{
			//	不在表中, 把Old + Pixel添加到String Table中
			this->Encode_WriteIndex (Old) ;
			this->Encode_AddStringToTable (Old, Pixel) ;
			Old = Pixel ;
			if (m_CurrTableIndex == LZW_MAX_TABLE_SIZE) // 表填满
			{
				this->Encode_WriteIndex (Pixel) ;
				this->Encode_WriteIndex (m_LZW_CLEAR) ;
				this->Encode_InitStringTable () ;
				Old = this->Encode_GetNextPixel () ;
			}
		}
	}
	this->Encode_WriteIndex (Old) ;
	this->Encode_WriteIndex (m_LZW_END) ;
	delete[] m_pHash ;
	return (m_pCurOut - OutBuffer + 1) ;
}


HE_VOID CHE_PDF_HexFilter::Encode( HE_LPBYTE pData, HE_DWORD length, CHE_DynBuffer & buffer )
{
	if ( pData == NULL || length == 0  )
	{
		return;
	}

    HE_BYTE data[2];
	HE_DWORD index = 0; 
    while( length-- )
    {
        data[0]  = (*pData & 0xF0) >> 4;
        data[0] += (data[0] > 9 ? 'A' - 10 : '0');
		
        data[1]  = (*pData & 0x0F);
        data[1] += (data[1] > 9 ? 'A' - 10 : '0');
		
		buffer.Write( data, 2 );
		
        ++pData;
    }
}

HE_VOID CHE_PDF_HexFilter::Decode( HE_LPBYTE pData, HE_DWORD length, CHE_DynBuffer & buffer )
{
	if ( pData == NULL || length == 0  )
	{
		return;
	}

	HE_BOOL	bLow = TRUE;
    HE_BYTE chr = 0;
	HE_BYTE	val = 0;
	HE_DWORD index = 0;
    while( length-- ) 
    {
        chr  = *pData;
        if( bLow ) 
        {
            val = (chr & 0x0F);
            bLow = FALSE;
        }
        else
        {
            val = ((val << 4) | chr);
            bLow = TRUE;
			buffer.Write( &val, 1 );
        }
        ++pData;
    }
}

HE_VOID CHE_PDF_ASCII85Filter::Encode( HE_LPBYTE pData, HE_DWORD length, CHE_DynBuffer & buffer )
{
	m_count = 0;
    m_tuple = 0;

	HE_DWORD  c;
    const HE_CHAR *   z = "z";
	
    while( length ) 
    {
        c = *pData & 0xff;
        switch (m_count++) {
		case 0: m_tuple |= ( c << 24); break;
		case 1: m_tuple |= ( c << 16); break;
		case 2: m_tuple |= ( c <<  8); break;
		case 3:
			m_tuple |= c;
			if( 0 == m_tuple ) 
			{
				buffer.Write( (unsigned char*)z, 1 );
			}
			else
			{
				this->EncodeTuple( m_tuple, m_count, buffer ); 
			}
			
			m_tuple = 0;
			m_count = 0;
			break;
        }
        --length;
        ++pData;
    }
	if ( m_count > 0 )
	{
		this->EncodeTuple( m_tuple, m_count, buffer );
	}
	buffer.Write( (unsigned char*)("~>"), 2 );
}

HE_VOID CHE_PDF_ASCII85Filter::Decode( HE_LPBYTE pData, HE_DWORD length, CHE_DynBuffer & buffer )
{
	m_count = 0;
    m_tuple = 0;
    HE_BOOL foundEndMarker = FALSE;
	const HE_DWORD sPowers85[] = { 85*85*85*85, 85*85*85, 85*85, 85, 1 };
	
    while( length && !foundEndMarker ) 
    {
        switch ( *pData ) 
        {
		default:
			if ( *pData < '!' || *pData > 'u') 
			{
				//error
				return;
			}
			
			m_tuple += ( *pData - '!') * sPowers85[m_count++];
			if( m_count == 5 ) 
			{
				WidePut( m_tuple, m_count-1, buffer );
				m_count = 0;
				m_tuple = 0;
			}
			break;
		case 'z':
			if (m_count != 0 ) 
			{
				//error
				return;
			}
			this->WidePut( 0, 4, buffer );
			break;
		case '~':
			++pData; 
			--length;
			if( length && *pData != '>' ) 
			{
				//error
				return;
			}
			foundEndMarker = true;
			break;
		case '\n': case '\r': case '\t': case ' ':
		case '\0': case '\f': case '\b': case 0177:
			break;
        }
        --length;
        ++pData;
	}
	
	if( m_count > 1 ) 
	{
		WidePut( m_tuple, m_count-1, buffer );
		m_count = 0;
		m_tuple = 0;
	}
}

HE_VOID CHE_PDF_ASCII85Filter::EncodeTuple( HE_DWORD tuple, HE_DWORD count, CHE_DynBuffer & buffer )
{
	HE_DWORD i = 5;
	HE_DWORD z = 0;
	HE_CHAR buf[5];
	HE_CHAR out[5];
	HE_CHAR* start = buf;
	
	do 
	{
		*start++ = (HE_CHAR)(tuple % 85);
		tuple /= 85;
	} 
	while (--i > 0);
	
	i = count;
	do 
	{
		out[z++] = (HE_CHAR)(*--start) + '!';
	} 
	while( i-- > 0 );

	buffer.Write( (unsigned char*)out, z );
}
	
HE_VOID CHE_PDF_ASCII85Filter::WidePut( HE_DWORD tuple, HE_DWORD bytes, CHE_DynBuffer & buffer )
{
    char data[4];

	data[0] = static_cast<char>(tuple >> 24);
	data[1] = static_cast<char>(tuple >> 16);
	data[2] = static_cast<char>(tuple >>  8);
	data[3] = static_cast<char>(tuple);

	if ( bytes != 4 )
	{
		switch ( bytes )
		{
		case 3:
			if ( data[3] != 0 )
			{
				data[2]++;
			}
			break;
		case 2:
			if ( data[2] != 0 || data[3] != 0 )
			{
				data[1]++;
			}
			break;
		case 1:
			if ( data[1] != 0 || data[2] != 0 || data[3] != 0 )
			{
				data[0]++;
			}
			break;
		default:
			break;
		}
	}

	buffer.Write( (unsigned char*)data, bytes );
}

HE_VOID CHE_PDF_FlateFilter::Encode( HE_LPBYTE pData, HE_DWORD length, CHE_DynBuffer & buffer )
{
	if ( pData == NULL || length == 0 )
	{
		return;
	}

	HE_BYTE tmpBuffer[4096];
	HE_DWORD nWrittenData = 0;
	z_stream stream;
	HE_INT32 param = Z_NO_FLUSH;

	stream.zalloc = NULL;
	stream.zfree = NULL;
    stream.avail_in = length;
    stream.next_in  = pData;
	deflateInit( &stream, Z_DEFAULT_COMPRESSION );
	
    while( TRUE )
	{
        stream.avail_out = 4096;
        stream.next_out  = tmpBuffer;

		switch ( deflate( &stream, param ) )
		{
		case Z_OK:
			if ( stream.avail_out != 0 )
			{
				param = Z_FINISH;
			}
			break;
		case Z_STREAM_END:
			nWrittenData = 4096 - stream.avail_out;
			buffer.Write( tmpBuffer, nWrittenData );
			deflateEnd( &stream );
			return;
			break;
		case Z_BUF_ERROR:
			break;
		default:
			deflateEnd( &stream );
			return;
			break;
		}
        nWrittenData = 4096 - stream.avail_out;
		buffer.Write( tmpBuffer, nWrittenData );
    }
}

HE_VOID CHE_PDF_FlateFilter::Decode( HE_LPBYTE pData, HE_DWORD length, CHE_DynBuffer & buffer )
{
	if ( pData == NULL || length == 0 )
	{
		return;
	}

	HE_BYTE * tmpBuffer = new HE_BYTE[4096];
	HE_DWORD nWrittenData = 0;
	z_stream stream;
	HE_INT32 param = Z_NO_FLUSH;

	stream.zalloc = NULL;
	stream.zfree = NULL;
	stream.avail_in = length;
	stream.next_in  = pData;
	inflateInit( &stream );

	while( TRUE )
	{
		stream.avail_out = 4096;
		stream.next_out  = tmpBuffer;
   
		switch ( inflate( &stream, param ) )
		{
		case Z_OK:
			if ( stream.avail_out != 0 )
			{
				param = Z_FINISH;
			}
			break;
		case Z_STREAM_END:
			nWrittenData = 4096 - stream.avail_out;
			buffer.Write( tmpBuffer, nWrittenData );
			inflateEnd( &stream );
			return;
			break;
		case Z_BUF_ERROR:
			break;
		default:
			inflateEnd( &stream );
			return;
			break;
		}   
		nWrittenData = 4096 - stream.avail_out;
		buffer.Write( tmpBuffer, nWrittenData );
	}
}

const unsigned short CHE_PDF_LZWFilter::s_masks[] = {	0x01FF,
														0x03FF,
														0x07FF,
														0x0FFF };

const unsigned short CHE_PDF_LZWFilter::s_clear  = 0x0100;      // clear table
const unsigned short CHE_PDF_LZWFilter::s_eod    = 0x0101;      // end of data

HE_VOID CHE_PDF_LZWFilter::Encode( HE_LPBYTE pData, HE_DWORD length, CHE_DynBuffer & buffer )
{
	return;
}

HE_VOID CHE_PDF_LZWFilter::Decode( HE_LPBYTE pData, HE_DWORD length, CHE_DynBuffer & outBuffer )
{
	m_mask       = 0;
    m_code_len   = 9;
    m_character  = 0;
	
    m_bFirst     = true;

	unsigned int       buffer_size = 0;
    const unsigned int buffer_max  = 24;
	
    HE_DWORD         old         = 0;
    HE_DWORD         code        = 0;
    HE_DWORD         buffer      = 0;
	
    TLzwItem           item;
	
    std::vector<unsigned char> data;
	
    if( m_bFirst ) 
    {
        m_character = *pData;
        m_bFirst    = false;
    }
	
    while( length ) 
    {
        // Fill the buffer
        while( buffer_size <= (buffer_max-8) && length )
        {
            buffer <<= 8;
            buffer |= static_cast<HE_DWORD>(static_cast<unsigned char>(*pData));
            buffer_size += 8;
			
            ++pData;
            length--;
        }
		
        // read from the buffer
        while( buffer_size >= m_code_len ) 
        {
            code         = (buffer >> (buffer_size - m_code_len)) & CHE_PDF_LZWFilter::s_masks[m_mask];
            buffer_size -= m_code_len;
			
            if( code == CHE_PDF_LZWFilter::s_clear ) 
            {
                m_mask     = 0;
                m_code_len = 9;
				
                InitTable();
            }
            else if( code == CHE_PDF_LZWFilter::s_eod ) 
            {
                length = 0;
                break;
            }
            else 
            {
                if( code >= m_table.size() )
                {
//                     if (old >= m_table.size())
//                     {
//                         PODOFO_RAISE_ERROR( ePdfError_ValueOutOfRange );
//                     }
                    data = m_table[old].value;
                    data.push_back( m_character );
                }
                else
                    data = m_table[code].value;
				
                // Write data to the output device
                //if( m_pPredictor ) 
                //    m_pPredictor->Decode( reinterpret_cast<char*>(&(data[0])), data.size(), GetStream() );
                //else
                //    GetStream()->Write( reinterpret_cast<char*>(&(data[0])), data.size());

				outBuffer.Write( (HE_LPBYTE)(reinterpret_cast<char*>(&(data[0]))), data.size() );
				
                m_character = data[0];
                if( old < m_table.size() ) // fix the first loop
                    data = m_table[old].value;
                data.push_back( m_character );
				
                item.value = data;
                m_table.push_back( item );
				
                old = code;
				
                switch( m_table.size() ) 
                {
				case 511:
				case 1023:
				case 2047:
					++m_code_len;
					++m_mask;
				default:
					break;
                }
            }
        }
    }
	return;
}

HE_VOID CHE_PDF_LZWFilter::InitTable()
{
	int      i;
	TLzwItem item;
	
	m_table.clear();
	m_table.reserve( 4096 );
	
	for( i=0;i<=255;i++ )
	{
		item.value.clear();
		item.value.push_back( static_cast<unsigned char>(i) );
		m_table.push_back( item );
	}
	
	// Add dummy entry, which is never used by decoder
	item.value.clear();
	m_table.push_back( item );
}

HE_VOID CHE_PDF_RLEFileter::Encode( HE_LPBYTE pData, HE_DWORD length, CHE_DynBuffer & buffer )
{
	if ( pData == NULL || length == 0 )
	{
		return;
	}

	CHE_DynBuffer buf( length, length );
	HE_BYTE byte = *pData;
	pData++;
	HE_DWORD lCount = 1;
	HE_BYTE countByte = 0;
	while ( length > 1 )
	{
		if ( byte == *pData )
		{
			if ( buf.GetByteCount() > 1 )
			{
				countByte = (HE_BYTE)(buf.GetByteCount() - 1);
				buffer.Write( &countByte, 1 );
				buffer.Write( buf );
				buf.Clear();
			}
			lCount++;
			if ( lCount == 128 )
			{
				countByte = (HE_BYTE)(lCount + 127);
				buffer.Write( &countByte, 1 );
				buffer.Write( &byte, 1 );
				lCount = 1;
				byte = *(++pData);
			}
		}else if ( byte != *pData )
		{
			if ( lCount >= 2 )
			{
				countByte = (HE_BYTE)(lCount + 127);
				buffer.Write( &countByte, 1 );
				buffer.Write( &byte, 1 );
				lCount = 1;
				byte = *pData;
			}else{
				buf.Write( &byte, 1 );
				byte = *pData;
				if ( buf.GetByteCount() == 128 )
				{
					countByte = (HE_BYTE)(buf.GetByteCount() - 1);
					buffer.Write( &countByte, 1 );
					buffer.Write( buf );
					buf.Clear();
				}
			}
		}
		length--;
		pData++;
	}

	if ( buf.GetByteCount() > 0 )
	{
		countByte = (HE_BYTE)(buf.GetByteCount() - 1);
		buffer.Write( &countByte, 1 );
		buffer.Write( buf );
		buf.Clear();
	}
	if ( lCount == 1 )
	{
		countByte = (HE_BYTE)(lCount - 1);
		buffer.Write( &countByte, 1 );
		buffer.Write( &byte, 1 );
	}else if ( lCount > 1 )
	{
		countByte = (HE_BYTE)(lCount + 127);
		buffer.Write( &countByte, 1 );
		buffer.Write( &byte, 1 );
	}
	countByte = 128;
	buffer.Write( &countByte, 1 );
}

HE_VOID CHE_PDF_RLEFileter::Decode( HE_LPBYTE pData, HE_DWORD length, CHE_DynBuffer & buffer )
{
	if ( pData == NULL || length == 0 )
	{
		return;
	}

	HE_BYTE	byteLen = 0;
	HE_BYTE byteData = 0;

	while ( length > 0 )
	{
		byteLen = *pData;
		pData++;
		length--;
		if ( byteLen == 128 )
		{
			return;
		}else if ( byteLen < 128 )
		{
			buffer.Write( pData, byteLen+1 );
			length-= byteLen+1;
			pData+= byteLen+1;
		}else if ( byteLen > 128 )
		{
			for ( HE_DWORD i = 0; i < byteLen - 127; i++ )
			{
				buffer.Write( pData, 1 );
			}
			pData++;
			length--;
		}
	}
}