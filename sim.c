/********************************************************************
*                           Sim.c                                   *
*********************************************************************
*  This is the global microsimulator code, included in sim?.c.
*********************************************************************/

#include "sim.h"                /* simulator header file         */

char hot_buffer[N];             /* read buffer for hotkey macro  */

#ifdef VMS                      /* include vax hotkey function   */
#include "vaxkey.c"             /*   if this is a VMS machine.   */
#endif

#ifdef THINK_C                  /* include mac hotkey function   */
#include "mackey.c"             /*   if this is a Macintosh.     */
#endif

#ifdef sun                      /* include tolower() function    */
#include "lower.c"              /* if this is a Sun machine.     */
#endif

   unsigned *control_store;     /* control store                 */
   char obj_name[N];            /* file name for object file     */
   int i;                       /* general purpose loop control  */
   char c;                      /* general purpose character     */
   int bkpt[MAX_BREAKS];        /* breakpoints                   */
   myint timer = 0;             /* elapsed clock cycles          */
   char *prompt = "=> ";        /* command entry prompt string   */
   char reply[N];               /* general purpose string        */
   int rpldex;                  /* index into reply[]            */
   int menus = TRUE;            /* display command options       */
   int logical_data = TRUE;     /* display data as LOGICAL       */
   int logging = FALSE;         /* flag for logging a session    */
   FILE *logfile;               /* logging file handle           */
   char logname[N];             /* file name for logging file    */

/********************************************************************
*                         whitespace()                              *
*********************************************************************
*  Returns TRUE if the character argument is
*       any control character (CR, tab, LF, etc.) except \0.
*********************************************************************/

int whitespace(c)
int c;
{
   return((c>0)&&(c<0x21));
}

/********************************************************************
*                         next_command()                            *
*********************************************************************
*  Sets up reply[] and index to access next command.
*  Returns the next command character found.
*  ('n' argument forces a read of a new line of input)
*********************************************************************/

char next_command(c)
char c;
{
   rpldex++;

   if(c=='n')               /* ignore rest of buffer; get new line  */
      reply[rpldex]=0;

/* find the next whitespace...  */

   while((reply[rpldex]!=0)&&(!whitespace(reply[rpldex])))
      rpldex++;

/* skip any control characters or spaces...  */

   while(whitespace(reply[rpldex]))
      rpldex++;

/* if at the end, get a new command string...   */

   while(reply[rpldex]==0)
   {
      myprintf("%s",prompt);
      gets(reply);
      rpldex=0;

/* ... and convert to lower case.  */

      for(i=0;i<strlen(reply);i++)
         reply[i]=tolower(reply[i]);
      if (logging) fprintf(logfile,"%s\n",reply);

/* skip any beginning spaces or control characters...   */

      while(whitespace(reply[rpldex]))
         rpldex++;
   }

   return(reply[rpldex]);
}

/********************************************************************
*                      last_command()                               *
*********************************************************************
*  TRUE if the current command is the last command in the string.
*********************************************************************/

int last_command()
{
   i=rpldex;                                /* find the next blank  */
   while((i<strlen(reply))&&(!whitespace(reply[i])))
     i++;
   while(whitespace(reply[i]))              /* skip any more blanks */
     i++;
   return(reply[i]==0);
}

/********************************************************************
*                          myprintf()                               *
*********************************************************************
*  Print to screen and log session to log file if logging is TRUE
*********************************************************************/

int
myprintf(fmt,pf1,pf2,pf3,pf4,pf5,pf6,pf7,pf8,pf9,
             pf10,pf11,pf12,pf13,pf14,pf15,pf16)
char *fmt;
char *pf1,*pf2,*pf3,*pf4,*pf5,*pf6,*pf7,*pf8;
char *pf9,*pf10,*pf11,*pf12,*pf13,*pf14,*pf15,*pf16;
{
   char tmp[512];

   sprintf(tmp,fmt,pf1,pf2,pf3,pf4,pf5,pf6,pf7,pf8,
                   pf9,pf10,pf11,pf12,pf13,pf14,pf15,pf16);
   printf("%s",tmp);
   if (logging) fprintf(logfile,"%s",tmp); 
}

/********************************************************************
*                        open_log()                                 *
*********************************************************************
*  Open log file and start session logging.
*********************************************************************/

