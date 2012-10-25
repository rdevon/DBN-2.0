//
//  Types.h
//  DBN
//
//  Created by Devon Hjelm on 7/11/12.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#ifndef DBN_Types_h
#define DBN_Types_h

#include <algorithm>
#include <stdlib.h>
#include "gsl/gsl_matrix.h"
#include "gsl/gsl_rng.h"
#include "gsl/gsl_vector.h"
#include <gsl/gsl_randist.h>
#include <gsl/gsl_permutation.h>
#include <gsl/gsl_randist.h>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_sort_vector.h>
#include <gsl/gsl_permute.h>
#include <vector>
#include <string>
#include <map>

#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include "dirent.h"
#include <sstream>
#include <gsl/gsl_statistics.h>
#define PI 3.1415

extern std::string MNISTpath;
extern std::string plotpath;
extern std::string fMRIpath;
extern std::string SPMpath;
extern std::string vertexPath;
extern std::string fragmentPath;
extern std::string fMRI_3D_path;
extern gsl_rng * r;

typedef gsl_matrix_float   Input_t;

typedef enum{BACKWARD, FORWARD} Direction_flag_t;

typedef enum{SAMPLE, NOSAMPLE} Sample_flag_t;

typedef enum{NEG, POS} Stat_flag_t;

typedef enum{TRAIN, TEST, TIMECOURSE} Data_flag_t;

typedef enum{FROZEN, ACTIVATED, SAMPLED} Node_status_flag_t;

typedef enum{WHITE = -100, GREY, BLACK, BLUE, RED, GREEN, YELLOW} Color_t;

#endif
