/*
 *      $Id: IrregularPlot.c,v 1.6 1994-05-12 23:51:26 boote Exp $
 */
/************************************************************************
*									*
*			     Copyright (C)  1992			*
*	     University Corporation for Atmospheric Research		*
*			     All Rights Reserved			*
*									*
************************************************************************/
/*
 *	File:		IrregularPlot.c
 *
 *	Author:		David Brown
 *			National Center for Atmospheric Research
 *			PO 3000, Boulder, Colorado
 *
 *	Date:		Tue Nov 16 15:18:58 MST 1993
 *
 *	Description:	Creates and manages a generic Irregular plot that
 *			can serve as the base of an overlay plot or be used 
 *			for Irregular plots that do not use hlu plot objects.
 *			In this context irregular means a plot where 
 *			the distance along either or both coordinate axes
 *			is not uniformly scaled.
 */

#include <stdio.h>
#include <ncarg/hlu/hluP.h>
#include <ncarg/hlu/IrregularPlotP.h>

#if 0
static NhlResource resources[] = {
	{ NhlNtfOverlayPlotBase,NhlCtfOverlayPlotBase,
		NhlTBoolean,sizeof(NhlBoolean),
		NhlOffset(NhlIrregularPlotLayerRec,trans.overlay_plot_base),
		NhlTImmediate,(NhlPointer)False},
};
#endif

/* base methods */


static NhlErrorTypes IrregularPlotClassInitialize(
#ifdef NhlNeedProto
	void
#endif
);

static NhlErrorTypes IrregularPlotClassPartInitialize(
#ifdef NhlNeedProto
	NhlLayerClass	lc
#endif
);

static NhlErrorTypes IrregularPlotInitialize(
#ifdef NhlNeedProto
        NhlLayerClass,     /* class */
        NhlLayer,          /* req */
        NhlLayer,          /* new */
        _NhlArgList,    /* args */
        int             /* num_args */
#endif
);

static NhlErrorTypes IrregularPlotSetValues(
#ifdef NhlNeedProto
        NhlLayer,          /* old */
        NhlLayer,          /* reference */
        NhlLayer,          /* new */
        _NhlArgList,    /* args */
        int             /* num_args*/
#endif
);

static NhlErrorTypes IrregularPlotDestroy(
#ifdef NhlNeedProto
        NhlLayer           /* inst */
#endif
);

static NhlErrorTypes IrregularPlotDraw(
#ifdef NhlNeedProto
        NhlLayer   /* layer */
#endif
);

static NhlErrorTypes SetUpTransObj(
#ifdef NhlNeedProto
	NhlIrregularPlotLayer	xnew,
	NhlIrregularPlotLayer	xold,
	NhlBoolean	init
#endif
);

NhlIrregularPlotLayerClassRec NhlirregularPlotLayerClassRec = {
        {
/* class_name			*/      "IrregularPlot",
/* nrm_class			*/      NrmNULLQUARK,
/* layer_size			*/      sizeof(NhlIrregularPlotLayerRec),
/* class_inited			*/      False,
/* superclass			*/      (NhlLayerClass)&NhltransformLayerClassRec,

/* layer_resources		*/	NULL, /* resources */
/* num_resources		*/	0, /*NhlNumber(resources), */
/* all_resources		*/	NULL,

/* class_part_initialize	*/	IrregularPlotClassPartInitialize,
/* class_initialize		*/	IrregularPlotClassInitialize,
/* layer_initialize		*/	IrregularPlotInitialize,
/* layer_set_values		*/	IrregularPlotSetValues,
/* layer_set_values_hook	*/	NULL,
/* layer_get_values		*/	NULL,
/* layer_reparent		*/	NULL,
/* layer_destroy		*/	IrregularPlotDestroy,

/* child_resources		*/	NULL,

/* layer_draw			*/      IrregularPlotDraw,

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
/* overlay_capability 		*/	_tfOverlayBaseOrMember,
/* data_to_ndc			*/	NULL,
/* ndc_to_data			*/	NULL,
/* data_polyline		*/	NULL,
/* ndc_polyline			*/	NULL
	},
	{
/* foo				*/	NULL
	}
};
	
NhlLayerClass NhlirregularPlotLayerClass = (NhlLayerClass)
					&NhlirregularPlotLayerClassRec;

