//
//  Viz_Units.cpp
//  DBN
//
//  Created by Devon Hjelm on 10/23/12.
//
//

#include "Viz_Units.h"
#include "Monitors.h"
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_statistics.h>
#include "Types.h"
#include "SupportFunctions.h"
#include <math.h>

void Tex_Unit::scale_matrix_and_threshold(Monitor* monitor){
   
   float threshold_value;
   if (monitor != NULL) threshold_value = monitor->threshold;
   else threshold_value = threshold;
   float max = gsl_stats_float_max(viz_matrix->data, 1, viz_matrix->size1*viz_matrix->size2);
   
   for (int i = 0; i < viz_matrix->size1; ++i)
      for (int j = 0; j < viz_matrix->size2; ++j) {
         float val = gsl_matrix_float_get(viz_matrix, i, j);
         
         if (val == WHITE) {}
         else if (max != 0 && val/max >= threshold_value) gsl_matrix_float_set(viz_matrix, i, j, val/max);
         else gsl_matrix_float_set(viz_matrix, i, j, GREY);
      }
   
}

// FOR UNIT MONITORS ------------------------------------------------------------------

void Viz_Unit::pack(Viz_Unit *into, int number) {
   int x_pos = number%(int)((into->x_size)/x_size);
   int y_pos = number/(int)((into->x_size)/x_size);
   
   float x_ref = into->x_position-into->x_size + x_size;
   float y_ref = into->y_position+into->y_size;
   
   set_coords(x_ref + 2*x_pos*x_size, y_ref-2*y_pos*y_size, 0);
}

void Viz_Unit::set_coords(float x, float y, float z) {
   x_position = x, y_position = y, z_position = z;
}

void Viz_Unit::scale(float scale_factor) {
   x_size *= scale_factor;
   y_size *= scale_factor;
   z_size *= scale_factor;
}

void Tex_Unit::clear(){
   gsl_matrix_float_set_zero(viz_matrix);
}

void Plot_Unit::clear(){
   gsl_vector_float_set_zero(line_set);
}

void Plot_Unit::load_into_monitor(Monitor *monitor){
   int index = std::max((int)monitor->plots.size(), (int)monitor->borders.size());
   monitor->plots[index] = this;
}

Border::Border(Viz_Unit *viz) {
   set_coords(viz->x_position, viz->y_position, viz->z_position);
   set_size(viz->x_size, viz->y_size, 0);
   line_set = gsl_vector_float_calloc(4);
}

void Stacked_Tex_Unit::step_forward() {
   ++current_p;
   if (current_p == textures.end()) --current_p;
}

void Stacked_Tex_Unit::step_back() {
   --current_p;
   if (current_p == (textures.begin()-1)) ++current_p;
}

void Grid_Viz_Unit::update() {
   float unit_area = (units[0]->x_size) * (units[0]->y_size);
   float total_area = (x_size*y_size);
   
   float scale_factor = sqrt(total_area/(unit_area * units.size()));
   
   //float scale_factor = 1;
   int i = 0;
   for (auto unit:units) {
      unit->update();
      unit->scale(scale_factor);
      unit->pack(this, i);
      ++i;
   }
}

void Grid_Viz_Unit::load_into_monitor(Monitor *monitor) {
   for (auto unit:units) unit->load_into_monitor(monitor);
}
