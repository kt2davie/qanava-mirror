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
// \file	uiNodesItemModel.h
// \author	benoit@qanava.org
// \date	2006 August 30
//-----------------------------------------------------------------------------


#ifndef uiNodesItemModel_h
#define uiNodeSItemModel_h


// Qanava headers
#include "../qanGraph.h"


// QT headers
#include <QAbstractItemModel>


//-----------------------------------------------------------------------------
namespace qan // ::qan
{
    namespace ui // ::qan::ui
    {
		//! .
		/*!
			\nosubgrouping
		*/
		class NodesItemModel : public QAbstractItemModel
		{
			Q_OBJECT

		public:

			NodesItemModel( Graph& graph, QObject *parent = 0 );

			virtual QVariant data( const QModelIndex &index, int role ) const;

			virtual bool hasChildren( const QModelIndex & parent = QModelIndex( ) ) const;

			virtual Qt::ItemFlags flags( const QModelIndex &index ) const;

			virtual QVariant headerData( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;

			virtual QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;

			virtual QModelIndex parent( const QModelIndex &index ) const;

			virtual int rowCount( const QModelIndex& parent = QModelIndex( ) ) const;

			virtual int columnCount( const QModelIndex& parent = QModelIndex( ) ) const;

		signals:

			void	rowsInserted( const QModelIndex& parent, int start, int end );

			void	rowsRemoved( const QModelIndex& parent, int start, int end );

		public slots:

			void	nodeAboutToBeInserted( const QModelIndex& parent, int start, int end );

			void	nodeAboutToBeRemoved( const QModelIndex& parent, int start, int end );

			void	nodeInserted( const QModelIndex& parent, int start, int end );

			void	nodeRemoved( const QModelIndex& parent, int start, int end );

		private:

			Graph&	_graph;

			bool		getShowStdAttributes( ) const { return _showStdAttributes; }

			bool		_showStdAttributes;
		};
    }  // ::qan::ui
} // ::qan
//-----------------------------------------------------------------------------


#endif // uiStyleEditor_h

