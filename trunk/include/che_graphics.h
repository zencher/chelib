#ifndef _CHE_GRAPH_H_
#define _CHE_GRAPH_H_

#include "che_base.h"
#include "che_datastructure.h"

//class CHE_Graphics;
//class CHE_Pen;
//class CHE_Font;
//class CHE_Color;
//class CHE_Point;
//class CHE_Rect;

#define GRAPHTYPE_INVALID		0
#define GRAPHTYPE_LINE			1
#define GRAPHTYPE_BCURVE		2
#define GRAPHTYPE_RECTANGLE		3
#define GRAPHTYPE_PATH			4

class CHE_GraphicsObject : public CHE_Object
{
public :
	CHE_GraphicsObject( HE_BYTE type = GRAPHTYPE_INVALID, HE_BOOL bConnect = FALSE, CHE_Allocator * pAllocator = NULL )
		: CHE_Object( pAllocator )
	{ m_type = type; m_bConnect = bConnect; }

	HE_BYTE	GetType() { return m_type; }

	HE_BOOL	IsConnect() { return m_bConnect; }

	HE_VOID Release();

private:
	HE_BYTE	m_type;
	HE_BOOL	m_bConnect;
};

class CHE_Line : public CHE_GraphicsObject
{
public:
	CHE_Line( HE_FLOAT StartX, HE_FLOAT StartY, HE_FLOAT EndX, HE_FLOAT EndY, HE_BOOL bConnect = FALSE, CHE_Allocator * pAllocator = NULL ) 
		: CHE_GraphicsObject( GRAPHTYPE_LINE, bConnect, pAllocator )
	{ m_fStartX = StartX; m_fStartY = StartY; m_fEndX = EndX; m_fEndY = EndY; }
	HE_FLOAT m_fStartX;
	HE_FLOAT m_fStartY;
	HE_FLOAT m_fEndX;
	HE_FLOAT m_fEndY;
};

class CHE_BCurve : public CHE_GraphicsObject
{
public:
	CHE_BCurve( HE_FLOAT X1, HE_FLOAT Y1, HE_FLOAT X2, HE_FLOAT Y2, HE_FLOAT X3, HE_FLOAT Y3,
				HE_FLOAT X4, HE_FLOAT Y4, HE_BOOL bConnect = FALSE, CHE_Allocator * pAllocator = NULL )
		: CHE_GraphicsObject( GRAPHTYPE_BCURVE, bConnect, pAllocator )
	{ m_fX1 = X1, m_fY1 = Y1; m_fX2 = X2; m_fY2 = Y2; m_fX3 = X3; m_fY3 = Y3; m_fX4 = X4; m_fY4 = Y4; }

	HE_FLOAT m_fX1;
	HE_FLOAT m_fY1;
	HE_FLOAT m_fX2;
	HE_FLOAT m_fY2;
	HE_FLOAT m_fX3;
	HE_FLOAT m_fY3;
	HE_FLOAT m_fX4;
	HE_FLOAT m_fY4;
};

class CHE_Rectangle : public CHE_GraphicsObject
{
public:
	CHE_Rectangle( HE_FLOAT leftTopX, HE_FLOAT leftTopY, HE_FLOAT width, HE_FLOAT height, HE_BOOL bConnect = FALSE, 
		CHE_Allocator * pAllocator = NULL ) : CHE_GraphicsObject( GRAPHTYPE_RECTANGLE, bConnect, pAllocator )
		{ m_fLeftTopX = leftTopX; m_fLeftTopY = leftTopY; m_fWidth = width; m_fHeight = height; }
	
	HE_FLOAT m_fLeftTopX;
	HE_FLOAT m_fLeftTopY;
	HE_FLOAT m_fWidth;
	HE_FLOAT m_fHeight;
};

class CHE_Path : public CHE_GraphicsObject
{
public:
	CHE_Path( CHE_Allocator * pAllocator = NULL ) : CHE_GraphicsObject( GRAPHTYPE_PATH, FALSE, pAllocator ), m_objArray( pAllocator ) {}

	HE_VOID		AddLine( CHE_Line & line );
	
	HE_VOID		AddBCrave( CHE_BCurve & crave );
	
	HE_VOID		AddRectangle( CHE_Rectangle & rect );

	CHE_GraphicsObject * GetGraph( HE_DWORD index ) { return (CHE_GraphicsObject *)m_objArray.GetItem( index ); }
	
	HE_DWORD	GetGraphCount() { return m_objArray.GetCount(); }

private:
	CHE_PtrArray	m_objArray; 
};

#endif