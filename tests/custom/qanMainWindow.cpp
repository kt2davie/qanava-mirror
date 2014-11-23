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
// \file	qanMainWindow.cpp
// \author	benoit@qanava.org
// \date	2014 10 19
//-----------------------------------------------------------------------------


// Qanava headers
#include "../../src/qanGraphView.h"
#include "./qanMainWindow.h"

// QT headers
#include <QHBoxLayout>
#include <QLabel>

using namespace qan;

CustomNodeItem::CustomNodeItem( qan::GraphScene& scene, CustomNode& node, QGraphicsItem* parent ) :
    qan::SimpleNodeItem( scene, node, parent, true, false ),
    _node( node )
{
    QGraphicsPixmapItem* pixmapItem = scene.addPixmap( QPixmap( node.getPixmapName( ) ) );
    pixmapItem->setParentItem( this );
    pixmapItem->setZValue( zValue( ) - 2. );

    // Draw a solid item background for shadow generation
    QGraphicsEllipseItem* pixmapBack = scene.addEllipse( pixmapItem->boundingRect( ) );
    pixmapBack->setParentItem( this );
    pixmapBack->setZValue( zValue( ) - 3. );
    setShapeItem( pixmapBack );

    QGraphicsEllipseItem* pixmapBorder = scene.addEllipse( pixmapItem->boundingRect( ) );
    pixmapBorder->setParentItem( this );
    pixmapBorder->setZValue( zValue( ) - 1. );
    pixmapBorder->setBrush( Qt::NoBrush );
    pixmapBorder->setPen( QPen( QColor( 50, 50, 50 ), 2.0 ) );

    _dimension = QPointF( pixmapBorder->boundingRect( ).width( ), pixmapBorder->boundingRect( ).height( ) );
    updateItem( );
}

void	CustomNodeItem::updateItem( )
{
    SimpleNodeItem::updateItem( );
    foreach ( Edge* edge, _node.getOutEdges( ) )
    {
        GraphItem* graphItem = getScene( ).getGraphItem( *edge );
        if ( graphItem != 0 )
            graphItem->updateItem( );
    }
    foreach ( Edge* edge, _node.getInEdges( ) )
    {
        GraphItem* graphItem = getScene( ).getGraphItem( *edge );
        if ( graphItem != 0 )
            graphItem->updateItem( );
    }
}

QPainterPath    CustomNodeItem::shape( ) const
{
    QPainterPath path;
    path.addEllipse( boundingRect( ) );
    return path;
}

qan::GraphItem*	CustomNodeItem::Factory::create( qan::GraphScene& scene, qan::Node& node, QGraphicsItem* parent )
{
    if ( QString( node.metaObject( )->className( ) ) == "CustomNode" )
    {
        CustomNode* customNode = static_cast< CustomNode* >( &node );
        return new CustomNodeItem( scene, *customNode, parent );
    }
    return 0;
}

