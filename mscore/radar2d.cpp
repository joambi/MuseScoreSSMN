
#include "Radar2D.h"
#include "libmscore/spatsymbol.h"


 Radar2D::Radar2D(QWidget *parent)
     : QWidget(parent)
 {
 }


/**
* Gerechnet wird in einem 100x100pixel Rechteck.
*/
QTransform Radar2D::displayTransform() const {
	QTransform t;
	
    int side = 200; //qMin(width(), height()) - 10;
	t.translate((qreal) width() / 2., (qreal) height() / 2.);
	t.scale((qreal) side / 2, (qreal) side / 2);
	
    return QTransform(1.,0.,0.,-1.,0.,0.) * t;
}

void Radar2D::paintEvent(QPaintEvent *)
 {
       
       QPainter painter(this);
       painter.setRenderHint(QPainter::Antialiasing);
       painter.setTransform(this->displayTransform());
       
       // Draw the circles
       painter.setPen(QColor(10,10,10));
       for(int i=1;i<=10;i++) {
             painter.drawArc(QRectF(-i*.1, -i*.1,.2*i, .2*i), 0, 360*16);
       }
       
       // Draw the lines.
       painter.setPen(QColor(0,0,0));
       painter.drawLine(QPointF(-1,0), QPointF(1,0));
       painter.drawLine(QPointF(0,1), QPointF(0,-1));
       
      
       // Draw the paths.
       QColor blue(0,0,255);
       painter.setPen(blue);
       foreach(QPainterPath p, paths) {
             painter.drawPath(p);
       }
       
       // Draw the endpoints.
       QBrush blueBrush(blue);
       painter.setBrush(blueBrush);
       foreach(QPointF p, endpoints) {
             painter.drawEllipse(QRectF(p.x()-.02, p.y()-.02, .04, .04));
       }
}

void Radar2D::setPaths(QList<QPainterPath> p) {
      paths = p;
      update();
}


void Radar2D::setEndpoints(QList<QPointF> p) {
      endpoints = p;
      update();
}


/**
* Gerechnet wird in einem 100x100pixel Rechteck.
*/
QTransform Radar2DFakeSphere::displayTransform() const {
	QTransform t;
	
    int side = 200; //qMin(width(), height()*2) - 10;
	t.translate((qreal) width() / 2., (qreal) height() / 2.*2);
	t.scale((qreal) side / 2, (qreal) side / 2);
	
    return QTransform(1.,0.,0.,-1.,0.,0.) * t;
}
 Radar2DFakeSphere::Radar2DFakeSphere(QWidget *parent)
     : Radar2D(parent)
 {
 }

Radar2DInputPolygon::Radar2DInputPolygon(QWidget *parent) : Radar2D(parent) {

	QPolygonF p;
	p.append(QPointF(0,0));
	p.append(QPointF(1,1));
	setPolygon(p);
	
	_closePolygon = false;
}

void Radar2DInputPolygon::paintEvent(QPaintEvent *e) {
	Radar2D::paintEvent(e);
	
	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing);
	painter.setTransform(this->displayTransform());	
	
	QPainterPath myPath;
	QPolygonF p = _polygon;
 	if(_closePolygon && _polygon.count()>0) {
 		p << p[0];
 	}
 	myPath.addPolygon(p);

    QColor blue(0,0,255);
    painter.setPen(blue);
    
    painter.drawPath(myPath);
    
    
 	// Add arrows for direction
    painter.resetTransform();
 	QPainterPath arrowPaths;
 	QTransform t = displayTransform();
 	for(int i=0; i < _polygon.count()-1;i++){
        
 		QPointF center = t * ( (_polygon[i+1] + _polygon[i]) * .5 );
 		QPointF v = QTransform(1.,0.,0.,-1.,0.,0.) * (_polygon[i+1] - _polygon[i]);
 		if(v == QPointF(0,0)) {
 			continue;
 		}
 		v = v / sqrt(v.x()*v.x() + v.y()*v.y());
 		
 		QPolygonF p;
 		p.append(center + QPointF(v.y(),-v.x())*5 - v*5);
 		p.append(center + v*5);
 		p.append(center -QPointF(v.y(),-v.x())*5 - v*5);
 		arrowPaths.addPolygon(p);
        
        // Stop after first arrows
        break;
 	}
 	painter.drawPath(arrowPaths);
 	painter.fillPath(arrowPaths,QBrush(blue));
}


void Radar2DInputPolygon::mousePressEvent ( QMouseEvent * e ) {
    QTransform t = displayTransform();
    QPolygonF p = polygon();
    qreal distance;
    int index = -1;
 	for(int i=0;i<p.count();i++) {
        qreal d = (e->posF() - t*p[i]).manhattanLength();
        if(d < 20) {
            // close enough for being a candidate
            if(index == -1 || d < distance) {
                distance = d;
                index = i;
            }
        }
 	} 
    _dragIndex = index;
}

