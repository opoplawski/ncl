/*
 *      $Id: Transform.c,v 1.49 2000-03-02 01:30:19 dbrown Exp $
 */
/************************************************************************
*									*
*			     Copyright (C)  1992			*
*	     University Corporation for Atmospheric Research		*
*			     All Rights Reserved			*
*									*
************************************************************************/
/*
 *	File:		Transform.c
 *
 *	Author:		Ethan Alpert
 *			National Center for Atmospheric Research
 *			PO 3000, Boulder, Colorado
 *
 *	Date:		Fri Oct 2 16:40:50 MDT 1992
 *
 *	Description:	Provides all subclasses of this class a generic hook
 *			into which are placed functions for the forward and
 *			reverse transformations to support point-n-click.
 *			
 *			LevelOne implies a linear transformation NDC<==>WINDOW
 *			LevelTwo implies a transformation  from  WINDOW<==>DATA
 *			
 *			Level two is used when maps or odd transformations are
 *			used.
 */

#include <ncarg/hlu/hluP.h>
#include <ncarg/hlu/TransformP.h>
#include <ncarg/hlu/TransObjP.h>
#include <ncarg/hlu/MapTransObj.h>
#include <ncarg/hlu/ConvertersP.h>

static _NhlRawObjCB callbacks[] = {
	{_NhlCBtfOverlayStatus,
         NhlOffset(NhlTransformLayerRec,trans.overlaystatuscb),
		 0,NULL,NULL,NULL}
};
#define Oset(field)     NhlOffset(NhlTransformLayerRec,trans.field)

static NhlResource resources[] = {

/* Begin-documented-resources */

	{ NhlNtfPlotManagerOn,NhlCtfPlotManagerOn,
		  NhlTBoolean,sizeof(NhlBoolean),
		  Oset(plot_manager_on),
		  NhlTImmediate,_NhlUSET((NhlPointer)True),
          	  _NhlRES_NOSACCESS,NULL},
	{ NhlNtfDoNDCOverlay,NhlCtfDoNDCOverlay,
		  NhlTBoolean,sizeof(NhlBoolean),
		  Oset(do_ndc_overlay),
		  NhlTImmediate,_NhlUSET((NhlPointer)False),0,NULL},

/* End-documented-resources */

	{ NhlNtfOverlayObject,NhlCtfOverlayObject,
		  NhlTPointer,sizeof(NhlPointer),
		  Oset(overlay_object),
		  NhlTImmediate,_NhlUSET((NhlPointer)NULL),
		  _NhlRES_PRIVATE,NULL},
	{ NhlNtfOverlayTrans,NhlCtfOverlayTrans,
		  NhlTPointer,sizeof(NhlPointer),
		  Oset(overlay_trans_obj),
		  NhlTImmediate,_NhlUSET((NhlPointer)NULL),
		  _NhlRES_PRIVATE,NULL},
	{ NhlNtfOverlayStatus,NhlCtfOverlayStatus,
		  NhlTInteger,sizeof(int),
		  Oset(overlay_status),
		  NhlTImmediate,_NhlUSET((NhlPointer)_tfNotInOverlay),
          	  _NhlRES_PRIVATE,NULL},
        
/* Intercepted resources */

	{"no.res","No.res",NhlTBoolean,sizeof(NhlBoolean),
		 Oset(x_min_set),NhlTImmediate,
		 _NhlUSET((NhlPointer)True),_NhlRES_PRIVATE,NULL},
	{NhlNtrXMinF,NhlCtrXMinF,NhlTFloat,sizeof(float),
		 Oset(x_min),NhlTProcedure,
		 _NhlUSET((NhlPointer)_NhlResUnset),_NhlRES_INTERCEPTED,NULL},
	{"no.res","No.res",NhlTBoolean,sizeof(NhlBoolean),
		 Oset(x_max_set),NhlTImmediate,
		 _NhlUSET((NhlPointer)True),_NhlRES_PRIVATE,NULL},
	{NhlNtrXMaxF,NhlCtrXMaxF,NhlTFloat,sizeof(float),
		 Oset(x_max),NhlTProcedure,
		 _NhlUSET((NhlPointer)_NhlResUnset),_NhlRES_INTERCEPTED,NULL},
	{"no.res","No.res",NhlTBoolean,sizeof(NhlBoolean),
		 Oset(x_axis_type_set),NhlTImmediate,
		 _NhlUSET((NhlPointer)True),_NhlRES_PRIVATE,NULL},
	{ NhlNtrXAxisType,NhlCtrXAxisType,NhlTAxisType,sizeof(NhlAxisType),
		Oset(x_axis_type),NhlTProcedure,
		 _NhlUSET((NhlPointer)_NhlResUnset),_NhlRES_INTERCEPTED,NULL},
	{"no.res","No.res",NhlTBoolean,sizeof(NhlBoolean),
		 Oset(x_log_set),NhlTImmediate,
		 _NhlUSET((NhlPointer)True),_NhlRES_PRIVATE,NULL},
	{ NhlNtrXLog,NhlCtrXLog,NhlTBoolean,sizeof(NhlBoolean),
		Oset(x_log),NhlTProcedure,
		 _NhlUSET((NhlPointer)_NhlResUnset),_NhlRES_INTERCEPTED,NULL},
	{"no.res","No.res",NhlTBoolean,sizeof(NhlBoolean),
		 Oset(x_reverse_set),NhlTImmediate,
		 _NhlUSET((NhlPointer)True),_NhlRES_PRIVATE,NULL},
	{ NhlNtrXReverse,NhlCtrXReverse,NhlTBoolean,sizeof(NhlBoolean),
		 Oset(x_reverse),NhlTProcedure,
		 _NhlUSET((NhlPointer)_NhlResUnset),_NhlRES_INTERCEPTED,NULL},
	{"no.res","No.res",NhlTBoolean,sizeof(NhlBoolean),
		 Oset(y_min_set),NhlTImmediate,
		 _NhlUSET((NhlPointer)True),_NhlRES_PRIVATE,NULL},
	{ NhlNtrYMinF,NhlCtrYMinF,NhlTFloat,sizeof(float),
		Oset(y_min),NhlTProcedure,
		 _NhlUSET((NhlPointer)_NhlResUnset),_NhlRES_INTERCEPTED,NULL},
	{"no.res","No.res",NhlTBoolean,sizeof(NhlBoolean),
		 Oset(y_max_set),NhlTImmediate,
		 _NhlUSET((NhlPointer)True),_NhlRES_PRIVATE,NULL},
	{ NhlNtrYMaxF,NhlCtrYMaxF,NhlTFloat,sizeof(float),
		Oset(y_max),NhlTProcedure,
		 _NhlUSET((NhlPointer)_NhlResUnset),_NhlRES_INTERCEPTED,NULL},
	{"no.res","No.res",NhlTBoolean,sizeof(NhlBoolean),
		 Oset(y_axis_type_set),NhlTImmediate,
		 _NhlUSET((NhlPointer)True),_NhlRES_PRIVATE,NULL},
	{ NhlNtrYAxisType,NhlCtrYAxisType,NhlTAxisType,sizeof(NhlAxisType),
		Oset(y_axis_type),NhlTProcedure,
		 _NhlUSET((NhlPointer)_NhlResUnset),_NhlRES_INTERCEPTED,NULL},
	{"no.res","No.res",NhlTBoolean,sizeof(NhlBoolean),
		 Oset(y_log_set),NhlTImmediate,
		 _NhlUSET((NhlPointer)True),_NhlRES_PRIVATE,NULL},
	{ NhlNtrYLog,NhlCtrYLog,NhlTBoolean,sizeof(NhlBoolean),
		Oset(y_log),NhlTProcedure,
		 _NhlUSET((NhlPointer)_NhlResUnset),_NhlRES_INTERCEPTED,NULL},
	{"no.res","No.res",NhlTBoolean,sizeof(NhlBoolean),
		 Oset(y_reverse_set),NhlTImmediate,
		 _NhlUSET((NhlPointer)True),_NhlRES_PRIVATE,NULL},
	{ NhlNtrYReverse,NhlCtrYReverse,NhlTBoolean,sizeof(NhlBoolean),
		 Oset(y_reverse),NhlTProcedure,
		 _NhlUSET((NhlPointer)_NhlResUnset),_NhlRES_INTERCEPTED,NULL},
	{ NhlNtrDataXStartF, NhlCtrDataXStartF, NhlTFloat, sizeof(float),
          	Oset(data_xstart),NhlTString, _NhlUSET("0.0"),
          	_NhlRES_GONLY|_NhlRES_INTERCEPTED,NULL },
	{ NhlNtrDataXEndF, NhlCtrDataXEndF, NhlTFloat, sizeof(float),
		Oset(data_xend),NhlTString, _NhlUSET("0.0"),
        	_NhlRES_GONLY|_NhlRES_INTERCEPTED,NULL },
	{ NhlNtrDataYStartF, NhlCtrDataYStartF, NhlTFloat, sizeof(float),
		Oset(data_ystart),NhlTString, _NhlUSET("0.0"),
        	_NhlRES_GONLY|_NhlRES_INTERCEPTED,NULL },
	{ NhlNtrDataYEndF, NhlCtrDataYEndF, NhlTFloat, sizeof(float),
		Oset(data_yend),NhlTString, _NhlUSET("0.0"),
        	_NhlRES_GONLY|_NhlRES_INTERCEPTED,NULL },
	{ NhlNtrLineInterpolationOn,NhlCtrLineInterpolationOn,
		NhlTBoolean,sizeof(NhlBoolean),
		Oset(line_interpolation_on),NhlTImmediate,
	  	_NhlUSET((NhlPointer)False),_NhlRES_INTERCEPTED,NULL}
        
};

