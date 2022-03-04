#include "UFLATHandler.h"
#include "lasolver/LASolver.h"
#include "TreeOps.h"
#include "Egraph.h"

UFLATHandler::UFLATHandler(SMTConfig & c, ArithLogic & l)
        : TSolverHandler(c)
        , logic(l)
{
    lasolver = new LASolver(config, logic);
    SolverId lra_id = lasolver->getId();
    tsolvers[lra_id.id] = lasolver;
    solverSchedule.push(lra_id.id);

    ufsolver = new Egraph(config, logic);

    SolverId uf_id = ufsolver->getId();
    tsolvers[uf_id.id] = ufsolver;
    solverSchedule.push(uf_id.id);

}

PTRef UFLATHandler::getInterpolant(const ipartitions_t&, std::map<PTRef, icolor_t> *, PartitionManager &)
{
    throw std::logic_error("Not implemented");
}

lbool UFLATHandler::getPolaritySuggestion(PTRef pt) const {
    if (lasolver->isKnown(pt)) { return lasolver->getPolaritySuggestion(pt); }
    if (ufsolver->isKnown(pt)) { return ufsolver->getPolaritySuggestion(pt); }
    return l_Undef;
}

