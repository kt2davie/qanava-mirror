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
// \file	qanNode.h
// \author	benoit@qanava.org
// \date	2004 February 15
//-----------------------------------------------------------------------------


#ifndef qanNode_h
#define qanNode_h


// Qanava headers
#include "./qanEdge.h"
#include "./qanProperties.h"


// QT headers
#include <QPointF>
#include <QList>
#include <QSet>
#include <QDateTime>
#include <QGraphicsItem>


//-----------------------------------------------------------------------------
//! Root qanava namespace 
namespace qan { // ::qan

		//! Model a node in a standard weighted and directed graph.
		/*!
			\nosubgrouping
		*/
		class Node : public QObject
		{
			Q_OBJECT

			/*! \name Node Constructor/Destructor *///-------------------------
			//@{
		public:

			//! Node constructor with label initialisation.
            Node( QString label );

			//! Node destructor.
			virtual ~Node( ) { emit destroyed( ); }

		private:

			Node( const Node& n );
			//@}
			//-----------------------------------------------------------------



			/*! \name Node Edges Management *///-------------------------------
			//@{
		public:

			//! Typedef for a QT list of pointer on Node.
			typedef QList< Node* >	List;

			//! Typedef for a QT set of pointer on Node.
			typedef QSet< Node* >	Set;

			//! Get a list of all nodes pointing to this node.
			const Edge::List&	getInEdges( ) const { return _inEdges; }

			//! Get a list of all nodes pointing to this node.
			Edge::List&			getInEdges( ) { return _inEdges; }

			//! Get a list of all node pointed by this node.
			const Edge::List&	getOutEdges( ) const { return _outEdges; }

			//! Get a list of all node pointed by this node.
			Edge::List&			getOutEdges( ) { return _outEdges; }

			//! Collect a list of this node sub nodes.
			void				collectOutNodes( Node::List& outNodes );

			//! Collect a list of this node in nodes.
			void				collectInNodes( Node::List& outNodes );

			//! Collect a set of unique nodes pointed by this node.
			void				collectOutNodesSet( Node::Set& outNodes ) const;

			//! Collect a set of unique nodes referencing this node.
			void				collectInNodesSet( Node::Set& nodes ) const;

			//! Add an in edge.
			void				addInEdge( Edge& edge ) { _inEdges << &edge; emit edgeInInserted( edge ); }

			//! Add an out edge.
			void				addOutEdge( Edge& edge ) { _outEdges << &edge; emit edgeOutInserted( edge );  }

			//! Get node in degree.
			unsigned int		getInDegree( ) const { return _inEdges.size( ); }

			//! Get node out degree.
			unsigned int		getOutDegree( ) const { return _outEdges.size( ); }

			//! Get a list of nodes adjacent to this (all in and out nodes, without this).
			void				getAdjacentNodesSet( Node::Set& nodes ) const;

			//! Get a list of nodes non adjacent to this (all nodes minus the adjacent node set collected with getAdjacentNodesSet()).
			void				getNonAdjacentNodesSet( Node::Set& nonAdjacentNodes, const Node::Set& graphNodes ) const;

			//! Return true if this node is a "leaf" (ie has no out edges).
			bool				isLeaf( ) const { return _outEdges.size( ) == 0; }

		private:

			//! Input edges.
			Edge::List			_inEdges;

			//! Output edges.
			Edge::List			_outEdges;
			//@}
			//-----------------------------------------------------------------


			/*! \name Node Property Management *///----------------------------
			//@{
		public:

			//! Fast access to this node associed graphics item (it is usually better to use the qan::GraphScene interface to access the associed graphics item).
			QGraphicsItem*		getGraphicsItem( ) { return _graphicsItem; }

			//! Used internally.
			void				setGraphicsItem( QGraphicsItem* graphicsItem ) { _graphicsItem = graphicsItem; }

			//! Fast access to this node associed graphics item (it is usually better to use the qan::GraphScene interface to access the associed graphics item).
			GraphItem*			getGraphItem( ) { return _graphItem; }

			//! Used internally.
			void				setGraphItem( GraphItem* graphItem ) { _graphItem = graphItem; }

		public:

			QGraphicsItem*		_graphicsItem;

			qan::GraphItem*		_graphItem;
			//@}
			//-----------------------------------------------------------------


			/*! \name Node Property Management *///----------------------------
			//@{
		public:
			
			const QString&	getLabel( ) const { return _label; }

			void			setLabel( const QString& label ) { _label = label; }

			QPointF&		getPosition( ) { return _position; }

			const QPointF&	getPosition( ) const { return _position; }

			void			setPosition( QPointF position ) { _position = position; }

			const QPointF&	getDimension( ) const { return _dimension; }

			void			setDimension( const QPointF& dimension ) { _dimension = dimension; }

			Properties&		getProperties( ) { return _properties; }

		protected:

			QString		_label;

			QPointF		_position;

			QPointF		_dimension;

			Properties	_properties;
			//@}
			//-----------------------------------------------------------------



			/*! \name Node Signals Management *///-----------------------------
			//@{
		signals:
			
			void	edgeInInserted( qan::Edge& );

			void	edgeOutInserted( qan::Edge& );

			void	destroyed( );
			//@}
			//-----------------------------------------------------------------
		};
} // ::qan
//-----------------------------------------------------------------------------


#endif // qanNode_h
