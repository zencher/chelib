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
    
    pdfEditView = [[CPDFEditView alloc] initWithFrame:rect];
    [_window setContentView:pdfEditView];
}

- (IBAction)onNextPage:(id)sender
{
    [pdfEditView nextPage];
}

- (IBAction)onPreviousPage:(id)sender
{
    [pdfEditView prevPage];
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
        [pdfEditView loadFile:[fileNames objectAtIndex: 0]];
    }
    
}
- (IBAction)onRotate:(id)sender {
    [pdfEditView rotate];
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
