/********************************************************************
*                          Trace1.c                                 *
*********************************************************************
*   Trace functions specific to board 1
********************************************************************/

#include "sim.h"
#include "micro1.h"
#include "trace.c"

/********************************************************************
*                         store_trace()                             *
*********************************************************************
*  Store the current state into the trace array.
********************************************************************/

int store_trace()
{
   if(savestate())
   {
      tr[tndx].mad=last_index;
      tr[tndx].tick=timer;
      tr[tndx].insw[0]=sw[0];
      tr[tndx].insw[1]=sw[1];
      tr[tndx].insw[2]=sw[2];
      tr[tndx].insw[3]=sw[3];
      tr[tndx].mpc=current_index;
      return(inc_tndx());
   }
   return(FALSE);
}

/********************************************************************
*                         reset_trace()                             *
*********************************************************************
*  Reset the trace configuration to defaults.
********************************************************************/

int reset_trace()
{
   int i;
   myint x = (ANY);

   clear_trace();
   tg.mad = x;
   st.mad = x;
   tg.tick = x;
   st.tick = x;
   for(i=0;i<4;i++)
   {
      tg.insw[i] = x;
      st.insw[i] = x;
   }
   tg.mpc = x;
   st.mpc = x;
   tgm.micro_ad = x;
   tgm.bop = x;
   tgm.pol = x;
   tgm.cond = x;
   tgm.data = x;
   stm.micro_ad = x;
   stm.bop = x;
   stm.pol = x;
   stm.cond = x;
   stm.data = x;
}

/********************************************************************
*                         matchstate()                              *
*********************************************************************
*   Return TRUE if current state matches spec state.
********************************************************************/

int matchstate(p,q)
struct trace_data *p;
struct micro_word *q;
{
   myint x = (ANY);
   if((p->mad!= x)&&(p->mad!=last_index))            return(FALSE);
   if((p->tick!= x)&&(p->tick!=timer))               return(FALSE);
   if((p->insw[0]!= x)&&(p->insw[0]!=sw[0]))         return(FALSE);
   if((p->insw[1]!= x)&&(p->insw[1]!=sw[1]))         return(FALSE);
   if((p->insw[2]!= x)&&(p->insw[2]!=sw[2]))         return(FALSE);
   if((p->insw[3]!= x)&&(p->insw[3]!=sw[3]))         return(FALSE);
   if((p->mpc!= x)&&(p->mpc!=current_index))         return(FALSE);
   if((q->micro_ad!= x)&&(q->micro_ad!=mw.micro_ad)) return(FALSE);
   if((q->bop!= x)&&(q->bop!=mw.bop))                return(FALSE);
   if((q->pol!= x)&&(q->pol!=mw.pol))                return(FALSE);
   if((q->cond!= x)&&(q->cond!=mw.cond))             return(FALSE);
   if((q->data!= x)&&(q->data!=mw.data))             return(FALSE);
   return(TRUE);
}

/********************************************************************
*                         spec_menu()                               *
*********************************************************************
*   Put up a menu of specification choices
********************************************************************/

int spec_menu(spectype)
char *spectype;
{
   int mwfield;

   myprintf("\n");
   myprintf("                   %s CONFIGURATION MENU\n\n",spectype);
   myprintf("               ---------- Signal Values ----------            \n");
   myprintf("       1] Cs_Ad            4] Switch 3         6] Switch 1    \n");
   myprintf("       2] Clock            5] Switch 2         7] Switch 0    \n");
   myprintf("       3] MPC                                                 \n");
   myprintf("\n");
   myprintf("               --------- Microword Values --------            \n");
   myprintf("       8] Micro_Ad        10] Pol             12] Data        \n");
   myprintf("       9] Bop             11] Cond                            \n");
   myprintf("\n");
   myprintf("                           0] Exit (Done)                     \n");
   myprintf("\n");
   myprintf("Your selection [0-12]? "); lnl;
   mwfield = get_dec();
   while(mwfield < 0 || mwfield > 12)
   {
      myprintf("Please choose a number between 0 and 12: "); lnl;
      mwfield = get_dec();
   }
   return(mwfield);
}

/********************************************************************
*                         set_spec()                                *
*********************************************************************
*  Configure the trace or store state.
********************************************************************/

