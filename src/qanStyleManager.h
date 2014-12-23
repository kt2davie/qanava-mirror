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
// \file	qanStyleManager.h
// \author	benoit@qanava.org
// \date	2005 January 03
//-----------------------------------------------------------------------------


#ifndef qan_StyleManager_h
#define qan_StyleManager_h


// QT headers
#include <QIcon>
#include <QVariant>
#include <QList>
#include <QMap>
#include <QAbstractListModel>

// QT Solutions (qtpropertybrowser) headers
#include "QtVariantProperty"

// Qanava headers
#include "./qanProperties.h"
#include "./qanNode.h"
#include "./qanEdge.h"


//-----------------------------------------------------------------------------
namespace qan // ::qan
{
	class Graph;
	
	typedef	Properties Style;

	//! Manage styles for a set of objects (usually graphics items).
	/*!
		Styles in Qanava are preferably managed directly trough dedicated methods in
		qan::GraphScene such as applyStyle() rather than accessing the qan::Graph style manager 
		directly.

		\sa GraphScene
		\nosubgrouping
	*/
	class StyleManager : public QAbstractListModel
	{
		Q_OBJECT

		/*! \name Manager Constructor/Destructor *///--------------------------
		//@{
	public:

		StyleManager( qan::Graph* graph, QObject* parent = 0 );

		virtual ~StyleManager( );

	protected:

		Graph&	_graph;
		//@}
		//---------------------------------------------------------------------



		/*! \name Interview Interface *///-------------------------------------
		//@{
	public:

		virtual	QVariant	data( const QModelIndex& index, int role = Qt::DisplayRole ) const;

		virtual Qt::ItemFlags	flags ( const QModelIndex& index ) const;

		QVariant			headerData ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;

		virtual int			rowCount( const QModelIndex& = QModelIndex( ) ) const { return _styles.size( ); }
		
		virtual	int			columnCount( const QModelIndex& = QModelIndex( ) ) const { return 1; }
		//@}
		//---------------------------------------------------------------------



		/*! \name Style Management *///----------------------------------------
		//@{
	public:

		//! Add a style in this manager.
		Style*			addStyle( QString name, QString target = QString( "" ) );

		//! Add a style in this manager.
		Style*			insertStyle( Style* style );

		//! Remove a style from this manager (style is no longer registered, but not deleted).
		void			removeStyle( Style* style );

		//! Test if a given style is registered in this manager.
		bool			hasStyle( const Style* style ) const { return _styles.contains( const_cast< Style* >( style ) ); }

		//! Get a style by name.
		Style*			getStyle( QString name );

		//! Get a default style for a specific target
		Style*			getTargetStyle( QString target );

		//! Get a list of all styles registered in this manager.
		Style::List&	getStyles( ) { return _styles; }

		//! Apply a style to an edge.
		void			styleEdge( qan::Edge& edge, QString styleName, bool updateItem = true );

		//! Remove every style mapping for a given edge.
		void			clearEdgeStyle( qan::Edge& edge );

		//! Get an edge current style (return 0 if edge is not styled).
		Style*			getStyle( qan::Edge& edge ) { return _edgeStyleMap.value( &edge ); }

		//! Get an edge current style name (return empty string if edge is not styled).
		QString			getStyleName( qan::Edge& edge ) { return ( _edgeStyleMap.contains( &edge ) ? _edgeStyleMap.value( &edge )->getName( ) : QString( "") ); }

		//! Apply a style to a node.
		void			styleNode( qan::Node& node, QString styleName );

		//! Remove every style mapping for a given node.
		void			clearNodeStyle( qan::Node& node );

		//! Get a node current style (return 0 if node is not styled).
		Style*			getStyle( qan::Node& node ) { return _nodeStyleMap.value( &node ); }

		//! Get a node current style name (return empty string if node is not styled).
		QString			getStyleName( qan::Node& node ) { return ( _nodeStyleMap.contains( &node ) ? _nodeStyleMap.value( &node )->getName( ) : QString( "") ); }

		//! Remove a node (in fact remove the mapping between the node and its eventual styles).
		void			removeMapping( void* item );

		//! Clear all mappings.
		void			clearMappings( );

	private slots:

        void			propertyValueChanged( QtProperty* p, const QVariant& value );

    signals:

		void			styleModified( qan::Style* style );

		void			styleAdded( qan::Style* style );

		void			styleRemoved( qan::Style* style );

	protected:

		Style::List		_styles;

		QMap< qan::Edge*, Style* >	_edgeStyleMap;

		QMap< qan::Node*, Style* >	_nodeStyleMap;
		//@}
		//---------------------------------------------------------------------


		/*! \name Image Cache Management *///----------------------------------
		//@{
	public:

		//! Get an image from its file name or its cached version the second call of the same filename.
		QImage			getImage( QString fileName );

		//! Clear this image manager mappings and data (images).
		void			clearImages( );

	protected:

		typedef QMap< QString, QImage >	NameImageMap;

		//! Map image filename to concrete QT images.
		NameImageMap	_nameImageMap;
		//@}
		//---------------------------------------------------------------------
	};
} // ::qan
//-----------------------------------------------------------------------------


#endif // qan_StyleManager_h


