C
C $Id: HLU.f,v 1.1 1994-05-12 23:51:24 boote Exp $
C
C****************************************************************
C								*
C			Copyright (C)  1994			*
C	University Corporation for Atmospheric Research		*
C			All Rights Reserved			*
C								*
C****************************************************************
C
C      File:            HLU.f
C
C      Author:          Jeff W. Boote
C                       National Center for Atmospheric Research
C                       PO 3000, Boulder, Colorado
C
C      Date:            Fri Mar 25 10:56:58 MST 1994
C
C      Description:     This file contains all the "general" hlu
C			subroutines to support the fortran interface
C			to the hlu library.
C
C
C	In Open.c
C
      subroutine nhlfopen
      	call nhl_fopen
      end
C
C	In Close.c
C
      subroutine nhlfclose
      call nhl_fclose
      end
C
C	In Create.c
C
      subroutine nhlfcreate (id_obj,name,cfunc,id_parent,id_rl,err_ret)
	integer id_obj,id_parent,id_rl,err_ret
	character*(*) name
	external cfunc
      	call nhl_fcreate(id_obj,name,cfunc,id_parent,id_rl,len(name),
     %							err_ret)
      end
C
C	SetValues.c
C
      subroutine nhlfsetvalues (id_obj,id_rl,err_ret)
	integer id_obj, id_rl, err_ret
	call nhl_fsetvalues(id_obj,id_rl,err_ret)
      end
C
C	GetValues.c
C
      subroutine nhlfgetvalues (id_obj,id_rl,err_ret)
	integer id_obj, id_rl, err_ret
	call nhl_fgetvalues(id_obj,id_rl,err_ret)
      end
C
C	Draw.c
C
      subroutine nhlfdraw (id_obj,err_ret)
	integer id_obj,err_ret
	call nhl_fdraw(id_obj,err_ret)
      end
C
C	Destroy.c
C
      subroutine nhlfdestroy (id_obj,err_ret)
	integer id_obj,err_ret
	call nhl_fdestroy(id_obj,err_ret)
      end
C
C	hlu.c
C
      subroutine nhlfname(id_obj,name_ret,ierr)
	integer id_obj,ierr
	character*(*) name_ret
	call nhl_fname(id_obj,name_ret,len(name_ret),ierr)
      end
C
C	hlu.c
C
      subroutine nhlfclassname(id_obj,name_ret,ierr)
	integer id_obj,ierr
	character*(*) name_ret
	call nhl_fclassname(id_obj,name_ret,len(name_ret),ierr)
      end
C
C	Reparent.c
C
      subroutine nhlfchangeworkstation(id_plot,id_work,ierr)
      	call nhlf_changeworkstation(id_plot,id_work,ierr)
      end