int open_log()
{
    char tmpname[N];
    int i;
    char c;

    myprintf("File name for session log file [.ssn]: "); 
    scanf("%s%c",tmpname,&c);

/* if the logging file name has no extension, add one [.ssn] */

    strcpy(logname,tmpname);
    i=0;
    while(tmpname[i] != '\0' && tmpname[i] != '.')
       i++;
    if(tmpname[i]=='\0')
       strcat(tmpname,".ssn");
    strcpy(logname,tmpname);
   if(logname[0]=='.')
   {
      myprintf("Bad filename [%s].\n",logname);
      return(0);
   }

/* open the logging file for output */

    if((logfile=fopen(logname,"w"))== NULL)
      myprintf("Error opening session log file %s.\n",logname);
   else
    { 
    logging = TRUE;
    myprintf("Logging session to %s.\n",logname);
    }
}

/********************************************************************
*                         close_log()                               *
*********************************************************************
*  Close log file and stop session logging.
*********************************************************************/

int close_log()
{
    myprintf("Closing session log %s... ",logname); lnl;
    fclose(logfile);
    logging = FALSE;
    myprintf(" closed.\n");
}

/********************************************************************
*                           more()                                  *
*********************************************************************
*  Waits for the return key to be pressed.
*********************************************************************/

int more()
{
   int k=0;

   myprintf("More... ([tab] to break)\n"); lnl;
   while(k<=0)
   {  
      hotkey(k); 
   }                           /* braces required. (macro)  */
   return(hot_buffer[0]==9);   /* signal tab character      */
}

/********************************************************************
*                          main()
*********************************************************************
*  This is the start of the main program.  Do initialization and
*  mempry allocation, put up the main menu, and process responses.
*  Loop until done.
*********************************************************************/

main()
{
   int end = 0;                /* done flag                 */

/* allocate memory for the control store  */

   control_store=(unsigned *) calloc((MAX_CS_WORDS*CS_INT_WIDTH),
                                       sizeof(unsigned));

   if (control_store == NULL)
   {
       myprintf("\n\nNot enough core memory available!\n");
       myprintf("Unable to allocate control store.\n");
       myprintf("      Exiting program...\n\n");
       exit(1);
   }

/* allocate memory for the ram and rom    */

   if (BOARD > 2)
     if (initialize_memory() == 0)
     {
         myprintf("\n\nNot enough core memory available!\n");
         myprintf("Unable to allocate ram and rom.\n");
         myprintf("      Exiting program...\n\n");
         exit(1);
     }
   
/* initialize arrays and structures...    */

   for(i=0;i<MAX_BREAKS;i++)
      bkpt[i] = ANY;
   reset_trace();
   reset_board();
   clear_cs();
   rpldex=0;

/* put up title screen...    */

   printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
   printf("  +------------------------------------------------------------+\n");
   printf("  |      Board %d microsimulator %s %s (c) %s      |\n",
                         BOARD,PLATFORM,VERSION,CPW);
   printf("  +------------------------------------------------------------+\n");
   printf("  |Software written by Bruce Littlefield to accompany the text |\n");
   printf("  |Microprogramming and Computer Architecture by Segee & Field.|\n");
   printf("  +------------------------------------------------------------+\n");
   printf("\n\n");

/* Put up the main menu and process commands until we are done...   */

   while(!end)
   {
      myprintf("\n===MAIN==="); 
      if(menus)
         printf("\nLOAD - DISPLAY - MODIFY - TRACE - RUN - OPTIONS - QUIT\n\n");
      switch(next_command('a'))
      {
         case '?':
         case 'h': help_main();                  break;
         case 'l': load_menu();                  break;
         case 'd': display_menu();               break;
         case 'm': modify_menu();                break;
         case 't': trace_menu();                 break;
         case 'r': run_menu();                   break;
         case 'o': options_menu();               break;
         case 'x':
         case 'q': end=verify_quit();            break;
          default: myprintf("Invalid command.\n"); break;
      }
   }
}

/*********************************************************************
*                            load_menu()                             *
**********************************************************************
*   Put up the LOAD menu and process responses.
**********************************************************************/

int load_menu()
{
   int end=FALSE;

   if(BOARD<3)
      load_control();
   else
      while(!end)
      {
         if(last_command())
         {
            myprintf("\n===LOAD===");
            if(menus)
               printf("\nCONTROL_STORE - ROM - QUIT\n\n");
         }
         switch(next_command('a'))
         {
            case '?':
            case 'h': help_load();                  break;
            case 'c': load_control();           return(0);
            case 'r': load_rom_file();          return(0);
            case 'x': verify_quit();                break;
            case 'q': end = TRUE;                   break;
            default : myprintf("Invalid command.\n"); break;
         }
      }
}

