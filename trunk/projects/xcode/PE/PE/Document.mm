//
//  Document.m
//  PE
//
//  Created by zencher on 14/12/27.
//  Copyright (c) 2014年 com.peroit. All rights reserved.
//

#import "Document.h"

@interface Document ()

@end

@implementation Document

- (instancetype)init {
    self = [super init];
    if (self) {
        // Add your subclass-specific initialization here.
        
    }
    return self;
}

- (void)windowControllerDidLoadNib:(NSWindowController *)aController {
    [super windowControllerDidLoadNib:aController];
    // Add any code here that needs to be executed once the windowController has loaded the document's window.
}

- (void)makeWindowControllers {
    winController = [[WindowController alloc] init];
    [self addWindowController:winController];
}

+ (BOOL)autosavesInPlace {
    return NO;
}

- (BOOL)readFromData:(NSData *)data ofType:(NSString *)typeName error:(NSError **)outError
{
    IHE_NSDataRead * pReadNSData = new IHE_NSDataRead( data );
    pdfFileData = [[PDFFileData alloc] initWithReadInf:pReadNSData];
    return YES;
}



@end


IHE_NSDataRead::IHE_NSDataRead( NSData * data )
{
    mpData = data;
}

IHE_NSDataRead::~IHE_NSDataRead()
{
    
}

HE_ULONG IHE_NSDataRead::GetSize()
{
    return [mpData length];
}

HE_ULONG IHE_NSDataRead::ReadBlock( HE_LPVOID buffer, HE_ULONG offset, HE_ULONG size )
{
    [mpData getBytes:buffer range:NSMakeRange(offset, size)];
    return size;
}

HE_BYTE	IHE_NSDataRead::ReadByte( HE_ULONG offset )
{
    HE_BYTE byte;
    [mpData getBytes:&byte range:NSMakeRange(offset, 1)];
    return byte;
}

HE_VOID	IHE_NSDataRead::Release()
{
    
}

