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
// \file	qanGrid.cpp
// \author	benoit@qanava.org
// \date	2004 December 05
//-----------------------------------------------------------------------------


// Qanava headers
#include "./qanGridItem.h"


// QT headers
#include <QPen>
#include <QPainter>
#include <QGraphicsLineItem>


namespace qan {  // ::qan


/* Grid Constructor/Destructor  *///-------------------------------------------
/*!
 */
GridItem::GridItem( GraphView* GraphView ) :
    QGraphicsObject( 0 )
{
	//connect( graphView->getGraphicsScene( ), SIGNAL( sceneRectChanged(const QRectF&) ),
	//		 this, SLOT( sceneRectChanged(const QRectF&) ) );

	GraphView->setGrid( this );
	setEnabled( false );
}

GridItem::~GridItem( )
{

}
//-----------------------------------------------------------------------------


/* Graphics View Grid Implementation *///--------------------------------------
QRectF	GridItem::boundingRect( ) const
{
	return QRectF( QPointF( 0., 0. ), QSizeF( 1., 1. ) );
}

void	GridItem::drawBackground( QPainter& painter, const QRectF& rect )
{
    Q_UNUSED( painter ); Q_UNUSED( rect );
}
//-----------------------------------------------------------------------------



/* Grid Content Management  *///-----------------------------------------------
void	GridItem::addLine( QLineF l, float w, bool dash, bool dot )
{
	QGraphicsLineItem* line = new QGraphicsLineItem( l, this );
	Q_UNUSED( line );
    Q_UNUSED( w ); Q_UNUSED( dash ); Q_UNUSED( dot );
	// TODO: dash, dot, w
}

void	GridItem::addRectangle( QRectF r, QColor c )
{
    Q_UNUSED( r ); Q_UNUSED( c );
}

void	GridItem::addText( const QString& text, QPointF p, bool bold )
{
    Q_UNUSED( text ); Q_UNUSED( p ); Q_UNUSED( bold );
}

void	GridItem::addHorizontalLine( QLineF l, int w, bool dash, bool dot )
{
    Q_UNUSED( l ); Q_UNUSED( w ); Q_UNUSED( dash ); Q_UNUSED( dot );
}

void	GridItem::addVerticalLine( QLineF l, int w, bool dash, bool dot )
{
    Q_UNUSED( l ); Q_UNUSED( w ); Q_UNUSED( dash ); Q_UNUSED( dot );
}
//-----------------------------------------------------------------------------



/* Regular Grid Management  *///-----------------------------------------------
GridRegularItem::GridRegularItem( GraphView* GraphView, int spacing ) : 
	GridItem( GraphView ), 
	_spacing( spacing ),
	_sizeMax( 0, 0 ), 
	_gradCount( 0, 0 )
{

}
//-----------------------------------------------------------------------------



/* CheckBoard Grid Management  *///--------------------------------------------
GridCheckBoardItem::GridCheckBoardItem( GraphView* GraphView, QColor white, QColor black, qreal length ) : 
	GridItem( GraphView ), 
	_white( white ),
	_black( black )
{
	_squaresPattern = QPixmap( length * 2, length * 2 );
	QPainter painter( &_squaresPattern );
	painter.fillRect( 0,		0,		length, length, white );
	painter.fillRect( length,	0,		length, length, black );
	painter.fillRect( 0,		length,	length, length, black );
	painter.fillRect( length,	length,	length, length, white );
	painter.end( );

	setEnabled( false );
}

void	GridCheckBoardItem::drawBackground( QPainter& painter, const QRectF& rect )
{
	GridItem::drawBackground( painter, rect );

	int left	= ( int )rect.left( );
	int top		= ( int )rect.top( );
	int x = ( ( left / 100 ) * 100 );
	int y = ( ( top / 100 ) * 100 );

	if ( left < 0 )
		x = x - 100;
	if ( top < 0 )
		y = y - 100;

	if ( !_squaresPattern.isNull( ) )
		painter.drawTiledPixmap( x, y, rect.width( ) + 100, rect.height( ) + 100, _squaresPattern );
}
//-----------------------------------------------------------------------------


} // ::qan
