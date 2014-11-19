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
	QFont nameFont; nameFont.setPointSize( 16 );
	setFont( nameFont );
	setDefaultTextColor( QColor( 50, 50, 50, 200 ) );
	setTextInteractionFlags( Qt::NoTextInteraction );	// Text interaction will be activated only in qan::GraphScene when item is double clicked

	setAcceptHoverEvents( true );
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
/*!
	The following style options are supported:
	<ul>
	<li> <b>'backcolor':</b> Background color, when there is no background image defined.
	<li> <b>'bordercolor':</b> Color of the item border.
	<li> <b>'backimage':</b> Background image (scaled to fit the item size).
	<li> <b>'maximumwidth':</b> Maximum width of the item, content is cropped to fit this with limit.
	<li> <b>'maximumheight':</b> Maximum height of the item, content is cropped to fit this height limit.
	<li> <b>'fontsize':</b> Base size for the font used to display the item label.
	<li> <b>'icon':</b> Display an icon centered in the left of the item.
	<li> <b>'hasshadow':</b> Set this value to false to supress the node shadow.
	</ul>
	
	An item with an empty style is transparent with no background and border.
 */
NodeItem::NodeItem( GraphScene& scene, Node& node, QGraphicsItem* parent, bool isMovable, bool showPropertiesWidget ) : 
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
    _dragOverItem( 0 ),
    _shapeItem( 0 ),
    _pixmapItem( 0 ),
    _labelItem( 0 ),
    _leftItem( 0 ),
    _bottomItem( 0 )
{
	setFlag( QGraphicsItem::ItemSendsGeometryChanges );
	setFlag( QGraphicsItem::ItemSendsScenePositionChanges );	// Catch position changes from node child of a qan::NodeGroup
	setZValue( 2.0 );
	setAcceptDrops( false );

	if ( showPropertiesWidget )
		activatePropertiesPopup( node.getProperties( ), 500, true );

	setLabelItem( getNode( ).getLabel( ) );
}

NodeItem::~NodeItem( ) { }
//-----------------------------------------------------------------------------


/* NodeItem Associed Graphics Item Management *///-----------------------------
QRectF	NodeItem::boundingRect( ) const
{
	QRectF br( 0., 0., _dimension.x( ), _dimension.y( ) );
	if ( _shapeItem != 0 )
		br = br.united( mapFromItem( _shapeItem, _shapeItem->boundingRect( ) ).boundingRect( ) );
	return br;
}

QPainterPath	NodeItem::shape( ) const
{
	QPainterPath qpp;
	qpp.addRect( boundingRect( ) );
	return qpp;
}

void	NodeItem::updateItem( )
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
	if ( _bottomItem != 0 )	// Set bottom item widget back color
	{
		QPalette p = _bottomItem->palette( );
		p.setColor( QPalette::Window, Qt::transparent );
		_bottomItem->setPalette( p );
	}

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

	// Updating node geometry and content
	{
		updateItemLayout( );

		// Check for item maximum dimension (if specified)
		QSizeF maxSize( -1., -1. );
		if ( style != 0 && style->has( "Maximum Size" ) )
			maxSize = style->get( "Maximum Size" ).toSizeF( );

		// Compute the item height according to the _label size once formatted and displayed
		/*if ( _labelItem != 0 )
		{
			// Do not resize the item larger than its maximum allowed size
			double textLayoutWidth = _labelItem->boundingRect( ).width( ) + 2.;
			double textLayoutHeight = _labelItem->boundingRect( ).height( ) + 2.;
			_dimension.setX( maxSize.width( ) > 0. ? qMin( textLayoutWidth, maxSize.width( ) ) : textLayoutWidth );
			_dimension.setY( maxSize.height( ) > 0. ? qMin( textLayoutHeight, maxSize.height( ) ) : textLayoutHeight );
		}*/

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
}

void	NodeItem::updateItemStyle( )
{
	updateItem( );
}
//-----------------------------------------------------------------------------


/* NodeItem Drawing Management *///--------------------------------------------
void	NodeItem::paint( QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget )
{
    Q_UNUSED( painter ); Q_UNUSED( option ); Q_UNUSED( widget );
	updateItemLayout( );

	if ( !_shadowColor.isValid( ) && _shadowEffect != 0 )
		_shadowEffect->setEnabled( false );
}

QVariant	NodeItem::itemChange( GraphicsItemChange change, const QVariant& value )
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

void	NodeItem::mouseMoveEvent( QGraphicsSceneMouseEvent* e )
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

void	NodeItem::mousePressEvent( QGraphicsSceneMouseEvent* e )
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

void	NodeItem::mouseReleaseEvent( QGraphicsSceneMouseEvent* e )
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

