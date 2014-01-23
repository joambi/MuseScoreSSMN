//=============================================================================
//  MuseScore
//  Music Composition & Notation
//  $Id:$
//
//  Copyright (C) 2011 Werner Schweer
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License version 2
//  as published by the Free Software Foundation and appearing in
//  the file LICENSE.GPL
//=============================================================================

#include "inspector.h"
#include "inspectorBeam.h"
#include "inspectorImage.h"
#include "inspectorLasso.h"
#include "inspectorGroupElement.h"
#include "musescore.h"
#include "scoreview.h"

#include "libmscore/element.h"
#include "libmscore/score.h"
#include "libmscore/box.h"
#include "libmscore/undo.h"
#include "libmscore/spacer.h"
#include "libmscore/note.h"
#include "libmscore/chord.h"
#include "libmscore/segment.h"
#include "libmscore/rest.h"
#include "libmscore/beam.h"
#include "libmscore/clef.h"
#include "libmscore/notedot.h"
#include "libmscore/hook.h"
#include "libmscore/stem.h"

#include "inspectorSpatSymbol.h"
#include "musescore.h"
#include "scoreview.h"

#include "libmscore/element.h"
#include "libmscore/spatsymbol.h"



void InspectorSpatSymbolExtend::setElement(Element* e) {
	element = static_cast<SpatSymbol*>(e);
}

#pragma mark -
#pragma mark Base

InspectorSpatSymbolBase::InspectorSpatSymbolBase(QWidget *parent) : InspectorSpatSymbolExtend(parent) {
    setupUi(this);
}

void InspectorSpatSymbolBase::setElement(Element *e) {
	InspectorSpatSymbolExtend::setElement(e);
    
	
	type->setText(element->spatTypeString());
}



void InspectorSpatSymbolBase::apply()
{
}




#pragma mark -
#pragma mark Bezier

InspectorSpatSymbolBezier::InspectorSpatSymbolBezier(QWidget *parent) : InspectorSpatSymbolExtend(parent) {
    setupUi(this);

	radar = new Radar2DInputBezier(this);
	
	gridLayout->addWidget(radar, 5,0,1,-1);
    
    
    connect(radar, SIGNAL(polygonChanged(QPolygonF)), SLOT(radarChanged(QPolygonF)) );
    
    connect(startX, SIGNAL(valueChanged(int)), SLOT(numericChangedStart(int)) );
    connect(startY, SIGNAL(valueChanged(int)), SLOT(numericChangedStart(int)) );
    connect(stopX, SIGNAL(valueChanged(int)), SLOT(numericChangedStop(int)) );
    connect(stopY, SIGNAL(valueChanged(int)), SLOT(numericChangedStop(int)) );
    
    connect(control1X, SIGNAL(valueChanged(int)), SLOT(numericChangedControlPoint1(int)) );
    connect(control1Y, SIGNAL(valueChanged(int)), SLOT(numericChangedControlPoint1(int)) );
    connect(control2X, SIGNAL(valueChanged(int)), SLOT(numericChangedControlPoint2(int)) );
    connect(control2Y, SIGNAL(valueChanged(int)), SLOT(numericChangedControlPoint2(int)) );
}

void InspectorSpatSymbolBezier::numericChangedStart(int i) {
	if(ignoreSignals) {
		return;
	}
	
	Score* score = element->score();
    score->startCmd();
    score->undoChangeProperty(element, P_START_POSITION, QPointF(startX->value(),startY->value())/100.);
    score->endCmd();
    mscore->endCmd();
}

void InspectorSpatSymbolBezier::numericChangedStop(int i) {
    
	if(ignoreSignals) {
		return;
	}
	
	Score* score = element->score();
    score->startCmd();
    score->undoChangeProperty(element, P_STOP_POSITION, QPointF(stopX->value(),stopY->value())/100.);
    score->endCmd();
    mscore->endCmd();
}

void InspectorSpatSymbolBezier::numericChangedControlPoint1(int i) {
    
	if(ignoreSignals) {
		return;
	}
	
	Score* score = element->score();
    score->startCmd();
    score->undoChangeProperty(element, P_CONTROL_POINT_1, QPointF(control1X->value(),control1Y->value())/100.);
    score->endCmd();
    mscore->endCmd();
}

void InspectorSpatSymbolBezier::numericChangedControlPoint2(int i) {
    
	if(ignoreSignals) {
		return;
	}
	
	Score* score = element->score();
    score->startCmd();
    score->undoChangeProperty(element, P_CONTROL_POINT_2, QPointF(control2X->value(),control2Y->value())/100.);
    score->endCmd();
    mscore->endCmd();
}



void InspectorSpatSymbolBezier::radarChanged(QPolygonF p) {
	if(ignoreSignals) {
		return;
	}

    //if (p[0] != element->startPosition() || p[3] != element->stopPosition()) 
    {
        Score* score     = element->score();
        score->startCmd();
        score->undoChangeProperty(element, P_START_POSITION, p[0]);
        score->undoChangeProperty(element, P_CONTROL_POINT_1, p[1]);
        score->undoChangeProperty(element, P_CONTROL_POINT_2, 2*p[2] - p[3]);
        score->undoChangeProperty(element, P_STOP_POSITION, p[2]);
        score->endCmd();
        mscore->endCmd();
    }	
}

