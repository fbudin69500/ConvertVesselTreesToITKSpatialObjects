#include "ebSURFACECOL.h"

namespace eb
{

SURFACECOL::SURFACECOL()
   {
   surfaces = 0;     //no surfaces defined on entry
   numsurfaces = 0;

   mode = 0;         //default is single object
   errormsg[0] = '\0';

   for (int i = 0; i < 3; i++)
      {
      min[i] = 0;
      max[i] = 0;
      }
   }
//----------------------------------------
SURFACECOL::~SURFACECOL()
   {ReleaseSurfaceMem();}
//---------------------------------------
void SURFACECOL::ReleaseSurfaceMem()
   {
   if (surfaces) delete [] surfaces;
   surfaces = 0;
   numsurfaces = 0;
   errormsg[0] = '\0';

   for (int i = 0; i < 3; i++)
      {
      min[i] = max[i] = 0;
      }
   }
//-------------------------------------
      //Load a maskfile. Options exist to either load all non-zero values into a single
      //object or to treat each different numerical non-zero object as an independent object
      //(all 2s go together, fe). In this latter case, objects are expected to begin at one
      //and increase one at a time. You will run out of mem if you try to load a million
      //different numbers. There is no 255 restriction, however.
int SURFACECOL::LoadMask(int* expected_dims, char* filename)
   {
   int i, zindex, yindex, slicesize;
   unsigned char* buff = 0;
   float f;
   register int x,y,z;


   ReleaseSurfaceMem();

            //check that the file loads
   meta::Image mi(filename);
   if (mi.Error())
      {
      strcpy_s(errormsg,200,  mi.Error()); 
      return 0;
      }

            //check that dims are correct
   int* d = mi.DimSize();
   for (i = 0; i < 3; i++)
      {
      if (expected_dims[i] != d[i])
         {
         strcpy_s(errormsg,200,  "Load failed; dimensions of mask different than expected");
         std::cerr<<std::endl<<errormsg<<std::endl;
         return 0;
         }
      }


 
    slicesize = d[0] * d[1];
    try
       {
       buff = new unsigned char[slicesize * d[2]];
       if (! buff)
          {strcpy_s(errormsg,200, "No mem for surface load"); return 0;}
       }
    catch(...)
       {strcpy_s(errormsg,200, "No mem for surface load"); return 0;}

    for (z = 0; z < d[2]; z++)
       {
       zindex = z * slicesize;
       for (y = 0; y < d[1]; y++)
          {
          yindex = zindex + y * d[0];
          for (x = 0; x < d[0]; x++)
             {
             f = mi.Get(x,y,z);
             buff[yindex + x] = (unsigned char) f;
             }
          }
       }

  
    LoadMaskBuffer(expected_dims, buff);
    delete [] buff;
    Show();

  

    return 1;
   }
 //-----------------------------------------------
 int SURFACECOL::LoadMaskBuffer(int* d, unsigned char* buff)
    {
    if (!buff) {std::cerr<<" No buffer in SURFACECOL::LoadMaskBuffer\n"; return 0;}
    ReleaseSurfaceMem();
    

    int composite, i, j, x,y,z, slicesize, zindex, yindex;
    int* m;
    int* M;

    for (i = 0; i < 3; i++)
       dims[i] = d[i];
 

    try
       {
              //you have loaded mask info into buff. Collect data.
      switch (mode)
         {
         case 0:       //single object

          surfaces = new SURFACE[1];
          if (!surfaces)
             {
             numsurfaces = 0;
             strcpy_s(errormsg,200,  "Load failed: out of mem");
             return 0;
             }
          numsurfaces = 1;
          surfaces[0].Load(buff, dims);

          break;

         case 1:       //predefined object number; turned out not to need it.
         
          break;

         case 2:       //auto load. Assumes a file with input data 1,2 etc

               //determine the number of objects to be loaded
          for (i = 0; i < dims[0] * dims[1] * dims[2]; i++)
             {
             if (buff[i] > numsurfaces)
                numsurfaces = buff[i];
             }

          if (numsurfaces == 0)
             {
             strcpy_s(errormsg,200,  "Load failed: No objects in image");
             std::cerr<<std::endl<<errormsg<<std::endl;
             return 0;
             }


           surfaces = new SURFACE[numsurfaces];

           if (!surfaces)
             {
             numsurfaces = 0;
             strcpy_s(errormsg,200,  "Load failed: out of mem");
             std::cerr<<"Load failed; out of mem\n";
             return 0;
             }

 
                     //prepare each surface for a new load
           slicesize = dims[0] * dims[1];
           for (i = 0; i < numsurfaces; i++)
              surfaces[i].InitiateLoad(buff, dims);

                     //cycle through the file, adding boundary points to each surface
           for (z = 0; z< dims[2]; z++)
              {
              zindex = slicesize * z;
              for (y = 0; y < dims[1]; y++)
                 {
                 yindex = zindex + y * dims[0];
                 for (x = 0; x < dims[0]; x++)
                    {
                    composite = yindex + x;
                    i = (int) buff[composite];
                    if (!i) continue;
                    if (surfaces[i - 1].IsMyBoundaryPoint(x,y,z,composite, buff, i))
                       surfaces[i - 1].AddPoint(x,y,z,composite);
                    }
                 }
              }

                     //tell each surface it is finished loading and to process.
           for (i = 0; i < numsurfaces; i++)
              surfaces[i].FinishLoad();

         
            break;
         }
       }

    catch(...)
       {
       numsurfaces = 0;
       strcpy_s(errormsg,200,  "Load failed: out of mem");
       return 0;
       }

            //Get the global min max for the defined objects
    for (i = 0; i < 3; i++)
       {
       min[i] = 999999;
       max[i] = -999999;
       }

    for (i = 0; i < numsurfaces; i++)
       {
       if(!surfaces[i].GetNumpoints()) continue;

       m = surfaces[i].GetMin();
       M = surfaces[i].GetMax();
       for (j = 0; j < 3; j++)
          {
          if (m[j] < min[j]) min[j] = m[j];
          if (M[j] > max[j]) max[j] = M[j];
          }
       }

    std::cerr<<std::endl<<numsurfaces<<" object(s) loaded\n";
    Show();

    return 1;
    }
    //--------------------------------------------
int SURFACECOL::Concatenate(SURFACECOL* a, SURFACECOL* b)
   {
   int i;

   for (i = 0; i < 3; i++)
      {
      if (a->dims[i] != b->dims[i])
         {
         std::cerr<<"Dimensions of the two surfaces are not the same\n";
         strcpy_s(errormsg,200,  "Dimensions of the two surfaces are not the same");
         return 0;
         }
      }

            //one of the passed pointers may be this class instance, so be careful--
            //get a copy before deleting your mem.
   int ns = a->numsurfaces + b->numsurfaces;
   SURFACE* s;

   try
      {
      s = new SURFACE[ns];
      if(!s)
         {
         std::cerr<<"No mem for concatenated surfaces\n";
         strcpy_s(errormsg,200,  "No mem for concatenated surfaces\n");
         return 0;
         }
      }
   catch(...)
      {
      std::cerr<<"No mem for concatenated surfaces\n";
      strcpy_s(errormsg,200,  "No mem for concatenated surfaces\n");
      return 0;
      }

   for (i = 0; i < a->numsurfaces; i++)
      s[i] = a->surfaces[i];

   for (i = 0; i < b->numsurfaces; i++)
      s[a->numsurfaces + i] = b->surfaces[i];

   int m[3];
   int M[3];

   for (i = 0; i < 3; i++)
      {
      m[i] = a->min[i];
      if (b->min[i] < m[i])
         m[i] = b->min[i];

      M[i] = a->max[i];
      if (b->max[i] > M[i])
         M[i] = b->max[i];
      }

   Releasemem();

   numsurfaces = ns;
   surfaces = s;
   for (i = 0; i < 3; i++)
      {
      min[i] = m[i];
      max[i] = M[i];
      }

   return 1;
   }
//------------------------------------------------------
      //All surfaces must have the same dimensions.
int SURFACECOL::LoadKnownSurfaces(int n, int* _dims, SURFACE* s)
   {
   Releasemem();

   if(n <= 0 || !s) return 0;

   numsurfaces = n;

   try
      {
      surfaces = new SURFACE[numsurfaces];
      if (!surfaces)
         {
         strcpy_s(errormsg,200, "Out of mem in SURFACECOL::LoadKnownSurfaces()");
         std::cerr<<"Out of mem in SURFACECOL::LoadKnownSurfaces()\n";
         return 0;
         }
      }
   catch(...)
      {
      strcpy_s(errormsg,200, "Out of mem in SURFACECOL::LoadKnownSurfaces()");
      std::cerr<<"Out of mem in SURFACECOL::LoadKnownSurfaces()\n";
      return 0;
      }

      int i, j;
      int* m;
      int* M;

 
   for (i = 0; i < n; i++)
      surfaces[i] = s[i];

   for(i = 0; i < 3; i++)
      dims[i] = _dims[i];

   m = surfaces[0].GetMin();
   M = surfaces[0].GetMax();
   for(i = 0; i < 3; i++)
      {
      min[i] = m[i];
      max[i] = M[i];
      }

   for (i = 1; i< numsurfaces; i++)
      {
      m = surfaces[i].GetMin();
      M = surfaces[i].GetMax();
      for (j = 0; j < 3; j++)
         {
         if (m[i] < min[i])  min[i] = m[i];
         else if (M[i] > max[i])  max[i] = M[i];
         }
      }
   Show();
   return 1;
   }

//-----------------------------------------
      //Say if a voxel of interest is contained in one of the lists,
      //and, if so, which one
int SURFACECOL::IsInList(int vox, int* whichcollection)
   {
   int returnval = 0;

   for (int i = 0; i < numsurfaces; i++)
      {
      if (surfaces[i].IsInList(vox))
         {
         *whichcollection = i;
         returnval = 1;
         break;
         }
      }
   return returnval;
   }

//----------------------------------------------------------
int SURFACECOL::RecalcMinMax()
   {
   if(!numsurfaces)      return 0;

   register int i, j;
   int* tempmin;
   int* tempmax;


   for (i = 0; i < numsurfaces; i++)
      surfaces[i].RecalcMinMax();

   tempmin = surfaces[0].GetMin();
   tempmax = surfaces[0].GetMax();

      
            //store the mins and maxes of the first surface
   for (i = 0; i < 3; i++)
      {
      min[i] = tempmin[i];
      max[i] = tempmax[i];
      }

   for (i = 1; i < numsurfaces; i++)
      {
      tempmin = surfaces[i].GetMin();
      tempmax = surfaces[i].GetMax();

      for (j = 0; j < 3; j++)
         {
         if (tempmin[j] < min[j])      min[j] = tempmin[j];
         if (tempmax[j] > max[j])      max[j] = tempmax[j];
         }
      }
   return 1;
   }
      

   




}//end of namespace