#undef Oset

/*
* Transform Methods
*/

static NhlErrorTypes TransformClassPartInit(
#if	NhlNeedProto
	NhlClass	lc
#endif
);

static NhlErrorTypes TransformInitialize(
#if	NhlNeedProto
        NhlClass,  /* class */
        NhlLayer,       /* req */
        NhlLayer,       /* new */
        _NhlArgList,    /* args */
        int             /* num_args */
#endif
);

static NhlErrorTypes TransformSetValues(
#if	NhlNeedProto
        NhlLayer,       /* old */
        NhlLayer,       /* reference */
        NhlLayer,       /* new */
        _NhlArgList,    /* args */
        int             /* num_args*/
#endif
);


static NhlErrorTypes TransformDataToNDC(
#if	NhlNeedProto
	NhlLayer		/* plot */,
	float*		/* x */,
	float*		/* y */,
	int		/* n */,
	float*		/* xout */,
	float*		/* yout */,
	float*		/*xmissing*/,
	float*		/*ymissing*/,
	int*		/*status*/,
	float*		/*out_of_range*/
#endif
);

static NhlErrorTypes TransformNDCToData(
#if	NhlNeedProto
	NhlLayer		/* plot */,
	float*		/* x */,
	float*		/* y */,
	int		/* n */,
	float*		/* xout */,
	float*		/* yout */,
	float*		/*xmissing*/,
	float*		/*ymissing*/,
	int*		/*status*/,
	float*		/*out_of_range*/
#endif
);

static NhlErrorTypes TransformDataPolyline(
#if	NhlNeedProto
	NhlLayer	/* plot */,
	float*		/* x */,
	float*		/* y */,
	int		/* n */
#endif
);

static NhlErrorTypes TransformNDCPolyline(
#if	NhlNeedProto
	NhlLayer	/* plot */,
	float*		/* x */,
	float*		/* y */,
	int		/* n */
#endif
);

static NhlErrorTypes TransformDataPolygon(
#if	NhlNeedProto
	NhlLayer	/* plot */,
	float*		/* x */,
	float*		/* y */,
	int		/* n */
#endif
);

static NhlErrorTypes TransformNDCPolygon(
#if	NhlNeedProto
	NhlLayer	/* plot */,
	float*		/* x */,
	float*		/* y */,
	int		/* n */
#endif
);

static NhlErrorTypes TransformDataPolymarker(
#if	NhlNeedProto
	NhlLayer	/* plot */,
	float*		/* x */,
	float*		/* y */,
	int		/* n */
#endif
);

static NhlErrorTypes TransformNDCPolymarker(
#if	NhlNeedProto
	NhlLayer	/* plot */,
	float*		/* x */,
	float*		/* y */,
	int		/* n */
#endif
);

NhlTransformClassRec NhltransformClassRec = {
        {
/* class_name			*/      "transformClass",
/* nrm_class			*/      NrmNULLQUARK,
/* layer_size			*/      sizeof(NhlTransformLayerRec),
/* class_inited			*/      False,
/* superclass			*/      (NhlClass)&NhlviewClassRec,
/* cvt_table			*/	NULL,

/* layer_resources		*/	resources,
/* num_resources		*/	NhlNumber(resources),
/* all_resources		*/	NULL,
/* callbacks			*/	callbacks,
/* num_callbacks		*/	NhlNumber(callbacks),
/* class_callbacks		*/	NULL,
/* num_class_callbacks		*/	0,

/* class_part_initialize	*/	TransformClassPartInit,
/* class_initialize		*/	NULL,
/* layer_initialize		*/	TransformInitialize,
/* layer_set_values		*/	TransformSetValues,
/* layer_set_values_hook	*/	NULL,
/* layer_get_values		*/	NULL,
/* layer_reparent		*/	NULL,
/* layer_destroy		*/	NULL,

/* child_resources		*/	NULL,

/* layer_draw			*/      NULL,

/* layer_pre_draw		*/      NULL,
/* layer_draw_segonly		*/	NULL,
/* layer_post_draw		*/      NULL,
/* layer_clear			*/      NULL

        },
	{
/* segment_wkid			*/	0,
/* get_bb			*/	NULL
	},
	{
/* overlay_capability 		*/	_tfNotOverlayCapable,
/* data_to_ndc			*/	TransformDataToNDC,
/* ndc_to_data			*/	TransformNDCToData,
/* data_polyline		*/	TransformDataPolyline,
/* ndc_polyline			*/	TransformNDCPolyline,
/* data_polygon			*/	TransformDataPolygon,
/* ndc_polygon			*/	TransformNDCPolygon,
/* data_polymarker		*/	TransformDataPolymarker,
/* ndc_polymarker		*/	TransformNDCPolymarker
	}
};
	
NhlClass NhltransformClass = (NhlClass)&NhltransformClassRec;

/*
 * Function:	TransformClassPartInit
 *
 * Description:	
 *
 * In Args:	
 *
 * Out Args:	
 *
 * Scope:	
 * Returns:	
 * Side Effect:	
 */
static NhlErrorTypes
TransformClassPartInit
#if	NhlNeedProto
(
	NhlClass	lc
)
#else
(lc)
	NhlClass	lc;
#endif
{
	NhlTransformClass	tlc = (NhlTransformClass)lc;
	NhlTransformClass	sc = (NhlTransformClass)
						lc->base_class.superclass;

        _NhlEnumVals   axistypelist[] = {
	{NhlIRREGULARAXIS,	"IrregularAxis"},
	{NhlLINEARAXIS,		"LinearAxis"},
	{NhlLOGAXIS,		"LogAxis"}
        };

	_NhlRegisterEnumType(NhltransformClass,
			NhlTAxisType,axistypelist,NhlNumber(axistypelist));
        
	if(tlc->trans_class.data_to_ndc == NhlInheritTransFunc)
		tlc->trans_class.data_to_ndc = sc->trans_class.data_to_ndc;
	if(tlc->trans_class.ndc_to_data == NhlInheritTransFunc)
		tlc->trans_class.ndc_to_data = sc->trans_class.ndc_to_data;

	if(tlc->trans_class.data_polyline == NhlInheritPolyTransFunc)
		tlc->trans_class.data_polyline = sc->trans_class.data_polyline;
	if(tlc->trans_class.ndc_polyline == NhlInheritPolyTransFunc)
		tlc->trans_class.ndc_polyline = sc->trans_class.ndc_polyline;

	if(tlc->trans_class.data_polygon == NhlInheritPolyTransFunc)
		tlc->trans_class.data_polygon = sc->trans_class.data_polygon;
	if(tlc->trans_class.ndc_polygon == NhlInheritPolyTransFunc)
		tlc->trans_class.ndc_polygon = sc->trans_class.ndc_polygon;

	if(tlc->trans_class.data_polymarker == NhlInheritPolyTransFunc)
		tlc->trans_class.data_polymarker 
			= sc->trans_class.data_polymarker;
	if(tlc->trans_class.ndc_polymarker == NhlInheritPolyTransFunc)
		tlc->trans_class.ndc_polymarker 
			= sc->trans_class.ndc_polymarker;
        
	return NhlNOERROR;
}
 
/*
 * Function:	TransformInitialize
 *
 * Description: 
 *
 * In Args: 	class	objects layer_class
 *		req	instance record of requested values
 *		new	instance record of new object
 *		args	list of resources and values for reference
 *		num_args 	number of elements in args.
 *
 * Out Args:	NONE
 *
 * Return Values:	Error Conditions
 *
 * Side Effects:	state change in GKS due to mapping transformations.
 */
/*ARGSUSED*/
static NhlErrorTypes
TransformInitialize
#if	NhlNeedProto
(
	NhlClass	class,
	NhlLayer		req,
	NhlLayer		new,
	_NhlArgList	args,
	int		num_args
)
#else
(class,req,new,args,num_args)
        NhlClass      class;
        NhlLayer           req;
        NhlLayer           new;
        _NhlArgList     args;
        int             num_args;
