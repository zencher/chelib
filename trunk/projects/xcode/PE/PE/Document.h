//
//  Document.h
//  PE
//
//  Created by zencher on 14/12/27.
//  Copyright (c) 2014年 com.peroit. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "PDFFileData.h"
#import "WindowController.h"


@interface Document : NSDocument
{
    PDFFileData *       pdfFileData;
    WindowController *  winController;
}


@end

#import "../../../../include/che_base.h"

class IHE_NSDataRead : public IHE_Read
{
public:
    IHE_NSDataRead( NSData * data  );
    
    ~IHE_NSDataRead();
    
    HE_ULONG	GetSize();
    
    HE_ULONG	ReadBlock( HE_LPVOID buffer, HE_ULONG offset, HE_ULONG size );
    
    HE_BYTE		ReadByte( HE_ULONG offset );
    
    HE_VOID		Release();
    
private:
    NSData *    mpData;
};