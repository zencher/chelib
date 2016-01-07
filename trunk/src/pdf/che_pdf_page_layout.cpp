//
//  che_pdf_page_layout.cpp
//  chelib
//
//  Created by zencher on 15/12/27.
//  Copyright © 2015年 com.peroit. All rights reserved.
//

#include "../../include/pdf/che_pdf_page_layout.h"


CHE_PDF_PageLayout::CHE_PDF_PageLayout()
 :  mScale(1.0f), mSpaceX(10), mSpaceY(10), mMode(PAGE_SINGLE_SCROLL), mZoom(ZOOM_FIX),
mRotate(ROTATE_0), mViewWidth(100), mViewHeight(100), mNeedUpdate(FALSE), mCurPageStart(0), mCurPageCountInView(1) { }

CHE_PDF_PageLayout::~CHE_PDF_PageLayout() { }

void CHE_PDF_PageLayout::SetScale( HE_FLOAT scale )
{
    mScale = scale;
    mNeedUpdate = TRUE;
}

void CHE_PDF_PageLayout::SetSpace( HE_UINT32 xspace, HE_UINT32 yspace )
{
    mSpaceX = xspace;
    mSpaceY = yspace;
    mNeedUpdate = TRUE;
}

void CHE_PDF_PageLayout::SetPageMode( HE_PDF_VIEW_PAGE_MODE mode )
{
    mMode = mode;
    mNeedUpdate = TRUE;
}

void CHE_PDF_PageLayout::SetZoomMode( HE_PDF_VIEW_ZOOM_MODE mode )
{
    mZoom = mode;
    mNeedUpdate = TRUE;
}

void CHE_PDF_PageLayout::SetRotateMode( HE_PDF_VIEW_ROTATE_MODE mode )
{
    mRotate = mode;
    mNeedUpdate = TRUE;
}

void CHE_PDF_PageLayout::SetViewSize( HE_FLOAT width, HE_FLOAT height )
{
    mViewWidth = width;
    mViewHeight = height;
    mNeedUpdate = TRUE;
}

void CHE_PDF_PageLayout::CleanPageSizeInfo()
{
    mPageSizes.clear();
}

void CHE_PDF_PageLayout::AddPageSize( HE_FLOAT width, HE_FLOAT height )
{
    HE_PDF_PAGE_SIZE size;
    size.width = width;
    size.height = height;
    mPageSizes.push_back( size );
}

void CHE_PDF_PageLayout::UpdatePageInfoSinglePage()
{
    CHE_Page_Rect bbox;
    CHE_Page_Rect page;
    HE_FLOAT offsetX = mSpaceX, offsetY = mSpaceY;
    HE_FLOAT tmpScale = mScale;
    
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
        mPageScaleInView.push_back( mScale );
        bbox.Union( page );
    }
    FinalAdjuest(bbox);
}

void CHE_PDF_PageLayout::UpdatePageInfoSinglePageScroll()
{
    CHE_Page_Rect bbox, page;
    HE_FLOAT offsetX = mSpaceX, offsetY = mSpaceY, tmpScale = mScale;
    
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
            page.right += GetPageWidthForRotateMode(index) * mScale;
            page.top = page.bottom = offsetY;
            page.bottom += GetPageHeightForRotateMode(index) * mScale;
            offsetY = page.bottom + mSpaceY;
            mPageRectInView.push_back( page );
            mPageScaleInView.push_back( mScale );
            bbox.Union( page );
        }
    }
    FinalAdjuest(bbox);
}