/*********************************************************************
*                          options_menu()                            *
**********************************************************************
*   Put up the OPTIONS menu and process responses.
**********************************************************************/

int options_menu()
{
   int end=FALSE;

      while(!end)
      {
         if(last_command())
         {
            myprintf("\n==OPTIONS="); 
            if(menus)
            {
               printf("\nSHOW_MENUS ");
               if(BOARD==2)
                  printf("- DATA_REPRESENTATION ");
               printf("- LOG_SESSION ");
               printf("- QUIT\n\n");
            }
         }
         switch(next_command('a'))
         {
            case '?':
            case 'h': help_options();               break;
            case 's': menus=(!menus);           return(0);
            case 'l': if (logging)
                      {
                         close_log();           return(0);
                      }
                      else
                      {
                         open_log();            return(0);
                      }
            case 'r':
            case 'd': if(BOARD==2)
                      {
                         myprintf("If data representation is changed");
                         myprintf(", trace data will be lost!\n");
                         myprintf("Do you wish to continue (y/n)? "); lnl;
                         if(next_command('n')=='y')
                         {
                            stop_trace();
                            clear_trace();
                            logical_data=(!logical_data);
                         }
                         myprintf("Data representation is now ");
                         if(logical_data)
                            myprintf("logical.\n");
                         else
                            myprintf("voltage-mapped.\n");
                         return(0);
                      }
                      else
                         myprintf("Invalid command.\n"); break;
            case 'x': verify_quit();                break;
            case 'q': end = TRUE;                   break;
            default : myprintf("Invalid command.\n"); break;
         }
      }
}

/*********************************************************************
*                            display_menu()                          *
**********************************************************************
*   Put up the DISPLAY menu and process responses.
**********************************************************************/

int display_menu()
{
   int end = FALSE;

   while(!end)
   {
      if(last_command())
      {
         myprintf("\n===DISP==="); 
         if(menus)
         {
            printf("\nCSTORE - MWORD ");
            if(BOARD<3)
               printf("- INPUTS ");
            else
               printf("- ROM - RAM ");
            printf("- ALL ");
            if(BOARD==4)
               printf("- GEN_REG - SEQ - ALU - PIT ");
            else
              printf("- LEDS ");
            printf("- TRACE - QUIT\n\n");
         }
      }
      switch(next_command('a'))
      {
         case '?':
         case 'h': help_disp();                  break;
         case 'c': display_cs();             return(0);
         case 'm': show_mw('d');             return(0);
         case 'r': switch(reply[rpldex+1])
                   {
                      case  0 : myprintf("Ambiguous command.\n"); break;
                      case 'o': display_rom();   return(0);
                      case 'a': display_ram();   return(0);
                      default : myprintf("Invalid command.\n"); break;
                   }
                   break;
         case 'a': if(BOARD!=4)
                   {
                      display_regs('a');
                      return(0);
                   }
                   if(reply[rpldex+1]=='l')
                   {
                      switch(reply[rpldex+2])
                      {
                         case  0 : myprintf("Ambiguous command.\n"); break;
                         case 'l': display_regs('a');     return(0);
                         case 'u': display_inputs('c');   return(0);
                         default : myprintf("Invalid command.\n"); break;
                      }
                   }
                   else
                      myprintf("Ambiguous command.\n");
                   break;
         case 'p': display_inputs('p');      return(0);
         case 'l': set_leds();               return(0);
         case 'i': display_inputs('i');      return(0);
         case 's': if(BOARD==4) display_inputs('m');      return(0);
         case 'g': if(BOARD==4) display_inputs('r');      return(0);
         case 't': show_trace();             return(0);
         case 'x': verify_quit();                break;
         case 'q': end = TRUE;                   break;
         default : myprintf("Invalid command.\n"); break;
      }
   }
}

/*********************************************************************
*                           modify_menu()                            *
**********************************************************************
*   Put up the MODIFY menu and process responses.
**********************************************************************/

