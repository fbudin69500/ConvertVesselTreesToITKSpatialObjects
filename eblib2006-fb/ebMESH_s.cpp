#include "ebMESH_s.h"

#include <memory>

      //eblib
#include "ebMATH.h"
#include "ebGEOM.h"
#include "ebLIST.h"

namespace eb
{

MESH_s::MESH_s()
   {
   Initmem();

         //give some default color and opacity
   col[0] = (float)0.6;
   col[1] = (float)0.4;
   col[2] = (float)0.20; 
   opacity = 1;

         //set default to triangles
   preferquads = 0;

         //default for triangle strips or sphere is division by 6
   nstrips = 6;
  }
//------------------------------------
MESH_s::MESH_s(const MESH_s& a)
   {
   Initmem();     

   for (int i = 0; i < 3; i++)
      {
      dims[i] = a.dims[i];
      steps[i] = a.steps[i];
      col[i] = a.col[i];
      }

   slicesize = a.slicesize;
   totalsize = a.totalsize;
   opacity = a.opacity;
   meshtype = a.meshtype;
   nstrips = a.nstrips;


   numvertices = a.numvertices;
   vertices = new FVEC[numvertices];

   numvertexindices = a.numvertexindices;
   vertexindices = new int[numvertexindices];

   numfacets = a.numfacets;
   numfacetsrfn = a.numfacetsrfn;
   facetsrfn = new FVEC[numfacetsrfn];

   vertexsrfn = new FVEC[numvertices];


   if(!vertexsrfn || !facetsrfn || !vertexindices || !vertices)
      Releasemem();

   int sfv = sizeof(FVEC);

   memcpy(vertices, a.vertices, numvertices * sfv);
   memcpy(vertexindices, a.vertexindices, numvertexindices * sizeof(int));
   memcpy(facetsrfn, a.facetsrfn, numfacetsrfn * sfv);
   memcpy(vertexsrfn, a.vertexsrfn, numvertices * sfv);

   numtrianglefacets = a.numtrianglefacets;
   onfacet = a.onfacet;
   vertexstartquads = a.vertexstartquads;
   }
//----------------------------------------------
MESH_s& MESH_s::operator =(const MESH_s& a)
   {
   if(this == &a) 
      return *this;

   Releasemem();
   Initmem();     

   for (int i = 0; i < 3; i++)
      {
      dims[i] = a.dims[i];
      steps[i] = a.steps[i];
      col[i] = a.col[i];
      }

   slicesize = a.slicesize;
   totalsize = a.totalsize;
   opacity = a.opacity;
   meshtype = a.meshtype;
   nstrips = a.nstrips;


   numvertices = a.numvertices;
   vertices = new FVEC[numvertices];

   numvertexindices = a.numvertexindices;
   vertexindices = new int[numvertexindices];

   numfacets = a.numfacets;
   numfacetsrfn = a.numfacetsrfn;
   facetsrfn = new FVEC[numfacetsrfn];

   vertexsrfn = new FVEC[numvertices];


   if(!vertexsrfn || !facetsrfn || !vertexindices || !vertices )
      Releasemem();

   else
      {
      int sfv = sizeof(FVEC);

      memcpy(vertices, a.vertices, numvertices * sfv);
      memcpy(vertexindices, a.vertexindices, numvertexindices * sizeof(int));
      memcpy(facetsrfn, a.facetsrfn, numfacetsrfn * sfv);
      memcpy(vertexsrfn, a.vertexsrfn, numvertices * sfv);
      }

   numtrianglefacets = a.numtrianglefacets;
   onfacet = a.onfacet;
   vertexstartquads = a.vertexstartquads;

   return *this;
   }


//-----------------------------------
MESH_s::~MESH_s()
   {Releasemem();}
//-------------------------------------
void MESH_s::Releasemem()
   {
   if (vertices)
      delete [] vertices;

   if (facetsrfn)
      delete [] facetsrfn;

   if (vertexsrfn)
      delete [] vertexsrfn;

   if(vertexindices)
      delete [] vertexindices;


   Initmem();
   }
//----------------------------------------
void MESH_s::Initmem()
   {
   vertices = 0;
   numvertices = 0;

   facetsrfn = 0;
   numfacets = 0;
   numtrianglefacets = 0;

   vertexsrfn = 0;

   vertexindices = 0;
   numvertexindices = 0;
   vertexstartquads = 0;


   meshtype = -1;          //invalid meshtype


   onfacet = 0;
   }
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
      //LOADS
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
int MESH_s::CreateVessel(VESS_s* vess, int* d, float* s)
   {
   float* pts = vess->GetSample(&numskelpoints);
   if(!pts) return 0;


   float* fptr = vess->GetColor();
   col[0] = fptr[0];
   col[1] = fptr[1];
   col[2] = fptr[2];

   if(!numskelpoints) return 0;


   return CreateVessel(pts, numskelpoints, d, s);
   }
//------------------------------------------
      //Right now am just using quads
int MESH_s::CreateOvoid(float* skel, float* scale)
   {
   Releasemem();
   numskelpoints = 1;
   preferquads = 1;

   if(!CreateOvoidQuadMesh(skel,scale))
      return 0;

   if(!CreateOvoidVertexNormals())
      return 0;

   meshtype = 0;
   return 1;
   }
//----------------------------------------------------------
int MESH_s::CreateVessel(float* pts, int num, int* d, float* s)
   {
   Releasemem();
   if (num <= 0 || !pts) return 0;

   numskelpoints = num;

     for (int i = 0; i < 3; i++)
      {
      dims[i] = d[i];
      steps[i] = s[i];
      }
     slicesize = d[0] * d[1];
     totalsize = slicesize * d[2];

   if(numskelpoints == 1)
      {
      float scale[3] = {pts[3],pts[3],pts[3]};
      return CreateOvoid(pts, scale);
      }

  
   if (preferquads)
      {
      if(!CreateVesselQuadMesh(pts))
         return 0;
      }
   else if(!CreateVesselTriangleMesh(pts))
      return 0;


   if(!CreateVesselFacetSurfaceNormals())
      return 0;

   if(!CreateVesselVertexNormals())
      return 0;

   meshtype = 1;                 //vessels

   opacity = 1;

   return 1;
   }
//----------------------------------------
      //This just coats the surfaces of voxels. No point in doing
      //anything other than quads. This function recognizes a single
      //id in the mask file as its own. 
int MESH_s::CreateSurface(SURFACE* surf, unsigned char* maskfile, float* s, int myid) 
   {
   if(!surf || !maskfile) return 0;

   Releasemem();

   preferquads = 1;
   numskelpoints = surf->GetNumpoints();
   int* d = surf->GetDims();

   for (int i = 0; i < 3; i++)
      {
      dims[i] = d[i];
      steps[i] = s[i];
      }
   slicesize = d[0] * d[1];
   totalsize = slicesize * d[2];

   std::cerr<<"Creating surface...";

   if(!CreateSurfaceQuadMesh(surf, maskfile, myid))
      return 0;

  vertexsrfn = new FVEC[numvertices];
  if (!vertexsrfn) {std::cerr<<"Out of mem in MESH_s\n"; Releasemem(); return 0;}

  std::cerr<<"creating normals...";

  CreateSurfaceVertexNormals();

  std::cerr<<"done\n";

    meshtype = 2;
   return 1;
   }

//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
      //FOR CALLER TO GET THE NEXT FACET
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
         //The triangle facets are placed first, if there is a mix
         //of triangles and quads. Use this function if you are going
         //to do flat or phong shading
FVEC* MESH_s::GetFirstTriangle(FVEC* facetnormal)
   {
   if(!numtrianglefacets) return 0;
   onfacet = 0;

   currentfacet[0] = vertices[vertexindices[0]];
   currentfacet[1] = vertices[vertexindices[1]];
   currentfacet[2] = vertices[vertexindices[2]];

   *facetnormal = facetsrfn[onfacet];
   return currentfacet;
   }
//------------------------------------------
      //Use this function for Gouraud shading.
FVEC* MESH_s::GetFirstTriangle(FVEC* facetnormal, FVEC* sn1, FVEC* sn2, FVEC* sn3)
   {
   if(!numtrianglefacets) return 0;
   onfacet = 0;
   int index = 3 * onfacet;

   currentfacet[0] = vertices[vertexindices[index]];
   currentfacet[1] = vertices[vertexindices[index + 1]];
   currentfacet[2] = vertices[vertexindices[index + 2]];

   *facetnormal = facetsrfn[onfacet];
   *sn1 = vertexsrfn[vertexindices[index]];
   *sn2 = vertexsrfn[vertexindices[index + 1]];
   *sn3 = vertexsrfn[vertexindices[index + 2]];

   return currentfacet;
   }
//-----------------------------------------------------------------
FVEC* MESH_s::GetNextTriangle(FVEC* facetnormal)
   {
   ++onfacet;
   if(onfacet >= numtrianglefacets) 
      return 0;
   int index = 3 * onfacet;

   currentfacet[0] = vertices[vertexindices[index]];
   currentfacet[1] = vertices[vertexindices[index + 1]];
   currentfacet[2] = vertices[vertexindices[index + 2]];

   *facetnormal = facetsrfn[onfacet];
   return currentfacet;
   }
//----------------------------------------------------------
FVEC* MESH_s::GetNextTriangle(FVEC* facetnormal, FVEC* sn1, FVEC* sn2, FVEC* sn3)
   {
   ++onfacet;
   if(onfacet >= numtrianglefacets) 
      return 0;
   int index = 3 * onfacet;

   currentfacet[0] = vertices[vertexindices[index]];
   currentfacet[1] = vertices[vertexindices[index + 1]];
   currentfacet[2] = vertices[vertexindices[index + 2]];

   *facetnormal = facetsrfn[onfacet];
   *sn1 = vertexsrfn[vertexindices[index]];
   *sn2 = vertexsrfn[vertexindices[index + 1]];
   *sn3 = vertexsrfn[vertexindices[index + 2]];

   return currentfacet;
   }
//--------------------------------------------------------
FVEC* MESH_s::GetFirstQuad(FVEC* facetnormal)
   {
    onfacet = numtrianglefacets;
   if (onfacet >= numfacets)
      return 0;

   int index = onfacet * 3;
   currentfacet[0] = vertices[vertexindices[index]];
   currentfacet[1] = vertices[vertexindices[index + 1]];
   currentfacet[2] = vertices[vertexindices[index + 2]];
   currentfacet[3] = vertices[vertexindices[index + 3]];

   *facetnormal = facetsrfn[onfacet];
   return currentfacet;
   }
//-----------------------------------------------
FVEC* MESH_s::GetFirstQuad(FVEC* facetnormal, FVEC* sn1, FVEC* sn2, FVEC* sn3, FVEC* sn4)
   {
   onfacet = numtrianglefacets;
   if (onfacet >= numfacets)
      return 0;

   int index = onfacet * 3;
   currentfacet[0] = vertices[vertexindices[index]];
   currentfacet[1] = vertices[vertexindices[index + 1]];
   currentfacet[2] = vertices[vertexindices[index + 2]];
   currentfacet[3] = vertices[vertexindices[index + 3]];

   *facetnormal = facetsrfn[onfacet];
   

   *sn1 = vertexsrfn[vertexindices[index]];
   *sn2 = vertexsrfn[vertexindices[index + 1]];
   *sn3 = vertexsrfn[vertexindices[index + 2]];
   *sn4 = vertexsrfn[vertexindices[index + 3]];

   return currentfacet;
   }
//----------------------------------------
FVEC* MESH_s::GetNextQuad(FVEC* facetnormal)
   {
   ++onfacet;
   if (onfacet >= numfacets) return 0;
   int index = vertexstartquads + (onfacet - numtrianglefacets) * 4;

   currentfacet[0] = vertices[vertexindices[index]];
   currentfacet[1] = vertices[vertexindices[index + 1]];
   currentfacet[2] = vertices[vertexindices[index + 2]];
   currentfacet[3] = vertices[vertexindices[index + 3]];

   *facetnormal = facetsrfn[onfacet];
   return currentfacet;
   }
//------------------------------------------
FVEC* MESH_s::GetNextQuad(FVEC* facetnormal, FVEC* sn1, FVEC* sn2, FVEC* sn3, FVEC* sn4)
   {
   ++onfacet;
   if (onfacet >= numfacets)
      return 0;

   int index = vertexstartquads + (onfacet - numtrianglefacets) * 4;
   currentfacet[0] = vertices[vertexindices[index]];
   currentfacet[1] = vertices[vertexindices[index + 1]];
   currentfacet[2] = vertices[vertexindices[index + 2]];
   currentfacet[3] = vertices[vertexindices[index + 3]];

   *facetnormal = facetsrfn[onfacet];
   *sn1 = vertexsrfn[vertexindices[index]];
   *sn2 = vertexsrfn[vertexindices[index + 1]];
   *sn3 = vertexsrfn[vertexindices[index + 2]];
   *sn4 = vertexsrfn[vertexindices[index + 3]];

   return currentfacet;
   }
//---------------------------------------
      //Get skeleton id from facet number. Only is useful for vessels
int MESH_s::GetSkeletonPointFromFacetNumber(int id)
   {
   if (!numfacets) return 0;
   if (nstrips <= 0) return 0;
   if (id < 0 || id >= numfacets) return 0;

   if (meshtype == 0 || meshtype > 1)     //not a vessel
      return id;


   int maxskelnum = (numfacets - 2 * nstrips)/(2 * nstrips);

   if (id < nstrips) return 0;         //on first endcap
   else if (id < 2 * nstrips) return maxskelnum; //on last endcap
   else return (id - 2 * nstrips)/(2 * nstrips);
   }
 //&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
      //LOADS AND SAVES
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
int MESH_s::Save(std::ofstream* ofile)
   {
   int i;
   float* fptr;

   *ofile<<std::endl<<"MESH_s\n";
   *ofile<<"Dims: "<<dims[0]<<"  "<<dims[1]<<"  "<<dims[2]<<std::endl;

         //correct for my cm handling
   *ofile<<"Steps: "<<steps[0]* 10<<"  "<<steps[1] * 10<<"  "<<steps[2] * 10<<std::endl;


   *ofile<<"Color: "<<col[0]<<"  "<<col[1]<<"  "<<col[2]<<std::endl;
   *ofile<<"NumSkelPoints: "<<numskelpoints<<std::endl;

            //note that slicesize and totalsize are not written--these can be calculated on load
            //Currentfacet and onfacet are also not set.
   *ofile<<"NumTriangleFacets: "<<numtrianglefacets<<std::endl;
   *ofile<<"NumFacets: "<<numfacets<<std::endl;
   *ofile<<"NumVertices: "<<numvertices<<std::endl;
   *ofile<<"VertexStartQuads: "<<vertexstartquads<<std::endl;
   *ofile<<"NumVertexIndices: "<<numvertexindices<<std::endl;
   *ofile<<"NumFacetSrfn: "<<numfacetsrfn<<std::endl;
   *ofile<<"MeshType: "<<meshtype<<std::endl;
   *ofile<<"NStrips:  "<<nstrips<<std::endl;
   *ofile<<"Preferquads: "<<preferquads<<std::endl;
   *ofile<<"END HEADER:\n";

   *ofile<<"VERTICES:\n";
   for (i = 0; i < numvertices; i++)
      {
      fptr = vertices[i].Get();
      *ofile<<fptr[0]<<"  "<<fptr[1]<<"  "<<fptr[2]<<std::endl;
      }

   *ofile<<"VERTEXINDICES:\n";
   for (i = 0; i < numvertexindices; i++)
      *ofile<<vertexindices[i]<<std::endl;


   *ofile<<"FACETSRFN:\n";
   for(i = 0; i < numfacetsrfn; i++)
      {
      fptr = facetsrfn[i].Get();
      *ofile<<fptr[0]<<"  "<<fptr[1]<<"  "<<fptr[2]<<std::endl;
      }

   *ofile<<"VERTEXSRFN:\n";
   for (i = 0; i < numvertices; i++)
      {
      fptr = vertexsrfn[i].Get();
      *ofile<<fptr[0]<<"  "<<fptr[1]<<"  "<<fptr[2]<<std::endl;
      }
   *ofile<<"END MESH_s\n";
   return 1;
   }
//------------------------------------------------------
int MESH_s::Load(std::ifstream* ifile)
   {
   char commandstring[500];

   TBOX tbox;
   float pt3[3];
   char* cptr;
   int i;

   if(!*ifile) return 0;
   Releasemem();

   nstrips = 6;         //default
   preferquads = 0;      //default


   while (ifile)
      {
      ifile->getline(commandstring, 299);
      if(!ifile) break;

      if(!strcmp(commandstring, "END HEADER:"))
         break;
      if (!strcmp(commandstring, "END MESH_s"))
         break;

      if(!strncmp(commandstring, "Dims:", 5))
         {
         cptr = commandstring + 5;
         tbox.Stringtonints(cptr, 3, dims);
         continue;
         }
      if(!strncmp(commandstring, "Steps:", 6))
         {
         cptr = commandstring + 6;
         tbox.Stringtonfloats(cptr, 3, steps);

               //correct for my cm handling
         steps[0] *= (float)0.1; 
         steps[1] *= (float)0.1; 
         steps[2] *= (float)0.1;

         continue;
         }
      if(!strncmp(commandstring, "Color:", 6))
         {
         cptr = commandstring + 6;
         tbox.Stringtonfloats(cptr, 3, col);
         continue;
         }
      if(!strncmp(commandstring, "NumSkelPoints:", 14))
         {
         cptr = commandstring + 14;
         numskelpoints = atoi(cptr);
         continue;
         }
      if(!strncmp(commandstring, "NumTriangleFacets:", strlen("NumTriangleFacets:")))
         {
         cptr = commandstring + strlen("NumTriangleFacets:");
         numtrianglefacets = atoi(cptr);
         continue;
         }

      if(!strncmp(commandstring, "NumFacets:", 10))
         {
         cptr = commandstring + 10;
         numfacets = atoi(cptr);
         continue;
         }
      
      if(!strncmp(commandstring, "NumVertices:", 12))
         {
         cptr = commandstring + 12;
         numvertices = atoi(cptr);
         continue;
         }
      if(!strncmp(commandstring, "VertexStartQuads:", 17))
         {
         cptr = commandstring + 17;
         vertexstartquads = atoi(cptr);
         continue;
         }
      if(!strncmp(commandstring,"NumVertexIndices: " , 17))
         {
         cptr = commandstring + 17;
         numvertexindices = atoi(cptr);
         continue;
         }
      if(!strncmp(commandstring,"NumFacetSrfn: " , 13))
         {
         cptr = commandstring + 13;
         numfacetsrfn = atoi(cptr);
         continue;
         }
      if(!strncmp(commandstring,"MeshType: " , 9))
         {
         cptr = commandstring + 9;
         meshtype = atoi(cptr);
         continue;
         }
      if(!strncmp(commandstring, "NStrips: ", 8))
         {
         cptr = commandstring + 8;
         nstrips = atoi(cptr);
         continue;
         }
      if (!strncmp(commandstring, "Preferquads: ", 12))
         {
         cptr = commandstring + 12;
         preferquads = atoi(cptr);
         continue;
         }
      
      }

   //you have loaded the header. Check that things are OK to here
   if(dims[0] <=0 || dims[1] <= 0 ||dims[2] <=0)
      {std::cerr<<"Invalid dimensions\n"; return 0;}
   if (numskelpoints <=0)
      {std::cerr<<"Invalid number of skeleton points"; return 0;}
   if (numfacets <= 0)
      {std::cerr<<"Invalid number of facets\n"; return 0;}
   if (numvertices <= 0)
      {std::cerr<<"Invalid number of vertices\n"; return 0;}
   if(numvertexindices <= 0)
      {std::cerr<<"Invalid number of vertexindices\n"; return 0;}
   if (numfacetsrfn <= 0)
      {std::cerr<<"Invalid number of facetsrfn\n"; return 0;}

      //do some minor calcs
   slicesize = dims[0] * dims[1];
   totalsize = slicesize * dims[2];

      //allocate memory before doing further loads
   vertices = new FVEC[numvertices];
   vertexindices = new int[numvertexindices];
   facetsrfn = new FVEC[numfacetsrfn];
   vertexsrfn = new FVEC[numvertices];

   if (!vertices || !vertexindices || !facetsrfn || ! vertexsrfn)
      {std::cerr<<"Out of mem; could not create mesh\n"; return 0;}


            //now load the data
   if (ifile) ifile->getline(commandstring, 499);
   int numfound = 0;      //you need to find four things correctly

   while (ifile)
      {
      if (!strcmp(commandstring, "END MESH_s")) break;

      if(!strcmp(commandstring, "VERTICES:"))
         {
         numfound += 1;
         for (i = 0; i < numvertices; i++)
            {
            ifile->getline(commandstring, 499);
            if (!ifile) return 0;

            tbox.Stringtonfloats(commandstring, 3, pt3);
            vertices[i].Set(pt3);
            }
         std::cerr<<"Loaded vertices...";
         }

      else if (!strcmp(commandstring, "VERTEXINDICES:"))
         {
         numfound += 1;
         for (i = 0; i < numvertexindices; i++)
            {
            ifile->getline(commandstring, 499);
            if (!ifile) return 0;

            vertexindices[i] = atoi(commandstring);
            }
         std::cerr<<"Loaded vertexindices....";
         }

      else if (!strcmp(commandstring, "FACETSRFN:"))
         {
         numfound +=1;
         for (i = 0; i < numfacetsrfn; i++)
            {
            ifile->getline(commandstring, 499);
            if (!ifile) return 0;
            tbox.Stringtonfloats(commandstring, 3, pt3);
            facetsrfn[i].Set(pt3);
            }
         std::cerr<<"Loaded facetsrfn...";
         }

      else if (!strcmp(commandstring, "VERTEXSRFN:"))
         {
         numfound += 1;
         for (i = 0; i < numvertices; i++)
            {
            ifile->getline(commandstring, 499);
            if (!ifile) return 0;
            tbox.Stringtonfloats(commandstring, 3, pt3);
            vertexsrfn[i].Set(pt3);
            }
         std::cerr<<"Loaded vertexsrfn...";
         }
      ifile->getline(commandstring, 499);
      }



    std::cerr<<std::endl;


   if (numfound == 4) return 1;      //you got everything seemingly OK
   return 0;
   }

//-----------------------------------------------
         //Get Min and Max vertex coords in world coords
int MESH_s::GetMinMax(float* mins, float* max)
   {
   if(!numvertices) return 0;

   FVEC minvec = vertices[0];
   FVEC maxvec = vertices[0];
   int i;

   for(i = 1; i < numvertices; i++)
      {
      minvec.Vmin(minvec, vertices[i]);
      maxvec.Vmax(maxvec, vertices[i]);
      }
      
  float* fptr = minvec.Get();
  float* ffptr = maxvec.Get();

  for (i = 0; i < 3; i++)
     {
     mins[i] = fptr[i];
     max[i] = ffptr[i];
     }
  return 1;
   }


//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&7
      //PROTECTED: OVOID MESH_s
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
int MESH_s::CreateOvoidQuadMesh(float* pts, float* scale)
   {
   int i, j, onvertexindex;
   int onvertex;
  
         //allocate memory
   numvertices = nstrips * (nstrips + 1);
   vertices = new FVEC[numvertices];
   numtrianglefacets = 0;
   numfacets = nstrips * (nstrips);
   vertexstartquads = 0;

   numvertexindices = 4 * numfacets;
   vertexindices = new int[numvertexindices];
   facetsrfn = new FVEC[numfacets];
   if(!vertices || !vertexindices || !facetsrfn)
      {std::cerr<<"Out of mem\n"; Releasemem(); return 0;}

            //the equation is from Programming in 3 Dimensions by Watkins
            //and Sharp
   int theta, dtheta;
   int phi, dphi;
   float sinphi, cosphi;
  
   dtheta = 360/nstrips;
   dphi = 180/nstrips;
   phi = 0;
   onvertex = 0; onvertexindex = 0;

   MATH m;

         //get the scale factors--this used to have a radius in here
         //too, but don't need it.
   float xs, ys, zs;
   xs = scale[0];
   ys = scale[1];
   zs = scale[2];

         //write the vertices first
   for (i = 0; i <= nstrips; i++)
      {
      theta = 0;
      for (j = 0; j < nstrips; j++)
         {
         sinphi = (float)m.SinD(phi);
         cosphi = (float)m.CosD(phi);
         vertices[onvertex].Set((float)(xs * sinphi * m.CosD(theta) + pts[0]),\
                                (float)(ys * sinphi * m.SinD(theta) + pts[1]),\
                                (float)(zs * cosphi + pts[2]));
         theta += dtheta;
         ++onvertex;
         }
      phi += dphi;
      }

               //Put in the indices and get the facetnormals at the same
               //time. 
   int iindex;
   onvertexindex = 0;
   onfacet = 0;
   GEOM geo;
   PNT pt1, pt2, pt3;
   VEC vec;
   double* dptr;

   for (i = 0; i < nstrips - 1; i++)
      {
      iindex = i * nstrips;
      for (j = 0; j < nstrips - 1; j++)
         {
         vertexindices[onvertexindex++] = iindex + j;
         vertexindices[onvertexindex++] = iindex + j + 1;
         vertexindices[onvertexindex++] = iindex + nstrips + j + 1;
         vertexindices[onvertexindex++] = iindex + nstrips + j;

                  //two of the points will be identical at the top
                  //and bottom, so be careful about points sent to
                  //determine srfn
         pt1.Set(vertices[iindex + j].Get());
         pt3.Set(vertices[iindex + nstrips + j + 1].Get());
         pt2.Set(vertices[iindex + nstrips + j].Get());
         geo.Getsrfn(pt1, pt2, pt3, &vec);
         dptr = vec.Get();
         facetsrfn[onfacet++].Set((float)dptr[0], (float)dptr[1], (float)dptr[2]);
         }

      vertexindices[onvertexindex++] = iindex + nstrips -1;
      vertexindices[onvertexindex++] = iindex;
      vertexindices[onvertexindex++] = iindex + nstrips;
      vertexindices[onvertexindex++] = iindex + nstrips + nstrips - 1;

      pt1.Set(vertices[iindex + nstrips - 1].Get());
      pt3.Set(vertices[iindex + nstrips].Get());
      pt2.Set(vertices[iindex + nstrips + nstrips - 1].Get());

      if(!geo.Getsrfn(pt1, pt2, pt3, &vec))      //will be normalized in geo
            std::cerr<<"No\n";
      dptr = vec.Get();
      facetsrfn[onfacet++].Set((float)dptr[0], (float)dptr[1], (float)dptr[2]);
      }

            //A different set of points collapses to 0 at the end,
            //so you need to process this separately.
     for (i = nstrips - 1; i < nstrips; i++)
      {
      iindex = i * nstrips;
      for (j = 0; j < nstrips - 1; j++)
         {
         vertexindices[onvertexindex++] = iindex + j;
         vertexindices[onvertexindex++] = iindex + j + 1;
         vertexindices[onvertexindex++] = iindex + nstrips + j + 1;
         vertexindices[onvertexindex++] = iindex + nstrips + j;

                  //two of the points will be identical at the top
                  //and bottom, so be careful about points sent to
                  //determine srfn
         pt1.Set(vertices[iindex + j].Get());
         pt3.Set(vertices[iindex + j + 1].Get());
         pt2.Set(vertices[iindex + nstrips + j + 1].Get());
         geo.Getsrfn(pt1, pt2, pt3, &vec);
         dptr = vec.Get();
         facetsrfn[onfacet++].Set((float)dptr[0], (float)dptr[1], (float)dptr[2]);
         }

      vertexindices[onvertexindex++] = iindex + nstrips -1;
      vertexindices[onvertexindex++] = iindex;
      vertexindices[onvertexindex++] = iindex + nstrips;
      vertexindices[onvertexindex++] = iindex + nstrips + nstrips - 1;

      pt1.Set(vertices[iindex + nstrips - 1].Get());
      pt3.Set(vertices[iindex].Get());
      pt2.Set(vertices[iindex + nstrips].Get());

      if(!geo.Getsrfn(pt1, pt2, pt3, &vec))      //will be normalized in geo
            std::cerr<<"No srfn in MESH_s\n";
      dptr = vec.Get();
      facetsrfn[onfacet++].Set((float)dptr[0], (float)dptr[1], (float)dptr[2]);
      }

   onfacet = 0;
   return 1;
   }
  //---------------------------------------------------
            //right now this is just for quads
int MESH_s::CreateOvoidVertexNormals()
   {

   vertexsrfn = new FVEC[numvertices];
   int* series = new int[nstrips + 1];

   if(!vertexsrfn || !series)
      {
      std::cerr<<"Out of mem in OvoidVertexNormals()\n";
      Releasemem();
      return 0;
      }

   int i, j;
   FVEC tempvec;

  
         //You need to handle the first and last vertex separately, as
         //each of these vertices actually represents nstrips vertices
         //collapsed into a point.
   for (i = 0; i < nstrips; i++)
      tempvec = tempvec + facetsrfn[i];
   tempvec.Vnormalize();

   for (i = 0; i < nstrips; i++)
      vertexsrfn[i] = tempvec;

   tempvec.Set(0,0,0);
   for (i = numfacets - nstrips; i < numfacets; i++)
     tempvec = tempvec + facetsrfn[i];
   tempvec.Vnormalize();

   for (i = numvertices - nstrips; i < numvertices; i++)
      vertexsrfn[i] = tempvec;

   
            //do the remaining vertices
   int iindex;

   for (i = 1; i <nstrips; i++)
      {
      iindex = i * nstrips;

      for (j = 0; j <nstrips; j++)
         {
         series[j + 1] = j + iindex;
         series[0] = series[nstrips];
         }

      for (j = 0; j < nstrips; j++)
         {
         vertexsrfn[iindex + j] = facetsrfn[series[j]] +\
                                  facetsrfn[series[j + 1]] +\
                                  facetsrfn[series[j] - nstrips] +\
                                  facetsrfn[series[j + 1] - nstrips];
         vertexsrfn[iindex + j].Vnormalize();
         }
      }  

   delete [] series;
   return 1;
   }



   
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
      //PROTECTED: CREATE VESSEL MESH_s
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

//------------------------------------------
int MESH_s::CreateVesselTriangleMesh(float* points)
   {
  int i, j, onvertexindex;
   float dirvec[3];
   OMAT mat;
   float vertvec[3] = {0,0,1};
   float len;
   int onvertex;
   int ll, lr, ul, ur;
   float* fptr;
   int pointdims = 4;      //sampled vessels only have 4 dims

         //allocate memory
   numvertices = nstrips * (numskelpoints) + 2;
   vertices = new FVEC[numvertices];
   numtrianglefacets = (numskelpoints - 1) * nstrips * 2;    //cylinder part
   numtrianglefacets += 2 * nstrips; //endcaps for each end
   numfacets = numtrianglefacets;

   numvertexindices = 3 * numfacets;
   vertexindices = new int[numvertexindices];
   if(!vertices || !vertexindices)
      {std::cerr<<"Out of mem\n"; Releasemem(); return 0;}


                      //create a circle with desired divisions. This is your
                  //model. It exists on the x,y,0 plane and has
                  //a radius of 1. It will subsequently be scaled, translated, and
                  //and tipped for each operation, and its defined points
                  //linked to create a jointed tubular structure.
   FVEC* modelcirclepoints = new FVEC[nstrips];
   FVEC* base = new FVEC[nstrips];
   FVEC* top = new FVEC[nstrips];
   int modelmemsize = nstrips * sizeof(FVEC);

   if (!modelcirclepoints || !base || !top)
      {std::cerr<<"Out of mem"; Releasemem(); return 0;}

   CreateCircleModel(modelcirclepoints);
       //define the first sequence of points at base, tilting according
         //to the direction vector given by the first two skeleton points
         //and scaling according to the radius and translating to 
         //actual center location.
   memcpy(top, modelcirclepoints, modelmemsize);
   ScaleModelCirclePoints(top, points[3]);
   dirvec[0] = points[pointdims] - points[0];
   dirvec[1] = points[pointdims + 1] - points[1];
   dirvec[2] = points[pointdims + 2] - points[2];

   mat.VectorToVectorRotation(vertvec, dirvec);
   for (j = 0; j< nstrips; j++)
      {
      fptr = top[j].Get();
      mat.TransformVec(3, fptr);     //tilt
      }


   TranslateModelCirclePoints(top, points);

         //The first set of unique vertices involve those for the endcap.
         //They will include the circle points you just did as well
         //as the center point given in points.
   vertices[0].Set(points[0], points[1], points[2]);
   for(i = 1; i <= nstrips; i++)
      vertices[i] = top[i - 1];

   onvertex = nstrips + 1;
   onvertexindex = 0;

   float* vesselpointer;
   float* lastvesselpointer = points;


         //Set the indices for the first endcap
   for (i = 0; i < nstrips - 1; i++)
      {
      vertexindices[onvertexindex++]= 0;    //the central point
      vertexindices[onvertexindex++] = i + 2;  //bottom right
      vertexindices[onvertexindex++] = i + 1;   //bottom left

      }
   vertexindices[onvertexindex++] = 0;
   vertexindices[onvertexindex++] = 1;
   vertexindices[onvertexindex++] = nstrips;




      //Set the vertex indices for the cylinders. You want to put all 
      //triangle strips first, so save nstrips places now to put the
      //top cap in later.
   onvertexindex = 6 * nstrips;

   for (i = 1; i < numskelpoints; i++)
      {
            //copy the present top circle to the bottom circle. The new
            //bottom circle will be scaled and translated and tilted
            //already. 
      memcpy(base, top, modelmemsize);

           //Get the next (x,y,z, rad) point from the inputdata. The pointdim
            //variable lets you skip to the the next set of (x,y,z,rad) if
            //the input data has extraneous dimensions. You are trying to
            //define the new top circle.
      vesselpointer =  points + (i * pointdims);

           //define the new top circle, multiplying by radius, tilting according
            //to the direction vector, and translating by the bottom skeleton
            //location
      memcpy(top, modelcirclepoints, modelmemsize);

      ScaleModelCirclePoints(top, vesselpointer[3]);

                       //change the z position according to distance
                  //between two points
      dirvec[0] = vesselpointer[0] - lastvesselpointer[0];
      dirvec[1] = vesselpointer[1] - lastvesselpointer[1];
      dirvec[2] = vesselpointer[2] - lastvesselpointer[2];
      len = Len(dirvec);
      for (j = 0; j < nstrips; j++)
           top[j].Axis(3, len);

               //try to make cylinder top at right angles to
               //direction of skeleton. This may help surface
               //normals later.      
      mat.VectorToVectorRotation(vertvec, dirvec);
     for (j = 0; j< nstrips; j++)
         mat.TransformVec(3, top[j].Get());     //tilt


                           //translate the top values and add
                           //to vertices
      for (j = 0; j < nstrips; j++)
         {
         top[j].Add(lastvesselpointer);
         vertices[onvertex + j] = top[j];
         }


                             //connect the vertices so as to make
                           //squares, and put the indices in vertexindices
                           //Addition should be counterclockwise to make
                           //surface normals come out right later. 
      for (j = 0; j < nstrips - 1; j++)
         {
         ll = (i-1) * nstrips + j + 1; //lower left point
         lr = (i- 1) * nstrips + j + 2; //lower right
         ur = i * nstrips + j + 2;      //upper right
         ul = i * nstrips + j + 1;          //upper left

         vertexindices[onvertexindex++] = ll;
         vertexindices[onvertexindex++] = lr;
         vertexindices[onvertexindex++] = ur;


         vertexindices[onvertexindex++] = ur;     
         vertexindices[onvertexindex++] = ul;
         vertexindices[onvertexindex++] = ll;
         }

      vertexindices[onvertexindex++] = lr;                                 //last lower right
      vertexindices[onvertexindex++] = (i - 1) * nstrips + 1;              //initial low left point
      vertexindices[onvertexindex++] = i * nstrips + 1;                  //initial low right

      vertexindices[onvertexindex++] = i * nstrips + 1;                 //initial upper left
      vertexindices[onvertexindex++] = ur;                              //last upper right
      vertexindices[onvertexindex++] = lr;                              //last lower right

      onvertex += nstrips;
      
      lastvesselpointer = vesselpointer;  //top vesselpointer is now at bottom
      vesselpointer += pointdims;   //go to next set of input points
      }
      
               //create the set of triangle caps for the top of the tube
               //you already have the circlepoints in top. You need to
               //add the central vertex and to connect the vertices
               //via vertexindices. You need to set onvertexindex
               //back to its appropriate place at the beginning so you
               //get all triangle strips first.
   onvertexindex = 3 * nstrips;

   vertices[onvertex].Set(lastvesselpointer[0], lastvesselpointer[1], lastvesselpointer[2]);
   int first = onvertex - nstrips;
   for (i = 0; i < nstrips - 1; i++)
      {
      vertexindices[onvertexindex++] = onvertex;
      vertexindices[onvertexindex++] = first + i;
      vertexindices[onvertexindex++] = first + i + 1;
      }
   vertexindices[onvertexindex] = onvertex;
   vertexindices[onvertexindex + 1] = vertexindices[onvertexindex - 1];
   vertexindices[onvertexindex + 2] = first;
 
   delete [] modelcirclepoints;
   delete [] base;
   delete [] top;

   vertexstartquads = 3 * numtrianglefacets;
   return 1;
   }

//----------------------------------------
int MESH_s::CreateVesselQuadMesh(float* points)
   {
  int i, j, onvertexindex;
   float dirvec[3];
   OMAT mat;
   float vertvec[3] = {0,0,1};
   float len;
   int onvertex;
   int ll, lr, ul, ur;
   float* fptr;

 
  int numquadfacets;
  int pointdims = 4;       //sampled vessels only have 4 dims

         //allocate memory
   numvertices = nstrips * (numskelpoints) + 2;
   vertices = new FVEC[numvertices];
   numquadfacets = (numskelpoints - 1) * nstrips;    //cylinder part
   numtrianglefacets = 2 * nstrips; //endcaps for each end
   numfacets = numquadfacets + numtrianglefacets;

   numvertexindices = 3 * numtrianglefacets + 4 * numquadfacets;
   vertexindices = new int[numvertexindices];
   if(!vertices || !vertexindices)
      {std::cerr<<"Out of mem\n"; Releasemem(); return 0;}


                      //create a circle with desired divisions. This is your
                  //model. It exists on the x,y,0 plane and has
                  //a radius of 1. It will subsequently be scaled, translated, and
                  //and tipped for each operation, and its defined points
                  //linked to create a jointed tubular structure.
                  
   FVEC* modelcirclepoints = new FVEC[nstrips];
   FVEC* base = new FVEC[nstrips];
   FVEC* top = new FVEC[nstrips];
   int modelmemsize = nstrips * sizeof(FVEC);

   if (!modelcirclepoints || !base || !top)
      {std::cerr<<"Out of mem"; Releasemem(); return 0;}

   CreateCircleModel(modelcirclepoints);
       //define the first sequence of points at base, tilting according
         //to the direction vector given by the first two skeleton points
         //and scaling according to the radius and translating to 
         //actual center location.
   memcpy(top, modelcirclepoints, modelmemsize);
   ScaleModelCirclePoints(top, points[3]);
   dirvec[0] = points[pointdims] - points[0];
   dirvec[1] = points[pointdims + 1] - points[1];
   dirvec[2] = points[pointdims + 2] - points[2];

   mat.VectorToVectorRotation(vertvec, dirvec);
   for (j = 0; j< nstrips; j++)
      {
      fptr = top[j].Get();
      mat.TransformVec(3, fptr);     //tilt
      }


   TranslateModelCirclePoints(top, points);

         //The first set of unique vertices involve those for the endcap.
         //They will include the circle points you just did as well
         //as the center point given in points.
   vertices[0].Set(points[0], points[1], points[2]);
   vertices[0] = vertices[0] - FVEC(dirvec[0] * points[3]/2, dirvec[1] * points[3]/2, dirvec[2] * points[3]/2);

   for(i = 1; i <= nstrips; i++)
      vertices[i] = top[i - 1];

   onvertex = nstrips + 1;
   onvertexindex = 0;

   float* vesselpointer;
   float* lastvesselpointer = points;


         //Set the indices for the first endcap
   for (i = 0; i < nstrips - 1; i++)
      {
      vertexindices[onvertexindex++]= 0;    //the central point
      vertexindices[onvertexindex++] = i + 2;  //bottom right
      vertexindices[onvertexindex++] = i + 1;   //bottom left

      }
   vertexindices[onvertexindex++] = 0;
   vertexindices[onvertexindex++] = 1;
   vertexindices[onvertexindex++] = nstrips;




      //Set the vertex indices for the cylinders. You want to put all 
      //triangle strips first, so save nstrips places now to put the
      //top cap in later.
   onvertexindex = 2 * nstrips * 3;

   for (i = 1; i < numskelpoints; i++)
      {
            //copy the present top circle to the bottom circle. The new
            //bottom circle will be scaled and translated and tilted
            //already. 
      memcpy(base, top, modelmemsize);

           //Get the next (x,y,z, rad) point from the inputdata. The pointdim
            //variable lets you skip to the the next set of (x,y,z,rad) if
            //the input data has extraneous dimensions. You are trying to
            //define the new top circle.
      vesselpointer =  points + (i * pointdims);

           //define the new top circle, multiplying by radius, tilting according
            //to the direction vector, and translating by the bottom skeleton
            //location
      memcpy(top, modelcirclepoints, modelmemsize);

      ScaleModelCirclePoints(top, vesselpointer[3]);

                       //change the z position according to distance
                  //between two points
      dirvec[0] = vesselpointer[0] - lastvesselpointer[0];
      dirvec[1] = vesselpointer[1] - lastvesselpointer[1];
      dirvec[2] = vesselpointer[2] - lastvesselpointer[2];
      len = Len(dirvec);
      for (j = 0; j < nstrips; j++)
           top[j].Axis(3, len);

               //try to make cylinder top at right angles to
               //direction of skeleton. This may help surface
               //normals later.      
      mat.VectorToVectorRotation(vertvec, dirvec);
     for (j = 0; j< nstrips; j++)
         mat.TransformVec(3, top[j].Get());     //tilt


                           //translate the top values and add
                           //to vertices
      for (j = 0; j < nstrips; j++)
         {
         top[j].Add(lastvesselpointer);
         vertices[onvertex + j] = top[j];
         }


                             //connect the vertices so as to make
                           //squares, and put the indices in vertexindices
                           //Addition should be counterclockwise to make
                           //surface normals come out right later. 
      for (j = 0; j < nstrips - 1; j++)
         {
         ll = (i-1) * nstrips + j + 1; //lower left point
         lr = (i- 1) * nstrips + j + 2; //lower right
         ur = i * nstrips + j + 2;      //upper right
         ul = i * nstrips + j + 1;          //upper left

         vertexindices[onvertexindex++] = ll;
         vertexindices[onvertexindex++] = lr;
         vertexindices[onvertexindex++] = ur;
         vertexindices[onvertexindex++] = ul;
         }

      vertexindices[onvertexindex++] = lr;                                 //last lower right
      vertexindices[onvertexindex++] = (i - 1) * nstrips + 1;              //initial low left point
      vertexindices[onvertexindex++] = i * nstrips + 1;                  //initial low right
      vertexindices[onvertexindex++] = ur;                              //last upper right

      onvertex += nstrips;
      
      lastvesselpointer = vesselpointer;  //top vesselpointer is now at bottom
      vesselpointer += pointdims;   //go to next set of input points
      }
      
               //create the set of triangle caps for the top of the tube
               //you already have the circlepoints in top. You need to
               //add the central vertex and to connect the vertices
               //via vertexindices. You need to set onvertexindex
               //back to its appropriate place at the beginning so you
               //get all triangle strips first.
   onvertexindex = nstrips * 3;

   vertices[onvertex].Set(lastvesselpointer[0], lastvesselpointer[1], lastvesselpointer[2]);
   vertices[onvertex] = vertices[onvertex] + FVEC(dirvec[0] * lastvesselpointer[3]/2, dirvec[1] * lastvesselpointer[3]/2, dirvec[2] * lastvesselpointer[3]/2);



   int first = onvertex - nstrips;
   for (i = 0; i < nstrips - 1; i++)
      {
      vertexindices[onvertexindex++] = onvertex;
      vertexindices[onvertexindex++] = first + i;
      vertexindices[onvertexindex++] = first + i + 1;
      }
   vertexindices[onvertexindex] = onvertex;
   vertexindices[onvertexindex + 1] = vertexindices[onvertexindex - 1];
   vertexindices[onvertexindex + 2] = first;
 
   delete [] modelcirclepoints;
   delete [] base;
   delete [] top;

   vertexstartquads = 3 * numtrianglefacets;

   return 1;
   }
//-------------------------------------------
int MESH_s::CreateVesselFacetSurfaceNormals()
   {
   float*fptr;
   double* dptr;
   GEOM geo;
   PNT pt1, pt2, pt3;
   VEC vec;
   FVEC junkvec;

   facetsrfn = new FVEC[numfacets];
   if (!facetsrfn)
      {Releasemem(); std::cerr<<"Out of mem in srfn\n"; return 0;}

   fptr = (float*)GetFirstTriangle(&junkvec);

   while (fptr)
      {
      pt1.Set(fptr[0], fptr[1], fptr[2]);
      pt2.Set(fptr[3], fptr[4], fptr[5]);
      pt3.Set(fptr[6], fptr[7], fptr[8]);

      geo.Getsrfn(pt1, pt2, pt3, &vec);      //will be normalized in geo

      dptr = vec.Get();

      facetsrfn[onfacet].Set( (float)dptr[0], (float)dptr[1], (float)dptr[2]);

      fptr = (float*) GetNextTriangle(&junkvec);
      }

  fptr = (float*)GetFirstQuad(&junkvec);

   while (fptr)
      {
      pt1.Set(fptr[0], fptr[1], fptr[2]);
      pt2.Set(fptr[3], fptr[4], fptr[5]);
      pt3.Set(fptr[6], fptr[7], fptr[8]);

      geo.Getsrfn(pt1, pt2, pt3, &vec);      //will be normalized in geo

      dptr = vec.Get();

      facetsrfn[onfacet].Set( (float)dptr[0], (float)dptr[1], (float)dptr[2]);
      fptr = (float*) GetNextQuad(&junkvec);
      }


   return 1;
   }
//-----------------------------------
int MESH_s::CreateVesselVertexNormals()
   {
   if (!facetsrfn) return 0;
   if (vertexsrfn) delete [] vertexsrfn;

   vertexsrfn = new FVEC[numvertices];
   if(!vertexsrfn)
      {std::cerr<<"Not enough mem for vertex normals\n"; Releasemem(); return 0;}
   memset(vertexsrfn, 0, numvertices * sizeof(FVEC));

            //this can be a little tricky because the facet
            //organization is 1) triangle mesh cap at base, 
            //2) triangle mesh cap at top, 3) sequence of triangle or
            //3)quad cylinders. However, you can get the first and last
            //vertexsrfn readily from any triangle facet at flat top or bottom
   vertexsrfn[0] = facetsrfn[0];
   vertexsrfn[numvertices - 1] = facetsrfn[nstrips];

   if (preferquads)
      VesselVertexNormalsQuads();
   else
      VesselVertexNormalsTriangles();

   return 1;
   }
//--------------------------------------------------
      //This is tricky because vertex 0 is on the endcap and vertices
      //1 - nstrips make up the first cylinder base. However, facets 0 to 
      //2 * nstrips are taken up by the twe endcaps.
void MESH_s::VesselVertexNormalsTriangles()
   {
   int i, j, k, nfaces, onvertexsrfn;
   nfaces = (nstrips + 1) * 2;
   int* facelist = new int[nfaces];               //list of facets in one row,
                                                  //with last two repeated in the front
   int twostrips = 2 * nstrips;                   //previous set of facets
   
        //you need to process the first and last circles of points differently
      //as they abut the flat top.Get the facets associated
      //with the top circle of points, each of which has only 5 associated
      //facets. This is a circular list, whose initial entry is a repeat
      //of the last facet pair.
   for (i = 2; i <nfaces; i++)
      facelist[i] = 2 * nstrips + i - 2;
   facelist[0] = facelist[nfaces - 2];
   facelist[1] = facelist[nfaces - 1];


               //you have read a circular list of the faces contributing
               //to a circle of the first cylinder into facelist. Get a list
               //of the faces used for each vertex around this circle.
               //The first two faces will be on the endcap and have the
               //the same surface normals.
   j = 0;
   onvertexsrfn = 1;

   for (i = 0; i <nstrips; i++)
      {
      vertexsrfn[onvertexsrfn] = facetsrfn[0] + facetsrfn[0] + facetsrfn[facelist[j]]\
         + facetsrfn[facelist[j + 2]] + facetsrfn[facelist[j + 3]];

      vertexsrfn[onvertexsrfn++].Vnormalize();

      //std::cerr<< facelist[j]<<"  "<<facelist[j + 2]<<"  "<<facelist[j + 3]<<std::endl;
      j = j + 2;
      }


               //process the intermediate vertices lying around
               //each cylinder ring. Do not do the last ring.
    for (i = 1; i <numskelpoints - 1; i++)      //skip last ring
       {

                        //get the next set of strip faces
       for (j = 0; j < nfaces; j++)
          facelist[j] += twostrips;
         
       j = 0;
       
       for (k = 0; k < nstrips; k++)      //for that circle of vertices
          {
          //std::cerr<< facelist[j]<<"  "<<facelist[j + 2]<<"  "<<facelist[j + 3]<<"  ";
          //std::cerr<<facelist[j] - twostrips<<"  "<<facelist[j+1]-twostrips<<"  "<<facelist[j+3]-twostrips;
          //std::cerr<<std::endl;

          vertexsrfn[onvertexsrfn] = facetsrfn[facelist[j]] +\
             facetsrfn[facelist[j+2]] + facetsrfn[facelist[j+3]] +\
             facetsrfn[facelist[j]-twostrips] + facetsrfn[facelist[j+1]-twostrips] +\
             facetsrfn[facelist[j+3]-twostrips];

          vertexsrfn[onvertexsrfn++].Vnormalize();

          j += 2;
          }
       }
       
               //Write the last ring of surfacenormals. Like the first ring,
               //two faces will be on a planar endcap and will have the same 
               //surfacenormals.
    j = 0;
    for (i = 0; i < nstrips; i++)
      {
      vertexsrfn[onvertexsrfn] = facetsrfn[numfacets - 1] +\
         facetsrfn[numfacets - 1] +\
         facetsrfn[facelist[j]] +\
         facetsrfn[facelist[j+1]] +\
         facetsrfn[facelist[j+3]];

      vertexsrfn[onvertexsrfn].Vnormalize();

      j+=2;
      ++onvertexsrfn;
      }

            //There is a last vertex, but you processed it at the beginning.
    //std::cerr<<std::endl;
    //for (i = 0; i < numvertices; i++)
       //vertexsrfn[i].Show();

   delete [] facelist;
   }
//--------------------------------------
void MESH_s::VesselVertexNormalsQuads()
   {
   int i, j, k, nfaces, onvertexsrfn;
   nfaces = (nstrips + 1);
   int* facelist = new int[nfaces];               //list of facets in one row,
                                                  //with last repeated in the front  

      //you need to process the first and last circles of points differently
      //as they abut the flat top. 
   for (i = 1; i <nfaces; i++)
      facelist[i] = 2 * nstrips + i - 1;
   facelist[0] = facelist[nfaces - 1];  //last is copied as the first


               //you have read a circular list of the faces contributing
               //to a circle of the first cylinder into facelist. Get a list
               //of the faces used for each vertex around this circle.
   j = 0;
   onvertexsrfn = 1;

   for (i = 0; i <nstrips; i++)
      {
      vertexsrfn[onvertexsrfn] = facetsrfn[0] + facetsrfn[0] + facetsrfn[facelist[j]]\
         + facetsrfn[facelist[j + 1]];

    vertexsrfn[onvertexsrfn++].Vnormalize();
      j++;
      }


               //process the intermediate vertices lying around
               //each cylinder ring. Do not do the last ring.
    for (i = 1; i <numskelpoints - 1; i++)      //skip last ring
       {

                        //get the next set of strip faces
       for (j = 0; j < nfaces; j++)
          facelist[j] += nstrips;
         
       j = 0;
       
       for (k = 0; k < nstrips; k++)      //for that circle of vertices
          {
           vertexsrfn[onvertexsrfn] = facetsrfn[facelist[j]] +\
             facetsrfn[facelist[j+1]] +\
             facetsrfn[facelist[j]- nstrips] + facetsrfn[facelist[j+1]-nstrips];

          vertexsrfn[onvertexsrfn++].Vnormalize();

          j++;
          }
       }
       
               //Write the last ring of surfacenormals. Like the first ring,
               //two faces will be on a planar endcap and will have the same 
               //surfacenormals.
    j = 0;
    for (i = 0; i < nstrips; i++)
      {
      vertexsrfn[onvertexsrfn] = facetsrfn[numfacets - 1] +\
         facetsrfn[numfacets - 1] +\
         facetsrfn[facelist[j]] +\
         facetsrfn[facelist[j+1]];
      vertexsrfn[onvertexsrfn].Vnormalize();

      j++;
      ++onvertexsrfn;
      }

            //There is a last vertex, but you processed it at the beginning.
 
   delete [] facelist;
   }

   
//--------------------------------

void MESH_s::CreateCircleModel(FVEC* v)
   {
    int i;
   MATH math;

   double anglestep = math.Radians(360.0/nstrips);
   double angle = 0;

   for(i = 0; i <nstrips; i++)
      {
      v[i].Set( (float)cos(angle), (float)sin(angle), 0);
      angle += anglestep;
      }
   }
//-----------------------------------
      //Scale the x,y points in the model circle according to radius.
void MESH_s::ScaleModelCirclePoints(FVEC* v, float rad)
   {
   float* fptr;

   for(int i = 0; i < nstrips; i++)
      {
      fptr = v[i].Get();
      fptr[0] *= rad;
      fptr[1] *= rad;
      }
   }
//-----------------------------------------
void MESH_s::TranslateModelCirclePoints(FVEC* v, float* pt)
   {
   float* fptr;

   for(int i = 0; i < nstrips; i++)
      {
      fptr = v[i].Get();
      fptr[0] += pt[0];
      fptr[1] += pt[1];
      fptr[2] += pt[2];
      }
   }
//-------------------------------
float MESH_s::Len(float* a)
   {
   return (float)(sqrt (a[0]*a[0] + a[1]*a[1] + a[2]*a[2]));
   }

//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&77
      //PROTECTED: SURFACE CREATION
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
int MESH_s::CreateSurfaceQuadMesh(SURFACE* surf, unsigned char* maskfile, int myid)
   {
   LIST <int>tempvertices;
   LIST<int> tempvertexlist;
   LIST<int> tempfsnlist;
   int* iptr;
   int val;
   int counter;
   int x,y,z;

   iptr = surf->GetPoints();
   int i;

   for (i = 0; i < numskelpoints; i++)
      {
      std::cerr<<i<<"-"<<numskelpoints<<"  ";
      val = iptr[i];
      DoRightFace(val, maskfile, &tempvertices, &tempvertexlist, &tempfsnlist, myid);
      DoLeftFace(val, maskfile, &tempvertices, &tempvertexlist, &tempfsnlist, myid);
      DoFrontFace(val, maskfile, &tempvertices, &tempvertexlist, &tempfsnlist, myid);
      DoBackFace(val, maskfile, &tempvertices, &tempvertexlist, &tempfsnlist, myid);
      DoTopFace(val, maskfile, &tempvertices, &tempvertexlist, &tempfsnlist, myid);
      DoBottomFace(val, maskfile, &tempvertices, &tempvertexlist, &tempfsnlist, myid);
      }


               //Now that you know the number, assign memory and
               //copy the vertices 
  numvertices = tempvertices.Count();
  if (!numvertices)
     {
     std::cerr<<"No vertices found in Maskmesh::Load()\n";
     return 0;
     }
 
  vertices = new FVEC[numvertices];
  if(!vertices)
     {
     std::cerr<<"Out of mem in Maskmesh::Load()\n";
     Releasemem();
     return 0;
     }
  
  tempvertices.Reset();
  counter = 0;
  while (tempvertices.Get(&val))
     {
     CompositeToVox(val, &x, &y, &z);
     vertices[counter++].Set(x * steps[0], y * steps[1], z * steps[2]);
     }
  tempvertices.Releasemem();


            //Do the same for vertexindices
  numvertexindices = tempvertexlist.Count();
  vertexindices = new int[numvertexindices];
  if (!vertexindices)
     {
     std::cerr<<"Out of mem in Maskmesh::Load()\n";
     Releasemem();
     return 0;
     }

  tempvertexlist.Reset();
  counter = 0;
  while (tempvertexlist.Get(&val))
     vertexindices[counter++] = val;

  tempvertexlist.Releasemem();

               //Do the same for facetsrfnindices
         //there are only 6 surface normals, but the
         //way you have things set up for other objects you need
         //to store a facetnormal for each face.
   FVEC front(0,0,1);        //front face   (0)
   FVEC back(0,0, -1);    //back face    (1)
   FVEC top(0, 1, 0);    //top face     (2)
   FVEC bottom(0, -1, 0);   //bottom face  (3)
   FVEC right(1, 0,0);     //right face   (4)
   FVEC left(-1, 0, 0);   //left face    (5)
  numfacets = numvertexindices/4;
  facetsrfn = new FVEC[numfacets];
  counter = 0;
  tempfsnlist.Reset();
  while (tempfsnlist.Get(&val))
     {
     std::cerr<<counter<<"  ";
     if (val == 0) facetsrfn[counter] = front;
     else if (val == 1) facetsrfn[counter] = back;
     else if (val == 2) facetsrfn[counter] = top;
     else if (val == 3) facetsrfn[counter] = bottom;
     else if (val == 4) facetsrfn[counter] = right;
     else if (val == 5) facetsrfn[counter] = left;
     ++counter;
     }
  tempfsnlist.Releasemem();

    numtrianglefacets = 0;
  vertexstartquads = 0;
  numfacetsrfn = numfacets;

   return 1;    
   }
//-----------------------------------------------
int MESH_s::CompositeToVox(int val, int* x, int* y, int* z)
   {
   if (val >= totalsize || val < 0) return 0;

   *z = val/slicesize;
   *y = ( val - (*z * slicesize))/dims[0];
   *x = val - (*z * slicesize) - (*y * dims[0]);
   return 1;
   }
//-------------------------------------------------
int MESH_s::DoLeftFace(int vox, unsigned char* tempbuff,\
LIST<int>* tempvertices, LIST<int>* tempvertexlist, LIST<int>* tempfsnlist, int myid)
   {
   int x, y, z, count;
   int ul, ll, lr, ur;
   int indexul, indexll, indexlr, indexur;

         //If there is a surface voxel to the left, don't draw the
         //right face
   if(!CompositeToVox(vox, &x, &y, &z))
      return 0;

   if (x - 1 >= 0 && tempbuff[vox - 1] == myid)
      return 0;

 
          //you need to draw the left face. There is no boundary voxel to left
         //Get the 4 vertices.
   ll = vox;                     //0,0,0
   lr = ll + slicesize;          //0,0,1
   ul = ll + dims[0];            //0,1,0
   ur = lr + dims[0];          //0,1,1

            //You need to add the vertices to the vertexlist if they aren't
            //there already, and figure out what the index to them is.
   count = tempvertices->Count();
   if (!tempvertices->IsInList(ll, &indexll))
      {
      tempvertices->AddToTail(ll);
      indexll = count;
      }

   count = tempvertices->Count();
   if(!tempvertices->IsInList(lr, &indexlr))
      {
      tempvertices->AddToTail(lr);
      indexlr = count;
      }

   count = tempvertices->Count();
   if (!tempvertices->IsInList(ur, &indexur))
      {
      tempvertices->AddToTail(ur);
      indexur = count;
      }

   count = tempvertices->Count();
   if (!tempvertices->IsInList(ul, &indexul))
      {
      tempvertices->AddToTail(ul);
      indexul = count;
      }

            //Add this quad, going counterclockwise as you would see
            //this face from outside, to tempvertexlist
   tempvertexlist->AddToTail(indexll);
   tempvertexlist->AddToTail(indexlr);
   tempvertexlist->AddToTail(indexur);
   tempvertexlist->AddToTail(indexul);

            //Add surface normals to faces. 5 is negative x vector.
   tempfsnlist->AddToTail(5);

   return 1;
   }

//------------------------------------------------
int MESH_s::DoRightFace(int vox, unsigned char* tempbuff,\
LIST<int>* tempvertices, LIST<int>* tempvertexlist, LIST<int>* tempfsnlist, int myid)
   {
   int x, y, z, count;
   int ul, ll, lr, ur;
   int indexul, indexll, indexlr, indexur;

         //If there is a surface voxel to the right, don't draw the
         //right face
   if(!CompositeToVox(vox, &x, &y, &z))
      return 0;

   if (x + 1 < dims[0] && tempbuff[vox + 1] == myid)
      return 0;

         //you need to draw the right face. There is no boundary voxel to right
         //Get the 4 vertices.
   lr = vox + 1;                 //1,0,0
   ll = lr + slicesize;          //1,0,1
   ur = lr + dims[0];            //1,1,0
   ul = ur + slicesize;          //1,1,1
  

            //You need to add the vertices to the vertexlist if they aren't
            //there already, and figure out what the index to them is.
   count = tempvertices->Count();
   if (!tempvertices->IsInList(ll, &indexll))
      {
      tempvertices->AddToTail(ll);
      indexll = count;
      }

   count = tempvertices->Count();
   if(!tempvertices->IsInList(lr, &indexlr))
      {
      tempvertices->AddToTail(lr);
      indexlr = count;
      }

   count = tempvertices->Count();
   if (!tempvertices->IsInList(ur, &indexur))
      {
      tempvertices->AddToTail(ur);
      indexur = count;
      }

   count = tempvertices->Count();
   if (!tempvertices->IsInList(ul, &indexul))
      {
      tempvertices->AddToTail(ul);
      indexul = count;
      }

            //Add this quad, going counterclockwise as you would see
            //this face from outside, to tempvertexlist
   tempvertexlist->AddToTail(indexll);
   tempvertexlist->AddToTail(indexlr);
   tempvertexlist->AddToTail(indexur);
   tempvertexlist->AddToTail(indexul);

            //Add surface normals to faces. 4 is positive x vector.
   tempfsnlist->AddToTail(4);

   return 1;
   }

//------------------------------------------------
int MESH_s::DoFrontFace(int vox, unsigned char* tempbuff,\
LIST<int>* tempvertices, LIST<int>* tempvertexlist, LIST<int>* tempfsnlist, int myid)
   {
   int x, y, z, count;
   int ul, ll, lr, ur;
   int indexul, indexll, indexlr, indexur;

         //If there is a surface voxel to the front, don't draw the
         //front face
   if(!CompositeToVox(vox, &x, &y, &z))
      return 0;

   if (z + 1 < dims[2] && tempbuff[vox + slicesize] == myid)
      return 0;

       //you need to draw the front face. There is no boundary voxel in front
         //Get the 4 vertices.
   ll = vox + slicesize;                     //0,0,1
   lr = ll + 1;                              //1,0,1
   ur = lr + dims[0];                        //1,1,1
   ul = ll + dims[0];                        //0,1,1

             //You need to add the vertices to the vertexlist if they aren't
            //there already, and figure out what the index to them is.
   count = tempvertices->Count();
   if (!tempvertices->IsInList(ll, &indexll))
      {
      tempvertices->AddToTail(ll);
      indexll = count;
      }

   count = tempvertices->Count();
   if(!tempvertices->IsInList(lr, &indexlr))
      {
      tempvertices->AddToTail(lr);
      indexlr = count;
      }

   count = tempvertices->Count();
   if (!tempvertices->IsInList(ur, &indexur))
      {
      tempvertices->AddToTail(ur);
      indexur = count;
      }

   count = tempvertices->Count();
   if (!tempvertices->IsInList(ul, &indexul))
      {
      tempvertices->AddToTail(ul);
      indexul = count;
      }

            //Add this quad, going counterclockwise as you would see
            //this face from outside, to tempvertexlist
   tempvertexlist->AddToTail(indexll);
   tempvertexlist->AddToTail(indexlr);
   tempvertexlist->AddToTail(indexur);
   tempvertexlist->AddToTail(indexul);

            //Add surface normals to face
   tempfsnlist->AddToTail(0);             //front face
   return 1;
   }
//-------------------------------------------------------
//------------------------------------------------
int MESH_s::DoBackFace(int vox, unsigned char* tempbuff,\
LIST<int>* tempvertices, LIST<int>* tempvertexlist, LIST<int>* tempfsnlist, int myid)
   {
   int x, y, z, count;
   int ul, ll, lr, ur;
   int indexul, indexll, indexlr, indexur;

         //If there is a surface voxel to the front, don't draw the
         //front face
   if(!CompositeToVox(vox, &x, &y, &z))
      return 0;

  if (z - 1 >= 0 && tempbuff[vox - slicesize] == myid)
      return 0;

  
         //you need to draw the back face. 
         //Get the 4 vertices.
   lr = vox;                                 //0,0,0
   ll = lr + 1;                              //1,0,0
   ur = lr + dims[0];                        //1,1,0
   ul = ll + dims[0];                        //0,1,0

            //You need to add the vertices to the vertexlist if they aren't
            //there already, and figure out what the index to them is.
   count = tempvertices->Count();
   if (!tempvertices->IsInList(ll, &indexll))
      {
      tempvertices->AddToTail(ll);
      indexll = count;
      }

   count = tempvertices->Count();
   if(!tempvertices->IsInList(lr, &indexlr))
      {
      tempvertices->AddToTail(lr);
      indexlr = count;
      }

   count = tempvertices->Count();
   if (!tempvertices->IsInList(ur, &indexur))
      {
      tempvertices->AddToTail(ur);
      indexur = count;
      }

   count = tempvertices->Count();
   if (!tempvertices->IsInList(ul, &indexul))
      {
      tempvertices->AddToTail(ul);
      indexul = count;
      }

            //Add this quad, going counterclockwise as you would see
            //this face from outside, to tempvertexlist
   tempvertexlist->AddToTail(indexll);
   tempvertexlist->AddToTail(indexlr);
   tempvertexlist->AddToTail(indexur);
   tempvertexlist->AddToTail(indexul);

           //Add surface normals to face 
   tempfsnlist->AddToTail(1);

   return 1;
   }
//-------------------------------------------
//------------------------------------------------
int MESH_s::DoTopFace(int vox, unsigned char* tempbuff,\
LIST<int>* tempvertices, LIST<int>* tempvertexlist, LIST<int>* tempfsnlist, int myid)
   {
   int x, y, z, count;
   int ul, ll, lr, ur;
   int indexul, indexll, indexlr, indexur;

         //If there is a surface voxel to the front, don't draw the
         //front face
   if(!CompositeToVox(vox, &x, &y, &z))
      return 0;

   if (y + 1 < dims[1] && tempbuff[vox + dims[0]] == myid)
      return 0;
           //you need to draw the top face. 
         //Get the 4 vertices.
   ul = vox + dims[0];           //0,1,0
   ll = ul + slicesize;          //0,1,1
   lr = ll + 1;                  //1,1,1
   ur = ul + 1;                  //1,1,0

    
            //You need to add the vertices to the vertexlist if they aren't
            //there already, and figure out what the index to them is.
   count = tempvertices->Count();
   if (!tempvertices->IsInList(ll, &indexll))
      {
      tempvertices->AddToTail(ll);
      indexll = count;
      }

   count = tempvertices->Count();
   if(!tempvertices->IsInList(lr, &indexlr))
      {
      tempvertices->AddToTail(lr);
      indexlr = count;
      }

   count = tempvertices->Count();
   if (!tempvertices->IsInList(ur, &indexur))
      {
      tempvertices->AddToTail(ur);
      indexur = count;
      }

   count = tempvertices->Count();
   if (!tempvertices->IsInList(ul, &indexul))
      {
      tempvertices->AddToTail(ul);
      indexul = count;
      }

            //Add this quad, going counterclockwise as you would see
            //this face from outside, to tempvertexlist
   tempvertexlist->AddToTail(indexll);
   tempvertexlist->AddToTail(indexlr);
   tempvertexlist->AddToTail(indexur);
   tempvertexlist->AddToTail(indexul);

            //Add surface normals to face 
   tempfsnlist->AddToTail(2);
   return 1;
   }
//------------------------------------------------
int MESH_s::DoBottomFace(int vox, unsigned char* tempbuff,\
LIST<int>* tempvertices, LIST<int>* tempvertexlist, LIST<int>* tempfsnlist, int myid)
   {
   int x, y, z, count;
   int ul, ll, lr, ur;
   int indexul, indexll, indexlr, indexur;

         //If there is a surface voxel to the front, don't draw the
         //front face
   if(!CompositeToVox(vox, &x, &y, &z))
      return 0;

     if (y - 1 >= 0 && tempbuff[vox - dims[0]] == myid)
      return 0;

            //you need to draw the bottom face. 
         //Get the 4 vertices.
   ll = vox;                     //0,0,0
   lr = vox + 1;                 //1,0,0
   ul = ll + slicesize;          //0,0,1
   ur = lr + slicesize;          //1,0,1

   
            //You need to add the vertices to the vertexlist if they aren't
            //there already, and figure out what the index to them is.
   count = tempvertices->Count();
   if (!tempvertices->IsInList(ll, &indexll))
      {
      tempvertices->AddToTail(ll);
      indexll = count;
      }

   count = tempvertices->Count();
   if(!tempvertices->IsInList(lr, &indexlr))
      {
      tempvertices->AddToTail(lr);
      indexlr = count;
      }

   count = tempvertices->Count();
   if (!tempvertices->IsInList(ur, &indexur))
      {
      tempvertices->AddToTail(ur);
      indexur = count;
      }

   count = tempvertices->Count();
   if (!tempvertices->IsInList(ul, &indexul))
      {
      tempvertices->AddToTail(ul);
      indexul = count;
      }

            //Add this quad, going counterclockwise as you would see
            //this face from outside, to tempvertexlist
   tempvertexlist->AddToTail(indexll);
   tempvertexlist->AddToTail(indexlr);
   tempvertexlist->AddToTail(indexur);
   tempvertexlist->AddToTail(indexul);

            //Add surface normals to face 
   tempfsnlist->AddToTail(3);
   return 1;
   }
//------------------------------------------
void MESH_s::CreateSurfaceVertexNormals()
   {
   FVEC svec;
   FVEC* fptr;
   int i, index, val;

   memset(vertexsrfn, 0, numvertices * sizeof(FVEC));
   

    fptr = GetFirstQuad(&svec);
    while (fptr)
      {
      index = 4 * (onfacet);
      val = vertexindices[index];
      vertexsrfn[val] = vertexsrfn[val] + svec;
      val = vertexindices[index + 1];
      vertexsrfn[val] = vertexsrfn[val] + svec;
      val = vertexindices[index + 2];
      vertexsrfn[val] = vertexsrfn[val] + svec;
      fptr = GetNextQuad(&svec);
       }

         

    for (i = 0; i < numvertices; i++)
      vertexsrfn[i].Vnormalize();
      
    }

}//end of namespace