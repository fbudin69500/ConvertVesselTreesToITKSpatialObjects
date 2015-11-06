#include "ebANGIOV.h" 

namespace eb
{

ANGIOV::ANGIOV()
{Default();}            //set default settings

   //default settings are reasonable angiographic parameters and
   //display sizes. The eye is on the origin, the film is
   //36 inches away and measures 11.5 * 11.5. 
   //Default display is 400*400.
void ANGIOV::Default()
  {
  vrp.Set(0,0,-36);          //viewer reference pt
  vup.Set(0,1,0);            //up is y
  eye.Set(0,0,0);
  maxu = 11.5/2.0;
  maxv = maxu;
  dispwinx = 400; dispwiny = 400;
  Done();
  } 

   
         // ***********************
          //set (u,v) window 
void ANGIOV::Window(double a, double b)
  {
  if (a<0) a*= -1;
  if (b<0) b*=-1;
  if (a==0 || b==0) {a = 1; b = 1;}
  maxu = a * 0.5; maxv = b * 0.5;
  }
   // *************************
int ANGIOV::Done()
  {
  double a,b,c;
  VEC u,v;
  OMAT transmatrix, rotmatrix;
  double det;
  vieworient.Identity();
  inverseorient.Identity();
  eyetopix.Identity();
  

         //*************************
   //First make the viewer transformation matrices. Vieworient defines
   //a coordinate system using the user's choice of up and the viewplane
   //normal.

              //make a transformation matrix that puts eye on the origin
  eye.Get(&a, &b, &c);
  transmatrix.Translate(-a, -b, -c);

             //now make a rotation matrix
  VEC vpn = eye - vrp;  //this is the surface normal of the viewplane
  vpn.Vnormalize();
  vup.Vnormalize();
  u.Vcross(vup, vpn);     //vector at right angles to each; x axis
  if (u.Vlen() <.001) 
      {
      std::cerr<<"Up and vpn cannot lie on same axis.\n";
      return 0;
      }
                      
  u.Vnormalize();
  v.Vcross(vpn, u);      //final up (y) axis
  v.Vnormalize();


  rotmatrix.Coordinate_Rotation(u.Get(),v.Get(),vpn.Get());

           //create vieworient and inverse matrix
  vieworient = rotmatrix * transmatrix;
  inverseorient = vieworient;
  inverseorient.Invert(&det);


         //*********************
         //BACKPROJECTION MATRIX
         //Use this matrix when shooting into the box and
         //calculating which voxels are hit. You are shooting
         //from a screensize of 2u,2v, that is represented
         //as running from -u through (+u-1).
   
  VEC dir, screenvec;
  viewvec = vrp - eye;      //unnormalized view vector

      //create a matrix that converts (u,v) viewplane pixels
      //into a world coordinate eye to pixel ray. Pixels
      //are located at the upper left corner of each square.
 OMAT scaley;
 scaley.Scale(1,-1,1);
 OMAT translatefirst;
 translatefirst.Translate(-maxu, maxv, 0);
 OMAT translatelast;
 translatelast.Translate(viewvec.Axis(1), viewvec.Axis(2), viewvec.Axis(3));
 OMAT iomat = inverseorient;
 eyetopix = translatelast * iomat * translatefirst * scaley;

      //********************************
      //FORWARD PROJECTION MATRIX. It does not
      //include a hither or yon for clipping; see MESHV for that.
      //paraproj stores z values in w and inverts the z
      //value (all object coords will be negative). The
      //points will need to be homogenized later.
  dist_to_win = viewvec.Vlen();  //save as class data
  viewvec.Vnormalize();      //save as class data

  if (dist_to_win == 0)
      {
      std::cerr<<"Invalid distance to win\n";
      return 0;
      }


  scaley.Identity();              
  scaley.Scale(1/(maxu), 1/(maxv), 1/dist_to_win);
  OMAT paraproj;
//  paraproj.Set(3,2,-1.0);    //puts -z vals in w 
   paraproj.SetElement(14,0);

  //paraproj.Set(3,3,0);
   paraproj.SetElement(15, 0);
  viewmap = paraproj * scaley * vieworient; 

      //*************************
      //make a matrix that converts unit cube to console
      //window. Pixels end up at the top left corner of each square
  scaley.Identity();
  translatelast.Identity();
  rotmatrix.Identity();         //will be used for scaling actually
  
  //scaley.Set(1,1,-1);      //correct for y position
   scaley.SetElement(5,-1);

  rotmatrix.Scale( (double)dispwinx, (double)dispwiny, 1);
  translatelast.Translate(1,1,1);
  cubetoscreen = rotmatrix * translatelast *  scaley;

   return 1;
 }
  
   //************************************
   //Get a vector between the eye point and a pixel on the
   //uv screen. This vector is normalized before return. Even though
    //v has gone out of scope by the time the caller gets it, this seems
    //to work, maybe because v returns only simple doubles.
VEC ANGIOV::Eyetopix(int x, int y)
  {
   /*
  PNT pnt(x,y,0);
  eyetopix.Transform(&pnt);
  VEC v(pnt);
  v.Vnormalize();
  return v;
   */

   double pnt[3] = {x,y,0};
   eyetopix.TransformPt(3, pnt);
   VEC v(pnt[0], pnt[1], pnt[2]);
   v.Vnormalize();
  return v;
  }

VEC ANGIOV::Eyetopix(double x, double y)
  {
  double pnt[3] = {x,y,0};
  eyetopix.TransformPt(3, pnt);
  VEC v(pnt[0], pnt[1], pnt[2]);
  v.Vnormalize();
  return v;
  }


   //*********************************
   //Transform a point by cubetoscreen
double* ANGIOV::Cubetoscreen(double* d, int dims)
  {cubetoscreen.TransformPt(dims, d); return d;}


}//end of namespace
