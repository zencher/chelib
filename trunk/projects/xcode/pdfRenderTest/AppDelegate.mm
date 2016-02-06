//
//  AppDelegate.m
//  pdfRenderTest
//
//  Created by Frank Zeng on 13-5-31.
//  Copyright (c) 2013年 com.peroit. All rights reserved.
//

#import "AppDelegate.h"

@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
    NSRect rect = [[_window contentView] frame];
    
    displayScrollView = [[CHEPDFView alloc] initWithFrame:[[_window contentView] frame]];
    rect.size.width = rect.size.width / 4;
    outlineView = [[CHEPDFOutlineView alloc] initWithFrame:rect];
    
    mainView = [[CHEPDFSplitView alloc] initWithFrame:[[_window contentView] frame]];
    [mainView setVertical:YES];
    [mainView setDividerStyle:NSSplitViewDividerStyleThin];
    [mainView addArrangedSubview:outlineView];
    [mainView addArrangedSubview:displayScrollView];
    [_window setContentView:mainView];
}

- (IBAction)onNextPage:(id)sender
{
    [displayScrollView nextPage];
}

- (IBAction)onPreviousPage:(id)sender
{
    [displayScrollView prevPage];
}

- (IBAction)onScaleChanged:(id)sender
{
}

- (void) openPanelDidEnd:(NSOpenPanel *)sheet
              returnCode:(int)returnCode
             contextInfo:(void *)context
{
    if (returnCode == NSModalResponseOK)
    {
        NSArray *fileNames = [sheet filenames];
        [displayScrollView loadFile:[fileNames objectAtIndex: 0]];
        [outlineView setDataSource:[displayScrollView mainView]];
    }
    
}
- (IBAction)onRotate:(id)sender {
    [displayScrollView rotate];
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
