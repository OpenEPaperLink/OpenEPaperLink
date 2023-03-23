#ifndef _PRINTF_H_
#define _PRINTF_H_


//our printf has some special abilities
//for example "*" will modify the param to be a __xdata pointer to whatever it would have been instead
//it must then be paramed as "(uintptr_near_t)&value"
//for cc1110 (but not for ZBS) code and xdata addrs are the same, so __code pointers will also work!

//"%s"  param takes a generic pointer, but assumes it is an xdata/code (no string support in pdata/idata)
//"%ls" takes an xdata(/code in cc111x) pointer instead :)

//"%m/%M" will print a mac, an __xdata pointer to which has been provided

//no support for passing NULL to %s

//not re-entrant if %d/%u are used

#pragma callee_saves pr
void pr(const char __code *fmt, ...) __reentrant;

#pragma callee_saves spr
void spr(char __xdata* out, const char __code *fmt, ...) __reentrant;



#endif
