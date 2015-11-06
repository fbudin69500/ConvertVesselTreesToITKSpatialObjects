//Array class for multiple dimensioned points; uses same calls as my
//list classes and so easy for me to use. Probably the std c++ lib 
//classes are as good or better, but this works easier for me.

//It is up
//to the caller to keep track of data entries--if you ask for a valid
//array value that you have not yet set, you may get garbage. 

/*
modified 6/02 to go into std namespace and do exception handling.

  Elizabeth Bullitt
*/

#ifndef _eb_ARRAY_T_h_
#define _eb_ARRAY_T_h_

#include "ebINCLUDES.h"
//#include "MemoryLeak.h"
namespace eb
{

template<class T>
class ARRAYT
   {
   public:
      ARRAYT();
      ARRAYT(int d, int n);
      ARRAYT(int d, int n, const T* vals);
      ARRAYT(const ARRAYT<T>& a);
      ~ARRAYT();
      ARRAYT& operator = (const ARRAYT<T>& a);


      void Releasemem();
      int Realloc(int d, int n);            //returns 0 on failure


      int GetDims()                        {return dims;}
      int Count()                           {return num;}
      int Num()                              {return num;}

      T* Get()                              {return data;}
      T* Get(int a)                        
         {if (a < num) return &data[dims * a]; return 0;}

               //The first does error checking and is fairly safe, the second
               //is fast and you better be right.
      int Set(int a, const T* val);
      int Set(const T* val);

               //Specialized sets for the first four vals
      int Set(int a, T x);
      int Set(int a, T x, T y);
      int Set(int a, T x, T y, T z);
      int Set(int a, T x, T y, T z, T w);

               //The call below will reallocate memory, copy earlier data, and add the new data. The incoming
               //data must be of the same dimension as any old data.
      int Add(int npoints, int d, T* points);
      int Add(ARRAYT<T>* a)            {return Add(a.num, a.dims, a.data);}

               //matrix type operations that act on entire array for the first
               //d vals of each point
      void Translate(int d, double* vals);
      void Scale (int d, double* vals);

      void Show();
      int Load(std::ifstream* ifile);
      int Save(std::ofstream* ofile);

   protected:
      T* data;
      int dims;
      int num;
   };

}//end of namespace

#include "ebARRAYT.cpp"

#endif



      

