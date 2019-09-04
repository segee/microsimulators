/**********************************************************************
*                             Logic4.c                                *
***********************************************************************
*   Contains the alu and sequencing logic for board 4.
***********************************************************************/


#include "sim.h"
#include "micro4.h"

myunsigned temp;
int dbit,qbt,firstqbt,divsgn;

/**********************************************************************
*                           sequencer() 
***********************************************************************
*   Performs the microsequencer logic.
**********************************************************************/

int sequencer()
{
   switch(ms.inst)
   {
      case 0:  ms.sp=0;                                /* JZ   */
               current_index=0;
               break;

      case 1:  if(cc==1)                               /* CJS  */
                  current_index=ms.mpc;
               else
               {
                  current_index=ms.data;
                  if(ms.sp==9) ms.sp--;
                  ms.stack[ms.sp++]=ms.mpc;
               }
               break;

      case 2:  current_index=ms.data;                  /* JMAP */
               break;

      case 3:  if(cc==1)                               /* CJP  */
                  current_index=ms.mpc;
               else
                  current_index=ms.data;
               break;

      case 4:  if(cc==0)                               /* PUSH */
                  ms.rc=ms.data;
               if(ms.sp==9) ms.sp--;
               ms.stack[ms.sp++]=ms.mpc;
               current_index=ms.mpc;
               break;

      case 5:  if(cc==1)                               /* JSRP */
                  current_index=ms.rc;
               else
                  current_index=ms.data;
               if(ms.sp==9) ms.sp--;
               ms.stack[ms.sp++]=ms.mpc;
               break;

      case 6:  if(cc==1)                               /* CJV  */
                  current_index=ms.mpc;
               else
                  current_index=ms.data;
               break;

      case 7:  if(cc==1)                               /* JRP  */
                  current_index=ms.rc;
               else
                  current_index=ms.data;
               break;

      case 8:  if(ms.rc!=0)                            /* RFCT */
               {
                  ms.rc--;
                  current_index=ms.stack[ms.sp];
               }
               else
               {
                  current_index=ms.mpc;
                  if(ms.sp!=0) ms.sp--;
               }
               break;

      case 9:  if(ms.rc!=0)                            /* RPCT */
               {
                  ms.rc--;
                  current_index=ms.data;
               }
               else
                  current_index=ms.mpc;
               break;

      case 10: if(cc==1)                               /* CRTN */
                  current_index=ms.mpc;
               else
               {
                  if(ms.sp!=0) ms.sp--;
                  current_index=ms.stack[ms.sp];
               }
               break;

      case 11: if(cc==1)                               /* CJPP */
                  current_index=ms.mpc;
               else
               {
                  current_index=ms.data;
                  if(ms.sp!=0) ms.sp--;
               }
               break;

      case 12: ms.rc=ms.data;                          /* LDCT */
               current_index=ms.mpc;
               break;

      case 13: if(cc==1)                               /* LOOP */
                  current_index=ms.stack[ms.sp];
               else
               {
                  current_index=ms.mpc;
                  if(ms.sp!=0) ms.sp--;
               }
               break;

      default:
      case 14: current_index=ms.mpc;                   /* CONT */
               break;

      case 15: if(ms.rc!=0)                            /* TWB  */
               {
                  ms.rc--;
                  if(cc==1)
                     current_index=ms.stack[ms.sp];
                  else
                  {
                     current_index=ms.mpc;
                     if(ms.sp!=0) ms.sp--;
                  }
               }
               else
               {
                  if(ms.sp!=0) ms.sp--;
                  if(cc==1)
                     current_index=ms.data;
                  else
                     current_index=ms.mpc;
               }
               break;
   }
   full=(ms.sp==9)? 0:1;
   if(mw.rld==0) ms.rc=ms.data;
   ms.mpc=current_index+1;
}

/************************************************************
*          setz()       set the z bit
*************************************************************/

int setz(bus)
myunsigned bus;
{
   zbit=(bus)? 0:1;
   return(zbit);
}

