/********************************************************************
*                           Csio.c
*********************************************************************
*       MicroSimulator Input/Output Control Functions
*********************************************************************/

#include "sim.h"             /* header file for simulator functions */
#include <math.h>

   int current_index; /* current control_store word          */

/********************************************************************
 *               functions found in this package
 ********************************************************************/

   int edit_cs();
   int getbits();
   int putbits();
   int load_cs();
   int block(), block1(), getbcount(), getaddress();
   int display_cs();
   int clear_cs();
   int store_cs();
   int putbcount(), putaddress(), wblock1();
   int get_hex(), get_dec();
   myint get_raw_hex(), get_raw_dec();

/********************************************************************
 *                 globals for these functions
 ********************************************************************/

   char  obj_file[N];   /* object file name                         */
   int   object;        /* input stream for object file             */
   int   width;         /* width of microword in bytes              */
   int   address;       /* control store offset (max 16 bits)       */
   int   bcount;        /* byte count for cs object block           */


/****************************************************************
*   get_hex()     usage:   hex_value=get_hex();
*   get_dec()              dec_value=get_dec();
****************************************************************/

int get_hex()
{
   long temp;
   char ch;

   while(scanf("%lx%c",&temp,&ch)==0)
   {
      myprintf("\nSyntax error, retry: ");
      lnl;
      ch=getchar();
   }
   return((unsigned int)temp);
}

int get_dec()
{
   long temp;
   char ch;

   while(scanf("%ld%c",&temp,&ch)==0)
   {
      myprintf("\nSyntax error, retry: ");
      lnl;
      ch=getchar();
   }
   return((int)temp);
}

/****************************************************************
*   get_raw_hex()     usage:   myint value=get_raw_xxx();
*   get_raw_dec()     returns -1 if invalid hex or decimal character
****************************************************************/

myint get_raw_hex()
{
   long temp;
   char ch;

   if(scanf("%lx%c",&temp,&ch)==0)
   {
      ch=getchar();
      return((myint)(-1));
   }
   return((myint)temp);
}

myint get_raw_dec()
{
   long temp;
   int i;
   char s[N];

   gets(s);
   i=0;
   while(s[i] == ' ') i++;
   if((s[i] >= '0') && (s[i] <= '9'))
      temp = atol(s);
   else
      temp = (ANY);
   return((myint)temp);
}

/********************************************************************
*  edit_cs function: allows the user to edit the control store bits.
*********************************************************************/

int edit_cs()
{
   unsigned i,j,p,n;
   unsigned v;
   int temp;
   char c;

   for(j=0xffff;j>0xfff;)
   {
      myprintf("\nControl store word to edit [0-fff hex]: ");
      j=get_hex();
      if (logging) fprintf(logfile,"%x\n",j);
      if(j>0xfff)
      {
         myprintf("Invalid response.");
         lnl;
      }
   }
	i=j*CS_INT_WIDTH;
   myprintf("Bit position of least significant bit [0-63 decimal]: ");
   p=get_dec();
   if (logging) fprintf(logfile,"%d\n",p);
   myprintf("Number of bits to edit [1-16 decimal]: ");
   n=get_dec();
   if (logging) fprintf(logfile,"%d\n",n);
   temp=getbits(control_store+i,p,n);
   if(temp!=(-1))
   {
      v=temp;
      myprintf("Current value [hex]: %x\n",v);
      myprintf("New value [hex]: ");
      temp=get_hex();
      v=(unsigned)temp;
      if (logging) fprintf(logfile,"%x\n",v);
      temp=putbits(control_store+i,p,n,v);
      if(temp==(-1))
      {
         myprintf("\nInvalid arguments.\n");
         return(-1);
      }
      else
         return(0);    /* signal success */
   }
   else
   {
         myprintf("\nInvalid arguments.\n");
         return(-1);
   }
}

/********************************************************************
*  getbits function:  extracts integer value of selected cs-word bits
*********************************************************************
*  cs  is a pointer to the CS_BIT_WIDTH-bit control store word
*  p   is the lsb position (0=rightmost position)
*  n   is the number of bits (<=INTSIZE)
*********************************************************************/

