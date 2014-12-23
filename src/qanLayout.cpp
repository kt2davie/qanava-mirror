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
// \file	qanLayout.cpp
// \author	benoit@qanava.org
// \date	2004 May 22
//-----------------------------------------------------------------------------

// Qt headers
#include <QtMath>

// Qanava headers
#include "./qanLayout.h"
#include "./qanSimpleLayout.h"


namespace qan { // ::qan

/* Random Layout Generation Management *///------------------------------------
void	Random::layout( qan::Node::List& rootNodes, qan::Node::Set& nodes, QRectF br, qan::Node* center, QProgressDialog* progress )
{
    Q_UNUSED( rootNodes ); Q_UNUSED( center );
	layout( nodes, br, 0, progress );
}

void	Random::layout( qan::Node::Set& nodes, QRectF br, qan::Node* center, QProgressDialog* progress )
{
    Q_UNUSED( center );
	if ( progress != 0 )
	{
		progress->setMaximum( nodes.size( )  );
		progress->setValue( 0 );
	}
	
	foreach ( Node* node, nodes )
	{
		// Compute a global bounding rect in scene CS for node item and its direct childs (usually the properties widget and a shadow)
		QRectF nodeBr = node->getGraphicsItem( )->sceneBoundingRect( );
		QList< QGraphicsItem* > nodeChilds = node->getGraphicsItem( )->childItems( );
		foreach ( QGraphicsItem* nodeChild, nodeChilds )
			nodeBr = nodeBr.united( nodeChild->sceneBoundingRect( ) );

		// Generate coordinate inside the scene rect so that node bounding boxe is not outside scene rect after moving
		qreal rx = qMin( ( ( qrand( ) % 1000 ) / 1000.0 ), 0.999 );
		qreal ry = qMin( ( ( qrand( ) % 1000 ) / 1000.0 ), 0.999 );
		qreal maxW = br.width( ) - nodeBr.width( ) - 1.;
		qreal maxH = br.height( ) - nodeBr.height( ) - 1.;
		float x = qMin( br.left( ) + rx * maxW, br.right( ) - nodeBr.width( ) );
		float y = qMin( br.top( )  + ry * maxH, br.bottom( ) - nodeBr.height( ) );

		node->setPosition( QPointF( x, y ) );

		if ( progress != 0 )
			progress->setValue( progress->value( ) + 1 );
	}

	if ( progress != 0 )
		progress->close( );
}
//-----------------------------------------------------------------------------


/* Force Layout Generation Management *///-------------------------------------
void	UndirectedGraph::layout( qan::Node::List& rootNodes, qan::Node::Set& nodes, QRectF br, qan::Node* center, QProgressDialog* progress )
{
    qan::Node::Set rootNodesSet = rootNodes.toSet( );
    layout( rootNodesSet, nodes, br, center, progress );
}

void	UndirectedGraph::layout( qan::Node::Set& groupRootNodes, qan::Node::Set& nodes, QRectF br, qan::Node* center, QProgressDialog* progress )
{
	int runCount = 5;
	if ( progress != 0 )
	{
		progress->setMaximum( runCount );
		progress->setValue( 0 );
	}

	// Configure a virtual center node
	Node::Set nodesSet( nodes );

	// Configure a virtual center node
	if ( center != 0 )
		_center.setPosition( center->getPosition( ) );
	else
		_center.setPosition( br.center( ) );

	// Apply the spring force algorithm
	for ( int iter = 0; iter < runCount; iter++ )
	{
		double modification = 0.;

		// Compute new nodes positions using the spring embedder model
		foreach ( Node* node, nodes )
		{
			Node::Set adjacentInNodes;
			node->collectInNodesSet( adjacentInNodes );
			adjacentInNodes.unite( groupRootNodes );

			Node::Set adjacentOutNodes;
			node->collectOutNodesSet( adjacentOutNodes );

			if ( node->getInDegree( ) == 0 )
				adjacentInNodes.insert( &_center );

			QPointF fspring = computeSpringForce( *node, adjacentInNodes );
			QPointF frep = computeRepulseForce( *node, adjacentOutNodes, nodes );
			QPointF delta = ( frep + fspring ) / ( float )( nodes.size( ) + 1.f );

			// Compute a global bounding rect in scene CS for node item and its direct childs (usually the properties widget and a shadow)
			/*QRectF nodesBr = node->getGraphicsItem( )->sceneBoundingRect( );
			QList< QGraphicsItem* > nodeChilds = node->getGraphicsItem( )->childItems( );
			foreach ( QGraphicsItem* nodeChild, nodeChilds )
				nodesBr = nodesBr.united( nodeChild->sceneBoundingRect( ) );*/

			QPointF position = node->getPosition( ) + delta;
			if ( br.contains( position ) )	// Clip generated position 
				node->setPosition( position );
				
			modification += length2( delta );
		}

		// Apply modifications for a virtual center node connected to all root nodes
		{
			QPointF fspring = computeSpringForce( _center, groupRootNodes );
			QPointF frep = computeRepulseForce( _center, groupRootNodes, nodes );

			QPointF delta( 0., 0. );
			delta = ( frep + fspring ) / ( float )( nodes.size( ) + 1.f );
			_center.setPosition( _center.getPosition( ) + delta );
		}

		// Stop iterating if node have converged to a fixed position
		if ( modification < ( 5. * nodes.size( ) ) )
			break;

		// Update progress bar
		if ( progress != 0 )
			progress->setValue( iter );
		if ( progress != 0 && progress->wasCanceled( ) )
			break;
	}

	if ( progress != 0 )
		progress->close( );
}

QPointF	UndirectedGraph::computeRepulseForce( Node& u, Node::Set& adjacentNodes, Node::Set& nodes )
{
	QPointF force( 0., 0. );

	// Compute repulsion forces for all non adjacent nodes
	Node::Set nonAdjacentNodes( nodes );
	nonAdjacentNodes.subtract( adjacentNodes );
	if ( u.getInDegree( ) > 0 )
		nonAdjacentNodes.insert( &_center );

	foreach ( Node* v, nonAdjacentNodes )
	{
		if ( v == &u )
			continue;

		// Compute repulsion forces between item and 
		QPointF& pu = u.getPosition( );
		QPointF& pv = v->getPosition( );
		QPointF uv = pv - pu;
		uv *= - ( 80. * 80. ) / ( 1.0 + length2( uv ) ); 
		force += uv;
	}
	return force;
}

QPointF	UndirectedGraph::computeSpringForce( Node& u, Node::Set& adjacentNodes )
{
	QPointF force( 0., 0. );

	// Compute attraction forces for all adjacent nodes (roots nodes are considered adjacents)
	foreach ( Node* v, adjacentNodes )
	{
		// Compute attraction forces between item and 
		QPointF& pu = u.getPosition( );
		QPointF& pv = v->getPosition( );
		QPointF uv = pv - pu;

		double l = length( uv ) / 100.f;
		double size = 1.;
		if ( l > 1.0 ) 
            size = 2.f * qLn( l );
            //size = 2.f * log( l ); // FIXME v0.9.3 didn't compile on linux

		uv *= size;
		force += uv;
	}

	return force;
}

float		UndirectedGraph::length( const QPointF& v )
{
    return qSqrt( ( v.x( ) * v.x( ) ) + ( v.y( ) * v.y( ) ) );
}

float		UndirectedGraph::length2( const QPointF& v )
{
	return ( v.x( ) * v.x( ) ) + ( v.y( ) * v.y( ) );
}
//-----------------------------------------------------------------------------


} // ::qan