/************************************************************
*          setn()        set the n bit
************************************************************/

int setn(bus)
myunsigned bus;
{
   nbit=(bus&0x8000)? 1:0;
   return(nbit);
}

/************************************************************
*          setc()        set the c bit
************************************************************/

int setc(temp1)
myint temp1;
{
   cbit=(temp1&0x10000)? 1:0;
   return(cbit);
}

/************************************************************
*          setv1()        set the v bit (1 arg,shift)
************************************************************/

int setv1(bus1)
myunsigned bus1;
{
   vbit=(((bus1&0x4000)<<1)!=(bus1&0x8000))? 1:0;
   return(vbit);
}

/************************************************************
*          setv()        set the v bit (3 args)
************************************************************/

int setv(bus1,bus2,bus3)
myunsigned bus1,bus2,bus3;
{
   vbit=(((bus1&0x8000)==(bus2&0x8000))
           &&((bus1&0x8000)!=(bus3&0x8000)))? 1:0;
   return(vbit);
}


/************************************************************
*   group1()  alu group 1 instructions
************************************************************/

int group1()
{
   myunsigned x,w;

   switch(mw.alu_func&0x0f)           /* arithmetic & logic */
   {
      case 1: temp=r_bus+s_bus+cin;                  /* ADD */
              y_bus=temp&0xffff;
              setv(r_bus,s_bus,y_bus);
              break;

      case 2: temp=(~r_bus&0xffff)+s_bus+cin;       /* SUBR */
              y_bus=temp&0xffff;
              setv(~r_bus,s_bus,y_bus);
              break;

      case 3: temp=(~s_bus&0xffff)+r_bus+cin;       /* SUBS */
              y_bus=temp&0xffff;
              setv(r_bus,~s_bus,y_bus);
              break;

      case 4: temp=s_bus+cin;                       /* INCS */
              y_bus=temp&0xffff;
              setv(0,s_bus,y_bus);
              break;

      case 5: temp=(~s_bus&0xffff)+cin;            /* INCNS */
              y_bus=temp&0xffff;
              setv(0,~s_bus,y_bus);
              break;

      case 6: temp=r_bus+cin;                       /* INCR */
              y_bus=temp&0xffff;
              setv(0,r_bus,y_bus);
              break;

      case 7: temp=(~r_bus&0xffff)+cin;            /* INCNR */
              y_bus=temp&0xffff;
              setv(0,~r_bus,y_bus);
              break;

      case 9: y_bus=r_bus^s_bus;                     /* XOR */
              break;

     case 10: y_bus=r_bus&s_bus;                     /* AND */
              break;

     case 11: y_bus=r_bus|s_bus;                      /* OR */
              break;

     case 12: y_bus=~(r_bus&s_bus);                 /* NAND */
              break;

     case 13: y_bus=~(r_bus|s_bus);                  /* NOR */
              break;

     case 14: y_bus=s_bus&(~r_bus);                /* ANDNR */
              break;

     default:              /* group 3, 4, or 5 instructions */
              break;
   }

   setc(temp);                /* set carry bit before shift */

   switch((mw.alu_func>>4)&0x0f)       /* shift as required */
   {
      case 0: if(ssf==1)                             /* SRA */
              {
                 sio8=(y_bus&0x100)? 0:1;
                 sio0=(y_bus&1)? 0:1;
                 y_bus=y_bus>>1;
                 if(vbit==0)                     /*sign xtnd*/
                    y_bus=y_bus|((y_bus&0x4000)<1);
                 else
                    y_bus=y_bus&(~((y_bus&0x4000)<1));
              }
              break;

      case 1: if(ssf==1)                            /* SRAD */
              {
                 sio0=(mq&1)? 0:1;
                 x=(y_bus&1)<<15;
                 mq=((mq>>1)|x)&0xffff;
                 sio8=(y_bus&0x100)? 0:1;
                 y_bus=y_bus>>1;
                 if(vbit==0)                     /*sign xtnd*/
                    y_bus=y_bus|((y_bus&0x4000)<1);
                 else
                    y_bus=y_bus&(~((y_bus&0x4000)<1));
               }
               break;

      case 2: if(ssf==1)                             /* SRL */
              {
                 sio8=(y_bus&0x100)? 0:1;
                 y_bus=y_bus>>1;
                 sio0=1;                      /* set high Z */
              }
              break;

      case 3: if(ssf==1)                            /* SRLD */
              {
                 x=(y_bus&1)? 0x8000:0;
                 mq=((mq>>1)|x)&0xffff;
                 sio8=(y_bus&0x100)? 0:1;
                 y_bus=y_bus>>1;
                 sio0=1;                      /* set high Z */
              }
              break;

      case 4: if(ssf==1)                             /* SLA */
              {
                 if(vbit==0)
                    setv1(y_bus);
                 sio8=(y_bus&0x80)? 0:1;
                 y_bus=y_bus<<1;
                 sio0=1;                          /* high Z */
              }
              break;

      case 5: if(ssf==1)                            /* SLAD */
              {
                 if(vbit==0)
                    setv1(y_bus);
                 sio8=(y_bus&0x80)? 0:1;
                 y_bus=y_bus<<1;
                 x=setn(mq);
                 y_bus=y_bus|x;
                 mq=(mq<<1)&0xffff;
                 sio0=1;
              }
              break;

      case 6: if(ssf==1)                             /* SLC */
              {
                 sio8=(y_bus&0x80)? 0:1;
                 x=setn(y_bus);
                 sio0=(x)? 0:1;
                 y_bus=(y_bus<<1)|x;
              }
              break;

      case 7: if(ssf==1)                            /* SLCD */
              {
                 sio8=(y_bus&0x80)? 0:1;
                 x=setn(y_bus);
                 sio0=(x)? 0:1;
                 y_bus=(y_bus<<1)|setn(mq);
                 mq=((mq<<1)|x)&0xffff;
              }
              break;

      case 8: if(ssf==1)                             /* SRC */
              {
                 sio8=(y_bus&0x100)? 0:1;
                 x=(y_bus&1)? 0x8000:0;
                 sio0=(x)? 0:1;
                 y_bus=(y_bus>>1)|x;
              }
              break;

      case 9: if(ssf==1)                            /* SRCD */
              {
                 sio8=(y_bus&0x100)? 0:1;
                 sio0=(mq&1)? 0:1;
                 x=(y_bus&1)? 0x8000:0;
                 w=(mq&1)? 0x8000:0;
                 y_bus=(y_bus>>1)|w;
                 mq=((mq>>1)|x)&0xffff;
              }
              break;

     case 10: if(ssf==1)                           /* MQSRA */
              {
                 sio0=(mq&1)? 0:1;
                 sio8=1;
                 x=mq&0x8000;
                 mq=((mq>>1)|x)&0xffff;
              }
              break;

     case 11: if(ssf==1)                           /* MQSRL */
              {
                 sio0=(mq&1)? 0:1;
                 sio8=1;
                 mq=(mq>>1)&0xffff;
              }
              break;

     case 12: if(ssf==1)                           /* MQSLL */
              {
                 mq=(mq<<1)&0xffff;
                 sio0=sio8=1;
              }
              break;

     case 13: if(ssf==1)                           /* MQSLC */
              {
                 x=setn(mq);
                 mq=((mq<<1)|x)&0xffff;
                 sio0=(x)? 0:1;
              }
              break;

     case 14: mq=y_bus&0xffff;                    /* LOADMQ */
              ssf=mq&1;
              dbit=qbt=firstqbt=divsgn=0;
              sio0=sio8=1;
              break;

     default:
     case 15: sio0=sio8=1;                          /* PASS */
              break;
   }
   y_bus&=0xffff;                      /* clean up y-bus         */
   setz(y_bus);                        /* test for zero result   */
   setn(y_bus);                        /* test for high-bit set  */
}

