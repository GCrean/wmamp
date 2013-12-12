#ifndef _DIALOG_H
#define _DIALOG_H

#define DIALOG_UNKNOWN   0
#define DIALOG_OK   1
#define DIALOG_YES  2
#define DIALOG_NO   4

int dialog_box (const char *text, int buttons);

#endif
