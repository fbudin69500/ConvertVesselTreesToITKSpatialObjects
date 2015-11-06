#include "ebVESSCOL.h"

namespace eb
{

const int VESSCOLMAXINCREMENT = 2000;      //extra vessels for splitchild ops


   //&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
      //CONSTRUCTOR, DESTRUCTOR, MEM
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
VESSCOL::VESSCOL()
   {
   vessnum = 0;               //no vessels on entry
   maxvess = VESSCOLMAXINCREMENT;    //can add a limited number of vessels

   try
      {
      vess = new VESS[maxvess];         //set default to this limited number.
                                       //File load can set any arbitrary number.
      if (!vess)
         {
         maxvess = 0;
         std::cerr<<"No mem for vessel creation\n";
         strcpy(err, "No mem for vessel creation");
         }
      }
   catch(...)
      {
      maxvess = 0;
      std::cerr<<"No mem for vessel creation\n";
      strcpy(err, "No mem for vessel creation");
      }

         //default is to correct for bad MR spacing
   corfactor[0] = corfactor[1] = (float) 1; corfactor[2] = (float)1;
   didcenter = 0;
   center[0] = center[1] = center[2] = 0;
   strcpy(err, "");

   }
//----------------------------------------
VESSCOL::VESSCOL(const VESSCOL& a)
   {
   int i;

   for (i = 0; i < 3; i++)
      {
      dims[i] = a.dims[i];
      steps[i] = a.steps[i];
      corfactor[i] = a.corfactor[i];
      center[i] = a.center[i];
      }
   ndims = a.ndims;
   didcenter = a.didcenter;

   vessnum = a.vessnum;
   maxvess = a.maxvess;
   vess = 0;

         //I may have a bug here. Try to fix
   if (vessnum >= maxvess)
      maxvess = vessnum + VESSCOLMAXINCREMENT;

   try
      {
      vess = new VESS[maxvess];
      if (!vess)
         {
         vessnum = maxvess = 0;
         std::cerr<<"No mem for vessel creation\n";
         strcpy(err, "No mem for vessel creation");
         }
      }
   catch(...)
      {
      maxvess = vessnum = 0;
      std::cerr<<"No mem for vessel creation\n";       strcpy(err, "No mem for vessel creation");
      }

   if (vessnum > 0)
      {
      for (i = 0; i < vessnum; i++)
         vess[i] = a.vess[i];
      }
   rootlist = a.rootlist;
   }
//---------------------------------------------------------------
VESSCOL& VESSCOL::operator = (const VESSCOL& a)
   {
   if (&a == this) return *this;

   Releasemem();
   int i;

   for (i = 0; i < 3; i++)
      {
      dims[i] = a.dims[i];
      steps[i] = a.steps[i];
      corfactor[i] = a.corfactor[i];
      center[i] = a.center[i];
      }
   ndims = a.ndims;
   didcenter = a.didcenter;

   vessnum = a.vessnum;
   maxvess = a.maxvess;
   vess = 0;

         //I may have a bug here. Try to fix
   if (vessnum >= maxvess)
      maxvess = vessnum + VESSCOLMAXINCREMENT;

   try
      {
      vess = new VESS[maxvess];
      if (!vess)
         {
         maxvess = vessnum = 0;
         std::cerr<<"No mem for vessel creation\n";       strcpy(err, "No mem for vessel creation");
         }
      }
   catch(...)
      {
      vessnum = maxvess = 0;
      std::cerr<<"No mem for vessel creation\n";      strcpy(err, "No mem for vessel creation");
      }

   if (vessnum > 0)
      {
      for (i = 0; i < vessnum; i++)
         vess[i] = a.vess[i];
      }

   rootlist = a.rootlist;

   return *this;
   }


//----------------------------------------------------------------
VESSCOL::~VESSCOL()
   {Releasemem();}
//-----------------------------------------------------------------
void VESSCOL::Releasemem()
   {
   if (vess)
      {
      delete [] vess;
      vess = 0;
      }
    vessnum = 0;
    maxvess = VESSCOLMAXINCREMENT;

    rootlist.Releasemem();
    didcenter = 0;
   center[0] = center[1] = center[2] = 0;
    strcpy(err, "");
   }
//-----------------------------------------------------
void VESSCOL::SetFixFactors(float a, float b, float c)
   {

   if (a == 0 || b == 0 || c == 0) return;
   float tc[3] = {a,b,c};
   float* fptr;
   int num;

   int i,j;

   if (corfactor[0])  tc[0] = a/corfactor[0];
   if (corfactor[1])  tc[1] = b/corfactor[1];
   if (corfactor[2])  tc[2] = c/corfactor[2];

   for (i = 0; i < vessnum; i++)
      {
      vess[i].Mult(tc[0], tc[1], tc[2]);
      vess[i].Radmult(tc[0]);

            //do sample points also
      fptr = vess[i].GetSample(&num);
      for (j = 0; j < num; j++)
         {
         fptr[4 * j] = tc[0] * fptr[4 * j];
         fptr[4 * j + 1] = tc[1] * fptr[4 * j + 1];
         fptr[4 * j + 2] = tc[2] * fptr[4 * j + 2];
         fptr[4 * j + 3] = tc[0] * fptr[4 * j + 3];
         }

      }


   corfactor[0] = a;
   corfactor[1] = b;
   corfactor[2] = c;

   }

//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
      //PUBLIC: GET/MANIPULATE  LISTS OF IDS OF VESSEL TYPES
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
void VESSCOL::AddRoot(int i)
   {
   if (i < 0 || i >= vessnum) return;
   if (!rootlist.IsInList(i))
      rootlist.AddToTail(i);
   vess[i].TreeType("root");

            //remove any reference to parent and the parent's reference to child
            //A root does not have a parent.
   int par;
   if (vess[i].VParent(&par))
      vess[par].DeleteChild(i);
   vess[i].VParent(-1);
   vess[i].AttachPt(-1);
   }

//-----------------------------------------
         //This will remove a vessel from root status, but will not affect
         //any child connections previously made by this vessel. 
void VESSCOL::RemoveRoot(int i)
   {
   if (i < 0 || i >= vessnum) return;
   int par;

   rootlist.DeleteByValue(i);

   if(!vess[i].VParent(&par))
         vess[i].TreeType("Orphan");
   }
//----------------------------------------
int VESSCOL::GetRoots(LIST<int>* lst)
   {
   *lst = rootlist;
   int val, count = 0;
   lst->Reset();
   while (lst->Get(&val)) ++count;
   return count;
   }

int VESSCOL::GetOrphans(LIST<int>* lst)
{
   lst->Releasemem();
   int count = 0;

   for (int i = 0; i < vessnum; i++)
      {
      if (vess[i].IsRoot()) continue;
      if(vess[i].VParent()) continue;
      lst->AddToTail(i);
      ++count;
      }
   return count;
   }
//--------------------------------------

int VESSCOL::GetBlocked(LIST<int>* lst)
   {
   lst->Releasemem();
   int count = 0;
   for (int i = 0; i < vessnum; i++)
      {
      if (!vess[i].IsVisible())
         {
         lst->AddToTail(i);
         ++count;
         }
      }
   return count;
   }
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
      //PUBLIC: GET CHILDLISTS
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
int VESSCOL::GetValidChildren(int vessid, LIST<int>* lst)
   {
   if (vessid < 0 || vessid >= vessnum) return 0;

   if(!vess[vessid].IsVisible())
      return 0;
   
   int count = 0;
   count = RecursiveChildAllowed(vessid, lst);
   return count;
   }

   

//------------------------------
   int VESSCOL::GetAllChildren (int vessid, LIST<int>* lst)
   {
   if (vessid < 0 || vessid >= vessnum) return 0;

   int count = 0;
   count = RecursiveChildAll(vessid, lst);
   return count;
   }
//-----------------------------------
   float* VESSCOL::GetFloatAttachPt(int i)
   {
   if (i < 0 || i >= vessnum) return 0;

   int par, attach;
   if (!vess[i].VParent(&par)) return 0;
   if (!vess[i].AttachPt(&attach)) return 0;

   return vess[par].Point(attach);
   }

//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
         //PUBLIC: CHANGE ATTACHMENTS
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
         //These will not change artery/vein status
void VESSCOL::OrphanizeAll()
   {
   int i;

   for (i = 0; i < vessnum; i++)
      {
      vess[i].DeleteChildren();
      vess[i].VParent(-1);
      vess[i].AttachPt(-1);
      vess[i].TreeType("Orphan");
      }
   rootlist.Releasemem();
   }
//-----------------------------------
      //This will detach a from its parent. All other connections remain
int VESSCOL::Detach(int a)
   {
   int par;

   if (a < 0 || a >= vessnum)
      return 0;

         //deal with a's parent
   if (vess[a].VParent(&par))
      vess[par].DeleteChild(a);
   vess[a].VParent(-1);
   vess[a].AttachPt(-1);
   vess[a].TreeType("Orphan");

   rootlist.DeleteByValue(a);
   return 1;
   }
//-----------------------------------------------------------

