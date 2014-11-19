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
// \file	uiStyleEditorWidget.cpp
// \author	benoit@qanava.org
// \date	2014/03/01
//-----------------------------------------------------------------------------


// QT headers
#include <QVBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QToolBar>

// Qanava headers
#include "../qanGraph.h"
#include "./uiStyleEditorWidget.h"


namespace qan { // ::qan


//-----------------------------------------------------------------------------
StyleEditorWidget::StyleEditorWidget( QWidget* parent, qan::Graph& graph ) : 
	QWidget( parent ), 
	_variantFactory( 0 ),
	_propertyEditor( 0 ),
	_duplicateStyle( 0 ), 
	_graph( graph )
{
	_propertyEditor = new QtTreePropertyBrowser( this );
	_variantFactory = new QtVariantEditorFactory( this );

	_styles = new QComboBox( this );
	_styles->setModel( &_graph.getStyleManager( ) );
	_styles->setModelColumn( 0 );
	_styles->setEditable( true );
	if ( _styles->lineEdit( ) != 0 )
	{
		_styles->lineEdit( )->setText( "Style name" );
		QFont font( _styles->lineEdit( )->font( ) );
		font.setItalic( true );
		_styles->lineEdit( )->setFont( font );
		connect( _styles->lineEdit( ), SIGNAL( cursorPositionChanged( int, int ) ), this, SLOT( stylesCursorPositionChanged( int, int ) ) );
	}
	_styles->setInsertPolicy( QComboBox::NoInsert ); // Do not automatically add text entered text, leave that work for style manager list model
	connect( _styles, SIGNAL( activated( int ) ), this, SLOT( styleIndexChanged( int ) ) );
	_styles->setEnabled( true );

	_duplicateStyle = new QToolButton( this );
	_duplicateStyle->setIcon( QIcon( "./images/plus.png" ) );
	_duplicateStyle->setText( "Duplicate" );
	_duplicateStyle->setEnabled( false );
	connect( _duplicateStyle, SIGNAL( clicked( ) ), this, SLOT( duplicateStyle( ) ) );

	QVBoxLayout* vbox = new QVBoxLayout( this );
	vbox->setMargin( 1 );

	QHBoxLayout* hbox = new QHBoxLayout( this );
	hbox->setMargin( 1 );
	hbox->addWidget( _styles );
	hbox->addWidget( _duplicateStyle );

	vbox->addLayout( hbox );
	vbox->addWidget( _propertyEditor );

	// Show the first available style by default
	if ( _graph.getStyleManager( ).getStyles( ).size( ) > 0 )
		setStyle( _graph.getStyleManager( ).getStyles( ).at( 0 ) );
}

void	StyleEditorWidget::setStyle( Style* style )
{
    if ( style == 0 )
	{
		if ( _styles->lineEdit( ) != 0 )
			_styles->lineEdit( )->setText( QString( "Style name" ) );
		_duplicateStyle->setEnabled( false );
		return;
	}

	_propertyEditor->clear( );
	_propertyEditor->setFactoryForManager( ( QtVariantPropertyManager* )style, _variantFactory );

	QList< QtVariantProperty* >& props = style->getProperties( );
	foreach( QtVariantProperty* p, props )
		_propertyEditor->addProperty( p );

	// Set the style active in styles combo box
	_styles->setCurrentIndex( _graph.getStyleManager( ).getStyles( ).indexOf( style ) );
	if ( _styles->lineEdit( ) != 0 )
		_styles->lineEdit( )->setText( style->getName( ) );
}

void	StyleEditorWidget::duplicateStyle( )
{
	Style* srcStyle = _graph.getStyleManager( ).getStyles( ).at( _styles->currentIndex( ) );
	if ( srcStyle != 0 )
	{
		Style* style = _graph.getStyleManager( ).addStyle( _styles->currentText( ), "" );	// Use the combo box label for new style name
		if ( style != 0 )
		{
			QList< QtVariantProperty* >& srcProps = srcStyle->getProperties( );
			foreach( QtVariantProperty* p, srcProps )
			{
				if ( p->propertyType( ) == QtVariantPropertyManager::enumTypeId() )
				{
					QStringList enumNames = p->attributeValue( QLatin1String( "enumNames" ) ).toStringList( );
					QtVariantProperty* dp = style->addProperty( p->propertyName( ), QtVariantPropertyManager::enumTypeId(), enumNames );
					if ( dp != 0 )
						dp->setValue( p->value( ) );
				}
				else
					style->addProperty( p->propertyName( ), p->value( ) );
			}

			_duplicateStyle->setEnabled( false );
			setStyle( style );
		}
	}
}

void	StyleEditorWidget::styleIndexChanged( int index )
{
	Style* style = _graph.getStyleManager( ).getStyles( ).at( index );
	if ( style != 0 )
		setStyle( style );
}

void	StyleEditorWidget::stylesCursorPositionChanged( int old, int n )
{
    Q_UNUSED( old );
    Q_UNUSED( n );
	_duplicateStyle->setEnabled( true );

	// Activate the duplicate button if the entered text is different from the current styles names
	if ( _styles->lineEdit( ) != 0 )
	{
		qan::Style::List styles = _graph.getStyleManager( ).getStyles( );
		foreach ( Style* style, styles )
			if ( style->getName( ) == _styles->lineEdit( )->text( ) )
			{
				_duplicateStyle->setEnabled( false );
				return;
			}
	}
}
//-----------------------------------------------------------------------------

} // ::qan
