#include "ebOBJECTVR.h"

#include "ebRESCALET.h"

namespace eb
{

OBJECTVR::OBJECTVR()
   {
         //3D amount to increase radius by both on projection circle and in depth
         //when you come back to render. The default val is 1/2 voxel in z and 1 voxel
         //in xy. You end up with tiny holes at some distances if you use 1/2 voxel only
         //in xy--not sure, but I think that this may be because of the integer math required
         //(if two vessels each of radius 1.49999 have skeletons separated by 3 pixels, you
         //get a one pixel hole at place of closest approachment). So need an expansion factor. 
   surfradaddxy = 0.5;     //expands x-y radius to 1 voxel (projected)
   surfradaddz = 0.0;

   slicebuff = 0;
   winx = winy = 0;
   raystepsize = (float)0.11;
   currentpos = 0;
   isactive = 1;
   mymem = 0;
   }
//------------------------------------------------------
OBJECTVR::~OBJECTVR()
   {
   if (mymem && slicebuff)
      delete [] slicebuff;

   }
//------------------------------------------------------------
      //Change the camera intrinsic settings. This will result in recalc of 
      //a couple of matrices
void OBJECTVR::SetCameraIntrinsics(int wx, int wy, double a)
   {
   winx = wx;
   winy = wy;
   winsize = wx * wy;
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
void OBJECTVR::SetSliceBuffer(unsigned char* buff, const int* d, const float* s)
   {
   if (mymem && slicebuff)
      delete [] slicebuff;
   mymem = 0;

   slicedims = d;
   slicesteps = s;
   slicebuff = buff;
   slicesize = d[0] * d[1];
   radorig3D = (float)(0.5 * sqrt(s[0] * s[0] + s[1] * s[1] + s[2] * s[2]));



   if(buff && s[0] && s[1] && s[2])
      {
     
            //For purposes of speed later, you want to do intensity interpolations
            //using voxel corners as holding the values, but you REALLY want the voxel
            //center to hold these values. The worldtovox matrix is used just before
            //interpolation, and converts from world to voxel coords but also moves
            //the intensity calcs to the corner points. Note that this handling is different
            //than in the vessel rendering module; reason is that the other module uses
            //floating point representation of voxel coords and the current module has only
            //integer info for the surface points. 
      OMAT trans; trans.Translate(-0.5, -0.5, -0.5);
      OMAT scale; scale.Scale(1/slicesteps[0], 1/slicesteps[1], 1/slicesteps[2]);
      worldtovox = trans * scale;
      
      }

   }
//--------------------------------------------------------
      //take in a ushort buffer.with mem assigned elsewhere.  You must assign mem for a scaled
      //uchar buffer
void OBJECTVR::SetSliceBufferAllocMem(const unsigned short* buff, const int* d, const float* s, int imin, int imax)
   {
         //delete a previous buffer
   if (mymem && slicebuff)
      delete [] slicebuff;

   slicebuff = 0;
   mymem = 1;

   slicedims = d;
   slicesteps = s;
   slicesize = d[0] * d[1];
   radorig3D = (float)(0.5 * sqrt(s[0] * s[0] + s[1] * s[1] + s[2] * s[2]));


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
//--------------------------------------------------------
void OBJECTVR::AdjustStepsForThickSlice()
   {
   float s[2] = {0,0};

         //look for something with a huge jump in one direction
   if (slicesteps[2] > 2 * slicesteps[0])
      {s[0] = slicesteps[0]; s[1] = slicesteps[1];}
   else if (slicesteps[0] > 2 * slicesteps[1])
      {s[0] = slicesteps[1]; s[1] = slicesteps[2];}
   else if (slicesteps[1] > 2 * slicesteps[0])
      {s[0] = slicesteps[0]; s[1] = slicesteps[2];}

   if (s[0])
   radorig3D = (float)(0.5 * sqrt(s[0] * s[0] + s[1] * s[1]));
   }



//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
      //PUBLIC DRAW FUNCTIONS
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
         //Create a point cloud of surface points. The function bypasses
         //pointzbuff.
int OBJECTVR::PointCloud
(ZBUFF* pointzbuff, ZBUFF* circlezbuff, unsigned char* outputimage, SURFACECOL* scol, int minid)
   {
   if (!isactive) return 0;
 
            //set up matrices
   if(!PrepareMatrices())    {std::cerr<<"Matrix preperation failed in surfacevr::pointcloud()\n";return 0;}
   
   int i, numpts;
   int* iptr;
   int myid = minid;

            //project points onto zbuffer, placing a point only if no closer object
            //occupies the same point
   for (i = 0; i < scol->GetNumSurfaces(); i++)
      {
      if(!scol->IsVisible(i)) {++myid; continue;}
      iptr = scol->GetPoints(i, &numpts);
      WriteSkeletonInfo(numpts, myid, iptr, circlezbuff);
      ++myid;
      }

            //write the identified points to outputimage with arbitrary intensity 128. It will
            //not interfere with vessel writes as these are are less than minid.
    for (i = 0; i < winsize; i++)
      {
      if (circlezbuff[i].GetZ() && circlezbuff[i].GetVessID() >= minid)
           outputimage[i] = 128;
      }

    return 1;
   }
//-------------------------------------------------------------------
int OBJECTVR::PointCloudColorBuffer
(ZBUFF* pointzbuff, ZBUFF* circlezbuff, unsigned char* outputimage, SURFACECOL* scol, int minid)
   {
   if(!isactive || !scol->GetNumSurfaces()) return 0;

           //set up matrices
   if(!PrepareMatrices())    return 0;

   float holdrad = radorig3D;
   radorig3D = 0;
   
   int i, numpts;
   int* iptr;
   int myid = minid;

            //project points onto point zbuffer, placing a point only if no closer object
            //occupies the same point
   for (i = 0; i < scol->GetNumSurfaces(); i++)
      {
      if(!scol->IsVisible(i)) {++myid; continue;}
      iptr = scol->GetPoints(i, &numpts);
      WriteSkeletonInfoColorBuffer(numpts, myid, iptr, pointzbuff, scol->GetCol(i));
      ++myid;
      }
  
                  //write front surface circles to circlezbuff if nothing else is in front.
                  //Restrict analysis to id numbers greater or equal to # of vessels.
    WriteFrontCircleInfoFromSkeletonInfoColorBuffer(pointzbuff, circlezbuff, minid);

                  //raycast for a short distance around front surface
    RaycastObjectSurfaceColorBuffer(circlezbuff, minid, outputimage);

    radorig3D = holdrad;

    return 1;
 
   /*
   if (!isactive) return 0;
 
            //set up matrices
   if(!PrepareMatrices())    {std::cerr<<"Matrix preperation failed in surfacevr::pointcloud()\n";return 0;}
   
   int i, numpts;
   int* iptr;
   int myid = minid;

            //project points onto zbuffer, placing a point only if no closer object
            //occupies the same point
   for (i = 0; i < scol->GetNumSurfaces(); i++)
      {
      if(!scol->IsVisible(i)) {++myid; continue;}
      iptr = scol->GetPoints(i, &numpts);
      WriteSkeletonInfo(numpts, myid, iptr, circlezbuff);
      ++myid;
      }

            //write the identified points to outputimage with arbitrary intensity 128. It will
            //not interfere with vessel writes as these are are less than minid.
    for (i = 0; i < winsize; i++)
      {
      if (circlezbuff[i].GetZ() && circlezbuff[i].GetVessID() >= minid)
           outputimage[3 * i] = outputimage[3 * i + 1] = outputimage[3 * i + 2] = 128;
      }

    return 1;
    */
   }
//---------------------------------------------------------------
         //Volume render only for a short distance around front surface
int OBJECTVR::SurfaceVolumeRender
(ZBUFF* pointzbuff, ZBUFF* circlezbuff, unsigned char* outputimage, SURFACECOL* scol, int minid)
   {
   if(!isactive || !scol->GetNumSurfaces()) return 0;

           //set up matrices
   if(!PrepareMatrices())    return 0;
   
   int i, numpts;
   int* iptr;
   int myid = minid;

            //project points onto point zbuffer, placing a point only if no closer object
            //occupies the same point
   for (i = 0; i < scol->GetNumSurfaces(); i++)
      {
      if(!scol->IsVisible(i)) {++myid; continue;}
      iptr = scol->GetPoints(i, &numpts);
      WriteSkeletonInfo(numpts, myid, iptr, pointzbuff);
      ++myid;
      }
  
                  //write front surface circles to circlezbuff if nothing else is in front.
                  //Restrict analysis to id numbers greater or equal to # of vessels.
    WriteFrontCircleInfoFromSkeletonInfo(pointzbuff, circlezbuff, minid);

                  //raycast for a short distance around front surface
    RaycastObjectSurface(circlezbuff, minid, outputimage);

    return 1;
   }
//---------------------------------------------------------------
//---------------------------------------------------------------
         //Volume render only for a short distance around front surface into a color buffer
int OBJECTVR::SurfaceVolumeRenderColorBuffer
(ZBUFF* pointzbuff, ZBUFF* circlezbuff, unsigned char* outputimage, SURFACECOL* scol, int minid)
   {
   if(!isactive || !scol->GetNumSurfaces()) return 0;



           //set up matrices
   if(!PrepareMatrices())    return 0;
   
   int i, numpts;
   int* iptr;
   int myid = minid;


            //project points onto point zbuffer, placing a point only if no closer object
            //occupies the same point
   for (i = 0; i < scol->GetNumSurfaces(); i++)
      {
      if(!scol->IsVisible(i)) {++myid; continue;}     
      iptr = scol->GetPoints(i, &numpts);
      WriteSkeletonInfoColorBuffer(numpts, myid, iptr, pointzbuff, scol->GetCol(i));
      ++myid;

      }
  
                  //write front surface circles to circlezbuff if nothing else is in front.
                  //Restrict analysis to id numbers greater or equal to # of vessels.
    WriteFrontCircleInfoFromSkeletonInfoColorBuffer(pointzbuff, circlezbuff, minid);

    RaycastObjectSurfaceColorBuffer(circlezbuff, minid, outputimage);


    return 1;
   }
//-------------------------------------------------------------------------
         //Volume render through object
int OBJECTVR::DeepVolumeRender
(ZBUFF* pointzbuff, ZBUFF* circlezbuff, unsigned char* outputimage, SURFACECOL* scol, int minid)
   {
   if(!isactive || !scol->GetNumSurfaces()) return 0;

           //set up matrices
   if(!PrepareMatrices())    return 0;
   
   int i, x,y, numpts;
   int* iptr;
   int myid = minid;


            //project points onto point zbuffer, placing a point only if no closer object
            //occupies the same point
   for (i = 0; i < scol->GetNumSurfaces(); i++)
      {
      if(!scol->IsVisible(i)) {++myid; continue;}
      iptr = scol->GetPoints(i, &numpts);  
      WriteSkeletonInfo(numpts, myid, iptr, pointzbuff);
      ++myid;
      }
  
                  //write front surface circles to circlezbuff if nothing else is in front.
                  //Restrict analysis to id numbers greater or equal to # of vessels.
                  //Front circles give the place for rays to start.
    WriteFrontCircleInfoFromSkeletonInfo(pointzbuff, circlezbuff, minid);

                  //write back surface info to circlezbuff for each point
                  //written to in the function above. The "backz" value tells
                  //the ray when to stop. For non-vessel objects, this backz value
                  //is stored in the radius slot of the zbuffer. As implemented,
                  //the process will destroy pointzbuff info, so any other class writing
                  //to these buffers must have written to circlezbuff first. 
   WriteBackCircleInfoFromSkeletonInfo(pointzbuff, circlezbuff, minid, scol);

                  //ray cast over indicated path.
   float depthrad = (float)(1.0 + surfradaddz) * radorig3D;        //added depth if desired
   for (i = 0; i < winsize; i++)
      {
      if (circlezbuff[i].GetZ() && circlezbuff[i].GetVessID() >= minid)
         {
         y = i/winx;
         x = i - (y * winx);
         RaycastDeepPoint(x, y, circlezbuff[i].GetZ(), circlezbuff[i].GetRRad(), depthrad, outputimage);
         }
      }
   return 1;
   }
//-------------------------------------------------------------------------
         //Volume render through object
int OBJECTVR::DeepVolumeRenderColorBuffer
(ZBUFF* pointzbuff, ZBUFF* circlezbuff, unsigned char* outputimage, SURFACECOL* scol, int minid)
   {
   if(!isactive || !scol->GetNumSurfaces()) return 0;

           //set up matrices
   if(!PrepareMatrices())    return 0;
   
   int i, x,y, numpts;
   int* iptr;
   int myid = minid;


            //project points onto point zbuffer, placing a point only if no closer object
            //occupies the same point
   for (i = 0; i < scol->GetNumSurfaces(); i++)
      {
      if(!scol->IsVisible(i)) {++myid; continue;}
      iptr = scol->GetPoints(i, &numpts);  
      WriteSkeletonInfo(numpts, myid, iptr, pointzbuff);
      ++myid;
      }
  
                  //write front surface circles to circlezbuff if nothing else is in front.
                  //Restrict analysis to id numbers greater or equal to # of vessels.
                  //Front circles give the place for rays to start.
    WriteFrontCircleInfoFromSkeletonInfo(pointzbuff, circlezbuff, minid);

                  //write back surface info to circlezbuff for each point
                  //written to in the function above. The "backz" value tells
                  //the ray when to stop. For non-vessel objects, this backz value
                  //is stored in the radius slot of the zbuffer. As implemented,
                  //the process will destroy pointzbuff info, so any other class writing
                  //to these buffers must have written to circlezbuff first. 
   WriteBackCircleInfoFromSkeletonInfo(pointzbuff, circlezbuff, minid, scol);

                  //ray cast over indicated path.
   float depthrad = (float)(1.0 + surfradaddz) * radorig3D;        //added depth if desired
   for (i = 0; i < winsize; i++)
      {
      if (circlezbuff[i].GetZ() && circlezbuff[i].GetVessID() >= minid)
         {
         y = i/winx;
         x = i - (y * winx);
         RaycastDeepPointColorBuffer
(x, y, circlezbuff[i].GetZ(), circlezbuff[i].GetRRad(), depthrad, outputimage);
         }
      }
   return 1;
   }


//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
      //PROTECTED--GENERAL
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

      //Call this first before any of the draw functions. Make a matrix that inverts
      //the currentposition (restores voxel box to its default position) and additionally
      //moves from world coords to voxel floating point coordinates. It will return 0
      //if the slicebuffer is not set yet.
int OBJECTVR::PrepareMatrices()
   {
   if (!slicebuff) {std::cerr<<"no data slices loaded in OBJECTVR\n"; return 0;}

   double det;

         //make a matrix that converts from world coordinatesto voxel coordinates. A voxel
         //location is moved from center to corner. 
   invertcurrentpos = *currentpos;
   invertcurrentpos.Invert(&det);
   invertcurrentpos = worldtovox * invertcurrentpos;  
                                                      


         //vttoworld transformed is the inverse of invertcurrentpos. It moves from voxel 
         //coordinates to world coordinates. Voxel "center" is also moved to center of voxel. 
   OMAT trans, scale;
   trans.Translate(0.5, 0.5, 0.5);
   scale.Scale(slicesteps[0], slicesteps[1], slicesteps[2]);
   vctoworldtransformed = *currentpos * scale * trans;
   return 1;
   }
//-----------------------------------------------------------------------------
      //break down a composite integer point to x,y,z and put to float
void OBJECTVR::CompositePointToFloats(int val, float* fpts)
   {
   int x,y,z;

   z = val/slicesize;
   y = (val - (z * slicesize))/slicedims[0];
   x = val - (z * slicesize + y * slicedims[0]);
   fpts[0] = (float) x;
   fpts[1] = (float) y;
   fpts[2] = (float) z;
   }
//---------------------------------------------------------
      //Get normalized direction vector between a pixel and raysource
void OBJECTVR::EyeToPix(float pixx, float pixy, float* vec)
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
unsigned char OBJECTVR::GetIntensityFromFPTransformedCoords(float x, float y, float z)
   {

   float pt[3] = {x,y,z};
   

         //find the x,y,z position of the point in untransformed coords. This matrix will
         //also convert from voxel to world coords
   invertcurrentpos.TransformPt(3, pt);

  
         //This floating point 3D point is in the middle of some cube. Check boundaries.
  if (pt[0] < 0 || pt[1] < 0 || pt[2] < 0) return 0; 

  int minv[3];
   minv[0] = (int) pt[0];
   minv[1] = (int) pt[1];
   minv[2] = (int) pt[2];



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
   if (finalval > 255) finalval = 255;
   else if (finalval < 0) finalval = 0;
   */

       //Variant of the above--try to reduce multiplications. there is almost certainly
       //a better way to do this--this is the most time-consuming step of entire program.
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
  
   return (unsigned char) finalval;
   }

//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
      //PROTECTED: SPECIFIC FOR NARROW RENDERING OF FRONT SURFACES
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
         //write surface points to pointbuffer. If you render this alone, you get
         //a cloud of points           
void OBJECTVR::WriteSkeletonInfo(int numpts, int myid, int* iptr, ZBUFF* pointzbuff)
   {
   int i;
   float fpts[3];
   float transformed_z;
   int zbuffintx, zbuffinty, zbuffindex;
 

   for (i = 0; i < numpts; i++)
      {
            //Get integer composite point into 3 float points.
      CompositePointToFloats(iptr[i], fpts); 

               //transform to world coords in new position given by currentpos. The
               //matrix also moves you to center of the voxel.
      vctoworldtransformed.TransformPt(3, fpts);

               //keep track of z position
      transformed_z = fpts[2];

                        //project transformed point onto viewplane
      frontproj.TransformPt(3, fpts);

                   //Get the integer pixel associated with the projection point
      zbuffintx = (int)(fpts[0] + 0.5);   ;
      if (zbuffintx < 0 || zbuffintx >= winx) continue;

      zbuffinty = (int)(fpts[1] + 0.5);              
      if(zbuffinty < 0 || zbuffinty >= winy) continue;
      zbuffindex = zbuffinty * winx + zbuffintx;

                  //if the z position of this point is in front of
                  //anything written on zbuff at that spot, record information
                  //about this projected point. The "rad" slot used by vessels
                  //is used by non-vessel objects as a potential farz position (where ray ends).
                  //You haven't gotten to this step yet, but store the "front" value
                  //in the farz (rad) slot as the default.
       if(pointzbuff[zbuffindex].GetZ() && pointzbuff[zbuffindex].GetZ() >= transformed_z) 
            continue;
       pointzbuff[zbuffindex].Set(myid, i, transformed_z, transformed_z);
       }
   }

//------------------------------------------------------------------------------------
         //write surface points to pointbuffer. If you render this alone, you get
         //a cloud of points. This color version gives partial color           
void OBJECTVR::WriteSkeletonInfoColorBuffer
(int numpts, int myid, int* iptr, ZBUFF* pointzbuff, float* col)
   {
   int i;
   float fpts[3];
   float transformed_z;
   int zbuffintx, zbuffinty, zbuffindex;
 

   for (i = 0; i < numpts; i++)
      {
            //Get integer composite point into 3 float points.
      CompositePointToFloats(iptr[i], fpts); 

               //transform to world coords in new position given by currentpos. The
               //matrix also moves you to center of the voxel.
      vctoworldtransformed.TransformPt(3, fpts);

               //keep track of z position
      transformed_z = fpts[2];

                        //project transformed point onto viewplane
      frontproj.TransformPt(3, fpts);

                   //Get the integer pixel associated with the projection point
      zbuffintx = (int)(fpts[0] + 0.5);   ;
      if (zbuffintx < 0 || zbuffintx >= winx) continue;

      zbuffinty = (int)(fpts[1] + 0.5);              
      if(zbuffinty < 0 || zbuffinty >= winy) continue;
      zbuffindex = zbuffinty * winx + zbuffintx;

                  //if the z position of this point is in front of
                  //anything written on zbuff at that spot, record information
                  //about this projected point. The "rad" slot used by vessels
                  //is used by non-vessel objects as a potential farz position (where ray ends).
                  //You haven't gotten to this step yet, but store the "front" value
                  //in the farz (rad) slot as the default.
       if(pointzbuff[zbuffindex].GetZ() && pointzbuff[zbuffindex].GetZ() >= transformed_z) 
            continue;
       pointzbuff[zbuffindex].Set(myid, i, transformed_z, transformed_z, col);
       }
   }
//-------------------------------------------------------------
         //Draw front circles on circlebuff
void OBJECTVR::WriteFrontCircleInfoFromSkeletonInfo(ZBUFF* pointzbuff, ZBUFF* circlezbuff, int minid)
   {
   float rad = radorig3D * (1 + surfradaddxy) ;    //dilation factor for circle projection
   int i;
   int xcenter, ycenter, projrad;
   float transformed_z;
   int x, y, d, deltaE, deltaSE;


   for (i = 0; i < winsize; i++)
      {
            //skip pixels not written to or written to by vessel objects with lower ID #s
      transformed_z = pointzbuff[i].GetZ();
      if (!transformed_z || pointzbuff[i].GetVessID() < minid)
         continue;

   

               //get pixel coords xcenter, ycenter and the radius on projection
      ycenter = i/winx;
      xcenter = i - (ycenter * winx);
      projrad = (int)(-rad * dist_to_win/transformed_z + 0.5);

               //Check that you are not out of bounds on the window
      if (xcenter - projrad < 0 || ycenter - projrad < 0)
         continue;
      if (xcenter + projrad >= winx || ycenter + projrad >= winy) 
         continue;

               //Do filled Bressenham circles
      x = 0;
      y = projrad;
      d = 1 -projrad;
      deltaE = 3;
      deltaSE = -2 * projrad + 5;
      SolidcpointsFrontObject(x, y, xcenter, ycenter, i, pointzbuff, circlezbuff);


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
       SolidcpointsFrontObject(x,y,xcenter,ycenter, i, pointzbuff, circlezbuff);
       }
     }
  }
//-----------------------------------------------------------------
//-------------------------------------------------------------
         //Draw front circles on circlebuff. This version writes color
void OBJECTVR::WriteFrontCircleInfoFromSkeletonInfoColorBuffer
(ZBUFF* pointzbuff, ZBUFF* circlezbuff, int minid)
   {
   float rad = radorig3D * (1 + surfradaddxy) ;    //dilation factor for circle projection
   int i;
   int xcenter, ycenter, projrad;
   float transformed_z;
   int x, y, d, deltaE, deltaSE;


   for (i = 0; i < winsize; i++)
      {
            //skip pixels not written to or written to by vessel objects with lower ID #s
      transformed_z = pointzbuff[i].GetZ();
      if (!transformed_z || pointzbuff[i].GetVessID() < minid)
         continue;

   

               //get pixel coords xcenter, ycenter and the radius on projection
      ycenter = i/winx;
      xcenter = i - (ycenter * winx);
      projrad = (int)(-rad * dist_to_win/transformed_z + 0.5);

               //Check that you are not out of bounds on the window
      if (xcenter - projrad < 0 || ycenter - projrad < 0)
         continue;
      if (xcenter + projrad >= winx || ycenter + projrad >= winy) 
         continue;

               //Do filled Bressenham circles
      x = 0;
      y = projrad;
      d = 1 -projrad;
      deltaE = 3;
      deltaSE = -2 * projrad + 5;
      SolidcpointsFrontObjectColorBuffer(x, y, xcenter, ycenter, i, pointzbuff, circlezbuff);


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
       SolidcpointsFrontObjectColorBuffer(x,y,xcenter,ycenter, i, pointzbuff, circlezbuff);
       }
     }
  }
//-----------------------------------------------------------------
      //Write points along boundary of circle
void OBJECTVR::SolidcpointsFrontObject
(int x, int y, int xc, int yc, int zbuffindex, ZBUFF* pointzbuff, ZBUFF* circlezbuff)
   {
   DrawPerspectiveLineCircleFrontObject(-x + xc, x + xc, y + yc, zbuffindex, pointzbuff, circlezbuff);
   DrawPerspectiveLineCircleFrontObject(-y + xc, y + xc, x + yc, zbuffindex, pointzbuff, circlezbuff);
   DrawPerspectiveLineCircleFrontObject(-y + xc, y + xc, -x + yc, zbuffindex, pointzbuff, circlezbuff);
   DrawPerspectiveLineCircleFrontObject(-x + xc, x + xc, -y + yc, zbuffindex, pointzbuff, circlezbuff);
   }
//-----------------------------------------------------------------
      //Write points along boundary of circle. Color version
void OBJECTVR::SolidcpointsFrontObjectColorBuffer
(int x, int y, int xc, int yc, int zbuffindex, ZBUFF* pointzbuff, ZBUFF* circlezbuff)
   {
   DrawPerspectiveLineCircleFrontObjectColorBuffer(-x + xc, x + xc, y + yc, zbuffindex, pointzbuff, circlezbuff);
   DrawPerspectiveLineCircleFrontObjectColorBuffer(-y + xc, y + xc, x + yc, zbuffindex, pointzbuff, circlezbuff);
   DrawPerspectiveLineCircleFrontObjectColorBuffer(-y + xc, y + xc, -x + yc, zbuffindex, pointzbuff, circlezbuff);
   DrawPerspectiveLineCircleFrontObjectColorBuffer(-x + xc, x + xc, -y + yc, zbuffindex, pointzbuff, circlezbuff);
   }
//-----------------------------------------------------------------------------
      //Write info to zbuff array at each circle point. Recalc rad for each point
void OBJECTVR::DrawPerspectiveLineCircleFrontObject
(int minx, int maxx, int y, int zbuffindex, ZBUFF* pointzbuff, ZBUFF* circlezbuff)
   {
   int i;
   int yindex = y * winx;
   int index;

   for (i = minx; i <= maxx; i++)
      {
      index = yindex + i;

  
          //for blocky objects, check that you have not already cast a ray through a
          //closer point. Otherwise you are repeating ray passage
       
       if (circlezbuff[index].GetZ() && circlezbuff[index].GetZ() >= pointzbuff[zbuffindex].GetZ())
            continue;
    
         //write the object point to circlezbuff. For non-vessel objects, the rrad slot
         //is used for farz (where the ray stops) rather than for radius.
      circlezbuff[index].SetVess(pointzbuff[zbuffindex].GetVessID(), pointzbuff[zbuffindex].GetPointID());
      circlezbuff[index].SetZ(pointzbuff[zbuffindex].GetZ());
      circlezbuff[index].SetRRad(pointzbuff[zbuffindex].GetZ());
      }
   }
//-----------------------------------------------------------------------------
      //Write info to zbuff array at each circle point. Recalc rad for each point. Color version
void OBJECTVR::DrawPerspectiveLineCircleFrontObjectColorBuffer
(int minx, int maxx, int y, int zbuffindex, ZBUFF* pointzbuff, ZBUFF* circlezbuff)
   {
   int i;
   int yindex = y * winx;
   int index;

   for (i = minx; i <= maxx; i++)
      {
      index = yindex + i;

  
          //for blocky objects, check that you have not already cast a ray through a
          //closer point. Otherwise you are repeating ray passage
       
       if (circlezbuff[index].GetZ() && circlezbuff[index].GetZ() >= pointzbuff[zbuffindex].GetZ())
            continue;
    
         //write the object point to circlezbuff. For non-vessel objects, the rrad slot
         //is used for farz (where the ray stops) rather than for radius.
      circlezbuff[index].SetVess(pointzbuff[zbuffindex].GetVessID(), pointzbuff[zbuffindex].GetPointID());
      circlezbuff[index].SetZ(pointzbuff[zbuffindex].GetZ());
      circlezbuff[index].SetRRad(pointzbuff[zbuffindex].GetZ());
      circlezbuff[index].SetCol(pointzbuff[zbuffindex].GetCol());
      }
   }
//---------------------------------------------------------------
      //Step 4A: Raycast only a short distance around front surface
void OBJECTVR::RaycastObjectSurface(ZBUFF* circlezbuff, int minid, unsigned char* outputimage)
   {
   int i, x, y;
   float zpos;
   float rayrad = radorig3D * (1 + surfradaddz);


   for (i = 0; i < winsize; i++)
      {
      zpos = circlezbuff[i].GetZ();
      if (!zpos || circlezbuff[i].GetVessID() < minid)
         continue;

      y = i/winx;
      x = i - (y * winx);

      RaycastObjectPoint(x, y, zpos, rayrad, outputimage);
      }
   }
//---------------------------------------------------------------
      //Step 4A: Raycast only a short distance around front surface
void OBJECTVR::RaycastObjectSurfaceColorBuffer(ZBUFF* circlezbuff, int minid, unsigned char* outputimage)
   {
   int i, x, y;
   float zpos;
   float rayrad = radorig3D * (1 + surfradaddz);     


   for (i = 0; i < winsize; i++)
      {
      zpos = circlezbuff[i].GetZ();
      if (!zpos || circlezbuff[i].GetVessID() < minid)
         continue;

      y = i/winx;
      x = i - (y * winx);

      RaycastObjectPointColorBuffer(x, y, zpos, rayrad, outputimage, circlezbuff[i].GetCol());
      }
   }
//-----------------------------------------------------------
      //raycast for only some distance around surface
void OBJECTVR::RaycastObjectPoint(int x, int y, float zposfront, float rayrad, unsigned char* outputimage)
   {
     float dir[3];
   float pt[3];
   float t;
   unsigned char intensity, maxintensity = 0;


           //Get the normalized ray direction
   EyeToPix((float)x, (float)y,dir);

             //Get the x,y,z coords of the point on that ray where z = zpos. The origin
            //is 0,0,0 so you do not need to take the origin into account. Dir[2] will
            //always have a value under this system so you do not need to check.

   t = (zposfront - rayrad)/dir[2];
   pt[0] = t * dir[0];
   pt[1] = t * dir[1];
   pt[2] = zposfront - rayrad;

   maxintensity = GetIntensityFromFPTransformedCoords(pt[0], pt[1], pt[2]);

   float distance = 0;

               //walk along the ray until you get to 2 * rad
   while (distance < 2 * rayrad)
      {
      pt[0] += raystepsize * dir[0];
      pt[1] += raystepsize * dir[1];
      pt[2] += raystepsize * dir[2];
      intensity = GetIntensityFromFPTransformedCoords(pt[0], pt[1], pt[2]);
      if (intensity > maxintensity) maxintensity = intensity;
      distance += raystepsize;
      }
   outputimage[y * winx + x] = maxintensity;
   }
//-----------------------------------------------------------
      //raycast for only some distance around surface
void OBJECTVR::RaycastObjectPointColorBuffer
(int x, int y, float zposfront, float rayrad, unsigned char* outputimage, float* objectcol)
   {
     float dir[3];
   float pt[3];
   float t;
   unsigned char intensity, maxintensity = 0;


           //Get the normalized ray direction
   EyeToPix((float)x, (float)y,dir);

             //Get the x,y,z coords of the point on that ray where z = zpos. The origin
            //is 0,0,0 so you do not need to take the origin into account. Dir[2] will
            //always have a value under this system so you do not need to check.

   t = (zposfront - rayrad)/dir[2];
   pt[0] = t * dir[0];
   pt[1] = t * dir[1];
   pt[2] = zposfront - rayrad;

   maxintensity = GetIntensityFromFPTransformedCoords(pt[0], pt[1], pt[2]);

   float distance = 0;

               //walk along the ray until you get to 2 * rad
   while (distance < 2 * rayrad)
      {
      pt[0] += raystepsize * dir[0];
      pt[1] += raystepsize * dir[1];
      pt[2] += raystepsize * dir[2];
      intensity = GetIntensityFromFPTransformedCoords(pt[0], pt[1], pt[2]);
      if (intensity > maxintensity) maxintensity = intensity;
      distance += raystepsize;
      }
   int val = 3 * (y * winx + x);
   outputimage[val] = (unsigned char)(maxintensity * objectcol[0]);
   outputimage[val + 1] = (unsigned char)(maxintensity * objectcol[1]);
   outputimage[val + 2] = (unsigned char)(maxintensity * objectcol[2]);

   //std::cerr<<objectcol[0]<<"  "<<objectcol[1]<<"  "<<objectcol[2]<<std::endl;
   }
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
      //ADDITIONAL/REPLACEMENT FUNCTIONS FOR DEEP VOLUME RENDERING
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
      //For each point on circlebuff that has an object projecting to it, find the
      //backz for that object (the place the ray will stop). Backz is written to the
      //rad slot of the zbuffer. This process erases pointzbuff info, so you need to be
      //finished with it before coming here. 
void OBJECTVR::WriteBackCircleInfoFromSkeletonInfo
(ZBUFF* pointzbuff, ZBUFF* circlezbuff, int minid, SURFACECOL* scol)
   {
   int i, j;
   float rad = radorig3D * (1 + surfradaddxy) ;    //dilation factor for circle projection
   float transformed_z;
   int numpts;
   int myid;
   float fpts[4];
   int zbuffintx, zbuffinty, zbuffindex;


   float* tempfarbuff = new float[winsize];
   if (!tempfarbuff)
      {
      std::cerr<<"\nRan out of mem; cannot process far side of objects\n";
      return;
      }

   for (i = 0; i < scol->GetNumSurfaces(); i++)      //for each object
      {
               //skip hidden objects
      if (!scol->IsVisible(i))  continue;
      myid = minid + i;
      numpts = scol->GetNumpoints(i);

               //clear the centerlinezbuff; all you want to know about is this object.
      memset(tempfarbuff, 0, winsize * sizeof(float));
      memset(pointzbuff, 0, winsize * sizeof(ZBUFF));

      for (j = 0; j < numpts; j++)
         {
         scol->GetPoint(i, j, &fpts[0], &fpts[1], &fpts[2]);
         vctoworldtransformed.TransformPt(3, fpts); 

               //keep track of z position
         transformed_z = fpts[2];

                        //project transformed point onto viewplane
         frontproj.TransformPt(3, fpts);

                   //Get the integer pixel associated with the projection point
         zbuffintx = (int)(fpts[0] + 0.5);
         if (zbuffintx < 0 || zbuffintx >= winx) continue;

         zbuffinty = (int)(fpts[1] + 0.5);
         if(zbuffinty < 0 || zbuffinty >= winy) continue;
         zbuffindex = zbuffinty * winx + zbuffintx;
                  //in the farz (rad) slot as the default. 
         if(pointzbuff[zbuffindex].GetZ() && pointzbuff[zbuffindex].GetZ() <= transformed_z) 
            continue;
         pointzbuff[zbuffindex].SetZ(transformed_z);
         }

               //You have written the skeleton points of the back surface. Now do circles
               // on the tempbuff
      for (j = 0; j < winsize; j++)
         {
         if (pointzbuff[j].GetZ())
            PerspectiveObjectBackCircleToFloatbuff(pointzbuff, j, rad, tempfarbuff);
         }

               //OK. For each point in zbuff with myid, set the backz to tempfarbuff val.
      for (j = 0; j < winsize; j++)
         {
         if (circlezbuff[j].GetZ() && circlezbuff[j].GetVessID() == myid)
            circlezbuff[j].SetRRad(tempfarbuff[j]);
         }
      }
   delete [] tempfarbuff;
   }
//-----------------------------------------------------
         //For a single object, write far circles onto a temporary float buffer
void OBJECTVR::PerspectiveObjectBackCircleToFloatbuff
(ZBUFF* pointzbuff, int zbuffindex, float rad, float* tempfarbuff)
   {
              //get the the rounded integer projection point x,y represented by zbuffindex
               //As circles overwrite their own skeleton info, make sure you restore
               //the initial info for each skeleton point by copying from centerlinezbuff
    int ycenter = zbuffindex/winx;
    int xcenter = zbuffindex - (ycenter * winx);

    
               //Get the z position of the rotated, translated 3D point
   float transformed_z = pointzbuff[zbuffindex].GetZ();
   tempfarbuff[zbuffindex] = transformed_z;

           //Get an approximation of an integer rad on projection given the z position
           //of the point in question and the point's 3D radius.
   int projrad = (int)( -rad * dist_to_win/transformed_z + 0.5);

   
 
           //check that you will not walk outside of the bounds of the 2D zbuff array
   if(xcenter - projrad <0 || ycenter - projrad < 0) return;
   if(projrad + xcenter >= winx || projrad + ycenter >= winy) return;

   
            //begin Bressenham circles, modified for use here
  int x = 0;
  int y = projrad;
  int d = 1 -projrad;
  int deltaE = 3;
  int deltaSE = -2 * projrad + 5;
  SolidcpointsBackObject(x, y, xcenter, ycenter, zbuffindex, projrad, tempfarbuff);


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
      SolidcpointsBackObject(x, y, xcenter, ycenter, zbuffindex, projrad, tempfarbuff);
    }
  }
