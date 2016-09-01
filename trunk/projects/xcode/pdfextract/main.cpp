//
//  main.cpp
//  pdfextract
//
//  Created by zencher on 16/4/5.
//  Copyright © 2016年 com.peroit. All rights reserved.
//

#include <iostream>

#include "che_pdf_file.h"
#include "che_pdf_document.h"
#include "che_pdf_objclone.h"

int main(int argc, const char * argv[])
{
    if ( argc != 3 ) {
        std::cout << "pdfextract filePathAndName page" << std::endl;
        return 0;
    }

    IHE_Read * pfile = HE_CreateFileRead(argv[1]);
    if (!pfile)
    {
        return 0;
    }
    
    HE_INT32 pageIndex = atoi(argv[2]);
    
    CHE_PDF_File file;
    CHE_PDF_Document * pDoc = NULL;
    CHE_PDF_PageTree * pPageTree = NULL;
    CHE_PDF_Page * pPage = NULL;
    CHE_PDF_DictionaryPtr pageDict;
    if (file.Open(pfile))
    {
        pDoc = CHE_PDF_Document::CreateDocument(&file);
        pPageTree = pDoc->GetPageTree();
        pPage = pPageTree->GetPage(pageIndex);
        pageDict = pPage->GetPageDict();
    }

    
    CHE_ObjectCloneMgr ObjCloneMgr;
    
    CHE_PDF_File newFile;
    CHE_PDF_Document * pNewDoc = CHE_PDF_Document::CreateDocument(&newFile);
    CHE_PDF_PageTree * pNewPageTree = pNewDoc->GetPageTree();
    CHE_PDF_Page * pNewPage = NULL;
    CHE_PDF_DictionaryPtr newPageDict;
    
    newFile.SetPDFVersion(file.GetPDFVersion());
    pNewPageTree->AppendPage( 0, 0 );
    pNewPage = pNewPageTree->GetPage( pNewPageTree->GetPageCount() - 1 );
    newPageDict = pNewPage->GetPageDict();
    
    CHE_ByteString key;
    CHE_PDF_ObjectPtr tmpObjPtr;
    
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

    CHE_PDF_DictionaryPtr tmpDict = CloneDirectDictObj(pPage->GetResourcesDict(), &newFile, &ObjCloneMgr);
    newPageDict->SetDictionary("Resources", tmpDict);
    
    CHE_PDF_ArrayPtr tmpArray = CloneDirectArrayObj(pPage->GetMediaBoxArray(), &newFile, &ObjCloneMgr);
    newPageDict->SetArray("MediaBox", tmpArray);
    
    HE_INT32 rotate = pPage->GetRotate();
    newPageDict->SetInteger("Rotate", rotate);
    
    CHE_PDF_Page::ReleasePage(pPage);
    CHE_PDF_Page::ReleasePage(pNewPage);

    char tmpStr[1024];
    sprintf( tmpStr, "%s.page%04d.pdf", argv[1], pageIndex);
    
    IHE_Write * pWrite = HE_CreateFileWrite( tmpStr );
    
    newFile.Save( pWrite, FALSE );
    
    HE_DestoryIHEWrite( pWrite );
    return 0;
}
