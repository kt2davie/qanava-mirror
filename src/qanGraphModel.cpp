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
// \file	qanGraphModel.cpp
// \author	benoit@qanava.org
// \date	2007 December 27
//-----------------------------------------------------------------------------


// Qanava headers
#include "./qanStyleManager.h"
#include "./qanGraphModel.h"
#include "./qanGraph.h"


//-----------------------------------------------------------------------------
namespace qan { // ::qan


//-----------------------------------------------------------------------------
GraphModel::GraphModel( StyleManager& styleManager ) : 
	_styleManager( styleManager )
{

}
//-----------------------------------------------------------------------------


/* Graph Topology Listener *///------------------------------------------------
void	GraphModel::collectInNodes( qan::Node& node, Node::Set& inNodes )
{
	foreach ( Edge* edge, node.getInEdges( ) )
	{
		if ( edge->type( ) == Edge::DIRECTED )
			inNodes.insert( &edge->getSrc( ) );
	}
}

void	GraphModel::collectEdgeInNodes( qan::Edge& edge, Node::Set& inNodes )
{
	inNodes.insert( &edge.getSrc( ) );
	if ( edge.type( ) == Edge::HYPER )
		inNodes.unite( static_cast< qan::HEdge* >( &edge )->getHSrc( ) );
}

void	GraphModel::collectEdgeOutNodes( qan::Node& src, qan::Edge& edge, Node::Set& outNodes )
{
	outNodes.insert( &edge.getDst( ) );
	if ( edge.type( ) == Edge::HYPER )	
	{
		HEdge* hEdge = static_cast< HEdge* >( &edge );
		outNodes.unite( hEdge->getHDst( ) );

		if ( &edge.getSrc( ) == &src )			// Visiting an hyper edge source node (adding all HDst/out, hSrc/in and dst)
			outNodes.unite( hEdge->getHDst( ) );
		else if ( &edge.getSrc( ) != &src )	// Visiting an hyper edge hSource node (ie, it is an hIn node for the hyper edge, but not the hyper edge source
			outNodes.insert( &hEdge->getSrc( ) );
	}
}

void	GraphModel::edgeInserted( qan::Edge& edge )
{
	// Collecting edge in nodes items
	QList< QStandardItem* > inNodeItems;
	Node::Set inNodes; collectEdgeInNodes( edge, inNodes );
		foreach ( Node* inNode, inNodes )
			inNodeItems.append( _nodeItemMap.values( inNode ) );

	// Remove out nodes item hierarchy connected to edge in nodes items
	Node::Set outNodes; collectEdgeOutNodes( edge.getSrc( ), edge, outNodes );
	foreach ( Node* outNode, outNodes )
	{
		QList< QStandardItem* > outNodeItems = _nodeItemMap.values( outNode );
		foreach ( QStandardItem* outNodeItem, outNodeItems )
			if ( outNodeItem->parent( ) == 0 || 
				 inNodeItems.contains( outNodeItem->parent( ) ) || 
				 outNodeItem->parent( ) == invisibleRootItem( ) )
				removeItemHierarchy( outNodeItem );	
	}

	// Get edge in nodes items
	inNodes.clear( ); collectEdgeInNodes( edge, inNodes );
	if ( inNodes.size( ) > 0 )
	{
		// Add out node hierarchy to existing in nodes items
		foreach ( Node* inNode, inNodes )
		{
			QList< QStandardItem* > inNodeItems = _nodeItemMap.values( inNode );
			foreach ( QStandardItem* inNodeItem, inNodeItems )
			{
				// Add out Nodes item hierarchy
				Node::Set edgeOutNodes; collectEdgeOutNodes( *inNode, edge, edgeOutNodes );
				foreach ( qan::Node* outNode, edgeOutNodes )
				{
					qan::Node::Set visited; //visited.insert( inNode );
					visitNodeHierarchy( *outNode, inNodeItem, visited );
				}
			}
			if ( inNodeItems.size( ) == 0 )		// In node has never been inserted
			{
				qan::Node::Set visited; 
				visitNodeHierarchy( *inNode, invisibleRootItem( ), visited );
			}
		}		
	}
}

void	GraphModel::edgeModified( qan::Edge& edge )
{
	if ( edge.type( ) == Edge::HYPER )
	{
		HEdge* hEdge = static_cast< HEdge* >( &edge );
		Q_UNUSED( hEdge );

		// Collect edge src items
		QList< QStandardItem* > edgeSrcItems = _nodeItemMap.values( &edge.getSrc( ) );
		foreach ( QStandardItem* edgeSrcItem, edgeSrcItems )
			removeItemHierarchy( edgeSrcItem );

		// Remove all hSrc hierarchy, then re-add it
		edgeInserted( edge );
	}
}

void	GraphModel::edgeRemoved( qan::Edge& edge )
{
	// Collecting edge in nodes items
	QList< QStandardItem* > inNodeItems;
	Node::Set inNodes; collectEdgeInNodes( edge, inNodes );
		foreach ( Node* inNode, inNodes )
			inNodeItems.append( _nodeItemMap.values( inNode ) );

	// Remove out nodes item hierarchy connected to edge in nodes items
	Node::Set outNodes; collectEdgeOutNodes( edge.getSrc( ), edge, outNodes );
	foreach ( Node* outNode, outNodes )
	{
		QList< QStandardItem* > outNodeItems = _nodeItemMap.values( outNode );
		foreach ( QStandardItem* outNodeItem, outNodeItems )
			if ( inNodeItems.contains( outNodeItem->parent( ) ) )
				removeItemHierarchy( outNodeItem );	
	}

	if ( edge.type( ) == Edge::HYPER ) // Recurse also in hierarchies under src items (src is an out node for in node in hyper edges)
	{
		QList< QStandardItem* > outNodeItems = _nodeItemMap.values( &edge.getSrc( ) );
		foreach ( QStandardItem* outNodeItem, outNodeItems )
			if ( inNodeItems.contains( outNodeItem->parent( ) ) )
				removeItemHierarchy( outNodeItem );	
	}
}

void	GraphModel::nodeInserted( qan::Node& node )
{
	// Remove an eventual existing node hierarchy
	removeNodeHierarchy( node );
	
	// Insert back the node
	Node::Set visited;
	visitNodeHierarchy( node, invisibleRootItem( ), visited );
}

void	GraphModel::nodeRemoved( qan::Node& node )
{
	// Remove all dst node hierarchies (dst will be re-inserted back if the node became a root node)
	removeNodeHierarchy( node );
}

void	GraphModel::nodeChanged( qan::Node& node )
{
	QStandardItem* qStdItem = _nodeItemMap.value( &node );
	if ( qStdItem != 0 )
	{
		qStdItem->setText( node.getLabel( ) );
		emit itemChanged( qStdItem );
	}
}
//-----------------------------------------------------------------------------


/* Interview Model Management *///---------------------------------------------
void	GraphModel::setGraph( qan::Graph& graph )
{
	foreach ( Node* rootNode, graph.getRootNodes( ) )
		visit( *rootNode, invisibleRootItem( ) );
}

void	GraphModel::visit( qan::Node& node, QStandardItem* parent )
{
	QStandardItem* nodeItem = addItem( node, parent );
	if ( nodeItem == 0 )
		return;

	Node::Set outNodes;
	collectOutNodes( node, outNodes );

	// Visit sub nodes
	foreach ( Node* outNode, outNodes )
	{
		Node::Set visited;
		visitNodeHierarchy( *outNode, nodeItem, visited );
	}
}

void	GraphModel::collectOutNodes( qan::Node& node, Node::Set& outNodes )
{
	foreach ( Edge* edge, node.getOutEdges( ) )
	{
		if ( edge->type( ) == Edge::DIRECTED )
			outNodes.insert( &edge->getDst( ) );
		else if ( edge->type( ) == Edge::HYPER && &edge->getSrc( ) == &node )	// Visiting an hyper edge source node (adding all HDst/out and dst)
		{
			HEdge* hEdge = static_cast< HEdge* >( edge );
			outNodes.insert( &hEdge->getDst( ) );
			outNodes.unite( hEdge->getHDst( ) );
		}
		else if ( edge->type( ) == Edge::HYPER && &edge->getSrc( ) != &node )	// Visiting an hyper edge hSource node (ie, it is an hIn node for the hyper edge, but not the hyper edge source
		{
			HEdge* hEdge = static_cast< HEdge* >( edge );
			outNodes.insert( &hEdge->getSrc( ) );
		}
	}
}

void	GraphModel::removeNodeHierarchy( qan::Node& node )
{
	Node::Set	visited;
	removeNodeHierarchy( node, visited );
}

void	GraphModel::removeNodeHierarchy( qan::Node& node, Node::Set& visited )
{
	if ( visited.contains( &node ) )
		return;
	visited.insert( &node );

	QList< QStandardItem* > nodeItems = _nodeItemMap.values( &node );
	foreach ( QStandardItem* nodeItem, nodeItems )
	{
		Node::Set outNodes; collectOutNodes( node, outNodes );
		foreach ( Node* outNode, outNodes )
			removeNodeHierarchy( *outNode, visited );

		QStandardItem* nodeParentItem = nodeItem->parent( );
		if ( nodeParentItem == 0 )
			nodeParentItem = invisibleRootItem( );
		if ( nodeParentItem != 0 )
			nodeParentItem->removeRow( nodeItem->row( ) );

		_itemNodeMap.remove( nodeItem, &node );
	}
	_nodeItemMap.remove( &node );
}

void	GraphModel::removeItemHierarchy( QStandardItem* item )
{
	while ( item->rowCount( ) > 0 )
		removeItemHierarchy( item->child( 0 ) );

	if ( item->parent( ) != 0 )
		item->parent( )->removeRow( item->row( ) );
	else
		invisibleRootItem( )->removeRow( item->row( ) );

	QList< qan::Node* >	itemNodes = _itemNodeMap.values( item );
	foreach ( qan::Node* itemNode, itemNodes )
		_nodeItemMap.remove( itemNode, item );
	_itemNodeMap.remove( item );
}

void	GraphModel::visitNodeHierarchy( qan::Node& node, QStandardItem* parent, Node::Set& visited )
{
	if ( visited.contains( &node ) )
		return;

	QStandardItem* nodeItem = addItem( node, parent );
	visited.insert( &node );

	Node::Set outNodes;
	collectOutNodes( node, outNodes );

	// Visit sub nodes
	foreach ( Node* outNode, outNodes )
		visitNodeHierarchy( *outNode, nodeItem, visited );
}

QStandardItem*	GraphModel::addItem( qan::Node& node, QStandardItem* parent )
{
	QStandardItem* item = new QStandardItem( node.getLabel( ) );

	QVariant data = qVariantFromValue( ( void* )( &node ) );
	item->setData( data );
	_nodeItemMap.insert( &node, item );
	_itemNodeMap.insert( item, &node );

	if ( parent != 0 )
		parent->appendRow( item );

	return item;
}
//-----------------------------------------------------------------------------


/* Interview Interface *///----------------------------------------------------
QVariant	GraphModel::data( const QModelIndex &index, int role ) const
{
	QVariant d;
	if ( role == Qt::DecorationRole )
	{
		/*const Node* node = getModelIndexNode( index );
		if ( node != 0 )
			d = getNodeDecoration( *node );*/
	} else if ( role == Qt::DisplayRole )
	{
		const Node* node = getIndexNode( index );
		if ( node != 0 )
			d = node->getLabel( );
	} else d = QStandardItemModel::data( index, role );

	return d;
}

qan::Node*	GraphModel::getIndexNode( QModelIndex index )
{
	const QStandardItem* item = itemFromIndex( index );
	if ( item != 0 )
	{
		void* node = qvariant_cast< void* >( item->data( ) );
		return reinterpret_cast< qan::Node* >( node );
	}
	return 0;
}

const qan::Node*	GraphModel::getIndexNode( QModelIndex index ) const
{
	const QStandardItem* item = itemFromIndex( index );
	if ( item != 0 )
	{
		void* node = qvariant_cast< void* >( item->data( ) );
		return reinterpret_cast< const qan::Node* >( node );
	}
	return 0;
}//-----------------------------------------------------------------------------


} // ::qan
//-----------------------------------------------------------------------------


