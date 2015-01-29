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
// \file	qanController.h
// \author	benoit@qanava.org
// \date	2003 August 13
//-----------------------------------------------------------------------------


#ifndef qanController_h
#define qanController_h


// Qanava headers
#include "./qanNode.h"


// QT headers
#include <QObject>
#include <QAction>
#include <QGraphicsView>
#include <QGraphicsRectItem>
#include <QActionGroup>
#include <QSlider>
#include <QTouchEvent>


//-----------------------------------------------------------------------------
namespace qan { // ::qan

	class GraphView;

	//! Manage complex view modification operations in an AbstractItemView object (for example, panning, selection, etc.).
	/*!
		\nosubgrouping
	*/
	class Controller : public QObject
	{
		Q_OBJECT

	public:

		class Manager
		{
		public:

			Manager( QObject* parent ) : _actionGroup( new QActionGroup( parent ) ) { }

			virtual ~Manager( ) { }

			virtual bool	keyPressEvent( QKeyEvent* e );

			virtual bool	mousePressEvent( QMouseEvent* e );

			virtual bool	mouseReleaseEvent( QMouseEvent* e );

			virtual bool	mouseMoveEvent( QMouseEvent* e );

			virtual bool	mouseDoubleClickEvent( QMouseEvent* e );

			virtual	bool	wheelEvent( QWheelEvent* e );

            virtual	bool	touchEvent( QTouchEvent* e );

            void			registerController( Controller* controller );

			static bool		allItemsDisabled( QList< QGraphicsItem* >& items );

			QList< Controller* >	getControllers( ) { return _controllers; }

			Controller*				getController( QString name );

		private:

			QList< Controller* >	_controllers;

			QActionGroup*			_actionGroup;
		};

		Controller( QString name, GraphView& GraphView );

		virtual	~Controller( ) { }

        virtual bool	keyPressEvent( QKeyEvent* e ) { Q_UNUSED( e ); return false; }

        virtual bool	mousePressEvent( QMouseEvent* e ) { Q_UNUSED( e ); return false; }

        virtual bool	mouseReleaseEvent( QMouseEvent* e ) { Q_UNUSED( e ); return false; }

        virtual bool	mouseMoveEvent( QMouseEvent* e ) { Q_UNUSED( e ); return false; }

        virtual bool	mouseDoubleClickEvent( QMouseEvent* e ) { Q_UNUSED( e ); return false; }

        virtual	bool	wheelEvent( QWheelEvent* e ) { Q_UNUSED( e ); return false; }

        virtual	bool	touchEvent( QTouchEvent* e ) { Q_UNUSED( e ); return false; }

        QString			getName( ) { return _name; }

	protected:

		GraphView&		getGraphView( ) { return _graphView; }

	private:

		GraphView&			_graphView;

		QAction*			_action;

		QString				_name;

	protected:

		void				setAction( QAction* action ) { _action = action; }

	public:

		//! Get a custom action by name in this controller.
        virtual	QAction*	getAction( QString name ) { Q_UNUSED( name ); return 0; }

		//! Get a custom widget by name in this controller.
        virtual	QWidget*	getWidget( QString name ) { Q_UNUSED( name ); return 0; }

	public slots:

		QAction*			getAction( ) { return _action; }

	signals:

		//! Emmitted when the controller action (must) state changes.
		void				toggled( bool on );
	};



	//! .
	/*!
		\nosubgrouping
	*/
	class PanController : public Controller
	{
		Q_OBJECT

		/*! \name PanController Constructor/Destructor *///--------------------
		//@{
	public:

		//! PanController constructor with associed graphics view initialization.
		PanController( GraphView& graphView );

		//! PanController virtual destructor.
		virtual ~PanController( ) { }
		//@}
		//---------------------------------------------------------------------


		//! .
		enum State
		{
			NONE		= 0,
			KEYBOARD	= 1,
			PAN			= 2,
			PANNING		= 8,
		};

		/*! \name Keyboard Navigation Management *///--------------------------
		//@{
	public:

		//! .
		virtual bool	keyPressEvent( QKeyEvent* e );

		//! Enable or disable keyboard navigation (with arrow keys).
		void			setKeyboardNavigation( bool state );

		//! Get the current keyboard navigation state.
		bool			getKeyboardNavigation( ) const { return _keyboardNavigation; }

	private:

		//! Keyboard navigation state.
		bool			_keyboardNavigation;

		//! Keyboard navigation intensity.
		double			_keyboardNavigationIntensity;
		//@}
		//---------------------------------------------------------------------



		/*! \name Panning and Zooming Management *///--------------------------
		//@{
	public:

		virtual bool	mouseReleaseEvent( QMouseEvent* e );

	public:

		State			getMode( ) const { return _mode; }

		void			setMode( State mode ) { _mode = mode; }

	protected slots:

		void			toggled( bool state );

	private:

		State			_mode;
		//@}
		//---------------------------------------------------------------------
	};


	//! 
	/*!		
		\nosubgrouping
	*/
	class ZoomController : public Controller
	{
		Q_OBJECT

		/*! \name ZoomController Constructor/Destructor *///-------------------
		//@{
	public:

		//! ZoomController constructor with associed graphics view initialization.
		ZoomController( GraphView& GraphView );

		//! ZoomController virtual destructor.
		virtual ~ZoomController( ) { }
		//@}
		//---------------------------------------------------------------------


		//! .
		enum State
		{
			ZOOMRECT_NONE		= 0,
			ZOOMRECT			= 4,
			ZOOMRECT_ZOOMING	= 16
		};

		/*! \name Zoom Window Management *///----------------------------------
		//@{
	public:

		virtual bool	mousePressEvent( QMouseEvent* e );

		virtual bool	mouseReleaseEvent( QMouseEvent* e );

		virtual bool	mouseMoveEvent( QMouseEvent* e );

	public:

		State			getState( ) const { return _zoomRectState; }

		void			setState( State state ) { _zoomRectState = state; }

	private:

        //! Recursively look for an ancestor of a given type in an item paren hierarchy.
        QGraphicsItem*      lookForParent( QGraphicsItem* item, int type );

		//! .
		QPointF				_zoomRectStart;

		//! Geometric item used to model the dashed zoom selection window .
		QGraphicsRectItem*	_zoomRectItem;

		State				_zoomRectState;

		QAction*			_actionZoomRect;

		QAction*			_actionZoomNav;
		//@}
		//---------------------------------------------------------------------


		/*! \name Zoom In/Out Management *///----------------------------------
		//@{
	public:

		virtual	bool		wheelEvent( QWheelEvent* e );

        virtual	bool        touchEvent( QTouchEvent* e );

        virtual	QAction*	getAction( QString name );

		virtual	QWidget*	getWidget( QString name );

	protected slots:

		void				zoomIn( bool state );

		void				zoomOut( bool state );

		void				zoomSliderValueChanged( int value );

	private:

        qreal               _pinchZoomStart;

        double				_zoomMin;

		double				_zoomMax;

		void				updateZoomSlider( );

	private:

		QAction*			_actionZoomIn;

		QAction*			_actionZoomOut;

		QSlider*			_zoomSlider;
		//@}
		//---------------------------------------------------------------------
	};
} // ::qan
//-----------------------------------------------------------------------------


#endif // qanController_h