int modify_menu()
{
   int end = FALSE;
   myint myindex;

   while(!end)
   {
      if(last_command())
      {
         myprintf("\n==MODIFY==");
         if(menus)
         {
            printf("\nCSTORE - MWORD ");
            switch(BOARD)
            {
               case 1:  printf("- INPUTS - MPC ");                   break;
               case 2:  printf("- GO - INPUTS - REGISTERS - MPC ");  break;
               case 3:  printf("- ROM - RAM - REGISTERS - MPC ");    break;
               case 4:  printf("- INPUTS - ROM - RAM - SEQ - GEN_REG - ALU - PIT ");
               default: break;
            }
            printf("- TRACE - QUIT\n\n");
         }
      }
      switch(next_command('a'))
      {
         case '?':
         case 'h': help_edit();                  break;
         case 'c': edit_cs();  save_cs();    return(0);
         case 'm': if(BOARD==4)
                   {
                     show_mw('m');
                     return(0);
                   }
                   switch(reply[rpldex+1])
                   {
                      case  0 : myprintf("Ambiguous command.\n"); break;
		      case 'w':
                      case 'i': show_mw('m');  return(0);
                      case 'p': myindex = current_index;
                                request("MPC = ",&(myindex),0xfffL);
                                current_index = (int) myindex;
                                return(0);
                      default : myprintf("Invalid command.\n"); break;
                   }
                   break;
         case 't': config_trace();            return(0);
         case 'r': if(BOARD<=2)
                   {
                      set_inputs('r');
                      return(0);
                   }
                   else
                      switch(reply[rpldex+1])
                      {
                         case  0 : myprintf("Ambiguous command.\n"); break;
                         case 'e': set_inputs('r'); return(0);
                         case 'o': edit_rom();
                                   save_rom();
                                   return(0);
                         case 'a': edit_ram();      return(0);
                         default : myprintf("Invalid command.\n"); break;
                      }
                   break;
         case 'g': if(BOARD==4)
                      set_inputs('r');
                   else
                      set_inputs('g');
                   return(0);
         case 'p': set_inputs('p');          return(0);
         case 'i': if(BOARD==3)
                   {
		      myprintf("Invalid command.\n");
		      break;
		   }
                   set_inputs('i');          return(0);
         case 's': if(BOARD==4) set_inputs('m');      return(0);
         case 'a': if(BOARD==4) set_inputs('c');      return(0);
         case 'x': verify_quit();                break;
         case 'q': end = TRUE;                   break;
         default : myprintf("Invalid command.\n"); break;
      }
   }
}

/*********************************************************************
*                         load_control()                             *
**********************************************************************
*   Load the control store from a file.
**********************************************************************/

int load_control()
{
   int error;

   myprintf("Object file [.obj]: ");
   gets(obj_name);
   if (logging) fprintf(logfile,"%s\n",obj_name);
   error=load_cs(obj_name);
   if(error)
      myprintf("Try another file.\n");
}

/*********************************************************************
*                         verify_quit()
**********************************************************************
*   Verify that the user really wants to quit the program.
*********************************************************************/

int verify_quit()
{
   myprintf("\nReally quit?  ");
   if(next_command('n')=='y')
   {
      free(control_store);
      if (BOARD > 2)
         release_memory();
      if (logging) close_log();
      myprintf("\nNormal program termination.\n\n");
      exit(0);
   }
   return(FALSE);
}

/*****************************************************************
*                           save_cs()                            *
******************************************************************
*  Allow user to save the control store to a file.
*****************************************************************/

int save_cs()
{
   int i;

   myprintf("\nSave modified control store [y/n]? ");
   if(next_command('n')=='y')
   {
      myprintf("File name for control store object file [.obj]: "); 
      gets(obj_name);
      if (logging) fprintf(logfile,"%s\n",obj_name);
      store_cs(obj_name);
   }
}

/*****************************************************************
*                         save_rom()                             *
******************************************************************
*  Allow user to save rom to a file
*****************************************************************/

int save_rom()
{
   getchar();                             /* flush input buffer */
   myprintf("\nSave modified ROM [y/n]? ");
   if(next_command('n')=='y')
   {
      myprintf("Enter a filename [.mac]: ");
      gets(obj_name);
      if (logging) fprintf(logfile,"%s\n",obj_name);
      store_rom(obj_name);
   }
}

/*****************************************************************
*                            show_mw()                           *
******************************************************************
*   Display and/or edit microword. ('m' argument = modify.)
*****************************************************************/

int show_mw(ch)
char ch;
{
   int save_index;

   myprintf("Micro address [hex]? "); 
   save_index=get_hex();
   if (logging) fprintf(logfile,"%x\n",save_index);
   load_mw(save_index);
   if(ch=='m')
   {
      edit_mw();
      display_mw();
      myprintf("Store microword into control_store (y/n)? ");
      if(next_command('n')=='y')
      {
         store_mw(save_index);
         save_cs();
      }
      else
         myprintf("Not stored.\n");
      myprintf("\n");
   }
   else
   {
      display_mw();
   }
}

