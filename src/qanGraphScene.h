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
// \file	qanGraphScene.h
// \author	benoit@qanava.org
// \date	2005 November 22
//-----------------------------------------------------------------------------


#ifndef qanGraphScene_h
#define qanGraphScene_h


// Qanava headers
#include "./qanNodeItem.h"
#include "./qanEdgeItem.h"
#include "./qanGraphModel.h"
#include "./qanStyleManager.h"
#include "./qanNodeGroup.h"

// QT headers
#include <QAbstractItemModel>
#include <QAbstractItemView>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsItem>
#include <QMap>
#include <QStandardItemModel>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsItemGroup>


//-----------------------------------------------------------------------------
namespace qan { // ::qan

	class Grid;
	class Layout;

	//! Show a standard qan::Graph as a scene that could be displayed in a QGraphicsView.
	/*!
		\nosubgrouping
	*/
	class GraphScene : public QGraphicsScene, public GraphListener
	{
		Q_OBJECT

		/*! \name GraphScene Object Management *///----------------------------
		//@{
	public:

		//! GraphScene constructor with optionnal settings for graph widget initialization.
		GraphScene( StyleManager& _styleManager, QWidget* parent = 0, 
					QColor backColor = QColor( 170, 171, 205 ), QSize size = QSize( 300, 150 ) );

		//! GraphScene virtual destructor.
		virtual ~GraphScene( );

	public:

		StyleManager&	getStyleManager( ) {  return _styleManager; }

	protected:

		StyleManager&	_styleManager;
		//@}
		//---------------------------------------------------------------------



		/*! \name Scene Management *///----------------------------------------
		//@{
	public:

		//! Clear the scene from all its graphics elements (styles are not destroyed).
		void			clear( );

		//! FIXME v0.4.0
		void			updatePositions( Node* except = 0 );
		//@}
		//---------------------------------------------------------------------


		/*! \name Graph Topology Management *///-------------------------------
		//@{
	public:

		virtual void	init( Node::List& rootNodes ) { insertNodesGraphItems( rootNodes ); }

		virtual void	edgeInserted( qan::Edge& edge );

		virtual void	edgeModified( qan::Edge& edge );

		virtual void	edgeRemoved( qan::Edge& edge );

		virtual void	nodeInserted( qan::Node& node );

		virtual void	nodeRemoved( qan::Node& node );

		virtual void	nodeChanged( qan::Node& node );

	public:

		//! Create graph items for a set of given nodes and their sub-nodes.
		void	insertNodesGraphItems( Node::List& rootNodes );

	private:

		//! Create graph item for a given node and all its sub nodes.
		void	insertNodeGraphItem( qan::Node& node );

		void	insertEdgeGraphItem( qan::Edge& edge );
		//@}
		//---------------------------------------------------------------------


		/*! \name Node Group Management *///-----------------------------------
		//@{
	public:

		//! Add a node group to this scene, ownership goes to scene.
		virtual	void				addNodeGroup( qan::NodeGroup& nodeGroup );

		//! Remove a node group from this scene, ownership goes back to the caller.
		virtual void				removeNodeGroup( qan::NodeGroup& nodeGroup );

		QList< qan::NodeGroup* >&	getNodeGroups( ) { return _nodeGroups; }

	signals:

		void						nodeGroupAdded( qan::NodeGroup* nodeGroup );

		void						nodeGroupRemoved( qan::NodeGroup* nodeGroup );

	protected:

		QList< qan::NodeGroup* >	_nodeGroups;
		//@}
		//---------------------------------------------------------------------


		/*! \name Item Drag and Drop Management *///---------------------------
		//@{
	public:

		void						addDropTarget( QGraphicsItem* dropTarget ) { _dropTargets.append( dropTarget ); }

		QList< QGraphicsItem* >		getDropTargets( ) { return _dropTargets; }

		void						emitItemDragMove( qan::NodeItem* nodeItem, QGraphicsItem* target ) { emit itemDragMove( nodeItem, target ); }

