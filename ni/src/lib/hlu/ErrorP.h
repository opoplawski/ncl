/*
 *      $Id: ErrorP.h,v 1.5 1994-05-12 23:51:14 boote Exp $
 */
/************************************************************************
*									*
*			     Copyright (C)  1992			*
*	     University Corporation for Atmospheric Research		*
*			     All Rights Reserved			*
*									*
************************************************************************/
/*
 *	File:		ErrorP.h
 *
 *	Author:		Jeff W. Boote
 *			National Center for Atmospheric Research
 *			PO 3000, Boulder, Colorado
 *
 *	Date:		Tue Oct 20 11:43:12 MDT 1992
 *
 *	Description:	This is the private header file for the error object.
 */
#ifndef _NErrorP_h
#define _NErrorP_h

#include <ncarg/hlu/BaseP.h>
#include <ncarg/hlu/FortranP.h>
#include <ncarg/hlu/Error.h>

/*
 * Private resource - defines mode of Error Object (C or Fortran)
 */
#define	_NhlNerrMode	"err.Mode"
#define	_NhlCerrMode	"Err.Mode"

typedef struct _NhlErrorLayerRec *NhlErrorLayer;
typedef struct _NhlErrorLayerClassRec *NhlErrorLayerClass;

typedef struct _NhlErrorLayerCRec *_NhlErrorLayerC;
typedef struct _NhlErrorLayerCClassRec *_NhlErrorLayerCClass;

typedef struct _NhlErrorLayerFRec *_NhlErrorLayerF;
typedef struct _NhlErrorLayerFClassRec *_NhlErrorLayerFClass;

/*
 * Private Global functions
 */
extern void _NhlInitError(
#if	NhlNeedProto
	_NhlC_OR_F	init_type
#endif
);

extern void _NhlCloseError(
#if	NhlNeedProto
	void
#endif
);

/*
 * Private structures & definitions
 */

typedef struct _NhlETable{
	unsigned	start;
	unsigned	len;
	Const char	**errs;
} NhlETable;


/*
 * Class declarations
 */
typedef struct _NhlErrorLayerPart {
	/* User setable resource fields */
	NhlBoolean	buffer_errors;
	NhlErrorTypes	error_level;
	NhlBoolean	print_errors;
	char		*error_file;

	/* private resource fields */
	_NhlC_OR_F	error_mode;

	/* Internal private fields */
	int		child;
	int		num_emsgs;
	int		len_emsgs;
	NhlErrMsgList	emsgs;

	int		num_etables;
	int		len_etables;
	NhlETable	*etables;

	FILE		*private_fp;
	int		private_eunit;

} NhlErrorLayerPart;

typedef struct _NhlErrorLayerCPart {
	/* User setable resource fields */
	FILE		*fp;
	/* private */
	NhlBoolean	my_fp;
} NhlErrorLayerCPart;

typedef struct _NhlErrorLayerFPart {
	/* User setable resource fields */
	int		eunit;
	/* private */
	NhlBoolean	my_eunit;
} NhlErrorLayerFPart;

typedef struct _NhlErrorLayerRec {
	NhlBaseLayerPart	base;
	NhlErrorLayerPart	error;
} NhlErrorLayerRec;

typedef struct _NhlErrorLayerCRec {
	NhlObjLayerPart		base;
	NhlErrorLayerCPart	cerror;
} _NhlErrorLayerCRec;

typedef struct _NhlErrorLayerFRec {
	NhlObjLayerPart		base;
	NhlErrorLayerFPart	ferror;
} _NhlErrorLayerFRec;

typedef struct _NhlErrorLayerClassPart {
	int num_error_instances;
} NhlErrorLayerClassPart;

typedef struct _NhlErrorLayerCClassPart {
	int foo;
} NhlErrorLayerCClassPart;

typedef struct _NhlErrorLayerFClassPart {
	int foo;
} NhlErrorLayerFClassPart;

typedef struct _NhlErrorLayerClassRec {
	NhlBaseLayerClassPart	base_class;
	NhlErrorLayerClassPart	error_class;
} NhlErrorLayerClassRec;

typedef struct _NhlErrorLayerCClassRec {
	NhlObjLayerClassPart	base_class;
	NhlErrorLayerCClassPart	cerror_class;
} _NhlErrorLayerCClassRec;

typedef struct _NhlErrorLayerFClassRec {
	NhlObjLayerClassPart	base_class;
	NhlErrorLayerFClassPart	ferror_class;
} _NhlErrorLayerFClassRec;

extern NhlErrorLayerClassRec NhlerrorLayerClassRec;

/*
 * Fortran functions that error.c calls
 */

extern void _NHLCALLF(nhl_finqunit,NHLF_INQUNIT)(
#if	NhlNeedProto
	int	*unit_num,
	int	*connected,
	int	*ierr
#endif
);

extern void _NHLCALLF(nhl_fopnunit,NHLF_OPNUNIT)(
#if	NhlNeedProto
	int		*unit_num,
	_NhlFString	file_name,
	int		*file_name_len,
	int		*err
#endif
);

extern void _NHLCALLF(nhl_fclsunit,NHLF_CLSUNIT)(
#if	NhlNeedProto
	int		*unit_num,
	int		*err
#endif
);

extern void _NHLCALLF(nhl_fprnmes,NHLF_PRNMES)(
#if	NhlNeedProto
	int		*unit_num,
	_NhlFString	message,
	int		*message_len
#endif
);

extern int _NHLCALLF(i1mach,I1MACH)(
#if	NhlNeedProto
	int	*qnum
#endif
);

#endif /* _NErrorP_h */	
