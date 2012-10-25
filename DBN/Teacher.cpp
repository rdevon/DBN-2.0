//
//  Teacher.cpp
//  DBN
//
//  Created by Devon Hjelm on 8/10/12.
//
//

#include "Teacher.h"
#include "RBM.h"
#include "Viz.h"
#include "SupportFunctions.h"
#include "Layers.h"
#include "Connections.h"
#include "Monitors.h"

void Teacher::multiply_rate() {
   learning_multiplier *=2;
}

void Teacher::divide_rate() {
   learning_multiplier /=2;
}

ContrastiveDivergence::ContrastiveDivergence(float momentum, int k, int batchsize, int e) : momentum(momentum), k(k), batchsize(batchsize), epochs(e)
{
   monitor = NULL;
   identity = gsl_vector_float_alloc(batchsize);
   gsl_vector_float_set_all(identity, 1);
}

void ContrastiveDivergence::getStats(RBM *rbm){
   
   rbm->make_bottom_to_top_transmit_list();
   // Positive stats
   rbm->catch_stats(POS);
   
   // Gibbs VH sample k times.
   for(int g = 0; g < k; ++g) rbm->gibbs_VH();
     // Negative stats.
   rbm->catch_stats(NEG);
}

void ContrastiveDivergence::teachRBM(RBM *rbm){
   learning = true;
   int epoch = 0;
   learning_multiplier = 1;
   while (learning){
      
      rbm->make_batch(batchsize);
      rbm->sample_flag = SAMPLE;
      rbm->d_flag = TRAIN;
      
      // Get dimensions
      
      std::cout << std::endl << "Teaching RBM with input, epoch" << epoch << std::endl << "     K: " << k << std::endl << "     Batch Size: " << batchsize << std::endl << "Learning multiplier " << learning_multiplier << std::endl;
      
      rbm->init_data();
      // Loop through the input
      int batchnumber = 1;
      rbm->make_input_to_top_transmit_list();
      while ( rbm->transmit(FORWARD) ){
         
         // Gets statistics by performing CD.
         getStats(rbm);
         
         // Update the parameters.
         rbm->update(this);
         
         // And monitor
         if (batchnumber%100 == 0) {
            //monitor->update();
            std::cout << std::flush;
            std::cout << "Batch number: " << batchnumber << std::endl;
            //monitor->update();
         }
         batchnumber+=1;
         rbm->make_batch(batchsize);
         rbm->make_input_to_top_transmit_list();
      }
      ++epoch;
      monitor->update();
   }
   
}
