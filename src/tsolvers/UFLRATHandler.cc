#include "UFLRATHandler.h"
#include "lrasolver/LRASolver.h"
#include "TreeOps.h"
#include "InterpolatingEgraph.h"
#include "Egraph.h"

UFLRATHandler::UFLRATHandler(SMTConfig& c, LRALogic& l, vec<DedElem>& d, TermMapper& tmap)
        : LRATHandler(c, l, d, tmap)
        , logic(l)
{
    lrasolver = new LRASolver(config, logic, deductions);
    SolverId lra_id = lrasolver->getId();
    tsolvers[lra_id.id] = lrasolver;
    solverSchedule.push(lra_id.id);

    ufsolver = config.produce_proofs > 0 ? new InterpolatingEgraph(config, logic, deductions)
                                       : new Egraph(config, logic, deductions);

    SolverId uf_id = ufsolver->getId();
    tsolvers[uf_id.id] = ufsolver;
    solverSchedule.push(uf_id.id);

}

UFLRATHandler::~UFLRATHandler() {}

Logic &UFLRATHandler::getLogic()
{
    return logic;
}

PTRef UFLRATHandler::getInterpolant(const ipartitions_t& mask, map<PTRef, icolor_t> *labels)
    {
        InterpolatingEgraph* iegraph = dynamic_cast<InterpolatingEgraph*>(ufsolver);
        assert(iegraph);
        return iegraph->getInterpolant(mask, labels);
    }

