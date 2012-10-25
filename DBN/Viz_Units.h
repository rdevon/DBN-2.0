//
//  Viz_Units.h
//  DBN
//
//  Created by Devon Hjelm on 10/23/12.
//
//

#ifndef __DBN__Viz_Units__
#define __DBN__Viz_Units__

#include <iostream>
#include <gsl/gsl_matrix.h>
#include <vector>

using std::vector;

class Border;
class Monitor;

class Viz_Unit {
public:
   
   bool                    up_to_date;
   bool                    on;
   
   Border                  *border;
   
   float                   x_position,
   y_position,
   z_position;
   float                   x_size,
   y_size,
   z_size;
   
   float                   color[4];
   
   void set_coords(float x, float y, float z);
   void set_size(float x, float y, float z){x_size = x, y_size = y, z_size = z;}
   
   Viz_Unit(){}
   
   virtual void update() = 0;
   void pack(Viz_Unit *into, int number);
   virtual void load_into_monitor(Monitor *monitor) = 0;
   virtual void scale(float scale_factor);
};

class Tex_Unit : public Viz_Unit {
public:
   
   float                   threshold;
   float                   stat_value;
   
   gsl_matrix_float        *viz_matrix;
   std::vector<float>      stat_vector;
   Tex_Unit(){}
   
   void scale_matrix_and_threshold(Monitor* = NULL);
   virtual void update() = 0;
   void load_into_monitor(Monitor *monitor){}
   void clear();
};

class Stacked_Tex_Unit : public Tex_Unit {
public:
   float threshold;
   float stat_value;
   
   std::vector<Tex_Unit*>                 textures;
   std::vector<Tex_Unit*>::iterator       current_p;
   
   Tex_Unit                               *current;
   
   Stacked_Tex_Unit(){}
   
   virtual void update() = 0;
   void load_into_monitor(Monitor *monitor){}
   void clear();
   void step_forward();
   void step_back();
};

class Grid_Viz_Unit : public Viz_Unit {
public:
   float threshold;
   
   std::vector<Viz_Unit*>           units;
   
   void update();
   void load_into_monitor(Monitor*);
   void clear();
};

class Plot_Unit : public Viz_Unit {
public:
   
   gsl_vector_float        *line_set;
   Plot_Unit(){}
   virtual void update() = 0;
   void load_into_monitor(Monitor *monitor);
   void clear();
};

class Border : public Plot_Unit {
public:
   Border(Viz_Unit *viz_unit);
   void update(){}
};

#endif /* defined(__DBN__Viz_Units__) */
