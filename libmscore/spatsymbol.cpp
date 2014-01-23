
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

#include "spatsymbol.h"
#include "score.h"
#include "chordrest.h"
#include "system.h"
#include "measure.h"
#include "staff.h"
#include "stafftype.h"
#include "undo.h"
#include "segment.h"


//---------------------------------------------------------
//   add
//---------------------------------------------------------

void SpatSymbol::add(Element* e)
      {
      if (e->type() == SPATIALIZATION_MODIFIER) {
      	qDebug("SpatSymbol::add type %s\n", e->name());
      	
            e->setParent(this);
            _leafs.append(e);
            }
      else
            qDebug("SpatSymbol::add: unsupported type %s\n", e->name());
      }

//---------------------------------------------------------
//   remove
//---------------------------------------------------------

void SpatSymbol::remove(Element* e)
      {
      if (!_leafs.removeOne(e))
                  qDebug("SpatSymbol::remove: element <%s> not found\n", e->name());
      }



bool SpatSymbol::acceptDrop(MuseScoreView*, const QPointF&, Element* e) const
      {
      int type = e->type();
      return type == SPATIALIZATION_MODIFIER;
      }

//---------------------------------------------------------
//   drop
//---------------------------------------------------------

Element* SpatSymbol::drop(const DropData& data)
      {
      Element* e = data.element;
      if (e->type() == SPATIALIZATION_MODIFIER) {
                  e->setParent(this);
                  e->setTrack(0);
                  e->layout();
                  {
                  QPointF uo(data.pos - e->canvasPos() - data.dragOffset);
                  e->setUserOff(uo);
                  }
                  score()->undoAddElement(e);
                  return e;
                  
                  /*
            el->setParent(this);
            QPointF p = data.pos - pagePos() - data.dragOffset;
            el->setUserOff(p);
            score()->undoAddElement(el);
            return el;*/
            }
      else
            delete e;
      return 0;
      }

      
int SpatSymbol::spatFamily() const { 
	switch(_spatType) {
		case SPATTYPE_MODIFIER_MARKOV:
		case SPATTYPE_MODIFIER_SINE:
		case SPATTYPE_MODIFIER_SAW:
		case SPATTYPE_MODIFIER_TRIANGLE:
		case SPATTYPE_MODIFIER_SQUARE:
		case SPATTYPE_MODIFIER_END:
			return SPATFAMILY_MODIFIER;
	}
	return SPATFAMILY_TRAJECTORY;
}

SpatSymbol::SpatSymbol(Score* s) 
      : Element(s),
      _spatType(SPATTYPE_LINE_2D),
      _startPosition(0,0),
      _stopPosition(0,.0),
      _centerPosition(.0, .0),
      _radius(.4),
      _direction(DIRECTION_COUNTERCLOCKWISE),
      _rotations(0),
      _startAngle(0.0),
      _stopAngle(0.0),
      _innerRadius(0.0),
      _oscMessage1(""),
      _oscMessage2(""),
      _modifier_parent(NULL),
      _modifiers(),
_vertices(),
_markovValues(),
      
      doc(NULL)
      
{ 
      setFlags(ELEMENT_MOVABLE | ELEMENT_SELECTABLE);  
}

SpatSymbol::SpatSymbol(const SpatSymbol& s) 
      : Element(s), 
      _spatType(s.spatType()),
      _duration(4.0), 
      _vertices(s.vertices()),
      
      _startPosition(s.startPosition()),
      _stopPosition(s.stopPosition()),
      _acceleration(s.acceleration()),
      _startSpeed(s.startSpeed()),
      
      _centerPosition(s.centerPosition()),
      _radius(s.radius()),
      _direction(s.direction()),
      _rotations(s.rotations()),
      _startAngle(s.startAngle()),
      _stopAngle(s.stopAngle()),
      
      _innerRadius(s.innerRadius()),
      
      _oscMessage1(s.oscMessage1()),
      _oscMessage2(s.oscMessage2()),
      
      _controlPoint1(s.controlPoint1()),
      _controlPoint2(s.controlPoint2()),
_modifier_parent(NULL),
_modifiers(),
_markovValues(),
            
      doc(NULL)
{
    
    
    foreach(Element* e, s._leafs) {
        Element* ee = e->clone();
        ee->setParent(this);
        _leafs.append(ee);
    }

}


QString SpatSymbol::spatTypeString() const {
      switch(_spatType) {
                  
            case SPATTYPE_END:
            	return "End";
                  
            case SPATTYPE_POSITION_2D:
            	return "Position";
                break;
                  
            case SPATTYPE_POSITION_ALTERNATION:
            	return "Position Alternation";
                break;
                  
            case SPATTYPE_LINE_2D:
            	return "Line open";
                break;
                  
            case SPATTYPE_LINE_CLOSED:
            	return "Line closed";
                break;
                  
            case SPATTYPE_POLYGON:
            	return "Polygon";
                break;
                  
            case SPATTYPE_POLYGON_CLOSED:
            	return "Polygon closed";
                break;
                  
            case SPATTYPE_POLYLINE:
            	return "Polyline";
                break;
                  
            case SPATTYPE_POLYLINE_CLOSED:
            	return "Polyline closed";
                break;
      
            case SPATTYPE_CIRCLE_2D:
            	return "Circle open";
                break;
      
            case SPATTYPE_CIRCLE_BF:
            	return "Circle closed";
                break;
      
            case SPATTYPE_CIRCLE_SLINKY:
            	return "Slinky";
                break;
      
            case SPATTYPE_CIRCLE_SLINKY_CLOSED:
            	return "Slinky closed";
                break;
      
            case SPATTYPE_SPIRAL_IN2OUT:
            	return "Spiral in->out";
                break;
      
            case SPATTYPE_SPIRAL_OUT2IN:
            	return "Spiral out->in";
                break;
      
            case SPATTYPE_SPIRAL_IN2OUT_CLOSED:
            	return "Spiral in->out closed";
                break;
      
            case SPATTYPE_SPIRAL_OUT2IN_CLOSED:
            	return "Spiral out->in closed";
                break;
      
            case SPATTYPE_BEZIER_CUBIC: 
            	return "Cubic Bezier";
                break;
      
            case SPATTYPE_BEZIER_CUBIC_CLOSED: 
            	return "Cubic Bezier closed";
                break;
      
            case SPATTYPE_BEZIER_CUBIC_SPLINE: 
            	return "Cubic Bezier spline";
                break;
      
            case SPATTYPE_BEZIER_CUBIC_SPLINE_CLOSED: 
            	return "Cubic Bezier spline closed";
                break;
      
            case SPATTYPE_BERNOULLI_LEMNISCATE: 
            	return "Lemniscate of Bernoulli";
                break;
                
      
            case SPATTYPE_OSC_MESSAGE: 
            	return "OSC Message";
                break;
                
            case SPATTYPE_DUMMY_SYMBOL:
            	return _oscMessage2;
            	
            case SPATTYPE_MODIFIER_MARKOV:
            	return "Random Modifier";
            	
            case SPATTYPE_MODIFIER_SINE:
            	return "Sine Modifier";
            	
            case SPATTYPE_MODIFIER_SAW:
            	return "Saw Modifier";
            	
            case SPATTYPE_MODIFIER_TRIANGLE:
            	return "Triangle Modifier";
            	
            case SPATTYPE_MODIFIER_SQUARE:
            	return "Square Modifier";
            	
            case SPATTYPE_MODIFIER_END:
            	return "End Modifier";
        }
	return "Error";
}

void SpatSymbol::setSpatType(int t) { 
      _spatType = t;
      doc = NULL;	
}


int SpatSymbol::duration() const{
      return _duration;
}

QLineF SpatSymbol::dragAnchor() const
      {
      qreal xp = 0.0;
      for (Element* e = parent(); e; e = e->parent())
            xp += e->x();
      qreal yp = measure()->system()->staffY(staffIdx());
      QPointF p(xp, yp);
      return QLineF(p, canvasPos());
      }
     
