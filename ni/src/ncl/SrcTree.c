
/*
 *      $Id: SrcTree.c,v 1.5 1993-12-21 19:18:05 ethan Exp $
 */
/************************************************************************
*									*
*			     Copyright (C)  1993			*
*	     University Corporation for Atmospheric Research		*
*			     All Rights Reserved			*
*									*
************************************************************************/
/*
 *	File:		SrcTree.c
 *
 *	Author:		Ethan Alpert
 *			National Center for Atmospheric Research
 *			PO 3000, Boulder, Colorado
 *
 *	Date:		Mon Jul 12 12:11:51 MDT 1993
 *
 *	Description:	Code for constructing individual source tree nodes
 */
#ifdef __cplusplus
extern "C" {
#endif
#include <stdio.h>
#include <ncarg/hlu/hlu.h>
#include <data_objs/NclData.h>
#include <defs.h>
#include <Symbol.h>
#include <SrcTree.h>
#include <errno.h>
#include <y.tab.h>

extern int cur_line_number;
extern char *cur_load_file;

NclGenericNode **node_list = NULL;
int node_list_index = 0;
int cur_node_list_size = NCL_SRC_TREE_NODE_LIST_SIZE;

/*
* These are the string name equivalents of the typedef found in
* SrcTree.h
*/

char *src_tree_names[] = {"Ncl_BLOCK", "Ncl_RETURN", "Ncl_IFTHEN",
                        "Ncl_IFTHENELSE", "Ncl_VISBLKSET", "Ncl_VISBLKGET",
			"Ncl_VISBLKCREATE", "Ncl_DOFROMTO",
                        "Ncl_DOFROMTOSTRIDE", "Ncl_BUILTINPROCCALL",
                        "Ncl_EXTERNALPROCCALL", "Ncl_PROCCALL", "Ncl_FUNCDEF",
                        "Ncl_EXTERNFUNCDEF",    "Ncl_LOCALVARDEC",
                        "Ncl_DIMSIZELISTNODE","Ncl_PROCDEF","Ncl_EXTERNPROCDEF",
                        "Ncl_ASSIGN", "Ncl_IDNREF", "Ncl_INTSUBSCRIPT",
                        "Ncl_COORDSUBSCRIPT", "Ncl_SINGLEINDEX",
                        "Ncl_RANGEINDEX", "Ncl_NEGEXPR", "Ncl_NOTEXPR",
                        "Ncl_MODEXPR", "Ncl_OREXPR", "Ncl_ANDEXPR",
                        "Ncl_XOREXPR", "Ncl_LTSELECTEXPR", "Ncl_GTSELECTEXPR",
                        "Ncl_PLUSEXPR", "Ncl_MINUSEXPR", "Ncl_MULEXPR",
                        "Ncl_MATMULEXPR", "Ncl_DIVEXPR", "Ncl_EXPEXPR",
                        "Ncl_LEEXPR", "Ncl_GEEXPR", "Ncl_GTEXPR", "Ncl_LTEXPR",
                        "Ncl_EQEXPR", "Ncl_NEEXPR", "Ncl_REAL", "Ncl_INT",
                        "Ncl_STRING", "Ncl_BUILTINFUNCCALL",
                        "Ncl_EXTERNFUNCCALL", "Ncl_FUNCCALL", "Ncl_ARRAY",
                        "Ncl_ROWLIST","Ncl_ROWCOLUMNNODE","Ncl_DOWHILE",
			"Ncl_VAR", "Ncl_VARDIMNUM", "Ncl_VARATT",
                        "Ncl_VARDIMNAME", "Ncl_VARCOORD",
                        "Ncl_FILEVAR", "Ncl_IDNEXPR",
			"Ncl_RESOURCE","Ncl_GETRESOURCE", "Ncl_OBJ",
			"Ncl_BREAK", "Ncl_CONTINUE","Ncl_FILEVARATT",
			"Ncl_FILEVARDIMNAME", "Ncl_FILEVARDIMNUM",
			"Ncl_FILECOORD"
			};
/*
* These are the string equivalents of the attribute tags assigned to 
* identifier references to determine the context of the reference
*/

char *ref_node_names[] = { "Ncl_READIT", "Ncl_WRITEIT", "Ncl_PARAMIT" };


/*
 * Function:	_NclRegisterNode
 *
 * Description:	 This function is called from all of the "_Make" functions.
 *		it inserts a pointer to the srctree node into a list. Each 
 * 		node contains a destroy_it field that is a pointer to a
 *		function that will free the tree node. All this is necessary
 *		because the parser can sometimes get into a state where 
 *		tree nodes are lost. This mechansim allows the entire source 
 *		tree to be freed regardless of the error status.
 *
 * In Args:	thenode Must be a source tree node defined in SrcTree.h
 *
 * Out Args:	NONE
 *
 * Returns:	NONE
 * Side Effect:	Possible memory allocation.
 */
void _NclRegisterNode
#if __STDC__
(struct ncl_genericnode *thenode)
#else
(thenode) 
	struct ncl_genericnode *thenode;
#endif
{
	if(node_list == NULL) {
		node_list = (NclGenericNode**)NclMalloc((unsigned)
				sizeof(NclGenericNode*) * cur_node_list_size);
		if(node_list == NULL) {		
			NhlPError(FATAL,E_UNKNOWN,"_NclRegisterNode: Error while trying to allocate memory for source tree, can't continue");
			return;
		}
	}
	if(node_list_index >= cur_node_list_size) {	
		cur_node_list_size *= 2;
		node_list = (NclGenericNode**)NclRealloc(node_list,(unsigned)
				sizeof(NclGenericNode*) * cur_node_list_size);
		if(node_list == NULL) {		
			NhlPError(FATAL,E_UNKNOWN,"_NclRegisterNode: Error while trying to allocate memory for source tree, can't continue");
			return;
		}
	}
	node_list[node_list_index] = thenode;
	node_list_index++;
	return;
	
}

/*
 * Function:	_NclResetNodeList
 *
 * Description:	Just sets the global varaible node_list_index back to 0
 *
 * In Args:	NONE
 *
 * Out Args:	NONE
 *
 * Scope:	
 * Returns:	NONE
 * Side Effect:	If called before freeing the tree memory leaks will occur!
 */
void _NclResetNodeList
#if __STDC__
(void)
#else 
()
#endif
{
	node_list_index = 0;
	return;
	
}



void _NclGenericDestroy
#if  __STDC__
(struct ncl_genericnode *thenode)
#else
(thenode)
	struct ncl_genericnode *thenode;
#endif
{
	NclFree((void*)thenode);
}


void *_NclMakeBreakCont
#if  __STDC__
(NclSymbol *thesym)
#else
(thesym)
	NclSymbol *thesym;
#endif
{
	NclBreak *tmp = (NclBreak*)NclMalloc((unsigned)sizeof(NclBreak));

	if(thesym->type == BREAK) {
		tmp->kind = Ncl_BREAK;
		tmp->name = src_tree_names[Ncl_BREAK];
	} else {
		tmp->kind = Ncl_CONTINUE;
		tmp->name = src_tree_names[Ncl_CONTINUE];
	}
	tmp->line = cur_line_number;
	tmp->file = cur_load_file;
	tmp->destroy_it = (NclSrcTreeDestroyProc)_NclGenericDestroy;
	_NclRegisterNode((NclGenericNode*)tmp);
	return(tmp);
}
/*
 * Function:	_NclMakeReturn
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
void *_NclMakeReturn
#if  __STDC__
(void * return_expr)
#else
(return_expr) 
	void * return_expr;
#endif
{
	NclReturn *tmp = (NclReturn*)NclMalloc((unsigned)sizeof(NclReturn));
	
	if(tmp == NULL) {
		NhlPError(FATAL,errno,"Not enough memory for source tree construction");
		return(NULL);
	}
	tmp->kind = Ncl_RETURN;
	tmp->name = src_tree_names[Ncl_RETURN];
	tmp->line = cur_line_number;
	tmp->file = cur_load_file;
	tmp->destroy_it = (NclSrcTreeDestroyProc)_NclGenericDestroy;
	tmp->expr = return_expr;
	_NclRegisterNode((NclGenericNode*)tmp);
	return((void*)tmp);
}


/*
 * Function:	_NclMakeIfThen
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
void *_NclMakeIfThen
#if  __STDC__
(void * conditional_expr, NclSrcListNode * block_stmnt_list)
#else
(conditional_expr, block_stmnt_list)
void * conditional_expr;
NclSrcListNode * block_stmnt_list;
#endif
{
	NclIfThen* tmp = (NclIfThen*)NclMalloc((unsigned)sizeof(NclIfThen));
	
	if(tmp == NULL) {
		NhlPError(FATAL,errno,"Not enough memory for source tree construction");
		return(NULL);
	}
	tmp->kind = Ncl_IFTHEN;
	tmp->name = src_tree_names[Ncl_IFTHEN];
	tmp->line = cur_line_number;
	tmp->file = cur_load_file;
	tmp->destroy_it = (NclSrcTreeDestroyProc)_NclGenericDestroy;
	tmp->cond_expr = conditional_expr;
	tmp->block_stmnt_list = block_stmnt_list;
	_NclRegisterNode((NclGenericNode*)tmp);
	return((void*)tmp);
}


/*
 * Function:	_NclMakeIfThenElse
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
void *_NclMakeIfThenElse
#if  __STDC__
(void * conditional_expr, NclSrcListNode * block_stmnt_list1, NclSrcListNode * block_stmnt_list2)
#else
(conditional_expr, block_stmnt_list1, block_stmnt_list2)
void * conditional_expr;
NclSrcListNode * block_stmnt_list1;
NclSrcListNode * block_stmnt_list2;
#endif
{
	NclIfThenElse *tmp = (NclIfThenElse*)NclMalloc(
					(unsigned)sizeof(NclIfThenElse));
	
	if(tmp == NULL) {
		NhlPError(FATAL,errno,"Not enough memory for source tree construction");
		return(NULL);
	}
	tmp->kind = Ncl_IFTHENELSE;
	tmp->name = src_tree_names[Ncl_IFTHENELSE];
	tmp->line = cur_line_number;
	tmp->file = cur_load_file;
	tmp->destroy_it = (NclSrcTreeDestroyProc)_NclGenericDestroy;
	tmp->cond_expr = conditional_expr;
	tmp->block_stmnt_list1 = block_stmnt_list1;
	tmp->block_stmnt_list2 = block_stmnt_list2;
	_NclRegisterNode((NclGenericNode*)tmp);
	return((void*)tmp);
}


void _NclGResDestroy
#if __STDC__
(struct ncl_genericnode *thenode)
#else
(thenode)
	struct ncl_genericnode *thenode;
#endif
{
	NclGetResource *tmp = (NclGetResource*)thenode;

	NclFree(tmp->res_name);
	NclFree(tmp);
}

/*
 * Function:	_NclMakeGetResource
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
void *_NclMakeGetResource
#if __STDC__
(char *resname, NclSymbol *var)
#else
(resname , var)
	char *resname;
	NclSymbol *var;
#endif
{
	NclGetResource *tmp = (NclGetResource*)NclMalloc((unsigned)sizeof(NclGetResource));

	tmp->kind = Ncl_GETRESOURCE;
	tmp->name = src_tree_names[Ncl_GETRESOURCE];
	tmp->line = cur_line_number;
	tmp->file = cur_load_file;
	tmp->destroy_it = (NclSrcTreeDestroyProc)_NclGResDestroy;
	tmp->res_name = (char*)NclMalloc((unsigned)strlen(resname)+1);
	if(tmp->res_name != NULL) {
		strcpy(tmp->res_name,resname);
	}
	tmp->var = var;
	_NclRegisterNode((NclGenericNode*)tmp);
	return((void*)tmp);
	
}

void _NclResDestroy
#if __STDC__
(struct ncl_genericnode *thenode)
#else
(thenode)
	struct ncl_genericnode *thenode;
#endif
{
	NclResource *tmp = (NclResource*)thenode;

	NclFree(tmp->res_name);
	NclFree(tmp);
}
/*
 * Function:	_NclMakeResource
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
void *_NclMakeResource
#if __STDC__
(char *resname, void *expr)
#else
(resname , expr)
	char *resname;
	void *expr;
#endif
{
	NclResource *tmp = (NclResource*)NclMalloc((unsigned)sizeof(NclResource));

	tmp->kind = Ncl_RESOURCE;
	tmp->name = src_tree_names[Ncl_RESOURCE];
	tmp->line = cur_line_number;
	tmp->file = cur_load_file;
	tmp->res_name = (char*)NclMalloc((unsigned)strlen(resname)+1);
	if(tmp->res_name != NULL) {
		strcpy(tmp->res_name,resname);
	}
	tmp->destroy_it = (NclSrcTreeDestroyProc)_NclResDestroy;
	tmp->expr = expr;
	_NclRegisterNode((NclGenericNode*)tmp);
	return((void*)tmp);
}


/*
 * Function:	_NclMakeObjRef
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
/*
extern void *_NclMakeObjRef
#ifdef __STDC__
(NclSymbol* obj)
#else
(obj)
	NclSymbol *obj;
#endif
{
	NclObj *tmp = (NclObj*)NclMalloc((unsigned)sizeof(NclObj));
	
	tmp->kind = Ncl_OBJ;
	tmp->name = src_tree_names[Ncl_OBJ];
	tmp->line = cur_line_number;
	tmp->file = cur_load_file;
	tmp->destroy_it = (NclSrcTreeDestroyProc)_NclGenericDestroy;

	tmp->obj = obj;
	_NclRegisterNode((NclGenericNode*)tmp);
	return((void*)tmp); 
}
*/
/*
 * Function:	_NclMakeVis
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
void *_NclMakeVis
#if  __STDC__
(void *objname,void *objtype,NclSrcListNode* resource_list,NclSrcTreeTypes nodetype)
#else
(objname,objtype,resource_list,nodetype)
void * objname;
void* objtype;
NclSrcListNode * resource_list;
NclSrcTreeTypes nodetype;
#endif
{
	NclVisblk *tmp = (NclVisblk*)NclMalloc((unsigned)sizeof(NclVisblk));

	tmp->kind = nodetype;
	tmp->name = src_tree_names[nodetype];
	tmp->line = cur_line_number;
	tmp->file = cur_load_file;
	tmp->destroy_it = (NclSrcTreeDestroyProc)_NclGenericDestroy;

	tmp->objname = objname;
	tmp->resource_list = resource_list;
	tmp->objtype = objtype;
	
	_NclRegisterNode((NclGenericNode*)tmp);
	return((void*)tmp);
}

/*
 * Function:	_NclMakeNewListNode
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
NclSrcListNode *_NclMakeNewListNode
#if  __STDC__
(void)
#else
()
#endif
{
	NclSrcListNode *tmp = (NclSrcListNode*)NclMalloc(
					(unsigned)sizeof(NclSrcListNode));
	if(tmp == NULL) {
		NhlPError(FATAL,errno,"Not enough memory for source tree construction");
		return(NULL);
	}
	tmp->node = NULL;
	tmp->next = NULL;
	return(tmp);

}


/*
 * Function:	_NclMakeDoFromTo
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
void *_NclMakeDoFromTo
#if  __STDC__
(void* var, void *start_expr, void *end_expr, NclSrcListNode *block_stmnt_list)
#else
(var,start_expr,end_expr,block_stmnt_list)
void *var;
void *	start_expr;
void *  end_expr;
NclSrcListNode * block_stmnt_list;
#endif
{
	NclDoFromTo *tmp = (NclDoFromTo*)NclMalloc((unsigned)sizeof(NclDoFromTo));
	if(tmp == NULL) {
		NhlPError(FATAL,errno,"Not enough memory for source tree construction");
		return(NULL);
	}
	tmp->kind = Ncl_DOFROMTO;
	tmp->name = src_tree_names[Ncl_DOFROMTO];
	tmp->line = cur_line_number;
	tmp->file = cur_load_file;
	tmp->destroy_it = (NclSrcTreeDestroyProc)_NclGenericDestroy;
	tmp->inc_var = var;
	tmp->start_expr = start_expr;
	tmp->end_expr = end_expr;
	tmp->block_stmnt_list = block_stmnt_list;
	_NclRegisterNode((NclGenericNode*)tmp);
	return((void*)tmp);
}


/*
 * Function:	_NclMakeDoFromToStride
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
void *_NclMakeDoFromToStride
#if  __STDC__
(void *var, void *start_expr, void *end_expr, void *stride_expr, NclSrcListNode *block_stmnt_list)
#else
(var,start_expr,end_expr,stride_expr,block_stmnt_list)
void *var;
void *start_expr;
void *end_expr;
void *stride_expr;
NclSrcListNode *block_stmnt_list;
#endif
{
	NclDoFromToStride *tmp = (NclDoFromToStride*)NclMalloc(
					(unsigned)sizeof(NclDoFromToStride));
	if(tmp == NULL) {
		NhlPError(FATAL,errno,"Not enough memory for source tree construction");
		return(NULL);
	}
	tmp->kind = Ncl_DOFROMTOSTRIDE;
	tmp->name = src_tree_names[Ncl_DOFROMTOSTRIDE];
	tmp->line = cur_line_number;
	tmp->file = cur_load_file;
	tmp->destroy_it = (NclSrcTreeDestroyProc)_NclGenericDestroy;
	tmp->inc_var = var;
	tmp->start_expr = start_expr;
	tmp->end_expr = end_expr;
	tmp->stride_expr = stride_expr;
	tmp->block_stmnt_list = block_stmnt_list;
	_NclRegisterNode((NclGenericNode*)tmp);
	return((void*)tmp);
}


/*
 * Function:	_NclMakeBuiltinProcCall
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
void *_NclMakeProcCall
#if  __STDC__
(NclSymbol * proc, NclSrcListNode * arg_list,NclSrcTreeTypes type)
#else
(proc, arg_list,type)
NclSymbol * proc;
NclSrcListNode * arg_list;
NclSrcTreeTypes type;
#endif
{
	NclProcCall *tmp = (NclProcCall*)NclMalloc(
					(unsigned)sizeof(NclProcCall));
	if(tmp == NULL) {
		NhlPError(FATAL,errno,"Not enough memory for source tree construction");
		return(NULL);
	}
	tmp->kind = type;
	tmp->name = src_tree_names[type];
	tmp->line = cur_line_number;
	tmp->file = cur_load_file;
	tmp->destroy_it = (NclSrcTreeDestroyProc)_NclGenericDestroy;
	tmp->proc = proc;
	tmp->arg_list = arg_list;
	_NclRegisterNode((NclGenericNode*)tmp);
	return((void*)tmp);
}

/*
 * Function:	_NclMakeNFunctionDef
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
void *_NclMakeNFunctionDef
#if  __STDC__
(NclSymbol *func, NclSrcListNode * dec_list,  void *block, NclSymTableListNode* thescope)
#else 
(func,dec_list,block,thescope)
NclSymbol *func;
NclSrcListNode * dec_list;
void* block;
NclSymTableListNode* thescope; 
#endif
{
	NclFuncDef *tmp = (NclFuncDef*)NclMalloc(
				(unsigned)sizeof(NclFuncDef));
	NclProcFuncInfo  *tmp1 = (NclProcFuncInfo*)NclMalloc((unsigned)
				sizeof(NclProcFuncInfo));
	NclSrcListNode *step, *step1;
	NclLocalVarDec *var_dec;
	NclDimSizeListNode *dim_size;
	int i=0,j=0;


	if(tmp == NULL) {
		NhlPError(FATAL,errno,"Not enough memory for source tree construction");
		return(NULL);
	}
	tmp->kind = Ncl_FUNCDEF;
	tmp->name = src_tree_names[Ncl_FUNCDEF];
	tmp->line = cur_line_number;
	tmp->file = cur_load_file;
	tmp->func = func;
	tmp->dec_list = dec_list;
	tmp->block = block;
	tmp->scope = thescope;
	tmp->destroy_it = (NclSrcTreeDestroyProc)_NclGenericDestroy;
/*
* Create argument numbers and type template and attach it to functions symbol
*/
	if(tmp1 == NULL) {
		NhlPError(FATAL,errno,"Not enough memory for source tree construction");
		return(NULL);
	}
	if(tmp->dec_list != NULL) {
		step = tmp->dec_list;
		i = 0;
		while(step != NULL) {
			i++;
			step = step->next;
		}
		step = tmp->dec_list;
		tmp1->nargs = i;
		tmp1->theargs = (NclArgTemplate*)
				NclMalloc((unsigned)sizeof(NclArgTemplate)*i);
		step = tmp->dec_list;
		i = 0;
		while( step != NULL ) {
			var_dec = (NclLocalVarDec*)step->node;
			if(var_dec->dim_size_list != NULL) {
				tmp1->theargs[i].is_dimsizes = 1;
				step1 = var_dec->dim_size_list;
				j = 0;
				while(step1 != NULL) {
					dim_size = (NclDimSizeListNode*)step1->node;
					tmp1->theargs[i].dim_sizes[j] = dim_size->size;
					step1 = step1->next;
					j++;
				}
			} else {
				tmp1->theargs[i].is_dimsizes = 0;
			}
			tmp1->theargs[i].arg_data_type = var_dec->data_type;
			
			step = step->next;		
			i++;
		}
		tmp1->thesym = func;
		tmp1->mach_rec_ptr= NULL;
	} else {
		tmp1->nargs = 0;
		tmp1->theargs = NULL;
		tmp1->thesym = func;
		tmp1->mach_rec_ptr = NULL;
	}
	func->u.procfunc = tmp1;
	_NclRegisterNode((NclGenericNode*)tmp);
	return((void*)tmp);
}

