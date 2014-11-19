/*
	Copyright (C) 2008-2014 Benoit AUTHEMAN

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
// \file	qanGridItem.h
// \author	benoit@qanava.org
// \date	2004 December 05
//-----------------------------------------------------------------------------


#ifndef qanGridItem_h
#define qanGridItem_h


// Qanava headers
#include "./qanGrid.h"
#include "./qanGraphView.h"


// QT headers
#include <QGraphicsScene>
#include <QGraphicsItem>


//-----------------------------------------------------------------------------
namespace qan { // ::qan

	class Canvas;


    //! Implements the abstract methods defined in class Grid. Unmaintened code for 0.9.1 ...
	/*!
        \deprecated
		\nosubgrouping
	*/
	class GridItem : public QObject, public QGraphicsItem, public Grid
	{
		Q_OBJECT

		/*! \name Grid Constructor/Destructor  *///----------------------------
		//@{
	public:

		//! Grid constructor with canvas and backcolor initialization (backcolor is not drawn, except for optimized line's backcolor).
		GridItem( GraphView* GraphView );

		//! Grid destructor, once destroyed, all graphic items are cleared from the grid canvas.
		virtual	~GridItem( );
		//@}
		//---------------------------------------------------------------------



		/*! \name Graphics View Grid Implementation *///-----------------------
		//@{
	public:

		QRectF			boundingRect( ) const;

        virtual void	paint( QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = 0 ) { Q_UNUSED( painter ); Q_UNUSED( option ); Q_UNUSED( widget ); }

		virtual	void	drawBackground( QPainter& painter, const QRectF& rect );
		//@}
		//---------------------------------------------------------------------



		/*! \name Grid Construction Management  *///---------------------------
		//@{
	public:

		//! \copydoc Grid::addLine()
		virtual void	addLine( QLineF l, float w = 1, bool dash = false, bool dot = false );

		//! \copydoc Grid::addRectangle()
		virtual void	addRectangle( QRectF r, QColor c );

		//! \copydoc Grid::addText()
		virtual void	addText( const QString& text, QPointF p, bool bold = false );

		//! \copydoc Grid::addHorizontalLine()
		virtual void	addHorizontalLine( QLineF l, int w = 1, bool dash = false, bool dot = false );

		//! \copydoc Grid::addVerticalLine()
		virtual void	addVerticalLine( QLineF l, int w = 1, bool dash = false, bool dot = false );
		//@}
		//---------------------------------------------------------------------
	};



	//! Regular grid draws a square pattern of horizontal and vertical dotted lines (ie a standard grid!).
	/*!
		<img src="./images/regulargridshot.png" alt="Qanava regular grid">
		
		\nosubgrouping
	*/
	class GridRegularItem : public GridItem
	{
		/*! \name Regular Grid Management  *///--------------------------------
		//@{
	public:

		//! GridRegularItem constructor with grid line spacing initialization.
		GridRegularItem( GraphView* GraphView, int spacing = 60 );

		virtual ~GridRegularItem( ) { }

	private:

		int		_spacing;

		QSize	_sizeMax;

		QPoint	_gradCount;
		//@}
		//---------------------------------------------------------------------
	};


	//! Draw a bicolor checkboard pattern regular grid.
	/*!
		\nosubgrouping
	*/
	class GridCheckBoardItem : public GridItem
	{
		Q_OBJECT

		/*! \name CheckBoard Grid Management  *///-----------------------------
		//@{
	public:

		//! GridCheckBoardItem constructor with black, white color and square size initialization.
		GridCheckBoardItem( GraphView* GraphView, QColor white, QColor black, qreal length = 50. );

		virtual ~GridCheckBoardItem( ) { }

		virtual	void	drawBackground( QPainter& painter, const QRectF& rect );

	private:

		QPixmap			_squaresPattern;

		QColor			_white;

		QColor			_black;
		//@}
		//---------------------------------------------------------------------
	};

} // ::qan
//-----------------------------------------------------------------------------


#endif // qanGridItem_h

