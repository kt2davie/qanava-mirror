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
// \file	qanTreeLayout.cpp
// \author	benoit@qanava.org
// \date	2007 January 06
//-----------------------------------------------------------------------------


// Qanava headers
#include "./qanTreeLayout.h"


namespace qan { // ::qan


/* Hierarchy Layout Generation Management *///---------------------------------
void	HierarchyTree::layout( qan::Node::List& rootNodes, qan::Node::Set& nodes, QRectF br, qan::Node* center, QProgressDialog* progress )
{
    Q_UNUSED( center );
    qan::Node::Set rootNodesSet = rootNodes.toSet( );
    layout( rootNodesSet, nodes, br, progress );
}


void	HierarchyTree::layout( Node::Set& rootNodes, Node::Set& nodes, QRectF br, QProgressDialog* progress )
{
    Q_UNUSED( br );

	// Configure the progress monitor
	if ( progress != 0 )
	{
		progress->setMaximum( nodes.size( ) );
		progress->setValue( 0 );
	}

	// Layout nodes subgraphs as hierachy tree
	_marked.clear( );
	QPointF topLeft( 0., 0. );
	foreach ( Node* rootNode, rootNodes )
	{
		QRectF subNodesBr = layout( *rootNode, topLeft, 0, progress );
		topLeft.ry( ) = subNodesBr.bottom( ) + _spacing.y( );
	}
	
	if ( progress != 0 )
		progress->close( );
}

// Top-Down version 
QRectF	HierarchyTree::layout( Node& node, QPointF topLeft, int depth, QProgressDialog* progress )
{
	if ( _marked.contains( &node ) )
		return QRectF( );
	_marked.insert( &node );
	if ( progress != 0 && progress->wasCanceled( ) )
		return QRectF( );

	// Setup position of this node
	node.setPosition( topLeft );
	topLeft.rx( ) += node.getGraphicsItem( )->sceneBoundingRect( ).width( ) + _spacing.x( );

	// Set subnodes position
	QRectF subNodesBr;
	Node::Set outNodes; node.collectOutNodesSet( outNodes );
	foreach ( Node* outNode, outNodes )
	{
		subNodesBr = subNodesBr.united( layout( *outNode, topLeft, depth + 1, progress ) );
		topLeft.ry( ) = subNodesBr.bottom( ) + _spacing.y( );
	}

	// Adjust this node position according to sub node bounding rect
	QPointF nodePosition( node.getPosition( ) );
	if ( node.getOutDegree( ) != 0 )	// do not adjust height if there is no multiple sub nodes
	{
		nodePosition.ry( ) = subNodesBr.top( ) + ( subNodesBr.height( ) - node.getGraphicsItem( )->sceneBoundingRect( ).height( ) ) / 2.; 
		node.setPosition( nodePosition );
	}

	if ( progress != 0 )
		progress->setValue( progress->value( ) + 1 );

	return subNodesBr.united( QRectF( node.getPosition( ), node.getGraphicsItem( )->sceneBoundingRect( ).size( ) ) );
}
//-----------------------------------------------------------------------------


} // ::qan
