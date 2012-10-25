//
//  main.cpp
//  DBN
//
//  Created by Devon Hjelm on 7/10/12.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#include <iostream>
#include "Types.h"
#include "gsl/gsl_sort_vector.h"
#include "gsl/gsl_permute.h"
#include "IO.h"
#include "Layers.h"
#include "Connections.h"
#include "RBM.h"
#include "Viz.h"
#include "Monitors.h"
#include "DBN.h"

int main (int argc, const char * argv[])
{
   //--------------RNG INIT STUFF
   
   srand((unsigned)time(0));
   
   long seed;
   r = gsl_rng_alloc (gsl_rng_rand48);     // pick random number generator
   seed = time (NULL) * getpid();
   gsl_rng_set (r, seed);                  // set seed
   
   //--------------
   //LOAD DATASET and INIT
   
   DataSet *data1 = new DataSet;
#if 1
   data1->load_single_3D_fMRI();
   
   GaussianLayer *fMRI3D_layer = new GaussianLayer((int)data1->dims[3]);
   Input_Edge *fMRI3D_edge = new Input_Edge(data1, fMRI3D_layer);
   fMRI3D_layer->shapeInput(data1);
   
   ReLULayer *hidden_layer = new ReLULayer(16);
   Connection *connection = new Connection(fMRI3D_layer, hidden_layer);
   connection->learning_rate = 0.0000001;
   connection->decay = 0;
   
   float momentum = 0.7;
   float k = 1;
   float batchsize = 1;
   float epochs = 2000;
   
   ContrastiveDivergence *cd = new ContrastiveDivergence(momentum, k, batchsize, epochs);
   
   Single_3D_Data_Monitor *mon = new Single_3D_Data_Monitor(data1);
   cd->monitor = mon;
   
   DBN *dbn = new DBN;
   dbn->add(fMRI3D_edge);
   dbn->add(connection);
   dbn->teacher = cd;
   dbn->learn();
   
   exit(0);
#endif
   data1->loadfMRI(true,false,false);
   //data1.loadMNIST();
   
   //INIT LAYERS
   
   GaussianLayer fMRI((int)data1->train->size2);
   Input_Edge ie1(data1, &fMRI);
   fMRI.shapeInput(data1);
   
   ReLULayer h1(16);
   Connection c1(&fMRI, &h1);
   c1.learning_rate = 0.000001;
   c1.decay = 0.0000001;
   
   SigmoidLayer h2(16);
   Connection c2(&h1,&h2);
   c2.learning_rate = 0.000001;
   c2.decay = 0.0000001;
   
   SigmoidLayer h3(35);
   Connection c3(&h2,&h3);
   c3.learning_rate = 0.000001;
   c3.decay = 0;
   
   /*
   SigmoidLayer MNIST((int)data1.train->size2);
   Input_Edge ie1(&data1, &MNIST);
   MNIST.shapeInput(&data1);
   
   SigmoidLayer h1(50);
   Connection c1(&MNIST, &h1);
   c1.learning_rate = 0.01;
   c1.decay = 0;
   
   SigmoidLayer h2(50);
   Connection c2(&h1, &h2);
   c1.learning_rate = 0.01;
   c1.decay = 0;

   */
   //LEARNING PARAMETERS
   
   ContrastiveDivergence cdLearner(momentum, k, batchsize, epochs);
   
   //dbn.add(&c3);
   
   
   return 0; 
}

