//
//  Document.h
//  PDFEditor
//
//  Created by zencher on 16/2/22.
//  Copyright © 2016年 com.peroit. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "PdfDocumentData.h"
#import "MyWinController.h"

@interface Document : NSDocument
{
    PdfDocumentData * pdfDoc;
    MyWinController *  winController;
}

-(PdfDocumentData*)getPdfDocumentData;

@end

