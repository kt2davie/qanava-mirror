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
// \file	qanGraphicsContainer.h
// \author	benoit@qanava.org
// \date	2014 12 09
//-----------------------------------------------------------------------------

#ifndef qanGraphicsContainer_h
#define qanGraphicsContainer_h

// QT headers
#include <QGraphicsProxyWidget>
#include <QGraphicsLinearLayout>
#include <QGraphicsObject>
#include <QGraphicsDropShadowEffect>

// Qanava headers
#include "qanGraphicsController.h"


//-----------------------------------------------------------------------------
namespace qan { // ::qan 

    class GraphicsContainer;

    //! .
    class GraphicsDecoration : public QGraphicsObject, public QGraphicsLayoutItem
	{
		Q_OBJECT

    public:
        /*! Define graphics decoration look and feel.  */
        enum Config {
            Round = 2,
            Square = 4,
            ShowBorder = 8,
            SquareWithBorder = Square | ShowBorder
        };

	public:
        GraphicsDecoration( QGraphicsItem* parent, QSizeF minimumSize, Config config = Round, Qt::AlignmentFlag flag = Qt::AlignTop,
                            QColor light = QColor( 200, 220, 240, 240 ), QColor dark =  QColor( 240, 240, 255, 240 ) );
        virtual ~GraphicsDecoration( ) { }

    private:
        Q_DISABLE_COPY( GraphicsDecoration );

	public:
		virtual QRectF		boundingRect () const { return _br; }
		void				setRect( QRectF r ) { _br = r; }

		virtual void		paint ( QPainter *, const QStyleOptionGraphicsItem *, QWidget * );

		void				setTitle( QString title ) { _title = title; }
        void                setTitleColor( QColor titleColor ) { _titleColor = titleColor; }

    public slots:
        void				focusedIn( );
        void				focusedOut( );

	protected:
		//! Intercept position change to update item bounding rect.
		virtual QVariant	itemChange( QGraphicsItem::GraphicsItemChange change, const QVariant& value );

        virtual void        mouseMoveEvent(QGraphicsSceneMouseEvent * event);
        virtual void        mousePressEvent(QGraphicsSceneMouseEvent * event);
        virtual void        mouseReleaseEvent(QGraphicsSceneMouseEvent * event);
        qreal				_zValue;		// Internally used cache for container zvalue

	protected:
        QRectF				_br;
        Config              _config;
        Qt::AlignmentFlag	_flag;
        QColor              _titleColor;
        QString				_title;

	private:
		QColor			_light;
		QColor			_dark;
        bool            _focused;

        /*! \name Graphics layout item implementation *///---------------------
        //@{
    public:
        virtual void    setGeometry( const QRectF& geom );
    protected:
        virtual QSizeF  sizeHint( Qt::SizeHint which, const QSizeF & constraint = QSizeF( ) ) const;
        //@}
        //---------------------------------------------------------------------
	};


    //! Model a pure graphics item based window to contain an arbitray graphics layout based item (such as a graphics widget for example).
    class GraphicsContainer : public QGraphicsObject, public QGraphicsLayout
	{
		Q_OBJECT

        /*! \name GraphicsContainer Object Management *///---------------------
        //@{
    public:
        GraphicsContainer( QRectF dimension, GraphicsDecoration::Config config = GraphicsDecoration::SquareWithBorder, QGraphicsItem* parent = 0 );
        virtual ~GraphicsContainer( ) { }

    private:
        Q_DISABLE_COPY( GraphicsContainer );

    public:
		enum { Type = UserType + 41 };
        virtual int         type( ) const { return Type; }
        //@}
        //---------------------------------------------------------------------


        /*! \name Container Content Management *///----------------------------
        //@{
    public:
        //! Add a graphics item with a corresponding graphics layout in this container (usually, the two pointers are the same...).
        void                setContent( QGraphicsItem* item, QGraphicsLayoutItem* itemLayout );

		//! Set the container window title.
        void                setTitle( QString title ) { _title->setTitle( title ); }

    protected:
        //! Could be overriden by user to configure decoration creation, use extremely carrefully, since it is a virtual method called from the constructor.
        virtual void        configureDecorations( QSizeF titleSize, QSizeF bottomSize );

        QGraphicsItem*      _contentItem;
        GraphicsDecoration*	_title;
        GraphicsDecoration*	_bottom;
        qreal				_zValue;		// Internally used cache for container zvalue
        GraphicsDecoration::Config  _config;
        qan::GraphicsController*    _controller;

    protected slots:
         void               expanded( bool isExpanded );
        //@}
        //---------------------------------------------------------------------


        /*! \name Container Graphics Item Management *///----------------------
        //@{
    public:
        virtual QRectF      boundingRect( ) const { return _br; }
        virtual void        paint ( QPainter *, const QStyleOptionGraphicsItem *, QWidget * );

	protected:
		//! Intercept position change to update item bounding rect.
		virtual QVariant	itemChange( QGraphicsItem::GraphicsItemChange change, const QVariant& value );

        QRectF              _br;
        //@}
        //---------------------------------------------------------------------


        /*! \name Graphics layout item implementation *///---------------------
        //@{
    public:
        virtual void            updateGeometry( );
        virtual void            setGeometry( const QRectF& geom );

        virtual int             count( ) const;
        virtual QGraphicsLayoutItem*    itemAt( int i ) const;
        virtual void            removeAt( int index );

    protected:
        virtual QSizeF          sizeHint( Qt::SizeHint which, const QSizeF & constraint = QSizeF( ) ) const;

    private:
        QGraphicsLinearLayout*  _layout;
        //@}
        //---------------------------------------------------------------------
    };

} // ::qan
//-----------------------------------------------------------------------------


#endif // qanGraphicsContainer_h