void InspectorSpatSymbolBezier::setElement(Element *e) {
	InspectorSpatSymbolExtend::setElement(e);
	
	ignoreSignals = true;
    
    QPolygonF p(4);
    p[0] = element->startPosition();
    p[1] = element->controlPoint1();
    p[2] = element->stopPosition();
    p[3] = 2*p[2] - element->controlPoint2();
    radar->setPolygon(p);
    
    startX->setValue(round(element->startPosition().x()*100.));
    startY->setValue(round(element->startPosition().y()*100.));
    stopX->setValue(round(element->stopPosition().x()*100.));
    stopY->setValue(round(element->stopPosition().y()*100.));
    
    control1X->setValue(round(element->controlPoint1().x()*100.));
    control1Y->setValue(round(element->controlPoint1().y()*100.));
    control2X->setValue(round(element->controlPoint2().x()*100.));
    control2Y->setValue(round(element->controlPoint2().y()*100.));
    
	
    ignoreSignals = false;
}

#pragma mark -
#pragma mark BezierSpline

InspectorSpatSymbolBezierSpline::InspectorSpatSymbolBezierSpline(QWidget *parent) : InspectorSpatSymbolExtend(parent) {
    setupUi(this);

	radar = new Radar2DInputBezier(this);
	
	gridLayout->addWidget(radar, 5,0,1,-1);
    
    
    connect(radar, SIGNAL(polygonChanged(QPolygonF)), SLOT(radarChanged(QPolygonF)) );
    connect(rotations, SIGNAL(valueChanged(int)), SLOT(changedRotations(int)) );
    connect(vertices, SIGNAL(valueChanged(int)), SLOT(changedNumberOfVertices(int)) );

    connect(startX, SIGNAL(valueChanged(int)), SLOT(numericChangedStart(int)) );
    connect(startY, SIGNAL(valueChanged(int)), SLOT(numericChangedStart(int)) );
    connect(stopX, SIGNAL(valueChanged(int)), SLOT(numericChangedStop(int)) );
    connect(stopY, SIGNAL(valueChanged(int)), SLOT(numericChangedStop(int)) );    
}


void InspectorSpatSymbolBezierSpline::numericChangedStart(int i) {
	if(ignoreSignals) {
		return;
	}
	
    // NO undo/redo!
    
    QPolygonF p = element->vertices();
    if(p.size() <= 1) {
    	return;
    }
    QPointF P = QPointF(startX->value(),startY->value())/100.;
    QPointF diff = p[0] - P;
    p[0] = P;
    p[1] -= diff;
    
    element->setVertices(p);
    
    // Fake-update
    Score* score = element->score();
    score->startCmd();
    score->endCmd();
    mscore->endCmd();
}

void InspectorSpatSymbolBezierSpline::numericChangedStop(int i) {
    
	if(ignoreSignals) {
		return;
	}
	
    // NO undo/redo!
    
    QPolygonF p = element->vertices();
    if(p.size() <= 1) {
    	return;
    }
    QPointF P = QPointF(stopX->value(),stopY->value())/100.;
    QPointF diff = p[p.size()-2] - P;
    p[p.size()-2] = P;
    p[p.size()-1] -= diff;    
    element->setVertices(p);
    
    // Fake-update
    Score* score = element->score();
    score->startCmd();
    score->endCmd();
    mscore->endCmd();
}

void InspectorSpatSymbolBezierSpline::changedNumberOfVertices(int i) {
	if(ignoreSignals) {
		return;
	}
	
	// NO undo/redo!!!
	QPolygonF p = element->vertices();
	p.resize((i+1)*2);
	element->setVertices(p);
    
    // Fake-update
    Score* score = element->score();
    score->startCmd();
    score->endCmd();
    mscore->endCmd();
}

void InspectorSpatSymbolBezierSpline::changedRotations(int i) {
	if(ignoreSignals) {
		return;
	}
	
	Score* score = element->score();
    score->startCmd();
    score->undoChangeProperty(element, P_ROTATIONS, i);
    score->endCmd();
    mscore->endCmd();
}

void InspectorSpatSymbolBezierSpline::radarChanged(QPolygonF p) {
	if(ignoreSignals) {
		return;
	}

    // NO undo/redo!
    element->setVertices(p);
    
    // Fake-update
    Score* score = element->score();
    score->startCmd();
    score->endCmd();
    mscore->endCmd();
    
}

void InspectorSpatSymbolBezierSpline::setElement(Element *e) {
	InspectorSpatSymbolExtend::setElement(e);
	
	ignoreSignals = true;
    
    QPolygonF p = element->vertices();
    radar->setPolygon(p);
	vertices->setValue(p.size()/2 - 1);
    
    
	rotations->setValue(element->rotations());
	if (p.size() >= 2) {
	    startX->setValue(round(p.first().x()*100.));
    	startY->setValue(round(p.first().y()*100.));
    	stopX->setValue(round(p[p.size()-2].x()*100.));
    	stopY->setValue(round(p[p.size()-2].y()*100.));
    }
    
    	
    ignoreSignals = false;
}


#pragma mark -
#pragma mark Line

InspectorSpatSymbolLine::InspectorSpatSymbolLine(QWidget *parent) : InspectorSpatSymbolExtend(parent) {
    setupUi(this);

	radar = new Radar2DInputPolygon(this);
	
	gridLayout->addWidget(radar, 3,0,1,-1);
    
    
    connect(radar, SIGNAL(polygonChanged(QPolygonF)), SLOT(radarChanged(QPolygonF)) );
    connect(rotations, SIGNAL(valueChanged(int)), SLOT(changedRotations(int)) );
    
    connect(startX, SIGNAL(valueChanged(int)), SLOT(numericChangedStart(int)) );
    connect(startY, SIGNAL(valueChanged(int)), SLOT(numericChangedStart(int)) );
    connect(stopX, SIGNAL(valueChanged(int)), SLOT(numericChangedStop(int)) );
    connect(stopY, SIGNAL(valueChanged(int)), SLOT(numericChangedStop(int)) );
}

