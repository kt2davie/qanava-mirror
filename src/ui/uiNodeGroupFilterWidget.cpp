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
// \file	uiNodeGroupFilterWidget.h
// \author	benoit@qanava.org
// \date	2014 June 21
//-----------------------------------------------------------------------------

// QT headers
#include <QGraphicsView>
#include <QPalette>
#include <QGraphicsLinearLayout>
#include <QLabel>
#include <QScrollBar>
#include <QMenu>
#include <QToolBar>
#include <QSizePolicy>

// Qanava headers
#include "../qanGraphScene.h"
#include "./uiNodeGroupFilterWidget.h"


namespace qan { // ::qan
namespace ui  { // ::qan::ui

NodeGroupFilterWidget::NodeGroupFilterWidget( QGraphicsView& view, qan::GraphScene& scene ) : 
	QGraphicsWidget( 0 ),
	_view( view ),
	_scene( scene )
{
	connect( _view.verticalScrollBar( ), SIGNAL( valueChanged( int ) ), this, SLOT( viewViewportChanged( int ) ) );
	connect( _view.horizontalScrollBar( ), SIGNAL( valueChanged( int ) ), this, SLOT( viewViewportChanged( int ) ) );
	connect( &_view, SIGNAL( viewResized( QSize ) ), this, SLOT( viewResized( QSize ) ) );

	connect( &scene, SIGNAL( nodeGroupAdded( qan::NodeGroup* ) ), this, SLOT( nodeGroupAdded( qan::NodeGroup* ) ) );
	connect( &scene, SIGNAL( nodeGroupRemoved( qan::NodeGroup* ) ), this, SLOT( nodeGroupRemoved( qan::NodeGroup* ) ) );

	QPalette p = palette( );
	p.setColor( QPalette::Window, Qt::transparent );
	setPalette( p );

	_vLayout = new QGraphicsLinearLayout( Qt::Vertical, this ); 
	_vLayout->setSpacing( 5. );
	_vLayout->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
	foreach ( qan::NodeGroup* group, scene.getNodeGroups( ) )
		nodeGroupAdded( group );
	
	setFlag( QGraphicsItem::ItemIgnoresTransformations, true );
	setVisible( true );
	setPos( 0., 0. );
	setZValue( 1100. );
}

NodeGroupFilterWidget::~NodeGroupFilterWidget( )
{
	// Remove widget items...
	QList< QGraphicsWidget* > groupWidgets = _groupWidgetMap.values();
	foreach ( QGraphicsWidget* groupWidget, groupWidgets )
		_scene.removeItem( groupWidget );
}

void	NodeGroupFilterWidget::filterGroupStateChanged( int state )
{
	QCheckBox* filterGroup = qobject_cast< QCheckBox* >( sender( ) );
	qan::NodeGroup* nodeGroup = _filterGroupMap.value( filterGroup );
	if ( nodeGroup != 0 )
		nodeGroup->setVisible( state == Qt::Checked );
}

void	NodeGroupFilterWidget::viewResized( QSize s )
{
    Q_UNUSED( s );
    setPos( _view.mapToScene( QPoint( 0, 0 ) ) );
}

void	NodeGroupFilterWidget::viewViewportChanged( int value )
{
    Q_UNUSED( value );
	setPos( _view.mapToScene( QPoint( 0, 0 ) ) );
}

void	NodeGroupFilterWidget::nodeGroupAdded( qan::NodeGroup* nodeGroup )
{
	if ( nodeGroup == 0 )
		return;
	if ( _groupLayoutMap.contains( nodeGroup ) )	// Do not add node groups multiple times
		return;

	connect( nodeGroup, SIGNAL( nameChanged( qan::NodeGroup*, QString ) ), this, SLOT( nodeGroupNameChanged( qan::NodeGroup*, QString ) ) );

	QGraphicsLinearLayout* hLayout = new QGraphicsLinearLayout( _vLayout ); 
	hLayout->setSpacing( 5. );
	hLayout->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );

	QCheckBox* filterGroup = new QCheckBox( );
	filterGroup->setChecked( true );
	_filterGroupMap.insert( filterGroup, nodeGroup );
	connect( filterGroup, SIGNAL( stateChanged( int ) ), this, SLOT( filterGroupStateChanged( int ) ) );

	QGraphicsWidget* filterGroupWidget = _scene.addWidget( filterGroup );
	hLayout->addItem( filterGroupWidget );

	QLabel* groupLabel = new QLabel( nodeGroup->getName( ) );
	_groupLabelMap.insert( nodeGroup, groupLabel );
	QGraphicsWidget* groupNameWidget = _scene.addWidget( groupLabel );
	hLayout->addItem( groupNameWidget );
	hLayout->addStretch( );
	_vLayout->addItem( hLayout );

	_groupWidgetMap.insert( nodeGroup, filterGroupWidget );
	_groupWidgetMap.insert( nodeGroup, groupNameWidget );
	_groupLayoutMap.insert( nodeGroup, hLayout );
}

void	NodeGroupFilterWidget::nodeGroupRemoved( qan::NodeGroup* nodeGroup )
{
	nodeGroup->disconnect( this );	// No longuer monitor nameChanged() signal...

	// Remove group associed items and layouts
	QList< QGraphicsWidget* > groupWidgets = _groupWidgetMap.values( nodeGroup );
	foreach ( QGraphicsWidget* groupWidget, groupWidgets )
		_scene.removeItem( groupWidget );
	QList< QGraphicsLinearLayout* > groupLayouts = _groupLayoutMap.values( nodeGroup );
	foreach ( QGraphicsLinearLayout* groupLayout, groupLayouts )
		_vLayout->removeItem( groupLayout );
	
	_groupWidgetMap.remove( nodeGroup );
	_groupLabelMap.remove( nodeGroup );
	_groupLayoutMap.remove( nodeGroup );
}

void	NodeGroupFilterWidget::nodeGroupNameChanged( qan::NodeGroup* nodeGroup, QString name )
{
	QGraphicsWidget* groupNameWidget = _groupWidgetMap.value( nodeGroup, 0 );
	if ( groupNameWidget != 0 )
	{
		QLabel* groupNameLabel = _groupLabelMap.value( nodeGroup, 0 );
		if ( groupNameLabel != 0 )
		{
			groupNameLabel->setText( name );
			groupNameLabel->update( );
			groupNameLabel->adjustSize( );	// Force graphics linear layout h size update (QTBUG?)
		}
	}
}

} // ::qan::ui
} // ::qan
