//-----------------------------------------------------------------------------
// This file is a part of the Cervlet software.
//
// \file	cvltCervletWindow.h
// \author	benoit@qanava.org
// \date	2014 11 19
//-----------------------------------------------------------------------------


#ifndef window_h
#define window_h


// QT headers
#include <QMainWindow>
#include <QMenu>
#include <QQuickWidget>

// Qanava headers
#include "./QanMenuBar/qanQmlMenuBar.h"

//-----------------------------------------------------------------------------
//! 
/*!
	\nosubgrouping
*/
    class Window : public QMainWindow
	{
		Q_OBJECT

	public:
        Window( QApplication* application, QWidget* parent = 0 );

        virtual ~Window( ) { }

    protected:
        void resizeEvent(QResizeEvent*);

        qan::QmlMenuBar*       _qmlMenuBar;

	private:
        Q_DISABLE_COPY( Window );

		QApplication*		_application;

    protected slots:

        virtual void    testAction1( );
};
//-----------------------------------------------------------------------------


#endif // window_h