void _NclEFunctionDefDestroy
#if __STDC__
(struct ncl_genericnode *thenode)
#else 
(thenode)
	struct ncl_genericnode *thenode;
#endif
{
	NclExternFuncDef *tmp = (NclExternFuncDef*)thenode;

	NclFree((void*)tmp->path_info_string);	
	NclFree((void*)tmp);
	
}

/*
 * Function:	_NclMakeEFunctionDef
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
void* _NclMakeEFunctionDef
#if  __STDC__
(NclSymbol *func, NclSrcListNode *dec_list, char *path_info_string,NclSymTableListNode* thescope)
#else
(func, dec_list, path_info_string,thescope)
NclSymbol *func;
NclSrcListNode *dec_list;
char *path_info_string;
NclSymTableListNode *thescope;
#endif
{
	NclExternFuncDef *tmp = (NclExternFuncDef*)NclMalloc(
					(unsigned)sizeof(NclExternFuncDef));
	if(tmp == NULL) {
		NhlPError(FATAL,errno,"Not enough memory for source tree construction");
		return(NULL);
	}
	tmp->kind = Ncl_EXTERNFUNCDEF;
	tmp->name = src_tree_names[Ncl_EXTERNFUNCDEF];
	tmp->line = cur_line_number;
	tmp->file = cur_load_file;
	tmp->destroy_it = (NclSrcTreeDestroyProc)_NclEFunctionDefDestroy;
	tmp->func = func;
	tmp->dec_list = dec_list;
	tmp->path_info_string = (char*)NclMalloc(
					(unsigned)strlen(path_info_string)+1);
	if(tmp->path_info_string == NULL) {
		NhlPError(FATAL,errno,"Not enough memory for source tree construction");
		return(NULL);
	}
	strcpy(tmp->path_info_string,path_info_string);
	tmp->scope = thescope;
	_NclRegisterNode((NclGenericNode*)tmp);
	return((void*)tmp);
}


/*
 * Function:	_NclMakeLocalVarDec
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
void* _NclMakeLocalVarDec
#if  __STDC__
(NclSymbol* var, NclSrcListNode *dim_size_list, NclSymbol *param_type)
#else 
(var,dim_size_list,param_type)
NclSymbol* var;
NclSrcListNode *dim_size_list;
NclSymbol* param_type;
#endif
{
	NclLocalVarDec *tmp = (NclLocalVarDec*)NclMalloc((unsigned)
					sizeof(NclLocalVarDec));
	
	if(tmp == NULL) {
		NhlPError(FATAL,errno,"Not enough memory for source tree construction");
		return(NULL);
	}
	tmp->kind = Ncl_LOCALVARDEC;
	tmp->name = src_tree_names[Ncl_LOCALVARDEC];
	tmp->line = cur_line_number;
	tmp->file = cur_load_file;
	tmp->destroy_it = (NclSrcTreeDestroyProc)_NclGenericDestroy;
	tmp->var = var;
	tmp->dim_size_list = dim_size_list;
	tmp->data_type = param_type;
	_NclRegisterNode((NclGenericNode*)tmp);
	return((void*)tmp);
}


/*
 * Function:	_NclMakeDimSizeNode
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
void * _NclMakeDimSizeNode
#if  __STDC__
(int size)
#else
(size)
	int size;
#endif
{
	NclDimSizeListNode *tmp = (NclDimSizeListNode*)NclMalloc((unsigned)
					sizeof(NclDimSizeListNode));
	if(tmp == NULL) {
		NhlPError(FATAL,errno,"Not enough memory for source tree construction");
		return(NULL);
	}
	tmp->kind = Ncl_DIMSIZELISTNODE;
	tmp->name = src_tree_names[Ncl_DIMSIZELISTNODE];
	tmp->line = cur_line_number;
	tmp->file = cur_load_file;
	tmp->destroy_it = (NclSrcTreeDestroyProc)_NclGenericDestroy;
	if(size == -1) {
		tmp->any = 1;
	} else {
		tmp->any = 0;
	}
	tmp->size = size;
	_NclRegisterNode((NclGenericNode*)tmp);
	return((void*)tmp);
}


/*
 * Function:	_NclMakeProcDef
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
void * _NclMakeProcDef
#if  __STDC__
(NclSymbol *var, NclSrcListNode *arg_list, void* block,NclSymTableListNode *thescope)
#else 
(var, arg_list, block,thescope)
NclSymbol *var;
NclSrcListNode *arg_list;
void* block;
NclSymTableListNode *thescope;
#endif
{
	NclProcDef *tmp = (NclProcDef*)NclMalloc((unsigned)sizeof(NclProcDef));
	NclProcFuncInfo  *tmp1 = (NclProcFuncInfo*)NclMalloc((unsigned)
                                sizeof(NclProcFuncInfo));
        NclSrcListNode *step, *step1;
        NclLocalVarDec *var_dec;
        NclDimSizeListNode *dim_size;
        int i=0,j=0;

	if(tmp == NULL) {
		NhlPError(FATAL,errno,"Not enough memory for source tree construction");
		return(NULL);
	}
	tmp->kind = Ncl_PROCDEF;
	tmp->name = src_tree_names[Ncl_PROCDEF];
	tmp->line = cur_line_number;
	tmp->file = cur_load_file;
	tmp->destroy_it = (NclSrcTreeDestroyProc)_NclGenericDestroy;
	tmp->proc = var;	
	tmp->dec_list = arg_list;
	tmp->block = block;
	tmp->scope = thescope;
        if(tmp1 == NULL) {
                NhlPError(FATAL,errno,"Not enough memory for source tree construction");
                return(NULL);
        }
        if(tmp->dec_list != NULL) {
                step = tmp->dec_list;
                i = 0;
                while(step != NULL) {
                        i++;
                        step = step->next;
                }
                step = tmp->dec_list;
                tmp1->nargs = i;
                tmp1->theargs = (NclArgTemplate*)
                                NclMalloc((unsigned)sizeof(NclArgTemplate)*i);
                step = tmp->dec_list;
                i = 0;
                while( step != NULL ) {
                        var_dec = (NclLocalVarDec*)step->node;
                        if(var_dec->dim_size_list != NULL) {
                                tmp1->theargs[i].is_dimsizes = 1;
                                step1 = var_dec->dim_size_list;
                                j = 0;
                                while(step1 != NULL) {
                                        dim_size = (NclDimSizeListNode*)step1->node;
                                        tmp1->theargs[i].dim_sizes[j] = dim_size->size;
                                        step1 = step1->next;
                                        j++;
                                }
                        } else {
                                tmp1->theargs[i].is_dimsizes = 0;
                        }
                        tmp1->theargs[i].arg_data_type = var_dec->data_type;

                        step = step->next;
                        i++;
                }
                tmp1->thesym = var;
                tmp1->mach_rec_ptr = NULL;
        } else {
                tmp1->nargs = 0;
                tmp1->theargs = NULL;
                tmp1->thesym = var;
                tmp1->mach_rec_ptr= NULL;
        }
        var->u.procfunc = tmp1;
	_NclRegisterNode((NclGenericNode*)tmp);
	return((void*)tmp);
}

void _NclEProcDestroy
#if  __STDC__
(struct ncl_genericnode *thenode)
#else
(thenode)
	struct ncl_genericnode *thenode;
#endif
{
	NclExternProcDef *tmp = (NclExternProcDef*)thenode;

	NclFree((void*)tmp->path_info_string);
	NclFree((void*)tmp);
}

/*
 * Function:	_NclMakeExternalProcDef
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
void* _NclMakeExternalProcDef
#if  __STDC__
(NclSymbol *var,NclSrcListNode *dec_list, char* path_info_string,NclSymTableListNode *thescope)
#else
(var,dec_list,path_info_string,thescope)
NclSymbol *var;
NclSrcListNode *dec_list;
char* path_info_string;
NclSymTableListNode *thescope;
#endif
{
	NclExternProcDef *tmp = (NclExternProcDef*)NclMalloc(
					(unsigned)sizeof(NclExternProcDef));
	if(tmp == NULL) {
		NhlPError(FATAL,errno,"Not enough memory for source tree construction");
		return(NULL);
	}
	
	tmp->kind = Ncl_EXTERNPROCDEF;
	tmp->name = src_tree_names[Ncl_EXTERNPROCDEF];
	tmp->line = cur_line_number;
	tmp->file = cur_load_file;
	tmp->destroy_it = (NclSrcTreeDestroyProc)_NclEProcDestroy;
	tmp->proc = var;
	tmp->dec_list = dec_list;
	tmp->scope = thescope;
	tmp->path_info_string = (char*)NclMalloc(
					(unsigned)strlen(path_info_string)+1);
	if(tmp->path_info_string == NULL) {
		NhlPError(FATAL,errno,"Not enough memory for source tree construction");
		return(NULL);
	}
	_NclRegisterNode((NclGenericNode*)tmp);
	return((void*)tmp);
}


/*
 * Function:	_NclMakeAssignment
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
void* _NclMakeAssignment
#if  __STDC__
(void *name_ref, void *expr)
#else
(name_ref,expr)
void *name_ref;
void *expr;
#endif
{
	NclAssign *tmp = (NclAssign*)NclMalloc((unsigned)sizeof(NclAssign));
	
	if(tmp == NULL) {
		NhlPError(FATAL,errno,"Not enough memory for source tree construction");
		return(NULL);
	}
	tmp->kind = Ncl_ASSIGN;
	tmp->name = src_tree_names[Ncl_ASSIGN];
	tmp->line = cur_line_number;
	tmp->file = cur_load_file;
	tmp->left_side = name_ref;
	tmp->destroy_it = (NclSrcTreeDestroyProc)_NclGenericDestroy;
	tmp->right_side = expr;
	
	_NclRegisterNode((NclGenericNode*)tmp);
	return((void*)tmp);
}


/*
 * Function:	_NclMakeIdnRef
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
void* _NclMakeIdnRef
#if  __STDC__
(void *name, NclSrcListNode *subscript_list)
#else
(name, subscript_list)
void *name;
NclSrcListNode *subscript_list;
#endif
{
	NclIdnRef *tmp = (NclIdnRef*)NclMalloc((unsigned)sizeof(NclIdnRef));
	if(tmp == NULL) {
		NhlPError(FATAL,errno,"Not enough memory for source tree construction");
		return(NULL);
	}
	tmp->kind = Ncl_IDNREF;
	tmp->name = src_tree_names[Ncl_IDNREF];
	tmp->line = cur_line_number;
	tmp->file = cur_load_file;
	tmp->destroy_it = (NclSrcTreeDestroyProc)_NclGenericDestroy;
	tmp->thename = name;
	tmp->subscript_list = subscript_list;
	_NclRegisterNode((NclGenericNode*)tmp);
	return((void*)tmp);
}


/*
 * Function:	_NclMakeIntSubscript
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
void* _NclMakeIntSubscript
#if  __STDC__
(void * subexpr, char * dimname )
#else  
(subexpr,dimname)
void * subexpr; 
char * dimname;
#endif
{
	NclSubscript *tmp = (NclSubscript*)NclMalloc(
					(unsigned)sizeof(NclSubscript));
	if(tmp == NULL) {
		NhlPError(FATAL,errno,"Not enough memory for source tree construction");
		return(NULL);
	}
	tmp->kind = Ncl_INTSUBSCRIPT;
	tmp->name = src_tree_names[Ncl_INTSUBSCRIPT];
	tmp->line = cur_line_number;
	tmp->file = cur_load_file;
	tmp->destroy_it = (NclSrcTreeDestroyProc)_NclGenericDestroy;
	tmp->subexpr = subexpr;
	if(dimname != NULL) {
		tmp->dimname = (char*)NclMalloc((unsigned)strlen(dimname) +1);
		strcpy(tmp->dimname, dimname);
	} else {
		tmp->dimname = NULL;
	}	
	_NclRegisterNode((NclGenericNode*)tmp);
	return((void*)tmp);
}

void _NclCoordSubscriptDestroy
#if __STDC__
(struct ncl_genericnode *thenode)
#else
(thenode)
	struct ncl_genericnode *thenode;
#endif
{
	NclSubscript *tmp = (NclSubscript*)thenode;

	if(tmp->dimname != NULL) {
		NclFree((void*)tmp->dimname);
	}
	NclFree((void*)tmp);
}
/*
 * Function:	_NclMakeCoordSubscript
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
void* _NclMakeCoordSubscript
#if __STDC__
(void *subexpr, char *dimname)
#else
(subexpr, dimname)
void *subexpr;
char *dimname;
#endif
{
	NclSubscript *tmp = (NclSubscript*)NclMalloc(
					(unsigned)sizeof(NclSubscript));
	if(tmp == NULL) {
		NhlPError(FATAL,errno,"Not enough memory for source tree construction");
		return(NULL);
	}
	tmp->kind = Ncl_COORDSUBSCRIPT;
	tmp->name = src_tree_names[Ncl_COORDSUBSCRIPT];
	tmp->line = cur_line_number;
	tmp->file = cur_load_file;
	tmp->destroy_it = (NclSrcTreeDestroyProc)_NclCoordSubscriptDestroy;
	tmp->subexpr =subexpr;
	if(dimname != NULL) {
                tmp->dimname = (char*)NclMalloc((unsigned)strlen(dimname) +1);
                strcpy(tmp->dimname, dimname);
        } else {
                tmp->dimname = NULL;
        }       
	_NclRegisterNode((NclGenericNode*)tmp);
        return((void*)tmp);
}


/*
 * Function:	_NclMakeSingleIndex
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
void* _NclMakeSingleIndex
#if  __STDC__
(void *expr)
#else
(expr)
	void *expr;
#endif
{
	NclSingleIndex *tmp = (NclSingleIndex*)NclMalloc((unsigned)
					sizeof(NclSingleIndex));
	
	if(tmp == NULL) {
		NhlPError(FATAL,errno,"Not enough memory for source tree construction");
		return(NULL);
	}
	tmp->kind = Ncl_SINGLEINDEX;
	tmp->name = src_tree_names[Ncl_SINGLEINDEX];
	tmp->line = cur_line_number;
	tmp->file = cur_load_file;
	tmp->destroy_it = (NclSrcTreeDestroyProc)_NclGenericDestroy;
	tmp->expr = expr;
	_NclRegisterNode((NclGenericNode*)tmp);
	return((void*)tmp);
}


/*
 * Function:	_NclMakeRangeIndex
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
void* _NclMakeRangeIndex
#if  __STDC__
(void * start_expr, void * end_expr, void* stride)
#else
(start_expr,end_expr,stride)
void * start_expr;
void * end_expr;
void * stride;
#endif
{
	NclRangeIndex *tmp = (NclRangeIndex*)NclMalloc((unsigned)
					sizeof(NclRangeIndex));
	
	if(tmp == NULL) {
		NhlPError(FATAL,errno,"Not enough memory for source tree construction");
		return(NULL);
	}
	tmp->kind = Ncl_RANGEINDEX;
	tmp->name = src_tree_names[Ncl_RANGEINDEX];
	tmp->line = cur_line_number;
	tmp->file = cur_load_file;
	tmp->destroy_it = (NclSrcTreeDestroyProc)_NclGenericDestroy;
	tmp->start_expr = start_expr;
	tmp->end_expr = end_expr;
	tmp->stride = stride;
	_NclRegisterNode((NclGenericNode*)tmp);
	return((void*)tmp);
}


/*
 * Function:	_NclMakeUnaryExpr
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
void * _NclMakeUnaryExpr
#if  __STDC__
(void * expr, NclSrcTreeTypes type)
#else
(expr,type)
void * expr;
NclSrcTreeTypes type;
#endif
{
	NclMonoExpr *tmp = (NclMonoExpr*)NclMalloc((unsigned)
					sizeof(NclMonoExpr));
	if(tmp == NULL) {
		NhlPError(FATAL,errno,"Not enough memory for source tree construction");
		return(NULL);
	}
	tmp->kind = type;
	tmp->name = src_tree_names[type];
	tmp->line = cur_line_number;
	tmp->file = cur_load_file;
	tmp->destroy_it = (NclSrcTreeDestroyProc)_NclGenericDestroy;
	tmp->expr = expr;
	_NclRegisterNode((NclGenericNode*)tmp);
	return((void*)tmp);
}

/*
 * Function:	_NclMakeIdnExpr
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
void *_NclMakeIdnExpr
#if __STDC__
(void *idn_ref_node)
#else 
(idn_ref_node)
	void *idn_ref_node;
#endif
{

	NclIdnExpr *tmp = (NclIdnExpr*)NclMalloc((unsigned)sizeof(NclIdnExpr));
	
	tmp->kind = Ncl_IDNEXPR;
	tmp->name = src_tree_names[Ncl_IDNEXPR];
	tmp->line = cur_line_number;
	tmp->file = cur_load_file;
	tmp->destroy_it = (NclSrcTreeDestroyProc)_NclGenericDestroy;
	tmp->idn_ref_node = idn_ref_node;
	_NclRegisterNode((NclGenericNode*)tmp);
	return((void*)tmp);
}

/*
 * Function:	_NclMakeExpr
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
void * _NclMakeExpr
#if  __STDC__
(void * left_expr, void * right_expr, NclSrcTreeTypes type)
#else
(left_expr,right_expr,type)
void * left_expr;
void * right_expr;
NclSrcTreeTypes type;
#endif
{
	NclDualExpr *tmp = (NclDualExpr*)NclMalloc((unsigned)
					sizeof(NclDualExpr));
	if(tmp == NULL) {
		NhlPError(FATAL,errno,"Not enough memory for source tree construction");
		return(NULL);
	}
	tmp->kind = type;
	tmp->name = src_tree_names[type];
	tmp->line = cur_line_number;
	tmp->file = cur_load_file;
	tmp->destroy_it = (NclSrcTreeDestroyProc)_NclGenericDestroy;
	tmp->left_expr = left_expr;
	tmp->right_expr = right_expr;
	_NclRegisterNode((NclGenericNode*)tmp);
	return((void*)tmp);
}


/*
 * Function:	_NclMakeRealExpr
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
void * _NclMakeRealExpr
#if __STDC__
(float real)
#else
(real)
float real;
#endif
{	
	NclReal *tmp = (NclReal*)NclMalloc((unsigned)sizeof(NclReal));
	
	if(tmp == NULL) {
		NhlPError(FATAL,errno,"Not enough memory for source tree construction");
		return(NULL);
	}
	tmp->kind = Ncl_REAL;	
	tmp->name = src_tree_names[Ncl_REAL];
	tmp->line = cur_line_number;
	tmp->file = cur_load_file;
	tmp->destroy_it = (NclSrcTreeDestroyProc)_NclGenericDestroy;
	tmp->real = real;
	tmp->ref_type = Ncl_READIT;
	_NclRegisterNode((NclGenericNode*)tmp);
	return((void*)tmp);
}


/*
 * Function:	_NclMakeIntExpr
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
void * _NclMakeIntExpr
#if  __STDC__
(int integer)
#else
(integer)
int integer;
#endif
{
	NclInt *tmp = (NclInt*)NclMalloc((unsigned)sizeof(NclInt));
	
	if(tmp == NULL) {
		NhlPError(FATAL,errno,"Not enough memory for source tree construction");
		return(NULL);
	}
	tmp->kind = Ncl_INT;	
	tmp->name = src_tree_names[Ncl_INT];
	tmp->line = cur_line_number;
	tmp->file = cur_load_file;
	tmp->destroy_it = (NclSrcTreeDestroyProc)_NclGenericDestroy;
	tmp->integer= integer;
	tmp->ref_type = Ncl_READIT;
	_NclRegisterNode((NclGenericNode*)tmp);
	return((void*)tmp);
}


void _NclStringExprDestroy
#if __STDC__
(struct ncl_genericnode *thenode)
#else
(thenode)
	struct ncl_genericnode *thenode;
#endif
{
	NclString *tmp = (NclString*)thenode;

	NclFree((void*)tmp->string);
	NclFree((void*)tmp);
}
/*
 * Function:	_NclMakeStringExpr
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
void * _NclMakeStringExpr
#if  __STDC__
(char * str)
#else
(str)
char * str;
#endif
{
	NclString *tmp = (NclString*)NclMalloc((unsigned)sizeof(NclString));
	
	if(tmp == NULL) {
		NhlPError(FATAL,errno,"Not enough memory for source tree construction");
		return(NULL);
	}
	tmp->kind = Ncl_STRING;	
	tmp->name = src_tree_names[Ncl_STRING];
	tmp->line = cur_line_number;
	tmp->file = cur_load_file;
	tmp->destroy_it = (NclSrcTreeDestroyProc)_NclStringExprDestroy;
	if(str != NULL) {
		tmp->string = (char*)NclMalloc((unsigned)strlen(str)+1);
		if(tmp->string == NULL) {	
			NhlPError(FATAL,errno,"Not enough memory for source tree construction");
			return(NULL);
		}
	}
	strcpy(tmp->string, str);
	tmp->ref_type = Ncl_READIT;
	_NclRegisterNode((NclGenericNode*)tmp);
	return((void*)tmp);
}


/*
 * Function:	_NclMakeFuncCall
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
void * _NclMakeFuncCall
#if  __STDC__
(NclSymbol * fname, NclSrcListNode * argument_list,NclSrcTreeTypes type)
#else
(fname, argument_list,type)
NclSymbol * fname; 
NclSrcListNode * argument_list;
NclSrcTreeTypes type;
#endif
{
	NclFuncCall * tmp = (NclFuncCall*)NclMalloc((unsigned)sizeof(NclFuncCall));
	if(tmp == NULL) {
		NhlPError(FATAL,errno,"Not enough memory for source tree construction");
		return(NULL);
	}
	tmp->kind = type;
	tmp->name = src_tree_names[type];
	tmp->line = cur_line_number;
	tmp->file = cur_load_file;
	tmp->destroy_it = (NclSrcTreeDestroyProc)_NclGenericDestroy;
	tmp->func = fname;
	tmp->arg_list = argument_list;
	tmp->ref_type = Ncl_READIT;
	_NclRegisterNode((NclGenericNode*)tmp);
	return((void*)tmp);
}



/*
 * Function:	
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
void *_NclMakeArrayNode
#if  __STDC__
(NclRclList* rc_list)
#else
(rc_list)
NclRclList* rc_list;
#endif
{
	NclArray *tmp = (NclArray*)NclMalloc((unsigned)sizeof(NclArray));
	
	if(tmp == NULL) {
		NhlPError(FATAL,errno,"Not enough memory for source tree construction");
		return(NULL);
	}
	tmp->kind = Ncl_ARRAY;
	tmp->name = src_tree_names[Ncl_ARRAY];
	tmp->line = cur_line_number;
	tmp->file = cur_load_file;
	tmp->destroy_it = (NclSrcTreeDestroyProc)_NclGenericDestroy;
	tmp->rcl = rc_list;
	tmp->ref_type = Ncl_READIT;
	_NclRegisterNode((NclGenericNode*)tmp);
	return((void*)tmp);
}


/*
 * Function:	_NclMakeRowList
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
NclRclList *_NclMakeRowList
#if __STDC__
(void)
#else
()
#endif
{
	NclRclList *tmp = (NclRclList*)NclMalloc((unsigned)sizeof(NclRclList));

	if(tmp == NULL) {
		NhlPError(FATAL,errno,"Not enough memory for source tree construction");
		return(NULL);
	}
	return(tmp);
}


/*
 * Function:	_NclMakeWhile
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
void *_NclMakeWhile
#if  __STDC__
(void * cond_expr, NclSrcListNode * statements)
#else
(cond_expr, statements)
void * cond_expr; 
NclSrcListNode * statements;
#endif
{
	NclDoWhile *tmp = (NclDoWhile*)NclMalloc((unsigned)sizeof(NclDoWhile));
	if(tmp == NULL) {
		NhlPError(FATAL,errno,"Not enough memory for source tree construction");
		return(NULL);
	}
	tmp->kind = Ncl_DOWHILE;
	tmp->name = src_tree_names[Ncl_DOWHILE];
	tmp->line = cur_line_number;
	tmp->file = cur_load_file;
	tmp->destroy_it = (NclSrcTreeDestroyProc)_NclGenericDestroy;
	tmp->cond_expr = cond_expr;
	tmp->stmnts = statements;
	_NclRegisterNode((NclGenericNode*)tmp);
	return((void*)tmp);
}


/*
 * Function:	_NclMakeBlock
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
void *_NclMakeBlock
#if  __STDC__
(NclSrcListNode *statements)
#else
(statements)
NclSrcListNode *statements;
#endif
{
	NclBlock *tmp = (NclBlock*)NclMalloc((unsigned)sizeof(NclBlock));
	if(tmp == NULL) {
		NhlPError(FATAL,errno,"Not enough memory for source tree construction");
		return(NULL);
	}
	tmp->kind = Ncl_BLOCK;
	tmp->name = src_tree_names[Ncl_BLOCK];
	tmp->line = cur_line_number;
	tmp->file = cur_load_file;
	tmp->destroy_it = (NclSrcTreeDestroyProc)_NclGenericDestroy;
	tmp->stmnts = statements;
	_NclRegisterNode((NclGenericNode*)tmp);
	return((void*)tmp);
}

void putspace
#if __STDC__
(int i,FILE *fp)
#else
(i,fp)
	int i;
	FILE *fp;
#endif
{	
	int j;

	for(j = 0;  j < 3*i; j++) {
		putc(' ',fp);
	}
}

void _NclPrintSymbol
#if  __STDC__
(NclSymbol *sym,FILE *fp)
#else
(sym,fp)
	NclSymbol *sym;
	FILE *fp;
#endif
{
	switch(sym->type) {
	case INTEGER:
		fprintf(fp,"%s\t","INTEGER");
		break;
	case FLOAT:
		fprintf(fp,"%s\t","FLOAT");
		break;
	case LONG:
		fprintf(fp,"%s\t","LONG");
		break;
	case DOUBLE:
		fprintf(fp,"%s\t","DOUBLE");
		break;
	case BYTE:
		fprintf(fp,"%s\t","BYTE");
		break;
	case CHARACTER:
		fprintf(fp,"%s\t","CHARACTER");
		break;
	case NUMERIC:
		fprintf(fp,"%s\t","NUMERIC");
		break;
	case FILETYPE:
		fprintf(fp,"%s\t","FILETYPE");
		break;
	case SHORT:
		fprintf(fp,"%s\t","SHORT");
		break;
	case UNDEF:
		fprintf(fp,"%s\t","UNDEF");
		break;
	case VAR:
		fprintf(fp,"%s\t","VAR");
		break;
	case DO:
		fprintf(fp,"%s\t","DO");
		break;
	case QUIT:
		fprintf(fp,"%s\t","QUIT");
		break;
	case PROC:
		fprintf(fp,"%s\t","PROC");
		break;
	case EPROC:
		fprintf(fp,"%s\t","EPROC");
		break;
	case NPROC:
		fprintf(fp,"%s\t","NPROC");
		break;
	case BGIN:
		fprintf(fp,"%s\t","BGIN");
		break;
	case END:
		fprintf(fp,"%s\t","END");
		break;
	case FUNC:
		fprintf(fp,"%s\t","FUNC");
		break;
	case EFUNC:
		fprintf(fp,"%s\t","EFUNC");
		break;
	case NFUNC:
		fprintf(fp,"%s\t","NFUNC");
		break;
	case FDIM:
		fprintf(fp,"%s\t","FDIM");
		break;
	case IF:
		fprintf(fp,"%s\t","IF");
		break;
	case THEN:
		fprintf(fp,"%s\t","THEN");
		break;
	case OBJNAME:
		fprintf(fp,"%s\t","OBJNAME");
		break;
	case OBJTYPE:
		fprintf(fp,"%s\t","OBJTYPE");
		break;
	case DFILE:
		fprintf(fp,"%s\t","DFILE");
		break;
	case KEYFUNC:
		fprintf(fp,"%s\t","KEYFUNC");
		break;
	case KEYPROC:
		fprintf(fp,"%s\t","KEYPROC");
		break;
	case VSBLKCREATE:
		fprintf(fp,"%s\t","VSBLKCREATE");
		break;
	case VSBLKSET:
		fprintf(fp,"%s\t","VSBLKSET");
		break;
	case VSBLKGET:
		fprintf(fp,"%s\t","VSBLKGET");
		break;
	case ELSE:
		fprintf(fp,"%s\t","ELSE");
		break;
	case EXTERNAL:
		fprintf(fp,"%s\t","EXTERNAL");
		break;
	case RETURN:
		fprintf(fp,"%s\t","RETURN");
		break;
	case FILEVAR:
		fprintf(fp,"%s\t","FILEVAR");
		break;
	case BREAK:
		fprintf(fp,"%s\t","BREAK");
		break;
	case CONTINUE:
		fprintf(fp,"%s\t","CONTINUE");
		break;
	default:
		break;
	}
	fprintf(fp,"S: %s\tL: %d\tOf: %d\n",sym->name,sym->level,sym->offset);
}

void _NclPrintTree
#if __STDC__
(void *root,FILE *fp)
#else
(root,fp)
	void *root;
	FILE *fp;
#endif
{
	NclGenericNode *groot = (NclGenericNode*)root;
	NclSrcListNode *step;
	static int i = 0;;

if(groot != NULL) {
/*
	if(groot->file != NULL) {
*/
		fprintf(fp,"%d) ",groot->line);		
