
/*----------------------------------------------------------------------*/
/* This widget implements a 2D value chooser. A spot may be moved*/
/* about inside a square, and its coordinates are returned to the*/
/* callback function*/
/**/
/* It is a subclass of core.*/
/**/
/* Default Translations:*/
/* <Btn1Down>: set()*/
/* <Btn1Motion>: set() drag()*/
/**/
/* Resources:               Type:                Defaults:*/
/* XtNforeground           : pixel    : XtNDefaultForeground*/
/* XkwNminimum_x           : double    : 0.0*/
/* XkwNmaximum_x           : double    : 1.0*/
/* XkwNminimum_y           : double    : 0.0*/
/* XkwNmaximum_y           : double    : 1.0*/
/* XkwNvalue_x             : double    : 0.5*/
/* XkwNvalue_y             : double    : 0.5*/
/* XkwNvalueChangeCallback : callback : NULL*/
/**/
/* Note that due to what appears to be some strange bug in Xt, possibly*/
/* due in part to the combined use of cc (for the X libraries) and gcc*/
/* (for the karmawidgets library), the values for the doubles (XkwNvalue_x*/
/* etc), must be passed to XtVaSetValues or XtVaCreateWidget as a pointer*/
/* to  a double variable, not just by value. If anyone can explain this*/
/* to me, I would be most grateful to hear about it.*/
/**/
/*----------------------------------------------------------------------*/

#ifndef TWODPOS__H
#define TWODPOS__H

extern WidgetClass twodposWidgetClass;
typedef struct _TwodposClassRec *TwodposWidgetClass;
typedef struct _TwodposRec *TwodposWidget;

#define XtIsTwodpos(w) XtIsSubclass((w), twodposWidgetClass)

#define XkwNminimum_x "minimum_x"
#define XkwNmaximum_x "maximum_x"
#define XkwNminimum_y "minimum_y"
#define XkwNmaximum_y "maximum_y"
#define XkwNvalue_x "value_x"
#define XkwNvalue_y "value_y"
#define XkwNvalueChangeCallback "valueChangeCallback"

#define XkwCMinimum_x "Minimum_x"
#define XkwCMaximum_x "Maximum_x"
#define XkwCMinimum_y "Minimum_y"
#define XkwCMaximum_y "Maximum_y"
#define XkwCValue_x "Value_x"
#define XkwCValue_y "Value_y"

typedef struct _TwodposCallbackStruct
{
  XEvent *event;
  double value_x;
  double value_y;
} TwodposCallbackStruct, *TwodposCallbackPtr;

#endif
