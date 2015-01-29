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
// \file	qanNodeGroup.h
// \author	benoit@qanava.org
// \date	2014 april 19
//-----------------------------------------------------------------------------


#ifndef qanNodeGroup_h
#define qanNodeGroup_h

// QT headers
#include <QGraphicsItem>
#include <QGraphicsLayoutItem>
#include <QGraphicsLinearLayout>

// Qanava headers
#include "./qanNodeRectItem.h"
#include "./qanStyleManager.h"


//-----------------------------------------------------------------------------
namespace qan { // ::qan

	class Grid;
	class Layout;


    //! Model a group of nodes showing a consistent layout and behaviour.
    /*! qan::NodeGroup support two kind of layouts:
            \li Graphics layout based on QGraphicsLayout can be used in a way similar to QGraphicsWidget with setGraphLayout() method.
            \li Qanava graph layout based on qan::Layout could be used with the setGraphLayout() method.
            \li Using QGraphicsLayout and qan::Layout simultaneously will lead to unpredictable results (warnings will be issued in debug mode).

        Since qan::NodeGroup inherits from QGraphicsLayoutItem, it can be inserted in QGraphicsLayout
        layouts (such as QGraphicsLinearLayout), alternatively, group content could be laid out using a graphics layout by calling
        \c setLayout() with a QGraphicLinearLayout for example. Nodes could be laidout after insertion in a group using a Qanava
        layout (qan::HierarchyTree or qan::UndirectedGraph) sets via setGraphLayout(). Before adding a group in a standard Qt
        graphics layout, take care of disabling node drag move support by calling \c setMovable( false ).

        While the drag and drop interface is similar to Qt, Qanava node group provide a custom mechanism to
        directly insert node via drag and drop. %Node dragging is accepted by default, use \c setAcceptDrops( false )
        to disable qan::NodeItem insertion via drag and drop.

        When a group is added direclty in a graphics layout hosted in a QGraphicsWidget based item, graphics widget should
        be used as a \c parent argument at group creation (you could also call \c setParentGraphicsItem() after creation).

		\nosubgrouping
	*/
    class NodeGroup : public QGraphicsObject, public QGraphicsLayout
	{
		Q_OBJECT
        Q_INTERFACES( QGraphicsLayout )

		/*! \name NodeGroup Object Management *///-----------------------------
		//@{
	public:

        //! %NodeGroup constructor.
        /*!
         * \param scene     Graphics scene when the group should be added (call qan::GraphScene::addNodeGroup() with
         *                  the created group).
         * \param name      Name will be shown at the top left corner of the group graphics item (it is
         *                  dynamically editable by the user or can be set manually with setName() method).
         * \param parent    Default to 0, node groups should be top level scene graphics items.
         */
        NodeGroup( qan::GraphScene& scene, QString name, QGraphicsItem* parent = 0 );

		qan::GraphScene& getScene( ) { return _scene; }

    protected:
	
		qan::GraphScene& _scene;
		//@}
		//---------------------------------------------------------------------


        /*! \name GroupLayout  Management *///---------------------------------
		//@{
	public:

        virtual void	setGraphLayout( qan::Layout* graphLayout );

        virtual void	setLayout( QGraphicsLinearLayout* layout );

        qan::Layout*	getGraphLayout( ) { return _graphLayout; }

		virtual void	layout( );

		Properties&		getProperties( ) { return _properties; }

	protected:
	
        qan::Layout*	_graphLayout;

        QGraphicsLinearLayout* _layout;

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
		
        virtual void	itemDragMove( qan::NodeItem* item, QGraphicsItem* target );

        virtual void	itemDragLeave( qan::NodeItem* item, QGraphicsItem* target );

        virtual void	itemDropped( qan::NodeItem* item, QGraphicsItem* target );
		//@}
		//---------------------------------------------------------------------


		/*! \name Graphics Item Management *///--------------------------------
		//@{
	public:

		virtual QRectF			boundingRect( ) const;

        void					paint( QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = 0 );

        virtual void            updateGroup( );

        virtual void            setVisible( bool v );

    protected slots:

        void                    itemMoved( QPointF curPos, QPointF oldPos );

    protected:

        //! .
        virtual	void            groupMoved( QPointF curPos, QPointF oldPos );

		QRectF					getNodesSceneBoundingRect( ) const;
		
		//! Solid group bounding rect.
		QRectF					_br;

		QGraphicsRectItem*		getBackgroundItem( ) { return _background; }

	private:

		//! Optional group background
		QGraphicsRectItem*		_background;
		//@}
		//---------------------------------------------------------------------


        /*! \name Decoration Items Management *///-----------------------------
        //@{
    public:

        void                                setElementItem( void* element, QGraphicsItem* item ) { _elementItemMap.insert( element, item ); }

        QList< QGraphicsItem* >             getElementItems( void* element ) { return _elementItemMap.values( element ); }

        void                                removeElementItem( void* element ) { _elementItemMap.remove( element ); }

    protected:

        QMultiMap< void*, QGraphicsItem* >	_elementItemMap;
        //@}
        //---------------------------------------------------------------------



        /*! Graphics layout item implementation *///---------------------------
        //@{
    public:
        virtual void    updateGeometry( );
        virtual void    setGeometry( const QRectF& geom );

        virtual int     count( ) const;
        virtual QGraphicsLayoutItem*    itemAt( int i ) const;
        virtual void	removeAt( int index );

    protected:
        virtual QSizeF  sizeHint( Qt::SizeHint which, const QSizeF & constraint = QSizeF( ) ) const;
        //@}
        //---------------------------------------------------------------------


		/*! \name Group Node Management *///-----------------------------------
		//@{
	public:

        //! Insert a given node in this group.
        /*! Insertion will fails if no layout has been set for this group with
         * setLayout() or setGraphLayout() methods.*/
		virtual void		addNode( qan::Node& node );

        //! Default implementation updates the layout when an edge is added, override that method to disable this standard behaviour.
		virtual void		addEdge( qan::Edge& edge );

		bool				hasNode( qan::Node& node ) { return _nodes.contains( &node ); }

		virtual void		removeNode( qan::Node& node );

		qan::Node::Set&		getNodes( ) { return _nodes; }

		const qan::Node::Set&	getNodes( ) const { return _nodes; }

        //! Get an unordered set of root nodes for this group (a node will not appears multiple times).
		/*! A node will be a group root node if it has no "in" nodes, or if its "in" nodes are not registered in the group.	*/
		void				getRootNodes( qan::Node::Set& rootNode );

	protected:

		qan::Node::Set		_nodes;
		//@}
		//---------------------------------------------------------------------


		/*! \name Group Mouse Management *///----------------------------------
		//@{
	public:

        //! When set to false, prevent the group to be dragged by mouse (for example, when the group is part of a graphice view layout).
        /*! \default true           */
        void            setMovable( bool isMovable ) { _isMovable = isMovable; }

	protected:

		virtual	void	mouseMoveEvent( QGraphicsSceneMouseEvent* e );
		virtual void	mousePressEvent( QGraphicsSceneMouseEvent* e );
		virtual void	mouseReleaseEvent( QGraphicsSceneMouseEvent* e );
		virtual void	wheelEvent( QGraphicsSceneWheelEvent* e );

		QPointF			_mousePos;
		bool			_mousePressed;
        bool            _isMovable;
		//@}
		//---------------------------------------------------------------------
	};
} // ::qan
//-----------------------------------------------------------------------------


#endif // qanNodeGroup_h

