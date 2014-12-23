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
// \file	qanGraphicsController.h
// \author	benoit@qanava.org
// \date	2014 December 20
//-----------------------------------------------------------------------------

#ifndef qanGraphicsController_h
#define qanGraphicsController_h

// QT headers
#include <QGraphicsItem>
#include <QGraphicsLayoutItem>
#include <QGraphicsDropShadowEffect>

// Qanava headers
// Nil

namespace qan { // ::qan

    class GraphicsResizer;

    //! Add minimize/maximize, close and develop/envelop buttons to graphics item trought a standard QGraphicsLayoutItem interface.
    /*!
        \nosubgrouping
    */
    class GraphicsController : public QGraphicsObject
    {
        Q_OBJECT
        friend class GraphicsResizer;

    public:
        /*! Define what kind of controls are shown by this controller, default is a close and maximize button   */
        enum Config {
            ShowClose = 2,
            ShowMaximize = 4,
            ShowExpand = 8,
            AutoHide = 16,
            ShowFocusShadow = 32,
            ShowBorder = 64,
            ShowCloseMaximize = ShowClose | ShowMaximize | AutoHide,
            ShowAll = ShowClose | ShowMaximize | ShowExpand | ShowFocusShadow | AutoHide,
            AlwaysShowAll = ShowClose | ShowMaximize | ShowExpand | ShowFocusShadow,
            AlwaysShowAllWithBorder = ShowClose | ShowMaximize | ShowExpand | ShowBorder | ShowFocusShadow
        };

        /*! \name GraphicsController Object Management *///-------------
        //@{
    public:
        GraphicsController( QGraphicsItem* target, QGraphicsLayoutItem* targetLayout, QSizeF buttonSize = QSizeF( 15., 15.), GraphicsController::Config config = ShowCloseMaximize );
        virtual ~GraphicsController( ) { }

        void    setControlColor( QColor controlColor ) { _controlColor = controlColor; }
        void    setFocusedInColor( QColor focusedInColor ) { _focusedInColor = focusedInColor; }
        void    setFocusedOutColor( QColor focusedOutColor ) { _focusedOutColor = focusedOutColor; }

    private:
        Q_DISABLE_COPY( GraphicsController );

    protected:
        //! Subclass to add user specific behaviour when minimized/maximized.
        virtual void            maximized( );
        virtual void            minimized( );
        virtual void            closed( );

        //! Resizer target (ie item that is resized by this resizer).
        QGraphicsItem*          _target;

        //! Target associed layout item (usually, target inherits from QGraphicsLayoutItem).
        QGraphicsLayoutItem*    _targetLayout;

        Config                  _config;
        QGraphicsDropShadowEffect*  _shadow;
        QColor                      _controlColor;

        QColor                  _focusedInColor;
        QColor                  _focusedOutColor;

    signals:

        void                    expanded( bool isExpanded );
        void                    focusedIn( );
        void                    focusedOut( );
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
        //! Set the decoration height height (with will be automatically adapted (default to 25.0).
        void            setGripSize( qreal buttonHeight ) { _buttonSize.setHeight( buttonHeight ); }

    protected:
        virtual bool    sceneEventFilter( QGraphicsItem* watched, QEvent* event );
        virtual void	hoverEnterEvent( QGraphicsSceneHoverEvent* e );
        virtual void    hoverMoveEvent( QGraphicsSceneHoverEvent* event );
        virtual void    hoverLeaveEvent( QGraphicsSceneHoverEvent* event );
        virtual void	mouseMoveEvent(QGraphicsSceneMouseEvent* event);
        virtual void	mousePressEvent(QGraphicsSceneMouseEvent* event);
        virtual void	mouseReleaseEvent(QGraphicsSceneMouseEvent* event);

        //! Force the controller to go to te minized state (target is not minimized), used internally for resizer/controller coordination.
        void            forceMinimzedState( );

        QRectF          maximizeBr( ) const;
        QRectF          closeBr( ) const;
        QRectF          expandBr( ) const;
        QSizeF          _buttonSize;

        bool            _showMaximize;
        bool            _hilightMaximize;
        bool            _isMaximized;
        QRectF          _minimizedGeom;

        bool            _showClose;
        bool            _hilightClose;

        bool            _showExpand;
        bool            _hilightExpand;
        bool            _isExpanded;
        //@}
        //-------------------------------------------------------------------------
    };

} // ::qan

#endif // qanGraphicsController_h