void InspectorSpatSymbolLine::numericChangedStart(int i) {
	if(ignoreSignals) {
		return;
	}
	
	Score* score = element->score();
    score->startCmd();
    score->undoChangeProperty(element, P_START_POSITION, QPointF(startX->value(),startY->value())/100.);
    score->endCmd();
    mscore->endCmd();
}

void InspectorSpatSymbolLine::numericChangedStop(int i) {
    
	if(ignoreSignals) {
		return;
	}
	
	Score* score = element->score();
    score->startCmd();
    score->undoChangeProperty(element, P_STOP_POSITION, QPointF(stopX->value(),stopY->value())/100.);
    score->endCmd();
    mscore->endCmd();
}


void InspectorSpatSymbolLine::changedRotations(int i) {
	if(ignoreSignals) {
		return;
	}
	
	Score* score = element->score();
    score->startCmd();
    score->undoChangeProperty(element, P_ROTATIONS, i);
    score->endCmd();
    mscore->endCmd();
}

void InspectorSpatSymbolLine::radarChanged(QPolygonF p) {
	if(ignoreSignals) {
		return;
	}

    if (p[0] != element->startPosition() || p[1] != element->stopPosition()) {
        Score* score     = element->score();
        score->startCmd();
        score->undoChangeProperty(element, P_START_POSITION, p[0]);
        score->undoChangeProperty(element, P_STOP_POSITION, p[1]);
        score->endCmd();
        mscore->endCmd();
    }	
}

void InspectorSpatSymbolLine::setElement(Element *e) {
	InspectorSpatSymbolExtend::setElement(e);
	
	ignoreSignals = true;
    
    QPolygonF p(2);
    p[0] = element->startPosition();
    p[1] = element->stopPosition();
    radar->setPolygon(p);
    
    startX->setValue(round(element->startPosition().x()*100.));
    startY->setValue(round(element->startPosition().y()*100.));
    stopX->setValue(round(element->stopPosition().x()*100.));
    stopY->setValue(round(element->stopPosition().y()*100.));
    
	rotations->setValue(element->rotations());
	
    ignoreSignals = false;
}

#pragma mark -
#pragma mark ModifierMarkov

InspectorSpatSymbolModifierMarkov::InspectorSpatSymbolModifierMarkov(QWidget *parent) : InspectorSpatSymbolExtend(parent) {
    setupUi(this);

    connect(radius, SIGNAL(valueChanged(int)), SLOT(numericChangedRadius(int)) );
    
    connect(startX, SIGNAL(valueChanged(int)), SLOT(numericChangedStart(int)) );
    connect(startY, SIGNAL(valueChanged(int)), SLOT(numericChangedStart(int)) );
    connect(stopX, SIGNAL(valueChanged(int)), SLOT(numericChangedStop(int)) );
    connect(stopY, SIGNAL(valueChanged(int)), SLOT(numericChangedStop(int)) );
}

void InspectorSpatSymbolModifierMarkov::numericChangedStart(int i) {
	if(ignoreSignals) {
		return;
	}
	
	Score* score = element->score();
    score->startCmd();
    score->undoChangeProperty(element, P_START_POSITION, QPointF(startX->value(),startY->value())/100.);
    score->endCmd();
    mscore->endCmd();
}

void InspectorSpatSymbolModifierMarkov::numericChangedStop(int i) {
    
	if(ignoreSignals) {
		return;
	}
	
	Score* score = element->score();
    score->startCmd();
    score->undoChangeProperty(element, P_STOP_POSITION, QPointF(stopX->value(),stopY->value())/100.);
    score->endCmd();
    mscore->endCmd();
}



void InspectorSpatSymbolModifierMarkov::numericChangedRadius(int i) {
	if(ignoreSignals) {
		return;
	}
	
	Score* score = element->score();
    score->startCmd();
    score->undoChangeProperty(element, P_RADIUS, i/100.);
    score->endCmd();
    mscore->endCmd();
}

void InspectorSpatSymbolModifierMarkov::setElement(Element *e) {
	InspectorSpatSymbolExtend::setElement(e);
	
	ignoreSignals = true;
    
    
    startX->setValue(round(element->startPosition().x()*100.));
    startY->setValue(round(element->startPosition().y()*100.));
    stopX->setValue(round(element->stopPosition().x()*100.));
    stopY->setValue(round(element->stopPosition().y()*100.));
    
	radius->setValue(round(element->radius()*100.));
	
    ignoreSignals = false;
}

#pragma mark -
#pragma mark Polygon

InspectorSpatSymbolPolygon::InspectorSpatSymbolPolygon(QWidget *parent) : InspectorSpatSymbolExtend(parent) {
    setupUi(this);

	radar = new Radar2DInputPolygon(this);
	
	gridLayout->addWidget(radar, 5,0,1,-1);
    
    
    connect(radar, SIGNAL(polygonChanged(QPolygonF)), SLOT(radarChanged(QPolygonF)) );
    connect(rotations, SIGNAL(valueChanged(int)), SLOT(changedRotations(int)) );
    connect(vertices, SIGNAL(valueChanged(int)), SLOT(changedNumberOfVertices(int)) );
        
    connect(startX, SIGNAL(valueChanged(int)), SLOT(numericChangedStart(int)) );
    connect(startY, SIGNAL(valueChanged(int)), SLOT(numericChangedStart(int)) );
    connect(stopX, SIGNAL(valueChanged(int)), SLOT(numericChangedStop(int)) );
    connect(stopY, SIGNAL(valueChanged(int)), SLOT(numericChangedStop(int)) );
}

void InspectorSpatSymbolPolygon::numericChangedStart(int i) {
	if(ignoreSignals) {
		return;
	}
	
    // NO undo/redo!
    
    QPolygonF p = element->vertices();
    if(p.size() < 1) {
    	return;
    }
    p[0] = QPointF(startX->value(),startY->value())/100.;
    
    element->setVertices(p);
    
    // Fake-update
    Score* score = element->score();
    score->startCmd();
    score->endCmd();
    mscore->endCmd();
}