void CHE_PDF_PageLayout::UpdatePageInfoDoublePage()
{
    CHE_Page_Rect bbox, pageLeft, pageRight;
    HE_FLOAT offsetX = mSpaceX, offsetY = mSpaceY, tmpScale = mScale;
    
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
                pageLeft.right += GetPageWidthForRotateMode(index) * mScale;
                pageLeft.top = pageLeft.bottom = offsetY;
                pageLeft.bottom += GetPageHeightForRotateMode(index) * mScale;
                mPageRectInView.push_back( pageLeft );
                mPageScaleInView.push_back( mScale );
                bbox.Union( pageLeft );
                offsetX = pageLeft.right + mSpaceX;
            }else{
                pageRight.left = pageRight.right = offsetX;
                pageRight.right += GetPageWidthForRotateMode(index) * mScale;
                pageRight.top = pageRight.bottom = offsetY;
                pageRight.bottom += GetPageHeightForRotateMode(index) * mScale;
                mPageRectInView.push_back( pageRight );
                mPageScaleInView.push_back( mScale );
                bbox.Union( pageRight );
            }
        }
    }
    FinalAdjuest(bbox);
}

void CHE_PDF_PageLayout::UpdatePageInfoDoublePageScroll()
{
    CHE_Page_Rect bbox, pageLeft, pageRight;
    HE_FLOAT offsetX = mSpaceX, offsetY = mSpaceY, tmpScale = mScale;
    
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
                pageLeft.right += GetPageWidthForRotateMode(index) * mScale;
                pageLeft.top = pageLeft.bottom = offsetY;
                pageLeft.bottom += GetPageHeightForRotateMode(index) * mScale;
                mPageRectInView.push_back( pageLeft );
                mPageScaleInView.push_back( mScale );
                bbox.Union( pageLeft );
                offsetX = pageLeft.right + mSpaceX;
            }else{
                pageRight.left = pageRight.right = offsetX;
                pageRight.right += GetPageWidthForRotateMode(index) * mScale;
                pageRight.top = pageRight.bottom = offsetY;
                pageRight.bottom += GetPageHeightForRotateMode(index) * mScale;
                mPageRectInView.push_back( pageRight );
                mPageScaleInView.push_back( mScale );
                bbox.Union( pageRight );
                offsetX = mSpaceX;
                offsetY = ( ( pageRight.bottom > pageLeft.bottom ) ? pageRight.bottom : pageLeft.bottom ) + mSpaceY;
            }
        }
    }
    FinalAdjuest(bbox);
}

void CHE_PDF_PageLayout::UpdatePageInViewRectInfo()
{
    mPageRectInView.clear();
    mPageScaleInView.clear();
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

HE_PDF_PAGE_SIZE CHE_PDF_PageLayout::GetContentSize()
{
    HE_PDF_PAGE_SIZE size;
    size.width = mContentWidth;
    size.height = mContentHeight;
    return size;
}

CHE_Page_Rect CHE_PDF_PageLayout::GetPageRectInView(HE_ULONG pageIndex)
{
    return mPageRectInView[pageIndex-mCurPageStart];
}

HE_FLOAT CHE_PDF_PageLayout::GetPageScaleInView(HE_ULONG pageIndex)
{
    return mPageScaleInView[pageIndex-mCurPageStart];
}

void CHE_PDF_PageLayout::SetCurPageRange( HE_ULONG pageIndex, HE_ULONG pageCount )
{
    mCurPageStart = pageIndex;
    mCurPageCountInView = pageCount;
}

HE_PDF_PAGE_RANGE CHE_PDF_PageLayout::GetCurPageRange()
{
    HE_PDF_PAGE_RANGE range;
    range.pageStart = mCurPageStart;
    range.pageCount = mCurPageCountInView;
    return range;
}

inline HE_FLOAT CHE_PDF_PageLayout::GetPageWidthForRotateMode(HE_ULONG pageIndex)
{
    if ( mRotate == ROTATE_0 || mRotate == ROTATE_180 ) {
        return mPageSizes[pageIndex].width;
    }
    return mPageSizes[pageIndex].height;
}

inline HE_FLOAT CHE_PDF_PageLayout::GetPageHeightForRotateMode(HE_ULONG pageIndex)
{
    if ( mRotate == ROTATE_0 || mRotate == ROTATE_180 ) {
        return mPageSizes[pageIndex].height;
    }
    return mPageSizes[pageIndex].width;
}

void CHE_PDF_PageLayout::FinalAdjuest(const CHE_Page_Rect & bbox)
{
    HE_FLOAT offsetX = 0, offsetY = 0;
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