int getbits(cs,p,n)               /* get value of selected bits of  */
unsigned  *cs;                    /* a control-store word.          */
unsigned  p,n;
{
   unsigned mask,temp;
   int i,k;

   p+=n-1;                        /* point to msb                   */
   if (p<CS_BIT_WIDTH && p>=(n-1) && n<=INTSIZE)
   {
      i=CS_INT_WIDTH-1-p/INTSIZE; /* determine element #            */
      k=p%INTSIZE;                /* determine position within      */
      mask=~(~0<<n);              /* create n-bit mask              */
      if (n>(k+1))                /* if bits span two ints...       */
      {
         temp=(*(cs+i+1))>>(INTSIZE+k+1-n);  /* justify ls bits      */
         temp|=(*(cs+i))<<(n-k-1);           /* justify ms bits      */
      }
      else
      {
         temp=(*(cs+i))>>(k+1-n);   /* right-justify desired bits     */
      }
      return((unsigned int)(temp&mask));          /* return value of selected bits  */
   }
   else return(-1);
}


/********************************************************************
*  putbits function:  store a value into selected cs_word bits.
*********************************************************************
*  cs  is a pointer to the CS_BIT_WIDTH-bit control store word
*  p   is the lsb position (0=rightmost position)
*  n   is the number of bits (<=INTSIZE)
*  v   is the value (int) to load into the selected bits
*********************************************************************/

int putbits(cs,p,n,v)             /* change selected bits of a cs_  */
unsigned  *cs;                    /* word to the value given.       */
unsigned  p,v,n;
{
   int i,k;
   unsigned mask,mask2,v2;

   p+=n-1;                        /* point to msb                   */
   if (p<CS_BIT_WIDTH && p>=(n-1) && n<=INTSIZE)
   {
      i=CS_INT_WIDTH-1-p/INTSIZE; /* determine element #            */
      k=p%INTSIZE;                /* determine position within      */
      mask=mask2=~(~0<<n);        /* create n-bit masks             */
      v=v2=v&mask;                /* zero out all unused value bits */
      if (n>(k+1))                /* if bits span two ints          */
      {
         mask=mask>>(n-k-1);      /* justify ms mask bits           */
         v=v>>(n-k-1);            /* position ms value bits         */
         (*(cs+i))&=(~mask);      /* zero selected cs bits          */
         *(cs+i)=v;               /* replace with new ms bits       */

         mask2=mask2<<(INTSIZE+k+1-n);
         v2=v2<<(INTSIZE+k+1-n);        /* do the same with ls bits */
         v2&=0xffff;
         (*(cs+i+1))&=(~mask2);
         *(cs+i+1)|=v2;
      }
      else
      {
         mask=mask<<(k+1-n);         /* position the mask bits         */
         v=v<<(k+1-n);               /* position the value bits        */
         (*(cs+i))&=(~mask);         /* zero selected cs_word bits     */
         (*(cs+i))|=v;               /* replace with new value         */
      }
      return(0);                     /* signal success                 */
   }
   else return(-1);
}


/**********************************************************************
*    Read object code file created by the microassembler.
*
*    File format is:
*       Block #1
*          Word width   (16 bits;  microword width - modulo 16)
*          Address Base (16 bits;  microword width - again)
*          Transfer address (32 bits; always zero - ignored)
*       Block #2 to N-1
*          Byte count (16 bits; # of data bytes in block; <=248)
*          Load address LSW (16 bits; Control store offset
*                   in number of microwords - microaddress)
*          Load address MSW (16 bits; always zero)
*          Data bytes (number is equal to byte count above)
*       Block N
*          Zero byte count (16 bits of zero)
*          Load address (32 bits in LSW/MSW order; ignored)
*********************************************************************/

/*********************************************************************
 *                         load_cs(f_name)                           *
 *********************************************************************
 *  This is the routine to load the cs from an object file.
 *  It updates the cs_word index of the highest word loaded.
 *  It changes current_index to point to cs_word zero.
 *********************************************************************/

