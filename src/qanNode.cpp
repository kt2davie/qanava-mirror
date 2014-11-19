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
// \file	qanNode.cpp
// \author	benoit@qanava.org
// \date	2004 February 15
//-----------------------------------------------------------------------------


// Qanava headers
#include "./qanNode.h"
#include "./qanGraph.h"


namespace qan { // ::qan


/* Node Constructor/Destructor *///--------------------------------------------
Node::Node( QString label ) :
	QObject( 0 ),
	_graphicsItem( 0 ),
	_graphItem( 0 ),
	_label( "" ),
	_position( 0., 0. ),
	_dimension( 0., 0. ),
	_properties( "properties", this )
{
	setLabel( label );
	QPointF v( 0., 0. );
	setPosition( v );
	setDimension( v );
}
//-----------------------------------------------------------------------------



/* Node Edges Management *///--------------------------------------------------
void	Node::collectOutNodes( Node::List& outNodes )
{
	foreach ( Edge* outEdge, _outEdges )
		outNodes << &outEdge->getDst( );
}

void	Node::collectInNodes( Node::List& inNodes )
{
	foreach ( qan::Edge* inEdge, _inEdges )
		inNodes << &inEdge->getSrc( );
}

void	Node::collectOutNodesSet( Node::Set& outNodes ) const
{
	// Add all out edge destination nodes to the out nodes set
	foreach ( qan::Edge* outEdge, getOutEdges( ) )
		outNodes.insert( &outEdge->getDst( ) );
}

void	Node::collectInNodesSet( Node::Set& inNodes ) const
{
	// Add all in edge source nodes to the in nodes set
	foreach ( qan::Edge* inEdge, getInEdges( ) )
		inNodes.insert( &inEdge->getSrc( ) );
}

void			Node::getAdjacentNodesSet( Node::Set& adjacentNodes ) const
{
	Node::Set outNodes; collectOutNodesSet( outNodes );
	Node::Set inNodes; collectInNodesSet( inNodes );

	adjacentNodes.unite( outNodes );
	adjacentNodes.unite( inNodes );
}

void	Node::getNonAdjacentNodesSet( Node::Set& nonAdjacentNodes, const Node::Set& graphNodes ) const
{
	Node::Set adjacentNodes; getAdjacentNodesSet( adjacentNodes );
	nonAdjacentNodes.unite( graphNodes );
	nonAdjacentNodes.subtract( adjacentNodes );

	// Remove the node idself from to non adjacent nodes list
	nonAdjacentNodes.remove( const_cast< Node* >( this ) );
}
//-----------------------------------------------------------------------------


} // ::qan

