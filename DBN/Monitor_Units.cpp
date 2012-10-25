//
//  File.cpp
//  DBN
//
//  Created by Devon Hjelm on 10/23/12.
//
//

#include "Monitor_Units.h"
#include "MLP.h"
#include "Layers.h"
#include "Monitors.h"
#include "IO.h"

Simple_3D_Monitor::Simple_3D_Monitor(DataSet *data) {
   threshold = 0;
   set_size(8, 5, 0);
   set_coords(0, 0, 0);
   
   gsl_vector_float_view row = gsl_matrix_float_subrow(data->train, 0, 0, data->train->size2);
   gsl_vector_float *vec = gsl_vector_float_calloc(data->dims[0]*data->dims[1]*data->dims[2]);
   
   data->apply_mask(vec, &row.vector);
   
   for (int k = 0; k < data->dims[2]; ++k) {
      gsl_matrix_float *mat = gsl_matrix_float_calloc(data->dims[0], data->dims[1]);
      gsl_vector_float_view subdata =  gsl_vector_float_subvector(vec, k*data->dims[0]*data->dims[1], data->dims[0]*data->dims[1]);
   
      for (int i = 0; i < data->dims[0]; ++i) {
         for (int j = 0; j < data->dims[1]; ++j) {
            float value = gsl_vector_float_get(&subdata.vector, j+i*data->dims[1]);
            gsl_matrix_float_set(mat, i, j, value);
         }
      }
      Static_Tex *tex = new Static_Tex(mat);
      tex->threshold = threshold;
      textures.push_back(tex);
   }
   current_p = textures.begin() + textures.size()/2;
   gsl_matrix_float *tex_mat = gsl_matrix_float_calloc((*current_p)->viz_matrix->size1, (*current_p)->viz_matrix->size2);
   current = new Static_Tex(tex_mat);
}

void Simple_3D_Monitor::update() {
   auto tex = *current_p;
   gsl_matrix_float_memcpy(current->viz_matrix, tex->viz_matrix);
   tex->set_coords(x_position, y_position, z_position);
}

void Simple_3D_Monitor::load_into_monitor(Monitor *monitor) {
   int i = std::max((int)monitor->borders.size(), (int)monitor->plots.size());
   auto tex = current;
   monitor->tex_units[i] = tex;
}

Static_Tex::Static_Tex(gsl_matrix_float *tex) {
   color[0] = 1, color[1] = 0, color[2] = 0, color [3] = 1;
   on = true;
   viz_matrix = tex;
   set_size(.59, .69, 0);
   set_coords(0, 0, 0);
}

fMRI_Layer_Monitor::fMRI_Layer_Monitor(MLP *source_mlp, int epochs) {
   mlp = source_mlp;
   set_size(8, 5, 0);
   set_coords(0, 2, 0);
   rc_monitor = new Reconstruction_Cost_Monitor(mlp, epochs);
   rc_monitor->set_coords(x_position, 1-7.5, 0);
   rc_monitor->set_size(8, 1.5, 0);
   rc_monitor->border = new Border(rc_monitor);
   feature_layer = NULL;
}

void fMRI_Layer_Monitor::update() {
   mlp->make_path_from_data_to_top("fMRI");
   Layer *top = mlp->transmit_list.back()->to;
   
   if (top != feature_layer) {
      up_to_date = false;
      feature_layer = top;
      
      //if (feature_layer->nodenum <= 18) scale_factor = 1;
      //else scale_factor = .5;
      for (auto feature:feature_images) {
         gsl_matrix_float_free(((Tex_Unit*)feature->fMRI_image)->viz_matrix);
         delete feature;
      }
      feature_images.clear();
      for (int i = 0; i < feature_layer->nodenum; ++i) {
         fMRI_Feature_Monitor *new_feature = new fMRI_Feature_Monitor(i, mlp);
         feature_images.push_back(new_feature);
      }
      float image_area = (feature_images[0]->x_size) * (feature_images[0]->y_size);
      float total_area = (x_size*y_size);
      
      float scale_factor = sqrt(total_area/(image_area * feature_layer->nodenum));
      
      //float scale_factor = 1;
      for (auto new_feature:feature_images) {
         new_feature->scale(scale_factor);
         new_feature->pack(this, new_feature->feature);
         new_feature->border = new Border(new_feature);
      }
   }
   
   mlp->d_flag = TIMECOURSE;
   rc_monitor->update();
   for (auto feature:feature_images) {
      feature->pack(this, feature->feature);
      feature->timecourse->set_coords(feature->x_position, feature->y_position-feature->y_size*(.75), feature->z_position);
      feature->timecourse->update();
   }
   for (auto feature:feature_images) {
      feature->border->set_coords(feature->x_position, feature->y_position, feature->z_position);
      feature->fMRI_image->set_coords(feature->x_position, feature->y_position+feature->y_size/4, feature->z_position);
      feature->fMRI_image->update();
   }
}

