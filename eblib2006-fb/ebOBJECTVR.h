/*
Volume render segmented objects very quickly and flexibly. Paper is in press in IEEE-TMI as of 5/02.

  Elizabeth Bullitt
  */

#ifndef _ebSURFACE_VR_h
#define _ebSURFACE_VR_h


#include "ebINCLUDES.h"
#include "ebSURFACECOL.h"
#include "ebZBUFF.h"

#include "ebOMAT.h"
#include < math.h>
//#include "MemoryLeak.h"
namespace eb
{

class OBJECTVR
   {
   public:
      OBJECTVR();
      ~OBJECTVR();
         
      void Activate()            {isactive = 1;}
      void Inactivate()          {isactive = 0;}
      int IsActive()             {return isactive;}

               //Sets and Gets
      void SetSurfRadAddXY(float a)                 {surfradaddxy = a;}
      void SetSurfRadAddZ(float a)                  {surfradaddz = a;}
      void SetRayStepSize(float a)                  {raystepsize = a;}
      void SetRaystepSize(float a)                  {raystepsize = a;}
      int GetSliceSize()                            {return slicesize;}
      int Valid()                                   {if (slicebuff) return 1; return 0;}
      void SetSteps(float* s)                       
         {slicesteps = s; radorig3D = (float)(0.5 * sqrt(s[0] * s[0] + s[1] * s[1] + s[2] * s[2]));
      worldtovox.Scale(1/slicesteps[0], 1/slicesteps[1], 1/slicesteps[2]);}


            //Change camera intrinsics or extrinsics
      void SetCameraIntrinsics(int wx, int wy, double a);
      void SetCurrentPos(OMAT* cp)      {currentpos = cp;}

            //Set the input MRA or set to NULL
      void SetSliceBuffer(unsigned char* buff, const int* d, const float* s);
      void ZeroSlices()      {if (mymem) delete []slicebuff; mymem = 0; slicebuff = 0;}
      unsigned char* GetSliceBuffer()      {return slicebuff;}

               //Set 3D slice data from a ushort buff held elsewhere, but make the program 
            //responsible for memory allocation/deletion for the uchar buff
      void SetSliceBufferAllocMem(const unsigned short* buff, const int* d, const float* s, int imin, int imax);

  
               //Very anisotropic voxels can be tough on display of a slice. Try to make the slice
               //thinner
      void AdjustStepsForThickSlice();


                     //DISPLAY FUNCTIONS
      int PointCloud
(ZBUFF* pointzbuff, ZBUFF* circlezbuff, unsigned char* outputimage, SURFACECOL* scol, int minid);

      int PointCloudColorBuffer
(ZBUFF* pointzbuff, ZBUFF* circlezbuff, unsigned char* outputimage, SURFACECOL* scol, int minid);

      int SurfaceVolumeRender
(ZBUFF* pointzbuff, ZBUFF* circlezbuff, unsigned char* outputimage, SURFACECOL* scol, int minid);

            int SurfaceVolumeRenderColorBuffer
(ZBUFF* pointzbuff, ZBUFF* circlezbuff, unsigned char* outputimage, SURFACECOL* scol, int minid);

      int DeepVolumeRender
(ZBUFF* pointzbuff, ZBUFF* circlezbuff, unsigned char* outputimage, SURFACECOL* scol, int minid);

      int DeepVolumeRenderColorBuffer
(ZBUFF* pointzbuff, ZBUFF* circlezbuff, unsigned char* outputimage, SURFACECOL* scol, int minid);


   protected:
      float surfradaddxy;     //increase circle size on viewplane
      float surfradaddz;      //increase depth of ray cast
      float radorig3D;        //half diagonal of voxel
      float raystepsize;

               //camera intrinsics
      double angle;              //field of view
      double dist_to_win;        //distance src to viewplane
      int winx, winy, winsize;   //display window size in pixels

               //matrices
      OMAT* currentpos;        //position matrix sent as pointer from another class
                                 //this matrix is the exact equivalent of opengl's modelview 
                                       //matrix

                                
      OMAT invertcurrentpos;   //inverted currentposition matrix that also converts
                                 //from world coords in cm to voxel coords

