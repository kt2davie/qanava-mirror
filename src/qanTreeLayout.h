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
// \file	qanTreeLayout.h
// \author	benoit@qanava.org
// \date	2007 January 06
//-----------------------------------------------------------------------------


#ifndef qanTreeLayout_h
#define qanTreeLayout_h


// Qanava headers
#include "./qanLayout.h"


//-----------------------------------------------------------------------------
namespace qan { // ::qan


	//! Layout an undirected graph as a directed top-down tree.
	/*!
		\nosubgrouping
	*/
	class HierarchyTree : public Layout
	{
		/*! \name HierarchyTree Constructor/Destructor *///---------------------
		//@{
	public:

		//! HierarchyTree constructor with orientation initialization.
		/*! \param spacing spacing between node on x and y (ex: 120, 70).	*/ 
		HierarchyTree( QPointF spacing = QPointF ( 50., 20. ) ) :
			Layout( ), _spacing( spacing ) {  }
		//@}
		//---------------------------------------------------------------------



		/*! \name Hierarchy Layout Generation Management *///------------------
		//@{
	public:

		//! Layout a node group as a hierarchy tree.
		virtual void	layout( qan::Node::List& rootNodes, qan::Node::Set& nodes, QRectF br, qan::Node* center, QProgressDialog* progress = 0 );

	protected:

		//! Layout a node hierarchy as a hierarchy tree.
		void			layout( Node::Set& rootNodes, Node::Set& nodes, QRectF br, QProgressDialog* progress = 0 );

		//! Layout a node hierarchy as a hierarchy tree.
		QRectF			layout( Node& node, QPointF topLeft, int depth, QProgressDialog* progress = 0 );

		//! Spacing on x and y between tree nodes.
		QPointF			_spacing;

		//! Container for already laid out nodes.
		Node::Set		_marked;
		//@}
		//---------------------------------------------------------------------
	};
} // ::qan
//-----------------------------------------------------------------------------


#endif // qanTreeLayout_h

