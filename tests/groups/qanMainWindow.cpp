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
// \file	qanMainWindow.cpp
// \author	benoit@qanava.org
// \date	2014 10 19
//-----------------------------------------------------------------------------


// Qanava headers
#include "../../src/qanGraphView.h"
#include "../../src/qanGrid.h"
#include "../../src/qanLayout.h"
#include "../../src/qanTreeLayout.h"
#include "../../src/qanGraphicsResizer.h"
#include "../../src/ui/uiNodeGroupFilterWidget.h"
#include "../../src/ui/uiNodeGroupFilterWidget.h"
#include "./qanMainWindow.h"

// QT headers
#include <QToolBar>
#include <QAction>
#include <QTextEdit>
#include <QVariant>

using namespace qan;


/* Dynamic Group Management *///-----------------------------------------------
RelationalGroup::RelationalGroup( qan::GraphScene& scene, QString name ) :
    NodeGroup( scene, name )
{ 
    setGraphLayout( new qan::UndirectedGraph( ) );
}

void	RelationalGroup::addNode( qan::Node& node )
{
	NodeGroup::addNode( node );
    qan::NodeItem* nodeItem = ( qan::NodeItem* )node.getGraphItem( );
    if ( nodeItem != 0 )
        nodeItem->setMovable( true );
}

