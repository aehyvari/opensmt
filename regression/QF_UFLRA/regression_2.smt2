(set-info :smt-lib-version 2.6)
(set-logic QF_UFLRA)
(set-info :status unsat)
(declare-fun f3 (Real) Real)
(declare-fun f4 () Real)
(declare-fun f5 () Real)
(assert (= (f3 f4) (- 1)))
(assert (not (=> (= f5 f4) (not (= (f3 f5) 1.0)))))
(check-sat)
(exit)
