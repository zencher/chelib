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
    HE_FLOAT offsetX = 0, offsetY = 0;
    HE_FLOAT tmpScale = mScale;
    
    if ( mRotate == ROTATE_0 || mRotate == ROTATE_180 )
    {
        if ( mZoom == ZOOM_FIT )
        {
            page.left = 0;
            page.right = (mViewWidth - 2 * mSpaceX);
            tmpScale = (mViewWidth - 2 * mSpaceX) / mPageSizes[mCurPageStart].width;
            page.top = page.bottom = 0;
            page.bottom += mPageSizes[mCurPageStart].height * tmpScale;
            mPageRectInView.push_back( page );
            mPageScaleInView.push_back( tmpScale );
            bbox.Union( page );
        }else if( mZoom == ZOOM_FIX )
        {
            page.left = 0;
            page.right = mPageSizes[mCurPageStart].width * mScale;
            page.top = page.bottom = 0;
            page.bottom += mPageSizes[mCurPageStart].height * mScale;
            mPageRectInView.push_back( page );
            mPageScaleInView.push_back( mScale );
            bbox.Union( page );
        }
    }else{
        if ( mZoom == ZOOM_FIT )
        {
            page.left = 0;
            page.right = (mViewWidth - 2 * mSpaceX);
            tmpScale = (mViewWidth - 2 * mSpaceX) / mPageSizes[mCurPageStart].height;
            page.top = page.bottom = 0;
            page.bottom += mPageSizes[mCurPageStart].width * tmpScale;
            mPageRectInView.push_back( page );
            mPageScaleInView.push_back( tmpScale );
            bbox.Union( page );
        }else if( mZoom == ZOOM_FIX )
        {
            page.left = 0;
            page.right = mPageSizes[mCurPageStart].height * mScale;
            page.top = page.bottom = 0;
            page.bottom += mPageSizes[mCurPageStart].width * mScale;
            mPageRectInView.push_back( page );
            mPageScaleInView.push_back( mScale );
            bbox.Union( page );
        }
    }
    
    mContentWidth = bbox.Width() + 2 * mSpaceX;
    if ( mContentWidth < mViewWidth )
    {
        mContentWidth = mViewWidth;
    }
    mContentHeight = bbox.Height() + 2 * mSpaceY;
    if ( mContentHeight < mViewHeight )
    {
        mContentHeight = mViewHeight;
    }
    
    offsetX = (mContentWidth - mPageRectInView[mCurPageStart].Width() ) / 2;
    offsetY = (mContentHeight - mPageRectInView[mCurPageStart].Height() ) / 2;
    mPageRectInView[mCurPageStart].right += offsetX;
    mPageRectInView[mCurPageStart].left += offsetX;
    mPageRectInView[mCurPageStart].top += offsetY;
    mPageRectInView[mCurPageStart].bottom += offsetY;
}

void CHE_PDF_PageLayout::UpdatePageInfoSinglePageScroll()
{
    CHE_Page_Rect bbox;
    CHE_Page_Rect page;
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
        }
    }else{ // ROTATE_MODE_90 || ROTATE_MODE_270
        //widht is height, height is width
        if ( mZoom == ZOOM_FIT )
        {
            for ( size_t index = 0; index < mPageSizes.size(); ++index )
            {
                page.left = 0;
                page.right = (mViewWidth - 2 * mSpaceX);
                tmpScale = (mViewWidth - 2 * mSpaceX) / mPageSizes[index].height;
                page.top = page.bottom = offset;
                page.bottom += mPageSizes[index].width * tmpScale;
                offset = page.bottom;
                offset += mSpaceY;
                mPageRectInView.push_back( page );
                mPageScaleInView.push_back( tmpScale );
                bbox.Union( page );
            }
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
        }
    }
    
    mContentWidth = bbox.Width() + 2 * mSpaceX;
    mContentHeight = bbox.Height() + 2 * mSpaceY;
    if ( mContentWidth < mViewWidth )
    {
        mContentWidth = mViewWidth;
    }
    if ( mContentHeight < mViewHeight )
    {
        mContentHeight = mViewHeight;
    }
    for ( size_t index = 0; index < mPageSizes.size(); ++index )
    {
        offset = (mContentWidth - mPageRectInView[index].Width() ) / 2;
        mPageRectInView[index].right += offset;
        mPageRectInView[index].left += offset;
    }
}

void CHE_PDF_PageLayout::UpdatePageInfoDoublePage()
{
    
}