/***********************************************************
*   group3()  alu group 3 (byte) instructions
**********************************************************/

int group3()
{
   myunsigned mask,mask2;

   mask=((csel<<4)|asel)&0xff;
   switch((mw.alu_func>>4)&0x0f)
   {
      case 0: if(sio0==0)                    /* SET1 */
                 r_bus=mask;
              else
                 r_bus=mask<<8;
              y_bus=s_bus&r_bus;
              if(sio0==0)
                 setz(y_bus&0xff);
              else
                 setz(y_bus&0xff00);
              break;

      case 1: if(sio0==0)                    /* SET0 */
                 r_bus=~mask&0xffff;
              else
                 r_bus=~(mask<<8)&0xffff;
              y_bus=s_bus&r_bus;
              if(sio0==0)
                 setz(y_bus&0xff);
              else
                 setz(y_bus&0xff00);
              break;

      case 2: if(sio0==0)                    /* TB1  */
                 r_bus=mask;
              else
                 r_bus=(mask<<8);
              mask2=s_bus&r_bus;
              zbit=(mask2==mask)? 1:0;
              break;

      case 3: if(sio0==0)                    /* TB0  */
                 r_bus=mask;
              else
                 r_bus=(mask<<8);
              mask2=s_bus&r_bus;
              zbit=(mask2==0)? 1:0;
              break;

      case 4: if(setn(s_bus)==1)             /* ABS  */
                 temp=(~s_bus&0xffff)+cin;
              else
                 temp=s_bus;
              y_bus=temp&0xffff;
              vbit=(s_bus==0x8000)? 1:0;
              ssf=nbit;
              setz(y_bus);
              cbit=(s_bus==0)? 1:0;
              break;

      case 5: ssf=(setn(s_bus)==0)? 1:0;        /* SMTC */
              if(ssf==1)
                 temp=s_bus;
              else
              {
                 temp=(~s_bus&0xffff)+cin;
                 if(setn(temp)==1)
                    temp&=0x17fff;
                 else
                    temp|=0x08000;
              }
              y_bus=temp&0xffff;
              vbit=(s_bus==0x8000)? 1:0;
              if(vbit==1)
                 y_bus=0;
              setn(y_bus);
              setz(y_bus);
              cbit=(s_bus==0)? 1:0;
              break;

      case 6: r_bus=asel&0x0f;                 /* ADDI  */
              temp=r_bus+s_bus+cin;
              y_bus=temp&0xffff;
              setc(temp);
              setn(y_bus);
              setz(y_bus);
              setv(r_bus,s_bus,y_bus);
              break;

      case 7: r_bus=asel&0x0f;                 /* SUBI  */
              temp=(~r_bus&0xffff)+s_bus+cin;
              y_bus=temp&0xffff;
              setc(temp);
              setn(y_bus);
              setz(y_bus);
              setv(~r_bus,s_bus,y_bus);
              break;

      case 8: if(sio8==0)                      /* BADD  */
              {                       /*  msb  */
                 temp=(r_bus&0xff00)+(s_bus&0xff00)+(cin<<8);
                 setv(r_bus,s_bus,temp&0xffff);
                 y_bus=(temp&0xff00)|(s_bus&0x00ff);
              }
              else
              {                       /*  lsb  */
                 temp=((r_bus&0xff)+(s_bus&0xff)+cin)<<8;
                 setv(r_bus<<8,s_bus<<8,temp&0xffff);
                 y_bus=((temp>>8)&0xff)|(s_bus&0xff00);
              }
              setc(temp);
              setn(temp&0xffff);
              setz(temp&0xffff);
              break;

      case 9: if(sio8==0)                      /* BSUBS */
              {                       /*  msb  */
                 temp=(r_bus&0xff00)+((~s_bus)&0xff00)+(cin<<8);
                 setv(r_bus,~s_bus,temp&0xffff);
                 y_bus=(temp&0xff00)|(s_bus&0x00ff);
              }
              else
              {                       /*  lsb  */
                 temp=((r_bus&0xff)+((~s_bus)&0xff)+cin)<<8;
                 setv(r_bus<<8,(~s_bus)<<8,temp&0xffff);
                 y_bus=((temp>>8)&0xff)|(s_bus&0xff00);
              }
              setc(temp);
              setn(temp&0xffff);
              setz(temp&0xffff);
              break;

     case 10: if(sio8==0)                     /* BSUBR */
              {                       /*  msb  */
                 temp=((~r_bus)&0xff00)+(s_bus&0xff00)+(cin<<8);
                 setv(~r_bus,s_bus,temp&0xffff);
                 y_bus=(temp&0xff00)|(s_bus&0x00ff);
              }
              else
              {                       /*  lsb  */
                 temp=(((~r_bus)&0xff)+(s_bus&0xff)+cin)<<8;
                 setv((~r_bus)<<8,s_bus<<8,temp&0xffff);
                 y_bus=((temp>>8)&0xff)|(s_bus&0xff00);
              }
              setc(temp);
              setn(temp&0xffff);
              setz(temp&0xffff);
              break;

     case 11: if(sio8==0)                     /* BINCS */
              {                       /*  msb  */
                 temp=(s_bus&0xff00)+(cin<<8);
                 setv(0,s_bus,temp&0xffff);
                 y_bus=(temp&0xff00)|(s_bus&0x00ff);
              }
              else
              {                       /*  lsb  */
                 temp=((s_bus&0xff)+cin)<<8;
                 setv(0,s_bus<<8,temp&0xffff);
                 y_bus=((temp>>8)&0xff)|(s_bus&0xff00);
              }
              setc(temp);
              setn(temp&0xffff);
              setz(temp&0xffff);
              break;

     case 12: if(sio8==0)                     /* BINCNS */
              {                       /*  msb  */
                 temp=((~s_bus)&0xff00)+(cin<<8);
                 setv(0,~s_bus,temp&0xffff);
                 y_bus=(temp&0xff00)|(s_bus&0x00ff);
              }
              else
              {                       /*  lsb  */
                 temp=(((~s_bus)&0xff)+cin)<<8;
                 setv(0,(~s_bus)<<8,temp&0xffff);
                 y_bus=((temp>>8)&0xff)|(s_bus&0xff00);
              }
              setc(temp);
              setn(temp&0xffff);
              setz(temp&0xffff);
              break;

     case 13: if(sio8==0)                     /* BXOR  */
              {                       /*  msb  */
                 temp=(r_bus&0xff00)^(s_bus&0xff00);
                 y_bus=(temp&0xff00)|(s_bus&0x00ff);
              }
              else
              {                       /*  lsb  */
                 temp=((r_bus&0xff)^(s_bus&0xff))<<8;
                 y_bus=((temp>>8)&0xff)|(s_bus&0xff00);
              }
              setn(temp&0xffff);
              setz(temp&0xffff);
              break;

     case 14: if(sio8==0)                     /* BAND  */
              {                       /*  msb  */
                 temp=(r_bus&0xff00)&(s_bus&0xff00);
                 y_bus=(temp&0xff00)|(s_bus&0x00ff);
              }
              else
              {                       /*  lsb  */
                 temp=((r_bus&0xff)&(s_bus&0xff))<<8;
                 y_bus=((temp>>8)&0xff)|(s_bus&0xff00);
              }
              setn(temp&0xffff);
              setz(temp&0xffff);
              break;

     case 15: if(sio8==0)                     /* BOR   */
              {                       /*  msb  */
                 temp=(r_bus&0xff00)|(s_bus&0xff00);
                 y_bus=(temp&0xff00)|(s_bus&0x00ff);
              }
              else
              {                       /*  lsb  */
                 temp=((r_bus&0xff)|(s_bus&0xff))<<8;
                 y_bus=((temp>>8)&0xff)|(s_bus&0xff00);
              }
              setn(temp&0xffff);
              setz(temp&0xffff);
              break;
    }
}

