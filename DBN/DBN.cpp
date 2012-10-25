//
//  DBN.cpp
//  DBN
//
//  Created by Devon Hjelm on 8/27/12.
//
//

#include "DBN.h"
#include "Connections.h"
#include "RBM.h"
#include "Layers.h"
#include "IO.h"
#include "Monitors.h"

DBN::DBN() {rc_MLP = new MLP;}

void DBN::learn(){
   teacher->monitor->teacher = teacher;
   int level = 0;
   for (auto input:inputs) rc_MLP->add(input);
   while (1){
      RBM *rbm = make_rbm_level(level);
      for (auto edge:(rbm->edges)) rc_MLP->add((Connection*)edge);
      
      if (rbm->edges.size() == 0) break;
      //if (level > 0) rbm->toggle_noise();
      
      rbm->teacher = teacher;
      rbm->learn();
      //rbm->turn_off();
      std::cout << "Done training " << level << " layer." << std::endl;
      ++level;
   }
}