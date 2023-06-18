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
#define  PANEL_Finish                     2       /* control type: command, callback function: finish */
#define  PANEL_Quit                       3       /* control type: command, callback function: quit */
#define  PANEL_Load_File                  4       /* control type: command, callback function: load_file */
#define  PANEL_Col_Cut                    5       /* control type: command, callback function: col_cut */
#define  PANEL_Row_Cut                    6       /* control type: command, callback function: row_cut */
#define  PANEL_out_text                   7       /* control type: textBox, callback function: (none) */


     /* Control Arrays: */

          /* (no control arrays in the resource file) */


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */

int  CVICALLBACK col_cut(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK finish(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK load_file(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK quit(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK row_cut(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