int set_spec(p,q,spectype)
struct trace_data *p;
struct micro_word *q;
char *spectype;
{
   while (TRUE)       /* infinite loop */
   {
      switch(spec_menu(spectype)) 
      {
	case 0:  return(0);                                     break;
	case 1:  requestit("Cs_ad = ",&(p->mad),0xffL);         break;
	case 2:  requestit("Clock = ",&(p->tick),9999L);        break;
	case 3:  requestit("MPC = ",&(p->mpc),0x0fL);           break;
	case 4:  requestit("Switch 3 = ",&(p->insw[3]),1L);     break;
	case 5:  requestit("Switch 2 = ",&(p->insw[2]),1L);     break;
	case 6:  requestit("Switch 1 = ",&(p->insw[1]),1L);     break;
	case 7:  requestit("Switch 0 = ",&(p->insw[0]),1L);     break;
	case 8:  requestit("Micro_ad = ",&(q->micro_ad),0xffL); break;
	case 9:  requestit("Bop = ",&(q->bop),0x07L);           break;
	case 10: requestit("Pol = ",&(q->pol),1L);              break;
	case 11: requestit("Cond = ",&(q->cond),3L);            break;
	case 12: requestit("Data = ",&(q->data),0x0fL);         break;
        default:                                                break;
      }
   }
}

/********************************************************************
*                         show_spec()                               *
*********************************************************************
*  Display the trace or store state.
********************************************************************/

int show_spec(p,q,j)
struct trace_data *p;
struct micro_word *q;
int *j;
{
      int i=(*j);
      if(showoff("Cs_ad =    ",p->mad,j)==2)      return(-1);
      if(showoff("Clock =    ",p->tick,j)==2)     return(-1);
      if(showoff("Switch 3 = ",p->insw[3],j)==2)  return(-1);
      if(showoff("Switch 2 = ",p->insw[2],j)==2)  return(-1);
      if(showoff("Switch 1 = ",p->insw[1],j)==2)  return(-1);
      if(showoff("Switch 0 = ",p->insw[0],j)==2)  return(-1);
      if(showoff("MPC =      ",p->mpc,j)==2)      return(-1);
      if(showoff("Bop =      ",q->bop,j)==2)      return(-1);
      if(showoff("Data =     ",q->data,j)==2)     return(-1);
      if(showoff("Cond =     ",q->cond,j)==2)     return(-1);
      if(showoff("Pol =      ",q->pol,j)==2)      return(-1);
      if(showoff("Micro_ad = ",q->micro_ad,j)==2) return(-1);
      if(*j==i) myprintf("ANY state\n");
}

/********************************************************************
*                       show_trace()                                *
*********************************************************************
*   Display the trace_data to the screen.
********************************************************************/

int show_trace()
{

   int i,j,first,last;
   char *header = "Cs_Ad Switches   MPC  Clock Data Cond Pol Bop MicAd\n\n";

   first=trigptr;
   last=trigptr+step_count;
   myprintf("%s",header);

   for(i=first;i<last;i++)
   {
      j=i%TMAX;
      myprintf(" %04lx  %01lx %01lx %01lx %01lx",
          tr[j].mad,tr[j].insw[3],tr[j].insw[2],tr[j].insw[1],
          tr[j].insw[0]);
      myprintf("  %04lx  %05ld",
          tr[j].mpc,tr[j].tick);

      load_mw((int)tr[j].mad);
      myprintf("   %01lx    %01lx   %01lx   %01lx    %02lx\n\n",
          mw.data,mw.cond,mw.pol,mw.bop,mw.micro_ad);

      if((i-first)%9==8)
      {
         if(more())
            return(0);
         myprintf("%s",header);
      }
   }
}

/********************************************************************
*                       save_trace()                                *
*********************************************************************
*   Save the trace data to a file for later printing.
********************************************************************/

int save_trace()
{
   char *header = "Cs_Ad Switches   MPC  Clock Data Cond Pol Bop MicAd\n\n";
   char fname[N], ch;
   FILE *fp, *fopen();
   int i,j,first,last;

   myprintf("Filename for trace data [.trc]: "); 
   scanf("%s%c",fname,&ch);
   if (logging) fprintf(logfile,"%s\n",fname);
   i=0;
   while((fname[i]!='\0')&&(fname[i]!='.'))
      i++;
   if(fname[i]=='\0')
      strcat(fname,".trc");
   if(fname[0]=='.')
   {
      myprintf("Bad filename [%s].\n",fname);
      return(0);
   }

   if((fp=fopen(fname,"w"))==NULL)
      myprintf("Can't open %s\n",fname);
   else
   {
      first=trigptr;
      last=trigptr+step_count;
      fprintf(fp,"%s",header);

      for(i=first;i<last;i++)
      {
         j=i%TMAX;
         fprintf(fp," %04lx  %01lx %01lx %01lx %01lx",
             tr[j].mad,tr[j].insw[3],tr[j].insw[2],tr[j].insw[1],
             tr[j].insw[0]);
         fprintf(fp,"  %04lx  %05ld",
             tr[j].mpc,tr[j].tick);

         load_mw((int)tr[j].mad);
         fprintf(fp,"   %01lx    %01lx   %01lx   %01lx    %02lx\n\n",
             mw.data,mw.cond,mw.pol,mw.bop,mw.micro_ad);
      }
      fclose(fp);
      myprintf("File %s created.\n\n",fname);
   }
}

