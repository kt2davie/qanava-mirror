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
// \file	qanNodeGroup.cpp
// \author	benoit@qanava.org
// \date	2014 april 19
//-----------------------------------------------------------------------------


// Qanava headers
#include "./qanGraph.h"
#include "./qanGrid.h"
#include "./qanLayout.h"
#include "./qanTreeLayout.h"
#include "./qanNodeGroup.h"

// QT headers
#include <QVBoxLayout>
#include <QTimer>
#include <QScrollBar>
#include <QTextDocument>


namespace qan { // ::qan


/* NodeGroup Object Management *///--------------------------------------------
NodeGroup::NodeGroup( qan::GraphScene& scene, QString name, QGraphicsItem* parent ) : 
	QObject( ), QGraphicsItem( parent ),
	_scene( scene ),
	_layout( 0 ),
	_properties( "properties" ),
	_name( name ),
	_nameItem( 0 ),
	_acceptDrops( false ),
	_dragOver( false ),
	_br( QRectF( ) ),
	_background( 0 ),
	_mousePressed( false ),
	_addAsChilds( true )
{
	connect( &scene, SIGNAL( itemDragMove( qan::NodeItem*, QGraphicsItem* ) ), this, SLOT( itemDragMove( qan::NodeItem*, QGraphicsItem* ) ) );
	connect( &scene, SIGNAL( itemDragLeave( qan::NodeItem*, QGraphicsItem* ) ), this, SLOT( itemDragLeave( qan::NodeItem*, QGraphicsItem* ) ) );
	connect( &scene, SIGNAL( itemDropped( qan::NodeItem*, QGraphicsItem* ) ), this, SLOT( itemDropped( qan::NodeItem*, QGraphicsItem* ) ) );

	_nameItem = new LabelEditorItem( name, "<< Group name >>", this );
	_nameItem->setPos( QPointF( 0., -_nameItem->boundingRect( ).height( ) ) );
	connect( _nameItem, SIGNAL( textModified( ) ), this, SLOT( nameTextModified( ) ) );

	_background = scene.addRect( boundingRect( ).adjusted( -1, -1, 1, 1 ), QPen( QColor( 190, 190, 190, 75 ) ), QBrush( QColor( 190, 190, 190, 75 ) ) );
	_background->setParentItem( this );
}
//-----------------------------------------------------------------------------


/* Group Management *///-------------------------------------------------------
void	NodeGroup::setLayout( qan::Layout* layout ) { _layout = layout; }

void	NodeGroup::setVisible( bool v )
{
	QGraphicsItem::setVisible( v );

	if ( !_addAsChilds )	// If nodes are child items of this group, graphics item will take care of hidding/showing thems more efficiently
		foreach ( qan::Node* node, _nodes ) // Hide nodes
		{
			qan::GraphItem* nodeItem = _scene.getGraphItem( *node );
			QGraphicsItem* nodeGraphicsItem = nodeItem->getGraphicsItem( );
			if ( nodeGraphicsItem != 0 )
				nodeGraphicsItem->setVisible( v );
		}

	foreach ( qan::Node* node, _nodes ) 		// Hide nodes edges graphics items
	{
		qan::Edge::Set edges( node->getInEdges( ).toSet( ) ); 
		edges.unite( node->getOutEdges( ).toSet( ) );
		foreach ( qan::Edge* edge, edges )
			edge->getGraphicsItem( )->setVisible( v );
	}
}

void	NodeGroup::layout( )
{
	if ( _layout != 0 )
	{
		qan::Node::Set rootNodes; getRootNodes( rootNodes );
        qan::Node::List rootNodesList = rootNodes.toList( );
        _layout->layout( rootNodesList, _nodes, _scene.sceneRect( ), 0, 0 );
	}

	// Update laid out nodes
	foreach ( qan::Node* node, getNodes( ) )
	{
		QGraphicsItem* nodeItem = node->getGraphicsItem( );
		if ( nodeItem != 0 )
			nodeItem->setPos( node->getPosition( ) );
	}

	// Force edge item update
	qan::Edge::Set edges;
	foreach ( qan::Node* node, getNodes( ) )
		edges.unite( node->getOutEdges( ).toSet( ) );
	foreach ( qan::Edge* edge, edges )
		edge->getGraphItem( )->updateItem( );

	updateBoundingRect( );
}
//-----------------------------------------------------------------------------


/* Group Name Management *///--------------------------------------------------
void	NodeGroup::nameTextModified( )
{
	_name = _nameItem->toPlainText( );
	emit nameChanged( this, _name );
}
//-----------------------------------------------------------------------------


/* Graphics Item Management *///-----------------------------------------------
QRectF	NodeGroup::boundingRect( ) const
{
	if ( _br.isEmpty( ) )
		return QRectF( QPointF( 0., 0. ), getMinimumSize( ) );
	return _br;
}

QRectF	NodeGroup::updateBoundingRect( )
{
	// Update this group bounding rect
	prepareGeometryChange( );

	QRectF sceneBr = getNodesSceneBoundingRect( );
	QSizeF minSize = getMinimumSize( );
	if ( sceneBr.width( ) < minSize.width( ) )
		sceneBr.setWidth( minSize.width( ) );
	if ( sceneBr.height( ) < minSize.height( ) )
		sceneBr.setHeight( minSize.height( ) );

	_br = QRectF( QPointF( 0., 0. ), sceneBr.size( ) );
	setPos( sceneBr.topLeft( ) );
	if ( _background != 0 )
		_background->setRect( boundingRect( ) ); 

	return sceneBr;
}

QRectF	NodeGroup::getNodesSceneBoundingRect( ) const
{
	QRectF sceneBr;
	foreach ( qan::Node* node, _nodes )
	{
		QGraphicsItem* nodeItem = node->getGraphicsItem( );
		if ( !sceneBr.isValid( ) )
			sceneBr = nodeItem->sceneBoundingRect( );
		else
			sceneBr = sceneBr.united( nodeItem->sceneBoundingRect( ) );
	}
	return sceneBr;
}
		
void	NodeGroup::updateNodesPositions( bool keepTopLeft )
{
	if ( _layout != 0 )
	{
		qan::Node::Set rootNodes; getRootNodes( rootNodes );
        qan::Node::List rootNodesList = rootNodes.toList( );
        _layout->layout( rootNodesList, _nodes, _scene.sceneRect( ), 0, 0 );
		foreach ( qan::Node* node, getNodes( ) )
			node->getGraphicsItem( )->setPos( node->getPosition( ) );

		if ( !_addAsChilds )
		{
			// Update this group bounding rect
			QPointF topLeft = pos( );

			prepareGeometryChange( );
			QRectF sceneBr = updateBoundingRect( );
			setPos( sceneBr.topLeft( ) );

			if ( keepTopLeft )
			{
				groupMoved( topLeft, pos( ) );
				setPos( topLeft );
			}
		}
	}

	updateBoundingRect( );

	// Update in and out edges for group's nodes
	qan::Edge::Set edges;
	foreach ( qan::Node* node, getNodes( ) )
		edges.unite( node->getOutEdges( ).toSet( ) );
	foreach ( qan::Node* node, getNodes( ) )
		edges.unite( node->getInEdges( ).toSet( ) );
	foreach ( qan::Edge* edge, edges )
		edge->getGraphItem( )->updateItem( );

	if ( _background != 0 )
		_background->setRect( boundingRect( ).adjusted( -1, -1, 1, 1 ) );
}

void	NodeGroup::paint( QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget )
{
    Q_UNUSED( option ); Q_UNUSED( widget );
	if ( _dragOver )
	{
		painter->setPen( Qt::NoPen );
		painter->setBrush( QBrush( QColor( 50, 50, 50, 25 ) ) );
		QRectF itemOutline( boundingRect( ) );
		if ( _nameItem != 0 && _nameItem->isVisible( ) )
			itemOutline = itemOutline.united( mapRectFromItem( _nameItem, _nameItem->boundingRect( ) ) );
		painter->drawRect( itemOutline );
	}
	else
		painter->setBrush( QBrush( ) );
}
//-----------------------------------------------------------------------------


/* Drag and Drop Management *///-----------------------------------------------
void	NodeGroup::itemDragMove( qan::NodeItem* item, QGraphicsItem* target )
{
    Q_UNUSED( item );
	if ( !getAcceptDrops( ) )
		return;

	if ( target != this )
		return;
	_dragOver = true;
	update( );
}

void	NodeGroup::itemDragLeave( qan::NodeItem* item, QGraphicsItem* target )
{
    Q_UNUSED( item );
	if ( !getAcceptDrops( ) )
		return;

	if ( target != this )
		return;
    if ( _dragOver == true )
	{
		_dragOver = false;
		update( );
	}
}

void	NodeGroup::itemDropped( qan::NodeItem* item, QGraphicsItem* target )
{
	if ( !getAcceptDrops( ) )
		return;
	if ( item != 0 && target == this )
		addNode( item->getNode( ) );
	_dragOver = false;
	update( );
}
//-----------------------------------------------------------------------------


/* Group Node Management *///--------------------------------------------------
void	NodeGroup::addNode( qan::Node& node )
{
	_nodes.insert( &node );
	qan::NodeItem* nodeItem = ( qan::NodeItem* )node.getGraphItem( );
	Q_ASSERT( nodeItem );

	nodeItem->setZValue( zValue( ) + 1.0 );
	nodeItem->setDraggable( false );	// Once added in a group the item is no longer draggable to another group.
	if ( _addAsChilds && nodeItem != 0 )
	{
		node.getGraphicsItem( )->setParentItem( this );
		nodeItem->setMovable( false );		// If the group is movable, node item will become a child of this group, let the group manage node "moving"
		updateNodesPositions( );
	}
	else
	{
		nodeItem->setMovable( true );
		nodeItem->setFlag( QGraphicsItem::ItemSendsGeometryChanges, false );
		nodeItem->setFlag( QGraphicsItem::ItemSendsScenePositionChanges, false );
		connect( nodeItem, SIGNAL( itemMoved( QPointF, QPointF ) ), this, SLOT( itemMoved( QPointF, QPointF ) ) );
		updateNodesPositions( true );
	}
}
		
void	NodeGroup::addEdge( qan::Edge& edge )
{
    Q_UNUSED( edge );
	updateNodesPositions( _addAsChilds ? false : true );
}

void	NodeGroup::removeNode( qan::Node& node )
{ 
	_nodes.remove( &node ); 

	updateBoundingRect( );

	if ( _background != 0 )
		_background->setRect( boundingRect( ) ); 
}

void	NodeGroup::getRootNodes( qan::Node::Set& rootNodes )
{
	// Find all root nodes in the given node list
	foreach ( Node* node, getNodes( ) )
	{
		if ( node->getInDegree( ) == 0 )
			rootNodes.insert( node );

		// A node with no in nodes in the group is a root node from the group point of view
		bool hasInNodeInGroup = false;
		qan::Node::Set inNodes; node->collectInNodesSet( inNodes );
		foreach ( qan::Node* inNode, inNodes )
			if ( getNodes( ).contains( inNode ) )
				hasInNodeInGroup = true;

		if ( !hasInNodeInGroup )
			rootNodes.insert( node );
	}
}

void	NodeGroup::itemMoved( QPointF curPos, QPointF oldPos )
{
	qan::NodeItem* movedNode = ( qan::NodeItem* )sender( );
	if ( !_addAsChilds && _layout != 0 && movedNode != 0 )	// A group that is not movable should update nodes positions if one of them is modified (ex: dynamic relationnal layout)
	{
		qan::Node::Set rootNodes; getRootNodes( rootNodes );
        qan::Node::List rootNodesList = rootNodes.toList( );
        _layout->layout( rootNodesList, _nodes, _scene.sceneRect( ), &movedNode->getNode( ), 0 );
	
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
		QRectF sceneBr = updateBoundingRect( );
		setPos( sceneBr.topLeft( ) );
		if ( _background != 0 )
			_background->setRect( boundingRect( ) ); 
	}

	if ( _addAsChilds )
	{
		QPointF delta =  curPos - oldPos;
		setPos( pos( ) + delta );
		groupMoved( curPos, oldPos );
	}
}
//-----------------------------------------------------------------------------


/* Group Mouse Management *///-------------------------------------------------
void	NodeGroup::mouseMoveEvent( QGraphicsSceneMouseEvent* e )
{
	if ( _mousePressed )
	{
		QPointF groupPos = scenePos( );
		QPointF mousePos = e->scenePos( );
		QPointF d =  mousePos - _mousePos;
		if ( !qFuzzyCompare( d.manhattanLength( ), 0. ) )
		{
			moveBy( d.x( ), d.y( ) );
			groupMoved( scenePos( ), groupPos );
			_mousePos = mousePos;
		}
		e->accept( );
	}
	else
		QGraphicsItem::mouseMoveEvent( e );
}

void	NodeGroup::mousePressEvent( QGraphicsSceneMouseEvent* e )
{
	if ( e->button( ) == Qt::LeftButton )
	{
		_mousePressed = true;
		_mousePos = e->scenePos( );
		e->accept( );
	}
	else
		QGraphicsItem::mousePressEvent( e );
}

void	NodeGroup::mouseReleaseEvent( QGraphicsSceneMouseEvent* e )
{
	if ( _mousePressed == true )
	{
		_mousePressed = false;
		e->accept( );
	}
	else
		QGraphicsItem::mouseReleaseEvent( e );
}

void	NodeGroup::wheelEvent( QGraphicsSceneWheelEvent* e )
{
	e->ignore( );	// Ignore mouse wheel events, so that they are propagated to the navigation system.
}
//-----------------------------------------------------------------------------

} // ::qan


