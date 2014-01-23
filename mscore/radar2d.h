//=============================================================================
//  MuseScore
//  Music Composition & Notation
//  $Id:$
//
//  Copyright (C) 2011 Werner Schweer and others
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License version 2
//  as published by the Free Software Foundation and appearing in
//  the file LICENSE.GPL
//=============================================================================

#ifndef __RADAR2D_H__
#define __RADAR2D_H__



class Radar2D : public QWidget {
      Q_OBJECT

      public:
            Radar2D(QWidget* parent = 0);
      
            void setPaths(QList<QPainterPath> p);
            void setEndpoints(QList<QPointF> p);
            
		virtual QSize sizeHint() const {return QSize(200,200); };
		virtual QSize minimumSizeHint() const {return sizeHint(); };
      protected:
            QList<QPainterPath> paths;
            QList<QPointF> endpoints;
      
            virtual void paintEvent(QPaintEvent *event);
     		virtual QTransform displayTransform() const;
      
      
      };
      
class Radar2DFakeSphere : public Radar2D {
      Q_OBJECT

      public:
            Radar2DFakeSphere(QWidget* parent = 0);
		virtual QSize sizeHint() const {return QSize(200,100); };
		virtual QSize minimumSizeHint() const {return sizeHint(); };
      protected:
     		virtual QTransform displayTransform() const;
      
      
      };

class Radar2DInputPolygon : public Radar2D {
    Q_OBJECT

	public:
		Radar2DInputPolygon(QWidget* parent = 0);
		
		void setPolygon(QPolygonF p) { _polygon = p; update(); };
		QPolygonF polygon() const { return _polygon; };
		
		virtual QSize sizeHint() const {return QSize(200,200); };
		virtual QSize minimumSizeHint() const {return sizeHint(); };
		
		// Indicate if the polygon should be closed on drawing
		bool _closePolygon;
		
	signals:
		void polygonChanged(QPolygonF p);	
		
	protected:
		QPolygonF _polygon;
		int _dragIndex;
		
        virtual void paintEvent(QPaintEvent *event);
    	virtual void mousePressEvent ( QMouseEvent * event );
    	virtual void mouseMoveEvent ( QMouseEvent * event );
    	virtual void mouseReleaseEvent ( QMouseEvent * event );
};


class Radar2DInputBezier : public Radar2DInputPolygon {
    Q_OBJECT

	public:
		Radar2DInputBezier(QWidget* parent = 0);
		
	protected:
		bool _dragMirrorHandle;
        virtual void paintEvent(QPaintEvent *event);
		
    	virtual void mousePressEvent ( QMouseEvent * event );
    	virtual void mouseMoveEvent ( QMouseEvent * event );
};


#endif

