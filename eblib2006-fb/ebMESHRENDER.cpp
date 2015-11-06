#include "ebMESHRENDER.h"

namespace eb
{

MESHRENDER::MESHRENDER()
   {
   winx = winy = 0;
   currentpos = 0;
   angle = 0;
   dogouraud = 1;
   isactive = 1;
   idoffset = 0;
   }
//---------------------------------
MESHRENDER::MESHRENDER(int wx, int wy, double a, OMAT* cp)
   {
   SetCameraIntrinsics(wx, wy, a);
   SetCurrentPos(cp);
   }
//------------------------------------------------------
void MESHRENDER::SetCameraIntrinsics(int wx, int wy, double a)
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

   }
//---------------------------------------------
void MESHRENDER::Render(MESHCOL* mc, ZBUFF* circlezbuff, unsigned char* colorbuff, int x, int y, int _idoffset)
   {
   if(!isactive || !mc->GetMeshnum()) return;

   idoffset = _idoffset;

   if(dogouraud) DrawPixelsWithVertexNormals(mc,circlezbuff,colorbuff, x,y);
   else DrawPixelsWithFacetNormals(mc,circlezbuff,colorbuff, x,y);
   }
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
      //PROTECTED: GENERAL FUNCTIONS
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
int MESHRENDER::Round(float a)
   {
   if (a >= 0) return (int)(a + 0.5);
   return (int)(a - 0.5);
   }
//--------------------------------
double MESHRENDER::Dot(float* a, float* b)
   {
   return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
   }
