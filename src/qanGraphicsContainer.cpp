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
// \file	qanGraphicsContainer.h
// \author	benoit@qanava.org
// \date	2014 12 09
//-----------------------------------------------------------------------------


// QT headers
#include <QApplication>
#include <QPainter>
#include <QCursor>
#include <QGraphicsProxyWidget>
#include <QGraphicsLinearLayout>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsScene>
#include <QSizePolicy>

// Qanava headers
#include "qanGraphicsResizer.h"
#include "qanGraphicsContainer.h"


namespace qan { // ::qan


//-----------------------------------------------------------------------------
GraphicsDecoration::GraphicsDecoration( QGraphicsItem* parent, QSizeF minimumSize,
                                        Config config, Qt::AlignmentFlag flag,
                                        QColor light, QColor dark ) :
    QGraphicsObject( parent ),
    QGraphicsLayoutItem( ),
    _br( QRectF( QPointF( 0., 0. ), minimumSize ) ),
    _config( config ),
	_flag( flag ),
    _titleColor( QColor( 10, 10, 10, 240 ) ),
    _title( "" ),
    _light( light ),
    _dark( dark ),
    _focused( false )
{
	setFlag( QGraphicsItem::ItemIsFocusable, true );
    setMinimumSize( minimumSize );
    setPreferredSize( minimumSize );
    setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );
}

void	GraphicsDecoration::paint( QPainter* painter, const QStyleOptionGraphicsItem*, QWidget* )
{
    QColor light = ( _focused ? _light : _light.darker( 120 ) );
    QColor dark = ( _focused ? _dark : _dark.darker( 120 ) );

    if ( _flag == Qt::AlignTop )
	{
        QRectF r = _br.adjusted( 0.5, 0.5, -0.5, -0. );
		QLinearGradient linearGrad( QPointF( 0, 0 ), QPointF( 0, 10 ) );
        linearGrad.setColorAt( 0, dark );
        linearGrad.setColorAt( 1, light );
		painter->setBrush( linearGrad );
        painter->setPen( QPen( QBrush( light ), 1.0 ) );
        if ( _config & Round )
        {
            painter->setClipRect( _br.adjusted( 0., 0., 0.5, 0.5 ) );
            r.setBottom( _br.height( ) + 5. );  // Expand br by corner height
            painter->drawRoundedRect( r, 5., 5. );
        }
        else if ( _config & Square )
            painter->drawRect( r );

		// Draw title text, adapt font to title bar dimension
		if ( !_title.isEmpty( ) )
		{
			QString title = _title;
            QRectF textBr( QRectF( 25., 0.2, _br.width( ) - 15., _br.height( ) + 1. - 0.2 ) );
			QFont font = painter->font( );
			font.setBold( false );

			// Find a correct font size according to the window title bar height
			{
				QFontMetricsF fm( font );
				qreal textWidth = fm.width( title );
				qreal textHeight = fm.height( );
				while ( textHeight > textBr.height( ) && font.pointSizeF( ) > 0.1 )			// Secured by > 0.1
				{
					font.setPointSizeF( font.pointSizeF( ) - 0.1 );
					QFontMetrics fm( font );
					textWidth = fm.width( title );
					textHeight = fm.height( );
				}
				title = fm.elidedText( _title, Qt::ElideRight, textBr.width( ) ); 
			}

            painter->setPen( QPen( _titleColor ) );
			painter->setFont( font );
			painter->drawText( textBr, title );
		}
	}
    else if ( _flag == Qt::AlignBottom )
	{
        QRectF r = _br.adjusted( 0.5, 0.0, -0.5, -0.5 );
		QLinearGradient linearGrad( QPointF( 0, 0 ), QPointF( 0, 5 ) );
        linearGrad.setColorAt( 0, dark );
        linearGrad.setColorAt( 1, light );

		painter->setBrush( linearGrad );
        painter->setPen( QPen( QBrush( light ), 1 ) );
        if ( _config & Round )
        {
            painter->setClipRect( _br.adjusted( 0., -0.5, 0.5, 0. ) );
            r.setTop( -3. );  // Expand br by corner height
            painter->drawRoundedRect( r, 3, 3 );
        }
        else if ( _config & Square )
            painter->drawRect( r );

		// Draw a separator for the resize grip bar
		painter->setPen( QPen( Qt::lightGray ) );
        painter->drawLine( QPointF( _br.width( ) - 15., 0.5 ), QPointF( _br.width( ) - 15., _br.height( ) - 1.0 ) );
	}
}

void	GraphicsDecoration::focusedIn( )
{
    _focused = true;
	update( );
}

void	GraphicsDecoration::focusedOut( )
{
    _focused = false;
	update( );
}

QVariant	GraphicsDecoration::itemChange( GraphicsItemChange change, const QVariant& value )
{
	if ( change == QGraphicsItem::ItemPositionHasChanged )
	{
	}
	return QGraphicsItem::itemChange( change, value );
}

