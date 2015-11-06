/*
Linked list with each link containing n-dimensional data of type T. Calls are similar to
those in LIST.

  Modified 6/02 to go into std namespace, exception handling.
  Class is similar to what exists in the standard template library, but too many programs
use it for me to delete it right now.

  Elizabeth Bullitt
*/

#ifndef _eb_ARLIST_h_
#define _eb_ARLIST_h_

#include "ebINCLUDES.h"         //define std here or not
//#include "MemoryLeak.h"
namespace eb
{

      //multi point data in each link. 
template <class T>
class ARLINK
   {
   public:
      ARLINK();
      ARLINK(const T* d, int a);
      ARLINK(const ARLINK& b);
      ~ARLINK();
      ARLINK& operator = (const ARLINK& b);
      void Set(const T* d, int a);
      
         //data is public for fast ARLIST access. No one else should
         //be using ARLINK anyway
      int dims;
      T* data;
      ARLINK<T>* next;
   };

      //This version of the class does the following:
      //      1. Performs a hard copy of all data passed and of copy/assignment
      //      2. Uses a fixed number of dims for each link (use VARLIST
      //            for lists that need a variable number of dims for each link)

template <class T>
class ARLIST
   {
   public:
      ARLIST()                           {onlink = head = tail = 0; num = 0; dims = 1;}
      ARLIST(int d)                     {onlink = head = tail = 0; num = 0; dims = d;}
      ARLIST(const ARLIST& al);
      ARLIST(const T* vals, int d, int n);
      ARLIST& operator = (const ARLIST& al);
      ~ARLIST()                           {Releasemem();}

               //Simple sets and gets. Setting dimensions will
               //erase any previous data.
      int GetDims()                        {return dims;}
      void SetDims(int a)               {Releasemem(); dims = a;}
      int Count()                           {return num;}
      int Num()                              {return num;}

            //Set pointer to current link
      void Reset()                        {onlink = head;}
      void ResetHead()                  {Reset();}
      void ResetTail()                  {onlink = tail;}

            //Delete value at onlink or delete all data
      int Delete();
      void Releasemem();

            //Simple sets and gets of data. IT IS THE CALLER'S
            //RESPONSIBILITY TO SEND AN ARRAY OF PROPER SIZE OR
            //TO HANDLE A RETURNED POINTER APPROPRIATELY. MEM SHOULD
            //BE ALLOCATED FOR PASSED PARAMS; RETURNED POINTERS ARE TO
            //DATA HELD IN THE LIST AND WILL POINT TO dims VALUES.
      int Add(const T* a)            {return AddToHead(a);}
      int AddToTail(const T* a);
      int AddToHead(const T* a);
      int Get(T* a);
      T* GetPointer();            
      int GetHead(T* a)                  {onlink = head; return Get(a);}
      //T* GetHeadPointer()               {onlink = head; return GetPointer(a);}
      T* GetHeadPointer()               {onlink = head; return GetPointer();}
      int GetTail(T* a)                  {onlink = tail; return Get(a);}
      //T* GetTailPointer()              {onlink = tail; return GetPointer(a);}
      T* GetTailPointer()              {onlink = tail; return GetPointer();}

               //Insert or Pop.  Pop will delete the link after the data 
               //is copied to caller
      int Insert(const T* a);
      int PopHead(T* a);
      int PopTail(T* a);

               //Look at the present value or the next one without
               //incrementing pointer. Or reverse
      int Peek(T* a);
      int PeekNext(T* a);
      int Increment()            
         {if(!onlink) return 0; onlink = onlink->next; return 1;}
      int Reverse();

               //Find a value in the list by axis or in total
      int IsInList(const T* a);
        int IsInList(const T* a, int* where);
      int IsInList(int axis, T val);
      int DeleteByValue(const T* a);
      int DeleteByValue(int axis, T val);


        void Show();


   



   protected:
      ARLINK<T>* head;
      ARLINK<T>* tail;
      ARLINK<T>* onlink;
      int num;
      int dims;
         
   };
}//end of namespace

#include "ebARLIST.cpp"
#endif