void InspectorSpatSymbolPolygon::numericChangedStop(int i) {
    
	if(ignoreSignals) {
		return;
	}
	
    // NO undo/redo!
    
    QPolygonF p = element->vertices();
    if(p.size() < 1) {
    	return;
    }
    p[p.size()-1] = QPointF(stopX->value(),stopY->value())/100.;
    element->setVertices(p);
    
    // Fake-update
    Score* score = element->score();
    score->startCmd();
    score->endCmd();
    mscore->endCmd();
}


void InspectorSpatSymbolPolygon::changedNumberOfVertices(int i) {
	if(ignoreSignals) {
		return;
	}
	
	// NO undo/redo!!!
	QPolygonF p = element->vertices();
	p.resize(i);
	element->setVertices(p);
    
    // Fake-update
    Score* score = element->score();
    score->startCmd();
    score->endCmd();
    mscore->endCmd();
}

void InspectorSpatSymbolPolygon::changedRotations(int i) {
	if(ignoreSignals) {
		return;
	}
	
	Score* score = element->score();
    score->startCmd();
    score->undoChangeProperty(element, P_ROTATIONS, i);
    score->endCmd();
    mscore->endCmd();
}

void InspectorSpatSymbolPolygon::radarChanged(QPolygonF p) {
	if(ignoreSignals) {
		return;
	}

    // NO undo/redo!
    element->setVertices(p);
    
    // Fake-update
    Score* score = element->score();
    score->startCmd();
    score->endCmd();
    mscore->endCmd();
    
}

void InspectorSpatSymbolPolygon::setElement(Element *e) {
	InspectorSpatSymbolExtend::setElement(e);
	
	ignoreSignals = true;
    
    QPolygonF p = element->vertices();
    radar->setPolygon(p);
    radar->_closePolygon = (element->spatType() == SPATTYPE_POLYGON || element->spatType() == SPATTYPE_POLYGON_CLOSED);
    
	vertices->setValue(p.size());
    
	rotations->setValue(element->rotations());
    if(p.size() >= 1) {
        startX->setValue(round(p.first().x()*100.));
        startY->setValue(round(p.first().y()*100.));
        stopX->setValue(round(p.last().x()*100.));
        stopY->setValue(round(p.last().y()*100.));
    }
	
    ignoreSignals = false;
}


#pragma mark -
#pragma mark Circle

InspectorSpatSymbolCircle::InspectorSpatSymbolCircle(QWidget *parent)
:  InspectorSpatSymbolExtend(parent) {
	setupUi(this);
	
    connect(centerX, SIGNAL(valueChanged(int)), SLOT(numericChangedCenter(int)) );
    connect(centerY, SIGNAL(valueChanged(int)), SLOT(numericChangedCenter(int)) );
    
    connect(radius, SIGNAL(valueChanged(int)), SLOT(numericChangedRadius(int)) );
    connect(direction, SIGNAL(currentIndexChanged(int)), SLOT(numericChangedDirection(int)) );
    
    connect(rotations, SIGNAL(valueChanged(int)), SLOT(changedRotations(int)) );
    
    connect(startAngle, SIGNAL(valueChanged(int)), SLOT(numericChangedStartAngle(int)) );
    connect(stopAngle, SIGNAL(valueChanged(int)), SLOT(numericChangedStopAngle(int)) );
}

void InspectorSpatSymbolCircle::numericChangedCenter(int i) {
	if(ignoreSignals) {
		return;
	}

	Score* score = element->score();
    score->startCmd();
    score->undoChangeProperty(element, P_CENTER_POSITION, QPointF(centerX->value(),centerY->value())/100.);
    score->endCmd();
    mscore->endCmd();
}


void InspectorSpatSymbolCircle::numericChangedRadius(int i) {
	if(ignoreSignals) {
		return;
	}
	
	Score* score = element->score();
    score->startCmd();
    score->undoChangeProperty(element, P_RADIUS, i/100.);
    score->endCmd();
    mscore->endCmd();
}


void InspectorSpatSymbolCircle::numericChangedDirection(int i) {
	if(ignoreSignals) {
		return;
	}

	Score* score = element->score();
    score->startCmd();
    score->undoChangeProperty(element, P_SPAT_DIRECTION, i);
    score->endCmd();
    mscore->endCmd();
}


void InspectorSpatSymbolCircle::changedRotations(int i) {
	if(ignoreSignals) {
		return;
	}
	
	Score* score = element->score();
    score->startCmd();
    score->undoChangeProperty(element, P_ROTATIONS, i);
    score->endCmd();
    mscore->endCmd();
}


void InspectorSpatSymbolCircle::numericChangedStartAngle(int i) {
	if(ignoreSignals) {
		return;
	}
	
	Score* score = element->score();
    score->startCmd();
    score->undoChangeProperty(element, P_START_ANGLE, (qreal) i);
    score->endCmd();
    mscore->endCmd();
}


void InspectorSpatSymbolCircle::numericChangedStopAngle(int i) {
	if(ignoreSignals) {
		return;
	}
	
	Score* score = element->score();
    score->startCmd();
    score->undoChangeProperty(element, P_STOP_ANGLE, (qreal) i);
    score->endCmd();
    mscore->endCmd();
}

void InspectorSpatSymbolCircle::setElement(Element *e) {
	InspectorSpatSymbolExtend::setElement(e);
	
	ignoreSignals = true;
	
	centerX->setValue(round(element->centerPosition().x()*100.));
	centerY->setValue(round(element->centerPosition().y()*100.));
	radius->setValue(round(element->radius()*100.));
	direction->setCurrentIndex(element->direction());
	rotations->setValue(element->rotations());
	startAngle->setValue(round(element->startAngle()));
	stopAngle->setValue(round(element->stopAngle()));
	
	ignoreSignals = false;
}


