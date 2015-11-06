#include "ebTBOX.h"

namespace eb
{
   //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
   // PUBLIC FUNCTIONS
   //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
      //*******************************
             //returns first instance of target STRING. Nondestructive of
             //original data.
char* TBOX::Instring(char* originalsource, char* ctarget)
  {  
  int done = 0;
  char* ct2;
  char* cs2;
  char* csource = originalsource;

          //check for impossible situations
  if(csource == NULL || ctarget ==NULL)  return NULL;
  if(*csource == '\0' || *ctarget == '\0') return NULL;

         //loop through each character of csource
  while (!done && *csource != '\0')
    {
    if (*csource != *ctarget)   csource++;
    else {
         cs2 = csource;
         ct2 = ctarget;
    
         do {
            ct2++;
            cs2++;
            }while ( (*cs2 ==*ct2) && (*ct2 != '\0')  && (*cs2 != '\0'));

         if (*ct2 == '\0')
            return csource;
         else  csource++;
         }
      }
  if (!done)   csource = NULL;

  return csource;                        
  }

        //********************************
        //returns last instance of target CHAR. Use for things like finding
        //a filename after a directory listing separated by '/'. Nondestructive
        //of original data.
char* TBOX::Instringlastchar(char* source, char target)
  {
  if (source == NULL) return NULL;
  char* returnval = NULL;
  char* temp = source;

  while (*temp++ != '\0')
    if (*temp == target) returnval = temp;
  
  return returnval;
  }

        //************************
        //Similar to strncpy except that the target is guaranteed null-
        //terminated. Does not affect source. Note that a length of 5 will
        //copy only 4 chars, with the last space reserved for '\0'.  
char* TBOX::Strncopy(char* dest, char* src, int len)
  {
  char* d1 = dest;
  char* s1 = src;
  for (int i = 0; i<len-1; i++)
    {
    *d1++ = *s1;
    if (*s1 == '\0')  break; 
    ++s1;
    }
  dest[len-1] = '\0';
  return dest;
  }
       //*********************************
       //Convert a string to lower or upper case. Will not affect numbers.
       //The original string is modified here so send a copy if you want
       //to keep the original.
unsigned char TBOX::Lowercase(char* s)
  {
  if(!s) return 0;
  int y = (int)strlen(s);
 
  for (int x = 0; x<y; x++)
    {
    if (s[x] >='A' && s[x]<='Z')
       s[x] = (char)(s[x]+32);
    }
  return 1;
  }



unsigned char TBOX::Uppercase(char* s)
  {
  if(!s) return 0;
  int y = (int) strlen(s);
 
  for (int x = 0; x<y; x++)
    {
    if (s[x] >='a' && s[x]<='z')
       s[x] = (char) (s[x] -32);
    }
  return 1;
  }
   //***************************
   //Find numerical info separated by spaces
void TBOX::Stringto2ints(char* stringline, int* cubex, int* cubey)
  {
  char* cptr = stringline;
  char* cptr2;

 
  while (isspace(*cptr)) ++cptr;
  cptr2 = cptr;
  while(!isspace(*cptr2)) ++cptr2;
  *cptr2 = '\0';
  *cubex = atoi(cptr);

  cptr = cptr2;  ++cptr;
  while (isspace(*cptr)) ++cptr;
  cptr2 = cptr;
  while(!isspace(*cptr2))  {if (*cptr2=='\0') break; ++cptr2;}
  *cptr2 = '\0';
  *cubey = atoi(cptr);
  }

void TBOX::Stringto3ints
(char* stringline, int* cubex, int* cubey,int* cubez)
  {
  char* cptr = stringline;
  char* cptr2;

 
  while (isspace(*cptr)) ++cptr;
  cptr2 = cptr;
  while(!isspace(*cptr2)) ++cptr2;
  *cptr2 = '\0';
  *cubex = atoi(cptr);

  cptr = cptr2;  ++cptr;
  while (isspace(*cptr)) ++cptr;
  cptr2 = cptr;
  while(!isspace(*cptr2)) ++cptr2;
  *cptr2 = '\0';
  *cubey = atoi(cptr);

  cptr = cptr2; ++cptr;
  while (isspace(*cptr)) ++cptr;
  cptr2 = cptr;
  while(!isspace(*cptr2))  {if (*cptr2=='\0') break; ++cptr2;}
  *cptr2 = '\0';
  *cubez = atoi(cptr);
  }