void SpatSymbol::loadSymbol() {

      QString path = ":/data/spatialization/";
      switch(_spatType) {
                  
            case SPATTYPE_END:
            	path += "spat_end.svg";
                break;
                  
            case SPATTYPE_POSITION_2D:
            	path += "spat_fix.svg";
                break;
                  
            case SPATTYPE_POSITION_ALTERNATION:
            	path += "spat_alternate.svg";
                break;
                  
            case SPATTYPE_LINE_2D:
            	path += "spat_line.svg";
                break;
                  
            case SPATTYPE_LINE_CLOSED:
            	path += "spat_line_bf.svg";
                break;
                  
            case SPATTYPE_POLYGON:
            	path += "spat_polygon.svg";
                break;
                  
            case SPATTYPE_POLYGON_CLOSED:
            	path += "spat_polygon_bf.svg";
                break;
                  
            case SPATTYPE_POLYLINE:
            	path += "spat_polyline.svg";
                break;
      
            case SPATTYPE_POLYLINE_CLOSED:
            	path += "spat_polyline_bf.svg";
                break;
                
            case SPATTYPE_CIRCLE_2D:
            	path += "spat_circle.svg";
            	break;

            case SPATTYPE_CIRCLE_BF:
            	path += "spat_circle_bf.svg";
                break;
      
            case SPATTYPE_CIRCLE_SLINKY:
            	path += "spat_slinky.svg";
                break;
      
            case SPATTYPE_CIRCLE_SLINKY_CLOSED:
            	path += "spat_slinky_bf.svg";
                break;
      
            case SPATTYPE_SPIRAL_IN2OUT:
            	path += "spat_spiral_in.svg";
                break;
      
            case SPATTYPE_SPIRAL_IN2OUT_CLOSED:
            	path += "spat_spiral_in_bf.svg";
                break;
      
            case SPATTYPE_SPIRAL_OUT2IN:
            	path += "spat_spiral_out.svg";
                break;
      
            case SPATTYPE_SPIRAL_OUT2IN_CLOSED:
            	path += "spat_spiral_out_bf.svg";
                break;
      
            case SPATTYPE_BEZIER_CUBIC:
            	path += "spat_curve.svg";
                break;
      
            case SPATTYPE_BEZIER_CUBIC_CLOSED:
            	path += "spat_curve_bf.svg";
                break;
      
            case SPATTYPE_BEZIER_CUBIC_SPLINE:
            	path += "spat_bezierspline.svg";
                break;
      
            case SPATTYPE_BEZIER_CUBIC_SPLINE_CLOSED:
            	path += "spat_bezierspline_bf.svg";
                break;
      
            case SPATTYPE_BERNOULLI_LEMNISCATE: 
            	path += "spat_8.svg";
                break;
      
            case SPATTYPE_OSC_MESSAGE: 
            	path += "osc.svg";
                break;
            
            case SPATTYPE_DUMMY_SYMBOL:
            	path += _oscMessage1;
            	break;
            	
            case SPATTYPE_MODIFIER_MARKOV:
            	path += "mod_rand.svg";
            	break;
            	
            case SPATTYPE_MODIFIER_SINE:
            	path += "mod_sin.svg";
            	break;
            	
            case SPATTYPE_MODIFIER_SAW:
            	path += "mod_saw.svg";
            	break;
            	
            case SPATTYPE_MODIFIER_TRIANGLE:
            	path += "mod_tri.svg";
            	break;
            	
            case SPATTYPE_MODIFIER_SQUARE:
            	path += "mod_sqr.svg";
            	break;
            	
            case SPATTYPE_MODIFIER_END:
            	path += "mod_end.svg";
            	break;
      }
      
      QFile f(path);
      if (!f.open(QIODevice::ReadOnly)) {
      		qDebug() << "Cannot read symbol at path: " + path;
            return;
    	}
      QByteArray ba = f.readAll();
      f.close();
      
      doc = new QSvgRenderer(ba);
} 

void SpatSymbol::draw(QPainter* painter) const {

	if(spatType() == SPATTYPE_OSC_MESSAGE && _oscMessage1.length() != 0) {
	
		painter->setFont(QFont("Courier", 10));
		painter->setPen(Qt::black);
		if(selected()) {
			painter->setPen(Qt::blue);
		}
        painter->drawText(0, 0, _oscMessage1);
        return;
	}
	

      if (!doc) {
            painter->setBrush(Qt::NoBrush);
            painter->setPen(Qt::black);
            painter->drawRect(bbox());
            painter->drawLine(0.0, 0.0, bbox().width(), bbox().height());
            painter->drawLine(bbox().width(), 0.0, 0.0, bbox().height());
            }
      else{
      		doc->render(painter, bbox());
    	}
      if (selected() && !(score() && score()->printing())) {
            painter->setBrush(Qt::NoBrush);
            painter->setPen(Qt::blue);
            painter->drawRect(bbox());
            }
}

void SpatSymbol::layout() {

   	// Make sure that the grapic is loaded.
   		if(!doc) {
			loadSymbol();
   		}
   		
			
		if (doc && doc->isValid()) {
       		QSizeF size = doc->defaultSize(); 
       		qreal f = parent() ? .7 : .5;
       		adjustReadPos();
			bbox().setRect(0.0, 0.0, size.width() * f , size.height() * f);
	  	}
   		
}

SpatSymbol* SpatSymbol::clone() const   { 
      return new SpatSymbol(*this); 
      }
      
      



