//
//  Cpdf_page_layout.h
//  chelib
//
//  Created by zencher on 15/12/27.
//  Copyright © 2015年 com.peroit. All rights reserved.
//

#ifndef _CHELIB_PDF_PAGE_LAYOUT_H_
#define _CHELIB_PDF_PAGE_LAYOUT_H_

#include "../base.h"

#include <vector>
#include <cfloat>
#include <cmath>

namespace chelib {

enum PDF_VIEW_PAGE_MODE
{
    PAGE_SINGLE,
    PAGE_DOUBLE,
    PAGE_SINGLE_SCROLL,
    PAGE_DOUBLE_SCROLL
};

enum PDF_VIEW_ZOOM_MODE
{
    //ZOOM_MODE_ACTUAL,
    ZOOM_FIX,
    ZOOM_FIT
    //ZOOM_MODE_FIT_HEIGHT
};

enum PDF_VIEW_ROTATE_MODE
{
    ROTATE_0,
    ROTATE_90,
    ROTATE_180,
    ROTATE_270
};

struct PDF_PAGE_SIZE
{
    FLOAT width;
    FLOAT height;
};

struct PDF_PAGE_RANGE
{
    size_t pageStart;
    size_t pageCount;
};

class CPage_Rect
{
public:
    CPage_Rect() : left(0), top(0), right(0), bottom(0) {}
    CPage_Rect( FLOAT l, FLOAT t, FLOAT r, FLOAT b )
    : left(l), top(t), right(r), bottom(b) {}
    
    bool IsEmpty() const
    {
        return ( fabs(right-left) < FLT_EPSILON && fabs(bottom-top) < FLT_EPSILON );
    }
    
    bool IsUnion( const CPage_Rect & rect )
    {
        FLOAT x = 0, y = 0;
        x = (rect.left + rect.right) / 2;
        y = (rect.top + rect.bottom) / 2;
      
        if ( x > left && x < right && y > top && y < bottom )
        {
            return true;
        }
        return false;
    }
    
    void Union( const CPage_Rect & rect )
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
                CPage_Rect tmpRect;
                tmpRect.left = ( left < rect.left ) ? left : rect.left;
                tmpRect.top = ( top < rect.top ) ? top : rect.top;
                tmpRect.right = ( right > rect.right ) ? right : rect.right;
                tmpRect.bottom = ( bottom > rect.bottom ) ? bottom : rect.bottom;
                *this = tmpRect;
            }
        }
    }
    
    bool operator == ( const CPage_Rect & rect ) const
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
    
    bool operator != ( const CPage_Rect & rect ) const
    {
        return ! operator==( rect );
    }
    
    FLOAT Width() const
    {
        return fabsf(right-left);
    }
    
    FLOAT Height() const
    {
        return fabsf(bottom-top);
    }
    
    FLOAT	left;
    FLOAT	top;
    FLOAT	right;
    FLOAT	bottom;
};

class CPDF_ThumbnailPageLayout
{
public:
    CPDF_ThumbnailPageLayout();
    ~CPDF_ThumbnailPageLayout();
    
    void SetViewSize( FLOAT width, FLOAT height );
    
    void CleanPageSizeInfo();
    void AddPageSize( FLOAT width, FLOAT height );
    
    PDF_PAGE_SIZE GetContentSize();
    
    void UpdatePageInViewRectInfo();
    
    CPage_Rect GetPageRectInView(size_t pageIndex);
    
    FLOAT GetPageScaleInView(size_t pageIndex);
    
private:
    void FinalAdjuest(const CPage_Rect & bbox);
    
    uint32                           mSpaceX;
    uint32                           mSpaceY;
    FLOAT                            mViewWidth;
    FLOAT                            mViewHeight;
    FLOAT                            mContentWidth;
    FLOAT                            mContentHeight;
    
    std::vector<PDF_PAGE_SIZE>    mPageSizes;
    std::vector<CPage_Rect>       mPageRectInView;
    std::vector<FLOAT>               mPageScaleInView;
};

class CPDF_PageLayout
{
public:
    CPDF_PageLayout();
    ~CPDF_PageLayout();
    
    void SetScale( FLOAT scale );
    void SetSpace( uint32 xspace, uint32 yspace );
    void SetPageMode( PDF_VIEW_PAGE_MODE mode );
    void SetZoomMode( PDF_VIEW_ZOOM_MODE mode );
    void SetRotateMode( PDF_VIEW_ROTATE_MODE mode );
    void SetViewSize( FLOAT width, FLOAT height );
    
    FLOAT GetScale() const { return mScale; }
    PDF_VIEW_PAGE_MODE GetPageMode() const { return mMode; }
    PDF_VIEW_ZOOM_MODE GetZoomMode() const { return mZoom; }
    PDF_VIEW_ROTATE_MODE GetRotateMode() const { return mRotate; }
    
    void CleanPageSizeInfo();
    void AddPageSize( FLOAT width, FLOAT height );
    
    PDF_PAGE_SIZE GetContentSize();
    
    void UpdatePageInViewRectInfo();
    
    CPage_Rect GetPageRectInView(size_t pageIndex);
    
    FLOAT GetPageScaleInView(size_t pageIndex);
    
    void SetCurPageRange( size_t pageIndex, size_t pageCount );
    PDF_PAGE_RANGE GetCurPageRange();
    
private:
    FLOAT GetPageWidthForRotateMode(size_t pageIndex);
    FLOAT GetPageHeightForRotateMode(size_t pageIndex);
    
    void FinalAdjuest(const CPage_Rect & bbox);
    
    void UpdatePageInfoSinglePage();
    void UpdatePageInfoSinglePageScroll();
    void UpdatePageInfoDoublePage();
    void UpdatePageInfoDoublePageScroll();
    
    FLOAT                   mScale;
    uint32                  mSpaceX;
    uint32                  mSpaceY;
    PDF_VIEW_PAGE_MODE   mMode;
    PDF_VIEW_ZOOM_MODE   mZoom;
    PDF_VIEW_ROTATE_MODE mRotate;
    FLOAT                   mViewWidth;
    FLOAT                   mViewHeight;
    FLOAT                   mContentWidth;
    FLOAT                   mContentHeight;
    std::vector<PDF_PAGE_SIZE>    mPageSizes;
    bool                             mNeedUpdate;
    std::vector<CPage_Rect>       mPageRectInView;

    std::vector<FLOAT>               mPageScaleInView;
    
    size_t                   mCurPageStart;
    size_t                   mCurPageCountInView;
};

}//namespace

#endif