/*
 * Function:	IrregularPlotClassInitialize
 *
 * Description:
 *
 * In Args:	NONE
 *
 * Out Args:	NONE
 *
 * Return Values:	ErrorConditions
 *
 * Side Effects:	NONE
 */
static NhlErrorTypes
IrregularPlotClassInitialize
#if __STDC__
(
	void
)
#else
()
#endif
{

	return NhlNOERROR;
}

/*
 * Function:	IrregularPlotClassPartInitialize
 *
 * Description:	This function initializes fields in the 
 *		NhlIrregularPlotLayerClassPart that cannot be initialized 
 *		statically.
 *		Calls _NhlRegisterChildClass for the overlay manager object.
 *
 * In Args:	
 *		NhlLayerClass	lc	NhlLayer Class to init
 *
 * Out Args:	
 *
 * Scope:	static
 * Returns:	NhlErrorTypes
 * Side Effect:	
 */

static NhlErrorTypes
IrregularPlotClassPartInitialize
#if	__STDC__
(
	NhlLayerClass	lc	/* NhlLayer Class to init	*/
)
#else
(lc)
	NhlLayerClass	lc;	/* NhlLayer Class to init	*/
#endif
{
	NhlErrorTypes	ret = NhlNOERROR, subret = NhlNOERROR;
	char		*e_text;
	char		*entry_name = "IrregularPlotClassPartInitialize";

/*
 * Register children objects
 */
	subret = _NhlRegisterChildClass(lc,NhloverlayLayerClass,
					False,False,NULL);

	if ((ret = MIN(ret,subret)) < NhlWARNING) {
		e_text = "%s: error registering %s";
		NhlPError(NhlFATAL,NhlEUNKNOWN,e_text,entry_name,
			  "NhloverlayLayerClass");
		return(NhlFATAL);
	}

	subret = _NhlRegisterChildClass(lc,NhlirregularType2TransObjLayerClass,
					False,False,NULL);

	if ((ret = MIN(ret,subret)) < NhlWARNING) {
		e_text = "%s: error registering %s";
		NhlPError(NhlFATAL,NhlEUNKNOWN,e_text,entry_name,
			  "NhlmapTransObjLayerClass");
		return(NhlFATAL);
	}

	return ret;
}


/*
 * Function:	IrregularPlotInitialize
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
IrregularPlotInitialize
#if     __STDC__
(
	NhlLayerClass	class,
	NhlLayer	req,
	NhlLayer	new,
	_NhlArgList	args,
	int		num_args
)
#else
(class,req,new,args,num_args)
        NhlLayerClass      class;
        NhlLayer           req;
        NhlLayer           new;
        _NhlArgList     args;
        int             num_args;
#endif
{
	NhlErrorTypes		ret = NhlNOERROR, subret = NhlNOERROR;
	char			*entry_name = "IrregularPlotInitialize";
	NhlIrregularPlotLayer	inew = (NhlIrregularPlotLayer) new;
	NhlIrregularPlotLayerPart	*irp = &(inew->irrplot);



/* Initialize private fields */

	irp->overlay_object = NULL;
	
	
/* Set up the Irregular transformation */

	subret = SetUpTransObj(inew, (NhlIrregularPlotLayer) req, True);
	if ((ret = MIN(ret,subret)) < NhlWARNING) 
		return ret;

/* Manage the overlay */

	subret = _NhlManageOverlay(&irp->overlay_object,new,req,
			       True,NULL,0,entry_name);
	if ((ret = MIN(ret,subret)) < NhlWARNING) 
		return ret;

	return ret;
}

/*
 * Function:	IrregularPlotSetValues
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
static NhlErrorTypes IrregularPlotSetValues
#if  __STDC__
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
	NhlErrorTypes			ret = NhlNOERROR, subret = NhlNOERROR;
	char				*entry_name = "IrregularPlotSetValues";
	NhlIrregularPlotLayer		inew = (NhlIrregularPlotLayer) new;
	NhlIrregularPlotLayerPart	*irp = &(inew->irrplot);



/* Set up the Irregular transformation */

	subret = SetUpTransObj(inew, (NhlIrregularPlotLayer) old, False);
	ret = MIN(ret,subret);

/* Manage the overlay */

	subret = _NhlManageOverlay(&irp->overlay_object,new,old,
			       False,NULL,0,entry_name);
	if ((ret = MIN(ret,subret)) < NhlWARNING) 
		return ret;

	return ret;
}

