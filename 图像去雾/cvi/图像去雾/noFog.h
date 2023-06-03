/**************************************************************************/
/* LabWindows/CVI User Interface Resource (UIR) Include File              */
/* Copyright (c) National Instruments 2022. All Rights Reserved.          */
/*                                                                        */
/* WARNING: Do not add to, delete from, or otherwise modify the contents  */
/*          of this include file.                                         */
/**************************************************************************/

#include <userint.h>

#ifdef __cplusplus
    extern "C" {
#endif

     /* Panels and Controls: */

#define  PANEL                            1
#define  PANEL_READ                       2       /* control type: command, callback function: read */
#define  PANEL_DARKCHANNEL                3       /* control type: command, callback function: darkChannel */
#define  PANEL_EQUALIZATION               4       /* control type: command, callback function: equalization */
#define  PANEL_QUIT                       5       /* control type: command, callback function: quit */
#define  PANEL_TEXTBOX                    6       /* control type: textBox, callback function: (none) */


     /* Control Arrays: */

          /* (no control arrays in the resource file) */


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */

int  CVICALLBACK darkChannel(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK equalization(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK quit(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK read(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
