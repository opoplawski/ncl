C
C $Id: mdrgfa.f,v 1.4 2007-09-20 21:44:45 kennison Exp $
C
C                Copyright (C)  2000
C        University Corporation for Atmospheric Research
C                All Rights Reserved
C
C This file is free software; you can redistribute it and/or modify
C it under the terms of the GNU General Public License as published
C by the Free Software Foundation; either version 2 of the License, or
C (at your option) any later version.
C
C This software is distributed in the hope that it will be useful, but
C WITHOUT ANY WARRANTY; without even the implied warranty of
C MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
C General Public License for more details.
C
C You should have received a copy of the GNU General Public License
C along with this software; if not, write to the Free Software
C Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
C USA.
C
      SUBROUTINE MDRGFA (XCRA,YCRA,NCRA,IAAI,IAGI,NGPS)
C
C This routine is called by ARSCAM, which has been called by MDRGSX,
C which has been called by MDRGSF.  If the polygon defined by the
C arguments is one that should be filled, the polygon is transformed
C from its position on the unit square to a 1-degree square on a
C lat/lon grid and then to its position on the map defined by the
C current state of EZMAP and then filled.
C
        REAL XCRA(NCRA),YCRA(NCRA)
        INTEGER IAAI(NGPS),IAGI(NGPS),NGPS
C
C Declare required EZMAP common blocks.
C
        COMMON /MAPRGD/  ICOL(5),ICSF(5),IDPF,LCRA,NILN,NILT,OLAT,OLON
        INTEGER          ICOL,ICSF,IDPF,LCRA,NILN,NILT
        REAL             OLAT,OLON
        SAVE   /MAPRGD/
C
C Declare local variables.
C
        REAL             UVAL,VVAL,XVPL,XVPR,YVPB,YVPT,XWDL,XWDR,YWDB,
     +                   YWDT
        INTEGER          LNLG
C
C Because of the way MDRGFA is called, we know that the polygon has but
C one area identifier, relative to edge group identifier 1; if that id
C is greater than 0 and the polygon has at least three points, then we
C transform and fill it.
C
        IF (NCRA.GT.2.AND.IAAI(1).GT.0) THEN
          IF (ICSF(IAAI(1)).GE.0) THEN
            NCRM=0
            DO 101 I=1,NCRA
              CALL MAPTRN (OLAT+YCRA(I),OLON+XCRA(I),UVAL,VVAL)
              IF (UVAL.NE.1.E12) THEN
                NCRM=NCRM+1
                XCRA(NCRM)=UVAL
                YCRA(NCRM)=VVAL
              END IF
  101       CONTINUE
            IF (NCRM.GT.2) THEN
              CALL GSFACI (ICSF(IAAI(1)))
              CALL GFA    (NCRM,XCRA,YCRA)
            END IF
          END IF
        END IF
C
C Done.
C
        RETURN
C
      END