#pragma mark -
#pragma mark ModifierSine

InspectorSpatSymbolModifierSine::InspectorSpatSymbolModifierSine(QWidget *parent)
:  InspectorSpatSymbolExtend(parent) {
	setupUi(this);
	
    connect(radius, SIGNAL(valueChanged(int)), SLOT(numericChangedRadius(int)) );
    connect(rotations, SIGNAL(valueChanged(int)), SLOT(changedRotations(int)) );
}


void InspectorSpatSymbolModifierSine::numericChangedRadius(int i) {
	if(ignoreSignals) {
		return;
	}
	
	Score* score = element->score();
    score->startCmd();
    score->undoChangeProperty(element, P_RADIUS, i/100.);
    score->endCmd();
    mscore->endCmd();
}


void InspectorSpatSymbolModifierSine::changedRotations(int i) {
	if(ignoreSignals) {
		return;
	}
	
	Score* score = element->score();
    score->startCmd();
    score->undoChangeProperty(element, P_ROTATIONS, i);
    score->endCmd();
    mscore->endCmd();
}



void InspectorSpatSymbolModifierSine::setElement(Element *e) {
	InspectorSpatSymbolExtend::setElement(e);
	
	ignoreSignals = true;
	
	radius->setValue(round(element->radius()*100.));
	rotations->setValue(element->rotations());
	
	ignoreSignals = false;
}


#pragma mark -
#pragma mark Spiral

InspectorSpatSymbolSpiral::InspectorSpatSymbolSpiral(QWidget *parent)
:  InspectorSpatSymbolExtend(parent) {
	setupUi(this);
	
    connect(centerX, SIGNAL(valueChanged(int)), SLOT(numericChangedCenter(int)) );
    connect(centerY, SIGNAL(valueChanged(int)), SLOT(numericChangedCenter(int)) );
    
    connect(radius, SIGNAL(valueChanged(int)), SLOT(numericChangedRadius(int)) );
    connect(innerRadius, SIGNAL(valueChanged(int)), SLOT(numericChangedInnerRadius(int)) );
    connect(direction, SIGNAL(currentIndexChanged(int)), SLOT(numericChangedDirection(int)) );
    
    connect(rotations, SIGNAL(valueChanged(int)), SLOT(changedRotations(int)) );
    
    connect(startAngle, SIGNAL(valueChanged(int)), SLOT(numericChangedStartAngle(int)) );
    connect(stopAngle, SIGNAL(valueChanged(int)), SLOT(numericChangedStopAngle(int)) );
}

void InspectorSpatSymbolSpiral::numericChangedCenter(int i) {
	if(ignoreSignals) {
		return;
	}

	Score* score = element->score();
    score->startCmd();
    score->undoChangeProperty(element, P_CENTER_POSITION, QPointF(centerX->value(),centerY->value())/100.);
    score->endCmd();
    mscore->endCmd();
}


void InspectorSpatSymbolSpiral::numericChangedRadius(int i) {
	if(ignoreSignals) {
		return;
	}
	
	Score* score = element->score();
    score->startCmd();
    score->undoChangeProperty(element, P_RADIUS, i/100.);
    score->endCmd();
    mscore->endCmd();
}


void InspectorSpatSymbolSpiral::numericChangedInnerRadius(int i) {
	if(ignoreSignals) {
		return;
	}
	
	Score* score = element->score();
    score->startCmd();
    score->undoChangeProperty(element, P_INNER_RADIUS, i/100.);
    score->endCmd();
    mscore->endCmd();
}


void InspectorSpatSymbolSpiral::numericChangedDirection(int i) {
	if(ignoreSignals) {
		return;
	}

	Score* score = element->score();
    score->startCmd();
    score->undoChangeProperty(element, P_SPAT_DIRECTION, i);
    score->endCmd();
    mscore->endCmd();
}


void InspectorSpatSymbolSpiral::changedRotations(int i) {
	if(ignoreSignals) {
		return;
	}
	
	Score* score = element->score();
    score->startCmd();
    score->undoChangeProperty(element, P_ROTATIONS, i);
    score->endCmd();
    mscore->endCmd();
}


void InspectorSpatSymbolSpiral::numericChangedStartAngle(int i) {
	if(ignoreSignals) {
		return;
	}
	
	Score* score = element->score();
    score->startCmd();
    score->undoChangeProperty(element, P_START_ANGLE, (qreal) i);
    score->endCmd();
    mscore->endCmd();
}


void InspectorSpatSymbolSpiral::numericChangedStopAngle(int i) {
	if(ignoreSignals) {
		return;
	}
	
	Score* score = element->score();
    score->startCmd();
    score->undoChangeProperty(element, P_STOP_ANGLE, (qreal) i);
    score->endCmd();
    mscore->endCmd();
}

void InspectorSpatSymbolSpiral::setElement(Element *e) {
	InspectorSpatSymbolExtend::setElement(e);
	
	ignoreSignals = true;
	
	centerX->setValue(round(element->centerPosition().x()*100.));
	centerY->setValue(round(element->centerPosition().y()*100.));
	radius->setValue(round(element->radius()*100.));
	innerRadius->setValue(round(element->innerRadius()*100.));
	direction->setCurrentIndex(element->direction());
	rotations->setValue(element->rotations());
	startAngle->setValue(round(element->startAngle()));
	stopAngle->setValue(round(element->stopAngle()));
	
	ignoreSignals = false;
}

#pragma mark -
#pragma mark BernoulliLemniscate

