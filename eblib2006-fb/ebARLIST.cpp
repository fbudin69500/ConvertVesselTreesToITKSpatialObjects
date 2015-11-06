#ifndef _ebARLIST_cpp_
#define _ebARLIST_cpp_

#include "ebARLIST.h"

namespace eb
{

//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
      //ARLINK
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
template <class T>
ARLINK<T>::ARLINK()
   {dims = 1; data = 0; next = 0;}

//--------------------------------------
template <class T>
ARLINK<T>::ARLINK(const T* d, int a)
   {
   dims = a;
   next = 0;
   try {data = new T[a];}
   catch(...) {std::cout<<"Mem failure in ARLINK\n";}
   for (int i = 0; i <a; i++)
      data[i] = d[i];
   }
//----------------------------------
template <class T>
ARLINK<T>::ARLINK(const ARLINK<T>& b)
   {
   dims = b.dims;
   next = 0;
   try {data = new T[dims]; }
   catch(...) {std::cout<<"Mem failure in ARLINK\n";}

   for (int i = 0; i< dims; i++)
      data[i] = b.data[i];
   }
//----------------------------------
template <class T>
ARLINK<T>& ARLINK<T>::operator = (const ARLINK<T>& b)
   {Set(b.data, b.dims); return *this;}
//--------------------------

template <class T>
ARLINK<T>::~ARLINK()
   {
   if (data) delete [] data;
   }
//---------------------------
template <class T>
void ARLINK<T>::Set(const T* d, int a)
   {
   if (data) delete [] data;
   try {data = new T[a];}
   catch(...) {std::cout<<"Mem failure in ARLINK::Set\n"; return;}

   dims = a;

   for (int i = 0; i < dims; i++)
      data[i] = d[i];
   }

//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
      //ARLIST
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

               //^^^^^^^^^^^^^^^^^^^^^^^
               // CONSTRUCTORS, ASSIGNMENT
               //^^^^^^^^^^^^^^^^^^^^^^^^
template <class T>
ARLIST<T>::ARLIST(const ARLIST<T>& al)
   {
   T* tptr;
   ARLINK<T>* linkptr;
   dims = al.dims;
   head = tail = onlink = 0;
   num = 0;

   linkptr = al.head;
   while (linkptr)
      {
      tptr = linkptr->data;
      AddToTail(tptr);
      linkptr = linkptr->next;
      }
   onlink = head;
   }
//--------------------------------
template <class T>
ARLIST<T>& ARLIST<T>::operator = (const ARLIST<T>& al)
   {
   if (this == &al) return *this;

   Releasemem();

   T* tptr;
   ARLINK<T>* linkptr;
   dims = al.dims;

   linkptr = al.head;
   while (linkptr)
      {
      tptr = linkptr->data;
      AddToTail(tptr);
      linkptr = linkptr->next;
      }
   onlink = head;
   return *this;
   }
//-------------------------------
template <class T>
ARLIST<T>::ARLIST(const T* vals, int d, int n)
   {
   dims = d;
   num = 0;
   head = tail = onlink = 0;

   for (int i = 0; i < n; i++)
      AddToTail(&vals[i]);

   onlink = head;
   }

               //^^^^^^^^^^^^^^^^^^^^^^^^^^^
               //DELETE A LINK OR ALL LINKS
               //^^^^^^^^^^^^^^^^^^^^^^^^^^^
template <class T>
int ARLIST<T>::Delete()
   {
   if (!onlink) return 0;
   ARLINK<T>* temp;

   if (onlink == head)
      {
      temp = head->next;
      if (!temp) tail = 0;
      delete(head);
      head = onlink = temp;
      }
   else      //you need to find the value before onlink
      {
      temp = head;
      while (temp->next != onlink)
         temp = temp->next;
      temp->next = onlink->next;
      if (onlink == tail)
         tail = temp;
      delete onlink;
      }
   --num;
   return 1;
   }
//------------------------------------------------------
            
template<class T>
void ARLIST<T>::Releasemem()
   {
   ARLINK<T>* temp;
   while (head)
      {
      temp = head->next; 
      delete head;   
      head = temp;
      }
   onlink = tail = head = 0; 
   num = 0;
   }

