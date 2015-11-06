/*
ebTBOX.h
1/7/94
purpose: small class of string and char functions for general purpose use
in extracting data from text files.
indirect include: None
keywords: string

3/99
Removed "byte" ref (ms views "byte" as SIGNED char). 

4/99
Added GetWord() and Strip() functions.

6/02
Put into std namespace added exception handling

   Elizabeth Bullitt
*/


#ifndef ebTBOXH
#define ebTBOXH

#include "ebINCLUDES.h"      //define std namespace or not
//#include "MemoryLeak.h"

namespace eb
{


 class TBOX
  {
  public:
   char* Instring(char*, char*);   //returns first instance of target STRING
   char* Instringlastchar(char*,char);  //returns last instance of target CHAR
   char* Strncopy(char*,char*,int);     //strcpy guaranteed null terminated
   unsigned char Lowercase(char*);
   unsigned char Uppercase(char*);

   //get numerical info separated by spaces
   void Stringto2ints(char*, int*, int*);
   void Stringto3ints(char*, int*,int*,int*);
   unsigned char Stringtondoubles(char*, int, double*);   //string, number of points, dbls
   unsigned char Stringtonfloats(char*, int, float*);
    unsigned char Stringtonints(char*, int, int*);

        //Get a piece of  info, also giving the offset into the
        //string the next position after the number was extracted
   unsigned char GetInt(char*, unsigned int*, int*);
   unsigned char GetFloat(char*, unsigned int*, double*);
    unsigned char GetFloat(char* a, unsigned int* i, float* f);

            //Get the pointer to the space after a char string
    unsigned char GetWord(char*, unsigned int*);

               //Strip leading whitespace from a char string. Nondestructive--
               //gives offset only
    unsigned char Strip(char*, unsigned int*);

               //will return string to n but begin at a delimiter
    unsigned char Stringtondoubles(char* string, int numpts, double* pts, char delim);
    unsigned char Stringtonfloats(char* string, int numpts, float* pts, char delim);
    unsigned char Stringtonints(char* string, int numpts, int* pts, char delim);
  };

}//end of namespace

#endif