void SpatSymbol::write(Xml& xml) const {
      xml.stag(name());
      Element::writeProperties(xml);
      
      switch(spatType()) {
                  
            case SPATTYPE_END: 
                  xml.tag("spattype", "end");
                  break;
                  
            case SPATTYPE_POSITION_2D: 
                  xml.tag("spattype", "position 2d");
                  break;
                  
            case SPATTYPE_POSITION_ALTERNATION: 
                  xml.tag("spattype", "position alternation");
                  break;
                  
            case SPATTYPE_LINE_2D: 
                  xml.tag("spattype", "line 2d");
                  break;
                  
            case SPATTYPE_LINE_CLOSED: 
                  xml.tag("spattype", "line closed");
                  break;
                  
            case SPATTYPE_POLYGON: 
                  xml.tag("spattype", "polygon");
                  break;
                  
            case SPATTYPE_POLYGON_CLOSED: 
                  xml.tag("spattype", "polygon closed");
                  break;
                  
            case SPATTYPE_POLYLINE: 
                  xml.tag("spattype", "polyline");
                  break;
                  
            case SPATTYPE_POLYLINE_CLOSED: 
                  xml.tag("spattype", "polyline closed");
                  break;
      
            case SPATTYPE_CIRCLE_2D: 
                  xml.tag("spattype", "circle 2d");
                  break;
      
            case SPATTYPE_CIRCLE_BF: 
                  xml.tag("spattype", "circle bf");
                  break;
      
            case SPATTYPE_CIRCLE_SLINKY: 
                  xml.tag("spattype", "circle slinky");
                  break;
      
            case SPATTYPE_CIRCLE_SLINKY_CLOSED: 
                  xml.tag("spattype", "circle slinky closed");
                  break;
      
            case SPATTYPE_SPIRAL_IN2OUT: 
                  xml.tag("spattype", "spiral in2out");
                  break;
      
            case SPATTYPE_SPIRAL_OUT2IN: 
                  xml.tag("spattype", "spiral out2in");
                  break;
      
            case SPATTYPE_SPIRAL_IN2OUT_CLOSED: 
                  xml.tag("spattype", "spiral in2out closed");
                  break;
      
            case SPATTYPE_SPIRAL_OUT2IN_CLOSED: 
                  xml.tag("spattype", "spiral out2in closed");
                  break;
      
            case SPATTYPE_BEZIER_CUBIC:
                  xml.tag("spattype", "bezier cubic");
                  break;
      
            case SPATTYPE_BEZIER_CUBIC_CLOSED:
                  xml.tag("spattype", "bezier cubic closed");
                  break;
      
            case SPATTYPE_BEZIER_CUBIC_SPLINE:
                  xml.tag("spattype", "bezier cubic spline");
                  break;
              
            case SPATTYPE_BEZIER_CUBIC_SPLINE_CLOSED:
                  xml.tag("spattype", "bezier cubic spline closed");
                  break;
      
            case SPATTYPE_BERNOULLI_LEMNISCATE:
                  xml.tag("spattype", "bernoulli lemniscate");
                  break;
      
            case SPATTYPE_OSC_MESSAGE: 
                  xml.tag("spattype", "osc message");
                  break;
      
            case SPATTYPE_DUMMY_SYMBOL: 
                  xml.tag("spattype", "dummy symbol");
                  break;
                  
            case SPATTYPE_MODIFIER_MARKOV: 
                  xml.tag("spattype", "modifier markov");
                  break;
                  
            case SPATTYPE_MODIFIER_SINE: 
                  xml.tag("spattype", "modifier sine");
                  break;
                  
            case SPATTYPE_MODIFIER_SAW: 
                  xml.tag("spattype", "modifier saw");
                  break;
                  
            case SPATTYPE_MODIFIER_TRIANGLE: 
                  xml.tag("spattype", "modifier triangle");
                  break;
                  
            case SPATTYPE_MODIFIER_SQUARE: 
                  xml.tag("spattype", "modifier square");
                  break;
                  
            case SPATTYPE_MODIFIER_END: 
                  xml.tag("spattype", "modifier end");
                  break;
      }
      
      xml.tag("duration", _duration);
      
      xml.tag("start_position", _startPosition);
      xml.tag("stop_position", _stopPosition);
      
      xml.stag("vertices");
      QPolygonF v = vertices();
      for(int i=0; i<v.size(); i++) {
      	xml.tag("vertex", v.at(i));
      }
      xml.etag(); // vertices
      
      switch (acceleration()) {
            case ACCELERATION_ZERO:
                  xml.tag("acceleration", "zero");
                  break;
                  
            case ACCELERATION_CONSTANT:
                  xml.tag("acceleration", "constant");
                  break;
                  
            case ACCELERATION_EXPONENTIAL:
                  xml.tag("acceleration", "exponential");
                  break;
            
  //          default:
  //                qDebug() << "Undefined acceleration(): " + acceleration();
      }
      
      xml.tag("start_speed", startSpeed());
      
      
      // Properties of Circle 2D
      xml.tag("center_position", _centerPosition);
      
      xml.tag("radius", _radius);
      xml.tag("direction", ( _direction == DIRECTION_CLOCKWISE ? "cw" : "ccw") );
      xml.tag("rotations", rotations());
      xml.tag("start_angle", _startAngle);
      xml.tag("stop_angle", _stopAngle);
      
      xml.tag("inner_radius", _innerRadius);
      
      xml.tag("osc_message1", _oscMessage1); 
      xml.tag("osc_message2", _oscMessage2);
      
      xml.tag("control_point_1", _controlPoint1);
      xml.tag("control_point_2", _controlPoint2); 
      
      
      
      xml.etag();
      }


void SpatSymbol::read(XmlReader& e)
{
      
      while (e.readNextStartElement()) {
            const QStringRef& tag(e.name());
            
            if (tag == "vertices") {
            	_vertices = QPolygonF();
            	e.readNext();
            	while(!(e.tokenType() ==QXmlStreamReader::EndElement
            	  && e.name() == "vertices")) {
                    if(e.name() != "vertex" && e.tokenType() != QXmlStreamReader::StartElement) {
                        e.readNext();
                        continue;
                    }
            		_vertices.append(e.readPoint());
            		e.readNext();
            	}
            }
            
            if (tag == "spattype") {
        		QString val(e.readElementText());
                  if(val == "end") {
                        setSpatType(SPATTYPE_END);
                  } else if(val == "position 2d") {
                        setSpatType(SPATTYPE_POSITION_2D);
                  } else if(val == "position alternation") {
                        setSpatType(SPATTYPE_POSITION_ALTERNATION);
                  } else if(val == "line 2d") {
                        setSpatType(SPATTYPE_LINE_2D);
                  } else if(val == "line closed") {
                        setSpatType(SPATTYPE_LINE_CLOSED);
                  } else if(val == "polygon") {
                        setSpatType(SPATTYPE_POLYGON);
                  } else if(val == "polygon closed") {
                        setSpatType(SPATTYPE_POLYGON_CLOSED);
                  } else if(val == "polyline") {
                        setSpatType(SPATTYPE_POLYLINE);
                  } else if(val == "polyline closed") {
                        setSpatType(SPATTYPE_POLYLINE_CLOSED);
                  } else if(val == "circle 2d") {
                        setSpatType(SPATTYPE_CIRCLE_2D);
                  } else if(val == "circle bf") {
                        setSpatType(SPATTYPE_CIRCLE_BF);
                  } else if(val == "circle slinky") {
                        setSpatType(SPATTYPE_CIRCLE_SLINKY);
                  } else if(val == "circle slinky closed") {
                        setSpatType(SPATTYPE_CIRCLE_SLINKY_CLOSED);
                  } else if(val == "spiral in2out") {
                        setSpatType(SPATTYPE_SPIRAL_IN2OUT);
                  } else if(val == "spiral out2in") {
                        setSpatType(SPATTYPE_SPIRAL_OUT2IN);
                  } else if(val == "spiral in2out closed") {
                        setSpatType(SPATTYPE_SPIRAL_IN2OUT_CLOSED);
                  } else if(val == "spiral out2in closed") {
                        setSpatType(SPATTYPE_SPIRAL_OUT2IN_CLOSED);
                  } else if(val == "bezier cubic") {
                        setSpatType(SPATTYPE_BEZIER_CUBIC);
                  } else if(val == "bezier cubic closed") {
                        setSpatType(SPATTYPE_BEZIER_CUBIC_CLOSED);
                  } else if(val == "bezier cubic spline") {
                        setSpatType(SPATTYPE_BEZIER_CUBIC_SPLINE);
                  } else if(val == "bezier cubic spline closed") {
                        setSpatType(SPATTYPE_BEZIER_CUBIC_SPLINE_CLOSED);
                  } else if(val == "bernoulli lemniscate") {
                        setSpatType(SPATTYPE_BERNOULLI_LEMNISCATE);
                  } else if(val == "osc message") {
                        setSpatType(SPATTYPE_OSC_MESSAGE);
                  } else if(val == "dummy symbol") {
                        setSpatType(SPATTYPE_DUMMY_SYMBOL);
                  } else if(val == "modifier markov") {
                        setSpatType(SPATTYPE_MODIFIER_MARKOV);
                  } else if(val == "modifier sine") {
                        setSpatType(SPATTYPE_MODIFIER_SINE);
                  } else if(val == "modifier square") {
                        setSpatType(SPATTYPE_MODIFIER_SQUARE);
                  } else if(val == "modifier triangle") {
                        setSpatType(SPATTYPE_MODIFIER_TRIANGLE);
                  } else if(val == "modifier saw") {
                        setSpatType(SPATTYPE_MODIFIER_SAW);
                  } else if(val == "modifier end") {
                        setSpatType(SPATTYPE_MODIFIER_END);
                  }else{
                        qDebug() << "Unknown spattype: " << val;
                        setSpatType(SPATTYPE_LINE_2D);
                  }
            
            } else if (tag == "duration") {
        		  QString val(e.readElementText());
                  setDuration(val.toFloat());
            
            /* Line 2D */
            } else if (tag == "start_position") {
                  _startPosition = e.readPoint();
            } else if (tag == "stop_position") {
                  _stopPosition = e.readPoint();
            } else if (tag == "acceleration") {
        		  QString val(e.readElementText());
                  if(val == "zero") { setAcceleration(ACCELERATION_ZERO); }
                  if(val == "constant") { setAcceleration(ACCELERATION_CONSTANT); }
                  if(val == "exponential") { setAcceleration(ACCELERATION_EXPONENTIAL); }
            } else if (tag == "acceleration_parameter") {
        		  QString val(e.readElementText());
                  setStartSpeed(val.toFloat());
            }else if (tag == "start_speed") {
        		  QString val(e.readElementText());
                  setStartSpeed(val.toFloat());
                  
            /* Circle 2D */
            } else if (tag == "center_position") {
                  _centerPosition = e.readPoint();
            } else if (tag == "radius") {
        		  QString val(e.readElementText());
                  setRadius(val.toFloat());
            } else if (tag == "rotations") {
        		  QString val(e.readElementText());
                  setRotations(val.toFloat());
            } else if (tag == "direction") {
        		  QString val(e.readElementText());
                  setDirection((val== "cw" ? DIRECTION_CLOCKWISE : DIRECTION_COUNTERCLOCKWISE));
            } else if (tag == "start_angle") {
        		  QString val(e.readElementText());
                  setStartAngle(val.toFloat());
            } else if (tag == "stop_angle") {
        		  QString val(e.readElementText());
                  setStopAngle(val.toFloat());
                  
            } else if (tag == "inner_radius") {
        		  QString val(e.readElementText());
                  setInnerRadius(val.toFloat());
                  
        	/* Bezier */
            } else if (tag == "control_point_1") {
                  _controlPoint1 = e.readPoint();
            } else if (tag == "control_point_2") {
                  _controlPoint2 = e.readPoint();
                  
            /* osc message */
            } else if (tag == "osc_message1") {
        		  QString val(e.readElementText());
                  setOSCMessage1(val);
            } else if (tag == "osc_message2") {
        		  QString val(e.readElementText());
                  setOSCMessage2(val);
                  
            } else if (!Element::readProperties(e)) {
            //      domError(e);
            }
      }
      
}

