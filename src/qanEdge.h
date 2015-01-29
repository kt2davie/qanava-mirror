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
// \file	qanEdge.cpp
// \author	benoit@qanava.org
// \date	2004 February 15
//-----------------------------------------------------------------------------


#ifndef qanEdge_h
#define qanEdge_h


// Qanava headers
#include "./qanConfig.h"
#include "./qanProperties.h"


// QT headers
#include <QObject>
#include <QString>
#include <QList>
#include <QSet>
#include <QMap>
#include <QGraphicsItem>


//-----------------------------------------------------------------------------
namespace qan { // ::qan

		class Node;
		class GraphItem;

		//! Model a weighted directed edge between two nodes.
		/*!
			\nosubgrouping
		*/
		class Edge : public QObject
		{
			Q_OBJECT

			//! Attribute role.
			enum Role
			{
				WEIGHT			= 0,
				LABEL			= 1,
				USER			= 2
			};

			/*! \name Generator Constructor/Destructor *///--------------------
			//@{
		public:

			//! Edge type (if type is HYPER, this object is an HEdge).
			enum Type
			{
				UNDIRECTED		= 0,
				DIRECTED		= 1,	// Default
				HYPER			= 2
			};

			//! Node constructor with source and destination initialization.
			Edge( Node& src, Node& dst, float weight = 1.f ) : QObject( 0 ),
				_graphicsItem( 0 ),
				_graphItem( 0 ),
                _properties( "properties", this ),
                _type( DIRECTED ),
				_src( &src ),
				_dst( &dst ),
				_label( "" ),
				_weight( 0. )
			{
				setWeight( weight );
				setLabel( QString( "" ) );
			}

			//! Node constructor with source and destination initialization.
			Edge( Node* src, Node* dst, float weight = 1.f ) : QObject( 0 ),
				_graphicsItem( 0 ),
				_graphItem( 0 ),
				_properties( "properties", this ),
				_type( DIRECTED ),
				_src( src ),
				_dst( dst ),
				_label( "" ),
				_weight( 0. )
			{
				setWeight( weight );
				setLabel( QString( "" ) );
			}

			//! Typedef for a QT list of Edge.
			typedef	QList< Edge* >	List;

			//! Typedef for a QT set of Edge.
			typedef	QSet< Edge* >	Set;

			//! Return this edge graphics item (usually based on qan::EdgeItem). Use this method carrefully, it is usually better to get a reference to the associed graphics item trought qan::GraphScene.
			QGraphicsItem*		getGraphicsItem( ) { return _graphicsItem; }

			void				setGraphicsItem( QGraphicsItem* graphicsItem ) { _graphicsItem = graphicsItem; }

			//! Fast access to this node associed graphics item (it is usually better to use the qan::GraphScene interface to access the associed graphics item).
			GraphItem*			getGraphItem( ) { return _graphItem; }

			//! Used internally.
			void				setGraphItem( GraphItem* graphItem ) { _graphItem = graphItem; }

			Properties&			getProperties( ) { return _properties; }

		protected:

			QGraphicsItem*		_graphicsItem;

			GraphItem*			_graphItem;

            Properties			_properties;
            //@}
			//-----------------------------------------------------------------



			/*! \name Source/Destination Management *///-----------------------
			//@{
		public:

			Type		type( ) const { return _type; }

			//! Test if the edge has a source node.
			bool		hasSrc( ) const { return ( _src != 0 ); }

			//! Test if the edge has a destination node.
			bool		hasDst( ) const { return ( _dst != 0 ); }

			//! Get edge source node.
			Node&		getSrc( ) { return *_src; }

			//! Get edge destination node.
			Node&		getDst( ) { return *_dst; }

			//! Get const reference on the edge source node.
			const Node&	getSrc( ) const { return *_src; }

			//! Get const reference on the edge destination node.
			const Node&	getDst( ) const { return *_dst; }

			//! Get edge's weight.
			float		getWeight( ) const { return _weight; }

			//! Set edge's weight.
			void		setWeight( float weight ) { _weight = weight; }

			//! Set edge source and destination (use this method carefully it is normally reserved for serialization implementation).
			void		set( Node* src, Node* dst );

			//! Get this edge label.
			QString		getLabel( ) const { return _label; }

			//! Set this edge label.
			void		setLabel( const QString& label ) { _label = label; }

		protected:

			//! Edge type.
			Type	_type;

			//! Edge source.
			Node*	_src;

			//! Edge destination.
			Node*	_dst;

			QString	_label;

			float	_weight;
			//@}
			//-----------------------------------------------------------------
		};

		//! Model a weighted directed hyper edge (not a general hyper edge, but rahter a standard direct edge with and hyper extension).
		class HEdge : public Edge
		{
			Q_OBJECT

		public:

			//! Node constructor with source and destination initialization.
			/*!	\param	src			Base edge source.
				\param	dst			Base edge destination.
				\param	hSrc		Eventual hyper in edge source.
				\param	hDst		Eventual hyper out edge destination.
				\param	SrcLabel 	Label for the hyper in edge from src2 to base edge.
				\param	DstLabel 	Label for the hyper out edge from base edge to dst2.	*/
			HEdge( Node* src, Node* dst, Node* hSrc = 0, Node* hDst = 0, QString hSrcLabel = QString( ""), QString hDstLabel = QString( "") );

			//! Add an hyper out edge to a given destination node.
			void			insertHDst( qan::Node& dst, QString label );

			//! Add an hyper out edge from a given source node.
			void			insertHSrc( qan::Node& source, QString label );

			//! Remove a given source or destination node from this hyper edge.
			void			removeHNode( qan::Node& dst, QString label );

			//! Return all hyper destination nodes (original destination should be added manually to the result to have a complete set).
			QSet< Node* >&	getHDst( ) { return _hDst; }

			//! Return all hyper source nodes (original source should be added manually to the result to have a complete set).
			QSet< Node* >&	getHSrc( ) { return _hSrc; }
			
			//! Return the mapping between hyper edge in/out nodes and their associed label.
			QMap< Node*, QString >&	getHNodeLabelMap( ) { return _hNodeLabelMap; }

		protected:

			QSet< Node* >			_hDst;
			QSet< Node* >			_hSrc;
			QMap< Node*, QString >	_hNodeLabelMap;
		};
} // ::qan
//-----------------------------------------------------------------------------


#endif // qanEdge_h
