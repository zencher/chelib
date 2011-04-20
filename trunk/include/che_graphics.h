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

#define GRAPHTYPE_LINE			0
#define GRAPHTYPE_RECTANGLE		1
#define GRAPHTYPE_PATH			2

class CHE_GraphicsObject : public CHE_Object
{
public :
	HE_BYTE	GetType() { return m_type; } 
	HE_BYTE m_type;
};

class CHE_Line : public CHE_GraphicsObject
{
public:
	CHE_Line( HE_LONG lStartX, HE_LONG lStartY, HE_LONG lEndX, HE_LONG lEndY )
		{ m_lStartX = lStartX; m_lStartY = lStartY; m_lEndX = lEndX; m_lEndY = lEndY; m_type = GRAPHTYPE_LINE; }
	HE_LONG m_lStartX;
	HE_LONG m_lStartY;
	HE_LONG m_lEndX;
	HE_LONG m_lEndY;
};

class CHE_Rectangle : public CHE_GraphicsObject
{
public:
	CHE_Rectangle( HE_LONG leftTopX, HE_LONG leftTopY, HE_DWORD dwWidth, HE_DWORD dwHeight )
		{ m_lLeftTopX = leftTopX; m_lLeftTopY = leftTopY; m_dwWidth = dwWidth; m_dwHeight = dwHeight;m_type = GRAPHTYPE_RECTANGLE;  }
	
	HE_LONG m_lLeftTopX;
	HE_LONG m_lLeftTopY;
	HE_DWORD m_dwWidth;
	HE_DWORD m_dwHeight;
};

// class CHE_BezierLine : public CHE_GraphicsObject
// {
// 
// };

class CHE_Path : public CHE_GraphicsObject
{
public:
	HE_VOID		AddLine( CHE_Line & line );
	HE_VOID		AddRectangle( CHE_Rectangle & rect );
	CHE_GraphicsObject * GetGraph( HE_DWORD index ) { return (CHE_GraphicsObject *)m_objArray.GetItem( index ); }
	HE_DWORD	GetGraphCount() { return m_objArray.GetCount(); }

private:
	CHE_PtrArray	m_objArray; 
};

#endif