#endif
{
	NhlErrorTypes		ret = NhlNOERROR, subret = NhlNOERROR;
	char			*entry_name = "TransformInitialize";
	char			*e_text;
	NhlTransformLayer	tnew = (NhlTransformLayer) new;
	NhlTransformLayerPart	*tfp = &(tnew->trans);

	tfp->data_xstart = 0.0;
	tfp->data_xend = 1.0;
	tfp->data_ystart = 0.0;
	tfp->data_yend = 1.0;
        
	if (tfp->x_min_set)
		tfp->sticky_x_min_set = True;
	else {
		tfp->x_min = 0.0;
		tfp->sticky_x_min_set = False;
	}
	if (tfp->x_max_set)
		tfp->sticky_x_max_set = True;
	else {
		tfp->x_max = 1.0;
		tfp->sticky_x_max_set = False;
	}
	if (tfp->y_min_set)
		tfp->sticky_y_min_set = True;
	else {
		tfp->y_min = 0.0;
		tfp->sticky_y_min_set = False;
	}
	if (tfp->y_max_set)
		tfp->sticky_y_max_set = True;
	else {
		tfp->y_max = 1.0;
		tfp->sticky_y_max_set = False;
	}
	if (! tfp->x_reverse_set)
		tfp->x_reverse = False;
	if (! tfp->y_reverse_set)
		tfp->y_reverse = False;
        
        if (tfp->x_axis_type_set) {
                tfp->x_log = tfp->x_axis_type == NhlLOGAXIS ? True : False;
                tfp->x_log_set = True;
        }
	else if (tfp->x_log_set) {
                tfp->x_axis_type = tfp->x_log ? NhlLOGAXIS : NhlLINEARAXIS;
                tfp->x_axis_type_set = True;
        }
        else {
                tfp->x_log = False;
                tfp->x_axis_type = NhlLINEARAXIS;
        }
        
        if (tfp->y_axis_type_set) {
                tfp->y_log = tfp->y_axis_type == NhlLOGAXIS ? True : False;
                tfp->y_log_set = True;
        }
	else if (tfp->y_log_set) {
                tfp->y_axis_type = tfp->y_log ? NhlLOGAXIS : NhlLINEARAXIS;
                tfp->y_axis_type_set = True;
        }
        else {
                tfp->y_log = False;
                tfp->y_axis_type = NhlLINEARAXIS;
        }
        return NhlNOERROR;
        
}
/*
 * Function:	TransformSetValues
 *
 * Description: 
 *
 * In Args:	old	copy of old instance record
 *		reference	requested instance record
 *		new	new instance record	
 *		args 	list of resources and values for reference
 *		num_args	number of elements in args.
 *
 * Out Args:	NONE
 *
 * Return Values:	ErrorConditions
 *
 * Side Effects:
 */
/*ARGSUSED*/
static NhlErrorTypes TransformSetValues
#if	NhlNeedProto
(
	NhlLayer	old,
	NhlLayer	reference,
	NhlLayer	new,
	_NhlArgList	args,
	int		num_args
)
#else
(old,reference,new,args,num_args)
	NhlLayer	old;
	NhlLayer	reference;
	NhlLayer	new;
	_NhlArgList	args;
	int		num_args;
#endif
{
	NhlErrorTypes		ret = NhlNOERROR, subret = NhlNOERROR;
	char			*entry_name = "TransformSetValues";
	char			*e_text;
	NhlTransformLayer	tnew = (NhlTransformLayer) new;
 	NhlTransformLayerPart	*tfp = &(tnew->trans);
	NhlTransformLayer	told = (NhlTransformLayer) old;

	if (_NhlArgIsSet(args,num_args,NhlNtrXMinF)) {
		tfp->x_min_set = True;
                tfp->sticky_x_min_set = True;
        }
	if (_NhlArgIsSet(args,num_args,NhlNtrXMaxF)) {
		tfp->x_max_set = True;
                tfp->sticky_x_max_set = True;
        }
	if (_NhlArgIsSet(args,num_args,NhlNtrXReverse))
		tfp->x_reverse_set = True;
	if (_NhlArgIsSet(args,num_args,NhlNtrXLog))
		tfp->x_log_set = True;
	if (_NhlArgIsSet(args,num_args,NhlNtrXAxisType))
		tfp->x_axis_type_set = True;
	if (_NhlArgIsSet(args,num_args,NhlNtrYMinF)) {
		tfp->y_min_set = True;
                tfp->sticky_y_min_set = True;
        }
	if (_NhlArgIsSet(args,num_args,NhlNtrYMaxF)) {
		tfp->y_max_set = True;
                tfp->sticky_y_max_set = True;
        }
	if (_NhlArgIsSet(args,num_args,NhlNtrYReverse))
		tfp->y_reverse_set = True;
	if (_NhlArgIsSet(args,num_args,NhlNtrYLog))
		tfp->y_log_set = True;
	if (_NhlArgIsSet(args,num_args,NhlNtrYAxisType))
		tfp->y_axis_type_set = True;
        
        if (tfp->x_axis_type_set) {
                tfp->x_log = tfp->x_axis_type == NhlLOGAXIS ? True : False;
                tfp->x_log_set = True;
        }
	else if (tfp->x_log_set) {
                if (tfp->x_log) {
                        tfp->x_axis_type = NhlLOGAXIS;
                        tfp->x_axis_type_set = True;
                }
                else if (tfp->x_axis_type != NhlIRREGULARAXIS) {
                        tfp->x_axis_type = NhlLINEARAXIS;
                        tfp->x_axis_type_set = True;
                }
        }
        
        if (tfp->y_axis_type_set) {
                tfp->y_log = tfp->y_axis_type == NhlLOGAXIS ? True : False;
                tfp->y_log_set = True;
        }
	else if (tfp->y_log_set) {
                if (tfp->y_log) {
                        tfp->y_axis_type = NhlLOGAXIS;
                        tfp->y_axis_type_set = True;
                }
                else if (tfp->y_axis_type != NhlIRREGULARAXIS) {
                        tfp->y_axis_type = NhlLINEARAXIS;
                        tfp->y_axis_type_set = True;
                }
        }

        return NhlNOERROR;
}

/*
 * Function:	TransformDataToNDC
 *
 * Description: Transform Data space to NDC space method for the Transform. 
 *		Takes one or more x,y pairs of data points and converts them
 *		into their respective NDC values.
 *
 * In Args:	plot	instance record
 *		x	array of x axis values in data coordinates
 * 		y	array of y axis values in data coordinates
 *		n	number of elements
 *		xout	storage provided for output x ndc vals
 *		yout	storage provided for output y ndc vals
 *		xmissing	holds value of missing value in x if NULL
 *				no missing value
 *		ymissing	holds value of missing value in y if NULL
 *				no missing value
 *
 * Out Args:	xout	does not allocate storage
 *		yout	does not allocate storage
 *		status  1 if an out of range condition occurs; else 0;
 *		out_of_range	the out of range indicator value that
 *				replaces all out of range values.
 *
 * Return Values:	Error Conditions
 *
 * Side Effects:	NONE
 */
static NhlErrorTypes TransformDataToNDC
#if	NhlNeedProto
(NhlLayer plot,float* x,float* y,int n,
 float* xout,float* yout,float* xmissing,float* ymissing, 
 int *status, float * out_of_range)
#else
(plot,x,y,n,xout,yout,xmissing,ymissing,status,out_of_range)
	NhlLayer		plot;
	float		*x;
	float		*y;
	int		n;
	float		*xout;
	float		*yout;
	float		*xmissing;
	float		*ymissing;
	int		*status;
	float		*out_of_range;
#endif
{
	char			*entry_name = "TransformNDCToData";
	NhlErrorTypes		ret = NhlNOERROR, subret = NhlNOERROR;
	char			*e_text;
	NhlTransformLayerPart	*tfp = &(((NhlTransformLayer) plot)->trans);
	NhlLayer		top;
	int			mystatus = 0;

	if (tfp->overlay_status == _tfCurrentOverlayMember && 
	    ! tfp->do_ndc_overlay &&
	    tfp->overlay_trans_obj != NULL) {
		top = tfp->overlay_trans_obj;
	}
	else {
		if ((top = tfp->trans_obj) == NULL) {
			e_text = 
			   "%s: no transformation object recorded for pid %d";
			NhlPError(NhlFATAL,NhlEUNKNOWN,e_text,
				  entry_name,plot->base.id);
			return(ret);
	        }
	}

/*
 * If the TransObj is a MapTransObj then set the longitude limits 
 * expected by the data.
 * The MapTransSetValues call does a SetTrans internally, so there's
 * no need to explicitly do a SetTrans in this case.
 * ***WRONG*** actually when these resources are set by themselves it's
 * a special case and no SetTrans happens. So SetTrans must always be called.
 */
	if ((top->base.layer_class)->base_class.class_name 
	    == NhlmapTransObjClass->base_class.class_name) {
		subret = NhlVASetValues(top->base.id,
					NhlNtrDataXStartF,tfp->data_xstart,
					NhlNtrDataXEndF,tfp->data_xend,
					NULL);
	}
	subret = _NhlSetTrans(top, plot);

	if ((ret = MIN(ret,subret)) < NhlWARNING) {
		e_text = "%s: error setting transformation";
		NhlPError(NhlFATAL,NhlEUNKNOWN,e_text, entry_name);
		return(ret);
	}

	NhlVAGetValues(top->base.id,
		     NhlNtrOutOfRangeF,out_of_range,NULL);

	subret = _NhlDataToWin(top,x,y,n,xout,yout,
			       &mystatus,xmissing,ymissing);

	if ((ret = MIN(ret,subret)) < NhlWARNING) {
		e_text = "%s: error transforming from NDC to window";
		NhlPError(NhlFATAL,NhlEUNKNOWN,e_text, entry_name);
		return ret;
	}
	*status = mystatus ? 1 : 0;

	subret = _NhlWinToNDC(top,xout,yout,n,xout,yout,
			      &mystatus,out_of_range,out_of_range);

	if ((ret = MIN(ret,subret)) < NhlWARNING) {
		e_text = "%s: error transforming from window to data";
		NhlPError(NhlFATAL,NhlEUNKNOWN,e_text, entry_name);
		return ret;
	}
	*status = mystatus ? 1 : *status;

	return(ret);

}

