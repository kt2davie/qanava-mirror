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
// \file	qanGraphModel.h
// \author	benoit@qanava.org
// \date	2007 December 27
//-----------------------------------------------------------------------------


#ifndef canGraphModel_h
#define canGraphModel_h


// Qanava headers
#include "./qanNodeRectItem.h"
#include "./qanEdgeItem.h"


// QT headers
#include <QStandardItemModel>


//-----------------------------------------------------------------------------
namespace qan { // ::qan

	class Graph;

	//! Standard interface for template listeners used in qan::Graph.
	class GraphListener
	{
		/*! \name Graph Topology Listener *///---------------------------------
		//@{
	public:

		typedef QList< GraphListener* >	List;

        virtual void	init( Node::List& rootNodes ) { Q_UNUSED( rootNodes ); }

        virtual void	edgeInserted( qan::Edge& edge ) { Q_UNUSED( edge ); }

        virtual void	edgeModified( qan::Edge& edge ) { Q_UNUSED( edge ); }

        virtual void	edgeRemoved( qan::Edge& edge ) { Q_UNUSED( edge ); }

        virtual void	nodeInserted( qan::Node& node ) { Q_UNUSED( node ); }

        virtual void	nodeRemoved( qan::Node& node ) { Q_UNUSED( node ); }

        virtual void	nodeChanged( qan::Node& node ) { Q_UNUSED( node ); }
		//@}
		//---------------------------------------------------------------------		
	};


	//! Expose a graph data structure to a QT Interview model.
	/*! This class currently does not supports circuits (2014/03/11).

		\nosubgrouping
	*/
	class GraphModel : public QStandardItemModel, public GraphListener
	{
	public:

		GraphModel( StyleManager& styleManager );
		
	protected:

		StyleManager&	_styleManager;


		/*! \name Graph Topology Listener *///---------------------------------
		//@{
	public:

		virtual void	edgeInserted( qan::Edge& edge );

		virtual void	edgeRemoved( qan::Edge& edge );

		virtual void	edgeModified( qan::Edge& edge );

		virtual void	nodeInserted( qan::Node& node );

		virtual void	nodeRemoved( qan::Node& node );

		virtual void	nodeChanged( qan::Node& node );
		//@}
		//---------------------------------------------------------------------		


		/*! \name Interview Model Management *///------------------------------
		//@{
	public:

		void			setGraph( qan::Graph& graph );

	protected:

		void			visit( qan::Node& node, QStandardItem* parent );

		void			visitNodeHierarchy( qan::Node& node, QStandardItem* parent, Node::Set& visited );

		void			removeNodeHierarchy( qan::Node& node );

		void			removeNodeHierarchy( qan::Node& node, Node::Set& visited );

		void			removeItemHierarchy( QStandardItem* item );

		void			collectInNodes( qan::Node& node, Node::Set& inNodes );

		void			collectOutNodes( qan::Node& node, Node::Set& outNodes );

		void			collectEdgeOutNodes( qan::Node& src, qan::Edge& edge, Node::Set& outNodes );

		void			collectEdgeInNodes( qan::Edge& edge, Node::Set& inNodes );

		QStandardItem*	addItem( qan::Node& node, QStandardItem* parent = 0 );

	protected:

		typedef			QMultiMap< qan::Node*, QStandardItem* >	NodeItemMap;
		typedef			QMultiMap< QStandardItem*, qan::Node* >	ItemNodeMap;

		NodeItemMap		_nodeItemMap;
		ItemNodeMap		_itemNodeMap;
		//@}
		//---------------------------------------------------------------------		



		/*! \name Interview Interface *///-------------------------------------
		//@{
	public:

		virtual QVariant		data( const QModelIndex &index, int role ) const;

		//! Get a pointer on the node corresponding to a given index.
		qan::Node*			getIndexNode( QModelIndex index );

		//! Get a pointer on the node corresponding to a given index.
		const qan::Node*	getIndexNode( QModelIndex index ) const;
		//@}
		//---------------------------------------------------------------------		
	};
} // ::qan
//-----------------------------------------------------------------------------


#endif // canGraphModel_h

