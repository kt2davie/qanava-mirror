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
// \file	qanContainerGraphicsItem.h
// \author	benoit@qanava.org
// \date	2013 02 23
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
#include "qanContainerGraphicsItem.h"


namespace qan { // ::qan


//-----------------------------------------------------------------------------
ContainerGraphicsDecoration::ContainerGraphicsDecoration( ContainerGraphicsItem& container, QRectF dimension, Qt::AlignmentFlag flag, QGraphicsItem* parent ) :
	QObject( ), QGraphicsItem( parent ),
	_br( dimension ),
	_flag( flag ),
    _title( "" ),
    _container( container ),
	_expandButton( 0 ),
	_lightFocused( QColor( 200, 220, 240, 240 ) ),
	_darkFocused( QColor( 240, 240, 255, 240 ) ),
	_lightUnfocused( QColor( 170, 190, 210, 210 ) ),
	_darkUnfocused( QColor( 210, 210, 225, 210 ) ),
	_light( _lightUnfocused ),
    _dark( _darkUnfocused )
{
	setFlag( QGraphicsItem::ItemIsFocusable, true );

	if ( flag == Qt::AlignTop )
	{
		_expandButton = new QPushButton( "+", 0 );
		_expandButton->setGeometry( 0, 0, 15, 15 );
		connect( _expandButton, SIGNAL( clicked( bool ) ), this, SLOT( expandButtonToggled( ) ) );

		QGraphicsProxyWidget* ebp = new QGraphicsProxyWidget( this );
		ebp->setWidget( _expandButton );
		ebp->setGeometry( QRectF( 5, 1, 15, 15 ) );
		ebp->setMaximumSize( 15, 15 );
	}
}

void	ContainerGraphicsDecoration::paint( QPainter* painter, const QStyleOptionGraphicsItem*, QWidget* )
{
	if ( _flag == Qt::AlignTop )
	{
		QRectF cr = _br;
		cr.setWidth( _br.width( ) );
		cr.setHeight( _br.height( ) + 1 );

		QRectF r = _br;
		r.setBottom( _br.height( ) + 5. );
		r.setRight( _br.width( ) - 1 );		// remove 1 to take pen width into account
		QLinearGradient linearGrad( QPointF( 0, 0 ), QPointF( 0, 10 ) );
		linearGrad.setColorAt( 0, _dark );
		linearGrad.setColorAt( 1, _light );

		painter->setClipRect( cr );
		painter->setBrush( linearGrad );
		//painter->setPen( QPen( QBrush( linearGrad ), 1 ) );
		painter->setPen( QPen( QBrush( _light ), 1 ) );
		painter->drawRoundedRect( r, 5, 5 );

		// Draw title text, adapt font to title bar dimension
		if ( !_title.isEmpty( ) )
		{
			QString title = _title;
			QRectF textBr( QRectF( 25., 0.1, _br.width( ) - 15., _br.height( ) + 1. - 0.2 ) );
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

			painter->setPen( QPen( QColor( 10, 10, 10, 240 ) ) );
			painter->setFont( font );
			painter->drawText( textBr, title );
		}
	}
	else
	{
		QRectF cr = _br;
		cr.setWidth( _br.width( ) );
		cr.setHeight( _br.height( ) + 1 );

		QRectF r = _br;
		r.setTop( -4. );
		r.setWidth( _br.width( ) - 1 );		// remove 1 to take pen width into account
		QLinearGradient linearGrad( QPointF( 0, 0 ), QPointF( 0, 5 ) );
		linearGrad.setColorAt( 0, _dark );
		linearGrad.setColorAt( 1, _light );

		painter->setClipRect( cr );
		painter->setBrush( linearGrad );
		//painter->setPen( QPen( QBrush( linearGrad ), 1 ) );
		painter->setPen( QPen( QBrush( _light ), 1 ) );
		painter->drawRoundedRect( r, 3, 3 );

		// Draw a separator for the resize grip bar
		painter->setPen( QPen( Qt::lightGray ) );
		painter->drawLine( QPointF( _br.width( ) - 15., 0. ), QPointF( _br.width( ) - 15., _br.height( ) ) );
	}
}

void	ContainerGraphicsDecoration::focusIn( )
{
	_light	= _lightFocused;
	_dark	= _darkFocused;
	update( );
}

void	ContainerGraphicsDecoration::focusOut( )
{
	_light	= _lightUnfocused;
	_dark	= _darkUnfocused;
	update( );
}

void	ContainerGraphicsDecoration::updateExpandButton( )
{
	if ( _container._content != 0 )
		_expandButton->setText( _container._content->isVisible( ) ? "-" : "+" );
}

QVariant	ContainerGraphicsDecoration::itemChange( GraphicsItemChange change, const QVariant& value )
{
	if ( change == QGraphicsItem::ItemPositionHasChanged )
	{
	}
	return QGraphicsItem::itemChange( change, value );
}

void	ContainerGraphicsDecoration::expandButtonToggled( )
{
	_container.expandItem( );
	updateExpandButton( );
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
ContainerGraphicsItem::ContainerGraphicsItem( QRectF dimension, QGraphicsItem* parent, qan::GraphItem* nodeItem ) : 
	QGraphicsObject( parent ),
	_child( 0 ),
    _br( dimension ),
    _title( 0 ),
    _bottom( 0 ),
    _content( 0 ),
    _titleSize( 0., 16. ),
	_bottomSize( 0., 4. ),
	_contentMargin( 0., 0. ),
	_contentSize( -1., -1. ),
	_nodeItem( nodeItem ),
	_zValue( 0. )
{
	setAcceptHoverEvents( true );
	setFlag( QGraphicsItem::ItemIsMovable, false );
	setFlag( QGraphicsItem::ItemStacksBehindParent, true );
	setFlag( QGraphicsItem::ItemIsFocusable, true );
	setFlag( QGraphicsItem::ItemSendsGeometryChanges );
	setFlags( QGraphicsItem::ItemSendsScenePositionChanges );	// FIXME qanava v0.5.0

	_title = new ContainerGraphicsDecoration( *this, QRectF( 0, 0, dimension.width( ), _titleSize.height( ) ), Qt::AlignTop, this );
	_title->setAcceptHoverEvents( true );
	_title->setFocusProxy( this );

	_bottom = new ContainerGraphicsDecoration( *this, QRectF( 0., 0., dimension.width( ), _bottomSize.height( ) ), Qt::AlignBottom, this );
	_bottom->setPos( 0., dimension.height( ) - _bottomSize.height( ) );
	_bottom->setFocusProxy( this );
	_bottom->setAcceptHoverEvents( true );

	QRectF contentSize( 0, 0, dimension.width( ), 
							  dimension.height( ) - _titleSize.height( ) - _bottomSize.height( ) - ( 2 * _contentMargin.height( ) ) );
	_content = new QGraphicsRectItem( contentSize, this );
	_content->setPos( QPointF( 0., _titleSize.height( ) + _contentMargin.height( ) ) );
	_content->setPen( QPen( Qt::NoPen) );
	_content->setFlag( QGraphicsItem::ItemStacksBehindParent, true );
	_content->setFlag( QGraphicsItem::ItemIsFocusable, true );
	_content->setFocusProxy( this );
	_title->updateExpandButton( );

	//_content->stackBefore( _bottom );
}

void ContainerGraphicsItem::init( )
{
	_title->installSceneEventFilter( this );	
	_content->installSceneEventFilter( this );	
	_bottom->installSceneEventFilter( this );
	if ( _child != 0 )
		_child->installSceneEventFilter( this );
}

void	ContainerGraphicsItem::expandItem( )
{
	if ( _content == 0 )
		return;

	if ( _content->isVisible( ) )
	{
		QPointF topLeft = _br.topLeft( );
		QSizeF decorationSize( _titleSize.width( ) + _bottomSize.width( ),  _titleSize.height( ) );
		
		_content->setVisible( false );
		if ( !_content->isVisible( ) )
		{
			// Save content size to restore it when expand is called again
			QRectF contentSize( 0, 0, _br.width( ), 
								  _br.height( ) - _titleSize.height( ) - _bottomSize.height( ) - 2 * _contentMargin.height( ) );
			_contentSize = contentSize.size( );

			prepareGeometryChange( );
			QSizeF decorationSize( qMin( 85., _br.width( ) ), _titleSize.height( ) + _bottomSize.height( ) );
			_br = QRectF( _br.topLeft( ), decorationSize );
			resize( );
		}
	}
	else
	{
		_content->setVisible( true );
		if ( !_contentSize.isEmpty( ) && _contentSize.isValid( ) )
			_content->setRect( QRectF( QPointF( 0., 0. ), _contentSize ) );
		QSizeF decorationSize( 0, _titleSize.height( ) + _bottomSize.height( ) );
		QRectF contentBr = _content->boundingRect( );
		
		if ( _child != 0 )
			contentBr = contentBr.united( _child->boundingRect( ) );
		_br = QRectF( _br.topLeft( ), decorationSize + contentBr.size( ) );
		resize( );
	}
	emit expanded( isVisible( ) );
}

bool	ContainerGraphicsItem::isExpanded( )
{
	return ( _content != 0 ? _content->isVisible( ) : true );
}

void	ContainerGraphicsItem::resize( )
{
	prepareGeometryChange( );

	// Get the content child item minimum size
	QRectF minimumSize = _br;
	if ( _child != 0 && _child->isVisible( ) )
	{
		minimumSize = QRectF( QPointF( 0., 0. ), _child->minimumSize( ) );
		minimumSize.setHeight( minimumSize.height( ) + _titleSize.height( ) + _bottomSize.height( ) + 2 * _contentMargin.height( ) );
	}

	// Compute a bounding rect according to the existing requested one and the item minimum size
	QRectF minimumBr = _br.united( minimumSize );
	if ( minimumBr.intersects( _br ) )
	{
		_br = minimumBr;

		// Adapt the container title, bottom and content size according to the bounding rect
		{
			_title->setRect( QRectF( 0, 0, _br.width( ), _titleSize.height( ) ) );

			_bottom->setPos( 0., _br.height( ) - _bottomSize.height( ) );
			_bottom->setRect( QRectF( 0., 0., _br.width( ), _bottomSize.height( ) ) );

			QRectF contentSize( 0, 0, _br.width( ), 
									  _br.height( ) - _titleSize.height( ) - _bottomSize.height( ) - 2 * _contentMargin.height( ) );
			_content->setRect( contentSize );
			_content->setPos( QPointF( 0., _titleSize.height( ) + _contentMargin.height( ) ) );
			//_contentSize = contentSize.size( );

			if ( _child != 0 )
			{
				_child->setPos( 0., 0. );
				_child->resize( _content->boundingRect( ).size( ) - QSizeF( 0., 0. ) );
			}
		}
	}
	// Update an eventual associed node item
	if ( _nodeItem != 0 )
		_nodeItem->updateItem( );
}

void	ContainerGraphicsItem::resizeContent( QSizeF size )
{
	_br.setWidth( size.width( ) );
	_br.setHeight( size.height( ) );
	resize( );
}

void ContainerGraphicsItem::setChild( QGraphicsWidget* child )
{
	_child = child;
	_child->setParentItem( _content );
	_child->setPos( 0., 0. );
	resize( );		// Force resizing to take into account an eventual child minmum size, or resize child to container dimension
	_child->installSceneEventFilter( this );
	_child->setFocusProxy( this );
	_title->updateExpandButton( );
}

void	ContainerGraphicsItem::setTitleSize( QSizeF size )
{
	_titleSize = size;
}

void	ContainerGraphicsItem::setBottomSize( QSizeF size )
{
	_bottomSize = size;
}

QVariant	ContainerGraphicsItem::itemChange( GraphicsItemChange change, const QVariant& value )
{
	if ( change == QGraphicsItem::ItemPositionHasChanged )
	{
		if ( _nodeItem != 0 )
			_nodeItem->updateItem( );
	}
	return QGraphicsItem::itemChange( change, value );
}

void	ContainerGraphicsItem::focusOutEvent ( QFocusEvent* e )
{
	_title->focusOut( );
	_bottom->focusOut( );
	QGraphicsItem::focusOutEvent( e );
}

void ContainerGraphicsItem::focusInEvent ( QFocusEvent* e )
{
	_title->focusIn( );
	_bottom->focusIn( );
	QGraphicsItem::focusInEvent( e );
}

bool ContainerGraphicsItem::sceneEventFilter( QGraphicsItem* watched, QEvent* e )
{
	if ( watched == _title )
	{
		if ( e->type( ) == QEvent::GraphicsSceneMousePress )		// Mouse press must be catched with true, or mouse move will be ignored
		{
			_zValue = zValue( );		// Force the item on-top while moving
			setZValue( 100.0 );
			return true;
		}
		if ( e->type( ) == QEvent::GraphicsSceneMouseMove )			// Handle container move when dragging the title bar
		{
			QGraphicsSceneMouseEvent* moveEvent = ( QGraphicsSceneMouseEvent* )( e );
			if ( _content != 0 && _content->isVisible( ) && cursor( ).shape( ) == Qt::SizeBDiagCursor )		// Handle resizing, only if content is visible (ie 
			{
				double dx = moveEvent->pos( ).x( ) - moveEvent->lastPos( ).x( ); 
				double dy = moveEvent->pos( ).y( ) - moveEvent->lastPos( ).y( ); 

				// Get the content child item minimum size
				QRectF minimumSize = _br.adjusted( 0, dy, dx, 0 );
				if ( _child != 0 && _child->isVisible( ) )
				{
					minimumSize = QRectF( QPointF( 0., 0. ), _child->minimumSize( ) );
					minimumSize.setHeight( minimumSize.height( ) + _titleSize.height( ) + _bottomSize.height( ) + 2 * _contentMargin.height( ) );
				}

				// Verify that the user does not resize under child item minimum size
				if ( _br.adjusted( 0, dy, dx, 0 ).contains( minimumSize ) )
				{
					prepareGeometryChange( );
					_br.adjust( 0, dy, dx, 0 );
					moveBy( 0, dy );
					resize( );
				}
			}
			else
			{	// Move the container
				double dx = moveEvent->pos( ).x( ) - moveEvent->lastPos( ).x( ); 
				double dy = moveEvent->pos( ).y( ) - moveEvent->lastPos( ).y( );
				moveBy( dx, dy );
			}
		}
		if ( e->type( ) == QEvent::GraphicsSceneMouseRelease )		
		{
			setZValue( _zValue );
			setCursor( Qt::ArrowCursor );		// Reset cursor = stop resizing
			return true;
		}
		if ( e->type( ) == QEvent::GraphicsSceneHoverMove )				// Set the resizing cursor on hover move in top right corner
		{
			QGraphicsSceneHoverEvent* hoverEvent = (QGraphicsSceneHoverEvent*)e;
			QPointF pos = mapFromItem( _title, hoverEvent->pos( ) );
			if ( pos.x( ) >= boundingRect( ).width( ) - 15. &&		// Set the resizing cursor on hover move in top right corner
				 pos.y( ) <= 15. )
			{
				setCursor( Qt::SizeBDiagCursor );
				return true;
			}
			else
				setCursor( Qt::ArrowCursor );
		}
	}
	if ( watched == _content )
	{
		if ( e->type( ) == QEvent::FocusOut )
		{
			_title->setTitle( "unfocused" );
			_title->update( );
		}
		// Return false, so the content widget can handle event unless we want to catch him on first (exemple: right/left resize)
		//e->ignore( );
		return false;
	}
	else if ( watched == _bottom )
	{
		if ( e->type( ) == QEvent::GraphicsSceneMousePress )		// Mouse press must be catched with true, or mouse move will be ignored
		{
			return true;
		}

		if ( e->type( ) == QEvent::GraphicsSceneMouseMove )
		{
			QGraphicsSceneMouseEvent* moveEvent = ( QGraphicsSceneMouseEvent* )( e );
			if ( _content != 0 && _content->isVisible( ) && cursor( ).shape( ) == Qt::SizeFDiagCursor )		// Handle resizing, only if content is visible (ie 
			{
				double dx = moveEvent->pos( ).x( ) - moveEvent->lastPos( ).x( ); 
				double dy = moveEvent->pos( ).y( ) - moveEvent->lastPos( ).y( ); 

				// Get the content child item minimum size
				QRectF minimumSize = _br.adjusted( 0, 0, dx, dy );
				if ( _child != 0 && _child->isVisible( ) )
				{
					minimumSize = QRectF( QPointF( 0., 0. ), _child->minimumSize( ) );
					minimumSize.setHeight( minimumSize.height( ) + _titleSize.height( ) + _bottomSize.height( ) + 2 * _contentMargin.height( ) );
				}

				// Verify that the user does not resize under child item minimum size
				if ( _br.adjusted( 0, 0, dx, dy ).contains( minimumSize ) )
				{
					prepareGeometryChange( );
					_br.adjust( 0, 0, dx, dy );
					resize( );
				}
			}
		}

		if ( e->type( ) == QEvent::GraphicsSceneHoverMove )				// Set the resizing cursor on hover move in bottom right corner
		{
			QGraphicsSceneHoverEvent* hoverEvent = (QGraphicsSceneHoverEvent*)e;
			QPointF pos = mapFromItem( _bottom, hoverEvent->pos( ) );
			if ( pos.x( ) >= boundingRect( ).width( ) - 10. && 
				 pos.y( ) >= boundingRect( ).height( ) - 5. )
			{
				setCursor( Qt::SizeFDiagCursor );
				return true;
			}	
			else
				setCursor( Qt::ArrowCursor );
		}

		if ( e->type( ) == QEvent::GraphicsSceneHoverLeave )			// Reset rezise cursor when the mouse hover out bottom bar
		{
			QGraphicsSceneHoverEvent* hoverEvent = (QGraphicsSceneHoverEvent*)e;
			setCursor( Qt::ArrowCursor );
		}
	}
	return QGraphicsItem::sceneEventFilter( watched, e );
}
//-----------------------------------------------------------------------------


} // ::qan
