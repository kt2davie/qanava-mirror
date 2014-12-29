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
#include <QtDebug>
#include <QVBoxLayout>
#include <QTimer>
#include <QScrollBar>
#include <QTextDocument>
#include <QGraphicsLayout>


namespace qan { // ::qan


/* NodeGroup Object Management *///--------------------------------------------
NodeGroup::NodeGroup( qan::GraphScene& scene, QString name, QGraphicsItem* parent ) :
    QGraphicsObject( parent ), QGraphicsLayout( ),
	_scene( scene ),
    _graphLayout( 0 ),
    _layout( 0 ),
	_properties( "properties" ),
	_name( name ),
	_nameItem( 0 ),
    _acceptDrops( true ),
	_dragOver( false ),
	_br( QRectF( ) ),
	_background( 0 ),
	_mousePressed( false ),
    _isMovable( true )
{
    setFlag( QGraphicsItem::ItemIsMovable, false );
    setFlag( QGraphicsItem::ItemIsSelectable, false );
    setFlag( QGraphicsItem::ItemClipsToShape, false );

    // Graphics layout item initialisation
    setGraphicsItem( this );
    setMinimumSize( QSizeF( 185., 85. ) );
    setPreferredSize( QSizeF( 185., 85. ) );

    // Node group initialisation
    connect( &scene, SIGNAL( itemDragMove( qan::SimpleNodeItem*, QGraphicsItem* ) ), this, SLOT( itemDragMove( qan::SimpleNodeItem*, QGraphicsItem* ) ) );
    connect( &scene, SIGNAL( itemDragLeave( qan::SimpleNodeItem*, QGraphicsItem* ) ), this, SLOT( itemDragLeave( qan::SimpleNodeItem*, QGraphicsItem* ) ) );
    connect( &scene, SIGNAL( itemDropped( qan::SimpleNodeItem*, QGraphicsItem* ) ), this, SLOT( itemDropped( qan::SimpleNodeItem*, QGraphicsItem* ) ) );

	_nameItem = new LabelEditorItem( name, "<< Group name >>", this );
	_nameItem->setPos( QPointF( 0., -_nameItem->boundingRect( ).height( ) ) );
	connect( _nameItem, SIGNAL( textModified( ) ), this, SLOT( nameTextModified( ) ) );

    _br.setSize( preferredSize( ) );
    _background = scene.addRect( QRectF( QPointF( 0., 0. ), preferredSize( ) ),
                                 QPen( QColor( 190, 190, 190, 75 ) ), QBrush( QColor( 190, 190, 190, 75 ) ) );
    _background->setParentItem( this );
    QGraphicsLayout::setGraphicsItem( this );
    QGraphicsLayout::setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding, QSizePolicy::DefaultType );
    updateGeometry( );  // Force Br generation
    updateGroup( );     // Force bounding rect generation
}
//-----------------------------------------------------------------------------


/* Group Management *///-------------------------------------------------------
void	NodeGroup::setGraphLayout( qan::Layout* graphLayout )
{
    _graphLayout = graphLayout;
}

void	NodeGroup::setLayout( QGraphicsLinearLayout* layout )
{
#ifdef QT_DEBUG
    if ( _graphLayout != 0 )
    {
    qDebug() << "NodeGroup::setLayout - Can't set a graphics layout if a regular Qanava graph layout is already sets... ";
    Q_ASSERT( false );
    }
#endif
    if ( layout != 0 && layout->isLayout( ) )
    {
        _layout = layout;
        layout->setParentLayoutItem( this );
    }
}

