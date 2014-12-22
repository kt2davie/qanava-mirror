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
// \file	qanController.cpp
// \author	benoit@qanava.org
// \date	2003 August 13
//-----------------------------------------------------------------------------


// Qanava headers
#include "./qanStyleManager.h"
#include "./qanController.h"
#include "./qanGraphView.h"
#include "./qanGraphicsContainer.h"


// QT headers
#include <QApplication>
#include <QMatrix>
#include <QTimer>
#include <QPixmap>
#include <QKeyEvent>
#include <QResizeEvent>
#include <QMouseEvent>
#include <QScrollBar>


namespace qan {  // ::qan


bool	Controller::Manager::keyPressEvent( QKeyEvent* e )
{
	foreach ( Controller* controller, _controllers )
		if ( controller->keyPressEvent( e ) )
			return true;
	return false;
}

bool	Controller::Manager::mousePressEvent( QMouseEvent* e )
{
	foreach ( Controller* controller, _controllers )
		if ( controller->mousePressEvent( e ) )
			return true;
	return false;
}

bool	Controller::Manager::mouseReleaseEvent( QMouseEvent* e )
{
	foreach ( Controller* controller, _controllers )
		if ( controller->mouseReleaseEvent( e ) )
			return true;
	return false;
}

bool	Controller::Manager::mouseMoveEvent( QMouseEvent* e )
{
	foreach ( Controller* controller, _controllers )
		if ( controller->mouseMoveEvent( e ) )
			return true;
	return false;
}

bool	Controller::Manager::mouseDoubleClickEvent( QMouseEvent* e )
{
	foreach ( Controller* controller, _controllers )
		if ( controller->mouseDoubleClickEvent( e ) )
			return true;
	return false;
}

bool	Controller::Manager::wheelEvent( QWheelEvent* e )
{
	foreach ( Controller* controller, _controllers )
		if ( controller->wheelEvent( e ) )
			return true;
	return false;
}

bool	Controller::Manager::touchEvent( QTouchEvent* e )
{
    foreach ( Controller* controller, _controllers )
        if ( controller->touchEvent( e ) )
            return true;
    return false;
}

Controller*		Controller::Manager::getController( QString name )
{
	foreach ( Controller* controller, _controllers )
		if ( controller->getName( ) == name )
			return controller;
	return 0;
}

void	Controller::Manager::registerController( Controller* controller )
{
	if ( controller == 0 )
		return;

	_controllers.append( controller );
	QAction* controllerAction = controller->getAction( );
	if ( controllerAction->isCheckable( ) )
		_actionGroup->addAction( controllerAction );  // Only one checkable action active at a time
}

bool	Controller::Manager::allItemsDisabled( QList< QGraphicsItem* >& items )
{
	foreach ( const QGraphicsItem* item, items )
		if ( item->isEnabled( ) )
			return false;
	return true;
}

Controller::Controller( QString name, GraphView& graphView ) : 
	_graphView( graphView ),
	_action( 0 ),
	_name( name )
{ 

}


/* PanController Constructor/Destructor *///-----------------------------------
PanController::PanController( GraphView& GraphView ) : 
	Controller( "pan_ctrl_toggle_pan", GraphView ),
	_keyboardNavigation( true ),
	_keyboardNavigationIntensity( 5.0 ),
	_mode( NONE )
{
	QAction* action = new QAction( this );
	action->setCheckable( true );
	action->setText( "Pan" );
	action->setVisible( true );
	action->setIcon( QIcon( "images/qanava_pan.png" ) );
	connect( action, SIGNAL( toggled( bool ) ), this, SLOT( toggled( bool ) ) );
	setAction( action );
	setKeyboardNavigation( true );

	action->toggle( );
}

void	PanController::toggled( bool state )
{
	setMode( state ? PAN : NONE );
	getGraphView( ).setDragMode( state ? QGraphicsView::ScrollHandDrag : QGraphicsView::NoDrag );
	if ( getMode( ) == PAN )
		getGraphView( ).viewport( )->setCursor( QCursor( Qt::ArrowCursor ) );	
	getGraphView( ).setResizeAnchor( QGraphicsView::AnchorUnderMouse );
	getGraphView( ).setTransformationAnchor( QGraphicsView::AnchorUnderMouse );
}
//-----------------------------------------------------------------------------



/* PanController Keyboard Management *///--------------------------------------
bool	PanController::keyPressEvent( QKeyEvent* e )
{
	int dx( 0 );
	int dy( 0 );

	getGraphView( ).setFocusPolicy( Qt::ClickFocus );
	if ( getKeyboardNavigation( ) )
	{
		switch ( e->key( ) )
		{
		case Qt::Key_Left:	dx -= ( int )_keyboardNavigationIntensity; break;
		case Qt::Key_Right:	dx += ( int )_keyboardNavigationIntensity; break;
		case Qt::Key_Up:	dy -= ( int )_keyboardNavigationIntensity;  break;
		case Qt::Key_Down:	dy += ( int )_keyboardNavigationIntensity; ; break;
		default:
			e->ignore( ); break;
		}
	}

	QScrollBar* v = getGraphView( ).verticalScrollBar( );
	QScrollBar* h = getGraphView( ).horizontalScrollBar( );
	v->setValue( v->value( ) + dy );
	h->setValue( h->value( ) + dx );

	return false;
}

/*! To use keyboard navigation, ensure that this widget parent widget has at least a 'Click'
    focus policy (in Designer or with the QWidget::setFocusPolicy() method).
*/
void	PanController::setKeyboardNavigation( bool state )
{
	_keyboardNavigation = state;
	if ( state )
	{
		getGraphView( ).viewport( )->setFocusPolicy( Qt::StrongFocus );
		getGraphView( ).setFocusPolicy( Qt::StrongFocus );
	}
}
//-----------------------------------------------------------------------------



/* PanController Mouse Management *///-----------------------------------------
bool	PanController::mouseReleaseEvent( QMouseEvent* e )
{
    Q_UNUSED( e );
	if ( getMode( ) == PAN )
		getGraphView( ).viewport( )->setCursor( QCursor( Qt::ArrowCursor ) );
	return false;
}
//-----------------------------------------------------------------------------



/* ZoomController Constructor/Destructor *///----------------------------------
ZoomController::ZoomController( GraphView& graphView ) : 
	Controller( "zoom_controller", graphView ),
	_zoomRectStart( 0., 0. ),
	_zoomRectItem( 0 ),
	_zoomRectState( ZOOMRECT_NONE ),
	_actionZoomRect( new QAction( this ) ),
	_actionZoomNav( new QAction( this ) ),
    _pinchZoomStart( 1.0 ),
	_zoomMin( 0.25 ),
	_zoomMax( 4.0 ),
	_actionZoomIn( new QAction( this ) ),
	_actionZoomOut( new QAction( this ) ),
	_zoomSlider( 0 )
{
	_actionZoomRect->setCheckable( true );
	_actionZoomRect->setChecked( false );
	_actionZoomRect->setText( "Zoom Window" );
	_actionZoomRect->setVisible( true );
	_actionZoomRect->setIcon( QIcon( "images/qanava_zoomwindow.png" ) );
	setAction( _actionZoomRect );

	_actionZoomNav->setCheckable( true );
	_actionZoomNav->setText( "Zoom Navigation" );
	_actionZoomNav->setVisible( true );
	_actionZoomNav->setChecked( true );


	_zoomRectItem = 0;		// Zoom rectangle is created on the first mouse press event because the graphview owning this 
							// controller does not necessarilly already have an associed scene at this point.

	_actionZoomIn->setText( "Zoom In" );
	_actionZoomIn->setVisible( true );
	_actionZoomIn->setIcon( QIcon( "images/qanava_zoomin.png" ) );
	connect( _actionZoomIn, SIGNAL( triggered( bool ) ), this, SLOT( zoomIn( bool ) ) );

	_actionZoomOut->setText( "Zoom Out" );
	_actionZoomOut->setVisible( true );
	_actionZoomOut->setIcon( QIcon( "images/qanava_zoomout.png" ) );
	connect( _actionZoomOut, SIGNAL( triggered( bool ) ), this, SLOT( zoomOut( bool ) ) );

	_zoomSlider = new QSlider( Qt::Horizontal, &graphView );
	_zoomSlider->setMinimum( 1 );
	_zoomSlider->setMaximum( 99 );
	_zoomSlider->setPageStep( 10 );
	_zoomSlider->setSliderPosition( 50 );
	_zoomSlider->setMinimumWidth( 190 );
	connect( _zoomSlider, SIGNAL( valueChanged(int) ), this, SLOT( zoomSliderValueChanged(int) ) );
	updateZoomSlider( );
}
//-----------------------------------------------------------------------------



/* Zoom Window Management *///-------------------------------------------------
bool	ZoomController::mousePressEvent( QMouseEvent* e )
{
	// Navigation must not occurs if a node is under the mouse
	QPointF p = getGraphView( ).mapToScene( e->pos( ) );
	QList< QGraphicsItem* > items = getGraphView( ).scene( )->items( p );
	if ( ( items.size( ) == 0  || Manager::allItemsDisabled( items ) ) && _actionZoomRect->isChecked( ) )
	{
		if ( _zoomRectItem == 0 )
		{
			_zoomRectItem = new QGraphicsRectItem( QRectF( 0., 0., 50., 50. ), 0 );
			getGraphView( ).scene( )->addItem( _zoomRectItem );
			_zoomRectItem->setZValue( 100. );
			QPen p;
			p.setStyle( Qt::DotLine );
			p.setWidth( 1 );
			p.setColor( QColor( 55, 55, 55 ) );
			_zoomRectItem->setPen( p );
		}

		_zoomRectStart = getGraphView( ).mapToScene( e->pos( ) );
		_zoomRectItem->setVisible( true );
		_zoomRectItem->setRect( QRectF( _zoomRectStart, QSizeF( 1., 1. ) ) );
		setState( ZOOMRECT_ZOOMING );
		return true;
	}
	return false;
}

bool	ZoomController::mouseReleaseEvent( QMouseEvent* e )
{
    Q_UNUSED( e );

	if ( _actionZoomRect->isChecked( ) && getState( ) == ZOOMRECT_ZOOMING )
	{
		// Verify that zoom window rect is not too small, creating a too large zoom factor
		QRectF zoomRect = _zoomRectItem->rect( ).normalized( );
		zoomRect.setWidth( zoomRect.width( ) > 25. ? zoomRect.width( ) : 25.0 );
		zoomRect.setHeight( zoomRect.height( ) > 25. ? zoomRect.height( ) : 25.0 );
		
		// Apply the zoom via QGraphicsView fitInView, get back the zoom, and update our zoom system for min/max checking and zoombar update
		getGraphView( ).fitInView( zoomRect, Qt::KeepAspectRatioByExpanding );
		updateZoomSlider( );	

		_zoomRectItem->setVisible( false );
		setState( ZOOMRECT_NONE );  // Stop zooming until next click
		_actionZoomRect->setChecked( false );	// Disable the zoom rect mode after it has been activated once, so that the user can drag the new view
		return true;
	}
	return false;
}

bool	ZoomController::mouseMoveEvent( QMouseEvent* e )
{
	if ( _actionZoomRect->isChecked( ) && getState( ) == ZOOMRECT_ZOOMING && _zoomRectItem != 0 )
	{
		QPointF p = getGraphView( ).mapToScene( e->pos( ) );

		QRectF r( _zoomRectStart.x( ), _zoomRectStart.y( ),
				  p.x( ) - _zoomRectStart.x( ), p.y( ) - _zoomRectStart.y( ) );
		_zoomRectItem->setRect( r );
		_zoomRectItem->show( );
		return true;
	}
	return false;
}

QGraphicsItem*      ZoomController::lookForParent( QGraphicsItem* item, int type )
{
    if ( item->type( ) == type )
        return item;
    return ( item->parentItem( ) != 0 ? lookForParent( item->parentItem( ), type ) : 0 );
}
//-----------------------------------------------------------------------------



/* Zoom In/Out Management *///-------------------------------------------------
bool		ZoomController::wheelEvent( QWheelEvent* e )
{
    QPointF p = getGraphView( ).mapToScene( e->pos( ) );

    if ( _actionZoomNav->isChecked( ) )
    {
        QGraphicsItem* item = getGraphView( ).scene( )->itemAt( p, QTransform( ) );
        if ( item == 0 ||
             lookForParent( item, qan::GraphicsContainer::Type ) == 0 ) // Do not catch wheel event for widget in container items
        {
            getGraphView( ).setTransformationAnchor( QGraphicsView::AnchorUnderMouse );

            double acceleration = qAbs( e->delta( ) ) / 120.;
            double scaleDelta = 1.15 * acceleration;
            double scaleFactor = ( e->delta( ) > 0. ? scaleDelta : 1.0 / scaleDelta );
            if ( e->orientation( ) == Qt::Vertical )
            {
                getGraphView( ).scale( scaleFactor, scaleFactor );
                updateZoomSlider( );
            }
            return true;
        }
    }
    return false;
}

bool	ZoomController::touchEvent( QTouchEvent* e )
{
    // Slighty modified code from qt mice pinchzoom example
    switch ( e->type( ) )
    {
    case QEvent::TouchBegin:
    case QEvent::TouchUpdate:
    case QEvent::TouchEnd:
        {
            QList< QTouchEvent::TouchPoint > touchPoints = e->touchPoints( );
            if ( touchPoints.count( ) == 2 )
            {
                // Determine scale factor
                const QTouchEvent::TouchPoint &touchPoint0 = touchPoints.first();
                const QTouchEvent::TouchPoint &touchPoint1 = touchPoints.last();
                qreal currentScaleFactor =
                        QLineF( touchPoint0.pos( ), touchPoint1.pos( ) ).length( ) /
                        QLineF( touchPoint0.startPos( ), touchPoint1.startPos( ) ).length( );

                if ( e->touchPointStates( ) & Qt::TouchPointReleased )
                {
                    // if one of the fingers is released, remember the current scale
                    // factor so that adding another finger later will continue zooming
                    // by adding new scale factor to the existing remembered value.
                    _pinchZoomStart *= currentScaleFactor;
                    currentScaleFactor = 1.;
                }
                getGraphView( ).setTransform( QTransform().scale( _pinchZoomStart * currentScaleFactor,
                                       _pinchZoomStart * currentScaleFactor ) );
                //updateZoomSlider( );
                return true;
            }
            break;
        }
    default:
        break;
    }
    return false;
}

QAction*	ZoomController::getAction( QString name )
{
    if ( name == "zoom_ctrl_zoom_in" )
        return _actionZoomIn;
    else if ( name == "zoom_ctrl_zoom_out" )
        return _actionZoomOut;
    else if ( name == "zoom_ctrl_toggle_zoom_rect" )
        return _actionZoomRect;
    else if ( name == "zoom_ctrl_toggle_zoom_nav" )
        return _actionZoomNav;

    return 0;
}

QWidget*	ZoomController::getWidget( QString name )
{
    return ( name == "zoom_ctrl_zoom_slider" ? _zoomSlider : 0 );
}

void		ZoomController::zoomIn( bool state )
{
    Q_UNUSED( state );

    getGraphView( ).setTransformationAnchor( QGraphicsView::AnchorViewCenter );
	getGraphView( ).scale( 1.1, 1.1 );	
	updateZoomSlider( );
}

void		ZoomController::zoomOut( bool state )
{
    Q_UNUSED( state );

    getGraphView( ).setTransformationAnchor( QGraphicsView::AnchorViewCenter );
	getGraphView( ).scale( 0.9, 0.9 );
	updateZoomSlider( );
}

void		ZoomController::zoomSliderValueChanged( int value )
{
	double zoom = _zoomMin + ( value * ( _zoomMax - _zoomMin ) / 100.0 );

	QTransform m = getGraphView( ).transform( );
	double actualZoom = m.m11( );

	// Scale from the current scale factor to the desired one centering on the view center...
	getGraphView( ).setTransformationAnchor( QGraphicsView::AnchorViewCenter );
	getGraphView( ).scale( zoom / actualZoom, zoom / actualZoom );
}

void		ZoomController::updateZoomSlider( )
{
	QTransform m = getGraphView( ).transform( );
	double zoom = m.m11( );

	if ( zoom > _zoomMax )
		zoom = _zoomMax;
	if ( zoom < _zoomMin )
		zoom = _zoomMin;

	disconnect( _zoomSlider, SIGNAL( valueChanged(int) ), this, SLOT( zoomSliderValueChanged(int) ) );

	int zoomPercent = ( ( zoom - _zoomMin ) * 100.0 / ( _zoomMax - _zoomMin ) );
	_zoomSlider->setValue( zoomPercent );

	connect( _zoomSlider, SIGNAL( valueChanged(int) ), this, SLOT( zoomSliderValueChanged(int) ) );
}
//-----------------------------------------------------------------------------


} // ::qan

