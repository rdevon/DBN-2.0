//
//  MLP.cpp
//  DBN
//
//  Created by Devon Hjelm on 8/30/12.
//
//

#include "MLP.h"
#include "Connections.h"
#include "IO.h"
#include "SupportMath.h"
#include "SupportFunctions.h"
#include "Layers.h"
#include "RBM.h"

Input_Edge::Input_Edge(DataSet *ds, Layer* to_layer){
   to = to_layer;
   from = NULL;
   dataset = ds;
   input_matrix = gsl_matrix_float_calloc(dataset->height, dataset->width);
}

int Input_Edge::transmit_signal(Sample_flag_t sample_flag){
   switch (direction_flag) {
      case FORWARD   : return pull_data(sample_flag);
      case BACKWARD  : return 1;
   }
}

int Input_Edge::pull_data(Sample_flag_t s_flag){
   Input_t *input;
   
   if (d_flag == TRAIN)             input = dataset->train;
   else if (d_flag == TEST)         input = dataset->test;
   else if (d_flag == TIMECOURSE)   input = dataset->extra;
   
   if (dataset->index + to->batchsize > input->size1) {
      dataset->index = 0;
      to->status = SAMPLED;
      return 0;
   }
   
   gsl_matrix_float_view databatch = gsl_matrix_float_submatrix(input, dataset->index, 0, to->batchsize, to->nodenum);
   gsl_matrix_float_transpose_memcpy(to->samples, &(databatch.matrix));
   
   if (to->noisy && s_flag == SAMPLE) to->apply_noise();
   
   dataset->index += (to->batchsize);
   to->status = SAMPLED;
   return 1;
}

void Input_Edge::pull_model(){
   //  dataset->transform_for_viz(input_matrix, to->samples);
}

struct Input_Edge::Data_Is {
   Data_Is(std::string name) : dataname(name) {}
   bool operator () (Input_Edge *input) {return input->dataset->name == dataname;}
   std::string dataname;
};
//-------------------------------------------

void Edge::find_path_to(MLP *mlp, Layer *to, std::vector<Edge*> path){
   
   if (find(path.begin(), path.end(), this) != path.end()) return;
   path.push_back(this);
   
   if (this->to == to || this->from == to) {
      mlp->transmit_list = path;
   }
   
   for (auto edge:mlp->edges) {
      if (edge->to == to || edge->from == from || edge->from == to || edge->to == from)
         edge->find_path_to(mlp, to, path);
   }
   return;
}

//-------------------------------------------

MLP *MLP::make_path(Layer *from, Layer *to) {
   MLP *path = new MLP;
   std::vector<Edge*> min_path;
   for (auto edge:edges)
      if (edge->to == from || edge->from == from) {
         edge->find_path_to(this, to, std::vector<Edge*>());
         if (transmit_list.size() <= min_path.size() || min_path.size() == 0)
            min_path = transmit_list;
      }
   
   path->edges = min_path;
   
   for (auto input:inputs) {
      if (input->to == from || input->to == to) path->inputs.push_back(input);
   }
   
   path->make_path_transit_list(from, to);
   return path;
}

MLP *MLP::make_path_to_bottom(Layer *src){
   MLP *path = new MLP;
   
   Is_Below is_below_src(src, this);
   for (auto edge:edges) {
      if (( is_below_src(edge) )) path->edges.push_back(edge);
   }
   
   for (auto input:inputs) {
      if (( is_below_src(input) )) path->inputs.push_back(input);
   }
   
   path->transmit_list = path->edges;
   for (auto input:path->inputs)
      path->transmit_list.push_back(input);
   
   for (edge_list_t::reverse_iterator r_iter = path->transmit_list.rbegin(); r_iter != path->transmit_list.rend(); ++r_iter)
      path->r_transmit_list.push_back(*r_iter);
   return path;
}

MLP *MLP::make_path_from_bottom(Layer *dest) {
   MLP *path = make_path_to_bottom(dest);
   std::reverse(path->edges.begin(), path->edges.end());
   std::reverse(path->transmit_list.begin(), path->transmit_list.end());
   std::reverse(path->r_transmit_list.begin(), path->r_transmit_list.end());
   return path;
}

RBM *MLP::make_rbm_level(int level) {
   RBM *rbm = new RBM;
   for (auto edge:edges) edge->level = 0;
   auto edge = edges.begin();
   while ( !check_levels() ) {
      Layer *to = (*edge)->to;
      Comes_from comes_from_to(to);
      Edge *edge_above = *std::find_if(edges.begin(), edges.end(), comes_from_to);
      if (edge_above != *edges.end() && edge_above->level <= (*edge)->level) ++edge_above->level;
      ++edge;
      if (edge == edges.end()) edge = edges.begin();
   }
   
   for (auto edge:edges) {
      edge->from->noise = 0.1;
      if (edge->level == level)
         rbm->edges.push_back(edge);
   }
   transport_data(rbm);
   rbm->make_input_to_top_transmit_list();
   return rbm;
}

