/*
ebGEOM.h

2/25/00

This is an extremely old class that uses a matrix system I don't
use any more. I don't want to rewrite this class either. So define
the old matrix class here so you otherwise don't have to know about it

Modified 6/02 to go into std namespace

Elizabeth Bullitt

*/ 

#ifndef ebGEOMH
#define ebGEOMH


#include"ebVEC.h"
#include "ebINCLUDES.h"      //define std namespace or not
//#include "MemoryLeak.h"
namespace eb
{

 /*__declspec(dllexport)*/ class GEOM
  {
  public:
  GEOM() {}
  ~GEOM() {}

      //line equations
unsigned char Pointtoline      //Get distance and projection of a point on a line
  (PNT, VEC, PNT, double*, PNT*);
unsigned char Pointtolinewithdir(PNT, VEC, PNT, double*, PNT*, VEC*);
unsigned char Lineintersect      //Get intersection between two lines if present
  (PNT, VEC, PNT,VEC, PNT*);
unsigned char Closestpoint      //Get closest point between 2 lines not necessarily intersecting
  (PNT, VEC, PNT, VEC, PNT*, PNT*, PNT*, double*);

      //*******************************
      //plane equations
unsigned char Getsrfn            //Get plane surface normal
     (PNT, PNT, PNT, VEC*);
double PlaneD(PNT, VEC);            //Get D of plane equation
     
      //*************************
      //Line/plane/triangle intersections

unsigned char Linethroughplane      //Does a line intersect a plane?
   (PNT, VEC, PNT, PNT, PNT, PNT*);
unsigned char Linethroughplane      //Does a line intersect a characterized plane?
   (PNT, VEC, VEC, double, PNT*); 
unsigned char Linethroughplane      //Get t only if line intersects. Plane
     (PNT src, VEC dir, PNT pt, VEC srfn, double* tt); //has pt + srfn
unsigned char Pointintriangle   //Is a point inside an arbitrarily oriented triangle
   (PNT, PNT, PNT, PNT);
unsigned char Linethroughtriangle   //Does a 3D line pass through a triangle?
   (PNT, VEC, PNT, PNT, PNT, PNT*, PNT*);

      //***********************************
      //pyramids
unsigned char Linethroughpyramid            //Checks only sides, not base
   (PNT, VEC, PNT, PNT, PNT,PNT,PNT, PNT*, PNT*);
unsigned char Linethroughpyramid(PNT, VEC, PNT, PNT, PNT,PNT,PNT); //faster yes-no version


      //************************************
      //Miscellaneous
unsigned char Refldir      //Get reflection direction of a vector from a surface with known srfn
     (VEC, VEC, VEC*);

         //***********************************

         //Frenet frame vectors and space curve stuff. GetCircleRadius
         //gives the radius of the oscultating circle given 3 pts
         //along the curve.
int GetFrenetFrame(VEC velocity, VEC accel, VEC* tangent, VEC* normal, VEC* B);
double GetTriangleArea(PNT a, PNT b, PNT c);
double GetCircleRadius(PNT a, PNT b, PNT c);

};
 
}//end of namespace

#endif
