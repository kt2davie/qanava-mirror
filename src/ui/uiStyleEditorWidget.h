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
// \file	uiStyleEditorWidget.h
// \author	benoit@qanava.org
// \date	2014/03/01
//-----------------------------------------------------------------------------


#ifndef uiStyleEditorWidget_h
#define uiStyleEditorWidget_h


// QT headers
#include <QGraphicsProxyWidget>
#include <QTextEdit>
#include <QGraphicsLinearLayout>
#include <QLabel>
#include <QDialog>
#include <QListWidget>
#include <QComboBox>
#include <QToolButton>

// QT Solutions (qtpropertybrowser) headers
#include "QtVariantProperty"
#include "QtTreePropertyBrowser"

// Qanava headers
#include "../qanStyleManager.h"


//-----------------------------------------------------------------------------
namespace qan { // ::qan

	class StyleEditorWidget : public QWidget
	{
		Q_OBJECT

	public:

		StyleEditorWidget( QWidget* parent, qan::Graph& graph );

		//! Set the currently edited style.
		void	setStyle( Style* style );

	protected:
		
		QComboBox*				_styles;
		
		QtVariantEditorFactory* _variantFactory;

		QtTreePropertyBrowser*	_propertyEditor;
		
		QToolButton*			_duplicateStyle;

		qan::Graph&				_graph;

	protected slots:

		void		duplicateStyle( );

		void		styleIndexChanged( int index );

		void		stylesCursorPositionChanged( int old, int n );
	};

} // ::qan
//-----------------------------------------------------------------------------


#endif // uiStyleEditorWidget_h

