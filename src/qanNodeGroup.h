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
// \file	qanNodeGroup.h
// \author	benoit@qanava.org
// \date	2014 april 19
//-----------------------------------------------------------------------------


#ifndef qanNodeGroup_h
#define qanNodeGroup_h

// QT headers
#include <QGraphicsItem>


// Qanava headers
#include "./qanNodeItem.h"
#include "./qanStyleManager.h"


//-----------------------------------------------------------------------------
namespace qan { // ::qan

	class Grid;
	class Layout;



	//! Models a group of nodes showing a consistent layout and behaviour.
	/*!
		\nosubgrouping
	*/
	class NodeGroup : public QObject, public QGraphicsItem
	{
		Q_OBJECT

		/*! \name NodeGroup Object Management *///-----------------------------
		//@{
	public:

		NodeGroup( qan::GraphScene& scene, QString name, QGraphicsItem* parent = 0 );

		qan::GraphScene& getScene( ) { return _scene; }

	protected:
	
		qan::GraphScene& _scene;
		//@}
		//---------------------------------------------------------------------
	

		/*! \name Group Management *///----------------------------------------
		//@{
	public:

		virtual void	setLayout( qan::Layout* layout );

		qan::Layout*	getLayout( ) { return _layout; }

		virtual void	layout( );

		virtual void	setVisible( bool v );

		Properties&		getProperties( ) { return _properties; }

	protected:
	
		qan::Layout*	_layout;

		Properties		_properties;
		//@}
		//---------------------------------------------------------------------


		/*! \name Group Name Management *///-----------------------------------
		//@{
	public:

		QString					getName( ) { return _name; }

	protected:

		//! Could called from a subclass to disable the group name edition, or modify editor position by calling its setPos() method.
		QGraphicsTextItem*		getNameItem( ) { return _nameItem; }

	signals:
		
		QString					nameChanged( qan::NodeGroup* nodeGroup, QString name );

	private:
	
		QString					_name;

		LabelEditorItem*		_nameItem;

	protected slots:

		void					nameTextModified( );
		//@}
		//---------------------------------------------------------------------


		/*! \name Drag and Drop Management *///--------------------------------
		//@{
	public:

		//! \return true if group accept node drag and drop to automatically add node item to this group.
		bool			getAcceptDrops( ) const { return _acceptDrops; }

		//! Set to true if the group accept node item drops to automatically add node item to this group.
		void			setAcceptDrops( bool acceptDrops ) { _acceptDrops = acceptDrops; }

	protected:

		bool			_acceptDrops;
		
		//! Internally used as a flag when a node item is dragged other this group.
		bool			_dragOver;

	protected slots:
		
		void			itemDragMove( qan::NodeItem* item, QGraphicsItem* target );

		void			itemDragLeave( qan::NodeItem* item, QGraphicsItem* target );

		void			itemDropped( qan::NodeItem* item, QGraphicsItem* target );
		//@}
		//---------------------------------------------------------------------


		/*! \name Graphics Item Management *///--------------------------------
		//@{
	public:

		virtual QRectF			boundingRect( ) const;

		virtual QSizeF			getMinimumSize( ) const { return QSizeF( 170., 75. ); }

		//! Generate a bounding box taking into account bounding rect from group nodes. Return the bounding rect in scene coordinate system.
		virtual	QRectF			updateBoundingRect( );

		void					paint( QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = 0 );

		void					setElementItem( void* element, QGraphicsItem* item ) { _elementItemMap.insert( element, item ); }

		QList< QGraphicsItem* >	getElementItems( void* element ) { return _elementItemMap.values( element ); }

		void					removeElementItem( void* element ) { _elementItemMap.remove( element ); }

		//! Set keepTopLeft to true if you do not want layout to move the group.
		void					updateNodesPositions( bool keepTopLeft = false );

	protected:

		QRectF					getNodesSceneBoundingRect( ) const;
		
		//! Solid group bounding rect.
		QRectF					_br;

		QMultiMap< void*, QGraphicsItem* >	_elementItemMap;

		QGraphicsRectItem*		getBackgroundItem( ) { return _background; }

	private:

		//! Optional group background
		QGraphicsRectItem*		_background;
		//@}
		//---------------------------------------------------------------------


		/*! \name Group Node Management *///-----------------------------------
		//@{
	public:

		virtual void		addNode( qan::Node& node );

		//! Default implementation update the layout when an edge is added, override that method to avoid that standard behaviour.
		virtual void		addEdge( qan::Edge& edge );

		bool				hasNode( qan::Node& node ) { return _nodes.contains( &node ); }

		virtual void		removeNode( qan::Node& node );

		qan::Node::Set&		getNodes( ) { return _nodes; }

		const qan::Node::Set&	getNodes( ) const { return _nodes; }

		//! Get an unordered list of root nodes for this group (a node will not appears multiple times).
		/*! A node will be a group root node if it has no "in" nodes, or if its "in" nodes are not registered in the group.	*/
		void				getRootNodes( qan::Node::Set& rootNode );

	protected:

		qan::Node::Set		_nodes;

	protected slots:

		void				itemMoved( QPointF curPos, QPointF oldPos );
		//@}
		//---------------------------------------------------------------------


		/*! \name Group Mouse Management *///----------------------------------
		//@{
	public:

		//! Set to true if added node became child items of this group.
		void			setAddAsChilds( bool addAsChilds ) { _addAsChilds = addAsChilds; }

	protected:

		virtual	void	mouseMoveEvent( QGraphicsSceneMouseEvent* e );
		virtual void	mousePressEvent( QGraphicsSceneMouseEvent* e );
		virtual void	mouseReleaseEvent( QGraphicsSceneMouseEvent* e );
		virtual void	wheelEvent( QGraphicsSceneWheelEvent* e );

		//! Could be overriden by sub classes to update layout when the group is moved (bounding rect will be updated before this method is called).
        virtual	void	groupMoved( QPointF curPos, QPointF oldPos ) { Q_UNUSED( curPos ); Q_UNUSED( oldPos ); }

		QPointF			_mousePos;
		bool			_mousePressed;
		bool			_addAsChilds;
		//@}
		//---------------------------------------------------------------------
	};
} // ::qan
//-----------------------------------------------------------------------------


#endif // qanNodeGroup_h

