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
// \file	qanNodeRectItem.cpp
// \author	benoit@qanava.org
// \date	2004 October 13
//-----------------------------------------------------------------------------

// QT headers
#include <QFont>
#include <QPainter>
#include <QPixmap>
#include <QGraphicsTextItem>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsTextItem>
#include <QProgressBar>
#include <QTransform>
#include <QMimeData>
#include <QLabel>
#include <QTextDocument>

// Qanava headers
#include "./qanNodeRectItem.h"
#include "./qanStyleManager.h"
#include "./qanGraphScene.h"

namespace qan {	// ::qan


/* NodeRectItem Constructor / Destructor *///----------------------------------
NodeRectItem::NodeRectItem( GraphScene& scene, Node& node, bool isMovable, bool showPropertiesWidget ) :
    NodeItem( scene, node, isMovable, showPropertiesWidget )
{
    // Graphics layout item initialisation
    QGraphicsLinearLayout* layout = new QGraphicsLinearLayout( Qt::Vertical, this );
    setLayout( layout );
    layout->setContentsMargins( 2., 2., 2., 2. );

    Q_ASSERT( getLabelItem( ) != 0 );
    getLabelItem( )->setParentLayoutItem( getLayout( ) );
    layout->addItem( getLabelItem( ) );

    prepareGeometryChange( );
    _br.setSize( getLayout( )->preferredSize( ) );
    setGeometry( QRectF( QPointF(), getLayout( )->preferredSize( ) ) );

    setAcceptDrops( true );
    setAcceptHoverEvents( true );
	updateItem( );
    setGeometry( QRectF( QPointF(), getLayout( )->preferredSize( ) ) );
}

NodeRectItem::~NodeRectItem( )
{
    if ( getLayout( ) != 0 && getLabelItem( ) != 0 )
    {
        QGraphicsLinearLayout* layout = static_cast< QGraphicsLinearLayout* >( getLayout( ) );
        layout->removeItem( getLabelItem( ) );
    }
}
//-----------------------------------------------------------------------------


/* NodeRectItem Associed Graphics Item Management *///-------------------------
void    NodeRectItem::paint( QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget )
{
    NodeItem::paint( painter, option, widget );

    Q_UNUSED( option ); Q_UNUSED( widget );

    qreal   shapeRadius( 4. );
    painter->setPen( _itemPen );
    painter->setBrush( _itemBrush );
    QRectF br = boundingRect( );
    QRectF roundedRect = br.adjusted( _borderWidth / 2., _borderWidth / 2, -_borderWidth / 2., -_borderWidth / 2);
    painter->drawRoundedRect( roundedRect, shapeRadius, shapeRadius );

    if ( _layout )
        _layout->invalidate( );
}

QPainterPath	NodeRectItem::shape( ) const
{
    QPainterPath path;
    qreal shapeRadius( 4. );
    path.addRoundedRect( boundingRect( ), shapeRadius, shapeRadius );
    return path;
}
//-----------------------------------------------------------------------------


/* Style Drag and Drop Management *///-----------------------------------------
void	NodeRectItem::dragEnterEvent( QGraphicsSceneDragDropEvent* e )
{
    e->setAccepted( e->mimeData( )->hasFormat( "holographe/style" ) );
}

void	NodeRectItem::dragLeaveEvent( QGraphicsSceneDragDropEvent* e )
{
    Q_UNUSED( e );
}

void	NodeRectItem::dropEvent( QGraphicsSceneDragDropEvent* e )
{
	if ( e->mimeData( )->hasFormat( "holographe/style" ) )
	{
		QString styleName = QString( e->mimeData( )->data( "holographe/style" ) );
		getScene( ).getStyleManager( ).styleNode( getNode( ), styleName );
	}
    updateItem( );
	update( );
}
//---------------------------------------------------------------------

} // ::qan

