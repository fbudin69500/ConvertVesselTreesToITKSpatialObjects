/*
Vessel collection class

  modified for namespace std 6/2002
*/

#ifndef _ebVESS_COL_s
#define _ebVESS_COL_s

#include "ebVESS_s.h"
#include "ebLIST.h"
#include "ebFILEHEADERIO.h"

#include "ebINCLUDES.h"      //define std namespace or not
//#include "MemoryLeak.h"
namespace eb
{

class VESSCOL_s:public FILEHEADERIO
   {
   public:
      VESSCOL_s();
      ~VESSCOL_s();
        VESSCOL_s(const VESSCOL_s& a);
        VESSCOL_s& operator = (const VESSCOL_s&a);

        char* Error()            {if (strlen(err)) return err; return 0;}
        char* GetError()         {return Error();}

        
                     //standard mem management
      void Releasemem();


                  //Get number of vessels and, if needed to do
                  //something special, the vessels themselves
      int GetMaxvess()            {return maxvess;}
      VESS_s* GetVess()            {return vess;}   
      VESS_s* GetVess(int i)          {return &vess[i];}


               //Spacing may be incorrect in some machines. Set correction factors.
               //This will apply to any points already loaded.
        void SetFixFactors(float a, float b, float c);

               //Get/set some vessel parameters. There is no error
               //checking here for speed, so be careful that i is less
               //than vessnum. Hide() sets opacity to 0, and this vessel
               //is then viewed as blocked.
        float* GetColor(int i)          {return vess[i].GetColor();}
        void SetColor(int i, float* a)         {vess[i].SetColor(a);}
        void SetColor(int i, float a, float b, float c)    
           {vess[i].SetColor(a,b,c);}
        float GetOpacity(int i)         {return vess[i].GetOpacity();}
        void SetOpacity (int i, float a) {vess[i].SetOpacity(a);}


                        //more intuitive calls to set opacity
      void Show(int i)                           {vess[i].Show();}
      void Hide (int i)                           {vess[i].Hide();}
        void Hide()                                 {for (int i=0;i<vessnum;i++) vess[i].Hide();}
      float IsVisible(int i)                  {return vess[i].IsVisible();}

                        //set the first and last allowed points
      int Proxoffset(int i)                  {return vess[i].Proxoffset();}
      int Distoffset(int i)                  {return vess[i].Distoffset();}
      void Proxoffset(int i,int a)             {vess[i].Proxoffset(a);}
      void Distoffset(int i, int a)             {vess[i].Distoffset(a);}
      void Resetoffset(int i)                   {vess[i].Resetoffset();}
      void ResetOffset(int i)                  {vess[i].Resetoffset();}
      void ResetAll()                           {for (int i = 0; i < vessnum; i++)
         {vess[i].Resetoffset(); vess[i].Show();} }


               //Each value in childlist consists of 2 ints: the first is the ID
               //of the child vessel and the second is its attachment point on the
               //parent. Note that the passed address of a in GetChild should be the
               //valid address of two ints, not just one. Caller must assign mem for a.
      void ResetChild(int i)               {vess[i].ResetChild();}
      int GetChild(int i, int* a)         {return vess[i].GetChild(a);}
        ARLIST<int>* GetChildList(int a)    {return vess[a].GetChildList();}
        void DeleteChildren(int a)            {vess[a].DeleteChildren();}
        void AddChild(int i, int* a)          {vess[i].AddChild(a[0], a[1]);}

               //Get parent and parental attachment point of a sent vessel
               //Returns 0 if no parent
      int IsRoot(int i)                     {return vess[i].IsRoot();}
      int VParent(int i, int* a)      {return vess[i].VParent(a);}
      void VParent(int i, int a)      {vess[i].VParent(a);}
      int AttachPt(int i, int* a)      {return vess[i].AttachPt(a);}
      void AttachPt(int i, int a)      {vess[i].AttachPt(a);}
      float* MyStart(int i)               {return vess[i].MyStart();} //return proxoffset
        void ShowChild(int a)           {vess[a].ShowChild();}                                                                                       //if artery, dist -1
                                                                                             //if vein

