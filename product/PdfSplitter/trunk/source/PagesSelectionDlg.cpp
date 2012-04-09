// PagesSelectionDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "PdfSplitter.h"
#include "PagesSelectionDlg.h"
#include "afxdialogex.h"

CPagesSelectionDlg * gpDlg = NULL;

static void UpdataBeginNumberBox()
{
	CHE_WD_Appearance * pTmpAppear = gpDlg->mpBeginNumberBox->GetBackGroundAppear();
	CHE_WD_AppearItem * pTmpItem = pTmpAppear->mItems[1];
	CHE_WD_AppearText * pTmpText = (CHE_WD_AppearText*)( pTmpItem );
	wchar_t tmpStr[16];
	wsprintf( tmpStr, L"%d", gpDlg->mPageBegin );
	pTmpText->SetText( tmpStr );
	gpDlg->mpBeginNumberBox->Refresh();
}

static void UpdataEndNumberBox()
{
	CHE_WD_Appearance * pTmpAppear = gpDlg->mpEndNumberBox->GetBackGroundAppear();
	CHE_WD_AppearItem * pTmpItem = pTmpAppear->mItems[1];
	CHE_WD_AppearText * pTmpText = (CHE_WD_AppearText*)( pTmpItem );
	wchar_t tmpStr[16];
	wsprintf( tmpStr, L"%d", gpDlg->mPageEnd );
	pTmpText->SetText( tmpStr );
	gpDlg->mpEndNumberBox->Refresh();
}

static void UpdateScrollBeginBlock()
{
	unsigned int BeginPosi = 0, EndPosi = 0;
	CHE_WD_Area * pBeginBlock = gpDlg->mpScrollBar->GetChild( 2 );
	CHE_WD_Area * pEndBlock = gpDlg->mpScrollBar->GetChild( 3 );
	pBeginBlock->SetPositionX( 25 + ( gpDlg->mPageBegin - 1 ) * 1.0 / ( gpDlg->mPageCount - 1 ) * 381 );
	BeginPosi = pBeginBlock->GetPositionX() + 15;
	//pEndBlock->SetPositionX(  55 + ( gpDlg->mPageEnd - 1 ) * 1.0 / ( gpDlg->mPageCount - 1 ) * 381 );
	EndPosi = pEndBlock->GetPositionX() + 15;

	CHE_WD_DragArea * pDragArea = (CHE_WD_DragArea*)pBeginBlock;
	pDragArea->SetRange( 25, 63, EndPosi-45, 63 );
	pDragArea = (CHE_WD_DragArea*)pEndBlock;
	pDragArea->SetRange( BeginPosi+15, 63, 436, 63 );

	CHE_WD_Area * pScrollf = gpDlg->mpScrollBar->GetChild( 1 );
	pScrollf->SetPositionX( BeginPosi );
	pScrollf->SetWidth( EndPosi - BeginPosi );
	gpDlg->mpScrollBar->Refresh();
}

static void UpdateScrollEndBlock()
{
	unsigned int BeginPosi = 0, EndPosi = 0;
	CHE_WD_Area * pBeginBlock = gpDlg->mpScrollBar->GetChild( 2 );
	CHE_WD_Area * pEndBlock = gpDlg->mpScrollBar->GetChild( 3 );
	//pBeginBlock->SetPositionX( 25 + ( gpDlg->mPageBegin - 1 ) * 1.0 / ( gpDlg->mPageCount - 1 ) * 381 );
	BeginPosi = pBeginBlock->GetPositionX() + 15;
	pEndBlock->SetPositionX(  55 + ( gpDlg->mPageEnd - 1 ) * 1.0 / ( gpDlg->mPageCount - 1 ) * 381 );
	EndPosi = pEndBlock->GetPositionX() + 15;

	CHE_WD_DragArea * pDragArea = (CHE_WD_DragArea*)pBeginBlock;
	pDragArea->SetRange( 25, 63, EndPosi-45, 63 );
	pDragArea = (CHE_WD_DragArea*)pEndBlock;
	pDragArea->SetRange( BeginPosi+15, 63, 436, 63 );

	CHE_WD_Area * pScrollf = gpDlg->mpScrollBar->GetChild( 1 );
	pScrollf->SetPositionX( BeginPosi );
	pScrollf->SetWidth( EndPosi - BeginPosi );
	gpDlg->mpScrollBar->Refresh();
}