/***********************************************************
*   group4()  alu group 4 instructions
**********************************************************/

int group4()
{
   int x;

   switch((mw.alu_func>>4)&0x0f)
   {
     case 0: break;                                  /* reserved */

     case 1: if(ssf==0)                                   /* SEL */
                temp=r_bus+cin;
             else
                temp=s_bus;
             y_bus=temp&0xffff;
             setz(y_bus);
             setn(y_bus);
             vbit=cbit=0;
             break;

     case 2: if(!setv1(mq))                             /* SNORM */
             {
                temp=mq;
                temp=temp<<1;
                setc(temp);
                mq=temp&0xffff;
                ssf=1;
                temp=s_bus+cin;
             }
             else
             {
                ssf=0;
                temp=s_bus;
             }
             y_bus=temp&0xffff;
             setz(mq);
             setn(mq);
             break;

     case 3: if(!setv1(s_bus))                          /* DNORM */
             {
                x=setn(mq);
                mq=(mq<<1)&0xffff;
                temp=(s_bus<<1)|x;
                setc(temp);
                y_bus=temp&0xffff;
                sio8=(y_bus&0x100)? 0:1;
                ssf=1;
                sio0=(x)? 0:1;
             }
             else
             {
                ssf=0;
                y_bus=s_bus;
             }
             setz(y_bus);
             setn(y_bus);
             break;

    case 4:  if(s_bus!=0)                               /* DIVRF */
             {
                if((firstqbt==1)&&(ssf==0))
                   temp=s_bus+r_bus;
                else if((firstqbt==0)&&(ssf==1))
                   temp=s_bus+(~r_bus&0xffff)+cin;
                else if((firstqbt==0)&&(ssf==0)
                        &&(((r_bus+s_bus)&0xffff)==0))
                           temp=0;
                else if((firstqbt==1)&&(ssf==1)
                        &&(s_bus==r_bus))
                           temp=0;
                else
                   temp=s_bus;
             }
             else
                temp=s_bus;
             y_bus=temp&0xffff;
             if(((setn(mq)==1)&&(y_bus!=0))
                ||((setn(r_bus)==1)&&(y_bus==0)))
                   ssf=1;
             else
                   ssf=0;
             vbit=((divsgn==0)&&(setn(y_bus)==1))? 1:0;
             if((divsgn==1)&&(setz(y_bus)==setn(y_bus)))
                vbit=1;
             v_latch|=vbit;
             setc(temp);
             setz(y_bus);
             nbit=vbit=0;
             break;

    case 5:  temp=s_bus+ssf;                            /* SDIVQF */
             mq=y_bus=temp&0xffff;
             setc(temp);
             vbit=((firstqbt==1)&&(setn(y_bus)==1))? 1:0;
             if((firstqbt==0)&&(setz(y_bus)==setn(y_bus)))
               vbit=1;
             vbit=vbit|v_latch|setz(r_bus);
             setn(y_bus);
             setz(y_bus);
             break;

    case 6:  if(mq&1==1)                                /* SMULI */
             {
                ssf=1;
                temp=s_bus+r_bus+cin;
             }
             else
             {
                ssf=0;
                temp=s_bus;
             }
             mq=(mq>>1)&0xffff;            /* double right shift */
             if(temp&1==1)
                mq|=0x8000;
             y_bus=(temp>>1)&0x7fff;
             y_bus|=(temp&0x8000);                /* sign extend */
             setz(y_bus|mq);
             setc(temp);
             setn(y_bus);
             sio8=(y_bus&0x080)? 0:1;
             break;

    case 7:  if(mq&1==1)                                /* SMULT */
             {
                ssf=1;
                temp=s_bus+(~r_bus&0xffff)+cin;
             }
             else
             {
                ssf=0;
                temp=s_bus;
             }
             mq=(mq>>1)&0xffff;            /* double right shift */
             if(temp&1==1)
                mq|=0x8000;
             y_bus=(temp>>1)&0x7fff;
             y_bus|=(temp&0x8000);                /* sign extend */
             setz(y_bus|mq);
             setc(temp);
             setn(y_bus);
             sio8=(y_bus&0x080)? 0:1;
             break;

    case 8:  temp=s_bus;                               /* SDIVIN */
             x=setn(mq);
             qbt=((r_bus&0x8000)==(s_bus&0x8000))? 1:0;
             y_bus=(temp<<1)|x;          /* double left shift */
             mq=((mq<<1)|qbt)&0xffff;    /*    with qbt       */
             y_bus&=0xffff;
             ssf=qbt;
             divsgn=setn(s_bus);         /* preserve sign of dvnd */
             v_latch=setz(r_bus);        /* test for divide-by-0  */
             if(divsgn==setn(y_bus))     /* test for big dividend */
                v_latch=1;
             firstqbt=qbt;               /* save initial qbt      */
             sio8=(y_bus&0x100)? 0:1;
             sio0=(x)? 0:1;
             vbit=nbit=cbit=0;
             break;

    case 9:  if(qbt==0)                                /* SDIVIS */
                temp=r_bus+s_bus;
             else
                temp=s_bus+(~r_bus&0xffff)+cin;
             y_bus=temp&0xffff;
             setc(temp);
             qbt=((r_bus&0x8000)==(y_bus&0x8000))? 1:0;
             if(divsgn==setn(s_bus))
                 v_latch=1;
             if(setz(r_bus)==1)
                 v_latch=1;
             if((qbt==1)&&(firstqbt==1))
                v_latch=1;
             x=setn(mq);
             y_bus=(temp<<1)|x;          /* double left shift */
             mq=((mq<<1)|qbt)&0xffff;    /*    with qbt       */
             y_bus&=0xffff;
             setz(y_bus|mq);
             ssf=qbt;
             v_latch=v_latch|setz(r_bus);
             vbit=nbit=0;
             sio8=(y_bus&0x100)? 0:1;
             sio0=(x)? 0:1;
             break;

    case 10: if(qbt==0)                                /* SDIVI  */
                temp=r_bus+s_bus;
             else
                temp=s_bus+(~r_bus&0xffff)+cin;
             y_bus=temp&0xffff;
             setc(temp);
             qbt=((r_bus&0x8000)==(y_bus&0x8000))? 1:0;
             x=setn(mq);
             y_bus=(temp<<1)|x;          /* double left shift */
             mq=((mq<<1)|qbt)&0xffff;    /*    with qbt       */
             y_bus&=0xffff;
             setz(y_bus|mq);
             ssf=qbt;
             vbit=nbit=0;
             sio8=(y_bus&0x100)? 0:1;
             sio0=(x)? 0:1;
             break;

    case 11: temp=s_bus+(~r_bus&0xffff)+cin;          /* UDIVIS */
             ssf=dbit=setc(temp);
             firstqbt=qbt=1;
             divsgn=0;
             x=setn(mq);
             y_bus=(temp<<1)|x;          /* double left shift */
             mq=((mq<<1)|ssf)&0xffff;    /*    with carry     */
             y_bus&=0xffff;
             setz(y_bus|mq);
             vbit=(((myunsigned)s_bus>=(myunsigned)r_bus)
                  ||(r_bus==0))? 1:0;
             v_latch=vbit;
             nbit=0;
             sio8=(y_bus&0x100)? 0:1;
             sio0=(x)? 0:1;
             break;

    case 12: if(ssf==0)                                 /* UDIVI */
                temp=r_bus+s_bus;
             else
                temp=s_bus+(~r_bus&0xffff)+cin;
             if(setc(temp)==0) ssf=0;
             else if(ssf==1) ssf=1;
             else if(dbit==0) ssf=1;
             else ssf=0;
             dbit=cbit;
             x=setn(mq);
             y_bus=(temp<<1)|x;          /* double left shift */
             mq=((mq<<1)|ssf)&0xffff;    /*    with carry     */
             y_bus&=0xffff;
             setz(y_bus|mq);
             nbit=0;
             vbit=0;
             sio8=(y_bus&0x100)? 0:1;
             sio0=(x)? 0:1;
             break;

    case 13: if(mq&1==1)                                /* UMULI */
             {
                ssf=1;
                temp=s_bus+r_bus+cin;
             }
             else
             {
                ssf=0;
                temp=s_bus+cin;
             }
             mq=(mq>>1)&0xffff;            /* double right shift */
             if(temp&1==1)
                mq|=0x8000;
             y_bus=(temp>>1)&0xffff;               /* with carry */
             setz(y_bus|mq);
             setc(temp);
             setn(y_bus);
             sio8=(y_bus&0x080)? 0:1;
             break;

    case 14: if(qbt==0)                                /* SDIVIT */
                temp=r_bus+s_bus;
             else
                temp=s_bus+(~r_bus&0xffff)+cin;
             y_bus=temp&0xffff;
             setc(temp);
             qbt=((r_bus&0x8000)==(y_bus&0x8000))? 1:0;
             mq=((mq<<1)|qbt)&0xffff;    /* shift mq (qbt) only */
             setz(y_bus|mq);
             ssf=qbt;
             vbit=nbit=0;
             break;

    case 15: if(ssf==0)                                 /* UDIVIT */
                temp=r_bus+s_bus;
             else
                temp=s_bus+(~r_bus&0xffff)+cin;
             if(setc(temp)==0) ssf=0;
             else if(ssf==1) ssf=1;
             else if(dbit==0) ssf=1;
             else ssf=0;
             dbit=cbit;
             mq=((mq<<1)|ssf)&0xffff;           /* left shift mq */
             y_bus=(temp&0xffff);
             setz(y_bus|mq);
             nbit=0;
             vbit=0;
             break;

    default: break;
   }
}

