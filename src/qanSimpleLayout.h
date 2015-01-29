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
// \file	qanSimpleLayout.h
// \author	benoit@qanava.org
// \date	2007 January 06
//-----------------------------------------------------------------------------


#ifndef qanSimpleLayout_h
#define qanSimpleLayout_h


// Qanava headers
#include "./qanLayout.h"


//-----------------------------------------------------------------------------
namespace qan { // ::qan

	//! Layout a graph as concentric circles of nodes.
	/*!
		\nosubgrouping
	*/
	class Concentric : public Layout
	{
		/*! \name Concentric Constructor/Destructor *///-----------------------
		//@{
	public:

		//! Concentric constructor.
		Concentric( double azimutDelta = 45., double circleInterval = 50. ) : 
			Layout( ), _azimutDelta( azimutDelta ), _circleInterval( circleInterval ) { }

	private:

		double	_azimutDelta;

		double	_circleInterval;
		//@}
		//---------------------------------------------------------------------



		/*! \name Concentric Layout Management *///----------------------------
		//@{
	public:

		//! .
		virtual void	layout( Graph& graph, QProgressDialog* progress = 0 );
		//@}
		//---------------------------------------------------------------------
	};


	//! Layout nodes in a (logarithm) colimacon.
	/*!
		\nosubgrouping
	*/
	class Colimacon : public Layout
	{
		/*! \name Colimacon Constructor/Destructor *///------------------------
		//@{
	public:

		//! Colimacon constructor.
		Colimacon( double azimutDelta = 15., double circleInterval = 10. ) :
			Layout( ), _azimutDelta( azimutDelta ), _circleInterval( circleInterval ) { }

	private:

		double	_azimutDelta;

		double	_circleInterval;
		//@}
		//---------------------------------------------------------------------



		/*! \name Colimacon Layout Management *///-----------------------------
		//@{
	public:

		//! .
		virtual void	layout( Graph& graph, QProgressDialog* progress = 0 );
		//@}
		//---------------------------------------------------------------------
	};
} // ::qan
//-----------------------------------------------------------------------------


#endif // qanSimpleLayout_h

