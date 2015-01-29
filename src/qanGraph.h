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
// \file	qanGraph.h
// \author	benoit@qanava.org
// \date	2004 February 15
//-----------------------------------------------------------------------------


#ifndef qanGraph_h
#define qanGraph_h


// Qanava headers
#include "./qanEdge.h"
#include "./qanNode.h"
#include "./qanGraphScene.h"
#include "./qanStyleManager.h"


// QT headers
#include <QMap>
#include <QList>
#include <QStandardItemModel>


//-----------------------------------------------------------------------------
namespace qan { // ::qan

	//! Model a standard weighted directed graph using a node-list, edge-list representation.
	/*!
		\nosubgrouping
	*/
	class Graph : public QObject
		{
			Q_OBJECT

		public:

			/*! \name Graph Constructor/Destructor *///------------------------
			//@{
		public:

			//! Graph default constructor.
			Graph( );

			//! Graph default destructor. All registered nodes and edges are invalid after graph destruction.
			virtual ~Graph( );

		private:
            Q_DISABLE_COPY( Graph );
			//@}
			//-----------------------------------------------------------------


			/*! \name Listeners and Models Management *///---------------------
			//@{
		public:

			GraphScene&			getM( ) { return _m; }
			const GraphScene&	getM( ) const { return _m; }

			GraphModel&			getO( ) { return _o; }
			const GraphModel&	getO( ) const { return _o; }

			//! Take care of updating models when topology has been initialized using create* methods.
			/*! This method is usualy usefull before connecting a graph to a view when it has been initialized
				whitout using the insert methods who are automatically taking care of updating models.	*/
			void				updateModels( );

			//! Add a listener to this graph, graph does not get ownership for the listener.
			void				addListener( GraphListener& listener ) { _listeners.append( &listener ); }

		private:

			GraphScene			_m;
			GraphModel			_o;

			GraphListener::List	_listeners;
			//@}
			//-----------------------------------------------------------------



			/*! \name Edge/Node Management *///--------------------------------
			//@{
		public:

			//! Clear the graph from all its edges and nodes.
			void			clear( );

			Node*			insertNode( Node* node, QString styleName = "" );

            Node*			insertNode( QString name, QString styleName = "" );

			void			removeNode( Node& node );

			Edge*			insertEdge( Node& src, Node& dst, QString styleName = "", float weight = 1.f, bool generatePos = false );

			HEdge*			insertHEdge( Node& src, Node& dst, Node::List& hSrc, Node::List& hDst, QString styleName = "" );

			HEdge*			insertHInEdge( Node& src, Node& dst1, Node& hSrc, QString hSrcLabel = "", QString styleName = "" );

			HEdge*			insertHOutEdge( Node& src, Node& dst1, Node& hDst, QString hDstLabel = "", QString styleName = "" );

			void			removeEdge( Edge& edge );

			bool			hasEdge( Node& a, Node& b ) const;

		private:

			//! List of nodes currently registered in this graph.
			Node::List		_nodes;

			//! Set of nodes currently registered in this graph (used for fast node search).
			Node::Set		_nodesSet;

			//! List of edges currently connecting the registered nodes in this graph.
			Edge::List		_edges;
			//@}
			//-----------------------------------------------------------------



			/*! \name Graph Edge/Node access *///------------------------------
			//@{
		public:

			//! Get graph node count.
			unsigned int	getNodeCount( ) const { return _nodes.size( ); }

			//! Get graph's nodes list (list must be used read-only).
			Node::List&		getNodes( ) { return _nodes; }

			//! Get graph's edges list (list must be used read-only).
			Edge::List&		getEdges( ) { return _edges; }

			//! Find an existing edge between a given source and destination node.
			Edge*			findEdge( Node& src, Node& dst );

			//! Find an existing hyper edge between a given source and destination node.
			HEdge*			findHEdge( Node& src, Node& dst );

			//! Collect a set of unique node registered in this graph.
			void			collectNodes( Node::Set& nodes ) const;
			//@}
			//-----------------------------------------------------------------



			/*! \name Graph Search Management *///-----------------------------
			//@{
		public:

			//! Find a node of a given label in the graph.
			Node*			findNode( const QString& label );

			//! Find the nth registered node in this graph (For internal use only).
			/*! This method should only be used in repositories just after node loading when their initial order has not been altered. */
			Node*			findNode( int nodeIndex );

			//! Get the index where a node is currently registered in this graph.
			/*! This method should only be used in repositories just after node loading when their initial order has not been altered. */
			int				findNode( const Node& node ) const;

			//! Find a node object for a specific node (if such an object has been specified during node adjunction).
			void*			findObject( Node* node );

			//! Search for a specific node in this graph and return true if the node is found.
			bool			hasNode( Node* node ) const;
			//@}
			//-----------------------------------------------------------------



			/*! \name Root Node Management *///--------------------------------
			//@{
		public:

			//! Get the currently registered root node for this graph.
			Node::List&		getRootNodes( ) { return _rootNodes; }

			//! Get the currently registered root node for this graph.
			Node::Set&		getRootNodesSet( ) { return _rootNodesSet; }

			//! Generate the root set by automatically looking for nodes with no super nodes (O(n), n numbers of nodes).
			void			generateRootNodes( );

			//! Test if a given node is a graph root node.
			bool			isRootNode( const Node& node ) const;

		protected:

			//! Add a root to this graph (ie a node that doesn't have super nodes.)
			/*! \param node must be already registered in the graph. 		*/
			void			addRootNode( Node& node );

			//! Remove a node from the root node list
			void			removeRootNode( Node& node );

		private:

			//! Ordered root nodes for this graph's subgraphs.
			Node::List		_rootNodes;

			//! Root nodes of graph's subgraphs (used only to test root node existence).
			Node::Set		_rootNodesSet;
			//@}
			//-----------------------------------------------------------------



			/*! \name Style Management *///------------------------------------
			//@{
		public:

			qan::StyleManager&	getStyleManager( ) { return _styleManager; }

		private:

			qan::StyleManager	_styleManager;
			//@}
			//-----------------------------------------------------------------
		};
} // ::qan
//-----------------------------------------------------------------------------


#endif // qanGraph_h

