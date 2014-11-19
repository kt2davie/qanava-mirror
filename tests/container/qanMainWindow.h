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
// \file	qanMainWindow.h
// \author	benoit@qanava.org
// \date	2005 November 11
//-----------------------------------------------------------------------------


#ifndef canMainWindow_h
#define canMainWindow_h


// Qanava headers
#include "../../src/qanGraph.h"
#include "../../src/qanGraphView.h"
#include "../../src/qanNodeGroup.h"
#include "ui_qanMainWindow.h"

// QT headers
#include <QMainWindow>
#include <QTreeView>
#include <QMenu>
#include <QTextEdit>
#include <QGraphicsLinearLayout>
#include <QCheckBox>
#include <QGraphicsItem>
#include <QGraphicsView>

	class PixmapWidget : public QGraphicsView
	{
		Q_OBJECT

	public:

		PixmapWidget( QPixmap pixmap );

		void			showContextMenu( QPoint position, QPointF scenePos );

	protected:
		
		QGraphicsPixmapItem*	_pixmapItem;

		QGraphicsScene*			_pixmapScene;

		virtual void	wheelEvent( QWheelEvent* e );

		virtual	bool	viewportEvent( QEvent* e );
	};

	class ContainerWidget : public QGraphicsWidget
	{
		Q_OBJECT

	public:

		ContainerWidget( QGraphicsScene& scene, QGraphicsView* qw, QGraphicsItem* parent );

	protected:

		QGraphicsScene&			_scene;
		QGraphicsLinearLayout*	_vLayout;
	};


//-----------------------------------------------------------------------------
//! 
/*!
	\nosubgrouping
*/
	class MainWindow : public QMainWindow, public Ui::MainWindow
	{
		Q_OBJECT

	public:

		MainWindow( QApplication* application, QWidget* parent = 0 );

		virtual ~MainWindow( ) { }

	private:

		MainWindow( const MainWindow& );

		MainWindow& operator=( const MainWindow& );

		QApplication*		_application;

		qan::Graph*			_graph;
};
//-----------------------------------------------------------------------------


#endif // qanMainWindow_h

