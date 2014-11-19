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
// \file	qanMainWindow.cpp
// \author	benoit@qanava.org
// \date	2014 10 19
//-----------------------------------------------------------------------------


// Qanava headers
#include "../../src/qanGraphView.h"
#include "../../src/qanGrid.h"
#include "../../src/qanLayout.h"
#include "../../src/qanTreeLayout.h"
#include "../../src/qanContainerGraphicsItem.h"
#include "./qanMainWindow.h"
#include "./ui/uiStyleEditorWidget.h"
#include "./ui/uiStyleBrowserWidget.h"

// QT headers
#include <QToolBar>
#include <QAction>
#include <QSplitter>
#include <QMessageBox>
#include <QTextEdit>
#include <QPushButton>
#include <QVariant>
#include <QCheckBox>
#include <QProgressBar>

using namespace qan;


//-----------------------------------------------------------------------------
MainWindow::MainWindow( QApplication* application, QWidget* parent ) : 
	QMainWindow( parent ),
	_application( application )
{
	setupUi( this );
    QHBoxLayout* hbox = new QHBoxLayout( _frame );
	hbox->setMargin( 0 );

	// Generate a simple graph
	_graph = new Graph( );

	qan::GraphView* graphView = new qan::GraphView( _frame, Qt::white );
	graphView->setGraph( *_graph );
	hbox->addWidget( graphView );
	new qan::GridCheckBoardItem( graphView, palette( ).color( QPalette::Base ), palette( ).color( QPalette::AlternateBase ) );

	// Configure style editor
	StyleEditorWidget* styleEditor = new qan::StyleEditorWidget( _frame, *_graph );
	styleEditor->setGeometry( styleEditor->geometry( ).x( ), styleEditor->geometry( ).y( ), 125, styleEditor->geometry( ).height( ) );
	qan::Style* nodeStyle = _graph->getStyleManager( ).getTargetStyle( "qan::Node" );	// Get the default node style
	if ( nodeStyle != 0 )
		styleEditor->setStyle( nodeStyle );

	// FIXME: v0.9.1 hack to hide the zoom ctrl slidebar
	QWidget* zoomSlider = graphView->getControllerWidget( "zoom_ctrl_zoom_slider" );
	if ( zoomSlider != 0 )
		zoomSlider->hide( );

	qan::Style* qStyle = _graph->getM( ).getStyleManager( ).addStyle( "qStyle", "qan::Node" );
	qStyle->addProperty( "No Background", QVariant( false ) );
	qStyle->addProperty( "Back Color", QVariant( QColor( 60, 100, 250 ) ) );
	qStyle->addProperty( "Border Color", QVariant( QColor( 0, 0, 0 ) ) );
	QStringList lineStyles; lineStyles << "Solid line" << "Dash line" << "Dot line" << "Dash Dot line" << "Dash Dot Dot line";
	qStyle->addProperty( "Border Style", QtVariantPropertyManager::enumTypeId(),  lineStyles );
	qStyle->addProperty( "Border Width", QVariant( 1.0 ) );
	qStyle->addProperty( "Font", QVariant( QFont( "Helvetica", 26, QFont::Bold ) ) );
	qStyle->addProperty( "Maximum Size", QVariant( QSizeF( 100., 25. ) ) );
	qStyle->addProperty( "Has Shadow", QVariant( true ) );
	qStyle->addProperty( "Shadow Color", QVariant( QColor( 50, 50, 50 ) ) );
	qStyle->addProperty( "Shadow Offset", QVariant( QSizeF( 5., 5. ) ) );

	qan::Style* aStyle = _graph->getM( ).getStyleManager( ).addStyle( "aStyle", "qan::Node" );
	aStyle->addProperty( "No Background", QVariant( false ) );
	aStyle->addProperty( "Back Color", QVariant( QColor( 105, 130, 220 ) ) );
	aStyle->addProperty( "Border Color", QVariant( QColor( 60, 100, 250 ) ) );
	aStyle->addProperty( "Border Style", 1 );
	aStyle->addProperty( "Border Width", QVariant( 1.0 ) );
	aStyle->addProperty( "Font", QVariant( QFont( "Helvetica", 18, QFont::Bold ) ) );
	aStyle->addProperty( "Maximum Size", QVariant( QSizeF( 100., 25. ) ) );
	aStyle->addProperty( "Has Shadow", QVariant( true ) );
	aStyle->addProperty( "Shadow Color", QVariant( QColor( 50, 50, 50 ) ) );
	aStyle->addProperty( "Shadow Offset", QVariant( QSizeF( 3., 3. ) ) );

	qan::Style* nStyle = _graph->getM( ).getStyleManager( ).addStyle( "nStyle", "qan::Node" );
	nStyle->addProperty( "No Background", QVariant( false ) );
	nStyle->addProperty( "Back Color", QVariant( QColor( 250, 40, 40 ) ) );
	nStyle->addProperty( "Border Color", QVariant( QColor( 0, 0, 0 ) ) );
	nStyle->addProperty( "Border Style", QtVariantPropertyManager::enumTypeId(),  lineStyles );
	nStyle->addProperty( "Border Width", QVariant( 1.0 ) );
	nStyle->addProperty( "Font", QVariant( QFont( "Helvetica", 24, QFont::Bold ) ) );
	nStyle->addProperty( "Maximum Size", QVariant( QSizeF( 100., 25. ) ) );
	nStyle->addProperty( "Has Shadow", QVariant( true ) );
	nStyle->addProperty( "Shadow Color", QVariant( QColor( 50, 50, 50 ) ) );
	nStyle->addProperty( "Shadow Offset", QVariant( QSizeF( 4., 4. ) ) );

	qan::Style* vStyle = _graph->getM( ).getStyleManager( ).addStyle( "vStyle", "qan::Node" );
	vStyle->addProperty( "No Background", QVariant( false ) );
	vStyle->addProperty( "Back Color", QVariant( QColor( 30, 230, 50 ) ) );
	vStyle->addProperty( "Border Color", QVariant( QColor( 0, 0, 0 ) ) );
	vStyle->addProperty( "Border Style", QtVariantPropertyManager::enumTypeId(),  lineStyles );
	vStyle->addProperty( "Border Width", QVariant( 1.0 ) );
	vStyle->addProperty( "Font", QVariant( QFont( "Helvetica", 18 ) ) );
	vStyle->addProperty( "Maximum Size", QVariant( QSizeF( 100., 25. ) ) );
	vStyle->addProperty( "Has Shadow", QVariant( true ) );
	vStyle->addProperty( "Shadow Color", QVariant( QColor( 50, 50, 50 ) ) );
	vStyle->addProperty( "Shadow Offset", QVariant( QSizeF( 4., 4. ) ) );

	qan::Style* a3Style = _graph->getM( ).getStyleManager( ).addStyle( "a3Style", "qan::Node" );
	a3Style->addProperty( "No Background", QVariant( false ) );
	a3Style->addProperty( "Back Color", QVariant( QColor( 180, 105, 220 ) ) );
	a3Style->addProperty( "Border Color", QVariant( QColor( 60, 100, 250 ) ) );
	a3Style->addProperty( "Border Style", 3 );
	a3Style->addProperty( "Border Width", QVariant( 1.0 ) );
	a3Style->addProperty( "Font", QVariant( QFont( "Helvetica", 14, QFont::Bold ) ) );
	a3Style->addProperty( "Maximum Size", QVariant( QSizeF( 100., 25. ) ) );
	a3Style->addProperty( "Has Shadow", QVariant( true ) );
	a3Style->addProperty( "Shadow Color", QVariant( QColor( 50, 50, 50 ) ) );
	a3Style->addProperty( "Shadow Offset", QVariant( QSizeF( 4., 4. ) ) );

	Node* q = new qan::Node( "Q" );
	_graph->insertNode( q, "qStyle");

	Node* a = new qan::Node( "a" );
	_graph->insertNode( a, "aStyle");

	Node* n = new qan::Node( "n" );
	_graph->insertNode( n, "nStyle");

	Node* a2 = new qan::Node( "a" );
	_graph->insertNode( a2, "aStyle");

	Node* v = new qan::Node( "v" );
	_graph->insertNode( v, "vStyle");

	Node* a3 = new qan::Node( "a" );
	_graph->insertNode( a3, "a3Style");

	Edge* qa = _graph->insertEdge( *q, *a );
	Edge* an = _graph->insertEdge( *a, *n );
	Edge* na2 = _graph->insertEdge( *n, *a2 );
	Edge* a2v = _graph->insertEdge( *a2, *v );
	Edge* va3 = _graph->insertEdge( *v, *a3 );

	_graph->getM( ).setSceneRect( QRectF( 0.0, 0., 1000., 1000. ) );

	// Add a style browser widget to dynamically set node's style
	ui::StyleBrowserWidget* styleBrowserWidget = new qan::ui::StyleBrowserWidget( *graphView, _graph->getM( ), _graph->getStyleManager( ) );
	styleBrowserWidget->setParent( graphView );
	_graph->getM( ).addItem( styleBrowserWidget );

	QSplitter* qSplitter = new QSplitter( _frame );
	qSplitter->addWidget( graphView );
	qSplitter->addWidget( styleEditor );
	qSplitter->setStretchFactor( 0, 3 );
	qSplitter->setStretchFactor( 1, 1 );
	hbox->addWidget( qSplitter );
}
//-----------------------------------------------------------------------------


