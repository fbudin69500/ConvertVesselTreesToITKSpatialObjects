#include "ebMESHCOL_s.h"

#include "ebVESSCOLUTILS_s.h"

namespace eb
{

MESHCOL_s::MESHCOL_s()
   {
   Initmem();
   nstrips = 6;
   preferquads = 0;
   }
//------------------------------
MESHCOL_s::MESHCOL_s(const MESHCOL_s& a)
   {
   Initmem();
   nmesh = a.nmesh;
   nstrips = a.nstrips;
   preferquads = a.preferquads;

   if(nmesh)
      {
      mesh = new MESH_s[nmesh];
      if (!mesh)
         std::cerr<<"Out of mem in MESHCOL_s()\n";
      else
         {
         for (int i = 0; i < nmesh; i++)
            mesh[i] = a.mesh[i];
         }
      }
   }
//--------------------------------------------
MESHCOL_s& MESHCOL_s::operator = (const MESHCOL_s& a)
   {
   if (this == &a) return *this;

   Releasemem();
   nmesh = a.nmesh;
   nstrips = a.nstrips;
   preferquads = a.preferquads;

   if(nmesh)
      {
      mesh = new MESH_s[nmesh];
      if (!mesh)
         std::cerr<<"Out of mem in MESHCOL_s()\n";
      else
         {
         for (int i = 0; i < nmesh; i++)
            mesh[i] = a.mesh[i];
         }
      }

   return *this;
   }
//-------------------------------------------
MESHCOL_s::~MESHCOL_s()
   {Releasemem();}
//-------------------------------------------
void MESHCOL_s::Releasemem()
   {
   if (nmesh) 
      delete [] mesh;
   Initmem();
   }
//---------------------------------
void MESHCOL_s::Initmem()
   {
   nmesh = 0;
   mesh = 0;
   }
//-----------------------------------
int MESHCOL_s::AllocMemForNMeshes(int n)
   {
   Releasemem();
   if (n >= 0)
      {
      nmesh = n;
      mesh = new MESH_s[n];
      if (!mesh) return 0;
      }

   for(int i = 0; i < nmesh; i++)
      {
      mesh[i].SetNStrips(nstrips);
      mesh[i].SetPreferquads(preferquads);
      }
   return 1;
   }
//-----------------------------------
int MESHCOL_s::CreateSurfaces(SURFACECOL* mc, unsigned char* maskfile, float* s)
   {
   Releasemem();
   AllocMemForNMeshes(mc->GetVessnum());
   if(!mesh) {nmesh = 0; return 0;}

   SURFACE* temp;
   int i;

   for (i = 0; i <nmesh; i++)
     {
     temp = mc->GetSurface(i);
     if(!mesh[i].CreateSurface(temp, maskfile, s, i + 1))
         {
         Releasemem();
         return 0;
         }
     }

   return 1;
   }
//----------------------------------------
   //This allows creation of a mesh that contains vessels but also has extraslots
   //spaces for other things later.
int MESHCOL_s::CreateVessels(VESSCOL_s* vc, int extraslots)
   {
   Releasemem();
   int num = vc->GetVessnum();
   if (extraslots > 0)
      num += extraslots;


   AllocMemForNMeshes(num);
   if (!mesh)  {nmesh = 0; return 0;}
   float* fptr;


   for (int i = 0; i < vc->GetVessnum(); i++)
      {
      mesh[i].SetColor(vc->GetColor(i));
      mesh[i].SetOpacity(vc->GetOpacity(i));

     //if(!vc->IsVisible(i)) continue;

     fptr = vc->GetSample(i, &num);
     if (num <= 0) {std::cerr<<"\n\nMESHCOL_s::CreateVessels():Failure to create vessel mesh for vess "<<i<<" with "<<num <<" points; continuing\n\n";continue;}
     

      if(!mesh[i].CreateVessel(fptr, num, vc->GetDims(), vc->GetSteps()))
         {
         Releasemem();
         std::cerr<<"\n\nMESHCOL_s::CreateVessels():Failure to create vessel mesh for vess "<<i<<" with "<<num <<" points\n\n";
         return 0;
         }
     }
   return 1;
   }

