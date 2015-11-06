//Surface mesh for surface rendering


#ifndef _ebMESH_s_h_
#define _ebMESH_s_h_

      //eblib
#include "ebVESS_s.h"
#include "ebSURFACE.h"
#include "ebOMAT.h"
#include "ebFVEC.h"
//#include "MemoryLeak.h"
namespace eb
{

class MESH_s
   {
   public:
      MESH_s();
      MESH_s(const MESH_s& a);
      MESH_s& operator = (const MESH_s&a);
      ~MESH_s();

      void Releasemem();
      void Initmem();


            //Color and opacity handling
      void SetColor(float a, float b, float c)
         {col[0] = a; col[1] = b; col[2] = c;}
      void SetColor(float* c)    
         {col[0] = c[0]; col[1] = c[1]; col[2] = c[2];}
      float* GetColor()                            {return col;}
      void SetOpacity(float a)                     {opacity = a;}
      float GetOpacity()                           {return opacity;}
      void Show()                                  {opacity = 1;}
      void Hide()                                  {opacity = 0;}

               //some surfaces can only be defined by triangles,
               //but (when possible) preferquads will do quads instead.
               //Will only apply on next load. 
      void SetPreferquads(int a)                   {preferquads = a;}

               //Set the number of circle subdivision
      void SetNStrips(int a)        {if (a < 3) a = 3; nstrips = a;}

               //Create an object from some kind of input data:
               //1) sphere turned to ovoid by scaling
               //2) vessel
               //3) Generic object of arbitrary configuration
      //Each mesh can consist of only one kind of object. A MeshCollection
      //can contain meshes of various types
      int CreateOvoid(float* skel, float* scale);
      int CreateVessel(VESS_s* vess, int* d, float* s);
      int CreateVessel(float* pts, int num, int* d, float* s);
      int CreateSurface(SURFACE* surf, unsigned char* maskfile, float* s, int myid);              
      
    
               //Required for Gouraud shading. Call will take effect only
               //after a load. Depending on timing, you may want to make
               //this protected and automatically performed on load.
      int CalculateVertexNormals();
  
               //Some objects may have a mix of quads and triangles
               //and, right now, am giving choice of flat or Gouraud.
               //If GetFirst returns 0, you should not pursue.
               //The single parameter gives facet normals for flat shading.
               //The multiple parameter gives vertex normals for Gouraud. 
               //Triangles should be called first.
      FVEC* GetFirstTriangle(FVEC* facetnormal);
      FVEC* GetFirstTriangle(FVEC* facetnormal, FVEC* sn1, FVEC* sn2, FVEC* sn3);
      FVEC* GetNextTriangle(FVEC* facetnormal);
      FVEC* GetNextTriangle(FVEC* facetnormal, FVEC* sn1, FVEC* sn2, FVEC* sn3);
      FVEC* GetFirstQuad(FVEC* facetnormal);
      FVEC* GetFirstQuad(FVEC* facetnormal, FVEC* sn1, FVEC* sn2, FVEC* sn3, FVEC* sn4);
      FVEC* GetNextQuad(FVEC* facetnormal);
      FVEC* GetNextQuad(FVEC* facetnormal, FVEC* sn1, FVEC* sn2, FVEC* sn3, FVEC* sn4);



            //Scale, translate, rotate, or otherwise change all points
            //and associated surface normals (the vectors will not translate)
      void Transform(const OMAT* mat);

            //Get a skeleton point associated with a facet number.
            //This only really affects tubes. Spheres and "other" will
            //just return the facet number back. If you have sampled
            //the tube before creation, it's up to the caller to sort it
            //out.
      int GetSkeletonPointFromFacetNumber(int id);


            //Load and Save
      int Save(std::ofstream* ofile);
      int Load(std::ifstream* ifile);

            //Get the min and max vertex coords in world coords
      int GetMinMax(float* mins, float* max);

   protected:
      int dims[3];
      float steps[3];
      int slicesize, totalsize;
      float col[3];
      float opacity;
      int preferquads;                 //alternative is to do all triangles
      int numskelpoints;

      FVEC currentfacet[4];
      int onfacet;
      int numtrianglefacets;
      int numfacets;            

      int numvertices;
      int vertexstartquads;
      FVEC* vertices;

      int* vertexindices;
      int numvertexindices;

      FVEC* facetsrfn;     
      int numfacetsrfn;       //same as numfacets in vessels and spheres
                              //6 for general things
       FVEC* vertexsrfn;     //there will be numvertices number of them

            //specific to vessel creation (capped end) or sphere creation
            //(number of subdivisions is same along both axes).
      int nstrips;

            //defines sphere, vessel (series of linked cylinders with
            //cap at final ends), and generic anything. Used to help retrieve the
            //skeleton point if something is picked 0=ovoid, 1=tubes, 2=anything
      int meshtype;

         

         //OVOID CREATION
      int CreateOvoidQuadMesh(float* pts, float* scale);
      int CreateOvoidVertexNormals();

               
         //VESSEL CREATION
      int CreateVesselQuadMesh(float* pts);
      int CreateVesselTriangleMesh(float* pts);
      void CreateVesselEndcapMesh(float* pts);
      void CreateCircleModel(FVEC* modelcirclepoints);
      void ScaleModelCirclePoints(FVEC* v, float rad);
      void TranslateModelCirclePoints(FVEC* v, float* pt);
      float Len(float* a);

      int CreateVesselFacetSurfaceNormals();
      int CreateVesselVertexNormals();
      void VesselVertexNormalsQuads();
      void VesselVertexNormalsTriangles();

            //SURFACE CREATION
      int CreateSurfaceQuadMesh(SURFACE* surf, unsigned char* maskfile, int myid);
      int CompositeToVox(int val, int* x, int* y, int* z);

      int DoRightFace
         (int val, unsigned char* maskfile, LIST<int>* tempvertices, \
         LIST<int>* tempvertexlist, LIST<int>* tempfsnlist, int myid);
      int DoLeftFace
         (int val, unsigned char* maskfile, LIST<int>* tempvertices, \
         LIST<int>* tempvertexlist, LIST<int>* tempfsnlist, int myid); 
     int DoFrontFace
         (int val, unsigned char* maskfile, LIST<int>* tempvertices, \
         LIST<int>* tempvertexlist, LIST<int>* tempfsnlist, int myid);
     int DoBackFace
         (int val, unsigned char* maskfile, LIST<int>* tempvertices, \
         LIST<int>* tempvertexlist, LIST<int>* tempfsnlist, int myid);
     int DoTopFace
         (int val, unsigned char* maskfile, LIST<int>* tempvertices, \
         LIST<int>* tempvertexlist, LIST<int>* tempfsnlist, int myid);
     int DoBottomFace
         (int val, unsigned char* maskfile, LIST<int>* tempvertices, \
         LIST<int>* tempvertexlist, LIST<int>* tempfsnlist, int myid);
     void CreateSurfaceVertexNormals();
   };

}//end of namespace

#endif

