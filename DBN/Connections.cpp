//
//  Connections.cpp
//  DBN
//
//  Created by Devon Hjelm on 8/15/12.
//
//

#include "Connections.h"
#include "Layers.h"
#include "Types.h"

Connection::Connection(Layer *from_layer, Layer *to_layer) {
   learning_on = true;
   from = from_layer, to = to_layer;
   
   weights = gsl_matrix_float_alloc(to->nodenum, from->nodenum);
   for (int i = 0; i < to->nodenum; ++i)
      for (int j = 0; j < from->nodenum; ++j)
         gsl_matrix_float_set(weights, i, j, (float)gsl_ran_gaussian(r, 0.01));
   
   mat_update = gsl_matrix_float_calloc(to->nodenum, from->nodenum);
   node_projections = gsl_vector_float_alloc(from->nodenum);
}

void Connection::make_batch(int batchsize){
   from->make_batch(batchsize);
   to->make_batch(batchsize);
}

int Connection::transmit_signal(Sample_flag_t s_flag){
   
   CBLAS_TRANSPOSE_t transFlag;
   Layer *input, *output;
   
   if (direction_flag == FORWARD) {
      input = from;
      output = to;
      transFlag = CblasNoTrans;
   }
   
   else if (direction_flag == BACKWARD){
      input = to;
      output = from;
      transFlag = CblasTrans;
   }
   
   switch (input->status) {
      case FROZEN    : break;
      case ACTIVATED : input->finish_activation(s_flag); break;
      case SAMPLED   : break;
   }
   
   switch (output->status) {
      case FROZEN    : return 1;
      case ACTIVATED : break;
      case SAMPLED   : gsl_matrix_float_set_zero(output->activations); break;
   }
   
   gsl_blas_sgemm(transFlag, CblasNoTrans, 1, weights, input->samples, 1, output->activations);
   output->status = ACTIVATED;
   return 1;
}

void Connection::catch_stats(Stat_flag_t stat_flag, Sample_flag_t sample_flag){
   
   stat1 = to->stat1;
   stat2 = from->stat1;
   stat3 = to->stat2;
   stat4 = from->stat2;
   
   from->catch_stats(stat_flag, sample_flag);
   if      (stat_flag == NEG) to->catch_stats(stat_flag, NOSAMPLE);
   else if (stat_flag == POS) to->catch_stats(stat_flag, SAMPLE);
}

void Connection::update(ContrastiveDivergence* teacher){
   if (!learning_on) return;
   from->learning_rate = learning_rate;
   to->learning_rate = learning_rate;
   from->update(teacher);
   to->update(teacher);
   
   gsl_matrix_float *weight_update = mat_update;
   float rate = teacher->learning_multiplier*learning_rate/((float)teacher->batchsize);
   //learning_rate/=(float)teacher->batchsize;
   
   gsl_blas_sgemm(CblasNoTrans, CblasTrans , rate, stat1, stat2, teacher->momentum, weight_update);
   gsl_blas_sgemm(CblasNoTrans, CblasTrans , -rate, stat3, stat4, 1, weight_update);
   
   gsl_matrix_float *weightdecay = gsl_matrix_float_alloc(weights->size1, weights->size2);
   gsl_matrix_float_memcpy(weightdecay, weights);
   gsl_matrix_float_scale(weightdecay, decay);
   gsl_matrix_float_sub(weight_update, weightdecay);
   
   gsl_matrix_float_add(weights, weight_update);
   
   gsl_matrix_float_free(weightdecay);
}