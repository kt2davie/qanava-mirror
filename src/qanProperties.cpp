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
// \file	qanProperties.cpp
// \author	benoit@qanava.org
// \date	2014 April 12
//-----------------------------------------------------------------------------


// Qanava headers
#include "./qanProperties.h"

// QT headers
#include <QColor>
#include <QVariant>
#include <QFont>
#include <QSet>


namespace qan { // ::qan

QtVariantProperty*		Properties::addProperty( QString name, QVariant value )
{
	// Update existing properties
	QtVariantProperty* p = getProperty( name );
	if ( p != 0 )
		p->setValue( value );
	else
	{
		// Otherwise, create it
		p = QtVariantPropertyManager::addProperty( value.type( ), name );
		if ( p != 0 )
		{
			p->setValue( value );
			_properties.append( p );

			if ( value.type( ) == QVariant::Double )
				p->setAttribute( QLatin1String( "singleStep" ), 0.1 );
		}
	}

	return p;
}

QtVariantProperty*	Properties::addProperty( QString name, int propertyType, QStringList enumNames )
{
	// Update existing properties
	QtVariantProperty* p = getProperty( name );
	if ( p == 0 )
		p = QtVariantPropertyManager::addProperty( propertyType, name );

	if ( p != 0 )
	{
		p->setAttribute( QLatin1String( "enumNames" ), enumNames);
		p->setValue( 0 );
		_properties.append( p );
	}

	return p;
}

QVariant	Properties::get( QString propertyName )
{
	foreach( QtProperty* p, _properties )
		if ( p->propertyName( ) == propertyName )
		{
			QtVariantProperty* vp = variantProperty( p );
			return vp->value( );
		}
	return QVariant( );
}

QColor	Properties::getColor( QString propertyName )
{
	return get( propertyName ).value< QColor >( );
}
		
QtVariantProperty*	Properties::getProperty( QString propertyName )
{
	foreach( QtProperty* p, _properties )
		if ( p->propertyName( ) == propertyName )
			return variantProperty( p );
	return 0;
}

} // ::qan

