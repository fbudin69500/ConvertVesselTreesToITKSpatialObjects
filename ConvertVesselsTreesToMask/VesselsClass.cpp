#include "VesselsClass.h"

VesselsClass::VesselsClass()
{
    m_Reconnect = 1 ; 
    m_Resample = 1;
    m_Concatenate = 1;
    m_Sampling = 10 ;
    m_Modified = 0;
    m_IsValid = 0;
}


bool VesselsClass::IsValid() const
{
    return m_IsValid;
}

void VesselsClass::Update()
{
    if(!m_Modified )
    {
        return;
    }
    if( m_FileName.empty() || !vc.Load( m_FileName.c_str() ) ) 
    {
        m_IsValid = 0;
        return ;
    }

  vc.ResetAll();    //make sure nothing is blocked or hidden. Use full
                     //data on load.
  if(m_Concatenate)
  {
      Concatenate();
  }
  if(m_Reconnect)
  {
      Reconnect();
  }
           //It is possible that this program might load a set of vessels taken from Stephen's program alone
         //and not turned into trees. So make sure that this program can handle this situation. Turn
         //all orphans (not normally displayed) into trees.
   eb::LIST<int> lst;
   vc.GetOrphans(&lst);
   int val;
   lst.Reset();
   while (lst.Get(&val))
      vc.AddRoot(val);
  if(m_Resample)
  {
      Resample();
  }
  m_IsValid = 1;
m_Modified = 0;
}


//make vessels that are end-to-end into a single vessel
int VesselsClass::Concatenate()
{
    if( !vc.GetVessnum() )
      return 0;

    //std::cerr<<"Concatenating child-parent. Input # = "<<inputvc.GetVessnum();

   int i, val, newvessnum;
   int twoi[2];
   eb::VESS* newvess = 0;
   eb::ARLIST<int> templist;
   templist.SetDims(2);

   newvessnum = vc.GetVessnum();


  //create a modifiable copy of inputvc and concatenate the vessels here.
   eb::VESSCOL tempvc;
   tempvc = vc;
   

   for (i = 0; i < newvessnum; i++)
      {
      val = tempvc.NPoints(i) - 1;
      tempvc.ResetChild(i);
      while (tempvc.GetChild(i, twoi))
         {  
         if (twoi[1] == val)        //a child arises from this last point
            {
            ConcatenateVessels(i, twoi[0], &tempvc);
            newvessnum -= 1;
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

   if (dontcontinue)
      {
      //std::cerr<<std::endl<<std::endl<<"ABORTING EBCHARACTERISTICS::COMPRESS(): ERROR!\n\n";
      return 0;
      }
   
   
   //end debug

   if (newvessnum)
      {
      newvess = new eb::VESS[newvessnum];
      for (i = 0; i < newvessnum; i++)
         newvess[i] = *(tempvc.GetVess(i));

      vc.LoadWorldTree(newvessnum, newvess, tempvc.GetDims(), tempvc.GetSteps());
      }

   if (newvess)
   delete [] newvess;

   //RecalcLists();

   //std::cerr<<"   Output # = "<<inputvc.GetVessnum()<<std::endl;

   return 1;
}


//Copy from int ebANALYZEWIN::LoadVesselFilename(char* s)
void VesselsClass::Reconnect()
{
   //am having trouble with connection points that are
   //identical to a child point. This makes holes. Adjust.
   eb::VESSCOLUTILS utils;
   vc = *(utils.ProcessConnectionPoints(&vc));
}

eb::VESSCOL VesselsClass::GetOutput() const
{
    //Update();
    return vc;
}


void VesselsClass::Resample()
{
    int num;
    for (int i = 0; i < vc.GetVessnum(); i++)
      vc.DoConnectedAllowedSample(i, m_Sampling, &num);
}



//Copy of void ebCHARACTERISTICS::ConcatenateVessels(int par, int child, eb::VESSCOL_s* tempvc)
void VesselsClass::ConcatenateVessels(int par, int child, eb::VESSCOL* tempvc)
   {

   eb::VESS* vessptr1;
   eb::VESS* vessptr2;
   eb::VESS tempvess;
   int pos, otherpar, origparpts, i;
   int twoi[2];
   eb::ARLIST<int> templist;
   templist.SetDims(2);
  
            //combine the points of the two vessels and substitute for
            //vess[par]
   vessptr1 = tempvc->GetVess(par);
   origparpts = vessptr1->NPoints();
   vessptr2 = tempvc->GetVess(child);
   tempvess.ConcatenateVessels(*vessptr1, *vessptr2);
   tempvess.DeleteChild(child);     //remove vess[child] from childlist
   tempvess.DeleteChild(par);       //just in case
   vessptr1->Releasemem(); //Remove the original parent. Child will be remove later in the function.
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

////Copy from void ebCHARACTERISTICS::RecalcLists(eb::VESSCOL_s* vc)
//   //recalculate the root list. Numbering has likely changed
//void VesselsClass::RecalcLists()
//   {
//   int i;
//   eb::LIST<int>* rl = vc.GetRootlist();
//   rl->Releasemem();
//
//   for (i = 0; i< vc.GetVessnum(); i++)
//      {
//      if (vc.IsRoot(i))
//         rl->AddToTail(i);
//      }
//   }

