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

void CHE_PDF_PageLayout::SetViewSize( HE_UINT32 width, HE_UINT32 height )
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
    CHE_Page_Rect spaceRect;
    HE_FLOAT tmpScale = mScale;
    
    if ( mRotate == ROTATE_0 || mRotate == ROTATE_180 )
    {
        if ( mZoom == ZOOM_FIT )
        {
            page.left = 0;
        }else if( mZoom == ZOOM_FIX )
        {
            spaceRect.left = 0;
            spaceRect.right = 100;
            spaceRect.top = 0;
            spaceRect.bottom = mSpaceY;
            bbox.Union(  spaceRect );
            
            page.left = 0;
            page.right = mPageSizes[mCurPageStart].width * mScale;
            page.top = page.bottom = mSpaceY;
            page.bottom += mPageSizes[mCurPageStart].height * mScale;
            mPageRectInView.push_back( page );
            mPageScaleInView.push_back( mScale );
            
            spaceRect.left = 0;
            spaceRect.right = page.right;
            spaceRect.top = page.bottom;
            spaceRect.bottom = page.bottom + mSpaceY;
            
            bbox.Union( page );
            bbox.Union(  spaceRect );
        }
    }else{
        if ( mZoom == ZOOM_FIT )
        {
            page.left = 0;
        }else if( mZoom == ZOOM_FIX )
        {
            page.left = 0;
            page.right = mPageSizes[mCurPageStart].height * mScale;
            page.top = page.bottom = mSpaceY;
            page.bottom += mPageSizes[mCurPageStart].width * mScale;
            mPageRectInView.push_back( page );
            mPageScaleInView.push_back( mScale );
            
            spaceRect.left = 0;
            spaceRect.right = page.right;
            spaceRect.top = 0;
            spaceRect.bottom = mSpaceY;
            bbox.Union(  spaceRect );
            
            spaceRect.left = 0;
            spaceRect.right = page.right;
            spaceRect.top = page.bottom;
            spaceRect.bottom = page.bottom + mSpaceY;
            
            //bbox.Union( page );
            
            bbox.Union(  spaceRect );
        }
    }
    
    HE_FLOAT bboxWidth = bbox.Width();
    HE_FLOAT bboxHeight = bbox.Height();
    HE_FLOAT left = 0, top = 0;
    if ( bboxWidth < mViewWidth )
    {
        bboxWidth = mViewWidth;
    }
    if ( bboxHeight < mViewHeight )
    {
        bboxHeight = mViewHeight;
    }
    
    left = (bboxWidth - mPageRectInView[0].Width() ) / 2;
    top = (bboxHeight - mPageRectInView[0].Height() - 2 * mSpaceY ) / 2;
    mPageRectInView[0].right += left;
    mPageRectInView[0].left += left;
    mPageRectInView[0].top += top;
    mPageRectInView[0].bottom += top;
    
    mContentWidth = bboxWidth;
    mContentHeight = bboxHeight;

}