      OMAT frontproj;                   //front project to unit cube and put to screen
                                          //this is close to the combination of opengl's/glut's
                                          //setting viewport to (winx, winy) and calling 
                                          //gluPerspective(angle, 1.0, 0.0, dist_to_win);
                                          //Total perspective front projection matrix, if
                                          //you did not need to retrieve z values in the middle,
                                          //would be frontproj * currentpos.

                                          //
      OMAT eyetopix;                    //calc ray from src (0,0,0) to viewplane pixel 
                                          //at (x,y, -dist_to_win)

      OMAT worldtovox;                  //convert from world coords (cm) to fp voxel coords

      OMAT vctoworldtransformed;        //find voxel center, put into world coords,
                                          //and transform by currentposition

            //3D slice greyscale info (MRA). Mem held elsewhere!!! Be careful...
      const int* slicedims;               //number of voxels in x,y,z
      const float* slicesteps;            //spacing (cm) in x,y,z
      unsigned char* slicebuff;     //greyscale data
      int slicesize;                       //number of voxels in one z slice

      int isactive;                       //a safer returnval than the buffer.
      
      int mymem;


               //General ray casting functions
      int PrepareMatrices();
      void CompositePointToFloats(int a, float* fpts);
      unsigned char GetIntensityFromFPTransformedCoords(float x, float y, float z);
      void EyeToPix(float x, float y, float* dir);

               //Specific ray casting functions for narrow rendering of front surface. The
               //color functions will write partial color
      void WriteSkeletonInfo(int numpts, int myid, int* iptr, ZBUFF* pointzbuff);
      void WriteSkeletonInfoColorBuffer(int numpts, int myid, int* iptr, ZBUFF* pointzbuff, float* col);

      void WriteFrontCircleInfoFromSkeletonInfo(ZBUFF* pointzbuff, ZBUFF* circlezbuff, int minid);
      void WriteFrontCircleInfoFromSkeletonInfoColorBuffer(ZBUFF* pointzbuff, ZBUFF* circlezbuff, int minid);

      
      void SolidcpointsFrontObject
 (int x, int y, int xcenter, int ycenter, int index, ZBUFF* pointzbuff, ZBUFF* circlezbuff);
      void SolidcpointsFrontObjectColorBuffer
 (int x, int y, int xcenter, int ycenter, int index, ZBUFF* pointzbuff, ZBUFF* circlezbuff);
      
      void DrawPerspectiveLineCircleFrontObject
 (int minx, int maxx, int y, int index, ZBUFF* pointzbuff, ZBUFF* circlezbuff);
            void DrawPerspectiveLineCircleFrontObjectColorBuffer
 (int minx, int maxx, int y, int index, ZBUFF* pointzbuff, ZBUFF* circlezbuff);

      void RaycastObjectSurface(ZBUFF* circlezbuff, int minid, unsigned char* outputimage);
      void RaycastObjectSurfaceColorBuffer(ZBUFF* circlezbuff, int minid, unsigned char* outputimage);

      void RaycastObjectPoint(int x, int y, float zpos, float rayrad,unsigned char* outputimage);
      void RaycastObjectPointColorBuffer
(int x, int y, float zpos, float rayrad,unsigned char* outputimage, float* objectcol);



            //Additional/replacement ray casting functions for depth rendering. The color buffer
            //functions here do not write color, but do write into a triplet buffer
   void WriteBackCircleInfoFromSkeletonInfo(ZBUFF* pointzbuff, ZBUFF* circlezbuff, int minid, SURFACECOL* scol);
   void PerspectiveObjectBackCircleToFloatbuff(ZBUFF* pointzbuff, int j, float rad, float* tempfarbuff);
      void SolidcpointsBackObject
         (int x, int y, int xcenter, int ycenter, int zbuffindex, int projrad, float* tempfarbuff);
      void DrawPerspectiveCircleBackObject(int minx, int maxx, int y, int zbuffindex, int projrad, float* tempfarbuff);
      void  RaycastDeepPoint
(int x, int y, float nearz, float farz, float addrad, unsigned char* outputimage);
     void  RaycastDeepPointColorBuffer
(int x, int y, float nearz, float farz, float addrad, unsigned char* outputimage);


   };


}//end of namespace

#endif