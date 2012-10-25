//
//  RBM.cpp
//  DBN
//
//  Created by Devon Hjelm on 7/19/12.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#include "SupportFunctions.h"
#include "RBM.h"
#include "Connections.h"
#include "Layers.h"

RBM::RBM () {free_energy = 0;}

void RBM::getFreeEnergy(){
}

void RBM::learn(){
   teacher->teachRBM(this);
}

void RBM::gibbs_HV(){
   transmit(FORWARD);
   transmit(BACKWARD);
}

void RBM::gibbs_VH(){
   transmit(BACKWARD);
   transmit(FORWARD);
}

void RBM::update(ContrastiveDivergence *cd){
   for (auto edge:edges) {
      Connection *connection = (Connection*)(edge);
      connection->update(cd);
   }
}

void RBM::catch_stats(Stat_flag_t stat_flag){
   for (auto edge:edges) {
      Connection *connection = (Connection*)(edge);
      connection->catch_stats(stat_flag, SAMPLE);
   }
}

void RBM::turn_off() {
   for (auto edge:edges) {
      ((Connection*)edge)->learning_on = false;
      edge->from->learning_on = false;
      edge->to->learning_on = false;
   }
}

void RBM::toggle_noise() {
   for (auto edge:edges) {
      edge->to->noise = !edge->to->noise;
      edge->from->noise = !edge->from->noise;
   }
}