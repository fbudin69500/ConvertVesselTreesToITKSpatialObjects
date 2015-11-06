#include "ebFILEHEADERIO.h"

#include "ebTBOX.h"

namespace eb
{

FILEHEADERIO::FILEHEADERIO()
   {
   Init();
   corfactor[0] = 1; corfactor[1] = 1; corfactor[2] = (float)1;
   }
//--------------------------------------------------
void FILEHEADERIO::Set(int nd, int* d, float* s, int vn)
   {
   if (nd < 2 || nd > 3)
      {
      std::cerr<<"Invalid dimensions"; return;
      }

   dims[2] = 1;
   steps[2] = (float)1;

   for (int i = 0; i < nd; i++)
      {
      dims[i] = d[i];
      steps[i] = s[i];
      }
   ndims = nd;
   vessnum = vn;
   }

//-------------------------------------------------
void FILEHEADERIO::Init()
   {
   dims[0] = dims[1] = dims[2] = 0;
   steps[0] = steps[1] = steps[2] = 0;
   vessnum = 0;
   ndims = 3;
   }
//--------------------------------------------
int FILEHEADERIO::LoadHeader(std::ifstream* ifile)
   {
    char s[500];
   TBOX tbox;
   char* cptr;
   int returnval = 1;
   double d[4];

         //my header requires 3 things: Dimensions, voxelsize, and nobjects. Ignore everything
         //else. Header ends at End Header:
   Init();

   while (*ifile)
      {
      ifile->getline(s, 299);
      if (!tbox.Uppercase(s)) continue;
      if(!strncmp(s, "END HEADER:", 11)) break;

            //Stephen requires number of dimensions. My old files do not. If NDims is set to 
            //something other than 3, get out.
      if(!strncmp(s, "NDIMS:", 5))
         {
         cptr = s + 6;
         ndims = atoi(cptr);
         }

               //look for voxel dimensions
      else if (!strncmp(s, "DIMENSIONS:", 11))
         {
         cptr = s + 12;
       if (!tbox.Stringtondoubles(cptr, 3, d))
            returnval = 0;
      dims[0] = (int)d[0];
      dims[1] = (int)d[1];
      dims[2] = (int)d[2];
         }

            //look for voxel stepsize
      else if (!strncmp(s, "VOXELSIZE:", 10))
         {
         cptr = s + 11;
       if (!tbox.Stringtondoubles(cptr, 3, d))
            returnval =  0;
       steps[0] = (float)d[0]/10  * corfactor[0];
       steps[1] = (float)d[1]/10 * corfactor[1];
       steps[2] = (float)d[2]/10 * corfactor[2];
         }

            //look for number of objects
      else if (!strncmp(s, "NOBJECTS:", 8))
         {
         cptr = s + 9; 
         vessnum = atoi(cptr);
         }
      }     //end while ifile
   
            //prevent trouble later
   if (ndims == 2)
      {
      dims[2] = 1;
      steps[2] = 1;
      }


             //check that mandatory things were found
   if(dims[0] <= 0 || dims[1] <= 0 || dims[2] <= 0)
         {
         std::cerr<<"Invalid dimensions\n";
         returnval = 0;
         }

   if(steps[0] <= 0 || steps[1] <= 0 || steps[2] <= 0)
         {
         std::cerr<<"Invalid stepsize\n";
         returnval = 0;
        }

    if(vessnum <= 0)
         {
         std::cerr<<"Invalid vessel number in header\n";
         returnval = 0;
         }

   if(!returnval) Init();

    return returnval;
   }

//--------------------------------------------------
int FILEHEADERIO::WriteHeader(std::ofstream* ofile)
   {
   int i;

   if(!*ofile) return 0;

   *ofile<<"NDims: "<<ndims<<std::endl;

   *ofile<<"Dimensions: ";
   for (i = 0; i < ndims; i++)
      *ofile<<dims[i]<<"  ";
   *ofile<<std::endl;

   *ofile<<"VoxelSize: ";
   for (i = 0; i < ndims; i++)
      *ofile<<steps[i]* 10/corfactor[i]<<"  ";
   *ofile<<std::endl;

   *ofile<<"NObjects: ";
   *ofile<<vessnum<<std::endl;

   *ofile<<"End Header:"<<std::endl<<std::endl;

    return 1;
   }

}//end of namespace