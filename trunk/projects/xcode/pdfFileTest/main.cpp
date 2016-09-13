//
//  main.cpp
//  pdfFileTest
//
//  Created by Frank Zeng on 13-5-28.
//  Copyright (c) 2013年 com.peroit. All rights reserved.
//

#include <iostream>
using namespace std;

#include "../../../include/pdf/che_pdf_file.h"

using namespace chelib;

int main(int argc, const char * argv[])
{
    IRead * pFileRead = CreateFileIRead( "/Users/zencher/Desktop/test1.pdf" );
    if ( !pFileRead ) {
        cout << "IHE_Read object create failed!" << endl;
    }
    
    CPDF_File pdfFile;
    if ( pdfFile.Open( pFileRead ) == FALSE )
    {
        cout << "Open file failed!" << endl;
    }
    
    cout << "Open file successed." << endl;
    cout << "File version " << pdfFile.GetPDFVersion() << endl;
    
    return 0;
}

