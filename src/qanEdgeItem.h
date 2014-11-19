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
// \file	qanEdgeItem.h
// \author	benoit@qanava.org
// \date	2007 February 08
//-----------------------------------------------------------------------------


#ifndef qanEdgeItem_h
#define qanEdgeItem_h


// Qanava headers
#include "./qanNode.h"
#include "./qanGraphItem.h"

// QT headers
#include <QGraphicsItem>
#include <QContextMenuEvent>
#include <QGraphicsSimpleTextItem>


namespace qan { // ::qan

	class NodeItem;

	//! Models an edge as a direct graphic line on the graph scene (edge is ended with an arrow and supports several style options).
	class EdgeItem : public GraphItem
	{
		Q_OBJECT

		/*! \name NodeItem Constructor / Destructor *///-----------------------
		//@{
	public:

		EdgeItem( GraphScene& scene, Edge& edge, QGraphicsItem* parent );

		virtual		~EdgeItem( );

		Edge&		getEdge( ) { return _edge; }
   
		enum { Type = UserType + 42 + 1 };

		virtual int	type( ) const { return Type; }

	protected:

		Edge&		_edge;
		//@}
		//---------------------------------------------------------------------


		/*! \name EdgeItem Associed Graphics Item Management *///--------------
		//@{
	public:

		virtual	QGraphicsItem*	getGraphicsItem( ) { return static_cast< QGraphicsItem* >( this ); }

	public slots:

		virtual void			updateItem( );

		virtual void			updateItemStyle( );

	protected:

		QLineF					_line;

		bool					_drawBRect;

		bool					_hasArrow;

		float					_arrowSize;

		int						_lineStyle;

		float					_lineWidth;

		QColor					_lineColor;
		//@}
		//---------------------------------------------------------------------


		/*! \name Edge Drawing Management *///---------------------------------
		//@{
	public:

		void				paint( QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = 0 );

		virtual QRectF		boundingRect( ) const;
				
		QPainterPath		shape( ) const;

	protected:

		QRectF						_br;

		QPolygonF					_bp;

		QGraphicsSimpleTextItem*	_labelItem;

	public:

		static QLineF	getLineIntersection( QLineF line, QRectF srcBr, QRectF dstBr );

		static void		drawArrow( QPainter* painter, QLineF line, QColor color, float arrowSize = 4. );
		//@}
		//---------------------------------------------------------------------

	public:

		//! Standard direct line edge factory for qan::Edge objects
		class Factory : public GraphItem::Factory
		{
		public:

			virtual	GraphItem*	create( GraphScene& scene, Edge& edge, QGraphicsItem* parent )
			{ 
				GraphItem* edgeItem = new EdgeItem( scene, edge, parent );
				edgeItem->updateItem( );
				return edgeItem;
			}

			virtual	QString		getTargetClassName( ) { return QString( "qan::Edge" ); }
		};	
	};


	//! Models an hyper edge as a standard edge connect with multiple sources and destination nodes linked to an original 2edge link.
	class HEdgeItem : public EdgeItem
	{
		Q_OBJECT

		/*! \name NodeItem Constructor / Destructor *///-----------------------
		//@{
	public:

		HEdgeItem( GraphScene& scene, HEdge& edge, QGraphicsItem* parent );

		virtual ~HEdgeItem( );

		HEdge&		getHEdge( ) { return _hEdge; }

	protected:

		HEdge&		_hEdge;
		//@}
		//---------------------------------------------------------------------


		/*! \name EdgeItem Associed Graphics Item Management *///--------------
		//@{
	public:
		virtual	QGraphicsItem*	getGraphicsItem( ) { return static_cast< QGraphicsItem* >( this ); }

		virtual void			updateItem( );

		//! Parse a given hyper edge style, and setup this item protected style properties.
		void					updateItemStyle( ); 

	protected:

		QColor					_inLineColor, _outLineColor;
		int						_inLineStyle, _outLineStyle;
		double					_hLineWidth;
		//@}
		//---------------------------------------------------------------------


		/*! \name Edge Drawing Management *///---------------------------------
		//@{
	public:

		void			paint( QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = 0 );

		virtual QRectF	boundingRect( ) const;

	protected:

		QRectF								_hLabelRect;

		QList< QLineF >						_edgeOutLines;

		QList< QGraphicsSimpleTextItem* >	_edgeOutLabels;

		QList< QLineF >						_edgeInLines;

		QList< QGraphicsSimpleTextItem* >	_edgeInLabels;
		//@}
		//---------------------------------------------------------------------

	public:

		class Factory : public GraphItem::Factory
		{
		public:

			virtual	GraphItem*	create( GraphScene& scene, Edge& edge, QGraphicsItem* parent ) 
			{
				GraphItem* hEdge = new HEdgeItem( scene, *qobject_cast< HEdge* >( &edge ), parent );
				hEdge->updateItem( );
				return hEdge;
			}

			virtual	QString		getTargetClassName( ) { return QString( "qan::HEdge" ); }
		};	
	};



	//! Model an edge on the graphic scene wich is modelled only with vertical and horizontal lines.
	class OrthoEdgeItem : public EdgeItem
	{
		Q_OBJECT

		/*! \name NodeItem Constructor / Destructor *///-----------------------
		//@{
	public:

		OrthoEdgeItem( GraphScene& scene, Edge& edge, QGraphicsItem* parent ) : EdgeItem( scene, edge, parent ) { }

		virtual ~OrthoEdgeItem( ) { }
		//@}
		//---------------------------------------------------------------------


		/*! \name Ortho Graphics Item Management *///--------------------------------
		//@{
	public:

		virtual void	updateItem( );
		//@}
		//---------------------------------------------------------------------


		/*! \name Ortho Edge Drawing Management *///---------------------------
		//@{
	public:

		void			paint( QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = 0 );
		//@}
		//---------------------------------------------------------------------	};

		class Factory : public GraphItem::Factory
		{
		public:

			virtual	GraphItem*	create( GraphScene& scene, Edge& edge, QGraphicsItem* parent ) 
				{ return new OrthoEdgeItem( scene, edge, parent ); }

			virtual	QString		getTargetClassName( ) { return QString( "qan::Edge" ); }
		};	
	};
} // ::qan

#endif // qanEdgeItem_h

