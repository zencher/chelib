//
//  WindowController.h
//  chelib
//
//  Created by zencher on 14/12/27.
//  Copyright (c) 2014年 com.peroit. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "CHEPDFEditView.h"

@interface MyWinController : NSWindowController
{
    CHEPDFEditView * pdfView;
}

- (id)init;

- (CHEPDFEditView*)getPdfView;

@end
