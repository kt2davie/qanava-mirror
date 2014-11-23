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
// \file	qanSimpleNodeItem.cpp
// \author	benoit@qanava.org
// \date	2014 November 22
//-----------------------------------------------------------------------------


// Qanava headers
#include "./qanSimpleNodeItem.h"
#include "./qanStyleManager.h"
#include "./qanGraphScene.h"

// QT headers
#include <QFont>
#include <QPainter>
#include <QPixmap>
#include <QGraphicsTextItem>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QTextEdit>
#include <QTransform>
#include <QMimeData>


namespace qan {	// ::qan


/* SimpleNodeItem Constructor/Destructor *///----------------------------------
SimpleNodeItem::SimpleNodeItem( GraphScene& scene, Node& node, QGraphicsItem* parent, bool isMovable, bool showPropertiesWidget ) :
	GraphItem( scene, parent ),
	_node( node ),
	_scene( scene ),
	_dimension( 170.0, 45.0 ),
	_shadowColor( ),
	_shadowOffset( QPointF( 4., 4. ) ),
	_shadowEffect( 0 ),
    _mousePos( -1. , -1. ),
    _mousePressed( false ),
	_isMovable( isMovable ),
	_isDraggable( true ),
	_dragMove( false ),
    _shapeItem( 0 ),
    _dragOverItem( 0 )
{
	setFlag( QGraphicsItem::ItemSendsGeometryChanges );
	setFlag( QGraphicsItem::ItemSendsScenePositionChanges );	// Catch position changes from node child of a qan::NodeGroup
	setZValue( 2.0 );
	setAcceptDrops( false );

	if ( showPropertiesWidget )
		activatePropertiesPopup( node.getProperties( ), 500, true );
}

SimpleNodeItem::~SimpleNodeItem( ) { }
//-----------------------------------------------------------------------------


/* SimpleNodeItem Associed Graphics Item Management *///-----------------------
QRectF	SimpleNodeItem::boundingRect( ) const
{
	QRectF br( 0., 0., _dimension.x( ), _dimension.y( ) );
	if ( _shapeItem != 0 )
		br = br.united( mapFromItem( _shapeItem, _shapeItem->boundingRect( ) ).boundingRect( ) );
	return br;
}

QPainterPath	SimpleNodeItem::shape( ) const
{
	QPainterPath qpp;
	qpp.addRect( boundingRect( ) );
	return qpp;
}

void	SimpleNodeItem::updateItem( )
{
	qan::Style* style = _styleManager.getStyle( getNode( ) );
	if ( style == 0 )
	{
		style = _styleManager.getTargetStyle( "qan::Node" );
		if ( style != 0 )
			_styleManager.styleNode( _node, style->getName( ) );
	}
	QBrush b;
	if ( _shapeItem != 0 )
		b = _shapeItem->brush( );
	QColor backColor = QColor( 255, 255, 255 );
	if ( style != 0 && style->has( "Back Color" ) )
	{
		backColor = style->getColor( "Back Color" );
		b.setStyle( Qt::SolidPattern );
		b.setColor( backColor );
	}
	if ( style != 0 && style->has( "No Background" ) && style->get( "No Background" ).toBool( ) )
		b.setStyle( Qt::NoBrush );

	if ( _shapeItem != 0 )
		_shapeItem->setBrush( b );

	QPen p;
	if ( _shapeItem != 0 )
		p = _shapeItem->pen( );
	QColor borderColor = QColor( 0, 0, 0 );
	Qt::PenStyle borderStyle = Qt::SolidLine;
	float borderWidth = 1.0; 
	if ( style != 0 && style->has( "Border Color" ) )
		borderColor = style->getColor( "Border Color" );
	if ( style != 0 && style->has( "Border Style" ) )
		borderStyle = ( Qt::PenStyle )( style->get( "Border Style" ).toInt( ) + 1 );	// +1 since 0 is noline and does not appears in style selection dialog
	if ( style != 0 && style->has( "Border Width" ) )
		borderWidth = style->get( "Border Width" ).toFloat( );
	p.setColor( borderColor );
	p.setStyle( borderStyle );
	p.setJoinStyle( Qt::RoundJoin );
	p.setWidthF( borderWidth );
	if ( _shapeItem != 0 )
		_shapeItem->setPen( p );

    bool hasShadow = false;
    if ( style != 0 && style->has( "Has Shadow" ) )
        hasShadow = style->get( "Has Shadow" ).toBool( );
    if ( hasShadow )
    {
        if ( style->has( "Shadow Color" ) )
            _shadowColor = style->getColor( "Shadow Color" );
        else
            _shadowColor = QColor( 105, 105, 105 );
        _shadowOffset = QPointF( 4., 4. );
        if ( style->has( "Shadow Offset" ) )
        {
            QSizeF shadowOffset = style->get( "Shadow Offset" ).toSizeF( );
            _shadowOffset = QPointF( shadowOffset.width( ), shadowOffset.height( ) );
        }

        if ( _shadowEffect == 0 )
            _shadowEffect = new QGraphicsDropShadowEffect( this );
        if ( _shadowEffect != 0 && _shapeItem != 0 )
        {
            _shadowEffect->setColor( _shadowColor );
            _shadowEffect->setOffset( _shadowOffset );
            _shadowEffect->setBlurRadius( 2.0 );
            _shapeItem->setGraphicsEffect( _shadowEffect );
            _shadowEffect->setEnabled( true );
        }
    }
    else
    {
        if ( _shadowEffect != 0 )
            _shadowEffect->setEnabled( false );
        _shadowColor = QColor( ); // Invalid color since there is no shadow
    }
}

void	SimpleNodeItem::updateItemStyle( )
{
	updateItem( );
}
//-----------------------------------------------------------------------------


/* NodeItem Drawing Management *///--------------------------------------------
void	SimpleNodeItem::paint( QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget )
{
    Q_UNUSED( painter ); Q_UNUSED( option ); Q_UNUSED( widget );

	if ( !_shadowColor.isValid( ) && _shadowEffect != 0 )
		_shadowEffect->setEnabled( false );

    //painter->drawRect( boundingRect( ).toRect( ) );
}

QVariant	SimpleNodeItem::itemChange( GraphicsItemChange change, const QVariant& value )
{
	if ( change == QGraphicsItem::ItemPositionHasChanged || change == QGraphicsItem::ItemScenePositionHasChanged )
	{
		//if ( !qFuzzyCompare( ( _node.getPosition( ) - pos( ) ).manhattanLength( ), 0. ) )
		{
			_node.setPosition( pos( ) );
			foreach ( Edge* edge, _node.getOutEdges( ) )
				edge->getGraphItem( )->updateItem( );
			foreach ( Edge* edge, _node.getInEdges( ) )
				edge->getGraphItem( )->updateItem( );
		}
	}
	return 	QGraphicsItem::itemChange( change, value );
}

void	SimpleNodeItem::mouseMoveEvent( QGraphicsSceneMouseEvent* e )
{
	if ( _mousePressed && _isMovable )
	{
		QPointF oldPos	= scenePos( );
		QPointF d		=  e->scenePos( ) - _mousePos;
		if ( !qFuzzyCompare( 1. + d.manhattanLength( ), 1. ) )
		{
			hidePropertiesPopup( );
			moveBy( d.x( ), d.y( ) );
			emit itemMoved( pos( ), oldPos );
			getNode( ).setPosition( pos( ) );
			_mousePos = e->scenePos( );

			if ( isDraggable( ) )
			{
				// Detect collision with a drag target item in scene
				bool dragOverItem = false;
				foreach ( QGraphicsItem* dropTarget, _scene.getDropTargets( ) )
					if ( dropTarget->sceneBoundingRect( ).contains( sceneBoundingRect( ) ) )
					{
						_scene.emitItemDragMove( this, dropTarget );
						dragOverItem = true;
						_dragOverItem = dropTarget;
					}
				// If we were previously dragging over an item and there is no longer collision, send a drag leave signal to that item
				if ( dragOverItem == false && _dragOverItem != 0 )
				{
					_scene.emitItemDragLeave( this, _dragOverItem );
					_dragOverItem = 0;
				}
			}
		}
		e->accept( );
	}
	else
		QGraphicsItem::mouseMoveEvent( e );
}

void	SimpleNodeItem::mousePressEvent( QGraphicsSceneMouseEvent* e )
{
	if ( _isMovable && e->button( ) == Qt::LeftButton )
	{
		_mousePressed = true;
		_mousePos = e->scenePos( );
		update( );
		e->accept( );
	}
	else
		QGraphicsItem::mousePressEvent( e );
}

void	SimpleNodeItem::mouseReleaseEvent( QGraphicsSceneMouseEvent* e )
{
	if ( isDraggable( ) && _dragOverItem != 0 )
	{
		_scene.emitItemDropped( this, _dragOverItem );
		_dragOverItem = 0;
	}
	_mousePressed = false;
	update( );
	QGraphicsItem::mouseReleaseEvent( e );
}
//-----------------------------------------------------------------------------


} // ::qan

