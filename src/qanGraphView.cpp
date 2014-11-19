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
// \file	qanGraphView.cpp
// \author	benoit@qanava.org
// \date	2006 July 29
//-----------------------------------------------------------------------------


// Qanava headers
#include "./qanGraphView.h"
#include "./qanGridItem.h"


// QT headers
#include <QPen>
#include <QMouseEvent>
#include <QMimeData>


//-----------------------------------------------------------------------------
namespace qan { // ::qan


/* GraphView Constructors/Destructors *///----------------------------------
GraphView::GraphView( QWidget* parent, QColor backColor, QSize size ) :
	QGraphicsView( parent ),
	_graph( 0 ),
	_grid( 0 ),
	_controllerManager( this )
{
	configureView( backColor, size );
	setAcceptDrops( true );	// Accept file drop, and cast the dataDropped signal
	getControllerManager( ).registerController( new PanController( *this ) );
	getControllerManager( ).registerController( new ZoomController( *this ) );

    viewport( )->setAttribute( Qt::WA_AcceptTouchEvents );
}

void	GraphView::configureView( QColor backColor, QSize size )
{
    Q_UNUSED( size );
	setHorizontalScrollBarPolicy( Qt::ScrollBarAsNeeded );
	setVerticalScrollBarPolicy( Qt::ScrollBarAsNeeded );
	setAlignment( Qt::AlignLeft | Qt::AlignTop );
	setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );

	QPalette palette;
	palette.setColor( backgroundRole( ), backColor );
	viewport( )->setPalette( palette );
}

void	GraphView::setGraph( Graph& graph )
{
	_graph = &graph;
	GraphScene& graphScene = graph.getM( );
	
	// FIXME v0.4.0
	graphScene.setItemIndexMethod( QGraphicsScene::NoIndex );
	
	setCacheMode( CacheBackground );
    setViewportUpdateMode( BoundingRectViewportUpdate );
    setRenderHint( QPainter::Antialiasing );
    setTransformationAnchor( AnchorUnderMouse );

	graphScene.init( graph.getRootNodes( ) );
	setScene( &graphScene );
}
//-----------------------------------------------------------------------------



/* Grid Management *///--------------------------------------------------------
void	GraphView::drawBackground( QPainter* painter, const QRectF& rect )
{
	if ( _grid != 0 )
		_grid->drawBackground( *painter, rect );
}
//-----------------------------------------------------------------------------



/* View Controller Management *///---------------------------------------------
void	GraphView::keyPressEvent( QKeyEvent* e )
{
	QGraphicsView::keyPressEvent( e );

	if ( _controllerManager.keyPressEvent( e ) )
	{
		viewport( )->update( );
		e->accept( );
	} else e->ignore( );
}

void	GraphView::mousePressEvent( QMouseEvent* e )
{
	QGraphicsView::mousePressEvent( e );

	if ( _controllerManager.mousePressEvent( e ) )
	{
		viewport( )->update( );
		e->accept( );
	}
	else e->ignore( );
}

void	GraphView::mouseReleaseEvent( QMouseEvent* e )
{
	QGraphicsView::mouseReleaseEvent( e );

	if ( _controllerManager.mouseReleaseEvent( e ) )
	{
		viewport( )->update( );
		e->accept( );
	}
	else e->ignore( );
}

void	GraphView::mouseMoveEvent( QMouseEvent* e )
{
	if ( _controllerManager.mouseMoveEvent( e ) )
	{
		viewport( )->update( );
		e->accept( );
	}
	else
	{
		e->ignore( );
		QGraphicsView::mouseMoveEvent( e );
	}
}

void	GraphView::mouseDoubleClickEvent( QMouseEvent* e )
{
	if ( _controllerManager.mouseDoubleClickEvent( e ) )
		viewport( )->update( );

	QGraphicsItem* item = itemAt( e->pos( ) );
	if ( item != 0 )
		emit itemDoubleClicked( item->parentItem( ) != 0 ? item->parentItem( ) : item );

	QGraphicsView::mouseDoubleClickEvent( e );
}

void	GraphView::wheelEvent( QWheelEvent* e )
{
	if ( _controllerManager.wheelEvent( e ) )
	{
		viewport( )->update( );
		e->accept( );
	}
	else
		QGraphicsView::wheelEvent( e );
}

void	GraphView::resizeEvent ( QResizeEvent* e )
{
	QGraphicsView::resizeEvent( e );

	emit viewResized( e->size( ) );
}

bool    GraphView::viewportEvent( QEvent* e )
{
    switch ( e->type( ) )
    {
    case QEvent::TouchBegin:
    case QEvent::TouchUpdate:
    case QEvent::TouchEnd:
        if ( _controllerManager.touchEvent( static_cast< QTouchEvent* >( e ) ) )
        {
            viewport( )->update( );
            return true;
        }
    default:
        break;
    }
    return QGraphicsView::viewportEvent( e );
}
/*!
	\return an action with the given name, or 0 if no such action exists in the actually registered controllers of this graphview.
*/
QAction*	GraphView::getControllerAction( QString name )
{
	Controller* controller = _controllerManager.getController( name );
	if ( controller != 0 )
		return controller->getAction( );

	foreach ( Controller* controller, _controllerManager.getControllers( ) )
	{
		QAction* action = controller->getAction( name );  // Look for an action by name
		if ( action != 0 )
			return action;
	}
	return 0;
}

/*!
	\return a widget with the given name and this graphview as parent, or 0 if no such widget exists in the registered controllers of this graphview.
*/
QWidget*	GraphView::getControllerWidget( QString name )
{
	foreach ( Controller* controller, _controllerManager.getControllers( ) )
	{
		QWidget* widget = controller->getWidget( name );
		if ( widget != 0 )
			return widget;
	}
	return 0;
}
//-----------------------------------------------------------------------------


/* Content Drag and Drop Management *///---------------------------------------
void	GraphView::dragEnterEvent( QDragEnterEvent* e )
{
	if ( e->mimeData( )->hasFormat( "holographe/file" ) )
		e->accept( );
	else
		QGraphicsView::dragEnterEvent( e );
}

void GraphView::dragMoveEvent( QDragMoveEvent* e )
{
	if ( e->mimeData( )->hasFormat( "holographe/file" ) ) 
	{
		e->setDropAction( Qt::MoveAction );
		e->accept( );
	}
	else
		QGraphicsView::dragMoveEvent( e );
}

void	GraphView::dropEvent( QDropEvent* e )
{
	if ( e->mimeData( )->hasFormat( "holographe/file" ) )
	{
		emit dataDropped( e->mimeData( )->data( "holographe/file" ) );
		e->accept( );
		update( );
	}
	else
		QGraphicsView::dropEvent( e );
}
//---------------------------------------------------------------------


} // ::qan


