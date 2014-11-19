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
// \file	qanGraphItem.cpp
// \author	benoit@qanava.org
// \date	2012 01 07
//-----------------------------------------------------------------------------


// QT headers
#include <QGraphicsLinearLayout>
#include <QTimer>

// Qanava headers
#include "./qanGraphItem.h"
#include "./qanGraphScene.h"


namespace qan { // ::qan

/* GraphItem Properties Management *///----------------------------------------
PropertiesWidget::PropertiesWidget( GraphScene& scene, qan::Properties& properties, QGraphicsItem * parent ) :
	QGraphicsWidget( parent ),
    _propertiesEditorWidget( 0 ),
	_properties( properties ),
	_hasHover( false )
{
    // Initialize edge properties editor
	_propertyEditor = new QtTreePropertyBrowser( );
	_propertiesEditorWidget = scene.addWidget( _propertyEditor );
	_propertiesEditorWidget->setParentItem( this );
	_variantFactory = new QtVariantEditorFactory( this );

	QGraphicsLinearLayout* layout = new QGraphicsLinearLayout( );
	layout->setContentsMargins( 0., 0., 0., 0. );
	layout->addItem( _propertiesEditorWidget );
	setLayout( layout );

	setAcceptHoverEvents( true );
	setZValue( 1000. );
	setFlag( QGraphicsItem::ItemIgnoresTransformations, true );
}
		
void	PropertiesWidget::updateProperties( )
{
    QSizeF propertiesEditorSize( _propertiesEditorWidget->preferredSize( ) );
    propertiesEditorSize.setHeight( qMax( 50., 30. + _properties.getProperties( ).size( ) * 25. ) );
	_propertiesEditorWidget->setGeometry( QRectF( _propertiesEditorWidget->pos( ), propertiesEditorSize ) ); 
	if ( _propertiesEditorWidget != 0 )
	{
		_propertyEditor->clear( );
		_propertyEditor->setFactoryForManager( ( QtVariantPropertyManager* )&_properties, _variantFactory );
		QList< QtVariantProperty* >& props = _properties.getProperties( );
		foreach( QtVariantProperty* p, props )
            _propertyEditor->addProperty( p );
    }
}

void	PropertiesWidget::hoverEnterEvent( QGraphicsSceneHoverEvent* e )
{
	QGraphicsWidget::hoverEnterEvent( e );
	_hasHover = true;
	e->accept( );
}

void	PropertiesWidget::hoverMoveEvent( QGraphicsSceneHoverEvent* e )
{
	QGraphicsWidget::hoverMoveEvent( e );
	_hasHover = true;
	e->ignore( );
}

void	PropertiesWidget::hoverLeaveEvent( QGraphicsSceneHoverEvent* e )
{
	QGraphicsWidget::hoverLeaveEvent( e );
	_hasHover = false;
	setVisible( false );
	e->ignore( );
}
//-----------------------------------------------------------------------------


/* GraphItem Constructor / Destructor *///-------------------------------------
GraphItem::GraphItem( GraphScene& scene, QGraphicsItem* parent ) :
	QGraphicsObject( parent ),
	_scene( scene ), 
	_styleManager( scene.getStyleManager( ) ),
	_propertiesWidget( 0 ),
	_hovering( false ),
	_hoveringPos( 0., 0. ),
	_showBottom( false )
{ 
	setFlag( QGraphicsItem::ItemIsMovable, false );
	setFlag( QGraphicsItem::ItemIsSelectable, false );
	setFlag( QGraphicsItem::ItemClipsToShape, false );		// Considerable drawing speed up

	setAcceptHoverEvents( false );
}

GraphItem::~GraphItem( )
{
	if ( getGraphicsItem( ) != 0 )
		delete getGraphicsItem( );
}
		
QPointF	GraphItem::getAnchor( qan::Edge& edge )
{
    Q_UNUSED( edge );
	return ( getGraphicsItem( ) != 0 ? getGraphicsItem( )->boundingRect( ).center( ) : QPointF( 0., 0. ) );
}
//-----------------------------------------------------------------------------


/* EdgeItem Properties Widget Management *///----------------------------------
void	GraphItem::activatePropertiesPopup( qan::Properties& properties, int popupDelay, bool showBottom )
{
	if ( _propertiesWidget == 0 )
	{
		setAcceptHoverEvents( true );

		// Initialize edge properties editor
		_propertiesWidget = new PropertiesWidget( _scene, properties );
		_scene.addItem( _propertiesWidget );
		_propertiesWidget->setVisible( false );

		_showBottom = showBottom;
		_popupDelay = popupDelay;
	}
}

void	GraphItem::hidePropertiesPopup( )
{
	if ( _propertiesWidget != 0 )
	{
		_hovering = false;
		_propertiesWidget->setVisible( false );
	}
}

void	GraphItem::hoverMoveEvent( QGraphicsSceneHoverEvent* e )
{
	QGraphicsItem::hoverMoveEvent ( e );

	bool showPopup = shape( ).contains( e->pos( ) );
	if ( showPopup )	// If the distance is small, the cursor is almost on the edge, show the popup
		getScene( ).emitShowItemPopup( this, e->scenePos( ) );		// show the label
	else 
		getScene( ).emitHideItemPopup( this );

	// Showing the properties widget after a few milliseconds if the user is still 'hovering'
	_hoveringPos = e->scenePos( );
	if ( showPopup )
	{
		_hovering = true;
		if ( _propertiesWidget->isVisible( ) && _propertiesWidget->hasHover( ) )
			showPropertiesWidget( ); //_propertiesWidget->setPos( _hoveringPos );
		else
			QTimer::singleShot( _popupDelay, this, SLOT( showPropertiesWidget( ) ) ); 
	}
	else
	{		
		if ( _propertiesWidget->isVisible( ) && !_propertiesWidget->hasHover( ) )
			hidePropertiesPopup( );
		_hovering = false;
	}
	e->ignore( );
}

void	GraphItem::hoverEnterEvent( QGraphicsSceneHoverEvent* e )
{
	bool showPopup = shape( ).contains( e->pos( ) );

	// Showing the properties widget after a few milliseconds if the user is still 'hovering'
	_hoveringPos = e->scenePos( );
	if ( showPopup )
	{
		_hovering = true;
		QTimer::singleShot( _popupDelay, this, SLOT( showPropertiesWidget( ) ) ); 
	}

	QGraphicsItem::hoverEnterEvent( e );
}

void	GraphItem::hoverLeaveEvent( QGraphicsSceneHoverEvent* e )
{
	getScene( ).emitHideItemPopup( this );

	// Hide the property editor widget (it will stay visible if mouse is currently hovering over it, see sceneEventFilter)
	_hovering = false;
	_hoveringPos = QPointF( 0., 0. );
	if ( _propertiesWidget->isVisible( ) )
		QTimer::singleShot( 100, this, SLOT( delayedHoverLeaveEvent( ) ) );	// hoverLeave is called before properties widget hoverEnter automatically, delay it...

	QGraphicsItem::hoverLeaveEvent( e );
}

void	GraphItem::showPropertiesWidget( )
{
	if ( _hovering && !_propertiesWidget->isVisible( ) )	// Do not show the widget if hovering is over
	{
		// Show the property editor with actual edge properties
		_propertiesWidget->setVisible( true );
		if ( _showBottom )
			_propertiesWidget->setPos( mapToScene( boundingRect( ).bottomLeft( ) ) );
		else
			_propertiesWidget->setPos( _hoveringPos );
		_propertiesWidget->updateProperties( );
		_propertiesWidget->setFocus( Qt::MouseFocusReason );
	} else if ( _hovering && _propertiesWidget->isVisible( ) )
	{
		if ( _showBottom )
			_propertiesWidget->setPos( mapToScene( boundingRect( ).bottomLeft( ) ) );
		else
			_propertiesWidget->setPos( _hoveringPos );
	}
}

void	GraphItem::delayedHoverLeaveEvent( )
{
	if ( !_propertiesWidget->hasHover( ) )
		_propertiesWidget->setVisible( false );
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
GraphItem*	GraphItem::Factory::create( qan::GraphScene& scene, qan::Node& node, QGraphicsItem* parent )
{
    Q_UNUSED( scene ); Q_UNUSED( node ); Q_UNUSED( parent ); return 0;
}

GraphItem*	GraphItem::Factory::create( qan::GraphScene& scene, qan::Edge& edge, QGraphicsItem* parent )
{
    Q_UNUSED( scene ); Q_UNUSED( edge ); Q_UNUSED( parent ); return 0;
}
//-----------------------------------------------------------------------------


} // ::qan

