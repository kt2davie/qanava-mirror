/*
	Copyright (C) 2008-2015 Benoit AUTHEMAN

    This file is part of Qanava.

    Qanava is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Qanava is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with Qanava.  If not, see <http://www.gnu.org/licenses/>.
*/

//-----------------------------------------------------------------------------
// This file is a part of the Qanava software.
//
// \file	qanConfig.h
// \author	benoit@qanava.org
// \date	2004 September 21
//-----------------------------------------------------------------------------


#ifndef qanConfig_h
#define qanConfig_h

#ifdef Q_CC_MSVC

#pragma warning( disable:4786 )  // VC6: Too long template name in debug informations

#pragma warning( disable:4251 )  // VC7.1: BOOST dll interface warning

#pragma warning( disable:4275 )  // VC7.1: BOOST dll interface warning

#pragma warning( disable:4290 )  // VC7.1: Explicit C++ exception specification

#pragma warning( disable:4100 )  // VC7.1: Unreferenced formal parameter

#pragma warning( disable:4244 )  // VC7.1: BOOST date and time warning

#endif

#endif // qanConfig_h

