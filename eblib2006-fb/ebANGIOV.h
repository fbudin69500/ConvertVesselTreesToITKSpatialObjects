/*
ebANGIOV.h
Viewing parameter system for use in angiogram project. Supports both front
and back projection. Front projection moves the volume of space relative to a 
fixed viewing parameter system. Back projection uses a fixed spatial volume 
and inversely moves the viewing parameter system. This class therefore provides
a variety of matrices and inverse matrices for different uses.
   There are also two sets of window parameters:  a (u,v) system 
which includes dist_to_win, and a dispwin system which maps the (u,v) 
viewplane to a viewing window. The reconstruction program uses (u,v) as the 
larger window upon which planar curves have been defined and the dispwin 
system as a smaller display window. The matrix returns permit clipping if
needed during front projection.

   1) Front projection: use Getviewmap to transform a point input in
      world coordinates into a unit cube. Homogenize the point,
      then multiply by cubetoscreen to get dispwin x,y coords.
   2) Back projection used during angiographic reconstruction:
      the (u,v) values MUST be the size of the input image in
      pixels and dist_to_win MUST be calculated in pixel units. 
      Here, pixels are assumed square. The eye is assumed to be at
      (0,0,0) and the viewplane at (0,0, -dist_to_win).
         Use Eyetopix to find the vector between the eye
      and an (x,y) pixel on the viewplane.  The returned
      vector will be in world coordinates.
         The matrix inverseorient reverses the viewing
      orientation transform--you shouldn't normally need it
      or vieworient here but they're available on need.
   3) Windows must be entered at 1/2 height. This includes dispwin.
   4) Viewvec is returned normalized. If you need it unnormalized,
      recalculate it from vrp - eye.

4/99: ebCONSOLE removed; Done revised to return a returnval

6/02: Revised to fit into std namespace, exception handling

Elizabeth Bullitt
   
*/
#ifndef ANGIOVH
#define ANGIOVH


#include "ebOMAT.h"
#include "ebVEC.h"
#include "ebINCLUDES.h"   //define std or not here   
//#include "MemoryLeak.h"

namespace eb
{

 class ANGIOV
  {
  protected:

      //real world data
    PNT vrp;           //viewer reference point; origin on view plane. Define
                         //in world coords--this is center of xray film.
    PNT eye;           //Center of projection. Define in world coords--this
          //is the ray source.
    double maxu,maxv;     //window size in uvn coords. Size is 2*maxu, 2*maxv
          //This is the measured x-ray size in cms, inches or
          //whatever, or can represent the size in pixels
          //of the window size on which ribbons were defined. 

    VEC vup;       //viewer's desired up vector
    VEC viewvec;   //
   
    double dist_to_win;      //distance to WINSIZE in whatver units

      //Size of display window in pixels
    double dispwinx, dispwiny;

      //Matrices    
     OMAT vieworient;         //viewer transformation matrix
     OMAT viewmap;         //point to (u,v) viewplane;
     OMAT cubetoscreen;      //point on (u,v) viewplane to dispwin;
               //apply after homogenization.


     OMAT inverseorient;      //inverse viewer transform
     OMAT eyetopix;         //vector from eye to pixel on (u,v) 

  public:
          //constructor,  default settings
    ANGIOV();        
    void Default();
    

           //gets and sets
    void Vrp(PNT p)                        {vrp = p;}
    PNT Vrp()                              {return vrp;}
    void Vrp(double a, double b, double c)   {vrp.Set(a,b,c);}
    void Vup(VEC a)                          {vup = a; vup.Vnormalize();}
    void Vup(double a, double b, double c)
          {vup.Set(a,b,c); vup.Vnormalize();}
    VEC Vup()       {return vup;}
    void Eye(PNT p)                        {eye = p;}
    void Eye(double a, double b, double c)   {eye.Set(a,b,c);}
    PNT Eye()                              {return eye;}
    VEC Viewvec()                        {return viewvec;}
    
    OMAT& Vieworient()                     {return vieworient;}
    OMAT& Ioview()                         {return inverseorient;}
    OMAT& Eyetopix()         {return eyetopix;}

    OMAT& Viewmap()         {return viewmap;}
    OMAT& Cubetoscreen()         {return cubetoscreen;}
    
   
    void Window(double, double);                     //view on block; maxu, maxv
    void Window(double*a, double*b)       {*a = 2* maxu; *b = 2* maxv;}

    void Dispwindow(double a, double b)
   {dispwinx=a * 0.5; dispwiny=b * 0.5;}

   double Windistance()      {return dist_to_win;}
   double Winratio()         {return dispwinx/maxu;}


    VEC Eyetopix(int, int);      //gives vector eye to (u,v) pixel
    VEC Eyetopix(double, double);   //gives vector eye to (u,v) point

    double* Cubetoscreen(double* d, int dims = 4);   //gives point transformation to DISPWIN screen.
    
            //workhorse functions
   int Done();                                    //create final matrices
  };


}//end of namespace
#endif
