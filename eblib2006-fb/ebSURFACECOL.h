/*
Class that provides a collection of "surface objects" that can
be selectively volume rendered. The input uchar "mask file" is assumed
to be 0 where no object is present and to have n objects represented
by vals 1,2....n indicating the voxels contained within an object. (Object
1 has all voxels labeled 1). Queries from the outside use option base 0
(request for object 0 will actually return info for object 1). An individual
object may have disconnected parts in space, but rendering will be slower
than if a disconnected object is split into two.

  The class is designed for use with an MRA containing normal intensity values;
the mask file must have the same dimensions or a crash may ensue. In an attempt
to protect against this, any request to load a mask file is associated with a list
of "expected dimensions"--the number of voxels in x,y,z the mask file should 
contain. Load will be refused if the dimensions do not match.

  Three modes are allowed: 1) Single object: any non-zero val in mask will
                              be taken as belonging to a single surface (default)
                           2) Predef max: predefine a max number of surfaces; mask
                              file is assumed to contain 1,2,3...; no number
                              higher than max will be processed
                           3) Auto: the mask file will be pre-examined to determine
                                 the highest value and the same number of surfaces
                                 will be created and loaded. Be careful here--if
                                 you send a regular MRA you are likely to get 256 surfaces.

                              
Modified 6/02 for namespace std, exception handling
*/

#ifndef _ebSURFACECOL_h
#define _ebSURFACECOL_h


#include "ebSURFACE.h"
#include "MetaImageLib.h"
#include "ebINCLUDES.h"      //define std namespace or not
//#include "MemoryLeak.h"
namespace eb
{

class SURFACECOL
   {
   public:
      SURFACECOL();
      ~SURFACECOL();
      void ReleaseSurfaceMem();
      void Releasemem()             {ReleaseSurfaceMem();}

                  //Set the mode for next load. Single means that all non-zero
                  //surface points will go to one obhect. Auto means
                  //that multiple objects will be generated.
      void SetModeSingle()          {ReleaseSurfaceMem(); mode = 0;}
      void SetModeAuto()            {ReleaseSurfaceMem(); mode = 2;}
      void SetLoadMode(int a)       {ReleaseSurfaceMem(); mode = a;}
      int GetLoadMode()             {return mode;}

                  //Load a mask file which contains object ids
      int LoadMask(int* expected_dims, char* filename);
      int LoadMaskBuffer(int* dims, unsigned char* buff);


                  //Get info. BE CAREFUL. FOR SPEED, THERE ARE NO BOUNDS CHECKS SO
                  //YOU CAN CRASH HERE. 
      int GetNumSurfaces()          {return numsurfaces;}
      int GetVessnum()              {return numsurfaces;}
      int GetMode()                 {return mode;}
      int GetNumpoints(int a)       {return surfaces[a].GetNumpoints();}
      int* GetPoints(int a)         {return surfaces[a].GetPoints();}
      int* GetPoints(int a, int* n) {*n = surfaces[a].GetNumpoints(); return GetPoints(a);}
      int GetPoint(int a, int i)    {return surfaces[a].GetPoint(i);}
      void GetPoint(int a, int i, int* x, int* y, int* z) {surfaces[a].GetPoint(i, x,y,z);}
      void GetPoint(int a, int i, float* x, float* y, float* z) {surfaces[a].GetPoint(i, x,y,z);}
      int* GetMin(int a)            {return surfaces[a].GetMin();}
      int* GetMax(int a)            {return surfaces[a].GetMax();}
      void Hide(int a)              {surfaces[a].Hide();}
      void Show(int a)              {surfaces[a].Show();}
      int IsVisible(int a)          {return surfaces[a].IsVisible();}
      SURFACE* GetSurface(int a)  {return &surfaces[a];}
      float* GetCol(int a)            {return surfaces[a].GetCol();}
      void SetCol(int a, float* c)  {surfaces[a].SetCol(c);}

                //Manipulate all
      void Hide()                   {for (int i = 0; i < numsurfaces; i++) surfaces[i].Hide();}
      void Show()                   {for (int i = 0; i < numsurfaces; i++) surfaces[i].Show();}
      int* GetMin()                 {return &min[0];}
      int* GetMax()                 {return &max[0];}
      int* GetDims()                {return dims;}
      void SetOpacity(int a, float i)  {surfaces[a].SetOpacity(i);}
      float GetOpacity(int a)               {return surfaces[a].GetOpacity();}
      void SetOpacity(float a)      {for (int i = 0; i < numsurfaces; i++) surfaces[i].SetOpacity(a);}



                  //retrieve an errormessage
      char* GetError()              {if (strlen(errormsg)) return errormsg; return 0;}

                  //concatenate two surfacecollections or load
                  //a predefined set of surfaces
      int Concatenate(SURFACECOL* a, SURFACECOL* b);
      int LoadKnownSurfaces(int n, int* _dims, SURFACE* s);

                  //Return the first surfacecollection that
                  //contains a voxel of interest
      int IsInList(int vox, int* whichcollection);

                  //Recalculate mins and maxes. Useful if a transformation has
                  //been applied to the values
      int RecalcMinMax();


   protected:
      int dims[3];                  //MRA dims

      SURFACE* surfaces;
      int numsurfaces;              //number of surfaces actually used

      int mode;                     //determine how load occurs
      int usermax;

      int min[3];                   //bounding box of all defined surfaces
      int max[3];

      char errormsg[200];
   };

}//end of namespace

#endif
