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
// \file	qanMainWindow.cpp
// \author	benoit@qanava.org
// \date	2005 November 11
//-----------------------------------------------------------------------------


// Qanava headers
#include "../../src/qanGraphView.h"
#include "../../src/qanGrid.h"
#include "../../src/qanLayout.h"
#include "../../src/qanTreeLayout.h"
#include "../../src/qanGraphicsContainer.h"
#include "../../src/qanGraphicsResizer.h"
#include "./qanMainWindow.h"
#include "./ui/uiStyleEditorWidget.h"


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
PixmapWidget::PixmapWidget( QPixmap pixmap ) :
	QGraphicsView( ),
	_pixmapItem( 0 ),
	_pixmapScene( 0 ) 
{
	_pixmapScene = new QGraphicsScene( );
	setInteractive( true );	// Enable mouse input
	setTransformationAnchor( QGraphicsView::AnchorUnderMouse );
	setResizeAnchor( QGraphicsView::AnchorUnderMouse );
	setDragMode( QGraphicsView::ScrollHandDrag );

	setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
	setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );

	setScene( _pixmapScene );
	_pixmapItem = _pixmapScene->addPixmap( pixmap );
	_pixmapItem->setFlag( QGraphicsItem::ItemIsMovable, false );
}

void	PixmapWidget::wheelEvent( QWheelEvent* e )
{
	QPointF p = mapToScene( e->pos( ) );
	double d = ( e->delta( ) > 0 ? 1.1 : 0.9 );
	scale( d, d );
	centerOn( p );
	e->accept( );
}

bool	PixmapWidget::viewportEvent( QEvent* e )
{
	bool r = QGraphicsView::viewportEvent( e );	// ensure the event is handled before updating...

	if ( e->type( ) == QEvent::HoverEnter ||		// Detect user inputs, and force the tag synchro agent update
		 e->type( ) == QEvent::HoverLeave ||
		 e->type( ) == QEvent::HoverMove ||
		 e->type( ) == QEvent::MouseMove ||
		 e->type( ) == QEvent::MouseButtonPress ||
		 e->type( ) == QEvent::MouseButtonRelease ||
		 e->type( ) == QEvent::Wheel ||
		 e->type( ) == QEvent::Resize )
	{
			//_tagSyncAgent.update( );
	}
	return r;
}




ContainerWidget::ContainerWidget( QGraphicsScene& scene, QGraphicsView* gw, QGraphicsItem* parent ) : 
	QGraphicsWidget( parent ),
	_scene( scene )
{ 
    Q_UNUSED( gw );
    _vLayout = new QGraphicsLinearLayout( Qt::Vertical );
	_vLayout->setContentsMargins( 0.2, 0.2, 0.2, 0.2 );
    setLayout( _vLayout );

	QString contentText3 = "<html><p><b>HTML TITLE</b></p><p>sddas dsq dsq dsqfzea fa ffezzefh az rtg re he hyte jyt etg zefeaze dazejk hgkjhag ztge hy th eyt h tg aer fazfaerfze gt htyj ur g vafae</p>	\
							<p>sddas dsq dsddzeq dfzeasfq fa ffezzef az rtg re he hyte jsdfghyyt etg zefeafezfazeze daze ag ghfjhkfztge hy th eyt h tg aer fazfaerfze gt htyj ur g vafae</p>				\
							<p>szeafdfdazeas dsq dfezasddzeq dfzeasfq fa ffezzef az rtg re hesfdg hyte jyt etg zefeafezfazeze daze ag ztge hy th eyt h tg aer fazfaerfze gt htyj ur g vafae</p>		\
							<p>szeadsff dfdasdzf eas dhgfsq ghdf ezhgashddzeerq derzfzezeadszfq fdza ffezzef az rtg re hesfdg hydsfte jfdsyt etg zefefgdaf ezfazeze daze kjhgag ztge hjkhy th eyt h tg aer fazfaerfze gt htyj ur g vafae</p>		\
							</html>";
	QTextDocument* qDocument3 = new QTextDocument( );
	qDocument3->setHtml( contentText3 );

	QTextEdit* textEdit = new QTextEdit( );
	textEdit->setDocument( qDocument3 );
	QGraphicsWidget* proxy = scene.addWidget( textEdit );
	_vLayout->addItem( proxy ); 
}

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

	// FIXME: v0.9.1 hack to hide the zoom ctrl slidebar
	QWidget* zoomSlider = graphView->getControllerWidget( "zoom_ctrl_zoom_slider" );
	if ( zoomSlider != 0 )
		zoomSlider->hide( );

    { // Graphics container test
        // Add a graphics container item with an HTML graphics widget
        qan::GraphicsContainer* container1 = new qan::GraphicsContainer( QRectF(0., 0., 300., 200. ) );
        _graph->getM( ).addItem( container1 );
        ContainerWidget* gw = new ContainerWidget( _graph->getM( ), graphView, container1 );
        container1->setContent( gw, gw );
        container1->setPos( 100, 150 );
        container1->setTitle( "HTML container item test" );

        qan::GraphicsContainer* container3 = new qan::GraphicsContainer( QRectF(0., 0., 300., 200. ), qan::GraphicsDecoration::Round );
        _graph->getM( ).addItem( container3 );
        ContainerWidget* gw2 = new ContainerWidget( _graph->getM( ), graphView, container1 );
        container3->setContent( gw2, gw2 );
        container3->setPos( 100, 350 );
        container3->setTitle( "HTML in a round graphics container" );

        qan::GraphicsContainer* container2 = new qan::GraphicsContainer( QRectF(0., 0., 300., 200. ) );
        _graph->getM( ).addItem( container2 );
        QGraphicsWidget* proxy = _graph->getM( ).addWidget( new PixmapWidget( QPixmap( ":/test.png" ) ) );
        container2->setContent( proxy, proxy );
        container2->setPos( 450, 150 );
        container2->setTitle( "Graphics view inside a container inside a graphics view!" );
    }

    { // Graphics controller test
        QLabel* label = new QLabel( "this is a standard QLabel" );
        label->setGeometry( QRect( 0, 0, 130, 100 ) );
        QGraphicsWidget* labelProxy = _graph->getM( ).addWidget( label );
        labelProxy->setPos( 15., 15. );

        // FIXME: I know this is ugly, but I should maintain VC++2010 support without strongly typed enums...
        qan::GraphicsController* controller = new qan::GraphicsController( labelProxy, labelProxy, QSizeF( 20, 20 ), GraphicsController::ShowCloseMaximize );
        //qan::GraphicsController* controller = new qan::GraphicsController( labelProxy, labelProxy, QSizeF( 20, 20 ), GraphicsController::Config( GraphicsController::ShowCloseMaximize | GraphicsController::ShowFocusShadow ) );
        qan::GraphicsResizer* resizer = new qan::GraphicsResizer( controller, labelProxy, controller ); Q_UNUSED( resizer );
    }

	_graph->getM( ).setSceneRect( QRectF( 0.0, 0., 1000., 1000. ) );
}
//-----------------------------------------------------------------------------