		void						emitItemDragLeave( qan::NodeItem* nodeItem, QGraphicsItem* target ) { emit itemDragLeave( nodeItem, target ); }

		void						emitItemDropped( qan::NodeItem* nodeItem, QGraphicsItem* target ) { emit itemDropped( nodeItem, target ); }

	signals:

		void						itemDragMove( qan::NodeItem* nodeItem, QGraphicsItem* target );

		void						itemDragLeave( qan::NodeItem* nodeItem, QGraphicsItem* target );

		void						itemDropped( qan::NodeItem* nodeItem, QGraphicsItem* target );

	protected:

		QList< QGraphicsItem* >		_dropTargets;
		//@}
		//---------------------------------------------------------------------


		/*! \name Graph Item Management *///-----------------------------------
		//@{
	public:
	
		//! Get a graph item associed to a node.
		GraphItem*		getGraphItem( qan::Node& node ) const;

		//! Get a graph item associed to a node.
		GraphItem*		getGraphItem( qan::Edge& node ) const;

		//! Get the node at a selected scene position.
        Node*			getNodeAt( QPointF scenePos  );

	protected:

		//! Create a graphic item for a given node using registered graph item factories (and its associed graph item).
		void			createGraphItem( Node& node );

		//! Create a graphic item for a given edge using registered graph item factories (and its associed graph item).
		void			createGraphItem( Edge& edge );

		typedef	QMap< GraphItem*, QGraphicsItem* >	GraphItemGraphicItemMap;

		typedef	QMap< Node*, GraphItem* >			NodeGraphItemMap;

		typedef	QMap< Edge*, GraphItem* >			EdgeGraphItemMap;

		NodeGraphItemMap	_nodeGraphItemMap;

		EdgeGraphItemMap	_edgeGraphItemMap;
		//---------------------------------------------------------------------


		/*! \name Node Selection Management *///-------------------------------
		//@{
	public:

		void			emitShowItemPopup( qan::GraphItem* item, QPointF p ) { emit showItemPopup( item, p ); }

		void			emitHideItemPopup( qan::GraphItem* item ) { emit hideItemPopup( item ); }

	signals:

		//! Signal emmitted when a node is double clicked with mouse position in scene coordinate and screen coordinate (screen coordinates can be used for popup menus).
		void			nodeDoubleClicked( qan::Node* node, QPointF scenePos, QPoint screenPos );

		//! Signal emmitted when a node is right clicked with mouse position in scene coordinate and screen coordinate.
		void			nodeClicked( qan::Node* node, Qt::MouseButton button, QPointF scenePos, QPoint screenPos );

		void			showItemPopup( qan::GraphItem* item, QPointF p );

		void			hideItemPopup( qan::GraphItem* item );

	private:

		virtual	void	mouseDoubleClickEvent( QGraphicsSceneMouseEvent* mouseEvent );

		virtual	void	mouseReleaseEvent( QGraphicsSceneMouseEvent* mouseEvent );
		//---------------------------------------------------------------------


		/*! \name Graph Item Factory Management *///---------------------------
		//@{
	public:


		//! Add a graph item factory (factory will be owned by this scene graph).
		/*! Note: factory will be added first in the internal list of factory, so a proprietary factory inserted this way will
		    override the standard edge and node factories. */
		// TODO 0.4.0: Destroy the factory list in GraphScene dtor.
		void						addGraphItemFactory( GraphItem::Factory*	factory ) { _graphItemFactories.prepend( factory ); }

		//! Get a graph item factory for a given node or edge classname.
		GraphItem::Factory*			getGraphItemFactory( QString className );

		//! Get a graph item factory list for a given node or edge classname.
		GraphItem::Factory::List	getGraphItemFactories( QString className );

	private:

		GraphItem::Factory::List	_graphItemFactories;
		//@}
		//---------------------------------------------------------------------
	};
} // ::qan
//-----------------------------------------------------------------------------


#endif // qanGraphScene_h

