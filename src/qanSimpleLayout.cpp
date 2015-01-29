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
// \file	qanSimpleLayout.cpp
// \author	benoit@qanava.org
// \date	2006 January 06
//-----------------------------------------------------------------------------

// Qt headers
#include <QtMath>

// Qanava headers
#include "./qanSimpleLayout.h"


namespace qan { // ::qan


/* Concentric Layout Management *///-------------------------------------------
void	Concentric::layout( Graph& graph, QProgressDialog* progress )
{
	// Configure the progress monitor
	if ( progress != 0 )
	{
		progress->setMaximum( graph.getNodeCount( ) );
		progress->setValue( 0 );
	}

	QPointF center = graph.getM( ).sceneRect( ).center( );
	int		nodesPerCircle = ( int )( 360. / _azimutDelta );
	Node::List::iterator nodeIter = graph.getNodes( ).begin( );
	for ( int n = 0; n < ( int )graph.getNodes( ).size( ); n++, nodeIter++ )
	{
		Node& node = **nodeIter;
		double azimutIndex = ( n % nodesPerCircle );
		double azimut = azimutIndex * _azimutDelta;

		int circleIndex = 1 + ( n / nodesPerCircle );
        double cx = qSin( azimut * 3.14156 / 180. ) * ( circleIndex * _circleInterval );
        double cy = qCos( azimut * 3.14156 / 180. ) * ( circleIndex * _circleInterval );

		node.getPosition( ).rx( ) = center.x( ) + cx;
		node.getPosition( ).ry( ) = center.y( ) + cy;
		if ( progress != 0 )
			progress->setValue( n );
	}
	if ( progress != 0 )
		progress->close( );
}
//-----------------------------------------------------------------------------



/* Colimacon Layout Management *///--------------------------------------------
void	Colimacon::layout( Graph& graph, QProgressDialog* progress )
{
	// Configure the progress monitor
	if ( progress != 0 )
	{
		progress->setMaximum( graph.getNodeCount( ) );
		progress->setValue( 0 );
	}

	QPointF center = graph.getM( ).sceneRect( ).center( );
	int		nodesPerCircle = ( int )( 360. / _azimutDelta );
	Q_UNUSED( nodesPerCircle );
	Node::List::iterator nodeIter = graph.getNodes( ).begin( );
	for ( int n = 0; n < ( int )graph.getNodes( ).size( ); n++, nodeIter++ )
	{
		Node& node = **nodeIter;
		//double azimutIndex = ( double )( n % nodesPerCircle );
		double azimut = n * _azimutDelta;

		//int circleIndex = 1 + ( int )( n / nodesPerCircle );
		double cx = sin( azimut * 3.14156 / 180. ) * ( log( 1. + n ) * 10 * _circleInterval );
		double cy = cos( azimut * 3.14156 / 180. ) * ( log( 1. + n ) * 10 * _circleInterval );

		node.getPosition( ).rx( ) = center.x( ) + cx;
		node.getPosition( ).ry( ) = center.y( ) + cy;

		if ( progress != 0 )
			progress->setValue( n );
	}
	if ( progress != 0 )
		progress->close( );
}
//-----------------------------------------------------------------------------


} // ::qan
