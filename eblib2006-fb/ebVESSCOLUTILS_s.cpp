#include "ebVESSCOLUTILS_S.h"

#include "ebFVEC.h"

namespace eb
{

VESSCOL_s* VESSCOLUTILS_s::CompressVC(VESSCOL_s* inputvc)
   {
   if(!inputvc || !inputvc->GetVessnum())
      return 0;


   vc.Releasemem();

   int i, val, newvessnum;
   int twoi[2];
   VESS_s* newvess = 0;
   int counter = 0;
   ARLIST<int> templist;
   templist.SetDims(2);

   newvessnum = inputvc->GetVessnum();


            //find out how many total vessels you will need after 
            //compression
   for (i = 0; i < inputvc->GetVessnum(); i++)
      {
      inputvc->ResetChild(i);
      val = inputvc->NPoints(i) - 1;      //last valid point on vess[i]
      while (inputvc->GetChild(i, twoi))
         {
         if (twoi[1] == val)        //a child arises from this last point
            {
            ++counter;
            twoi[1] = twoi[0];      //child
            twoi[0] = i;            //parent

            --newvessnum;           //child will need to be combined with parent
            break;                  //any second attached vessel will remain a child
            }
         }
      }

  //create a modifiable copy of inputvc and concatenate the vessels here.
   VESSCOL_s tempvc;
   tempvc = *inputvc;

   for (i = 0; i < inputvc->GetVessnum(); i++)
      {
      val = tempvc.NPoints(i) - 1;
      tempvc.ResetChild(i);
      while (tempvc.GetChild(i, twoi))
         {  
         if (twoi[1] == val)        //a child arises from this last point
            {
            ConcatenateVessels(i, twoi[0], &tempvc);
            i = 0;
            break;
            }
         }
      }


         //debug
   int temp = 0;
   int j, k, m;
   int dontcontinue = 0;
   int found;
   
   for (i = 0; i < tempvc.GetVessnum(); i++)
      {
      if (tempvc.NPoints(i))
         ++temp;
      else break;

      if(tempvc.VParent(i, &j) && j >= newvessnum)
         {
         std::cerr<<"Bad parent "<<j<<"  for child "<<i<<std::endl;
         dontcontinue = 1;
         }
      if (tempvc.AttachPt(i, &k) && (k>= tempvc.NPoints(j) || k < 0))
         {
         std::cerr<<"Bad attachment at point "<<k<<" for child "<<i<<"  to parent "<<j<<", with "<<tempvc.NPoints(j) -1<<" points"<<std::endl;
         dontcontinue = 1;


               //query the parent's child list
         tempvc.ResetChild(j);
         found = 0;
         while (tempvc.GetChild(j, twoi))
            {
            if (twoi[0] == i)
               {
               found = 1;
               std::cerr<<"     child found in parent list\n";
               std::cerr<<"     attachpt listed as "<<k<<"  parentlist lists as "<<twoi[1]<<std::endl;
               }
            }
         if (!found)
            {
            std::cerr<<"      CHILD NOT IN PARENT'S CHILDLIST LIST "<<j<<"\n";
            for (m = 0; m < tempvc.GetVessnum(); m++)
               {
               tempvc.ResetChild(m);
               while (tempvc.GetChild(m, twoi))
                  {
                  if (twoi[0] == i)
                     std::cerr<<"    CHILD FOUND IN CHILDLIST OF VESSEL "<<m<<" at position "<<twoi[1]<<" with possible points of "<<tempvc.NPoints(m)<<std::endl;
                  }
               }

            }

         }
      }
   std::cerr<<"expected vessel number  "<<newvessnum<<"   actual number  "<<temp<<std::endl;

   if (dontcontinue)
      {
      std::cerr<<std::endl<<std::endl<<"ABORTING VESSCOLUTILS::COMPRESS(): ERROR!\n\n";
      return 0;
      }
   
   
   //end debug

   if (newvessnum)
      {
      newvess = new VESS_s[newvessnum];
      for (i = 0; i < newvessnum; i++)
         newvess[i] = *(tempvc.GetVess(i));

      vc.LoadWorldTree(newvessnum, newvess, tempvc.GetDims(), tempvc.GetSteps());
      }

   if (newvess)
   delete [] newvess;

   RecalcLists();

   return &vc;
   }
//---------------------------------------------
VESSCOL_s* VESSCOLUTILS_s::ProcessConnectionPoints(VESSCOL_s* v)
   {
   int numchanges = 0;

   int i, par, pos;
   float* fptr1;
   float* fptr2;
   FVEC vec;
   VESS_s* vessptr;

   vc = *v;

   for (i = 0; i < vc.GetVessnum(); i++)
      {
      if (vc.VParent(i, &par))
         {
         fptr1 = vc.GetFloatAttachPt(i);
         fptr2 = vc.Point(i, 0);
         vec.Set(fptr1[0] - fptr2[0], fptr1[1] - fptr2[1], fptr1[2] - fptr2[2]);
         if (!vec.Vlen())     //points are identical
            {
                  //look for a a more distal point on the parent. If this
                  //is not valid, take a more proximal one.
            vc.AttachPt(i, &pos);
            if (pos < vc.NPoints(par) - 2)
               {
               pos += 2;
               ++numchanges;
               }
            else if (pos > 1)
               {
               pos -= 2;
               ++numchanges;
               }
            vc.AttachPt(i, pos);
            vessptr= vc.GetVess(par);
            vessptr->DeleteChild(i);
            vessptr->AddChild(i, pos);
            }
         }
      }

   
   std::cerr<<numchanges<<" points changed to avoid 0 vector\n";
   return &vc;
   }




//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&77
      //PROTECTED
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
void VESSCOLUTILS_s::ConcatenateVessels(int par, int child, VESSCOL_s* tempvc)
   {
   VESS_s* vessptr1;
   VESS_s* vessptr2;
   VESS_s tempvess;
   int pos, otherpar, origparpts, i;
   int twoi[2];
   ARLIST<int> templist;
   templist.SetDims(2);
  
            //combine the points of the two vessels and substitute for
            //vess[par]
   vessptr1 = tempvc->GetVess(par);
   origparpts = vessptr1->NPoints();
   vessptr2 = tempvc->GetVess(child);
   tempvess.ConcatenateVessels(*vessptr1, *vessptr2);
   tempvess.DeleteChild(child);     //remove vess[child] from childlist
   tempvess.DeleteChild(par);       //just in case
   *vessptr1 = tempvess;

            //you are going to shift everything above child one place to
            //the left (getting rid of child). Any parent value greater
            //than child must be decremented by one, and any child listed
            //in the child list that is greater than child must also be
            //changed
   for (i = 0; i < tempvc->GetVessnum(); i++)
      {
      if (tempvc->VParent(i, &otherpar))
         {
         if (otherpar == child)     //must change both parent and position
            {
            tempvc->AttachPt(i, &pos);
            pos += origparpts;
            tempvc->VParent(i, par);
            tempvc->AttachPt(i, pos);
            }

                  //you have adjusted everyone originally attached to
                  //child to the new position on par. Now left shift
                  //all par ids to one less
         tempvc->VParent(i, &otherpar);
         if (otherpar > child)
            tempvc->VParent(i, otherpar - 1);
         }
      }


                  //now left shift all ids > child in the childlists
   for (i = 0; i < tempvc->GetVessnum(); i++)
      {
      templist.Releasemem();
      tempvc->ResetChild(i);
      while (tempvc->GetChild(i, twoi))
         {
         if (twoi[0] > child)
            twoi[0] -= 1;
         templist.AddToTail(twoi);
         }

            
      tempvc->DeleteChildren(i);
      templist.Reset();
      while (templist.Get(twoi))
         tempvc->AddChild(i, twoi);
      }

            //Now left shift all vessels greater than child and
            //empty the last non-empty one
   for (i = child; i < tempvc->GetVessnum() - 1; i++)
      {
      vessptr1 = tempvc->GetVess(i);
      vessptr2 = tempvc->GetVess(i + 1);

      if (!vessptr2->NPoints())
         {
         vessptr1->Releasemem();
         break;
         }
      *(vessptr1) = *(vessptr2);

      }
 

            //if this is the first pass through, empty the last slot
   if (tempvc->NPoints(tempvc->GetVessnum() - 1))
      {
      vessptr1 = tempvc->GetVess(tempvc->GetVessnum() -1);
      vessptr1->Releasemem();
      }
   }

//---------------------------------------------
void VESSCOLUTILS_s::RecalcLists()
   {
   LIST<int>* rl = vc.GetRootlist();
   rl->Releasemem();

   for (int i = 0; i< vc.GetVessnum(); i++)
      {
      if (vc.IsRoot(i))
         rl->AddToTail(i);
      }
   }




}//end of namespace