void Radar2DInputPolygon::mouseMoveEvent ( QMouseEvent * e ) {
    if(_dragIndex == -1) { 
        return;
    }
    QTransform t = displayTransform();
    t = t.inverted();
    QPointF p = e->posF() * t;
    // Bound p inside [-1,1]^2
    p.setX(qMax(qMin(p.x(),1.),-1.));
    p.setY(qMax(qMin(p.y(),1.),-1.));
    
    _polygon[_dragIndex] = p;
    update();
}

void Radar2DInputPolygon::mouseReleaseEvent ( QMouseEvent * e ) {
	emit polygonChanged(_polygon);
}

Radar2DInputBezier::Radar2DInputBezier(QWidget *parent) : Radar2DInputPolygon(parent) {
}

void Radar2DInputBezier::paintEvent(QPaintEvent *e) {
	Radar2D::paintEvent(e);
	
	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing);
	painter.setTransform(this->displayTransform());	
	

    
    
    // Abort if we have not enough points or if there's a odd number of points
    if(_polygon.count() < 4 || _polygon.count() % 2 == 1) {
    	return;
    }
    
    // Create the vector with the line
    QPolygonF controlPoints;
    for(int i=0;i<_polygon.count();i++) {
    	if(i != 0 && i % 2 == 0 && (i+1) < _polygon.count()) {
    		QPointF a = _polygon.at(i);
    		QPointF b = _polygon.at(i+1);
    		controlPoints.append((a+a-b));
    	}
    	controlPoints.append(_polygon.at(i));
    }
    controlPoints.pop_back();
    

    QColor gray(30,30,30);
    painter.setPen(gray);    
    
    
    // Draw handle lines
    // First handle:
    painter.drawLine(_polygon[0], _polygon[1]);
    // Last handle:
    painter.drawLine(_polygon[_polygon.size()-2], 2*_polygon[_polygon.size()-2] - _polygon.last());
	// All others
	for(int i=3;i<_polygon.count()-2;i+=2) {
		painter.drawLine(2*_polygon[i-1] - _polygon[i], _polygon[i]);
	}
    
    
 	// Add arrows for direction on first line
    painter.resetTransform();
 	QPainterPath arrowPaths;
 	QTransform t = displayTransform();
 	if(controlPoints.count() >= 2) {
 		
 		QPointF center = t * ( (controlPoints[1] + controlPoints[0]) * .5 );
 		QPointF v = QTransform(1.,0.,0.,-1.,0.,0.) * (controlPoints[1] - controlPoints[0]);
 		if(v != QPointF(0,0)) {
            v = v / sqrt(v.x()*v.x() + v.y()*v.y());
 		
            QPolygonF p;
            p.append(center + QPointF(v.y(),-v.x())*5 - v*5);
            p.append(center + v*5);
            p.append(center -QPointF(v.y(),-v.x())*5 - v*5);
            arrowPaths.addPolygon(p);
        }
 	}
 	painter.drawPath(arrowPaths);
 	painter.fillPath(arrowPaths,QBrush(gray));
 	
 	
    if((controlPoints.count() -1)  %3 != 0) {
        return;
    }
    
 	for(int i=0;i+3<controlPoints.count();i+=3) {
 		QPainterPath myPath = QPainterPath();
    	myPath.moveTo(controlPoints[i]);
    	myPath.cubicTo(controlPoints[i+1],controlPoints[i+2],controlPoints[i+3]);
    	
    	QColor blue(0,0,255);
		painter.setTransform(this->displayTransform());	
    	painter.setPen(blue);
    	painter.drawPath(myPath);
    }
}

void Radar2DInputBezier::mousePressEvent ( QMouseEvent * e ) {
    QTransform t = displayTransform();
    QPolygonF p = polygon();
    qreal distance;
    int index = -1;
 	for(int i=0;i<p.count();i++) {
        qreal d = (e->posF() - t*p[i]).manhattanLength();
        if(d < 20) {
            // close enough for being a candidate
            if(index == -1 || d <= distance) {
                distance = d;
                index = i;
                _dragMirrorHandle = false;
            }
        }
        
        // Check if it's a mirror of the handle
        if(i % 2 == 1 && i > 1) {
            qreal d = (e->posF() - t*(2*p[i-1] - p[i]) ).manhattanLength();
			if(d < 20) {
				// close enough for being a candidate
				if(index == -1 || d <= distance) {
					distance = d;
					index = i;
					_dragMirrorHandle = true;
				}
			}
        }
        
 	} 
    _dragIndex = index;
}

void Radar2DInputBezier::mouseMoveEvent ( QMouseEvent * e ) {
    if(_dragIndex == -1) { 
        return;
    }
    QTransform t = displayTransform();
    t = t.inverted();
    QPointF p = e->posF() * t;
    // Bound p inside [-1,1]^2
    p.setX(qMax(qMin(p.x(),1.),-1.));
    p.setY(qMax(qMin(p.y(),1.),-1.));
    
    
    // If it's a mirror handle
    if(_dragMirrorHandle) {
    	p = 2*_polygon[_dragIndex-1] - p;
    }
    
    // If it's a point (not a controlPoint) move also the handle
    if(_dragIndex % 2 == 0) {
    	_polygon[_dragIndex+1] += p - _polygon[_dragIndex];
    }
    
    
    _polygon[_dragIndex] = p;
    update();
}