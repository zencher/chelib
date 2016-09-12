//
//  Cpdf_page_layout.cpp
//  chelib
//
//  Created by zencher on 15/12/27.
//  Copyright © 2015年 com.peroit. All rights reserved.
//

#include "../../include/pdf/pdf_page_layout.h"

namespace chelib {

CPDF_ThumbnailPageLayout::CPDF_ThumbnailPageLayout()
 : mSpaceX(20), mSpaceY(30), mViewWidth(100), mViewHeight(100), mContentWidth(100), mContentHeight(100) {}

CPDF_ThumbnailPageLayout::~CPDF_ThumbnailPageLayout() {}

void CPDF_ThumbnailPageLayout::SetViewSize(FLOAT width, FLOAT height)
{
    mViewWidth = width;
    mViewHeight = height;
}

void CPDF_ThumbnailPageLayout::CleanPageSizeInfo()
{
    mPageSizes.clear();
}

void CPDF_ThumbnailPageLayout::AddPageSize(FLOAT width, FLOAT height)
{
    PDF_PAGE_SIZE size;
    size.width = width;
    size.height = height;
    mPageSizes.push_back(size);
}

PDF_PAGE_SIZE CPDF_ThumbnailPageLayout::GetContentSize()
{
    PDF_PAGE_SIZE size;
    size.width = mContentWidth;
    size.height = mContentHeight;
    return size;
}

void CPDF_ThumbnailPageLayout::UpdatePageInViewRectInfo()
{
    mPageRectInView.clear();
    mPageScaleInView.clear();
    
    CPage_Rect bbox, page;
    FLOAT offsetX = mSpaceX, offsetY = mSpaceY, tmpScale = 1.0f;
    /*FLOAT width = mViewWidth, height = mViewHeight;
    if (mViewWidth >= 150) {
        width = 150;
        height =
    }*/
    for ( size_t index = 0; index < mPageSizes.size(); ++index )
    {
        page.left = page.right = offsetX;
        if (mViewWidth >= 150)
        {
            tmpScale = (150 - 2 * mSpaceX) / mPageSizes[index].width;
            page.right += (150 - 2 * mSpaceX);
        }else{
            page.right += (mViewWidth - 2 * mSpaceX);
            tmpScale = (mViewWidth - 2 * mSpaceX) / mPageSizes[index].width;
        }
        page.top = page.bottom = offsetY;
        page.bottom += mPageSizes[index].height * tmpScale;
        offsetY = page.bottom + mSpaceY;
        mPageRectInView.push_back( page );
        mPageScaleInView.push_back( tmpScale );
        bbox.Union( page );
    }
    FinalAdjuest(bbox);
}

CPage_Rect CPDF_ThumbnailPageLayout::GetPageRectInView(size_t pageIndex)
{
    return mPageRectInView[pageIndex];
}

FLOAT CPDF_ThumbnailPageLayout::GetPageScaleInView(size_t pageIndex)
{
    return mPageScaleInView[pageIndex];
}

void CPDF_ThumbnailPageLayout::FinalAdjuest(const CPage_Rect & bbox)
{
    FLOAT offsetX = 0;
    mContentWidth = bbox.Width() + 2 * mSpaceX;
    mContentHeight = bbox.Height() + 2 * mSpaceY;
    if ( mContentWidth < mViewWidth )
    {
        offsetX = (mViewWidth - mContentWidth) / 2;
        for ( size_t index = 0; index < mPageSizes.size(); ++index )
        {
            mPageRectInView[index].left += offsetX;
            mPageRectInView[index].right += offsetX;
        }
        mContentWidth = mViewWidth;
    }
}


CPDF_PageLayout::CPDF_PageLayout()
 :  mScale(1.0f), mSpaceX(10), mSpaceY(10), mMode(PAGE_SINGLE_SCROLL), mZoom(ZOOM_FIX),
mRotate(ROTATE_0), mViewWidth(100), mViewHeight(100), mNeedUpdate(false), mCurPageStart(0), mCurPageCountInView(1) { }

CPDF_PageLayout::~CPDF_PageLayout() { }

void CPDF_PageLayout::SetScale( FLOAT scale )
{
    mScale = scale;
    mNeedUpdate = TRUE;
}

void CPDF_PageLayout::SetSpace( uint32 xspace, uint32 yspace )
{
    mSpaceX = xspace;
    mSpaceY = yspace;
    mNeedUpdate = TRUE;
}

void CPDF_PageLayout::SetPageMode( PDF_VIEW_PAGE_MODE mode )
{
    mMode = mode;
    mNeedUpdate = TRUE;
}

void CPDF_PageLayout::SetZoomMode( PDF_VIEW_ZOOM_MODE mode )
{
    mZoom = mode;
    mNeedUpdate = TRUE;
}

void CPDF_PageLayout::SetRotateMode( PDF_VIEW_ROTATE_MODE mode )
{
    mRotate = mode;
    mNeedUpdate = TRUE;
}

void CPDF_PageLayout::SetViewSize( FLOAT width, FLOAT height )
{
    mViewWidth = width;
    mViewHeight = height;
    mNeedUpdate = TRUE;
}

void CPDF_PageLayout::CleanPageSizeInfo()
{
    mPageSizes.clear();
}

void CPDF_PageLayout::AddPageSize( FLOAT width, FLOAT height )
{
    PDF_PAGE_SIZE size;
    size.width = width;
    size.height = height;
    mPageSizes.push_back( size );
}

void CPDF_PageLayout::UpdatePageInfoSinglePage()
{
    CPage_Rect bbox;
    CPage_Rect page;
    FLOAT offsetX = mSpaceX, offsetY = mSpaceY;
    FLOAT tmpScale = mScale;
    
    mCurPageCountInView = 1;
    if ( mZoom == ZOOM_FIT )
    {
        page.left = page.right = offsetX;
        page.right += (mViewWidth - 2 * mSpaceX);
        tmpScale = (mViewWidth - 2 * mSpaceX) / GetPageWidthForRotateMode(mCurPageStart);
        page.top = page.bottom = offsetY;
        page.bottom += GetPageHeightForRotateMode(mCurPageStart) * tmpScale;
        mPageRectInView.push_back( page );
        mPageScaleInView.push_back( tmpScale );
        bbox.Union( page );
    }else if( mZoom == ZOOM_FIX )
    {
        page.left = page.right = offsetX;
        page.right += GetPageWidthForRotateMode(mCurPageStart) * mScale;
        page.top = page.bottom = offsetY;
        page.bottom += GetPageHeightForRotateMode(mCurPageStart) * mScale;
        mPageRectInView.push_back( page );
        mPageScaleInView.push_back( tmpScale );
        bbox.Union( page );
    }
    FinalAdjuest(bbox);
}

void CPDF_PageLayout::UpdatePageInfoSinglePageScroll()
{
    CPage_Rect bbox, page;
    FLOAT offsetX = mSpaceX, offsetY = mSpaceY, tmpScale = mScale;
    
    mCurPageStart = 0;
    mCurPageCountInView = mPageSizes.size();
    if ( mZoom == ZOOM_FIT )
    {
        for ( size_t index = 0; index < mPageSizes.size(); ++index )
        {
            page.left = page.right = offsetX;
            page.right += (mViewWidth - 2 * mSpaceX);
            tmpScale = (mViewWidth - 2 * mSpaceX) / GetPageWidthForRotateMode(index);
            page.top = page.bottom = offsetY;
            page.bottom += GetPageHeightForRotateMode(index) * tmpScale;
            offsetY = page.bottom + mSpaceY;
            mPageRectInView.push_back( page );
            mPageScaleInView.push_back( tmpScale );
            bbox.Union( page );
        }
    }else if ( mZoom == ZOOM_FIX )
    {
        for ( size_t index = 0; index < mPageSizes.size(); ++index )
        {
            page.left = page.right = offsetX;
            page.right += GetPageWidthForRotateMode(index) * tmpScale;
            page.top = page.bottom = offsetY;
            page.bottom += GetPageHeightForRotateMode(index) * tmpScale;
            offsetY = page.bottom + mSpaceY;
            mPageRectInView.push_back( page );
            mPageScaleInView.push_back( tmpScale );
            bbox.Union( page );
        }
    }
    FinalAdjuest(bbox);
}

void CPDF_PageLayout::UpdatePageInfoDoublePage()
{
    CPage_Rect bbox, pageLeft, pageRight;
    FLOAT offsetX = mSpaceX, offsetY = mSpaceY, tmpScale = mScale;
    
    mCurPageCountInView = 2;
    if ( mCurPageStart + mCurPageCountInView > mPageSizes.size() )
    {
        mCurPageCountInView = 1;
        return UpdatePageInfoSinglePage();
    }
    
    if ( mZoom == ZOOM_FIT )
    {
        for ( size_t index = mCurPageStart; index < mCurPageStart + mCurPageCountInView; ++index )
        {
            if ( index % 2 == 0 )
            {
                pageLeft.left = pageLeft.right = offsetX;
                pageLeft.right += ( (mViewWidth - mSpaceX * 3) / 2 );
                tmpScale = pageLeft.Width() / GetPageWidthForRotateMode(index);
                pageLeft.top = pageLeft.bottom = offsetY;
                pageLeft.bottom += GetPageHeightForRotateMode(index) * tmpScale;
                mPageRectInView.push_back( pageLeft );
                mPageScaleInView.push_back( tmpScale );
                bbox.Union( pageLeft );
                offsetX = pageLeft.right + mSpaceX;
            }else{
                pageRight.left = pageRight.right = offsetX;
                pageRight.right += ( (mViewWidth - mSpaceX * 3) / 2 );
                tmpScale = pageRight.Width() / GetPageWidthForRotateMode(index);
                pageRight.top = pageRight.bottom = offsetY;
                pageRight.bottom += GetPageHeightForRotateMode(index) * tmpScale;
                mPageRectInView.push_back( pageRight );
                mPageScaleInView.push_back( tmpScale );
                bbox.Union( pageRight );
            }
        }
    }else if ( mZoom == ZOOM_FIX )
    {
        for ( size_t index = mCurPageStart; index < mCurPageStart + mCurPageCountInView; ++index )
        {
            if ( index % 2 == 0 )
            {
                pageLeft.left = pageLeft.right = offsetX;
                pageLeft.right += GetPageWidthForRotateMode(index) * tmpScale;
                pageLeft.top = pageLeft.bottom = offsetY;
                pageLeft.bottom += GetPageHeightForRotateMode(index) * tmpScale;
                mPageRectInView.push_back( pageLeft );
                mPageScaleInView.push_back( mScale );
                bbox.Union( pageLeft );
                offsetX = pageLeft.right + mSpaceX;
            }else{
                pageRight.left = pageRight.right = offsetX;
                pageRight.right += GetPageWidthForRotateMode(index) * tmpScale;
                pageRight.top = pageRight.bottom = offsetY;
                pageRight.bottom += GetPageHeightForRotateMode(index) * tmpScale;
                mPageRectInView.push_back( pageRight );
                mPageScaleInView.push_back( tmpScale );
                bbox.Union( pageRight );
            }
        }
    }
    FinalAdjuest(bbox);
}

void CPDF_PageLayout::UpdatePageInfoDoublePageScroll()
{
    CPage_Rect bbox, pageLeft, pageRight;
    FLOAT offsetX = mSpaceX, offsetY = mSpaceY, tmpScale = mScale;
    
    mCurPageStart = 0;
    mCurPageCountInView = mPageSizes.size();
    if ( mZoom == ZOOM_FIT )
    {
        for ( size_t index = 0; index < mPageSizes.size(); ++index )
        {
            if ( index % 2 == 0 )
            {
                pageLeft.left = pageLeft.right = offsetX;
                pageLeft.right += ( (mViewWidth - mSpaceX * 3) / 2 );
                tmpScale = pageLeft.Width() / GetPageWidthForRotateMode(index);
                pageLeft.top = pageLeft.bottom = offsetY;
                pageLeft.bottom += GetPageHeightForRotateMode(index) * tmpScale;
                mPageRectInView.push_back( pageLeft );
                mPageScaleInView.push_back( tmpScale );
                bbox.Union( pageLeft );
                offsetX = pageLeft.right + mSpaceX;
            }else{
                pageRight.left = pageRight.right = offsetX;
                pageRight.right += ( (mViewWidth - mSpaceX * 3) / 2 );
                tmpScale = pageRight.Width() / GetPageWidthForRotateMode(index);
                pageRight.top = pageRight.bottom = offsetY;
                pageRight.bottom += GetPageHeightForRotateMode(index) * tmpScale;
                mPageRectInView.push_back( pageRight );
                mPageScaleInView.push_back( tmpScale );
                bbox.Union( pageRight );
                offsetX = mSpaceX;
                offsetY = ( ( pageRight.bottom > pageLeft.bottom ) ? pageRight.bottom : pageLeft.bottom ) + mSpaceY;
            }
        }
    }else if ( mZoom == ZOOM_FIX )
    {
        for ( size_t index = 0; index < mPageSizes.size(); ++index )
        {
            if ( index % 2 == 0 )
            {
                pageLeft.left = pageLeft.right = offsetX;
                pageLeft.right += GetPageWidthForRotateMode(index) * tmpScale;
                pageLeft.top = pageLeft.bottom = offsetY;
                pageLeft.bottom += GetPageHeightForRotateMode(index) * tmpScale;
                mPageRectInView.push_back( pageLeft );
                mPageScaleInView.push_back( tmpScale );
                bbox.Union( pageLeft );
                offsetX = pageLeft.right + mSpaceX;
            }else{
                pageRight.left = pageRight.right = offsetX;
                pageRight.right += GetPageWidthForRotateMode(index) * mScale;
                pageRight.top = pageRight.bottom = offsetY;
                pageRight.bottom += GetPageHeightForRotateMode(index) * mScale;
                mPageRectInView.push_back( pageRight );
                mPageScaleInView.push_back( tmpScale );
                bbox.Union( pageRight );
                offsetX = mSpaceX;
                offsetY = ( ( pageRight.bottom > pageLeft.bottom ) ? pageRight.bottom : pageLeft.bottom ) + mSpaceY;
            }
        }
    }
    FinalAdjuest(bbox);
}

void CPDF_PageLayout::UpdatePageInViewRectInfo()
{
    mPageRectInView.clear();
    mPageScaleInView.clear();
    
    if ( mViewWidth <= 3 * mSpaceX || mViewHeight <= 3 * mSpaceY )
    {
        return;
    }
    
    switch ( mMode )
    {
        case PAGE_SINGLE:
            UpdatePageInfoSinglePage();
            break;
        case PAGE_DOUBLE:
            UpdatePageInfoDoublePage();
            break;
        case PAGE_SINGLE_SCROLL:
            UpdatePageInfoSinglePageScroll();
            break;
        case PAGE_DOUBLE_SCROLL:
            UpdatePageInfoDoublePageScroll();
            break;
        default: break;
    }
}

PDF_PAGE_SIZE CPDF_PageLayout::GetContentSize()
{
    PDF_PAGE_SIZE size;
    size.width = mContentWidth;
    size.height = mContentHeight;
    return size;
}

CPage_Rect CPDF_PageLayout::GetPageRectInView(size_t pageIndex)
{
    return mPageRectInView[pageIndex-mCurPageStart];
}

FLOAT CPDF_PageLayout::GetPageScaleInView(size_t pageIndex)
{
    return mPageScaleInView[pageIndex-mCurPageStart];
}

void CPDF_PageLayout::SetCurPageRange( size_t pageIndex, size_t pageCount )
{
    mCurPageStart = pageIndex;
    mCurPageCountInView = pageCount;
}

PDF_PAGE_RANGE CPDF_PageLayout::GetCurPageRange()
{
    PDF_PAGE_RANGE range;
    range.pageStart = mCurPageStart;
    range.pageCount = mCurPageCountInView;
    return range;
}

inline FLOAT CPDF_PageLayout::GetPageWidthForRotateMode(size_t pageIndex)
{
    if ( mRotate == ROTATE_0 || mRotate == ROTATE_180 ) {
        return mPageSizes[pageIndex].width;
    }
    return mPageSizes[pageIndex].height;
}

inline FLOAT CPDF_PageLayout::GetPageHeightForRotateMode(size_t pageIndex)
{
    if ( mRotate == ROTATE_0 || mRotate == ROTATE_180 ) {
        return mPageSizes[pageIndex].height;
    }
    return mPageSizes[pageIndex].width;
}

void CPDF_PageLayout::FinalAdjuest(const CPage_Rect & bbox)
{
    FLOAT offsetX = 0, offsetY = 0;
    mContentWidth = bbox.Width() + 2 * mSpaceX;
    mContentHeight = bbox.Height() + 2 * mSpaceY;
    if ( mContentWidth < mViewWidth )
    {
        offsetX = (mViewWidth - mContentWidth) / 2;
        for ( size_t index = mCurPageStart; index < mCurPageStart + mCurPageCountInView; ++index )
        {
            mPageRectInView[index-mCurPageStart].left += offsetX;
            mPageRectInView[index-mCurPageStart].right += offsetX;
        }
        mContentWidth = mViewWidth;
    }
    if ( mContentHeight < mViewHeight )
    {
        offsetY = (mViewHeight - mContentHeight) / 2;
        for ( size_t index = mCurPageStart; index < mCurPageStart + mCurPageCountInView; ++index )
        {
            mPageRectInView[index-mCurPageStart].top += offsetY;
            mPageRectInView[index-mCurPageStart].bottom += offsetY;
        }
        mContentHeight = mViewHeight;
    }
}

}//namespace