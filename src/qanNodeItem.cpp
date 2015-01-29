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
// \file	qanNodeItem.cpp
// \author	benoit@qanava.org
// \date	2014 November 22
//-----------------------------------------------------------------------------

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
#include <QGraphicsLayout>

// Qanava headers
#include "./qanNodeItem.h"
#include "./qanStyleManager.h"
#include "./qanGraphScene.h"

namespace qan {	// ::qan

/* LabelEditorItem Management *///---------------------------------------------
LabelEditorItem::LabelEditorItem( QString text, QString defaultText, QGraphicsItem* parent, QGraphicsLayoutItem* parentLayout ) :
    QGraphicsTextItem( text, parent ),
    QGraphicsLayoutItem( parentLayout ),
    _defaultText( defaultText )
{
    setFlag( QGraphicsItem::ItemIsMovable, false );
    setTextInteractionFlags( Qt::NoTextInteraction );	// Text interaction will be activated only in qan::GraphScene when item is double clicked
    setAcceptHoverEvents( true );

    QFont nameFont; nameFont.setPointSize( 16 );
    setFont( nameFont );
    setDefaultTextColor( QColor( 40, 40, 40, 200 ) );

    // Graphics layout item initialisation
    setGraphicsItem( this );
    setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
}

/*void	LabelEditorItem::paint( QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget )
{
    QGraphicsTextItem::paint( painter, option, widget );
    painter->setPen( Qt::red );
    painter->drawRect( boundingRect( ) );
}*/

void	LabelEditorItem::keyPressEvent( QKeyEvent* e )
{
    if ( e->matches( QKeySequence::Paste ) )
    {
        e->accept( );
        clearFocus( );
    }
    if ( e->key( ) == Qt::Key_Enter || e->key( ) == Qt::Key_Return )
    {
        emit textModified( );
        e->accept( );
        clearFocus( );
    }

    QGraphicsTextItem::keyPressEvent( e );

    prepareGeometryChange( );
    adjustSize( );
    setPreferredSize( boundingRect( ).size( ) );
    setMinimumSize( boundingRect( ).size( ) );
    if ( parentLayoutItem( ) != 0 )
        parentLayoutItem( )->updateGeometry( );
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


/* Graphics layout item implementation *///------------------------------------
void LabelEditorItem::updateGeometry( )
{
    // Force parent layout update to take into account this item new geometry
    // Note Qt 5.4.0: Should have been done automatically by Qt in base method, strange...
    if ( parentLayoutItem( ) != 0 && parentLayoutItem( )->isLayout( ) )
    {
        QGraphicsLayout* gl = static_cast< QGraphicsLayout* >(  parentLayoutItem( ) );
        gl->invalidate( ); // Do not call activate(), it cause flickering...
    }
    QGraphicsLayoutItem::updateGeometry( );
}

void LabelEditorItem::setGeometry( const QRectF& geom )
{
    prepareGeometryChange( );
    QGraphicsLayoutItem::setGeometry( geom );

    setPos( geom.topLeft( ) );
    adjustSize( );
}

QSizeF LabelEditorItem::sizeHint( Qt::SizeHint which, const QSizeF& constraint ) const
{
    switch ( which )
    {
    case Qt::MinimumSize:
        return QSizeF( 10., 10. );
        break;
    case Qt::PreferredSize:
        return boundingRect( ).size( );
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
//-----------------------------------------------------------------------------


/* NodeItem Object Management *///---------------------------------------------
NodeItem::NodeItem( GraphScene& scene, Node& node, bool isMovable, bool showPropertiesWidget ) :
    GraphItem( scene ), QGraphicsLayout( 0 ),
	_node( node ),
	_scene( scene ),
    _br( QRectF( ) ),
    _labelItem( 0 ),
    _dragOverItem( 0 ),
    _shadowColor( ),
	_shadowOffset( QPointF( 4., 4. ) ),
	_shadowEffect( 0 ),
    _itemPen( QPen( Qt::black ) ),
    _itemBrush( Qt::NoBrush ),
    _borderWidth( 1.0 ),
    _mousePos( -1. , -1. ),
    _mousePressed( false ),
	_isMovable( isMovable ),
	_isDraggable( true ),
	_dragMove( false ),
    _layout( 0 )
{
    setFlag( QGraphicsItem::ItemSendsGeometryChanges, true );
    setFlag( QGraphicsItem::ItemSendsScenePositionChanges, true );  // Catch position changes from node child of a qan::NodeGroup
    setFlag( QGraphicsItem::ItemContainsChildrenInShape, true );    // Speed up drawing and collision detection
    setFlag( QGraphicsItem::ItemIsSelectable, false );
    setAcceptDrops( false );
    setAcceptHoverEvents( false );
    setZValue( 2.0 );

    _labelItem = new LabelEditorItem( getNode( ).getLabel( ), "<< label >>", this, _layout );
    _labelItem->setZValue( zValue( ) + 2. );
    _labelItem->setPos( QPointF( 0., 0. ) );

    setGraphicsItem( this );
    setMinimumSize( QSizeF( 18., 18. ) );

    if ( showPropertiesWidget )
        activatePropertiesPopup( node.getProperties( ), 500, true );
}

NodeItem::~NodeItem( ) { }
//-----------------------------------------------------------------------------


/* NodeItem Associed Graphics Item Management *///-----------------------------
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
	QColor backColor = QColor( 255, 255, 255 );
	if ( style != 0 && style->has( "Back Color" ) )
	{
		backColor = style->getColor( "Back Color" );
        if ( backColor == Qt::white )
            backColor = QColor( 225, 225, 225 );
        QRectF br = boundingRect( );
        QLinearGradient gradient( br.topLeft( ), br.bottomRight( ) );
        gradient.setColorAt( 0., Qt::white );
        gradient.setColorAt( 1., backColor );
        //_itemBrush.setStyle( Qt::SolidPattern );
        //_itemBrush.setColor( backColor );
        _itemBrush = QBrush( gradient );
	}
	if ( style != 0 && style->has( "No Background" ) && style->get( "No Background" ).toBool( ) )
        _itemBrush.setStyle( Qt::NoBrush );

	QColor borderColor = QColor( 0, 0, 0 );
	Qt::PenStyle borderStyle = Qt::SolidLine;
	float borderWidth = 1.0; 
	if ( style != 0 && style->has( "Border Color" ) )
		borderColor = style->getColor( "Border Color" );
	if ( style != 0 && style->has( "Border Style" ) )
		borderStyle = ( Qt::PenStyle )( style->get( "Border Style" ).toInt( ) + 1 );	// +1 since 0 is noline and does not appears in style selection dialog
	if ( style != 0 && style->has( "Border Width" ) )
		borderWidth = style->get( "Border Width" ).toFloat( );
    _itemPen.setColor( borderColor );
    _itemPen.setStyle( borderStyle );
    _itemPen.setJoinStyle( Qt::RoundJoin );
    _itemPen.setWidthF( borderWidth );

    if ( style != 0 && style->has( "Border Width" ) )
        _borderWidth = style->get( "Border Width" ).toFloat( );

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
        if ( _shadowEffect != 0 )
        {
            _shadowEffect->setColor( _shadowColor );
            _shadowEffect->setOffset( _shadowOffset );
            _shadowEffect->setBlurRadius( 2.0 );
            _shadowEffect->setEnabled( true );
            setGraphicsEffect( _shadowEffect );
        }
    }
    else
    {
        if ( _shadowEffect != 0 )
            _shadowEffect->setEnabled( false );
        _shadowColor = QColor( ); // Invalid color since there is no shadow
    }

    // Compute the _label size once it is laid out as rich text html
    {
        QFont font;
        if ( style != 0 && style->has( "Font" ) )
            font = style->get( "Font" ).value< QFont >( );

        if ( _labelItem != 0 )
        {
            _labelItem->setHtml( getNode( ).getLabel( ) );
            _labelItem->setFont( font );
            _labelItem->setPreferredSize( _labelItem->boundingRect( ).size( ) );    // Call mandatory for Qt 5.4
            _labelItem->setMinimumSize( _labelItem->boundingRect( ).size( ) );    // Call mandatory for Qt 5.4
            _labelItem->adjustSize( );
            prepareGeometryChange( );
            _br.setSize( _layout->preferredSize( ) );
            // FIXME: call setGeometry( ) ?
        }
    }

    // Update edges
    foreach ( Edge* edge, _node.getOutEdges( ) )
        edge->getGraphItem( )->updateItem( );
    foreach ( Edge* edge, _node.getInEdges( ) )
        edge->getGraphItem( )->updateItem( );

    updateGeometry( );
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

	if ( !_shadowColor.isValid( ) && _shadowEffect != 0 )
		_shadowEffect->setEnabled( false );
    painter->setPen( Qt::red );
    painter->drawRect( boundingRect( ) );
}

void	NodeItem::labelTextModified( )
{
    getNode( ).setLabel( _labelItem->toPlainText( ) );
    updateItem( );
}

/* Mouse Move/Drag Management *///---------------------------------------------
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
            emit itemMoved( scenePos( ), oldPos );
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


/* Graphics layout item implementation *///------------------------------------
void NodeItem::updateGeometry( )
{
    QGraphicsLayout::updateGeometry( );

    if ( _layout != 0 )     // Update geometry has probably been called by child layout, so modify geometry according to child layout
    {
        prepareGeometryChange( );
        _br.setSize( _layout->preferredSize( ) );
    }
}

void NodeItem::setGeometry( const QRectF& geom )
{
    QGraphicsLayout::setGeometry( geom );

    // Update sublayout
    if ( _layout != 0 )
    {
        // Let sublayout compute an "ideal" geometry, then set it to this group
        /*if ( parentLayoutItem( ) != 0 &&  parentLayoutItem( )->isLayout( ) )
            _layout->setGeometry( geom );   // When there is a super layout, take the given position into account
        else                                // Necessary when a node item is added in a layout contained ino a graphics widget (eg style browser), graphics item hierarcy is no longer taken into account
            _layout->setGeometry( QRectF( QPointF( 0., 0. ), geom.size( ) ) );  // If no super layout is specified, use this node position, since layout item will be child items
        */
        qreal left, top, right, bottom;
        getContentsMargins( &left, &top, &right, &bottom );
        QSizeF s( geom.size( ) );
        s.rwidth( ) -= left + right;
        s.rheight( ) -= top + bottom;
        _layout->setGeometry( QRectF( QPointF( left, top ), s ) );  // If no super layout is specified, use this node position, since layout item will be child items
        prepareGeometryChange( );
        QSizeF layoutPrefSize( _layout->preferredSize( ) );
        _br = QRectF( QPointF( 0., 0. ), layoutPrefSize ); // (usefull to avoid resizing smaller than the sub layout content...)
    }
    setPos( geom.topLeft( ) );
}

int	NodeItem::count( ) const
{
    return 1;
}

QGraphicsLayoutItem*	NodeItem::itemAt( int i ) const
{
    return ( _layout != 0  && i == 0 ? _layout : 0 );
}

void	NodeItem::removeAt( int index )
{
    Q_UNUSED( index );
}

QSizeF NodeItem::sizeHint( Qt::SizeHint which, const QSizeF& constraint ) const
{
    switch ( which )
     {
     case Qt::MinimumSize:
        return ( _layout != 0 ? _layout->minimumSize( ) : QSizeF( ) );
        break;
     case Qt::PreferredSize:
        {
            QSizeF layoutPrefSize;
            if ( _layout != 0 )
                layoutPrefSize = _layout->preferredSize( );
            return layoutPrefSize;
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
//-----------------------------------------------------------------------------

} // ::qan

