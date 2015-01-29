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
// \file	qanMainWindow.h
// \author	benoit@qanava.org
// \date	2014 10 19
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

	//! Models a group of nodes showing a relationnal graph of actors.
	/*!
		\nosubgrouping
	*/
	class RelationalGroup : public qan::NodeGroup
	{
		Q_OBJECT

        /*! \name Dynamic Layout Management *///-------------------------------
		//@{
	public:
		RelationalGroup( qan::GraphScene& scene, QString name );

    public:
        virtual void		addNode( qan::Node& node );

    protected slots:
        void				itemMoved( QPointF curPos, QPointF oldPos );
        //@}
		//---------------------------------------------------------------------
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

