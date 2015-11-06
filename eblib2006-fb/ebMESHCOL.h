#ifndef _ebMESHCOL_h
#define _ebMESHCOL_h

#include "ebMESH.h"

      //eblib
#include "ebVESSCOL.h"
#include "ebSURFACECOL.h"
//#include "MemoryLeak.h"
namespace eb
{

class MESHCOL
   {
   public:
      MESHCOL();
      ~MESHCOL();
      MESHCOL(const MESHCOL& a);
      MESHCOL& operator = (const MESHCOL& a);

      void Releasemem();
      void Initmem();
      int AllocMemForNMeshes(int n);

      int GetMeshnum()           {return nmesh;}
      int GetVessnum()           {return nmesh;}
      MESH* GetMesh(int aa)    {return &mesh[aa];}

     
            //Color and opacity handling
      void SetColor(int aa, float a, float b, float c) {mesh[aa].SetColor(a,b,c);}
      void SetColor(int aa, float* c) {mesh[aa].SetColor(c);}
      float* GetColor(int aa)         {return mesh[aa].GetColor();}
      void SetOpacity(int aa, float a){mesh[aa].SetOpacity(a);}
      float GetOpacity(int aa)        {return mesh[aa].GetOpacity();}
      void Show()                     {for (int i = 0; i <nmesh; i++) mesh[i].Show();}
      void Hide()                     {for (int i = 0; i <nmesh; i++) mesh[i].Hide();}                

               //some surfaces can only be defined by triangles,
               //but (when possible) preferquads will do quads instead.
               //Will only apply on next load. 
      void SetPreferquads(int a)      {preferquads = a;}

               //Set the number of circle subdivision
      void SetNStrips(int a)          {nstrips = a;}

 
               //Create an object from some kind of input data:
               //1) sphere turned to ovoid by scaling
               //2) vessel
               //3) Generic object of arbitrary configuration
      //Each mesh describes one object. A MeshCollection
      //can contain meshes of various types. The following functions
      //will create a surface mesh in slot aa, which must have been created
      //via AllocMem() earlier. 
      int CreateOvoid(int aa, float* skel, float* scale)
         {if (aa >= nmesh) return 0; return mesh[aa].CreateOvoid(skel, scale);}
      int CreateVessel(int aa, VESS* vess, int* d, float* s) 
         {if (aa >= nmesh) return 0; return mesh[aa].CreateVessel(vess, d, s);}
      int CreateSurface(int aa, SURFACE* surf, unsigned char* mf, float* s, int myid) 
         {if (aa >= nmesh) return 0; return mesh[aa].CreateSurface(surf, mf, s, myid);}
      
                  //These functions will zero memory and then create a
                  //meshcollection from the set of objects sent. The mesh
                  //will then be only vessels, for example, or only surfaces etc.
                  //This sometimes makes it easier to manipulate groups.
                  
      int CreateVessels(VESSCOL* vc, int extraslots = 0);
      int CreateVessel(VESSCOL* vc, int a);      //put something into an index in existence

      int CreateSurfaces(SURFACECOL* mc, unsigned char* maskfile, float* s);
      
    
               //Required for Gouraud shading. Call will take effect only
               //after a load. Depending on timing, you may want to make
               //this protected and automatically performed on load.
      int CalculateVertexNormals(int aa)  {mesh[aa].CalculateVertexNormals();}
  
              //Some objects may have a mix of quads and triangles
               //and, right now, am giving choice of flat or Gouraud.
               //If GetFirst returns 0, you should not pursue.
               //The single parameter gives facet normals for flat shading.
               //The multiple parameter gives vertex normals for Gouraud. 
               //Triangles should be called first.
      FVEC* GetFirstTriangle(int aa, FVEC* facetnormal)
         {return mesh[aa].GetFirstTriangle(facetnormal);}
      FVEC* GetFirstTriangle(int aa, FVEC* facetnormal, FVEC* sn1, FVEC* sn2, FVEC* sn3)
         {return mesh[aa].GetFirstTriangle(facetnormal, sn1, sn2, sn3);}
      FVEC* GetNextTriangle(int aa, FVEC* facetnormal)
         {return mesh[aa].GetNextTriangle(facetnormal);}
      FVEC* GetNextTriangle(int aa, FVEC* facetnormal, FVEC* sn1, FVEC* sn2, FVEC* sn3)
         {return mesh[aa].GetNextTriangle(facetnormal, sn1, sn2, sn3);}
      FVEC* GetFirstQuad(int aa, FVEC* facetnormal)
         {return mesh[aa].GetFirstQuad(facetnormal);}
      FVEC* GetFirstQuad(int aa, FVEC* facetnormal, FVEC* sn1, FVEC* sn2, FVEC* sn3, FVEC* sn4)
         {return mesh[aa].GetFirstQuad(facetnormal, sn1, sn2, sn3, sn4);}
      FVEC* GetNextQuad(int aa, FVEC* facetnormal)
         {return mesh[aa].GetNextQuad(facetnormal);}
      FVEC* GetNextQuad(int aa, FVEC* facetnormal, FVEC* sn1, FVEC* sn2, FVEC* sn3, FVEC* sn4)
         {return mesh[aa].GetNextQuad(facetnormal, sn1, sn2, sn3, sn4);}



            //Scale, translate, rotate, or otherwise change all points
            //and associated surface normals (the vectors will not translate)
      void Transform(int aa, const OMAT* mat)  {mesh[aa].Transform(mat);}
      void Transform(const OMAT* mat)
         {for (int i = 0; i < nmesh; i++) mesh[i].Transform(mat);}

            //This is only meaninful for vessels, but get skeleton pt from
            //facet number
      int GetSkeletonPointFromFacetNumber(int aa, int id)
         {return mesh[aa].GetSkeletonPointFromFacetNumber(id);}

            //Get the min and max vertex coords in world coords
      int GetMinMax(float* mins, float* max);



            //It takes a large amount of time to generate complex objects. Better have a save/load.
      int Load(char* fname);
      int Save(char* fname);

   protected:
      int nmesh;
      MESH* mesh;

      int nstrips;
      int preferquads;

         };

}//end of namespace

#endif

