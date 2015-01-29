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
// \file	uiStyleBrowserWidget.h
// \author	benoit@qanava.org
// \date	2014 June 23
//-----------------------------------------------------------------------------

#ifndef uiStyleBrowserWidget_h
#define uiStyleBrowserWidget_h

// QT headers
#include <QGraphicsWidget>
#include <QGraphicsLayoutItem>
#include <QGraphicsGridLayout>

// Qanava headers
#include "../qanStyleManager.h"
#include "../qanGraphScene.h"
#include "../qanNodeRectItem.h"


namespace qan { // ::qan
namespace ui  { // ::qan::ui

class StyleNodeItem : public qan::NodeRectItem
{
	Q_OBJECT

	/*! \name StyleNodeItem Object Management *///-----------------------------
	//@{
public:

    StyleNodeItem( qan::GraphScene& scene, qan::Node& node );
	//@}
	//-------------------------------------------------------------------------


	/*! \name Graphics Layout Management *///----------------------------------
	//@{
public:

/*	virtual void	setGeometry ( const QRectF & rect );

protected:

	virtual QSizeF	sizeHint( Qt::SizeHint which, const QSizeF & constraint = QSizeF( ) ) const;
*/	//@}
	//-------------------------------------------------------------------------


	/*! \name Node Item Mouse Management *///----------------------------------
	//@{
	protected:

		virtual	void		mouseMoveEvent( QGraphicsSceneMouseEvent* e );
		virtual void		mousePressEvent( QGraphicsSceneMouseEvent* e );
		virtual void		mouseReleaseEvent( QGraphicsSceneMouseEvent* e );
	//@}
	//-------------------------------------------------------------------------
};

/*! Show a transparent list of group with an associed checkable control to show or hide group items
*/
class StyleBrowserWidget : public QGraphicsWidget
{
	Q_OBJECT

	/*! \name StyleBrowserWidget Object Management *///------------------------
	//@{
public:

	StyleBrowserWidget( QGraphicsView& view, qan::GraphScene& scene, qan::StyleManager& styleManager );

	virtual ~StyleBrowserWidget( );

	void	forceUpdate( ) { viewViewportChanged( 0 ); }

protected:

	void						resetStyleItems( );

	void						resetStyleItemsLayout( );

	QList< qan::Node* >			_styleNodes;

    //QList< StyleNodeItem* >		_styleNodeItems;
    QList< qan::NodeItem* >		_styleNodeItems;
    //@}
	//-------------------------------------------------------------------------


	/*! \name Style Management *///--------------------------------------------
	//@{
protected:

	QGraphicsView&			_view;

	qan::GraphScene&		_scene;

	qan::StyleManager&		_styleManager;

protected slots:

	void	viewViewportChanged( int value );

	void	viewResized( QSize s );

	void	styleManagerStyleModified( qan::Style* style );

	void	styleManagerStyleAdded( qan::Style* style );

	void	styleManagerStyleRemoved( qan::Style* style );

private:

    qreal   _widthCache;  // Cache used internally to limit call to resetStyleItemsLayout()
	//@}
	//-------------------------------------------------------------------------
};
//-----------------------------------------------------------------------------

} // ::qan::ui
} // ::qan

#endif	// uiStyleBrowserWidget_h
