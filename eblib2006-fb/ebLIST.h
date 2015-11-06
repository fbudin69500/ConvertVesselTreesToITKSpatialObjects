#ifndef _eb_LIST_h
#define _eb_LIST_h

//Templated list/queue/stack class. Use ARLIST for multiple values
//and LIST for single parameter values. 

/*
Modified 6/02 to go into std namespace
Modified 3/08 to get a value pointer so you can releasemem for linked list of linked lists
Modified 3/08 to add a bookmark so you can jump back to a designated place
*/

#include "ebINCLUDES.h"      //define std namespace or not
//#include "MemoryLeak.h"
namespace eb
{

template <class T>
class LINK
   {
   public:
      LINK()                                    {next = NULL;}
      LINK(const T& a)                     {next = NULL; val = a;}
      LINK(const LINK& a)               {val = a.val; next = a.next;}
      LINK& operator = (const LINK& a);
   
         //public for rapid access in LIST, which is the only
         //class that should be using LINK
      T val;
      LINK<T>* next;
   };


template <class T>
class LIST
   {
   public:
      LIST()            {onlink = head = tail = bookmark = bookmark2 = NULL; num = 0;}
      LIST(const LIST& a);            //this will make a hard copy
      LIST& operator =(const LIST& a);   //this will make a hard copy

      virtual ~LIST()                           {Releasemem();}

      int Count()                        {return num;}
      int Num()                           {return num;}

      virtual void Releasemem();


                  //Reset the pointer onlink
      void Reset()                     {onlink = head;}
      void ResetTail()               {onlink = tail;}

                  //This is a singly linked list so you can't back up.
                  //However, you can set a bookmark. Added March 2008
      void Bookmark()      {bookmark = onlink;}
      void Bookmark2()   {bookmark2 = onlink;}
      void SetBookmarkNull() {bookmark = NULL;}
      void GoToBookmark()   {onlink = bookmark;}
      void GoToBookmark2(){onlink = bookmark2;}
      void SetBookmark2Null() {bookmark2 = NULL;}


               //Add a value. Adding to head makes a list or stack.
               //Adding to tail makes a queue.
      int Add(const T& a);
      int AddToHead(const T& a)   {return Add(a);}
      int AddToTail(const T& a);

            //Get a value. Returns 0 on failure
      int Get(T* a);
      int GetHead(T* a)            {onlink = head; return Get(a);}
      int GetTail(T* a)            {onlink = tail; return Get(a);}


            //Get a pointer to val
      LINK<T>* GetOnlink(){return onlink;}


               //Insert or Delete link at onlink or Pop (get data while deleting)
      int Insert(const T& a);
      int Delete();
      int PopHead(T* a);
      int PopTail(T* a);

               //Process by specific value
      int GoToVal(const T& a);
      int GetCurrentPosition();
      int IsInList(const T& a)      {return GoToVal(a);}      //historical
        int IsInList(const T& a, int* where);
      int DeleteByValue(const T&a);
      int GetFirstCommon(LIST* lst, T* a);


               //Look at present value or at next value without
               //incrementing pointer. Of value if you have a sorted list
      int Peek(T* a);
      int PeekNext(T* a);
      int Increment()            
         {if(!onlink) return 0; onlink = onlink->next;return 1;}
      int Reverse();

      
               //Debugging
      void Show()
         {
         onlink = head; std::cerr<<std::endl; 
         while (onlink){std::cerr<<onlink->val<<", "; onlink = onlink->next;}
         }
      


   protected:
      LINK <T> * onlink;
      LINK <T> * head;
      LINK <T> * tail;
      LINK <T> * bookmark;
      LINK <T> * bookmark2;
      int num;
   };

}//end of namespace

//Templates want everything in the header. Get the code out of the
//header.

#include "ebLIST.cpp"


#endif