/*
	}
*/
	switch(groot->kind) {
		case Ncl_BLOCK:
		{	
			NclBlock *block = (NclBlock*)root;

			putspace(i,fp);
			fprintf(fp,"%s\n",block->name);
			i++;
			step = block->stmnts;
			while(step != NULL) {
				_NclPrintTree(step->node,fp);
				step = step->next;
			}
			i--;
		}
		break;
		case Ncl_RETURN:
		{
			NclReturn *ret = (NclReturn*)root;

			putspace(i,fp);	
			fprintf(fp,"%s\n",ret->name);
			i++;
			_NclPrintTree(ret->expr,fp);
			i--;
		}
			break;
		case Ncl_IFTHEN:
		{
			NclIfThen *ifthen = (NclIfThen*)root;
			
			putspace(i,fp);
			fprintf(fp,"%s\n",ifthen->name);
			i++;
			_NclPrintTree(ifthen->cond_expr,fp);
			step = ifthen->block_stmnt_list;
			while(step != NULL) {
				_NclPrintTree(step->node,fp);
				step = step->next;
			}
			i--;
			
		}
			break;
		case Ncl_IFTHENELSE:
		{
			NclIfThenElse *ifthenelse = (NclIfThenElse*)root;
			
			putspace(i,fp);
			fprintf(fp,"%s\n",ifthenelse->name);
			i++;
			_NclPrintTree(ifthenelse->cond_expr,fp);
			step = ifthenelse->block_stmnt_list1;
			while(step!= NULL) {
				_NclPrintTree(step->node,fp);
				step = step->next;
			}
			step = ifthenelse->block_stmnt_list2;
			while(step!= NULL) {
				_NclPrintTree(step->node,fp);
				step = step->next;
			}
			i--;
		}
			break;
/*
		case Ncl_OBJ:
		{
			NclObj *obj = (NclObj*)root;
			putspace(i,fp);
			fprintf(fp,"%s\n",obj->name);
			i++;
			putspace(i,fp);
			_NclPrintSymbol(obj->obj,fp);
			i--;
		}
			break;
*/
		case Ncl_VISBLKCREATE:
		case Ncl_VISBLKSET:
		case Ncl_VISBLKGET:
		{
			NclVisblk *vblk = (NclVisblk*)root;
			
			putspace(i,fp);
			fprintf(fp,"%s\n",vblk->name);
			i++;
			_NclPrintTree(vblk->objname,fp);
			if(vblk->kind ==Ncl_VISBLKCREATE) {
				_NclPrintTree(vblk->objtype,fp);
			}
			step = vblk->resource_list;
			while(step!= NULL) {
				_NclPrintTree(step->node,fp);
				step = step->next;
			}
			i--;
		}
			break;
		case Ncl_RESOURCE:
		{
			NclResource *resource = (NclResource*)root;
			putspace(i,fp);
			fprintf(fp,"%s\n",resource->name);
			i++;
			putspace(i,fp);
			fprintf(fp,"%s\n",resource->res_name);
			_NclPrintTree(resource->expr,fp);
			i--;
		}
			break;
		case Ncl_GETRESOURCE:
		{
			NclGetResource *resource = (NclGetResource*)root;
			putspace(i,fp);
			fprintf(fp,"%s\n",resource->name);
			i++;
			putspace(i,fp);
			fprintf(fp,"%s\n",resource->res_name);
			putspace(i,fp);
			_NclPrintSymbol(resource->var,fp);
			i--;
		}
			break;
		case Ncl_DOFROMTO:
		{
			NclDoFromTo *dofromto = (NclDoFromTo*)root;
			
			putspace(i,fp);
			fprintf(fp,"%s\n",dofromto->name);
			i++;
			_NclPrintTree(dofromto->inc_var,fp);
			_NclPrintTree(dofromto->start_expr,fp);
			_NclPrintTree(dofromto->end_expr,fp);
			step = dofromto->block_stmnt_list;
			while(step != NULL) {
				_NclPrintTree(step->node,fp);
				step= step->next;
			}
			i--;	
		}
			break;
		case Ncl_DOFROMTOSTRIDE:
		{
			NclDoFromToStride *dofromtostride = 
						(NclDoFromToStride*) root;	

			putspace(i,fp);
			fprintf(fp,"%s\n",dofromtostride->name);
			i++;
			_NclPrintTree(dofromtostride->inc_var,fp);
			_NclPrintTree(dofromtostride->start_expr,fp);
			_NclPrintTree(dofromtostride->end_expr,fp);
			_NclPrintTree(dofromtostride->stride_expr,fp);
			step = dofromtostride->block_stmnt_list;
			while(step != NULL) {
				_NclPrintTree(step->node,fp);
				step= step->next;
			}
			i--;
		}
			break;
		case Ncl_BUILTINPROCCALL:
		case Ncl_EXTERNALPROCCALL:
		case Ncl_PROCCALL:
		{	
			NclProcCall *proccall = (NclProcCall*)root;
			
			putspace(i,fp);
			fprintf(fp,"%s\n",proccall->name);
			i++;
			putspace(i,fp);
			_NclPrintSymbol(proccall->proc,fp);
			step = proccall->arg_list;
			while(step!= NULL) {
				_NclPrintTree(step->node,fp);
				step = step->next;
			}
			i--;
		}
			break;
		case Ncl_FUNCDEF:
		{
			NclFuncDef *funcdef = (NclFuncDef*)root;
	
			putspace(i,fp);
			fprintf(fp,"%s\n",funcdef->name);
			i++;
			putspace(i,fp);
			_NclPrintSymbol(funcdef->func,fp);
			step = funcdef->dec_list;
			while(step!= NULL) {
				_NclPrintTree(step->node,fp);
				step = step->next;
			}	
/*
			step = funcdef->local_dec_list;
			while(step!= NULL) {
				_NclPrintTree(step->node,fp);
				step = step->next;
			}	
*/
			_NclPrintTree(funcdef->block,fp);
			i--;
		}	
			break;
		case Ncl_EXTERNFUNCDEF:
		{
			NclExternFuncDef *externfuncdef = (NclExternFuncDef*)
							 root;

			putspace(i,fp);
			fprintf(fp,"%s\n",externfuncdef->name);
			i++;
			putspace(i,fp);
			_NclPrintSymbol(externfuncdef->func,fp);
			step = externfuncdef->dec_list;
			while(step!= NULL) {
				_NclPrintTree(step->node,fp);
				step = step->next;
			}
			putspace(i,fp);
			fprintf(fp,"%s",externfuncdef->path_info_string);
			i--;
		}
			break;
		case Ncl_LOCALVARDEC:
		{
			NclLocalVarDec *localvardec = (NclLocalVarDec*)root;
	
			putspace(i,fp);
			fprintf(fp,"%s\n",localvardec->name);
			i++;
			putspace(i,fp);
			_NclPrintSymbol(localvardec->var,fp);	
			step = localvardec->dim_size_list;
			while(step!= NULL) {
				_NclPrintTree(step->node,fp);
				step = step->next;
			}
			if(localvardec->data_type != NULL) {
				putspace(i,fp);
				_NclPrintSymbol(localvardec->data_type,fp);
			}
			i--;
		}
			break;
		case Ncl_DIMSIZELISTNODE:
		{
			NclDimSizeListNode *dimsizelistnode = 
						(NclDimSizeListNode*)root;
			putspace(i,fp);
			fprintf(fp,"%s\n",dimsizelistnode->name);
			i++;
			if(dimsizelistnode->any) {
				fprintf(fp,"ANYSIZE\n");
			} else {
				putspace(i,fp);
				fprintf(fp,"%d\n",dimsizelistnode->size);
			}
			i--;
		}
			break;
		case Ncl_PROCDEF:
		{
			NclProcDef * procdef = (NclProcDef*)root;
			putspace(i,fp);
			fprintf(fp,"%s\n",procdef->name);
			i++;
			putspace(i,fp);
			_NclPrintSymbol(procdef->proc,fp);
			step = procdef->dec_list;
			while(step != NULL) {
				_NclPrintTree(step->node,fp);
				step = step->next;
			}
/*
			step = procdef->local_dec_list;
			while(step != NULL) {
				_NclPrintTree(step->node,fp);
				step = step->next;
			}
*/
			_NclPrintTree(procdef->block,fp);
			i--;
		}
			break;
		case Ncl_EXTERNPROCDEF:
		{
			NclExternProcDef *externprocdef = (NclExternProcDef*)
						root;
			
			putspace(i,fp);
			fprintf(fp,"%s\n",externprocdef->name);
			i++;
			putspace(i,fp);
			_NclPrintSymbol(externprocdef->proc,fp);
			step = externprocdef->dec_list;
			while(step != NULL) {
				_NclPrintTree(step->node,fp);
				step = step->next;
			}
			putspace(i,fp);
			fprintf(fp,"%s\n", externprocdef->path_info_string);
			i--;
		}
			break;
		case Ncl_ASSIGN:
		{
			NclAssign *assign = (NclAssign*)root;
			putspace(i,fp);
			fprintf(fp,"%s\n",assign->name);
			i++;
			_NclPrintTree(assign->left_side,fp);
			_NclPrintTree(assign->right_side,fp);
			i--;
		}
			break;
		case Ncl_IDNREF:
		{
			NclIdnRef *nameref = (NclIdnRef*)root;
			putspace(i,fp);
			fprintf(fp,"%s\n",nameref->name);
			i++;
			_NclPrintTree(nameref->thename,fp);
			step = nameref->subscript_list;
			while(step != NULL) {
				_NclPrintTree(step->node,fp);
				step = step->next;
			}
			i--;
		}
			break;
		case Ncl_INTSUBSCRIPT:	
		case Ncl_COORDSUBSCRIPT:	
		{
			NclSubscript *subscript = (NclSubscript*)
					root;

			putspace(i,fp);
			fprintf(fp,"%s\n",subscript->name);
			i++;
			if(subscript->dimname != NULL) {
				putspace(i,fp);
				fprintf(fp,"%s\n",subscript->dimname);
			}
			if(subscript->subexpr != NULL) {
				_NclPrintTree(subscript->subexpr,fp);
			} else {
				putspace(i,fp);
				fprintf(fp,"ALL\n");
			}
			i--;
		}
			break;
		case Ncl_SINGLEINDEX:
		{
			NclSingleIndex *singleindex = (NclSingleIndex*)root;
			
			putspace(i,fp);
			fprintf(fp,"%s\n",singleindex->name);
			i++;
			_NclPrintTree(singleindex->expr,fp);
			i--;
		}
			break;
		case Ncl_RANGEINDEX:
		{
			NclRangeIndex *rangeindex = (NclRangeIndex*)root;
			putspace(i,fp);
			fprintf(fp,"%s\n",rangeindex->name);
			i++;
			if(rangeindex->start_expr != NULL) {
				_NclPrintTree(rangeindex->start_expr,fp);
			} else {
				putspace(i,fp);
				fprintf(fp,"START\n");
			}
			if(rangeindex->end_expr != NULL) {
				_NclPrintTree(rangeindex->end_expr,fp);
			} else {
				putspace(i,fp);
				fprintf(fp,"END\n");
			}
			if(rangeindex->stride!= NULL) {
				_NclPrintTree(rangeindex->stride ,fp);
			} else {
				putspace(i,fp);
				fprintf(fp,"DEFAULT STRIDE\n");
			}
			i--;
		}
			break;
		case Ncl_IDNEXPR:
		{
			NclIdnExpr *idnexpr = (NclIdnExpr*)root;
			putspace(i,fp);
			fprintf(fp,"%s\n",idnexpr->name);
			i++;
			_NclPrintTree(idnexpr->idn_ref_node,fp);
			i--;
		}
			break;
		case Ncl_NEGEXPR:
		case Ncl_NOTEXPR:
		{
			NclMonoExpr *monoexpr = (NclMonoExpr*) root;
			putspace(i,fp);
			fprintf(fp,"%s\n",monoexpr->name);
			i++;
			_NclPrintTree(monoexpr->expr,fp);
			i--;
		}
			break;
		case Ncl_MODEXPR:
		case Ncl_OREXPR:
		case Ncl_ANDEXPR:
		case Ncl_XOREXPR:
		case Ncl_LTSELECTEXPR:
		case Ncl_GTSELECTEXPR:
		case Ncl_PLUSEXPR:
		case Ncl_MINUSEXPR:
		case Ncl_MULEXPR:
		case Ncl_MATMULEXPR:
		case Ncl_DIVEXPR:
		case Ncl_EXPEXPR:
		case Ncl_LEEXPR:
		case Ncl_GEEXPR:
		case Ncl_GTEXPR:
		case Ncl_LTEXPR:
		case Ncl_EQEXPR:
		case Ncl_NEEXPR:
		{
			NclDualExpr *dualexpr = (NclDualExpr*)root;
			putspace(i,fp);
			fprintf(fp,"%s\n",dualexpr->name);
			i++;
			_NclPrintTree(dualexpr->left_expr,fp);
			_NclPrintTree(dualexpr->right_expr,fp);
			i--;
		}
			break;
		case Ncl_REAL:
		{
			NclReal *real = (NclReal*)root;
			putspace(i,fp);
			fprintf(fp,"%s\n",real->name);
			putspace(i+1,fp);
			fprintf(fp,"%s\t",ref_node_names[real->ref_type]);
			fprintf(fp,"%g\n",real->real);
		}
			break;
		case Ncl_INT:
		{
			NclInt *integer = (NclInt*)root;
			putspace(i,fp);
			fprintf(fp,"%s\n",integer->name);
			putspace(i+1,fp);
			fprintf(fp,"%s\t",ref_node_names[integer->ref_type]);
			fprintf(fp,"%d\n",integer->integer);
		}
			break;
		case Ncl_STRING:
		{
			NclString *str= (NclString*)root;
			putspace(i,fp);
			fprintf(fp,"%s\n",str->name);
			putspace(i+1,fp);
			fprintf(fp,"%s\t",ref_node_names[str->ref_type]);
			fprintf(fp,"%s\n",str->string);
		}
			break;
		case Ncl_BUILTINFUNCCALL:
		case Ncl_EXTERNFUNCCALL:
		case Ncl_FUNCCALL:
		{	
			NclFuncCall *funccall = (NclFuncCall*)root;
			
			putspace(i,fp);
			fprintf(fp,"%s\n",funccall->name);
			i++;
			putspace(i,fp);
			fprintf(fp,"%s\t",ref_node_names[funccall->ref_type]);
			_NclPrintSymbol(funccall->func,fp);
			step = funccall->arg_list;
			while(step != NULL) {
				_NclPrintTree(step->node,fp);
				step = step->next;
			}
			i--;
		}
			break;
		case Ncl_ARRAY:
		{
			NclArray *array = (NclArray*)root;
			putspace(i,fp);
			fprintf(fp,"%s\n",array->name);
			i++;
			putspace(i,fp);
			fprintf(fp,"%s\t",ref_node_names[array->ref_type]);
			fprintf(fp,"nelem:%d\n",array->rcl->nelem);
			step = array->rcl->list;
			while(step != NULL) {
				_NclPrintTree(step->node,fp);
				step = step->next;
			}
			i--;
		}
			break;
		case Ncl_DOWHILE:
		{
			NclDoWhile *dowhilel = (NclDoWhile*)root;
			putspace(i,fp);
			fprintf(fp,"%s\n",dowhilel->name);
			i++;
			_NclPrintTree(dowhilel->cond_expr,fp);
			step = dowhilel->stmnts;
			while(step != NULL) {	
				_NclPrintTree(step->node,fp);
				step = step->next;
			}
			i--;
		}
			break;
		case Ncl_VAR:
		{
			NclVar *var = (NclVar*)root;
			putspace(i,fp);
			fprintf(fp,"%s\n",var->name);
			i++;
			putspace(i,fp);
			fprintf(fp,"%s\t",ref_node_names[var->ref_type]);
			_NclPrintSymbol(var->sym,fp);
			step = var->subscript_list;
			while(step != NULL) {
				_NclPrintTree(step->node,fp);
				step = step->next;
			}
			i--;
		}
			break;
		case Ncl_FILEVARDIMNUM:
		{
			NclFileVarDim *filevardim = (NclFileVarDim*)root;
			putspace(i,fp);
			fprintf(fp,"%s\n",filevardim->name);
			i++;
			putspace(i,fp);
			fprintf(fp,"%s\n",filevardim->filevar);
			putspace(i,fp);
			fprintf(fp,"%s\t",ref_node_names[filevardim->ref_type]);
			_NclPrintSymbol(filevardim->filesym,fp);
			putspace(i,fp);
			fprintf(fp,"dim#: %d\n",filevardim->u.dimnum);
			i--;
		}
			break;
		case Ncl_VARDIMNUM:
		{
			NclVarDim *vardim = (NclVarDim*)root;
			putspace(i,fp);
			fprintf(fp,"%s\n",vardim->name);
			i++;
			putspace(i,fp);
			fprintf(fp,"%s\t",ref_node_names[vardim->ref_type]);
			_NclPrintSymbol(vardim->sym,fp);
			putspace(i,fp);
			fprintf(fp,"dim#: %d\n",vardim->u.dimnum);
			i--;
		}
			break;
		case Ncl_FILEVARATT:
		{
			NclFileVarAtt *filevaratt = (NclFileVarAtt*)root;
			putspace(i,fp);
			i++;
			fprintf(fp,"%s\n",filevaratt->name);
			putspace(i,fp);
			fprintf(fp,"%s\t",ref_node_names[filevaratt->ref_type]);
			_NclPrintSymbol(filevaratt->filesym,fp);
			putspace(i,fp);
			fprintf(fp,"attname: %s\n",filevaratt->filevar);
			putspace(i,fp);
			fprintf(fp,"attname: %s\n",filevaratt->attname);
			step = filevaratt->subscript_list ;
			while(step != NULL) {
				_NclPrintTree(step->node,fp);
				step = step->next;
			}
			i--;
		}
			break;
		case Ncl_VARATT:
		{
			NclVarAtt *varatt = (NclVarAtt*)root;
			putspace(i,fp);
			i++;
			fprintf(fp,"%s\n",varatt->name);
			putspace(i,fp);
			fprintf(fp,"%s\t",ref_node_names[varatt->ref_type]);
			_NclPrintSymbol(varatt->sym,fp);
			putspace(i,fp);
			fprintf(fp,"attname: %s",varatt->attname);
			step = varatt->subscript_list ;
			while(step != NULL) {
				_NclPrintTree(step->node,fp);
				step = step->next;
			}
			i--;
		}
			break;
		case Ncl_FILEVARDIMNAME:
		{
			NclFileVarDim *filevardim = (NclFileVarDim*)root;
			putspace(i,fp);
			fprintf(fp,"%s\n",filevardim->name);
			i++;
			putspace(i,fp);
			fprintf(fp,"%s\t",ref_node_names[filevardim->ref_type]);
			_NclPrintSymbol(filevardim->filesym,fp);
			putspace(i,fp);
			fprintf(fp,"%s\n",filevardim->filevar);
			putspace(i,fp);
			fprintf(fp,"dim name: %s\n",filevardim->u.dimname);
			i--;
		}
			break;
		case Ncl_VARDIMNAME:
		{
			NclVarDim *vardim = (NclVarDim*)root;
			putspace(i,fp);
			fprintf(fp,"%s\n",vardim->name);
			i++;
			putspace(i,fp);
			fprintf(fp,"%s\t",ref_node_names[vardim->ref_type]);
			_NclPrintSymbol(vardim->sym,fp);
			putspace(i,fp);
			fprintf(fp,"dim name: %s",vardim->u.dimname);
			i--;
		}
			break;
		case Ncl_FILEVARCOORD:
		{
			NclFileCoord *filecoord = (NclFileCoord*)root;
			putspace(i,fp);
			fprintf(fp,"%s\n",filecoord->name);
			i++;
			putspace(i,fp);
			fprintf(fp,"%s\t",ref_node_names[filecoord->ref_type]);
			_NclPrintSymbol(filecoord->filesym,fp);
			putspace(i,fp);
			fprintf(fp,"%s\n",filecoord->filevar);
			putspace(i,fp);
			fprintf(fp,"coordname: %s\n",filecoord->coord_name);
			step = filecoord->subscript_list;
			while(step != NULL) {
				_NclPrintTree(step->node,fp);
				step = step->next;
			}
			i--;
		}
			break;
		case Ncl_VARCOORD:
		{
			NclCoord *coord = (NclCoord*)root;
			putspace(i,fp);
			fprintf(fp,"%s\n",coord->name);
			i++;
			putspace(i,fp);
			fprintf(fp,"%s\t",ref_node_names[coord->ref_type]);
			_NclPrintSymbol(coord->sym,fp);
			putspace(i,fp);
			fprintf(fp,"coordname: %s\n",coord->coord_name);
			step = coord->subscript_list;
			while(step != NULL) {
				_NclPrintTree(step->node,fp);
				step = step->next;
			}
			i--;
		}
			break;
		case Ncl_FILEVAR:
		{
			NclFileVar *filevar = (NclFileVar*)root;
			putspace(i,fp);
			fprintf(fp,"%s\n",filevar->name);
			i++;
			putspace(i,fp);
			fprintf(fp,"%s\t",ref_node_names[filevar->ref_type]);
			_NclPrintSymbol(filevar->dfile,fp);
			putspace(i,fp);
			fprintf(fp,"%s\n",filevar->filevar);
			step = filevar->subscript_list;
			while(step != NULL) {
				_NclPrintTree(step->node,fp);
				step = step->next;
			}
			i--;
		}
			break;
		case Ncl_BREAK:
		case Ncl_CONTINUE:
		{
			putspace(i,fp);
			fprintf(fp,"%s\n",groot->name);
			i++;
			putspace(i,fp);
			fprintf(fp,"%d\n",groot->line);
			i--;
		}
		break;
		
		default:
		
		fprintf(fp,"UNRECOGNIZED ENUM VALUE!\n");
			break;
	}
	return;
} else {
	fprintf(fp,"ERROR NULL NODE FOUND!\n");
}
}