//----------------------------------
void MESHRENDER::Normalize(float* a)
   {
   float len = (float)(sqrt(a[0] * a[0] + a[1] * a[1] + a[2] * a[2]));
   if (len)
      {
      a[0] = a[0]/len;
      a[1] = a[1]/len;
      a[2] = a[2]/len;
      }
   }
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
      //FLAT SHADING
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
int MESHRENDER::DrawPixelsWithFacetNormals
(MESHCOL* vc, ZBUFF* circlezbuff, unsigned char* colorbuff, int x, int y)
   {
   int i, j, k, dobreak;
   float* fptr;
   float* col;
   int icol[3];
   float zpos;
   OMAT totalproj;
   FVEC srfn;
   double costheta;
   float ambient[3];
   float diffuse[3];
   float vv[3] = {0,0, 1};
   int onfacet;
   
         //Get total projection matrix for faster multiplications
   totalproj = frontproj * (*currentpos);


         //allows 3 or 4 vertices per facet
   float pts[12];    //allows 4 vertices of 3D points
   int iptsx[4];     //allows 4 viewplane x projections per facet
   int iptsy[4];     //allows 4 viewplane y projections per facet

   for (i = 0; i < vc->GetMeshnum(); i++)
      {
      if(!vc->GetOpacity(i)) continue;

              
                  //divide the color up into ambient and diffuse. Skip
                  //specular--I don't think you want it.
      col = vc->GetColor(i);
      for (j = 0; j < 3; j++)
         {
         ambient[j] = col[j]/10;
         diffuse[j] = col[j];
         }
                     //PROCESS ALL TRIANGLE FACETS
      fptr = (float*)vc->GetFirstTriangle(i, &srfn);
      numverticesperfacet = 3;
      onfacet = 0;

     

      while (fptr)
         {

                          //copy the point data, as you will need to change it
         for (k = 0; k < 3 * numverticesperfacet; k++)
            pts[k] = fptr[k];
         
            
                  //check if surfacenormal is away from you and continue
                  //if so. If the dot product of the inverted viewvector and
                  //transformed normal is < 0, the face is pointing away.
                  //This means you only have to process about half the
                  //facets. Viewvector here is approximated as central ray
                  //(going backwards)
      totalproj.TransformPt(3, pts);
      
      currentpos->TransformVec(3, srfn.Get());

      costheta = Dot(vv, srfn.Get());


            //if the surface normal is pointing away from you,
            //don't bother processing this face. You will have another
            //face with a surface normal pointing at you.
      if (costheta < 0)
         {        
         fptr = (float*)vc->GetNextTriangle(i, &srfn);
         ++onfacet;
         continue;
         }
         
                     //Get the z position of this facet. The z position
                     //is assumed to be in the middle of the triangle
         float tt[3] = {fptr[0], fptr[1], fptr[2]};
         for (k = 1; k < numverticesperfacet; k++)
            {
            tt[0] +=fptr[3 * k];
            tt[1] += fptr[3 * k + 1];
            tt[2] += fptr[3 * k + 2];
            }
         tt[0] = tt[0]/numverticesperfacet; tt[1]= tt[1]/numverticesperfacet; tt[2] = tt[2]/numverticesperfacet;
         currentpos->TransformPt(3, tt);
         zpos = tt[2];
         

                        //Project the remaining vertex or vertices
         for (j = 1; j < numverticesperfacet; j++)
            totalproj.TransformPt(3, pts + 3 * j);

    
                  //Get the integer projection points
         for (j = 0; j < numverticesperfacet; j++)
            {
            iptsx[j] = Round(pts[3 * j]);
            iptsy[j] = Round(pts[3 * j + 1]);
            }
 
 
                  //check if projection is out of bounds
         dobreak = 0;
         for (k = 0; k < numverticesperfacet; k++)
            {
            if (iptsx[k] < 0 || iptsy[k] < 0 || iptsx[k] >= x || iptsy[k] >= y)
               {
               dobreak = 1; break;
               }
            }
         if(dobreak)
            {
            fptr = (float*)vc->GetNextTriangle(i, &srfn);
            ++onfacet;
            continue;
            }
   
 
                  //define a color for this facet
         GetColorForFacet(ambient, diffuse, icol, (float)costheta);

               //Draw this facet. The last passed param is onpoint, which
               //you haven't calculated yet and would be needed for simulation
               //of catheter passage. Come back and do this later. DEBUG.
         DrawFlatFacet(iptsx, iptsy, icol, circlezbuff, colorbuff, x, y, zpos, i + idoffset, onfacet);

         fptr = (float*)vc->GetNextTriangle(i, &srfn);
         ++onfacet;
         }
           

                    //PROCESS ALL QUAD FACETS
      fptr = (float*)vc->GetFirstQuad(i, &srfn);
      numverticesperfacet = 4;


      while (fptr)
         {
                          //copy the point data, as you will need to change it
         for (k = 0; k < 3 * numverticesperfacet; k++)
            pts[k] = fptr[k];
         
            
                  //check if surfacenormal is away from you and continue
                  //if so. If the dot product of the inverted viewvector and
                  //transformed normal is < 0, the face is pointing away.
                  //This means you only have to process about half the
                  //facets. Viewvector here is approximated as central ray
                  //(going backwards)
      totalproj.TransformPt(3, pts);
      
      currentpos->TransformVec(3, srfn.Get());

      costheta = Dot(vv, srfn.Get());

      if (costheta < 0)
         {        
         fptr = (float*)vc->GetNextQuad(i, &srfn);
         ++onfacet;
         continue;
         }
     
         
                     //Get the z position of this facet. The z position
                     //is assumed to be in the middle of the triangle
         float tt[3] = {fptr[0], fptr[1], fptr[2]};
         for (k = 1; k < numverticesperfacet; k++)
            {
            tt[0] +=fptr[3 * k];
            tt[1] += fptr[3 * k + 1];
            tt[2] += fptr[3 * k + 2];
            }
         tt[0] = tt[0]/numverticesperfacet; tt[1]= tt[1]/numverticesperfacet; tt[2] = tt[2]/numverticesperfacet;
         currentpos->TransformPt(3, tt);
         zpos = tt[2];
         

                        //Project the remaining vertex or vertices
         for (j = 1; j < numverticesperfacet; j++)
            totalproj.TransformPt(3, pts + 3 * j);

    
                  //Get the integer projection points
         for (j = 0; j < numverticesperfacet; j++)
            {
            iptsx[j] = Round(pts[3 * j]);
            iptsy[j] = Round(pts[3 * j + 1]);
            }
 
 
                  //check if projection is out of bounds
         dobreak = 0;
         for (k = 0; k < numverticesperfacet; k++)
            {
            if (iptsx[k] < 0 || iptsy[k] < 0 || iptsx[k] >= x || iptsy[k] >= y)
               {
               dobreak = 1; break;
               }
            }
         if(dobreak)
            {
            fptr = (float*)vc->GetNextQuad(i, &srfn);
            ++onfacet;
            continue;
            }
   
 
                  //define a color for this facet
         GetColorForFacet(ambient, diffuse, icol, (float)costheta);

               //Draw this facet. The last passed param is onpoint, 
         DrawFlatFacet(iptsx, iptsy, icol, circlezbuff, colorbuff, x, y, zpos, i + idoffset, onfacet);

         fptr = (float*)vc->GetNextQuad(i, &srfn);
         ++onfacet;
         }
        }
   return 1;
   }
 //----------------------------------------------------------------
      //this uses only ambient and diffuse. Don't want to spend the time
      //on specular calcs, and probably don't want it anyway. The passed
      //param costheta is the dot product between view vector and facet
      //srfn. Assume the light source is at eyepoint--then you can use
      //the same costheta and vv. You will already have excluded all facets
      //with a costheta < 0, so you don't need to check for that here. 
