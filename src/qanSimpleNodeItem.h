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
// \file	qanSimpleNodeItem.h
// \author	benoit@qanava.org
// \date	2014 November 22
//-----------------------------------------------------------------------------


#ifndef canSimpleNodeItem_h
#define canSimpleNodeItem_h


// Qanava headers
#include "./qanNode.h"
#include "./qanGraphItem.h"
#include "./qanEdgeItem.h"


// QT headers
#include <QGraphicsItem>
#include <QGraphicsRectItem>
#include <QGraphicsTextItem>
#include <QGraphicsProxyWidget>
#include <QGraphicsDropShadowEffect>
#include <QGraphicsSceneDragDropEvent>
#include <QGraphicsLayoutItem>

// QT Solutions (qtpropertybrowser) headers
#include "QtVariantProperty"
#include "QtTreePropertyBrowser"


//-----------------------------------------------------------------------------
namespace qan { // ::qan

    //! Model an abstract node item in a Qt graphics view, with built-in style and graphics layout support.
    /*!
    The following style options are supported:
    <ul>
    <li> <b>'backcolor':</b> Background color, when there is no background image defined.
    <li> <b>'bordercolor':</b> Color of the item border.
    <li> <b>'maximumwidth':</b> Maximum width of the item, content is cropped to fit this with limit.
    <li> <b>'maximumheight':</b> Maximum height of the item, content is cropped to fit this height limit.
    <li> <b>'fontsize':</b> Base size for the font used to display the item label.
    <li> <b>'hasshadow':</b> Set this value to false to supress the node shadow.
    </ul>
        \nosubgrouping
    */
    class SimpleNodeItem : public GraphItem, public QGraphicsLayoutItem
    {
        Q_OBJECT
        Q_INTERFACES( QGraphicsLayoutItem )

        /*! \name SimpleNodeItem Constructor / Destructor *///-----------------
        //@{
    public:

        SimpleNodeItem( GraphScene& scene, Node& node, QGraphicsItem* parent, bool isMovable = true, bool showPropertiesWidget = true );

        virtual ~SimpleNodeItem( );

    private:
        Q_DISABLE_COPY( SimpleNodeItem );

      public:

        Node&	getNode( ) { return _node; }

        enum { Type = UserType + 42 + 3 };

        virtual int	type( ) const { return Type; }

    protected:

        Node&		_node;

        GraphScene&	_scene;
        //@}
        //---------------------------------------------------------------------


        /*! \name NodeItem Associed Graphics Item Management *///-------------
        //@{
    public:

        virtual QGraphicsItem*		getGraphicsItem( ) { return static_cast< QGraphicsItem* >( this ); }

        virtual QRectF				boundingRect( ) const;

        virtual QPainterPath		shape( ) const;

    public slots:

        virtual void				updateItem( );

        virtual void				updateItemStyle( );
        //@}
        //---------------------------------------------------------------------


        /*! \name NodeItem Drawing Management *///-----------------------------
        //@{
    public:

        void	paint( QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = 0 );

        bool	isDraggable( ) const { return _isDraggable; }

        void	setDraggable( bool isDraggable ) { _isDraggable = isDraggable; }

        void	setMovable( bool isMovable ) { _isMovable = isMovable; }

        //! Set a concrete shape item used to draw this node item (usually a QGraphicsRectItem or QGraphicsPolygonItem).
        void	setShapeItem( QAbstractGraphicsShapeItem* shapeItem ) { _shapeItem = shapeItem; }

    protected:

        QPointF						_dimension;

        QColor						_shadowColor;

        QPointF						_shadowOffset;

        QGraphicsDropShadowEffect*	_shadowEffect;

    protected:

        virtual QVariant	itemChange( QGraphicsItem::GraphicsItemChange change, const QVariant& value );

    signals:

        void				itemMoved( QPointF pos, QPointF oldPos );

    protected:

        virtual	void		mouseMoveEvent( QGraphicsSceneMouseEvent* e );
        virtual void		mousePressEvent( QGraphicsSceneMouseEvent* e );
        virtual void		mouseReleaseEvent( QGraphicsSceneMouseEvent* e );

        QPointF				_mousePos;
        bool				_mousePressed;
        bool				_isMovable;

        bool				_isDraggable;
        bool				_dragMove;

        QAbstractGraphicsShapeItem*	_shapeItem;
        QGraphicsItem*              _dragOverItem;
        //@}
        //---------------------------------------------------------------------


        /* Graphics layout item implementation *///----------------------------
    public:
        virtual void    updateGeometry( );
        virtual void    setGeometry( const QRectF& geom );
    protected:
        virtual QSizeF  sizeHint( Qt::SizeHint which, const QSizeF & constraint = QSizeF( ) ) const;
        //---------------------------------------------------------------------
    };
} // ::qan
//-----------------------------------------------------------------------------


#endif // qanSimpleNodeItem_h

