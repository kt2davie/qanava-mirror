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
// \file	uiNodesItemModel.cpp
// \author	benoit@qanava.org
// \date	2006 August 30
//-----------------------------------------------------------------------------


// Qanava headers
#include "./uiNodesItemModel.h"


// QT headers
#include <QFont>


namespace qan { // ::qan
namespace ui  { // ::qan::ui


//-----------------------------------------------------------------------------
NodesItemModel::NodesItemModel( Graph& graph, QObject *parent ) :
	_graph( graph )
{
    Q_UNUSED( parent );
	connect( &graph.getO( ), SIGNAL( rowAboutToBeInserted(const QModelIndex&,int,int) ), this, SLOT( nodeAboutToBeInserted(const QModelIndex&,int,int) ) );
	connect( &graph.getO( ), SIGNAL( rowAboutToBeRemoved(const QModelIndex&,int,int) ), this, SLOT( nodeAboutToBeRemoved(const QModelIndex& parent,int,int) ) );
	connect( &graph.getO( ), SIGNAL( rowInserted(const QModelIndex&,int,int) ), this, SLOT( nodeInserted(const QModelIndex&,int,int) ) );
	connect( &graph.getO( ), SIGNAL( rowRemoved(const QModelIndex&,int,int) ), this, SLOT( nodeRemoved(const QModelIndex&,int,int) ) );
}


QVariant NodesItemModel::data( const QModelIndex &index, int role ) const
{
	if ( !index.isValid( ) )
		return QVariant( "index invalid" );

	QVariant d;

	if ( ( role == Qt::FontRole ) && ( index.column( ) == 0 ) )
	{
		QFont font;
		font.setWeight( QFont::Bold );
		d = font;
	}

	if ( role == Qt::DisplayRole )
	{
		Node* node = _graph.findNode( index.row( ) );
		if ( node != 0 )
		{
			if ( index.column( ) == 0 )
				d =	QString( node->getLabel( ) );
			if ( index.column( ) > 0 )
			{
				//int attributeRole = index.column( ) - 1 + Node::StdAttributeCount;
				d = "NoData"; // FIXME _graph.getNodeAttrHolder( ).toString( *node, attributeRole, QString( "" ) );
			}
		}
	}
	return d;
}

bool NodesItemModel::hasChildren( const QModelIndex& parent ) const
{
    Q_UNUSED( parent );
    return false;
}

Qt::ItemFlags NodesItemModel::flags( const QModelIndex& index ) const
{
	return ( index.isValid( ) ? Qt::ItemIsSelectable | Qt::ItemIsEnabled : Qt::ItemIsEnabled );
}

QVariant NodesItemModel::headerData( int section, Qt::Orientation orientation, int role ) const
{
    Q_UNUSED( section ); Q_UNUSED( orientation );
    QVariant d;
	if ( role == Qt::DisplayRole )
	{
		d = QString( "NoData" );	// FIXME v0.9
		/*int attributeRole = section - 1 + Node::StdAttributeCount;
		if ( section == 0 )
			return QString( "Label" );
		else if ( attributeRole >= 0 && attributeRole < ( int )_graph.getNodeAttrHolder( ).getCount( ) )
			d = _graph.getNodeAttrHolder( ).getName( attributeRole );*/
	}
	return d;
}

QModelIndex NodesItemModel::index(int row, int column, const QModelIndex& parent ) const
{
    Q_UNUSED( parent );
	return createIndex( row, column, ( row * 10000 ) + column );
}

QModelIndex NodesItemModel::parent( const QModelIndex &index ) const
{
    Q_UNUSED( index );
	return QModelIndex( );
}

int NodesItemModel::rowCount( const QModelIndex& parent ) const
{
    Q_UNUSED( parent );
	return _graph.getNodeCount( );
}

int	NodesItemModel::columnCount( const QModelIndex& parent ) const
{
    Q_UNUSED( parent );
	return ( 1 ); // FIXME  + _graph.getNodeAttrHolder( ).getCount( ) - Node::StdAttributeCount );
}

void	NodesItemModel::nodeAboutToBeInserted( const QModelIndex& parent, int start, int end )
{
    Q_UNUSED( end );

	QModelIndex nodeItem = _graph.getO( ).index( start, 0, parent );
	if ( nodeItem.isValid( ) )
	{
		Node* node = static_cast< Node* >( nodeItem.internalPointer( ) );
		int id = _graph.findNode( *node );
		if ( id != -1 )
			beginInsertRows( QModelIndex( ), id, id + 1 );
	}
}

void	NodesItemModel::nodeAboutToBeRemoved( const QModelIndex& parent, int start, int end )
{
    Q_UNUSED( parent ); Q_UNUSED( start ); Q_UNUSED( end );
/*	int id = _graph.findNode( node );
	if ( id != -1 )
		beginRemoveRows( QModelIndex( ), id, id + 1 );*/
}

void	NodesItemModel::nodeInserted( const QModelIndex& parent, int start, int end )
{
    Q_UNUSED( end );
	QModelIndex nodeItem = _graph.getO( ).index( start, 0, parent );
	if ( nodeItem.isValid( ) )
	{
		Node* node = static_cast< Node* >( nodeItem.internalPointer( ) );
		int id = _graph.findNode( *node );
		if ( id != -1 )
			emit rowsInserted( QModelIndex( ), id, id + 1 );
		emit layoutChanged( );
	}
}

void	NodesItemModel::nodeRemoved( const QModelIndex& parent, int start, int end )
{
    Q_UNUSED( parent ); Q_UNUSED( start ); Q_UNUSED( end );
	/*int id = _graph.findNode( node );
	if ( id != -1 )
		endRemoveRows( );
	emit layoutChanged( );*/
}
//-----------------------------------------------------------------------------


} // ::qan::ui
} // ::qan

