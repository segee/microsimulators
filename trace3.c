/********************************************************************
*                          Trace3.c                                 *
*********************************************************************
*   Trace functions specific to board 3
********************************************************************/

#include "sim.h"
#include "micro3.h"
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
      tr[tndx].reg[0]=acc;
      tr[tndx].reg[1]=sr;
      tr[tndx].reg[2]=sph;
      tr[tndx].reg[3]=spl;
      tr[tndx].reg[4]=marh;
      tr[tndx].reg[5]=marl;
      tr[tndx].reg[6]=pch;
      tr[tndx].reg[7]=pcl;
      tr[tndx].reg[8]=data_in;
      tr[tndx].bus[0]=a_bus;
      tr[tndx].bus[1]=b_bus;
      tr[tndx].bus[2]=f_bus;
      tr[tndx].bus[3]=data_bus;
      tr[tndx].bus[4]=address_bus;
      tr[tndx].mpc=current_index;
      tr[tndx].cmicro=cmicro;
      tr[tndx].leds=leds;
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
   for(i=0;i<9;i++)
   {
      tg.reg[i] = x;
      st.reg[i] = x;
   }
   for(i=0;i<5;i++)
   {
      tg.bus[i] = x;
      st.bus[i] = x;
   }
   tg.mpc = x;
   st.mpc = x;
   tg.cmicro = x;
   st.cmicro = x;
   tg.leds = x;
   st.leds = x;
   tgm.micro_ad = x;
   tgm.constant = x;
   tgm.bop = x;
   tgm.count = x;
   tgm.op_con_ctl = x;
   tgm.alu_source = x;
   tgm.alu_dest = x;
   tgm.ad_bus = x;
   tgm.alu_func = x;
   tgm.cin = x;
   tgm.inc_pc = x;
   tgm.en_sr = x;
   tgm.r_w = x;
   tgm.vma = x;
   stm.micro_ad = x;
   stm.constant = x;
   stm.bop = x;
   stm.count = x;
   stm.op_con_ctl = x;
   stm.alu_source = x;
   stm.alu_dest = x;
   stm.ad_bus = x;
   stm.alu_func = x;
   stm.cin = x;
   stm.inc_pc = x;
   stm.en_sr = x;
   stm.r_w = x;
   stm.vma = x;
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
   if((p->mad!= x)&&(p->mad!=last_index))                   return(FALSE);
   if((p->tick!= x)&&(p->tick!=timer))                      return(FALSE);
   if((p->reg[0]!= x)&&(p->reg[0]!=acc))                    return(FALSE);
   if((p->reg[1]!= x)&&(p->reg[1]!=sr))                     return(FALSE);
   if((p->reg[2]!= x)&&(p->reg[2]!=sph))                    return(FALSE);
   if((p->reg[3]!= x)&&(p->reg[3]!=spl))                    return(FALSE);
   if((p->reg[4]!= x)&&(p->reg[4]!=marh))                   return(FALSE);
   if((p->reg[5]!= x)&&(p->reg[5]!=marl))                   return(FALSE);
   if((p->reg[6]!= x)&&(p->reg[6]!=pch))                    return(FALSE);
   if((p->reg[7]!= x)&&(p->reg[7]!=pcl))                    return(FALSE);
   if((p->reg[8]!= x)&&(p->reg[8]!=data_in))                return(FALSE);
   if((p->bus[0]!= x)&&(p->bus[0]!=a_bus))                  return(FALSE);
   if((p->bus[1]!= x)&&(p->bus[1]!=b_bus))                  return(FALSE);
   if((p->bus[2]!= x)&&(p->bus[2]!=f_bus))                  return(FALSE);
   if((p->bus[3]!= x)&&(p->bus[3]!=data_bus))               return(FALSE);
   if((p->bus[4]!= x)&&(p->bus[4]!=address_bus))            return(FALSE);
   if((p->mpc!= x)&&(p->mpc!=current_index))                return(FALSE);
   if((p->cmicro!= x)&&(p->cmicro!=cmicro))                 return(FALSE);
   if((p->leds!= x)&&(p->leds!=leds))                       return(FALSE);
   if((q->micro_ad != x)&&(q->micro_ad!=mw.micro_ad))       return(FALSE);
   if((q->constant != x)&&(q->constant!=mw.constant))       return(FALSE);
   if((q->bop != x)&&(q->bop!=mw.bop))                      return(FALSE);
   if((q->count != x)&&(q->count!=mw.count))                return(FALSE);
   if((q->op_con_ctl != x)&&(q->op_con_ctl!=mw.op_con_ctl)) return(FALSE);
   if((q->alu_source != x)&&(q->alu_source!=mw.alu_source)) return(FALSE);
   if((q->alu_dest != x)&&(q->alu_dest!=mw.alu_dest))       return(FALSE);
   if((q->ad_bus != x)&&(q->ad_bus!=mw.ad_bus))             return(FALSE);
   if((q->alu_func != x)&&(q->alu_func!=mw.alu_func))       return(FALSE);
   if((q->cin != x)&&(q->cin!=mw.cin))                      return(FALSE);
   if((q->inc_pc != x)&&(q->inc_pc!=mw.inc_pc))             return(FALSE);
   if((q->en_sr != x)&&(q->en_sr!=mw.en_sr))                return(FALSE);
   if((q->r_w != x)&&(q->r_w!=mw.r_w))                      return(FALSE);
   if((q->vma != x)&&(q->vma!=mw.vma))                      return(FALSE);
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
   myprintf("               ---------- Signal Values ----------             \n");
   myprintf("     1] Cs_Ad        6] A_Bus       11] ACC         16]PCH     \n");
   myprintf("     2] Clock        7] B_Bus       12] SPH         17]PCL     \n");
   myprintf("     3] MPC          8] F_Bus       13] SPL         18]SR      \n");
   myprintf("     4] C_micro      9] Data_Bus    14] MARH        19]Data_In \n");
   myprintf("     5] Leds        10] Addr_Bus    15] MARL                   \n");
   myprintf("\n");
   myprintf("               --------- Microword Values --------             \n");
   myprintf("    20] Micro_Ad    24] OP_CON_CTL  28] ALU_Func    31]EN_SR   \n");
   myprintf("    21] Constant    25] ALU_Source  29] Cin         32]R_W     \n");
   myprintf("    22] Bop         26] ALU_Dest    30] Inc_PC      33]VMA     \n");
   myprintf("    23] Count       27] Ad_Bus                                 \n");
   myprintf("\n");
   myprintf("                           0] Exit (Done)                      \n");
   myprintf("\n");
   myprintf("Your selection [0-33]? "); lnl;
   mwfield = get_dec();
   while(mwfield < 0 || mwfield > 33)
   {
      myprintf("Please choose a number between 0 and 33: "); lnl;
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
	case 0:  return(0);                                        break;
	case 1:  requestit("Cs_ad = ",&(p->mad),0xfffL);           break;
	case 2:  requestit("Clock = ",&(p->tick),9999L);           break;
	case 11: requestit("ACC = ",&(p->reg[0]),0xffL);           break;
	case 18: requestit("SR = ",&(p->reg[1]),0xffL);            break;
	case 12: requestit("SPH = ",&(p->reg[2]),0xffL);           break;
	case 13: requestit("SPL = ",&(p->reg[3]),0xffL);           break;
	case 14: requestit("MARH = ",&(p->reg[4]),0xffL);          break;
	case 15: requestit("MARL = ",&(p->reg[5]),0xffL);          break;
	case 16: requestit("PCH = ",&(p->reg[6]),0xffL);           break;
	case 17: requestit("PCL = ",&(p->reg[7]),0xffL);           break;
	case 19: requestit("Data_in = ",&(p->reg[8]),0xffL);       break;
	case 6:  requestit("A_bus = ",&(p->bus[0]),0xffL);         break;
	case 7:  requestit("B_bus = ",&(p->bus[1]),0xffL);         break;
	case 8:  requestit("F_bus = ",&(p->bus[2]),0xffL);         break;
	case 9:  requestit("Data_bus = ",&(p->bus[3]),0xffL);      break;
	case 10: requestit("Addr_bus = ",&(p->bus[4]),0xffffL);    break;
	case 3:  requestit("MPC = ",&(p->mpc),0xfffL);             break;
	case 4:  requestit("Cmicro = ",&(p->cmicro),1L);           break;
	case 5:  requestit("Leds = ",&(p->leds),0xffL);            break;
	case 20: requestit("MicroAd = ",&(q->micro_ad),0xfffL);    break;
	case 21: requestit("Constant = ",&(q->constant),0xffL);    break;
	case 22: requestit("Bop = ",&(q->bop),0x0fL);              break;
	case 23: requestit("Count = ",&(q->count),1L);             break;
	case 24: requestit("OpConCtl = ",&(q->op_con_ctl),3L);     break;
	case 25: requestit("AluSource = ",&(q->alu_source),0x0fL); break;
	case 26: requestit("AluDest = ",&(q->alu_dest),0x0fL);     break;
	case 27: requestit("AdBus = ",&(q->ad_bus),3L);            break;
	case 28: requestit("AluFunc = ",&(q->alu_func),0x1fL);     break;
	case 29: requestit("Cin = ",&(q->cin),3L);                 break;
	case 30: requestit("IncPC = ",&(q->inc_pc),1L);            break;
	case 31: requestit("EnSR = ",&(q->en_sr),1L);              break;
	case 32: requestit("R_W = ",&(q->r_w),1L);                 break;
	case 33: requestit("Vma = ",&(q->vma),1L);                 break;
        default: break;
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
   if(showoff("Micro_ad =  ",p->mad,j)==2)        return(-1);
   if(showoff("Clock =     ",p->tick,j)==2)       return(-1);
   if(showoff("ACC =       ",p->reg[0],j)==2)     return(-1);
   if(showoff("SR =        ",p->reg[1],j)==2)     return(-1);
   if(showoff("SPH =       ",p->reg[2],j)==2)     return(-1);
   if(showoff("SPL =       ",p->reg[3],j)==2)     return(-1);
   if(showoff("MARH =      ",p->reg[4],j)==2)     return(-1);
   if(showoff("MARL =      ",p->reg[5],j)==2)     return(-1);
   if(showoff("PCH =       ",p->reg[6],j)==2)     return(-1);
   if(showoff("PCL =       ",p->reg[7],j)==2)     return(-1);
   if(showoff("Data_in =   ",p->reg[8],j)==2)     return(-1);
   if(showoff("A_bus =     ",p->bus[0],j)==2)     return(-1);
   if(showoff("B_bus =     ",p->bus[1],j)==2)     return(-1);
   if(showoff("F_bus =     ",p->bus[2],j)==2)     return(-1);
   if(showoff("Data_bus =  ",p->bus[3],j)==2)     return(-1);
   if(showoff("Addr_bus =  ",p->bus[4],j)==2)     return(-1);
   if(showoff("MPC =       ",p->mpc,j)==2)        return(-1);
   if(showoff("Cmicro =    ",p->cmicro,j)==2)     return(-1);
   if(showoff("Leds =      ",p->leds,j)==2)       return(-1);
   if(showoff("MicroAd =   ",q->micro_ad,j)==2)   return(-1);
   if(showoff("Constant =  ",q->constant,j)==2)   return(-1);
   if(showoff("Bop =       ",q->bop,j)==2)        return(-1);
   if(showoff("Count =     ",q->count,j)==2)      return(-1);
   if(showoff("OpConCtl =  ",q->op_con_ctl,j)==2) return(-1);
   if(showoff("AluSource = ",q->alu_source,j)==2) return(-1);
   if(showoff("AluDest =   ",q->alu_dest,j)==2)   return(-1);
   if(showoff("AdBus =     ",q->ad_bus,j)==2)     return(-1);
   if(showoff("AluFunc =   ",q->alu_func,j)==2)   return(-1);
   if(showoff("Cin =       ",q->cin,j)==2)        return(-1);
   if(showoff("IncPC =     ",q->inc_pc,j)==2)     return(-1);
   if(showoff("EnSR =      ",q->en_sr,j)==2)      return(-1);
   if(showoff("R_W =       ",q->r_w,j)==2)        return(-1);
   if(showoff("Vma =       ",q->vma,j)==2)        return(-1);
   if(*j==i) myprintf("ANY state\n");
   return(0);
}

