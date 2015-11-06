#ifndef _ebINTCURVE_h_
#define _ebINTCURVE_h_

#include <math.h>    //fabs

#include "ebINCLUDES.h"      //define std namespace or not


#include "ebTBOX.h"
#include "ebLIST.h"

#include "ebOBJECTIO.h"    //base class
//#include "MemoryLeak.h"
/* This class is based on incredibly old code used for 3D curve reconstruction from biplane
views, and some programs still use it. Is rewritten to fit into the tube/vessel format. 
In order to avoid major rewrite, function calls remain the same, so terminology is sometimes
odd. The data is actually stored as floats now, and fp calls to set or get the data can be done
through the base class OBJECTIO. For backward compatibility, you can still make some calls to set/get points using
shorts--these are handled internally via casting. 
5/01

  revised 6/02 to go into std namespace
*/
namespace eb
{

class INTCURVE:public OBJECTIO
   {
   public:
      INTCURVE();
    INTCURVE(short);
    INTCURVE(const INTCURVE&);
    ~INTCURVE();
    INTCURVE& operator = (const INTCURVE&);
    void Setmem(short);                              //this just calls base classes realloc using a dim of 3

        //simple gets or reset inline
   short Getmaxpoints()       {return pointnum;}
   short Getnumused()         {return numused;}

     
        //Set startpoint and lastknown values; Get ILIST
   void Setstart(short);       //set startpoint 
   short Getstart()            {return startpoint;}
   void Setlastknown(short);   //Set lastknown value
   short Getlastknown()        {return lastknown;}
 

       //Get and set data. All return 0 on failure. The original program used shorts. Data is now stored
       //as floats. Use the base class calls to get individual points as floats. You must use the
      //base class calls to get all points at once using a float* to receive. 
   unsigned char Addpoint(short,short);         
   unsigned char Getpoint(short, short*,short*);        
   
   void Setrad(short a)    {rad = a;}
   short Getrad()      {return rad;}
   void Getrad(short* a)       {*a = rad;}


   unsigned char Islegal(short);   //is a number outside of array.
   
   //are my lastknown and numused points separated? Used to determine
   //if topless.
  short Gap();       
  unsigned char Topless(short);

   //multiply x,y and rad values by some factor. Used when
   //displaying a curve created on one screensize on a screen
   //of another size. You obviously need to make the changes on a
   //copy of the first curve if you want to keep using the original.
  void Multby(double a);


   //Functions dealing with curve parents (curves which could be
   //the source of the present one). Used for reconstruction.
   void Addribparent(short a)  {ribparents.Add(a);}
   LIST<short>* Getribparents() {return &ribparents;}

   
      //Save and load functions from text files. Files assumed open on entry.
      //SaveFileHeader writes the overall file header and should be written
      //only once at the beginning of the overall file save.
   int Load(std::ifstream* ifile);
    unsigned char SaveFileHeader(std::ofstream*, int a, int b, int num);
    unsigned char Save(std::ofstream*);

         //Get error message
    unsigned char* Error();

         //show the points for debugging purposes
    void Show();


   protected:
    short numused;
    short index;
    short startpoint;
    short lastknown;
    short rad;
    LIST<short> ribparents;      //list of possible curve parents
   char error[100];

   int INTCURVE::LoadPoints(std::ifstream* ifile);

  };

}//end of namespace

#endif
