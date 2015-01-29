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
// \file	uiNodeGroupFilterWidget.h
// \author	benoit@qanava.org
// \date	2014 June 21
//-----------------------------------------------------------------------------

#ifndef uiNodeGroupFilterWidget_h
#define uiNodeGroupFilterWidget_h

// QT headers
#include <QGraphicsView>
#include <QGraphicsWidget>
#include <QGraphicsLinearLayout>
#include <QCheckBox>
#include <QLabel>

// Qanava headers
#include "../qanNodeGroup.h"


namespace qan { // ::qan
namespace ui {  // ::qan::ui

/*! Show a transparent list of group with an associed checkable control to show or hide group items
*/
class NodeGroupFilterWidget : public QGraphicsWidget
{
	Q_OBJECT

public:

	NodeGroupFilterWidget( QGraphicsView& view, qan::GraphScene& scene );

	virtual ~NodeGroupFilterWidget( );

protected:

	QGraphicsView&											_view;

	qan::GraphScene&										_scene;

	QMap< QCheckBox*, qan::NodeGroup* >						_filterGroupMap;
	
	QMultiMap< qan::NodeGroup*, QGraphicsWidget* >			_groupWidgetMap;

	QMap< qan::NodeGroup*, QLabel* >						_groupLabelMap;

	QMultiMap< qan::NodeGroup*, QGraphicsLinearLayout* >	_groupLayoutMap;

	QGraphicsLinearLayout*									_vLayout;

protected slots:

	void	filterGroupStateChanged( int state );

	void	viewViewportChanged( int value );
	
	void	viewResized( QSize s );

	void	nodeGroupAdded( qan::NodeGroup* nodeGroup );

	void	nodeGroupRemoved( qan::NodeGroup* nodeGroup );

	void	nodeGroupNameChanged( qan::NodeGroup* nodeGroup, QString name );
};
//-----------------------------------------------------------------------------


} // ::qan::ui
} // ::qan

#endif // uiNodeGroupFilterWidget_h
