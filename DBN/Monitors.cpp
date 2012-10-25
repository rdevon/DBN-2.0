//
//  Monitors.cpp
//  DBN
//
//  Created by Devon Hjelm on 10/23/12.
//
//

#include "Monitors.h"
#include "Viz.h"
#include "Monitor_Units.h"
#include "Teacher.h"
#include "DBN.h"


New_Monitor::New_Monitor(){
   viz = new Visualizer(1000,1000);
   the_viz = viz;
   viz->init(1, 0);
}


Monitor::Monitor(){
   viz = new Visualizer(1000,1000);
   the_viz = viz;
   the_monitor = this;
   monitor_top = false;
   top_number = 10;
}

void Monitor::update() {
   threshold = 0;
   for (auto unit:main_units) {
      if (monitor_top) {}
      unit->update();
   }
   for (auto unit:main_units) {
      if (!unit->up_to_date) {
         reset_from_mains();
         viz->resize_maps((int)tex_units.size(), (int)plots.size()+(int)borders.size());
         break;
      }
   }
   viz->update(this);
}

void Monitor::update_stats() {
   for (auto unit:stat_units) unit->update();
}

void Monitor::reset_from_mains() {
   plots.clear();
   tex_units.clear();
   borders.clear();
   for (auto unit:main_units) {
      unit->load_into_monitor(this);
      unit->up_to_date = true;
   }
   for (auto unit:stat_units) {
      unit->load_into_monitor(this);
      unit->up_to_date = true;
   }
}

void Monitor::send_stop_signal() {
   teacher->learning = false;
}

void Monitor::toggle_monitor_top() {
   monitor_top = !monitor_top;
}

void Monitor::move_down_stack() {
   for (auto unit:stacked_units) unit->step_back();
}

void Monitor::move_up_stack() {
   for (auto unit:stacked_units) unit->step_forward();
}

Single_3D_Data_Monitor::Single_3D_Data_Monitor(DataSet *data) {
   threshold = 0;
   Simple_3D_Monitor *stack = new Simple_3D_Monitor(data);
   main_units.push_back(stack);
   stacked_units.push_back(stack);
   viz->init((int)tex_units.size(), (int)plots.size()+(int)borders.size());
}

Layer_3D_fMRI_Monitor::Layer_3D_fMRI_Monitor(DBN *dbn) {
   threshold = 0;
   
}

fMRI_Monitor::fMRI_Monitor(DBN *dbn){
   fMRI_layer_monitor = new fMRI_Layer_Monitor(dbn->rc_MLP, 2000);
   for (auto feature:fMRI_layer_monitor->feature_images) feature->fMRI_image->threshold = 0.2;
   
   main_units.push_back(fMRI_layer_monitor);
   
   viz->init((int)tex_units.size(), (int)plots.size()+(int)borders.size());
}

MNIST_Monitor::MNIST_Monitor(DBN *dbn) {
   MNIST_monitor = new MNIST_Layer_Monitor(dbn->rc_MLP, 10);
   MNIST_monitor->set_coords(0, 3, 0);
   
   rc_monitor = new Reconstruction_Cost_Monitor(MNIST_monitor->mlp, 10);
   rc_monitor->set_coords(0, 1-7.5, 0);
   rc_monitor->set_size(8, 1.5, 0);
   rc_monitor->border = new Border(rc_monitor);
   plots[0] = rc_monitor;
   borders[1] = rc_monitor->border;
   
   main_units.push_back(MNIST_monitor);
   stat_units.push_back(rc_monitor);
   
   viz->init((int)tex_units.size(), (int)plots.size()+(int)borders.size());
}