SpatOSCMessage SpatSymbol::oscMessage(int absoluteTick) {
      SpatOSCMessage m;
      if(spatType() != SPATTYPE_OSC_MESSAGE) {
            m.ignore = TRUE;
            return m;
      }
      // We have to check, if the symbol should be rendered.
      if (!(_beingRendered && absoluteTick >= this->tick())) {
            // playback ist ausserhalb des Wirkungsbereich des Symbols
            
            m.ignore = true;
            return m;
      }
      _beingRendered = false;
      m.ignore =false;
      
      m.setOSCMessage(_oscMessage1);
      return m;
}


void SpatSymbol::resetSymbol() {
	// Reset the markov-values:
	_markovValues.clear();
}
        
SpatCoordinates SpatSymbol::calculateCoordinates(int absoluteTick) {
	if(spatFamily() == SPATFAMILY_TRAJECTORY) {
        
		SpatCoordinates c = _calculateCoordinates(absoluteTick);
        
        // We have to calculate the rotation of the local coordinate system
		QTransform t;
		SpatCoordinates liminf = _calculateCoordinates(qMax(absoluteTick-10, tick()));
		SpatCoordinates limsup = _calculateCoordinates(qMin(absoluteTick+10, tick()+duration()));
		if(!(liminf == limsup) && _modifiers.size() > 0 ) {
			QLineF l(QPointF(liminf.x(), liminf.y()), QPointF(limsup.x(), limsup.y()) );
			l = l.normalVector();
			t = t.rotate(-l.angle() + 90);
		}
        
		foreach(SpatSymbol *s, _modifiers) {
            // We have to control, which modifier is allowed to interact
            if(absoluteTick >= s->tick() + s->duration() || absoluteTick < s->tick()) {
                continue;
            }
            
            // Transform the coordinate
            SpatCoordinates a = s->_calculateCoordinates(absoluteTick);
            QPointF p(a.x(), a.y());
            p = t.map(p);
            a.r = pow(pow(p.x(),2)+pow(p.y(),2), 0.5);
            a.phi = atan2(p.y(),p.x());
            
			c += a;
		}
		return c;
	}
	if(spatFamily() == SPATFAMILY_MODIFIER && _modifier_parent) {
		SpatCoordinates c;
		if(absoluteTick >= tick() + duration() || absoluteTick < tick()) {
			c.ignore = true;
			return c;
		}
		c = _modifier_parent->calculateCoordinates(absoluteTick);
		return c;
	}
	
	SpatCoordinates c;
	c.ignore = true;
	return c;
}

// Returns a value between 0 and 1
float spatRand() {
    return ((float)qrand())/RAND_MAX;
}
            

// Returns either -1, 0, 1 such that P[X_(n+1)=1|X_n=x] = max - |x|  
int spatRandMarkov(float value, float max, float p0) {
    float r = spatRand();
    if(r < p0)
        return 0;
    // r, r iid: just generate a new one to find out if we're going up or down
    r = spatRand();
    
    // in case value>0:
    float pUp = .5 - pow(qAbs(value)/max, 5)/2;
    if(value > 0) {
        return (r < pUp ? 1 : -1);
    }
    // case value<=0; just invert up and down
    return (r < pUp ? -1 : 1);
}

QPointF spatBoundQPointF(QPointF point, QPointF bound) {
	QPointF p;
	p.setX( qMax(-bound.x(), qMin(bound.x(), point.x()) ) );
	p.setY( qMax(-bound.y(), qMin(bound.y(), point.y()) ) );
	return p;
}