/*
 * Function:	IrregularPlotDestroy
 *
 * Description:
 *
 * In Args:	inst		instance record pointer
 *
 * Out Args:	NONE
 *
 * Return Values:	ErrorConditions
 *
 * Side Effects:	NONE
 */
static NhlErrorTypes IrregularPlotDestroy
#if __STDC__
(NhlLayer inst)
#else
(inst)
NhlLayer inst;
#endif
{
	NhlErrorTypes		ret = NhlNOERROR, subret = NhlNOERROR;
#if 0
	char			*e_text;
	char			*entry_name = "LogLinPlotDestroy";
#endif
	NhlIrregularPlotLayerPart	*irp =
				&(((NhlIrregularPlotLayer) inst)->irrplot);
	NhlTransformLayerPart	*irtp = &(((NhlTransformLayer) inst)->trans);


	if (irtp->overlay_status == _tfCurrentOverlayMember) {
		subret = NhlRemoveFromOverlay(
				irtp->overlay_object->base.parent->base.id,
					      inst->base.id,False);
		if ((ret = MIN(subret,ret)) < NhlWARNING)
			return NhlFATAL;
	}

	if (irp->overlay_object != NULL) {
		(void) _NhlDestroyChild(irp->overlay_object->base.id,inst);
		irp->overlay_object = NULL;
	}
	if (irtp->trans_obj != NULL) {
		(void) _NhlDestroyChild(irtp->trans_obj->base.id,inst);
		irtp->trans_obj = NULL;
	}
	
	return(ret);
}

/*
 * Function:	IrregularPlotDraw
 *
 * Description:	Currently NULL; may be a candidate for removal.
 *
 * In Args:	layer	IrregularPlot instance
 *
 * Out Args:	NONE
 *
 * Return Values: Error Conditions
 *
 * Side Effects: NONE
 */	

/*ARGSUSED*/
static NhlErrorTypes IrregularPlotDraw
#if  __STDC__
(NhlLayer layer)
#else
(layer)
        NhlLayer layer;
#endif
{
	NhlErrorTypes		ret = NhlNOERROR;

	return ret;
}

/*
 * Function:	SetUpTransObjs
 *
 * Description: Sets up the Irregular transformation object for the generic
 *		Irregular plot object. 
 *
 * In Args:	xnew	new instance record
 *		xold	old instance record if not initializing
 *		init	true if initialization
 *
 * Out Args:	NONE
 *
 * Return Values:	Error Conditions
 *
 * Side Effects:	Objects created and destroyed.
 */
/*ARGSUSED*/
static NhlErrorTypes SetUpTransObj
#if  __STDC__
(
	NhlIrregularPlotLayer	irnew,
	NhlIrregularPlotLayer	irold,
	NhlBoolean		init
)
#else 
(irnew,irold,init)
	NhlIrregularPlotLayer	irnew;
	NhlIrregularPlotLayer	irold;
	NhlBoolean		init;
#endif
{
	NhlErrorTypes			ret = NhlNOERROR, subret = NhlNOERROR;
	char 				*e_text;
	char				*entry_name;
	NhlTransformLayerPart		*tfp = &(irnew->trans);
	char				buffer[_NhlMAXRESNAMLEN];
	int				tmpid;
        NhlSArg				sargs[16];
        int				nargs = 0;


	entry_name = (init) ? 
		"IrregularPlotInitialize" : "IrregularPlotSetValues";
/*
 * The irregular plot actually uses the irregular type 2 transformation
 * object.
 */
	if (init || tfp->trans_obj == NULL) {

		sprintf(buffer,"%s",irnew->base.name);
		strcat(buffer,".Trans");

		subret = _NhlVACreateChild(&tmpid,buffer,
					 NhlirregularType2TransObjLayerClass,
					 (NhlLayer) irnew, NULL);

		ret = MIN(subret,ret);

		tfp->trans_obj = _NhlGetLayer(tmpid);

		if(tfp->trans_obj == NULL){
			e_text = "%s: Error creating transformation object";
			NhlPError(NhlFATAL,NhlEUNKNOWN,e_text,entry_name);
			return NhlFATAL;
		}

		return ret;
	}

	subret = _NhlALSetValuesChild(tfp->trans_obj->base.id,
				      (NhlLayer) irnew,sargs,nargs);
	return MIN(ret,subret);

}
