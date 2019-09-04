/********************************************************************
*                          Trace4.c                                 *
*********************************************************************
*   Trace functions specific to board 4
********************************************************************/

#include "sim.h"
#include "micro4.h"
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
      tr[tndx].ir=ir;
      tr[tndx].mar=mar;
      tr[tndx].din=din;
      tr[tndx].dout=dout;
      tr[tndx].microstatus=microstatus;
      tr[tndx].macrostatus=macrostatus;
      tr[tndx].rbus=r_bus;
      tr[tndx].sbus=s_bus;
      tr[tndx].ybus=y_bus;
      tr[tndx].dbus=data_bus;
      tr[tndx].abus=address_bus;
      tr[tndx].mpc=current_index;
      tr[tndx].pitin=pitin;
      tr[tndx].pitout=pitout;
      tr[tndx].ready=ready;
      tr[tndx].busreq=busreq;
      tr[tndx].bgrant=bgrant;
      tr[tndx].vma=vma;
      tr[tndx].rw=rw;
      tr[tndx].iack=iack;
      tr[tndx].intreq=intreq;
      tr[tndx].ssf=ssf;
      tr[tndx].sio0=sio0;
      tr[tndx].sio8=sio8;
      tr[tndx].asel=asel;
      tr[tndx].bsel=bsel;
      tr[tndx].csel=csel;
      tr[tndx].cin=cin;
      tr[tndx].cc=cc;
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
   myint x = (ANY);

   clear_trace();

   tg.abus=tg.dbus=tg.rbus=tg.sbus=tg.ybus=x;
   tg.mar=tg.din=tg.ir=tg.dout=tg.tick=x;
   tg.mad=tg.cin=tg.mpc=tg.pitin=tg.pitout=x;
   tg.microstatus=tg.macrostatus=tg.cc=x;
   tg.ready=tg.busreq=tg.bgrant=tg.vma=tg.rw=tg.iack=x;
   tg.intreq=tg.ssf=tg.sio0=tg.sio8=tg.asel=tg.bsel=tg.csel=x;

   st.abus=st.dbus=st.rbus=st.sbus=st.ybus=x;
   st.mar=st.ir=st.din=st.dout=st.tick=x;
   st.mad=st.cin=st.mpc=st.pitin=st.pitout=x;
   st.microstatus=st.macrostatus=st.cc=x;
   st.ready=st.busreq=st.bgrant=st.vma=st.rw=st.iack=x;
   st.intreq=st.ssf=st.sio0=st.sio8=st.asel=st.bsel=st.csel=x;

   tgm.micro_ad=tgm.seq=tgm.rld=tgm.cond=tgm.sr_func=x;
   tgm.sr_source=tgm.a_source=tgm.b_source=tgm.c_source=x;
   tgm.we=tgm.alu_func=tgm.cin=tgm.shift_cond=tgm.mem_access=x;
   tgm.hibernate=tgm.ld_mar=tgm.ld_ir=tgm.ld_dout=x;

   stm.micro_ad=stm.seq=stm.rld=stm.cond=stm.sr_func=x;
   stm.sr_source=stm.a_source=stm.b_source=stm.c_source=x;
   stm.we=stm.alu_func=stm.cin=stm.shift_cond=stm.mem_access=x;
   stm.hibernate=stm.ld_mar=stm.ld_ir=stm.ld_dout=x;

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
   if((p->mad!= x)&&(p->mad!=last_index))                  return(FALSE);
   if((p->tick!= x)&&(p->tick!=timer))                     return(FALSE);
   if((p->ir!= x)&&(p->ir!=ir))                            return(FALSE);
   if((p->mar!= x)&&(p->mar!=mar))                         return(FALSE);
   if((p->din!= x)&&(p->din!=din))                         return(FALSE);
   if((p->dout!= x)&&(p->dout!=dout))                      return(FALSE);
   if((p->microstatus!= x)&&(p->microstatus!=microstatus)) return(FALSE);
   if((p->macrostatus!= x)&&(p->macrostatus!=macrostatus)) return(FALSE);
   if((p->rbus!= x)&&(p->rbus!=r_bus))                     return(FALSE);
   if((p->sbus!= x)&&(p->sbus!=s_bus))                     return(FALSE);
   if((p->ybus!= x)&&(p->ybus!=y_bus))                     return(FALSE);
   if((p->dbus!= x)&&(p->dbus!=data_bus))                  return(FALSE);
   if((p->abus!= x)&&(p->abus!=address_bus))               return(FALSE);
   if((p->mpc!= x)&&(p->mpc!=current_index))               return(FALSE);
   if((p->pitin!= x)&&(p->pitin!=pitin))                   return(FALSE);
   if((p->pitout!= x)&&(p->pitout!=pitout))                return(FALSE);
   if((p->ready!= x)&&(p->ready!=ready))                   return(FALSE);
   if((p->busreq!= x)&&(p->busreq!=busreq))                return(FALSE);
   if((p->bgrant!= x)&&(p->bgrant!=bgrant))                return(FALSE);
   if((p->vma!= x)&&(p->vma!=vma))                         return(FALSE);
   if((p->rw!= x)&&(p->rw!=rw))                            return(FALSE);
   if((p->iack!= x)&&(p->iack!=iack))                      return(FALSE);
   if((p->intreq!= x)&&(p->intreq!=intreq))                return(FALSE);
   if((p->ssf!= x)&&(p->ssf!=ssf))                         return(FALSE);
   if((p->sio0!= x)&&(p->sio0!=sio0))                      return(FALSE);
   if((p->sio8!= x)&&(p->sio8!=sio8))                      return(FALSE);
   if((p->asel!= x)&&(p->asel!=asel))                      return(FALSE);
   if((p->bsel!= x)&&(p->bsel!=bsel))                      return(FALSE);
   if((p->csel!= x)&&(p->csel!=csel))                      return(FALSE);
   if((p->cin!= x)&&(p->cin!=cin))                         return(FALSE);
   if((p->cc!= x)&&(p->cc!=cc))                            return(FALSE);
   if((q->micro_ad!= x)&&(q->micro_ad!=mw.micro_ad))       return(FALSE);
   if((q->seq!= x)&&(q->seq!=mw.seq))                      return(FALSE);
   if((q->rld!= x)&&(q->rld!=mw.rld))                      return(FALSE);
   if((q->cond!= x)&&(q->cond!=mw.cond))                   return(FALSE);
   if((q->sr_func!= x)&&(q->sr_func!=mw.sr_func))          return(FALSE);
   if((q->sr_source!= x)&&(q->sr_source!=mw.sr_source))    return(FALSE);
   if((q->a_source!= x)&&(q->a_source!=mw.a_source))       return(FALSE);
   if((q->b_source!= x)&&(q->b_source!=mw.b_source))       return(FALSE);
   if((q->c_source!= x)&&(q->c_source!=mw.c_source))       return(FALSE);
   if((q->we!= x)&&(q->we!=mw.we))                         return(FALSE);
   if((q->alu_func!= x)&&(q->alu_func!=mw.alu_func))       return(FALSE);
   if((q->cin!= x)&&(q->cin!=mw.cin))                      return(FALSE);
   if((q->shift_cond!= x)&&(q->shift_cond!=mw.shift_cond)) return(FALSE);
   if((q->mem_access!= x)&&(q->mem_access!=mw.mem_access)) return(FALSE);
   if((q->hibernate!= x)&&(q->hibernate!=mw.hibernate))    return(FALSE);
   if((q->ld_mar!= x)&&(q->ld_mar!=mw.ld_mar))             return(FALSE);
   if((q->ld_ir!= x)&&(q->ld_ir!=mw.ld_ir))                return(FALSE);
   if((q->ld_dout!= x)&&(q->ld_dout!=mw.ld_dout))          return(FALSE);
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
   myprintf("               ---------- Signal Values ----------              \n");
   myprintf("     1] Cs_Ad        9] IR          17] A_Select    25]R_Bus    \n");
   myprintf("     2] Clock       10] MAR         18] B_Select    26]S_Bus    \n");
   myprintf("     3] MPC         11] Data_In     19] C_Select    27]Y_Bus    \n");
   myprintf("     4] Switches    12] Data_Out    20] Cin         28]Data_Bus \n");
   myprintf("     5] Leds        13] Micro_Stat  21] CC          29]Addr_Bus \n");
   myprintf("     6] Int_Req     14] Macro_Stat  22] Sio_0       30]Bus_Req  \n");
   myprintf("     7] Int_Ack     15] VMA         23] Sio_8       31]Bus_Grant\n");
   myprintf("     8] Ready       16] R_W         24] SSF                     \n");
   myprintf("\n");
   myprintf("               --------- Microword Values --------              \n");
   myprintf("    32] Micro_Ad    37] SR_Func     42] ALU_Func    46]Hibernate\n");
   myprintf("    33] Seq         38] SR_Source   43] Cin         47]Ld_MAR   \n");
   myprintf("    34] RLD         39] A_Source    44] Shift_Cond  48]LD_IR    \n");
   myprintf("    35] Cond        40] B_Source    45] Mem_Access  49]LD_Dout  \n");
   myprintf("    36] WE          41] C_Source                                \n");
   myprintf("\n");
   myprintf("                           0] Exit (Done)                       \n");
   myprintf("\n");
   myprintf("Your selection [0-49]? "); lnl;
   mwfield = get_dec();
   while(mwfield < 0 || mwfield > 49)
   {
      myprintf("Please choose a number between 0 and 49: "); lnl;
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
	case 0:  return(0);                                         break;
	case 1:  requestit("Cs_ad = ",&(p->mad),0xfffL);            break;
	case 2:  requestit("Clock = ",&(p->tick),9999L);            break;
	case 9:  requestit("IR = ",&(p->ir),0xffffL);               break;
	case 10: requestit("MAR = ",&(p->mar),0xffffL);             break;
	case 11: requestit("DataIn = ",&(p->din),0xffffL);          break;
	case 12: requestit("DataOut = ",&(p->dout),0xffffL);        break;
	case 13: requestit("MicroStat = ",&(p->microstatus),0x1fL); break;
	case 14: requestit("MacroStat = ",&(p->macrostatus),0x3fL); break;
	case 25: requestit("R_Bus = ",&(p->rbus),0xffffL);          break;
	case 26: requestit("S_Bus = ",&(p->sbus),0xffffL);          break;
	case 27: requestit("Y_Bus = ",&(p->ybus),0xffffL);          break;
	case 28: requestit("Data_Bus = ",&(p->dbus),0xffffL);       break;
	case 29: requestit("Addr_Bus = ",&(p->abus),0xffffL);       break;
	case 3:  requestit("MPC = ",&(p->mpc),0xfffL);              break;
	case 4:  requestit("Switches = ",&(p->pitin),0xffL);        break;
	case 5:  requestit("Leds = ",&(p->pitout),0xffL);           break;
	case 8:  requestit("Ready = ",&(p->ready),1L);              break;
	case 30: requestit("BusReq = ",&(p->busreq),1L);            break;
	case 31: requestit("BusGrant = ",&(p->bgrant),1L);          break;
	case 15: requestit("VMA Line = ",&(p->vma),1L);             break;
	case 16: requestit("R_W Line = ",&(p->rw),1L);              break;
	case 7:  requestit("IntAck = ",&(p->iack),1L);              break;
	case 6:  requestit("IntReq = ",&(p->intreq),1L);            break;
	case 24: requestit("SSF Line = ",&(p->ssf),1L);             break;
	case 22: requestit("Sio0 Line = ",&(p->sio0),1L);           break;
	case 23: requestit("Sio8 Line = ",&(p->sio8),1L);           break;
	case 17: requestit("A_Select = ",&(p->asel),0x0fL);         break;
	case 18: requestit("B_Select = ",&(p->bsel),0x0fL);         break;
	case 19: requestit("C_Select = ",&(p->csel),0x0fL);         break;
	case 20: requestit("Cin Line = ",&(p->cin),1L);             break;
	case 21: requestit("CC Line = ",&(p->cc),1L);               break;
	case 32: requestit("MicroAd = ",&(q->micro_ad),0xfffL);     break;
	case 33: requestit("Seq = ",&(q->seq),0x0fL);               break;
	case 34: requestit("RLD = ",&(q->rld),1L);                  break;
	case 35: requestit("Cond = ",&(q->cond),0x1fL);             break;
	case 37: requestit("SR_Func = ",&(q->sr_func),3L);          break;
	case 38: requestit("SR_Source = ",&(q->sr_source),1L);      break;
	case 39: requestit("A_Source = ",&(q->a_source),0x3fL);     break;
	case 40: requestit("B_Source = ",&(q->b_source),0x7fL);     break;
	case 41: requestit("C_Source = ",&(q->c_source),0x1fL);     break;
	case 36: requestit("WE = ",&(q->we),1L);                    break;
	case 42: requestit("AluFunc = ",&(q->alu_func),0x1ffL);     break;
	case 43: requestit("Cin = ",&(q->cin),3L);                  break;
	case 44: requestit("ShiftCond = ",&(q->shift_cond),7L);     break;
	case 45: requestit("MemAccess = ",&(q->mem_access),3L);     break;
	case 46: requestit("Hibernate = ",&(q->hibernate),1L);      break;
	case 47: requestit("Ld_MAR = ",&(q->ld_mar),1L);            break;
	case 48: requestit("Ld_IR = ",&(q->ld_ir),1L);              break;
	case 49: requestit("Ld_Dout = ",&(q->ld_dout),1L);          break;
        default:                                                    break;
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
   int i = (*j);
   if(showoff("Cs_ad =     ",p->mad,j)==2)         return(-1);
   if(showoff("Clock =     ",p->tick,j)==2)        return(-1);
   if(showoff("IR =        ",p->ir,j)==2)          return(-1);
   if(showoff("MAR =       ",p->mar,j)==2)         return(-1);
   if(showoff("DataIn =    ",p->din,j)==2)         return(-1);
   if(showoff("DataOut =   ",p->dout,j)==2)        return(-1);
   if(showoff("MicroStat = ",p->microstatus,j)==2) return(-1);
   if(showoff("MacroStat = ",p->macrostatus,j)==2) return(-1);
   if(showoff("R_Bus =     ",p->rbus,j)==2)        return(-1);
   if(showoff("S_Bus =     ",p->sbus,j)==2)        return(-1);
   if(showoff("Y_Bus =     ",p->ybus,j)==2)        return(-1);
   if(showoff("Data_Bus =  ",p->dbus,j)==2)        return(-1);
   if(showoff("Addr_Bus =  ",p->abus,j)==2)        return(-1);
   if(showoff("MPC =       ",p->mpc,j)==2)         return(-1);
   if(showoff("Switches =  ",p->pitin,j)==2)       return(-1);
   if(showoff("Leds =      ",p->pitout,j)==2)      return(-1);
   if(showoff("Ready =     ",p->ready,j)==2)       return(-1);
   if(showoff("BusReq =    ",p->busreq,j)==2)      return(-1);
   if(showoff("BusGrant =  ",p->bgrant,j)==2)      return(-1);
   if(showoff("VMA Line =  ",p->vma,j)==2)         return(-1);
   if(showoff("R_W Line =  ",p->rw,j)==2)          return(-1);
   if(showoff("IntAck =    ",p->iack,j)==2)        return(-1);
   if(showoff("IntReq =    ",p->intreq,j)==2)      return(-1);
   if(showoff("SSF Line =  ",p->ssf,j)==2)         return(-1);
   if(showoff("Sio0 Line = ",p->sio0,j)==2)        return(-1);
   if(showoff("Sio8 Line = ",p->sio8,j)==2)        return(-1);
   if(showoff("A_Select =  ",p->asel,j)==2)        return(-1);
   if(showoff("B_Select =  ",p->bsel,j)==2)        return(-1);
   if(showoff("C_Select =  ",p->csel,j)==2)        return(-1);
   if(showoff("Cin Line =  ",p->cin,j)==2)         return(-1);
   if(showoff("CC Line =   ",p->cc,j)==2)          return(-1);
   if(showoff("MicroAd =   ",q->micro_ad,j)==2)    return(-1);
   if(showoff("Seq =       ",q->seq,j)==2)         return(-1);
   if(showoff("RLD =       ",q->rld,j)==2)         return(-1);
   if(showoff("Cond =      ",q->cond,j)==2)        return(-1);
   if(showoff("SR_Func =   ",q->sr_func,j)==2)     return(-1);
   if(showoff("SR_Source = ",q->sr_source,j)==2)   return(-1);
   if(showoff("A_Source =  ",q->a_source,j)==2)    return(-1);
   if(showoff("B_Source =  ",q->b_source,j)==2)    return(-1);
   if(showoff("C_Source =  ",q->c_source,j)==2)    return(-1);
   if(showoff("WE =        ",q->we,j)==2)          return(-1);
   if(showoff("AluFunc =   ",q->alu_func,j)==2)    return(-1);
   if(showoff("Cin =       ",q->cin,j)==2)         return(-1);
   if(showoff("ShiftCond = ",q->shift_cond,j)==2)  return(-1);
   if(showoff("MemAccess = ",q->mem_access,j)==2)  return(-1);
   if(showoff("Hibernate = ",q->hibernate,j)==2)   return(-1);
   if(showoff("Ld_MAR =    ",q->ld_mar,j)==2)      return(-1);
   if(showoff("Ld_IR =     ",q->ld_ir,j)==2)       return(-1);
   if(showoff("Ld_Dout =   ",q->ld_dout,j)==2)     return(-1);
   if(*j == i) myprintf("ANY state\n");
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
   char *head1="CsAdS  IR   MAR DtIn DtOut MicSt MacSt RBus SBus";
   char *head2=" YBus DtBus AdBus CsAdF Clock\n";
   char *head3="PitIn PitOut Rdy* Brq* Bgt Vma* R/W* Iak* Int SSF Sio0*/8*";
   char *head4=" A/B/C_Sel Cin CC*\n";
   char *head5="LdDO/IR/MAR Hib MemAc ShCnd Cin AlFun WE";
   char *head6=" C/B/ASrc SSrc SFun Cnd RLd Seq MicAd\n\n";

   first=trigptr;
   last=trigptr+step_count;
   myprintf("%s%s%s%s%s%s",head1,head2,head3,head4,head5,head6);

   for(i=first;i<last;i++)
   {
      j=i%TMAX;
    myprintf(" %04lx %04lx %04lx %04lx  %04lx   %02lx    %02lx  %04lx %04lx",
        tr[j].mad,tr[j].ir,tr[j].mar,tr[j].din,tr[j].dout,
        tr[j].microstatus,tr[j].macrostatus,tr[j].rbus,tr[j].sbus);
    myprintf(" %04lx  %04lx  %04lx  %04lx %04ld\n",
        tr[j].ybus,tr[j].dbus,tr[j].abus,tr[j].mpc,tr[j].tick);
    myprintf("  %02lx    %02lx    %01lx    %01lx    %01lx   %01lx    %01lx    %01lx    %01lx   %01lx     %01lx",
        tr[j].pitin,tr[j].pitout,tr[j].ready,tr[j].busreq,tr[j].bgrant,tr[j].vma,
        tr[j].rw,tr[j].iack,tr[j].intreq,tr[j].ssf,tr[j].sio0);
    myprintf(" %01lx     %01lx %01lx %01lx    %01lx   %01lx\n",
        tr[j].sio8,tr[j].asel,tr[j].bsel,tr[j].csel,tr[j].cin,tr[j].cc);

    load_mw((int)tr[j].mad);
   myprintf("   %01lx  %01lx  %01lx   %01lx    %01lx     %01lx    %01lx   %03lx  %01lx ",
             mw.ld_dout,mw.ld_ir,mw.ld_mar,mw.hibernate,mw.mem_access,
             mw.shift_cond,mw.cin,mw.alu_func,mw.we);
   myprintf(" %02lx %02lx %02lx   %01lx    %01lx   %02lx  %01lx   %01lx   %03lx\n\n",
             mw.c_source,mw.b_source,mw.a_source,mw.sr_source,mw.sr_func,
             mw.cond,mw.rld,mw.seq,mw.micro_ad);

      if((i-first)%4==3)
      {
         if(more()) return(0);
         if(i<(last-1))
           myprintf("%s%s%s%s%s%s",head1,head2,head3,head4,head5,head6);
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
   FILE *fp, *fopen();
   char fname[N], ch;
   char *head1="CsAddS IR   MAR DtIn DtOut MicSt MacSt RBus SBus";
   char *head2=" YBus DtBus AdBus CsAdF Clock\n";
   char *head3="PitIn PitOut Rdy* Brq* Bgt Vma* R/W* Iak* Int SSF Sio0*/8*";
   char *head4=" A/B/C_Sel Cin CC*\n";
   char *head5="LdDO/IR/MAR Hib MemAc ShCnd Cin AlFun WE";
   char *head6=" C/B/ASrc SSrc SFun Cnd RLd Seq MicAd\n\n";

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
      fprintf(fp,"%s%s%s%s%s%s",head1,head2,head3,head4,head5,head6);

      for(i=first;i<last;i++)
      {
      j=i%TMAX;

   fprintf(fp," %04lx %04lx %04lx %04lx  %04lx   %02lx    %02lx  %04lx %04lx",
        tr[j].mad,tr[j].ir,tr[j].mar,tr[j].din,tr[j].dout,
        tr[j].microstatus,tr[j].macrostatus,tr[j].rbus,tr[j].sbus);
   fprintf(fp," %04lx  %04lx  %04lx  %04lx %04ld\n",
        tr[j].ybus,tr[j].dbus,tr[j].abus,tr[j].mpc,tr[j].tick);
  fprintf(fp,"  %02lx    %02lx    %01lx    %01lx    %01lx   %01lx    %01lx    %01lx    %01lx   %01lx     %01lx",
        tr[j].pitin,tr[j].pitout,tr[j].ready,tr[j].busreq,tr[j].bgrant,tr[j].vma,
        tr[j].rw,tr[j].iack,tr[j].intreq,tr[j].ssf,tr[j].sio0);
   fprintf(fp," %01lx     %01lx %01lx %01lx    %01lx   %01lx\n",
        tr[j].sio8,tr[j].asel,tr[j].bsel,tr[j].csel,tr[j].cin,tr[j].cc);

    load_mw((int)tr[j].mad);
  fprintf(fp,"   %01lx  %01lx  %01lx   %01lx    %01lx     %01lx    %01lx   %03lx  %01lx ",
             mw.ld_dout,mw.ld_ir,mw.ld_mar,mw.hibernate,mw.mem_access,
             mw.shift_cond,mw.cin,mw.alu_func,mw.we);
  fprintf(fp," %02lx %02lx %02lx   %01lx    %01lx   %02lx  %01lx   %01lx   %03lx\n\n",
             mw.c_source,mw.b_source,mw.a_source,mw.sr_source,mw.sr_func,
             mw.cond,mw.rld,mw.seq,mw.micro_ad);
      }
      fclose(fp);
      myprintf("File %s created.\n\n",fname);
   }
}

