/*
Class that collects surface voxels of one or more segmented objects. Used in volume
rendering.

  Revised 6/02 for std namespace
Elizabeth Bullitt
*/

#ifndef _ebSurface_h
#define _ebSurface_h

#include "MetaImageLib.h"
#include "ebLIST.h"
#include "ebOMAT.h"
#include "ebINCLUDES.h"      //define std namespace or not
//#include "MemoryLeak.h"
namespace eb
{

class SURFACE
   {
   public:
      SURFACE();
      ~SURFACE();
      SURFACE(const SURFACE& a);
      SURFACE& operator = (const SURFACE& a);

      void Releasemem();

               //The first two loads will take any non-zero value in the maskfile, and the
               //third will take only a particular numerical value. All 3 require that each
               //surface object go through the entire file.
      int Load(meta::Image* mi, float* cfactor);     
      int Load(unsigned char* buff, int* d); 
      int Load(unsigned char* buff, int* d, int myval);


               //This set of functions is used so that a caller (usually a surface collection
               //class) can iterate only once through the image data, adding the surface points
               //to each surface member. It makes for a faster load if there are multiple objects.
      int InitiateLoad(unsigned char* buff, int* d);
      int IsMyBoundaryPoint(int x, int y, int z, int composite, unsigned char* buff, int myval);
      void AddPoint(int x, int y, int z, int composite);
      void FinishLoad();

               //If a set of boundary points is known, you can instead
               //set these directly. Since these are coming in as voxels, you presumably
               //already know the mins and maxes and don't need to recalculate
      int LoadKnownCompositeBoundaryPoints(int* d, int n, int* voxels, int* m, int* M);



               //Gets and Sets
      int GetNumpoints()      {return numpoints;}
      int* GetPoints()        {return pts;}
      int  GetPoint(int a)    {return pts[a];}     //no error check; be careful
      void GetPoint(int a, int* x, int* y, int* z);
      void GetPoint(int a, float* x, float* y, float* z)
         {
         int i,j,k; GetPoint(a, &i, &j, &k); *x = float(i); *y = (float)j; *z = (float)k;
         }

                  //Return the spatial bounding box of this object
      int* GetMin()           {return min;}
      int* GetMax()           {return max;}

                  //Hide or show
      void Hide()             {block = 1;}
      void Show()             {block = 0;}
      int IsVisible()         {return !block;}
      float GetOpacity()         {if (!block) return opacity; else return 0;}
      void SetOpacity(float a)  {opacity = a;}

      int* GetDims()           {return dims;}

                  //Is a voxel of interest in the list
      int IsInList(int vox);

                  //Recalculate mins and maxes. This may be useful if you have
                  //transformed the points
      int RecalcMinMax();

      void SetCol(float* _col)  {col[0] = _col[0]; col[1] = _col[1]; col[2] = _col[2];}
      float* GetCol()  {return col;}

   protected:
      int dims[3];
      int slicesize;
      float col[3];
      float opacity;
      int block;            //if blocked, do not show regardless of opacity

      LIST<int> lst;


               //margins of object data in integer composite voxel coords:
      int numpoints;
      int* pts;

               //bounding box of image data
      int min[3];
      int max[3];

      void Initmem();
      int IsABoundaryPoint(int x, int y, int z, int composite, unsigned char* buff);

   };

}//end of namespace

#endif
