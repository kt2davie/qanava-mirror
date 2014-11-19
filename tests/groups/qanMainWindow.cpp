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
#include "../../src/qanGrid.h"
#include "../../src/qanLayout.h"
#include "../../src/qanTreeLayout.h"
#include "./qanMainWindow.h"
#include "./ui/uiNodeGroupFilterWidget.h"


// QT headers
#include <QToolBar>
#include <QAction>
#include <QSplitter>
#include <QMessageBox>
#include <QTextEdit>
#include <QPushButton>
#include <QVariant>
#include <QCheckBox>
#include <QProgressBar>

using namespace qan;


/* Group Management *///-------------------------------------------------------
RelationalGroup::RelationalGroup( qan::GraphScene& scene, QString name ) : 
	NodeGroup( scene, name )
{ 
	setLayout( new qan::HierarchyTree( ) ); 	// Set an horizontal tree layout for position initialisation
	setAddAsChilds( false );	
	setAcceptDrops( true );
}
//-----------------------------------------------------------------------------

void	RelationalGroup::addNode( qan::Node& node )
{
	NodeGroup::addNode( node );
}

void	RelationalGroup::groupMoved( QPointF curPos, QPointF oldPos )
{
	NodeGroup::groupMoved( curPos, oldPos );
	QPointF delta =  curPos - oldPos;

	qan::Edge::Set edges;
	foreach ( qan::Node* node, getNodes( ) )
	{
		node->getGraphicsItem( )->moveBy( delta.x( ), delta.y( ) );
		node->setPosition( node->getPosition( ) + delta );
		edges.unite( node->getOutEdges( ).toSet( ) );
		edges.unite( node->getInEdges( ).toSet( ) );
	}
	// Force node group edges update
	foreach ( qan::Edge* edge, edges )
		edge->getGraphItem( )->updateItem( );
}

void	RelationalGroup::itemMoved( QPointF curPos, QPointF oldPos )
{
	NodeGroup::itemMoved( curPos, oldPos );

	// Force node group edges update
	qan::Edge::Set edges;
	foreach ( qan::Node* node, getNodes( ) )
	{
		edges.unite( node->getOutEdges( ).toSet( ) );
		edges.unite( node->getInEdges( ).toSet( ) );
	}
	foreach ( qan::Edge* edge, edges )
		edge->getGraphItem( )->updateItem( );
}

