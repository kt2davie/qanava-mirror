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
// This file is a part of the Qarte software.
//
// \file	qanStyleManager.cpp
// \author	benoit@qanava.org
// \date	2005 January 03
//-----------------------------------------------------------------------------


// Qanava headers
#include "./qanStyleManager.h"
#include "./qanGraph.h"


// QT headers
#include <QColor>
#include <QVariant>
#include <QFont>
#include <QSet>


namespace qan { // ::qan


/* Interview Interface *///----------------------------------------------------
QVariant StyleManager::data( const QModelIndex &index, int role ) const
{
	if ( !index.isValid( ) )
		return QVariant( "index invalid" );

	QVariant d;
	if ( role == Qt::DisplayRole )
	{
		//Style* style = qobject_cast< Style* >( reinterpret_cast< QObject* >( index.internalPointer( ) ) );
		Style* style = _styles.at( index.row( ) );
		if ( style != 0 && !style->getName( ).isEmpty( ) )
			d = style->getName( );
		else
			d = QString( "Style #" ) + QString::number( _styles.indexOf( style ) );
	}
	return d;
}

Qt::ItemFlags StyleManager::flags( const QModelIndex& index ) const
{
	return ( index.isValid( ) ? Qt::ItemIsSelectable | Qt::ItemIsEnabled : Qt::ItemIsEnabled );
}

QVariant StyleManager::headerData( int section, Qt::Orientation /*orientation*/, int role ) const
{
	QVariant d;
	if ( role == Qt::DisplayRole && section == 0 )
		return QString( "Style name" );
	return d;
}
//-----------------------------------------------------------------------------



/* Style Manager Constructor/Destructor *///-----------------------------------
StyleManager::StyleManager( qan::Graph* graph, QObject* parent ) : 
	QAbstractListModel( parent ),
	_graph( *graph )
{
	// Configure default edge and node styles
	qan::Style* nodeStyle = addStyle( "default node", "qan::Node" );
	nodeStyle->addProperty( "No Background", QVariant( false ) );
	nodeStyle->addProperty( "Back Color", QVariant( QColor( 255, 255, 255 ) ) );
	nodeStyle->addProperty( "Border Color", QVariant( QColor( 0, 0, 0 ) ) );
    QStringList lineStyles; lineStyles << "Solid line" << "Dash line" << "Dot line" << "Dash Dot line" << "Dash Dot Dot line";
	nodeStyle->addProperty( "Border Style", QtVariantPropertyManager::enumTypeId(),  lineStyles );
	nodeStyle->addProperty( "Border Width", QVariant( 1.0 ) );
	nodeStyle->addProperty( "Font", QVariant( QFont( ) ) );
	nodeStyle->addProperty( "Maximum Size", QVariant( QSizeF( 100., 25. ) ) );
	nodeStyle->addProperty( "Has Shadow", QVariant( true ) );
	nodeStyle->addProperty( "Shadow Color", QVariant( QColor( 50, 50, 50 ) ) );
	nodeStyle->addProperty( "Shadow Offset", QVariant( QSizeF( 2., 2. ) ) );

	qan::Style* edgeStyle = addStyle( "default edge", "qan::Edge" );
	lineStyles.clear( ); lineStyles << "Solid line" << "Dash line" << "Dot line" << "Dash Dot line" << "Dash Dot Dot line";
	edgeStyle->addProperty( "Line Color", QVariant( QColor( Qt::black ) ) );
	edgeStyle->addProperty( "Line Style", QtVariantPropertyManager::enumTypeId(),  lineStyles );
	edgeStyle->addProperty( "Line Width", QVariant( 1.2 ) );
	edgeStyle->addProperty( "Arrow Size", QVariant( 4.0 ) );
    edgeStyle->addProperty( "Draw Bounding Rect", QVariant( false ) );

	qan::Style* hEdgeStyle = addStyle( "default hyper edge", "qan::HEdge" );
	lineStyles.clear( ); lineStyles << "Solid line" << "Dash line" << "Dot line" << "Dash Dot line" << "Dash Dot Dot line";
	hEdgeStyle->addProperty( "Line Color", QVariant( QColor( Qt::black ) ) );
	hEdgeStyle->addProperty( "Line Style", QtVariantPropertyManager::enumTypeId(),  lineStyles );
	hEdgeStyle->addProperty( "Line Width", QVariant( 1.2 ) );
	hEdgeStyle->addProperty( "Arrow Size", QVariant( 4.0 ) );
	hEdgeStyle->addProperty( "Draw Bounding Rect", QVariant( true ) );
	hEdgeStyle->addProperty( "Line In Color", QVariant( QColor( Qt::black ) ) );
	hEdgeStyle->addProperty( "Line Out Color", QVariant( QColor( Qt::black ) ) );
	hEdgeStyle->addProperty( "Line In Style", QtVariantPropertyManager::enumTypeId(),  lineStyles );
	hEdgeStyle->addProperty( "Line Out Style", QtVariantPropertyManager::enumTypeId(),  lineStyles );
	hEdgeStyle->addProperty( "Hyper Line Width", QVariant( 1.2 ) );
}

StyleManager::~StyleManager( )
{
	QSet< Style* >	styles = QSet< Style* >::fromList( _styles );
	foreach ( Style* style, styles )
			delete style;
	clearImages( );
}
//-----------------------------------------------------------------------------



/* Style Management *///-------------------------------------------------------
Style*	StyleManager::addStyle( QString name, QString target )
{
	if ( getStyle( name ) == 0 )
	{
		Style* style = new Style( name, this, target );
		connect( style, SIGNAL( valueChanged( QtProperty *, const QVariant & ) ), this, SLOT( propertyValueChanged( QtProperty *, const QVariant & ) ) );

		beginInsertRows( QModelIndex( ), _styles.size( ), _styles.size( ) + 1 );
		_styles.append( style ); 
		endInsertRows( );
		emit styleAdded( style );

		return style;
	}
	return getStyle( name );
}

Style*	StyleManager::insertStyle( Style* style )
{
	if ( getStyle( style->getName( ) ) == 0 )
	{
		connect( style, SIGNAL( valueChanged( QtProperty *, const QVariant & ) ), this, SLOT( propertyValueChanged( QtProperty *, const QVariant & ) ) );

		beginInsertRows( QModelIndex( ), _styles.size( ), _styles.size( ) + 1 );
		_styles.append( style ); 
		endInsertRows( );
		emit styleAdded( style );

		return style;
	}
	return 0;
}

void	StyleManager::removeStyle( Style* style )
{
	if ( !hasStyle( style ) )
		return;

	int stylePos = _styles.indexOf( style );
	beginRemoveRows( QModelIndex( ), stylePos, stylePos );
	if ( style != 0 )
	{
		emit styleRemoved( style );
		_styles.removeAll( style );
		delete style;
	}
	endRemoveRows( );
}

Style*	StyleManager::getStyle( QString name )
{
	foreach ( Style* style, _styles )
		if ( style->getName( ) == name )
			return style;
	return 0;
}

Style*	StyleManager::getTargetStyle( QString target )
{
	foreach ( Style* style, _styles )
		if ( style->getTarget( ) == target )
			return style;
	return 0;
}

/*!
	\param	doNotUpdateItem	used internally to set the style without updating the associed graph item (avoiding infinite recursion, and speed gain, when item is invisible).
 */
void	StyleManager::styleEdge( qan::Edge& edge, QString styleName, bool updateItem )
{
	if ( styleName.isEmpty( ) || styleName.isNull( ) )
		return;

	Style* style = getStyle( styleName );
	if ( style != 0 )
	{
		_edgeStyleMap.insert( &edge, style );
		GraphItem* edgeItem = _graph.getM( ).getGraphItem( edge );	// Update edge item with the new style
		if ( edgeItem != 0 && updateItem )
			edgeItem->updateItemStyle( );
	}
}

void	StyleManager::clearEdgeStyle( qan::Edge& edge )
{
	_edgeStyleMap.remove( &edge );
}

void	StyleManager::styleNode( qan::Node& node, QString styleName )
{
	if ( styleName.isEmpty( ) || styleName.isNull( ) )
		return;

	Style* style = getStyle( styleName );
	if ( style != 0 )
	{
		_nodeStyleMap.insert( &node, style );
		GraphItem* nodeItem = _graph.getM( ).getGraphItem( node );   // Update node item with the new style
		if ( nodeItem != 0 )
			nodeItem->updateItemStyle( );
	}
}

void	StyleManager::clearNodeStyle( qan::Node& node )
{
	_nodeStyleMap.remove( &node );
}

void	StyleManager::removeMapping( void* item )
{
	_nodeStyleMap.remove( ( qan::Node* )item );
	_edgeStyleMap.remove( ( qan::Edge* )item );
}

void	StyleManager::clearMappings( )
{
	_nodeStyleMap.clear( );
	_edgeStyleMap.clear( );
}

void	StyleManager::propertyValueChanged( QtProperty* p, const QVariant& value )
{
    Q_UNUSED( value );
	// Get the style corresponding to this attribute
	QtAbstractPropertyManager* propertyManager = p->propertyManager( );
	if ( propertyManager != 0 && _styles.contains( qobject_cast< Style* >( propertyManager ) ) )
	{
		Style* style = qobject_cast< Style* >( propertyManager );
		emit styleModified( style );

		// Get objects styled with the changing style
		QList< qan::Node* > nodes = _nodeStyleMap.keys( style );
		QList< qan::Edge* > edges = _edgeStyleMap.keys( style );

		foreach ( qan::Node* node, nodes )
		{
			GraphItem* nodeItem = _graph.getM( ).getGraphItem( *node );
			if ( nodeItem != 0 )
				nodeItem->updateItemStyle( );
			else if ( node->getGraphItem( ) != 0 )
				node->getGraphItem( )->updateItemStyle( );
		}
		foreach ( qan::Edge* edge, edges )
		{
			GraphItem* edgeItem = _graph.getM( ).getGraphItem( *edge );
			if ( edgeItem != 0 )
				edgeItem->updateItemStyle( );
		}
	}
}
//-----------------------------------------------------------------------------


/* Image Cache Management *///-------------------------------------------------
/*!
	\return	A cached image loaded from the given filename. If fileName is invalid, the returned image can be queried for isNull().
 */
QImage		StyleManager::getImage( QString fileName )
{
	QImage image = _nameImageMap.value( fileName, QImage( ) ); 
	if ( image.isNull( ) )
	{
		image = QImage( fileName );
		if ( !image.isNull( ) )
			_nameImageMap.insert( fileName, image );
	}
	return image;	// Return an image from the cache or a null image
}

void	StyleManager::clearImages( )
{
	_nameImageMap.clear( );
}
//-----------------------------------------------------------------------------


} // ::qan