      //***************************
      //Here is a more general function
unsigned char TBOX::Stringtondoubles(char* s, int n, double* d)
  {
  unsigned char returnval = 1;
  char* cptr;
  cptr = s;
  char* cptr2;
 
  for (int i = 0; i < n; i++)
    { 
    if (cptr == NULL || !strlen(cptr)) return 0;

    while (isspace(*cptr)) 
      {
      if (*cptr == '\n') {return 0;}
      ++cptr;
      }
  
    cptr2 = cptr;
    while(!isspace(*cptr2)) 
    {
    if (*cptr2=='\0') 
      {
      if (i < n-1)   returnval = 0;
      break;
      }
    ++cptr2;
    }

    *cptr2 = '\0';
    d[i] = atof(cptr);
    if (!returnval)  break;
    cptr = cptr2;
    ++cptr; 
    }

  return returnval;
  }
      //***************************
      //Here is a more general function for ints
unsigned char TBOX::Stringtonints(char* s, int n, int* d)
  {
  unsigned char returnval = 1;
  char* cptr;
  cptr = s;
  char* cptr2;
 
  for (int i = 0; i < n; i++)
    { 
    if (cptr == NULL || !strlen(cptr)) return 0;

    while (isspace(*cptr)) 
      {
      if (*cptr == '\n') {return 0;}
      ++cptr;
      }
  
    cptr2 = cptr;
    while(!isspace(*cptr2)) 
    {
    if (*cptr2=='\0') 
      {
      if (i < n-1)   returnval = 0;
      break;
      }
    ++cptr2;
    }

    *cptr2 = '\0';
    d[i] = atoi(cptr);
    if (!returnval)  break;
    cptr = cptr2;
    ++cptr; 
    }

  return returnval;
  }
//---------------------------------------------------
      //same as above for float
unsigned char TBOX::Stringtonfloats(char* s, int n, float* d)
  {
  unsigned char returnval = 1;
  char* cptr;
  cptr = s;
  char* cptr2;
   int i;
 
  for (i = 0; i < n; i++)
    { 
    if (cptr == NULL || !strlen(cptr)) return 0;

    while (isspace(*cptr)) 
      {
      if (*cptr == '\n') {return 0;}
      ++cptr;
      }
  
    cptr2 = cptr;
    while(!isspace(*cptr2)) 
    {
    if (*cptr2=='\0') 
      {
      if (i < n-1)   returnval = 0;
      break;
      }
    ++cptr2;
    }

    *cptr2 = '\0';
    d[i] = (float)atof(cptr);
    if (!returnval)  break;
    cptr = cptr2;
    ++cptr; 
    }

  return returnval;
  }

