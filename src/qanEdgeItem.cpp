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
// \file	qanEdgeItem.cpp
// \author	benoit@qanava.org
// \date	2007 February 08
//-----------------------------------------------------------------------------

// QT headers
#include <QPainter>
#include <QTimer>
#include <QGraphicsLinearLayout>
#include <QtCore/qmath.h>

// Qanava headers
#include "./qanNodeItem.h"
#include "./qanEdge.h"
#include "./qanEdgeItem.h"
#include "./qanGraphScene.h"


namespace qan {	// ::qan


/* Edge Constructor/Destructor *///--------------------------------------------
EdgeItem::EdgeItem( GraphScene& scene, Edge& edge, QGraphicsItem* parent ) : 
	GraphItem( scene, parent ),
	_edge( edge ),
	_line( ),
	_drawBRect( false ),
	_hasArrow( true ),
	_arrowSize( 4 ),
	_lineStyle( Qt::SolidLine ),
	_lineWidth( 2. ),
	_lineColor( Qt::black ),
	_br( ),
	_bp( ),
	_labelItem( 0 )
{
	setZValue( 1. );

	setAcceptedMouseButtons( Qt::NoButton );				// Do not accept mouse boutton (propagate the events on an object behind this edge)
	setFlag( QGraphicsItem::ItemClipsToShape, false );		// Considerable drawing speed up
	setFlag( QGraphicsItem::ItemIsMovable, false );
	setFlag( QGraphicsItem::ItemIsSelectable, false );
	setFlag( QGraphicsItem::ItemIsFocusable, false );
	setVisible( true );

	activatePropertiesPopup( edge.getProperties( ) );

	qan::Style* style = _styleManager.getStyle( edge );
	if ( style == 0 )
	{
		style = _styleManager.getTargetStyle( "qan::Edge" );
		if ( style != 0 )
			_styleManager.styleEdge( _edge, style->getName( ), false );	// doNotUpdateItem = false to avoid infinite recursion
	}

	// Inititalize edge label item
	_labelItem = new QGraphicsSimpleTextItem( this );
	_labelItem->setText( getEdge( ).getLabel( ) );
}

EdgeItem::~EdgeItem( ) { }
//-----------------------------------------------------------------------------


/* EdgeItem Graphics Item Management *///--------------------------------------
void	EdgeItem::updateItem( )
{
	// Get the source and destination node graphics items
	QGraphicsItem* srcGraphicsItem = getEdge( ).getSrc( ).getGraphicsItem( );
	QGraphicsItem* dstGraphicsItem = getEdge( ).getDst( ).getGraphicsItem( );
	Q_ASSERT( srcGraphicsItem != 0 );
	Q_ASSERT( dstGraphicsItem != 0 );

	QRectF srcBr = srcGraphicsItem->sceneBoundingRect( );
	QRectF dstBr = dstGraphicsItem->sceneBoundingRect( );
	_line = QLineF( srcBr.center( ), dstBr.center( ) ), 
	_line = getLineIntersection( _line, srcBr, dstBr );
	if ( qFuzzyCompare( 1 + _line.length( ) , 1 + 1.0e-200 ) ) 
		return;

	// Computing a bounding rect
	QRectF br( _line.p1( ), QSizeF( _line.p2( ).x( ) - _line.p1( ).x( ), _line.p2( ).y( ) - _line.p1( ).y( ) ) );
	double aSize( _arrowSize ); 
	br = br.normalized( ).adjusted( -aSize, -aSize, aSize, aSize );

	// Compute a bounding polygon
	const double Pi = 3.141592653;
	double TwoPi = 2.0 * Pi;
	double angle = qAcos( _line.dx( ) / _line.length( ) );
	if ( _line.dy( ) <= 0 )
		angle = TwoPi - angle;
	QTransform m;
	m.rotate( angle * 180. / Pi  );

	QPolygonF bp;
	QPointF p1( 0., 0. );
	QPointF p2( QPointF( _line.length( ), p1.y( ) ) );
	aSize += 0.1; 
	bp	<< p1 + QPointF( 0, -_arrowSize ) 
		<< p2 + QPointF( 0, -_arrowSize ) 
		<< p2 + QPointF( 0, _arrowSize ) 
		<< p1 + QPointF( 0, _arrowSize );	// Generate a rectangular polygon enclosing the equivalent horizontal edge line
	bp = m.map( bp );						// Rotate the bounding polygon according to edge line inclination
	bp.translate( _line.p1( ) - br.topLeft( ) );	// Mapping to local item coordinate

	prepareGeometryChange( );
	setPos( br.topLeft( ) );
	_br = QRectF( QPointF( 0., 0. ), br.size( ) );
	_bp = bp;

	// Update label item position
	if ( _labelItem != 0 )
		_labelItem->setPos( _br.center( ) - ( QPointF( _labelItem->boundingRect( ).width( ) / 2., 0. ) ) );
}

void	EdgeItem::updateItemStyle( )
{
	_hasArrow = true;
	_drawBRect = false;
	_lineStyle = Qt::SolidLine;
	_lineWidth = 2.;
	_arrowSize = 6.;

	qan::Style* style = _styleManager.getStyle( getEdge( ) );
	if ( style != 0 )
	{
		if ( style->has( "Draw Bounding Rect" ) )
			_drawBRect = ( Qt::PenStyle )( style->get( "Draw Bounding Rect" ).toBool( ) );
		if ( style->has( "Line Style" ) )
			_lineStyle = ( Qt::PenStyle )( style->get( "Line Style" ).toInt( ) + 1 );	// +1 since 0 is noline and does not appears in style selection dialog
		if ( style->has( "Line Color" ) )
			_lineColor = style->getColor( "Line Color" );
		const QVariant& lineWidth = style->get( "Line Width" );
		const QVariant& arrowSize = style->get( "Arrow Size" );
		_lineWidth = ( lineWidth.isValid( ) ? lineWidth.toFloat( ) : _lineWidth );
		_arrowSize = ( arrowSize.isValid( ) ? arrowSize.toFloat( ) : _arrowSize );
	}

	updateItem( );	// Geometry may change, for example according to arrow size
}
//-----------------------------------------------------------------------------


/* Edge Drawing Management *///------------------------------------------------
void	EdgeItem::paint( QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget )
{
    Q_UNUSED( option ); Q_UNUSED( widget );
	if ( !isVisible( ) )
		return;

	// Paint the line between src and dst
	QPen arrowPen( _lineColor, _lineWidth, ( Qt::PenStyle )_lineStyle, Qt::RoundCap, Qt::RoundJoin );
	if ( _hasArrow && _line.length( ) >= _arrowSize + 1. )
	{
		// Get the intersection between arrow and dst node
		painter->setPen( arrowPen );
		QLineF line( mapFromScene( _line.p1( ) ), mapFromScene( _line.p2( ) ) );
		drawArrow( painter, line, _lineColor, _arrowSize );
	}
	else		// Line is drawn in drawArrow, if there is no arrow, draw a simple line
	{
		painter->setPen( arrowPen );
		QLineF line( mapFromScene( _line.p1( ) ), mapFromScene( _line.p2( ) ) );
		painter->drawLine( line );
	}

	if ( _drawBRect )
	{
		// Debug code to visualize edge bbox
		painter->setPen( QPen( Qt::red ) );
		painter->setBrush( QBrush(  ) );
		painter->drawRect( _br );
		painter->setPen( QPen( Qt::green ) );
		painter->drawPolygon( _bp );
	}
}

QRectF	EdgeItem::boundingRect( ) const
{
	return _br;
}

QPainterPath	EdgeItem::shape( ) const
{
	QPainterPath path;
	path.addPolygon( _bp );
	return path;
}

QLineF	EdgeItem::getLineIntersection( QLineF line, QRectF srcBr, QRectF dstBr )
{
	// Test intersection with all borders
	QLineF top( dstBr.topLeft( ), dstBr.topRight( ) );
	QLineF right( dstBr.topRight( ), dstBr.bottomRight( ) );
	QLineF bottom( dstBr.bottomLeft( ), dstBr.bottomRight( ) );
	QLineF left( dstBr.topLeft( ), dstBr.bottomLeft( ) );

	QPointF destination = line.p2( );
	if ( !dstBr.isNull( ) )
	{
		QPointF intersection;
		if ( line.intersect( top, &intersection ) == QLineF::BoundedIntersection ||
			line.intersect( right, &intersection ) == QLineF::BoundedIntersection ||
			line.intersect( bottom, &intersection ) == QLineF::BoundedIntersection ||
			line.intersect( left, &intersection ) == QLineF::BoundedIntersection )
			destination = intersection;	
	}

	// Test intersection with all borders
	top = QLineF( srcBr.topLeft( ), srcBr.topRight( ) );
	right = QLineF( srcBr.topRight( ), srcBr.bottomRight( ) );
	bottom = QLineF( srcBr.bottomLeft( ), srcBr.bottomRight( ) );
	left = QLineF( srcBr.topLeft( ), srcBr.bottomLeft( ) );

	QPointF source = line.p1( );
	if ( !srcBr.isNull( ) )
	{
		QPointF intersection;
		if ( line.intersect( top, &intersection ) == QLineF::BoundedIntersection ||
			line.intersect( right, &intersection ) == QLineF::BoundedIntersection ||
			line.intersect( bottom, &intersection ) == QLineF::BoundedIntersection ||
			line.intersect( left, &intersection ) == QLineF::BoundedIntersection )
			source = intersection;	
	}

	return QLineF( source, destination );
}

/*! \note Arrow is drawn at line second point.
 */
void	EdgeItem::drawArrow( QPainter* painter, QLineF line, QColor color, float arrowSize )
{
	// Don't draw a null line
	if ( line.isNull( ) )
		return;
	if ( qAbs( line.length( ) ) < 0.0001 )
		return;

	const double Pi = 3.141592653;
	double TwoPi = 2.0 * Pi;
	double angle = ::acos( line.dx( ) / line.length( ) );
	if ( line.dy( ) <= 0 )
		angle = TwoPi - angle;

	painter->drawLine( line );

	painter->setRenderHint( QPainter::Antialiasing );
	painter->setPen( QPen( color, 1., Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin ) );
	painter->setBrush( QBrush( color ) );

	painter->save( );
	painter->translate( line.p2( ) );
	painter->rotate( angle * 180. / Pi );

	double arrowLength = arrowSize * 2.;
	QPointF dst = QPointF( -arrowLength /*- 1*/, 0. );
	QPolygonF poly;
	poly	<< QPointF( dst.x( ), dst.y( ) - arrowSize )
			<< QPointF( dst.x( ) + arrowLength, dst.y( ) )
			<< QPointF( dst.x( ), dst.y( ) + arrowSize ) << QPointF( dst.x( ), dst.y( ) - arrowSize );
	painter->drawPolygon( poly );
	painter->restore( );
}


/* HEdge Constructor/Destructor *///--------------------------------------------
HEdgeItem::HEdgeItem( GraphScene& scene, HEdge& hEdge, QGraphicsItem* parent ) : 
	EdgeItem( scene, hEdge, parent ),
	_hEdge( hEdge ),
	_inLineColor( Qt::black ), 
	_outLineColor( Qt::black ),
	_inLineStyle( Qt::SolidLine ), 
	_outLineStyle( Qt::SolidLine ),
	_hLineWidth( 2. ),
	_hLabelRect( )
{
	qan::Style* style = _styleManager.getStyle( hEdge );
	if ( style == 0 )
	{
		style = _styleManager.getTargetStyle( "qan::HEdge" );
		if ( style != 0 )
			_styleManager.styleEdge( _edge, style->getName( ), false );	// doNotUpdateItem = false to avoid infinite recursion
	}
}

HEdgeItem::~HEdgeItem( ) 
{ 
	foreach ( QGraphicsSimpleTextItem* outLabelItem, _edgeOutLabels )
		_scene.removeItem( outLabelItem );
	_edgeOutLabels.clear( );
	foreach ( QGraphicsSimpleTextItem* inLabelItem, _edgeInLabels )
		_scene.removeItem( inLabelItem );
	_edgeInLabels.clear( );
}
//-----------------------------------------------------------------------------


/* EdgeItem Graphics Item Management *///--------------------------------------
void	HEdgeItem::updateItem( )
{
	EdgeItem::updateItem( );

	// Put the in ou out hedge lines in cache
	_edgeOutLines.clear( );
	foreach ( QGraphicsSimpleTextItem* outLabelItem, _edgeOutLabels )
		_scene.removeItem( outLabelItem );
	_edgeOutLabels.clear( );
	QPointF hOrigin = QPointF( _line.p1( ) + QPointF( _line.dx( ) / 2., _line.dy( ) / 2. ) );

	QPolygonF edgePolygon;
	edgePolygon << _line.p1( ) << _line.p2( );

	foreach ( qan::Node* hDst, _hEdge.getHDst( ) )
	{
		QGraphicsItem* hDstGraphicsItem = hDst->getGraphicsItem( );
		Q_ASSERT( hDstGraphicsItem != 0 );

		QRectF hDstBr = hDstGraphicsItem->sceneBoundingRect( );
		QLineF line = QLineF( hOrigin, hDstBr.center( ) );
		line = EdgeItem::getLineIntersection( line, QRectF( ), hDstBr );
		_edgeOutLines.append( line );

		QGraphicsSimpleTextItem* outLabel = new QGraphicsSimpleTextItem( this );
		outLabel->setText( _hEdge.getHNodeLabelMap( ).value( hDst, QString( ) ) );
		_edgeOutLabels.append( outLabel );
		edgePolygon << line.p1( ) << line.p2( );
	}
	_edgeInLines.clear( );
	foreach ( QGraphicsSimpleTextItem* inLabelItem, _edgeInLabels )
		_scene.removeItem( inLabelItem );
	_edgeInLabels.clear( );
	foreach ( qan::Node* hSrc, _hEdge.getHSrc( ) )
	{
		QGraphicsItem* hSrcGraphicsItem = hSrc->getGraphicsItem( );
		Q_ASSERT( hSrcGraphicsItem != 0 );

		QRectF hSrcBr = hSrcGraphicsItem->sceneBoundingRect( );
		QLineF line = QLineF( hSrcBr.center( ), hOrigin );
		line = EdgeItem::getLineIntersection( line, hSrcBr, QRectF( ) );
		_edgeInLines.append( line );

		QGraphicsSimpleTextItem* inLabel = new QGraphicsSimpleTextItem( this );
		inLabel->setText( _hEdge.getHNodeLabelMap( ).value( hSrc, QString( ) ) );
		_edgeInLabels.append( inLabel );
		edgePolygon << line.p1( ) << line.p2( );
	}

	updateItemStyle( );

	// Configure the hedge base source and destination node
	prepareGeometryChange( );
	QRectF br( edgePolygon.boundingRect( ) );
	br = br.normalized( ).adjusted( -_arrowSize, -_arrowSize, _arrowSize, _arrowSize );

	prepareGeometryChange( );
	setPos( br.topLeft( ) );
	_br = QRectF( QPointF( 0., 0. ), br.size( ) );
}

void	HEdgeItem::updateItemStyle( )
{
	qan::Style* style = _styleManager.getStyle( _hEdge );
	if ( style != 0 )
	{
		if ( style->has( "Line In Color" ) )
			_inLineColor = style->getColor( "Line In Color" );
		if ( style->has( "Line Out Color" ) )
			_outLineColor = style->getColor( "Line Out Color" );

		if ( style->has( "Line In Style" ) )
			_inLineStyle = ( Qt::PenStyle )( style->get( "Line In Style" ).toInt( ) + 1 );	// +1 since 0 is noline and does not appears in style selection dialog
		if ( style->has( "Line Out Style" ) )
			_outLineStyle = ( Qt::PenStyle )( style->get( "Line Out Style" ).toInt( ) + 1 );	// +1 since 0 is noline and does not appears in style selection dialog

		_hLineWidth = 2.0;
		const QVariant& hLineWidth = style->get( "Hyper Line Width" );
		_hLineWidth = ( hLineWidth.isValid( ) ? hLineWidth.toFloat( ) : _hLineWidth );
	}
}
//-----------------------------------------------------------------------------


/* Edge Drawing Management *///------------------------------------------------
void	HEdgeItem::paint( QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget )
{
    Q_UNUSED( option ); Q_UNUSED( widget );
	if ( !isVisible( ) )
		return;

	QPen arrowPen( _lineColor, _lineWidth, ( Qt::PenStyle )_lineStyle, Qt::RoundCap, Qt::RoundJoin );
	painter->setPen( arrowPen );

	// Paint the line between src and dst
	QLineF line( mapFromScene( _line.p1( ) ), mapFromScene( _line.p2( ) ) );
	EdgeItem::drawArrow( painter, line, _lineColor, _arrowSize );

	// Paint the hyper edge junction point
	QPen originPen( _lineColor, _lineWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin );
	painter->setPen( originPen );
	painter->setBrush( _lineColor );
	QLineF localLine( mapFromScene( _line.p1( ) ), mapFromScene( _line.p2( ) ) );
	painter->drawEllipse( localLine.pointAt( 0.5 ), 3, 3 );

	// Paint the out lines
	arrowPen = QPen( _outLineColor, _hLineWidth, ( Qt::PenStyle )_outLineStyle, Qt::RoundCap, Qt::RoundJoin );
	int lineIndex( 0 );
	foreach ( QLineF outLine, _edgeOutLines )
	{
		painter->setPen( arrowPen );
		QLineF localOutLine( mapFromScene( outLine.p1( ) ), mapFromScene( outLine.p2( ) ) );
		EdgeItem::drawArrow( painter, localOutLine, _outLineColor, _arrowSize );

		QGraphicsSimpleTextItem* outLabel = _edgeOutLabels.at( lineIndex++ );
		outLabel->setPos( localOutLine.pointAt( 0.5 ) - QPointF( outLabel->boundingRect( ).width( ) / 2., 0. ) );
	}

	// Paint the in line
	arrowPen = QPen( _inLineColor, _hLineWidth, ( Qt::PenStyle )_inLineStyle, Qt::RoundCap, Qt::RoundJoin );
	lineIndex = 0;
	foreach ( QLineF inLine, _edgeInLines )
	{
		painter->setPen( arrowPen );
		QLineF localInLine( mapFromScene( inLine.p1( ) ), mapFromScene( inLine.p2( ) ) );
		EdgeItem::drawArrow( painter, localInLine, _inLineColor, _arrowSize );

		QGraphicsSimpleTextItem* inLabel = _edgeInLabels.at( lineIndex++ );
		inLabel->setPos( localInLine.pointAt( 0.5 ) - QPointF( inLabel->boundingRect( ).width( ) / 2., 0. ) );
	}

	// Debug code to visualize edge bbox
	if ( _drawBRect )
	{
		painter->setPen( QPen( Qt::red ) );
		painter->setBrush( QBrush( ) );
		QRectF br( boundingRect( ) );
		br.adjust( +1., +1., -1., -1. );
		painter->drawRect( br );
	}
}

QRectF	HEdgeItem::boundingRect( ) const
{
	return _br;
}
//-----------------------------------------------------------------------------



/* Ortho Edge Graphics Item Management *///------------------------------------
void	OrthoEdgeItem::updateItem( )
{
	EdgeItem::updateItem( );
}
//-----------------------------------------------------------------------------


/* Ortho Edge Drawing Management *///------------------------------------------
void	OrthoEdgeItem::paint( QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget )
{
    Q_UNUSED( option ); Q_UNUSED( widget );
	// Get the source and destination node corresponding graph items
	GraphItem* srcGraphItem = getScene( ).getGraphItem( getEdge( ).getSrc( ) );
	GraphItem* dstGraphItem = getScene( ).getGraphItem( getEdge( ).getDst( ) );
	if ( srcGraphItem == 0 || dstGraphItem == 0 )
		return;

	QGraphicsItem* srcGraphicsItem = srcGraphItem->getGraphicsItem( );
	QGraphicsItem* dstGraphicsItem = dstGraphItem->getGraphicsItem( );

	{
		QRectF srcBb = srcGraphicsItem->boundingRect( );
		QRectF dstBb = dstGraphicsItem->boundingRect( );

		QPointF srcCenter = srcGraphicsItem->mapToScene( srcBb.center( ) );
		QPointF dstCenter = dstGraphicsItem->mapToScene( dstBb.center( ) );

		QLineF finalLine;

		// src is top right from dst
		if ( ( srcCenter.x( ) > dstCenter.x( ) ) && ( srcCenter.y( ) < dstCenter.y( ) ) )
		{
			QRectF srcDstBb( QPointF( dstCenter.x( ), srcCenter.y( ) ), 
							 QSizeF( abs( srcCenter.x( ) - dstCenter.x( ) ), abs( srcCenter.y( ) - dstCenter.y( ) ) ) );

			QLineF lineA( mapFromScene( srcDstBb.topRight() ), mapFromScene( srcDstBb.bottomRight() ) );
			finalLine = QLineF( mapFromScene( srcDstBb.bottomRight() ), mapFromScene( srcDstBb.bottomLeft() ) );

			QPen arrowPen( Qt::black, _lineWidth, ( Qt::PenStyle )_lineStyle, Qt::RoundCap, Qt::RoundJoin );
			painter->setPen( arrowPen );
			painter->drawLine( lineA );
		}
		// src is top left from dst
		else if ( ( srcCenter.x( ) < dstCenter.x( ) ) && ( srcCenter.y( ) < dstCenter.y( ) ) )
		{
			QRectF srcDstBb( QPointF( srcCenter.x( ), srcCenter.y( ) ), 
							 QSizeF( abs( srcCenter.x( ) - dstCenter.x( ) ), abs( srcCenter.y( ) - dstCenter.y( ) ) ) );

			QLineF lineA( mapFromScene( srcDstBb.topLeft() ), mapFromScene( srcDstBb.bottomLeft() ) );
			finalLine = QLineF( mapFromScene( srcDstBb.bottomLeft() ), mapFromScene( srcDstBb.bottomRight() ) );

			QPen arrowPen( Qt::black, _lineWidth, ( Qt::PenStyle )_lineStyle, Qt::RoundCap, Qt::RoundJoin );
			painter->setPen( arrowPen );
			painter->drawLine( lineA );
		}
		// src is bottom left from dst
		else if ( ( srcCenter.x( ) < dstCenter.x( ) ) && ( srcCenter.y( ) > dstCenter.y( ) ) )
		{
			QRectF srcDstBb( QPointF( srcCenter.x( ), dstCenter.y( ) ), 
							 QSizeF( abs( srcCenter.x( ) - dstCenter.x( ) ), abs( srcCenter.y( ) - dstCenter.y( ) ) ) );

			QLineF lineA( mapFromScene( srcDstBb.bottomLeft() ), mapFromScene( srcDstBb.topLeft() ) );
			finalLine = QLineF( mapFromScene( srcDstBb.topLeft() ), mapFromScene( srcDstBb.topRight() ) );

			QPen arrowPen( Qt::black, _lineWidth, ( Qt::PenStyle )_lineStyle, Qt::RoundCap, Qt::RoundJoin );
			painter->setPen( arrowPen );
			painter->drawLine( lineA );
		}
		// src is bottom right from dst
		else if ( ( srcCenter.x( ) > dstCenter.x( ) ) && ( srcCenter.y( ) > dstCenter.y( ) ) )
		{
			QRectF srcDstBb( QPointF( dstCenter.x( ), dstCenter.y( ) ), 
							 QSizeF( abs( srcCenter.x( ) - dstCenter.x( ) ), abs( srcCenter.y( ) - dstCenter.y( ) ) ) );

			QLineF lineA( mapFromScene( srcDstBb.bottomRight() ), mapFromScene( srcDstBb.topRight() ) );
			finalLine = QLineF( mapFromScene( srcDstBb.topRight() ), mapFromScene( srcDstBb.topLeft() ) );

			QPen arrowPen( Qt::black, _lineWidth, ( Qt::PenStyle )_lineStyle, Qt::RoundCap, Qt::RoundJoin );
			painter->setPen( arrowPen );
			painter->drawLine( lineA );
		}

		// Find the intersection between final line and dst bounding box
		QRectF br = mapFromItem( dstGraphicsItem, dstGraphicsItem->boundingRect( ) ).boundingRect( );
		//QRectF br = mapFromScene( dstBb ).boundingRect( );   // FIXME v0.5.0, why mapFromScene doesn't works compared to mapFromItem ? (because EdgeItem has no common parent with dst...?)
		QLineF dstTop( br.topLeft( ), br.topRight( ) );
		QLineF dstRight( br.topRight( ), br.bottomRight( ) );
		QLineF dstBottom( br.bottomLeft( ), br.bottomRight( ) );
		QLineF dstLeft( br.topLeft( ), br.bottomLeft( ) );

		QPointF arrowEnd;
		if ( finalLine.intersect( dstTop, &arrowEnd ) == QLineF::BoundedIntersection ||
			finalLine.intersect( dstRight, &arrowEnd ) == QLineF::BoundedIntersection ||
			finalLine.intersect( dstBottom, &arrowEnd ) == QLineF::BoundedIntersection ||
			finalLine.intersect( dstLeft, &arrowEnd ) == QLineF::BoundedIntersection )
		{
			// Draw the arrow
			const double Pi = 3.141592653;
			double TwoPi = 2.0 * Pi;
			double angle = ::acos( finalLine.dx( ) / finalLine.length( ) );
			if ( finalLine.dy( ) <= 0 )
				angle = TwoPi - angle;
			painter->setPen( QPen( Qt::black, _lineWidth, ( Qt::PenStyle )_lineStyle, Qt::RoundCap, Qt::RoundJoin ) );
			EdgeItem::drawArrow( painter, QLineF( finalLine.p1( ), arrowEnd ), QColor( Qt::black ) );
		}
	}
}
//-----------------------------------------------------------------------------

}	// ::qan