/*
 * Function:	TransformNDCToData
 *
 * Description:	Transform objects NDC to Data method for the Transform. 
 *		Takes one or more x,y pairs of NDC points and converts them
 *		into their respective data values.
 *
 * In Args:	plot 	instance record pointer
 *		x	array of x ndc vals to convert
 *		y	array of y ndc vals to convert
 *		n	number of elements in x,y,xout and yout
 *	 	xout	storage provided by user for conversion output
 *		yout	storage provided by user for conversion output
 *		xmissing  missing values in x input
 *		ymissing  missing values in y input
 *
 * Out Args:	xout	but does not allocate storage
 *		yout	but does not allocate storage
 *		status  1 if an out of range condition occurs; else 0;
 *		out_of_range	the out of range indicator value that
 *				replaces all out of range values.
 *
 * Return Values:	Error Conditions
 *
 * Side Effects:	 NONE
 */
static NhlErrorTypes TransformNDCToData
#if	NhlNeedProto
(NhlLayer plot,float* x,float* y,int n,
 float* xout,float* yout,float *xmissing,float *ymissing,
 int *status, float * out_of_range)
#else
(plot,x,y,n,xout,yout,xmissing,ymissing,status,out_of_range)
	NhlLayer		plot;
	float*		x;
	float*		y;
	int		n;
	float*		xout;
	float*		yout;
	float*		xmissing;
	float*		ymissing;
	int		*status;
	float		*out_of_range;
#endif
{
	char			*entry_name = "TransformNDCToData";
	NhlErrorTypes		ret = NhlNOERROR, subret = NhlNOERROR;
	char			*e_text;
	NhlTransformLayerPart	*tfp = &(((NhlTransformLayer) plot)->trans);
	NhlLayer		top;
	int			mystatus = 0;

	if (tfp->overlay_status == _tfCurrentOverlayMember && 
	    ! tfp->do_ndc_overlay &&
	    tfp->overlay_trans_obj != NULL) {
		top = tfp->overlay_trans_obj;
	}
	else {
		if ((top = tfp->trans_obj) == NULL) {
			e_text = 
			   "%s: no transformation object recorded for pid %d";
			NhlPError(NhlFATAL,NhlEUNKNOWN,e_text,
				  entry_name,plot->base.id);
			return(ret);
	        }
	}

	subret = _NhlSetTrans(top, plot);

	if ((ret = MIN(ret,subret)) < NhlWARNING) {
		e_text = "%s: error setting transformation";
		NhlPError(NhlFATAL,NhlEUNKNOWN,e_text, entry_name);
		return(ret);
	}

	NhlVAGetValues(top->base.id,
		     NhlNtrOutOfRangeF,out_of_range,NULL);

	subret = _NhlNDCToWin(top,x,y,n,xout,yout,
			      &mystatus,xmissing,ymissing);

	if ((ret = MIN(ret,subret)) < NhlWARNING) {
		e_text = "%s: error transforming from NDC to window";
		NhlPError(NhlFATAL,NhlEUNKNOWN,e_text, entry_name);
		return ret;
	}
	*status = mystatus ? 1 : 0;

	subret = _NhlWinToData(top,xout,yout,n,xout,yout,
			       &mystatus,out_of_range,out_of_range);

	if ((ret = MIN(ret,subret)) < NhlWARNING) {
		e_text = "%s: error transforming from window to data";
		NhlPError(NhlFATAL,NhlEUNKNOWN,e_text, entry_name);
		return ret;
	}
	*status = mystatus ? 1 : *status;

	return(ret);

}

/*
 * Function:	TransformDataPolyline
 *
 * Description:	Immediate mode drawing of a polyline whose points are
 *		given in data coordinate space.
 *
 * In Args:	plot 	instance record pointer
 *		x	array of x data coordinate values
 *		y	array of y data coordinate values
 *		n	number of elements in x and y
 *
 * Out Args:	none
 *
 * Return Values:	Error Conditions
 *
 * Side Effects:	 NONE
 */
static NhlErrorTypes TransformDataPolyline
#if	NhlNeedProto
(NhlLayer plot,float* x,float* y,int n)
#else
(plot,x,y,n)
	NhlLayer		plot;
	float*		x;
	float*		y;
	int		n;
#endif
{
	NhlErrorTypes		ret = NhlNOERROR, subret = NhlNOERROR;
	NhlTransformLayer		tl = (NhlTransformLayer) plot;
	NhlTransformLayerPart	*tfp = &(tl->trans);
	NhlTransObjLayer		top;
 	NhlTransObjClass 	tocp;
	char			*e_text;
	char			*entry_name = "TransformDataPolyline";
	int			i;
	NhlBoolean		ismaptrans = False;

	if (n < 2) {
		e_text = "%s, not enough points for a line";
		NhlPError(NhlWARNING,NhlEUNKNOWN,e_text, entry_name);
		return NhlWARNING;
	}

/* 
 * Set up the transformation based on whether the plot is part of an
 * overlay.
 */
	if (tfp->overlay_status == _tfCurrentOverlayMember && 
	    ! tfp->do_ndc_overlay &&
	    tfp->overlay_trans_obj != NULL) {
		top = (NhlTransObjLayer) tfp->overlay_trans_obj;
	}
	else {
		if ((top = (NhlTransObjLayer) tfp->trans_obj) == NULL) {
			e_text = 
			   "%s: no transformation object recorded for pid %d";
			NhlPError(NhlFATAL,NhlEUNKNOWN,e_text,
				  entry_name,plot->base.id);
			return(ret);
	        }
	}

	tocp = (NhlTransObjClass) (top->base.layer_class);
	if (tocp->base_class.class_name == 
	    NhlmapTransObjClass->base_class.class_name) 
		ismaptrans = True;

	subret = _NhlActivateWorkstation(tl->base.wkptr);

	if ((ret = MIN(ret,subret)) < NhlWARNING) {
		e_text = "%s: error activating workstation";
		NhlPError(NhlFATAL,NhlEUNKNOWN,e_text, entry_name);
		return(ret);
	}

	_NhlSetLineInfo(tl->base.wkptr, plot);

/* Set the transformation */

	subret = _NhlSetTrans((NhlLayer) top, plot);

	if ((ret = MIN(ret,subret)) < NhlWARNING) {
		e_text = "%s: error setting transformation";
		NhlPError(NhlFATAL,NhlEUNKNOWN,e_text, entry_name);
		return(ret);
	}


	gset_clip_ind(GIND_CLIP);
/* Do a pen up to the first point */

	subret = _NhlDataLineTo((NhlLayer)top,*x++,*y++,1);

	if ((ret = MIN(ret,subret)) < NhlWARNING) 
		return ret;

/* Pen down for the remaining lines */

	for (i = 1; i < n; i++) { 
		subret = _NhlDataLineTo((NhlLayer)top,*x++,*y++,0);

		if ((ret = MIN(ret,subret)) < NhlWARNING) 
			return ret;

	}

/*
 * This call ensures a NCAR LASTD call
 */
	if (ismaptrans) {
		c_mapiqd();
	}
	else {
		subret = _NhlWorkstationLineTo(tl->base.wkptr,0.0,0.0,1);
	}

	c_plotif(0.0,0.0,2);

	gset_clip_ind(GIND_NO_CLIP);

        subret = _NhlDeactivateWorkstation(tl->base.wkptr);

	if ((ret = MIN(ret,subret)) < NhlWARNING) {
		e_text = "%s: error deactivating workstation";
		NhlPError(NhlFATAL,NhlEUNKNOWN,e_text, entry_name);
		return(ret);
	}

	return MIN(ret,subret);
}

/*
 * Function:	TransformNDCPolyline
 *
 * Description:	Immediate mode drawing of a polyline whose points are
 *		given in NDC space.
 *
 * In Args:	plot 	instance record pointer
 *		x	array of x NDC values
 *		y	array of y NDC values
 *		n	number of elements in x and y
 *
 * Out Args:	none
 *
 * Return Values:	Error Conditions
 *
 * Side Effects:	 NONE
 */
static NhlErrorTypes TransformNDCPolyline
#if	NhlNeedProto
(NhlLayer plot,float* x,float* y,int n)
#else
(plot,x,y,n)
	NhlLayer		plot;
	float*		x;
	float*		y;
	int		n;