/****************************************************************
*                       load_rom_file()                         *
*****************************************************************
*   Load the simulated rom from an external file.
****************************************************************/

int load_rom_file()
{
   int error;

   myprintf("Macrocode filename [.mac]: ");
   gets(obj_name);
   if (logging) fprintf(logfile,"%s\n",obj_name);
   error=load_rom(obj_name);
   if(error)
      myprintf("Try another file.\n");
}

/*****************************************************************
*                          run_menu()                            *
******************************************************************
*   Put up the RUN menu and process responses.
*****************************************************************/

int run_menu()

{
   int end = FALSE;
   int i,j;
   char s[N];

   while(!end)
   {
      if(last_command())
      {
         myprintf("\n===RUN===="); 
         if(menus)
         {
            printf("\nRESET - STEP - RUN - BREAKPOINTS - ");
            printf("MODIFY - DISPLAY - TRACE - QUIT\n\n");
         }
      }
      switch(next_command('a'))
      {
         case '?':
         case 'h': help_run();                  break;
         case 'r': switch(reply[rpldex+1])
                   {
                      case  0 : myprintf("Ambiguous command.\n");
                                break;
                      case 'e': reset_board();
                                display_regs('a');
                                break;
                      case 'u': step_clock();
                                while(!breakpoint())
                                {
                                   if(step_clock()) break;
                                   hotkey(i);
                                   if(i>0) break;
                                }
                                display_regs('a');
                                break;
                      default : myprintf("Invalid command.\n");
                                break;
                   }
                   break;
         case 's': step_n();
                   display_regs('a');
                   break;
         case 'b': set_breakpoints();            break;
         case 'd': display_menu();               break;
         case 'm': modify_menu();                break;
         case 't': trace_menu();                 break;
         case 'x': verify_quit();                break;
         case 'q': end = TRUE;                   break;
         default : myprintf("Invalid command.\n"); break;
      }
   }
}

/*********************************************************************
*                           step_n()                                 *
**********************************************************************
*   Step the clock n times (from command string)
**********************************************************************/

int step_n()
{
   int i,j;
   char s[N];

/* skip the rest of the current step command  */

   i=rpldex;
   while((reply[i]!=0)&&(!whitespace(reply[i])))
      i++;

/* convert the next command string to an integer */

   for(j=0;reply[i]!=0;i++,j++)
      s[j]=reply[i];
   s[j]=0;
   i=atoi(s);

/* if no number was given, step once...  */

   if(i==0)
      step_clock();

/* otherwise, step n times and reposition the index */

   else
   {
      for(j=0;j<i;j++)
         if(step_clock()) break;
      next_command('a');
   }
}

/*********************************************************************
*                        show_breakpoints()                          *
**********************************************************************
*   Display a list of all active breakpoints.
**********************************************************************/

int show_breakpoints()
{
   int clear = TRUE;

   for(i=0;i<MAX_BREAKS;i++)
      if(bkpt[i]>=0)
         clear=FALSE;
   if(clear)
      myprintf("\nNo breakpoints set.\n");
   else
   {
      myprintf("\nBreakpoints: ");
      for(i=0;i<MAX_BREAKS;i++)
         if(bkpt[i]>=0)
            myprintf("%04x ",bkpt[i]);
      myprintf("\n");
   }
   myprintf("\n");
}

/*********************************************************************
*                       breakpoint()                                 *
**********************************************************************
*    Returns TRUE if the current instruction is a set breakpoint.
**********************************************************************/

int breakpoint()
{
   int found = FALSE;

   for(i=0;i<MAX_BREAKS;i++)
      if(bkpt[i]==current_index)
         found = TRUE;
   return(found);
}

/*********************************************************************
*                          set_breakpoints()                         *
**********************************************************************
*   Put up the breakpoints menu and process responses.  Display,
*   clear, add (set), and/or remove (unset) breakpoints.
**********************************************************************/

