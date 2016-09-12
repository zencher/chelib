//
//  MyWindow.h
//  chelib/Users/zencher/programmer/projects/chelib/trunk/projects/xcode
//
//  Created by zencher on 16/3/6.
//  Copyright © 2016年 com.peroit. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "CPDFEditView.h"
#import "Document.h"

@interface MyWindow : NSWindow
{
    CPDFEditView * pdfView;
}

- (void)load:(PdfDocumentData*)data;

- (IBAction)onRotate:(id)sender;
- (IBAction)showThumbnail:(id)sender;
- (IBAction)showBookmark:(id)sender;

@end
