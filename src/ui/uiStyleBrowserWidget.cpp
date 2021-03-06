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
// \file	uiStyleBrowserWidget.h
// \author	benoit@qanava.org
// \date	2014 June 21
//-----------------------------------------------------------------------------

// QT headers
#include <QGraphicsView>
#include <QScrollBar>
#include <QApplication>
#include <QDrag>
#include <QMimeData>

// Qanava headers
#include "../qanGraphScene.h"
#include "./uiStyleBrowserWidget.h"


namespace qan { // ::qan
namespace ui  { // ::qan::ui

/* StyleNodeItem Object Management *///----------------------------------------
StyleNodeItem::StyleNodeItem( qan::GraphScene& scene, qan::Node& node ) :
    qan::NodeRectItem( scene, node, false, false )
{
	node.setGraphicsItem( this );
	node.setGraphItem( this );
	setAcceptDrops( false );
}
//-----------------------------------------------------------------------------


/* Graphics Layout Management *///---------------------------------------------
/*void	StyleNodeItem::setGeometry ( const QRectF & rect )
{
	setPos( rect.topLeft( ) );
}

QSizeF	StyleNodeItem::sizeHint( Qt::SizeHint which, const QSizeF & constraint ) const
{
    Q_UNUSED( which ); Q_UNUSED( constraint );
	return qan::NodeRectItem::boundingRect( ).size( );
}*/
//-----------------------------------------------------------------------------


/* Node Item Mouse Management *///---------------------------------------------
void	StyleNodeItem::mouseMoveEvent( QGraphicsSceneMouseEvent* e )
{
	if ( QLineF( e->screenPos(), e->buttonDownScreenPos( Qt::LeftButton ) ).length( ) > QApplication::startDragDistance( ) ) 
	{
		qan::Style* nodeStyle = getScene( ).getStyleManager( ).getStyle( getNode( ) );
		if ( nodeStyle != 0 )
		{
			QDrag *drag = new QDrag( e->widget( ) );
			QMimeData *mime = new QMimeData( );
			mime->setData( "holographe/style", nodeStyle->getName( ).toUtf8( ) );
			drag->setMimeData( mime );

			// Create a pixmap with this style node item as content
			QRectF r = boundingRect( );
			QPixmap pixmap( r.width( ), r.height( ) );
			pixmap.fill( QColor( 0, 0, 0, 0 ) );
			QPainter painter( &pixmap );
			painter.drawRect( r );
			scene( )->render( &painter, QRectF( ), sceneBoundingRect( ) );
			painter.end( );
			pixmap = pixmap.scaled( QSize( 60, 20 ), Qt::KeepAspectRatio );

			drag->setPixmap( pixmap );
			drag->setHotSpot( QPoint( 15, 20 ) );

			drag->exec( );
		}
	}

	NodeRectItem::mouseMoveEvent( e );
}

void	StyleNodeItem::mousePressEvent( QGraphicsSceneMouseEvent* e )
{
	e->accept( );
	//NodeRectItem::mousePressEvent( e );	// Do not pass up to the NodeItem mouse move detection code
}

void	StyleNodeItem::mouseReleaseEvent( QGraphicsSceneMouseEvent* e )
{
	NodeRectItem::mouseReleaseEvent( e );
}
//-----------------------------------------------------------------------------


/* StyleBRowserWidget Object Management *///-----------------------------------
StyleBrowserWidget::StyleBrowserWidget( QGraphicsView& view, qan::GraphScene& scene, qan::StyleManager& styleManager  ) : 
	QGraphicsWidget( 0 ),
	_view( view ),
	_scene( scene ),
    _styleManager( styleManager ),
    _widthCache( -1. )
{
    connect( _view.verticalScrollBar( ), SIGNAL( valueChanged( int ) ), this, SLOT( viewViewportChanged( int ) ) );
    connect( _view.horizontalScrollBar( ), SIGNAL( valueChanged( int ) ), this, SLOT( viewViewportChanged( int ) ) );
    connect( &_view, SIGNAL( viewResized( QSize ) ), this, SLOT( viewResized( QSize ) ) );
	connect( &styleManager, SIGNAL( styleModified( qan::Style* ) ), this, SLOT( styleManagerStyleModified( qan::Style* ) ) );
	connect( &styleManager, SIGNAL( styleAdded( qan::Style* ) ), this, SLOT( styleManagerStyleAdded( qan::Style* ) ) );
	connect( &styleManager, SIGNAL( styleRemoved( qan::Style* ) ), this, SLOT( styleManagerStyleRemoved( qan::Style* ) ) );

	QPalette p = palette( );
	p.setColor( QPalette::Window, Qt::transparent );
	setPalette( p );
	
    setFlag( QGraphicsItem::ItemIgnoresTransformations, true );
	setVisible( true );
	setPos( 0., 0. );
	setZValue( 1100. );

	resetStyleItems( );
	resetStyleItemsLayout( );
    setPos( _view.mapToScene( QPoint( 100, 0 ) ) );
}

StyleBrowserWidget::~StyleBrowserWidget( )
{
	// Remove existing nodes
	foreach ( qan::NodeItem* styleNodeItem, _styleNodeItems )
	{
		_scene.removeItem( styleNodeItem );
        delete styleNodeItem;     // Node items will be destroyed by their associed layout
	}
	_styleNodeItems.clear( );
	foreach ( qan::Node* styleNode, _styleNodes )
		delete styleNode;
	_styleNodes.clear( );
}

void	StyleBrowserWidget::resetStyleItems( )
{
	// Remove existing nodes
    foreach ( qan::NodeItem* styleNodeItem, _styleNodeItems )
    {
        _scene.removeItem( styleNodeItem );
        delete styleNodeItem;
    }
    _styleNodeItems.clear( );

	foreach ( qan::Node* styleNode, _styleNodes )
	{
		_styleManager.clearNodeStyle( *styleNode );
		delete styleNode;
	}
	_styleNodes.clear( );

	foreach ( qan::Style* style, _styleManager.getStyles( ) )
	{
		if ( style->getTarget( ) != "qan::Node" )
			continue;
		qan::Node* node = new qan::Node( style->getName( ) );
		_styleNodes.append( node );
		_styleManager.styleNode( *node, style->getName( ) );

        // Create a rect item to display styles, without using qan::Scene
        // to avoid adding "virtual" in graph topology
        NodeItem* styleItem = new StyleNodeItem( _scene, *node );
        _scene.addItem( styleItem );
        styleItem->setParentItem( this );
        _styleNodeItems.append( styleItem );
        node->setGraphicsItem( styleItem );
        node->setGraphItem( styleItem );
        styleItem->updateItem( );   // Mandatory
    }
}

void	StyleBrowserWidget::resetStyleItemsLayout( )
{
    _widthCache = _view.width( ); // Keep the viewport witdh in cache, layout will be valid until it change

    // Compute column width
    qreal maxWidth = 0.;
    qreal maxHeight = -1.;
    foreach ( qan::NodeItem* styleNodeItem, _styleNodeItems )
    {
        maxWidth = qMax( styleNodeItem->boundingRect( ).width( ), maxWidth );
        maxHeight = qMax( styleNodeItem->boundingRect( ).height( ), maxHeight );
    }

    int itemsByRow = ( _view.width( )/*viewport( )->size( ).width( )*/ - 100.0 ) / maxWidth;		// 100.0 is a margin
    if ( itemsByRow <= 0 )
        return;

    qreal margin = 7.;
    int i = 0;
    foreach ( qan::NodeItem* styleNodeItem, _styleNodeItems )
    {
        int c = i % itemsByRow;
        int r = i / itemsByRow;

        qreal x = margin + ( c * ( maxWidth + margin ) ) + ( ( maxWidth - styleNodeItem->boundingRect( ).width( )  ) / 2 );
        qreal y = margin + ( r * ( maxHeight + margin ) ) + ( ( maxHeight - styleNodeItem->boundingRect( ).height( ) ) / 2 );
        styleNodeItem->setPos( x, y );
        i++;
    }
}
//-----------------------------------------------------------------------------


/* Style Management *///-------------------------------------------------------
void	StyleBrowserWidget::viewViewportChanged( int value )
{
    Q_UNUSED( value );
    if ( _widthCache < 0 || !qFuzzyCompare( 1.0 + _widthCache, 1.0 + _view.width( ) ) )
        resetStyleItemsLayout( );       // Do not layout again if width has not been modified (ex: slot called because of a scroll bar move...=
    setPos( _view.mapToScene( QPoint( 100, 0 ) ) );
}

void	StyleBrowserWidget::viewResized( QSize s )
{
    Q_UNUSED( s );
    resetStyleItemsLayout( );
	setPos( _view.mapToScene( QPoint( 100, 0 ) ) );
}

void	StyleBrowserWidget::styleManagerStyleModified( qan::Style* style )
{
    Q_UNUSED( style );
    // Force updating node item, since these nodes are not taken into account by main graph scene
	foreach ( qan::NodeItem* styleNodeItem, _styleNodeItems )
		styleNodeItem->updateItem( );
}

void	StyleBrowserWidget::styleManagerStyleAdded( qan::Style* style )
{
    Q_UNUSED( style );
	resetStyleItems( );
	resetStyleItemsLayout( );
}

void	StyleBrowserWidget::styleManagerStyleRemoved( qan::Style* style )
{
    Q_UNUSED( style );
    resetStyleItems( );
	resetStyleItemsLayout( );
}
//-----------------------------------------------------------------------------

} // ::qan::ui
} // ::qan