void fMRI_Layer_Monitor::load_into_monitor(Monitor *monitor){
   int last;
   int start = std::max((int)monitor->borders.size(), (int)monitor->plots.size());
   for (auto feature_image:feature_images) {
      monitor->tex_units[start+feature_image->feature] = feature_image->fMRI_image;
      monitor->plots[start+2*feature_image->feature] = feature_image->timecourse;
      monitor->borders[start+2*feature_image->feature+1] = feature_image->border;
      last = start+2*feature_image->feature+1;
   }
   monitor->plots[last+1] = rc_monitor;
   monitor->borders[last + 2] = rc_monitor->border;
}

void fMRI_Layer_Monitor::turn_off_all_but_top(int number) {
   std::vector<float> stat_vec;
   for (auto feature:feature_images) stat_vec.push_back(feature->fMRI_image->stat_value);
   
   std::sort(stat_vec.begin(), stat_vec.end());
   
   float lowest_value = stat_vec[-number];
   
   for (auto feature:feature_images)
      if (feature->fMRI_image->stat_value < lowest_value) feature->toggle_on();
}

void fMRI_Layer_Monitor::turn_on_all() {
   for (auto feature:feature_images) {
      feature->fMRI_image->on = true;
      feature->timecourse->on = true;
   }
}

//--------

fMRI_Feature_Monitor::fMRI_Feature_Monitor(int feat, MLP *source_mlp) : feature(feat) {
   set_size(1, 1, 0);
   set_coords(0, 0, 0);
   
   mlp = source_mlp;
   fMRI_image = new Feature_to_Data_Monitor(feature, mlp);
   timecourse = new Timecourse_Monitor(feature, mlp);
   fMRI_image->set_size(.59, .69, 0);
   timecourse->set_size(.5, .25, 0);
}

void fMRI_Feature_Monitor::update() {
   fMRI_image->set_coords(x_position, y_position+.25, z_position);
   timecourse->set_coords(x_position, y_position-.25, z_position);
   timecourse->update();
   fMRI_image->update();
}

void fMRI_Feature_Monitor::load_into_monitor(Monitor *monitor) {
   monitor->tex_units[0] = fMRI_image;
   monitor->plots[0] = timecourse;
   monitor->borders[1] = border;
}

void fMRI_Feature_Monitor::scale(float scale_factor) {
   Viz_Unit::scale(scale_factor);
   fMRI_image->scale(scale_factor);
   timecourse->scale(scale_factor);
}

void fMRI_Feature_Monitor::toggle_on() {
   fMRI_image->on = !fMRI_image->on;
   timecourse->on = !timecourse->on;
}

MNIST_Layer_Monitor::MNIST_Layer_Monitor(MLP *source_mlp, int epochs) {
   mlp = source_mlp;
   set_size(8, 3, 0);
   set_coords(0, 2, 0);
   feature_layer = NULL;
}

void MNIST_Layer_Monitor::update() {
   mlp->d_flag = TRAIN;
   mlp->make_path_from_data_to_top("MNIST");
   Layer *top = mlp->transmit_list.back()->to;
   
   if (top != feature_layer) {
      up_to_date = false;
      feature_layer = top;
      
      float scale_factor = .5;
      
      //if (feature_layer->nodenum <= 18) scale_factor = 1;
      //else scale_factor = .5;
      
      feature_images.clear();
      for (int i = 0; i < feature_layer->nodenum; ++i) {
         MNIST_Feature_Monitor *new_feature = new MNIST_Feature_Monitor(i, mlp);
         new_feature->scale(scale_factor);
         new_feature->pack(this, i);
         new_feature->border = new Border(new_feature);
         feature_images.push_back(new_feature);
      }
   }
   
   for (auto feature:feature_images) {
      feature->pack(this, feature->feature);
      feature->border->set_coords(feature->x_position, feature->y_position, feature->z_position);
      feature->MNIST_image->set_coords(feature->x_position, feature->y_position+feature->y_size/4, feature->z_position);
      feature->MNIST_image->update();
   }
}

