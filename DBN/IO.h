//
//  IO.h
//  DBN
//
//  Created by Devon Hjelm on 7/23/12.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#ifndef DBN_IO_h
#define DBN_IO_h
#include "Types.h"
#include <stdint.h>
#include <arpa/inet.h>

class Layer;

class DataSet{
public:
   std::string name;
   int height, width, number, masksize, index;
   Input_t *train, *test, *validation, *extra;
   
   int dims[4];
   
   bool applymask;
   bool denorm;
   
   gsl_vector_float *meanImage;
   gsl_vector_float *mask;
   gsl_vector_float *image;
   gsl_vector_float *norm;
   
   DataSet(){
      masksize = 0, height = 1, width = 1;
      mask = NULL;
      meanImage = NULL;
      image = NULL;
      norm = NULL;
      applymask = false;
      denorm = false;
   }
   
   void loadMNIST();
   void loadfMRI(bool,bool,bool);
   void load_single_3D_fMRI();
   void loadSPM();
   void loadstim();
   void splitValidate(float percentage = .1);
   void removeMeanImage();
   void getMask();
   void removeMask();
   void transform_for_viz(gsl_matrix_float *dest, gsl_vector_float *src);
   void apply_mask(gsl_vector_float *dest, gsl_vector_float *src);
   void normalize();
};


#endif