void    GraphicsDecoration::mouseMoveEvent( QGraphicsSceneMouseEvent* e )
{
    QGraphicsSceneMouseEvent* moveEvent = ( QGraphicsSceneMouseEvent* )( e );
    {
        // Move the container
        double dx = moveEvent->pos( ).x( ) - moveEvent->lastPos( ).x( );
        double dy = moveEvent->pos( ).y( ) - moveEvent->lastPos( ).y( );
        Q_ASSERT( parentItem( ) != 0 );
        parentItem( )->moveBy( dx, dy );
    }
    // v0.9.3: Do not uncomment
    //QGraphicsObject::mouseMoveEvent( e );
}

void    GraphicsDecoration::mousePressEvent( QGraphicsSceneMouseEvent* e )
{
    _zValue = zValue( );		// Force the item on-top while moving
    setZValue( 100.0 );
    e->accept( );
    // v0.9.3: Do not uncomment
    //QGraphicsObject::mousePressEvent( e );
}

void    GraphicsDecoration::mouseReleaseEvent( QGraphicsSceneMouseEvent* e )
{
    if ( e->type( ) == QEvent::GraphicsSceneMouseRelease )
        setZValue( _zValue );
    // v0.9.3: Do not uncomment
    //QGraphicsObject::mouseReleaseEvent( e );
}
//-----------------------------------------------------------------------------


/* Graphics layout item implementation *///------------------------------------
void    GraphicsDecoration::setGeometry( const QRectF& geom )
{
    prepareGeometryChange( );
    QGraphicsLayoutItem::setGeometry( geom );

    _br = QRectF( QPointF( 0., 0. ), geom.size( ) );
    setPos( geom.topLeft( ) );
}

QSizeF  GraphicsDecoration::sizeHint( Qt::SizeHint which, const QSizeF& constraint ) const
{
    switch ( which )
    {
    case Qt::MinimumSize:
        {
            return QSizeF( 85., 45. );
        }
        break;
    case Qt::PreferredSize:
        {
            QSizeF prefSize = boundingRect( ).size( ).expandedTo( QSizeF( 85., 45. ) );
            return prefSize;
        }
        break;
    case Qt::MaximumSize:
        return QSizeF( 4096, 45. );
        break;
    case Qt::MinimumDescent:
        break;
    default:
        break;
    }
    return constraint;
}
//-----------------------------------------------------------------------------


/* GraphicsContainer Object Management *///------------------------------------
GraphicsContainer::GraphicsContainer( QRectF dimension, GraphicsDecoration::Config config, QGraphicsItem* parent ) :
	QGraphicsObject( parent ),
    QGraphicsLayout( ),
    _contentItem( 0 ),
    _title( 0 ),
    _bottom( 0 ),
    _zValue( 0. ),
    _config( config ),
    _controller( 0 ),
    _br( QRectF( ) ),
    _layout( 0 )
{
//    _config = GraphicsDecoration::Round;
    setZValue( 1.0 );
	setFlag( QGraphicsItem::ItemIsMovable, false );
    setFlag( QGraphicsItem::ItemStacksBehindParent, true );
    //setFlag( QGraphicsItem::ItemIsFocusable, true );
	setFlag( QGraphicsItem::ItemSendsGeometryChanges );
    setFlags( QGraphicsItem::ItemSendsScenePositionChanges );	// FIXME qanava v0.9.3

    configureDecorations( QSizeF( dimension.width( ), 16 ),  QSizeF( dimension.width( ), 4 ) );

    QGraphicsLayout::setGraphicsItem( this );
    QGraphicsLayout::setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding, QSizePolicy::DefaultType );

    _layout = new QGraphicsLinearLayout( Qt::Vertical, this );
    _layout->setParentLayoutItem( this );
    _layout->setSpacing( 0. );
    _layout->setContentsMargins( 0., 0., 0., 0. );
    _layout->addItem( _title );
    _layout->addItem( _bottom );

    if ( parentLayoutItem( ) == 0 )
        setGeometry( QRectF( QPointF( 0., 0. ), QSizeF( 200., 200. ) ) );
}
//-----------------------------------------------------------------------------


/* Container Content Management *///-------------------------------------------
/*!
 * \brief Container is not useable and will not handle events correctly until you call this function with an apprioriate graphics layout item.
 * \param item
 * \param itemLayout
 */
