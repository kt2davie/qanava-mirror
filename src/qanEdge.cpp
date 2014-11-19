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
// \file	qanEdge.cpp
// \author	benoit@qanava.org
// \date	2004 February 15
//-----------------------------------------------------------------------------


// Qanava headers
#include "./qanEdge.h"
#include "./qanNode.h"


namespace qan { // ::qan

void	Edge::set( Node* src, Node* dst )
{
	if ( src != 0 && dst != 0 )
	{
		_src = src; _dst = dst; 
		src->addOutEdge( *this );
		dst->addInEdge( *this );
	}
}

HEdge::HEdge( Node* src, Node* dst, Node* hSrc, Node* hDst, QString hSrcLabel, QString hDstLabel ) : 
	Edge( src, dst )
{
	_type = Edge::HYPER;
	if ( hSrc != 0 )
		insertHSrc( *hSrc, hSrcLabel );
	if ( hDst != 0 )
		insertHDst( *hDst, hDstLabel );
}

void	HEdge::insertHDst( qan::Node& hDst, QString label )
{
	_hDst.insert( &hDst );
	_hNodeLabelMap.insert( &hDst, label );
}

void	HEdge::insertHSrc( qan::Node& hSrc, QString label )
{
	_hSrc.insert( &hSrc );
	_hNodeLabelMap.insert( &hSrc, label );
}

void	HEdge::removeHNode( qan::Node& dst, QString label )
{
    Q_UNUSED( dst ); Q_UNUSED( label );
}

} // ::qan