void CHE_PDF_PageLayout::UpdatePageInfoSinglePageScroll()
{
    CHE_Page_Rect bbox;
    CHE_Page_Rect page;
    CHE_Page_Rect spaceRect;
    HE_FLOAT offset = mSpaceY;
    HE_FLOAT tmpScale = mScale;
    
    mCurPageStart = 0;
    mCurPageCountInView = mPageSizes.size();
    
    if ( mRotate == ROTATE_0 || mRotate == ROTATE_180 )
    {
        //with is width, height is height
        
        if ( mZoom == ZOOM_FIT )
        {
            for ( size_t index = 0; index < mPageSizes.size(); ++index )
            {
                page.left = 0;
                page.right = (mViewWidth - 2 * mSpaceX);
                tmpScale = (mViewWidth - 2 * mSpaceX) / mPageSizes[index].width;
                page.top = page.bottom = offset;
                page.bottom += mPageSizes[index].height * tmpScale;
                offset = page.bottom;
                offset += mSpaceY;
                mPageRectInView.push_back( page );
                mPageScaleInView.push_back( tmpScale );
                bbox.Union( page );
            }
            spaceRect.left = page.left;
            spaceRect.right = page.right;
            spaceRect.top = offset;
            spaceRect.bottom = spaceRect.top + mSpaceY;
            bbox.Union( spaceRect );
        }else if ( mZoom == ZOOM_FIX )
        {
            for ( size_t index = 0; index < mPageSizes.size(); ++index )
            {
                page.left = 0;
                page.right = mPageSizes[index].width * mScale;
                page.top = page.bottom = offset;
                page.bottom += mPageSizes[index].height * mScale;
                offset = page.bottom;
                offset += mSpaceY;
                mPageRectInView.push_back( page );
                mPageScaleInView.push_back( mScale );
                bbox.Union( page );
            }
            spaceRect.left = page.left;
            spaceRect.right = page.right;
            spaceRect.top = offset;
            spaceRect.bottom = spaceRect.top + mSpaceY;
            bbox.Union( spaceRect );
        }
        
    }else{ // ROTATE_MODE_90 || ROTATE_MODE_270
        //widht is height, height is width
        
        if ( mZoom == ZOOM_FIT )
        {
            for ( size_t index = 0; index < mPageSizes.size(); ++index )
            {
                page.left = 0;
                page.right = (mViewHeight - 2 * mSpaceX);
                tmpScale = (mViewHeight - 2 * mSpaceX) / mPageSizes[index].height;
                page.top = page.bottom = offset;
                page.bottom += mPageSizes[index].width * tmpScale;
                offset = page.bottom;
                offset += mSpaceY;
                mPageRectInView.push_back( page );
                mPageScaleInView.push_back( tmpScale );
                bbox.Union( page );
            }
            spaceRect.left = page.left;
            spaceRect.right = page.right;
            spaceRect.top = offset;
            spaceRect.bottom = spaceRect.top + mSpaceY;
            bbox.Union( spaceRect );

        }else if ( mZoom == ZOOM_FIX )
        {
            for ( size_t index = 0; index < mPageSizes.size(); ++index )
            {
                page.left = 0;
                page.right = mPageSizes[index].height * mScale;
                page.top = page.bottom = offset;
                page.bottom += mPageSizes[index].width * mScale;
                offset = page.bottom;
                offset += mSpaceY;
                mPageRectInView.push_back( page );
                mPageScaleInView.push_back( mScale );
                bbox.Union( page );
            }
            spaceRect.left = page.left;
            spaceRect.right = page.right;
            spaceRect.top = offset;
            spaceRect.bottom = spaceRect.top + mSpaceY;
            bbox.Union( spaceRect );
        }
    }
    
    HE_FLOAT bboxWidth = bbox.Width();
    HE_FLOAT bboxHeight = bbox.Height();
    HE_FLOAT left = 0;
    if ( bboxWidth < mViewWidth )
    {
        bboxWidth = mViewWidth;
    }
    if ( bboxHeight < mViewHeight )
    {
        bboxHeight = mViewHeight;
    }
    
    for ( size_t index = 0; index < mPageSizes.size(); ++index )
    {
        left = (bboxWidth - mPageRectInView[index].Width() ) / 2;
        mPageRectInView[index].right += left;
        mPageRectInView[index].left += left;
    }
    
    mContentWidth = bboxWidth;
    mContentHeight = bboxHeight;
}

void CHE_PDF_PageLayout::UpdatePageInViewRectInfo()
{
    mPageRectInView.clear();
    mPageScaleInView.clear();
    
    switch ( mMode )
    {
        case PAGE_SINGLE:
        {
            UpdatePageInfoSinglePage();
            break;
        }
        case PAGE_DOUBLE:
        {
            break;
        }
        case PAGE_SINGLE_SCROLL:
        {
            UpdatePageInfoSinglePageScroll();
            break;
        }
        case PAGE_DOUBLE_SCROLL:
        {
            
            break;
        }
        default:
            break;
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
    return mPageRectInView[pageIndex];
}

HE_FLOAT CHE_PDF_PageLayout::GetPageScaleInView(HE_ULONG pageIndex)
{
    return mPageScaleInView[pageIndex];
}

void CHE_PDF_PageLayout::GetCurPageRange( HE_ULONG pageIndex, HE_ULONG pageCount )
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