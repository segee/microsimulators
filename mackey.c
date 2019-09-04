/****************************************************************
*                     Mackey.c                                  *
*****************************************************************
* mac_hotkey()  This is the hotkey implementation for Macintosh.
****************************************************************/

int mac_hotkey()
{
   char onebyte;
   char mac_buffer[N];
   
   onebyte = * (char *) 0x185;  /* look in low memory for the byte     */
                                /* representing the key being pressed. */
                                
   if (onebyte != 0)            /* if there is a key being pressed,    */
   {
     gets(mac_buffer);                   /* get a string for the buffer.*/
     hot_buffer[0] = (unsigned) onebyte; /* insert the original byte,   */
     strcat(hot_buffer,mac_buffer);      /*  and add the rest.          */
    } 
                                  
    return((int)onebyte);
}

