//
//  main.cpp
//  pdfextract
//
//  Created by zencher on 16/4/5.
//  Copyright © 2016年 com.peroit. All rights reserved.
//

#include <iostream>

#include "pdf_file.h"
#include "pdf_document.h"
#include "pdf_objclone.h"

using namespace chelib;

int main(int argc, const char * argv[])
{
    if ( argc != 3 ) {
        std::cout << "pdfextract filePathAndName page" << std::endl;
        return 0;
    }

    IRead * pfile = CreateFileIRead(argv[1]);
    if (!pfile)
    {
        return 0;
    }
    
    int32 pageIndex = atoi(argv[2]);
    
    CPDF_File file;
    CPDF_Document * pDoc = NULL;
    CPDF_PageTree * pPageTree = NULL;
    CPDF_Page * pPage = NULL;
    CPDF_DictionaryPtr pageDict;
    if (file.Open(pfile))
    {
        pDoc = CPDF_Document::CreateDocument(&file);
        pPageTree = pDoc->GetPageTree();
        pPage = pPageTree->GetPage(pageIndex);
        pageDict = pPage->GetPageDict();
    }

    
    CObjectCloneMgr ObjCloneMgr;
    
    CPDF_File newFile;
    CPDF_Document * pNewDoc = CPDF_Document::CreateDocument(&newFile);
    CPDF_PageTree * pNewPageTree = pNewDoc->GetPageTree();
    CPDF_Page * pNewPage = NULL;
    CPDF_DictionaryPtr newPageDict;
    
    newFile.SetPDFVersion(file.GetPDFVersion());
    pNewPageTree->AppendPage( 0, 0 );
    pNewPage = pNewPageTree->GetPage( pNewPageTree->GetPageCount() - 1 );
    newPageDict = pNewPage->GetPageDict();
    
    CByteString key;
    CPDF_ObjectPtr tmpObjPtr;
    
    pageDict->MoveToFirst();
    while (pageDict->GetKeyAndElement(key, tmpObjPtr))
    {
        if ( !tmpObjPtr || key == "Type" || key == "Parent" )
        {
            continue;
        }
        switch(tmpObjPtr->GetType())
        {
            case OBJ_TYPE_NULL:
                newPageDict->SetNull(key);
                break;
            case OBJ_TYPE_BOOLEAN:
                newPageDict->SetBoolean(key, tmpObjPtr->GetBooleanPtr()->GetValue());
                break;
            case OBJ_TYPE_NUMBER:
                if (tmpObjPtr->GetNumberPtr()->IsInteger())
                {
                    newPageDict->SetInteger(key, tmpObjPtr->GetNumberPtr()->GetInteger());
                }else{
                    newPageDict->SetFloatNumber(key, tmpObjPtr->GetNumberPtr()->GetFloat());
                }
                break;
            case OBJ_TYPE_STRING:
                newPageDict->SetString(key, tmpObjPtr->GetStringPtr()->GetString());
                break;
            case OBJ_TYPE_NAME:
                newPageDict->SetName(key, tmpObjPtr->GetNamePtr()->GetString());
                break;
            case OBJ_TYPE_ARRAY:
                newPageDict->SetArray(key, CloneDirectArrayObj(tmpObjPtr->GetArrayPtr(), &newFile, &ObjCloneMgr));
                break;
            case OBJ_TYPE_DICTIONARY:
                newPageDict->SetDictionary(key, CloneDirectDictObj(tmpObjPtr->GetDictPtr(), &newFile, &ObjCloneMgr));
                break;
            case OBJ_TYPE_REFERENCE:
            {
                PDF_RefInfo refInfo = CloneIndirectObj(tmpObjPtr->GetRefPtr(), &newFile, &ObjCloneMgr);
                newPageDict->SetReference(key, refInfo.objNum, refInfo.genNum, &newFile);
                break;
            }
            default:
                break;
        }
    }

    CPDF_DictionaryPtr tmpDict = CloneDirectDictObj(pPage->GetResourcesDict(), &newFile, &ObjCloneMgr);
    newPageDict->SetDictionary("Resources", tmpDict);
    
    CPDF_ArrayPtr tmpArray = CloneDirectArrayObj(pPage->GetMediaBoxArray(), &newFile, &ObjCloneMgr);
    newPageDict->SetArray("MediaBox", tmpArray);
    
    int32 rotate = pPage->GetRotate();
    newPageDict->SetInteger("Rotate", rotate);
    
    CPDF_Page::ReleasePage(pPage);
    CPDF_Page::ReleasePage(pNewPage);

    char tmpStr[1024];
    sprintf( tmpStr, "%s.page%04d.pdf", argv[1], pageIndex);
    
    IWrite * pWrite = CreateFileIWrite( tmpStr );
    
    newFile.Save( pWrite, FALSE );
    
    DestoryIWrite( pWrite );
    return 0;
}
