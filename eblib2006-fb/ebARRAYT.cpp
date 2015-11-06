#ifndef _ebARRAY_T_cpp
#define _ebARRAY_T_cpp

#include "ebARRAYT.h"

namespace eb
{
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
      //CONSTRUCTORS, DESTRUCTOR, ASSIGNMENT
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
template <class T>
ARRAYT<T>::ARRAYT()
   {num = 0; dims = 1; data = 0;}
//------------------------------------
template <class T>
ARRAYT<T>::ARRAYT(int d, int n)
   {
   num = n; dims = d; 
   try {data = new T[n * d]; }
   catch(...) {std::cout<<"Mem failed in ARRAYT\n";}
   memset(data, 0, n * d * sizeof(T));
   }
//---------------------------------------
template <class T>
ARRAYT<T>::ARRAYT(int d, int n, const T* vals)
   {
   numused = num = n; 
   dims = d; 
   try {data = new T[n * d]; }
   catch(...) {std::cout<<"Mem failed in ARRAYT\n";}

   memcpy(data, vals, n * d * sizeof(T));
   }
//---------------------------------------
template <class T>
ARRAYT<T>::ARRAYT(const ARRAYT<T>& a)
   {
   dims = a.dims;
   numused = a.numused;
   num = a.num;
   try {data = new T[dims * num];}
   catch(...) {std::cout<<"Mem failed in ARRAYT\n";}

   memcpy(data, a.data, dims * num * sizeof(T));
   }
//----------------------------------------
template <class T>
ARRAYT<T>::~ARRAYT()
   {Releasemem();}
//-----------------------------------------
template <class T>
void ARRAYT<T>::Releasemem()
   {
   if (data) delete [] data;
   data = 0;
   num = 0;
   dims = 1;
   }
//------------------------------------------
template <class T>
ARRAYT<T>& ARRAYT<T>::operator = (const ARRAYT& a)
   {
   Releasemem();
   dims = a.dims;
   num = a.num;
   try {data = new T[dims * num];}
   catch(...) {std::cout<<"Mem failed in ARRAYT::assignment\n"; }

   memcpy(data, a.data, dims * num * sizeof(T));
   return * this;
   }
//-----------------------------------------
template <class T>
int ARRAYT<T>::Realloc(int d, int n)
   {
   Releasemem();
   dims = d; num = n;
   try 
      {
      data = new T[num * dims];
      if(!data) return 0;
      }
   catch(...) {std::cout<<"Mem failed in ARRAYT::Realloc\n"; return 0;}

   memset(data, 0, num * dims * sizeof(T));
   return 1;
   }
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
         //SETS AND GETS
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
template <class T>
int ARRAYT<T>::Set(int a, const T* val)
   {
   if (a >= num) return 0;
   for (int i = 0; i < dims; i++)
      data[dims * a + i] = val[i];
   return 1;
   }
//------------------------------------------
template <class T>
int ARRAYT<T>::Set(const T* val)
   {
   memcpy(data, val, T * dims * num);
   return 1;
   }
//-------------------------------------------
template <class T>
int ARRAYT<T>::Set(int a, T x)
   {
   if (a < num && dims) 
      {
      data[a * dims] = x;
      return 1;
      }
   return 0;
   }
//----------------------------------------------
template <class T>
int ARRAYT<T>::Set(int a, T x, T y)
   {
   if (a < num && dims >= 2)
      {
      data[a * dims] = x;
      data[a * dims + 1] = y;
      return 1;
      }
   return 0;
   }
//-----------------------------------
template <class T>
int ARRAYT<T>::Set(int a, T x, T y, T z)
   {
   if ( a < num && dims >= 3)
      {
      data[a * dims] = x;
      data[a * dims + 1] = y;
      data[a * dims + 2] = z;
      return 1;
      }
   return 0;
   }
//-------------------------------------
template <class T>
int ARRAYT<T>::Add(int npoints, int d, T* points)
   {
   if(!points || npoints<= 0) return 0;
   if(num && dims != d) return 0;
   if(d <= 0) return 0;

   dims = d;

   T* newdata = new (nothrow) T[dims * (num + npoints)];
   if (!T) return 0;

   if (num)
      memcpy(newdata, data, num * dims * sizeof(T));
   
   T* datapointer = newdata + (num * dims);
   memcpy(datapointer, points, npoints * dims * sizeof(T));
   

   if(data) delete [] data;
   data = newdata;
   num +=npoints;

   return 1;
   }



         //&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
         //MATRIX TYPE OPERATIONS, LOAD
         //&&&&&&&&&&&&&&&&&&&&&&&&&&&&
template <class T>
void ARRAYT<T>::Translate(int d, double* vals)
   {
   if (dims < d) d = dims;
   int i, j, index;

   for (j = 0; j < num; j++)
      {
      index = j * dims;
      for (i = 0; i < d; i++)
         {
         data[index + i] = (T)(data[index + i] + vals[i]);
         }
      }
   }
//-------------------------------------
template <class T>
void ARRAYT<T>::Scale(int d, double* vals)
   {
   if (dims < d) d = dims;
   int i, j, index;

   for (j = 0; j < num; j++)
      {
      index = j * dims;
      for (i = 0; i < d; i++)
         data[index + i] = (T)(data[index + i] * vals[i]);
      }
   }
//--------------------------------------
template <class T>
int ARRAYT<T>::Load(std::ifstream* ifile)
   {
   if (!ifile) return 0;
   char readline[200];
   char* s;
   int newdims, newnum;
   int returnval = 1;
   double* pts;
   TBOX tbox;
   int i,j;
  

   Releasemem();
    while (*ifile)
      {
      if (!ifile->getline(readline, 199)) break;
      if (!strlen(readline)) continue;

      if(!strncmp(readline, "Dims:", 5))
         {
         s = readline + 6;
         newdims = atoi(s);
         }

      else if (!strncmp(readline, "Pointnum:", 9))
         {
         s = readline + 9;
         newnum = atoi(s);
         }

      else if (!strncmp(readline, "Points:", 7))
         {
         if (newnum <= 0 || newdims <= 0) return 0;
         Realloc(newdims, newnum);
         try
            {
            pts = new double[newdims];
            T* temp = new T[newdims];
            }
         catch
            {
            std::cout<<"Mem failed in ARRAYT::Load\n"; 
            return 0;
            }


         for (j = 0; j < newnum; j++)
            {
            if(!ifile->getline(readline, 199))
               {returnval = 0; delete [] pts; break;}
            if(!tbox.Stringtondoubles(readline, newdims, pts))
               {
               delete [] pts; returnval = 0; std::cerr<<"Vess breaking at point "<<j<<"\n ";break;
               }
            for (i = 0; i < newdims; i++)
               temp[i] = (T)pts[i];
            Set(j, temp);
            }
         delete [] temp;
         break;
         }
       }
    return returnval;
    }
//------------------------------------------------------
template<class T>
void ARRAYT<T>::Show()
   {
   register int i, j;
   int index;

   for (i = 0; i<num; i++)
      {
      std::cout<<"(";
      index = i * dims;
      for (j = 0; j < dims - 1; j++)
         std::cout<<data[index + j]<<", ";
      std::cout<<data[index + j];
      std::cout<<")  ";
      }
   std::cout<<std::endl;
   }


}//end of namespace

#endif


