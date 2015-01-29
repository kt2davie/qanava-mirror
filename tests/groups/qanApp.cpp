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
// \file	qanApp.cpp
// \author	benoit@qanava.org
// \date	2014 10 19
//-----------------------------------------------------------------------------


// Qanava headers
#include "qanMainWindow.h"

// QT headers
#include <qapplication.h>
#include <qsplashscreen.h>


int	main( int argc, char** argv )
{
	QApplication app( argc, argv );

	MainWindow mainWindow( &app );
	mainWindow.show( );
	app.connect( &app, SIGNAL( lastWindowClosed( ) ), &app, SLOT( quit( ) ) );

    return app.exec( );
}

