(set-logic QF_LIA)
(set-info :status sat)
(declare-fun x () Int)
(assert (= 0 (mod x 2)))
(check-sat)