MLP *MLP::make_level_to_level(int bot, int top) {
   MLP *slice_MLP = new MLP;
   for (auto edge:edges) edge->level = 0;
   auto edge = edges.begin();
   while ( !check_levels() ) {
      Layer *to = (*edge)->to;
      Comes_from comes_from_to(to);
      Edge *edge_above = *std::find_if(edges.begin(), edges.end(), comes_from_to);
      if (edge_above != *edges.end() && edge_above->level <= (*edge)->level) ++edge_above->level;
      ++edge;
      if (edge == edges.end()) edge = edges.begin();
   }
   
   for (auto edge:edges)
      if (edge->level >= bot && edge->level <= top)
         slice_MLP->edges.push_back(edge);
   
   transport_data(slice_MLP);
   slice_MLP->make_input_to_top_transmit_list();
   return slice_MLP;
}

//---------------------------------------------------------------------

void MLP::make_unordered_transit_list(){
   transmit_list.clear();
   for (auto input:inputs)    transmit_list.push_back(input);
   for (auto edge:edges)      transmit_list.push_back(edge);
}

void MLP::make_input_to_top_transmit_list() {
   transmit_list.clear();
   r_transmit_list.clear();
   From_To_Check from_is_to;
   make_unordered_transit_list();
   std::sort(transmit_list.begin(), transmit_list.end(), from_is_to);
   for (edge_list_t::reverse_iterator r_iter = transmit_list.rbegin(); r_iter != transmit_list.rend(); ++r_iter)
      r_transmit_list.push_back(*r_iter);
}

void MLP::make_bottom_to_top_transmit_list() {
   From_To_Check from_is_to;
   transmit_list.clear();
   r_transmit_list.clear();
   for (auto edge:edges)      transmit_list.push_back(edge);
   std::sort(transmit_list.begin(), transmit_list.end(), from_is_to);
   for (edge_list_t::reverse_iterator r_iter = transmit_list.rbegin(); r_iter != transmit_list.rend(); ++r_iter)
      r_transmit_list.push_back(*r_iter);
}

void MLP::make_path_transit_list(Layer* from, Layer* to) {
   transmit_list.clear();
   r_transmit_list.clear();
   std::vector<Edge*> min_path;
   for (auto edge:edges)
      if (edge->to == from || edge->from == from) {
         edge->find_path_to(this, to, std::vector<Edge*>());
         if (transmit_list.size() <= min_path.size() || min_path.size() == 0) min_path = transmit_list;
      }
   transmit_list = min_path;
   
   for (auto input:inputs) {
      if (input->to == from) transmit_list.insert(transmit_list.begin(), input);
      if (input->to == to) transmit_list.push_back(input);
   }
   
   for (edge_list_t::reverse_iterator r_iter = transmit_list.rbegin(); r_iter != transmit_list.rend(); ++r_iter)
      r_transmit_list.push_back(*r_iter);
}

void MLP::make_path_from_data_to_top(std::string dataname) {
   transmit_list.clear();
   r_transmit_list.clear();
   
   Input_Edge::Data_Is datais(dataname);
   if (std::find_if(inputs.begin(), inputs.end(), datais) == inputs.end()) return;
   Edge *edge = *std::find_if(inputs.begin(), inputs.end(), datais);
   
   while (edge != *edges.end() && edge != *inputs.end()) {
      transmit_list.push_back(edge);
      Comes_from comes_from_edge(edge->to);
      edge = *std::find_if(edges.begin(), edges.end(), comes_from_edge);
   }
   
   for (edge_list_t::reverse_iterator r_iter = transmit_list.rbegin(); r_iter != transmit_list.rend(); ++r_iter)
      r_transmit_list.push_back(*r_iter);
   
}

//----------------------------------------------------------------------

void MLP::add(Connection* connection) {
   if (find (edges.begin(), edges.end(), (Edge*)connection) == edges.end()) edges.push_back(connection);
}

void MLP::add(Input_Edge* input_edge) {
   if (find (inputs.begin(), inputs.end(), (Edge*)input_edge) == inputs.end()) inputs.push_back(input_edge);
}

bool MLP::check_levels(){
   for (auto edge:edges) {
      Layer *to = edge->to;
      Comes_from comes_from_to(to);
      Edge *edge_above = *std::find_if(edges.begin(), edges.end(), comes_from_to);
      if (edge_above != *edges.end() && edge_above->level <= edge->level) return false;
   }
   return true;
}