void	NodeGroup::layout( )
{
    if ( _graphLayout == 0 )
        return;

    // Apply qanava layout
    qan::Node::Set rootNodes; getRootNodes( rootNodes );
    qan::Node::List rootNodesList = rootNodes.toList( );
    _graphLayout->layout( rootNodesList, _nodes, _scene.sceneRect( ), 0, 0 );

    updateGroup( ); // Update node with their new layout and generate a correct bounding rect
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
QRectF	NodeGroup::boundingRect( ) const { return _br; }

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

void	NodeGroup::updateGroup( )
{
    // Update sub items position according to their laid out position
    if ( _graphLayout != 0 )
    {
        qan::Node::Set rootNodes; getRootNodes( rootNodes );
        qan::Node::List rootNodesList = rootNodes.toList( );
        _graphLayout->layout( rootNodesList, _nodes, _scene.sceneRect( ), 0, 0 );
        foreach ( qan::Node* node, getNodes( ) )
            node->getGraphicsItem( )->setPos( node->getPosition( ) );
    }

    // Update group bounding rect
    if ( _graphLayout != 0 && _layout == 0 )
    {
        QSizeF minSize = minimumSize( );
        Q_ASSERT( minSize.isValid( ) );
        if ( _br.isNull( ) || _br.isEmpty( ) || !_br.isValid( ) )   // Bounding rect evntual initialization
        {
            prepareGeometryChange( );
            _br.setSize( minimumSize( ) );
        }

        if ( _br.width( ) < minSize.width( ) ||     // Expand actual Br to minimum size defined by user
            _br.height( ) < minSize.height( ) )
        {
            prepareGeometryChange( );
            _br.setSize( _br.size( ).expandedTo( minSize ) );
        }

        // Adapt bounding rect to the layed out content
        QRectF nodesSceneBr = getNodesSceneBoundingRect( );
        prepareGeometryChange( );
        _br = QRectF( QPointF( 0., 0. ), nodesSceneBr.size( ).expandedTo( minimumSize( ) ) );
        //setPos( nodesSceneBr.topLeft( ) );
    }

    // Update group nodes edges
    qan::Edge::Set edges;
    foreach ( qan::Node* node, getNodes( ) )
        edges.unite( node->getOutEdges( ).toSet( ) );
    foreach ( qan::Node* node, getNodes( ) )
        edges.unite( node->getInEdges( ).toSet( ) );
    foreach ( qan::Edge* edge, edges )
        edge->getGraphItem( )->updateItem( );

    if ( _background != 0 ) // Update group background according to the new bounding rect
        _background->setRect( boundingRect( ).adjusted( -1, -1, 1, 1 ) );
}

void	NodeGroup::setVisible( bool v )
{
    QGraphicsItem::setVisible( v );

    // Hide group node's graphics items (and their edges graphics items)
    foreach ( qan::Node* node, _nodes )
    {
        Q_ASSERT( node->getGraphicsItem( ) );
        node->getGraphicsItem( )->setVisible( v );

        // Hide nodes edges graphics items
        qan::Edge::Set edges( node->getInEdges( ).toSet( ) );
        edges.unite( node->getOutEdges( ).toSet( ) );
        foreach ( qan::Edge* edge, edges )
            edge->getGraphicsItem( )->setVisible( v );
    }
}

void	NodeGroup::groupMoved( QPointF curPos, QPointF oldPos )
{
    Q_UNUSED( oldPos );

    // Update group nodes edges
    qan::Edge::Set edges;
    foreach ( qan::Node* node, getNodes( ) )
        edges.unite( node->getOutEdges( ).toSet( ) );
    foreach ( qan::Node* node, getNodes( ) )
        edges.unite( node->getInEdges( ).toSet( ) );
    foreach ( qan::Edge* edge, edges )
        edge->getGraphItem( )->updateItem( );

    // Force group update when there is no parent layout to invalidate...
    if ( _layout != 0 && parentLayoutItem( ) == 0 )
        setGeometry( QRectF( curPos, boundingRect( ).size( ) ) );
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
//-----------------------------------------------------------------------------


/* Graphics layout item implementation *///------------------------------------
void NodeGroup::updateGeometry( )
{
    QGraphicsLayout::updateGeometry( );
}

void NodeGroup::setGeometry( const QRectF& geom )
{
    prepareGeometryChange( );
    QGraphicsLayout::setGeometry( geom );

    if ( _background != 0 )
        _background->setRect( QRectF( 0., 0., geom.width( ), geom.height( ) ) );
        //_background->setRect( boundingRect( ) );

    // Update sublayout
    if ( _layout != 0 )
    {
        _layout->setGeometry( geom );   // Let sublayout compute an "ideal" geometry, then set it to this group
        _br = QRectF( QPointF( 0., 0. ), _layout->geometry( ).size( ) ); // (usefull to avoid resizing smaller than the sub layout content...
        setPos( geom.topLeft( ) );
    }
    else
    {
        _br = QRectF( QPointF( 0., 0. ), geom.size( ) );
        setPos( geom.topLeft( ) );
    }
}

QSizeF NodeGroup::sizeHint( Qt::SizeHint which, const QSizeF& constraint ) const
{
    switch ( which )
    {
    case Qt::MinimumSize:
    {
        if ( _layout != 0 )
            return _layout->minimumSize( );
        return  sizeHint( which, constraint ); //SizeF( 185., 85. )
    }
    case Qt::PreferredSize:
    {
        if ( _layout != 0 )
        {
            QSizeF layoutPrefSize = _layout->preferredSize( );
            return layoutPrefSize;
        }
        QSizeF prefSize = boundingRect( ).size( );

        // If group is laid off with a qanava graph layout and that group must
        // adapt to content size, return an ideal br enclosing all laid out items
        /*if ( _graphLayout != 0 )
        {
            QRectF preferredBr = getNodesSceneBoundingRect( );
            prefSize = preferredBr.size( );
        }*/
        return prefSize;
    }
        break;
    case Qt::MaximumSize:
        return QSizeF( 1000, 1000 );
        break;
    case Qt::MinimumDescent:
        break;
    default:
        break;
    }
    return constraint;
}

int	NodeGroup::count( ) const
{
    return ( _layout != 0 ? _layout->count( ) : 0 );
}

QGraphicsLayoutItem*	NodeGroup::itemAt( int i ) const
{
    return ( _layout != 0 ? _layout->itemAt( i ) : 0 );
}

void	NodeGroup::removeAt( int index )
{
    if ( _layout != 0 )
        _layout->removeAt( index );
}
//-----------------------------------------------------------------------------


/* Drag and Drop Management *///-----------------------------------------------
void	NodeGroup::itemDragMove( qan::SimpleNodeItem* item, QGraphicsItem* target )
{
    Q_UNUSED( item );
	if ( !getAcceptDrops( ) )
		return;

	if ( target != this )
		return;
	_dragOver = true;
	update( );
}

void	NodeGroup::itemDragLeave( qan::SimpleNodeItem* item, QGraphicsItem* target )
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

void	NodeGroup::itemDropped( qan::SimpleNodeItem* item, QGraphicsItem* target )
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
    if ( _layout == 0 && _graphLayout == 0 )
        return; // Node can't be added to a group if there is no layout sets.

    qan::NodeItem* nodeItem = ( qan::NodeItem* )node.getGraphItem( );
    Q_ASSERT( nodeItem );
    _nodes.insert( &node );

    nodeItem->setZValue( zValue( ) + 1.0 );
    nodeItem->setDraggable( false );	// Once added in a group the item is no longer draggable to another group.

    if ( _layout != 0 )
    {
        nodeItem->setMovable( false );  // When added as a child, node is no longer movable, since its position is managed by Qt graphics layout
        _layout->addItem( nodeItem );
        nodeItem->setParentLayoutItem( _layout );
        //parentLayoutItem( )->updateGeometry( );
        if ( parentLayoutItem( ) != 0 && parentLayoutItem( )->isLayout( ) )
        {
            QGraphicsLayout* gl = static_cast< QGraphicsLayout* >(  parentLayoutItem( ) );
            gl->invalidate( );
        }

        // FIXME: see what is exactly necessary....
        _layout->invalidate( );
        _layout->activate();

        invalidate( );
        activate( );

        // Necessary to update groups who doesn't have a parent item layout
        if ( parentLayoutItem( ) == 0 )
            setGeometry( QRectF( pos( ), preferredSize( ) ) );
    } else if ( _graphLayout != 0 )
    {
        nodeItem->setParentItem( this );
        nodeItem->setMovable( false );
        nodeItem->setFlag( QGraphicsItem::ItemSendsGeometryChanges, false );
        nodeItem->setFlag( QGraphicsItem::ItemSendsScenePositionChanges, false );
        connect( nodeItem, SIGNAL( itemMoved( QPointF, QPointF ) ), this, SLOT( itemMoved( QPointF, QPointF ) ) );
    }

    updateGroup( );
    updateGeometry( );
}

void	NodeGroup::addEdge( qan::Edge& edge )
{
    Q_UNUSED( edge );
    updateGroup( );
}

void	NodeGroup::removeNode( qan::Node& node )
{ 
	_nodes.remove( &node ); 

    // FIXME v1.0:
        // Reparent node to scene eventually
        // Remove every decoration items associed to node graphics item.

    updateGroup( );
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
    Q_UNUSED( oldPos ); Q_UNUSED( curPos );
    qan::NodeItem* movedNode = static_cast< qan::NodeItem* >( sender( ) );
    Q_ASSERT( movedNode );

/*    if ( !_addAsChilds && _graphLayout != 0 && movedNode != 0 )	// A group that is not movable should update nodes positions if one of them is modified (ex: dynamic relationnal layout)
	{
		qan::Node::Set rootNodes; getRootNodes( rootNodes );
        qan::Node::List rootNodesList = rootNodes.toList( );
        _graphLayout->layout( rootNodesList, _nodes, _scene.sceneRect( ), &movedNode->getNode( ), 0 );
	
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
    }*/
}
//-----------------------------------------------------------------------------


/* Group Mouse Management *///-------------------------------------------------
void	NodeGroup::mouseMoveEvent( QGraphicsSceneMouseEvent* e )
{
    if ( _isMovable && _mousePressed )
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
    if ( _isMovable && e->button( ) == Qt::LeftButton )
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
    if ( _isMovable && _mousePressed == true )
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