void _NclFileVarDestroy
#if  __STDC__
(struct ncl_genericnode *thenode)
#else
(thenode)
	struct ncl_genericnode *thenode;
#endif
{
	NclFileVar* tmp = (NclFileVar*)thenode;
	NclFree(tmp->filevar);
	NclFree((void*)tmp);
}
void *_NclMakeFileVarRef
#if  __STDC__
(NclSymbol *dfile,char * filevar, NclSrcListNode * subscript_list, int type )
#else
(dfile ,filevar, subscript_list ,type)
NclSymbol * dfile;
char* filevar;
NclSrcListNode * subscript_list;
int type;
#endif
{
        NclFileVar *tmp = (NclFileVar*)NclMalloc((unsigned)sizeof(NclFileVar));
        if(tmp == NULL) {
                NhlPError(FATAL,errno,"Not enough memory for source tree construction");
                return(NULL);
        }
        tmp->kind = type;
        tmp->name = src_tree_names[type];
	tmp->line = cur_line_number;
	tmp->file = cur_load_file;
	tmp->destroy_it = (NclSrcTreeDestroyProc)_NclFileVarDestroy;
	tmp->dfile = dfile;
        tmp->filevar= (char*)NclMalloc((unsigned)strlen(filevar)+1);;
	if(tmp->filevar == NULL) {
		NhlPError(FATAL,errno,"_MakeFileVarRef: Memory allocation error");
		return(NULL);
	}
	strcpy(tmp->filevar,filevar);
	tmp->subscript_list = subscript_list;
	tmp->ref_type = Ncl_READIT;
	_NclRegisterNode((NclGenericNode*)tmp);
        return((void*)tmp);
}


