//-----------------------------------------------------------------------------
// This file is a part of the Cervlet software.
//
// \file	cvltCervletWindow.cpp
// \author	benoit@qanava.org
// \date	2014 11 19
//-----------------------------------------------------------------------------

// QT headers
#include <QHBoxLayout>
#include <QQuickWidget>
#include <QLabel>
#include <QGraphicsLinearLayout>
#include <QQuickItem>
#include <QMenuBar>
#include <QMenu>

// Cervlet headers
#include "./window.h"


//-----------------------------------------------------------------------------
Window::Window( QApplication* application, QWidget* parent ) :
	QMainWindow( parent ),
	_application( application )
{
    QWidget* frame = new QWidget( this );
    setCentralWidget( frame );

    QVBoxLayout* vbox = new QVBoxLayout( frame );
    vbox->setMargin( 0 );
    vbox->setSpacing( 0 );
    frame->setLayout( vbox );

    QLabel* label = new QLabel( );
    label->setText( "this is a test back widget label" );
    vbox->addWidget( label );

    // Create Qanava QML menu bar with the path to the QanMenuBar qml module stored in application ressource file,
    // and the qml file used to create the main QanMenuBar component
    _qmlMenuBar = new qan::QmlMenuBar( this, "qrc:/QanMenuBar/qan/MenuBar", QUrl( "qrc:/qmlmenubar.qml" ) );

    QAction* fileOpen = new QAction( tr( "&Open"), this );
    fileOpen->setShortcuts( QKeySequence::New );
    fileOpen->setStatusTip( tr("Open an existing file" ) );
    fileOpen->setProperty( "qan_menu_xml_icon", "qrc:/oxygen/document-open.svg" );
    connect( fileOpen, SIGNAL( triggered( ) ), this, SLOT( openFile( ) ) );

    QAction* fileSaveAs = new QAction( tr( "Save as" ), this );
    fileSaveAs->setShortcuts( QKeySequence::SaveAs );
    fileSaveAs->setStatusTip(tr("Save a file in a new repository"));
    fileSaveAs->setProperty( "qan_menu_xml_icon", "qrc:/oxygen/document-save-as.svgz" );
    connect( fileSaveAs, SIGNAL( triggered( ) ), this, SLOT( saveFileAs( ) ) );

    QAction* fileExit = new QAction( tr( "E&xit" ), this );
    fileExit->setShortcuts( QKeySequence::Quit );
    fileExit->setStatusTip(tr("Exit the application"));
    fileExit->setProperty( "qan_menu_xml_icon", "qrc:/oxygen/application-exit.svg" );
    connect( fileExit, SIGNAL( triggered( ) ), this, SLOT( close( ) ) );

    QMenu* fileMenu = menuBar( )->addMenu( "&File" );
    fileMenu->addAction( fileOpen );
    fileMenu->addAction( fileSaveAs );
    fileMenu->addSeparator( );
    fileMenu->addAction( fileExit );

    QAction* fileMenuShow = new QAction( tr( "File" ), this );
    fileMenuShow->setStatusTip( tr( "File menu" ) );
    fileMenuShow->setProperty( "qan_menu_xml_icon", "qrc:/oxygen/document-open-folder.svg" );

    QQuickItem* fileMenuItem = _qmlMenuBar->addMenuBarAction( fileMenuShow );
    _qmlMenuBar->addMenuBarAction( fileOpen, fileMenuItem );
    _qmlMenuBar->addMenuBarAction( fileSaveAs, fileMenuItem );
    _qmlMenuBar->addMenuBarSeparator( fileMenuItem );
    _qmlMenuBar->addMenuBarAction( fileExit, fileMenuItem );

    QAction* menu2Act1 = new QAction( tr( "My Act 1"), this );
    menu2Act1->setStatusTip( tr("Action 1 status" ) );
    menu2Act1->setProperty( "qan_menu_xml_icon", "qrc:/oxygen/view-restore.svg" );
    connect( menu2Act1, SIGNAL( triggered( ) ), this, SLOT( testAction1( ) ) );

    QAction* menu2Act2 = new QAction( tr( "My Act2" ), this );
    menu2Act2->setStatusTip( tr( "Action 2 status" ) );
    menu2Act2->setProperty( "qan_menu_xml_icon", "qrc:/oxygen/view-preview.svg" );

    QAction* menu2Act3 = new QAction( tr( "My Act 3" ), this );
    menu2Act3->setStatusTip( tr( "Action 3 status" ) );
    menu2Act3->setProperty( "qan_menu_xml_icon", "qrc:/oxygen/office-chart-ring.svg" );

    QMenu* menu2 = menuBar( )->addMenu( "Menu2" );
    menu2->addAction( menu2Act1 );
    menu2->addAction( menu2Act1 );
    menu2->addAction( menu2Act1 );

    QAction* menu2Show = new QAction( tr( "Menu 2" ), this );
    menu2Show->setStatusTip( tr( "Menu2" ) );
    menu2Show->setProperty( "qan_menu_xml_icon", "qrc:/oxygen/preferences-plugin.svg" );

    QQuickItem* menu2Item = _qmlMenuBar->addMenuBarAction( menu2Show );
    _qmlMenuBar->addMenuBarAction( menu2Act1, menu2Item );
    _qmlMenuBar->addMenuBarAction( menu2Act2, menu2Item );
    _qmlMenuBar->addMenuBarAction( menu2Act3, menu2Item );

    QAction* styleEdit = new QAction( tr( "Edit Styles" ), this );
    styleEdit->setStatusTip( tr( "Edit graph styles" ) );
    styleEdit->setCheckable( true );
    styleEdit->setProperty( "qan_menu_xml_icon", "qrc:/oxygen/media-playlist.svg" );

    QAction* styleShowBrowser = new QAction( tr( " Style \nBrowser" ), this );
    styleShowBrowser->setCheckable( true );
    styleShowBrowser->setStatusTip( tr( "Show style browser" ) );
    styleShowBrowser->setProperty( "qan_menu_xml_icon", "qrc:/oxygen/office-chart-ring.svg" );

    _qmlMenuBar->addMenuBarSeparator( );
    _qmlMenuBar->addMenuBarAction( styleEdit );
    _qmlMenuBar->addMenuBarAction( styleShowBrowser );
}

void Window::resizeEvent( QResizeEvent* e )
{
/*  _qmlMenuBar->setGeometry( 0, 0, 40, height( ) );
    //_qmlMenuBar->move( 0., 0. );
    //_qmlMenuBar->resize( 40, height( ) );
*/
    QMainWindow::resizeEvent( e );
}

void    Window::testAction1( )
{
    qDebug( "Window::testAction1( ) triggered from QML" );
}
//-----------------------------------------------------------------------------


