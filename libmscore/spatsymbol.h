//=============================================================================
//  MuseScore
//  Music Composition & Notation
//  $Id$
//
//  Copyright (C) 2002-2011 Werner Schweer
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License version 2
//  as published by the Free Software Foundation and appearing in
//  the file LICENCE.GPL
//=============================================================================

#ifndef __SPATSYMBOL_H__
#define __SPATSYMBOL_H__

#include "symbol.h"
#include "sym.h"
#include "image.h"

class Painter;
class ChordRest;
class SpatCoordinates;
class SpatOSCMessage;


enum {
	SPATFAMILY_TRAJECTORY,
	SPATFAMILY_MODIFIER,
};

enum {
      SPATTYPE_END,
      
      SPATTYPE_POSITION_2D,
      SPATTYPE_POSITION_ALTERNATION,
      
      SPATTYPE_LINE_2D,
      SPATTYPE_LINE_CLOSED,
      
      SPATTYPE_POLYGON,
      SPATTYPE_POLYGON_CLOSED,
      SPATTYPE_POLYLINE,
      SPATTYPE_POLYLINE_CLOSED,
      
      SPATTYPE_CIRCLE_2D,
      SPATTYPE_CIRCLE_BF, // Back&Forth
      SPATTYPE_CIRCLE_SLINKY,
      SPATTYPE_CIRCLE_SLINKY_CLOSED,
      
      SPATTYPE_SPIRAL_IN2OUT,
      SPATTYPE_SPIRAL_OUT2IN,
      SPATTYPE_SPIRAL_IN2OUT_CLOSED,
      SPATTYPE_SPIRAL_OUT2IN_CLOSED,
      
      SPATTYPE_BEZIER_CUBIC,
      SPATTYPE_BEZIER_CUBIC_CLOSED,
      
      SPATTYPE_BEZIER_CUBIC_SPLINE,
      SPATTYPE_BEZIER_CUBIC_SPLINE_CLOSED,
      
      SPATTYPE_BERNOULLI_LEMNISCATE,
      
      SPATTYPE_OSC_MESSAGE,
      SPATTYPE_DUMMY_SYMBOL,
      
      SPATTYPE_MODIFIER_MARKOV,
      SPATTYPE_MODIFIER_SINE,
      SPATTYPE_MODIFIER_SAW,
      SPATTYPE_MODIFIER_SQUARE,
      SPATTYPE_MODIFIER_TRIANGLE,
      SPATTYPE_MODIFIER_END,
      
};

enum {
      ACCELERATION_ZERO = 0,
      ACCELERATION_CONSTANT = 1,
      ACCELERATION_EXPONENTIAL = 2,
};

enum SpatCircleDirection {
      DIRECTION_CLOCKWISE = 0,
      DIRECTION_COUNTERCLOCKWISE = 1,
};


//---------------------------------------------------------
//   Articulation
//    articulation marks
//---------------------------------------------------------

class SpatSymbol : public Element {
      Q_DECLARE_TR_FUNCTIONS(SpatSymbol)
      Q_PROPERTY(int duration      READ duration      WRITE setDuration)
      Q_PROPERTY(QPointF startPosition      READ startPosition      WRITE setStartPosition)
      Q_PROPERTY(QPointF stopPosition      READ stopPosition      WRITE setStopPosition)
      
      friend class Seq;
      friend class Radar;
      
 protected:
      QSvgRenderer* doc;
      void loadSymbol();
     
     QList<Element*> _leafs;

      
      /* Global Properties */
      
      int _spatType;
      int _duration; // in ticks
      bool _beingRendered; // Only used during rendering.
      
      
      /* Type Line 2D */
      QPointF _startPosition;
      QPointF _stopPosition;
      int _acceleration;
      qreal _startSpeed;
      
      /* Polygon */
      QPolygonF _vertices;
      
      
      /* Type: Circle 2D */
      
      QPointF _centerPosition;
      qreal _radius;
      int _direction;
      int _rotations;
      qreal _startAngle;
      qreal _stopAngle;
      
      /* spiral */
      qreal _innerRadius;
      
      /* Type: OSC message */
      QString _oscMessage1;  
      QString _oscMessage2;     
      
    /* Bezier */
    QPointF _controlPoint1;
    QPointF _controlPoint2;
    
    /* Markov modifier */
    QList<QPointF> _markovValues;
      
      
public:

	// wird von seq verwendet:
      QList<SpatSymbol*> _modifiers;
      SpatSymbol *_modifier_parent;