void MESHRENDER::GetColorForFacet(float* ambient, float* diffuse, int* icol, float costheta)
   {
 

   for (int i = 0; i < 3; i++)
      {
      icol[i] = (int)((ambient[i] + diffuse[i] * costheta) * 255);
      if(icol[i] < 0) icol[i] = 0;
      if (icol[i] > 255) icol[i] = 255;
      }
   }

//----------------------------------------------------
         //This should work for any number of vertices per face
void MESHRENDER::DrawFlatFacet(int* iptsx, int* iptsy, int* icol, ZBUFF* circlebuff, unsigned char* colorbuff, int xdim, int ydim, float zpos, int vessid, int ptid)
   {
   int miny, maxy, minx, maxx;
   int i, index;
   int xc, yc, xpos;
   int lastvertex, thisvertex;
   float slope;
   float lastzpos;

         //Get the min and max y values of the n vertices
   miny = maxy = iptsy[0];

   for (i = 1; i < numverticesperfacet; i++)
      {
      if (iptsy[i] < miny) miny = iptsy[i];
      else if (iptsy[i] > maxy) maxy = iptsy[i];
      }

   if (miny < 0) miny = 0;
   if (maxy >= ydim) maxy = ydim - 1;


   for (yc = miny; yc <= maxy; yc++)
      {
      minx = xdim;
      maxx = -1;
      lastvertex = numverticesperfacet -1;      //last legal vertex
      for (thisvertex = 0; thisvertex <numverticesperfacet; thisvertex++)
         {
         if (iptsy[lastvertex] >= yc || iptsy[thisvertex] >= yc)
            {
            if (iptsy[lastvertex] <= yc || iptsy[thisvertex] <= yc)
               {
               if (iptsy[lastvertex] != iptsy[thisvertex])
                  {
                  slope = (float)(iptsx[thisvertex] - iptsx[lastvertex])/(float)(iptsy[thisvertex] - iptsy[lastvertex]);
                  xpos = Round(slope * (float)((yc - iptsy[lastvertex])) + iptsx[lastvertex]);
                  if (xpos < minx) minx = xpos;
                  if (xpos > maxx) maxx = xpos;
                  }
               }
            }
         lastvertex = thisvertex;
         }

      if (minx < 0) minx = 0;
      if (maxx >= xdim) maxx = xdim - 1;

      if (minx <= maxx)
         {
         for (xc = minx; xc <= maxx; xc++)
            {
                  //only write if this point is closer than one projecting
                  //to the same pixel
             index = yc * xdim + xc;
             lastzpos = circlebuff[index].GetZ();
             if(lastzpos && lastzpos >= zpos)
                continue;

             circlebuff[index].Set(vessid, ptid, zpos, 0); //no need for radius; = 0
            index = 3 * index;      //about to write to color triplet buffer

                     
            colorbuff[index] = icol[0];
            colorbuff[index + 1] = icol[1];
            colorbuff[index + 2] = icol[2];
            }
         }
      }
   }

