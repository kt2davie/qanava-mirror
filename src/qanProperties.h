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
// \file	qanProperties.h
// \author	benoit@qanava.org
// \date	2014 April 12
//-----------------------------------------------------------------------------


#ifndef qan_Properties_h
#define qan_Properties_h

// QT headers
#include <QVariant>
#include <QList>
#include <QMap>
#include <QAbstractListModel>

// Qanava headers
#include "qanConfig.h"

// QT Solutions (qtpropertybrowser) headers
#include "QtVariantProperty"

//-----------------------------------------------------------------------------
namespace qan // ::qan
{
    //! Define a set of properties based on Qt Solutions property manager.
	/*! Property currently supported are Int, Double, QFont, QColor, Bool, QSizeF and qtpropertymanager enums.
	  */
	class Properties : public QtVariantPropertyManager
	{
		Q_OBJECT

	public:

		Properties( QString name, QObject* parent = 0, QString target = "" ) : 
										QtVariantPropertyManager( parent ), _name( name ), _target( target ) { }

		QString							getName( ) const { return _name; }

		//! Add a property with a given value and the type corresponding to the variant type.
		/*! For an already existing property, it just set its new value.					*/
		QtVariantProperty*				addProperty( QString name, QVariant value );

		//! Add an enum property with name 'name'.
		/*! For an already existing property, it just set its new value.					*/
		QtVariantProperty*				addProperty( QString name, int propertyType, QStringList enumNames );

		//! \return true if this style has a property of name 'propertyName'.
		bool							has( QString propertyName ) { return get( propertyName ).isValid( ); }

		//! \return Value for property of a given 'propertyName', or an invalid QVariant if such a property does not exists.
		QVariant						get( QString propertyName );

		//! \return QColor for property of a given 'propertyName', or an invalid QVariant if such a property does not exists.
		QColor							getColor( QString propertyName );

		//! Return the QtVariantProperty of name "propertyName". 
		QtVariantProperty*				getProperty( QString propertyName );

		QList< QtVariantProperty* >&	getProperties( ) { return _properties; }

		//! Get the object class name targeted for this style (usually qan::Node or qan::Edge);
		QString							getTarget( ) const { return _target; }

		//! Set the object class name targeted for this style (usually qan::Node or qan::Edge);
		void							setTarget( QString target ) { _target = target; }

	protected:
		
		QList< QtVariantProperty* >	_properties;

		QString						_name;

		QString						_target;
	public:

		typedef QList< Properties* > List;
	};
} // ::qan
//-----------------------------------------------------------------------------

#endif // qan_Properties_h


