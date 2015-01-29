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
// \file	qanNodeRectItem.h
// \author	benoit@qanava.org
// \date	2004 October 13
//-----------------------------------------------------------------------------


#ifndef canNodeRectItem_h
#define canNodeRectItem_h

// QT headers
#include <QGraphicsItem>
#include <QGraphicsRectItem>
#include <QGraphicsTextItem>
#include <QGraphicsProxyWidget>
#include <QGraphicsDropShadowEffect>
#include <QGraphicsSceneDragDropEvent>

// QT Solutions (qtpropertybrowser) headers
#include "QtVariantProperty"
#include "QtTreePropertyBrowser"

// Qanava headers
#include "./qanNode.h"
#include "./qanGraphItem.h"
#include "./qanEdgeItem.h"
#include "./qanNodeItem.h"

//-----------------------------------------------------------------------------
namespace qan { // ::qan

	class NodeRectItem : public NodeItem
	{
		Q_OBJECT

		/*! \name NodeRectItem Constructor / Destructor *///-----------------------
		//@{
	public:
        NodeRectItem( GraphScene& scene, Node& node, bool isMovable = true, bool showPropertiesWidget = true );
        virtual ~NodeRectItem( );
		//@}
		//---------------------------------------------------------------------


		/*! \name NodeItem Associed Graphics Item Management *///-------------
		//@{
	public:
        virtual void	paint( QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = 0 );

        QPainterPath	shape( ) const;
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

            virtual	GraphItem*	create( GraphScene& scene, Node& node )
			{ 
                GraphItem* nodeItem = new NodeRectItem( scene, node );
				nodeItem->updateItem( );
				return nodeItem;
			}

			virtual	QString		getTargetClassName( ) { return QString( "qan::Node" ); }
		};
	};

} // ::qan
//-----------------------------------------------------------------------------


#endif // qanNodeRectItem_h