/********************************************************************
*                       show_trace()                                *
*********************************************************************
*   Display the trace_data to the screen.
********************************************************************/

int show_trace()
{
   int i,j,first,last;
   char *head1="CsAdd ACC SR  SP   MAR  PC  DI ABus BBus FBus";
   char *head2=" DtBus AdBus MPC Cmicr* Leds Clock\n";
   char *head3=" Vma R_W EnSR IncPC Cin AluFunc AdBus AluDest";
   char *head4=" AluSrc OpConCtl Count Bop MicAd\n\n";

   first=trigptr;
   last=trigptr+step_count;
   myprintf("%s",head1);
   myprintf("%s",head2);
   myprintf("%s",head3);
   myprintf("%s",head4);

   for(i=first;i<last;i++)
   {
      j=i%TMAX;
    myprintf(" %04lx  %02lx %02lx %02lx%02lx %02lx%02lx %02lx%02lx %02lx",
        tr[j].mad,tr[j].reg[0],tr[j].reg[1],tr[j].reg[2],tr[j].reg[3],
        tr[j].reg[4],tr[j].reg[5],tr[j].reg[6],tr[j].reg[7],tr[j].reg[8]);
    myprintf("  %02lx   %02lx   %02lx    %02lx  %04lx  %04lx   %01lx    %02lx  %05ld\n",
        tr[j].bus[0],tr[j].bus[1],tr[j].bus[2],tr[j].bus[3],tr[j].bus[4],
        tr[j].mpc,tr[j].cmicro,tr[j].leds,tr[j].tick);

    load_mw((int)tr[j].mad);
    myprintf("  %01lx   %01lx    %01lx    %01lx    %01lx     %02lx     %01lx   ",
        mw.vma,mw.r_w,mw.en_sr,mw.inc_pc,mw.cin,mw.alu_func,mw.ad_bus);
    myprintf("   %01lx       %01lx       %01lx      %01lx    %01lx   %03lx\n\n",
        mw.alu_dest,mw.alu_source,mw.op_con_ctl,mw.count,mw.bop,mw.micro_ad);

      if((i-first)%6==5)
      {
         if(more()) return(0);
         myprintf("%s",head1);
         myprintf("%s",head2);
         myprintf("%s",head3);
         myprintf("%s",head4);
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
   int i,j,first,last;
   char *head1="CsAdd ACC SR  SP   MAR  PC  DI ABus BBus FBus";
   char *head2=" DtBus AdBus MPC Cmicr* Leds Clock\n";
   char *head3=" Vma R_W EnSR IncPC Cin AluFunc AdBus AluDest";
   char *head4=" AluSrc OpConCtl Count Bop MicAd\n\n";
   FILE *fp, *fopen();
   char fname[N], ch;

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
      fprintf(fp,"%s",head1);
      fprintf(fp,"%s",head2);
      fprintf(fp,"%s",head3);
      fprintf(fp,"%s",head4);

      for(i=first;i<last;i++)
      {
         j=i%TMAX;
 fprintf(fp," %04lx  %02lx %02lx %02lx%02lx %02lx%02lx %02lx%02lx %02lx",
       tr[j].mad,tr[j].reg[0],tr[j].reg[1],tr[j].reg[2],tr[j].reg[3],
       tr[j].reg[4],tr[j].reg[5],tr[j].reg[6],tr[j].reg[7],tr[j].reg[8]);
 fprintf(fp,"  %02lx   %02lx   %02lx    %02lx  %04lx  %04lx   %01lx    %02lx  %05ld\n",
       tr[j].bus[0],tr[j].bus[1],tr[j].bus[2],tr[j].bus[3],tr[j].bus[4],
       tr[j].mpc,tr[j].cmicro,tr[j].leds,tr[j].tick);

         load_mw((int)tr[j].mad);
 fprintf(fp,"  %01lx   %01lx    %01lx    %01lx    %01lx     %02lx     %01lx   ",
       mw.vma,mw.r_w,mw.en_sr,mw.inc_pc,mw.cin,mw.alu_func,mw.ad_bus);
 fprintf(fp,"   %01lx       %01lx       %01lx      %01lx    %01lx   %03lx\n\n",
       mw.alu_dest,mw.alu_source,mw.op_con_ctl,mw.count,mw.bop,mw.micro_ad);
      }
      fclose(fp);
      myprintf("File %s created.\n\n",fname);
   }
}

