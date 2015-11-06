#include "ebSURFACE.h"

namespace eb
{

SURFACE::SURFACE()
   {
   Initmem();
   col[0] = 1; col[1] = 1; col[2] = 1;
   }
//_____________________________
SURFACE::SURFACE(const SURFACE& a)
   {
   int i;

   for (i = 0; i < 3; i++)
      {
      dims[i] = a.dims[i];
      min[i] = a.min[i];
      max[i] = a.max[i];
      }

   slicesize = a.slicesize;
   opacity = a.opacity;
   lst = a.lst;
   numpoints = a.numpoints;
   try
      {
      pts = new int[numpoints];
      if (!pts) 
         {
         std::cerr<<"Out of mem in surface constructor\n";
         numpoints = 0;
         }
      }
   catch(...)
      {
      std::cerr<<"Out of mem in surface constructor\n";
      numpoints = 0;
      }

   for (i = 0; i < numpoints; i++)
      pts[i] = a.pts[i];

   for (i = 0; i < 3; i++)
      col[i] = a.col[i];

   opacity = a.opacity;
   }
//-----------------------------------
SURFACE& SURFACE::operator = (const SURFACE& a)
   {
   Releasemem();
  int i;

   for (i = 0; i < 3; i++)
      {
      dims[i] = a.dims[i];
      min[i] = a.min[i];
      max[i] = a.max[i];
      }

   slicesize = a.slicesize;
   opacity = a.opacity;
   lst = a.lst;
   numpoints = a.numpoints;
   try
      {
      pts = new int[numpoints];
      if (!pts) 
         {
         std::cerr<<"Out of mem in surface constructor\n";
         numpoints = 0;
         }
      }
   catch(...) {std::cerr<<"Out of mem in surface constructor\n"; numpoints = 0;}

   for (i = 0; i < numpoints; i++)
      pts[i] = a.pts[i];

   for (i = 0; i < 3; i++)
      col[i] = a.col[i];

   opacity = a.opacity;

   return *this;
   }
//-------------------------------
SURFACE::~SURFACE()
   {
   Releasemem();
   }
//------------------------------
void SURFACE::Releasemem()
   {
   if (pts) delete [] pts;
   lst.Releasemem();
   Initmem();
   }
//----------------------------------------
void SURFACE::Initmem()
   {
   pts = 0;
   numpoints = 0;
   opacity = 1;
   }
//---------------------------------------------
      //No error check: be careful
void SURFACE::GetPoint(int a, int* x, int* y, int* z)
   {
   int val = pts[a];

   *z = val/slicesize;
   *y = (val - (*z * slicesize))/dims[0];
   *x = val - (*z * slicesize + *y * dims[0]);
   }
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
      //LOADS THAT REQUIRE ITERATING THROUGH DATASET
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
int SURFACE::Load(meta::Image* mi, float* cfactor)
   {
    Releasemem();

   if (!mi) return 0;
   if(mi->ElemType() != meta::MET_UCHAR)
      {
      std::cerr<<"SURFACE can only load unsigned char mask files"<<std::endl;
      return 0;
      }

   return Load( (unsigned char*)mi->Get(), mi->DimSize());
   }
//--------------------------------------------------------------------
      //Any boundary voxel of non-zero val will be added to list.
int SURFACE::Load(unsigned char* buff, int* d)
   {
   Releasemem();

   if (!buff) return 0;

   int x, y, z, zindex, yindex;
   int offset;
   int i;

   for (i = 0; i < 3; i++)
      {
      dims[i] = d[i];
      min[i] = 99999999;
      max[i] = 0;
      }

   if(dims[2] == 0) dims[2] = 1;
   slicesize = dims[1] * dims[0];

   for (z = 0; z < dims[2]; z++)
      {
      zindex = z * slicesize;
      for (y = 0; y < dims[1]; y++)
         {
         yindex = zindex + y * dims[0];
         for (x = 0; x < dims[0]; x++)
            {
            offset = yindex + x;
            if (IsABoundaryPoint(x,y,z, offset, buff))
               AddPoint(x,y,z,offset);
            }
         }
      }

   FinishLoad();
 
   return 1;
   }
//-------------------------------------------------------------------
      //Only boundary voxels AND voxels of value myval will be added to list
int SURFACE::Load(unsigned char* buff, int* d, int myval)
   {
   Releasemem();

   if (!buff) return 0;

   int x, y, z, zindex, yindex;
   int offset;
   int i;

   for (i = 0; i < 3; i++)
      {
      dims[i] = d[i];
      min[i] = 99999999;
      max[i] = 0;
      }

   if(dims[2] == 0) dims[2] = 1;
   slicesize = dims[1] * dims[0];

   for (z = 0; z < dims[2]; z++)
      {
      zindex = z * slicesize;
      for (y = 0; y < dims[1]; y++)
         {
         yindex = zindex + y * dims[0];
         for (x = 0; x < dims[0]; x++)
            {
            offset = yindex + x;
            if (IsMyBoundaryPoint(x,y,z, offset, buff, myval))
               AddPoint(x,y,z,offset);
            }
         }
      }

   FinishLoad();
   
   return 1;
   }
//------------------------------------------------
      //Load a set of known (x,y,z) points
int SURFACE::LoadKnownCompositeBoundaryPoints(int* d, int n, int* voxels, int* m, int* M)
   {
   Releasemem();

   int i;
   
   try
      {
      pts = new int[n];
      if(! pts)
         {
         std::cerr<<"no mem for points in SURFACE\n"; return 0;
         }
      }
   catch(...)
         {
         std::cerr<<"no mem for points in SURFACE\n"; return 0;
         }


   numpoints = n;

   for (i = 0; i < n; i++)
      pts[i] = voxels[i];

   for(i = 0; i < 3; i++)
      {
      min[i] = m[i];
      max[i] = M[i];
      }

   for (i = 0; i < 3; i++)
      dims[i] = d[i];
   slicesize = d[0] * d[1];

  
   return 1;
   }





//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
      //FUNCTIONS USED BY CALLER SO ONLY ONE ITERATION THROUGH DATA IS NEEDED
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
int SURFACE::InitiateLoad(unsigned char* buff, int* d)
   {
   Releasemem();

   if (!buff) return 0;
   int i;

   for (i = 0; i < 3; i++)
      {
      dims[i] = d[i];
      min[i] = 99999999;
      max[i] = 0;
      }

   if(dims[2] == 0) dims[2] = 1;
   slicesize = dims[1] * dims[0];

   return 1;
   }
//---------------------------------------------------------------------------
int SURFACE::IsMyBoundaryPoint(int x, int y, int z, int composite, unsigned char* buff, int myval)
   {
      //the point must have a value if it is a boundary point
   if (buff[composite] != myval) return 0;

         //If the point has a value and is on a top or bottom slice, at the beginning
         //or end of a row, etc, then it is automatically a boundary point
   if (x == 0 || x == dims[0] - 1) return 1;
   if (y == 0 || y == dims[1] - 1) return 1;
   if (z == 0 || z == dims[2] - 1) return 1;

         //if the point has myval, and an adjacent point does not, then this point
         //is a boundary point. You do not need to error check for bounds because
         //the calls above just took care of that. This just checks for face values;
         //does not process edges or corners
   if (buff[composite - 1] != myval || buff[composite + 1] != myval) return 1;     //x-1, x+1
   if (buff[z * slicesize + (y - 1) * dims[0] + x] != myval) return 1;         //y - 1
   if (buff[z * slicesize + (y + 1) * dims[0] + x] != myval) return 1;         //y + 1
   if (buff[ (z -1) * slicesize + y * dims[0] + x] != myval) return 1;         //z - 1
   if (buff[ (z +1) * slicesize + y * dims[0] + x] != myval) return 1;         //z + 1

         //if you got here, the point is surrounded by points that do have a value.
         //It is therefore not a boundary point.
   return 0;
   }
  //-----------------------------------------------------------------
void SURFACE::AddPoint(int x, int y, int z, int composite)
   {
   lst.AddToTail(composite);
   if (x < min[0]) min[0] = x;
   if (y < min[1]) min[1] = y;
   if (z < min[2]) min[2] = z;
   if (x > max[0]) max[0] = x;
   if (y > max[1]) max[1] = y;
   if (z > max[2]) max[2] = z; 
   }
//--------------------------------------------------------------------
void SURFACE::FinishLoad()
   {
   if (pts) delete [] pts; pts = 0;          //just in case.....
   numpoints = lst.Count();
   if (!numpoints) {return;}

   try
      {
      pts = new int[numpoints];
      if (!pts) {std::cerr<<"No mem for points on SURFACE::FinishLoad\n"; Releasemem(); return;}
      }
   catch(...)
      {std::cerr<<"No mem for points on SURFACE::FinishLoad\n"; Releasemem(); return;}


   int x, y;
   lst.Reset();
   for (x = 0; x < numpoints; x++)
      {
      lst.Get(&y);
      pts[x] = y;
      }

   lst.Releasemem();
   }

//-----------------------------------------
int SURFACE::IsInList(int vox)
   {
   int returnval = 0;

   for (int i = 0; i < numpoints; i++)
      {
      if(pts[i] == vox)
         {
         returnval = 1;
         break;
         }
      }
   return returnval;
   }

//----------------------------------------------
int SURFACE::RecalcMinMax()
   {
   if(!numpoints) return 0;

   int x, y, z;
   register int i;

   GetPoint(0, &x, &y, &z);

   min[0] = max[0] = x;
   min[1] = max[1] = y;
   min[2] = max[2] = z;


   for (i = 1; i < numpoints; i++)
      {
      GetPoint(i, &x, &y, &z);
      if (min[0] > x)  min[0] = x;
      else if (max[0] < x) max[0] = x;

      if (min[1] > y)  min[1] = y;
      else if (max[1] < y)  max[1] = y;

      if (min[2] > z) min[2] = z;
      else if (max[2] < z) max[2] = z;
      }

   return 1;
   }

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
   //PROTECTED: BOUNDARY STUFF
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
      //This version will accept any non-zero value
int SURFACE::IsABoundaryPoint(int x, int y, int z, int composite, unsigned char* buff)
   {
      //the point must have a value if it is a boundary point
   if (!buff[composite]) return 0;

         //If the point has a value and is on a top or bottom slice, at the beginning
         //or end of a row, etc, then it is automatically a boundary point
   if (x == 0 || x == dims[0] - 1) return 1;
   if (y == 0 || y == dims[1] - 1) return 1;
   if (z == 0 || z == dims[2] - 1) return 1;

         //if the point has a value, and an adjacent point does not, then this point
         //is a boundary point. You do not need to error check for bounds because
         //the calls above just took care of that. This just checks for face values;
         //does not process edges or corners
   if (buff[composite - 1] == 0 || buff[composite + 1] == 0) return 1;     //x-1, x+1
   if (buff[z * slicesize + (y - 1) * dims[0] + x] == 0) return 1;         //y - 1
   if (buff[z * slicesize + (y + 1) * dims[0] + x] == 0) return 1;         //y + 1
   if (buff[ (z -1) * slicesize + y * dims[0] + x] == 0) return 1;         //z - 1
   if (buff[ (z +1) * slicesize + y * dims[0] + x] == 0) return 1;         //z + 1

         //if you got here, the point is surrounded by points that do have a value.
         //It is therefore not a boundary point.
   return 0;
   }
 
}//end of namespace