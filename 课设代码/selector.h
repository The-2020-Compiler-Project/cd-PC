#pragma once
#include "common.h"
#include "iloc.h"

class Selector {
    vector<InterInst*>& ir;
    ILoc& iloc;
    void translate(InterInst* inst);

public:
    Selector(vector<InterInst*>& ir, ILoc& iloc);
    void select();
};