/*
 *	$Id: ictrans.h,v 1.11 2000-08-22 03:30:30 haley Exp $
 */
/************************************************************************
*                                                                       *
* This file is free software; you can redistribute it and/or modify     *
* it under the terms of the GNU General Public License as published     *
* by the Free Software Foundation; either version 2 of the License, or  *
* (at your option) any later version.                                   *
*                                                                       *
* This software is distributed in the hope that it will be useful, but  *
* WITHOUT ANY WARRANTY; without even the implied warranty of            *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
* General Public License for more details.                              *
*                                                                       *
* You should have received a copy of the GNU General Public License     *
* along with this software; if not, write to the Free Software          *
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307   *
* USA.                                                                  *
*                                                                       *
************************************************************************/

/***********************************************************************
*                                                                      *
*                          Copyright (C)  1990                         *
*            University Corporation for Atmospheric Research           *
*                          All Rights Reserved                         *
*                                                                      *
*                          NCAR View V3.00alpha                        *
*                                                                      *
***********************************************************************/

/*
 *
 *	Author		John Clyne
 * 
 *
 *	type declarations for ictrans
 */

#ifndef	_ictrans_
#define	_ictrans_

#include <ncarg/ctrans.h>

#ifndef	FALSE
#define	FALSE	0
#define	TRUE	~FALSE
#endif

#ifndef	ABS
#define	ABS(X)	(((X) < (0)) ? -(X) : (X))
#endif


#define	MAX_LINE_LEN	256 /* maximum length of a command line	*/

/*
 * default command to execute when a frame list is given without a command
 */
#define	DEFAULT_COMMAND	"list"

#define	DEFAULTFONT	"font1"

#define	MAXX	32767
#define	MAXY	MAXX

/*
 * Format of command table. (a command object)
 */
typedef	struct	{
	char	*c_name;	/* the name of the command	*/
	char    *c_help;        /* help string */
	char    *c_usage;	/* usage string */
	short	addresses;	/* is the command a 0,1 or 2 address command */
	short	data;		/* command may have data		 */
	int     (*c_handler)(); /* function to call */
}	CmdOp;


/*
 * structure for addressing frames
 */
typedef	struct	{
	int	start_frame,	/* first frame 	*/
		num_frames;	/* number of frames starting with first	*/
	} FrameCount;

/*
 * a list of frames
 */
typedef	struct	{
	int	num;		/* number of FrameCounts	*/
	int	num_alloced;	/* space allocated		*/
	FrameCount	*fc;	/* FrameCount list		*/
} FrameList;

typedef	struct	{
	FrameList	src_frames;
	FrameList	dst_frames;
	char	*name;
	char	*data;
	} Cmd;


/*
 * a complete command object
 */
typedef	struct {
	int	quiet;		/* operate in quiet mode?		*/	
	int	batch;		/* are we in batch mode?		*/
	int	force_prompt;	/* force issuing of prompt?		*/
	FILE	*fp;		/* file pointer for user prompts	*/
	int	current_frame,	/* current frame in the file		*/
		last_frame;	/* last frame in the file		*/
	Cmd	cmd;		/* name of command to execute and its data*/
	CmdOp	*c;		/* the command object		*/
	} ICommand, *ICommandPtr;

typedef	struct	{
	float	llx,lly,urx,ury;
	} DevWindow;

/*
 * struct that maintains a record of the state of ictrans
 */
typedef	struct	_icState {
	int	movie;
	int	dup;
	int	start_segment;
	int	stop_segment;
	int	skip;
	int	num_frames;
	short	loop;
	short	movietime;
	char	*spool_alias;
	char	*device;
	char	*font;
	char	*palette_file;
	char	*save_file;
	char	**file;
	DevWindow	dev_window;
	} IcState; 


#define	SMALL_MALLOC_BLOCK	10

extern	void	log_ct(
#ifdef	NeedFuncProto
	CtransRC	err
#endif
);

#endif	/* _ictrans_	*/
