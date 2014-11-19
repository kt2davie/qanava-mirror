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
// \file	qanGraphItem.h
// \author	benoit@qanava.org
// \date	2007 February 08
//-----------------------------------------------------------------------------


#ifndef qanGraphItem_h
#define qanGraphItem_h

// QT headers
#include <QGraphicsItem>
#include <QGraphicsWidget>
#include <QGraphicsObject>

// QT Solutions (qtpropertybrowser) headers
#include "QtVariantProperty.h"
#include "QtTreePropertyBrowser.h"

// Qanava headers
#include "./qanStyleManager.h"
#include "./qanNode.h"


namespace qan { // ::qan

	class GraphScene;

	//! Display a qt property browser in a graphics widget.
	class PropertiesWidget : public QGraphicsWidget
	{
		Q_OBJECT

		/*! \name GraphItem Properties Management *///-------------------------
		//@{
	public:

		PropertiesWidget( GraphScene& scene, qan::Properties& properties, QGraphicsItem * parent = 0 );

        QtVariantEditorFactory*		_variantFactory;

		QtTreePropertyBrowser*		_propertyEditor;

        QGraphicsProxyWidget*		_propertiesEditorWidget;

		void				updateProperties( );

		bool				hasHover( ) const { return _hasHover; }

	protected:

		virtual void		hoverEnterEvent( QGraphicsSceneHoverEvent* e );

		virtual void		hoverMoveEvent( QGraphicsSceneHoverEvent* e );

		virtual void		hoverLeaveEvent( QGraphicsSceneHoverEvent* e );

		qan::Properties&	_properties;

		bool				_hasHover;
		//@}
		//---------------------------------------------------------------------
	};


	//! Model a styleable item (either an edge or a node) on the graph
	class GraphItem : public QGraphicsObject
	{
		/*! \name GraphItem Constructor / Destructor *///----------------------
		//@{
		Q_OBJECT

	public:

		GraphItem( GraphScene& scene, QGraphicsItem* parent = 0 );

		virtual ~GraphItem( );

		typedef QList< GraphItem* >	List;
		//@}
		//---------------------------------------------------------------------


		/*! \name GraphItem Associed Graphics Item Management *///-------------
		//@{
	public:

        void				paint( QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = 0 ) { Q_UNUSED( painter ); Q_UNUSED( option ); Q_UNUSED( widget ); }

		virtual QRectF		boundingRect( ) const { return QRectF( ); }

		//! Get the Qt graphics item associed to this Qanava graph item.
		virtual QGraphicsItem*	getGraphicsItem( ) { return 0; }

		//! Get the graph scene this graph item is registered in.
		GraphScene&				getScene( ) { return _scene; }

		//! Get the graph scene this graph item is registered in.
		const GraphScene&		getScene( ) const { return _scene; }

		//! return an anchor point for a specific edge on this item.
		virtual QPointF			getAnchor( qan::Edge& edge );

	public slots:

		//! Force updating this graph item with its actual geometry.
		virtual void			updateItem( ) { }

		//! Force updating this graph item with its actual style.
		virtual void			updateItemStyle( ) { };

		virtual	void			removeItem( ) { }

        virtual	void			expandItem( bool expand ) { Q_UNUSED( expand ); }

		//! Return the actual item expanded state (true by default).
		virtual	bool			itemExpandState( ) { return true; }

	protected:

		GraphScene&				_scene; 
		//@}
		//---------------------------------------------------------------------


		/*! \name GraphItem Style Management *///------------------------------
		//@{
	protected slots:

		virtual	void		styleModified( ) { updateItem( ); }

	protected:

		StyleManager&		_styleManager;
		//@}
		//---------------------------------------------------------------------


		/*! \name EdgeItem Properties Widget Management *///-------------------
		//@{
	protected:

		//! Call from a sub classe with properties object to activate automatic properties edition, widget could be shown always at item bottom.
		void			activatePropertiesPopup( qan::Properties& properties, int popupDelay = 150, bool showBottom = false );

		void			hidePropertiesPopup( );

		virtual void	hoverEnterEvent( QGraphicsSceneHoverEvent* e );

		virtual void	hoverMoveEvent( QGraphicsSceneHoverEvent* e );

		virtual void	hoverLeaveEvent( QGraphicsSceneHoverEvent* e );

	protected slots:

		void			showPropertiesWidget( );

		void			delayedHoverLeaveEvent( );

	public:
		
		PropertiesWidget*			_propertiesWidget;

		bool						_hovering;

		QPointF						_hoveringPos;

		bool						_showBottom;

		int							_popupDelay;
		//@}
		//---------------------------------------------------------------------

	public:
		//! Virtual factory used to create GraphItem from qan::Node, qan::Edge, etc.
		class Factory
		{
		public:

			Factory( ) { }

            virtual	GraphItem*	create( qan::GraphScene& scene, qan::Node& node, QGraphicsItem* parent );

            virtual	GraphItem*	create( qan::GraphScene& scene, qan::Edge& edge, QGraphicsItem* parent );

			virtual	QString		getTargetClassName( ) { return QString( "" ); }

			typedef QList< Factory* > List;
		};
	};
} // ::qan

#endif // qanGraphItem_h