SpatCoordinates SpatSymbol::_calculateCoordinates(int absoluteTick) {

      SpatCoordinates c;
      c.channel = track()/VOICES + 1;
    c.ignore = false;
      
      // We have just an osc-message and therefore we always ignore.
      if(spatType() == SPATTYPE_END || spatType() == SPATTYPE_OSC_MESSAGE) {
            c.ignore = true;
            return c;
      }
      
      int tick = absoluteTick - this->tick();
      int duration = this->duration();
      qreal ratio = 1.0;
      if(duration) {
            ratio = tick * 1.0 / duration;
      }
      
      // We have to check, if the symbol should be rendered.
      // CAUTION/TODO:
      // If two symbols are following each other, this solution might cause problems, because
      // the old symbol has one interference with the new one.
      // Solution: Add a 'priority' flag to SpatCoordinates and reduce the priority of this message.
      // Before the messages are sent, it can be validated to have only one message per channel.
      if(tick >= 0 && tick < duration) {
            // Normal state
            _beingRendered = (tick != duration);
      }else if (_beingRendered && tick > duration) {
            // The symbol has just ended. Make sure, that the trajectory is at it's final location.
            _beingRendered = false;
            
            // Assume, that we are exactly at time 1:
            absoluteTick = this->tick() + duration;
            tick = duration;
            ratio = 1.0;
      }else{
            // playback ist ausserhalb des Wirkungsbereich des Symbols
            
            c.ignore = true;
            return c;
      }

      switch(spatType()) {
      
      		case SPATTYPE_MODIFIER_MARKOV: 
      		{
      			// Check, if the values are already calculated
      			// It's constructed like a brownian bridge...
                
                // The computation is reduced by using a resolution by forcing at minimum
                // every 5 ticks a new position.
                int resolution = 5;
                int controlPoints = duration/resolution;
                
      			if(_markovValues.size() != controlPoints) {
                    
      				QList<QPointF> m1, m2;
      				QPointF l1, l2;
      				
      				// Internally, we misuse the _start and _stopPosition for smoothness and maxDistance.
                    QPointF smoothness = _startPosition;
                    QPointF maxDist = _stopPosition;
                    float jumpSize = _radius;
                    
      				for(int i=0;i<controlPoints;i++) {
      					m1.append(l1);
      					l1 += QPointF(
                            spatRandMarkov(l1.x(),maxDist.x(), 1.-pow(2,-100*smoothness.x())),
                            spatRandMarkov(l1.y(),maxDist.y(), 1.-pow(2,-100*smoothness.y())) 
                            )*jumpSize;
                        l1 = spatBoundQPointF(l1, maxDist);
                        
      					m2.append(l2);
      					l2 += QPointF(
                                      spatRandMarkov(l2.x(),maxDist.x(), 1.-pow(2,-100*smoothness.x())),
                                      spatRandMarkov(l2.y(),maxDist.y(), 1.-pow(2,-100*smoothness.y())) 
                                      )*jumpSize;
                        l2 = spatBoundQPointF(l2, maxDist);
      				}
      				
      				_markovValues.clear();
      				for(int i=0;i<controlPoints;i++) {
      					float ratio = ((float)i)/controlPoints;
      					_markovValues.append(m1[i]*(1. - ratio) + m2[controlPoints-1-i]*ratio);
      				}
      			}
                
      		
      			  QPointF p = _markovValues[tick/resolution];
                  c.r = pow(pow(p.x(),2)+pow(p.y(),2), 0.5);
                  c.phi = atan2(p.y(),p.x());
                  c.psi = 0;
                  
                  return c;
      		}break;
      
      		case SPATTYPE_MODIFIER_SINE: 
      		{
      		QPointF p(0, radius()*sin(ratio*(M_PI*2*rotations())));
                  c.r = pow(pow(p.x(),2)+pow(p.y(),2), 0.5);
                  c.phi = atan2(p.y(),p.x());
                  c.psi = 0;
                  
                  return c;
      		}break;
      
      		case SPATTYPE_MODIFIER_SAW: 
      		{
      		      float percentage = ratio;
                  percentage *= rotations() ;
                  while(percentage > 1.) {
                    percentage -= 1.;
                  }
                  
      			QPointF p;
      			p.setY(radius()*(-1.+2.*radius()*percentage));
      			
                  c.r = pow(pow(p.x(),2)+pow(p.y(),2), 0.5);
                  c.phi = atan2(p.y(),p.x());
                  c.psi = 0;
                  
                  return c;
      		}break;
      
      		case SPATTYPE_MODIFIER_END: 
      		{
      			c.ignore = true;
                  return c;
      		}break;
      
      		case SPATTYPE_MODIFIER_TRIANGLE: 
      		{
      		      float percentage = ratio;
                  percentage *= rotations();
                  while(percentage > 1.) {
                    percentage -= 1.;
                  }
                  
      			QPointF p;
      			if(percentage < .5) {
      				p.setY(radius()*percentage*2);
      			}else{
      				p.setY(radius()*(1.-percentage)*2);
      			}
      			
                  c.r = pow(pow(p.x(),2)+pow(p.y(),2), 0.5);
                  c.phi = atan2(p.y(),p.x());
                  c.psi = 0;
                  
                  return c;
      		}break;
      
      		case SPATTYPE_MODIFIER_SQUARE: 
      		{
      		      float percentage = ratio;
                  percentage *= rotations();
                  while(percentage > 1.) {
                    percentage -= 1.;
                  }
                  
      			QPointF p;
      			if(percentage == 0 || percentage == 1) {
      				p.setY(0);
      			} else if(percentage < .5) {
      				p.setY(radius());
      			}else{
      				p.setY(-radius());
      			}
      			
                  c.r = pow(pow(p.x(),2)+pow(p.y(),2), 0.5);
                  c.phi = atan2(p.y(),p.x());
                  c.psi = 0;
                  
                  return c;
      		}break;
      		
            case SPATTYPE_POSITION_2D:
            {
                  QPointF p = centerPosition();
                  
                  c.r = pow(pow(p.x(),2)+pow(p.y(),2), 0.5);
                  c.phi = atan2(p.y(),p.x());
                  c.psi = 0;
                  
                  return c;
                  
            }break;
      
            case SPATTYPE_LINE_2D:
            {
                  // startSpeed ist meter/viertel, wir benötigen aber meter/tick
                  float startSpeed = this->startSpeed() / 480.0;
                  
                  // aktuell wird nur noch konstante geschwindigkeit unterstützt
                  float percentage = ratio;
                  /*
                  switch(acceleration()) {
                        case ACCELERATION_ZERO:
                              percentage = ratio;
                              break;
                              
                        case ACCELERATION_CONSTANT:
                        {
                              float a = 2.0*(1.0 - startSpeed*duration)/duration/duration;
                              percentage = .5*a*tick*tick + startSpeed*tick;
                        }
                              break;
                              
                        case ACCELERATION_EXPONENTIAL:
                              percentage = 1.0/(qExp(1.0)- 1.0)*(qExp(ratio)-1.0);
                              break;
                  }
                  */
                  
                  percentage *= rotations() + 1;
                while(percentage > 1.) {
                    percentage -= 1.;
                }
                  
                  QPointF p = startPosition()*(1.0 - percentage) + stopPosition()*percentage;
                  
                  c.r = pow(pow(p.x(),2)+pow(p.y(),2), 0.5);
                  c.phi = atan2(p.y(),p.x());
                  c.psi = 0;
                  
                  return c;
                  
            }break;
            
            
            case SPATTYPE_LINE_CLOSED:
            {
                  // startSpeed ist meter/viertel, wir benötigen aber meter/tick
                  float startSpeed = this->startSpeed() / 480.0;
                  
                  // aktuell wird nur noch konstante geschwindigkeit unterstützt
                  float percentage = ratio;
                  
                  percentage *= rotations() + 1;
                  while(percentage > 1.) {
                    percentage -= 1.;
                  }
                  
                  
                  percentage *= 2;
                  
                  if(percentage > 1) {
                  	percentage -= 1;
                  	percentage = 1. - percentage;
                  }
                  
                  QPointF p = startPosition()*(1.0 - percentage) + stopPosition()*percentage;
                  
                  
                  c.r = pow(pow(p.x(),2)+pow(p.y(),2), 0.5);
                  c.phi = atan2(p.y(),p.x());
                  c.psi = 0;
                  
                  return c;
                  
            }break;

            case SPATTYPE_POLYLINE_CLOSED:
            case SPATTYPE_POLYGON_CLOSED:
            {
                  ratio *= 2;
                  
                  if(ratio > 1) {
                  	ratio -= 1;
                  	ratio = 1. - ratio;
                  }
            }
            case SPATTYPE_POLYLINE:
            case SPATTYPE_POLYGON:
            {
                  // startSpeed ist meter/viertel, wir benötigen aber meter/tick
                  float startSpeed = this->startSpeed() / 480.0;
                  
                  // aktuell wird nur  konstante geschwindigkeit unterstützt
                  float percentage = ratio;
                  
                  percentage *= rotations() + 1;
                
                  while(percentage > 1.) {
                	percentage -= 1.;
                  }
                
                if(_vertices.size() <= 1) {
                    c.ignore = true;
                    return c;
                }
                
                  //Erstelle aus dem Polygon ein Vector aus Lines und summiere die Länge
                  QVector<QLineF> lines;
                  qreal length = 0.;
                  for(int i=1;i<_vertices.size();i++) {
                  	QLineF l = QLineF(_vertices.at(i-1), _vertices.at(i));
                  	lines.append(l);
                  	length += l.length();
                  }
                  
                  // Close it in case of polygon
                  if(spatType() == SPATTYPE_POLYGON || spatType() == SPATTYPE_POLYGON_CLOSED) {
                  	QLineF l = QLineF(_vertices.at(_vertices.size()-1), _vertices.at(0));
                  	lines.append(l);
                  	length += l.length();
                  }
                  
                  qreal pos = length * percentage;
                  
                  //Finde das aktuelle line-segment
                  qreal l2 = 0;
                  int i=0;
                  for(i=0;i<lines.size()-1;i++) {
                  	l2 += lines.at(i).length();
                  	if(l2 > pos) {
                  		// zurücksetzen auf das akutelle Linestück
                  		l2 -= lines.at(i).length();
                  		break;
                  	}
                  }
                  
                  QLineF line = lines.at(i);
                  
                  // Ratio relativ zum aktuellen Linestück
                  ratio = (pos - l2)/line.length();
                  QPointF p = line.p1()*(1.0 - ratio) + line.p2()*ratio;
                  
                  c.r = pow(pow(p.x(),2)+pow(p.y(),2), 0.5);
                  c.phi = atan2(p.y(),p.x());
                  c.psi = 0;
                  
                  return c;
                  
            }break;
            
            case SPATTYPE_POSITION_ALTERNATION:
            {
            	float segmentLength = 1. / (rotations() + 1);
            	
            	int seg = floor(ratio/segmentLength);
            	
                QPointF p = startPosition();
                if(seg % 2 == 1) {
                	p = stopPosition();
                }
                  
                c.r = pow(pow(p.x(),2)+pow(p.y(),2), 0.5);
            	c.phi = atan2(p.y(),p.x());
             	c.psi = 0;
             	
             	return c;
             	
            }break;
      
            case SPATTYPE_CIRCLE_2D:
            {
                  // We assume that {startAngle, stopAngle} is in the interval [-180,180].
                  // Calculate the total angle of this trajectory
                  qreal start_angle_math = M_PI*(1.0/2.0 - startAngle()/180.0);
                  while(start_angle_math < 0) {
                  	start_angle_math += 2*M_PI;
                  }
                  qreal stop_angle_math = M_PI*(1.0/2.0 - stopAngle()/180.0);
                  while(stop_angle_math < 0) {
                  	stop_angle_math += 2*M_PI;
                  }
                  
                  // The total_angle is the the angle, that the symbol does from start to end.
                  // A positive sign means CCW, negative CW.
                  qreal total_angle;
                  if(direction() == DIRECTION_COUNTERCLOCKWISE) {
                        total_angle = 2.0*M_PI*rotations();
                        if(start_angle_math > stop_angle_math) {
                              total_angle += 2*M_PI - (start_angle_math - stop_angle_math);
                        }else {
                              total_angle += stop_angle_math - start_angle_math;
                        }
                  }else {
                        total_angle = - 2.0*M_PI*rotations();
                        if(start_angle_math > stop_angle_math) {
                              total_angle -= (start_angle_math - stop_angle_math);
                        }else {
                              total_angle -= 2*M_PI - (stop_angle_math - start_angle_math);
                        }
                  }
                  
                  qreal x,y;
                  // Apply the dislocation by the center point.
                  x = centerPosition().x();
                  y = centerPosition().y();
                  
                  // Apply the dislocation induced by the angle.
                  x += radius()*cos(start_angle_math + total_angle*ratio);
                  y += radius()*sin(start_angle_math + total_angle*ratio);
                  
                  // Fill the data into the coordinates-object.
                  c.r = hypot (x,y);
                  c.phi = atan2(y,x);
                  c.psi = 0.0;
                  
                  return c;
            }break;
      
      
      
            case SPATTYPE_CIRCLE_SLINKY_CLOSED:
            {
                  ratio *= 2;
                  
                  if(ratio > 1) {
                  	ratio -= 1;
                  	ratio = 1. - ratio;
                  }
            }
            case SPATTYPE_CIRCLE_SLINKY:
            {
                  // We assume that {startAngle, stopAngle} is in the interval [-180,180].
                  // Calculate the total angle of this trajectory
                  qreal start_angle_math = M_PI*(1.0/2.0 - startAngle()/180.0);
                  while(start_angle_math < 0) {
                  	start_angle_math += 2*M_PI;
                  }
                  qreal stop_angle_math = M_PI*(1.0/2.0 - stopAngle()/180.0);
                  while(stop_angle_math < 0) {
                  	stop_angle_math += 2*M_PI;
                  }
                  
                  // The total_angle is the the angle, that the symbol does from start to end.
                  // A positive sign means CCW, negative CW.
                  qreal total_angle;
                  if(direction() == DIRECTION_COUNTERCLOCKWISE) {
                        total_angle = 2.0*M_PI*rotations();
                        if(start_angle_math > stop_angle_math) {
                              total_angle += 2*M_PI - (start_angle_math - stop_angle_math);
                        }else {
                              total_angle += stop_angle_math - start_angle_math;
                        }
                  }else {
                        total_angle = - 2.0*M_PI*rotations();
                        if(start_angle_math > stop_angle_math) {
                              total_angle -= (start_angle_math - stop_angle_math);
                        }else {
                              total_angle -= 2*M_PI - (stop_angle_math - start_angle_math);
                        }
                  }
                  
                  qreal x,y;
                  // Apply the dislocation by the center point.
                  QPointF p = startPosition()*(1.0 - ratio) + stopPosition()*ratio;
                  
                  x = p.x();
                  y = p.y();
                  
                  // Apply the dislocation induced by the angle.
                  x += radius()*cos(start_angle_math + total_angle*ratio);
                  y += radius()*sin(start_angle_math + total_angle*ratio);
                  
                  // Fill the data into the coordinates-object.
                  c.r = hypot (x,y);
                  c.phi = atan2(y,x);
                  c.psi = 0.0;
                  
                  return c;
            }break;
      
            case SPATTYPE_CIRCLE_BF:
            {
                  // We assume that {startAngle, stopAngle} is in the interval [-180,180].
                  // Calculate the total angle of this trajectory
                  qreal start_angle_math = M_PI*(1.0/2.0 - startAngle()/180.0);
                  while(start_angle_math < 0) {
                  	start_angle_math += 2*M_PI;
                  }
                  qreal stop_angle_math = M_PI*(1.0/2.0 - stopAngle()/180.0);
                  while(stop_angle_math < 0) {
                  	stop_angle_math += 2*M_PI;
                  }
                  
                  // The total_angle is the the angle, that the symbol does from start to end.
                  // A positive sign means CCW, negative CW.
                  qreal total_angle = 0;
                  if(direction() == DIRECTION_COUNTERCLOCKWISE) {
                        if(start_angle_math > stop_angle_math) {
                              total_angle += 2*M_PI - (start_angle_math - stop_angle_math);
                        }else {
                              total_angle += stop_angle_math - start_angle_math;
                        }
                  }else {
                        if(start_angle_math > stop_angle_math) {
                              total_angle -= (start_angle_math - stop_angle_math);
                        }else {
                              total_angle -= 2*M_PI - (stop_angle_math - start_angle_math);
                        }
                  }
                  
                  ratio *= rotations() + 1;
                  while(ratio > 1.) {
                  	ratio -= 1;
                  }
                  
                  ratio *= 2;
                  
                  if(ratio > 1) {
                  	ratio -= 1;
                  	ratio = 1. - ratio;
                  }
                  
                  qreal x,y;
                  // Apply the dislocation by the center point.
                  x = centerPosition().x();
                  y = centerPosition().y();
                  
                  // Apply the dislocation induced by the angle.
                  x += radius()*cos(start_angle_math + total_angle*ratio);
                  y += radius()*sin(start_angle_math + total_angle*ratio);
                  
                  // Fill the data into the coordinates-object.
                  c.r = hypot (x,y);
                  c.phi = atan2(y,x);
                  c.psi = 0.0;
                  
                  return c;
            }break;
      
            case SPATTYPE_SPIRAL_OUT2IN:
            {
                  // We assume that {startAngle, stopAngle} is in the interval [-180,180].
                  // Calculate the total angle of this trajectory
                  qreal start_angle_math = M_PI*(1.0/2.0 - startAngle()/180.0);
                  while(start_angle_math < 0) {
                  	start_angle_math += 2*M_PI;
                  }
                  qreal stop_angle_math = M_PI*(1.0/2.0 - stopAngle()/180.0);
                  while(stop_angle_math < 0) {
                  	stop_angle_math += 2*M_PI;
                  }
                  
                  // The total_angle is the the angle, that the symbol does from start to end.
                  // A positive sign means CCW, negative CW.
                  qreal total_angle;
                  if(direction() == DIRECTION_COUNTERCLOCKWISE) {
                        total_angle = 2.0*M_PI*rotations();
                        if(start_angle_math > stop_angle_math) {
                              total_angle += 2*M_PI - (start_angle_math - stop_angle_math);
                        }else {
                              total_angle += stop_angle_math - start_angle_math;
                        }
                  }else {
                        total_angle = - 2.0*M_PI*rotations();
                        if(start_angle_math > stop_angle_math) {
                              total_angle -= (start_angle_math - stop_angle_math);
                        }else {
                              total_angle -= 2*M_PI - (stop_angle_math - start_angle_math);
                        }
                  }
                  
                  qreal x,y;
                  // Apply the dislocation by the center point.
                  x = centerPosition().x();
                  y = centerPosition().y();
                  
                  // Apply the dislocation induced by the angle.
                  x += ((1. - ratio)*innerRadius() + ratio*radius())*cos(start_angle_math + total_angle*ratio);
                  y += ((1. - ratio)*innerRadius() + ratio*radius())*sin(start_angle_math + total_angle*ratio);
                  
                  // Fill the data into the coordinates-object.
                  c.r = hypot (x,y);
                  c.phi = atan2(y,x);
                  c.psi = 0.0;
                  
                  return c;
            }break;
      
            case SPATTYPE_SPIRAL_IN2OUT:
            {
                  // We assume that {startAngle, stopAngle} is in the interval [-180,180].
                  // Calculate the total angle of this trajectory
                  qreal start_angle_math = M_PI*(1.0/2.0 - startAngle()/180.0);
                  while(start_angle_math < 0) {
                  	start_angle_math += 2*M_PI;
                  }
                  qreal stop_angle_math = M_PI*(1.0/2.0 - stopAngle()/180.0);
                  while(stop_angle_math < 0) {
                  	stop_angle_math += 2*M_PI;
                  }
                  
                  // The total_angle is the the angle, that the symbol does from start to end.
                  // A positive sign means CCW, negative CW.
                  qreal total_angle;
                  if(direction() == DIRECTION_COUNTERCLOCKWISE) {
                        total_angle = 2.0*M_PI*rotations();
                        if(start_angle_math > stop_angle_math) {
                              total_angle += 2*M_PI - (start_angle_math - stop_angle_math);
                        }else {
                              total_angle += stop_angle_math - start_angle_math;
                        }
                  }else {
                        total_angle = - 2.0*M_PI*rotations();
                        if(start_angle_math > stop_angle_math) {
                              total_angle -= (start_angle_math - stop_angle_math);
                        }else {
                              total_angle -= 2*M_PI - (stop_angle_math - start_angle_math);
                        }
                  }
                  
                  qreal x,y;
                  // Apply the dislocation by the center point.
                  x = centerPosition().x();
                  y = centerPosition().y();
                  
                  // Apply the dislocation induced by the angle.
                  x += ((1. - ratio)*radius() + ratio*innerRadius())*cos(start_angle_math + total_angle*ratio);
                  y += ((1. - ratio)*radius() + ratio*innerRadius())*sin(start_angle_math + total_angle*ratio);
                  
                  // Fill the data into the coordinates-object.
                  c.r = hypot (x,y);
                  c.phi = atan2(y,x);
                  c.psi = 0.0;
                  
                  return c;
            }break;
      
            case SPATTYPE_SPIRAL_IN2OUT_CLOSED:
            {
                  // We assume that {startAngle, stopAngle} is in the interval [-180,180].
                  // Calculate the total angle of this trajectory
                  qreal start_angle_math = M_PI*(1.0/2.0 - startAngle()/180.0);
                  while(start_angle_math < 0) {
                  	start_angle_math += 2*M_PI;
                  }
                  qreal stop_angle_math = M_PI*(1.0/2.0 - stopAngle()/180.0);
                  while(stop_angle_math < 0) {
                  	stop_angle_math += 2*M_PI;
                  }
                  
                  // The total_angle is the the angle, that the symbol does from start to end.
                  // A positive sign means CCW, negative CW.
                  qreal total_angle;
                  if(direction() == DIRECTION_COUNTERCLOCKWISE) {
                        total_angle = 2.0*M_PI*rotations();
                        if(start_angle_math > stop_angle_math) {
                              total_angle += 2*M_PI - (start_angle_math - stop_angle_math);
                        }else {
                              total_angle += stop_angle_math - start_angle_math;
                        }
                  }else {
                        total_angle = - 2.0*M_PI*rotations();
                        if(start_angle_math > stop_angle_math) {
                              total_angle -= (start_angle_math - stop_angle_math);
                        }else {
                              total_angle -= 2*M_PI - (stop_angle_math - start_angle_math);
                        }
                  }
                  
                  qreal x,y;
                  // Apply the dislocation by the center point.
                  x = centerPosition().x();
                  y = centerPosition().y();
                  
                  ratio *= 2;
                  if(ratio > 1.) {
                  	ratio -= 1;
                  	
                  	ratio = 1. - ratio;
                  }
                  
                  // Apply the dislocation induced by the angle.
                  x += ((1. - ratio)*innerRadius() + ratio*radius())*cos(start_angle_math + total_angle*ratio);
                  y += ((1. - ratio)*innerRadius() + ratio*radius())*sin(start_angle_math + total_angle*ratio);
                  
                  // Fill the data into the coordinates-object.
                  c.r = hypot (x,y);
                  c.phi = atan2(y,x);
                  c.psi = 0.0;
                  
                  return c;
            }break;
      
            case SPATTYPE_SPIRAL_OUT2IN_CLOSED:
            {
                  // We assume that {startAngle, stopAngle} is in the interval [-180,180].
                  // Calculate the total angle of this trajectory
                  qreal start_angle_math = M_PI*(1.0/2.0 - startAngle()/180.0);
                  while(start_angle_math < 0) {
                  	start_angle_math += 2*M_PI;
                  }
                  qreal stop_angle_math = M_PI*(1.0/2.0 - stopAngle()/180.0);
                  while(stop_angle_math < 0) {
                  	stop_angle_math += 2*M_PI;
                  }
                  
                  // The total_angle is the the angle, that the symbol does from start to end.
                  // A positive sign means CCW, negative CW.
                  qreal total_angle;
                  if(direction() == DIRECTION_COUNTERCLOCKWISE) {
                        total_angle = 2.0*M_PI*rotations();
                        if(start_angle_math > stop_angle_math) {
                              total_angle += 2*M_PI - (start_angle_math - stop_angle_math);
                        }else {
                              total_angle += stop_angle_math - start_angle_math;
                        }
                  }else {
                        total_angle = - 2.0*M_PI*rotations();
                        if(start_angle_math > stop_angle_math) {
                              total_angle -= (start_angle_math - stop_angle_math);
                        }else {
                              total_angle -= 2*M_PI - (stop_angle_math - start_angle_math);
                        }
                  }
                  
                  qreal x,y;
                  // Apply the dislocation by the center point.
                  x = centerPosition().x();
                  y = centerPosition().y();
                  
                  ratio *= 2;
                  if(ratio > 1.) {
                  	ratio -= 1;
                  	
                  	ratio = 1. - ratio;
                  }
                  
                  // Apply the dislocation induced by the angle.
                  x += ((1. - ratio)*radius() + ratio*innerRadius())*cos(start_angle_math + total_angle*ratio);
                  y += ((1. - ratio)*radius() + ratio*innerRadius())*sin(start_angle_math + total_angle*ratio);
                  
                  // Fill the data into the coordinates-object.
                  c.r = hypot (x,y);
                  c.phi = atan2(y,x);
                  c.psi = 0.0;
                  
                  return c;
            }
      
            case SPATTYPE_BEZIER_CUBIC:
            {
                  qreal x,y;
                  QPointF point;
                  QPointF p[4] = {_startPosition, _controlPoint1, _controlPoint2, _stopPosition};
                  
                  point = qPow(1.-ratio,3.)*p[0] + 3.*qPow(1.-ratio,2.)*ratio*p[1] + 3*(1.-ratio)*qPow(ratio,2.)*p[2] + qPow(ratio,3.)*p[3];
                  x = point.x();
                  y = point.y();
                  
                  // Fill the data into the coordinates-object.
                  c.r = hypot (x,y);
                  c.phi = atan2(y,x);
                  c.psi = 0.0;
                  
                  return c;
            }
      
            case SPATTYPE_BEZIER_CUBIC_SPLINE_CLOSED:
            {
                  ratio *= 2;
                  
                  if(ratio > 1) {
                  	ratio -= 1;
                  	ratio = 1. - ratio;
                  }
            }
            case SPATTYPE_BEZIER_CUBIC_SPLINE:
            {
                  qreal x,y;
                  QPointF point;
                  
                  if(_vertices.count() < 4) {
                  	c.ignore = true;
                  	return c;
                  }
                  
                  // First locate the segment
                  int segments = _vertices.count()/2 - 1;
                  int segment = qMin((int)(ratio * segments), segments-1);
                  ratio = ratio - ((qreal)segment)/segments;
                  ratio *= segments;
                  
                  QPointF p[4] = {_vertices[segment*2], _vertices[segment*2+1], 
                  	2*_vertices[segment*2+2]-_vertices[segment*2+3], _vertices[segment*2+2]};
                  
                  point = qPow(1.-ratio,3.)*p[0] + 3.*qPow(1.-ratio,2.)*ratio*p[1] + 3*(1.-ratio)*qPow(ratio,2.)*p[2] + qPow(ratio,3.)*p[3];
                  x = point.x();
                  y = point.y();
                  
                  // Fill the data into the coordinates-object.
                  c.r = hypot (x,y);
                  c.phi = atan2(y,x);
                  c.psi = 0.0;
                  
                  return c;
            }
      
            case SPATTYPE_BEZIER_CUBIC_CLOSED:
            {
                  qreal x,y;
                  QPointF point;
                  QPointF p[4] = {_startPosition, _controlPoint1, _controlPoint2, _stopPosition};
                  
                  ratio *= 2;
                  if(ratio > 1.) {
                  	ratio -= 1;
                  	ratio = 1. - ratio;
                  }
                  
                  point = qPow(1.-ratio,3.)*p[0] + 3.*qPow(1.-ratio,2.)*ratio*p[1] + 3*(1.-ratio)*qPow(ratio,2.)*p[2] + qPow(ratio,3.)*p[3];
                  x = point.x();
                  y = point.y();
                  
                  // Fill the data into the coordinates-object.
                  c.r = hypot (x,y);
                  c.phi = atan2(y,x);
                  c.psi = 0.0;
                  
                  return c;
            }
      
            case SPATTYPE_BERNOULLI_LEMNISCATE:
            {
                  // We assume that {startAngle, stopAngle} is in the interval [-180,180].
                  // Calculate the total angle of this trajectory
                  qreal start_angle_math = M_PI*(1.0/2.0 - startAngle()/180.0);
                  while(start_angle_math < 0) {
                  	start_angle_math += 2*M_PI;
                  }
                  qreal stop_angle_math = M_PI*(1.0/2.0 - stopAngle()/180.0);
                  while(stop_angle_math < 0) {
                  	stop_angle_math += 2*M_PI;
                  }
                  
                  // The total_angle is the the angle, that the symbol does from start to end.
                  // A positive sign means CCW, negative CW.
                  qreal total_angle;
                  if(direction() == DIRECTION_COUNTERCLOCKWISE) {
                        total_angle = 2.0*M_PI*rotations();
                        if(start_angle_math > stop_angle_math) {
                              total_angle += 2*M_PI - (start_angle_math - stop_angle_math);
                        }else {
                              total_angle += stop_angle_math - start_angle_math;
                        }
                  }else {
                        total_angle = - 2.0*M_PI*rotations();
                        if(start_angle_math > stop_angle_math) {
                              total_angle -= (start_angle_math - stop_angle_math);
                        }else {
                              total_angle -= 2*M_PI - (stop_angle_math - start_angle_math);
                        }
                  }
                  qreal t = start_angle_math + total_angle*ratio;
                  
                  QPointF p;
                  p.setX(radius()*qCos(t)/(qPow(qSin(t),2)+1));
                  p.setY(radius()*qCos(t)*qSin(t)/(qPow(qSin(t),2) + 1));
                  
                  //Rotate
                  QTransform tr;
                  tr.translate(centerPosition().x(), centerPosition().y()).rotate(innerRadius()*100);
                  p = tr.map(p);
                  
                  
                  qreal x,y;
                  x = p.x();
                  y = p.y();
                  
                  // Fill the data into the coordinates-object.
                  c.r = hypot (x,y);
                  c.phi = atan2(y,x);
                  c.psi = 0.0;
                  
                  return c;
            }
      }
      
      qDebug() << "SpatSymbol::calculateCoordinates no such type: " << spatType();
      c.ignore =true;
      return c;
}
  
      
 int SpatSymbol::tick() {
      if(parent()) {
      Segment* s = static_cast<Segment *>(parent());
       return s->tick();
      }
      qDebug() << "SpatSymbol::tick() called without having a parent";
      return 0;
 }         
         
