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
// \file	qanGrid.h
// \author	benoit@qanava.org
// \date	2004 December 05
//-----------------------------------------------------------------------------


#ifndef qanGrid_h
#define qanGrid_h


// QT headers
#include <QLineF>
#include <QRectF>
#include <QSizeF>
#include <QColor>


//-----------------------------------------------------------------------------
namespace qan { // ::qan

	//! Defines the necessary methods to build a grid on a virtual canvas (draw lines, text, manage resizing, etc.).
	/*!		
		Grid geometry is added dynamically using virtual methods addLine(), addRectangle() and addText() implemented
		in a concrete grid for a particular graphic subsystem (for exemple QT GraphView and GridCheckBoardItem).

		\sa GridCheckBoardItem
		\nosubgrouping
	*/
	class Grid
	{
		/*! \name Grid Constructor and Destructor *///-------------------------
		//@{
	public:

		//! Grid constructor with eventual layout initialization.
		Grid( ) { }

		//! Grid destructor.
		virtual ~Grid( ) { }

	private:

		Grid( const Grid& g );
		//@}
		//---------------------------------------------------------------------



		/*! \name Layout and Size Management  *///-----------------------------
		//@{
	public:

		//! Resize the grid (ie, take care of resizing the specified horizontal and vertical lines, and notify the grid layout of the resize).
		virtual void	resize( QSizeF ) { }
		//@}
		//---------------------------------------------------------------------



		/*! \name Grid Construction Management  *///---------------------------
		//@{
	public:

		//! Add a grid line on the canvas with specific line settings.
		virtual void	addLine( QLineF l, float w = 1, bool dash = false, bool dot = false ) = 0;

		//! Add a grid rectangle on the canvas with specific size and color settings.
		virtual void	addRectangle( QRectF r, QColor c ) = 0;

		//! Add a grid text label on the canvas.
		virtual void	addText( const QString& text, QPointF p, bool bold = false ) = 0;

		//! Add an horizontal line to the grid canvas (horizontal lines are automatically resized when the grid canvas is resized).
		virtual void	addHorizontalLine( QLineF l, int w = 1, bool dash = false, bool dot = false ) = 0;

		//! Add a vertical line to the grid canvas (vertical lines are automatically resized when the grid canvas is resized).
		virtual void	addVerticalLine( QLineF l, int w = 1, bool dash = false, bool dot = false ) = 0;
		//@}
		//---------------------------------------------------------------------
	};
} // ::qan
//-----------------------------------------------------------------------------


#endif // qanGrid_h