InspectorSpatSymbolBernoulliLemniscate::InspectorSpatSymbolBernoulliLemniscate(QWidget *parent)
:  InspectorSpatSymbolExtend(parent) {
	setupUi(this);
	
    connect(centerX, SIGNAL(valueChanged(int)), SLOT(numericChangedCenter(int)) );
    connect(centerY, SIGNAL(valueChanged(int)), SLOT(numericChangedCenter(int)) );
    
    connect(radius, SIGNAL(valueChanged(int)), SLOT(numericChangedRadius(int)) );
    connect(innerRadius, SIGNAL(valueChanged(int)), SLOT(numericChangedInnerRadius(int)) );
    connect(direction, SIGNAL(currentIndexChanged(int)), SLOT(numericChangedDirection(int)) );
    
    connect(rotations, SIGNAL(valueChanged(int)), SLOT(changedRotations(int)) );
    
    connect(startAngle, SIGNAL(valueChanged(int)), SLOT(numericChangedStartAngle(int)) );
    connect(stopAngle, SIGNAL(valueChanged(int)), SLOT(numericChangedStopAngle(int)) );
}

void InspectorSpatSymbolBernoulliLemniscate::numericChangedCenter(int i) {
	if(ignoreSignals) {
		return;
	}

	Score* score = element->score();
    score->startCmd();
    score->undoChangeProperty(element, P_CENTER_POSITION, QPointF(centerX->value(),centerY->value())/100.);
    score->endCmd();
    mscore->endCmd();
}


void InspectorSpatSymbolBernoulliLemniscate::numericChangedRadius(int i) {
	if(ignoreSignals) {
		return;
	}
	
	Score* score = element->score();
    score->startCmd();
    score->undoChangeProperty(element, P_RADIUS, i/100.);
    score->endCmd();
    mscore->endCmd();
}


void InspectorSpatSymbolBernoulliLemniscate::numericChangedInnerRadius(int i) {
	if(ignoreSignals) {
		return;
	}
	
	Score* score = element->score();
    score->startCmd();
    score->undoChangeProperty(element, P_INNER_RADIUS, i/100.);
    score->endCmd();
    mscore->endCmd();
}


void InspectorSpatSymbolBernoulliLemniscate::numericChangedDirection(int i) {
	if(ignoreSignals) {
		return;
	}

	Score* score = element->score();
    score->startCmd();
    score->undoChangeProperty(element, P_SPAT_DIRECTION, i);
    score->endCmd();
    mscore->endCmd();
}


void InspectorSpatSymbolBernoulliLemniscate::changedRotations(int i) {
	if(ignoreSignals) {
		return;
	}
	
	Score* score = element->score();
    score->startCmd();
    score->undoChangeProperty(element, P_ROTATIONS, i);
    score->endCmd();
    mscore->endCmd();
}


void InspectorSpatSymbolBernoulliLemniscate::numericChangedStartAngle(int i) {
	if(ignoreSignals) {
		return;
	}
	
	Score* score = element->score();
    score->startCmd();
    score->undoChangeProperty(element, P_START_ANGLE, (qreal) i);
    score->endCmd();
    mscore->endCmd();
}


void InspectorSpatSymbolBernoulliLemniscate::numericChangedStopAngle(int i) {
	if(ignoreSignals) {
		return;
	}
	
	Score* score = element->score();
    score->startCmd();
    score->undoChangeProperty(element, P_STOP_ANGLE, (qreal) i);
    score->endCmd();
    mscore->endCmd();
}

void InspectorSpatSymbolBernoulliLemniscate::setElement(Element *e) {
	InspectorSpatSymbolExtend::setElement(e);
	
	ignoreSignals = true;
	
	centerX->setValue(round(element->centerPosition().x()*100.));
	centerY->setValue(round(element->centerPosition().y()*100.));
	radius->setValue(round(element->radius()*100.));
	innerRadius->setValue(round(element->innerRadius()*100.));
	direction->setCurrentIndex(element->direction());
	rotations->setValue(element->rotations());
	startAngle->setValue(round(element->startAngle()));
	stopAngle->setValue(round(element->stopAngle()));
	
	ignoreSignals = false;
}

#pragma mark -
#pragma mark Slinky

InspectorSpatSymbolSlinky::InspectorSpatSymbolSlinky(QWidget *parent)
:  InspectorSpatSymbolExtend(parent) {
	setupUi(this);
	
    connect(startX, SIGNAL(valueChanged(int)), SLOT(numericChangedStart(int)) );
    connect(startY, SIGNAL(valueChanged(int)), SLOT(numericChangedStart(int)) );
    connect(stopX, SIGNAL(valueChanged(int)), SLOT(numericChangedStop(int)) );
    connect(stopY, SIGNAL(valueChanged(int)), SLOT(numericChangedStop(int)) );
    
    connect(radius, SIGNAL(valueChanged(int)), SLOT(numericChangedRadius(int)) );
    connect(direction, SIGNAL(currentIndexChanged(int)), SLOT(numericChangedDirection(int)) );
    
    connect(rotations, SIGNAL(valueChanged(int)), SLOT(changedRotations(int)) );
    
    connect(startAngle, SIGNAL(valueChanged(int)), SLOT(numericChangedStartAngle(int)) );
    connect(stopAngle, SIGNAL(valueChanged(int)), SLOT(numericChangedStopAngle(int)) );
}

void InspectorSpatSymbolSlinky::numericChangedStart(int i) {
	if(ignoreSignals) {
		return;
	}
	
	Score* score = element->score();
    score->startCmd();
    score->undoChangeProperty(element, P_START_POSITION, QPointF(startX->value(),startY->value())/100.);
    score->endCmd();
    mscore->endCmd();
}