               //^^^^^^^^^^^^^^^^^^^^^^^
               //ADD OR GET DATA
               //^^^^^^^^^^^^^^^^^^^^^^^^
template <class T>
int ARLIST<T>::AddToHead(const T* val)
   {
   ARLINK<T>* link;
   
   try {link = new ARLINK<T>;}
   catch(...) {std::cout<<"Mem failure in ARLIST::AddtoHead\n"; return 0;}

   link->Set(val, dims);

   if(!head) tail = link;
   else link->next = head;

   head = onlink = link;
   ++num;
   return num;
   }
//---------------------------------------
template <class T>
int ARLIST<T>::AddToTail(const T* val)
   {
   ARLINK<T>* link;
   try {link = new ARLINK<T>;}
   catch(...) {std::cout<<"Mem failure in ARLIST::AddtoTail\n"; return 0;}

   link->Set(val, dims);
   if(!head) head = link;
   else
      tail->next = link;
   tail = onlink = link;
   ++num;

   return num;
   }
//--------------------------------------
template <class T>
int ARLIST<T>::Get(T* a)
   {
   if (!onlink) return 0;
   for (int i = 0; i < dims; i++)
      a[i] = onlink->data[i];
   onlink = onlink->next;
   return 1;
   }
//----------------------------------
template <class T>
T* ARLIST<T>::GetPointer()
   {
   T* returnval = 0;
   if (onlink)
      {
      returnval = onlink->data;
      onlink = onlink->next;
      }
   return returnval;
   }

//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
         //INSERT OR POP
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
template <class T>
      //insert value immediately after onlink
int ARLIST<T>::Insert(const T* a)
   {
         //if no values or if onlink has walked to end
   if (!onlink) return AddToTail(a);

   ARLINK<T>* newlink;
   try {newlink = new ARLINK<T>;}
   catch(...) {std::cout<<"Mem failure in ARLIST::Insert\n"; return 0;}

   newlink->Set(a, dims);

   if (onlink == tail)
      tail = newlink;
   else
      newlink->next = onlink->next;

   onlink->next = newlink;
   ++num;
   return num;
   }
//---------------------------------
template <class T>
int ARLIST<T>::PopHead(T* a)
   {
   if (!head) return 0;

   for (int i = 0; i < dims; i++)
      a[i] = head->data[i];

   if(head == tail)
      {
      delete head;
      tail = head = onlink = 0;
      }
   else
      {
      onlink = head->next;
      delete head;
      head = onlink;
      }
   --num;
   return 1;
   }
//---------------------------
template<class T>
int ARLIST<T>::PopTail(T* a)
   {
   if (!tail) return 0;

   for (int i = 0; i < dims; i++)
      a[i] = tail->data[i];

   if (head != tail)
      {
      ARLINK<T>* temp = head;
      while (temp->next != tail)
         temp = temp->next;
      delete tail;
      temp->next = 0;
      tail = temp;
      onlink = temp;
      }
   else
      {
      delete tail;
      head = tail = onlink = 0;
      }
   --num;
   return 1;
   }
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
      //PEEK, PEEKNEXT: DOES NOT INCREMENT POINTER. OR REVERSE
//&&&&&&&&&&&&&&&&&&&&&&&&
template <class T>
int ARLIST<T>::Peek(T* a)
   {
   if (!onlink) return 0;
   for (int i = 0; i < dims; i++)
      a[i] = onlink->data[i];
   return 1;
   }
//-----------------------------
template <class T>
int ARLIST<T>::PeekNext(T* a)
   {
   if(!onlink || !onlink->next) return 0;
   for (int i = 0; i < dims; i++)
      a[i] = onlink->next->data[i];

   return 1;
   }
//----------------------------
template <class T>
int ARLIST<T>::Reverse()
   {
   if (!num) return 0;

   ARLIST <T> temp;
   temp.SetDims(dims);

   onlink = head;
   T* tptr;

   while(tptr = GetPointer())
      temp.AddToHead(tptr);

   Releasemem();
   *this = temp;
   }
      
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
      //FIND A VALUE IN TOTO OR BY AXIS
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
template <class T>
int ARLIST<T>::IsInList(const T* val)
   {
   int i;

   onlink = head;
   while (onlink)
      {
      for(i = 0; i < dims; i++)
         if (onlink->data[i] != val[i]) break;

      if (i == dims) return 1;
      onlink = onlink->next;
      }
   return 0;
   }
//---------------------------------------------------
template <class T>
int ARLIST<T>::IsInList(const T* val, int* where)
   {
   int i;

   onlink = head;
    *where = -1;

   while (onlink)
      {
        *where += 1;

      for(i = 0; i < dims; i++)
         if (onlink->data[i] != val[i]) break;

      if (i == dims) return 1;
      onlink = onlink->next;
      }
   return 0;
   }
//----------------------------------------------------
template <class T>
int ARLIST<T>::IsInList(int axis, T val)
   {
   if (axis >= dims) return 0;

   onlink = head;
   while (onlink)
      {
      if (onlink->data[axis] == val) return 1;
      onlink = onlink->next;
      }
   return 0;
   }
//----------------------------------------------
template <class T>
int ARLIST<T>::DeleteByValue(int axis, T val)
   {
   if (!IsInList(axis, val)) return 0;

    T* temp;
    try {temp= new T[dims];}
    catch(...) {std::cout<<"Mem failure in ARLIST::DeleteByValue\n"; return 0;}


    onlink = head;
   while (onlink)
      {
        Peek(temp);
        if (temp[axis] == val) 
           {Delete(); Reset(); continue;}
      onlink = onlink->next;
      } 

    delete [] temp;
   return 1;
   }
//---------------------------------------
template <class T>
int ARLIST<T>::DeleteByValue(const T* vals)
   {
   if (!IsInList(vals)) return 0;
    int i;
    int returnval = 0;
    int doit;
   onlink = head;
   while (onlink)
      {
        doit = 1;
        for (i = 0; i < dims; i++)
           {
           if (onlink->data[i] != vals[i])
              {doit = 0; break;}
           }
         if (doit)
            {
            returnval = 1;
            Delete(); Reset(); continue;
            }
      onlink = onlink->next;
      }   
   return returnval;
   }
//-----------------------------------
template <class T>
void ARLIST<T>::Show()
   {
   T* temp;
   try {temp= new T[dims];}
   catch(...) {std::cout<<"Mem failure in ARLIST::AddtoHead\n"; return;}

   int i;

   Reset();
   if (!Get(temp))
      std::cerr<<"Empty list\n";
   else
      {
      Reset();
      while (Get(temp))
       {
         std::cerr<<"  [";
         for (i = 0; i < dims; i++)
            std::cerr<<temp[i]<<", ";
         std::cerr<<"]";
         }
      std::cerr<<std::endl;
      }

   delete [] temp;
   Reset();
   }

}//end of namespace

#endif

