#include "ebVESSVR.h"

#include "ebRESCALET.h"

namespace eb
{

VESSVR::VESSVR()
   {
   slicebuff = 0;
   winx = winy = 0;
   raystepsize = (float)0.11;
   currentpos = 0;
   isactive = 1;
   mymem = 0;
   }
//----------------------------------------------------------
VESSVR::VESSVR(int wx, int wy, double a, OMAT* cp)
   {
   slicebuff = 0;
   raystepsize = (float)0.11;      //default val is a little over a mm; our usual MR spacing
                                 //after correction for MR measurement errors.
   winx = wx;
   winy = wy;

   isactive = 1;
   mymem = 0;

   SetCameraIntrinsics(wx, wy, a);
   SetCurrentPos(cp);
   }
//-------------------------------------------------------------
VESSVR::~VESSVR()
   {
   if (mymem && slicebuff)
      delete [] slicebuff;
   }

//------------------------------------------------------------
      //Change the camera intrinsic settings. This will result in recalc of 
      //a couple of matrices
void VESSVR::SetCameraIntrinsics(int wx, int wy, double a)
   {
   winx = wx;
   winy = wy;
   angle = a;

   dist_to_win =  wy/(2.0 * tan(1.74532925199433E-002 * angle/2.0) );

          //Set up the front projection matrix 
     OMAT ovmap, otoscreen;
    ovmap.Viewmode(wx, wy, angle);
    otoscreen.Toscreen(wx, wy);
     frontproj = otoscreen * ovmap;

            //Set up a matrix that gives vec from src (0,0,0) to a pixel (x,y, - dist_to_win). 
            //This skips several steps as the eye is already at orgin and the coordinate
            //system is set as a right handed one with y up etc, and
            //the principle point is assumed to be at screen center. Z val
            //is negative as you are looking from origin down -z axis.
            //Also, the outputscreen uses the lower left corner as 0.  ANGIOV
            //in eblib gives many more choices, but don't need it here. 
   eyetopix.Translate(-wx/2.0, -wy/2.0, -dist_to_win);
   }
//-----------------------------------------------------------------------
      //take in a uchar buffer with memory held elsewhere
void VESSVR::SetSliceBuffer(unsigned char* buff, const int* d, const float* s)
   {

         //if you have already allocated mem for a buffer, delete it. This buffer is held
         //elsewhere
   if (mymem && slicebuff)
      delete [] slicebuff;

   mymem = 0;

            //take in the new data
   slicedims = d;
   slicesteps = s;
   slicebuff = buff;
   slicesize = d[0] * d[1];

   if(buff && s[0] && s[1] && s[2])
      {
     worldtovox.Scale(1/slicesteps[0], 1/slicesteps[1], 1/slicesteps[2]);
      }
   }
//-----------------------------------------------------
      //take in a ushort buffer.with mem assigned elsewhere.  You must assign mem for a scaled
      //uchar buffer
void VESSVR::SetSliceBufferAllocMem(const unsigned short* buff, const int* d, const float* s, int imin, int imax)
   {
         //delete a previous buffer
   if (mymem && slicebuff)
      delete [] slicebuff;

   slicebuff = 0;
   mymem = 1;

   slicedims = d;
   slicesteps = s;
   slicesize = d[0] * d[1];

   if(buff && s[0] && s[1] && s[2])
      worldtovox.Scale(1/slicesteps[0], 1/slicesteps[1], 1/slicesteps[2]);

         //create memory for the new buffer and rescale the unsigned short values into a uchar
         //buffer
   slicebuff = new unsigned char[slicesize * d[2]];

   if (!slicebuff)
      {
      std::cerr<<"Out of mem in vessel renderer\n";
      mymem = 0;
      return;
      }

   RESCALET<unsigned short> rscale;
   rscale.Convert(buff, slicesize * d[2], (float)imin, (float)imax, slicebuff, 0, 255);
   }

//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
      //PUBLIC DRAW FUNCTIONS
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
int VESSVR::Render(VESSCOL* vc, ZBUFF* pointzbuff, ZBUFF* circlezbuff, unsigned char* outputimage)
   {

            //prepare matrices for projection. Will change as point of view changes.
            //Do not proceed if no vessels or no mra data.
   if (!isactive)    return 0;
   if (!PrepareMatrices()) return 0;
   if(!vc->GetVessnum()) return 0;

   int i, numpts;
   float* fptr;


                 //write skeleton info
    for (i = 0; i < vc->GetVessnum(); i++)
            {
            if(!vc->IsVisible(i)) continue;
            fptr = vc->GetSample(i, &numpts);    //sample always gives 4D points
            WriteSkeletonInfo(numpts, 4, i, fptr, pointzbuff);
            }

            //take the skeleton info and draw circles on another zbuff. If you extract the
            //data and assign any marked pixel some color, you get a set of monochrome
            //vessels after this step. 
    WriteCircleInfoFromSkeletonInfo(pointzbuff, circlezbuff);
    RayCastVessel(circlezbuff, outputimage);

    return 1;
   }
//------------------------------------------------------------------
      //Same as above but writes into a final color buffer
int VESSVR::RenderColor(VESSCOL* vc, ZBUFF* pointzbuff, ZBUFF* circlezbuff, unsigned char* outputimage)
   {

            //prepare matrices for projection. Will change as point of view changes.
            //Do not proceed if no vessels or no mra data.
   if (!isactive)    return 0;
   if (!PrepareMatrices()) return 0;
   if(!vc->GetVessnum()) return 0;

   int i, numpts;
   float* fptr;

                 //write skeleton info
    for (i = 0; i < vc->GetVessnum(); i++)
            {
            if(!vc->IsVisible(i)) continue;
            fptr = vc->GetSample(i, &numpts);    //sample always gives 4D points
            WriteSkeletonInfo(numpts, 4, i, fptr, pointzbuff);
            }

            //take the skeleton info and draw circles on another zbuff. If you extract the
            //data and assign any marked pixel some color, you get a set of monochrome
            //vessels after this step. 

    WriteCircleInfoFromSkeletonInfo(pointzbuff, circlezbuff);

    RayCastVesselColor(circlezbuff, outputimage);

    return 1;
   }


//----------------------------------------------------
      //writes into an unsigned char greyscale buffer
int VESSVR::FrontBackRender(VESSCOL* vc, ZBUFF* pointzbuff, ZBUFF* circlezbuff, unsigned char* outputimage)
   {
            //prepare matrices for projection. Will change as point of view changes.
            //Do not proceed if no vessels or no mra data.
   if (!isactive) return 0;
   if (!PrepareMatrices()) return 0;
   if(!vc->GetVessnum()) return 0;

   ZBUFF* backcirclezbuff = new ZBUFF[winx * winy];
   if (!backcirclezbuff){std::cerr<<"No mem--can't draw\n"; return 0;}
   int i, numpts;
   float* fptr;

               //pass each ray from front of closest object to back of farthest
   for (i = 0; i < vc->GetVessnum(); i++)
            {
            if(!vc->IsVisible(i)) continue;
            fptr = vc->GetSample(i, &numpts);    //sample always gives 4D points
            WriteSkeletonInfo(numpts, 4, i, fptr, pointzbuff);
            }


   WriteFrontBackCircleInfoFromSkeletonInfo(pointzbuff, circlezbuff, backcirclezbuff);
   RayCastPointToPoint(circlezbuff, backcirclezbuff, outputimage);

   delete [] backcirclezbuff;
   return 1;
   }
//----------------------------------------------------
      //writes greyscale info into a color buffer
int VESSVR::FrontBackRenderColor(VESSCOL* vc, ZBUFF* pointzbuff, ZBUFF* circlezbuff, unsigned char* outputimage)
   {
            //prepare matrices for projection. Will change as point of view changes.
            //Do not proceed if no vessels or no mra data.
   if (!isactive) return 0;
   if (!PrepareMatrices()) return 0;
   if(!vc->GetVessnum()) return 0;

   ZBUFF* backcirclezbuff = new ZBUFF[winx * winy];
   if (!backcirclezbuff){std::cerr<<"No mem--can't draw\n"; return 0;}
   int i, numpts;
   float* fptr;

               //pass each ray from front of closest object to back of farthest
   for (i = 0; i < vc->GetVessnum(); i++)
            {
            if(!vc->IsVisible(i)) continue;
            fptr = vc->GetSample(i, &numpts);    //sample always gives 4D points
            WriteSkeletonInfo(numpts, 4, i, fptr, pointzbuff);
            }


   WriteFrontBackCircleInfoFromSkeletonInfo(pointzbuff, circlezbuff, backcirclezbuff);
   RayCastPointToPointColor(circlezbuff, backcirclezbuff, outputimage);

   delete [] backcirclezbuff;
   return 1;
   }

//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
      //PROTECTED GENERAL SEQUENCE OF FUNCTIONS
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
      //INDIVIDUAL FUNCTIONS
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
   //It should generally not be necessary to use these functions. However, they are left
   //public so that a caller can interrupt the process, mix and match, etc.