void InspectorSpatSymbolSlinky::numericChangedStop(int i) {
    
	if(ignoreSignals) {
		return;
	}
	
	Score* score = element->score();
    score->startCmd();
    score->undoChangeProperty(element, P_STOP_POSITION, QPointF(stopX->value(),stopY->value())/100.);
    score->endCmd();
    mscore->endCmd();
}

void InspectorSpatSymbolSlinky::numericChangedRadius(int i) {
	if(ignoreSignals) {
		return;
	}
	
	Score* score = element->score();
    score->startCmd();
    score->undoChangeProperty(element, P_RADIUS, i/100.);
    score->endCmd();
    mscore->endCmd();
}


void InspectorSpatSymbolSlinky::numericChangedDirection(int i) {
	if(ignoreSignals) {
		return;
	}

	Score* score = element->score();
    score->startCmd();
    score->undoChangeProperty(element, P_SPAT_DIRECTION, i);
    score->endCmd();
    mscore->endCmd();
}


void InspectorSpatSymbolSlinky::changedRotations(int i) {
	if(ignoreSignals) {
		return;
	}
	
	Score* score = element->score();
    score->startCmd();
    score->undoChangeProperty(element, P_ROTATIONS, i);
    score->endCmd();
    mscore->endCmd();
}


void InspectorSpatSymbolSlinky::numericChangedStartAngle(int i) {
	if(ignoreSignals) {
		return;
	}
	
	Score* score = element->score();
    score->startCmd();
    score->undoChangeProperty(element, P_START_ANGLE, (qreal) i);
    score->endCmd();
    mscore->endCmd();
}


void InspectorSpatSymbolSlinky::numericChangedStopAngle(int i) {
	if(ignoreSignals) {
		return;
	}
	
	Score* score = element->score();
    score->startCmd();
    score->undoChangeProperty(element, P_STOP_ANGLE, (qreal) i);
    score->endCmd();
    mscore->endCmd();
}

void InspectorSpatSymbolSlinky::setElement(Element *e) {
	InspectorSpatSymbolExtend::setElement(e);
	
	ignoreSignals = true;
	
    startX->setValue(round(element->startPosition().x()*100.));
    startY->setValue(round(element->startPosition().y()*100.));
    stopX->setValue(round(element->stopPosition().x()*100.));
    stopY->setValue(round(element->stopPosition().y()*100.));
	radius->setValue(round(element->radius()*100.));
	direction->setCurrentIndex(element->direction());
	rotations->setValue(element->rotations());
	startAngle->setValue(round(element->startAngle()));
	stopAngle->setValue(round(element->stopAngle()));
	
	ignoreSignals = false;
}

#pragma mark -
#pragma mark Position

InspectorSpatSymbolPosition::InspectorSpatSymbolPosition(QWidget *parent)
:  InspectorSpatSymbolExtend(parent) {
	setupUi(this);
	
    connect(centerX, SIGNAL(valueChanged(int)), SLOT(numericChangedCenter(int)) );
    connect(centerY, SIGNAL(valueChanged(int)), SLOT(numericChangedCenter(int)) );
}

void InspectorSpatSymbolPosition::numericChangedCenter(int i) {
	if(ignoreSignals) {
		return;
	}

	Score* score = element->score();
    score->startCmd();
    score->undoChangeProperty(element, P_CENTER_POSITION, QPointF(centerX->value(),centerY->value())/100.);
    score->endCmd();
    mscore->endCmd();
}

void InspectorSpatSymbolPosition::setElement(Element *e) {
	InspectorSpatSymbolExtend::setElement(e);
	
	ignoreSignals = true;
	
	centerX->setValue(round(element->centerPosition().x()*100.));
	centerY->setValue(round(element->centerPosition().y()*100.));
	
	ignoreSignals = false;
}

#pragma mark -
#pragma mark Position Alternation

InspectorSpatSymbolPositionAlternation::InspectorSpatSymbolPositionAlternation(QWidget *parent)
:  InspectorSpatSymbolExtend(parent) {
	setupUi(this);
	
    connect(rotations, SIGNAL(valueChanged(int)), SLOT(changedRotations(int)) );
    
    connect(startX, SIGNAL(valueChanged(int)), SLOT(numericChangedStart(int)) );
    connect(startY, SIGNAL(valueChanged(int)), SLOT(numericChangedStart(int)) );
    connect(stopX, SIGNAL(valueChanged(int)), SLOT(numericChangedStop(int)) );
    connect(stopY, SIGNAL(valueChanged(int)), SLOT(numericChangedStop(int)) );
}

void InspectorSpatSymbolPositionAlternation::changedRotations(int i) {
	if(ignoreSignals) {
		return;
	}
	
	Score* score = element->score();
    score->startCmd();
    score->undoChangeProperty(element, P_ROTATIONS, i);
    score->endCmd();
    mscore->endCmd();
}

void InspectorSpatSymbolPositionAlternation::numericChangedStart(int i) {
	if(ignoreSignals) {
		return;
	}
	
	Score* score = element->score();
    score->startCmd();
    score->undoChangeProperty(element, P_START_POSITION, QPointF(startX->value(),startY->value())/100.);
    score->endCmd();
    mscore->endCmd();
}

void InspectorSpatSymbolPositionAlternation::numericChangedStop(int i) {
    
	if(ignoreSignals) {
		return;
	}
	
	Score* score = element->score();
    score->startCmd();
    score->undoChangeProperty(element, P_STOP_POSITION, QPointF(stopX->value(),stopY->value())/100.);
    score->endCmd();
    mscore->endCmd();
}

void InspectorSpatSymbolPositionAlternation::setElement(Element *e) {
	InspectorSpatSymbolExtend::setElement(e);
	
	ignoreSignals = true;
	
    startX->setValue(round(element->startPosition().x()*100.));
    startY->setValue(round(element->startPosition().y()*100.));
    stopX->setValue(round(element->stopPosition().x()*100.));
    stopY->setValue(round(element->stopPosition().y()*100.));
	rotations->setValue(element->rotations());
	
	ignoreSignals = false;
}

