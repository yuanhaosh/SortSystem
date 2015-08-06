
//////////////////////////////////////////////////////////////////////////////
// Filename:	CBPxlFormat.h
// Description:	This file contains #defines and masks for definition of pixel
//				formats used with ConeBeam receptors.	
// Copyright:	Varian Medical Systems
//				All Rights Reserved
//				Varian Proprietary   
//////////////////////////////////////////////////////////////////////////////

#if !defined(CBPXLFORMAT_H_INCLUDED)
#define CBPXLFORMAT_H_INCLUDED


#define	  CB_ORIG_FRMT_MASK		0x0000FF	// retrieves the original data 
											// format from PixDataFormat
#define	  CB_CURR_FRMT_MASK		0x00FF00	// retrieves the current data 
											// format from PixDataFormat
#define	  CB_CORCTD_FRMT_MASK	0x000F00	// retrieves the current format 
											// from PixDataFormat if it's corrected
#define	  CB_RAW_FRMT_MASK		0x00F000	// retrieves the current format
											// from PixDataFormat if it's raw
#define	  CB_HILO_FRMT_MASK		0x0F0000	// retrieves the format
											// from PixDataFormat if it's a 
											// separated hi/lo DUAL_READ image

#define   CB_ORIG_NOT_DEFINED	0x000000	// original format is normal non-CB
											// same as enum in VIP_4030CB.h
#define   CB_ORIG_DUAL_READ		0x000001	// original format is DUAL_READ
											// same as enum in VIP_4030CB.h
#define   CB_ORIG_DYNAMIC_GAIN	0x000002	// original format is DYNAMIC_GAIN
											// same as enum in VIP_4030CB.h
#define   CB_FRMT_NORMAL		0x000000	// normal 16-bit unsigned data 
#define   CB_FRMT_EXP_2			0x000200	// format 2 most significant bits 
											// are exponent (base 2)for the 
											// remaining 14 bits (values up to
											// 17 bits may be stored)
#define   CB_FRMT_RAW_DUAL_RD	0x001000	// indicates pixels are in the raw 
											// uncorrectedstate as acquired by
											// DUAL_READ
#define   CB_FRMT_RAW_DYN_GN	0x002000	// indicates pixels are in the raw 
											// uncorrected state as acquired by
											// DYNAMIC_GAIN
#define   CB_FRMT_HIGN_DUAL_RD	0x010000	// indicates pixels are in the raw 
											// uncorrected state as acquired by 
											// DUAL_READ but separated - here 
											// HI_GAIN -- NOTE used by ViVA only.
#define   CB_FRMT_LOGN_DUAL_RD	0x020000	// indicates pixels are in the raw 
											// uncorrected state as acquired by 
											// DUAL_READ but separated - here 
											// LO_GAIN -- NOTE used by ViVA only.

//////////////////////////////////////////////////////////////////////////////


#endif // CBPXLFORMAT_H_INCLUDED