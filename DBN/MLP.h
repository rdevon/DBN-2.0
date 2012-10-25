//
//  MLP.h
//  DBN
//
//  Created by Devon Hjelm on 8/30/12.
//
//

#ifndef __DBN__MLP__
#define __DBN__MLP__

#include "Types.h"

class DataSet;
class Connection;
class Layer;
class Data_Function;
class MLP;
class RBM;

class Edge {
public:
   int level;
   Direction_flag_t  direction_flag;
   Layer *from, *to;
   
   Edge (){};
   
   virtual int transmit_signal(Sample_flag_t) = 0;
   void find_path_to(MLP *mlp, Layer *to, std::vector<Edge*> path);
};

class Input_Edge : public Edge {
public:
   Data_flag_t       d_flag;
   
   DataSet *dataset;
   
   gsl_matrix_float  *input_matrix;
   
   Input_Edge(DataSet *data, Layer* to_layer);
   ~Input_Edge(){}
   
   int transmit_signal(Sample_flag_t);
   int pull_data(Sample_flag_t);
   void pull_model();
   struct Data_Is;
};

class MLP{
public:
   Data_flag_t                               d_flag;
   
   typedef std::vector<Edge*>                edge_list_t;
   typedef edge_list_t::iterator             edge_list_iter_t;
   
   typedef std::vector<Input_Edge*>          input_edge_list_t;
   typedef input_edge_list_t::iterator       input_edge_list_iter_t;
   
   Sample_flag_t                             sample_flag;

   edge_list_t                               edges;
   input_edge_list_t                         inputs;
   edge_list_t                               transmit_list;
   edge_list_t                               r_transmit_list;
   
   float reconstruction_cost;
   
   MLP(){}
   
   struct From_To_Check {
      From_To_Check(){}
      bool operator ()(Edge* e1, Edge* e2) {
         Layer *from = e1->from;
         Layer *to = e2->to;
         return (from != to);
      }
   };
   
   struct Comes_from {
      Comes_from(Layer *from) : from(from) {}
      bool operator ()(Edge* edge) {
         return (edge->from == from);
      }
      Layer *from;
   };
   
   struct Leads_to {
      Leads_to(Layer *to) : to(to) {}
      bool operator ()(Edge* edge) {
         return (edge->to == to);
      }
      Layer *to;
   };
   
   struct Is_Below {
      Is_Below(Layer *l, MLP *m) : layer(l), mlp(m) {}
      bool operator ()(Edge *test_edge) {
         Edge *intermediate = test_edge;
         while (1) {
            Layer *to = intermediate->to;
            if (to == layer) return true;
            Comes_from comes_from_to(to);
            intermediate = *std::find_if(mlp->edges.begin(), mlp->edges.end(), comes_from_to);
            if (intermediate == *mlp->edges.end()) return false;
         }
      }
      
      Layer *layer;
      MLP *mlp;
   };
   
   void add(Connection* connection);
   void add(Input_Edge* input_edge);
   
   void make_unordered_transit_list();
   void make_input_to_top_transmit_list();
   void make_bottom_to_top_transmit_list();
   void make_path_transit_list(Layer* from, Layer* to);
   void make_path_from_data_to_top(std::string dataname);
   
   MLP *make_path(Layer* from, Layer* to);
   MLP *make_path_to_bottom(Layer* src);
   MLP *make_path_from_bottom(Layer* dest);
   RBM *make_rbm_level(int level);
   MLP *make_level_to_level(int bot, int top);
   
   void init_data();
   
   void set_status_all(Node_status_flag_t);
   
   int transmit(Direction_flag_t);
   
   void make_batch(int batch_size);
   void make_batch_for_whole_input();
   
   void getReconstructionCost();
   bool check_levels();
   
   void transport_data(MLP *to_mlp);
   bool is_hanging(Edge *edge);
};

#endif /* defined(__DBN__MLP__) */
