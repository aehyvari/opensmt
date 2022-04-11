/*
 * Copyright (c) 2021-2022, Antti Hyvarinen <antti.hyvarinen@gmail.com>
 * Copyright (c) 2021-2022, Martin Blicha <martin.blicha@gmail.com>
 *
 *  SPDX-License-Identifier: MIT
 *
 */

#ifndef OPENSMT_REWRITER_H
#define OPENSMT_REWRITER_H

#include "Logic.h"

template<typename TConfig>
class Rewriter {
protected:
    Logic & logic;
    TConfig & cfg;
public:
    Rewriter(Logic & logic, TConfig & cfg) : logic(logic), cfg(cfg) {}

    virtual PTRef rewrite(PTRef root) {
        // MB: If term has no children then single call to config is enough; this can save memory allocations if successful
        if (logic.isVar(root)) {
            return cfg.previsit(root) ? cfg.rewrite(root) : root;
        }

        struct DFSEntry {
            DFSEntry(PTRef term) : term(term) {}
            PTRef term;
            unsigned int nextChild = 0;
        };
        auto termMarks = logic.getTermMarks(logic.getPterm(root).getId());
        Map<PTRef, PTRef, PTRefHash> substitutions;
        vec<PTRef> auxiliaryArgs;
        std::vector<DFSEntry> toProcess;
        toProcess.emplace_back(root);
        while (not toProcess.empty()) {
            auto & currentEntry = toProcess.back();
            PTRef currentRef = currentEntry.term;
            auto currentId = logic.getPterm(currentRef).getId();
            if (not cfg.previsit(currentRef)) {
                toProcess.pop_back();
                termMarks.mark(currentId);
                continue;
            }
            assert(not termMarks.isMarked(currentId));
            Pterm const & term = logic.getPterm(currentRef);
            unsigned childrenCount = term.size();
            if (currentEntry.nextChild < childrenCount) {
                PTRef nextChild = term[currentEntry.nextChild];
                ++currentEntry.nextChild;
                auto childId = logic.getPterm(nextChild).getId();
                if (not termMarks.isMarked(childId)) {
                    toProcess.push_back(DFSEntry(nextChild));
                }
                continue;
            }
            // If we are here, we have already processed all children
            assert(not substitutions.has(currentRef));
            auxiliaryArgs.capacity(childrenCount);
            bool needsChange = false;
            for (unsigned i = 0; i < childrenCount; ++i) {
                PTRef target;
                bool childChanged = substitutions.peek(term[i], target);
                needsChange |= childChanged;
                assert(not childChanged or (logic.getSortRef(target) == logic.getSortRef(term[i])));
                PTRef newChild = childChanged ? target : term[i];
                auxiliaryArgs.push(newChild);
            }
            PTRef newTerm = needsChange ? logic.insertTerm(term.symb(), std::move(auxiliaryArgs)) : currentRef;
            auxiliaryArgs.clear();
            // The reference "term" has now been possibly invalidated! Do not access it anymore!

            PTRef rewritten = cfg.rewrite(newTerm);
            if (rewritten != newTerm or needsChange) {
                assert(logic.getSortRef(currentRef) == logic.getSortRef(rewritten));
                substitutions.insert(currentRef, rewritten);
            }
            termMarks.mark(currentId);
            toProcess.pop_back();
        }

        PTRef target;
        PTRef res = substitutions.peek(root, target) ? target : root;
        return res;
    }
};

class DefaultRewriterConfig {
public:
    virtual bool previsit(PTRef) { return true; } // should continue visiting
    virtual PTRef rewrite(PTRef term) { return term; } // don't do anything
};

class NoOpRewriter : Rewriter<DefaultRewriterConfig> {
    DefaultRewriterConfig config;
public:
    NoOpRewriter(Logic & logic) : Rewriter<DefaultRewriterConfig>(logic, config) {}
};

#endif //OPENSMT_REWRITER_H