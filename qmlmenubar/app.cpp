
//-----------------------------------------------------------------------------
// This file is a part of the Cervlet software.
//
// \file	cvltApp.cpp
// \author	benoit@qanava.org
// \date	2014 11 19
//-----------------------------------------------------------------------------

// QT headers
#include <QApplication>

#include "window.h"

int	main( int argc, char** argv )
{
	QApplication app( argc, argv );

    Window w( &app );
    w.resize(1024, 768);
	w.show( );
	//app.connect( &app, SIGNAL( lastWindowClosed( ) ), &app, SLOT( quit( ) ) );

	return app.exec( );
}

