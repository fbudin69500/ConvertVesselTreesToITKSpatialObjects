/*
Each vessel contains a set of 4D points (x,y,z, rad), a list of children (a 2D set
of values that contains the ID number of the child and the child's attachment 
position, and the ID of the parent. For arteries and vessels of indeterminate type,
a child's 0 point (first point in the 4D point array) is considered to be 
attached to the parent. This 0 point is considered the "head", and arteries are
connected like leeches, with the head attached to the parent. 
   Veins have an opposite flow direction, and are considered to be attached
to the parent by the tail. For both arteries and veins, blood flow is considered
as going from the 0 point to the NPoints-1 point.

      Each vessel also has a "proxoffset" and a "distoffset". These integers 
represent indices into the point array and allow clipping of a vessel. Calls
to "DoAllowedSample" will not return points proximal to proxoffset or distal to
distoffset, and calls to "GetValidChildren" will not return children attached
to the supposedly clipped areas. You still have access to the full data via
Get(0), DoSample(), and GetAllChildren().

   Each vessel has a color, represented as a red,green,blue,alpha setting. If
the opacity is set to 0, other programs may regard the vessel as invalid and
will ignore it and its children. The current implementation does not allow
setting of different colors or opacities along different points of the same
vessel other than the clipping provided by proxoffset and distoffset.

   Each vessel may have a parent ID, and a connection point to the parent, 
represented as an integer offset into the n-tuple point array of the parent.

  Elizabeth Bullitt 5/01

  mopdified 6/02 for namespace std. exception handling
  */


#ifndef _ebVESS_h
#define _ebVESS_h


#include "ebARLIST.h"      //list of multi-dimensional points of any type
#include "ebTBOX.h"        //string handling tools
#include "ebOBJECTIO.h"    //base class

#include <math.h>          //sqrt

#include "ebINCLUDES.h"      //define std namespace or not
//#include "MemoryLeak.h"
namespace eb
{

class VESS: public OBJECTIO
   {
   public:
    VESS();
   VESS(const VESS&);
   ~VESS();
   VESS& operator = (VESS& a);
   void Releasemem();

            //combine two vessels. This is order dependent. The first
            //vessel is assumed to come first. It will change the passed
            //vessels only by changing the child index pointer
    VESS& ConcatenateVessels(VESS& a, VESS& b);

   
            //proxoffset and distoffset determine how much of the vessel is shown
    int Proxoffset()        {return proxoffset;}
    void Proxoffset(int a)  {proxoffset = a;}
    int Distoffset()        {return distoffset;}
    void Distoffset(int a)  {distoffset = a;}
    void Resetoffset()      {proxoffset = 0; distoffset = pointnum;}
    void ResetOffset()      {proxoffset = 0; distoffset = pointnum;}
    int GetSamplestep()      {return samplestep;}


                  //queries to get first or last point
    float* Head()           {return Point(0);}
    float* Tail()           {return Point(distoffset - 1);}
    float* MyStart()      {if (this->IsVein()) return Point(distoffset - 1);
                        else return Point(proxoffset);}
    float* MyEnd()          {if (this->IsVein()) return Point(proxoffset);
                                 else return Point(distoffset - 1);}
   int MyStartInt()      {if (this->IsVein()) return distoffset - 1; return proxoffset;}
   int MyEndInt()         {if(this->IsVein()) return proxoffset; return distoffset - 1;}

 

                        //Distance functions
   int MinMaxBox(float* min, float* max); 
   int DangerousMinMaxBox(float* min, float* max);   //includes rad
    double MinDistSkel(float*);                           //just gives distance
    double MinDistSkel(float*, int*);                    //also gives location
   double MinDistAllowedSkel(float*, int*);           //proxoffset to distoffset                     //distance
   double Distsqr(float* a, float* b);


             //Translate all 3D points by some amount or change rad
   void Translate (float a, float b, float c);
   void Mult(float a, float b, float c);
   void Radmult(float rm);

                     //Get or set a sample of points. These will always include first and
                     //Last points, but will not include parental connection
    float* DoSample(int nskip, int* num);
                         //Same as above but just goes from proxoffset to distoffset
   float* DoAllowedSample(int nskip, int* num);
    float* DoConnectedSample(int nskip, int* num, float* parentpoint);
    float* DoConnectedAllowedSample(int nskip, int* num, float* parentpoint);
   float* GetSample(int* num)                     
       {if (sample) {*num = numsamplepoints; return sample;}
        else {DoAllowedSample(10, num); return sample; }
       }

   void DeleteSample();


               //Manipulate child list: each point in childlist is a 2D point that gives 
               //child ID and position along the parent's skeleton
   int AddChild(int vess, int pt);
   int DeleteChild(int vess)         {return childlist.DeleteByValue(0, vess);}
   void DeleteChildren()            {childlist.Releasemem();}
   void ResetChild()               {childlist.Reset();}
   int GetChild(int* i)            {return childlist.Get(i);} //returns 2 vals
   int HoldsChild(int a)             {return childlist.IsInList(0, a);}
   void ShowChild();                  //debugging purposes
    ARLIST<int>* GetChildList()            {return &childlist;}

    void Reverse();                     //reverse a vessel and childlist


                //File IO
    int Load(std::ifstream* ifile);
    int Save(std::ofstream* ofile);

   protected:
      
        int proxoffset;                 //set proximal cut
      int distoffset;                 //set distal cut
      ARLIST<int> childlist;        //child ids and attachment (2D point list)
      float* sample;                  //sample of skeleton points + rad
      int numsamplepoints;            //number of sample points
      int samplestep;

      int LoadHeader(std::ifstream* ifile)   {return OBJECTIO::LoadHeader(ifile);}
      int LoadPoints(std::ifstream* ifile);
      int SaveHeader(std::ofstream* ofile)   {return 0;}       //not called by this class
      int SavePoints(std::ofstream* ofile)   {return 0;}       //not called by this class
      };
}//end of namespace

#endif