#pragma mark -
#pragma mark OSCMessage

InspectorSpatSymbolOSCMessage::InspectorSpatSymbolOSCMessage(QWidget *parent)
:  InspectorSpatSymbolExtend(parent) {
	setupUi(this);
	
    connect(message1, SIGNAL(textEdited(const QString&)), SLOT(message1Changed(const QString&)) );
    connect(message2, SIGNAL(textEdited(const QString&)), SLOT(message2Changed(const QString&)) );
}

void InspectorSpatSymbolOSCMessage::message1Changed(const QString& m) {
	if(ignoreSignals) {
		return;
	}

	Score* score = element->score();
    score->startCmd();
    score->undoChangeProperty(element, P_OSC_MESSAGE_1, m);
    score->endCmd();
    mscore->endCmd();
}

void InspectorSpatSymbolOSCMessage::message2Changed(const QString& m) {
	if(ignoreSignals) {
		return;
	}

	Score* score = element->score();
    score->startCmd();
    score->undoChangeProperty(element, P_OSC_MESSAGE_2, m);
    score->endCmd();
    mscore->endCmd();
}

void InspectorSpatSymbolOSCMessage::setElement(Element *e) {
	InspectorSpatSymbolExtend::setElement(e);
	
	ignoreSignals = true;
	
	message1->setText(element->oscMessage1());
	message2->setText(element->oscMessage2());
	
	ignoreSignals = false;
}

#pragma mark -
#pragma mark Global Inspector

InspectorSpatializationSymbol::InspectorSpatializationSymbol(QWidget* parent)
: InspectorBase(parent)
{
    iElement = new InspectorElementElement(this);
    layout->addWidget(iElement);
    
    e = 0;
}


void InspectorSpatializationSymbol::setElement(Element* element)
{
    SpatSymbol *s = static_cast<SpatSymbol*>(element);
    
    
    iElement->setElement(element);
    
    if(!e || e->spatType() != s->spatType() ) {
    	
    	foreach(InspectorSpatSymbolExtend *subInspector, inspectors) {
    		layout->removeWidget(subInspector);
    		delete subInspector;
    	}
    	inspectors = QList<InspectorSpatSymbolExtend*>();
    	
    	inspectors.append(new InspectorSpatSymbolBase(this));
    	
    	switch(s->spatType()) {
    		case SPATTYPE_LINE_2D:
    		case SPATTYPE_LINE_CLOSED:
    			inspectors.append(new InspectorSpatSymbolLine(this));
    			break;
    			
    		case SPATTYPE_POLYGON:
    		case SPATTYPE_POLYGON_CLOSED:
    		case SPATTYPE_POLYLINE:
    		case SPATTYPE_POLYLINE_CLOSED:
    			inspectors.append(new InspectorSpatSymbolPolygon(this));
    			break;
    			
    		case SPATTYPE_CIRCLE_2D:
    		case SPATTYPE_CIRCLE_BF:
    			inspectors.append(new InspectorSpatSymbolCircle(this));
    			break;
    			
    		case SPATTYPE_SPIRAL_IN2OUT:
    		case SPATTYPE_SPIRAL_OUT2IN:
    		case SPATTYPE_SPIRAL_IN2OUT_CLOSED:
    		case SPATTYPE_SPIRAL_OUT2IN_CLOSED:
    			inspectors.append(new InspectorSpatSymbolSpiral(this));
    			break;
    			
    			
    		case SPATTYPE_CIRCLE_SLINKY:
      		case SPATTYPE_CIRCLE_SLINKY_CLOSED:
    			inspectors.append(new InspectorSpatSymbolSlinky(this));
    			break;
    		
    			
    		case SPATTYPE_POSITION_2D:
    			inspectors.append(new InspectorSpatSymbolPosition(this));
    			break;
    			
    		case SPATTYPE_POSITION_ALTERNATION:
    			inspectors.append(new InspectorSpatSymbolPositionAlternation(this));
    			break;
    			
    		case SPATTYPE_OSC_MESSAGE:
    			inspectors.append(new InspectorSpatSymbolOSCMessage(this));
    			break;
    			
    		case SPATTYPE_BEZIER_CUBIC:
    		case SPATTYPE_BEZIER_CUBIC_CLOSED:
    			inspectors.append(new InspectorSpatSymbolBezier(this));
    			break;
    			
    		case SPATTYPE_BEZIER_CUBIC_SPLINE:
    		case SPATTYPE_BEZIER_CUBIC_SPLINE_CLOSED:
    			inspectors.append(new InspectorSpatSymbolBezierSpline(this));
    			break;
    			
    		case SPATTYPE_BERNOULLI_LEMNISCATE:
    			inspectors.append(new InspectorSpatSymbolBernoulliLemniscate(this));
    			break;
    			
    		case SPATTYPE_MODIFIER_MARKOV:
    			inspectors.append(new InspectorSpatSymbolModifierMarkov(this));
    			break;
                
    		case SPATTYPE_MODIFIER_SINE:
    		case SPATTYPE_MODIFIER_SAW:
    		case SPATTYPE_MODIFIER_SQUARE:
    		case SPATTYPE_MODIFIER_TRIANGLE:
    			inspectors.append(new InspectorSpatSymbolModifierSine(this));
    			break;
    	}
    	
    	foreach(InspectorSpatSymbolExtend *subInspector, inspectors) {
    		layout->addWidget(subInspector);
    	}
    }
    
    foreach(InspectorSpatSymbolExtend *subInspector, inspectors) {
        subInspector->setElement(element);
    }
    
    
    e = s;
}
