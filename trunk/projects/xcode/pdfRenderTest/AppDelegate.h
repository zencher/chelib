//
//  AppDelegate.h
//  pdfRenderTest
//
//  Created by Frank Zeng on 13-5-31.
//  Copyright (c) 2013年 com.peroit. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "CHEPDFView.h"
#import "PdfDocumentData.h"

@interface AppDelegate : NSObject <NSApplicationDelegate>
{
    //NSScrollView * mScrollView;
    //CustomView * mContentView;
    //PdfDocumentData * mPdfDocument;
    CHEPDFView * displayScrollView;
}

@property (assign) IBOutlet NSWindow *window;

- (IBAction)onNextPage:(id)sender;

- (IBAction)onPreviousPage:(id)sender;

- (IBAction)onRotate:(id)sender;

- (IBAction)onScaleChanged:(id)sender;

- (void) openPanelDidEnd:(NSOpenPanel *)sheet
              returnCode:(int)returnCode
             contextInfo:(void *)context;

- (IBAction)onFileOpen:(id)sender;

@end