SpatCoordinates::SpatCoordinates() {
      ignore = false;
}

SpatCoordinates::SpatCoordinates(double x, double y) {
      ignore = false;
    
    r = hypot (x,y);
    phi = atan2(y,x);
    psi = 0.0;	
}


SpatCoordinates const& SpatCoordinates::operator+=(SpatCoordinates const& rhs) {

	if(rhs.ignore) {
		return *this;
	}

	double _x = x() + rhs.x();
	double _y = y() + rhs.y();
    
    r = hypot (_x,_y);
    phi = atan2(_y,_x);
    psi = 0.0;	
	
	return *this;
}

bool SpatCoordinates::operator == (const SpatCoordinates& c)
{
    return ( (c.r == r)
            && (c.phi == phi)
           && (c.psi == psi) );
}

double SpatCoordinates::x() const {
      return r*cos(phi);
}

double SpatCoordinates::y() const {
      return r*sin(phi);
}

 QDebug operator<<(QDebug dbg, const SpatCoordinates &c)
 {
     dbg.nospace() << "(" << c.x() << ", " << c.y() << " ignore: " << c.ignore << ")";

     return dbg.space();
 }         
           
//---------------------------------------------------------
//   getProperty
//---------------------------------------------------------

QVariant SpatSymbol::getProperty(P_ID propertyId) const
      {
      switch(propertyId) {
            case P_DURATION:
                  return duration();
            case P_START_POSITION:
                  return startPosition();
            case P_STOP_POSITION:
                  return stopPosition();
            
            case P_CENTER_POSITION:
            	return centerPosition();
            case P_RADIUS:
            	return radius();
            case P_INNER_RADIUS:
            	return innerRadius();
            case P_SPAT_DIRECTION:
            	return direction();
            case P_ROTATIONS:
            	return rotations();
            case P_START_ANGLE:
            	return startAngle();
            case P_STOP_ANGLE:
            	return stopAngle();
            	
            case P_OSC_MESSAGE_1:
            	return oscMessage1();
            case P_OSC_MESSAGE_2:
            	return oscMessage2();
            	
            default:
                  break;
            }
      return Element::getProperty(propertyId);
      }

