// **************************************************************************
// opccomn.cpp
//
// Description:
//	Implements OPC common data.  
//
//	File supplied by OPC Foundataion.
//
// DISCLAIMER:
//	This programming example is provided "AS IS".  As such Kepware, Inc.
//	makes no claims to the worthiness of the code and does not warranty
//	the code to be error free.  It is provided freely and can be used in
//	your own projects.  If you do find this code useful, place a little
//	marketing plug for Kepware in your code.  While we would love to help
//	every one who is trying to write a great OPC client application, the 
//	uniqueness of every project and the limited number of hours in a day 
//	simply prevents us from doing so.  If you really find yourself in a
//	bind, please contact Kepware's technical support.  We will not be able
//	to assist you with server related problems unless you are using KepServer
//	or KepServerEx.
// **************************************************************************


/* this file contains the actual definitions of */

/* the IIDs and CLSIDs */



/* link this file in with the server and any clients */





/* File created by MIDL compiler version 3.01.75 */

/* at Thu Oct 01 14:19:30 1998

 */

/* Compiler settings for opccomn.idl:

    Oicf (OptLev=i2), W1, Zp8, env=Win32, ms_ext, c_ext

    error checks: none

*/

//@@MIDL_FILE_HEADING(  )
#ifdef __cplusplus

extern "C"{

#endif 





#ifndef __IID_DEFINED__

#define __IID_DEFINED__



typedef struct _IID

{

    unsigned long x;

    unsigned short s1;

    unsigned short s2;

    unsigned char  c[8];

} IID;



#endif // __IID_DEFINED__



#ifndef CLSID_DEFINED

#define CLSID_DEFINED

typedef IID CLSID;

#endif // CLSID_DEFINED



const IID IID_IOPCShutdown = {0xF31DFDE1,0x07B6,0x11d2,{0xB2,0xD8,0x00,0x60,0x08,0x3B,0xA1,0xFB}};





const IID IID_IOPCCommon = {0xF31DFDE2,0x07B6,0x11d2,{0xB2,0xD8,0x00,0x60,0x08,0x3B,0xA1,0xFB}};





const IID IID_IOPCServerList = {0x13486D50,0x4821,0x11D2,{0xA4,0x94,0x3C,0xB3,0x06,0xC1,0x00,0x00}};





const IID LIBID_OPCCOMN = {0xB28EEDB1,0xAC6F,0x11d1,{0x84,0xD5,0x00,0x60,0x8C,0xB8,0xA7,0xE9}};





#ifdef __cplusplus

}

#endif