void *_NclMakeVarRef
#if  __STDC__
(NclSymbol *var,NclSrcListNode *subscript_list)
#else
(var,subscript_list)
NclSymbol *var;
NclSrcListNode *subscript_list;
#endif
{
	NclVar * tmp = (NclVar*)NclMalloc((unsigned)sizeof(NclVar));
	
	if(tmp == NULL) {
		NhlPError(FATAL,errno,"Not enough memory for source tree construction");
		return(NULL);
	}
	tmp->kind = Ncl_VAR;
	tmp->name = src_tree_names[Ncl_VAR];
	tmp->line = cur_line_number;
	tmp->file = cur_load_file;
	tmp->destroy_it = (NclSrcTreeDestroyProc)_NclGenericDestroy;
	tmp->sym = var;
	tmp->subscript_list = subscript_list;
	tmp->ref_type = Ncl_READIT;
	_NclRegisterNode((NclGenericNode*)tmp);
	return((void*)tmp);
}

void *_NclMakeVarDimNumRef
#if  __STDC__
(NclSymbol *var,int dimnum)
#else
(var,dimnum)
NclSymbol *var;
int dimnum;
#endif
{
	NclVarDim *tmp = (NclVarDim*)NclMalloc((unsigned)sizeof(NclVarDim));
	if(tmp == NULL) {
		NhlPError(FATAL,errno,"Not enough memory for source tree construction");
		return(NULL);
	}
	tmp->kind = Ncl_VARDIMNUM;
	tmp->name = src_tree_names[Ncl_VARDIMNUM];
	tmp->line = cur_line_number;
	tmp->file = cur_load_file;
	tmp->destroy_it = (NclSrcTreeDestroyProc)_NclGenericDestroy;
	tmp->sym = var;
	tmp->u.dimnum = dimnum;
	tmp->ref_type = Ncl_READIT;
	_NclRegisterNode((NclGenericNode*)tmp);
	return((void*)tmp);
}
void _NclFileVarDimNumRefDestroy
#if __STDC__
(struct ncl_genericnode *thenode)
#else
(thenode)
	struct ncl_genericnode *thenode;