void CHE_PDF_PageLayout::UpdatePageInfoDoublePageScroll()
{
    CHE_Page_Rect bbox;
    CHE_Page_Rect pageLeft;
    CHE_Page_Rect pageRight;
    
    HE_FLOAT offsetY = mSpaceY;
    HE_FLOAT offsetX = mSpaceX;
    HE_FLOAT preOffsetX = offsetX;
    //HE_FLOAT tmpScale = mScale;
    
    mCurPageStart = 0;
    mCurPageCountInView = mPageSizes.size();
    if ( mRotate == ROTATE_0 || mRotate == ROTATE_180 )
    {
        //with is width, height is height
        if ( mZoom == ZOOM_FIT )
        {
            for ( size_t index = 0; index < mPageSizes.size(); ++index )
            {
                /*page.left = 0;
                page.right = (mViewWidth - 2 * mSpaceX);
                tmpScale = (mViewWidth - 2 * mSpaceX) / mPageSizes[index].width;
                page.top = page.bottom = offset;
                page.bottom += mPageSizes[index].height * tmpScale;
                offset = page.bottom;
                offset += mSpaceY;
                mPageRectInView.push_back( page );
                mPageScaleInView.push_back( tmpScale );
                bbox.Union( page );*/
            }
        }else if ( mZoom == ZOOM_FIX )
        {
            for ( size_t index = 0; index < mPageSizes.size(); ++index )
            {
                if ( index % 2 == 0 )
                {
                    //page left
                    pageLeft.left = pageLeft.right = offsetX;
                    pageLeft.right += mPageSizes[index].width * mScale;
                    pageLeft.top = pageLeft.bottom = offsetY;
                    pageLeft.bottom += mPageSizes[index].height * mScale;
                    mPageRectInView.push_back( pageLeft );
                    mPageScaleInView.push_back( mScale );
                    bbox.Union( pageLeft );
                    offsetX = pageLeft.right + mSpaceX;
                }else{
                    //page right
                    pageRight.left = pageRight.right = offsetX;
                    pageRight.right += mPageSizes[index].width * mScale;
                    pageRight.top = pageRight.bottom = offsetY;
                    pageRight.bottom += mPageSizes[index].height * mScale;
                    mPageRectInView.push_back( pageRight );
                    mPageScaleInView.push_back( mScale );
                    bbox.Union( pageRight );
                    offsetX = mSpaceX;
                    offsetY = ( pageRight.bottom > pageLeft.bottom ) ? pageRight.bottom : pageLeft.bottom + mSpaceY;
                }
            }
        }
    }else{ // ROTATE_MODE_90 || ROTATE_MODE_270
        //widht is height, height is width
        if ( mZoom == ZOOM_FIT )
        {
            for ( size_t index = 0; index < mPageSizes.size(); ++index )
            {
                /*page.left = 0;
                page.right = (mViewWidth - 2 * mSpaceX);
                tmpScale = (mViewWidth - 2 * mSpaceX) / mPageSizes[index].height;
                page.top = page.bottom = offset;
                page.bottom += mPageSizes[index].width * tmpScale;
                offset = page.bottom;
                offset += mSpaceY;
                mPageRectInView.push_back( page );
                mPageScaleInView.push_back( tmpScale );
                bbox.Union( page );*/
            }
        }else if ( mZoom == ZOOM_FIX )
        {
            for ( size_t index = 0; index < mPageSizes.size(); ++index )
            {
                if ( index % 2 == 0 )
                {
                    //page left
                    pageLeft.left = pageLeft.right = offsetX;
                    pageLeft.right += mPageSizes[index].height * mScale;
                    pageLeft.top = pageLeft.bottom = offsetY;
                    pageLeft.bottom += mPageSizes[index].width * mScale;
                    mPageRectInView.push_back( pageLeft );
                    mPageScaleInView.push_back( mScale );
                    bbox.Union( pageLeft );
                    offsetX = pageLeft.right + mSpaceX;
                }else{
                    //page right
                    pageRight.left = pageRight.right = offsetX;
                    pageRight.right += mPageSizes[index].height * mScale;
                    pageRight.top = pageRight.bottom = offsetY;
                    pageRight.bottom += mPageSizes[index].width * mScale;
                    mPageRectInView.push_back( pageRight );
                    mPageScaleInView.push_back( mScale );
                    bbox.Union( pageRight );
                    offsetX = mSpaceX;
                    offsetY = ( pageRight.bottom > pageLeft.bottom ) ? pageRight.bottom : pageLeft.bottom + mSpaceY;
                }
            }
        }
    }
    
    mContentWidth = bbox.Width() + 2 * mSpaceX;
    mContentHeight = bbox.Height() + 2 * mSpaceY;
    if ( mContentWidth < mViewWidth )
    {
        mContentWidth = mViewWidth;
    }
    if ( mContentHeight < mViewHeight )
    {
        mContentHeight = mViewHeight;
    }
    for ( size_t index = 0; index < mPageSizes.size(); ++index )
    {
        if ( index % 2 == 0 )
        {
            if ( index + 1 < mPageSizes.size() )
            {
                preOffsetX = offsetX = ( mContentWidth - mPageRectInView[index].Width() - mPageRectInView[index + 1].Width() - mSpaceX * 3 ) / 2;
            }else{
                offsetX = preOffsetX;
            }
        }else{
            offsetX = ( mContentWidth - mPageRectInView[index].Width() - mPageRectInView[index - 1].Width() - mSpaceX * 3 ) / 2;
        }
        mPageRectInView[index].right += offsetX;
        mPageRectInView[index].left += offsetX;
    }
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
            UpdatePageInfoDoublePage();
            break;
        }
        case PAGE_SINGLE_SCROLL:
        {
            UpdatePageInfoSinglePageScroll();
            break;
        }
        case PAGE_DOUBLE_SCROLL:
        {
            UpdatePageInfoDoublePageScroll();
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