void    GraphicsContainer::setContent( QGraphicsItem* item, QGraphicsLayoutItem* itemLayout )
{
    item->setParentItem( this );

    QSizeF itemPrefSize = itemLayout->preferredSize( );
    _layout->insertItem( 1, itemLayout );
    itemLayout->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );

    _layout->invalidate( );
    setGeometry( QRectF( pos( ), itemPrefSize ) );
    invalidate( );
    _contentItem = item;

    qan::GraphicsController::Config controllerConfig;
    if ( _config & GraphicsDecoration::ShowBorder )
        controllerConfig = qan::GraphicsController::AlwaysShowAllWithBorder;
    else
        controllerConfig = qan::GraphicsController::AlwaysShowAll;
    _controller = new qan::GraphicsController( this, this, QSizeF( 12, 12 ), controllerConfig );
    _controller->setZValue( _title->zValue( ) + 1. );
    connect( _controller, SIGNAL( focusedIn( ) ), _title, SLOT( focusedIn( ) ) );
    connect( _controller, SIGNAL( focusedOut( ) ), _title, SLOT( focusedOut( ) ) );
    connect( _controller, SIGNAL( focusedIn( ) ), _bottom, SLOT( focusedIn( ) ) );
    connect( _controller, SIGNAL( focusedOut( ) ), _bottom, SLOT( focusedOut( ) ) );
    if ( _config & GraphicsDecoration::SquareWithBorder )
            _controller->setControlColor( Qt::white );

    connect( _controller, SIGNAL( expanded( bool ) ), this, SLOT( expanded( bool ) )  );
    qan::GraphicsResizer* resizer = new qan::GraphicsResizer( _controller, this, _controller ); Q_UNUSED( resizer );
}

/*!
 * \brief When overriding, do not call this base implementation to avoid creation of multiple decoration objects.
 * Protected member '_title' and '_bottom' must be initialized with subclasses of qan::GraphicsDecoration, with this
 * as a parent item.
 *
 * \param titleSize size for the title decoration (usually an height of 16 pixels).
 * \param bottomSize size for the bottom decoration (usually an height of 4 pixels).
 */
void    GraphicsContainer::configureDecorations( QSizeF titleSize, QSizeF bottomSize )
{
    QColor light = QColor( 200, 220, 240, 240 );
    QColor dark =  QColor( 240, 240, 255, 240 );

    if ( _config & GraphicsDecoration::SquareWithBorder )
    {
        dark = QColor( 150, 150, 150 );
        light = QColor( 100, 100, 100 );
        if ( _controller != 0 )
            _controller->setControlColor( Qt::white );
    }

    _title = new GraphicsDecoration( this, titleSize, _config, Qt::AlignTop, light, dark );
    if ( _config & GraphicsDecoration::SquareWithBorder )
        _title->setTitleColor( Qt::white );
    _title->setAcceptHoverEvents( true );
    _title->setFocusProxy( this );
    _title->setZValue( zValue( ) - 1.0 );

    _bottom = new GraphicsDecoration( this, bottomSize, _config, Qt::AlignBottom, light, dark );
    _bottom->setFocusProxy( this );
    _bottom->setAcceptHoverEvents( true );
    _bottom->setZValue( zValue( ) - 1.0 );
}

void    GraphicsContainer::expanded( bool isExpanded )
{
    if ( _contentItem == 0 )
        return;
    Q_ASSERT( _layout );

    if ( isExpanded && !_contentItem->isVisible( ) )
    {
        _contentItem->setVisible( true );
        _layout->invalidate( );
        setGeometry( QRectF( pos( ), _layout->geometry( ).size( ) ) );
    }
    else if ( !isExpanded && _contentItem->isVisible( ) )
    {
        _contentItem->setVisible( false );
        _layout->invalidate( );
        setGeometry( QRectF( pos( ), _layout->geometry( ).size( ) ) );
    }
}
//-----------------------------------------------------------------------------


/* Container Graphics Item Management *///-------------------------------------
void    GraphicsContainer::paint ( QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget )
{
    Q_UNUSED( painter ); Q_UNUSED( option ); Q_UNUSED( widget );
}

QVariant	GraphicsContainer::itemChange( GraphicsItemChange change, const QVariant& value )
{
    if ( change == QGraphicsItem::ItemPositionHasChanged )
    {
        // FIXME
    }
    return QGraphicsItem::itemChange( change, value );
}
//-----------------------------------------------------------------------------


/* Graphics layout item implementation *///------------------------------------
void GraphicsContainer::updateGeometry( )
{
    QGraphicsLayout::updateGeometry( );
}

void GraphicsContainer::setGeometry( const QRectF& geom )
{
    prepareGeometryChange( );
    QGraphicsLayout::setGeometry( geom );

    _layout->setGeometry( QRectF( QPointF( 0., 0. ), geom.size( ) ) );   // Let sublayout compute an "ideal" geometry, then set it to this group
    _br = QRectF( QPointF( 0., 0. ), _layout->geometry( ).size( ) ); // (usefull to avoid resizing smaller than the sub layout content...
    setPos( geom.topLeft( ) );
}

QSizeF GraphicsContainer::sizeHint( Qt::SizeHint which, const QSizeF& constraint ) const
{
    return _layout->sizeHint( which, constraint );
}

int	GraphicsContainer::count( ) const
{
    return _layout->count( );
}

QGraphicsLayoutItem*	GraphicsContainer::itemAt( int i ) const
{
    return _layout->itemAt( i );
}

void	GraphicsContainer::removeAt( int index )
{
    _layout->removeAt( index );
}
//-----------------------------------------------------------------------------


} // ::qan
