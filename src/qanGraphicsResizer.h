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
// \file	qanGraphicsResizer.h
// \author	benoit@qanava.org
// \date	2014 December 01
//-----------------------------------------------------------------------------

#ifndef qanGraphicsResizer_h
#define qanGraphicsResizer_h

// QT headers
#include <QGraphicsItem>
#include <QGraphicsLayoutItem>
#include <QGraphicsDropShadowEffect>

// Qanava headers
#include "./qanGraphicsController.h"


namespace qan { // ::qan

    //! Add a dynamic resizing capacity to graphics item trought a standard QGraphicsLayoutItem interface.
    /*!
        \nosubgrouping
    */
    class GraphicsResizer : public QGraphicsObject
    {
        Q_OBJECT

        /*! \name GraphicsResizer Object Management *///---------------------------
        //@{
    public:
        GraphicsResizer( QGraphicsItem* target, QGraphicsLayoutItem* targetLayout, GraphicsController* controller = 0, QSizeF gripSize = QSizeF( 10., 15. ) );
        virtual ~GraphicsResizer( ) { }

    private:
        Q_DISABLE_COPY( GraphicsResizer );

        //! Resizer target (ie item that is resized by this resizer).
        QGraphicsItem*          _target;

        //! Target associed layout item (usually, target inherits from QGraphicsLayoutItem).
        QGraphicsLayoutItem*    _targetLayout;

        //! Optionnal graphics controller associed to this resizer.
        GraphicsController*     _controller;
        //@}
        //-------------------------------------------------------------------------


        /*! \name Graphics Item Management *///------------------------------------
        //@{
    public:
        virtual QRectF			boundingRect( ) const;

        void					paint( QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = 0 );
        //@}
        //-------------------------------------------------------------------------


        /*! \name Mouse Events Management *///-------------------------------------
        //@{
    public:
        //! Set the resize grip size in item corners (default to 15.0).
        void            setGripSize( QSizeF gripSize ) { _gripSize = gripSize; }

    protected:
        virtual bool    sceneEventFilter( QGraphicsItem* watched, QEvent* e );

        virtual void	mouseMoveEvent( QGraphicsSceneMouseEvent* e );
        virtual void	mousePressEvent( QGraphicsSceneMouseEvent* e );
        virtual void	hoverEnterEvent( QGraphicsSceneHoverEvent* e );
        virtual void	hoverLeaveEvent( QGraphicsSceneHoverEvent* e );
        virtual void	hoverMoveEvent( QGraphicsSceneHoverEvent* e );

        //! Return true is a given position is inside a resizer corner, and set corner to the approriate corner.
        bool            inCorner( QPointF pos, Qt::Corner& corner );

        QSizeF          _gripSize;
        Qt::Corner      _corner;
        //@}
        //-------------------------------------------------------------------------
    };
} // ::qan

#endif // qanGraphicsResizer_h
