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
// \file	qanQmlMenuBar.h
// \author	benoit@qanava.org
// \date	2015 January 06
//-----------------------------------------------------------------------------

#ifndef qanQmlMenuBar_h
#define qanQmlMenuBar_h

// QT headers
#include <QAction>
#include <QQuickWidget>
#include <QQuickItem>
#include <QMouseEvent>
#include <QEvent>
#include <QPaintEvent>

// Qanava headers
// Nil

namespace qan { // ::qan
    //! .
    /*!
		Qanava QmlMenuBar use icons from the Oxygen icon set, originals SVG files could be found on: http://www.oxygen-icons.org/

        \nosubgrouping
    */
    class QmlMenuBar : public QQuickWidget
    {
        Q_OBJECT

        /*! \name QmlMenuBar Object Management *///----------------------------
        //@{
    public:
        QmlMenuBar( QWidget* parent, QString importPath, QUrl qmlFile );
        virtual ~QmlMenuBar( ) { }

    private:
        Q_DISABLE_COPY( QmlMenuBar )
        //@}
        //---------------------------------------------------------------------


        /*! \name Menu Item Management *///------------------------------------
        //@{
    public:
        //! Add a new QML menu element with a given action settings in a selected menu.
        QQuickItem*     addMenuBarAction( QAction* action, QQuickItem* parentMenuElement = 0 );

        //! Add a separator menu element in a selected menu.
        void            addMenuBarSeparator( QQuickItem* parentMenuElement = 0 );

    protected:

        //! Used internally from QML to update this widget mask when a QML menu is shown or hidden.
        Q_INVOKABLE void    resetMask( qreal x, qreal y, qreal w, qreal h );

        //! Used internally from QML to update this widget mask when a QML menu is shown or hidden.
        Q_INVOKABLE void    appendBrToMask( qreal x, qreal y, qreal w, qreal h );

        //! Used internally from QML to update this widget mask when a QML menu is shown or hidden.
        Q_INVOKABLE void    removeBrFromMask( qreal x, qreal y, qreal w, qreal h );

        virtual     void    paintEvent( QPaintEvent* e );

    private:
        QPolygonF           _maskPolygon;

        QRectF              _lastBr;
        //@}
        //---------------------------------------------------------------------
    };
} // ::qan

#endif // qanQmlMenuBar_h
