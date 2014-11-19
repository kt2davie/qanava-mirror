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
// \file	qanGraph.cpp
// \author	benoit@qanava.org
// \date	2004 February 15
//-----------------------------------------------------------------------------

// Qt headers
#include <QDateTime>

// Qanava headers
#include "./qanGraph.h"


namespace qan { // ::qan


/* GraphT Constructor/Destructor *///-------------------------------------------
Graph::Graph( ) :
	_m( _styleManager ), 
    _o( _styleManager ),
    _styleManager( this, this )
{
	_listeners.append( &_m );
	_listeners.append( &_o );
	foreach ( GraphListener* listener, _listeners )
		listener->init( _rootNodes );
}
//-----------------------------------------------------------------------------


/* Listeners and Models Management *///----------------------------------------
	void	Graph::updateModels( )
{
	foreach ( GraphListener* listener, _listeners )
		listener->init( _rootNodes );
}
//-----------------------------------------------------------------------------


/* Edge/Node Management *///---------------------------------------------------
Graph::~Graph( )
{
	clear( );
}
//-----------------------------------------------------------------------------



/* Edge/Node Management *///---------------------------------------------------
/*! This method clear the nodes, the fast node search cache, the node object mapping
    system and the edge list. Registered nodes and edges are not only dereferenced, but
	destroyed with a call to delete.
 */
void		Graph::clear( )
{
	_m.clear( );
	_o.clear( );

	foreach ( Edge* edge, _edges )
		delete edge;
	_edges.clear( );

	_nodesSet.clear( );
	foreach ( Node* node, _nodes )
		delete node;
	_nodes.clear( );
	_styleManager.clearMappings( );

    _rootNodes.clear( );
    _rootNodesSet.clear( );
}

Node*	Graph::insertNode( Node* node, QString styleName )
{
	Q_ASSERT( node != 0 );

	_nodes.push_back( node );
	_nodesSet.insert( node );
	_styleManager.styleNode( *node, styleName );

	if ( node->getInDegree( ) == 0 )
		addRootNode( *node );

	foreach ( GraphListener* listener, _listeners )
		listener->nodeInserted( *node );
	return node;
}

Node*	Graph::insertNode( QString name, QString styleName )
{
    return insertNode( new Node( name ), styleName );
}

void	Graph::removeNode( Node& node )
{
	_styleManager.clearNodeStyle( node );

	// Test if node subnodes will eventually became root nodes after their super node removal
	Node::Set	rootNodes;
	Node::List	outNodes; node.collectOutNodes( outNodes );
	foreach ( Node* outNode, outNodes )
	{
		if ( !isRootNode( *outNode ) && outNode->getInDegree( ) <= 1 )
			rootNodes.insert( outNode );
	}
	// Add orphan out nodes as root node
	foreach ( qan::Node* rootNode, rootNodes )
	{
		addRootNode( *rootNode );
		foreach ( GraphListener* listener, _listeners )
			listener->nodeInserted( *rootNode );
	}

	// Disconnect node from its "in edges".
	foreach ( qan::Edge* inEdge, node.getInEdges( ) )
	{
		removeEdge( *inEdge );
		//inEdge->getSrc( ).getOutEdges( ).removeAll( inEdge );
	}
	node.getInEdges( ).clear( );

	// Disconnect node from its "out edges".
	foreach ( qan::Edge* outEdge, node.getOutEdges( ) )
	{
		removeEdge( *outEdge );
		//outEdge->getDst( ).getInEdges( ).removeAll( outEdge );
	}
	node.getOutEdges( ).clear( );

	// Remove node from the graph various node list
	_nodes.removeAll( &node );
	_nodesSet.remove( &node );

	removeRootNode( node );

	foreach ( GraphListener* listener, _listeners )
		listener->nodeRemoved( node );

	_styleManager.removeMapping( &node );
}

Edge*	Graph::insertEdge( Node& src, Node& dst, QString styleName, float weight, bool generatePos )
{
	// Insert back the node to its new correct view position
	Edge* edge = new Edge( &src, &dst, weight );
	if ( edge->hasSrc( ) )
		edge->getSrc( ).addOutEdge( *edge );
	if ( edge->hasDst( ) )
	{
		edge->getDst( ).addInEdge( *edge );
		removeRootNode( edge->getDst( ) );   // Dst can't be a root node, supress it to maintain coherency
	}
	_edges.append( edge );
	_styleManager.styleEdge( *edge, styleName );
		
	if ( generatePos )
	{
		Node::List nodes;
		src.collectOutNodes( nodes );

		// Get the barycenter of 1 degree nodes
		QPointF baryCenter( 0., 0. );
		foreach ( qan::Node* node, nodes )
			baryCenter += node->getPosition( );
		baryCenter /= ( float )( nodes.size( ) > 1 ? nodes.size( ) : 2 );	// Divide by node size, or 2 if there is only one outnode to avoid barycenter beeing the outnode...

		src.setPosition( baryCenter );
		GraphItem* srcItem = _m.getGraphItem( src );
		if ( srcItem != 0 )
			srcItem->getGraphicsItem( )->topLevelItem( )->setPos( baryCenter );
	}

	foreach ( GraphListener* listener, _listeners )
		listener->edgeInserted( *edge );

	return edge;
}

HEdge*	Graph::insertHEdge( Node& src, Node& dst, Node::List& hSrcs, Node::List& hDsts, QString styleName )
{
	HEdge* hEdge = findHEdge( src, dst );	// Try to get an existing hedge between src and dst
	if ( hEdge != 0 )	// If it already exists, modify it
	{
		foreach ( Node* hSrc, hSrcs )
		{
			hEdge->getHSrc( ).insert( hSrc );
			hSrc->addOutEdge( *hEdge );
		}
		foreach ( Node* hDst, hDsts )
		{
			hEdge->getHDst( ).insert( hDst );
			hDst->addInEdge( *hEdge );
			removeRootNode( *hDst );   // Dst can't be a root node, supress it to maintain coherency
		}
		foreach ( GraphListener* listener, _listeners )
			listener->edgeModified( *hEdge );
	}
	else	// Otherwise, create it
	{
		hEdge = new HEdge( &src, &dst, 0, 0 );
		_styleManager.styleEdge( *hEdge, styleName );

		// HEdge SRC management
		if ( hEdge->hasSrc( ) )
			hEdge->getSrc( ).addOutEdge( *hEdge );
		if ( hEdge->hasDst( ) )
		{
			hEdge->getDst( ).addInEdge( *hEdge );
			removeRootNode( hEdge->getDst( ) );   // Dst can't be a root node, supress it to maintain coherency
		}
		foreach ( qan::Node* hSrc, hSrcs )
		{
			hEdge->getHSrc( ).insert( hSrc );
			hSrc->addOutEdge( *hEdge );
		}

		// HEdge DST management
		if ( hEdge->hasDst( ) )
		{
			hEdge->getDst( ).addInEdge( *hEdge );
			removeRootNode( hEdge->getDst( ) );   // Dst can't be a root node, supress it to maintain coherency
		}
		foreach ( qan::Node* hDst, hDsts )
		{
			hEdge->getHDst( ).insert( hDst );
			hDst->addInEdge( *hEdge );
			removeRootNode( *hDst );   // Dst can't be a root node, supress it to maintain coherency
		}

		_edges.append( hEdge );
		foreach ( GraphListener* listener, _listeners )
			listener->edgeInserted( *hEdge );
	}
	return hEdge;
}

HEdge*	Graph::insertHOutEdge( Node& src, Node& dst, Node& hDst, QString hDstLabel, QString styleName )
{
	HEdge* hEdge = findHEdge( src, dst );	// Try to get an existing hedge between src and dst
	if ( hEdge != 0 )	// If it already exists, modify it
	{
		hEdge->insertHDst( hDst, hDstLabel );
		hDst.addInEdge( *hEdge );
		removeRootNode( hDst );   // Dst can't be a root node, supress it to maintain coherency
		foreach ( GraphListener* listener, _listeners )
			listener->edgeModified( *hEdge );
	}
	else	// Otherwise, create it
	{
		hEdge = new HEdge( &src, &dst, 0, &hDst, QString( ""), hDstLabel );
		_styleManager.styleEdge( *hEdge, styleName );
		if ( hEdge->hasSrc( ) )
			hEdge->getSrc( ).addOutEdge( *hEdge );
		if ( hEdge->hasDst( ) )
		{
			hEdge->getDst( ).addInEdge( *hEdge );
			removeRootNode( hEdge->getDst( ) );   // Dst can't be a root node, supress it to maintain coherency
		}
		foreach ( qan::Node* hDst, hEdge->getHDst( ) )
		{
			hDst->addInEdge( *hEdge );
			removeRootNode( *hDst );   // Dst can't be a root node, supress it to maintain coherency
		}
		_edges.append( hEdge );
		foreach ( GraphListener* listener, _listeners )
			listener->edgeInserted( *hEdge );
	}
	return hEdge;
}

HEdge*	Graph::insertHInEdge( Node& src, Node& dst, Node& hSrc, QString hSrcLabel, QString styleName )
{
	HEdge* hEdge = findHEdge( src, dst );	// Try to get an existing hedge between src and dst
	if ( hEdge != 0 )	// If it already exists, modify it
	{
		hEdge->insertHSrc( hSrc, hSrcLabel );
		hSrc.addOutEdge( *hEdge );
		foreach ( GraphListener* listener, _listeners )
			listener->edgeModified( *hEdge );
	}
	else	// Otherwise, create it
	{
		hEdge = new HEdge( &src, &dst, &hSrc, 0, hSrcLabel, QString( "" ) );
		_styleManager.styleEdge( *hEdge, styleName );
		if ( hEdge->hasSrc( ) )
			hEdge->getSrc( ).addOutEdge( *hEdge );
		if ( hEdge->hasDst( ) )
		{
			hEdge->getDst( ).addInEdge( *hEdge );
			removeRootNode( hEdge->getDst( ) );   // Dst can't be a root node, supress it to maintain coherency
		}
		foreach ( qan::Node* hSrc, hEdge->getHSrc( ) )
			hSrc->addOutEdge( *hEdge );
		_edges.append( hEdge );
		foreach ( GraphListener* listener, _listeners )
			listener->edgeInserted( *hEdge );
	}
	return hEdge;
}

void	Graph::removeEdge( Edge& edge )
{
	foreach ( GraphListener* listener, _listeners )
		listener->edgeRemoved( edge );
	_styleManager.clearEdgeStyle( edge );

	edge.getSrc( ).getOutEdges( ).removeAll( &edge );
	edge.getDst( ).getInEdges( ).removeAll( &edge );

	Node::Set srcNodes;
	if ( edge.type( ) == Edge::HYPER )
		srcNodes.unite( static_cast< HEdge* >( &edge )->getHSrc( ) );
	srcNodes.insert( &edge.getSrc( ) );
	foreach ( Node* srcNode, srcNodes )
	{
		srcNode->getInEdges( ).removeAll( &edge );
		srcNode->getOutEdges( ).removeAll( &edge );
	}

	// Re insert nodes as root nodes if the edge destruction left them orphants
	Node::Set dstNodes;
	if ( edge.type( ) == Edge::HYPER )
		dstNodes.unite( static_cast< HEdge* >( &edge )->getHDst( ) );
	dstNodes.insert( &edge.getDst( ) );
	foreach ( Node* dstNode, dstNodes )
	{
		dstNode->getInEdges( ).removeAll( &edge );
		dstNode->getOutEdges( ).removeAll( &edge );
		if ( dstNode->getInDegree( ) == 0 )
			addRootNode( *dstNode );
	}

	_edges.removeAll( &edge );
	delete &edge;
}

/*!
	\return	true is there is an edge between a and b (whatever is orientation is), false otherwise.
	\note	this method does not works well with hyperedges.
 */
bool	Graph::hasEdge( Node& a, Node& b ) const
{
	Node::Set adjacentA;
	a.getAdjacentNodesSet( adjacentA );
	if ( adjacentA.find( &b ) != adjacentA.end( ) )
		return true;

	Node::Set adjacentB;
	b.getAdjacentNodesSet( adjacentB );
	if ( adjacentB.find( &a ) != adjacentB.end( ) )
		return true;

	return false;
}
//-----------------------------------------------------------------------------



/* Graph Edge/Node access *///-------------------------------------------------
/*! \return a pointer on the first edge between src and dst, 0 if not such edge exists (be carrefull, O(n) method). An hyperedge might be returned.
 */
Edge*	Graph::findEdge( Node& src, Node& dst )
{
	foreach ( Edge* edge, _edges )
		if ( &edge->getSrc( ) == &src && &edge->getDst( ) == &dst )
			return edge;
	return 0;
}
/*! \return a pointer on the first hyperedge between src and dst, 0 if not such hyperedge exists (be carrefull, O(n) method).
 */
HEdge*	Graph::findHEdge( Node& src, Node& dst )
{
	foreach ( Edge* edge, _edges )
		if ( edge->type( ) == Edge::HYPER && &edge->getSrc( ) == &src && &edge->getDst( ) == &dst )
			return ( HEdge* )edge;
	return 0;
}

void	Graph::collectNodes( Node::Set& nodes ) const
{
	nodes.reserve( _nodes.size( ) );
	foreach ( Node* node, _nodes )
		nodes.insert( node );
}
//-----------------------------------------------------------------------------



/* Graph Search Management *///------------------------------------------------
/*!
	\return	a pointer on the node of request label. 0 if no such node exists.
*/
Node*	Graph::findNode( const QString& label )
{
	foreach ( qan::Node* node, _nodes )
		if ( node->getLabel( ) == label )
			return node;
	return 0;
}

Node*	Graph::findNode( int nodeIndex )
{
	return _nodes.value( nodeIndex, 0 );
}

/*! 
	\result	The node index if node is registered in this graph, -1 if there is no node or index.
 */
int		Graph::findNode( const Node& node ) const
{
	return _nodes.indexOf( ( Node* )&node );
}

/*!
	\param	node	Node to be searched in this graph.
	\return			true if the node is found, false otherwise.
 */
bool	Graph::hasNode( Node* node ) const
{
	return ( _nodesSet.find( node ) != _nodesSet.end( ) );
}
//-----------------------------------------------------------------------------



/* Root Node Management *///---------------------------------------------------
/*!
	Root nodes manually inserted via addRoot() are cleared from the root nodes (and eventually
	automatically re-added if needed).
 */
void	Graph::generateRootNodes( )
{
	_rootNodes.clear( );
	_rootNodesSet.clear( );
	foreach ( Node* node, _nodes )
	{
		if ( node->getInDegree( ) == 0 )
			addRootNode( *node );
	}
}

void	Graph::addRootNode( Node& node )
{
	if ( !_rootNodes.contains( &node ) )
		_rootNodes << &node;
	_rootNodesSet << &node;

	foreach ( GraphListener* listener, _listeners )
		listener->nodeInserted( node );
}

void	Graph::removeRootNode( Node& node )
{
	_rootNodes.removeAll( &node );
	_rootNodesSet.remove( &node );
}

/*!
	\param	node	node that must be tested against the root nodes set (must be a graph node).
	\return			true if the node is a graph root node, false otherwise.
 */
bool	Graph::isRootNode( const Node& node ) const
{
	return ( _rootNodesSet.find( const_cast< Node* >( &node ) ) != _rootNodesSet.end( ) );
}
//-----------------------------------------------------------------------------


} // ::qan

