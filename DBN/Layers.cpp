//
//  Layers.cpp
//  DBN
//
//  Created by Devon Hjelm on 7/18/12.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#include <iostream>
#include "Layers.h"

Layer::Layer(int nodenum) : LearningUnit(), nodenum(nodenum), batchsize(1), energy(0), noisy(true) {
   learning_on = true;
   activations = gsl_matrix_float_calloc(nodenum, batchsize);
   expectations = gsl_matrix_float_calloc(nodenum, batchsize);
   samples = gsl_matrix_float_calloc(nodenum, batchsize);
   batchbiases = gsl_matrix_float_alloc(nodenum, batchsize);
   
   m_factor = gsl_vector_float_alloc(nodenum);
   sample_vector = gsl_vector_float_alloc(nodenum);
   gsl_vector_float_set_all(m_factor, 1);
   
   vec_update = gsl_vector_float_calloc(nodenum);
   mat_update = gsl_matrix_float_calloc(nodenum, batchsize);
   stat1 = gsl_matrix_float_alloc(nodenum, batchsize);
   stat2 = gsl_matrix_float_alloc(nodenum, batchsize);
   extra = gsl_matrix_float_alloc(nodenum, batchsize);
}

void Layer::make_batch(int bs){
   
   batchsize = bs;
   
   //For batch processing.
   gsl_matrix_float_free(activations);
   gsl_matrix_float_free(expectations);
   gsl_matrix_float_free(samples);
   gsl_matrix_float_free(batchbiases);
   gsl_matrix_float_free(stat1);
   gsl_matrix_float_free(stat2);
   gsl_matrix_float_free(extra);
   
   activations = gsl_matrix_float_calloc(nodenum, batchsize);
   expectations = gsl_matrix_float_calloc(nodenum, batchsize);
   samples = gsl_matrix_float_calloc(nodenum, batchsize);
   batchbiases = gsl_matrix_float_calloc(nodenum, batchsize);
   stat1 = gsl_matrix_float_calloc(nodenum, batchsize);
   stat2 = gsl_matrix_float_calloc(nodenum, batchsize);
   extra = gsl_matrix_float_calloc(nodenum, batchsize);
}

void Layer::expandBiases(){
   for (int j = 0; j < batchsize; ++j) gsl_matrix_float_set_col(batchbiases, j, biases);
}


void Layer::apply_noise(){
   for (int i = 0; i < nodenum; ++i)
      for (int j = 0; j < batchsize; ++j) {
         float u = gsl_rng_uniform(r);
         float val = gsl_matrix_float_get(samples, i, j);
         gsl_matrix_float_set(samples, i, j, val * (u >= noise));
      }
}

void Layer::finish_activation(Sample_flag_t s_flag){
   if (status == SAMPLED) return;
   
   expandBiases();
   gsl_matrix_float_add(activations, batchbiases);
   getExpectations();
   if (s_flag == SAMPLE) sample();
   else gsl_matrix_float_memcpy(samples, expectations);
   
   if (noisy && s_flag == SAMPLE) apply_noise();
   
   status = SAMPLED;
}

void Layer::update(ContrastiveDivergence *teacher){
   if (!learning_on) return;
   gsl_vector_float *bias_update = vec_update;
   float rate = teacher->learning_multiplier*learning_rate/(float)teacher->batchsize;
   gsl_blas_sgemv(CblasNoTrans, rate, stat1, teacher->identity, teacher->momentum, bias_update);
   gsl_blas_sgemv(CblasNoTrans, -rate, stat2, teacher->identity, 1, bias_update);
   gsl_vector_float *decay_term = gsl_vector_float_alloc(nodenum);
   gsl_vector_float_memcpy(decay_term, biases);
   gsl_vector_float_scale(decay_term, decay);
   gsl_vector_float_add(biases, bias_update);
   gsl_vector_float_sub(biases, decay_term);
   gsl_vector_float_free(decay_term);
}

void Layer::catch_stats(Stat_flag_t stat, Sample_flag_t sample){
   gsl_matrix_float *s;
   if (sample == SAMPLE) s = samples;
   else s = expectations;
   if (stat == POS) gsl_matrix_float_memcpy(stat1, s);
   else gsl_matrix_float_memcpy(stat2, s);
}