      //Call this first before any of the draw functions. Make a matrix that inverts
      //the currentposition (restores voxel box to its default position) and additionally
      //moves from world coords to voxel floating point coordinates. It will return 0
      //if the slicebuffer is not set yet.
int VESSVR::PrepareMatrices()
   {
   if (!slicebuff) return 0;

   double det;

   invertcurrentpos = *currentpos;
   invertcurrentpos.Invert(&det);
   invertcurrentpos = worldtovox * invertcurrentpos;

   return 1;
   }

//----------------------------------------------------------

         //Project skeleton points onto buffer. If you decide to draw now, you get a line
         //for each vessel.
void VESSVR::WriteSkeletonInfo(int numpts, int pointdims, int myid, const float* pts, ZBUFF* zbuff)
   {
    const float* fptr;
    float fpts[3];
    int i, index;
    float transformed_z;
    int zbuffintx, zbuffinty, zbuffindex;

    fptr = pts;

    for (i = 0; i < numpts; i++)
       {
       index = i * pointdims;
            //get current point from vessel list. These are in world coords but position
            //has not been moved to current position
       fpts[0] = fptr[index];
       fpts[1] = fptr[index + 1];
       fpts[2] = fptr[index + 2];

    

                  //move points to currentposition
       currentpos->TransformPt(3, fpts);

               //keep track of z position in transformed state
       transformed_z = fpts[2];

  
               //if you are so close to eye point that transformed_z is >= 0, do not continue:
               //you will end up in trouble later. 
       if (transformed_z >= 0) continue;
   
               //project transformed point onto viewplane
       frontproj.TransformPt(3, fpts);


                  //Get the integer pixel associated with the projection point
       zbuffintx = (int)(fpts[0] + 0.5);  
       if (zbuffintx < 0 || zbuffintx >= winx) continue;

       zbuffinty = (int)(fpts[1] + 0.5);  
       if(zbuffinty < 0 || zbuffinty >= winy) continue;

       zbuffindex = zbuffinty * winx + zbuffintx;
       
                  //if the z position of this point is in front of
                  //anything written on mask at that spot, record information
                  //about this projected point. Info is ID #, point#, z position, 3D radius.
       if(zbuff[zbuffindex].GetZ() && zbuff[zbuffindex].GetZ() >= transformed_z) 
            continue;
        zbuff[zbuffindex].Set(myid, i, transformed_z, fptr[index + 3]);
       }
    }

//------------------------------------------------------------------
      //Take the info from a zbuffer that has skeleton info and write a set of circles
      //to a second zbuffer.
void VESSVR::WriteCircleInfoFromSkeletonInfo(ZBUFF* skeletonbuff, ZBUFF* circlebuff)
   {
   for (int i = 0; i < winx * winy; i++)
      {
      if (skeletonbuff[i].GetZ())
            PerspectiveCircleVessel(skeletonbuff, circlebuff, i);
      }
   }
//--------------------------------------------------------------
      //This alternate writes front and back circles
void VESSVR::WriteFrontBackCircleInfoFromSkeletonInfo(ZBUFF* skeletonbuff, ZBUFF* frontbuff, ZBUFF* backbuff)
   {
  for (int i = 0; i < winx * winy; i++)
      {
      if (skeletonbuff[i].GetZ())
         PerspectiveFrontBackCircleVessel(skeletonbuff, frontbuff, backbuff, i);
      }
  }


//-----------------------------------------------------------------------
      //Raycast a vessel point into a greyscale buffer
void VESSVR::RayCastVessel(ZBUFF* circlebuff, unsigned char* outputbuff)
   {
   float dir[3];
   float pt[3];
   float t;
   float zpos;
   unsigned char intensity, maxintensity;
   float distance = 0;
   int x,y, index, zbuffindex;
   float rad;

  for (y = 0; y < winy; y++)
      {
      index = winx * y;
      for (x = 0; x < winx; x++)
         {
         zbuffindex = index + x;
         zpos = circlebuff[zbuffindex].GetZ();
         if (zpos) 
            {

            //Get the normalized ray direction
             EyeToPix((float)x, (float)y,dir);

             //Get the 3D rad of that vessel point (may be a dilated rad)
             rad = circlebuff[zbuffindex].GetRRad();
             distance = 0;

          //Get the x,y,z coords of the point on that ray where z = zpos. The origin
            //is 0,0,0 so you do not need to take the origin into account. Dir[2] will
            //always have a value under this system so you do not need to check.
             t = (zpos + rad)/dir[2];
             pt[0] = t * dir[0];
             pt[1] = t * dir[1];
             pt[2] = t * dir[2];

             maxintensity = GetIntensityFromFPTransformedCoords(pt[0], pt[1], pt[2]);


            //now walk along the ray for at least a distance 2 * rad looking for maxintensity
            while (distance < 2 * rad)
               {
               distance += raystepsize;
               pt[0] += raystepsize * dir[0];
               pt[1] += raystepsize * dir[1];
               pt[2] += raystepsize * dir[2];
               intensity = GetIntensityFromFPTransformedCoords(pt[0], pt[1], pt[2]);
               if (intensity > maxintensity) maxintensity = intensity;
              }
            outputbuff[y * winx + x] = maxintensity;  
            }
         }
     }
   }
//-----------------------------------------------------------------------
      //Raycast a vessel point as greyscale into a color  buffer
void VESSVR::RayCastVesselColor(ZBUFF* circlebuff, unsigned char* outputbuff)
   {

   float dir[3];
   float pt[3];
   float t;
   float zpos;
   unsigned char intensity, maxintensity;
   float distance = 0;
   int x,y, index, zbuffindex;
   float rad;
   int i;

  for (y = 0; y < winy; y++)
      {
      index = winx * y;
      for (x = 0; x < winx; x++)
         {
         zbuffindex = index + x;
         zpos = circlebuff[zbuffindex].GetZ();
         if (zpos) 
            {

            //Get the normalized ray direction
             EyeToPix((float)x, (float)y,dir);

             //Get the 3D rad of that vessel point (may be a dilated rad)
             rad = circlebuff[zbuffindex].GetRRad();
             distance = 0;

          //Get the x,y,z coords of the point on that ray where z = zpos. The origin
            //is 0,0,0 so you do not need to take the origin into account. Dir[2] will
            //always have a value under this system so you do not need to check.
             t = (zpos + rad)/dir[2];
             pt[0] = t * dir[0];
             pt[1] = t * dir[1];
             pt[2] = t * dir[2];

             maxintensity = GetIntensityFromFPTransformedCoords(pt[0], pt[1], pt[2]);


            //now walk along the ray for at least a distance 2 * rad looking for maxintensity
            while (distance < 2 * rad)
               {
               distance += raystepsize;
               pt[0] += raystepsize * dir[0];
               pt[1] += raystepsize * dir[1];
               pt[2] += raystepsize * dir[2];
               intensity = GetIntensityFromFPTransformedCoords(pt[0], pt[1], pt[2]);
               if (intensity > maxintensity) maxintensity = intensity;
               }

            int val =  3 * (y * winx + x);


            for (i = 0; i < 3; i++)
               outputbuff[val + i] = maxintensity;  

            }
         }
     }
   }
//----------------------------------------------------------
      //Alternative ray casting method that uses a front and back buffer
void VESSVR::RayCastPointToPoint(ZBUFF* circlebuff, ZBUFF* backcirclebuff, unsigned char* outputimage)
   {
 float dir[3];
   float pt[3];
   float t;
   float zposfront, zposback;
   unsigned char intensity, maxintensity;
   int x,y, index, zbuffindex;
   float frontrad, backrad;


  for (y = 0; y < winy; y++)
      {
      index = winx * y;
      for (x = 0; x < winx; x++)
         {
         zbuffindex = index + x;
         zposfront = circlebuff[zbuffindex].GetZ();
         if (zposfront) 
            {

            //Get the normalized ray direction
             EyeToPix((float)x, (float)y,dir);

             //Get the 3D rad of that vessel point (may be a dilated rad)
             frontrad = circlebuff[zbuffindex].GetRRad();
             backrad = backcirclebuff[zbuffindex].GetRRad();
             zposback = backcirclebuff[zbuffindex].GetZ();


          //Get the x,y,z coords of the point on that ray where z = zpos. The origin
            //is 0,0,0 so you do not need to take the origin into account. Dir[2] will
            //always have a value under this system so you do not need to check.
             t = (zposfront + frontrad)/dir[2];
             pt[0] = t * dir[0];
             pt[1] = t * dir[1];
             pt[2] = t * dir[2];

             maxintensity = GetIntensityFromFPTransformedCoords(pt[0], pt[1], pt[2]);


            //now walk along the ray for at least a distance 2 * rad looking for maxintensity
            while (pt[2] > zposback - backrad)
               {
               pt[0] += raystepsize * dir[0];
               pt[1] += raystepsize * dir[1];
               pt[2] += raystepsize * dir[2];
               intensity = GetIntensityFromFPTransformedCoords(pt[0], pt[1], pt[2]);
               if (intensity > maxintensity) maxintensity = intensity;
              }
            outputimage[y * winx + x] = maxintensity;         

            }
         }
     }
   }

//----------------------------------------------------------
      //Alternative ray casting method that uses a front and back buffer--same as above,
      //but writes greyscale info directly to a color buffer.
void VESSVR::RayCastPointToPointColor(ZBUFF* circlebuff, ZBUFF* backcirclebuff, unsigned char* outputimage)
   {
   float dir[3];
   float pt[3];
   float t;
   float zposfront, zposback;
   unsigned char intensity, maxintensity;
   int x,y, index, zbuffindex;
   float frontrad, backrad;
   int i;


  for (y = 0; y < winy; y++)
      {
      index = winx * y;
      for (x = 0; x < winx; x++)
         {
         zbuffindex = index + x;
         zposfront = circlebuff[zbuffindex].GetZ();
         if (zposfront) 
            {

            //Get the normalized ray direction
             EyeToPix((float)x, (float)y,dir);

             //Get the 3D rad of that vessel point (may be a dilated rad)
             frontrad = circlebuff[zbuffindex].GetRRad();
             backrad = backcirclebuff[zbuffindex].GetRRad();
             zposback = backcirclebuff[zbuffindex].GetZ();


          //Get the x,y,z coords of the point on that ray where z = zpos. The origin
            //is 0,0,0 so you do not need to take the origin into account. Dir[2] will
            //always have a value under this system so you do not need to check.
             t = (zposfront + frontrad)/dir[2];
             pt[0] = t * dir[0];
             pt[1] = t * dir[1];
             pt[2] = t * dir[2];

             maxintensity = GetIntensityFromFPTransformedCoords(pt[0], pt[1], pt[2]);


            //now walk along the ray for at least a distance 2 * rad looking for maxintensity
            while (pt[2] > zposback - backrad)
               {
               pt[0] += raystepsize * dir[0];
               pt[1] += raystepsize * dir[1];
               pt[2] += raystepsize * dir[2];
               intensity = GetIntensityFromFPTransformedCoords(pt[0], pt[1], pt[2]);
               if (intensity > maxintensity) maxintensity = intensity;
              }

            int val = 3 * (y * winx + x);
            for (i = 0; i < 3; i++)
               outputimage[val + i] = maxintensity;         

            }
         }
     }
   }

//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
      //PROTECTED FUNCTIONS RAYTRACE
 //&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
void VESSVR::PerspectiveCircleVessel(ZBUFF* centerlinezbuff, ZBUFF* circlebuff, int zbuffindex)
   {

               //get the the rounded integer projection point x,y represented by zbuffindex
               //As circles overwrite their own vessel's info, make sure you restore
               //the initial info for each skeleton point by copying from centerlinezbuff.
    int ycenter = zbuffindex/winx;
    int xcenter = zbuffindex - (ycenter * winx);
  

               //Get the z position of the rotated, translated 3D point
   float transformed_z = centerlinezbuff[zbuffindex].GetZ();

              //Get an approximation of an integer rad on projection given the z position
           //of the point in question and the point's 3D radius.
   int projrad = (int)( -centerlinezbuff[zbuffindex].GetRRad() * dist_to_win/transformed_z + 0.5);


           //check that you will not walk outside of the bounds of the 2D zbuff array. 
   if(xcenter - projrad <0 || ycenter - projrad < 0) return;
   if(projrad + xcenter >= winx || projrad + ycenter >= winy) return;

            //begin Bressenham circles, modified for use here
  int x = 0;
  int y = projrad;
  int d = 1 -projrad;
  int deltaE = 3;
  int deltaSE = -2 * projrad + 5;


   SolidcpointsVessel(x, y, xcenter, ycenter, zbuffindex, projrad, centerlinezbuff,circlebuff);
   while (y >x)
   {
   if (d < 0)
     {
     d = d + deltaE;
     deltaE = deltaE + 2;
     deltaSE = deltaSE + 2;
     x = x + 1;
     }
    else
      {
      d = d + deltaSE;
      deltaE = deltaE + 2;
      deltaSE = deltaSE + 4;
      x = x + 1;
      y = y - 1;
      }
    SolidcpointsVessel(x,y,xcenter,ycenter, zbuffindex, projrad, centerlinezbuff, circlebuff);
    }
  }
//---------------------------------------------------------
   //This alternate uses a front and back buffer. Slower, but better when using radmult.
void VESSVR::PerspectiveFrontBackCircleVessel(ZBUFF* centerlinezbuff, ZBUFF* frontbuff, ZBUFF* backbuff, int zbuffindex)
   {
             //get the the rounded integer projection point x,y represented by zbuffindex
               //As circles overwrite their own vessel's info, make sure you restore
               //the initial info for each skeleton point by copying from centerlinezbuff.
    int ycenter = zbuffindex/winx;
    int xcenter = zbuffindex - (ycenter * winx);
  

               //Get the z position of the rotated, translated 3D point
   float transformed_z = centerlinezbuff[zbuffindex].GetZ();
              //Get an approximation of an integer rad on projection given the z position
           //of the point in question and the point's 3D radius.
   int projrad = (int)( -centerlinezbuff[zbuffindex].GetRRad() * dist_to_win/transformed_z + 0.5);

 
           //check that you will not walk outside of the bounds of the 2D zbuff array
   if(xcenter - projrad <0 || ycenter - projrad < 0) return;
   if(projrad + xcenter >= winx || projrad + ycenter >= winy) return;

   
            //begin Bressenham circles, modified for use here
  int x = 0;
  int y = projrad;
  int d = 1 -projrad;
  int deltaE = 3;
  int deltaSE = -2 * projrad + 5;

   SolidFrontBackcpointsVessel(x, y, xcenter, ycenter, zbuffindex, projrad, centerlinezbuff, frontbuff, backbuff);
   while (y >x)
   {
   if (d < 0)
     {
     d = d + deltaE;
     deltaE = deltaE + 2;
     deltaSE = deltaSE + 2;
     x = x + 1;
     }
    else
      {
      d = d + deltaSE;
      deltaE = deltaE + 2;
      deltaSE = deltaSE + 4;
      x = x + 1;
      y = y - 1;
      }
    SolidFrontBackcpointsVessel(x, y, xcenter, ycenter, zbuffindex, projrad, centerlinezbuff,frontbuff, backbuff);

    }
  }

//-----------------------------------------------------------------
   //Step in drawing circles. This is a good debugging place.
void VESSVR::SolidcpointsVessel
(int x, int y, int xc, int yc, int zbuffindex, int projrad, ZBUFF* centerlinezbuff, ZBUFF* circlebuff)
   {
   DrawLinePerspectiveCircleVessel(-x + xc, x + xc, y + yc, zbuffindex, x, projrad,  centerlinezbuff, circlebuff);
   DrawLinePerspectiveCircleVessel(-y + xc, y + xc, x + yc, zbuffindex, y, projrad,  centerlinezbuff, circlebuff);
   DrawLinePerspectiveCircleVessel(-y + xc, y + xc, -x + yc, zbuffindex, y, projrad, centerlinezbuff, circlebuff);
   DrawLinePerspectiveCircleVessel(-x + xc, x + xc, -y + yc, zbuffindex, y, projrad, centerlinezbuff, circlebuff);
   }
//-----------------------------------------------------------------
   //Variant of above that uses front and back circles
void VESSVR::SolidFrontBackcpointsVessel
(int x, int y, int xc, int yc, int zbuffindex, int projrad, ZBUFF* centerlinezbuff, ZBUFF* frontbuff, ZBUFF* backbuff)
   {
   DrawFrontBackLinePerspectiveCircleVessel(-x + xc, x + xc, y + yc, zbuffindex, x, projrad,  centerlinezbuff, frontbuff, backbuff);
   DrawFrontBackLinePerspectiveCircleVessel(-y + xc, y + xc, x + yc, zbuffindex, y, projrad,  centerlinezbuff, frontbuff, backbuff);
   DrawFrontBackLinePerspectiveCircleVessel(-y + xc, y + xc, -x + yc, zbuffindex, y, projrad, centerlinezbuff, frontbuff, backbuff);
   DrawFrontBackLinePerspectiveCircleVessel(-x + xc, x + xc, -y + yc, zbuffindex, y, projrad, centerlinezbuff, frontbuff, backbuff);
   }
//---------------------------------------------------------------------
void VESSVR::DrawLinePerspectiveCircleVessel
(int minx, int maxx, int y, int zbuffindex, int len, int projrad, ZBUFF* centerlinezbuff, ZBUFF* circlebuff)
   {
   int i;
   int yindex = y * winx;
   float newrad;
   float oldrad = centerlinezbuff[zbuffindex].GetRRad();
   int index;


   newrad = oldrad * len/(float)projrad;

   if(newrad <= 0) return;



    for (i = minx; i <= maxx; i++)
      {
      index = yindex + i;

  

            //Check if this point has previously been written to and do not overwrite
            //if not indicated
      if(circlebuff[index].GetZ())     
         {
            //do not write over any point on a vessel closer to you. It is OK to write over
            //your own points for a point closer--otherwise you get dark spots. Z is always
            //negative, so a point closer to you has a larger z.
         if ( (circlebuff[index].GetVessID() !=centerlinezbuff[zbuffindex].GetVessID()) && (circlebuff[index].GetZ() >=centerlinezbuff[zbuffindex].GetZ())) 
            {
            continue;
            }
         }
   
      
         //write the vessel and vessel point tocirclebuff
      circlebuff[index].SetVess(centerlinezbuff[zbuffindex].GetVessID(),centerlinezbuff[zbuffindex].GetPointID());
      circlebuff[index].SetZ(centerlinezbuff[zbuffindex].GetZ());
      circlebuff[index].SetRRad(newrad);
      }
   }
//---------------------------------------------------------------------
void VESSVR::DrawFrontBackLinePerspectiveCircleVessel
(int minx, int maxx, int y, int zbuffindex, int len, int projrad, ZBUFF* centerlinezbuff, ZBUFF* frontbuff, ZBUFF* backbuff)
   {
   int i;
   int yindex = y * winx;
   float newrad;
   float oldrad = centerlinezbuff[zbuffindex].GetRRad();
   int index;
   newrad = oldrad * len/(float)projrad;
   float oldfrontz, oldbackz;


   if(newrad <= 0) return;


    for (i = minx; i <= maxx; i++)
      {
      index = yindex + i;
      oldfrontz = frontbuff[index].GetZ();
      oldbackz = backbuff[index].GetZ();
  

            //Check if this point has previously been written to and do not overwrite
            //if not indicated
      if(oldfrontz)     
         {
         if (oldfrontz < centerlinezbuff[zbuffindex].GetZ())  //farther away
            {
            frontbuff[index].SetVess(centerlinezbuff[zbuffindex].GetVessID(),centerlinezbuff[zbuffindex].GetPointID());
            frontbuff[index].SetZ(centerlinezbuff[zbuffindex].GetZ());
            frontbuff[index].SetRRad(newrad);
            }
         if(oldbackz > centerlinezbuff[zbuffindex].GetZ())  //closer
            {
            backbuff[index].SetVess(centerlinezbuff[zbuffindex].GetVessID(),centerlinezbuff[zbuffindex].GetPointID());
            backbuff[index].SetZ(centerlinezbuff[zbuffindex].GetZ());
            backbuff[index].SetRRad(newrad);
            }
         }
      else              //nothing written yet
         {
         frontbuff[index].SetVess(centerlinezbuff[zbuffindex].GetVessID(),centerlinezbuff[zbuffindex].GetPointID());
         frontbuff[index].SetZ(centerlinezbuff[zbuffindex].GetZ());
         frontbuff[index].SetRRad(newrad);
         backbuff[index].SetVess(centerlinezbuff[zbuffindex].GetVessID(),centerlinezbuff[zbuffindex].GetPointID());
         backbuff[index].SetZ(centerlinezbuff[zbuffindex].GetZ());
         backbuff[index].SetRRad(newrad);
         }
      }
   }
//--------------------------------------------------------------------------
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
      //PROTECTED::GENERAL VOLUME RENDERING FUNCTIONS
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
      //Get normalized direction vector between a pixel and raysource
void VESSVR::EyeToPix(float pixx, float pixy, float* vec)
   {
   vec[0] = pixx; 
   vec[1] = pixy; 
   vec[2] = 0;
   eyetopix.TransformPt(3, vec);       //transform as a point; include translations

         //normalize
   float len = (float)sqrt(vec[0] * vec[0] + vec[1] * vec[1] + vec[2] * vec[2]);
   if (len)
      {
      vec[0] = vec[0]/len;
      vec[1] = vec[1]/len;
      vec[2] = vec[2]/len;
      }
   }
//-----------------------------------------------------------------------
unsigned char VESSVR::GetIntensityFromFPTransformedCoords(float x, float y, float z)
   {
   float pt[3] = {x,y,z};
   

         //find the x,y,z position of the point in untransformed coords. This matrix will
         //also convert from voxel to world coords
   invertcurrentpos.TransformPt(3, pt);


         //This floating point 3D point is in the middle of some cube. Check boundaries.
  int minv[3];
   minv[0] = (int) pt[0];
   minv[1] = (int) pt[1];
   minv[2] = (int) pt[2];
  if (minv[0] < 0 || minv[1] < 0 || minv[2] < 0) return 0; 

 int maxv[3];
   maxv[0] = minv[0] + 1;
   maxv[1] = minv[1] + 1;
   maxv[2] = minv[2] + 1;
   if(maxv[0] >= slicedims[0]  || maxv[1] >= slicedims[1] || maxv[2] >= slicedims[2]) return 0; 
     

         //get the fractional distance of the point from min along each axis
   float fracx = pt[0] - minv[0];
   float fracy = pt[1] - minv[1];
   float fracz = pt[2] - minv[2];
 

   /*
          //get the interpolated greyscale value of the point along the minz face.
   unsigned char a = slicebuff[minv[2] * slicesize + minv[1] * slicedims[0] + minv[0]];  //0,0,0
   unsigned char b = slicebuff[minv[2] * slicesize + minv[1] * slicedims[0] + maxv[0]];  //1,0,0
   float gbottom = a + fracx * (b - a);
   a = slicebuff[minv[2] * slicesize + maxv[1] * slicedims[0] + minv[0]];                //0,1,0
   b = slicebuff[minv[2] * slicesize + maxv[1] * slicedims[0] + maxv[0]];                //1,1,0
   float gtop = a + fracx * (b - a);
   float frontval = gbottom  + fracy * (gtop - gbottom);

         //get the interpolated greyscale value of the point along the maxz face.
   a = slicebuff[maxv[2] * slicesize + minv[1] * slicedims[0] + minv[0]];                //0,0,1
   b = slicebuff[maxv[2] * slicesize + minv[1] * slicedims[0] + maxv[0]];                //1,0,1
   gbottom = a + fracx * (b - a);
   a = slicebuff[maxv[2] * slicesize + maxv[1] * slicedims[0] + minv[0]];                //0,1,1
   b = slicebuff[maxv[2] * slicesize + maxv[1] * slicedims[0] + maxv[0]];                //1,1,1
   gtop = a + fracx * (b - a);
   float backval = gbottom + fracy * (gtop - gbottom);

         //get the final greyscale value interpolated between the two faces
   float finalval = frontval + fracz * (backval - frontval);

*/

         //Variant of the above--try to reduce multiplications
   int index = minv[2] * slicesize + minv[1] * slicedims[0] + minv[0];
   unsigned char a = slicebuff[index];                                  //0,0,0
   unsigned char b = slicebuff[index + 1];                              //1,0,0
   float gbottom = a + fracx * (b - a);
   a = slicebuff[index + slicedims[0]];                                 //0,1,0
   b = slicebuff[index + slicedims[0] + 1];                             //1,1,0
   float gtop = a + fracx * (b - a);
   float frontval = gbottom  + fracy * (gtop - gbottom);

   int backindex = index + slicesize;
   a = slicebuff[backindex];                                            //0,0,1
   b = slicebuff[backindex + 1];                                        //1,0,1
   gbottom = a + fracx * (b - a);
   a = slicebuff[backindex + slicedims[0]];                             //0,1,1
   b = slicebuff[backindex + slicedims[0] + 1];                         //1,1,1
   gtop = a + fracx * (b - a);
   float backval = gbottom + fracy * (gtop - gbottom);

         //get the final greyscale value interpolated between the two faces
   float finalval = frontval + fracz * (backval - frontval);




/*
This is a more elegant presentation, taken from a Zuiderveld paper. It is marginally faster
but produces a grainy output. Maybe I made a mistake putting it in

   //here is put as valxyz
   unsigned char val000 = slicebuff[minv[2] * slicesize + minv[1] * slicedims[0] + minv[0]];
   unsigned char val001 = slicebuff[maxv[2] * slicesize + minv[1] * slicedims[0] + minv[0]];
   unsigned char val010 = slicebuff[minv[2] * slicesize + maxv[1] * slicedims[0] + minv[0]];
   unsigned char val011 = slicebuff[maxv[2] * slicesize + maxv[1] * slicedims[0] + minv[0]];
   unsigned char val100 = slicebuff[minv[2] * slicesize + minv[1] * slicedims[0] + maxv[0]];
   unsigned char val101 = slicebuff[maxv[2] * slicesize + minv[1] * slicedims[0] + maxv[0]];
   unsigned char val110 = slicebuff[minv[2] * slicesize + maxv[1] * slicedims[0] + maxv[0]];
   unsigned char val111 = slicebuff[maxv[2] * slicesize + maxv[1] * slicedims[0] + maxv[0]];

  


   float finalval = 
        (1 - fracx) * (1 - fracy) * (1 - fracz) * val000\
      + fracx       * (1 - fracy) * (1 - fracz) * val001\
      + (1 - fracx) * fracy       * (1 - fracz) * val010\
      + fracx       * fracy       * (1 - fracz) * val011\
      + (1 - fracx) * (1 - fracy) * fracz       * val100\
      + fracx       * (1 - fracy) * fracz       * val101\
      + (1 - fracx) * fracy       * fracz       * val110\
      + fracz       * fracy       * fracz       * val111;

*/

   if (finalval > 255) finalval = 255;
   else if (finalval < 0) finalval = 0;

  
   return (unsigned char) finalval;
   }


}//end of namespace