               //Get/set artery/vein status. The recursive functions
               //will assign all valid descendents the same status
      int IsArtery(int i)                  {return vess[i].IsArtery();}
      int IsVein(int i)                     {return vess[i].IsVein();}
      void Anat(int i, char* s)         {vess[i].Anat(s);}
      const char* Anat(int i)                     {return vess[i].Anat().c_str();}
      void MakeArtery(int i)            {vess[i].Anat("Artery");}
      void MakeVein(int i)               {vess[i].Anat("Vein");}
      void MakeUserAnat(int i, char* s)         {vess[i].Anat(s);}

      const char* TreeType(int a)               {return vess[a].TreeType().c_str();}
      void TreeType(int a, char* s)           {vess[a].TreeType(s);}
      const char* Type(int a)                       {return vess[a].TreeType().c_str();}
        void Type(int a, char* s)               {vess[a].TreeType(s);}



                  //return bounding box of a vessel. Min and max should
                  //be float[3] (mem is allocated by caller)
      int MinMaxBox(int i, float* min, float* max) 
         {return vess[i].MinMaxBox(min, max);}
        int MinMaxBox(float* min, float* max);
           



               //Get a point from an individual vessel. You can send 0 or
               //proxoffset to get the first point wanted. The first 4 values
               //will always be x,y,z,rad. If you use the pointer to get point
               //0 and want to manipulate the points in the caller, you also
               //need to know the pointdims of the vessel in question. A safer
               //call is to make individual queries for each point
      float* Point(int i, int a)      {return vess[i].Point(a);}
      int PointDim(int i)                  {return vess[i].PointDim();}
      int NPoints(int i)                  {return vess[i].NPoints();}

               //Get the point at which the vessel connects to parent. This will
               //be different for veins and arteries. End is the opposite end.
      float* GetStart(int i)            {return vess[i].MyStart();}
      float* GetEnd(int i)               {return vess[i].MyEnd();}

         
               //Get a sample of points from a vessel, either between
               //proxoffset and distoffset or for the whole vessel. Attachpt
               //is not included. The first and last points are always included.
               //Or get (for a later time) a sample already done. WARNING:
               //ALL SAMPLES RETURN 4D POINTS REGARDLESS OF POINTDIM.
               //DoSample will have the vessel store a set of points according to
               //nskip. The same sample can subsequently be retrieved by calling
               //GetSample. If you call GetSample first, you get a NULL pointer.
      float* DoSample(int i, int nskip, int* num)
         {return vess[i].DoSample(nskip, num);}
      float* DoAllowedSample(int i, int nskip, int* num)
         {return vess[i].DoAllowedSample(nskip, num);}
        void Sample(int nskip)      
           {int num; for (int i = 0; i <vessnum; i++)vess[i].DoSample(nskip, &num);}
      float* GetSample(int i, int* num)
         {return vess[i].GetSample(num);}
        float* DoConnectedSample(int i, int nskip, int* num);
        float* DoConnectedAllowedSample(int i, int nskip, int* num);
        int GetSamplestep(int a)      {return vess[a].GetSamplestep();}

               //Get the shortest distance between a 3D point and the 
               //vessel skeleton.
      double MinDistSkel(int i, float* pt, int* skelid)
         {return vess[i].MinDistSkel(pt, skelid);}

   
            //Get/set values from rootlist. As there are a small number of roots
            //and as this info is requested repeatedly, a separate list is kept
            //in this class and can be queried directly. 
      void ResetRoot()               {rootlist.Reset();}
      int GetRoot(int* i)            {return rootlist.Get(i);}
      void AddRoot(int i);
      void RemoveRoot(int i);
      int GetRoots(LIST<int>* lst);      
      int GetOrphans(LIST<int>* lst);
      int GetBlocked(LIST<int>* lst);
        LIST <int>* GetRootlist()          {return &rootlist;}


