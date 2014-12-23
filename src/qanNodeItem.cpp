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
// \file	qanNodeItem.cpp
// \author	benoit@qanava.org
// \date	2004 October 13
//-----------------------------------------------------------------------------


// Qanava headers
#include "./qanNodeItem.h"
#include "./qanStyleManager.h"
#include "./qanGraphScene.h"


// QT headers
#include <QFont>
#include <QPainter>
#include <QPixmap>
#include <QGraphicsTextItem>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsTextItem>
#include <QTextEdit>
#include <QHBoxLayout>
#include <QMainWindow>
#include <QGraphicsLinearLayout>
#include <QGraphicsGridLayout>
#include <QCheckBox>
#include <QProgressBar>
#include <QPushButton>
#include <QTransform>
#include <QMimeData>


namespace qan {	// ::qan

/* LabelEditorItem Management *///---------------------------------------------
LabelEditorItem::LabelEditorItem( QString text, QString defaultText, QGraphicsItem* parent ) : 
	QGraphicsTextItem( text, parent ),
	_defaultText( defaultText )
{ 
    setFlag( QGraphicsItem::ItemIsMovable, false );
    setTextInteractionFlags( Qt::NoTextInteraction );	// Text interaction will be activated only in qan::GraphScene when item is double clicked
    setAcceptHoverEvents( true );

    QFont nameFont; nameFont.setPointSize( 16 );
	setFont( nameFont );
	setDefaultTextColor( QColor( 50, 50, 50, 200 ) );
}

void	LabelEditorItem::keyPressEvent( QKeyEvent* e )
{
	if ( e->matches( QKeySequence::Paste ) )
	{
		e->accept( );
		clearFocus( );
		return;
	}
	if ( e->key( ) == Qt::Key_Enter || e->key( ) == Qt::Key_Return )
	{
		emit textModified( );
		e->accept( );
		clearFocus( );
		return;
	}

	QGraphicsTextItem::keyPressEvent( e );
}

void	LabelEditorItem::focusOutEvent( QFocusEvent* e )
{
	if ( e->lostFocus( ) )
	{
		if ( toPlainText( ).isEmpty( ) || toPlainText( ).isNull( ) )
			setPlainText( _defaultText );
		emit textModified( );
		setTextInteractionFlags( Qt::NoTextInteraction );
	}
	QGraphicsTextItem::focusOutEvent( e );
}

void	LabelEditorItem::mousePressEvent( QGraphicsSceneMouseEvent* e )
{
	QGraphicsTextItem::mousePressEvent( e );
}
//-----------------------------------------------------------------------------



/* NodeItem Constructor/Destructor *///----------------------------------------
NodeItem::NodeItem( GraphScene& scene, Node& node, QGraphicsItem* parent, bool isMovable, bool showPropertiesWidget ) :
    SimpleNodeItem( scene, node, parent, isMovable, showPropertiesWidget ),
    _pixmapItem( 0 ),
    _labelItem( 0 ),
    _leftItem( 0 ),
    _bottomItem( 0 )
{
    setLabelItem( getNode( ).getLabel( ) );
}

NodeItem::~NodeItem( ) { }
//-----------------------------------------------------------------------------


/* NodeItem Associed Graphics Item Management *///-----------------------------
void	NodeItem::updateItem( )
{
    SimpleNodeItem::updateItem( );

    qan::Style* style = _styleManager.getStyle( getNode( ) );
    if ( style == 0 )
    {
        style = _styleManager.getTargetStyle( "qan::Node" );
        if ( style != 0 )
            _styleManager.styleNode( _node, style->getName( ) );
    }

    if ( _bottomItem != 0 )	// Set bottom item widget back color
    {
        QPalette p = _bottomItem->palette( );
        p.setColor( QPalette::Window, Qt::transparent );
        _bottomItem->setPalette( p );
    }

    // Compute the _label size once it is laid out as rich text html
    {
        QFont font;
        if ( style != 0 && style->has( "Font" ) )
            font = style->get( "Font" ).value< QFont >( );

        if ( _labelItem == 0 )
            _labelItem = new LabelEditorItem( getNode( ).getLabel( ), "<< label >>", this );
        _labelItem->setParentItem( this );
        _labelItem->setHtml( getNode( ).getLabel( ) );
        _labelItem->setFont( font );
    }

    // Update node geometry and content
    updateItemLayout( );

    /*
        // Check for item maximum dimension (if specified)
        QSizeF maxSize( -1., -1. );
        if ( style != 0 && style->has( "Maximum Size" ) )
            maxSize = style->get( "Maximum Size" ).toSizeF( );

        // Compute the item height according to the _label size once formatted and displayed
        ( _labelItem != 0 )
        {
            // Do not resize the item larger than its maximum allowed size
            double textLayoutWidth = _labelItem->boundingRect( ).width( ) + 2.;
            double textLayoutHeight = _labelItem->boundingRect( ).height( ) + 2.;
            _dimension.setX( maxSize.width( ) > 0. ? qMin( textLayoutWidth, maxSize.width( ) ) : textLayoutWidth );
            _dimension.setY( maxSize.height( ) > 0. ? qMin( textLayoutHeight, maxSize.height( ) ) : textLayoutHeight );
        }
    */
}
//-----------------------------------------------------------------------------


/* Item Layout Management *///-------------------------------------------------
void	NodeItem::updateItemLayout( )
{
	// Compute item "ideal" dimension
	QSizeF margin( 2., 2. );
	QSizeF spacing( 2., 2. );

	// Layout left, pixmap and text top items horizontally
	QList< QGraphicsItem* > items; 
	if ( _leftItem != 0 )
		items.append( _leftItem ); 
	if ( _pixmapItem != 0 )
		items.append( _pixmapItem ); 
	if ( _labelItem != 0 )
		items.append( _labelItem );
	QSizeF size( 0., 0. );
	size.rwidth( ) += margin.width( );
	qreal height = margin.height( );
	foreach ( QGraphicsItem* item, items )
	{
		QRectF itemBr = item->boundingRect( );
		//if ( item->flags( ) & QGraphicsItem::ItemIgnoresTransformations )
		//	itemBr = item->deviceTransform( scene( )->views( ).first( )->viewportTransform( ) ).mapRect( item->boundingRect( ) );
		item->setPos( size.width( ), margin.height( ) );
		size.rwidth( ) += itemBr.width( );
		if ( item != items.last( ) ) 
			size.rwidth( ) += spacing.width( );	// Always add spacing between item, except after the last one
		height = qMax( height, itemBr.height( ) + margin.height( ) );
	}
	size.rwidth( ) += margin.width( );	// Add right margin
	size.rheight( ) = height + ( _bottomItem == 0 ? margin.height( ) : spacing.height( ) );	// If their is a botom item, add spacing, otherwise, add a bottom margin

	// Layout left, pixmap and text top items vertically
	foreach ( QGraphicsItem* item, items )
	{
		QRectF itemBr = item->boundingRect( );
		//if ( item->flags( ) & QGraphicsItem::ItemIgnoresTransformations )
		//	itemBr = item->deviceTransform( scene( )->views( ).first( )->viewportTransform( ) ).mapRect( item->boundingRect( ) );

		qreal itemTop = ( size.height( ) - itemBr.height( ) ) / 2.;
		item->setPos( item->pos( ).x( ), itemTop );
	}

	// Layout bottom item horizontally and vertically
	if ( _bottomItem != 0 )
	{
		_bottomItem->setGeometry( QRectF(	margin.width( ), size.height( ), 
											size.width( ) - 2. * margin.width( ), _bottomItem->preferredHeight( ) / 3. ) );

		QRectF itemBr = _bottomItem->boundingRect( );
		if ( _bottomItem->flags( ) & QGraphicsItem::ItemIgnoresTransformations )
			itemBr = _bottomItem->deviceTransform( scene( )->views( ).first( )->viewportTransform( ) ).mapRect( _bottomItem->boundingRect( ) );

		// setGeometry could fail, take the existing bottom item size into account...
		size.rwidth( ) = qMax( size.width( ), itemBr.width( ) + 2 * margin.width( ) );
		size.rheight( ) += itemBr.height( ) + margin.height( );
	}
	_dimension = QPointF( size.width( ), size.height( ) );
	
	// Set the ideal rect for this node item 
	setLayoutRect( QRectF( 0., 0., _dimension.x( ), _dimension.y( ) ) );
	getNode( ).setDimension( _dimension );
}

void	NodeItem::setLeftItem( QGraphicsProxyWidget* leftItem )
{
	_leftItem = leftItem;
}

void	NodeItem::setPixmapItem( QGraphicsPixmapItem* pixmapItem )
{
	_pixmapItem = pixmapItem;
}

void	NodeItem::setLabelItem( QString label )
{
	if ( _labelItem == 0 )
		_labelItem = new LabelEditorItem( label, QString( "<< label >>" ), this );
	if ( _labelItem != 0 )
		_labelItem->setPlainText( label );
}

void	NodeItem::setBottomItem( QGraphicsProxyWidget* bottomItem )
{
	_bottomItem = bottomItem;
}

void	NodeItem::labelTextModified( )
{
	getNode( ).setLabel( _labelItem->toPlainText( ) );
	updateItem( );
}
//-----------------------------------------------------------------------------


/* NodeRectItem Constructor / Destructor *///----------------------------------
NodeRectItem::NodeRectItem( GraphScene& scene, Node& node, QGraphicsItem* parent, bool isMovable, bool showPropertiesWidget ) :
	NodeItem( scene, node, parent, isMovable, showPropertiesWidget )
{
	setAcceptDrops( true );
    setAcceptHoverEvents( true );   // FIXME v1.0: verify that hover event is necessary for drag and drop support

	_rectItem = new QGraphicsRectItem( getGraphicsItem( ) );
	_rectItem->setParentItem( getGraphicsItem( ) );
	_rectItem->setRect( QRectF( node.getPosition( ), QSizeF( _dimension.x( ), _dimension.y( ) ) ) );
	_rectItem->setZValue( zValue( ) - 10. );
	if ( showPropertiesWidget )
		_rectItem->setAcceptHoverEvents( true );
	setShapeItem( _rectItem );

	updateItem( );
}

NodeRectItem::~NodeRectItem( )
{

}
//-----------------------------------------------------------------------------


/* NodeRectItem Associed Graphics Item Management *///------------------------
void	NodeRectItem::setLayoutRect( QRectF layoutBr )
{
	if ( _rectItem != 0 )
		_rectItem->setRect( layoutBr );
}

QRectF	NodeRectItem::boundingRect( ) const
{
	return _rectItem->boundingRect( );
}

QPainterPath	NodeRectItem::shape( ) const
{
	return NodeItem::shape( );
}
//-----------------------------------------------------------------------------


/* Style Drag and Drop Management *///-----------------------------------------
void	NodeRectItem::dragEnterEvent( QGraphicsSceneDragDropEvent* e )
{
	if ( e->mimeData( )->hasFormat( "holographe/style" ) )
	{
		e->setAccepted( true );
	}
	else
        e->setAccepted( false );
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
	update( );
}
//---------------------------------------------------------------------


} // ::qan

