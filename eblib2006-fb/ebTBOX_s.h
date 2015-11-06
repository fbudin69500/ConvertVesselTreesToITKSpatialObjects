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


#ifndef ebTBOX_s_H
#define ebTBOX_s_H

#include "ebINCLUDES.h"      //define std namespace or not
#include <sstream>
#include <vector>
#include <string>
//#include "MemoryLeak.h"
namespace eb
{

 class TBOX_s
  {
  public:
   TBOX_s();
   void SetErrorStream(std::ostream &_err);
   int Instring(std::string originalsource, std::string _ctarget);   //returns first instance of target STRING
   int Instringlastchar(std::string source, char target);  //returns last instance of target CHAR
   std::string Strncopy(std::string &dest, std::string src, int len);     //strcpy guaranteed null terminated
   bool Lowercase(std::string &s);
   bool Uppercase(std::string &s);

   //get numerical info separated by spaces
   int Stringto2ints(std::string stringline, int &cubex, int &cubey);

   int Stringto3ints(std::string stringline, int &cubex, int &cubey,int &cubez);
   template<class T> bool StringtonTs(std::string s, int n, std::vector<T> &d);
   bool Stringtondoubles(std::string s, int n, std::vector<double> &d);   //string, number of points, dbls
   bool Stringtonfloats(std::string s, int n, std::vector<float> &d);
   bool Stringtonints(std::string s, int n, std::vector<int> &d);

        //Get a piece of  info, also giving the offset into the
        //string the next position after the number was extracted
   template<class T> bool GetT(std::string s, unsigned int &offset, T &val);
   bool GetInt(std::string a, unsigned int &offset, int &val);
   bool GetDouble(std::string a, unsigned int &offset, double &val);
   bool GetFloat(std::string a, unsigned int &offset, float &val);

            //Get the pointer to the space after a char string
   bool GetWord(std::string s, unsigned int &t);
   bool GetWord(std::string s, unsigned int &t, std::string &out);

               //Strip leading whitespace from a char string. Nondestructive--
               //gives offset only
    bool Strip(std::string a, unsigned int& offset);
    bool Strip(std::string a, unsigned int& offset,std::string &out);

               //will return string to n but begin at a delimiter
    template<class T> bool StringtonTs(std::string s, int numpts, std::vector<T> &pts, char delim);
    bool Stringtondoubles(std::string s, int numpts, std::vector<double> &pts, char delim);
    bool Stringtonfloats(std::string s, int numpts, std::vector<float> &pts, char delim);
    bool Stringtonints(std::string s, int numpts, std::vector<int> &pts, char delim);
  private:
      std::ostream *err;
  };

}//end of namespace

#endif
