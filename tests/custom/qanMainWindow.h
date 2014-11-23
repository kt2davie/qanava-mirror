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
// \date	2014 11 22
//-----------------------------------------------------------------------------


#ifndef canMainWindow_h
#define canMainWindow_h


// Qanava headers
#include "../../src/qanGraph.h"
#include "../../src/qanGraphView.h"
#include "../../src/qanNodeGroup.h"

// QT headers
#include <QMainWindow>


class CustomNode : public qan::Node
{
    Q_OBJECT

public:
    CustomNode( QString pixmapName ) : qan::Node( "" ), _pixmapName( pixmapName ) { }
    QString     getPixmapName( ) { return _pixmapName; }

private:
    QString     _pixmapName;
};

class CustomNodeItem : public qan::SimpleNodeItem
{
    Q_OBJECT

public:

    CustomNodeItem( qan::GraphScene& scene, CustomNode& node, QGraphicsItem* parent );
    virtual ~CustomNodeItem( ) { }

protected:
    CustomNode&	_node;

public slots:
    virtual void				updateItem( );
    virtual QPainterPath		shape( ) const;

public:
    class Factory : public qan::GraphItem::Factory
    {
    public:
        Factory( ) : qan::GraphItem::Factory( ) { }
        virtual	qan::GraphItem*	create( qan::GraphScene& scene, qan::Node& node, QGraphicsItem* parent );
        virtual	QString			getTargetClassName( ) { return QString( "CustomNode" ); }
    };
};


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    MainWindow( );
    virtual ~MainWindow( ) { }

private:
    Q_DISABLE_COPY( MainWindow );

    qan::Graph*			_graph;
};


#endif // qanMainWindow_h

