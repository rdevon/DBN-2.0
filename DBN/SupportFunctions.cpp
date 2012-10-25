//
//  SupportFunctions.cpp
//  DBN
//
//  Created by Devon Hjelm on 7/20/12.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#include <iostream>
#include "SupportFunctions.h"

gsl_vector_int *makeShuffleList(int length){
   
   
   gsl_vector_int *list = gsl_vector_int_alloc(length);
   
   for (int i = 0; i < 10; ++i) gsl_vector_int_set(list, i, i);
   
   gsl_ran_shuffle(r, list->data, list->size, sizeof(int));
   
   return list;
}

void print_gsl(gsl_vector_float *v){
   for (int i = 0; i < v->size; ++i) std::cout << gsl_vector_float_get(v, i) << " ";
   std::cout << std::endl;
}

void print_gsl(gsl_vector_int *v){
   for (int i = 0; i < v->size; ++i) std::cout << gsl_vector_int_get(v, i) << " ";
   std::cout << std::endl;
}

void print_gsl(gsl_matrix_float *m){
   for (int j = 0; j < m->size2; ++j){
      for (int i = 0; i < m->size1; ++i) std::cout << gsl_matrix_float_get(m, i, j) << " ";
      std::cout << std::endl;
   }
}

void load_vec_into_matrix(gsl_vector_float *v, gsl_matrix_float *m){
   gsl_matrix_float_set_zero(m);
   for (int i = 0; i < m->size1; ++i)
      for (int j = 0; j < m->size2; ++j) {
         float val = gsl_vector_float_get(v, i*m->size2+j);
         gsl_matrix_float_set(m,i,j,val);
      }
}

void load_vec_into_matrix(gsl_matrix_float *v, gsl_matrix_float *m){
   gsl_matrix_float_set_zero(m);
   for (int i = 0; i < m->size1; ++i)
      for (int j = 0; j < m->size2; ++j) {
         float val = gsl_matrix_float_get(v, i*m->size2+j, 0);
         gsl_matrix_float_set(m,i,j,val);
      }
}

std::string readTextFile(const std::string& filename)
{
   std::ifstream infile(filename.c_str()); // File stream
   std::string source;                     // Text file string
   std::string line;                       // A line in the file
   
   // Make sure the file could be opened
   if(!infile.is_open())
   {
      std::cerr << "Could not open file: " << filename << std::endl;
   }
   
   // Read in the source one line at a time, then append it
   // to the source string. Not efficient.
   while(infile.good())
   {
      getline(infile, line);
      source = source + line + "\n";
   }
   
   infile.close();
   return source;
}

void save_gsl_matrix(gsl_matrix_float *m){
   std::string path = plotpath + "test.out";
   FILE *file_handle;
   file_handle = fopen(path.c_str(), "w");
   gsl_matrix_float_fprintf(file_handle, m, "%.5g");
   fclose(file_handle);
}