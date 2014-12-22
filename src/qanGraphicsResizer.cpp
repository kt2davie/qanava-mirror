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
// \file	qanGraphicsResizer.cpp
// \author	benoit@qanava.org
// \date	2014 December 01
//-----------------------------------------------------------------------------

// QT headers
#include <QGraphicsItem>
#include <QBrush>
#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <QCursor>
#include <QGraphicsScene>
#include <QGraphicsView>

// Qanava headers
#include "./qanGraphicsResizer.h"

namespace qan {	// ::qan

/* GraphicsResizer Object Management *///-------------------------------------- 
GraphicsResizer::GraphicsResizer( QGraphicsItem* target, QGraphicsLayoutItem* targetLayout, GraphicsController* controller, QSizeF gripSize ) :
    QGraphicsObject( target ),
    _target( target ),
    _targetLayout( targetLayout ),
    _controller( controller ),
    _gripSize( gripSize )
{
    setAcceptHoverEvents( true );
}
//-----------------------------------------------------------------------------


/* Graphics Item Management *///-----------------------------------------------
QRectF	GraphicsResizer::boundingRect( ) const
{
    Q_ASSERT( _target );
    return QRectF( QPointF( 0., 0. ), _target->boundingRect( ).size( ) );
}

void	GraphicsResizer::paint( QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget )
{
    Q_UNUSED( option ); Q_UNUSED( widget ); Q_UNUSED( painter );

/*  // FIXME: debug code
    painter->setPen( Qt::black );
    painter->drawRect( boundingRect( ) );
    painter->setPen( Qt::red );
    painter->drawRect( QRectF( QPointF( 0., 0. ), _targetLayout->minimumSize( ) ) );*/
}
//-----------------------------------------------------------------------------


/* Mouse Events Management *///------------------------------------------------
bool    GraphicsResizer::sceneEventFilter( QGraphicsItem* watched, QEvent* e )
{
    Q_ASSERT( _target );
    if ( watched != _target )
        return QGraphicsItem::sceneEventFilter( watched, e );

    if ( e->type( ) == QEvent::GraphicsSceneHoverEnter )
    {
        QGraphicsSceneHoverEvent* hoverEvent = ( QGraphicsSceneHoverEvent* )e; Q_UNUSED( hoverEvent );
        return false;
    }

    if ( e->type( ) == QEvent::GraphicsSceneHoverMove )				// Set the resizing cursor on hover move in bottom right corner
    {
        QGraphicsSceneHoverEvent* hoverEvent = ( QGraphicsSceneHoverEvent* )e; Q_UNUSED( hoverEvent );
        hoverMoveEvent( hoverEvent );
        return false;
    }

    if ( e->type( ) == QEvent::GraphicsSceneHoverLeave )			// Reset rezise cursor when the mouse hover out bottom bar
    {
        QGraphicsSceneHoverEvent* hoverEvent = ( QGraphicsSceneHoverEvent* )e; Q_UNUSED( hoverEvent );
        hoverLeaveEvent( hoverEvent );
        return false;
    }
    return QGraphicsItem::sceneEventFilter( watched, e );
}

void	GraphicsResizer::mouseMoveEvent( QGraphicsSceneMouseEvent* e )
{
    QGraphicsSceneMouseEvent* moveEvent = ( QGraphicsSceneMouseEvent* )( e );
    if (  _target->isVisible( ) &&          		// Handle resizing, only if content is visible (ie
          cursor( ).shape( ) == Qt::SizeFDiagCursor ||
          cursor( ).shape( ) == Qt::SizeBDiagCursor )
    {
        QPointF d( moveEvent->pos( ).x( ) - moveEvent->lastPos( ).x( ),
                  moveEvent->pos( ).y( ) - moveEvent->lastPos( ).y( ) );
        QSizeF minSize( _targetLayout->minimumSize( ) );

        if ( _corner == Qt::BottomRightCorner )
        {
            QRectF targetGeometry( _target->scenePos( ),
                                   _target->boundingRect( ).size( ) + QSizeF( d.x( ), d.y( ) ) );
            _targetLayout->setGeometry( QRectF( targetGeometry.topLeft( ),
                                                targetGeometry.size( ).expandedTo( _targetLayout->minimumSize( ) ) ) );
            e->accept();
            if ( _controller != 0 )
                _controller->forceMinimzedState( );
            return;
        }
        else if ( _corner == Qt::TopLeftCorner )
        {
            QRectF targetGeometry( _target->scenePos( ) + d,
                                     _target->boundingRect( ).size( ) - QSizeF( d.x( ), d.y( ) ) );
            if ( targetGeometry.width( ) < minSize.width( ) )
                d.rx( ) = 0;
            if ( targetGeometry.height( ) < minSize.height( ) )
                d.ry( ) = 0;
            _targetLayout->setGeometry( QRectF( _target->scenePos( ) + d,
                                                _target->boundingRect( ).size( ) - QSizeF( d.x( ), d.y( ) ) ) );
            e->accept();
            if ( _controller != 0 )
                _controller->forceMinimzedState( );
            return;
        }
        else if ( _corner == Qt::TopRightCorner )
        {
            QRectF targetGeometry( _target->scenePos( ) + QPointF( 0., d.y( ) ),
                                   _target->boundingRect( ).size( ) + QSizeF( d.x( ), -d.y( ) ) );
            if ( targetGeometry.width( ) < minSize.width( ) )
                d.rx( ) = 0;
            if ( targetGeometry.height( ) < minSize.height( ) )
                d.ry( ) = 0;
            _targetLayout->setGeometry( QRectF( _target->scenePos( ) + QPointF( 0., d.y( ) ),
                                                _target->boundingRect( ).size( ) + QSizeF( d.x( ), -d.y( ) ) ) );
            e->accept();
            if ( _controller != 0 )
                _controller->forceMinimzedState( );
            return;
        }
        else if ( _corner == Qt::BottomLeftCorner )
        {
            QRectF targetGeometry( _target->scenePos( ) + QPointF( d.x( ), 0 ),
                                   _target->boundingRect( ).size( ) + QSizeF( -d.x( ), d.y( ) ) );
            if ( targetGeometry.width( ) < minSize.width( ) )
                d.rx( ) = 0;
            if ( targetGeometry.height( ) < minSize.height( ) )
                d.ry( ) = 0;
            _targetLayout->setGeometry( QRectF( _target->scenePos( ) + QPointF( d.x( ), 0 ),
                                                _target->boundingRect( ).size( ) + QSizeF( -d.x( ), d.y( ) ) ) );
            e->accept();
            if ( _controller != 0 )
                _controller->forceMinimzedState( );
            return;
        }
        return;
    }
    QGraphicsObject::mouseMoveEvent( e );
}

void	GraphicsResizer::mousePressEvent( QGraphicsSceneMouseEvent* e )
{
    QGraphicsSceneMouseEvent* mousePressEvent = ( QGraphicsSceneMouseEvent* )( e );
    QPointF pos = mapFromItem( _target, mousePressEvent->pos( ) );
    Qt::Corner c;
    if ( inCorner( pos, c ) )
    {
        e->accept( );
        return;
    }
    QGraphicsObject::mousePressEvent( e );
}

void	GraphicsResizer::hoverEnterEvent( QGraphicsSceneHoverEvent* e )
{
    QPointF pos = mapFromItem( _target, e->pos( ) );
    Qt::Corner c;
    if ( inCorner( pos, c ) )
    {
        e->accept( );
        return;
    }
    // FIXME: Didn't succeed to let Qt (<=5.4) to automatically forward hover
    // events to a child item if they where catched in a this child item, so
    // manually forward hover events to an eventual associed controller...
    if ( _controller != 0 )
        _controller->hoverEnterEvent( e );
    QGraphicsObject::hoverEnterEvent( e );
}

void	GraphicsResizer::hoverLeaveEvent( QGraphicsSceneHoverEvent* e )
{
    setCursor( Qt::ArrowCursor );
    // FIXME: Didn't succeed to let Qt (<=5.4) to automatically forward hover
    // events to a child item if they where catched in a this child item, so
    // manually forward hover events to an eventual associed controller...
    if ( _controller != 0 )
        _controller->hoverLeaveEvent( e );
    QGraphicsObject::hoverLeaveEvent( e );
}

void	GraphicsResizer::hoverMoveEvent( QGraphicsSceneHoverEvent* e )
{
    QGraphicsSceneHoverEvent* hoverEvent = ( QGraphicsSceneHoverEvent* )e;
    QPointF pos = mapFromItem( _target, hoverEvent->pos( ) );
    Qt::Corner c;
    if ( inCorner( pos, c ) )
    {
        if ( c == Qt::TopLeftCorner || c == Qt::BottomRightCorner )
            setCursor( Qt::SizeFDiagCursor );
        if ( c == Qt::TopRightCorner || c == Qt::BottomLeftCorner )
            setCursor( Qt::SizeBDiagCursor );
        _corner = c;
        e->accept( );
    }
    else
    {
        _corner = (Qt::Corner)42;
        setCursor( Qt::ArrowCursor );
    }

    // FIXME: Didn't succeed to let Qt (<=5.4) to automatically forward hover
    // events to a child item if they where catched in a this child item, so
    // manually forward hover events to an eventual associed controller...
    if ( _controller != 0 )
        _controller->hoverMoveEvent( e );

    QGraphicsObject::hoverMoveEvent( e );
}

bool    GraphicsResizer::inCorner( QPointF pos, Qt::Corner& corner )
{
    QRectF br = boundingRect( );

    if ( pos.x( ) <= _gripSize.width( ) && pos.y( ) <= _gripSize.height( ) )
    {
        corner = Qt::TopLeftCorner;
        return true;
    }

    if ( pos.x( ) >= br.width( ) - _gripSize.width( ) && pos.y( ) <= _gripSize.height( ) )
    {
        corner = Qt::TopRightCorner;
        return true;
    }

    if ( pos.x( ) >= br.width( ) - _gripSize.width( ) && pos.y( ) >= br.height( ) - _gripSize.height( ) )
    {
        corner = Qt::BottomRightCorner;
        return true;
    }

    if ( pos.x( ) <= _gripSize.width( ) && pos.y( ) >= br.height( ) - _gripSize.height( ) )
    {
        corner = Qt::BottomLeftCorner;
        return true;
    }

    return false;
}
//-----------------------------------------------------------------------------

} // ::qan