void MNIST_Layer_Monitor::load_into_monitor(Monitor *monitor){
   int last;
   for (auto feature_image:feature_images) {
      monitor->tex_units[feature_image->feature] = feature_image->MNIST_image;
      monitor->borders[feature_image->feature] = feature_image->border;
      last = feature_image->feature;
   }
}

MNIST_Feature_Monitor::MNIST_Feature_Monitor(int feat, MLP *source_mlp) : feature(feat) {
   set_size(1, 1, 0);
   set_coords(0, 0, 0);
   
   mlp = source_mlp;
   MNIST_image = new Feature_to_Data_Monitor(feature, mlp);
   MNIST_image->set_size(1, 1, 0);
}

void MNIST_Feature_Monitor::update() {
   MNIST_image->set_coords(x_position, y_position, z_position);
   MNIST_image->update();
}

void MNIST_Feature_Monitor::load_into_monitor(Monitor *monitor) {
   monitor->tex_units[0] = MNIST_image;
   monitor->borders[1] = border;
}

void MNIST_Feature_Monitor::scale(float scale_factor) {
   Viz_Unit::scale(scale_factor);
   MNIST_image->scale(scale_factor);
}

Feature_to_Data_Monitor::Feature_to_Data_Monitor (int feat, MLP *source_mlp) : feature(feat) {
   color[0] = 1, color[1] = 0, color[2] = 0, color [3] = 1;
   mlp = source_mlp;
   mlp->sample_flag = NOSAMPLE;
   Input_Edge* output = (Input_Edge*)mlp->transmit_list[0];
   viz_matrix = gsl_matrix_float_alloc(output->input_matrix->size1, output->input_matrix->size2);
   
}

void Feature_to_Data_Monitor::update() {
   if (!on) {
      gsl_matrix_float_set_all(viz_matrix, WHITE);
   }
   
   Layer *from = mlp->transmit_list.back()->to;
   Layer *to = mlp->transmit_list[0]->to;
   
   stat_value = 0;
   for (int j = 0; j < to->activations->size2; ++j) {stat_value += gsl_matrix_float_get(to->activations, feature, j);}
   
   mlp->make_batch(1);
   Input_Edge* output = (Input_Edge*)mlp->transmit_list[0];
   gsl_matrix_float_set_zero(from->samples);
   gsl_matrix_float_set(from->samples, feature, 0, 1);
   
   mlp->transmit(BACKWARD);
   gsl_matrix_float_get_col(to->sample_vector, to->samples, 0);
   output->dataset->transform_for_viz(output->input_matrix, to->sample_vector);
   gsl_matrix_float_memcpy(viz_matrix,output->input_matrix);
}

Timecourse_Monitor::Timecourse_Monitor (int feat, MLP *source_mlp) : feature(feat) {
   on = true;
   color[0] = 0, color[1] = 1, color[2] = 0, color [3] = 1;
   mlp = source_mlp;
   mlp->sample_flag = NOSAMPLE;
   line_set = gsl_vector_float_calloc(220);
}

void Timecourse_Monitor::update() {
   if (!on) {
      gsl_vector_float_set_all(line_set, 0);
   }
   Layer* to = mlp->transmit_list.back()->to;
   gsl_vector_float_view timecourse = gsl_matrix_float_row(to->activations, feature);
   gsl_vector_float_memcpy(line_set, &timecourse.vector);
}

Reconstruction_Cost_Monitor::Reconstruction_Cost_Monitor(MLP *monitored_mlp, int epochs){
   on = true;
   set_coords(0, 0, 0);
   set_size(8, 2, 0);
   mlp = monitored_mlp;
   border = new Border(this);
   line_set = gsl_vector_float_calloc(epochs);
   epoch = 0;
}

void Reconstruction_Cost_Monitor::update() {
   mlp->getReconstructionCost();
   std::cout << "Reconstruction cost: " << mlp->reconstruction_cost << std::endl;
   gsl_vector_float_set(line_set, epoch, mlp->reconstruction_cost);
   ++epoch;
}

/*
 void Unit_Monitor::plot(){
 std::string filepath = plotpath + name + "viz.plot";
 FILE *file_handle;
 file_handle = fopen(filepath.c_str(), "w");
 
 for (int i = 0; i < viz_matrix->size1; ++i){
 for (int j = 0; j < viz_matrix->size2; ++j){
 float val = gsl_matrix_float_get(viz_matrix, i, j);
 fprintf(file_handle, "%f ", val);
 }
 fprintf(file_handle, "\n");
 }
 fflush(file_handle);
 fclose(file_handle);
 }
 */