//------------------------------------------------------
void OBJECTVR::SolidcpointsBackObject
  (int x, int y, int xc, int yc, int zbuffindex, int projrad, float* tempfarbuff)
   {
   DrawPerspectiveCircleBackObject(-x + xc, x + xc, y + yc, zbuffindex, projrad, tempfarbuff);
   DrawPerspectiveCircleBackObject(-y + xc, y + xc, x + yc, zbuffindex, projrad, tempfarbuff);
   DrawPerspectiveCircleBackObject(-y + xc, y + xc, -x + yc, zbuffindex, projrad, tempfarbuff);
   DrawPerspectiveCircleBackObject(-x + xc, x + xc, -y + yc, zbuffindex, projrad, tempfarbuff);
   }
//----------------------------------------------------------------------
void OBJECTVR::DrawPerspectiveCircleBackObject
(int minx, int maxx, int y, int zbuffindex, int projrad, float* tempfarbuff)
   {
   int i;
   int yindex = y * winx;
   int index;

  
   for (i = minx; i <= maxx; i++)
      {
      index = yindex + i;
      if(tempfarbuff[index] <= tempfarbuff[zbuffindex])
         continue;
      tempfarbuff[index] = tempfarbuff[zbuffindex];
      }
   }
//-----------------------------------------------------------
void  OBJECTVR::RaycastDeepPoint
(int x, int y, float nearz, float farz, float addrad, unsigned char* outputimage)
   {
    float dir[3];
   float pt[3];
   float t;
   unsigned char intensity, maxintensity = 0;


           //Get the normalized ray direction
   EyeToPix((float)x, (float)y,dir);

             //Get the x,y,z coords of the point on that ray where z = zpos. The origin
            //is 0,0,0 so you do not need to take the origin into account. Dir[2] will
            //always have a value under this system so you do not need to check.

   t = (nearz - addrad)/dir[2];
   pt[0] = t * dir[0];
   pt[1] = t * dir[1];
   pt[2] = nearz - addrad;

   maxintensity = GetIntensityFromFPTransformedCoords(pt[0], pt[1], pt[2]);

   float stopz = farz + addrad * dir[2];

               //walk along the ray until you get to farz
   while (pt[2] > stopz)
      {
      pt[0] += raystepsize * dir[0];
      pt[1] += raystepsize * dir[1];
      pt[2] += raystepsize * dir[2];
      intensity = GetIntensityFromFPTransformedCoords(pt[0], pt[1], pt[2]);
      if (intensity > maxintensity) maxintensity = intensity;
      }
   outputimage[y * winx + x] = maxintensity;
   }
