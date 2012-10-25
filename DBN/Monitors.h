//
//  Monitors.h
//  DBN
//
//  Created by Devon Hjelm on 10/23/12.
//
//

#ifndef __DBN__Monitors__
#define __DBN__Monitors__

#include <iostream>
#include <vector>
#include <map>

class Stacked_Tex_Unit;
class Visualizer;
class Teacher;
class Plot_Unit;
class Tex_Unit;
class Border;
class Viz_Unit;
class fMRI_Layer_Monitor;
class Reconstruction_Cost_Monitor;
class DBN;
class MNIST_Layer_Monitor;
class Simple_3D_Monitor;
class DataSet;
class Stacked_Tex_Unit;
class Grid_Viz_Unit;

using std::vector;

class Monitor {
public:
   Teacher                          *teacher;
   Visualizer                       *viz;
   std::vector<Stacked_Tex_Unit*>   stacked_units;
   std::map <int, Tex_Unit*>        tex_units;
   std::map <int, Plot_Unit*>       plots;
   std::map <int, Border*>          borders;
   std::vector<Viz_Unit*>           main_units;
   std::vector<Viz_Unit*>           stat_units;
   
   float                            threshold;
   
   bool                             monitor_top;
   int                              top_number;
   
   Monitor();
   void update();
   void update_stats();
   void reset_from_mains();
   void send_stop_signal();
   void toggle_monitor_top();
   void move_up_stack();
   void move_down_stack();
};

class New_Monitor {
public:
   Visualizer                       *viz;
   vector<Tex_Unit *>               tex_units;
   
   float                            threshold;
   
   New_Monitor();
   void update();
};

class Single_3D_Data_Monitor : public Monitor {
public:
   Simple_3D_Monitor             *monitor;
   Single_3D_Data_Monitor(DataSet *data);
};

class Layer_3D_fMRI_Monitor : public Monitor {
   Grid_Viz_Unit                     *layer_monitor;
   Reconstruction_Cost_Monitor       *rc_monitor;
   
   Layer_3D_fMRI_Monitor(DBN *dbn);
};

class fMRI_Monitor : public Monitor {
public:
   
   fMRI_Layer_Monitor            *fMRI_layer_monitor;
   Reconstruction_Cost_Monitor   *rc_monitor;
   
   fMRI_Monitor(DBN* dbn);
   
};


class MNIST_Monitor : public Monitor {
public:
   
   MNIST_Layer_Monitor            *MNIST_monitor;
   Reconstruction_Cost_Monitor    *rc_monitor;
   
   MNIST_Monitor(DBN* dbn);
   
};

#endif /* defined(__DBN__Monitors__) */