         //This function will disconnect all children from a, but
         //will leave deeper connections intact. 
int VESSCOL::Orphanize(int a)
   {
   int par, child;

   if (a < 0 || a >= vessnum)
      return 0;

            //deal with a's first generation children
   vess[a].ResetChild();
   while (vess[a].GetChild(&child))
      {
      vess[child].VParent(-1);
      vess[child].AttachPt(-1);
      vess[child].TreeType("Orphan");
      }
   vess[a].DeleteChildren();

         //deal with a's parent
   if (vess[a].VParent(&par))
      vess[par].DeleteChild(a);
   vess[a].VParent(-1);
   vess[a].AttachPt(-1);
   vess[a].TreeType("Orphan");

   rootlist.DeleteByValue(a);
   return 1;
   }
//------------------------------------
int VESSCOL::OrphanizeSubtree(int a)
   {
   if (a < 0 || a >= vessnum)
      return 0;

   LIST<int> lst;
   int child, par;

   GetAllChildren(a, &lst);
   lst.AddToTail(a);

            //deal with a's parent
   if (vess[a].VParent(&par))
      vess[par].DeleteChild(a);
   
   lst.Reset();
   while (lst.Get(&child))
      {
      vess[child].DeleteChildren();
      vess[child].VParent(-1);
      vess[child].AttachPt(-1);
      vess[child].TreeType("orphan");
      }
   rootlist.DeleteByValue(a);
   return 1;
   }
//-----------------------------------
      //This function has been modified to allow for veins, which attach
      //at the tail.
int VESSCOL::ConnectChildToParent(int child, int childpos, int par, int parpos)
   {
   LIST<int> lst;
   int val;
   int returnval = 1;

   if (child < 0 || child >= vessnum)
      {
      std::cerr<<"Invalid child\n";
        strcpy(err, "Invalid child");
      return 0;
      }
   if (par < 0 || par >= vessnum)
      {
      std::cerr<<"Invalid parent\n";
        strcpy(err, "Invalid parent");
      return 0;
      }
   if (childpos < 0 || childpos >= vess[child].NPoints())
      {
      std::cerr<<"Invalid child position\n";
        strcpy(err, "Invalid child position");
      return 0;
      }
   if (parpos < 0 || parpos >=vess[par].NPoints())
      {
      std::cerr<<"Invalid parent position\n";
        strcpy(err, "Invalid parent position");
      return 0;
      }
   if (par == child)
      {
      std::cerr<<"Child cannot be its own parent\n";
        strcpy(err, "Child cannot be its own parent");
      return 0;
      }
               //Check for an invalid loop
   GetAllChildren(child, &lst);
   lst.Reset();

   while (lst.Get(&val))
      {
      if (val == par)
         {
         std::cerr<<"Creates an invalid loop!\n Give par another parent first\n.";
            strcpy(err, "Creates an invalid loop");
         return 0;
         }
      }

            //remove old parent references and give a new one
   if (vess[child].VParent(&val))
         vess[val].DeleteChild(child);
   vess[child].VParent(par);
   vess[child].AttachPt(parpos);
   vess[child].TreeType("child");

            //if the childpos is the head and par is not vein, just connect
   if (childpos == 0 && (!vess[par].IsVein()))
      vess[par].AddChild(child, parpos);

            //if the connection is the tail and the par is not vein, reverse the vessel
   else if (childpos == vess[child].NPoints() -1)
      {
      if(!vess[par].IsVein())
         Reverse(child);
      vess[par].AddChild(child, parpos);
      }

               //otherwise you must split the child
   else
      {
      if (vess[par].IsVein())
         returnval = SplitChildVenous(child, childpos, par, parpos);
      else
         returnval = SplitChildArterial(child, childpos, par, parpos);
      }

   return returnval;
   }
   //------------------------------------------------------------------
         //Modified to allow for veins. If the vessel is a vein, flow
         //direction will be toward the
int VESSCOL::SplitVessel(int i, int pos)
   {
   if (i < 0 || i >= vessnum)
      {
      std::cerr<<"Invalid onpick in VESSCOL::SplitVessel()\n";
        strcpy(err, "Invalid onpick in SplitVessel");
      return 0;
      }

   if (pos <= 0 || pos >= vess[i].NPoints() - 1)
      {
      std::cerr<<"Invalid position in VESSCOL::SplitVessel()\n";
        strcpy(err, "Invalid position in SplitVessel()");
      return 0;
      }

   if ( (vessnum + 1) >= maxvess)
      {
      std::cerr<<"Insufficient extra vessels to make a new one in VESSCOL\n";
        strcpy(err, "Insufficient extra vessels");
      return 0;
      }

   VESS prox, dist;

            //set what you can here. 
   int val;
   float* fptr;
   float* fptr2;
   int twoi[2];

         //set header info
   prox.Type(vess[i].Type());
   dist.Type(vess[i].Type());

   prox.Anat(vess[i].Anat());
   dist.Anat(vess[i].Anat());

   prox.TreeType(vess[i].TreeType());
   dist.TreeType("child");

   if (vess[i].VParent(&val))
      {
      prox.VParent(val);
      if (vess[i].AttachPt(&val))
         prox.AttachPt(val);
      }

   dist.VParent(i);
   if (vess[i].IsVein())
      dist.AttachPt(0);
   else dist.AttachPt(pos - 1);

   prox.Name(vess[i].Name());

   if (vess[i].IsVein())
      {
      dist.Realloc(pos, vess[i].PointDim());
      prox.Realloc(vess[i].NPoints() - pos, vess[i].PointDim());
      }
   else
      {
      prox.Realloc(pos, vess[i].PointDim());
      dist.Realloc(vess[i].NPoints() - pos, vess[i].PointDim());
      }

      prox.SetColor(vess[i].GetColor());
      dist.SetColor(vess[i].GetColor());

   for (val = 0; val < vess[i].PointDim(); val ++)
      {
      prox.DimDesc(val, vess[i].DimDesc(val));
      dist.DimDesc(val, vess[i].DimDesc(val));
      }

            //jlists not included here; may need to come bacl

   if(vess[i].IsVein())
      {
      fptr = vess[i].Point(0);
      fptr2 = dist.Point(0);
      memcpy(fptr2, fptr, (vess[i].NPoints() - pos) * vess[i].PointDim() * sizeof(float));
      fptr = vess[i].Point(pos);
      fptr2 = prox.Point(0);
      }
   else
      {
               //set points
      fptr = vess[i].Point(0);
      fptr2 = prox.Point(0);
      memcpy(fptr2, fptr, (vess[i].NPoints() - pos) * vess[i].PointDim() * sizeof(float));

      fptr = vess[i].Point(pos);
      fptr2 = dist.Point(0);
      memcpy(fptr2, fptr, (pos * vess[i].PointDim() * sizeof(float)));
      }

         //deal with childlists. Prox will be defined as vess[i]. Dist will be
         //a new vessel
   vess[i].ResetChild();
   while (vess[i].GetChild(twoi))
      {
      if (twoi[i] >= pos) 
         break;
      if (vess[i].IsVein())
         dist.AddChild(twoi[0], twoi[1]);
      else
         prox.AddChild(twoi[0], twoi[1]);
      }

   if (vess[i].IsVein())
      prox.AddChild(vessnum, 0);
   else
      prox.AddChild(vessnum, pos - 1);      //dist is child of prox--id will be vessnum
   vess[vessnum].TreeType("Child");

   vess[i].ResetChild();
   while (vess[i].GetChild(twoi))
      {
      if (twoi[1] < pos) continue;
      vess[twoi[0]].VParent(vessnum);
      vess[twoi[0]].AttachPt(twoi[1] - pos);

      if(vess[i].IsVein())
         prox.AddChild(twoi[0], twoi[1] - pos);
      else
         dist.AddChild(twoi[0], twoi[1] - pos);
      }
            //copy the new vessels into the vess array
   prox.Resetoffset();
   dist.Resetoffset();
   vess[i] = prox;
   vess[vessnum] = dist;
   ++vessnum;
  return 1;
   }
   //-------------------------------
         //Reverse a vessel. Keep the original attachment to parent, which
         //now is likely to be a line shooting across space and will need to
         //be reset--however you can see it.
         //An individual vessel can reverse itself and adjust its own childlist.
         //However, you must externally change the connectpts of each child
int VESSCOL::Reverse(int a)
   {
   if (a >= vessnum || a < 0) {std::cerr<<"Invalid id\n"; strcpy(err,  "Invalid id");return 0;}
   vess[a].Reverse();

   int vals[2];
   vess[a].ResetChild();

               //reverse will switch the attachment points for the parent vessel,
               //but you must tell each child vessel what its new attachpt is.
   while (vess[a].GetChild(vals))
      vess[vals[0]].AttachPt(vals[1]);

   return 1;
   }
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
      //PUBLIC: GET CENTER
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&
float* VESSCOL::GetCenter()
   {
   if (!didcenter)
      {
      CalcCenter();
      didcenter = 1;
      }
   return center;
   }
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
      //RETURN BOUNDING BOX OF ALL VESSELS
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
int VESSCOL::MinMaxBox(float* min, float* max)
   {
   if (!vessnum) return 0;
   float tm[3]; float tM[3];
   int i, count = 0;

  for (i = 0; i < 3; i++)
     {
     max[i] = 0;
     min[i] = 9999;
     }

   for (i = 0; i < vessnum; i++)
      {
      if(!vess[i].IsVisible()) continue;
      count += MinMaxBox(i, tm, tM);
      if (max[0] < tM[0]) max[0] = tM[0];
      if (min[0] > tm[0]) min[0] = tm[0];
      if (max[1] < tM[1]) max[1] = tM[1];
      if (min[1] > tm[1]) min[1] = tm[1];
      if (max[2] < tM[2]) max[2] = tM[2];
      if (min[2] > tm[2]) min[2] = tm[2];
      }
    return count;
    }
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
      //SPECIALIZED SAMPLING
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
float* VESSCOL::DoConnectedSample(int i, int nskip, int* num)
   {
   float* fptr = GetFloatAttachPt(i);

    return vess[i].DoConnectedSample(nskip, num, fptr);
   }
//----------------------------------------------------
float* VESSCOL::DoConnectedAllowedSample(int i, int nskip, int* num)
   {
   float* fptr = GetFloatAttachPt(i);
   return vess[i].DoConnectedAllowedSample(nskip, num, fptr);
   }

//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
      //LOAD
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
int VESSCOL::Load(const char* fname)
   {
   Releasemem();

   int returnval = 1;
   int i, val, pos;
   std::ifstream ifile;


   ifile.open(fname);
   if(!ifile)
      {
      std::cerr<<"VESSCOL::Load--Unable to open input file\n"; 
      strcpy(err, "Unable to open input file");
      return 0;
      }
      

   if(!LoadHeader(&ifile)) 
      {ifile.close(); std::cerr<<"Could not load header"; strcpy(err, "could not load header");return 0;}

   for (i = 0; i < vessnum; i++)
      {
      if(!vess[i].Load(&ifile))
         {
         char s[100];
         sprintf(s, "LOAD FAILED on vessel %i of %i", i + 1, vessnum);
         std::cerr<<s;
         strcpy(err, s);
         returnval = 0;
         break;
         }
      }

   ifile.close();
      
   
                  //Now process the loaded vessels
   if(!returnval) return 0;


      for (i = 0; i < vessnum; i++)
      {
      vess[i].Distoffset(vess[i].NPoints());
//      vess[i].Show();
        vess[i].ID(i);

               //this is to fix a bug: the attach functions currently do not change
               //TreeType unless the vessel is known as root
      if (vess[i].VParent(&val))
         vess[i].TreeType("child");
   
      else if (vess[i].IsRoot())
         rootlist.AddToTail(i);
      
               //convert to world coordinates
        
      vess[i].VoxToWorld(steps[0], steps[1], steps[2]);
      }

  
               //give a childlist to each parent
   for (i = 0; i < vessnum; i++)
      {
      if (!vess[i].VParent(&val))
         continue;
   
      vess[i].AttachPt(&pos);
      vess[val].AddChild(i, pos);
      }

   return returnval;
   }
//---------------------------------------------
int VESSCOL::LoadWorldTree(int n, VESS* v, int* vox, float* nsteps)
   {
   Releasemem();

   if (n <= 0 || v == NULL || vox ==NULL || steps == NULL)
      return 0;

   steps[0] = nsteps[0];
   steps[1] = nsteps[1];
   steps[2] = nsteps[2];

   dims[0] = vox[0];
   dims[1] = vox[1];
   dims[2] = vox[2];

   if (steps[0] <= 0 || steps[1] <= 0 || steps[2]<= 0) return 0;
   if (dims[0] <=0 || dims[1] <= 0 || dims[2] <= 0) return 0;

   vessnum = n;
   maxvess = vessnum + VESSCOLMAXINCREMENT;

   try
      {
      vess = new VESS[maxvess];
      if (!vess)
         {
         vessnum = maxvess = 0;
         std::cerr<<"No mem for vessel creation\n";       strcpy(err, "No mem for vessel creation");
         }
      }
   catch(...)
      {
      vessnum = maxvess = 0;
      std::cerr<<"No mem for vessel creation\n";       strcpy(err, "No mem for vessel creation");
      }


   for (int i = 0; i < vessnum; i++)
      {
      vess[i] = v[i];
      if (vess[i].IsRoot())
         rootlist.AddToTail(i);
      }
   return 1;
   }
//------------------------------------
int VESSCOL::LoadVoxTree(int n, VESS* v, int* vox, float* nsteps)
   {
   Releasemem();

   if (n <= 0 || v == NULL || vox ==NULL || steps == NULL)
      return 0;

   steps[0] = nsteps[0];
   steps[1] = nsteps[1];
   steps[2] = nsteps[2];

   dims[0] = vox[0];
   dims[1] = vox[1];
   dims[2] = vox[2];

   if (steps[0] <= 0 || steps[1] <= 0 || steps[2]<= 0) return 0;
   if (dims[0] <=0 || dims[1] <= 0 || dims[2] <= 0) return 0;


   vessnum = n;
   maxvess = vessnum + VESSCOLMAXINCREMENT;
   try
      {
      vess = new VESS[maxvess];
      if (!vess)
         {
         vessnum = maxvess = 0;
         std::cerr<<"No mem for vessel creation\n";       strcpy(err, "No mem for vessel creation");
         }
      }
   catch(...)
      {
      vessnum = maxvess = 0;
      std::cerr<<"No mem for vessel creation\n";       strcpy(err, "No mem for vessel creation");
      }


   for (int i = 0; i < vessnum; i++)
      {
      vess[i] = v[i];
      if (vess[i].IsRoot())
         rootlist.AddToTail(i);
      vess[i].VoxToWorld(steps[0], steps[1], steps[2]);
      }
   return 1;
   }
//---------------------------------------------
void VESSCOL::SetHeaderParams(int* _dims, float* _steps)
   {
   for (int i = 0; i < 3; i++)
      {
      dims[i] = _dims[i];
      steps[i] = _steps[i];
      }
   }
//------------------------------------------------------
int VESSCOL::AddWorldVess(VESS* avess)
   {
   int par;

   if (vessnum >= maxvess - 1)
      {
      strcpy(err, "Vessel maximum exceeded. Cannot add");
      std::cerr<<"Vessel maximum exceeded. Cannot add\n";
      return 0;
      }

            //In rare cases you might call Releasemem(), expecting to use the extra vessels only.
   if(!vess)
      {
      try
         {
         vess = new VESS[maxvess];
         if(!vess) {maxvess = 0; return 0;}
         }
      catch(...)
         {maxvess = 0;}
      }


   ++vessnum;

   vess[vessnum - 1] = *avess;

   if(!vess[vessnum - 1].VParent(&par))
      AddRoot(vessnum - 1);

   return 1;
   }
//-------------------------------------------------------
int VESSCOL::AddVoxVess(VESS* avess)
   {
   int returnval = AddWorldVess(avess);
   if(returnval)
      vess[vessnum - 1].VoxToWorld(steps[0], steps[1], steps[2]);

   return returnval;
   }
//-----------------------------------------
int VESSCOL::RemoveLastVess()
   {
   if(!vessnum) return 0;

   int par;
   int twoi[2];

   if(vess[vessnum - 1].VParent(&par))
      vess[par].DeleteChild(vessnum - 1);

   vess[vessnum - 1].ResetChild();
   while (vess[vessnum - 1].GetChild(twoi))
      {
      vess[twoi[0]].VParent(-1);
      vess[twoi[0]].TreeType("orphan");
      }
   rootlist.DeleteByValue(vessnum - 1);

   vess[vessnum - 1].Releasemem();
   --vessnum;

   return 1;
   }

//-----------------------------------------
int VESSCOL::Save(char* fname)
   {
   if (!fname)
      {
      std::cerr<<"bad file name\n";
        strcpy(err, "bad file name");
      return 0;
      }

   int root, onvess, i;
   LIST<int> lst;
   LIST<int> discards;
   int count = 0;
   VESS* savevess;

            //get a count of the number of vessels
   rootlist.Reset();
   while (rootlist.Get(&root))
      {
      if (!vess[root].IsVisible())
         continue;
      ++count;               //root will be added
      lst.AddToTail(root);
      count +=GetValidChildren(root, &lst);
      }

   if (!count)
      {
      std::cerr<<"No vessels picked for save\n";
        strcpy(err, "No vessels picked for save");
      return 0;
      }

         //you will need to renumber parents and children. Make a copy
      //of the vessels you want
   if (!count) return 0;

   try
      {
      savevess = new VESS[count];
      if(!savevess)
         {
         std::cerr<<"Out of mem; cannot save\n";
         strcpy(err, "Out of mem; cannot save");
         return 0;
         }
      }
   catch(...)
      {
      std::cerr<<"Out of mem; cannot save\n";
      strcpy(err, "Out of mem; cannot save");
      return 0;
      }

                 //create a new, ordered sequence of the vessels to be saved
  onvess = 0;
  for (i = 0; i < vessnum; i++)
    {
    if (!lst.IsInList(i))
      {discards.AddToTail(i); continue;}
    savevess[onvess] = vess[i];
    ++onvess;
    }

                  //renumber the parents and children and convert to voxel
                  //coords
   for (i = 0; i <count; i++)
      {
      RenumberParent(&savevess[i], &discards);
      RenumberChildren(&savevess[i], &discards);
     savevess[i].WorldToVox(steps[0], steps[1], steps[2]);
     savevess[i].ID(i);
     savevess[i].Type("Tube");
                     //this is to fix a bug: the attach functions currently do not change
               //TreeType unless the vessel is known as root
     if (savevess[i].VParent())
       savevess[i].TreeType("child");

            //if the parent has proxoffset set, you need to change the attachpoint of each child
     int a, prox, apt;
     if (savevess[i].VParent(&a))
        {
        prox = savevess[a].Proxoffset();
        savevess[i].AttachPt(&apt);
        apt -= prox;
        savevess[i].AttachPt(apt);
        }
        

     }

   std::ofstream ofile;
    ofile.open(fname);
    if (!ofile)
       {
       std::cerr<<"Could not open output file\n"; 
       strcpy(err, "could not open output file");
       delete [] savevess; 
       return 0;
       }

               //Write the header
    int holdvessnum = vessnum;
    vessnum = count;
    if (!WriteHeader(&ofile)) {vessnum = holdvessnum; return 0;}
    vessnum = holdvessnum;

               //Write the vessels
    for (i = 0; i < count; i++)
       savevess[i].Save(&ofile);

    ofile.close();

    delete[]savevess;
   return 1;
   }
//-------------------------------------------------------
         //Save all regardless of attachment statys
int VESSCOL::SaveAll(char* fname)
   {
       std::ofstream ofile;
    ofile.open(fname);
    if (!ofile)
       {std::cerr<<"Could not open output file\n"; strcpy(err, "Could not open output file");return 0;}
    VESS tempvess;

    WriteHeader(&ofile);
    for (int i = 0; i < vessnum; i++)
       {
       tempvess = vess[i];
       
       tempvess.WorldToVox(steps[0], steps[1], steps[2]);
       tempvess.Save(&ofile);
       }

    ofile.close();
    return 1;
   }
//--------------------------------------------------------
int VESSCOL::SaveUnblocked(char* fname)
   {
   if (!fname)
      {
      std::cerr<<"bad file name\n";
        strcpy(err, "bad file name");
      return 0;
      }

   int root, onvess, i;
   LIST<int> lst;
   LIST<int> discards;
   int count = 0;
   VESS* savevess;

            //get a count of the number of vessels. Take all valid vessels first. You will exclude
            //children of a blocked parent.
   rootlist.Reset();
   while (rootlist.Get(&root))
      {
      if (!vess[root].IsVisible())
         continue;
      ++count;               //root will be added
      lst.AddToTail(root);
      count +=GetValidChildren(root, &lst);
      }


         //add to this list any orphan who is not blocked. Simplest way is to make it a root.
   for (i = 0; i < vessnum; i++)
      {
      if (vess[i].IsOrphan() && vess[i].IsVisible())
         {
         ++count;
         lst.AddToTail(i);
         vess[i].TreeType("Root");
         }
      }

   if (!count)
      {
      std::cerr<<"No vessels picked for save\n";
        strcpy(err, "No vessels picked for save");
      return 0;
      }

         //you will need to renumber parents and children. Make a copy
      //of the vessels you want
   if (!count) return 0;

   try
      {
      savevess = new VESS[count];
      if(!savevess)
         {
         std::cerr<<"Out of mem; cannot save\n";
         strcpy(err, "Out of mem; cannot save");
         return 0;
         }
      }
   catch(...)
      {
      std::cerr<<"Out of mem; cannot save\n";
      strcpy(err, "Out of mem; cannot save");
      return 0;
      }

                 //create a new, ordered sequence of the vessels to be saved
  onvess = 0;
  for (i = 0; i < vessnum; i++)
    {
    if (!lst.IsInList(i))
      {discards.AddToTail(i); continue;}
    savevess[onvess] = vess[i];
    ++onvess;
    }

                  //renumber the parents and children and convert to voxel
                  //coords
   for (i = 0; i <count; i++)
      {
      RenumberParent(&savevess[i], &discards);
      RenumberChildren(&savevess[i], &discards);
     savevess[i].WorldToVox(steps[0], steps[1], steps[2]);
     savevess[i].ID(i);
     savevess[i].Type("Tube");
                     //this is to fix a bug: the attach functions currently do not change
               //TreeType unless the vessel is known as root
     if (savevess[i].VParent())
       savevess[i].TreeType("child");
       }

   std::ofstream ofile;
    ofile.open(fname);
    if (!ofile)
       {
       std::cerr<<"Could not open output file\n"; 
       strcpy(err, "could not open output file");
       delete [] savevess; 
       return 0;
       }

               //Write the header
    int holdvessnum = vessnum;
    vessnum = count;
    if (!WriteHeader(&ofile)) {vessnum = holdvessnum; return 0;}
    vessnum = holdvessnum;

               //Write the vessels
    for (i = 0; i < count; i++)
       savevess[i].Save(&ofile);

    ofile.close();

    delete[]savevess;
   return 1;
   }
//---------------------------------------------------------
int VESSCOL::SaveBranchpointFile(char* fname)
   {
   if (!fname)
      {
      std::cerr<<"bad file name\n";
        strcpy(err, "Bad file name");
      return 0;
      }

   int i, par;
   float pt[3];
   float* fptr;

   std::ofstream ofile;
   ofile.open(fname);
   ofile<<"Save Branchpoints in voxel coordinates\n";

    WriteHeader(&ofile);


   for (i = 0; i < vessnum; i++)
      {
      if (vess[i].VParent(&par))
         {
         fptr = GetFloatAttachPt(i);
         pt[0] = fptr[0]/steps[0];
         pt[1] = fptr[1]/steps[1];
         pt[2] = fptr[2]/steps[2];
         ofile<<"PARENT: "<<par<<"\tCHILD: "<<i<<"\t"<<pt[0]<<"  "<<pt[1]<<"  "<<pt[2]<<std::endl;
         }
      }
   ofile.close();
   return 1;
   }

//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
   //PROTECTED::LOAD
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
int VESSCOL::LoadHeader(std::ifstream* ifile)
   {
   int returnval = FILEHEADERIO::LoadHeader(ifile);

  

    if (ndims != 3)
       {
       std::cerr<<"File must have 3 dimensions\n";
       strcpy(err, "File must have 3 dimensions");
       returnval = 0;
       }

    if(returnval)
       {
         maxvess = vessnum + VESSCOLMAXINCREMENT;

         try
            {
            vess = new VESS[maxvess];
            if(!vess)
               {
               std::cerr<<"No mem for vessels\n";
               strcpy(err, "No mem for vessels");
               vessnum = 0;
               return 0;
               }
            }
         catch(...)
            {
            std::cerr<<"no mem for vessels\n";
            strcpy(err, "No mem for vessels");
            vessnum = maxvess = 0;
            return 0;
            }
       }

   return returnval;
   }

//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
   //PROTECTED FUNCTIONS DEALING WITH GETTING A CHILDLIST
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
         //return a childlist that contains only children within
         //proxoffset-distoffset of parent, and that excludes any
         //consideration of vessels and subsequent children if the
         //parent opacity is 0
int VESSCOL::RecursiveChildAllowed(int a, LIST<int>* lst)
   {
   int count = 0;
   int childvals[2];
   int prox, dist;

   if (a < 0 || a >= vessnum) return 0;

         //Discard this vessel if it has opacity 0
   if (!vess[a].IsVisible()) return 0;
   
            //Get the proximal and distal offset of this vessel
   prox = vess[a].Proxoffset();
   dist = vess[a].Distoffset();


            //Get allowed children, adding to lst
   vess[a].ResetChild();
   while (vess[a].GetChild(childvals))
      {
      if (childvals[1] < prox) continue;      //disallowed proximally
      if (childvals[1] >= dist) continue;      //disallowed distally

      if (!vess[childvals[0]].IsVisible()) continue;   //child has no opacity
      lst->AddToTail(childvals[0]);
      count += 1;
      count +=RecursiveChildAllowed(childvals[0], lst);
      }
   return count;
   }

//---------------------------------------
         //Return all children regardless of validity. Useful when
         //resetting a subtree to something
int VESSCOL::RecursiveChildAll(int a, LIST<int>* lst)
   {
   int count = 0;
   int childvals[2];

   if (a < 0 || a >= vessnum) return 0;

   vess[a].ResetChild();
   while (vess[a].GetChild(childvals))
      {
      ++count;
      lst->AddToTail(childvals[0]);
      count += RecursiveChildAll(childvals[0], lst);
      }
   return count;
   }
//--------------------------
void VESSCOL::RenumberParent(VESS* v, LIST<int>* discards)
   {
   int oldparent, newparent;

   if (v->VParent(&oldparent))
      {
      newparent = Recount(oldparent, discards);
      v->VParent(newparent);
      }
   }
//-----------------------------
void VESSCOL::RenumberChildren(VESS* v, LIST<int>* discards)
   {
   int childinfo[2];
   ARLIST<int> finalchild(2);

  v->ResetChild();                                 //reset both childlist and position list.
  while(v->GetChild(childinfo))         //for each child and associated pos
    {
    childinfo[0] = Recount(childinfo[0], discards);
    if (childinfo[0]> -1)                          //if the renumbered child is included
      finalchild.AddToTail(childinfo);
    }

   
            //delete the initial child/offset lists and add the new ones/
  v->DeleteChildren();
  finalchild.Reset();
  while (finalchild.Get(childinfo))
      {
    v->AddChild(childinfo[0], childinfo[1]);
      vess[childinfo[0]].TreeType("child");
      }

   }

//-----------------------------
int VESSCOL::Recount(int startval, LIST<int>* excludes)
   {

            //the value -1 is a signal for no parent. If this value
            //is sent, give the same value back.
  if (startval == -1)
    return startval;

            //if startval is itself in the exclude list, it should
            //be excluded; the signal for a non-valid number is -1
  if (excludes->IsInList(startval))
    return -1;

            //startval is not in the exclude list. However, it is an
            //index into an array, and some array values prior to it
            //may have been excluded. You must renumber startval so
            //as to account for these excludes.
  int newval = 0;     //0 is the first valid id number
  int i;

  for (i = 0; i < startval; i++)
    {
    if (excludes->IsInList(i))
      continue;
    ++newval;
    }
  return newval;
  }
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
      //PROTECTED FUNCTIONS DEALING WITH CENTER/SPLITS/MISCELLANEOUS
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
void VESSCOL::CalcCenter()
   {
   center[0] = center[1] = center[2] = 0;

   if(!vessnum)
      {
      didcenter = 0;
      return;
      }

   int i, j, counter = 0;
   float* fptr;

   for(i = 0; i < vessnum; i++)
      {
        if(!vess[i].IsVisible()) continue;

      for (j = 0; j < vess[i].NPoints(); j++)
         {
         fptr = vess[i].Point(j);
         center[0] += fptr[0];
         center[1] += fptr[1];
         center[2] += fptr[2];
         ++counter;
         }
      }
   
   if (counter == 0)
      {
      didcenter = 0;
      return;
      }

   center[0] = center[0]/counter;
   center[1] = center[1]/counter;
   center[2] = center[2]/counter;

   //std::cerr<<"center is "<<center[0]<<"  "<<center[1]<<"  "<<center[2]<<std::endl;
   didcenter = 1;
   }
//---------------------------------------------
int VESSCOL::SplitChildArterial(int child, int childpos, int par, int parpos)
  {
   VESS headend, tailend;
  int counter;
  int i;
  char dohead = 0, dotail = 0;
   float* parentptr;
   float* childptr;
   int twoval[2];

            //check that you are not walking over the boundary
            //of the vessel array
  if (vessnum >= (maxvess - 1))
    {
      std::cerr<<"You must increase maxvess in VESSCOL.h--not enough vessels\n";
        strcpy(err, "You must increase maxvess in VESSCOL.h");
      return 0;
      }


  parentptr = vess[par].Point(parpos);  //parent's tail point
  childptr = vess[child].Point(childpos);

            //pos gives the offset into the orph point array. Calculate
            //the size you need for the two split vessels
  int headsize = childpos - vess[child].Proxoffset();
  int tailsize = vess[child].Distoffset() - childpos;

  if (headsize > 0) {dohead = 1; headend.Realloc(headsize, 4);}
  if (tailsize > 0) {dotail = 1; tailend.Realloc(tailsize, 4);}


            //now read the points into each of the split vessels. The
            //headend must be put in backwards (flow is from parent)
  if (dohead)
    {
    counter = 0;
    for (i = childpos - 1; i >= vess[child].Proxoffset(); i--)
      {
      childptr = vess[child].Point(i);  //get each point from child
      headend.Point(counter, childptr); //copy in reverse order to headend
      ++counter;
      }
               //keep any children
      vess[child].ResetChild();
      while (vess[child].GetChild(twoval))
         {
         if (twoval[1] < vess[child].Proxoffset()) continue;
         if (twoval[1] >= childpos) break;
         headend.AddChild(twoval[0], childpos - 1 - twoval[1]);
         vess[twoval[0]].AttachPt(childpos - 1 - twoval[1]);
         vess[twoval[0]].TreeType("child");

         }
    }

  if (dotail)
    {
    counter = 0;
    for (i = childpos; i < vess[child].Distoffset(); i++)
      {
      childptr = vess[child].Point(i);
      tailend.Point(counter, childptr);
      ++counter;
      }
                  //keep children
      vess[child].ResetChild();
      while(vess[child].GetChild(twoval))
         {
         if (twoval[1] < childpos) continue;
         if (twoval[1] >=vess[child].Distoffset()) break;
         tailend.AddChild(twoval[0], twoval[1] - childpos);
         vess[twoval[0]].AttachPt(twoval[1] - childpos);
         if (dohead) vess[twoval[0]].VParent(vessnum);
         vess[twoval[0]].TreeType("child");

         }
    }
                //Set the parentpoint in each child
  headend.AttachPt(parpos);
  tailend.AttachPt(parpos);
   headend.VParent(par);
   tailend.VParent(par);

                        //You want no blocks in the new children--you've taken
                        //care of proxoffset and distoffset already. You also
                        //need to get the colors right
   headend.Resetoffset();
   tailend.Resetoffset();
   headend.SetColor(vess[child].GetColor());
   tailend.SetColor(vess[child].GetColor());

                
  if (!dohead)
      vess[child] = tailend;
  else
      vess[child] = headend;

  if (dohead && dotail)
      {
      vess[vessnum] = tailend;
      ++vessnum;                        
      vess[par].AddChild(vessnum - 1, parpos); //add child to parent list
         vess[vessnum - 1].TreeType("child");

      }
   vess[par].AddChild(child, parpos);
   vess[child].TreeType("child");

   vess[vessnum - 1].Resetoffset();
   vess[child].Resetoffset();
   return 1;
  }
//---------------------------------------------
int VESSCOL::SplitChildVenous(int child, int childpos, int par, int parpos)
  {
   VESS headend, tailend;
  int counter;
  int i;
  char dohead = 0, dotail = 0;
   float* parentptr;
   float* childptr;
   int twoval[2];

            //check that you are not walking over the boundary
            //of the vessel array
  if (vessnum >= (maxvess - 1))
    {
      std::cerr<<"You must increase maxvess in VESSCOL.h--not enough vessels\n";
        strcpy(err, "You must increase maxvess in VESSCOL.h");
      return 0;
      }


  parentptr = vess[par].Point(parpos);  //parent's head point
  childptr = vess[child].Point(childpos);

            //pos gives the offset into the orph point array. Calculate
            //the size you need for the two split vessels
  int headsize = childpos - vess[child].Proxoffset();
  int tailsize = vess[child].Distoffset() - childpos;

  if (headsize > 0) {dohead = 1; headend.Realloc(headsize, 4);}
  if (tailsize > 0) {dotail = 1; tailend.Realloc(tailsize, 4);}


            //now read the points into each of the split vessels. The
            //tail end must be put in backwards (flow is toward parent)
  if (dohead)
    {
    counter = 0;
   
      for (i = vess[child].Proxoffset(); i < childpos; i++)
       {
      childptr = vess[child].Point(i);  //get each point from child
      headend.Point(counter, childptr); 
      ++counter;
         }

               //keep any children
      vess[child].ResetChild();
      while (vess[child].GetChild(twoval))
         {
         if (twoval[1] < vess[child].Proxoffset()) continue;
         if (twoval[1] >= childpos) break;

         headend.AddChild(twoval[0], twoval[1] - vess[child].Proxoffset());
         vess[twoval[0]].AttachPt(twoval[1] - vess[child].Proxoffset());
         vess[twoval[0]].TreeType("child");

         }
    }

  if (dotail)
    {
    counter = 0;
      for (i = vess[child].Distoffset() - 1; i >= childpos; i--)
      {
      childptr = vess[child].Point(i);
      tailend.Point(counter, childptr);
      ++counter;
      }
                  //keep children
      vess[child].ResetChild();
      while(vess[child].GetChild(twoval))
         {
         if (twoval[1] < childpos) continue;
         if (twoval[1] >=vess[child].Distoffset()) break;

         tailend.AddChild(twoval[0], vess[child].Distoffset() - childpos);
         vess[twoval[0]].AttachPt(vess[child].Distoffset() - childpos);
        if (dohead)    vess[twoval[0]].VParent(vessnum);
         vess[twoval[0]].TreeType("child");

         }
    }
                //Set the parentpoint in each child
  headend.AttachPt(parpos);
  tailend.AttachPt(parpos);
   headend.VParent(par);
   tailend.VParent(par);

                        //You want no blocks in the new children--you've taken
                        //care of proxoffset and distoffset already. You also
                        //need to get the colors right
   headend.Resetoffset();
   tailend.Resetoffset();
   headend.SetColor(vess[child].GetColor());
   tailend.SetColor(vess[child].GetColor());

                
  if (!dohead)
      vess[child] = tailend;
  else
      vess[child] = headend;

  if (dohead && dotail)
      {
      vess[vessnum] = tailend;
      ++vessnum;                        
      vess[par].AddChild(vessnum - 1, parpos); //add child to parent list
         vess[vessnum - 1].TreeType("child");
      }
   vess[par].AddChild(child, parpos);
   vess[child].TreeType("child");

   vess[vessnum - 1].Resetoffset();
   vess[child].Resetoffset();
   return 1;
  }

}//end of namespace
