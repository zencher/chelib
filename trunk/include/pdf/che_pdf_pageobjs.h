// #ifndef _CHE_PDF_PAGEOBJS_H_
// #define _CHE_PDF_PAGEOBJS_H_
// 
// #include "../che_base.h"
// #include "../che_datastructure.h"
// #include "../che_bitmap.h"
// #include "../che_graphics.h"
// #include "che_pdf_define.h"
// #include "che_pdf_objects.h"
// 
// 
// class CHE_PDF_Color : public CHE_Object
// {
// public:
// 	CHE_PDF_Color( PDF_COLORSPACE colorSpace = COLORSAPCE_DEVICE_GRAY, HE_DWORD value = 0xFF000000, CHE_Allocator * pAllocator = NULL )
// 		: CHE_Object( pAllocator )
// 	{
// 		m_colorSpace = colorSpace;
// 		m_value = value;
// 	}
// 
// 	HE_VOID SetColorSpace( PDF_COLORSPACE colorSpace ) { m_colorSpace = colorSpace; }
// 
// 	HE_VOID SetValue( HE_DWORD value ) { m_value = value; }
// 
// 	HE_BYTE GetColorSpace() { return m_colorSpace; }
// 
// 	HE_BYTE GetColorType()
// 	{
// 		if ( m_colorSpace > 0x0200 )
// 		{
// 			return COLORSPACE_SPECIAL;
// 		}else if ( m_colorSpace > 0x0100 )
// 		{
// 			return COLORSPACE_CIEBASE;
// 		}else{
// 			return COLORSAPCE_DEVICE;
// 		}
// 	}
// 
// 	HE_DWORD GetValue() { return m_value; }
// 
// 	HE_ARGB GetARGB() { return m_value; }
// 
// private:
// 	PDF_COLORSPACE			m_colorSpace;
// 	HE_DWORD				m_value;
// };
// 
// class CHE_PDF_TextObject;
// class CHE_PDF_PathObject;
// class CHE_PDF_OrderObject;
// 
// class CHE_PDF_ContentObject : public CHE_Object
// {
// public:
// 	CHE_PDF_ContentObject( PDF_CONTENTOBJ_TYPE type = CONTENTOBJ_INVALID, CHE_Allocator * pAllocator = NULL )
// 		: CHE_Object( pAllocator )
// 	{ m_Type = type; }
// 
// 	virtual ~CHE_PDF_ContentObject() {};
// 
// 	PDF_CONTENTOBJ_TYPE GetType() { return m_Type; }
// 
// 	HE_VOID Release();
// 
// private:
// 	PDF_CONTENTOBJ_TYPE m_Type;
// };
// 
// class CHE_PDF_TextObjectItem;
// 
// class CHE_PDF_TextObject : public CHE_PDF_ContentObject
// {
// public:
// 	static CHE_PDF_TextObject* Create( CHE_Allocator * pAllocator = NULL )
// 	{
// 		if ( pAllocator )
// 		{
// 			return pAllocator->New<CHE_PDF_TextObject>(  pAllocator );
// 		}else{
// 			return new CHE_PDF_TextObject( NULL );
// 		}
// 	}
// 
// 	HE_BOOL AppendItem( CHE_PDF_TextObjectItem * pItem );
// 
// 	CHE_PDF_TextObjectItem * GetItem( HE_DWORD index )
// 	{
// 		return (CHE_PDF_TextObjectItem*)m_arrTextItem.GetItem( index );
// 	}
// 
// 	HE_DWORD GetItemCount() { return m_arrTextItem.GetCount(); }
// 
// 	HE_VOID Clear();
// 
// private:
// 	CHE_PDF_TextObject( CHE_Allocator * pAllocator = NULL )
// 		: CHE_PDF_ContentObject( CONTENTOBJ_TEXT, pAllocator ), m_arrTextItem( pAllocator ) {}
// 	~CHE_PDF_TextObject() { Clear(); }
// 
// 	CHE_PtrArray	m_arrTextItem;
// 
// 	friend class CHE_PDF_ContentObject;
// 	friend class CHE_Allocator;
// };
// 
// class CHE_PDF_TextObjectItem : public CHE_Object
// {
// public:
// 	CHE_PDF_TextObjectItem(  CHE_Allocator * pAllocator = NULL )
// 		: CHE_Object( pAllocator ), m_str( pAllocator )
// 	{
// 		m_fPosiX = 0;
// 		m_fPosiY = 0;
// 		m_fCharSpace = 0;
// 		m_fWordSpace = 0;
// 		m_dwScale = 100;
// 		m_fLeading = 0;
// 		m_dwFontObjNum = 0;
// 		m_dwSize = 0;
// 		m_byteRender = 0;
// 		m_dwRise = 0;
// 		m_bKnockout = FALSE;
// 		m_fMatrixA = 1;
// 		m_fMatrixB = 0;
// 		m_fMatrixC = 0;
// 		m_fMatrixD = 1;
// 		m_fMatrixE = 0;
// 		m_fMatrixF = 0;
// 		m_pText = NULL;	
// 	}
// 	~CHE_PDF_TextObjectItem();
// 
// 	HE_VOID SetPosi( HE_FLOAT fX, HE_FLOAT fY ) { m_fPosiX = fX; m_fPosiY = fY; }
// 	HE_VOID SetCharSpace( HE_FLOAT cs ) { m_fCharSpace = cs; }
// 	HE_VOID SetWordSpace( HE_FLOAT ws ) { m_fWordSpace = ws; }
// 	HE_VOID SetScale( HE_DWORD scale ) { m_dwScale = scale; }
// 	HE_VOID SetLeading( HE_FLOAT leading ) { m_fLeading = leading; }
// 	HE_VOID SetFontObj( HE_DWORD objNum ) { m_dwFontObjNum = objNum; }
// 	HE_VOID SetSize( HE_DWORD size ) { m_dwSize = size; }
// 	HE_VOID SetRender( HE_BYTE render ) { m_byteRender = render; }
// 	HE_VOID SetRise( HE_LONG rise ) { m_dwRise = rise; }
// 	HE_VOID SetKnockout( HE_BOOL knockout ) { m_bKnockout = knockout; }
// 	HE_VOID SetMatrix( HE_FLOAT a, HE_FLOAT b, HE_FLOAT c, HE_FLOAT d, HE_FLOAT e, HE_FLOAT f )
// 	{
// 		m_fMatrixA = a; m_fMatrixB = b; m_fMatrixC = c; m_fMatrixD = d; m_fMatrixE = e; m_fMatrixF = f;
// 	}
// 	HE_VOID SetText( CHE_PDF_Object * pObj ) { m_pText = pObj; }
// 
// 	HE_VOID SetString( CHE_WideString & str ) { m_str = str; }
// 
// 	HE_FLOAT	GetPosiX() { return m_fPosiX; }
// 	HE_FLOAT	GetPosiY() { return m_fPosiY; }
// 	HE_FLOAT	GetCharSpace() { return m_fCharSpace; }
// 	HE_FLOAT	GetWordSpace() { return m_fWordSpace; }
// 	HE_DWORD	GetScale() { return m_dwScale; }
// 	HE_FLOAT	GetLeading() { return m_fLeading; }
// 	HE_DWORD	GetFontObjNum() { return m_dwFontObjNum; }
// 	HE_DWORD	GetSize() { return m_dwSize; }
// 	HE_BYTE		GetRender() { return m_byteRender; }
// 	HE_LONG		GetRise() { return m_dwRise; }
// 	HE_BOOL		GetKnockout() { return m_bKnockout; }
// 	HE_FLOAT	GetMatrixA() { return m_fMatrixA; }
// 	HE_FLOAT	GetMatrixB() { return m_fMatrixB; }
// 	HE_FLOAT	GetMatrixC() { return m_fMatrixC; }
// 	HE_FLOAT	GetMatrixD() { return m_fMatrixD; }
// 	HE_FLOAT	GetMatrixE() { return m_fMatrixE; }
// 	HE_FLOAT	GetMatrixF() { return m_fMatrixF; }
// 	CHE_WideString	*	GetStringPtr() { return &m_str; }
// 
// private:
// 	HE_FLOAT	m_fPosiX;
// 	HE_FLOAT	m_fPosiY;
// 	HE_FLOAT	m_fCharSpace;	//def: 0
// 	HE_FLOAT	m_fWordSpace;	//def: 0
// 	HE_DWORD	m_dwScale;		//def: 100
// 	HE_FLOAT	m_fLeading;		//def: 0
// 	HE_DWORD	m_dwFontObjNum;
// 	HE_DWORD	m_dwSize;
// 	HE_BYTE		m_byteRender;	//0 - 7
// 	HE_LONG		m_dwRise;		//0
// 	HE_BOOL		m_bKnockout;	//def: false
// 	HE_FLOAT	m_fMatrixA;		//def: 1
// 	HE_FLOAT	m_fMatrixB;		//def: 0
// 	HE_FLOAT	m_fMatrixC;		//def: 0
// 	HE_FLOAT	m_fMatrixD;		//def: 1
// 	HE_FLOAT	m_fMatrixE;		//def: 0
// 	HE_FLOAT	m_fMatrixF;		//def: 0
// 	CHE_PDF_Object *	m_pText;	//string or array
// 	CHE_WideString m_str;
// 
// 	friend class CHE_PDF_TextObject;
// };
// 
// class CHE_PDF_PathObject : public CHE_PDF_ContentObject
// {
// public:
// 	static CHE_PDF_PathObject* Create( CHE_Allocator * pAllocator = NULL )
// 	{
// 		if ( pAllocator )
// 		{
// 			return pAllocator->New<CHE_PDF_PathObject>( pAllocator );
// 		}else{
// 			return new CHE_PDF_PathObject( NULL );
// 		}
// 	}
// 
// 	HE_VOID SetType( PDF_PATH_TYPE opt ) { m_PathType = opt; }
// 	HE_VOID SetFillMode( PDF_FILL_MODE mode ) { m_FillMode = mode; }
// 
// 	PDF_PATH_TYPE GetType() { return m_PathType; }
// 	PDF_FILL_MODE GetFillMode() { return m_FillMode; }
// 
// 	HE_VOID	SetClipFillMode( PDF_FILL_MODE clipMode ) { m_ClipFillMode = clipMode; }
// 	PDF_FILL_MODE	GetClipFillMode() { return m_ClipFillMode; }
// 
// 	HE_BOOL AppendItem( CHE_GraphicsObject * pItem );
// 
// 	CHE_GraphicsObject * GetItem( HE_DWORD index )
// 	{
// 		return (CHE_GraphicsObject*)m_arrPathItem.GetItem( index );
// 	}
// 
// 	HE_DWORD GetItemCount() { return m_arrPathItem.GetCount(); }
// 
// 	HE_VOID Clear();
// 
// private:
// 	CHE_PDF_PathObject( CHE_Allocator * pAllocator = NULL ) 
// 		: CHE_PDF_ContentObject( CONTENTOBJ_PATH, pAllocator ), m_arrPathItem( pAllocator )
// 	{ m_FillMode = FILL_MODE_NOZERO; m_PathType = PATH_STROKE; m_ClipFillMode = FILL_MODE_NOZERO; }
// 	~CHE_PDF_PathObject() { Clear(); }
// 
// 	CHE_PtrArray	m_arrPathItem;
// 	PDF_PATH_TYPE	m_PathType;
// 	PDF_FILL_MODE	m_FillMode;
// 	PDF_FILL_MODE	m_ClipFillMode;
// 
// 	friend class CHE_PDF_ContentObject;
// 	friend class CHE_Allocator;
// };
// 
// class CHE_PDF_OrderParam : public CHE_Object
// {
// public:
// 	CHE_PDF_OrderParam( PDF_ORDER_TYPE Type = ORDER_UNKNOWN, CHE_Allocator * pAllocator = NULL ) :CHE_Object( pAllocator )
// 	{ m_Type = Type; }
// 
// 	~CHE_PDF_OrderParam() {};
// 
// 	PDF_ORDER_TYPE GetType() { m_Type; }
// 	HE_VOID	Release();
// 
// private:
// 	PDF_ORDER_TYPE m_Type;
// 
// 	friend CHE_Allocator;
// };
// 
// class CHE_PDF_OrderParam_LineWidth : public CHE_PDF_OrderParam
// {
// public:
// 	CHE_PDF_OrderParam_LineWidth( HE_FLOAT fLineWidth, CHE_Allocator * pAllocator = NULL )
// 		: CHE_PDF_OrderParam( ORDER_LINE_WIDTH, pAllocator )
// 	{ m_fLineWidth = fLineWidth; }
// 
// 	HE_VOID	SetLineWidth( HE_FLOAT fLineWidth ) { m_fLineWidth = fLineWidth; } 
// 	HE_FLOAT GetLineWidth() { return m_fLineWidth; }
// 
// private:
// 	HE_FLOAT m_fLineWidth;
// 
// 	friend CHE_Allocator;
// };
// 
// class CHE_PDF_OrderParam_LineCap : public CHE_PDF_OrderParam
// {
// public:
// 	CHE_PDF_OrderParam_LineCap( HE_BYTE lineCap, CHE_Allocator * pAllocator = NULL )
// 		: CHE_PDF_OrderParam( ORDER_LINE_CAP, pAllocator )
// 	{ m_lineCap = lineCap; }
// 
// 	HE_VOID SetLineCap( HE_BYTE lineCap ) { m_lineCap = lineCap; }
// 	HE_BYTE GetLineCap() { return m_lineCap; }
// 
// private:
// 	HE_BYTE m_lineCap;
// 
// 	friend CHE_Allocator;
// };
// 
// class CHE_PDF_OrderParam_LineJoin : public CHE_PDF_OrderParam
// {
// public:
// 	CHE_PDF_OrderParam_LineJoin( HE_BYTE lineJoin, CHE_Allocator * pAllocator = NULL )
// 		: CHE_PDF_OrderParam( ORDER_LINE_JION, pAllocator )
// 	{ m_lineJoin = lineJoin; }
// 
// 	HE_VOID SetLineJoin( HE_BYTE lineCap ) { m_lineJoin = lineCap; }
// 	HE_BYTE GetLineJoin() { return m_lineJoin; }
// private:
// 	HE_BYTE m_lineJoin;
// 
// 	friend CHE_Allocator;
// };
// 
// class CHE_PDF_OrderParam_MiterLimit : public CHE_PDF_OrderParam
// {
// public:
// 	CHE_PDF_OrderParam_MiterLimit( HE_FLOAT miterLimit, CHE_Allocator * pAllocator = NULL )
// 		: CHE_PDF_OrderParam( ORDER_MITER_LIMIT, pAllocator )
// 	{ m_fMiterLimit = miterLimit; }
// 
// 	HE_VOID	SetMiterLimit( HE_FLOAT fMiterLimit ) { m_fMiterLimit = fMiterLimit; } 
// 	HE_FLOAT GetMiterLimit() { return m_fMiterLimit; }
// 
// private:
// 	HE_FLOAT m_fMiterLimit;
// 
// 	friend CHE_Allocator;
// };
// 
// class CHE_PDF_OrderParam_DashPattern : public CHE_PDF_OrderParam
// {
// public:
// 	CHE_PDF_OrderParam_DashPattern( HE_FLOAT * pDashArray, HE_DWORD dwArraySize, HE_FLOAT DashPhase, CHE_Allocator * pAllocator )
// 		: CHE_PDF_OrderParam( ORDER_DASH_PATTERN, pAllocator )
// 	{
// 		m_fDashPhase = DashPhase;
// 		if ( pDashArray == NULL )
// 		{
// 			m_pDashArray = NULL;
// 			m_dwArraySize = 0;
// 		}else{
// 			m_pDashArray = GetAllocator()->NewArray<HE_FLOAT>( dwArraySize );
// 			for ( HE_DWORD i = 0; i < dwArraySize; i++ )
// 			{
// 				m_pDashArray[i] = pDashArray[i];
// 			}
// 			m_dwArraySize = dwArraySize;
// 		}
// 	}
// 
// 	HE_VOID		SetDashArray( HE_FLOAT * pDashArray, HE_DWORD dwArraySize )
// 	{
// 		if ( m_pDashArray != NULL )
// 		{
// 			GetAllocator()->DeleteArray( m_pDashArray );
// 		}
// 		if ( pDashArray == NULL )
// 		{
// 			m_pDashArray = NULL;
// 			m_dwArraySize = 0;
// 		}else{
// 			m_pDashArray = GetAllocator()->NewArray<HE_FLOAT>( dwArraySize );
// 			for ( HE_DWORD i = 0; i < dwArraySize; i++ )
// 			{
// 				m_pDashArray[i] = pDashArray[i];
// 			}
// 			m_dwArraySize = dwArraySize;
// 		}
// 	}
// 	HE_FLOAT *	GetDashArray() { return m_pDashArray; }
// 	HE_DWORD	GetArraySize() { return m_dwArraySize; }
// 	HE_DWORD	SetDashPhase( HE_FLOAT dashPhase ) { m_fDashPhase = dashPhase; }
// 	HE_FLOAT	GetDashPhase() { return m_fDashPhase; }
// 
// private:
// 	HE_FLOAT *	m_pDashArray;
// 	HE_FLOAT	m_fDashPhase;
// 	HE_DWORD	m_dwArraySize;
// 
// 	friend CHE_Allocator;
// };
// 
// class CHE_PDF_OrderParam_Flatness : public CHE_PDF_OrderParam
// {
// public:
// 	CHE_PDF_OrderParam_Flatness( HE_BYTE flatness, CHE_Allocator * pAllocator = NULL )
// 		: CHE_PDF_OrderParam( ORDER_FALTNESS, pAllocator )
// 	{
// 		m_flatness = flatness;
// 	}
// 
// 	HE_VOID		SetFlatness( HE_BYTE flatness ) { m_flatness = flatness; }
// 	HE_BYTE		GetFlatness() { return m_flatness; }
// 
// private:
// 	HE_BYTE		m_flatness;
// 
// 	friend CHE_Allocator;
// };
// 
// class CHE_PDF_OrderParam_Intent : public CHE_PDF_OrderParam
// {
// public:
// 	CHE_PDF_OrderParam_Intent( CHE_Allocator * pAllocator = NULL )
// 		: CHE_PDF_OrderParam( ORDER_INTENT, pAllocator ) {}
// 
// private:
// 	friend CHE_Allocator;
// };
// 
// class CHE_PDF_OrderParam_GraphState : public CHE_PDF_OrderParam
// {
// public:
// 	CHE_PDF_OrderParam_GraphState( CHE_Allocator * pAllocator = NULL )
// 		: CHE_PDF_OrderParam( ORDER_SET_STATE, pAllocator ), m_array( pAllocator ) {}
// 
// 	HE_DWORD GetCount() { return m_array.GetCount(); }
// 
// 	HE_VOID Append( CHE_PDF_OrderParam * pOrderParam )
// 	{
// 		if ( pOrderParam )
// 		{
// 			m_array.Append( (HE_LPVOID)pOrderParam ); 
// 		}
// 	}
// 
// 	CHE_PDF_OrderParam * GetItem( HE_DWORD index ) { return (CHE_PDF_OrderParam *)m_array.GetItem( index ); }
// 
// private:
// 	CHE_PtrArray m_array;
// 
// 	friend CHE_Allocator;
// };
// 
// class CHE_PDF_OrderParam_PushState : public CHE_PDF_OrderParam
// {
// public:
// 	CHE_PDF_OrderParam_PushState( CHE_Allocator * pAllocator )
// 		: CHE_PDF_OrderParam( ORDER_PUSH_STATE, pAllocator ) {}
// private:
// 	friend CHE_Allocator;
// };
// 
// class CHE_PDF_OrderParam_PopState : public CHE_PDF_OrderParam
// {
// public:
// 	CHE_PDF_OrderParam_PopState( CHE_Allocator * pAllocator )
// 		: CHE_PDF_OrderParam( ORDER_POP_STATE, pAllocator ) {}
// private:
// 
// 	friend CHE_Allocator;
// };
// 
// class CHE_PDF_OrderParam_Matrix : public CHE_PDF_OrderParam
// {
// public:
// 	CHE_PDF_OrderParam_Matrix( CHE_Allocator * pAllocator = NULL )
// 		: CHE_PDF_OrderParam( ORDER_MATRIX, pAllocator )
// 	{
// 		m_fA = 1; m_fB = 0; m_fC = 0; m_fD = 1; m_fE = 0; m_fF = 0;
// 	}
// 
// 	CHE_PDF_OrderParam_Matrix(	HE_FLOAT a, HE_FLOAT b, HE_FLOAT c, HE_FLOAT d, 
// 								HE_FLOAT e, HE_FLOAT f, CHE_Allocator * pAllocator = NULL )
// 								: CHE_PDF_OrderParam( ORDER_MATRIX, pAllocator )
// 	{
// 		m_fA = a; m_fB = b; m_fC = c; m_fD = d; m_fE = e; m_fF = f;
// 	}
// 
// 	HE_VOID SetMatrix(	HE_FLOAT a, HE_FLOAT b, HE_FLOAT c, 
// 						HE_FLOAT d, HE_FLOAT e, HE_FLOAT f )
// 	{
// 		m_fA = a; m_fB = b; m_fC = c; m_fD = d; m_fE = e; m_fF = f;
// 	}
// 
// 	HE_FLOAT GetMatrixA() { return m_fA; }
// 	HE_FLOAT GetMatrixB() { return m_fB; }
// 	HE_FLOAT GetMatrixC() { return m_fC; }
// 	HE_FLOAT GetMatrixD() { return m_fD; }
// 	HE_FLOAT GetMatrixE() { return m_fE; }
// 	HE_FLOAT GetMatrixF() { return m_fF; }
// 
// private:
// 	HE_FLOAT m_fA;
// 	HE_FLOAT m_fB;
// 	HE_FLOAT m_fC;
// 	HE_FLOAT m_fD;
// 	HE_FLOAT m_fE;
// 	HE_FLOAT m_fF;
// 
// 	friend CHE_Allocator;
// };
// 
// class CHE_PDF_OrderParam_StrokeColor : public CHE_PDF_OrderParam
// {
// public:
// 	CHE_PDF_OrderParam_StrokeColor( CHE_PDF_Color color, CHE_Allocator * pAllocator = NULL )
// 		: CHE_PDF_OrderParam( ORDER_STROKE_COLOR, pAllocator ) { m_color = color; }
// 
// 	HE_VOID SetColor( CHE_PDF_Color color ) { m_color = color; }
// 	CHE_PDF_Color GetColor() { return m_color; }
// 
// private:
// 	CHE_PDF_Color m_color;
// 
// 	friend CHE_Allocator;
// };
// 
// class CHE_PDF_OrderParam_FillColor : public CHE_PDF_OrderParam
// {
// 	CHE_PDF_OrderParam_FillColor( CHE_PDF_Color color, CHE_Allocator * pAllocator = NULL )
// 		: CHE_PDF_OrderParam( ORDER_STROKE_COLOR, pAllocator ) { m_color = color; }
// 
// 	HE_VOID SetColor( CHE_PDF_Color color ) { m_color = color; }
// 	CHE_PDF_Color GetColor() { return m_color; }
// 
// private:
// 	CHE_PDF_Color m_color;
// 
// 	friend CHE_Allocator;
// };
// 
// class CHE_PDF_OrderObject : public CHE_PDF_ContentObject
// {
// public:
// 	static CHE_PDF_OrderObject* Create( CHE_Allocator * pAllocator = NULL )
// 	{
// 		if ( pAllocator )
// 		{
// 			return pAllocator->New<CHE_PDF_OrderObject>( pAllocator );
// 		}else{
// 			return new CHE_PDF_OrderObject( NULL );
// 		}
// 	}
// 
// 	HE_VOID SetType( PDF_ORDER_TYPE type ) { m_Type = type; }
// 	PDF_ORDER_TYPE	GetType() { return m_Type; }
// 
// 	HE_VOID	SetParam( CHE_PDF_OrderParam * pParam ) { m_pParam = pParam; }
// 	CHE_PDF_OrderParam * GetParam() { return m_pParam; } 
// 
// private:
// 	CHE_PDF_OrderObject( CHE_Allocator * pAllocator = NULL ) 
// 		: CHE_PDF_ContentObject( CONTENTOBJ_ORDER, pAllocator )
// 	{ m_Type = ORDER_INIT_STATE; m_pParam = NULL; }
// 	
// 	~CHE_PDF_OrderObject() {}
// 
// 	PDF_ORDER_TYPE			m_Type;
// 	CHE_PDF_OrderParam *	m_pParam;
// 
// 	friend class CHE_PDF_ContentObject;
// 	friend class CHE_Allocator;
// };
// 
// #endif