#endif
{
	NclFileVarDim *tmp = (NclFileVarDim*)thenode;
	
	NclFree((void*)tmp->filevar);
	NclFree((void*)tmp);
}
void *_NclMakeFileVarDimNumRef
#if  __STDC__
(NclSymbol *var,char *filevar,int dimnum)
#else
(var,filevar,dimnum)
NclSymbol *var;
char *filevar;
int dimnum;
#endif
{
	NclFileVarDim *tmp = (NclFileVarDim*)NclMalloc((unsigned)sizeof(NclFileVarDim));
	if(tmp == NULL) {
		NhlPError(FATAL,errno,"Not enough memory for source tree construction");
		return(NULL);
	}
	tmp->kind = Ncl_FILEVARDIMNUM;
	tmp->name = src_tree_names[Ncl_FILEVARDIMNUM];
	tmp->line = cur_line_number;
	tmp->file = cur_load_file;
	tmp->destroy_it = (NclSrcTreeDestroyProc)_NclFileVarDimNumRefDestroy;
	tmp->filesym = var;
	tmp->u.dimnum = dimnum;
	tmp->filevar= (char*)NclMalloc((unsigned)strlen(filevar)+1) ;
	if(tmp->filevar == NULL) {
		NhlPError(FATAL,errno,"Not enough memory for source tree construction");
		return(NULL);
	}
	strcpy(tmp->filevar,filevar);
	tmp->ref_type = Ncl_READIT;
	_NclRegisterNode((NclGenericNode*)tmp);
	return((void*)tmp);
}

