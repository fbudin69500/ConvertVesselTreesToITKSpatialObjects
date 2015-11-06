/*
Class that surface renders mesh covered objects.
Elizabeth Bullitt
*/


#ifndef _ebMESHRENDER_s_h_
#define _ebMESHRENDER_s_h_

#include "ebMESHCOL_s.h"
#include "ebZBUFF.h"
#include "ebINCLUDES.h"

namespace eb
{

class MESHRENDER_s
   {
   public:
       MESHRENDER_s();
      MESHRENDER_s(int winwidth, int winheight, double fov, OMAT* cp);
      ~MESHRENDER_s()      {}

            //Setup functions
      void SetCameraIntrinsics (int winwidth, int winheight, double fov);
      void SetCurrentPos(OMAT* mat)           {currentpos = mat;}
      double GetWinDist()                        {return dist_to_win;}

              //Tell renderer it should or should not draw
      void Activate()         {isactive = 1;}
      void Inactivate()       {isactive = 0;}
      void Activate(int a)      {isactive = a;}
      int IsActive()          {return isactive;}
      void SetVisType(int a)  {dogouraud = a;}

     void Render(MESHCOL_s* mc, ZBUFF* circlezbuff, unsigned char* colorbuff, int x, int y, int _idoffset = 0);

     OMAT* GetFrontproj()      {return &frontproj;}

   protected:
                    //camera intrinsics
      double angle;              //field of view
      double dist_to_win;        //distance src to viewplane
      int winx, winy;            //display window size in pixels

               //matrices
      OMAT* currentpos;        //position matrix sent as pointer from another class
                                 //this matrix is the exact equivalent of opengl's modelview 
                                 //matrix
      OMAT frontproj;                   //front project to unit cube and put to screen
                                          //this is close to the combination of opengl's/glut's
                                          //setting viewport to (winx, winy) and calling 
                                          //gluPerspective(angle, 1.0, 0.0, dist_to_win);
                                          //Total perspective front projection matrix, if
                                          //you did not need to retrieve z values in the middle,
                                          //would be frontproj * currentpos.
      int isactive;             //controls whether or not to draw anything
      int numverticesperfacet;

      int dogouraud;

      int idoffset;           //if several renderers are involved,
                              //set an id offset so you can tell one object
                              //from another


                  //GENERAL
      int Round(float a);
      double Dot(float* a, float* b);
      void Normalize(float* a);

                  //Gouraud shading
      int DrawPixelsWithVertexNormals
(MESHCOL_s* mc, ZBUFF* circlezbuff, unsigned char* colorbuff, int x, int y);
      void DrawFacetGouraud(float* ambient, float* diffuse,\
        FVEC* srfn1, FVEC* srfn2, FVEC* srfn3,int* iptsx, int* iptsy,\
         ZBUFF* circlebuff, unsigned char* colorbuff, int xdim,\
         int ydim, float zpos, int vessid, int ptid);
      void DrawFacetGouraud(float* ambient, float* diffuse,\
        FVEC* srfn1, FVEC* srfn2, FVEC* srfn3,FVEC* srfn4, int* iptsx, int* iptsy,\
         ZBUFF* circlebuff, unsigned char* colorbuff, int xdim,\
         int ydim, float zpos, int vessid, int ptid);
      void GetColorForVertex(float* ambient, float* diffuse, FVEC* srfn, \
         FVEC* vv, FVEC* col);
 
                  //Flat shading
      int DrawPixelsWithFacetNormals
(MESHCOL_s* mc, ZBUFF* circlezbuff, unsigned char* colorbuff, int x, int y);
      void GetColorForFacet(float* ambient, float* diffuse, int* icol, \
         float costheta);
      void DrawFlatFacet(int* iptsx, int* iptsy, int* icol, ZBUFF* circlebuff, \
         unsigned char* colorbuff, int x, int y, float zpos, int vessid, \
         int ptid);

     
   };

}//end of namespace

#endif