void MLP::make_batch(int batch_size){
   for (auto edge:edges) {
      edge->from->make_batch(batch_size);
      edge->to->make_batch(batch_size);
   }
}

void MLP::make_batch_for_whole_input(){
   int min_input_size = INFINITY;
   Input_t *data;
   for (auto input:inputs) {
      if (d_flag == TRAIN)             data = input->dataset->train;
      else if (d_flag == TEST)         data = input->dataset->test;
      else if (d_flag == TIMECOURSE)   data = input->dataset->extra;
      if (data->size1 < min_input_size) min_input_size = (int)data->size1;
   }
   make_batch(min_input_size);
}

void MLP::set_status_all(Node_status_flag_t status) {
   for (auto edge:edges) {
      edge->from->status = status;
      edge->to->status = status;
   }
}

bool MLP::is_hanging(Edge *edge) {
   Layer *from = edge->from;
   Leads_to leads_to_from(from);
   if (std::find_if(edges.begin(), edges.end(), leads_to_from) != edges.end())
      return false;
   if (std::find_if(inputs.begin(), inputs.begin(), leads_to_from) != inputs.end())
      return false;
   return true;
}

void MLP::transport_data(MLP *to_mlp) {
   for (auto edge:to_mlp->edges) {
      Layer *dest = edge->from;
      Leads_to leads_to_dest(dest);
      if (std::find_if(inputs.begin(), inputs.end(), leads_to_dest) != inputs.end())
         to_mlp->inputs.push_back(*std::find_if(inputs.begin(), inputs.end(), leads_to_dest));
   }
   
   for (auto edge:to_mlp->edges) if (to_mlp->is_hanging(edge)) {
      Layer *dest = edge->from;
      MLP *input_tranport = make_path_from_bottom(dest);
      input_tranport->d_flag = TRAIN;
      input_tranport->sample_flag = NOSAMPLE;
      input_tranport->make_batch_for_whole_input();
      input_tranport->init_data();
      input_tranport->transmit(FORWARD);
      DataSet *dataset = new DataSet;
      dataset->train = gsl_matrix_float_alloc(dest->samples->size2, dest->samples->size1);
      gsl_matrix_float_transpose_memcpy(dataset->train, dest->samples);
      Input_Edge *input_edge = new Input_Edge(dataset, dest);
      to_mlp->inputs.push_back(input_edge);
   }
   
}

//------------------------------------------------------------------------------

int MLP::transmit(Direction_flag_t direction) {
   
   for (auto input:inputs) input->d_flag = d_flag;
   
   Layer *to1;
   Layer *to2 = NULL;
   set_status_all(SAMPLED);
   for (auto edge:transmit_list) {
      to1 = edge->to;
      if (to1 == to2) direction = (Direction_flag_t)(!direction);
      edge->direction_flag = direction;
      to2 = to1;
   }
   
   if (direction==FORWARD) for (auto edge:transmit_list) {
      if (edge->transmit_signal(sample_flag) == 0) return 0;
   }
   else                    for (auto edge:r_transmit_list) {
      if (edge->transmit_signal(sample_flag) == 0) return 0;
   }
   
   for (auto edge:transmit_list) {
      if (edge->to->status == ACTIVATED)
         edge->to->finish_activation(sample_flag);
      if (edge->from != NULL && edge->from->status == ACTIVATED)
         edge->from->finish_activation(sample_flag);
   }
   return 1;
}

void MLP::init_data(){
   for (auto input:inputs) {
      input->direction_flag = FORWARD;
      input->d_flag = d_flag;
      input->dataset->index = 0;
   }
}

void MLP::getReconstructionCost(){
   
   sample_flag = NOSAMPLE;
   init_data();
   make_batch_for_whole_input();
   transmit(FORWARD);
   
   if (inputs.size() != 0) for (auto input:inputs) {
      gsl_matrix_float_memcpy(input->to->extra, input->to->samples);
   }
   else {
      Layer *layer = transmit_list[0]->from;
      gsl_matrix_float_memcpy(layer->extra, layer->samples);
   }
   transmit(BACKWARD);
   reconstruction_cost = 0;
   
   if (inputs.size() != 0) for (auto input:inputs) {
      float rc = input->to->reconstructionCost(input->to->extra, input->to->samples);
      reconstruction_cost += rc;
   }
   else {
      Layer *layer = transmit_list[0]->from;
      reconstruction_cost = layer->reconstructionCost(layer->extra, layer->samples);
   }
}

