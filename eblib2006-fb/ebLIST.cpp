#include "ebLIST.h"
#ifndef _eb_LIST_cpp
#define _eb_LIST_cpp

//#include "ebLIST.h"

namespace eb
{

//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
      //LINK FUNCTIONS TOO LONG FOR HEADER
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
template <class T>
LINK<T>& LINK<T>::operator = (const LINK<T>& a)
   {
   if (this == &a) return *this; 
   val = a.val; 
   next = a.next;
   return *this;
   }


//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
      //CONSTRUCTORS, ASSIGNMENT, MEMORY
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
template <class T>
LIST<T>::LIST(const LIST<T>& a)
   {
   LINK<T>* temp = a.head;
   while (temp)
      {
      AddToTail(temp->val);
      temp = temp->next;
      }

   onlink = head;
   bookmark = a.bookmark;
   bookmark2 = a.bookmark2;
 //  return *this;
   }
//-------------------------------------
template <class T>
LIST<T>& LIST<T>::operator = (const LIST<T>& a)
   {
   if (this == &a) return *this;
   Releasemem();

  LINK<T>* temp = a.head;
   while (temp)
      {
      AddToTail(temp->val);
      temp = temp->next;
      }
   onlink = head;
   bookmark = a.bookmark;
   bookmark2 = a.bookmark2;

   return *this;

   }
   
//-------------------------------------
template <class T>

 void LIST<T>::Releasemem()
   {
   LINK<T>* temp;
   while (head)
      {
      temp = head->next; 
      delete head;   
      head = temp;
      }
   onlink = tail = head = bookmark = bookmark2 = NULL; 
   num = 0;
   }
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
      //ADDITIONS
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
template <class T>
int LIST<T>::Add(const T& a)
   {
   LINK<T>* link;
   try {link= new LINK<T>;}
   catch(...) {std::cerr<<"Memory failure en LIST::Add\n"; return 0;}
   link->val = a; 
   link->next = head; 
   if (head == NULL) {tail = link; bookmark = link;}
   head = link; onlink = link; bookmark = bookmark2 = link;
   ++num;
   return num;
   }
//----------------------------
template <class T>
int LIST<T>::AddToTail(const T& a)            
   {
   LINK<T>* link;
   try {link= new LINK<T>;}
   catch(...){std::cerr<<"Memory failure en LIST::AddToTail\n"; return 0;}

   link->val = a; link->next = NULL;
   if (tail) tail->next = link;
   if(head == NULL)   {head = link; bookmark = bookmark2 = link;}
   tail = link;    onlink = link; 
   ++num;
   return num;
   }

//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
      //GET
//&&&&&&&&&&&&&&&&&&&&&&&&&&&
template <class T>
int LIST<T>::Get(T* a)
   {
   int returnval = 0;
   if (onlink)
      {
      *a = onlink->val; 
      onlink = onlink->next; 
      returnval = 1;
      }
   return returnval;
   }
//-----------------------------------------

//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
      //PROCESS BY SPECIFIC VALUE
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
template <class T>
int LIST<T>::GoToVal(const T& a)
   {
   onlink = head;
   if(onlink == NULL) return 0;
   if(onlink->val == a) return 1;

   while (onlink->val != a)
      {
      onlink = onlink->next;
      if(onlink == NULL) break;
      if (onlink->val == a) return 1;
      }
   return 0;
   }


//return -1 if error, current position otherwise
template<class T>
int LIST<T>::GetCurrentPosition()
{
    LINK <T> * pos = head;
    if(pos == NULL) return -1;
    if(onlink == NULL) return -1;
    int poscount=0;
    while( pos != onlink && poscount<num)
    {
        poscount++;
        pos=pos->next;
    }
    if(poscount<num)
        {return poscount;}
    else return -1;
}

//----------------------------
template <class T>
int LIST<T>::IsInList(const T& a, int* where)
   {
   onlink = head;
   if(onlink == NULL) return 0;

    *where = NULL;
   if(onlink->val == a) return 1;

   while (onlink->val != a)
      {
      onlink = onlink->next;
      if(onlink == NULL) break;
        *where += 1;
      if (onlink->val == a) return 1;
      }
   return 0;
   }
//----------------------------
template <class T>
int LIST<T>::DeleteByValue(const T&a)
   {
   if (GoToVal(a)) 
      {Delete(); return 1;} 
   return 0;
   }
//------------------------------
template <class T>
int LIST<T>::GetFirstCommon(LIST* lst, T* a)
   {
   Reset(); 
   T b; 
   while (lst->Get(&b))
      {
      if (GoToVal(b))
         {*a = b; return 1;} 
      }
   return 0;
   }

//-------------------
template <class T>
int LIST<T>::Insert(const T& a)
   {
   if (onlink==NULL)      //if there are no values or onlink has walked to end
    return AddToTail(a);

   LINK<T>* addlink;
   try {addlink = new LINK<T>;}
   catch(...){std::cerr<<"Memory failure en LIST::Insert\n"; return 0;}

   addlink->val = a;

            //insert new value between onlink and onlink's next value
   addlink->next = onlink->next;
   onlink->next = addlink;
   ++num;
    
   if(onlink == tail) tail = addlink;
   bookmark = bookmark2 = addlink;
  return num;
  }

//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
      //DELETIONS
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
template <class T>
int LIST<T>::Delete()
   {
   if (onlink == NULL) return 0;
   LINK<T>* temp;
   if (onlink == head) 
      {
      if (tail == head) tail = NULL;
      onlink = head->next; delete head; head = onlink;
      }
   else if (onlink == tail)
      {
      temp = head; while (temp->next != tail) temp = temp->next;
      delete tail; tail = temp; tail->next = NULL; onlink = tail;
      }
   else
      {
      temp = head; while(temp->next != onlink) temp = temp->next;
      temp->next = onlink->next; delete onlink; onlink = temp;
      }
   --num; return 1;

   bookmark = bookmark2 = onlink;
   }
//----------------------------------
template <class T>
int LIST<T>::PopHead(T* a)         
   {
   //ResetHead(); 
      Reset();
   if (head) *a = onlink->val; 
   return Delete();
   }

//------------------------
template <class T>
int LIST<T>::PopTail(T* a)      
   {
   ResetTail(); 
   if (tail) *a = onlink->val; 
   return Delete();
   }
//--------------------------
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
      //RETURN CURRENT OR NEXT VALUE WITHOUT INCREMENTING POINTER
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
template <class T>
int LIST<T>::Peek(T* a)
   {
   if (onlink == NULL) return 0;
   *a = onlink->val;
   return 1;
   }
//-----------------------------
template <class T>
int LIST<T>::PeekNext(T* a)
   {
   if (onlink==NULL || onlink->next == NULL) return 0;
   LINK<T>* temp;
   temp = onlink->next;
   *a = temp->val;
   return 1;
   }
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
      //REVERSE OR BUBBLESORT THE TEMPLATED DATA
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
template <class T>
int LIST<T>::Reverse()
   {
   if (!num) return 0;
   T tempval;

   LIST<T> replacelist;
   Reset();
   while(this->Get(&tempval))
      replacelist.AddToHead(tempval);

  Releasemem();
   *this = replacelist;

   bookmark = bookmark2 = head;
   return 1;
   }

}//end of namespace

#endif