static void EventBeginNumberUpBtnClick( CHE_WD_Area * pArea )
{
	if ( gpDlg->mPageBegin == gpDlg->mPageEnd - 1 )
	{
		return;
	}
	gpDlg->mPageBegin++;
	UpdataBeginNumberBox();
	UpdateScrollBeginBlock();
}

static void EventBeginNumberDownBtnClick( CHE_WD_Area * pArea )
{
	if ( gpDlg->mPageBegin == 1 )
	{
		return;
	}
	gpDlg->mPageBegin--;
	UpdataBeginNumberBox();
	UpdateScrollBeginBlock();
}

static void EventEndNumberUpBtnClick( CHE_WD_Area * pArea )
{
	if ( gpDlg->mPageEnd == gpDlg->mPageCount )
	{
		return;
	}
	gpDlg->mPageEnd++;
	UpdataEndNumberBox();
	UpdateScrollEndBlock();
}

static void EventEndNumberDownBtnClick( CHE_WD_Area * pArea )
{
	if ( gpDlg->mPageEnd == gpDlg->mPageBegin + 1 )
	{
		return;
	}
	gpDlg->mPageEnd--;
	UpdataEndNumberBox();
	UpdateScrollEndBlock();
}

static void EventDragBeginBlock( CHE_WD_Area * pArea )
{
	int BeginPosi = pArea->GetPositionX() + 15;
	CHE_WD_Area * pScrollf = gpDlg->mpScrollBar->GetChild( 1 );
	pScrollf->SetPositionX( BeginPosi );
	CHE_WD_Area * pEndBlock = gpDlg->mpScrollBar->GetChild( 3 );
	int EndPosi = pEndBlock->GetPositionX() + 15;
	pScrollf->SetWidth( EndPosi - BeginPosi );
	CHE_WD_DragArea * pDragArea = (CHE_WD_DragArea*)pEndBlock;
	pDragArea->SetRange( BeginPosi+15, 63, 436, 63 );
	unsigned int tmpPosi = pArea->GetPositionX();
	unsigned int tmpPageIndex = (unsigned int)( 1.5 + ( tmpPosi - 25 ) * 1.0 / ( 381 * 1.0 / (gpDlg->mPageCount - 1) ) );
	if ( tmpPageIndex < gpDlg->mPageEnd )
	{
		gpDlg->mPageBegin = tmpPageIndex;
	}
	UpdataBeginNumberBox();
}

static void EventDragEndBlock( CHE_WD_Area * pArea )
{
	int EndPosi = pArea->GetPositionX() + 15;
	CHE_WD_Area * pScrollf = gpDlg->mpScrollBar->GetChild( 1 );
	CHE_WD_Area * pBeginBlock = gpDlg->mpScrollBar->GetChild( 2 );
	int BeginPosi = pBeginBlock->GetPositionX() + 15;
	pScrollf->SetWidth( EndPosi - BeginPosi );
	CHE_WD_DragArea * pDragArea = (CHE_WD_DragArea*)pBeginBlock;
	pDragArea->SetRange( 25, 63, EndPosi-45, 63 );
	unsigned int tmpPosi = pArea->GetPositionX();
	unsigned int tmpPageIndex = (unsigned int)( 1.5 + ( tmpPosi - 55 ) * 1.0 / ( 381 * 1.0 / (gpDlg->mPageCount - 1) ) );
	if ( tmpPageIndex > gpDlg->mPageBegin )
	{
		gpDlg->mPageEnd = tmpPageIndex;
	}
	UpdataEndNumberBox();
}

static void EventCancelBtn( CHE_WD_Area * pArea )
{
	gpDlg->EndDialog( 0 );
}

static void EventOkBtn( CHE_WD_Area * pArea )
{
	CListItem listItem;
	listItem.type = PAGE_RANGE;
	listItem.pageIndex = gpDlg->mPageBegin;
	listItem.pageCount = gpDlg->mPageEnd - gpDlg->mPageBegin + 1;
	theApp.AddPageListItem( listItem );
	gpDlg->EndDialog( 1 );
}

IMPLEMENT_DYNAMIC(CPagesSelectionDlg, CDialogEx)

