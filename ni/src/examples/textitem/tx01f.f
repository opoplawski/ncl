CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
C                                                                      C
C                Copyright (C)  1995                                   C
C        University Corporation for Atmospheric Research               C
C                all rights reserved                                   C
C                                                                      C
CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
C
C      File:           tx01f.f
C
C      Author:         Bob Lackman (converted by Ed Stautler)
C          National Center for Atmospheric Research
C          PO 3000, Boulder, Colorado
C
C      Date:           Fri Jan 06 18:31:18 mdt 1995
C
C      Description:    Demonstrates the textitem object
C                      defaults.
C
      external nhlfapplayerclass
      external nhlfxworkstationlayerclass
      external nhlftextitemlayerclass

      integer appid, wid, pid
      integer rlist, ierr

C
C Initialize the high level utility library
C
      call nhlfinitialize

C
C Create an application context. Set the app dir to the current
C directory so the application looks for a resource file in the
C working directory. In this example the resource file supplies the
C plot title only.
C
      call nhlfrlcreate(rlist,'setrl')
      call nhlfrlclear(rlist)
      call nhlfrlsetstring(rlist,'appDefaultParent','True',ierr)
      call nhlfrlsetstring(rlist,'appUsrDir','./',ierr)
      call nhlfcreate(appid,'tx01',nhlfapplayerclass,0,rlist,ierr)
C
C Create an xworkstation object.
C
      call nhlfrlclear(rlist)
      call nhlfrlsetstring(rlist,'wkPause','True',ierr)
      call nhlfcreate(wid,'tx01Work',nhlfxworkstationlayerclass,
     $     0,rlist,ierr)
C
C Specify the viewport extent of the object.
C
      call nhlfrlclear(rlist)
      call nhlfrlsetfloat(rlist,'vpXF',.2,ierr)
      call nhlfrlsetfloat(rlist,'vpYF',.8,ierr)
      call nhlfrlsetfloat(rlist,'vpWidthF',.6,ierr)
      call nhlfrlsetfloat(rlist,'vpHeightF',.6,ierr)

      call nhlfcreate(pid,'TextItems',nhlftextitemlayerclass,
     $     wid,rlist,ierr)

      call nhlfdraw(pid,ierr)
      call nhlfframe(wid,ierr)
      call nhlfdestroy(pid,ierr)
      call nhlfdestroy(wid,ierr)
      call nhlfdestroy(appid,ierr)
      call nhlfclose

      stop
      end