#endif
{
	char			*entry_name = "TransformNDCPolyline";
	char			*e_text;
	NhlErrorTypes		ret = NhlNOERROR, subret = NhlNOERROR;
	NhlTransformLayer		tl = (NhlTransformLayer) plot;
	NhlTransformLayerPart	*tfp = &(tl->trans);
	NhlTransObjLayer		top;
 	NhlTransObjClass 	tocp;
	int			i;

	if (n < 2) {
		e_text = "%s, not enough points for a line";
		NhlPError(NhlWARNING,NhlEUNKNOWN,e_text, entry_name);
		return NhlWARNING;
	}

/* 
 * Set up the transformation based on whether the plot is part of an
 * overlay.
 */
	if (tfp->overlay_status == _tfCurrentOverlayMember && 
	    ! tfp->do_ndc_overlay &&
	    tfp->overlay_trans_obj != NULL) {
		top = (NhlTransObjLayer) tfp->overlay_trans_obj;
	}
	else {
		if ((top = (NhlTransObjLayer) tfp->trans_obj) == NULL) {
			e_text = 
			   "%s: no transformation object recorded for pid %d";
			NhlPError(NhlFATAL,NhlEUNKNOWN,e_text,
				  entry_name,plot->base.id);
			return(ret);
	        }
	}
	tocp = (NhlTransObjClass) (top->base.layer_class);

	subret = _NhlActivateWorkstation(tl->base.wkptr);

	if ((ret = MIN(ret,subret)) < NhlWARNING) {
		e_text = "%s: error activating workstation";
		NhlPError(NhlFATAL,NhlEUNKNOWN,e_text, entry_name);
		return(ret);
	}

	_NhlSetLineInfo(tl->base.wkptr, plot);

/* Not sure if a set trans is required */

	subret = _NhlSetTrans((NhlLayer) top, plot);

	if ((ret = MIN(ret,subret)) < NhlWARNING) {
		e_text = "%s: error setting transformation";
		NhlPError(NhlFATAL,NhlEUNKNOWN,e_text, entry_name);
		return(ret);
	}

	gset_clip_ind(GIND_CLIP);

/* Do a pen up to the first point */

	subret = _NhlNDCLineTo((NhlLayer)top,*x++,*y++,1);

	if ((ret = MIN(ret,subret)) < NhlWARNING) 
		return ret;

/* Pen down for the remaining lines */

	for (i = 1; i < n; i++) { 
		subret = _NhlNDCLineTo((NhlLayer)top,*x++,*y++,0);

		if ((ret = MIN(ret,subret)) < NhlWARNING) 
			return ret;

	}

/*
 * This call ensures a NCAR LASTD call
 */
	subret = _NhlWorkstationLineTo(tl->base.wkptr,0.0,0.0,1);
	c_plotif(0.0,0.0,2);
	gset_clip_ind(GIND_NO_CLIP);


	if ((ret = MIN(ret,subret)) < NhlWARNING) {
		e_text = "%s: error drawing polyline";
		NhlPError(NhlFATAL,NhlEUNKNOWN,e_text, entry_name);
		return(ret);
	}

        subret = _NhlDeactivateWorkstation(tl->base.wkptr);

	if ((ret = MIN(ret,subret)) < NhlWARNING) {
		e_text = "%s: error deactivating workstation";
		NhlPError(NhlFATAL,NhlEUNKNOWN,e_text, entry_name);
		return(ret);
	}

	return MIN(ret,subret);
}


/*
 * Function:	TransformDataPolygon
 *
 * Description:	Immediate mode drawing of a polygon whose points are
 *		given in data coordinate space.
 *
 * In Args:	plot 	instance record pointer
 *		x	array of x data coordinate values
 *		y	array of y data coordinate values
 *		n	number of elements in x and y
 *
 * Out Args:	none
 *
 * Return Values:	Error Conditions
 *
 * Side Effects:	 NONE
 */
static NhlErrorTypes TransformDataPolygon
#if	NhlNeedProto
(NhlLayer plot,float* x,float* y,int n)
#else
(plot,x,y,n)
	NhlLayer		plot;
	float*		x;
	float*		y;
	int		n;
#endif
{
	NhlErrorTypes		ret = NhlNOERROR, subret = NhlNOERROR;
	NhlTransformLayer		tl = (NhlTransformLayer) plot;
	NhlTransformLayerPart	*tfp = &(tl->trans);
	NhlTransObjLayer		top;
 	NhlTransObjClass 	tocp;
	char			*e_text;
	char			*entry_name = "TransformDataPolygon";

	if (n < 3) {
		e_text = "%s, not enough points for a polygon";
		NhlPError(NhlWARNING,NhlEUNKNOWN,e_text, entry_name);
		return NhlWARNING;
	}

/* 
 * Set up the transformation based on whether the plot is part of an
 * overlay.
 */
	if (tfp->overlay_status == _tfCurrentOverlayMember && 
	    ! tfp->do_ndc_overlay &&
	    tfp->overlay_trans_obj != NULL) {
		top = (NhlTransObjLayer) tfp->overlay_trans_obj;
	}
	else {
		if ((top = (NhlTransObjLayer) tfp->trans_obj) == NULL) {
			e_text = 
			   "%s: no transformation object recorded for pid %d";
			NhlPError(NhlFATAL,NhlEUNKNOWN,e_text,
				  entry_name,plot->base.id);
			return(ret);
	        }
	}

	tocp = (NhlTransObjClass) (top->base.layer_class);

	subret = _NhlActivateWorkstation(tl->base.wkptr);

	if ((ret = MIN(ret,subret)) < NhlWARNING) {
		e_text = "%s: error activating workstation";
		NhlPError(NhlFATAL,NhlEUNKNOWN,e_text, entry_name);
		return(ret);
	}

	_NhlSetFillInfo(tl->base.wkptr, plot);

/* Set the transformation */

	subret = _NhlSetTrans((NhlLayer) top, plot);

	if ((ret = MIN(ret,subret)) < NhlWARNING) {
		e_text = "%s: error setting transformation";
		NhlPError(NhlFATAL,NhlEUNKNOWN,e_text, entry_name);
		return(ret);
	}

	gset_clip_ind(GIND_CLIP);
	subret = _NhlDataPolygon((NhlLayer)top,x,y,n);
	gset_clip_ind(GIND_NO_CLIP);

        subret = _NhlDeactivateWorkstation(tl->base.wkptr);

	if ((ret = MIN(ret,subret)) < NhlWARNING) {
		e_text = "%s: error deactivating workstation";
		NhlPError(NhlFATAL,NhlEUNKNOWN,e_text, entry_name);
		return(ret);
	}

	return MIN(ret,subret);
}

/*
 * Function:	TransformNDCPolygon
 *
 * Description:	Immediate mode drawing of a polygon whose points are
 *		given in NDC space.
 *
 * In Args:	plot 	instance record pointer
 *		x	array of x NDC values
 *		y	array of y NDC values
 *		n	number of elements in x and y
 *
 * Out Args:	none
 *
 * Return Values:	Error Conditions
 *
 * Side Effects:	 NONE
 */
static NhlErrorTypes TransformNDCPolygon
#if	NhlNeedProto
(NhlLayer plot,float* x,float* y,int n)
#else
(plot,x,y,n)
	NhlLayer		plot;
	float*		x;
	float*		y;
	int		n;
#endif
{
	char			*entry_name = "TransformNDCPolygon";
	char			*e_text;
	NhlErrorTypes		ret = NhlNOERROR, subret = NhlNOERROR;
	NhlTransformLayer		tl = (NhlTransformLayer) plot;
	NhlTransformLayerPart	*tfp = &(tl->trans);
	NhlTransObjLayer		top;
 	NhlTransObjClass 	tocp;

	if (n < 3) {
		e_text = "%s, not enough points for a polygon";
		NhlPError(NhlWARNING,NhlEUNKNOWN,e_text, entry_name);
		return NhlWARNING;
	}

/* 
 * Set up the transformation based on whether the plot is part of an
 * overlay.
 */
	if (tfp->overlay_status == _tfCurrentOverlayMember && 
	    ! tfp->do_ndc_overlay &&
	    tfp->overlay_trans_obj != NULL) {
		top = (NhlTransObjLayer) tfp->overlay_trans_obj;
	}
	else {
		if ((top = (NhlTransObjLayer) tfp->trans_obj) == NULL) {
			e_text = 
			   "%s: no transformation object recorded for pid %d";
			NhlPError(NhlFATAL,NhlEUNKNOWN,e_text,
				  entry_name,plot->base.id);
			return(ret);
	        }
	}
	tocp = (NhlTransObjClass) (top->base.layer_class);

	subret = _NhlActivateWorkstation(tl->base.wkptr);

	if ((ret = MIN(ret,subret)) < NhlWARNING) {
		e_text = "%s: error activating workstation";
		NhlPError(NhlFATAL,NhlEUNKNOWN,e_text, entry_name);
		return(ret);
	}

/* Set the transformation */

	subret = _NhlSetTrans((NhlLayer) top, plot);

	if ((ret = MIN(ret,subret)) < NhlWARNING) {
		e_text = "%s: error setting transformation";
		NhlPError(NhlFATAL,NhlEUNKNOWN,e_text, entry_name);
		return(ret);
	}

	_NhlSetFillInfo(tl->base.wkptr, plot);

	gset_clip_ind(GIND_CLIP);
	subret = _NhlWorkstationFill(tl->base.wkptr,x,y,n);
	gset_clip_ind(GIND_NO_CLIP);

	if ((ret = MIN(ret,subret)) < NhlWARNING) {
		e_text = "%s: error drawing polygon";
		NhlPError(NhlFATAL,NhlEUNKNOWN,e_text, entry_name);
		return(ret);
	}

        subret = _NhlDeactivateWorkstation(tl->base.wkptr);

	if ((ret = MIN(ret,subret)) < NhlWARNING) {
		e_text = "%s: error deactivating workstation";
		NhlPError(NhlFATAL,NhlEUNKNOWN,e_text, entry_name);
		return(ret);
	}

	return MIN(ret,subret);
}


/*
 * Function:	TransformDataPolymarker
 *
 * Description:	Immediate mode drawing of a polymarker whose points are
 *		given in data coordinate space.
 *
 * In Args:	plot 	instance record pointer
 *		x	array of x data coordinate values
 *		y	array of y data coordinate values
 *		n	number of elements in x and y
 *
 * Out Args:	none
 *
 * Return Values:	Error Conditions
 *
 * Side Effects:	 NONE
 */
static NhlErrorTypes TransformDataPolymarker
#if	NhlNeedProto
(NhlLayer plot,float* x,float* y,int n)
#else
(plot,x,y,n)
	NhlLayer		plot;
	float*		x;
	float*		y;
	int		n;
