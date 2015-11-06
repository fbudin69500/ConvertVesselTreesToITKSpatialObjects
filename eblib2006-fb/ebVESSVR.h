#ifndef _ebVESSVR_h
#define _ebVESSVR_h

 /*      //This class separates out the process of volume rendering vessels
         //from everything else. The position matrix, image data, vessel data, various zbuffers, 
         //and final output image buffer are all sent as passed pointers from outside
         //because the same data structures may be used elsewhere to write other 
         //things to the final output buffer. 
         //    Only limited error checking is performed. Before the first draw, caller must:

               1. Set the window size (wx, wy), which must be the same size as
                  any passed zbuffer and the same size as the final output buffer.
               2. Must set currentpos matrix or nothing will show. 
                  Currentpos is sent as a pointer, so you can change contents in another program
                  without having to call SetCurrentpos() again.
               3. SetSliceBuffer(). This provides the image intensity data
                  used during raytracing. 


  Revised 6/03. The class assumed that the image data was sent as a windowed volume of uchar,
  with the memory held elsewhere..
  This works fine, but can be onerous for the caller. Provide an option for the class to receive
  a ushort buffer with memory held elsewhere, and with the class responsible for creating the
  uchar buffer, which it subsequently must delete.
               
     
Elizabeth Bullitt
*/


#include "ebINCLUDES.h"
#include "ebZBUFF.h"
#include "ebOMAT.h"
#include "ebVESSCOL.h"

#include <math.h>
//#include "MemoryLeak.h"
namespace eb
{

class VESSVR
   {
   public:
      VESSVR();
      VESSVR(int wx, int wy, double a, OMAT* cp);
      ~VESSVR();

      void Activate()      {isactive = 1;}
      void Inactivate()    {isactive = 0;}
      int IsActive()       {return isactive;}
      

      double GetAngle()       {return angle;}
      double GetWinDistance() {return dist_to_win;}
      const unsigned char* GetSlices()    {return slicebuff;}
      const int* GetDims()          {return slicedims;}

            //Change intrinsics or extrinsics
      void SetCameraIntrinsics(int wx, int wy, double a);
      void SetCurrentPos(OMAT* cp)      {currentpos = cp;}      //uses a pointer, so updates automatically
      OMAT* GetInvertCurrentPos()       {return &invertcurrentpos;}

            //Change ray step size or stepsize
      void SetRaystepSize(float a) {raystepsize = a;}
      void SetSteps(float* a)      {slicesteps = a;     worldtovox.Scale(1/slicesteps[0], 1/slicesteps[1], 1/slicesteps[2]);
}

            //Set 3D slice (MRA) info as a uchar dataset. Program will refuse to run if slicebuff is not set.
            //Mem is held elsewhere and sent as passed pointer. 
      void SetSliceBuffer(unsigned char* buff, const int* d, const float* s);
      unsigned char* GetSliceBuffer()      {return slicebuff;}


            //Set 3D slice data from a ushort buff held elsewhere, but make the program 
            //responsible for memory allocation/deletion for the uchar buff
      void SetSliceBufferAllocMem(const unsigned short* buff, const int* d, const float* s, int imin, int imax);
            

      void ZeroSlices()      {if (mymem) delete []slicebuff; mymem = 0; slicebuff = 0;}

            //DRAW FUNCTIONS. 
            //The first is faster and best used when rendering with radius close to vessels'
            //actual radii. The second is slower, but uses a back buffer and so will render
            //"through" the object--use with radius dilation. Passsed params are pointers
            //to mem held elsewhere.
      int Render(VESSCOL* vc, ZBUFF* pointzbuff, ZBUFF* circlezbuff, unsigned char* outputimage);
      int FrontBackRender(VESSCOL* vc, ZBUFF* pointzbuff, ZBUFF* circlezbuff, unsigned char* outputimage);

 
            //These two functions are similar to the above, but assume writing greyscale info into
            //a colorbuffer rather than writing into a unsigned char greyscale buffer.
      int RenderColor(VESSCOL* vc, ZBUFF* pointzbuff, ZBUFF* circlezbuff, unsigned char* outputimage);
      int FrontBackRenderColor(VESSCOL* vc, ZBUFF* pointzbuff, ZBUFF* circlezbuff, unsigned char* outputimage);