      SpatSymbol(Score*);
      SpatSymbol(const SpatSymbol& s);
      virtual ElementType type() const      { return SPATIALIZATION_SYMBOL; }
      virtual SpatSymbol* clone() const;
            
    const QList<Element*>& leafs() const { return _leafs; }
      QList<Element*>& leafs()             { return _leafs; }
      virtual void add(Element*);
      virtual void remove(Element*);
    virtual bool acceptDrop(MuseScoreView*, const QPointF&, Element*) const;
      virtual Element* drop(const DropData&);
      
    virtual QLineF dragAnchor() const;
    Segment* segment() const         { return (Segment*)parent(); }
    Measure* measure() const         { return (Measure*)parent()->parent(); }
      
      virtual SpatCoordinates _calculateCoordinates(int tick);
      virtual SpatCoordinates calculateCoordinates(int tick);
      virtual SpatOSCMessage oscMessage(int tick);
      virtual int tick();
      
      virtual void draw(QPainter* p) const;
      virtual void layout();
      virtual void write(Xml& xml) const;
      virtual void read(XmlReader& e);
      
      void resetSymbol(); // Gets called from seq::calculateDurations() - used to reset random paths
      
      int spatType() const { return _spatType; }
      void setSpatType(int t);
      QString spatTypeString() const;
      
      int spatFamily() const;
      
      void setDuration(int d) { _duration = d; }
      int duration() const;
      
      virtual QVariant getProperty(P_ID propertyId) const;
      virtual bool setProperty(P_ID propertyId, const QVariant& v);
      
      
      
      /* Line 2D */
      
      void setStartPosition(const QPointF& p) { _startPosition = p; }
      QPointF startPosition() const { return _startPosition; }
      
      void setStopPosition(const QPointF& p) { _stopPosition = p; }
      QPointF stopPosition() const { return _stopPosition; }
      
      void setAcceleration(int a) { _acceleration = a; }
      int acceleration() const { return _acceleration; }
      
      void setStartSpeed(qreal p) { _startSpeed = p; }
      qreal startSpeed() const { return _startSpeed; }
      
      /* Vertices */
      void setVertices(QPolygonF v) { _vertices = v; }
      QPolygonF vertices() const { return _vertices; }
      
      /* Type: Circle 2D */
      
      void setCenterPosition(const QPointF& p) { _centerPosition = p; }
      QPointF centerPosition() const {return _centerPosition; }
      
      void setRadius(qreal r) { _radius = r; }
      qreal radius() const { return _radius; }
      
      void setDirection(int d) { _direction = d; }
      int direction() const { return _direction; }
      
      void setRotations(int r) { _rotations = r; }
      int rotations() const { return _rotations; }
      
      void setStartAngle(qreal a) { _startAngle = a; }
      qreal startAngle() const { return _startAngle; }
      
      void setStopAngle(qreal a) { _stopAngle = a; }
      qreal stopAngle() const { return _stopAngle; }
      
      void setInnerRadius(qreal a) { _innerRadius = a; }
      qreal innerRadius() const { return _innerRadius; }
      
      /* Type: OSC Message */
      void setOSCMessage1(QString m) { _oscMessage1 = m; }
      QString oscMessage1() const { return _oscMessage1; }
      void setOSCMessage2(QString m) { _oscMessage2 = m; }
      QString oscMessage2() const { return _oscMessage2; }
      
      /* Bezier Curve */
      void setControlPoint1(QPointF p) { _controlPoint1 = p; };
      QPointF controlPoint1() const { return _controlPoint1; }
      void setControlPoint2(QPointF p) { _controlPoint2 = p; };
      QPointF controlPoint2() const { return _controlPoint2; }
      

      };


class SpatOSCMessage {
      public:
      SpatOSCMessage() { ignore = true;};
      
      void setOSCMessage(QString m) { message = m; };
      QString oscMessage() { return message; };
      
      bool ignore;
      
      protected:
      QString message;
};


class SpatCoordinates {
      public:
      bool operator == (const SpatCoordinates& c);
      SpatCoordinates const& operator+=(SpatCoordinates const& rhs);
      SpatCoordinates();
      SpatCoordinates(double x, double y);
      int channel;
      bool ignore;
      double r;
      double phi;
      double psi;
      
      virtual double x() const;
      double y() const;
      
};



 QDebug operator<<(QDebug dbg, const SpatCoordinates &c);


#endif

