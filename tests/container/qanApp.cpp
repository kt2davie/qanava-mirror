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
// This file is a part of the Qarte software.
//
// \file	canApp.cpp
// \author	benoit@qanava.org
// \date	2005 April 19
//-----------------------------------------------------------------------------


// Qanava headers
#include "qanMainWindow.h"

// QT headers
#include <qapplication.h>
#include <qsplashscreen.h>


int	main( int argc, char** argv )
{
	// QT gui creation (Needed to allocate plugin qt components and application kernel)
	QApplication app( argc, argv );

	// Create main application form
	MainWindow mainWindow( &app );

	// Display main window and start processing events
	mainWindow.show( );
	app.connect( &app, SIGNAL( lastWindowClosed( ) ), &app, SLOT( quit( ) ) );

	// Process events
	return app.exec( );
}