            //Get list of children from a sent vessel. The first function
            //will not include any children disallowed by proxoffset/
            //distoffset and will not include any vessel or descendents
            //if the vessel has opacity 0. The second function will return
            //all children. Neither function includes the first vessel sent.
            //Note that mem is not released from lst on entry as caller may want
            //several groups; it is up to caller to releasemem of lst
            //when wanted.
      int GetValidChildren(int i, LIST<int>* lst);
      int GetAllChildren(int i, LIST<int>* lst);

               //Get the floating point value of a vessel's attachment point.
               //Returns NULL if no attachment
      float* GetFloatAttachPt(int i);

               //Change attachments
      void OrphanizeAll();
      int Orphanize(int a);      //this will separate all children of a and remove parent
      int Detach(int a);         //this will only remove the parent-child connection
      int OrphanizeSubtree(int a);  //this will remove a as root and orphanize all descendents
      int ConnectChildToParent(int child, int childpos, int par, int parpos);
      int SplitVessel(int i, int pos);
      int Reverse(int a);

               //Get the center of mass of all points
      float* GetCenter();

               //Modify the vessel values in some way
      void Translate(float a, float b, float c)
         {for (int i = 0; i < vessnum; i++) vess[i].Translate(a,b,c);}
      void Mult(float a, float b, float c)
         {for (int i = 0; i < vessnum; i++) vess[i].Mult(a,b,c);}
        void Radmult(int i, float a)            {vess[i].Radmult(a);}
        void Radmult(float a)
           {for (int i = 0; i < vessnum; i++) vess[i].Radmult(a);}



               //Load/Save. The first load takes vessels in voxel
               //coordinates from a named file. The second load takes a set of
               //vessels in world coordinates. The third load takes a set of 
               //vessels in voxel coordinates. Everything is put in world coords
               //on load.
      int Load(char* fname);
      int LoadWorldTree(int n, VESS_s* v, int* vox, float* nsteps);
      int LoadVoxTree(int n, VESS_s* v, int* vox, float* nsteps);

        
                        //This group is used when creating reconstructed vessels that
                        //must be added to a vessel collection that may or may not be 
                        //empty. You are restricted to adding by the VESSCOL_sMAXINCREMENT
                        //number. Added vessels are set to root status unless attached to
                        //someone else so they will be saved later. The Load functions
                        //above will delete any previously entered vessel data. The Add
                        //functions below allow incremental addition. 
        void SetHeaderParams(int* _dims, float* _steps);
        int AddWorldVess(VESS_s* avess);
        int AddVoxVess(VESS_s* avess);
        int RemoveLastVess();

                  //save will not save anything of no opacity and will only save
                  //if connected ultimately to a root. It will also renumber
                  //all parent child connections to refer to the number
                  //of vessels actually used.
      int Save(char* fname);
      int SaveAll(char* fname);
      int SaveUnblocked(char* fname);
      int SaveBranchpointFile(char* fname);






        protected:
        char err[200];

      VESS_s* vess;
      int maxvess;

      LIST<int> rootlist;                     //list of roots
   

      int didcenter;
      float center[3];

                  //functions used to determine children
      int RecursiveChildAllowed(int a, LIST<int>* lst);
      int RecursiveChildDisallowed(int a, LIST<int>* lst);
      int RecursiveChildAll(int a, LIST<int>* lst);

      
                  //functions used during save. Renumbering is needed
                  //if distoffset has been changed or if vessels have been hidden
      int Recount(int oldval, LIST<int>* discards);
      void RenumberParent(VESS_s* v, LIST<int>* discards);
      void RenumberChildren(VESS_s* v, LIST<int>* discards);

                  //calc center of all vessels. Used for rotations
      void CalcCenter();

                  //split a child. The functions are different for arteries and
                  //veins as the direction of flow is different.
      int SplitChildArterial(int child, int childpos, int par, int parentpos);
      int SplitChildVenous(int child, int childpos, int par, int parentpos);

                        //file
      int LoadHeader(std::ifstream* ifile);

        
 

   };

}//end of namespace

#endif