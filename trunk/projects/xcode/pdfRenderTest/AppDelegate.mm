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
    displayScrollView = [[CHEPDFView alloc] initWithFrame:[[_window contentView] frame]];
    [_window setContentView:displayScrollView];
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
    int x = 0;
}

- (void) openPanelDidEnd:(NSOpenPanel *)sheet
              returnCode:(int)returnCode
             contextInfo:(void *)context
{
    if (returnCode == NSOKButton)
    {
        NSArray *fileNames = [sheet filenames];
        [displayScrollView loadFile:[fileNames objectAtIndex: 0]];
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