   protected:
               //make renderer active or make it do nothing
      int isactive;

               //memory deletion is or is not required by this class
      int mymem;

               //camera intrinsics
      double angle;              //field of view
      double dist_to_win;        //distance src to viewplane
      int winx, winy;            //display window size in pixels

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

            //3D slice greyscale info (MRA). Mem held elsewhere
      const int* slicedims;               //number of voxels in x,y,z
      const float* slicesteps;            //spacing (cm) in x,y,z
      unsigned char* slicebuff;     //greyscale data
      int slicesize;                       //number of voxels in one z slice

            //step size for raytracing
      float raystepsize;

                    //&&&&&&&&&&&&&&&&&&&&&

            //Step 1: 
      int PrepareMatrices();

            //Step 2: 
            //Write skeleton info into a passed zbuffer.
      void WriteSkeletonInfo(int numpts, int pointdims, int myid, const float* pts, ZBUFF* zbuff);

            //Step 3:
            //Take info from skeletonbuff and write circles into circlebuff
      void WriteCircleInfoFromSkeletonInfo(ZBUFF* skeletonbuff, ZBUFF* circlebuff);

            //Step 4: Raycast in vessel-specific manner over designated volumes 
      void RayCastVessel(ZBUFF* circlebuff, unsigned char* outputbuff);

            //Alternative Step4. Write into a color buffer directly
      void RayCastVesselColor(ZBUFF* circlebuff, unsigned char* outputbuff);

      //------------------------------------------------------------
            //Alternative draw method that is slower, but looks better if you are using radmult.
            //It changes Steps 3 and 4.RayCastPointtoPointColor does the same thing, but
            //writes greyscale info into a color buffer.
      void WriteFrontBackCircleInfoFromSkeletonInfo(ZBUFF* skeletonbuff, ZBUFF* frontbuff, ZBUFF* backbuff);
      void RayCastPointToPoint(ZBUFF* circlezbuff, ZBUFF* backcirclezbuff, unsigned char* outputimage);
      void RayCastPointToPointColor(ZBUFF* circlezbuff, ZBUFF* backcirclezbuff, unsigned char* outputimage);




                     //Vessel-specific volume rendering functions
      void PerspectiveCircleVessel(ZBUFF* skelbuff, ZBUFF* circlebuff, int zbuffindex);
      void PerspectiveFrontBackCircleVessel(ZBUFF* skelbuff, ZBUFF* frontbuff, ZBUFF* backbuff, int zbuffindex);
      void SolidcpointsVessel(int x, int y, int xc, int yc, int zbuffindex, int projrad, ZBUFF* centerlinezbuff, ZBUFF* circlebuff);
      void SolidFrontBackcpointsVessel(int x, int y, int xc, int yc, int zbuffindex, int projrad, ZBUFF* centerlinezbuff, ZBUFF* frontbuff, ZBUFF* backbuff);

      void DrawLinePerspectiveCircleVessel
         (int minx, int maxx, int y, int zbuffindex, int len, int projrad, ZBUFF* centerlinezbuff, ZBUFF* circlebuff);
     void DrawFrontBackLinePerspectiveCircleVessel
(int minx, int maxx, int y, int zbuffindex, int len, int projrad, ZBUFF* centerlinezbuff, ZBUFF* frontbuff, ZBUFF* backbuff);

               //general volume rendering functions
      void EyeToPix(float pixx, float pixy, float* vec);
      unsigned char GetIntensityFromFPTransformedCoords(float x, float y, float z);
   };

}//end of namespace

#endif
