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
// \file	qanLayout.h
// \author	benoit@qanava.org
// \date	2004 May 22
//-----------------------------------------------------------------------------


#ifndef qanLayout_h
#define qanLayout_h


// Qanava headers
#include "./qanGraph.h"
#include "./qanGridItem.h"


// QT headers
#include <QRectF>
#include <QProgressDialog>


//-----------------------------------------------------------------------------
namespace qan { // ::qan

	//! Abstract interface defining a graph layout algorithm (placement of node in space).
	/*!
		\nosubgrouping
	*/
    class Layout : public QObject
	{
	Q_OBJECT

	public:
	
		/*! \name Layout Constructor/Destructor *///---------------------------
		//@{
		//! Layout constructor.
        Layout( ) : QObject( ) { }

        //! Layout virtual destructor.
		virtual ~Layout( ) { }

	private:
        Q_DISABLE_COPY( Layout );
		//@}
		//---------------------------------------------------------------------



		/*! \name Layout Generation Management *///----------------------------
		//@{
	public:

		//! Layout nodes from a given group, report progress in an optional progress bar.
		virtual void	layout( qan::Node::List& rootNodes, qan::Node::Set& nodes, QRectF br, qan::Node* center, QProgressDialog* progress = 0 ) = 0;
		//@}
		//---------------------------------------------------------------------
	};



	//! Randomly layout an undirected graph.
	/*!
		\nosubgrouping
	*/
	class Random : public Layout
	{
		Q_OBJECT

		/*! \name Random Constructor/Destructor *///---------------------------
		//@{
	public:

		//! Random constructor.
		Random( ) : Layout( ) { qsrand( QDateTime::currentDateTime( ).toTime_t( ) ); }
		//@}
		//---------------------------------------------------------------------


		/*! \name Random Layout Generation Management *///---------------------
		//@{
	public:

		virtual void	layout( qan::Node::List& rootNodes, qan::Node::Set& nodes, QRectF br, qan::Node* center, QProgressDialog* progress = 0 );

	protected:

		void			layout( qan::Node::Set& nodes, QRectF br, qan::Node* center, QProgressDialog* progress = 0 );
		//@}
		//---------------------------------------------------------------------
	};



	//! Layout an undirected graph using a spring force algorithm.
	/*!
		\nosubgrouping
	*/
	class UndirectedGraph : public Layout
	{
		Q_OBJECT

		/*! \name UndirectedGraph Constructor/Destructor *///------------------
		//@{
	public:

		//! UndirectedGraph constructor.
		UndirectedGraph( ) : Layout( ), _center( "" ) { }
		//@}
		//---------------------------------------------------------------------



		/*! \name Force Layout Generation Management *///----------------------
		//@{
	public:

		virtual void	layout( qan::Node::List& rootNodes, qan::Node::Set& nodes, QRectF br, qan::Node* center, QProgressDialog* progress = 0 );

	public:

		void			layout( qan::Node::Set& rootNodes, qan::Node::Set& nodes, QRectF br, qan::Node* center, QProgressDialog* progress = 0 );

		static void		add( QPointF& a, const QPointF& b );

		static void		scale( QPointF& p, float scale );

		static QPointF	vector( QPointF a, QPointF b );

		static float	length( const QPointF& v );

		static float	length2( const QPointF& v );

	private:

		QPointF			computeRepulseForce( Node& node, Node::Set& adjacentNodes, Node::Set& nodes );

		QPointF			computeSpringForce( Node& node, Node::Set& adjacentNodes );

		Node			_center;
		//@}
		//---------------------------------------------------------------------
	};
} // ::qan
//-----------------------------------------------------------------------------


#endif // qanLayout_h

