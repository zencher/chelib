//
//  che_pdf_page_layout.h
//  chelib
//
//  Created by zencher on 15/12/27.
//  Copyright © 2015年 com.peroit. All rights reserved.
//

#ifndef che_pdf_page_layout_h
#define che_pdf_page_layout_h

#include "../che_base.h"
#include "../che_graphics.h"
#include <vector>

enum HE_PDF_VIEW_PAGE_MODE
{
    PAGE_SINGLE,
    PAGE_DOUBLE,
    PAGE_SINGLE_SCROLL,
    PAGE_DOUBLE_SCROLL
};

enum HE_PDF_VIEW_ZOOM_MODE
{
    //ZOOM_MODE_ACTUAL,
    ZOOM_FIX,
    ZOOM_FIT
    //ZOOM_MODE_FIT_HEIGHT
};

enum HE_PDF_VIEW_ROTATE_MODE
{
    ROTATE_0,
    ROTATE_90,
    ROTATE_180,
    ROTATE_270
};

struct HE_PDF_PAGE_SIZE
{
    HE_FLOAT width;
    HE_FLOAT height;
};

struct HE_PDF_PAGE_RANGE
{
    HE_ULONG pageStart;
    HE_ULONG pageCount;
};

class CHE_Page_Rect
{
public:
    CHE_Page_Rect() : left(0), top(0), right(0), bottom(0) {}
    CHE_Page_Rect( HE_FLOAT l, HE_FLOAT t, HE_FLOAT r, HE_FLOAT b )
    : left(l), top(t), right(r), bottom(b) {}
    
    bool IsEmpty() const
    {
        return ( fabs(right-left) < FLT_EPSILON && fabs(bottom-top) < FLT_EPSILON );
    }
    
    bool IsUnion( const CHE_Page_Rect & rect )
    {
        CHE_Point p;
        p.x = (rect.left + rect.right) / 2;
        p.y = (rect.top + rect.bottom) / 2;
      
        if ( p.x > left && p.x < right && p.y > top && p.y < bottom )
        {
            return true;
        }
        return false;
    }
    
    void Union( const CHE_Page_Rect & rect )
    {
        if ( &rect != this )
        {
            if ( IsEmpty() )
            {
                *this = rect;
                return;
            }
            if ( !rect.IsEmpty() )
            {
                CHE_Page_Rect tmpRect;
                tmpRect.left = ( left < rect.left ) ? left : rect.left;
                tmpRect.top = ( top < rect.top ) ? top : rect.top;
                tmpRect.right = ( right > rect.right ) ? right : rect.right;
                tmpRect.bottom = ( bottom > rect.bottom ) ? bottom : rect.bottom;
                *this = tmpRect;
            }
        }
    }
    
    bool operator == ( const CHE_Page_Rect & rect ) const
    {
        if (	fabsf( left - rect.left ) <= FLT_EPSILON &&
                fabsf( right - rect.right ) <= FLT_EPSILON &&
                fabsf( top - rect.top ) <= FLT_EPSILON &&
                fabsf( bottom - rect.bottom ) <= FLT_EPSILON )
        {
            return true;
        }
        return false;
    }
    
    bool operator != ( const CHE_Page_Rect & rect ) const
    {
        return ! operator==( rect );
    }
    
    HE_FLOAT Width() const
    {
        return fabsf(right-left);
    }
    
    HE_FLOAT Height() const
    {
        return fabsf(bottom-top);
    }
    
    HE_FLOAT	left;
    HE_FLOAT	top;
    HE_FLOAT	right;
    HE_FLOAT	bottom;
};

class CHE_PDF_PageLayout
{
public:
    CHE_PDF_PageLayout();
    ~CHE_PDF_PageLayout();
    
    void SetScale( HE_FLOAT scale );
    void SetSpace( HE_UINT32 xspace, HE_UINT32 yspace );
    void SetPageMode( HE_PDF_VIEW_PAGE_MODE mode );
    void SetZoomMode( HE_PDF_VIEW_ZOOM_MODE mode );
    void SetRotateMode( HE_PDF_VIEW_ROTATE_MODE mode );
    void SetViewSize( HE_FLOAT width, HE_FLOAT height );
    
    HE_FLOAT GetScale() const { return mScale; }
    HE_PDF_VIEW_PAGE_MODE GetPageMode() const { return mMode; }
    HE_PDF_VIEW_ZOOM_MODE GetZoomMode() const { return mZoom; }
    HE_PDF_VIEW_ROTATE_MODE GetRotateMode() const { return mRotate; }
    
    void CleanPageSizeInfo();
    void AddPageSize( HE_FLOAT width, HE_FLOAT height );
    
    HE_PDF_PAGE_SIZE GetContentSize();
    
    void UpdatePageInViewRectInfo();
    
    CHE_Page_Rect GetPageRectInView(HE_ULONG pageIndex);
    
    HE_FLOAT GetPageScaleInView(HE_ULONG pageIndex);
    
    void SetCurPageRange( HE_ULONG pageIndex, HE_ULONG pageCount );
    HE_PDF_PAGE_RANGE GetCurPageRange();
    
private:
    HE_FLOAT GetPageWidthForRotateMode(HE_ULONG pageIndex);
    HE_FLOAT GetPageHeightForRotateMode(HE_ULONG pageIndex);
    
    void FinalAdjuest(const CHE_Page_Rect & bbox);
    
    void UpdatePageInfoSinglePage();
    void UpdatePageInfoSinglePageScroll();
    void UpdatePageInfoDoublePage();
    void UpdatePageInfoDoublePageScroll();
    
    HE_FLOAT                mScale;
    HE_UINT32               mSpaceX;
    HE_UINT32               mSpaceY;
    HE_PDF_VIEW_PAGE_MODE   mMode;
    HE_PDF_VIEW_ZOOM_MODE   mZoom;
    HE_PDF_VIEW_ROTATE_MODE mRotate;
    HE_FLOAT                mViewWidth;
    HE_FLOAT                mViewHeight;
    HE_FLOAT                mContentWidth;
    HE_FLOAT                mContentHeight;
    std::vector<HE_PDF_PAGE_SIZE>       mPageSizes;
    HE_BOOL                             mNeedUpdate;
    std::vector<CHE_Page_Rect>          mPageRectInView;
    std::vector<HE_FLOAT>               mPageScaleInView;
    
    HE_ULONG                   mCurPageStart;
    HE_ULONG                   mCurPageCountInView;
};


#endif /* che_pdf_page_layout_h */