int load_cs(f_name)
char *f_name;
{
   int i,j,cs_index;
   int  index,temp;           /* general purpose index,variable      */

   strcpy(obj_file,f_name);

/*   If the object file name has no extension, add one (.obj).  */

   index=0;
   while(obj_file[index] != '\0' && obj_file[index] != '.')
      index++;
   if (obj_file[index] == '\0')
      strcat(obj_file, ".obj");

/*   Open the object file for input as a binary file.  */

   if ((object = open(obj_file,READ_ARGS)) == -1)
   {
      myprintf("Error opening object file %s.\n",obj_file);
      return(-1);
   }

/*  Success! */

   myprintf("Loading... \n");
   block1();    /* read first block, get number of bytes in word */
   block();     /* read first data block */
   cs_index=address;
   while(bcount>0)
   {
       temp=block();      /* read the rest of the data blocks */
       if((address>cs_index)&&(temp>0))
          cs_index=address;
   }
   current_index=0;
   close(object);
   myprintf("Done.\n");
   return(0);
}

/********************************************************************
*                           block1()                                *
*********************************************************************
*  Routine to read the first block of the object code.
********************************************************************/

int block1()
{
   char buf[8];
   read(object,buf,8);
   width=buf[1]/8;        /* extract the microword width in bytes */
   if (width == 0)
   {
      myprintf("Bad file format. Width = 0!\n");
      myprintf("Exiting program...\n\n");
      exit(1);
   }
}

/********************************************************************
*                           block()                                 *
*********************************************************************
*  Routine to read the data blocks of object code.
********************************************************************/

int block()
{
   char buf[256];
   int i,j,n,intbuf;

   bcount=getbcount();                           /* get count */
   address=getaddress();                         /* get address */
   if(address>=MAX_CS_WORDS)
   {
      myprintf("\n\nControl store address out of range!\n");
      myprintf("Control store now contains garbage!!\n\n");
      n=bcount=0;
   }
   else
      n=read(object,buf,bcount);               /* get data bytes */

/*
 *    Load the control store from the read buffer such that the 
 *    object code is right-justified in the control-store word.
 */

   for(i=0;i<bcount/width;i++)
   {
      intbuf=0;
      for(j=0;j<width;j++)
      {
         if((width+j)%2==0)
            intbuf=(buf[i*width+j]&0xff)<<8;         /* even bytes */
         else
            intbuf=intbuf|(buf[i*width+j]&0xff);     /* odd bytes */

         *(control_store+((address+i)*CS_INT_WIDTH)
                        +((2*CS_INT_WIDTH-width+j)/2))=intbuf;
      }
   }
   return(n);
}

/****************************************************************
*                      display_cs()
*****************************************************************
*      Display the contents of the control_store.
*****************************************************************/

int display_cs()

{
   int i,j,k,m,temp;

   m=0;
   myprintf("\nNon-zero Control Store Words...\n");
   for(i=0;i<MAX_CS_WORDS;i++)
   {
      for(j=0,temp=0;j<CS_INT_WIDTH;j++)
         temp+=(*(control_store+(i*CS_INT_WIDTH)+(j)));
      if(temp!=0)
      {
         myprintf("%03x  ",i);
         for(k=0;k<CS_INT_WIDTH;k++)
            myprintf("%04x",(*(control_store+(i*CS_INT_WIDTH)+(k))));
         myprintf("\n");
         if(((++m)%20)==0)
            if(more())
            {
               myprintf("Cancelled!\n\n");
               return(0);
            }
      }
   }
}

/****************************************************************
*                      clear_cs()
*****************************************************************
*      Zero out the contents of the control_store.
*****************************************************************/

int clear_cs()
{
   int i,j;

   for(i=0;i<MAX_CS_WORDS;i++)
      for(j=0;j<CS_INT_WIDTH;j++)
         *(control_store+(i*CS_INT_WIDTH)+(j))=0;
}

/****************************************************************
*                      getbcount()
*****************************************************************
* Routine to extract the byte count for the current data block.
****************************************************************/

 int getbcount()
 {
   char buf[2];
   int temp;

   buf[0]=buf[1]=0;
   read(object,buf,2);
   temp=(buf[0]&0xff)*256+(buf[1]&0xff);
   return(temp);
 }