/**********************************************************
*   group5()  alu group 5 instructions
**********************************************************/

int group5()
{
   int x;

   switch((mw.alu_func>>4)&0x0f)
   {
      case 7: temp=r_bus+s_bus+cin;               /* BCDBIN */
              y_bus=temp&0xffff;
              setc(temp);
              setv(r_bus,s_bus,y_bus);      /* r + s + cin  */
              sio8=(y_bus&0x080)? 0:1;
              y_bus=y_bus<<1;
              sio0=1;                   /* arith left shift */
              x=setn(mq);              /* cir left shift mq */
              mq=((mq<<1)|x)&0xffff;
              setz(y_bus);
              setn(y_bus);
              break;

/*-----------------------------------------------------------
     Note: alu functions EX3BC & EX3C are NOT implemented.
------------------------------------------------------------*/

      case 8:                                    /* EX3BC */
              break;

      case 9:                                    /* EX3C  */
              break;

/*-----------------------------------------------------------*/

     case 10: if(qbt==0)                         /* SDIVO  */
                 temp=r_bus+s_bus;
              else
                 temp=s_bus+(~r_bus&0xffff)+cin;
              y_bus=temp&0xffff;
              setc(temp);
              ssf=setn(s_bus);
              if(divsgn==setn(s_bus))
                 v_latch=1;
              if(setz(r_bus)==1)
                 v_latch=1;
              if((qbt==0)&&(cbit==1))
                 v_latch=1;
              vbit=nbit=0;
              break;

     case 13: x=setn(mq);                         /* BINEX3 */
              mq=((mq<<1)|x)&0xffff;
              sio0=(x)? 0:1;
              temp=r_bus+s_bus+x;
              y_bus=temp&0xffff;
              setc(temp);
              setv(r_bus,s_bus,y_bus);
              setn(y_bus);
              setz(y_bus);
              break;

     default: ssf=0;                             /* CLR */
              dbit=qbt=firstqbt=divsgn=0;
     case 15: y_bus=0;                           /* NOP */
              zbit=1;
              break;
   }
}

/************************************************************
*                         alu()                             *
*************************************************************
*    Performs alu functions:  r_bus op s_bus => y_bus
*          with shift and mq operations included.
************************************************************/

myint alu()
{
   zbit=nbit=vbit=cbit=0;
   switch(mw.alu_func&0x0f)
   {
      case 0:  group4(); break;
      case 8:  group3(); break;
      case 15: group5(); break;
      default: group1(); break;
   }
}