CPagesSelectionDlg::CPagesSelectionDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CPagesSelectionDlg::IDD, pParent)
{
	gpDlg = this;
	mPageBegin = 1;
	mPageEnd = theApp.mpPageTree->GetPageCount();
	mPageCount = theApp.mpPageTree->GetPageCount();

	CHE_WD_Appearance * pTmpApper = NULL;
	CHE_WD_AppearImage * pTmpImage = NULL;
	CHE_WD_AppearText * pTmpText = NULL;
	mpInterActive = new MyIHE_WD_InterActive( this, theApp.m_hInstance );
	mpMainArea = new CHE_WD_Area( 0, 0, mpInterActive );
	pTmpApper = new CHE_WD_Appearance();
	pTmpImage = new CHE_WD_AppearImage();
	pTmpImage->SetImageFile( L"images\\background.png" );
	pTmpImage->SetStyle( APPEAR_IMAGE_STYLE_TILTING );
	pTmpApper->mItems.push_back( pTmpImage );
	pTmpText = new CHE_WD_AppearText;
	pTmpText->SetText( L"起始页" );
	pTmpText->SetSize( 12 );
	pTmpText->SetPositionX( 20 );
	pTmpText->SetPositionY( 18 );
	pTmpText->SetWidth( 60 );
	pTmpText->SetHeight( 30 );
	pTmpText->SetColor( 0xFF000000 );
	pTmpApper->mItems.push_back( pTmpText );
	pTmpText = new CHE_WD_AppearText;
	pTmpText->SetText( L"终止页" );
	pTmpText->SetSize( 12 );
	pTmpText->SetPositionX( 165 );
	pTmpText->SetPositionY( 18 );
	pTmpText->SetWidth( 60 );
	pTmpText->SetHeight( 30 );
	pTmpText->SetColor( 0xFF000000 );
	pTmpApper->mItems.push_back( pTmpText );
	mpMainArea->SetBackGroundAppear( pTmpApper );

	mpBeginNumberBox = new CHE_WD_Area( 45, 24, mpInterActive );
	mpBeginNumberBox->SetPositionX( 75 );
	mpBeginNumberBox->SetPositionY( 20 );
	pTmpApper = new CHE_WD_Appearance();
	pTmpImage = new CHE_WD_AppearImage();
	pTmpImage->SetImageFile( L"images\\numberBox.png" ); 
	pTmpImage->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	pTmpApper->mItems.push_back( pTmpImage );
	pTmpText = new CHE_WD_AppearText();
	wchar_t tmpStr[16];
	wsprintf( tmpStr, L"%d", mPageBegin );
	pTmpText->SetText( tmpStr );
	pTmpText->SetSize( 12 );
	pTmpText->SetWidth( 45 );
	pTmpText->SetHeight( 24 );
	pTmpText->SetHoriAlign( APPEAR_TEXT_HALIGNMENT_RIGHT );
	pTmpText->SetVertAlign( APPEAR_TEXT_VALIGNMENT_CENTER );
	pTmpText->SetColor( 0xFFFFFFFF );
	mpBeginNumberBox->SetBackGroundAppear( pTmpApper );
	pTmpApper->mItems.push_back( pTmpText );
	mpMainArea->AppendChild( mpBeginNumberBox );

	CHE_WD_Button * pButton = new CHE_WD_Button( mpInterActive );
	pButton->SetWidth( 18 );
	pButton->SetHeight( 11 );
	pButton->SetPositionX( 127 );
	pButton->SetPositionY( 20 );
	pTmpApper = new CHE_WD_Appearance();
	pTmpImage = new CHE_WD_AppearImage();
	pTmpImage->SetImageFile( L"images\\numberBoxUpBtn.png" ); 
	pTmpImage->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	pTmpApper->mItems.push_back( pTmpImage );
	pButton->SetBackGroundAppear( pTmpApper );
	pButton->SetClickEvent( EventBeginNumberUpBtnClick );

	pTmpApper = new CHE_WD_Appearance();
	pTmpImage = new CHE_WD_AppearImage();
	pTmpImage->SetImageFile( L"images\\numberBoxUpBtnHover.png" ); 
	pTmpImage->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	pTmpApper->mItems.push_back( pTmpImage );
	pButton->SetMouseOverAppear( pTmpApper );

	mpMainArea->AppendChild( pButton );

	pButton = new CHE_WD_Button( mpInterActive );
	pButton->SetWidth( 18 );
	pButton->SetHeight( 13 );
	pButton->SetPositionX( 127 );
	pButton->SetPositionY( 31 );
	pTmpApper = new CHE_WD_Appearance();
	pTmpImage = new CHE_WD_AppearImage();
	pTmpImage->SetImageFile( L"images\\numberBoxDownBtn.png" ); 
	pTmpImage->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	pTmpApper->mItems.push_back( pTmpImage );
	pButton->SetBackGroundAppear( pTmpApper );
	pButton->SetClickEvent( EventBeginNumberDownBtnClick );

	pTmpApper = new CHE_WD_Appearance();
	pTmpImage = new CHE_WD_AppearImage();
	pTmpImage->SetImageFile( L"images\\numberBoxDownBtnHover.png" ); 
	pTmpImage->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	pTmpApper->mItems.push_back( pTmpImage );
	pButton->SetMouseOverAppear( pTmpApper );

	mpMainArea->AppendChild( pButton );

	mpEndNumberBox = new CHE_WD_Area( 45, 24, mpInterActive );
	mpEndNumberBox->SetPositionX( 220 );
	mpEndNumberBox->SetPositionY( 20 );
	pTmpApper = new CHE_WD_Appearance();
	pTmpImage = new CHE_WD_AppearImage();
	pTmpImage->SetImageFile( L"images\\numberBox.png" ); 
	pTmpImage->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	pTmpApper->mItems.push_back( pTmpImage );
	pTmpText = new CHE_WD_AppearText();
	wsprintf( tmpStr, L"%d", mPageEnd );
	pTmpText->SetText( tmpStr );
	pTmpText->SetSize( 12 );
	pTmpText->SetWidth( 45 );
	pTmpText->SetHeight( 24 );
	pTmpText->SetHoriAlign( APPEAR_TEXT_HALIGNMENT_RIGHT );
	pTmpText->SetVertAlign( APPEAR_TEXT_VALIGNMENT_CENTER );
	pTmpText->SetColor( 0xFFFFFFFF );
	mpEndNumberBox->SetBackGroundAppear( pTmpApper );
	pTmpApper->mItems.push_back( pTmpText );
	mpMainArea->AppendChild( mpEndNumberBox );

	pButton = new CHE_WD_Button( mpInterActive );
	pButton->SetWidth( 18 );
	pButton->SetHeight( 11 );
	pButton->SetPositionX( 272 );
	pButton->SetPositionY( 20 );
	pTmpApper = new CHE_WD_Appearance();
	pTmpImage = new CHE_WD_AppearImage();
	pTmpImage->SetImageFile( L"images\\numberBoxUpBtn.png" ); 
	pTmpImage->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	pTmpApper->mItems.push_back( pTmpImage );
	pButton->SetBackGroundAppear( pTmpApper );
	pButton->SetClickEvent( EventEndNumberUpBtnClick );

	pTmpApper = new CHE_WD_Appearance();
	pTmpImage = new CHE_WD_AppearImage();
	pTmpImage->SetImageFile( L"images\\numberBoxUpBtnHover.png" ); 
	pTmpImage->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	pTmpApper->mItems.push_back( pTmpImage );
	pButton->SetMouseOverAppear( pTmpApper );

	mpMainArea->AppendChild( pButton );

	pButton = new CHE_WD_Button( mpInterActive );
	pButton->SetWidth( 18 );
	pButton->SetHeight( 13 );
	pButton->SetPositionX( 272 );
	pButton->SetPositionY( 31 );
	pTmpApper = new CHE_WD_Appearance();
	pTmpImage = new CHE_WD_AppearImage();
	pTmpImage->SetImageFile( L"images\\numberBoxDownBtn.png" ); 
	pTmpImage->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	pTmpApper->mItems.push_back( pTmpImage );
	pButton->SetBackGroundAppear( pTmpApper );
	pButton->SetClickEvent( EventEndNumberDownBtnClick );

	pTmpApper = new CHE_WD_Appearance();
	pTmpImage = new CHE_WD_AppearImage();
	pTmpImage->SetImageFile( L"images\\numberBoxDownBtnHover.png" ); 
	pTmpImage->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	pTmpApper->mItems.push_back( pTmpImage );
	pButton->SetMouseOverAppear( pTmpApper );

	mpMainArea->AppendChild( pButton );

	mpScrollBar = new CHE_WD_Area( 441, 10, mpInterActive );
	mpScrollBar->SetPositionX( 25 );
	mpScrollBar->SetPositionY( 65 );
	CHE_WD_Area * pArea = new CHE_WD_Area( 441, 10, mpInterActive );
	pArea->SetPositionX( 25 );
	pArea->SetPositionY( 65 );
	pTmpApper = new CHE_WD_Appearance();
	pTmpImage = new CHE_WD_AppearImage();
	pTmpImage->SetImageFile( L"images\\Scroll.png" ); 
	pTmpImage->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	pTmpApper->mItems.push_back( pTmpImage );
	pArea->SetBackGroundAppear( pTmpApper );
	mpScrollBar->AppendChild( pArea );
	pArea = new CHE_WD_Area( 411, 10, mpInterActive );
	pArea->SetPositionX( 40 );
	pArea->SetPositionY( 65 );
	pArea->EnableClip();
	pTmpApper = new CHE_WD_Appearance();
	pTmpImage = new CHE_WD_AppearImage();
	pTmpImage->SetImageFile( L"images\\Scrollf.png" ); 
	pTmpImage->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	pTmpApper->mItems.push_back( pTmpImage );
	pArea->SetBackGroundAppear( pTmpApper );
	mpScrollBar->AppendChild( pArea );
	CHE_WD_DragArea * pTmpDragArea = new CHE_WD_DragArea( mpInterActive );
	pTmpDragArea->SetWidth( 30 );
	pTmpDragArea->SetHeight( 14 );
	pTmpDragArea->SetPositionX( 25 );
	pTmpDragArea->SetPositionY( 63 );
	pTmpDragArea->SetRange( 25, 63, 406, 63 );
	pTmpApper = new CHE_WD_Appearance();
	pTmpImage = new CHE_WD_AppearImage();
	pTmpImage->SetImageFile( L"images\\ScrollBlock.png" ); 
	pTmpImage->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	pTmpApper->mItems.push_back( pTmpImage );
	pTmpDragArea->SetBackGroundAppear( pTmpApper );
	pTmpDragArea->SetDragEvent( EventDragBeginBlock );
	mpScrollBar->AppendChild( pTmpDragArea );
	pTmpDragArea = new CHE_WD_DragArea( mpInterActive );
	pTmpDragArea->SetWidth( 30 );
	pTmpDragArea->SetHeight( 14 );
	pTmpDragArea->SetPositionX( 436 );
	pTmpDragArea->SetPositionY( 63 );
	pTmpDragArea->SetRange( 55, 63, 436, 63 );
	pTmpApper = new CHE_WD_Appearance();
	pTmpImage = new CHE_WD_AppearImage();
	pTmpImage->SetImageFile( L"images\\ScrollBlock.png" ); 
	pTmpImage->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	pTmpApper->mItems.push_back( pTmpImage );
	pTmpDragArea->SetBackGroundAppear( pTmpApper );
	pTmpDragArea->SetDragEvent( EventDragEndBlock );
	mpScrollBar->AppendChild( pTmpDragArea );

	mpMainArea->AppendChild( mpScrollBar );

	mpOkBtn = new CHE_WD_Button( mpInterActive );
	mpOkBtn->SetWidth( 88 );
	mpOkBtn->SetHeight( 27 );
	mpOkBtn->SetPositionX( 280 );
	mpOkBtn->SetPositionY( 100 );
	pTmpApper = new CHE_WD_Appearance();
	pTmpImage = new CHE_WD_AppearImage();
	pTmpImage->SetImageFile( L"images\\OkBtn.png" );
	pTmpImage->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	pTmpApper->mItems.push_back( pTmpImage );
	mpOkBtn->SetBackGroundAppear( pTmpApper );
	pTmpApper = new CHE_WD_Appearance();
	pTmpImage = new CHE_WD_AppearImage();
	pTmpImage->SetImageFile( L"images\\OkBtnHover.png" );
	pTmpImage->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	pTmpApper->mItems.push_back( pTmpImage );
	mpOkBtn->SetMouseOverAppear( pTmpApper );
	mpOkBtn->SetClickEvent( EventOkBtn );
	mpMainArea->AppendChild( mpOkBtn );

	mpCancelBtn = new CHE_WD_Button( mpInterActive );
	mpCancelBtn->SetWidth( 88 );
	mpCancelBtn->SetHeight( 27 );
	mpCancelBtn->SetPositionX( 380 );
	mpCancelBtn->SetPositionY( 100 );
	pTmpApper = new CHE_WD_Appearance();
	pTmpImage = new CHE_WD_AppearImage();
	pTmpImage->SetImageFile( L"images\\CancelBtn.png" );
	pTmpImage->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	pTmpApper->mItems.push_back( pTmpImage );
	mpCancelBtn->SetBackGroundAppear( pTmpApper );
	pTmpApper = new CHE_WD_Appearance();
	pTmpImage = new CHE_WD_AppearImage();
	pTmpImage->SetImageFile( L"images\\CancelBtnHover.png" );
	pTmpImage->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	pTmpApper->mItems.push_back( pTmpImage );
	mpCancelBtn->SetMouseOverAppear( pTmpApper );
	mpCancelBtn->SetClickEvent( EventCancelBtn );
	mpMainArea->AppendChild( mpCancelBtn );
}

