//
//  AppDelegate.m
//  pdfRenderTest
//
//  Created by Frank Zeng on 13-5-31.
//  Copyright (c) 2013年 com.peroit. All rights reserved.
//

#import "AppDelegate.h"
#import "CustomView.h"

@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
    // Insert code here to initialize your application
    mScrollView = [[NSScrollView alloc] initWithFrame:[[_window contentView] frame]];
    [mScrollView setHasVerticalScroller:YES];
    [mScrollView setHasHorizontalScroller:YES];
    [mScrollView setBorderType:NSNoBorder];
    mContentView = [[CustomView alloc] initWithFrame:NSMakeRect(0, 0, 100, 100)];
    [mScrollView setDocumentView:mContentView];
    [_window setContentView:mScrollView];
}

- (IBAction)onNextPage:(id)sender
{
    [mContentView onNextPage];
}

- (IBAction)onPreviousPage:(id)sender
{
    [mContentView onPrePage];
}

- (void) openPanelDidEnd:(NSOpenPanel *)sheet
              returnCode:(int)returnCode
             contextInfo:(void *)context
{
    if (returnCode == NSOKButton)
    {
        NSArray *fileNames = [sheet filenames];
        mPdfDocument = [[PdfDocumentData alloc] initWithFilePath:[fileNames objectAtIndex: 0]];
        [mContentView newDocument:mPdfDocument];
    }
    
}

- (IBAction)onFileOpen:(id)sender
{
    NSOpenPanel *panel = [NSOpenPanel openPanel];
    [panel beginSheetForDirectory: nil
                             file: nil
                            types: [NSArray arrayWithObject: @"pdf"]
                   modalForWindow: _window
                    modalDelegate: self
                   didEndSelector: @selector(openPanelDidEnd:returnCode:contextInfo:)
                      contextInfo: nil];
}

@end
