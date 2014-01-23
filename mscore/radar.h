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

#ifndef __RADAR_H__
#define __RADAR_H__


#include "radar2d.h"
#include "libmscore/score.h"

class Element;
class Note;
class Inspector;
class Segment;
class Chord;

class Radar : public QDockWidget {
      Q_OBJECT

      QVBoxLayout* layout;
    //  InspectorBase* ie;
      QPushButton* apply;
      Element* _element;
      QList<Element*> _el;
      
      Radar2D *radar2d;
/*
      virtual void closeEvent(QCloseEvent*);

   private slots:
      void applyClicked();

   signals:
      void inspectorVisible(bool);

   public slots:
      void enableApply(bool val = true) { apply->setEnabled(val); }
      void reset();
*/
   public:
      Radar(QWidget* parent = 0);
      
      void setScore(Score *s);
  /*    void setElement(Element*);
      void setElementList(const QList<Element*>&);*/
      Element* element() const { return _element; }
      const QList<Element*>& el() const { return _el; } 
      };

#endif