CPagesSelectionDlg::~CPagesSelectionDlg()
{
	//销毁主区域背景
	CHE_WD_Appearance * pTmpAppear = mpMainArea->GetBackGroundAppear();
	delete pTmpAppear->mItems[0];
	delete pTmpAppear->mItems[1];
	delete pTmpAppear->mItems[2];
	delete pTmpAppear;

	//起始页数值框
	CHE_WD_Area * pTmpArea = mpMainArea->GetChild( 0 );
	pTmpAppear = pTmpArea->GetBackGroundAppear();
	delete pTmpAppear->mItems[0];
	delete pTmpAppear->mItems[1];
	delete pTmpAppear;
	delete pTmpArea;

	//起始页数值框up按钮
	pTmpArea = mpMainArea->GetChild( 1 );
	pTmpAppear = pTmpArea->GetBackGroundAppear();
	delete pTmpAppear->mItems[0];
	delete pTmpAppear;
	pTmpAppear = pTmpArea->GetMouseOverAppear();
	delete pTmpAppear->mItems[0];
	delete pTmpAppear;
	delete pTmpArea;

	//起始页数值框down按钮
	pTmpArea = mpMainArea->GetChild( 2 );
	pTmpAppear = pTmpArea->GetBackGroundAppear();
	delete pTmpAppear->mItems[0];
	delete pTmpAppear;
	pTmpAppear = pTmpArea->GetMouseOverAppear();
	delete pTmpAppear->mItems[0];
	delete pTmpAppear;
	delete pTmpArea;

	//终止页数值框
	pTmpArea = mpMainArea->GetChild( 3 );
	pTmpAppear = pTmpArea->GetBackGroundAppear();
	delete pTmpAppear->mItems[0];
	delete pTmpAppear->mItems[1];
	delete pTmpAppear;
	delete pTmpArea;

	//终止页数值框up按钮
	pTmpArea = mpMainArea->GetChild( 4 );
	pTmpAppear = pTmpArea->GetBackGroundAppear();
	delete pTmpAppear->mItems[0];
	delete pTmpAppear;
	pTmpAppear = pTmpArea->GetMouseOverAppear();
	delete pTmpAppear->mItems[0];
	delete pTmpAppear;
	delete pTmpArea;

	//终止页数值框down按钮
	pTmpArea = mpMainArea->GetChild( 5 );
	pTmpAppear = pTmpArea->GetBackGroundAppear();
	delete pTmpAppear->mItems[0];
	delete pTmpAppear;
	pTmpAppear = pTmpArea->GetMouseOverAppear();
	delete pTmpAppear->mItems[0];
	delete pTmpAppear;
	delete pTmpArea;

	//scroll bar
	pTmpArea = mpMainArea->GetChild( 6 );
	CHE_WD_Area * pTmpArea2 = pTmpArea->GetChild( 0 );
	pTmpAppear = pTmpArea2->GetBackGroundAppear();
	delete pTmpAppear->mItems[0];
	delete pTmpAppear;
	delete pTmpArea2;
	pTmpArea2 = pTmpArea->GetChild( 1 );
	pTmpAppear = pTmpArea2->GetBackGroundAppear();
	delete pTmpAppear->mItems[0];
	delete pTmpAppear;
	delete pTmpArea2;
	pTmpArea2 = pTmpArea->GetChild( 2 );
	pTmpAppear = pTmpArea2->GetBackGroundAppear();
	delete pTmpAppear->mItems[0];
	delete pTmpAppear;
	delete pTmpArea2;
	delete pTmpArea;

	//ok btn
	pTmpArea = mpMainArea->GetChild( 7 );
	pTmpAppear = pTmpArea->GetBackGroundAppear();
	delete pTmpAppear->mItems[0];
	delete pTmpAppear;
	pTmpAppear = pTmpArea->GetMouseOverAppear();
	delete pTmpAppear->mItems[0];
	delete pTmpAppear;
	delete pTmpArea;

	//cancel btn
	pTmpArea = mpMainArea->GetChild( 8 );
	pTmpAppear = pTmpArea->GetBackGroundAppear();
	delete pTmpAppear->mItems[0];
	delete pTmpAppear;
	pTmpAppear = pTmpArea->GetMouseOverAppear();
	delete pTmpAppear->mItems[0];
	delete pTmpAppear;
	delete pTmpArea;

	delete mpMainArea;
	delete mpInterActive;
}

void CPagesSelectionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CPagesSelectionDlg, CDialogEx)
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_WM_ERASEBKGND()
	ON_WM_ACTIVATE()
END_MESSAGE_MAP()

BOOL CPagesSelectionDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	CWnd *		pWnd = GetDlgItem( IDC_LOADDLG_MAIN );
	CPaintDC	dc( pWnd );
	mMemdc.CreateCompatibleDC( &dc );
	mBitmap.CreateCompatibleBitmap( &dc, mpMainArea->GetWidth(), mpMainArea->GetHeight() );
	CBitmap * olbBitmap = mMemdc.SelectObject( &mBitmap );
	mGraphics = ::new Graphics( mMemdc.GetSafeHdc() );
	mGraphics->SetSmoothingMode( SmoothingModeAntiAlias );
	mpInterActive->SetGraphics( mGraphics );
	mpMainArea->OnDraw();
	return TRUE;
}

void CPagesSelectionDlg::OnPaint()
{
	CPaintDC dc(this);
	DrawMainArea();
}

void CPagesSelectionDlg::DrawMainArea(void)
{
	CWnd *		pWnd = GetDlgItem( IDC_LOADDLG_MAIN );
	CPaintDC	dc( pWnd );
	mpMainArea->OnDraw();
	dc.BitBlt( 0, 0, mpMainArea->GetWidth(), mpMainArea->GetHeight(), &mMemdc, 0, 0, SRCCOPY );
}

void CPagesSelectionDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	mClientWidth = cx;
	mClientHeight = cy;

	mpMainArea->SetWidth( cx );
	mpMainArea->SetHeight( cy );

	CWnd * pWnd = GetDlgItem( IDC_LOADDLG_MAIN );
	if ( pWnd )
	{
		pWnd->MoveWindow( 0, 0, mpMainArea->GetWidth(), mpMainArea->GetHeight(), TRUE );
	}
}

void CPagesSelectionDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	mpMainArea->OnMouseLButtonDown( point.x, point.y );
	SetCapture();
	CDialogEx::OnLButtonDown(nFlags, point);
}


void CPagesSelectionDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	mpMainArea->OnMouseLButtonUp( point.x, point.y );
	ReleaseCapture();
	CDialogEx::OnLButtonUp(nFlags, point);
}


void CPagesSelectionDlg::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	mpMainArea->OnMouseLButtonDown( point.x, point.y );
	SetCapture();
	CDialogEx::OnLButtonDown(nFlags, point);
}

void CPagesSelectionDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	mpMainArea->OnMouseMove( point.x, point.y );
	CDialogEx::OnMouseMove(nFlags, point);
}


void CPagesSelectionDlg::OnOK()
{
	EventOkBtn( NULL );
}


BOOL CPagesSelectionDlg::OnEraseBkgnd(CDC* pDC)
{
	Invalidate(FALSE);
	return TRUE;
}


void CPagesSelectionDlg::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	Invalidate(FALSE);
	CDialogEx::OnActivate(nState, pWndOther, bMinimized);
}