void _NclVarDimNameRefDestroy
#if __STDC__
(struct ncl_genericnode *thenode)
#else
(thenode)
	struct ncl_genericnode *thenode;
#endif
{
	NclVarDim *tmp = (NclVarDim*)thenode;
	
	NclFree((void*)tmp->u.dimname);
	NclFree((void*)tmp);
}
void *_NclMakeVarDimNameRef
#if  __STDC__
(NclSymbol *var,char *dimname)
#else
(var,dimname)
NclSymbol *var;
char *dimname;
#endif
{
	NclVarDim *tmp = (NclVarDim*)NclMalloc((unsigned)sizeof(NclVarDim));
	if(tmp == NULL) {
		NhlPError(FATAL,errno,"Not enough memory for source tree construction");
		return(NULL);
	}
	tmp->kind = Ncl_VARDIMNAME;
	tmp->name = src_tree_names[Ncl_VARDIMNAME];
	tmp->line = cur_line_number;
	tmp->file = cur_load_file;
	tmp->destroy_it = (NclSrcTreeDestroyProc)_NclVarDimNameRefDestroy;
	tmp->sym = var;
	tmp->u.dimname= (char*)NclMalloc((unsigned)strlen(dimname)+1);
	if(tmp->u.dimname== NULL) {
		NhlPError(FATAL,errno,"Not enough memory for source tree construction");
		return(NULL);
	}
	strcpy(tmp->u.dimname,dimname);
	tmp->ref_type = Ncl_READIT;
	_NclRegisterNode((NclGenericNode*)tmp);
	return((void*)tmp);
}
void _NclFileVarDimNameRefDestroy
#if __STDC__
(struct ncl_genericnode *thenode)
#else
(thenode)
	struct ncl_genericnode *thenode;