#endif
{
	NhlErrorTypes		ret = NhlNOERROR, subret = NhlNOERROR;
	NhlTransformLayer		tl = (NhlTransformLayer) plot;
	NhlTransformLayerPart	*tfp = &(tl->trans);
	NhlTransObjLayer		top;
 	NhlTransObjClass 	tocp;
	char			*e_text;
	char			*entry_name = "TransformDataPolymarker";
	float			*xndc,*yndc;
	int			status;
	float			out_of_range; 

	if (n < 1) {
		e_text = "%s, polymarker is empty";
		NhlPError(NhlWARNING,NhlEUNKNOWN,e_text, entry_name);
		return NhlWARNING;
	}

/* 
 * Set up the transformation based on whether the plot is part of an
 * overlay.
 */
	if (tfp->overlay_status == _tfCurrentOverlayMember && 
	    ! tfp->do_ndc_overlay &&
	    tfp->overlay_trans_obj != NULL) {
		top = (NhlTransObjLayer) tfp->overlay_trans_obj;
	}
	else {
		if ((top = (NhlTransObjLayer) tfp->trans_obj) == NULL) {
			e_text = 
			   "%s: no transformation object recorded for pid %d";
			NhlPError(NhlFATAL,NhlEUNKNOWN,e_text,
				  entry_name,plot->base.id);
			return(ret);
	        }
	}

	xndc = NhlMalloc(n * sizeof(float));
	yndc = NhlMalloc(n * sizeof(float));
	subret = NhlDataToNDC(plot->base.id,x,y,n,xndc,yndc,
			      NULL,NULL,&status,&out_of_range);
	if ((ret = MIN(ret,subret)) < NhlWARNING) {
		e_text = "%s: error transforming marker locations";
		NhlPError(NhlFATAL,NhlEUNKNOWN,e_text, entry_name);
		return(ret);
	}
	/* throw away out of range values */

	if (status) {
		int i,j;
		i = 0;
		while (i < n-1) {
			if (xndc[i] == out_of_range || 
			    yndc[i] == out_of_range) {
				for (j = i; j < n-1; j++) {
					xndc[j] = xndc[j+1];
					yndc[j] = yndc[j+1];
				}
				n --;
			}
			else {
				i++;
			}
		}
		if (xndc[n-1] == out_of_range || yndc[n-1] == out_of_range) {
			n--;
		}
		if (n < 1) {
			e_text = "%s, polymarker out of range";
			NhlPError(NhlINFO,NhlEUNKNOWN,e_text, entry_name);
			return NhlINFO;
		}
	}

	tocp = (NhlTransObjClass) (top->base.layer_class);

	subret = _NhlActivateWorkstation(tl->base.wkptr);

	if ((ret = MIN(ret,subret)) < NhlWARNING) {
		e_text = "%s: error activating workstation";
		NhlPError(NhlFATAL,NhlEUNKNOWN,e_text, entry_name);
		return(ret);
	}

#if 0
/* Set the transformation */
/* not needed because DataToNDC has already set the trans -- but
   is it good practice to know this? */
	subret = _NhlSetTrans((NhlLayer) top, plot);

	if ((ret = MIN(ret,subret)) < NhlWARNING) {
		e_text = "%s: error setting transformation";
		NhlPError(NhlFATAL,NhlEUNKNOWN,e_text, entry_name);
		return(ret);
	}
#endif
	_NhlSetMarkerInfo(tl->base.wkptr, plot);

	gset_clip_ind(GIND_CLIP);
	subret = _NhlWorkstationMarker(tl->base.wkptr,xndc,yndc,n);
	gset_clip_ind(GIND_NO_CLIP);


	if ((ret = MIN(ret,subret)) < NhlWARNING) {
		e_text = "%s: error drawing polymarker";
		NhlPError(NhlFATAL,NhlEUNKNOWN,e_text, entry_name);
		return(ret);
	}

        subret = _NhlDeactivateWorkstation(tl->base.wkptr);

	if ((ret = MIN(ret,subret)) < NhlWARNING) {
		e_text = "%s: error deactivating workstation";
		NhlPError(NhlFATAL,NhlEUNKNOWN,e_text, entry_name);
		return(ret);
	}

	NhlFree(xndc);
	NhlFree(yndc);
	return MIN(ret,subret);
}

/*
 * Function:	TransformNDCPolymarker
 *
 * Description:	Immediate mode drawing of a polymarker whose points are
 *		given in NDC space.
 *
 * In Args:	plot 	instance record pointer
 *		x	array of x NDC values
 *		y	array of y NDC values
 *		n	number of elements in x and y
 *
 * Out Args:	none
 *
 * Return Values:	Error Conditions
 *
 * Side Effects:	 NONE
 */
static NhlErrorTypes TransformNDCPolymarker
#if	NhlNeedProto
(NhlLayer plot,float* x,float* y,int n)
#else
(plot,x,y,n)
	NhlLayer		plot;
	float*		x;
	float*		y;
	int		n;
#endif
{
	char			*entry_name = "TransformNDCPolymarker";
	char			*e_text;
	NhlErrorTypes		ret = NhlNOERROR, subret = NhlNOERROR;
	NhlTransformLayer		tl = (NhlTransformLayer) plot;
	NhlTransformLayerPart	*tfp = &(tl->trans);
	NhlTransObjLayer		top;
 	NhlTransObjClass 	tocp;

	if (n < 1) {
		e_text = "%s, polymarker is empty";
		NhlPError(NhlWARNING,NhlEUNKNOWN,e_text, entry_name);
		return NhlWARNING;
	}

/* 
 * Set up the transformation based on whether the plot is part of an
 * overlay.
 */
	if (tfp->overlay_status == _tfCurrentOverlayMember && 
	    ! tfp->do_ndc_overlay &&
	    tfp->overlay_trans_obj != NULL) {
		top = (NhlTransObjLayer) tfp->overlay_trans_obj;
	}
	else {
		if ((top = (NhlTransObjLayer) tfp->trans_obj) == NULL) {
			e_text = 
			   "%s: no transformation object recorded for pid %d";
			NhlPError(NhlFATAL,NhlEUNKNOWN,e_text,
				  entry_name,plot->base.id);
			return(ret);
	        }
	}
	tocp = (NhlTransObjClass) (top->base.layer_class);

	subret = _NhlActivateWorkstation(tl->base.wkptr);

	if ((ret = MIN(ret,subret)) < NhlWARNING) {
		e_text = "%s: error activating workstation";
		NhlPError(NhlFATAL,NhlEUNKNOWN,e_text, entry_name);
		return(ret);
	}


/* Not sure if a set trans is required */

	subret = _NhlSetTrans((NhlLayer) top, plot);

	if ((ret = MIN(ret,subret)) < NhlWARNING) {
		e_text = "%s: error setting transformation";
		NhlPError(NhlFATAL,NhlEUNKNOWN,e_text, entry_name);
		return(ret);
	}

	_NhlSetMarkerInfo(tl->base.wkptr, plot);

	gset_clip_ind(GIND_CLIP);
	subret = _NhlWorkstationMarker(tl->base.wkptr,x,y,n);
	gset_clip_ind(GIND_NO_CLIP);

	if ((ret = MIN(ret,subret)) < NhlWARNING) {
		e_text = "%s: error drawing polymarker";
		NhlPError(NhlFATAL,NhlEUNKNOWN,e_text, entry_name);
		return(ret);
	}

        subret = _NhlDeactivateWorkstation(tl->base.wkptr);

	if ((ret = MIN(ret,subret)) < NhlWARNING) {
		e_text = "%s: error deactivating workstation";
		NhlPError(NhlFATAL,NhlEUNKNOWN,e_text, entry_name);
		return(ret);
	}

	return MIN(ret,subret);
}


/*
 * Function:	_NhlIsOverlay
 *
 * Description:	
 *
 * In Args:	
 *
 * Out Args:	
 *
 * Scope:	
 * Returns:	
 * Side Effect:	
 */
NhlBoolean
_NhlIsOverlay
#if	NhlNeedProto
(
	int	pid
)
#else
(pid)
	int	pid;
#endif
{
	NhlLayer		l = _NhlGetLayer(pid);
	NhlTransformLayer	tl = NULL;


	if(! (l && _NhlIsTransform(l)))
		return False;

	tl = (NhlTransformLayer)l;

	if (tl->trans.overlay_status == _tfCurrentOverlayMember)
		return True;

	return False;
}


/*
 * Function:	_NhlIsAnnotation
 *
 * Description:	
 *
 * In Args:	
 *
 * Out Args:	
 *
 * Scope:	
 * Returns:	
 * Side Effect:	
 */
NhlBoolean
_NhlIsAnnotation
#if	NhlNeedProto
(
	int	pid
)
#else
(pid)
	int	pid;
#endif
{
	NhlLayer		l = _NhlGetLayer(pid);
	NhlViewLayer		vl = NULL;

	if (! (l && _NhlIsView(l)))
		return False;
	vl = (NhlViewLayer)l;
	if (vl->view.annomanager_id) {
		return True;
	}

	return False;
}


/*
 * Function:	_NhlIsPlotMember
 *
 * Description:	
 *
 * In Args:	
 *
 * Out Args:	
 *
 * Scope:	
 * Returns:	
 * Side Effect:	
 */
NhlBoolean
_NhlIsPlotMember
#if	NhlNeedProto
(
	int	pid
)
#else
(pid)
	int	pid;
#endif
{
	NhlLayer		l = _NhlGetLayer(pid);
	NhlViewLayer		vl = NULL;
	NhlTransformLayer	tl = NULL;

	if (_NhlIsAnnotation(pid) || _NhlIsOverlay(pid))
		return True;

	return False;
}


