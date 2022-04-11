//
// Created by Martin Blicha on 29.07.20.
//

#ifndef OPENSMT_VERIFICATIONUTILS_H
#define OPENSMT_VERIFICATIONUTILS_H

#include "SMTConfig.h"
#include "Logic.h"

class VerificationUtils {
    SMTConfig const & config;
    Logic & logic;
public:
    VerificationUtils(SMTConfig const & config, Logic & logic) : config(config), logic(logic) {}

    bool impliesExternal(PTRef, PTRef) const; // Check the result with an external solver

    bool verifyInterpolantExternal(PTRef partA, PTRef partB, PTRef itp) const; // Verify interpolant using an external solver

    bool verifyInterpolantInternal(PTRef partA, PTRef partB, PTRef itp); // Verify interpolant internally, using OpenSMT's MainSolver

    bool impliesInternal(PTRef antecedent, PTRef consequent); // Verify validity of implication `antecedent -> consequent`, using OpenSMT's MainSolver

private:
    bool checkSubsetCondition(PTRef p1, PTRef p2) const;
};



#endif //OPENSMT_VERIFICATIONUTILS_H
