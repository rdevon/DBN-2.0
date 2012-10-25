//
//  DBN.h
//  DBN
//
//  Created by Devon Hjelm on 8/27/12.
//
//

#ifndef __DBN__DBN__
#define __DBN__DBN__

#include <iostream>
#include "Teacher.h"
#include "MLP.h"
#include "Viz.h"

class RBM;
class Connection;
class Pathway;

class DBN : public Learner, public MLP {
public:
   
   MLP   *rc_MLP;
   
   DBN();
   
   void learn();
};

#endif /* defined(__DBN__DBN__) */