int set_breakpoints()
{
   char ch;
   int end = FALSE;
   int tempbk;

   show_breakpoints();
   while(!end)
   {
      if(last_command())
      {
         myprintf("\n===BKPTS=="); 
         if(menus)
            printf("\nDISPLAY - CLEAR_ALL - ADD - REMOVE - QUIT\n\n");
      }
      switch(next_command('a'))
      {
         case '?':
         case 'h': help_break();                 break;
         case 'd': show_breakpoints();           break;
         case 'c': for(i=0;i<MAX_BREAKS;i++)
                      bkpt[i] = ANY;
                   show_breakpoints();
                   break;
         case 's':
         case 'a': for(i=0;i<MAX_BREAKS;i++)
                      if(bkpt[i] == ANY)
                         break;
                   if(i==MAX_BREAKS)
                      myprintf("Too many breakpoints!\n");
                   else
                   {
                      myprintf("Enter new breakpoint [hex]: ");
                      tempbk=get_hex();
                      if (logging) fprintf(logfile,"%x\n",tempbk);
                      bkpt[i]=tempbk;
                   }
                   show_breakpoints();
                   break;
         case 'r': myprintf("Breakpoint to remove: ");
                   tempbk=get_hex();
                   if (logging) fprintf(logfile,"%x\n",tempbk);
                   for(i=0;i<MAX_BREAKS;i++)
                      if(bkpt[i] == tempbk)
                      {
                         bkpt[i] = ANY;
                         break;
                      }
                   if(i==MAX_BREAKS)
                      myprintf("Breakpoint %04x not found.\n",tempbk);
                   else
                      myprintf("Removed.\n");
                   show_breakpoints();
                   break;
         case 'x': verify_quit();                break;
         case 'q': end = TRUE;                   break;
         default : myprintf("Invalid command.\n"); break;
      }
   }
}

/*********************************************************************
*                            trace_menu()                            *
**********************************************************************
*   Put up the TRACE menu and process responses.
**********************************************************************/

int trace_menu()
{
   int end = FALSE;

   while(!end)
   {
      if(last_command())
      {
         myprintf("\n===TRACE==");
         if(menus)
            printf("\nCONFIGURE - BEGIN - END - DISPLAY - SAVE - QUIT\n\n");
      }
      switch(next_command('a'))
      {
         case '?':
         case 'h': help_trace();                 break;
         case 'c': config_trace();               break;
         case 'b': start_trace();            return(0);
         case 'e': stop_trace();             return(0);
         case 'd': show_trace();             return(0);
         case 's': save_trace();             return(0);
         case 'x': verify_quit();                break;
         case 'q': end = TRUE;                   break;
         default : myprintf("Invalid command.\n"); break;
      }
   }
}

/*********************************************************************
*                         config_trace()                             *
**********************************************************************
*   Put up the TRACE CONFIGURATION menu and process responses.
**********************************************************************/

int config_trace()
{
   int end = FALSE;

   while(!end)
   {
      if(last_command())
      {
         myprintf("\n===CONFIG="); 
         if(menus)
            printf("\nTRIGGER_ON - STORE_ON - DISPLAY - RESET - QUIT\n\n");
      }
      switch(next_command('a'))
      {
         case '?':
         case 'h': help_config();                break;
         case 't': set_trigger();                break;
         case 's': set_store();                  break;
         case 'd': show_config();                break;
         case 'r': reset_trace();
                   myprintf("Reset to defaults.\n");
                   break;
         case 'x': verify_quit();                break;
         case 'q': end = TRUE;                   break;
         default : myprintf("Invalid command.\n"); break;
      }
   }
}

/************************************************************************
*                            help_main()
*************************************************************************
*   Put up the help message for the MAIN menu level.
************************************************************************/

help_main()
{
  static char help_line[10][80] =
  {
     "\n\n          --------------- HELP! --------------- \n\n" ,
     "LOAD      Load the control store from a file.                   \n\n" ,
     "LOAD      Load the control store or ROM from a file.            \n\n" ,
     "DISPLAY   Display the control store, microword, registers,etc.  \n\n" ,
     "MODIFY    Change the control store, microword, registers, etc.  \n\n" ,
     "TRACE     Configure trace specifications, start and stop trace. \n\n" ,
     "RUN       Step, execute, set breakpoints, trace/display/modify. \n\n" ,
     "OPTIONS   Set menu, data representation and logging options.    \n\n" ,
     "QUIT/X    Exit the simulator. (X is available at all levels.)   \n\n" ,
     "HELP/?    This command description.                             \n\n"
  };

   myprintf("%s",help_line[0]);
   if(BOARD<3)
      myprintf("%s",help_line[1]);
   else
      myprintf("%s",help_line[2]);
   for(i=3;i<10;i++)
      myprintf("%s",help_line[i]);
}

