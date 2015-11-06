#ifndef _eblib_h_
#define _eblib_h

//The library requires the following for some modules to compile. It does not include
//anything from vtk.
         //1. fltk libraries(used under graphics display group)
         //2. Microsoft libraries (or equivalent)
                  //opengl32.lib
                  //wsock32.lib
         //3. Stephen Aylward's MetaImage files.

#include <ebINCLUDES.h>      //change if using std namespace or not here


               //Lists. 

#include <ebLIST.h>               //single items; acts as queue, stack, or list. Templated.
#include <ebARLIST.h>            //array of items; queue, stack, or list. Templated.
#include <ebARRAYT.h>            //Similar to arlist but acts as array. Templated.


               //Graphics display:
#include <ebSLICEWIN.h>             //2D slice window using fltk and opengl  
#include <ebZBUFF.h>                //specialized z buffer for volume rendering
#include <ebSURFACE.h>              //object for volume or mesh rendering
#include <ebSURFACECOL.h>           //collection of objects for volume or mesh rendering
#include <ebMESH.h>                  //surface mesh for an object
#include <ebMESHCOL.h>               //collection of meshes
#include <ebMESHRENDER.h>            //surface render (Gouraud shading) a mesh collection
#include <ebVESSVR.h>               //volume render vessels. Optimized for segmented vessels.
#include <ebOBJECTVR.h>               //volume render a set of arbitrarily shaped segmented objects.


               //Calculation
#include <ebDEFINE.h>                //constants definitions
#include <ebGEOM.h>                  //geometry module
#include <ebMATH.h>                  //quartics, quadratics, logs, radians to degrees
#include <ebVEC.h>                  //specialized 3D vector, + 4D points. Doubles only.
#include <ebFVEC.h>                 //float 3D vector
#include <ebOMAT.h>                  //geometrical transformation matrices
#include <ebSTAT.h>                  //very rudimentary statistical tools

               //Graphics and image processing
#include <ebANGIOV.h>               //camera intrinsic settings; front and back projection
#include <ebRESCALET.h>            //rescale image size or intensities. Templated

               //File IO and specialized vessel data structures
#include<ebTBOX.h>                  //text utilities (cstring)
#include<ebTBOX_s.h>                  //text utilities (std::string)
#include <ebFILEHEADERIO.h>            //loads file header; base class for object collections
#include <ebOBJECTIO.h>                //base class for objects, vessels, curves, surfaces etc
#include <ebOBJECTIO_s.h>                //base class for objects, vessels, curves, surfaces etc
#include <ebVESS.h>                  //individual 3D vessel
#include <ebVESSCOL.h>               //collection of 3D vessels
#include <ebVESSCOLUTILS.h>         //additional utilities for acting on a vessel collection
#include <ebVESS_s.h>                  //individual 3D vessel
#include <ebVESSCOL_s.h>               //collection of 3D vessels
#include <ebVESSCOLUTILS_s.h>         //additional utilities for acting on a vessel collection
#include <ebINTCURVE.h>               //2D curves designed for 3D reconstruction





#endif

