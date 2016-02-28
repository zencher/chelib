//
//  WindowController.m
//  chelib
//
//  Created by zencher on 14/12/27.
//  Copyright (c) 2014年 com.peroit. All rights reserved.
//

#import "MyWinController.h"
#import "Document.h"

@implementation MyWinController

- (id)init {
    self = [super initWithWindowNibName:@"Window"];
    if (self) {
        pdfView = nil;
    }
    return self;
}

- (IBAction)OnConfig:(id)sender {
    [pdfView rotate];
}

- (CHEPDFEditView*)getPdfView
{
    return pdfView;
}

- (void)windowDidLoad {
    [super windowDidLoad];
    
    // Implement this method to handle any initialization after your window controller's window has been loaded from its nib file.
    pdfView = [[CHEPDFEditView alloc] init];
    [[self window] setContentView:pdfView];
    [pdfView load:[[self document] getPdfDocumentData]];
}

@end
