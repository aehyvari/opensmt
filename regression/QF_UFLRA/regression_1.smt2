(set-logic QF_UFLRA)
(set-info :status unsat)
(declare-fun f (Real Real) Real)
(assert (and (< 0 (f 0.0 0.0)) (= 0.0 (f 0.0 0.0))))
(check-sat)
