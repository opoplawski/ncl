C
C $Id: mpsetr.f,v 1.17 2008-09-11 22:53:34 kennison Exp $
C
C                Copyright (C)  2000
C        University Corporation for Atmospheric Research
C                All Rights Reserved
C
C The use of this Software is governed by a License Agreement.
C
      SUBROUTINE MPSETR (WHCH,RVAL)
        CHARACTER*(*) WHCH
        REAL          RVAL
        IF (ICFELL('MPSETR - UNCLEARED PRIOR ERROR',1).NE.0) RETURN
        CALL MDSETR (WHCH,RVAL)
        IF (ICFELL('MPSETR',2).NE.0) RETURN
        RETURN
      END