/*
 * Function:	_NhlAnnotationBase
 *
 * Description:	
 *
 * In Args:	
 *
 * Out Args:	
 *
 * Scope:	
 * Returns:	
 * Side Effect:	
 */
int
_NhlAnnotationBase
#if	NhlNeedProto
(
	int	pid
)
#else
(pid)
	int	pid;
#endif
{
	NhlLayer		l = _NhlGetLayer(pid);
	NhlViewLayer		vl = NULL;
	NhlTransformLayer	tl = NULL;

	if (! (l && _NhlIsView(l)))
		return NhlNULLOBJID;
	vl = (NhlViewLayer)l;
	if (vl->view.annomanager_id && vl->view.overlay_id) {
		NhlLayer ovl = _NhlGetLayer(vl->view.overlay_id);
		return ovl->base.parent->base.id;
	}

	return NhlNULLOBJID;
}

/*
 * Function:	_NhlOverlayBase
 *
 * Description:	
 *
 * In Args:	
 *
 * Out Args:	
 *
 * Scope:	
 * Returns:	
 * Side Effect:	
 */
int
_NhlOverlayBase
#if	NhlNeedProto
(
	int	pid
)
#else
(pid)
	int	pid;
#endif
{
	NhlLayer		l = _NhlGetLayer(pid);
	NhlTransformLayer	tl = NULL;

	if(! (l && _NhlIsTransform(l)))
		return NhlNULLOBJID;
	tl = (NhlTransformLayer)l;
		
	if (tl->trans.overlay_status == _tfCurrentOverlayMember &&
	    tl->trans.overlay_object != NhlNULLOBJID)
		return tl->trans.overlay_object->base.parent->base.id;
	else if (tl->trans.overlay_status == _tfCurrentOverlayBase)
		return pid;

	return NhlNULLOBJID;
}

/*
 * Function:	_NhlBasePlot
 *
 * Description:	This routine returns the current base plot for a plot
 *              member. (Not necessarily the primary base plot.)
 *
 * In Args:	
 *
 * Out Args:	
 *
 * Scope:	
 * Returns:	
 * Side Effect:	
 */
int
_NhlBasePlot
#if	NhlNeedProto
(
	int	pid
)
#else
(pid)
	int	pid;
#endif
{
	int id;

	if (! _NhlGetLayer(pid))
		return  NhlNULLOBJID;

	id = pid;
	if (_NhlIsAnnotation(id)) {
		id = _NhlAnnotationBase(id);
	}

	return (_NhlOverlayBase(id));
}

/*
 * Function:	_NhlTopLevelView
 *
 * Description:	This routine returns the primary base plot of any plot
 *              member. Otherwise it returns the input pid.
 *
 * In Args:	
 *
 * Out Args:	
 *
 * Scope:	
 * Returns:	
 * Side Effect:	
 */
int
_NhlTopLevelView
#if	NhlNeedProto
(
	int	pid
)
#else
(pid)
	int	pid;
#endif
{
	int id;

	if (! _NhlGetLayer(pid))
		return  NhlNULLOBJID;
	
	id = pid;
	while (_NhlIsPlotMember(id)) {
		if (_NhlIsOverlay(id))
			id = _NhlOverlayBase(id);
		else
			id = _NhlAnnotationBase(id);
	}
	if (id > NhlNULLOBJID)
                return id;
/*
 * should never get to this point because id should always be valid
 */
	return pid;

}


/*
 * Function:	_NhlIsSimpleTransform
 *
 * Description:	
 *
 * In Args:	
 *
 * Out Args:	
 *
 * Scope:	
 * Returns:	
 * Side Effect:	
 */
NhlBoolean
_NhlIsSimpleTransform
#if	NhlNeedProto
(
	int	pid
)
#else
(pid)
	int	pid;
#endif
{
	NhlLayer	l = _NhlGetLayer(pid);

	if(l && _NhlIsTransform(l)) {
		NhlTransformLayer tl = (NhlTransformLayer) l;
		if (! tl->trans.plot_manager_on)
			return True;
	}
	return False;
}

/*
 * Function:	NhlIsTransform
 *
 * Description:	
 *
 * In Args:	
 *
 * Out Args:	
 *
 * Scope:	
 * Returns:	
 * Side Effect:	
 */
NhlBoolean
NhlIsTransform
#if	NhlNeedProto
(
	int	pid
)
#else
(pid)
	int	pid;
#endif
{
	NhlLayer	l = _NhlGetLayer(pid);

	if(l && _NhlIsTransform(l))
		return True;

	return False;
}

/*
 * Function:	nhlpfistransform
 *
 * Description:	
 *
 * In Args:	
 *
 * Out Args:	
 *
 * Scope:	
 * Returns:	
 * Side Effect:	
 */
void _NHLCALLF(nhlpfistransform,NHLPFISTRANSFORM)
#if	NhlNeedProto
(
	int	*id,
	int	*status
)
#else
(id,status)
	int	*id;
	int	*status;
#endif
{
	*status = NhlIsTransform(*id);

	return;
}

extern NhlErrorTypes _NhltfDrawSegment
#if	NhlNeedProto
(
        NhlLayer	plot,
	NhlLayer	trobj,
        NhlTransDat	*transdat,
	NhlString	entry_name
)
#else
(plot,trobj,transdat,entry_name)
        NhlLayer	plot;
	NhlLayer	trobj;
        NhlTransDat	*transdat;    				       
	NhlString	entry_name;

#endif
{
	NhlErrorTypes	ret = NhlNOERROR, subret = NhlNOERROR;
	NhlString	trobj_name;
	float		mr,ml,mt,mb;
	float		x[3],y[3];
	NhlViewLayer	vl = (NhlViewLayer)plot;
	NhlSegTransList	steptrans = vl->view.plot_segments_list;
	NhlString	e_text;
	int		save_ig;

	if (transdat == NULL)
		return NhlNOERROR;

	trobj_name = (trobj->base.layer_class)->base_class.class_name;
	if (trobj_name == NhlmapTransObjClass->base_class.class_name) {
		subret = NhlVAGetValues(trobj->base.id,
					NhlNmpLeftMapPosF,&ml,
					NhlNmpRightMapPosF,&mr,
					NhlNmpBottomMapPosF,&mb,
					NhlNmpTopMapPosF,&mt,NULL);
		if ((ret = MIN(ret,subret)) < NhlWARNING) return ret;
		x[0] = ml;
		y[0] = mb;
		x[1] = ml;
		y[1] = mt;
		x[2] = mr;
		y[2] = mt;
	}
	else {
		x[0] = vl->view.fl;
		y[0] = vl->view.fb;
		x[1] = vl->view.fl;
		y[1] = vl->view.ft;
		x[2] = vl->view.fr;
		y[2] = vl->view.ft;
	}
        
	while(steptrans != NULL) {
		if (steptrans->seg_trans_dat == transdat)
			break;
		steptrans = steptrans->next;
	}
	if (steptrans == NULL) {
		e_text = "%s: internal error locating segment";
		NhlPError(NhlFATAL,NhlEUNKNOWN,e_text,entry_name);
		return NhlFATAL;
	}

	c_nggeti("IG",&save_ig);
	c_ngseti("IG",1);
	_NhlComputeSegTrans(steptrans->seg_trans_dat,steptrans->seg_trans,x,y);
	_NhlSetSegTrans( steptrans->seg_trans_dat,steptrans->seg_trans);

	subret = _NhlActivateWorkstation(plot->base.wkptr);
	if ((ret = MIN(subret,ret)) < NhlWARNING) return ret;

	subret = _NhlDrawSegment(transdat,_NhlWorkstationId(plot->base.wkptr));
	if ((ret = MIN(subret,ret)) < NhlWARNING) return ret;

	subret = _NhlDeactivateWorkstation(plot->base.wkptr);
	c_ngseti("IG",save_ig);

	return MIN(subret,ret);
}

extern NhlErrorTypes _NhltfInitSegment
#if	NhlNeedProto
(
        NhlLayer	plot,
	NhlLayer	trobj,
	NhlTransDat	**transdat,					      
	NhlString	entry_name
)
#else
(plot,trobj,transdat,entry_name)
        NhlLayer	plot;
	NhlLayer	trobj;
        NhlTransDat	**transdat;    				       
	NhlString	entry_name;
#endif
{
	NhlErrorTypes	ret = NhlNOERROR, subret = NhlNOERROR;
	char		*e_text;
	NhlString	trobj_name;
	float		mr,ml,mt,mb;
	float		x[3],y[3];
	NhlViewLayer	vl = (NhlViewLayer)plot;

	if (*transdat != NULL)
		_NhlDeleteViewSegment(plot,*transdat);
	if ((*transdat = _NhlNewViewSegment(plot)) == NULL) {
		e_text = "%s: error opening segment";
		NhlPError(NhlFATAL,NhlEUNKNOWN,e_text,entry_name);
		return NhlFATAL;
	}

	trobj_name = (trobj->base.layer_class)->base_class.class_name;
	if (trobj_name == NhlmapTransObjClass->base_class.class_name) {
		subret = NhlVAGetValues(trobj->base.id,
					NhlNmpLeftMapPosF,&ml,
					NhlNmpRightMapPosF,&mr,
					NhlNmpBottomMapPosF,&mb,
					NhlNmpTopMapPosF,&mt,NULL);
		if ((ret = MIN(ret,subret)) < NhlWARNING) return ret;
		x[0] = ml;
		y[0] = mb;
		x[1] = ml;
		y[1] = mt;
		x[2] = mr;
		y[2] = mt;
	}
	else {
		x[0] = vl->view.fl;
		y[0] = vl->view.fb;
		x[1] = vl->view.fl;
		y[1] = vl->view.ft;
		x[2] = vl->view.fr;
		y[2] = vl->view.ft;
	}
	_NhlResetSegTransDat(*transdat,x,y);
	_NhlStartSegment(*transdat);
	return ret;
}

