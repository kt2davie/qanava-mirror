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
// \file	qanQmlMenuBar.cpp
// \author	benoit@qanava.org
// \date	2015 January 06
//-----------------------------------------------------------------------------

// QT headers
#include <QQmlEngine>

// Qanava headers
#include "./qanQmlMenuBar.h"

namespace qan {	// ::qan

/* QmlMenuBar Object Management *///-------------------------------------------
QmlMenuBar::QmlMenuBar( QWidget* parent, QString importPath, QUrl qmlFile ) :
    QQuickWidget( parent )
{
    engine( )->addImportPath( importPath );
    engine( )->addImportPath( "qrc:/" );
    setResizeMode( QQuickWidget::SizeViewToRootObject );

    setSource( qmlFile );
    setAttribute( Qt::WA_TranslucentBackground, true );
    setAttribute( Qt::WA_AlwaysStackOnTop, true );
    setVisible( true );
    setClearColor( Qt::transparent );
}
//-----------------------------------------------------------------------------


/* Menu Item Management *///---------------------------------------------------
/*!
 * \param action action that must be added to this QML menu bar, ownership of action is not transfered to menu bar.
 */
QQuickItem*    QmlMenuBar::addMenuBarAction( QAction* action, QQuickItem* parentMenuElement )
{
    QQuickItem* qmlMenuBar = rootObject( );
    QVariant valueReturn;
    QVariant valueArgParentMenuElement = QVariant::fromValue( parentMenuElement );
    QVariant valueArgLabel = QString( action->iconText( ) );
    QVariant valueArgSource = action->property( "qan_menu_xml_icon" );
    QVariant valueArgIsChecked = QVariant::fromValue( action->isCheckable() && action->isChecked( ) );
    QVariant valueArgAction = QVariant::fromValue( action );
    QMetaObject::invokeMethod( ( QObject* )qmlMenuBar,
                               "addMenuElement",
                               Q_RETURN_ARG( QVariant, valueReturn ),
                               Q_ARG( QVariant, valueArgParentMenuElement ),
                               Q_ARG( QVariant, valueArgLabel ),
                               Q_ARG( QVariant, valueArgSource ),
                               Q_ARG( QVariant, valueArgIsChecked ),
                               Q_ARG( QVariant, valueArgAction )
                              );

    // Connect the qml menu element activated signal to qaction activate slot
    QQuickItem* qmlMenuElement = qobject_cast< QQuickItem* >( valueReturn.value< QObject* >( ) );
    if ( qmlMenuElement != 0 )
    {
        connect( ( const QObject* )qmlMenuElement, SIGNAL( activated( ) ), action, SLOT( trigger( ) ) );

        if ( action->isCheckable( ) )
            qmlMenuElement->setProperty( "checkable", QVariant( true ) );

        // Quite complicated lambda expression to convert a bool to QVariant...
        connect( action, &QAction::toggled, [=] {
            QVariant valueArgChecked = QVariant::fromValue( action->isChecked( ) );
            QMetaObject::invokeMethod( ( QObject* )qmlMenuElement,
                                       "setChecked",
                                       Q_ARG( QVariant, valueArgChecked ) );
            } );
    }
    return qmlMenuElement;
}

void    QmlMenuBar::addMenuBarSeparator( QQuickItem* parentMenuElement )
{
    QQuickItem* qmlMenuBar = rootObject( );
    QVariant valueArgParentMenuElement = QVariant::fromValue( parentMenuElement );
    QMetaObject::invokeMethod( ( QObject* )qmlMenuBar, "addSeparator",
                               Q_ARG( QVariant, valueArgParentMenuElement ) );
}
//-----------------------------------------------------------------------------

} // ::qan