//-----------------------------------------------------------------------------
MainWindow::MainWindow( ) : QMainWindow( 0 )
{
    QWidget* frame = new QWidget( this );
    setCentralWidget( frame );

    QHBoxLayout* hbox = new QHBoxLayout( frame );
	hbox->setMargin( 0 );

	// Generate a simple graph
	_graph = new Graph( );
    _graph->getM( ).addGraphItemFactory( new CustomNodeItem::Factory( ) );

    qan::GraphView* graphView = new qan::GraphView( frame, Qt::white );
	graphView->setGraph( *_graph );
    hbox->addWidget( graphView );

	// FIXME: v0.9.1 hack to hide the zoom ctrl slidebar
	QWidget* zoomSlider = graphView->getControllerWidget( "zoom_ctrl_zoom_slider" );
	if ( zoomSlider != 0 )
		zoomSlider->hide( );

    qan::Style* nodeStyle = _graph->getM( ).getStyleManager( ).addStyle( "Custom style", "qan::Node" );
    nodeStyle->addProperty( "No Background", QVariant( false ) );
    nodeStyle->addProperty( "Back Color", QVariant( QColor( 60, 100, 250 ) ) );
    nodeStyle->addProperty( "Border Color", QVariant( QColor( 0, 0, 0 ) ) );
	QStringList lineStyles; lineStyles << "Solid line" << "Dash line" << "Dot line" << "Dash Dot line" << "Dash Dot Dot line";
    nodeStyle->addProperty( "Border Style", QtVariantPropertyManager::enumTypeId(),  lineStyles );
    nodeStyle->addProperty( "Border Width", QVariant( 1.0 ) );
    nodeStyle->addProperty( "Font", QVariant( QFont( "Helvetica", 40, QFont::Bold ) ) );
    nodeStyle->addProperty( "Maximum Size", QVariant( QSizeF( 100., 25. ) ) );
    nodeStyle->addProperty( "Has Shadow", QVariant( true ) );
    nodeStyle->addProperty( "Shadow Color", QVariant( QColor( 100, 100, 100, 160 ) ) );
    nodeStyle->addProperty( "Shadow Offset", QVariant( QSizeF( 5., 5. ) ) );

    qan::Style* edgeStyle = _graph->getM( ).getStyleManager( ).addStyle( "Custom edge style", "qan::Edge" );
    edgeStyle->addProperty( "Line Color", QVariant( QColor( 50, 50, 50 ) ) );
    QStringList edgeStyles; edgeStyles << "Solid line" << "Dash line" << "Dot line" << "Dash Dot line" << "Dash Dot Dot line";
    QtVariantProperty* lineStyle = edgeStyle->addProperty( "Line Style", QtVariantPropertyManager::enumTypeId( ), edgeStyles );
    lineStyle->setValue( 2 );
    edgeStyle->addProperty( "Line Width", QVariant( 4.0 ) );
    edgeStyle->addProperty( "Arrow Size", QVariant( 6.0 ) );
    edgeStyle->addProperty( "Draw Bounding Rect", QVariant( false) );

    qan::Style* edgeStyle2 = _graph->getM( ).getStyleManager( ).addStyle( "Custom edge style2", "qan::Edge" );
    //edgeStyle2->addProperty( "Line Color", QVariant( QColor( 250, 10, 210 ) ) );
    edgeStyle2->addProperty( "Line Color", QVariant( QColor( 145, 5, 180 ) ) );
    QStringList edgeStyles2; edgeStyles2 << "Solid line" << "Dash line" << "Dot line" << "Dash Dot line" << "Dash Dot Dot line";
    QtVariantProperty* lineStyle2 = edgeStyle2->addProperty( "Line Style", QtVariantPropertyManager::enumTypeId( ), edgeStyles2 );
    lineStyle2->setValue( 1 );
    edgeStyle2->addProperty( "Line Width", QVariant( 3.0 ) );
    edgeStyle2->addProperty( "Arrow Size", QVariant( 6.0 ) );
    edgeStyle2->addProperty( "Draw Bounding Rect", QVariant( false) );


    {
        Node* q = new CustomNode( ":/images/Q.png" );
        _graph->insertNode( q, "Custom style");

        Node* a = new CustomNode( ":/images/a.png" );
        _graph->insertNode( a, "Custom style");

        Node* n = new CustomNode( ":/images/n.png" );
        _graph->insertNode( n, "Custom style");

        Node* a2 = new CustomNode( ":/images/a.png" );
        _graph->insertNode( a2, "Custom style");

        Node* v = new CustomNode( ":/images/v.png" );
        _graph->insertNode( v, "Custom style");

        Node* a3 = new CustomNode( ":/images/a.png" );
        _graph->insertNode( a3, "Custom style");

        Edge* qa = _graph->insertEdge( *q, *a, "Custom edge style" );
		Edge* an = _graph->insertEdge( *a, *n );
        Edge* na2 = _graph->insertEdge( *n, *a2, "Custom edge style2" );
        Edge* a2v = _graph->insertEdge( *a2, *v, "Custom edge style2" );
		Edge* va3 = _graph->insertEdge( *v, *a3 );
	}

    //_graph->getM( ).setSceneRect( QRectF( 0.0, 0., 1000., 1000. ) );
}
//-----------------------------------------------------------------------------


