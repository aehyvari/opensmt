(set-option :produce-interpolants true)
(set-option :pure-lookahead)
(set-logic QF_UF)
(set-info :status unsat)
(declare-sort I 0)
(declare-fun op (I I) I)
(declare-fun e4 () I)
(declare-fun e () I)
(assert (! (let ((?v_19 (op (op e4 e4) e4)) (?v_17 (op (op (op e e) e) (op e e)))) (and (or (or (= e4 (op e e4)) (= (op e4 e4) (op e4 (op e4 e4)))) (= (op e (op (op e e) e)) (op (op (op e e) e) (op e e)))) (or (= e4 (op e (op (op (op e e) e) (op e e)))) (or (= e4 (op e4 e4)) (= (op e4 e) (op e4 e4)))) (or (= e4 (op e4 e4)) (or (= e (op e (op e e))) (= (op e4 e4) (op e4 (op e4 e4))))) (or (= e4 (op e e4)) (or (= (op e e4) (op (op e e) e)) (= (op e4 e4) (op (op e4 e) (op e e4))))) (or (= e4 (op e4 e4)) (or (= (op e4 e4) (op e4 e)) (= e (op (op (op e e) e) e)))) (or (= e4 (op e4 e4)) (or (= (op e4 e4) (op (op e4 e4) (op e4 e4))) (= (op (op e e) e) (op (op (op e e) e) (op (op e e) e))))) (or (= e4 (op e4 (op e e4))) (or (= e4 (op e4 e4)) (= (op e e) (op (op (op e e) e) (op (op (op e e) e) (op e e)))))) (or (= e4 (op (op (op e e) e) e4)) (or (= (op e e4) (op e4 e4)) (= (op e4 e4) (op e4 e)))) (or (= e4 (op e4 e)) (or (= (op e4 e) (op e4 e4)) (= (op (op (op e e) e) (op e e)) (op (op (op (op e e) e) (op e e)) e)))) (or (= e4 (op e4 e4)) (or (= (op e4 e4) (op e e4)) (= e (op (op (op (op e e) e) (op e e)) (op (op e e) e))))) (or (= e4 (op e4 e4)) (or (= (op e4 e4) (op e e4)) (= (op (op e e) e) (op (op (op (op e e) e) (op e e)) (op (op (op e e) e) (op e e)))))) (or (= e4 (op (op (op (op e e) e) (op e e)) (op e e))) (or (= (op e4 e4) (op e4 e)) (= (op e4 e4) (op (op e4 e4) (op (op e4 e4) (op e e4)))))) (or (= e4 (op e4 (op e4 e))) (or (= (op e4 e4) (op (op e4 e4) (op e4 e4))) (= (op e e) (op (op (op (op e e) e) (op e e)) e4)))) (or (= e4 (op (op e e) (op (op e e) e))) (or (= e4 (op e4 e)) (= (op e4 e4) (op e4 e)))) (or (= e4 (op (op e4 e4) (op (op e4 e4) (op e4 e)))) (or (= e (op (op e e) (op (op (op e e) e) (op e e)))) (= (op e4 e4) (op (op e e) (op (op e4 e4) (op e4 e4)))))) (or (= e4 (op (op e e) (op e e))) (or (= e4 (op e4 e4)) (= (op e e) (op (op e e) (op e e))))) (or (= e4 (op (op e e) e4)) (or (= (op e4 e4) (op e4 e)) (= (op (op e e) e4) (op (op (op e e) e) (op e e))))) (or (= (op e e) (op e4 (op (op e e) e))) (or (= (op e4 e4) (op e4 e)) (= e4 (op e4 (op e4 (op e4 e)))))) (or (= e4 (op e4 e)) (or (= (op e e4) (op e4 e4)) (= (op (op (op e e) e) (op e e)) (op e4 (op (op (op e e) e) (op e e)))))))) :named NzAsoSoDTNNtSwa ))
(assert (! (and (or (= e4 (op e4 e)) (or (= e4 (op e4 e4)) (= (op e e4) (op (op (op e e) e) (op e e))))) (or (= (op e4 (op e e)) (op (op e e) e)) (or (= e4 (op e4 e4)) (= (op e e4) (op (op e e) (op (op (op e e) e) (op e e)))))) (or (= (op (op e e) e4) (op (op (op e e) e) (op e e))) (or (= e4 (op e4 e4)) (= (op e e) (op (op (op (op e e) e) (op e e)) (op e e))))) (or (= e4 (op e4 e4)) (or (= (op e e4) (op (op (op e e) e) (op e e))) (= (op (op (op e e) e) (op e e)) (op (op (op (op e e) e) (op e e)) e))))) :named CVXixMTnMRdm ))
(assert (! (or (= (op (op (op e e) e) (op e e)) (op e4 (op (op (op e e) e) (op e e)))) (and (not (= (op e e4) (op (op e e4) (op e e4)))) (and (not (= e4 (op e4 e4))) (not (= (op e4 e4) (op e4 e)))))) :named TChjxTzKUoBw ))
(assert (! (and (or (= (op e e) (op (op (op (op e e) e) (op e e)) (op e e))) (or (= e4 (op e4 e4)) (and (= e4 (op (op e e) (op (op e4 e4) e4))) (= (op e e) (op e4 (op (op e e) e)))))) (or (= e4 (op (op e e) (op (op e e) e))) (or (= e4 (op (op e4 e4) (op e4 (op e4 e4)))) (and (= e4 (op (op e4 e4) (op e4 (op e4 e4)))) (= e4 (op (op e e) (op (op (op e e) e) (op e e)))))))) :named IzmygTEiGlaZ ))
(assert (! (or true (= e4 (op e4 (op e4 e4)))) :named zLIPHukTCnRQg ))
(assert (! (and (not (= (op e4 e4) (op e4 e))) (not (= (op e4 e4) (op e e4))) (not (= (op e4 e4) (op e e))) (not (= (op e4 e) (op e e))) (= (op e4 e4) (op e4 (op e4 e))) (= (op e4 e) (op (op e4 e) e)) (= (op e4 e4) (op (op e4 e) e4)) (not (= (op e4 e4) (op (op e e4) e4))) (not (= (op e4 e4) (op e4 (op e e)))) (not (= (op e4 e4) (op (op e e) e4))) (not (= (op e4 e) (op e4 (op e e)))) (not (= (op e e4) (op (op e e4) e4))) (not (= (op e e) (op e4 (op e e)))) (not (= (op e4 e) (op (op e4 e4) e4))) (not (= (op e4 e) (op e4 (op e4 e)))) (not (= (op e e4) (op e4 (op e4 e)))) (not (= (op e e4) (op e (op e4 e4)))) (not (= (op e e4) (op e4 (op e e4)))) (not (= (op e4 e4) (op e (op e e4)))) (not (= (op e4 e4) (op e (op e4 e)))) (not (= (op e4 e) (op e4 (op e e4)))) (not (= (op e4 e4) (op e4 (op e e4)))) (not (= (op e e4) (op (op e4 e) e))) (not (= (op e4 e4) (op (op e4 e) e))) (not (= (op e e4) (op e (op e e4)))) (not (= (op e4 e) (op (op e e4) e))) (not (= (op e e4) (op (op e4 e) e4))) (not (= (op e e) (op e (op e4 e)))) (not (= (op e e) (op e (op e e)))) (= (op e4 e4) (op (op e4 e) (op e4 e))) (= (op e4 e4) (op (op e e4) (op e4 e4))) (= (op e4 e) (op e4 (op e4 (op e4 e)))) (not (= (op e (op e4 e)) (op e4 (op e4 e)))) (not (= (op e4 e4) (op e4 (op e (op e e))))) (not (= (op e4 (op e e)) (op e4 (op e e4)))) (not (= (op (op e4 e) e4) (op e4 (op e4 e4)))) (not (= (op e (op e4 e)) (op e4 (op e e4)))) (not (= (op e (op e e4)) (op e4 (op e e4)))) (not (= (op e4 e4) (op e4 (op e4 (op e e4))))) (not (= (op e4 e4) (op e4 (op e (op e4 e))))) (not (= (op e e) (op e4 (op e4 (op e4 e))))) (not (= (op e4 e) (op e4 (op e (op e4 e))))) (not (= (op e4 (op e4 e)) (op e4 (op e e4)))) (not (= (op e4 e4) (op (op e e4) (op e e4)))) (not (= (op e4 e) (op (op e e4) (op e e4)))) (not (= (op e4 (op e4 e)) (op e (op e4 e4)))) (not (= (op e4 e4) (op (op e e4) (op e e)))) (not (= (op e4 e4) (op e4 (op e4 (op e e))))) (not (= (op e4 e4) (op e4 (op e4 (op e4 e))))) (not (= (op e4 e4) (op (op e e) (op e4 e4)))) (not (= (op e4 e) (op (op e4 e4) (op e e)))) (not (= (op e e4) (op (op e4 e) (op e e4)))) (not (= (op e e4) (op (op e e4) (op e4 e4)))) (not (= (op e e4) (op e4 (op e4 (op e4 e))))) (not (= (op e e4) (op (op e e4) (op e e)))) (not (= (op e4 e4) (op (op e e4) (op e4 e)))) (not (= (op e e4) (op (op e e) (op e4 e)))) (not (= (op e4 e4) (op (op e e) (op e e4)))) (not (= (op e e4) (op (op e e) (op e e)))) (not (= (op e e4) (op (op e e) (op e e4)))) (= (op (op e e4) e) (op e4 (op e4 (op e4 e4)))) (not (= (op e4 e4) (op (op e e4) (op e4 (op e e4))))) (not (= (op e4 e) (op (op (op e e) e) (op e e)))) (not (= (op e4 e4) (op (op e4 (op e4 e)) (op e4 e4)))) (not (= (op e4 e) (op (op e4 e) (op e4 (op e4 e4))))) (not (= (op e4 e4) (op (op e4 e) (op (op e e4) e)))) (not (= (op e4 e4) (op (op e4 e) (op e (op e4 e))))) (not (= (op e4 (op e4 e)) (op (op e e4) (op e e4)))) (not (= (op e (op e4 e4)) (op (op e4 e4) (op e e4)))) (not (= (op e4 (op e e4)) (op (op e e4) (op e4 e4)))) (not (= (op e4 (op e e4)) (op (op e4 e) (op e4 e4)))) (not (= (op e4 e4) (op (op e4 e4) (op e4 (op e4 e))))) (not (= (op e4 e4) (op (op e4 e4) (op e4 (op e e))))) (not (= (op e e4) (op e4 (op (op e e4) (op e4 e4))))) (not (= (op e4 e4) (op (op e4 e4) (op e (op e4 e))))) (not (= (op e e4) (op (op e4 e) (op e4 (op e4 e4))))) (not (= (op e e4) (op (op e4 e4) (op e4 (op e4 e))))) (not (= (op e4 e4) (op (op e e) (op e4 (op e4 e4))))) (not (= (op e4 e4) (op (op e4 e) (op e4 (op e e4))))) (not (= (op e4 e4) (op (op e4 e4) (op (op e e4) e)))) (not (= (op e4 (op e4 e4)) (op (op e4 e) (op e e4)))) (not (= (op e4 e4) (op (op e4 e) (op e4 (op e e))))) (not (= (op e4 e4) (op (op e4 e) (op e (op e4 e4))))) (not (= (op e (op e e4)) (op (op e e) (op e e4)))) (not (= (op e4 e4) (op (op e4 (op e4 e)) (op e4 e)))) (not (= (op e4 (op e4 e4)) (op (op e4 e4) (op e4 (op e4 e))))) (not (= (op e4 (op e4 (op e4 e4))) (op (op e4 e4) (op e e4)))) (not (= (op e e4) (op e4 (op (op (op e e) e) (op e e))))) (not (= (op e4 e4) (op e4 (op (op e4 e4) (op e4 (op e4 e)))))) (not (= (op (op e4 e4) (op e4 e4)) (op (op e4 e) (op e4 e4)))) (not (= (op (op e e4) (op e4 e4)) (op (op e4 e4) (op e4 e4)))) (not (= (op e e4) (op (op e4 e4) (op (op e4 e4) (op e4 e4))))) (not (= (op e4 e4) (op (op e4 e4) (op (op e4 e4) (op e e))))) (not (= (op (op e4 e4) (op e4 e4)) (op (op e4 e) (op e4 e)))) (= (op e (op e4 (op e e))) (op (op (op e e) e) (op e e))) (not (= (op e4 e4) (op (op e4 (op e4 e4)) (op (op e4 e) (op e e4))))) (not (= (op e4 (op e4 e4)) (op (op e4 e4) (op (op e4 e) (op e e4))))) (not (= (op (op e e) e4) (op (op e e) (op (op (op e e) e) (op e e))))) (not (= (op (op e e) (op e e)) (op (op e e) (op (op e e) (op e (op e e)))))) (not (= (op (op e e) (op e e)) (op (op (op (op e e) e) (op e e)) (op e e))))) :named TOxXaAGUgyir ))
(assert (! (and (not (= e e4)) (not (= e4 (op e e))) (not (= e4 (op e4 (op e e4)))) (not (= e4 (op e4 (op e4 e)))) (not (= e4 (op (op e e4) e))) (not (= e (op (op e4 e) (op e4 (op e4 e))))) (not (= e (op e4 (op (op e e4) (op e4 e4))))) (not (= (op e e4) (op (op e e4) (op e4 e)))) (not (= (op e4 e) (op (op e4 e4) (op e4 (op e e4)))))) :named cqnHAgAkys ))
(assert (! (= e (op e4 e4)) :named WkdrAUTyuBVZpg ))
(assert (! (not (and (= (op (op e e) e) (op e4 (op e e))) (and (= (op (op (op e e) e) (op e e)) (op e4 (op (op (op e e) e) (op e e)))) (and (= (op e e) (op e4 (op (op e e) e))) (and (= e4 (op e4 e)) (= (op (op e e) e4) (op (op (op e e) e) (op e e))) (and (= (op e e) (op (op e e) (op e e))) (and (= e (op e (op e e))) (= e4 (op (op (op e e) e) e4)) (= (op e e4) (op (op e e) e)) (= e (op (op (op e e) e) e)) (= e4 (op (op e e) (op (op e e) e))) (= e (op (op e e) (op (op (op e e) e) (op e e)))) (= e4 (op (op (op (op e e) e) (op e e)) (op e e))) (= (op e e) (op (op (op (op e e) e) (op e e)) e4)) (= e (op (op (op (op e e) e) (op e e)) (op (op e e) e))) (= (op (op e e) e) (op (op (op e e) e) (op (op e e) e))) (= (op e e) (op (op (op e e) e) (op (op (op e e) e) (op e e)))) (= (op (op (op e e) e) (op e e)) (op (op (op (op e e) e) (op e e)) e)) (= (op (op e e) e) (op (op (op (op e e) e) (op e e)) (op (op (op e e) e) (op e e)))) (and (= e4 (op e (op (op (op e e) e) (op e e)))) (= (op e (op (op e e) e)) (op (op (op e e) e) (op e e))))))))))) :named rBzIDwgNLHH ))
(check-sat)
(get-interpolants (and CVXixMTnMRdm IzmygTEiGlaZ TOxXaAGUgyir WkdrAUTyuBVZpg ) (and NzAsoSoDTNNtSwa TChjxTzKUoBw zLIPHukTCnRQg cqnHAgAkys rBzIDwgNLHH ))
