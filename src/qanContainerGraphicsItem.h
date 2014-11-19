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


#ifndef qanContainerGraphicsItem_h
#define qanContainerGraphicsItem_h


// QT headers
#include <QGraphicsProxyWidget>
#include <QGraphicsLinearLayout>
#include <QPushButton>
#include <QGraphicsObject>

// Qanava headers
#include "./qanGraphItem.h"


//-----------------------------------------------------------------------------
namespace qan { // ::qan 

	class ContainerGraphicsItem;

	//! Model a mysire graphics view proprietary window decoration.
	class ContainerGraphicsDecoration : public QObject, public QGraphicsItem
	{
		Q_OBJECT

	public:

		ContainerGraphicsDecoration( ContainerGraphicsItem& container, QRectF dimension, Qt::AlignmentFlag flag = Qt::AlignTop, QGraphicsItem* parent = 0 );

	public:

		virtual QRectF		boundingRect () const { return _br; }
		void				setRect( QRectF r ) { _br = r; }

		virtual void		paint ( QPainter *, const QStyleOptionGraphicsItem *, QWidget * );

		void				setTitle( QString title ) { _title = title; }

		void				focusIn( );
		void				focusOut( );

		//! Update the expand button state (+ or -) according to current content satus.
		void				updateExpandButton( );

	protected:

		//! Intercept position change to update item bounding rect.
		virtual QVariant	itemChange( QGraphicsItem::GraphicsItemChange change, const QVariant& value );

	protected:

		QRectF					_br;
		Qt::AlignmentFlag		_flag;
		QString					_title;
		ContainerGraphicsItem&	_container;
		QPushButton*			_expandButton;

	private:
		QColor			_lightFocused;
		QColor			_darkFocused;
		QColor			_lightUnfocused;
		QColor			_darkUnfocused;
		QColor			_light;
		QColor			_dark;

	protected slots:

		void				expandButtonToggled( );
	};


	//! Model a mysire graphics view proprietary window (ie a window without a proxy window widget).
	class ContainerGraphicsItem : public QGraphicsObject
	{
		Q_OBJECT

	public:

		ContainerGraphicsItem( QRectF dimension, QGraphicsItem* parent = 0, qan::GraphItem* nodeItem = 0 );

		enum { Type = UserType + 41 };

		virtual int type( ) const { return Type; }

		void	setChild( QGraphicsWidget* child );

		//! Initialize container event filter after the container has been added to a scene.
		void	init( );

		//! Resize the content according to the current bounding rect.
		void	resize( );

		//! Resize the container to a given size (the content will be smaller, since decoration and margin is not taken into account).
		void	resizeContent( QSizeF size );

		//! Set the container window title.
		void	setTitle( QString title ) { _title->setTitle( title ); }

		QGraphicsWidget*	_child;
		
		virtual QRectF	boundingRect () const { return _br; }
		virtual void	paint ( QPainter *, const QStyleOptionGraphicsItem *, QWidget * ) { }
		QRectF	_br;

		ContainerGraphicsDecoration*	_title;
		ContainerGraphicsDecoration*	_bottom;
		QGraphicsRectItem*				_content;

		void	setTitleSize( QSizeF size );
		void	setBottomSize( QSizeF size );

		QSizeF	_titleSize;
		QSizeF	_bottomSize;
		QSizeF	_contentMargin;

	public:

		//! Expand, ie hide or show the item content, useded internally (but could be user-overidden).
		virtual void		expandItem( );

		//! Return true if the item is expanded (ie its content is visible), false otherwise.
		virtual	bool		isExpanded( );

	protected:

		//! Used internally to restore container size after it has been un-expanded.
		QSizeF				_contentSize;

		//! Intercept position change to update item bounding rect.
		virtual QVariant	itemChange( QGraphicsItem::GraphicsItemChange change, const QVariant& value );

		//! Eventual node item associed to this container item (used to update edges and topological features).
		qan::GraphItem*		_nodeItem;

	private:

		qreal				_zValue;		// Internally used cache for container zvalue

	protected:
		virtual void focusOutEvent ( QFocusEvent* e );		
		virtual void focusInEvent ( QFocusEvent* e );
		virtual bool sceneEventFilter( QGraphicsItem* watched, QEvent* e );

	signals:

		void		expanded( bool visible );
	};

} // ::qan
//-----------------------------------------------------------------------------


#endif // qanContainerGraphicsItem_h

