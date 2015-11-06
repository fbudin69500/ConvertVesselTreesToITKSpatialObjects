#ifndef _ebOBJECT_IO_s_h
#define _ebOBJECT_IO_s_h

      //base class for segmented image objects. There is an object header, which must
      //contain all mandatory fields and which may contain any arbitrary number of optional
      //fields. The header ends with NPoints:, which is then followed by pointnum lines of 
      //pointdims float points.
/*
Modified 6/02 to go into std namespace

  Elizabeth Bullitt
*/

#include "ebINCLUDES.h"      //define std namespace or not
#include "ebTBOX_s.h"        //string handling tools; module from eblib
//#include "MemoryLeak.h"

namespace eb
{

class OBJECTIO_s
   {
   public:
      OBJECTIO_s();
      OBJECTIO_s(const OBJECTIO_s&);
      virtual ~OBJECTIO_s();
      virtual OBJECTIO_s& operator = (const OBJECTIO_s& a);
      OBJECTIO_s& Copy(const OBJECTIO_s& a);
      virtual void Releasemem();


      void SetLimitPointdims(int a)          {limitpointdims = a;}
      int GetLimitPointdims()                {return limitpointdims;}

               //get/set optional fields

            //Color handling. The fourth value in color sets opacity
      void SetColor(float* a) {col[0] = a[0]; col[1] = a[1]; col[2] = a[2];}
      void SetColor(float a, float b, float c)
         {col[0] = a; col[1] = b; col[2] = c;}
      float* GetColor()       {return col;}
      void SetOpacity(float a) {col[3] = a;}
      float GetOpacity()      {return col[3];}


    void Show()             {col[3] = 1;}
    void Hide()             {col[3] = 0;}
    float IsVisible()       {return col[3];}

    int NPoints()           {return pointnum;}
    int GetPointnum()       {return pointnum;}
    int PointDim()          {return pointdim;}

              //Get/set connection of this vessel to its parent
    void VParent(int a)            {vparent = a;}
   int VParent(int* a)      {if (vparent == -1) return 0; *a = vparent; return 1;}
   int VParent()         {if (vparent == -1) return 0; return 1;}

   void AttachPt(int a)         {attachpt = a;}
   int AttachPt(int* a)      {if (attachpt == -1) return 0; *a = attachpt; return 1;}

               //Get/set IDs. On save, all numbers are recalculated if some vessels
               //are omitted from save
    void ID (int a)                  {id = a;}
   int ID(int* a)                  {if (id == -1) return 0; *a = id; return 1;}




                  //check case independent treetype (root, child, orphan)
                  //and case independent anat (artery, vein)
   int IsRoot();
   int IsOrphan();
   int IsArtery();
   int IsVein();
   void Anat(std::string a);
    std::string Anat()                        {return anat;}
    void TreeType(std::string a);
   std::string TreeType()               {return treetype;}
   void Type(std::string a);                 
    std::string Type()                        {return type;}
    void Name(std::string a);
    std::string Name()                        {return name;}                     
    std::string DimDesc(int a);
    int DimDesc(int a, std::string s);


                        //Get or set points
    int Realloc(int pn, int pd);
   int Point(int a, float* p);            //set one multidimensional pt after realloc
   int Points(int pn, int pd, float* p);   //realloc and replace points
   float* Point(int a);               //get a multidimensional point. If you send 0, get all

    
               //converts grid fp voxels to world coords in cm and back. Passed
                  //params are fp values that convert from vox to world. Rad is also
                  //affected; stephen's output files put rad in terms of xstep.
   int VoxToWorld(double xs, double ys, double zs);
   int WorldToVox(double xs, double ys, double zs);
   int VoxToWorld(double* x)               {return VoxToWorld(x[0], x[1], x[2]);}
   int WorldToVox(double* x)               {return WorldToVox(x[0], x[1], x[2]);}


   protected:
               //optional fields
      int id;
      //char* type;                //Tube, intcurve2D, contour, surfacepoints, 
      std::string type;                //Tube, intcurve2D, contour, surfacepoints, 
      //char* anat;                //artery, vein
      std::string anat;                //artery, vein
      //char* treetype;            //root, orphan, child;
      std::string treetype;            //root, orphan, child;
      int vparent;               //parent vessel or other object
      int attachpt;              //info about connection
      //char* name; 
      std::string name; 
      float col[4];              //RGB + opacity

               //mandatory items
      int pointnum;
      int pointdim;
      float* pts;
      //char** dimdesc;
      std::vector< std::string > dimdesc;

               //can be set or called by caller
      int limitpointdims;              //limit loads or saves to some dimension
      

    
            //The load and save functions separate the headers and points so that
            //a derived class can do various things.
            //of parts of a set of objects.
      virtual int LoadHeader(std::istream* ifile);
      virtual int LoadPoints(std::istream* ifile);
      virtual int SaveHeader(std::ofstream* ofile);
      virtual int SavePoints(std::ofstream* ofile);  
      void DeleteDimDesc();
    };

}//end of namespace

#endif

