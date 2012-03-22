//******************************************************************************
//   Copyright (C) 2001-2008 by Jozef Starosczyk
//   ixen@copyhandler.com
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU Library General Public License
//   (version 2) as published by the Free Software Foundation;
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU Library General Public
//   License along with this program; if not, write to the
//   Free Software Foundation, Inc.,
//   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//
//******************************************************************************
/// @file ErrorConstants.h
/// @brief Contains error constants used throughout the project.
//******************************************************************************
#ifndef __ERRORCONSTANTS_H__
#define __ERRORCONSTANTS_H__

enum EFileError
{
	eDeleteError,		///< Problem occured when tried to delete the fs object
	eSeekError,			///< Problem occured when tried to set file pointer
	eResizeError,		///< Problem occured when tried to change size of the fs object
	eReadError,			///< Problem occured when tried to read data from file
	eWriteError,		///< Problem occured when tried to write data to a file
	eFastMoveError,		///< Problem occured when tried to perform fast move operation (that does not involve copying contents)
	eCreateError		///< Problem occured when tried to create the fs object
};

#endif
