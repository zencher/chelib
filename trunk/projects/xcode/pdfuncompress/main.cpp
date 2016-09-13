//
//  main.cpp
//  pdfuncompress
//
//  Created by zencher on 16/2/2.
//  Copyright © 2016年 com.peroit. All rights reserved.
//

#include <iostream>

#include "che_pdf_file.h"

using namespace chelib;

int main(int argc, const char * argv[]) {
    
    if ( argc != 2 ) {
        std::cout << "pdfuncompress filePathAndName" << std::endl;
        return 0;
    }
    
    char tmp[1024];
    IRead * pfile = CreateFileIRead(argv[1]);
    if ( pfile )
    {
        CPDF_File file;
        if ( file.Open( pfile ) )
        {
            sprintf( tmp, "%s.uncompressd.pdf", argv[1]);
            IWrite * pWrite = CreateFileIWrite(tmp);
            if ( pWrite )
            {
                file.Save( pWrite, FALSE );
            }
        }
    }
    return 0;
}
