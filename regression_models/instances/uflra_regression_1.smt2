(set-logic QF_UFLRA)
(declare-fun f (Real) Real)
(assert (< (f 0.0) 0))
(check-sat)
