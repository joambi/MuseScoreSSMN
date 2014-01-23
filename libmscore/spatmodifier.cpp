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

#include "spatmodifier.h"
#include "score.h"
#include "chordrest.h"
#include "system.h"
#include "measure.h"
#include "staff.h"
#include "stafftype.h"
#include "undo.h"
#include "segment.h"
      
      



SpatModifier::SpatModifier(Score* s) 
      : Element(s),
      
      doc(NULL)
      
{ 
      setFlags(ELEMENT_MOVABLE | ELEMENT_SELECTABLE);  
}

SpatModifier::SpatModifier(const SpatModifier& s) 
      : Element(s), 
      doc(NULL)
      
{
}


QString SpatModifier::spatTypeString() const {
      switch(_spatType) {
                  
            case SPATMODIFIER_RANDOM:
            	return "Random";
                  
        }
	return "Error";
}

void SpatModifier::setSpatType(int t) { 
      _spatType = t;
      	
      QString path = ":/data/spatialization/";
      switch(t) {
                  
            case SPATMODIFIER_RANDOM:
            	path += "jitter-saw.svg";
                break;
      }
      qDebug() << path;
      QFile f(path);
      if (!f.open(QIODevice::ReadOnly)) {
            return;
    	}
      QByteArray ba = f.readAll();
      f.close();
      
      doc = new QSvgRenderer(ba);
      	
}



QLineF SpatModifier::dragAnchor() const
      {
      qreal xp = 0.0;
      for (Element* e = parent(); e; e = e->parent())
            xp += e->x();
      qreal yp = measure()->system()->staffY(staffIdx());
      QPointF p(xp, yp);
      return QLineF(p, canvasPos());
      }
      

void SpatModifier::draw(QPainter* painter) const {


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

void SpatModifier::layout() {
      setbbox(QRectF(-10.0, -20.0, 20.0, 20.0));
      setPos(0.0, -30.0);
      adjustReadPos();
   //   BSymbol::layout();
   	
   	// Make sure that the grapic is loaded.
   		if(!doc) {
   			setSpatType(spatType());
   		}
}

SpatModifier* SpatModifier::clone() const   { 
      return new SpatModifier(*this); 
      }
      
      



void SpatModifier::write(Xml& xml) const {
      xml.stag(name());
      Element::writeProperties(xml);
      
      switch(spatType()) {
                  
            case SPATMODIFIER_RANDOM: 
                  xml.tag("spatmodifier", "random");
                  break;
      }
      
      xml.etag();
      }


void SpatModifier::read(XmlReader& e)
{
      
      while (e.readNextStartElement()) {
            const QStringRef& tag(e.name());
            
            if (tag == "spatmodifier") {
        		QString val(e.readElementText());
                  if(val == "random") {
                        setSpatType(SPATMODIFIER_RANDOM);
                  }
            
            }       
      }
      
}
                   
