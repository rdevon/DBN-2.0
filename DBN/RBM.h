//
//  RBM.h
//  DBN
//
//  Created by Devon Hjelm on 7/19/12.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#ifndef DBN_RBM_h
#define DBN_RBM_h
#include "MLP.h"
#include "Teacher.h"

class Connection;

class RBM : public Learner, public MLP {
public:
   
   float                               free_energy;
   
   ~RBM(){}
   RBM();
   
   void getFreeEnergy();
   void gibbs_HV();
   void gibbs_VH();

   void learn();
   
   void update(ContrastiveDivergence*);
   void catch_stats(Stat_flag_t s);
   void turn_off();
   void toggle_noise();
   
};

#endif
