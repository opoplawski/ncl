C
C $Id: WORKSTATION.f,v 1.1 1994-05-12 23:52:48 boote Exp $
C
C****************************************************************
C								*
C			Copyright (C)  1994			*
C	University Corporation for Atmospheric Research		*
C			All Rights Reserved			*
C								*
C****************************************************************
C
C      File:            WORKSTATION.f
C
C      Author:          Jeff W. Boote
C                       National Center for Atmospheric Research
C                       PO 3000, Boulder, Colorado
C
C      Date:            Fri Apr 15 16:57:55 MDT 1994
C
C      Description:     
C
      subroutine nhlfupdateworkstation(idwork,ierr)
      	call nhlf_updateworkstation(idwork,ierr)
      end
      subroutine nhlfclearworkstation(idwork,ierr)
      	call nhlf_clearworkstation(idwork,ierr)
      end
      subroutine nhlfframe(id_work,ierr)
	call nhl_fframe(id_work,ierr)
      end
      subroutine nhlfsetcolor(id_work,index,rred,rgreen,rblue,ierr)
	call nhl_fsetcolor(id_work,index,rred,rgreen,rblue,ierr)
      end
      subroutine nhlffreecolor(id_work,index,ierr)
	call nhl_ffreecolor(id_work,index,ierr)
      end
      subroutine nhlfnewcolor(id_work,rred,rgreen,rblue,indx_ret)
	call nhl_fnewcolor(id_work,rred,rgreen,rblue,indx_ret)
      end
      subroutine nhlfgetgksci(id_work,indx_hlu,indx_gks_ret)
	call nhl_fgetgksci(id_work,indx_hlu,indx_gks_ret)
      end
      subroutine nhlfisallocatedcolor(id_work,indx,iret)
	call nhl_fisallocatedcolor(id_work,indx,iret)
      end
      subroutine nhlfgetgksworkid(id_work,id_gks_ret)
	call nhl_fgetgksworkid(id_work,id_gks_ret)
      end
      subroutine nhlfnewmarker(id_work,marker,xoff,yoff,aspadj,sizeadj,
     %	indx_ret)

	integer id_work,indx_ret
	character*(*) marker
	real xoff,yoff,aspadj,sizeadj

	call nhl_fnewmarker(id_work,marker,len(marker),xoff,yoff,aspadj,
     %		sizeadj,indx_ret)
      end
      subroutine nhlfsetmarker(id_work,indx,marker,xoff,yoff,aspadj,
     %	sizeadj,ierr)

	integer id_work,indx,ierr
	character*(*) marker
	real xoff,yoff,aspadj,sizeadj

	call nhl_fsetmarker(id_work,indx,marker,len(marker),xoff,yoff,
     %		aspadj,sizeadj,indx_ret)
      end
