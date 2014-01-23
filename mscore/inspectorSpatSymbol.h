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

#ifndef __INSPECTORSPATSYMBOL_H__
#define __INSPECTORSPATSYMBOL_H__

#include "inspectorBase.h"
#include "inspector.h"
#include "ui_inspector_element.h"
#include "ui_inspector_vbox.h"
#include "ui_inspector_hbox.h"
#include "ui_inspector_articulation.h"
#include "ui_inspector_spacer.h"
#include "ui_inspector_segment.h"
#include "ui_inspector_note.h"
#include "ui_inspector_chord.h"

#include "ui_inspector_spatsymbol_base.h"
#include "ui_inspector_spatsymbol_line.h"
#include "ui_inspector_spatsymbol_polygon.h"
#include "ui_inspector_spatsymbol_circle.h"
#include "ui_inspector_spatsymbol_spiral.h"
#include "ui_inspector_spatsymbol_bernoulli_lemniscate.h"
#include "ui_inspector_spatsymbol_slinky.h"
#include "ui_inspector_spatsymbol_bezier.h"
#include "ui_inspector_spatsymbol_bezier_spline.h"
#include "ui_inspector_spatsymbol_position.h"
#include "ui_inspector_spatsymbol_position_alternation.h"
#include "ui_inspector_spatsymbol_oscmessage.h"
#include "ui_inspector_spatsymbol_modifier_markov.h"
#include "ui_inspector_spatsymbol_modifier_sine.h"

#include "radar2d.h"

class Element;
class SpatSymbol;
class InspectorElementElement;



class InspectorSpatSymbolExtend : public QWidget {
	Q_OBJECT

	protected:
		SpatSymbol *element;
    	bool ignoreSignals;	
		
	public:
		virtual void setElement(Element*);
		InspectorSpatSymbolExtend(QWidget *parent) : QWidget(parent) { element = 0; ignoreSignals = false;};
};


class InspectorSpatSymbolBase : public InspectorSpatSymbolExtend, Ui::InspectorSpatSymbolBase {
    Q_OBJECT
    
    private slots:
    
public slots:
    virtual void apply();
    
public:
    InspectorSpatSymbolBase(QWidget* parent = 0);
    virtual void setElement(Element*);
};


class InspectorSpatSymbolModifierSine : public InspectorSpatSymbolExtend, Ui::InspectorSpatSymbolModifierSine {
		Q_OBJECT
		
	public:
		InspectorSpatSymbolModifierSine(QWidget *parent = 0);
    	virtual void setElement(Element*);	
    	
    public slots:
    	void numericChangedRadius(int i);
    	void changedRotations(int i);
};

class InspectorSpatSymbolCircle : public InspectorSpatSymbolExtend, Ui::InspectorSpatSymbolCircle {
		Q_OBJECT
		
	public:
		InspectorSpatSymbolCircle(QWidget *parent = 0);
    	virtual void setElement(Element*);	
    	
    public slots:
    	void numericChangedCenter(int i);
    	void numericChangedRadius(int i);
    	void numericChangedDirection(int i);
    	void changedRotations(int i);
    	void numericChangedStartAngle(int i);
    	void numericChangedStopAngle(int i);
};

class InspectorSpatSymbolSpiral : public InspectorSpatSymbolExtend, Ui::InspectorSpatSymbolSpiral {
		Q_OBJECT
		
	public:
		InspectorSpatSymbolSpiral(QWidget *parent = 0);
    	virtual void setElement(Element*);	
    	
    public slots:
    	void numericChangedCenter(int i);
    	void numericChangedRadius(int i);
    	void numericChangedInnerRadius(int i);
    	void numericChangedDirection(int i);
    	void changedRotations(int i);
    	void numericChangedStartAngle(int i);
    	void numericChangedStopAngle(int i);
};

class InspectorSpatSymbolBernoulliLemniscate : public InspectorSpatSymbolExtend, Ui::InspectorSpatSymbolBernoulliLemniscate {
		Q_OBJECT
		
	public:
		InspectorSpatSymbolBernoulliLemniscate(QWidget *parent = 0);
    	virtual void setElement(Element*);	
    	
    public slots:
    	void numericChangedCenter(int i);
    	void numericChangedRadius(int i);
    	void numericChangedInnerRadius(int i);
    	void numericChangedDirection(int i);
    	void changedRotations(int i);
    	void numericChangedStartAngle(int i);
    	void numericChangedStopAngle(int i);
};

class InspectorSpatSymbolSlinky : public InspectorSpatSymbolExtend, Ui::InspectorSpatSymbolSlinky {
		Q_OBJECT
		
	public:
		InspectorSpatSymbolSlinky(QWidget *parent = 0);
    	virtual void setElement(Element*);	
    	
