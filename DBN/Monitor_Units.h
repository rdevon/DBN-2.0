//
//  File.h
//  DBN
//
//  Created by Devon Hjelm on 10/23/12.
//
//

#ifndef __DBN__File__
#define __DBN__File__

#include <iostream>
#include <vector>
#include "Viz_Units.h"



using std::vector;

class DataSet;
class MNIST_Feature_Monitor;
class MLP;
class Layer;
class Feature_to_Data_Monitor;
class fMRI_Feature_Monitor;
class Reconstruction_Cost_Monitor;
class Static_Tex;


class Simple_3D_Monitor : public Stacked_Tex_Unit {
public:
   
   Simple_3D_Monitor(DataSet *data);
   void load_into_monitor(Monitor*);
   void update();
};

class Static_Tex : public Tex_Unit {
public:
   Static_Tex(gsl_matrix_float *tex);
   void update(){}
};

class MNIST_Layer_Monitor : public Viz_Unit {
public:
   
   std::vector<MNIST_Feature_Monitor*>  feature_images;
   
   MLP                                 *mlp;
   Layer                               *feature_layer;
   
   MNIST_Layer_Monitor(MLP *mlp, int epochs);
   void load_into_monitor(Monitor *monitor);
   void update();
};

class MNIST_Feature_Monitor : public Viz_Unit {
public:
   int feature;
   Feature_to_Data_Monitor       *MNIST_image;
   
   MLP                           *mlp;
   
   MNIST_Feature_Monitor(int feat, MLP *mlp);
   void load_into_monitor(Monitor *monitor);
   void update();
   void scale(float scale_factor);
};

class fMRI_Layer_Monitor: public Viz_Unit {
public:
   
   std::vector<fMRI_Feature_Monitor*>     feature_images;
   
   MLP                                    *mlp;
   Layer                                  *feature_layer;
   Reconstruction_Cost_Monitor            *rc_monitor;
   
   fMRI_Layer_Monitor(MLP *mlp, int epochs);
   void load_into_monitor(Monitor *monitor);
   void update();
   void turn_off_all_but_top(int number);
   void turn_on_all();
};

class fMRI_Feature_Monitor : public Viz_Unit {
public:
   int feature;
   Feature_to_Data_Monitor       *fMRI_image;
   Plot_Unit                     *timecourse;
   
   MLP                           *mlp;
   
   fMRI_Feature_Monitor(int feat, MLP *mlp);
   void load_into_monitor(Monitor *monitor);
   void update();
   void scale(float scale_factor);
   void toggle_on();
};

class Feature_to_Data_Monitor : public Tex_Unit {
public:
   int                     feature;
   
   MLP                     *mlp;
   
   Feature_to_Data_Monitor(int feat, MLP *path);
   void update();
};

class Timecourse_Monitor : public Plot_Unit {
public:
   int feature;
   
   MLP                     *mlp;
   
   Timecourse_Monitor(int feat, MLP *path);
   void update();
};

class Reconstruction_Cost_Monitor : public Plot_Unit {
public:
   MLP                     *mlp;
   int epoch;
   Reconstruction_Cost_Monitor(MLP *mlp, int epochs);
   void update();
};

#endif /* defined(__DBN__File__) */
