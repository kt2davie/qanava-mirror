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
// \file	qanNodeItem.h
// \author	benoit@qanava.org
// \date	2004 October 13
//-----------------------------------------------------------------------------


#ifndef canNodeItem_h
#define canNodeItem_h


// Qanava headers
#include "./qanNode.h"
#include "./qanGraphItem.h"
#include "./qanEdgeItem.h"
#include "./qanSimpleNodeItem.h"

// QT headers
#include <QGraphicsItem>
#include <QGraphicsRectItem>
#include <QGraphicsTextItem>
#include <QGraphicsProxyWidget>
#include <QGraphicsDropShadowEffect>
#include <QGraphicsSceneDragDropEvent>

// QT Solutions (qtpropertybrowser) headers
#include "QtVariantProperty.h"
#include "QtTreePropertyBrowser.h"


//-----------------------------------------------------------------------------
namespace qan { // ::qan
	
	//! Models an editable graphics label item (based on graphics text item).
	class LabelEditorItem : public QGraphicsTextItem
	{
		Q_OBJECT

		/*! \name LabelEditorItem Management *///------------------------------
		//@{
	public:

		LabelEditorItem( QString text, QString defaultText, QGraphicsItem* parent );

		enum { Type = UserType + 42 };

		virtual	int		type( ) const { return Type; }

	protected:

		virtual void	keyPressEvent( QKeyEvent* e );
		
		virtual void	focusOutEvent( QFocusEvent* e );

		virtual void	mousePressEvent( QGraphicsSceneMouseEvent* e );
		
		QString			_defaultText;

	signals:

		void			textModified( );
		//@}
		//---------------------------------------------------------------------
	};


    //! Model an abstract node item in a Qt graphics view, with built-in user defined shape support and label edition.
	/*!
		\nosubgrouping
	*/
    class NodeItem : public SimpleNodeItem
	{
		Q_OBJECT

		/*! \name NodeItem Constructor / Destructor *///-----------------------
		//@{
	public:

		NodeItem( GraphScene& scene, Node& node, QGraphicsItem* parent, bool isMovable = true, bool showPropertiesWidget = true );

		virtual ~NodeItem( );

		enum { Type = UserType + 42 + 2 };

		virtual int	type( ) const { return Type; }

    private:
        Q_DISABLE_COPY( NodeItem );
		//@}
		//---------------------------------------------------------------------


		/*! \name NodeItem Associed Graphics Item Management *///-------------
		//@{
	public slots:

		virtual void				updateItem( );
		//@}
		//---------------------------------------------------------------------


		/*! \name Item Layout Management *///----------------------------------
		//@{
	protected:

		//! Indicate to a specialized class the ideal size of the item computed in updateItemLayout().
		virtual	void			setLayoutRect( QRectF layoutBr ) = 0;

		//! Layout this item left, pixmap, label and bottom item, and return the ideal item rect trought setLayoutRect().
		void					updateItemLayout( );
		
		void					setLeftItem( QGraphicsProxyWidget* leftItem );

		void					setPixmapItem( QGraphicsPixmapItem* pixmapItem );

		void					setLabelItem( QString label );

		void					setBottomItem( QGraphicsProxyWidget* leftItem );

	protected:

		QGraphicsPixmapItem*		_pixmapItem;

		LabelEditorItem*			_labelItem;

		QGraphicsProxyWidget*		_leftItem;

		QGraphicsProxyWidget*		_bottomItem;

	protected slots:
			
		void						labelTextModified( );
		//@}
		//---------------------------------------------------------------------
	};

	class NodeRectItem : public NodeItem
	{
		Q_OBJECT

		/*! \name NodeRectItem Constructor / Destructor *///-----------------------
		//@{
	public:

		NodeRectItem( GraphScene& scene, Node& node, QGraphicsItem* parent, bool isMovable = true, bool showPropertiesWidget = true );

		virtual ~NodeRectItem( );
		//@}
		//---------------------------------------------------------------------


		/*! \name NodeItem Associed Graphics Item Management *///-------------
		//@{
	public:

		virtual QRectF		boundingRect( ) const;
		
		QPainterPath		shape( ) const;

		virtual	void		setLayoutRect( QRectF layoutBr );

	protected:

		QGraphicsRectItem*	_rectItem;
		//@}
		//---------------------------------------------------------------------


		/*! \name Style Drag and Drop Management *///--------------------------
		//@{

	protected:

		virtual void dragEnterEvent( QGraphicsSceneDragDropEvent* e );

		virtual void dragLeaveEvent( QGraphicsSceneDragDropEvent* e );

		virtual void dropEvent( QGraphicsSceneDragDropEvent* e );
		//@}
		//---------------------------------------------------------------------

	public:

		//! Standard rectangular node factory for qan::Node objects
		class Factory : public GraphItem::Factory
		{
		public:

			virtual	GraphItem*	create( GraphScene& scene, Node& node, QGraphicsItem* parent ) 
			{ 
				GraphItem* nodeItem = new NodeRectItem( scene, node, parent );
				nodeItem->updateItem( );
				return nodeItem;
			}

			virtual	QString		getTargetClassName( ) { return QString( "qan::Node" ); }
		};
	};

} // ::qan
//-----------------------------------------------------------------------------


#endif // qanNodeItem_h