//---------------------------------------------------------
//   setProperty
//---------------------------------------------------------

bool SpatSymbol::setProperty(P_ID propertyId, const QVariant& v)
      {
      switch(propertyId) {
            case P_DURATION:
                  setDuration(v.toReal());
                  break;
            case P_START_POSITION:
                  setStartPosition(v.toPointF());
                  break;
            case P_STOP_POSITION:
                  setStopPosition(v.toPointF());
                  break;
                  
        	case P_CENTER_POSITION:
        		setCenterPosition(v.toPointF());
        		break;
        	case P_RADIUS:
        		setRadius(v.toReal());
        		break;
        	case P_INNER_RADIUS:
        		setInnerRadius(v.toReal());
        		break;
        	case P_SPAT_DIRECTION:
        		setDirection(v.toInt());
        		break;
        	case P_ROTATIONS:
        		setRotations(v.toInt());
        		break;
        	case P_START_ANGLE:
        		setStartAngle(v.toReal());
        		break;
        	case P_STOP_ANGLE:
        		setStopAngle(v.toReal());
        		break;
        		
            case P_CONTROL_POINT_1:
                  setControlPoint1(v.toPointF());
                  break;
            case P_CONTROL_POINT_2:
                  setControlPoint2(v.toPointF());
                  break;
        		
        	case P_OSC_MESSAGE_1:
        		setOSCMessage1(v.toString());
        		break;
        	case P_OSC_MESSAGE_2:
        		setOSCMessage2(v.toString());
        		break;
        		
            default:
                  if (!Element::setProperty(propertyId, v))
                        return false;
                  break;
            }
      score()->setLayoutAll(true);
      return true;
      }                    
                           
