/*
 * Copyright (c) 2021, Martin Blicha <martin.blicha@gmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "ArrayTHandler.h"

#include "InterpolatingEgraph.h"

ArrayTHandler::ArrayTHandler(SMTConfig & c, Logic & l)
        : TSolverHandler(c)
        , logic(l)
{
    egraph = config.produce_inter() > 0 ? new InterpolatingEgraph(config, logic)
                                        : new Egraph(config, logic);

    SolverId my_id = egraph->getId();
    tsolvers[my_id.id] = egraph;
    solverSchedule.push(my_id.id);

    arraySolver = new ArraySolver(logic, *egraph, c);
    my_id = arraySolver->getId();
    tsolvers[my_id.id] = arraySolver;
    solverSchedule.push(my_id.id);
}

ArrayTHandler::~ArrayTHandler() {
}

void ArrayTHandler::clearSolver() {
    TSolverHandler::clearSolver();
}

Logic & ArrayTHandler::getLogic() {
    return logic;
}

const Logic & ArrayTHandler::getLogic() const {
    return logic;
}

PTRef
ArrayTHandler::getInterpolant(const ipartitions_t & mask, map<PTRef, icolor_t> *map1, PartitionManager & pmanager) {
    throw std::logic_error("Interpolation not supported yet");
}

lbool ArrayTHandler::getPolaritySuggestion(PTRef pt) const {
    return TSolverHandler::getPolaritySuggestion(pt);
}