#endif
{
	NclFileVarDim *tmp = (NclFileVarDim*)thenode;
	
	NclFree((void*)tmp->u.dimname);
	NclFree((void*)tmp->filevar);
	NclFree((void*)tmp);
}
void *_NclMakeFileVarDimNameRef
#if  __STDC__
(NclSymbol *filesym,char *filevar,char *dimname)
#else
(filesym,filevar,dimname)
NclSymbol *filesym;
char *filevar;
char *dimname;
#endif
{
	NclFileVarDim *tmp = (NclFileVarDim*)NclMalloc((unsigned)sizeof(NclFileVarDim));
	if(tmp == NULL) {
		NhlPError(FATAL,errno,"Not enough memory for source tree construction");
		return(NULL);
	}
	tmp->kind = Ncl_FILEVARDIMNAME;
	tmp->name = src_tree_names[Ncl_FILEVARDIMNAME];
	tmp->line = cur_line_number;
	tmp->file = cur_load_file;
	tmp->destroy_it = (NclSrcTreeDestroyProc)_NclFileVarDimNameRefDestroy;
	tmp->filesym = filesym;
	tmp->u.dimname= (char*)NclMalloc((unsigned)strlen(dimname)+1);
	if(tmp->u.dimname== NULL) {
		NhlPError(FATAL,errno,"Not enough memory for source tree construction");
		return(NULL);
	}
	strcpy(tmp->u.dimname,dimname);
	tmp->filevar= (char*)NclMalloc((unsigned)strlen(filevar)+1);
	if(tmp->filevar == NULL) {
		NhlPError(FATAL,errno,"Not enough memory for source tree construction");
		return(NULL);
	}
	strcpy(tmp->filevar,filevar);
	tmp->ref_type = Ncl_READIT;
	_NclRegisterNode((NclGenericNode*)tmp);
	return((void*)tmp);
}
void _NclFileVarAttRefDestroy
#if  __STDC__
(struct ncl_genericnode *thenode)
#else 
(thenode)
	struct ncl_genericnode *thenode;
#endif
{
	NclFileVarAtt *tmp =(NclFileVarAtt*)thenode;
	NclFree((void*)tmp->attname);
	NclFree((void*)tmp->filevar);
	NclFree((void*)tmp);
}
void *_NclMakeFileVarAttRef
#if  __STDC__
(NclSymbol *file,char* filevar, char *attname,NclSrcListNode *subscript_list)
#else
(file,filevar,attname,subscript_list)
NclSymbol *file;
char *filevar;
char *attname;
NclSrcListNode *subscript_list;
#endif
{
	NclFileVarAtt *tmp =(NclFileVarAtt*)NclMalloc((unsigned)sizeof(NclFileVarAtt));
	if(tmp == NULL) {
		NhlPError(FATAL,errno,"Not enough memory for source tree construction");
		return(NULL);
	}
	tmp->kind = Ncl_FILEVARATT;
	tmp->name = src_tree_names[Ncl_FILEVARATT];
	tmp->line = cur_line_number;
	tmp->file = cur_load_file;
	tmp->destroy_it = (NclSrcTreeDestroyProc)_NclFileVarAttRefDestroy;
	tmp->filesym = file;
	tmp->attname= (char*)NclMalloc((unsigned)strlen(attname)+1);
	if(tmp->attname == NULL) {
		NhlPError(FATAL,errno,"Not enough memory for source tree construction");
		return(NULL);
	}	
	strcpy(tmp->attname,attname);
	tmp->filevar = (char*)NclMalloc((unsigned)strlen(filevar)+1);;
	if(tmp->filevar== NULL) {
		NhlPError(FATAL,errno,"Not enough memory for source tree construction");
		return(NULL);
	}	
	strcpy(tmp->filevar,filevar);
	tmp->subscript_list = subscript_list;

	tmp->ref_type = Ncl_READIT;	
	_NclRegisterNode((NclGenericNode*)tmp);
	return((void*)tmp);
}
void _NclVarAttRefDestroy
#if  __STDC__
(struct ncl_genericnode *thenode)
#else 
(thenode)
	struct ncl_genericnode *thenode;
#endif
{
	NclVarAtt *tmp =(NclVarAtt*)thenode;
	NclFree((void*)tmp->attname);
	NclFree((void*)tmp);
}
void *_NclMakeVarAttRef
#if  __STDC__
(NclSymbol *var,char *attname,NclSrcListNode *subscript_list)
#else
(var,attname,subscript_list)
NclSymbol *var;
char *attname;
NclSrcListNode *subscript_list;
#endif
{
	NclVarAtt *tmp =(NclVarAtt*)NclMalloc((unsigned)sizeof(NclVarAtt));
	if(tmp == NULL) {
		NhlPError(FATAL,errno,"Not enough memory for source tree construction");
		return(NULL);
	}
	tmp->kind = Ncl_VARATT;
	tmp->name = src_tree_names[Ncl_VARATT];
	tmp->line = cur_line_number;
	tmp->file = cur_load_file;
	tmp->destroy_it = (NclSrcTreeDestroyProc)_NclVarAttRefDestroy;
	tmp->sym = var;
	tmp->attname= (char*)NclMalloc((unsigned)strlen(attname)+1);
	if(tmp->attname == NULL) {
		NhlPError(FATAL,errno,"Not enough memory for source tree construction");
		return(NULL);
	}	
	strcpy(tmp->attname,attname);
	tmp->subscript_list = subscript_list;

	tmp->ref_type = Ncl_READIT;	
	_NclRegisterNode((NclGenericNode*)tmp);
	return((void*)tmp);
}
void _NclFileVarCoordRefDestroy
#if __STDC__
(struct ncl_genericnode *thenode)
#else
(thenode)
	struct ncl_genericnode *thenode;
#endif
{
	NclFileCoord *tmp= (NclFileCoord*)thenode;

	NclFree((void*)tmp->coord_name);
	NclFree((void*)tmp->filevar);
	NclFree((void*)tmp);
}
void *_NclMakeFileVarCoordRef
#if  __STDC__
(NclSymbol *var,char* filevar, char *coord,NclSrcListNode *subscript_list)
#else
(var,coord,filevar,subscript_list)
NclSymbol *var;
char *filevar;
char *coord;
NclSrcListNode *subscript_list;
#endif
{
	NclFileCoord *tmp= (NclFileCoord*)NclMalloc((unsigned)sizeof(NclFileCoord));

	if(tmp == NULL) {
		NhlPError(FATAL,errno,"Not enough memory for source tree construction");
		return(NULL);
	}
	tmp->kind = Ncl_FILEVARCOORD;
	tmp->name = src_tree_names[Ncl_FILEVARCOORD];
	tmp->line = cur_line_number;
	tmp->file = cur_load_file;
	tmp->destroy_it = (NclSrcTreeDestroyProc)_NclFileVarCoordRefDestroy;
	tmp->filesym = var;
	tmp->filevar = (char*)NclMalloc((unsigned)strlen(filevar)+1);
	if(tmp->filevar== NULL) {
		NhlPError(FATAL,errno,"Not enough memory for source tree construction");
		return(NULL);
	}	
	strcpy(tmp->filevar,filevar);

	tmp->coord_name = (char*)NclMalloc((unsigned)strlen(coord)+1);
	if(tmp->coord_name == NULL) {
		NhlPError(FATAL,errno,"Not enough memory for source tree construction");
		return(NULL);
	}	
	strcpy(tmp->coord_name,coord);
	tmp->subscript_list = subscript_list;
	tmp->ref_type = Ncl_READIT;
	_NclRegisterNode((NclGenericNode*)tmp);
	return((void*)tmp);
}
void _NclVarCoordRefDestroy
#if __STDC__
(struct ncl_genericnode *thenode)
#else
(thenode)
	struct ncl_genericnode *thenode;
#endif
{
	NclCoord *tmp= (NclCoord*)thenode;

	NclFree((void*)tmp->coord_name);
	NclFree((void*)tmp);
}
void *_NclMakeVarCoordRef
#if  __STDC__
(NclSymbol *var,char *coord,NclSrcListNode *subscript_list)
#else
(var,coord,subscript_list)
NclSymbol *var;
char *coord;
NclSrcListNode *subscript_list;
#endif
{
	NclCoord *tmp= (NclCoord*)NclMalloc((unsigned)sizeof(NclCoord));

	if(tmp == NULL) {
		NhlPError(FATAL,errno,"Not enough memory for source tree construction");
		return(NULL);
	}
	tmp->kind = Ncl_VARCOORD;
	tmp->name = src_tree_names[Ncl_VARCOORD];
	tmp->line = cur_line_number;
	tmp->file = cur_load_file;
	tmp->destroy_it = (NclSrcTreeDestroyProc)_NclVarCoordRefDestroy;
	tmp->sym = var;
	tmp->coord_name = (char*)NclMalloc((unsigned)strlen(coord)+1);
	if(tmp->coord_name == NULL) {
		NhlPError(FATAL,errno,"Not enough memory for source tree construction");
		return(NULL);
	}	
	strcpy(tmp->coord_name,coord);
	tmp->subscript_list = subscript_list;
	tmp->ref_type = Ncl_READIT;
	_NclRegisterNode((NclGenericNode*)tmp);
	return((void*)tmp);
}






void _NclFreeTree
#if __STDC__
(void *root,int is_error)
#else
(root,is_error)
        void *root;
	int is_error;
#endif
{
	int i;

	for(i = 0 ; i< node_list_index; i++) {
		(*node_list[i]->destroy_it)((struct ncl_genericnode*)node_list[i]);
	}
	_NclResetNodeList();
}
#ifdef __cplusplus
}
#endif