    public slots:
		void numericChangedStart(int i);
		void numericChangedStop(int i);
    	void numericChangedRadius(int i);
    	void numericChangedDirection(int i);
    	void changedRotations(int i);
    	void numericChangedStartAngle(int i);
    	void numericChangedStopAngle(int i);
};

class InspectorSpatSymbolLine : public InspectorSpatSymbolExtend, Ui::InspectorSpatSymbolLine {
		Q_OBJECT
	
		QVBoxLayout *layout;
		Radar2DInputPolygon *radar;
		
		
	
	public slots:
		void radarChanged(QPolygonF p);
		void numericChangedStart(int i);
		void numericChangedStop(int i);
    	void changedRotations(int i);
		
	
	public:
   		InspectorSpatSymbolLine(QWidget* parent = 0);
    	virtual void setElement(Element*);	
};

class InspectorSpatSymbolModifierMarkov : public InspectorSpatSymbolExtend, Ui::InspectorSpatSymbolModifierMarkov {
		Q_OBJECT
	
		QVBoxLayout *layout;
		
		
	
	public slots:
		void numericChangedStart(int i);
		void numericChangedStop(int i);
    	void numericChangedRadius(int i);
		
	
	public:
   		InspectorSpatSymbolModifierMarkov(QWidget* parent = 0);
    	virtual void setElement(Element*);	
};


class InspectorSpatSymbolPolygon : public InspectorSpatSymbolExtend, Ui::InspectorSpatSymbolPolygon {
		Q_OBJECT
	
		QVBoxLayout *layout;
		Radar2DInputPolygon *radar;
		
		
	
	public slots:
		void radarChanged(QPolygonF p);
		void changedNumberOfVertices(int i);
    	void changedRotations(int i);
		void numericChangedStart(int i);
		void numericChangedStop(int i);
		
	
	public:
   		InspectorSpatSymbolPolygon(QWidget* parent = 0);
    	virtual void setElement(Element*);	
};


class InspectorSpatSymbolBezier : public InspectorSpatSymbolExtend, Ui::InspectorSpatSymbolBezier {
		Q_OBJECT
	
		QVBoxLayout *layout;
		Radar2DInputBezier *radar;
		
		
	
	public slots:
		void radarChanged(QPolygonF p);
		void numericChangedStart(int i);
		void numericChangedStop(int i);
		void numericChangedControlPoint1(int i);
		void numericChangedControlPoint2(int i);
		
	
	public:
   		InspectorSpatSymbolBezier(QWidget* parent = 0);
    	virtual void setElement(Element*);	
};

class InspectorSpatSymbolBezierSpline : public InspectorSpatSymbolExtend, Ui::InspectorSpatSymbolBezierSpline {
		Q_OBJECT
	
		QVBoxLayout *layout;
		Radar2DInputBezier *radar;
		
		
	
	public slots:
		void radarChanged(QPolygonF p);
		void changedNumberOfVertices(int i);
    	void changedRotations(int i);
		void numericChangedStart(int i);
		void numericChangedStop(int i);
		
	
	public:
   		InspectorSpatSymbolBezierSpline(QWidget* parent = 0);
    	virtual void setElement(Element*);	
};


class InspectorSpatSymbolPosition : public InspectorSpatSymbolExtend, Ui::InspectorSpatSymbolPosition {
		Q_OBJECT
		
	public:
		InspectorSpatSymbolPosition(QWidget *parent = 0);
    	virtual void setElement(Element*);	
    	
    public slots:
    	void numericChangedCenter(int i);
};

class InspectorSpatSymbolPositionAlternation : public InspectorSpatSymbolExtend, Ui::InspectorSpatSymbolPositionAlternation {
		Q_OBJECT
		
	public:
		InspectorSpatSymbolPositionAlternation(QWidget *parent = 0);
    	virtual void setElement(Element*);	
    	
    public slots:
    	void changedRotations(int i);
		void numericChangedStart(int i);
		void numericChangedStop(int i);
};


class InspectorSpatSymbolOSCMessage : public InspectorSpatSymbolExtend, Ui::InspectorSpatSymbolOSCMessage {
		Q_OBJECT
		
	public:
		InspectorSpatSymbolOSCMessage(QWidget *parent = 0);
    	virtual void setElement(Element*);	
    	
    public slots:
    	void message1Changed(const QString&);
    	void message2Changed(const QString&);
};

class InspectorSpatializationSymbol : public InspectorBase {
    Q_OBJECT
    
    SpatSymbol *e;
    
    InspectorElementElement* iElement;
    QList<InspectorSpatSymbolExtend*> inspectors;
    
    void block(bool);
    bool dirty() const;
    
    private slots:
    
public:
    InspectorSpatializationSymbol(QWidget* parent);
    virtual void setElement(Element*);
};

#endif

