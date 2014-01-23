//=============================================================================
//  MuseScore
//  Linux Music Score Editor
//  $Id: menus.cpp 5651 2012-05-19 15:57:26Z lasconic $
//
//  Copyright (C) 2002-2011 Werner Schweer and others
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License version 2.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//=============================================================================

#include "libmscore/score.h"
#include "palette.h"
#include "libmscore/note.h"
#include "libmscore/chordrest.h"
#include "libmscore/dynamic.h"
#include "libmscore/slur.h"
#include "libmscore/sym.h"
#include "libmscore/hairpin.h"
#include "scoreview.h"
#include "musescore.h"
#include "edittempo.h"
#include "libmscore/select.h"
#include "libmscore/tempo.h"
#include "libmscore/segment.h"
#include "libmscore/undo.h"
#include "icons.h"
#include "libmscore/bracket.h"
#include "libmscore/ottava.h"
#include "libmscore/textline.h"
#include "libmscore/trill.h"
#include "libmscore/pedal.h"
#include "libmscore/clef.h"
#include "libmscore/timesig.h"
#include "libmscore/barline.h"
#include "libmscore/layoutbreak.h"
#include "symboldialog.h"
#include "libmscore/volta.h"
#include "libmscore/keysig.h"
#include "libmscore/breath.h"
#include "libmscore/arpeggio.h"
#include "libmscore/tremolo.h"
#include "libmscore/repeat.h"
#include "libmscore/tempotext.h"
#include "libmscore/glissando.h"
#include "libmscore/articulation.h"
#include "libmscore/chord.h"
#include "libmscore/drumset.h"
#include "libmscore/spacer.h"
#include "libmscore/measure.h"
#include "libmscore/fret.h"
#include "libmscore/staffstate.h"
#include "libmscore/fingering.h"
#include "libmscore/bend.h"
#include "libmscore/tremolobar.h"
#include "libmscore/chordline.h"
#include "libmscore/stafftext.h"
#include "libmscore/instrchange.h"
#include "workspace.h"
#include "libmscore/icon.h"
#include "libmscore/accidental.h"
#include "libmscore/harmony.h"
#include "libmscore/rehearsalmark.h"
#include "shortcut.h"

#include "libmscore/spatsymbol.h"
#include "libmscore/spatmodifier.h"


extern bool useFactorySettings;


struct spatializationDummy {
	int spatType;
	QString path;
	QString name;
};

/*
ls | xargs -I %d echo "<file>data/spatialization/%d</file>" > tmp.txt
*/

static const spatializationDummy spatLine[] = {
	{SPATTYPE_LINE_2D, "", "" },
	{SPATTYPE_LINE_CLOSED, "", "" },
	{SPATTYPE_POLYLINE, "", "" },
	{SPATTYPE_POLYLINE_CLOSED, "", "" },
	
	//spatPolygon
	{SPATTYPE_POLYGON, "", "" },
	{SPATTYPE_POLYGON_CLOSED, "", "" },
	
	//spatCircular
	{SPATTYPE_CIRCLE_2D, "", "" },
	{SPATTYPE_CIRCLE_BF, "", "" },
	{SPATTYPE_CIRCLE_SLINKY, "", "" },
	{SPATTYPE_CIRCLE_SLINKY_CLOSED, "", "" },
	{SPATTYPE_SPIRAL_IN2OUT, "", "" },
	{SPATTYPE_SPIRAL_OUT2IN, "", "" },
	{SPATTYPE_SPIRAL_IN2OUT_CLOSED, "", "" },
	{SPATTYPE_SPIRAL_OUT2IN_CLOSED, "", "" },
	
	//spatCurve
	{SPATTYPE_BEZIER_CUBIC, "", "" },
	{SPATTYPE_BEZIER_CUBIC_CLOSED, "", "" },
	{SPATTYPE_BEZIER_CUBIC_SPLINE, "", "" },
	{SPATTYPE_BEZIER_CUBIC_SPLINE_CLOSED, "", "" },
	{SPATTYPE_BERNOULLI_LEMNISCATE, "", "" },
    
    //EndMarker
    {SPATTYPE_END, "", ""},
    {SPATTYPE_DUMMY_SYMBOL, "spat_pause.svg", "Pause"},
    {SPATTYPE_DUMMY_SYMBOL, "spat_pause_end.svg", "Pause end"},
    
	{-1, "", "" },
};