//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
      //GOURAUD SHADING
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
int MESHRENDER::DrawPixelsWithVertexNormals
(MESHCOL* mc, ZBUFF* circlezbuff, unsigned char* colorbuff, int x, int y)
   {
   int i, j, k, dobreak;
   float* fptr;
   float* col;
   float zpos;
   OMAT totalproj;
   FVEC srfn1;
   FVEC srfn2;
   FVEC srfn3;
   FVEC srfn4;
   FVEC facetsrfn;
   FVEC tempvec;
   double costheta;
   float ambient[3];
   float diffuse[3];
   float vv[3] = {0,0,1};
   int onfacet;
   
         //Get total projection matrix for faster multiplications
   totalproj = frontproj * (*currentpos);

         //allows 3 or 4 vertices per facet
   float pts[12];    //allows 4 vertices of 3D points
   int iptsx[4];     //allows 4 viewplane x projections per facet
   int iptsy[4];     //allows 4 viewplane y projections per facet


            
   for (i = 0; i < mc->GetMeshnum(); i++)
      {
      if(!mc->GetOpacity(i)) continue;

              
                  //divide the color up into ambient and diffuse. Skip
                  //specular--I don't think you want it.
      col = mc->GetColor(i);
      for (j = 0; j < 3; j++)
         {
         ambient[j] = col[j]/5;
         diffuse[j] = col[j];
         }

               //PROCESS TRIANGLE FACETS
      fptr = (float*)mc->GetFirstTriangle(i, &facetsrfn, &srfn1, &srfn2, &srfn3); 
      onfacet = 0;
      numverticesperfacet = 3;
      
      while (fptr)
         {
  
                  //copy the point data, as you will need to change it
         for (k = 0; k < 3 * numverticesperfacet; k++)
            pts[k] = fptr[k];
         
            
                  //check if surfacenormal is away from you and continue
                  //if so. If the dot product of the viewvector and
                  //transformed normal is < 0, the face is pointing away.
                  //This means you only have to process about half the
                  //facets. Viewvector here is approximated as eye looking
                  //at one of the projected vertices.
 
      currentpos->TransformVec(3, facetsrfn.Get());
      costheta = Dot(vv, facetsrfn.Get());


      if (costheta < 0)
         {   
         fptr = (float*)mc->GetNextTriangle(i, &facetsrfn, &srfn1, &srfn2, &srfn3);
         ++onfacet;
         continue;
         }
          
      totalproj.TransformPt(3, pts);
      currentpos->TransformVec(3, srfn1.Get());
      currentpos->TransformVec(3, srfn2.Get());
      currentpos->TransformVec(3, srfn3.Get());

                     //Get the z position of this facet. The z position
                     //is assumed to be in the middle of the triangle
         float tt[3] = {fptr[0], fptr[1], fptr[2]};
         for (k = 1; k < numverticesperfacet; k++)
            {
            tt[0] +=fptr[3 * k];
            tt[1] += fptr[3 * k + 1];
            tt[2] += fptr[3 * k + 2];
            }
         tt[0] = tt[0]/numverticesperfacet; tt[1]= tt[1]/numverticesperfacet; tt[2] = tt[2]/numverticesperfacet;
         currentpos->TransformPt(3, tt);
         zpos = tt[2];
         

                        //Project the remaining vertex or vertices. You
                        //already projected the first one while
                        //checking surface normal conditions in order
                        //to eliminate drawing some surfaces.
         for (j = 1; j < numverticesperfacet; j++)
            totalproj.TransformPt(3, pts + 3 * j);

    
                  //Get the integer projection points
         for (j = 0; j < numverticesperfacet; j++)
            {
            iptsx[j] = Round(pts[3 * j]);
            iptsy[j] = Round(pts[3 * j + 1]);
            }
 
 
                  //check if projection is out of bounds
         dobreak = 0;
         for (k = 0; k < numverticesperfacet; k++)
            {
            if (iptsx[k] < 0 || iptsy[k] < 0 || iptsx[k] >= x || iptsy[k] >= y)
               {
               dobreak = 1; break;
               }
            }
         if(dobreak)
            {
            fptr = (float*)mc->GetNextTriangle(i, &facetsrfn, &srfn1, &srfn2, &srfn3);
            ++onfacet;
            continue;
            }
   
                     //Last param is current facet. You will need to query
                     //the mesh later to interpret in terms of skeleton points
         DrawFacetGouraud(ambient, diffuse, &srfn1, &srfn2, &srfn3, iptsx,\
            iptsy, circlezbuff, colorbuff, x,y,zpos,i + idoffset, onfacet);
 

         fptr = (float*)mc->GetNextTriangle(i, &facetsrfn, &srfn1, &srfn2, &srfn3);
         ++onfacet;
         }

               //PROCESS QUAD FACETS
      fptr = (float*)mc->GetFirstQuad(i, &facetsrfn, &srfn1, &srfn2, &srfn3, &srfn4); 
      numverticesperfacet = 4;
      
      while (fptr)
         {
  
                  //copy the point data, as you will need to change it
         for (k = 0; k < 3 * numverticesperfacet; k++)
            pts[k] = fptr[k];
         
            
                  //check if surfacenormal is away from you and continue
                  //if so. If the dot product of the viewvector and
                  //transformed normal is < 0, the face is pointing away.
                  //This means you only have to process about half the
                  //facets. Viewvector here is approximated as eye looking
                  //at one of the projected vertices.
      currentpos->TransformVec(3, facetsrfn.Get());

      costheta = Dot(vv, facetsrfn.Get());

      if (costheta < 0)
         {   
         fptr = (float*)mc->GetNextQuad(i, &facetsrfn, &srfn1, &srfn2, &srfn3, &srfn4);
         ++onfacet;
         continue;
         }

       totalproj.TransformPt(3, pts);
      
      currentpos->TransformVec(3, srfn1.Get());
      currentpos->TransformVec(3, srfn2.Get());
      currentpos->TransformVec(3, srfn3.Get());
      currentpos->TransformVec(3, srfn4.Get());   
      
                     //Get the z position of this facet. The z position
                     //is assumed to be in the middle of the triangle
         float tt[3] = {fptr[0], fptr[1], fptr[2]};
         for (k = 1; k < numverticesperfacet; k++)
            {
            tt[0] +=fptr[3 * k];
            tt[1] += fptr[3 * k + 1];
            tt[2] += fptr[3 * k + 2];
            }
         tt[0] = tt[0]/numverticesperfacet; tt[1]= tt[1]/numverticesperfacet; tt[2] = tt[2]/numverticesperfacet;
         currentpos->TransformPt(3, tt);
         zpos = tt[2];
         

                        //Project the remaining vertex or vertices. You
                        //already projected the first one while
                        //checking surface normal conditions in order
                        //to eliminate drawing some surfaces.
         for (j = 1; j < numverticesperfacet; j++)
            totalproj.TransformPt(3, pts + 3 * j);

    
                  //Get the integer projection points
         for (j = 0; j < numverticesperfacet; j++)
            {
            iptsx[j] = Round(pts[3 * j]);
            iptsy[j] = Round(pts[3 * j + 1]);
            }
 
 
                  //check if projection is out of bounds
         dobreak = 0;
         for (k = 0; k < numverticesperfacet; k++)
            {
            if (iptsx[k] < 0 || iptsy[k] < 0 || iptsx[k] >= x || iptsy[k] >= y)
               {
               dobreak = 1; break;
               }
            }
         if(dobreak)
            {
            fptr = (float*)mc->GetNextQuad(i, &facetsrfn, &srfn1, &srfn2, &srfn3, &srfn4);
            ++onfacet;
            continue;
            }
   
                     //Last param is current facet. You will need to query
                     //the mesh later to interpret in terms of skeleton points
         DrawFacetGouraud(ambient, diffuse, &srfn1, &srfn2, &srfn3, &srfn4, iptsx,\
            iptsy, circlezbuff, colorbuff, x,y,zpos,i + idoffset,onfacet);
 

         fptr = (float*)mc->GetNextQuad(i, &facetsrfn, &srfn1, &srfn2, &srfn3, &srfn4);
         ++onfacet;
         }


      }

   return 1;
   }
   //---------------------------------------------------------------
         //Triangle shading
