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
// \file	qanNodeItem.h
// \author	benoit@qanava.org
// \date	2014 November 22
//-----------------------------------------------------------------------------


#ifndef canNodeItem_h
#define canNodeItem_h


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
#include <QGraphicsLinearLayout>

// QT Solutions (qtpropertybrowser) headers
#include "QtVariantProperty"
#include "QtTreePropertyBrowser"


//-----------------------------------------------------------------------------
namespace qan { // ::qan

    //! Models an editable graphics label item (based on graphics text item).
    class LabelEditorItem : public QGraphicsTextItem, public QGraphicsLayoutItem
    {
        Q_OBJECT
        Q_INTERFACES( QGraphicsLayoutItem )

        /*! \name LabelEditorItem Management *///------------------------------
        //@{
    public:
        LabelEditorItem( QString text, QString defaultText,
                         QGraphicsItem* parent, QGraphicsLayoutItem* parentLayout );

        enum { Type = UserType + 42 };

        virtual	int		type( ) const { return Type; }
    private:
        Q_DISABLE_COPY( LabelEditorItem );

    protected:
        virtual void	keyPressEvent( QKeyEvent* e );

        virtual void	focusOutEvent( QFocusEvent* e );

        virtual void	mousePressEvent( QGraphicsSceneMouseEvent* e );

        QString			_defaultText;

    signals:
        void			textModified( );
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


    //! Model an abstract node item in a Qt graphics view, with built-in style and graphics layout support.
    /*! FIXME: deprecated documentation.

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
    class NodeItem : public GraphItem, public QGraphicsLayout
    {
        Q_OBJECT
        Q_INTERFACES( QGraphicsLayout )

        /*! \name NodeItem Object Management *///------------------------------
        //@{
    public:

        NodeItem( GraphScene& scene, Node& node, bool isMovable = true, bool showPropertiesWidget = true );

        virtual ~NodeItem( );

    private:
        Q_DISABLE_COPY( NodeItem )

    public:
        Node&	getNode( ) { return _node; }

        enum { Type = UserType + 42 + 3 };
        virtual int	type( ) const { return Type; }

    protected:
        Node&		_node;
        GraphScene&	_scene;
        //@}
        //---------------------------------------------------------------------


        /*! \name Graphics Item Management *///--------------------------------
        //@{
    public:
        virtual QGraphicsItem*  getGraphicsItem( ) { return static_cast< QGraphicsItem* >( this ); }

        virtual QRectF          boundingRect( ) const { return _br; }
        virtual QPainterPath	shape( ) const;

    public slots:
        virtual void            updateItem( );
        virtual void            updateItemStyle( );

    protected:
        QRectF&                 getBr( ) { return _br; }
        QRectF                  _br;
        //@}
        //---------------------------------------------------------------------


        /*! \name Graphics Drawing Management *///-----------------------------
        //@{
    public:

        virtual void    paint( QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = 0 );

    protected slots:
        void            labelTextModified( );

    protected:
        LabelEditorItem*			getLabelItem( ) { return _labelItem; }

        LabelEditorItem*			_labelItem;
        QGraphicsItem*              _dragOverItem;

        QColor						_shadowColor;
        QPointF						_shadowOffset;
        QGraphicsDropShadowEffect*	_shadowEffect;
        QPen                        _itemPen;
        QBrush                      _itemBrush;
        qreal                       _borderWidth;
        //@}
        //---------------------------------------------------------------------


        /*! \name Mouse Move/Drag Management *///------------------------------
        //@{
    public:
        void                setDraggable( bool isDraggable ) { _isDraggable = isDraggable; }
        bool                isDraggable( ) const { return _isDraggable; }
        void                setMovable( bool isMovable ) { _isMovable = isMovable; }

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

    private:
        bool				_dragMove;
        //@}
        //---------------------------------------------------------------------


        /* Graphics layout item implementation *///----------------------------
    public:
        virtual void        updateGeometry( );
        virtual void        setGeometry( const QRectF& geom );

        virtual int         count( ) const;
        virtual QGraphicsLayoutItem*    itemAt( int i ) const;
        virtual void        removeAt( int index );

    protected:
        virtual QSizeF      sizeHint( Qt::SizeHint which, const QSizeF & constraint = QSizeF( ) ) const;

        void                setLayout( QGraphicsLayout* layout ) { _layout = layout; }
        QGraphicsLayout*    getLayout( ) { return _layout; }

        QGraphicsLayout*    _layout;
        //---------------------------------------------------------------------
    };
} // ::qan
//-----------------------------------------------------------------------------


#endif // qanNodeItem_h

