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

#ifndef __SPATMODIFIER_H__
#define __SPATMODIFIER_H__

#include "symbol.h"
#include "sym.h"
#include "image.h"

class Painter;
class ChordRest;
class SpatCoordinates;
class SpatOSCMessage;



enum {
      SPATMODIFIER_RANDOM,
};



class SpatModifier : public Element {
      Q_DECLARE_TR_FUNCTIONS(SpatModifier)
      
      
 protected:
      QSvgRenderer* doc;

      
      /* Global Properties */
      
      int _spatType;
      
      
      
public:
      SpatModifier(Score*);
      SpatModifier(const SpatModifier& s);
      virtual ElementType type() const      { return SPATIALIZATION_MODIFIER; }
      virtual SpatModifier* clone() const;
      
    virtual QLineF dragAnchor() const;
    Segment* segment() const         { return (Segment*)parent()->parent(); }
    Measure* measure() const         { return (Measure*)parent()->parent()->parent(); }
      
      
      virtual void draw(QPainter* p) const;
      virtual void layout();
      virtual void write(Xml& xml) const;
      virtual void read(XmlReader& e);
      
      
      int spatType() const { return _spatType; }
      void setSpatType(int t);
      QString spatTypeString() const;
      
      
      
      
      

      };



#endif