static const spatializationDummy spatModifier[] = {
	{SPATTYPE_MODIFIER_SINE, "", "" },
	{SPATTYPE_MODIFIER_SAW, "", "" },
    {SPATTYPE_MODIFIER_SQUARE, "", "" },
    {SPATTYPE_MODIFIER_TRIANGLE, "", "" },
    {SPATTYPE_MODIFIER_MARKOV, "", "" },
	
	//positionQualityMod
	{SPATTYPE_DUMMY_SYMBOL, "SS_PA_radiation.svg", "Source Radiation"},
	{SPATTYPE_DUMMY_SYMBOL, "SS_reverb.svg", "Source Reverberation"},
	{SPATTYPE_DUMMY_SYMBOL, "SS_points_scale.svg", "Scale"},
	
	//stopEndMarker
    {SPATTYPE_MODIFIER_END, "", ""},
    {SPATTYPE_DUMMY_SYMBOL, "spat_end_tot.svg", "end"},
    {-1, "", ""}
};

static const spatializationDummy roomShapes[] = {
	{SPATTYPE_DUMMY_SYMBOL, "room_cube.svg", "Cube"},
	{SPATTYPE_DUMMY_SYMBOL, "room_hemi.svg", "Hemisphere"},
	{SPATTYPE_DUMMY_SYMBOL, "room_church.svg", "Church"},
	{SPATTYPE_DUMMY_SYMBOL, "room_other.svg", "Other"},
	{SPATTYPE_DUMMY_SYMBOL, "room_reverb.svg", "Reverberance"},
	{-1, "",""}
};

static const spatializationDummy positionMusician[] = {
	{SPATTYPE_DUMMY_SYMBOL, "perf_fix.svg", "Fix"},
	{SPATTYPE_DUMMY_SYMBOL, "perf_fix_out.svg", "Fix out"},
	{SPATTYPE_DUMMY_SYMBOL, "perf_line.svg", "Line"},
	{SPATTYPE_DUMMY_SYMBOL, "perf_line_out.svg", "Line out"},
	{SPATTYPE_DUMMY_SYMBOL, "perf_arc.svg", "Arc"},
	{SPATTYPE_DUMMY_SYMBOL, "perf_arc_out.svg", "Arc out"},
	{SPATTYPE_DUMMY_SYMBOL, "perf_palin.svg", "Palindrome"},
	{SPATTYPE_DUMMY_SYMBOL, "perf_palin_out.svg", "Palindrome out"},
	{SPATTYPE_DUMMY_SYMBOL, "perf_rotate.svg", "Rotate"},
	{SPATTYPE_DUMMY_SYMBOL, "perf_rotate_out.svg", "Rotate out"},
	{SPATTYPE_DUMMY_SYMBOL, "perf_free.svg", "Free"},
	{SPATTYPE_DUMMY_SYMBOL, "perf_free_out.svg", "Free out"},
	{SPATTYPE_DUMMY_SYMBOL, "perf_other.svg", "Other"},
	{SPATTYPE_DUMMY_SYMBOL, "perf_other_out.svg", "Other out"},
	{-1, "",""}
};