 //----------------------------------------------------------
int MESHCOL_s::CreateVessel(VESSCOL_s* vc, int a)
   {
   if (nmesh <= a) return 0;
   if(!mesh) return 0;

   float* fptr;
   int num;

   mesh[a].SetColor(vc->GetColor(a));
   mesh[a].SetOpacity(vc->GetOpacity(a));

   //if(!vc->IsVisible(a)) return 1;

   fptr = vc->GetSample(a, &num);
   if(!fptr) return 0;

   if(!mesh[a].CreateVessel(fptr, num, vc->GetDims(), vc->GetSteps()))
      return 0;

   return 1;
   }
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
      //SAVES and LOAD
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
int MESHCOL_s::Save(char* fname)
   {
   if (!fname) {std::cerr<<"No filename\n"; return 0;}

   std::ofstream ofile;
   ofile.open(fname);
   if(!ofile) {std::cerr<<"Could not open file\n"; return 0;}

   ofile<<"MESH file\n";
   ofile<<"NMesh:  "<<nmesh<<std::endl;
   ofile<<"NStrips:  "<<nstrips<<std::endl;
   ofile<<"PreferQuads: "<<preferquads<<std::endl;
   ofile<<"End Header:\n\n";

  
   for (int i = 0; i <nmesh; i++)
      mesh[i].Save(&ofile);

   ofile.close();

   std::cerr<<"Mesh saved\n";
   return 1;
   }
//-----------------------------------------
int MESHCOL_s::Load(char* fname)
   {
   if(!fname) return 0;
   std::ifstream ifile;
   ifile.open(fname);
   if(!ifile) return 0;

   Releasemem();

   char commandstring[500];
   int OK = 0;
   char* cptr;
   int i;

         //check that this is the right kind of file
   while (ifile)
      {
      ifile.getline(commandstring, 499);
      if (!strcmp(commandstring, "MESH file"))
         {
         OK = 1;
         break;
         }
      }

   if(!OK)
      {
      std::cerr<<"This is not a mesh file\n";
      return 0;
      }

         //Get the header and allocate mem for meshes. Although nstrips and preferquads are
         //written in the header, it may be best for each mesh to have its own. Am not loading
         //it here, therefore. 
   while (ifile)
      {
      if (!strcmp(commandstring, "End Header:")) break;
      ifile.getline(commandstring, 499);
      
      if(!strncmp(commandstring, "NMesh:", 6))
         {
         cptr = commandstring + 6;
         nmesh = atoi(cptr);
         continue;
         }
      }

   if (nmesh <= 0)
      {
      std::cerr<<"Invalid number of meshes. Returning\n";
      return 0;
      }

   if(!AllocMemForNMeshes(nmesh))
      {
      std::cerr<<"Could not create meshes\n";
      Releasemem();
      return 0;
      }

   for (i = 0; i < nmesh; i++)
      {
      if (!mesh[i].Load(&ifile))
         {
         std::cerr<<"Creation of mesh "<<i<<" failed; returning\n";
         Releasemem();
         return 0;
         }
      }

   return 1;
   }
//----------------------------------------------
   //Get the min and max vertex coords
int MESHCOL_s::GetMinMax(float* mins, float* max)
   {
   if (!nmesh) return 0;

   if(!mesh[0].GetMinMax(mins, max)) return 0;

   float tempmin[3], tempmax[3];

   for (int i = 1; i < nmesh; i++)
      {
      if(!mesh[i].GetMinMax(tempmin, tempmax)) continue;
      for (int j = 0; j < 3; i++)
         {
         if (tempmin[j] < mins[j]) mins[j] = tempmin[j];
         if (tempmax[j] > max[j]) max[j] = tempmax[j];
         }
      }
   return 1;
   }

   


}//end of namespace