void	RelationalGroup::itemMoved( QPointF curPos, QPointF oldPos )
{
	NodeGroup::itemMoved( curPos, oldPos );

    qan::NodeItem* movedNode = static_cast< qan::NodeItem* >( sender( ) );
    Q_ASSERT( movedNode != 0 );

    if ( _graphLayout != 0 )
    {
        qan::Node::Set rootNodes; getRootNodes( rootNodes );
        qan::Node::List rootNodesList = rootNodes.toList( );
        QRectF sceneSceneRect( _scene.sceneRect( ).translated( -pos( ) ) ); // Translate scene rect by group position, since layout is done in group local CCS
        _graphLayout->layout( rootNodesList, _nodes, sceneSceneRect, &movedNode->getNode( ), 0 );

        qan::Edge::Set edges;
        foreach ( qan::Node* node, getNodes( ) )
        {
            if ( node->getGraphItem( ) != movedNode )
                node->getGraphicsItem( )->setPos( node->getPosition( ) );
            edges.unite( node->getOutEdges( ).toSet( ) );
        }
        foreach ( qan::Edge* edge, edges )
            edge->getGraphItem( )->updateItem( );

        // Update this group bounding rect
        prepareGeometryChange( );

        // Compute laid out nodes Br
        QRectF layoutBr = movedNode->getGraphicsItem( )->sceneBoundingRect( );
        foreach ( qan::Node* node, getNodes( ) )
            layoutBr = layoutBr.united( node->getGraphicsItem( )->sceneBoundingRect( ) );

        // Translate the group to follow item move
        QPointF delta = curPos - oldPos;
        movedNode->getGraphicsItem( )->moveBy( -delta.x( ), -delta.y( ) );
        QRectF groupGeometry = QRectF( pos( ), _br.size( ) );
        groupGeometry.translate( delta );
        setGeometry( groupGeometry );

        // Fix the group top left position according to the new layout bounding rect
        QPointF groupDelta = layoutBr.topLeft( ) - groupGeometry.topLeft( );
        foreach ( qan::Node* node, getNodes( ) )
            node->getGraphicsItem( )->moveBy( -groupDelta.x( ), -groupDelta.y( ) );
        //moveBy( groupDelta );
        QRectF groupLayoutGeometry( pos( ) + groupDelta, layoutBr.size( ) );
        if ( groupLayoutGeometry.isValid( ) && !groupLayoutGeometry.isEmpty( ) )
            setGeometry( groupLayoutGeometry );
    }

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

    // Test node to be dragged in groups
    Node& hb = *_graph->insertNode( "Node B", "default node");
    Node& hb1 = *_graph->insertNode( "Node B1", "default node");
    Node& hb11 = *_graph->insertNode( "Node B11", "default node");
    Edge* hbhb1 = _graph->insertEdge( hb, hb1 ); Q_UNUSED( hbhb1 );
    Edge* hb1hb11 = _graph->insertEdge( hb1, hb11 ); Q_UNUSED( hb1hb11 );

    {/*
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
        Edge* haha1 = _graph->insertEdge( ha, ha1 ); Q_UNUSED( haha1 );
        Edge* ha1ha11 = _graph->insertEdge( ha1, ha11 ); Q_UNUSED( ha1ha11 );
        Edge* haha2 = _graph->insertEdge( ha, ha2 ); Q_UNUSED( haha2 );
        Edge* h2ha21 = _graph->insertEdge( ha2, ha21 ); Q_UNUSED( h2ha21 );
        treeGroup->addNode( ha );
        treeGroup->addNode( ha1 );
        treeGroup->addNode( ha2 );
        treeGroup->addNode( ha11 );
        treeGroup->addNode( ha21 );

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
        Edge* rarb = _graph->insertEdge( *ra, *rb ); Q_UNUSED( rarb );
        Edge* rarc = _graph->insertEdge( *ra, *rc ); Q_UNUSED( rarc );
        Edge* rard = _graph->insertEdge( *ra, *rd ); Q_UNUSED( rard );
        Edge* rare = _graph->insertEdge( *ra, *re ); Q_UNUSED( rare );
        Edge* rerf = _graph->insertEdge( *re, *rf ); Q_UNUSED( rerf );
        Edge* rarf = _graph->insertEdge( *ra, *rf ); Q_UNUSED( rarf );
        relationalGroup->addNode( *ra );
        relationalGroup->addNode( *rb );
        relationalGroup->addNode( *rc );
        relationalGroup->addNode( *rd );
        relationalGroup->addNode( *re );
        relationalGroup->addNode( *rf );

        qan::Layout* relationalLayout = new qan::UndirectedGraph( );
        relationalGroup->setLayout( relationalLayout );*/
    }

    // Test graphics layout with node groups
    {
        // Add a moveable group with a qanava hierarchy tree layout
/*        qan::NodeGroup* hierarchyGroup = new qan::NodeGroup( _graph->getM( ), "Hierarchy 1" );
        hierarchyGroup->setMovable( true );
        hierarchyGroup->setGraphLayout( new qan::HierarchyTree( ) );
        _graph->getM( ).addNodeGroup( *hierarchyGroup );

        hierarchyGroup->addNode( hb );
        hierarchyGroup->addNode( hb1 );
        hierarchyGroup->addNode( hb11 );


        Node& n1 = *_graph->insertNode( "Node 1", "default node" );
        Node& n2 = *_graph->insertNode( "Node 2", "default node" );
        Node& n3 = *_graph->insertNode( "Node 3", "default node" );
        Node& n4 = *_graph->insertNode( "Node 4", "default node" );

        // Add a group with a graphics layout in a graphics widget that has an horizontal graphics layout
        QGraphicsWidget* sceneWidget = new QGraphicsWidget( 0 );
        QGraphicsLinearLayout* groupLayout = new QGraphicsLinearLayout( sceneWidget );
        sceneWidget->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding, QSizePolicy::DefaultType );
        sceneWidget->setPos( 50, 50 );
        sceneWidget->resize( 400, 300 );
        sceneWidget->setLayout( groupLayout );
        _graph->getM( ).addItem( sceneWidget );

        qan::NodeGroup* vLinearGroup = new qan::NodeGroup( _graph->getM( ), "vLinear 1", sceneWidget );
        vLinearGroup->setMovable( false );
        vLinearGroup->setLayout( new QGraphicsLinearLayout( Qt::Vertical ) );
        _graph->getM( ).addNodeGroup( *vLinearGroup );
        vLinearGroup->addNode( n4 );
        vLinearGroup->addNode( n3 );

        groupLayout->addItem( reinterpret_cast< qan::NodeItem *>( n1.getGraphItem( ) ) );
        groupLayout->addItem( reinterpret_cast< qan::NodeItem *>( n2.getGraphItem( ) ) );
        groupLayout->addItem( vLinearGroup );
        groupLayout->addItem( reinterpret_cast< qan::NodeItem *>( n3.getGraphItem( ) ) );
*/

        // Resizable node group test
        Node& r1 = *_graph->insertNode( "R1", "default node" );
        Node& r2 = *_graph->insertNode( "R2", "default node" );
        Node& r3 = *_graph->insertNode( "R3", "default node" );
        Node& r4 = *_graph->insertNode( "R4", "default node" ); Q_UNUSED( r4 );

        qan::NodeGroup* resizableGroup = new qan::NodeGroup( _graph->getM( ), "Resizable Group" );
        resizableGroup->setAcceptDrops( true );
        resizableGroup->setLayout( new QGraphicsLinearLayout( Qt::Vertical ) );
        _graph->getM( ).addNodeGroup( *resizableGroup );
        resizableGroup->addNode( r1 );
        resizableGroup->addNode( r2 );
        resizableGroup->addNode( r3 );

        qan::GraphicsController* controller = new qan::GraphicsController( resizableGroup, resizableGroup );
        qan::GraphicsResizer* resizer = new qan::GraphicsResizer( controller, resizableGroup );     Q_UNUSED( resizer );
        //new qan::GraphicsResizerDecoration( resizableGroup, resizableGroup );

        // Group with a hierarchy tree
        qan::NodeGroup* hierarchyGroup = new qan::NodeGroup( _graph->getM( ), "Group 2" );
        hierarchyGroup->setGraphLayout( new qan::HierarchyTree( ) );
        hierarchyGroup->setAcceptDrops( true );
        hierarchyGroup->setMovable( true );
        _graph->getM( ).addNodeGroup( *hierarchyGroup );
        _graph->getM( ).addDropTarget( hierarchyGroup );

        hierarchyGroup->addNode( hb );
        hierarchyGroup->addNode( hb1 );
        hierarchyGroup->addNode( hb11 );

        // Group with a dynamic undirected graph layout
        qan::NodeGroup* graphGroup = new RelationalGroup( _graph->getM( ), "Group 2" );
        graphGroup->setAcceptDrops( true );
        graphGroup->setMovable( true );
        _graph->getM( ).addNodeGroup( *graphGroup );
        _graph->getM( ).addDropTarget( graphGroup );

        Node* ra = new qan::Node( "Node A" );
        _graph->insertNode( ra, "default actor node");
        Node* rb = new qan::Node( "Node B" );
        _graph->insertNode( rb, "default actor node");
        Node* rc = new qan::Node( "Node C" );
        _graph->insertNode( rc, "default actor node");
        Node* rd = new qan::Node( "Node D" );
        _graph->insertNode( rd, "default actor node");
        Node* re = new qan::Node( "Node E" );
        _graph->insertNode( re, "default actor node");
        Node* rf = new qan::Node( "Node F" );
        _graph->insertNode( rf, "default actor node");
        Edge* rarb = _graph->insertEdge( *ra, *rb ); Q_UNUSED( rarb );
        Edge* rarc = _graph->insertEdge( *ra, *rc ); Q_UNUSED( rarc );
        Edge* rard = _graph->insertEdge( *ra, *rd ); Q_UNUSED( rard );
        Edge* rare = _graph->insertEdge( *ra, *re ); Q_UNUSED( rare );
        Edge* rerf = _graph->insertEdge( *re, *rf ); Q_UNUSED( rerf );
        Edge* rarf = _graph->insertEdge( *ra, *rf ); Q_UNUSED( rarf );
        graphGroup->addNode( *ra );
        graphGroup->addNode( *rb );
        graphGroup->addNode( *rc );
        graphGroup->addNode( *rd );
        graphGroup->addNode( *re );
        graphGroup->addNode( *rf );

/*        qan::NodeGroup* group3 = new qan::NodeGroup( _graph->getM( ), "Group 3" );
        group3->setLayout( new QGraphicsLinearLayout( Qt::Vertical ) );
        group3->setAcceptDrops( true );
        group3->setMovable( false );
        _graph->getM( ).addNodeGroup( *group3 );
        _graph->getM( ).addDropTarget( group3 );
        group3->addNode( n1 );
        group3->addNode( n2 );
        group3->addNode( n3 );
        group3->addNode( n4 );*/

        //groupLayout->addItem( group1 );
        //groupLayout->addItem( group2 );
        //groupLayout->addItem( group3 );
    }

	// Add a style browser widget to dynamically set node's style
	ui::NodeGroupFilterWidget* groupFilterWidget = new qan::ui::NodeGroupFilterWidget( *graphView, _graph->getM( ) );
	groupFilterWidget->setParent( graphView );
	_graph->getM( ).addItem( groupFilterWidget );
}
//-----------------------------------------------------------------------------