void MESHRENDER::DrawFacetGouraud(float* ambient, float* diffuse,\
         FVEC* srfn1, FVEC* srfn2, FVEC* srfn3,int* iptsx, int* iptsy,\
         ZBUFF* circlebuff, unsigned char* colorbuff, int xdim,\
         int ydim, float zpos, int vessid, int ptid)
   {
  int miny, maxy, minx, maxx;
   int i, k, index;
   int xc, yc, xpos;
   int lastvertex, thisvertex;
   float slope;
   float lastzpos;
   FVEC minxcol;
   FVEC maxxcol;
   FVEC tempvec;
   float t;
   float deltax;
   float* fptr;



         //1) Get the color for each vertex
   FVEC vertexcols[3];
   FVEC vv(0,0,1);

   GetColorForVertex(ambient, diffuse, srfn1, &vv, vertexcols);
   GetColorForVertex(ambient, diffuse, srfn2, &vv, vertexcols + 1);
   GetColorForVertex(ambient, diffuse, srfn3, &vv, vertexcols + 2);
            
          //Get the min and max y values of the n vertices
   miny = maxy = iptsy[0];

   for (i = 1; i < numverticesperfacet; i++)
      {
      if (iptsy[i] < miny) miny = iptsy[i];
      else if (iptsy[i] > maxy) maxy = iptsy[i];
      }

   if (miny < 0) miny = 0;
   if (maxy >= ydim) maxy = ydim - 1;

               //begin scan conversion
   for (yc = miny; yc <= maxy; yc++)      //for yc covers y extent
      {
      minx = xdim;
      maxx = -1;
      lastvertex = numverticesperfacet -1;      //last legal vertex
      for (thisvertex = 0; thisvertex <numverticesperfacet; thisvertex++)
         {
         if (iptsy[lastvertex] >= yc || iptsy[thisvertex] >= yc)
            {
            if (iptsy[lastvertex] <= yc || iptsy[thisvertex] <= yc)
               {
               if (iptsy[lastvertex] != iptsy[thisvertex])
                  {
                           //process spatial information
                  deltax = (float)(iptsx[thisvertex] - iptsx[lastvertex]);
                  slope = deltax/(float)(iptsy[thisvertex] - iptsy[lastvertex]);
                  xpos = Round(slope * (float)((yc - iptsy[lastvertex])) + iptsx[lastvertex]);
 
                           //Working here
                           //Get color information for this spot on an edge
                  if (deltax)
                     t = (xpos - iptsx[lastvertex])/(deltax);
                  else 
                     {
                     deltax = (float)(iptsy[thisvertex] - iptsy[lastvertex]);
                     if (deltax)
                        t = ((float)yc - iptsy[lastvertex])/deltax;
                     else t = 0;
                     }

                  tempvec = vertexcols[lastvertex] + ((vertexcols[thisvertex] - vertexcols[lastvertex]) * t);
                  
                  if (xpos < minx) 
                     {
                     minx = xpos;
                     minxcol = tempvec;
                     }
                  if (xpos > maxx)
                     {
                     maxx = xpos;
                     maxxcol = tempvec;
                     }
                  }
               }
            }
         lastvertex = thisvertex;
         }

      if (minx < 0) minx = 0;
      if (maxx >= xdim) maxx = xdim - 1;

      if (minx <= maxx)
         {
         deltax = (float)(maxx - minx);
         for (xc = minx; xc <= maxx; xc++)
            {
                  //only write if this point is closer than one projecting
                  //to the same pixel
             index = yc * xdim + xc;
             lastzpos = circlebuff[index].GetZ();
             if(lastzpos && lastzpos >= zpos)
                continue;

             circlebuff[index].Set(vessid, ptid, zpos, 0); //no need for radius; = 0
            index = 3 * index;      //about to write to color triplet buffer

                  //debug: working here
            if (deltax)
               t = (xc - minx)/deltax;
            else
               t = 0;
            
            tempvec = minxcol + ((maxxcol - minxcol) * t);

            fptr = tempvec.Get();
            k = (int)(fptr[0] * 255); 
               if (k > 255) k = 255;
               else if (k < 0) k = 0;
               colorbuff[index] = (unsigned char)k;

            k = (int)(fptr[1] * 255);
               if (k > 255) k = 255;
               else if (k < 0) k = 0;
               colorbuff[index + 1] = (unsigned char)k;

            k = (int)(fptr[2] * 255);
               if (k > 255) k = 255;
               else if (k < 0) k = 0;
               colorbuff[index + 2] = (unsigned char)k;
            }
         }
      }
   } 
