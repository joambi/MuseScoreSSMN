

#include "radar.h"
#include "radar2d.h"
#include "musescore.h"
#include "seq.h"
#include "libmscore/element.h"
#include "libmscore/mscore.h"
#include "libmscore/chord.h"
#include "libmscore/segment.h"
#include "libmscore/spatsymbol.h"

extern Seq *seq;

void Radar::setScore(Score *cs) {

	// Calculate the durations
	seq->calculateSpatDurations();

      QList<QPainterPath> paths;
      QList<QPointF> endpoints;
      
      QList<Element*> el;
      if (cs) {
            el = cs->selection().elements();
      }
      
      // We search all the chords, that are selected:
      QList<Chord*> chords;
      foreach(Element* e, el) {
            if(e->type() != Element::NOTE) {
                  // We are just interested in notes.
                  continue;
            }
            
            // Add the chord of the note, if it's not yet added.
            Chord* cr = static_cast<Chord*>(e->parent());
            if(!chords.contains(cr)) {
                  chords.append(cr);
            }
      }
      
      if(!chords.count()) {
            // No chords selected.
            
            // Let's check, if we have a spatsymbol selected.
            if(el.count() == 1 && el.first()->type() == Element::SPATIALIZATION_SYMBOL) {
                  // We have a spatsymbol. Let's just draw it's trace.
                  
                  SpatSymbol *s = static_cast<SpatSymbol*>(el.first());
                  // HACK!
                  s->_beingRendered = false;
                  int tick = s->tick();
                  QPainterPath path;
                  SpatCoordinates coord = s->calculateCoordinates(tick);
                  SpatCoordinates lastCoordinate = coord;
                  
                  path.moveTo(coord.x(),  coord.y());
                  // We walk along the trajectory until it's end.
                  while(true) {
                        tick++;
                        coord = s->calculateCoordinates(tick);
                        if(coord.ignore) {
                        	// Stop as soon as the symbol says so.
                        	break;
                        }
                        
                        if(lastCoordinate == coord) {
                              // Just ignore, if we're still on the same place.
                              continue;
                        }
                        
                        path.lineTo(coord.x(),  coord.y());
                        lastCoordinate = coord;
                  }
                  endpoints.append(QPointF(lastCoordinate.x(), lastCoordinate.y()));
                  paths.append(path);
            }
            radar2d->setPaths(paths);
            radar2d->setEndpoints(endpoints);
            return;
      }
      
      // We need a list of all available spatsymbols.
      QList<SpatSymbol*> spatSymbols;
      for (Segment* s = cs->firstSegment(); s; s = s->next1()) {
            foreach(Element* e, s->annotations()) {
                  if(!e || e->type() != Element::SPATIALIZATION_SYMBOL) {
                        continue;
                  }
                  SpatSymbol *spat = static_cast<SpatSymbol*>(e);
                  // HACK!
                  spat->_beingRendered = false;
                  spatSymbols.append(spat);
            }
      }
      
      // We process each chord and find the trajectory on it.
      foreach(Chord *c, chords) {
            
            bool seekingStart = true;
            QPainterPath path;
            SpatCoordinates lastCoordinate;
            int tick = c->tick();
            
            // We process as long as the duration of the chord is.
            while(tick < c->tick() + c->actualTicks()) {
                  // We try to find a spatsymbol, that handles the current tick.
                  SpatSymbol *s = NULL;
                  foreach(SpatSymbol *ss, spatSymbols) {
                        
                        // Check that we are on the same track:
                        if(ss->track()/VOICES != c->track()/VOICES) {
                              continue;
                        }
                        
                        // Check if this spatsymbol is responsible.
                        SpatCoordinates coord = ss->calculateCoordinates(tick);
                        if(!coord.ignore) {
                              // We found our spatsymbol.
                              s = ss;
                              break;
                        }
                  }
                  
                  // We didn't find a spatsymbol.
                  if(!s) {
                        // We move on to the next tick.
                        tick++;
                        continue;
                  }
                  
                  // We process at maximum the length of the chord.
                  while(tick < c->tick() + c->actualTicks()) {
                        SpatCoordinates coord = s->calculateCoordinates(tick);
                        
                        if(coord.ignore) {
                              // We reached the end of this spatsymbol.
                              break;
                        }
                        
                        // We already alter the tick.
                        tick ++;
                        
                        if(!seekingStart && lastCoordinate == coord) {
                              // It's the same position; so we ignore it.
                              continue;
                        }
                        lastCoordinate = coord;
                        
                        // We just found the start
                        if(seekingStart) {
                              seekingStart = false;
                              path.moveTo(coord.x(), coord.y());
                        }else{
                              path.lineTo(coord.x(), coord.y());
                        }
                  }
            }
            
            // Add the path only, if it is complete.
            if(!seekingStart) {
                  paths.append(path);
            }
      }
      
      // We now build the endpoints
      
      // We need a list of all the tracks and the global maxticks.
      QList<QVariant> tracks;
      int maxTick = 0;
      foreach(Chord *c, chords) {
            int t = c->track()/VOICES;
            if(!tracks.contains(QVariant(t))) {
                  tracks.append(QVariant(t));
            }
            maxTick = qMax(maxTick, c->tick() + c->actualTicks() -1);
      }
      
      // For each track, find out the last occurence:
      foreach(QVariant v, tracks) {
            int track = v.toInt();
            for(int tick = maxTick; tick >=0; tick--) {
                  foreach(SpatSymbol *ss, spatSymbols) {
                        
                        // Check that we are on the same track:
                        if(ss->track()/VOICES != track) {
                              continue;
                        }
                        
                        // Check if this spatsymbol is responsible.
                        SpatCoordinates coord = ss->calculateCoordinates(tick);
                        if(!coord.ignore) {
                              // We found our spatsymbol.
                              endpoints.append(QPointF(coord.x(), coord.y()));
                              tick = 0;
                              break;
                        }
                  }
            }
            
      }
      
      radar2d->setPaths(paths);
      radar2d->setEndpoints(endpoints);
}

               
               
Radar::Radar(QWidget* parent)
   : QDockWidget(tr("Radar"), parent)
      {
      setObjectName("Radar");
      setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
      
      layout = new QVBoxLayout;
      
      radar2d = new Radar2D();
      layout->addWidget(radar2d);
      layout->addWidget(new Radar2DFakeSphere());
      
      QWidget* mainWidget = new QWidget;
      mainWidget->setLayout(layout);
      setWidget(mainWidget);
      return;
      /*
      QScrollArea* sa = new QScrollArea;
      sa->setWidgetResizable(true);
      QWidget* mainWidget = new QWidget;
      mainWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding); //??
      setWidget(sa);
      sa->setWidget(mainWidget);

      layout = new QVBoxLayout;
      mainWidget->setLayout(layout);
   //   ie        = 0;
      _element  = 0;
      QHBoxLayout* hbox = new QHBoxLayout;
      apply = new QPushButton;
      apply->setText(tr("Apply"));
      apply->setEnabled(false);
      hbox->addWidget(apply);
      layout->addStretch(10);
      layout->addLayout(hbox);
      connect(apply, SIGNAL(clicked()), SLOT(applyClicked()));
      */
      }
      