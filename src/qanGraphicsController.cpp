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
// \file	qanGraphicsController.cpp
// \author	benoit@qanava.org
// \date	2014 December 20
//-----------------------------------------------------------------------------

// QT headers
#include <QGraphicsItem>
#include <QBrush>
#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsScene>
#include <QGraphicsView>

// Qanava headers
#include "./qanGraphicsController.h"

namespace qan {	// ::qan

/* GraphicsController Object Management *///-----------------------------------
GraphicsController::GraphicsController( QGraphicsItem* target, QGraphicsLayoutItem* targetLayout, QSizeF buttonSize, Config config ) :
    QGraphicsObject( target ),
    _target( target ),
    _targetLayout( targetLayout ),
    _config( config ),
    _shadow( 0 ),
    _controlColor( Qt::darkGray ),
    _focusedInColor( QColor( 130, 200, 250 ) ) ,
    _focusedOutColor( QColor( 240, 240, 240 ) ),
    _buttonSize( buttonSize ),
    _showMaximize( false ),
    _hilightMaximize( false ),
    _isMaximized( false ),
    _minimizedGeom( QRectF( ) ),
    _showClose( false ),
    _hilightClose( false ),
    _showExpand( false ),
    _hilightExpand( false ),
    _isExpanded( true )
{
    setAcceptHoverEvents( true );
    target->setAcceptHoverEvents( true );
    target->installSceneEventFilter( this );
    _minimizedGeom = targetLayout->geometry( );

    // If control button auto hiding is disabled, show buttons by default.
    if ( !( _config & AutoHide ) )
    {
        _showMaximize = true;
        _showClose = true;
        _showExpand = true;
    }

    if ( _config & ShowFocusShadow )
    {
        _shadow = new QGraphicsDropShadowEffect( this );
        _shadow->setColor( _focusedOutColor );
        _shadow->setOffset( 7., 7. );
        _shadow->setBlurRadius( 10. );
        _target->setGraphicsEffect( _shadow );
    }
}

void    GraphicsController::maximized( )
{
    Q_ASSERT( _targetLayout );
    Q_ASSERT( _target );

    QRectF geom( _targetLayout->geometry( ) );
    QList< QGraphicsView* > views = _target->scene( )->views( );
    if ( views.size( ) > 0 )
    {
        QGraphicsView* view = views.at( 0 );
        QPointF sceneViewOrigin = view->mapToScene( QPoint( 0, 0 ) );
        QPointF sceneViewDimension = view->mapToScene( QPoint( view->viewport()->width( ), view->viewport()->height( ) ) );
        QSizeF sceneViewSize = QSizeF( sceneViewDimension.x( ), sceneViewDimension.y( ) ).boundedTo( _targetLayout->maximumSize( ) );
        geom.setTopLeft( sceneViewOrigin );
        geom.setSize( sceneViewSize );
        _minimizedGeom = _targetLayout->geometry( );
        _targetLayout->setGeometry( geom );
    }
    _target->update( );
    update( );
}

void    GraphicsController::minimized( )
{
    Q_ASSERT( _targetLayout );
    Q_ASSERT( _target );

    _targetLayout->setGeometry( _minimizedGeom );
    _target->update( );
    update( );
}

void    GraphicsController::closed( )
{
    _target->setVisible( false );
}
//-----------------------------------------------------------------------------


/* Graphics Item Management *///-----------------------------------------------
QRectF	GraphicsController::boundingRect( ) const
{
    Q_ASSERT( _target );
    return QRectF( QPointF( 0., 0. ), _target->boundingRect( ).size( ) );
}

void	GraphicsController::paint( QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget )
{
    Q_UNUSED( option ); Q_UNUSED( widget );

    if ( ( _config & ShowMaximize ) && _showMaximize )
    {
        QRectF maxBr = maximizeBr( );
        painter->setPen( _controlColor );
        painter->setBrush( Qt::NoBrush );
        if ( _isMaximized )
        {
            QRectF adjMaxBr = maxBr.adjusted( 3., 1., -3. , -4. );
            QRectF adjMaxBr2 = adjMaxBr.translated( QPointF( -2., 3. ) );
            QPen pen( _controlColor );

            painter->setPen( pen );
            painter->drawRect( adjMaxBr );
            painter->drawRect( adjMaxBr2 );
            painter->drawLine( adjMaxBr.topLeft( ) + QPointF( 0.99, 0.99 ),
                               adjMaxBr.topRight( ) + QPointF( -0.99, 0.99 ) );
            painter->drawLine( adjMaxBr2.topLeft( ) + QPointF( 0.99, 0.99 ),
                               adjMaxBr2.topRight( ) + QPointF( -0.99, 0.99 ) );
        }
        else
        {
            QRectF adjMaxBr = maxBr.adjusted( 1., 1., -1. , -1. );
            QPen pen( _controlColor );
            painter->setPen( pen );
            painter->drawRect( adjMaxBr );
            painter->drawLine( adjMaxBr.topLeft( ) + QPointF( 0.99, 0.99 ),
                               adjMaxBr.topRight( ) + QPointF( -0.99, 0.99 ) );
        }

        if ( _hilightMaximize )
        {
            painter->setPen( Qt::NoPen );
            painter->setBrush( QBrush( QColor( 150, 150, 150, 125 ) ) );
            painter->drawRect( maxBr.adjusted( -1, -1, 1, 1 ) );
        }
    }

    if ( ( _config & ShowClose ) && _showClose )
    {
        QRectF buttonBr = closeBr( );
        painter->setPen( _controlColor );
        painter->drawLine( buttonBr.topLeft( ) + QPointF( 1., 1. ), buttonBr.bottomRight( ) - QPointF( 1., 1. ) );
        painter->drawLine( buttonBr.bottomLeft( ) + QPointF( 1., -1. ), buttonBr.topRight( ) + QPointF( -1., 1. ) );
        if ( _hilightClose )
        {
            painter->setPen( Qt::NoPen );
            painter->setBrush( QBrush( QColor( 150, 150, 150, 125 ) ) );
            painter->drawRect( buttonBr.adjusted( -1, -1, 1, 1 ) );
        }
    }

    if ( ( _config & ShowExpand ) && _showExpand )
    {
        QRectF buttonBr = expandBr( );

        QPen expandPen( _controlColor );
        expandPen.setJoinStyle( Qt::RoundJoin );
        expandPen.setCapStyle( Qt::SquareCap );
        painter->setPen( expandPen );

        if ( _isExpanded )
        {
            QPointF topCorner = buttonBr.topLeft( ) + QPointF( 1., 2. );
            QPointF bottomCorner = buttonBr.bottomLeft( ) + QPointF( 1., -2. );
            QPointF rightCorner = QPointF( buttonBr.right( ) - 3.0, buttonBr.top( ) + buttonBr.width( ) / 2. );

            QPolygonF arrowPoly;
            arrowPoly << topCorner << rightCorner << bottomCorner << topCorner;
            painter->drawPolyline( arrowPoly );
            painter->drawLine( topCorner + QPointF( 0.99 , 1 ), bottomCorner + QPointF( 0.99, -1. ) );
        }
        else
        {
            QPointF leftCorner = buttonBr.topLeft( ) + QPointF( 2., 1. );
            QPointF rightCorner = QPointF( buttonBr.topRight( ) + QPointF( -2., 1. ) );
            QPointF bottomCorner = buttonBr.bottomLeft( ) + QPointF( buttonBr.width( ) / 2., -3. );

            QPolygonF arrowPoly;
            arrowPoly << leftCorner << rightCorner << bottomCorner << leftCorner;
            painter->drawPolyline( arrowPoly );
            painter->drawLine( leftCorner + QPointF( 1. , 1. ), rightCorner + QPointF( -1., 1. ) );
        }
        if ( _hilightExpand )
        {
            painter->setPen( Qt::NoPen );
            painter->setBrush( QBrush( QColor( 150, 150, 150, 125 ) ) );
            painter->drawRect( buttonBr.adjusted( -1, -1, 1, 1 ) );
        }
    }

    if ( _config & ShowBorder )
    {
        painter->setPen( QPen( QColor( 49., 49., 49. ), 1.0 ) );
        painter->setBrush( Qt::NoBrush );
        QRectF br = boundingRect( ).adjusted( 0.5, 0.5, -0.5, -0.5 );
        painter->drawRect( br );
    }
}
//-----------------------------------------------------------------------------


/* Mouse Events Management *///------------------------------------------------
bool    GraphicsController::sceneEventFilter( QGraphicsItem* watched, QEvent* e )
{
    Q_ASSERT( _target );
    if ( watched != _target )
        return QGraphicsItem::sceneEventFilter( watched, e );


    if ( e->type( ) == QEvent::GraphicsSceneHoverEnter )
    {
        hoverEnterEvent( ( QGraphicsSceneHoverEvent* )e );
        return true;
    }

    if ( e->type( ) == QEvent::GraphicsSceneHoverMove )
    {
        hoverMoveEvent( ( QGraphicsSceneHoverEvent* )e );
        return false;
    }

    if ( e->type( ) == QEvent::GraphicsSceneHoverLeave )
    {
        hoverLeaveEvent( ( QGraphicsSceneHoverEvent* )e );
        return true;
    }
    return QGraphicsItem::sceneEventFilter( watched, e );
}

void	GraphicsController::hoverEnterEvent( QGraphicsSceneHoverEvent* e )
{
    Q_ASSERT( _target != 0 );

    _showMaximize = true;
    _showClose = true;
    _showExpand = true;

    emit focusedIn( );
    if ( _shadow != 0 && ( _config & ShowFocusShadow ) )
    {
        _shadow->setColor( _focusedInColor );
        _shadow->update( );
    }
    update( );
}

void    GraphicsController::hoverMoveEvent( QGraphicsSceneHoverEvent* e )
{
    QPointF pos = e->pos( );
    if ( ( _config & ShowMaximize ) &&  maximizeBr( ).contains( pos ) )
    {
        if ( _hilightMaximize == false )    // Try to minimize update calls
        {
            _hilightMaximize = true;
            update( );
        }
        _hilightMaximize = true;
        return;
    }
    else if ( ( _config & ShowClose ) && closeBr( ).contains( pos ) )
    {
        if ( _hilightClose == false )    // Try to minimize update calls
        {
            _hilightClose = true;
            update( );
        }
        _hilightClose = true;
        return;
    }
    else if ( ( _config & ShowExpand ) && expandBr( ).contains( pos ) )
    {
        if ( _hilightExpand == false )    // Try to minimize update calls
        {
            _hilightExpand = true;
            update( );
        }
        _hilightExpand = true;
        return;
    } else
    {
        _hilightMaximize = false;
        _hilightClose = false;
        _hilightExpand = false;
        update( );
    }

    QGraphicsItem::hoverMoveEvent( e );
}

void    GraphicsController::hoverLeaveEvent( QGraphicsSceneHoverEvent* e )
{
    if ( _config & AutoHide )
    {
        _showMaximize = false;
        _showClose = false;
        _showExpand = false;
    }
    emit focusedOut( );
    if ( _shadow != 0 && ( _config & ShowFocusShadow ) )
    {
        _shadow->setColor( _focusedOutColor );
        _shadow->update( );
    }

    update( );
}

void	GraphicsController::mouseMoveEvent( QGraphicsSceneMouseEvent* e )
{
    Q_UNUSED( e );
}

void	GraphicsController::mousePressEvent( QGraphicsSceneMouseEvent* e )
{
    QGraphicsSceneMouseEvent* mousePressEvent = ( QGraphicsSceneMouseEvent* )( e );
    QPointF pos = mapFromItem( _target, mousePressEvent->pos( ) );
    if ( maximizeBr( ).contains( pos ) )
    {
       // Maximize/minimize geometry
       _isMaximized = !_isMaximized;
       if ( _isMaximized )
           maximized( );
       else
           minimized( );
       update( );
       e->accept( );
       return;
    }
    else if ( closeBr( ).contains( pos ) )
    {
        closed( );
        e->accept( );
        return;
    }
    else if ( expandBr( ).contains( pos ) )
    {
        // Maximize/minimize geometry
        _isExpanded = !_isExpanded;
        emit expanded( _isExpanded );
        update( );
        e->accept( );
        return;
    }
    e->ignore( );
}

void	GraphicsController::mouseReleaseEvent( QGraphicsSceneMouseEvent* e )
{
    Q_UNUSED( e );
}

void    GraphicsController::forceMinimzedState( )
{
    Q_ASSERT( _targetLayout );

    _isMaximized = false;
    _minimizedGeom = _targetLayout->geometry( );
    update( );
}

QRectF  GraphicsController::maximizeBr( ) const
{
    qreal hMargin = 10.;
    qreal vMargin = 3.;
    return QRectF( boundingRect( ).topRight( ) - QPointF( ( _buttonSize.width( ) * 2. ) + ( hMargin * 2. ), -vMargin ),
                             _buttonSize );
}

QRectF  GraphicsController::closeBr( ) const
{
    qreal hMargin = 10.;
    qreal vMargin = 3.;
    return QRectF( boundingRect( ).topRight( ) - QPointF( _buttonSize.width( ) + hMargin, -vMargin ),
                             _buttonSize );
}

QRectF  GraphicsController::expandBr( ) const
{
    qreal hMargin = 10.;
    qreal vMargin = 3.;
    return QRectF( boundingRect( ).topLeft( ) + QPointF( hMargin, vMargin ), _buttonSize );
}
//-----------------------------------------------------------------------------

} // ::qan

