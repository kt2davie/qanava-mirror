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
// \file	qanGraphScene.cpp
// \author	benoit@qanava.org
// \date	2005 November 22
//-----------------------------------------------------------------------------


// Qanava headers
#include "./qanGraph.h"
#include "./qanEdge.h"
#include "./qanGraphScene.h"
#include "./qanGrid.h"
#include "./qanLayout.h"
#include "./qanNodeItem.h"


// QT headers
#include <QVBoxLayout>
#include <QTimer>
#include <QScrollBar>


//-----------------------------------------------------------------------------
namespace qan { // ::qan

/* GraphScene Object Management *///-------------------------------------------
/*!
	\param	backgroundColor	Graph canvas background color.
 */
GraphScene::GraphScene( StyleManager& styleManager, QWidget* parent, QColor backgroundColor, QSize size ) :
	QGraphicsScene( parent ),
	_styleManager( styleManager )
{ 
    Q_UNUSED( backgroundColor ); Q_UNUSED( size );
	addGraphItemFactory( new NodeRectItem::Factory( ) );
	addGraphItemFactory( new EdgeItem::Factory( ) );
	addGraphItemFactory( new HEdgeItem::Factory( ) );
}

GraphScene::~GraphScene( )
{
	// FIXME v0.4.0 Destroy GraphItem factories

	foreach ( qan::NodeGroup* nodeGroup, _nodeGroups )
		delete nodeGroup;
}
//-----------------------------------------------------------------------------



/* Scene Management *///-------------------------------------------------------
/*! Styles registered in the style manager are not cleared, and can be reused when this
    model will receive new edges and noeds.
 */
void	GraphScene::clear( )
{
    // Clear all graphics items in the graphic view
	foreach ( GraphItem* item, _edgeGraphItemMap.values( ) )
	{
		removeItem( item->getGraphicsItem( ) );
		delete item;
	}
	foreach ( GraphItem* item, _nodeGraphItemMap.values( ) )
	{
		removeItem( item->getGraphicsItem( ) );
		delete item;
	}

	// Clear all mappings
	_nodeGraphItemMap.clear( );
	_edgeGraphItemMap.clear( );
}

// FIXME v0.4.0 hack from previous versions of Qanava, the layout and position system should be redesigned in v0.5.0
void	GraphScene::updatePositions( Node* except )
{
    Q_UNUSED( except );
	// Update all nodes positions	// FIXME v0.4.0: Would be more efficient to iterate over graph.getNodes( )
	foreach ( GraphItem* nodeGraphItem, _nodeGraphItemMap.values( ) )
	{
		const Node* node = _nodeGraphItemMap.key( nodeGraphItem, 0 );
		if ( nodeGraphItem != 0 && node != 0 )
			nodeGraphItem->getGraphicsItem( )->setPos( node->getPosition( ).x( ), node->getPosition( ).y( ) );
	}
}
//-----------------------------------------------------------------------------


/* Graph Topology Management *///----------------------------------------------
void	GraphScene::edgeInserted( qan::Edge& edge )
{
	insertEdgeGraphItem( edge );

	foreach ( qan::NodeGroup* nodeGroup, _nodeGroups )
		if ( nodeGroup->hasNode( edge.getSrc( ) ) || nodeGroup->hasNode( edge.getDst( ) ) )
			nodeGroup->addEdge( edge );
}

void	GraphScene::edgeModified( qan::Edge& edge )
{
	GraphItem* edgeItem = getGraphItem( edge );
	if ( edgeItem != 0 )
		edgeItem->updateItem( );
}

void	GraphScene::edgeRemoved( qan::Edge& edge )
{
	GraphItem* edgeItem = getGraphItem( edge );
	if (  edgeItem != 0 )
		delete edgeItem;
}

void	GraphScene::nodeInserted( qan::Node& node )
{
	insertNodeGraphItem( node );
}

void	GraphScene::nodeRemoved( qan::Node& node )
{
	GraphItem* nodeItem = getGraphItem( node );
	if (  nodeItem != 0 )
	{
		_nodeGraphItemMap.remove( &node );
		delete nodeItem;
	}

	foreach ( qan::NodeGroup* nodeGroup, _nodeGroups )
		if ( nodeGroup->hasNode( node ) )
			nodeGroup->removeNode( node );
}

void	GraphScene::nodeChanged( qan::Node& node )
{
	GraphItem* graphItem = getGraphItem( node );
	if (  graphItem != 0 )
		graphItem->updateItem( );
}

void	GraphScene::insertNodesGraphItems( Node::List& rootNodes )
{
	foreach ( Node* node, rootNodes )
		insertNodeGraphItem( *node );
}

void	GraphScene::insertNodeGraphItem( qan::Node& node )
{
	// Don't insert a node that is already existing
	if ( getGraphItem( node ) )
		return;
	createGraphItem( node );

	// Insert node edges
	Edge::Set edges = Edge::Set::fromList( node.getInEdges( ) );
	edges.unite( Edge::Set::fromList( node.getOutEdges( ) ) );
	foreach ( Edge* edge, edges )
		insertEdgeGraphItem( *edge );

	// Recurse in sub nodes
	foreach ( Edge* edge, node.getOutEdges( ) )
	{
		Node::Set dsts; dsts.insert( &edge->getDst( ) );
		if ( edge->type( ) == Edge::HYPER )
		{
			dsts.unite( ( ( HEdge* )( edge ) )->getHDst() );
			dsts.unite( ( ( HEdge* )( edge ) )->getHSrc() );
		}
		foreach ( Node* n, dsts )
			insertNodeGraphItem( *n );
	}
}

void	GraphScene::insertEdgeGraphItem( qan::Edge& edge )
{
	if ( getGraphItem( edge ) ) // Do not insert an already existing edge
		return;
	createGraphItem( edge );
}
//-----------------------------------------------------------------------------


/* Node Group Management *///--------------------------------------------------
void	GraphScene::addNodeGroup( qan::NodeGroup& nodeGroup )
{
	_nodeGroups.append( &nodeGroup );
	addItem( &nodeGroup );
    if ( nodeGroup.getAcceptDrops( ) )
        addDropTarget( &nodeGroup );
	emit nodeGroupAdded( &nodeGroup );
}

void	GraphScene::removeNodeGroup( qan::NodeGroup& nodeGroup )
{
	_nodeGroups.removeAll( &nodeGroup );
	removeItem( &nodeGroup );
    _dropTargets.removeAll( &nodeGroup );   // Eventually, remove the given nodegroup from drop targets list
	emit nodeGroupRemoved( &nodeGroup );
}
//-----------------------------------------------------------------------------


/* Graph Item Management *///--------------------------------------------------
/*!	\return	0 if there is no graph item corresponding to the given node.
 */
GraphItem*	GraphScene::getGraphItem( qan::Node& node ) const
{
	return _nodeGraphItemMap.value( &node, 0 );
}

/*!	\return	0 if there is no graph item corresponding to a given edge.
 */
GraphItem*	GraphScene::getGraphItem( qan::Edge& edge ) const
{
	return _edgeGraphItemMap.value( &edge, 0 );
}

/*!	\return	0 if there is no node at the given position, or a pointer to the most top node.
 */
Node*	GraphScene::getNodeAt( QPointF scenePos )
{
    Q_UNUSED( scenePos );
//	QList< QGraphicsItem* > qItems = items( scenePos );
//	foreach ( QGraphicsItem* qItem, qItems )
//	{
		// FIXME v0.4.0
//		qan::Node* node = getGraphicItemNode( qItem );
//		if ( node != 0 )
//			return node;
//	}
	return 0;
}

void	GraphScene::createGraphItem( Node& node )
{
	// Get a graph item factory for the node
	GraphItem::Factory::List factories = getGraphItemFactories( node.metaObject( )->className( ) );
	foreach ( GraphItem::Factory* factory, factories )
	{
		GraphItem* graphItem = factory->create( *this, node, 0 );
		if ( graphItem != 0 )
		{
			addItem( graphItem->getGraphicsItem( ) );	// Hack for QT 5.3
			_nodeGraphItemMap.insert( &node, graphItem );
			node.setGraphicsItem( graphItem->getGraphicsItem( ) );
			node.setGraphItem( graphItem  );
			break;
		}
	}
}

void	GraphScene::createGraphItem( Edge& edge )
{
	// Get a graph item factory for the node
	GraphItem::Factory::List factories = getGraphItemFactories( edge.metaObject( )->className( ) );
	foreach ( GraphItem::Factory* factory, factories )
	{
		GraphItem* graphItem = factory->create( *this, edge, 0 );
		if ( graphItem != 0 )
		{
			addItem( graphItem->getGraphicsItem( ) );	// Hack for QT 5.3
			_edgeGraphItemMap.insert( &edge, graphItem );
			edge.setGraphicsItem( graphItem->getGraphicsItem( ) );
			edge.setGraphItem( graphItem );
			break;
		}
	}
}
//-----------------------------------------------------------------------------


/* Node Selection Management *///----------------------------------------------
void	GraphScene::mouseDoubleClickEvent( QGraphicsSceneMouseEvent* mouseEvent )
{
	qan::Node* node = getNodeAt( mouseEvent->scenePos( ) );
	if ( node != 0 )
		emit nodeDoubleClicked( node, mouseEvent->scenePos( ), mouseEvent->screenPos( ) );

	// Activate label edition when a qan::LabelEditorItem is double clicked in scene
	QGraphicsItem* item = itemAt( mouseEvent->scenePos( ), QTransform( ) );
	if ( item != 0 && item->type( ) == qan::LabelEditorItem::Type )
	{
		QGraphicsTextItem* ti = (QGraphicsTextItem* )item;
		if ( ti->textInteractionFlags( ) == Qt::TextEditorInteraction )
		{
			ti->setTextInteractionFlags( Qt::NoTextInteraction );
		}
		else
		{
			ti->setTextInteractionFlags( Qt::TextEditorInteraction );
			setFocus( Qt::MouseFocusReason );
		}
	}
	QGraphicsScene::mouseDoubleClickEvent( mouseEvent );
}

void	GraphScene::mouseReleaseEvent( QGraphicsSceneMouseEvent* mouseEvent )
{
	qan::Node* node = getNodeAt( mouseEvent->scenePos( ) );
	if ( node != 0 )
		emit nodeClicked( node, mouseEvent->button( ), mouseEvent->scenePos( ), mouseEvent->screenPos( ) );
	QGraphicsScene::mouseReleaseEvent( mouseEvent );
}
//-----------------------------------------------------------------------------


/* Graph Item Factory Management *///------------------------------------------
GraphItem::Factory*	GraphScene::getGraphItemFactory( QString className )
{
	// FIXME v0.4.0+ Should provide a _faster_ access to factories, with an uptodate classname to factory map.
	foreach ( GraphItem::Factory* factory, _graphItemFactories )
		if ( factory->getTargetClassName( ) == className )
			return factory;
	return 0;
}

GraphItem::Factory::List	GraphScene::getGraphItemFactories( QString className )
{
	GraphItem::Factory::List factories;
	foreach ( GraphItem::Factory* factory, _graphItemFactories )
		if ( factory->getTargetClassName( ) == className )
			factories.append( factory );
	return factories;
}
//-----------------------------------------------------------------------------


} // ::qan
//-----------------------------------------------------------------------------