//---------------------------------------------------------------
         //Quad shading
void MESHRENDER::DrawFacetGouraud(float* ambient, float* diffuse,\
         FVEC* srfn1, FVEC* srfn2, FVEC* srfn3,FVEC* srfn4, int* iptsx, int* iptsy,\
         ZBUFF* circlebuff, unsigned char* colorbuff, int xdim,\
         int ydim, float zpos, int vessid, int ptid)
   {
  int miny, maxy, minx, maxx;
   int i, k, index;
   int xc, yc, xpos;
   int lastvertex, thisvertex;
   float slope;
   float lastzpos;
   FVEC minxcol;
   FVEC maxxcol;
   FVEC tempvec;
   float t;
   float deltax;
   float* fptr;



         //1) Get the color for each vertex
   FVEC vertexcols[4];
   FVEC vv(0,0,1);

   GetColorForVertex(ambient, diffuse, srfn1, &vv, vertexcols);
   GetColorForVertex(ambient, diffuse, srfn2, &vv, vertexcols + 1);
   GetColorForVertex(ambient, diffuse, srfn3, &vv, vertexcols + 2);
   GetColorForVertex(ambient, diffuse, srfn4, &vv, vertexcols + 3);
            
          //Get the min and max y values of the n vertices
   miny = maxy = iptsy[0];

   for (i = 1; i < numverticesperfacet; i++)
      {
      if (iptsy[i] < miny) miny = iptsy[i];
      else if (iptsy[i] > maxy) maxy = iptsy[i];
      }

   if (miny < 0) miny = 0;
   if (maxy >= ydim) maxy = ydim - 1;

               //begin scan conversion
   for (yc = miny; yc <= maxy; yc++)      //for yc covers y extent
      {
      minx = xdim;
      maxx = -1;
      lastvertex = numverticesperfacet -1;      //last legal vertex
      for (thisvertex = 0; thisvertex <numverticesperfacet; thisvertex++)
         {
         if (iptsy[lastvertex] >= yc || iptsy[thisvertex] >= yc)
            {
            if (iptsy[lastvertex] <= yc || iptsy[thisvertex] <= yc)
               {
               if (iptsy[lastvertex] != iptsy[thisvertex])
                  {
                           //process spatial information
                  deltax = (float)(iptsx[thisvertex] - iptsx[lastvertex]);
                  slope = deltax/(float)(iptsy[thisvertex] - iptsy[lastvertex]);
                  xpos = Round(slope * (float)((yc - iptsy[lastvertex])) + iptsx[lastvertex]);
 
                           
                           //Get color information for this spot on an edge
                  if (deltax)
                     t = (xpos - iptsx[lastvertex])/(deltax);
                  else 
                     {
                     deltax = (float)(iptsy[thisvertex] - iptsy[lastvertex]);
                     if (deltax)
                        t = ((float)yc - iptsy[lastvertex])/deltax;
                     else t = 0;
                     }

                  tempvec = vertexcols[lastvertex] + ((vertexcols[thisvertex] - vertexcols[lastvertex]) * t);
                  
                  if (xpos < minx) 
                     {
                     minx = xpos;
                     minxcol = tempvec;
                     }
                  if (xpos > maxx)
                     {
                     maxx = xpos;
                     maxxcol = tempvec;
                     }
                  }
               }
            }
         lastvertex = thisvertex;
         }

      if (minx < 0) minx = 0;
      if (maxx >= xdim) maxx = xdim - 1;

      if (minx <= maxx)
         {
         deltax = (float)(maxx - minx);
         for (xc = minx; xc <= maxx; xc++)
            {
                  //only write if this point is closer than one projecting
                  //to the same pixel
             index = yc * xdim + xc;
             lastzpos = circlebuff[index].GetZ();
             if(lastzpos && lastzpos >= zpos)
                continue;

             circlebuff[index].Set(vessid, ptid, zpos, 0); //no need for radius; = 0
            index = 3 * index;      //about to write to color triplet buffer

                  //debug: working here
            if (deltax)
               t = (xc - minx)/deltax;
            else
               t = 0;
            
            tempvec = minxcol + ((maxxcol - minxcol) * t);

            fptr = tempvec.Get();
            k = (int)(fptr[0] * 255); 
               if (k > 255) k = 255;
               else if (k < 0) k = 0;
               colorbuff[index] = (unsigned char)k;

            k = (int)(fptr[1] * 255);
               if (k > 255) k = 255;
               else if (k < 0) k = 0;
               colorbuff[index + 1] = (unsigned char)k;

            k = (int)(fptr[2] * 255);
               if (k > 255) k = 255;
               else if (k < 0) k = 0;
               colorbuff[index + 2] = (unsigned char)k;
            }
         }
      }
   } 
//---------------------------------------------------
void MESHRENDER:: GetColorForVertex(float* ambient, float* diffuse, FVEC* srfn, FVEC* vv, FVEC* col)
   {
   float costheta = vv->Vdot(*srfn);
   
   col->Set(ambient[0] + diffuse[0] * costheta, ambient[1] + diffuse[1] * costheta,\
      ambient[2] + diffuse[2] * costheta);
   }

}//end of namespace