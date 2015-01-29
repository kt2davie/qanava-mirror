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
// \file	qanGraphView.h
// \author	benoit@qanava.org
// \date	2006 July 29
//-----------------------------------------------------------------------------


#ifndef canGraphView_h
#define canGraphView_h


// Qanava headers
#include "./qanController.h"
#include "./qanGraphScene.h"
#include "./qanGraph.h"
#include "./qanGrid.h"
#include "./qanGridItem.h"

// QT headers
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsItem>
#include <QProgressDialog>
#include <QDragLeaveEvent>
#include <QDropEvent>
#include <QDragEnterEvent>


//-----------------------------------------------------------------------------
namespace qan { // ::qan

	class GridItem;
	class Grid;
	class Layout;

	class GraphView : public QGraphicsView
	{
		Q_OBJECT

		/*! \name GraphView Constructors/Destructors *///-------------------
		//@{
	public:

		GraphView( QWidget* parent = 0, QColor backColor = QColor( 170, 171, 205 ), QSize size = QSize( 300, 150 ) );

		virtual QSize	sizeHint( ) const { return QSize( 300, 250 ); }

		void			setGraph( Graph& graph );

	protected:

		GraphScene*		getGraphScene( ) { return &_graph->getM( ); }

		void			configureView( QColor backColor, QSize size );

	private:

		Graph*			_graph;
		//@}
		//---------------------------------------------------------------------



		/*! \name Grid Management *///-----------------------------------------
		//@{
	public:

		void			setGrid( GridItem* grid ) { _grid = grid; }

		GridItem*		getGrid( ) { return _grid; }

	protected:

		virtual void	drawBackground( QPainter* painter, const QRectF& rect );

	private:

		GridItem*		_grid;
		//@}
		//---------------------------------------------------------------------



		/*! \name View Controllers Management *///-----------------------------
		//@{
	protected:

		virtual void		keyPressEvent( QKeyEvent* e );

		virtual void		mousePressEvent( QMouseEvent* e );

		virtual void		mouseReleaseEvent( QMouseEvent* e );

		virtual void		mouseMoveEvent( QMouseEvent* e );

		virtual	void		mouseDoubleClickEvent( QMouseEvent* e );

		virtual	void		wheelEvent( QWheelEvent* e );

		virtual void		resizeEvent ( QResizeEvent* e );

        virtual bool        viewportEvent( QEvent* e );

	signals:

		void				itemDoubleClicked( QGraphicsItem* item );

		void				viewResized( QSize s );

	public:

		//! Get this item view controller manager.
		Controller::Manager&	getControllerManager( ) { return 	_controllerManager; }

		//! Get an action with a given name from a registered controller.
		QAction*				getControllerAction( QString name );

		//! Get a widget with a given name from a registered controller (ex: a slide bar from the zoom controller).
		QWidget*				getControllerWidget( QString name );

	private:

		Controller::Manager		_controllerManager;
		//@}
		//---------------------------------------------------------------------

		
		/* Content Drag and Drop Management *///---------------------------------------
		//@{
	protected:

		virtual void	dragEnterEvent( QDragEnterEvent* e );

		virtual void	dragMoveEvent( QDragMoveEvent* e );

		virtual void	dropEvent( QDropEvent* e );

	signals:

		void			dataDropped( QString data );
		//@}
		//---------------------------------------------------------------------
	};
} // ::qan
//-----------------------------------------------------------------------------


#endif // canGraphView_h