/****************************************************************
*                      getaddress()
*****************************************************************
*  Routine to extract the load offset (address) from the block.
****************************************************************/

 int getaddress()
 {
   char buf[4];
   int temp;

   buf[0]=buf[1]=buf[2]=buf[3]=0;
   read(object,buf,4);
   temp=(buf[0]&0xff)*0x100+(buf[1]&0xff);
   return(temp);
 }

/*****************************************************************
*                      store_cs(f_name)                          *
******************************************************************
*  This is the routine to store the cs to an object file.
******************************************************************/

int store_cs(f_name)
char *f_name;
{
   int  index;                /* general purpose index               */

   if (width==0)              /* initialize width if not yet done    */
      switch (BOARD)
      {
         case '1':
         case '2': width = 4;
                   break;
         default:
                   width = 8;
                   break;
      }
         
    strcpy(obj_file,f_name);

   /*   If the object file name has no extension, add one (.obj).  */

   index=0;
   while(obj_file[index] != '\0' && obj_file[index] != '.')
      index++;
   if (obj_file[index] == '\0')
      strcat(obj_file, ".obj");

   /*   Open the object file for output as a binary file.  */

   if ((object = open(obj_file,WRITE_ARGS)) == -1)
   {
      myprintf("Error opening object file %s.\n",obj_file);
      exit(2);
   }
   myprintf("Storing %s...\n",obj_file);
   wblock1();    /* write first block, (number of bytes in word) */
   wblock();     /* write data blocks to the file                */
   wzblock();    /* write a block with zero byte count           */
   close(object);
   myprintf("Done.\n");
}

/*****************************************************************
*                      wblock1()                                 *
******************************************************************
*  Routine to write the first block of the object code.
*****************************************************************/

int wblock1()
{
   char buf[8];
   buf[0]=buf[2]=buf[4]=buf[5]=buf[6]=buf[7]=0;
   buf[1]=buf[3]=width*8;              /* microword width in bits */
   write(object,buf,8);
}

/*****************************************************************
*                      wzblock()                                 *
******************************************************************
*  Routine to write the last block (zero byte count).
******************************************************************/

int wzblock()
{
   char buf[6];
   buf[0]=buf[1]=buf[2]=buf[3]=buf[4]=buf[5]=0;
   write(object,buf,6);
}

/*****************************************************************
*                      wblock()                                  *
******************************************************************
*  Routine to write the data blocks of object code.
******************************************************************/

int wblock()
{
   char buf[256];
   int i,j,zflag,bptr;
   int last=0;
   int first=1;

   bcount=0;
   address=0;

/*  loop through all cs_words and process them.  */

   for(i=0;i<MAX_CS_WORDS;i++)
   {

/*  check for zero cs_word.  */

      for(zflag=0,j=0;j<CS_INT_WIDTH;j++)
         zflag=zflag + (*(control_store+(i*CS_INT_WIDTH)+(j)));

/*  ignore zero control store words.  */

      if(zflag!=0)
      {
/*
 * if the last word was zero and this one is not,
 *    OR the buffer is getting full,
 *    OR this is the first time through,
 * write the buffer (except for the first one)
 *    AND reset the buffer and pointers.
 */
         if(last==0||bcount>=240||first==1)
         {
            if(first!=1)              /* write if not first cs_word */
            {
               buf[2]=(address&0xff00)>>8;
               buf[3]=address&0xff;
               buf[1]=bcount&0xff;
               write(object,buf,bcount+6);
            }
            first=0;
            bcount=0;
            for (bptr=0;bptr<256;bptr++)
               buf[bptr]=0;
            address=i;
            bptr=6;
         }

/*  stuff buffer with current non-zero cs_word bytes.  */

         for(j=CS_INT_WIDTH-(width/2);j<CS_INT_WIDTH;j++)
         {
            buf[bptr++]=((*(control_store+(i*CS_INT_WIDTH)
                        +(j)))>>8)&0xff;
            buf[bptr++]=(*(control_store+(i*CS_INT_WIDTH)
                        +(j)))&0xff;
            bcount+=2;
         }
      }
      last=zflag;
   }

/*  Write out last data record (flush buffer).  */

   if(bcount>0)
   {
      buf[2]=(address&0xff00)>>8;
      buf[3]=address&0xff;
      buf[1]=bcount&0xff;
      write(object,buf,bcount+6);
   }
}