/************************************************************************
*                              help_load()
*************************************************************************
*   Put up the help message for the LOAD menu level.
************************************************************************/

help_load()
{
  static char help_line[7][80] =
  {
     "\n\n          --------------- HELP! --------------- \n\n" ,
     "CONTROL_STORE   Load the control store from an external object    \n" ,
     "                file (masm output .obj file).                   \n\n" ,
     "ROM             Load the 8K simulated ROM from an external        \n" ,
     "                file (hex file of bytes - .mac file).           \n\n" ,
     "QUIT            Return to previous command level.               \n\n" ,
     "HELP/?          This command description.                       \n\n"
  };

   for(i=0;i<7;i++)
      myprintf("%s",help_line[i]);
}

/************************************************************************
*                               help_disp()
*************************************************************************
*   Put up the help message for the DISPLAY menu level.
************************************************************************/

help_disp()
{
  static char help_line[12][80] =
  {
     "\n\n          --------------- HELP! --------------- \n\n" ,
     "CSTORE         Display the non-zero control store words in hex. \n\n" ,
     "MWORD          Display a control store word broken down by function.\n\n" ,
     "INPUTS         Display the input switch values.                 \n\n" ,
     "ROM/RAM        Display the contents of ROM or RAM in hex.       \n\n" ,
     "ALL            Display all registers, busses, and signals.      \n\n" ,
     "GEN_REG        Display the IR, MAR, status, and data registers. \n\n" ,
     "SEQ/ALU/PIT    Display the microsequencer, ALU, or PIT registers.\n\n" ,
     "LEDS           Display the outputs, clock, MPC, etc.            \n\n" ,
     "TRACE          Display the trace data collected.                \n\n" ,
     "QUIT           Return to previous command level.                \n\n" ,
     "HELP/?         This command description.                        \n\n"
  };

   for(i=0;i<3;i++)
      myprintf("%s",help_line[i]);
   if(BOARD<3)
      myprintf("%s",help_line[3]);
   else   
      myprintf("%s",help_line[4]);
   myprintf("%s",help_line[5]);
   if(BOARD==4)
      for(i=6;i<8;i++)
         myprintf("%s",help_line[i]);
   else
      myprintf("%s",help_line[8]);
   for(i=9;i<12;i++)
      myprintf("%s",help_line[i]);
      
}

/************************************************************************
*                            help_edit()
*************************************************************************
*   Put up the help message for the MODIFY menu level.
************************************************************************/

help_edit()
{
  static char help_line[13][80] =
  {
     "\n\n          --------------- HELP! --------------- \n\n" ,
     "CSTORE         Modify bits within the control store.            \n\n" ,
     "MWORD          Modify a control store word by function.         \n\n" ,
     "INPUTS         Change the state of the input switches.          \n\n" ,
     "INPUTS/GO      Change the state of the input switches or Go button.\n\n",
     "ROM/RAM        Modify bytes within the ROM or RAM.              \n\n" ,
     "REGISTERS      Change the contents of any or all registers.     \n\n" ,
     "MPC            Change the MPC contents (next CS address).       \n\n" ,
     "SEQ/ALU/PIT    Change the microsequencer, ALU, or PIT registers.\n\n" ,
     "GEN_REG        Modify general registers SR, IR, MAR, Din, Dout. \n\n" ,
     "TRACE          Modify the trace configuration or toggle tracing.\n\n" ,
     "QUIT           Return to previous command level.                \n\n" ,
     "HELP/?         This command description.                        \n\n"
  };

   for(i=0;i<3;i++)
      myprintf("%s",help_line[i]);
   if((BOARD==1) || (BOARD==4))
      myprintf("%s",help_line[3]);
   if(BOARD==2)
      myprintf("%s",help_line[4]);
   if(BOARD>2)
      myprintf("%s",help_line[5]);
   if((BOARD==2) || (BOARD==3))
      myprintf("%s",help_line[6]);
   if(BOARD<4)
      myprintf("%s",help_line[7]);
   else
      for(i=8;i<10;i++)
         myprintf("%s",help_line[i]);
   for(i=10;i<13;i++)
      myprintf("%s",help_line[i]);
}

/************************************************************************
*                                 help_run()                            *
*************************************************************************
*   Put up the help message for the RUN menu level.
************************************************************************/