extern NhlErrorTypes _NhltfCheckCoordBounds
#if	NhlNeedProto
(
        NhlTransformLayer	new,
	NhlTransformLayer	old,
        NhlBoolean		use_irr_trans,
	NhlString		entry_name
)
#else
(new,old,use_irr_trans,entry_name)
        NhlTransformLayer	new;
	NhlTransformLayer	old;
        NhlBoolean		use_irr_trans;
	NhlString		entry_name;
        
#endif
{
	NhlErrorTypes	ret = NhlNOERROR, subret = NhlNOERROR;
        NhlTransformLayerPart	*tfp = &new->trans;
	char		*e_text;
	float		ftmp;
        
        if (old) {
                NhlTransformLayerPart	*otfp = &old->trans;

                if (tfp->data_xstart != otfp->data_xstart ||
                    tfp->data_xend != otfp->data_xend) {
                        tfp->sticky_x_min_set = False;
                        tfp->sticky_x_max_set = False;
                }
                if (tfp->data_ystart != otfp->data_ystart ||
                    tfp->data_yend != otfp->data_yend) {
                        tfp->sticky_y_min_set = False;
                        tfp->sticky_y_max_set = False;
                }
        }
        
        if (! tfp->sticky_x_min_set)
                tfp->x_min = MIN(tfp->data_xstart,tfp->data_xend);
        if (! tfp->sticky_x_max_set)
                tfp->x_max = MAX(tfp->data_xstart,tfp->data_xend);
        if (! tfp->sticky_y_min_set)
                tfp->y_min = MIN(tfp->data_ystart,tfp->data_yend);
        if (! tfp->sticky_y_max_set)
                tfp->y_max = MAX(tfp->data_ystart,tfp->data_yend);

        if (tfp->x_min >= MAX(tfp->data_xstart,tfp->data_xend) ||
            tfp->x_max <= MIN(tfp->data_xstart,tfp->data_xend)) {
		e_text = "%s: X coordinates out of data range: defaulting";
		ret = MIN(ret,NhlWARNING);
		NhlPError(NhlWARNING,NhlEUNKNOWN,e_text,entry_name);
                tfp->x_min = MIN(tfp->data_xstart,tfp->data_xend);
                tfp->x_max = MAX(tfp->data_xstart,tfp->data_xend);
                tfp->sticky_x_min_set = tfp->sticky_x_max_set = False;
        }
	if (tfp->x_min == tfp->x_max) {
		e_text = "%s: Zero X coordinate span: defaulting";
		ret = MIN(ret,NhlWARNING);
		NhlPError(NhlWARNING,NhlEZEROSPAN,e_text,entry_name);
		tfp->x_min = 0.0; 
		tfp->x_max = 1.0;
                tfp->sticky_x_min_set = tfp->sticky_x_max_set = False;
	}
	else if (tfp->x_min > tfp->x_max) {
		e_text = "%s: Min X coordinate exceeds max: exchanging";
		ret = MIN(ret,NhlWARNING);
		NhlPError(NhlWARNING,NhlEUNKNOWN,e_text,entry_name);
		ftmp = tfp->x_min;
		tfp->x_min = tfp->x_max;
		tfp->x_max = ftmp;
                tfp->sticky_x_min_set = tfp->sticky_x_max_set = False;
	}
	if (! use_irr_trans && tfp->x_log && tfp->x_min <= 0.0) {
		e_text = 
		 "%s: Log axis requires all positive extent: setting %s False";
		ret = MIN(ret,NhlWARNING);
		NhlPError(NhlWARNING,NhlEUNKNOWN,e_text,entry_name,NhlNtrXLog);
		tfp->x_log = False;
                tfp->x_axis_type = tfp->x_axis_type == NhlLOGAXIS ?
                        NhlLINEARAXIS : tfp->x_axis_type;
	}
        else if (use_irr_trans && tfp->x_axis_type == NhlLOGAXIS &&
                 (tfp->x_min <= 0.0 ||
                  MIN(tfp->data_xstart,tfp->data_xend) <= 0.0 )) {
		e_text = 
		   "%s: Log axis requires all positive extent: setting %s to LinearAxis (%s to False)";
		ret = MIN(ret,NhlWARNING);
		NhlPError(NhlWARNING,NhlEUNKNOWN,e_text,entry_name,
                          NhlNtrXAxisType,NhlNtrXLog);
		tfp->x_log = False;
                tfp->x_axis_type = NhlLINEARAXIS;
        }
        
        if (tfp->y_min >= MAX(tfp->data_ystart,tfp->data_yend) ||
            tfp->y_max <= MIN(tfp->data_ystart,tfp->data_yend)) {
		e_text = "%s: Y coordinates out of data range: defaulting";
		ret = MIN(ret,NhlWARNING);
		NhlPError(NhlWARNING,NhlEUNKNOWN,e_text,entry_name);
                tfp->y_min = MIN(tfp->data_ystart,tfp->data_yend);
                tfp->y_max = MAX(tfp->data_ystart,tfp->data_yend);
                tfp->sticky_y_min_set = tfp->sticky_y_max_set = False;
        }
	if (tfp->y_min == tfp->y_max) {
		e_text = "%s: Zero Y coordinate span: defaulting";
		ret = MIN(ret,NhlWARNING);
		NhlPError(NhlWARNING,NhlEZEROSPAN,e_text,entry_name);
		tfp->y_min = 0.0; 
		tfp->y_max = 1.0;
                tfp->sticky_y_min_set = tfp->sticky_y_max_set = False;
	}
	else if (tfp->y_min > tfp->y_max) {
		e_text = "%s: Min Y coordinate exceeds max: exchanging";
		ret = MIN(ret,NhlWARNING);
		NhlPError(NhlWARNING,NhlEUNKNOWN,e_text,entry_name);
		ftmp = tfp->y_min;
		tfp->y_min = tfp->y_max;
		tfp->y_max = ftmp;
                tfp->sticky_y_min_set = tfp->sticky_y_max_set = False;
	}
	if (! use_irr_trans && tfp->y_log && tfp->y_min <= 0.0) {
		e_text = 
		 "%s: Log axis requires all positive extent: setting %s False";
		ret = MIN(ret,NhlWARNING);
		NhlPError(NhlWARNING,NhlEUNKNOWN,e_text,entry_name,NhlNtrYLog);
		tfp->y_log = False;
                tfp->y_axis_type = tfp->y_axis_type == NhlLOGAXIS ?
                        NhlLINEARAXIS : tfp->y_axis_type;
	}
        else if (use_irr_trans && tfp->y_axis_type == NhlLOGAXIS &&
                 (tfp->y_min <= 0.0 ||
                  MIN(tfp->data_ystart,tfp->data_yend) <= 0.0 )) {
		e_text = 
		   "%s: Log axis requires all positive extent: setting %s to LinearAxis (%s to False)";
		ret = MIN(ret,NhlWARNING);
		NhlPError(NhlWARNING,NhlEUNKNOWN,e_text,entry_name,
                          NhlNtrYAxisType,NhlNtrYLog);
		tfp->y_log = False;
                tfp->y_axis_type = NhlLINEARAXIS;
        }
                
        if (use_irr_trans) {
                e_text = 
"%s: irregular transformation requires %s to be within data coordinate range: resetting";
                if (tfp->x_min < MIN(tfp->data_xstart,tfp->data_xend)) {
                        NhlPError(NhlWARNING,NhlEUNKNOWN,e_text,
                                  entry_name,NhlNtrXMinF);
                        ret = MIN(ret,NhlWARNING);
                        tfp->x_min = MIN(tfp->data_xstart,tfp->data_xend);
                        tfp->sticky_x_min_set = False;
                }
                if (tfp->x_max > MAX(tfp->data_xstart,tfp->data_xend)) {
                        NhlPError(NhlWARNING,NhlEUNKNOWN,e_text,
                                  entry_name,NhlNtrXMaxF);
                        ret = MIN(ret,NhlWARNING);
                        tfp->x_max = MAX(tfp->data_xstart,tfp->data_xend);
                        tfp->sticky_x_max_set = False;
                }
                if (tfp->y_min < MIN(tfp->data_ystart,tfp->data_yend)) {
                        NhlPError(NhlWARNING,NhlEUNKNOWN,e_text,
                                  entry_name,NhlNtrYMinF);
                        ret = MIN(ret,NhlWARNING);
                        tfp->y_min = MIN(tfp->data_ystart,tfp->data_yend);
                        tfp->sticky_y_min_set = False;
                }
                if (tfp->y_max > MAX(tfp->data_ystart,tfp->data_yend)) {
                        NhlPError(NhlWARNING,NhlEUNKNOWN,e_text,
                                  entry_name,NhlNtrYMaxF);
                        ret = MIN(ret,NhlWARNING);
                        tfp->y_max = MAX(tfp->data_ystart,tfp->data_yend);
                        tfp->sticky_y_max_set = False;
                }
        }
        
        return ret;
}


