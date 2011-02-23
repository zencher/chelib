#ifndef _CHE_PDF_OBJECTS_H_
#define _CHE_PDF_OBJECTS_H_

#include "../che_base.h"

class CHE_PDF_Object : public CHE_Object
{
};

class CHE_PDF_Boolean : public CHE_PDF_Object
{

};

class CHE_PDF_Number : public CHE_PDF_Object
{

};

class CHE_PDF_String : public CHE_PDF_Object
{

};

class CHE_PDF_Name : public CHE_PDF_Object
{

};

class CHE_PDF_Array : public CHE_PDF_Object
{

};

class CHE_PDF_Dictionary : public CHE_PDF_Object
{

};

class CHE_PDF_Stream : public CHE_PDF_Object
{

};

class CHE_PDF_Null	: public CHE_PDF_Object
{

};

class CHE_PDF_Reference : public CHE_PDF_Object
{

};

#endif