//-----------------------------------------------------------
void  OBJECTVR::RaycastDeepPointColorBuffer
(int x, int y, float nearz, float farz, float addrad, unsigned char* outputimage)
   {
    float dir[3];
   float pt[3];
   float t;
   unsigned char intensity, maxintensity = 0;


           //Get the normalized ray direction
   EyeToPix((float)x, (float)y,dir);

             //Get the x,y,z coords of the point on that ray where z = zpos. The origin
            //is 0,0,0 so you do not need to take the origin into account. Dir[2] will
            //always have a value under this system so you do not need to check.

   t = (nearz - addrad)/dir[2];
   pt[0] = t * dir[0];
   pt[1] = t * dir[1];
   pt[2] = nearz - addrad;

   maxintensity = GetIntensityFromFPTransformedCoords(pt[0], pt[1], pt[2]);

   float stopz = farz + addrad * dir[2];

               //walk along the ray until you get to farz
   while (pt[2] > stopz)
      {
      pt[0] += raystepsize * dir[0];
      pt[1] += raystepsize * dir[1];
      pt[2] += raystepsize * dir[2];
      intensity = GetIntensityFromFPTransformedCoords(pt[0], pt[1], pt[2]);
      if (intensity > maxintensity) maxintensity = intensity;
      }
   int val = 3 * (y * winx + x);
   outputimage[val] = outputimage[val + 1] = outputimage[val + 2] = maxintensity;
   }


}//end of namespace