//-----------------------------------------------------------------------------
MainWindow::MainWindow( QApplication* application, QWidget* parent ) : 
	QMainWindow( parent ),
	_application( application )
{
	setupUi( this );
    QHBoxLayout* hbox = new QHBoxLayout( _frame );
	hbox->setMargin( 0 );

	// Generate a simple graph
	_graph = new Graph( );

	qan::GraphView* graphView = new qan::GraphView( _frame, Qt::white );
	graphView->setGraph( *_graph );
	new qan::GridCheckBoardItem( graphView, palette( ).color( QPalette::Base ), palette( ).color( QPalette::AlternateBase ) );
	hbox->addWidget( graphView );

	// FIXME: v0.9.1 hack to hide the zoom ctrl slidebar
	QWidget* zoomSlider = graphView->getControllerWidget( "zoom_ctrl_zoom_slider" );
	if ( zoomSlider != 0 )
		zoomSlider->hide( );

	qan::NodeGroup* treeGroup = new qan::NodeGroup( _graph->getM( ), "Tree Group" );
	treeGroup->setLayout( new qan::HierarchyTree( ) );
	treeGroup->setAcceptDrops( true );
	_graph->getM( ).addNodeGroup( *treeGroup );
	_graph->getM( ).addDropTarget( treeGroup );
	Node& ha = *_graph->insertNode( "Node A", "default node");
	Node& ha1 = *_graph->insertNode( "Node A1", "default node");
	Node& ha2 = *_graph->insertNode( "Node A2", "default node");
	Node& ha11 = *_graph->insertNode( "Node A11", "default node");
	Node& ha21 = *_graph->insertNode( "Node A21", "default node");
	Edge* haha1 = _graph->insertEdge( ha, ha1 );
	Edge* ha1ha11 = _graph->insertEdge( ha1, ha11 );
	Edge* haha2 = _graph->insertEdge( ha, ha2 );
	Edge* h2ha21 = _graph->insertEdge( ha2, ha21 );
	treeGroup->addNode( ha );
	treeGroup->addNode( ha1 );
	treeGroup->addNode( ha2 );
	treeGroup->addNode( ha11 );
	treeGroup->addNode( ha21 );

	Node& hb = *_graph->insertNode( "Node B", "default node");
	Node& hb1 = *_graph->insertNode( "Node B1", "default node");
	Edge* hbhb1 = _graph->insertEdge( hb, hb1 );

	qan::Style* actorStyle = _graph->getM( ).getStyleManager( ).addStyle( "default actor node", "qan::Node" );
	actorStyle->addProperty( "No Background", QVariant( false ) );
	actorStyle->addProperty( "Back Color", QVariant( QColor( 130, 120, 250 ) ) );
	actorStyle->addProperty( "Border Color", QVariant( QColor( 0, 0, 0 ) ) );
	QStringList lineStyles; lineStyles << "Solid line" << "Dash line" << "Dot line" << "Dash Dot line" << "Dash Dot Dot line";
	actorStyle->addProperty( "Border Style", QtVariantPropertyManager::enumTypeId(),  lineStyles );
	actorStyle->addProperty( "Border Width", QVariant( 1.0 ) );
	actorStyle->addProperty( "Font", QVariant( QFont( ) ) );
	actorStyle->addProperty( "Maximum Size", QVariant( QSizeF( 100., 25. ) ) );
	actorStyle->addProperty( "Has Shadow", QVariant( true ) );
	actorStyle->addProperty( "Shadow Color", QVariant( QColor( 50, 50, 50 ) ) );
	actorStyle->addProperty( "Shadow Offset", QVariant( QSizeF( 2., 2. ) ) );

	qan::NodeGroup* relationalGroup = new RelationalGroup( _graph->getM( ), "Undirected Layout Group" );
	_graph->getM( ).addNodeGroup( *relationalGroup );
	_graph->getM( ).addDropTarget( relationalGroup );
	Node* ra = new qan::Node( "Node A" );
	ra->getProperties( ).addProperty( "Type", "Actor" );
	_graph->insertNode( ra, "default actor node");
	Node* rb = new qan::Node( "Node B" );
	rb->getProperties( ).addProperty( "Type", "Actor" );
	_graph->insertNode( rb, "default actor node");
	Node* rc = new qan::Node( "Node C" );
	rc->getProperties( ).addProperty( "Type", "Actor" );
	_graph->insertNode( rc, "default actor node");
	Node* rd = new qan::Node( "Node D" );
	rd->getProperties( ).addProperty( "Type", "Actor" );
	_graph->insertNode( rd, "default actor node");
	Node* re = new qan::Node( "Node E" );
	re->getProperties( ).addProperty( "Type", "Actor" );
	_graph->insertNode( re, "default actor node");
	Node* rf = new qan::Node( "Node F" );
	rf->getProperties( ).addProperty( "Type", "Actor" );
	_graph->insertNode( rf, "default actor node");
	Edge* rarb = _graph->insertEdge( *ra, *rb );
	Edge* rarc = _graph->insertEdge( *ra, *rc );
	Edge* rard = _graph->insertEdge( *ra, *rd );
	Edge* rare = _graph->insertEdge( *ra, *re );
	Edge* rerf = _graph->insertEdge( *re, *rf );
	Edge* rarf = _graph->insertEdge( *ra, *rf );
	relationalGroup->addNode( *ra );
	relationalGroup->addNode( *rb );
	relationalGroup->addNode( *rc );
	relationalGroup->addNode( *rd );
	relationalGroup->addNode( *re );
	relationalGroup->addNode( *rf );

	qan::Layout* relationalLayout = new qan::UndirectedGraph( );
	relationalGroup->setLayout( relationalLayout );

	// Add a style browser widget to dynamically set node's style
	ui::NodeGroupFilterWidget* groupFilterWidget = new qan::ui::NodeGroupFilterWidget( *graphView, _graph->getM( ) );
	groupFilterWidget->setParent( graphView );
	_graph->getM( ).addItem( groupFilterWidget );

	_graph->getM( ).setSceneRect( QRectF( 0.0, 0., 1000., 1000. ) );
}
//-----------------------------------------------------------------------------