static const spatializationDummy positionMicrophone[] = {
	{SPATTYPE_DUMMY_SYMBOL, "mic_fix.svg", "Fix"},
	{SPATTYPE_DUMMY_SYMBOL, "mic_fix_out.svg", "Fix out"},
	{SPATTYPE_DUMMY_SYMBOL, "mic_pendular.svg", "Pendular"},
	{SPATTYPE_DUMMY_SYMBOL, "mic_pendular_out.svg", "Pendular out"},
	{SPATTYPE_DUMMY_SYMBOL, "mic_choreo.svg", "Choreography"},
	{SPATTYPE_DUMMY_SYMBOL, "mic_choreo_out.svg", "Choreography out"},
	{SPATTYPE_DUMMY_SYMBOL, "mic_other.svg", "Other"},
	{SPATTYPE_DUMMY_SYMBOL, "mic_other_out.svg", "Other out"},
	{-1, "",""}
};

static const spatializationDummy positionLoudspeaker[] = {
	{SPATTYPE_DUMMY_SYMBOL, "ls_fix.svg", "Fix"},
	{SPATTYPE_DUMMY_SYMBOL, "ls_fix_out.svg", "Fix out"},
	{SPATTYPE_DUMMY_SYMBOL, "ls_line.svg", "Line"},
	{SPATTYPE_DUMMY_SYMBOL, "ls_line_out.svg", "Line out"},
	{SPATTYPE_DUMMY_SYMBOL, "ls_arc.svg", "Arc"},
	{SPATTYPE_DUMMY_SYMBOL, "ls_arc_out.svg", "Arc out"},
	{SPATTYPE_DUMMY_SYMBOL, "ls_up.svg", "Up"},
	{SPATTYPE_DUMMY_SYMBOL, "ls_up_out.svg", "Up out"},
	{SPATTYPE_DUMMY_SYMBOL, "ls_down.svg", "Down"},
	{SPATTYPE_DUMMY_SYMBOL, "ls_down_out.svg", "Down out"},
	{SPATTYPE_DUMMY_SYMBOL, "ls_palin.svg", "Palindrome"},
	{SPATTYPE_DUMMY_SYMBOL, "ls_palin_out.svg", "Palindrome out"},
	{SPATTYPE_DUMMY_SYMBOL, "ls_pendular.svg", "Pendular"},
	{SPATTYPE_DUMMY_SYMBOL, "ls_pendular_out.svg", "Pendular out"},
	{SPATTYPE_DUMMY_SYMBOL, "ls_other.svg", "Other"},
	{SPATTYPE_DUMMY_SYMBOL, "ls_other_out.svg", "Other out"},
	{SPATTYPE_DUMMY_SYMBOL, "ls_choreo.svg", "Choreography"},
	{SPATTYPE_DUMMY_SYMBOL, "ls_choreo_out.svg", "Choreography out"},
	{-1, "",""}
};