  //-------------------------------------------------------------
  //Extract an int from a string, letting the caller know where the pointer
  //is now for extraction of something else from the same string
 unsigned char TBOX::GetInt(char* s, unsigned int* offset, int* val)
  {
    if (!s || strlen(s) == 0) return 0;
    char* copystring;

    try {copystring = new char[strlen(s) + 1];}
    catch(...) {std::cout<<"Mem alloc error in TBOX\n"; return 0;}

  strcpy(copystring, s);

  char* cptr1 = copystring;
  char* cptr2;

  while (isspace(*cptr1))
    {
    ++cptr1;
    *offset = *offset + 1;
    if (*cptr1 == '\0') {delete [] copystring; return 0;}
    }
  cptr2 = cptr1;

  while (!isspace(*cptr2))
    {
    ++cptr2;
    *offset = *offset + 1;
    if (*cptr2 == '\0') break;
    }

            //if you are at the end of string do not increment offset.
            //If you are not at string end, put offset to next position.
    if (*cptr2 != '\0') *offset = *offset + 1;


   *cptr2 = '\0';
   *val = atoi(cptr1);
   delete [] copystring;
   return 1;
   }
   //-------------------------------------------------------
   //Same as above but for float
unsigned char TBOX::GetFloat(char* s, unsigned int* offset, double* val)
  {
  if (!s || strlen(s) == 0) return 0;

  char* copystring;
  try  {copystring= new char[strlen(s) + 1];}
  catch(...) {std::cout<<"Mem alloc error in TBOX\n"; return 0;}

  strcpy(copystring, s);

  char* cptr1 = copystring;
  char* cptr2;

  while (isspace(*cptr1))
    {
    ++cptr1;
    *offset = *offset + 1;
    if (*cptr1 == '\0') {delete [] copystring; return 0;}
    }
  cptr2 = cptr1;

  while (!isspace(*cptr2))
    {
    ++cptr2;
    *offset = *offset + 1;
    if (*cptr2 == '\0') break;
    }

   if (*cptr2 != '\0') *offset = *offset + 1;
   *cptr2 = '\0';                  
   *val = atof(cptr1);   

   delete [] copystring;
   return 1;
   }
   //---------------------------------------------------------
unsigned char TBOX::GetFloat(char* a, unsigned int* i, float* f)
   {
   double d; 
   unsigned char rv = GetFloat(a, i, &d);
   *f = (float)d;   
   return rv;
   }

   //----------------------------------
         //Look for a char string with whitespace on the far end ( a "word"
         //in the literary sense). The function is not destructive of the
         //original string and returns only the offset of that space in t
unsigned char TBOX::GetWord(char* s, unsigned int* t)
      {
      if (!s || strlen(s) == 0) return 0; 
      *t = 0;
      char* cptr = s;

      while(isspace(*cptr))
         {
         ++cptr;
         if (*cptr == '\0') return 0;
         *t = *t + 1;
         }

      while(!isspace(*cptr))
         {
         ++cptr;
         *t = *t + 1;
         if (*cptr == '\0') break;
         }
      return 1;
      }
   //---------------------------
         //Strip leading whitespace from a char string. Nondestructive
unsigned char TBOX::Strip(char* a, unsigned int* offset)
   {
   if (!a || !strlen(a)) return 0;
   *offset = 0;
   char* s = a;

   while(isspace(*s)) {++s; *offset = *offset + 1;}
   return 1;
   }

//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
               //will return string to n but begin at a delimiter
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
unsigned char TBOX::Stringtondoubles(char* string, int numpts, double* pts, char delim)
   {
   char* cptr = Instringlastchar(string, delim);
   if (!cptr || !strlen(cptr))
      return 0;
                  //advance beyond the target
   ++cptr;
   if (!cptr || !strlen(cptr))
      return 0;

   return Stringtondoubles(cptr, numpts, pts);
   }



unsigned char TBOX::Stringtonfloats(char* string, int numpts, float* pts, char delim)
   {
   char* cptr = Instringlastchar(string, delim);
   if (!cptr || !strlen(cptr))
      return 0;
                  //advance beyond the target
   ++cptr;
   if (!cptr || !strlen(cptr))
      return 0;

   return Stringtonfloats(cptr, numpts, pts);
   }

unsigned char TBOX::Stringtonints(char* string, int numpts, int* pts, char delim)
   {
   char* cptr = Instringlastchar(string, delim);
   if (!cptr || !strlen(cptr))
      return 0;
                  //advance beyond the target
   ++cptr;
   if (!cptr || !strlen(cptr))
      return 0;

   return Stringtonints(cptr, numpts, pts);
   }

}//end of namespace
