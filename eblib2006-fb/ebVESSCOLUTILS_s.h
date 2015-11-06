#ifndef _ebVESSCOL_UTILS_s_h
#define _ebVESSCOL_UTILS_s_h

//the vessel collection class is getting horribly complicated.
//Add new manipulations here; later you may want to move some things
//in vesscol into this class. 

#include "ebVESSCOL_s.h"
#include "ebINCLUDES.h"
//#include "MemoryLeak.h"
namespace eb
{

class VESSCOLUTILS_s
   {
   public:


            //Combine "end-to-end" child/parents into one vessel.
            //If two branches occur at the endpoint, one will remain
            //a branch but the other should continue. Helpful for things
            //like determining tortuosity and branch frequency (one should
            //not include as a branch something that is really a
            //continuation, for example). The function will not alter
            //the original collection.
      VESSCOL_s* CompressVC(VESSCOL_s* v);


            //Sometimes a connection point on the parent is identical
            //to a point on the child. This leads to a zero vector and
            //produces a hole. Process the connection points. 
      VESSCOL_s* ProcessConnectionPoints(VESSCOL_s* v);

 


   protected:
      VESSCOL_s vc;

      void ConcatenateVessels(int par, int child, VESSCOL_s* tempvc);
      void RecalcLists();

   };

}//end of namespace

#endif