help_run()
{
  static char help_line[9][80] =
  {
     "\n\n          --------------- HELP! --------------- \n\n" ,
     "RESET        Reset the board. Zero the counter and registers. \n\n" ,
     "STEP n       Step the clock one or more cycles. (default 1)   \n\n" ,
     "RUN          Step the clock until the return key is pressed,    \n" ,
     "             or until a breakpoint is reached.                \n\n" ,
     "BREAKPOINTS  Display, add, remove, and clear breakpoints.     \n\n" ,
     "MODIFY/DISPLAY/TRACE     Access these menus.                  \n\n" ,
     "QUIT         Return to previous command level.                \n\n" ,
     "HELP/?       This command description.                        \n\n"
  };

   for(i=0;i<9;i++)
      myprintf("%s",help_line[i]);
}

/************************************************************************
*                             help_trace()                              *
*************************************************************************
*   Put up the help message for the TRACE menu level.
************************************************************************/

help_trace()
{
  static char help_line[8][80] =
  {
     "\n\n          --------------- HELP! --------------- \n\n" ,
     "CONFIGURE   Trace configuration trigger, store, etc. \n\n" ,
     "BEGIN       Start a new trace.                                \n\n" ,
     "END         Stop tracing.                                     \n\n" ,
     "DISPLAY     Display the trace data.                           \n\n" ,
     "SAVE        Save the trace data to a file.                    \n\n" ,
     "QUIT        Return to previous command level.                 \n\n" ,
     "HELP/?      This command description.                         \n\n"
  };

   for(i=0;i<8;i++)
      myprintf("%s",help_line[i]);
}

/************************************************************************
*                             help_config()                             *
*************************************************************************
*   Put up the help message for the TRACE CONFIGURATION menu level.
************************************************************************/

help_config()
{
  static char help_line[7][80] =
  {
     "\n\n          --------------- HELP! --------------- \n\n" ,
     "TRIGGER_ON  Set the trace trigger specifications.             \n\n" ,
     "STORE_ON    Specify the states to store in the trace.         \n\n" ,
     "DISPLAY     Display the current trace configuration.          \n\n" ,
     "RESET       Reset all of the trace specifications.            \n\n" ,
     "QUIT        Return to previous command level.                 \n\n" ,
     "HELP/?      This command description.                         \n\n"
  };

   for(i=0;i<7;i++)
      myprintf("%s",help_line[i]);
}


/************************************************************************
*                              help_break()                             *
*************************************************************************
*   Put up the help message for the BREAKPOINTS menu level.
************************************************************************/

help_break()
{
  static char help_line[7][80] =
  {
     "\n\n          --------------- HELP! --------------- \n\n" ,
     "DISPLAY     List all breakpoints.                             \n\n" ,
     "CLEAR_ALL   Clear all breakpoints.                            \n\n" ,
     "ADD         Add or set a breakpoint.                          \n\n" ,
     "REMOVE      Remove an existing breakpoint.                    \n\n" ,
     "QUIT        Return to previous command level.                 \n\n" ,
     "HELP/?      This command description.                         \n\n"
  };

   for(i=0;i<7;i++)
      myprintf("%s",help_line[i]);
}


/************************************************************************
*                            help_options()                             *
*************************************************************************
*   Put up the help message for the OPTIONS menu level.
************************************************************************/

help_options()
{
  static char help_line[13][80] =
  {
     "\n\n          --------------- HELP! --------------- \n\n" ,
     "SHOW_MENUS                                                      \n" ,
     "            Toggle the flag that inhibits/allows the command    \n" ,
     "            options menu to be displayed.                     \n\n" ,
     "DATA_REPRESENTATION                                             \n" ,
     "            Toggle data value representation (specifically for  \n" ,
     "            the IA, IB, TA, TB, and F_registers and the A, B,   \n" ,
     "            and F_busses) between LOGICAL (1 = enabled) and     \n" ,
     "            VOLTAGE_MAPPED (1 = HIGH).                        \n\n" ,
     "LOG_SESSION                                                     \n" ,
     "            Start or stop session logging to a file.          \n\n" ,
     "QUIT        Return to previous command level.                 \n\n" ,
     "HELP/?      This command description.                         \n\n"
  };

   for(i=0;i<4;i++)
      myprintf("%s",help_line[i]);
   if(BOARD==2)
      for(i=4;i<9;i++)
         myprintf("%s",help_line[i]);
   for(i=9;i<13;i++)
      myprintf("%s",help_line[i]);
}

