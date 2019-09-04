/********************************************************************
*                          Trace2.c                                 *
*********************************************************************
*   Trace functions specific to board 2
********************************************************************/

#include "sim.h"
#include "micro2.h"
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
      tr[tndx].insw[0]=ia_reg;
      tr[tndx].insw[1]=ib_reg;
      tr[tndx].insw[2]=op_code;
      tr[tndx].reg[0]=ta_reg;
      tr[tndx].reg[1]=tb_reg;
      tr[tndx].reg[2]=f_reg;
      tr[tndx].bus[0]=a_bus;
      tr[tndx].bus[1]=b_bus;
      tr[tndx].bus[2]=f_bus;
      tr[tndx].mpc=current_index;
      tr[tndx].bit[0]=z_bit;
      tr[tndx].bit[1]=n_bit;
      tr[tndx].bit[2]=c4_bit;
      tr[tndx].bit[3]=c8_bit;
      tr[tndx].bit[4]=go_button;
      tr[tndx].bit[5]=zp_bit;
      tr[tndx].rep=logical_data;
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
   for(i=0;i<3;i++)
   {
      tg.insw[i] = x;
      st.insw[i] = x;
      tg.reg[i] = x;
      st.reg[i] = x;
      tg.bus[i] = x;
      st.bus[i] = x;
   }
   tg.mpc = x;
   st.mpc = x;
   tgm.micro_ad = x;
   tgm.bop = x;
   tgm.count = x;
   tgm.a_source = x;
   tgm.b_source = x;
   tgm.alu_dest = x;
   tgm.alu_func = x;
   tgm.cin = x;
   stm.micro_ad = x;
   stm.bop = x;
   stm.count = x;
   stm.a_source = x;
   stm.b_source = x;
   stm.alu_dest = x;
   stm.alu_func = x;
   stm.cin = x;
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
   if((p->mad!= x)&&(p->mad!=last_index))             return(FALSE);
   if((p->tick!= x)&&(p->tick!=timer))                return(FALSE);
   if((p->insw[0]!= x)&&(p->insw[0]!=ia_reg))         return(FALSE);
   if((p->insw[1]!= x)&&(p->insw[1]!=ib_reg))         return(FALSE);
   if((p->insw[2]!= x)&&(p->insw[2]!=op_code))        return(FALSE);
   if((p->reg[0]!= x)&&(p->reg[0]!=ta_reg))           return(FALSE);
   if((p->reg[1]!= x)&&(p->reg[1]!=tb_reg))           return(FALSE);
   if((p->reg[2]!= x)&&(p->reg[2]!=f_reg))            return(FALSE);
   if((p->bus[0]!= x)&&(p->bus[0]!=a_bus))            return(FALSE);
   if((p->bus[1]!= x)&&(p->bus[1]!=b_bus))            return(FALSE);
   if((p->bus[2]!= x)&&(p->bus[2]!=f_bus))            return(FALSE);
   if((p->mpc!= x)&&(p->mpc!=current_index))          return(FALSE);
   if((q->micro_ad != x)&&(q->micro_ad!=mw.micro_ad)) return(FALSE);
   if((q->bop != x)&&(q->bop!=mw.bop))                return(FALSE);
   if((q->count != x)&&(q->count!=mw.count))          return(FALSE);
   if((q->a_source != x)&&(q->a_source!=mw.a_source)) return(FALSE);
   if((q->b_source != x)&&(q->b_source!=mw.b_source)) return(FALSE);
   if((q->alu_dest != x)&&(q->alu_dest!=mw.alu_dest)) return(FALSE);
   if((q->alu_func != x)&&(q->alu_func!=mw.alu_func)) return(FALSE);
   if((q->cin != x)&&(q->cin!=mw.cin))                return(FALSE);
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
   myprintf("     1] Cs_Ad        4] IA_Reg       7] TA_Reg      10]A_Bus    \n");
   myprintf("     2] Clock        5] IB_Reg       8] TB_Reg      11]B_Bus    \n");
   myprintf("     3] MPC          6] OP_Code      9] F_Reg       12]F_Bus    \n");
   myprintf("\n");
   myprintf("               --------- Microword Values --------              \n");
   myprintf("    13] Micro_Ad    15] Count       17] Bop         19]Cin      \n");
   myprintf("    14] A_Source    16] B_Source    18] ALU_Func    20]ALU_Dest \n");
   myprintf("\n");
   myprintf("                           0] Exit (Done)                       \n");
   myprintf("\n");
   myprintf("Your selection [0-20]? "); lnl;
   mwfield = get_dec();
   while(mwfield < 0 || mwfield > 20)
   {
      myprintf("Please choose a number between 0 and 20: "); lnl;
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
	case 0:  return(0);                                    break;
	case 1:  requestit("Cs_ad = ",&(p->mad),0xffL);        break;
	case 2:  requestit("Clock = ",&(p->tick),9999L);       break;
	case 4:  requestit("IA_reg = ",&(p->insw[0]),0xffL);   break;
	case 5:  requestit("IB_reg = ",&(p->insw[1]),0xffL);   break;
	case 6:  requestit("OP_Code = ",&(p->insw[2]),0x0fL);  break;
	case 7:  requestit("TA_reg = ",&(p->reg[0]),0xffL);    break;
	case 8:  requestit("TB_reg = ",&(p->reg[1]),0xffL);    break;
	case 9:  requestit("F_reg = ",&(p->reg[2]),0xffL);     break;
	case 10: requestit("A_bus = ",&(p->bus[0]),0xffL);     break;
	case 11: requestit("B_bus = ",&(p->bus[1]),0xffL);     break;
	case 12: requestit("F_bus = ",&(p->bus[2]),0xffL);     break;
	case 3:  requestit("MPC = ",&(p->mpc),0xffL);          break;
	case 13: requestit("MicroAd = ",&(q->micro_ad),0xffL); break;
	case 15: requestit("Count = ",&(q->count),1L);         break;
	case 17: requestit("Bop = ",&(q->bop),0x0fL);          break;
	case 14: requestit("ASource = ",&(q->a_source),1L);    break;
	case 16: requestit("BSource = ",&(q->b_source),1L);    break;
	case 18: requestit("AluFunc = ",&(q->alu_func),0x1fL); break;
	case 19: requestit("Cin = ",&(q->cin),1L);             break;
	case 20: requestit("AluDest = ",&(q->alu_dest),0x07L); break;
        default:                                               break;
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
   int i= (*j);
   if(showoff("Cs_ad =     ",p->mad,j)==2)      return(-1);
   if(showoff("Clock =     ",p->tick,j)==2)     return(-1);
   if(showoff("IA_reg =    ",p->insw[0],j)==2)  return(-1);
   if(showoff("IB_reg =    ",p->insw[1],j)==2)  return(-1);
   if(showoff("OP_Code =   ",p->insw[2],j)==2)  return(-1);
   if(showoff("TA_reg =    ",p->reg[0],j)==2)   return(-1);
   if(showoff("TB_reg =    ",p->reg[1],j)==2)   return(-1);
   if(showoff("F-reg =     ",p->reg[2],j)==2)   return(-1);
   if(showoff("A_bus =     ",p->bus[0],j)==2)   return(-1);
   if(showoff("B_bus =     ",p->bus[1],j)==2)   return(-1);
   if(showoff("F_bus =     ",p->bus[2],j)==2)   return(-1);
   if(showoff("MPC =       ",p->mpc,j)==2)      return(-1);
   if(showoff("MicroAd =   ",q->micro_ad,j)==2) return(-1);
   if(showoff("Count =     ",q->count,j)==2)    return(-1);
   if(showoff("Bop =       ",q->bop,j)==2)      return(-1);
   if(showoff("ASource =   ",q->a_source,j)==2) return(-1);
   if(showoff("BSource =   ",q->b_source,j)==2) return(-1);
   if(showoff("AluFunc =   ",q->alu_func,j)==2) return(-1);
   if(showoff("Cin =       ",q->cin,j)==2)      return(-1);
   if(showoff("AluDest =   ",q->alu_dest,j)==2) return(-1);
   if(showoff("MicroAd =   ",q->micro_ad,j)==2) return(-1);
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
   char bit[6];
   int i,j,k,first,last;
   char *head1="CsAdd  IA IB OpCode  TA TB  F  ABus BBus FBus  ";
   char *head2="Z N C4 C8 GO ZP  MPC  Clock\n";
   char *head3=" AluDest Cin AluFunc BSource ASource Bop Count MicroAd\n\n";
   char *head4a=" [logical]\n\n";
   char *head4b=" [1=HIGH] \n\n";

   first=trigptr;
   last=trigptr+step_count;
   myprintf("%s",head1);
   myprintf("%s",head2);
   myprintf("%s",head3);

   for(i=first;i<last;i++)
   {
      j=i%TMAX;

      if(tr[j].rep)
      {
        myprintf(" %03lx   %02lx %02lx    %01lx    ",
          tr[j].mad,(~tr[j].insw[0])&0xffL,
          (~tr[j].insw[1])&0xffL,(tr[j].insw[2])&0xfL);
        myprintf("%02lx %02lx %02lx   %02lx   %02lx   %02lx",
          (~tr[j].reg[0])&0xffL,(~tr[j].reg[1])&0xffL,(~tr[j].reg[2])&0xffL,
          (~tr[j].bus[0])&0xffL,(~tr[j].bus[1])&0xffL,(~tr[j].bus[2])&0xffL);
      }
      else
      {
        myprintf(" %03lx   %02lx %02lx    %01lx    ",
          tr[j].mad,(tr[j].insw[0])&0xffL,
          (tr[j].insw[1])&0xffL,(tr[j].insw[2])&0xfL);
        myprintf("%02lx %02lx %02lx   %02lx   %02lx   %02lx",
          (tr[j].reg[0])&0xffL,(tr[j].reg[1])&0xffL,(tr[j].reg[2])&0xffL,
          (tr[j].bus[0])&0xffL,(tr[j].bus[1])&0xffL,(tr[j].bus[2])&0xffL);
      }
      for(k=0;k<6;k++)
      {
         bit[k] = (tr[j].bit[k]==1)?     'H':'L';
      }
      myprintf("   %c %c  %c  %c  %c  %c   %02lx  %05ld\n",
         bit[0],bit[1],bit[2],bit[3],bit[4],bit[5],tr[j].mpc,tr[j].tick);

      load_mw((int)tr[j].mad);
      myprintf("    %01lx     %01lx     %02lx      %01lx       %01lx     %01lx",
        mw.alu_dest,mw.cin,mw.alu_func,mw.b_source,mw.a_source,mw.bop);
      myprintf("    %01lx     %03lx    ",mw.count,mw.micro_ad);
      if(tr[j].rep)
         myprintf("%s",head4a);
      else
         myprintf("%s",head4b);

      if((i-first)%6==5)
      {
         if(more()) return(0);
         myprintf("%s",head1);
         myprintf("%s",head2);
         myprintf("%s",head3);
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
   FILE *fp, *fopen();
   char fname[N], ch;

   char bit[6];
   int i,j,k,first,last;
   char *head1="CsAdd  IA IB OpCode  TA TB  F  ABus BBus FBus  ";
   char *head2="Z N C4 C8 GO ZP  MPC  Clock\n";
   char *head3=" AluDest Cin AluFunc BSource ASource Bop Count MicroAd\n\n";
   char *head4a=" [logical]\n\n";
   char *head4b=" [1=HIGH] \n\n";

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

      for(i=first;i<last;i++)
      {
         j=i%TMAX;

         if(tr[j].rep)
         {
           fprintf(fp," %03lx   %02lx %02lx    %01lx    ",
             tr[j].mad,(~tr[j].insw[0])&0xffL,
             (~tr[j].insw[1])&0xffL,(tr[j].insw[2])&0xfL);
           fprintf(fp,"%02lx %02lx %02lx   %02lx   %02lx   %02lx",
             (~tr[j].reg[0])&0xffL,(~tr[j].reg[1])&0xffL,(~tr[j].reg[2])&0xffL,
             (~tr[j].bus[0])&0xffL,(~tr[j].bus[1])&0xffL,(~tr[j].bus[2])&0xffL);
         }
         else
         {
           fprintf(fp," %03lx   %02lx %02lx    %01lx    ",
             tr[j].mad,(tr[j].insw[0])&0xffL,
             (tr[j].insw[1])&0xffL,(tr[j].insw[2])&0xfL);
           fprintf(fp,"%02lx %02lx %02lx   %02lx   %02lx   %02lx",
             (tr[j].reg[0])&0xffL,(tr[j].reg[1])&0xffL,(tr[j].reg[2])&0xffL,
             (tr[j].bus[0])&0xffL,(tr[j].bus[1])&0xffL,(tr[j].bus[2])&0xffL);
         }

         for(k=0;k<6;k++)
         {
              bit[k] = (tr[j].bit[k]==1)?     'H':'L';
         }

         fprintf(fp,"   %c %c  %c  %c  %c  %c   %02lx  %05ld\n",
              bit[0],bit[1],bit[2],bit[3],bit[4],bit[5],tr[j].mpc,tr[j].tick);

         load_mw((int)tr[j].mad);
         fprintf(fp,"    %01lx     %01lx     %02lx      %01lx       %01lx     %01lx",
             mw.alu_dest,mw.cin,mw.alu_func,mw.b_source,mw.a_source,mw.bop);
         fprintf(fp,"    %01lx     %03lx    ", mw.count,mw.micro_ad);
         if(tr[j].rep)
              fprintf(fp,"%s",head4a);
         else
              fprintf(fp,"%s",head4b);
      }
      fclose(fp);
      myprintf("File %s created.\n\n",fname);
   }
}