static const spatializationDummy sourcesAndPlanes[] = {
	{SPATTYPE_DUMMY_SYMBOL, "spat_SS_point_root.svg", "Root"},
	{SPATTYPE_DUMMY_SYMBOL, "spat_SS_point_root_out.svg", "Root out"},
	{SPATTYPE_DUMMY_SYMBOL, "spat_SS_group_root.svg", "Root group"},
	{SPATTYPE_DUMMY_SYMBOL, "spat_SS_group_root_out.svg", "Root group"},
	{SPATTYPE_DUMMY_SYMBOL, "spat_SS_point_PA.svg", "Position"},
	{SPATTYPE_DUMMY_SYMBOL, "spat_SS_point_PA_out.svg", "Position out"},
	{SPATTYPE_DUMMY_SYMBOL, "spat_SS_point_PA_alternate.svg", "Position Alternation"},
	{SPATTYPE_DUMMY_SYMBOL, "spat_SS_point_PA_alternate_out.svg", "Position Alternation out"},
	{SPATTYPE_DUMMY_SYMBOL, "spat_SS_group_PA.svg", "Group"},
	{SPATTYPE_DUMMY_SYMBOL, "spat_SS_group_PA_out.svg", "Group out"},
	{SPATTYPE_DUMMY_SYMBOL, "spat_SS_group_PA_alternate.svg", "Alternate Group"},
	{SPATTYPE_DUMMY_SYMBOL, "spat_SS_group_PA_alternate_out.svg", "Alternate Group out"},
	{SPATTYPE_DUMMY_SYMBOL, "SS_plane_front.svg", "Front"},
	{SPATTYPE_DUMMY_SYMBOL, "SS_plane_center.svg", "Center"},
	{SPATTYPE_DUMMY_SYMBOL, "SS_plane_back.svg", "Back"},
	{SPATTYPE_DUMMY_SYMBOL, "SS_place_plane.svg", "Projected Plane"},
	
	//d3symbols
	{SPATTYPE_DUMMY_SYMBOL, "spat_3D_SS_point_root.svg", "3D root"},
	{SPATTYPE_DUMMY_SYMBOL, "spat_3D_SS_point_root_out.svg", "3D root out"},
	{SPATTYPE_DUMMY_SYMBOL, "spat_3D_SS_point_PA.svg", "3D placement"},
	{SPATTYPE_DUMMY_SYMBOL, "spat_3D_SS_point_PA_out.svg", "3D placement out"},
	{SPATTYPE_DUMMY_SYMBOL, "spat_3D_SS_group_root.svg", "3D root group"},
	{SPATTYPE_DUMMY_SYMBOL, "spat_3D_SS_group_root_out.svg", "3D root group out"},
	{SPATTYPE_DUMMY_SYMBOL, "spat_3D_SS_point_PA.svg", "3D group"},
	{SPATTYPE_DUMMY_SYMBOL, "spat_3D_SS_point_PA_out.svg", "3D group out"},
	{-1, "",""}
};



static const spatializationDummy objects[] = {
	{SPATTYPE_DUMMY_SYMBOL, "perf_place.svg","Stage"},
	{SPATTYPE_DUMMY_SYMBOL, "accessory_place.svg","Accessory"},
	
	//externals
    {SPATTYPE_DUMMY_SYMBOL, "spat_algo.svg", "Algo"},
    {SPATTYPE_OSC_MESSAGE, "", ""},
    {-1, "", ""}
};


Palette* createSpatSymbolPalette(QString title, const spatializationDummy *elements) {
      Palette *sp = new Palette;
      sp->setName(title);
      sp->setGrid(42, 45);
      sp->setDrawGrid(true);
      for(int i=0;elements[i].spatType != -1; i++) {
      	SpatSymbol* s = new SpatSymbol(gscore);
      	s->setSpatType(elements[i].spatType);
      	s->setOSCMessage1(elements[i].path);
      	s->setOSCMessage2(elements[i].name);
      	sp->append(s, s->spatTypeString());
      }
      
      return sp;  
}


void MuseScore::appendSpatializationPalettes()
      {
      
      //Positioning
      paletteBox->addPalette(createSpatSymbolPalette("SSMN Positioning", sourcesAndPlanes));
      
      //Trajectory
      paletteBox->addPalette(createSpatSymbolPalette("SSMN Trajectory", spatLine));
      
      //Modifiers
      paletteBox->addPalette(createSpatSymbolPalette("SSMN Modifiers", spatModifier));
      
      //OSC&Adjuncts
      paletteBox->addPalette(createSpatSymbolPalette("SSMN OSC & Adjuncts", objects));
      
      //performer
      paletteBox->addPalette(createSpatSymbolPalette("SSMN Performer", positionMusician));
      
      paletteBox->addPalette(createSpatSymbolPalette("SSMN Microphone", positionMicrophone));
      
      paletteBox->addPalette(createSpatSymbolPalette("SSMN Loudspeaker", positionLoudspeaker));
      
      paletteBox->addPalette(createSpatSymbolPalette("SSMN Room", roomShapes));
      
      
      
      
      }

