/*********************************************************************
Author: Simone Fulvio Rollini <simone.rollini@gmail.com>

Periplo -- Copyright (C) 2013, Simone Fulvio Rollini

Periplo is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Periplo is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Periplo. If not, see <http://www.gnu.org/licenses/>.
 *********************************************************************/

#include "PG.h"

#include "VerificationUtils.h"

#include <fstream>
#include <sys/wait.h>

bool
ProofGraph::verifyPartialInterpolant(ProofNode *n, const ipartitions_t& mask)
{
    if(verbose())
        std::cout << "; Verifying partial interpolant" << '\n';
    bool res = verifyPartialInterpolantA(n, mask);
    if(!res)
    {
        opensmt_error("Partial interpolant soundness does not hold for A");
        assert(false);
    }
    res = verifyPartialInterpolantB(n, mask);
    if(!res)
    {
        opensmt_error("Partial interpolant soundness does not hold for B");
        assert(false);
    }
    if(verbose())
        std::cout << "; Partial interpolant is sound" << '\n';
    return res;
}

bool
ProofGraph::verifyPartialInterpolantA(ProofNode *n, const ipartitions_t& mask)
{
    // Check A /\ ~(C|a,ab) -> I, i.e., A /\ ~(C|a,ab) /\ ~I unsat
    Logic& logic = this->logic_;
    icolor_t var_class;
    icolor_t var_color;
    std::vector< Lit > & cl = n->getClause();
    vec<PTRef> restricted_clause;

    const size_t size = cl.size( );
    for( size_t i = 0 ; i < size ; i ++ )
    {
        Var v = var(cl[i]);
        var_class = getVarClass2( v );
        assert( var_class == icolor_t::I_B || var_class == icolor_t::I_A || var_class == icolor_t::I_AB );
        if( var_class == icolor_t::I_B || var_class == icolor_t::I_A ) var_color = var_class;
        else
        {
            // Determine color of AB variable
            if( isColoredA( n,v ) ) var_color = icolor_t::I_A;
            else if ( isColoredB( n,v )  ) var_color = icolor_t::I_B;
            else if ( isColoredAB( n,v ) ) var_color = icolor_t::I_AB;
            else opensmt_error( "Variable " << v << " has no color in clause " << n->getId() );
        }
        if( var_color == icolor_t::I_A || var_color == icolor_t::I_AB )
        {
            PTRef ptaux = varToPTRef(var(cl[i]));
            if(sign(cl[i]))
                ptaux = logic.mkNot(ptaux);
            restricted_clause.push(ptaux);
        }
    }

    PTRef cl_ptref = logic.mkNot(logic.mkOr(std::move(restricted_clause)));
    PTRef implicant = logic.mkAnd({pmanager.getPartition(mask, PartitionManager::part::A), cl_ptref});

    /*
    cout << "; MASK IS " << mask << endl;
    cout << "; PARTITION A IS " << logic.printTerm(logic.getPartitionA(mask)) << endl;
    cout << "; TCLAUSE IS " << logic.printTerm(cl_ptref) << endl;
    cout << "; PARTIAL INTERPOLANT IS " << logic.printTerm(n->getPartialInterpolant()) << endl;
    */

    bool res = VerificationUtils(config, logic).impliesExternal(implicant, n->getPartialInterpolant());
    assert(res);
    return res;
}

bool
ProofGraph::verifyPartialInterpolantB(ProofNode *n, const ipartitions_t& mask)
{
    // Check B /\ ~(C|b,ab) -> ~I, i.e., B /\ ~(C|b,ab) /\ I unsat 
    Logic& logic = this->logic_;
    icolor_t var_class;
    icolor_t var_color;
    std::vector< Lit > & cl = n->getClause();
    vec<PTRef> restricted_clause;

    const size_t size = cl.size( );
    for( size_t i = 0 ; i < size ; i ++ )
    {
        Var v = var(cl[i]);
        var_class = getVarClass2( v );
        assert( var_class == icolor_t::I_B || var_class == icolor_t::I_A || var_class == icolor_t::I_AB );
        if( var_class == icolor_t::I_B || var_class == icolor_t::I_A ) var_color = var_class;
        else
        {
            // Determine color of AB variable
            if( isColoredA( n,v ) ) var_color = icolor_t::I_A;
            else if ( isColoredB( n,v )  ) var_color = icolor_t::I_B;
            else if ( isColoredAB( n,v ) ) var_color = icolor_t::I_AB;
            else opensmt_error( "Variable " << v << " has no color in clause " << n->getId() );
        }
        if( var_color == icolor_t::I_B || var_color == icolor_t::I_AB )
        {
            PTRef ptaux = varToPTRef(var(cl[i]));
            if(sign(cl[i]))
                ptaux = logic.mkNot(ptaux);
            restricted_clause.push(ptaux);
        }
    }

    PTRef cl_ptref = logic.mkNot(logic.mkOr(std::move(restricted_clause)));
    PTRef implicant = logic.mkAnd({pmanager.getPartition(mask, PartitionManager::part::B), cl_ptref});

    /*
    cout << "; MASK IS " << mask << endl;
    cout << "; PARTITION B IS " << logic.printTerm(logic.getPartitionB(mask)) << endl;
    cout << "; TCLAUSE IS " << logic.printTerm(cl_ptref) << endl;
    cout << "; PARTIAL INTERPOLANT IS " << logic.printTerm(n->getPartialInterpolant()) << endl;
    */

    bool res = VerificationUtils(config, logic).impliesExternal(implicant, logic.mkNot(n->getPartialInterpolant()));
    assert(res);
    return res;
}

/************************ VERIFICATION OF INTERPOLANTS *********************************/

void ProofGraph::verifyPartialInterpolantFromLeaves( ProofNode* n, const ipartitions_t& A_mask )
{
    if( verbose() > 0 ) { std::cerr << "# Checking soundness of the interpolant" << '\n'; }
    // NOTE no reason at the moment to verify partial interpolants besides those
    // at the leaves and the global one at the root
    assert(n->isLeaf() || isRoot(n));

    PTRef partial_interp = n->getPartialInterpolant();
    assert(partial_interp != PTRef_Undef);

    // Check that the interpolant is on the shared signature
    std::set<PTRef> pred_set;
    getPredicatesSetFromInterpolantIterative( partial_interp, pred_set );
    for (PTRef pred : pred_set) {
        // Skip true and false
        if (pred == logic_.getTerm_true() or pred == logic_.getTerm_false()) continue;
        assert(getVarClass(PTRefToVar(pred), A_mask) == icolor_t::I_AB);
    }
    if( verbose() > 0 ) { std::cerr << "# The interpolant is on the shared signature" << '\n'; }


    // Check A /\ ~(C|a,ab) -> I, i.e., A /\ ~(C|a,ab) /\ ~I unsat

    // First stage: print declarations
    const char * name = "verifyinterp_A.smt2";
    std::ofstream dump_out( name );
    logic_.dumpHeaderToFile( dump_out );

    std::string varsDecl("");
    for(int i = 2; i <= static_cast<int>(getMaxIdVar()); ++i)
    {
        varsDecl += "(declare-fun " + logic_.printTerm(varToPTRef(i)) + " () Bool)\n";
    }
    dump_out << varsDecl << '\n';

    // Print only A atoms
    unsigned A_added = 0;
    for (clauseid_t leave_id : leaves_ids) {
        ProofNode* n = getNode(leave_id);
        icolor_t clause_color = getClauseColor( n->getInterpPartitionMask(), A_mask );
        assert( clause_color == icolor_t::I_A || clause_color == icolor_t::I_B );
        if ( clause_color == icolor_t::I_A )
        {
            if(A_added == 0)
            {
                dump_out << "(assert " << '\n';
                dump_out << "(and" << '\n';
                A_added++;
            }
            printClause( dump_out, n->getClause() );
            dump_out << '\n';
        }
    }
    if(A_added > 0) dump_out << "))" << '\n';

    if(n->isLeaf())
    {
        // Add negation of clause restriction
        // In labeling, classes and colors are distinct
        icolor_t var_class;
        icolor_t var_color;
        std::vector<Lit> & cl = n->getClause();
        const size_t size = cl.size( );

        if( cl.size() > 0 )
        {
            unsigned num_added=0;
            for( size_t i = 0 ; i < size ; i ++ )
            {
                Var v = var(cl[i]);
                var_class = getVarClass2( v );
                assert( var_class == icolor_t::I_B || var_class == icolor_t::I_A || var_class == icolor_t::I_AB );
                if( var_class == icolor_t::I_B || var_class == icolor_t::I_A ) var_color = var_class;
                else
                {
                    // Determine color of AB variable
                    if( isColoredA( n,v ) ) var_color = icolor_t::I_A;
                    else if ( isColoredB( n,v )  ) var_color = icolor_t::I_B;
                    else if ( isColoredAB( n,v ) ) var_color = icolor_t::I_AB;
                    else opensmt_error( "Variable has no color" );
                }
                if( var_color == icolor_t::I_A || var_color == icolor_t::I_AB )
                {
                    if(num_added==0)
                    {
                        dump_out << "(assert" << '\n';
                        dump_out << "(and" << '\n';
                        num_added++;
                    }
                    // Add negated literal
                    if( sign(cl[i]) )
                        dump_out << logic_.printTerm(varToPTRef( v )) << '\n';
                    else
                        dump_out << "(not " << logic_.printTerm(varToPTRef( v )) << " )" << '\n';
                }
            }
            if(num_added>0) dump_out << "))" << '\n';
        }
    }

    // Add partial interpolant
    //dump_out << "(assert (not " << partial_interp << ") )" << '\n';
    logic_.dumpFormulaToFile( dump_out, partial_interp, true );
    dump_out << "(check-sat)" << '\n';
    dump_out << "(exit)" << '\n';
    dump_out.close( );

    // Check !
    bool tool_res;
    if ( int pid = fork() )
    {
        int status;
        waitpid(pid, &status, 0);
        switch ( WEXITSTATUS( status ) )
        {
            case 0:
                tool_res = false;
                break;
            case 1:
                tool_res = true;
                break;
            default:
                perror( "# Error: Certifying solver returned weird answer (should be 0 or 1)" );
                exit( EXIT_FAILURE );
        }
    }
    else
    {
        if( verbose() > 0 ) { std::cerr << "# Checking A -> I" << '\n'; }
        execlp( config.certifying_solver, config.certifying_solver, name, NULL );
        perror( "Error: Certifying solver had some problems (check that it is reachable and executable)" );
        exit( EXIT_FAILURE );
    }
    if ( tool_res == true )
        //opensmt_error( "External tool says A /\\ ~(C|a,ab) -> I does not hold" );
    opensmt_error( "External tool says A -> I does not hold" );

    // Check B /\ ~(C|b,ab) -> ~I, i.e., B /\ ~(C|b,ab) /\ I unsat

    const char * name2 = "verifyinterp_B.smt2";
    dump_out.open( name2 );
    logic_.dumpHeaderToFile( dump_out );

    dump_out << varsDecl << '\n';

    // Print only B atoms
    unsigned B_added = 0;
    for (clauseid_t leave_id : leaves_ids) {
        ProofNode* n = getNode(leave_id);
        icolor_t clause_color = getClauseColor( n->getInterpPartitionMask(), A_mask );
        assert( clause_color == icolor_t::I_A || clause_color == icolor_t::I_B );
        if ( clause_color == icolor_t::I_B )
        {
            if(B_added == 0)
            {
                dump_out << "(assert " << '\n';
                dump_out << "(and" << '\n';
                B_added++;
            }
            printClause( dump_out, n->getClause() );
            dump_out << '\n';
        }
    }
    if(B_added > 0) dump_out << "))" << '\n';

    if(n->isLeaf())
    {
        icolor_t var_class;
        icolor_t var_color;
        std::vector<Lit> & cl = n->getClause();
        const size_t size = cl.size( );
        // Add negation of clause restriction
        // In labeling, classes and colors are distinct
        if( cl.size() > 0 )
        {
            unsigned num_added = 0;
            for( size_t i = 0 ; i < size ; i ++ )
            {
                Var v = var(cl[i]);
                var_class = getVarClass2( v );
                assert( var_class == icolor_t::I_B || var_class == icolor_t::I_A || var_class == icolor_t::I_AB );
                if( var_class == icolor_t::I_B || var_class == icolor_t::I_A ) var_color = var_class;
                else
                {
                    // Determine color of AB variable
                    if( isColoredA( n,v ) ) var_color = icolor_t::I_A;
                    else if ( isColoredB( n,v )  ) var_color = icolor_t::I_B;
                    else if ( isColoredAB( n,v ) ) var_color = icolor_t::I_AB;
                    else opensmt_error( "Variable has no color" );
                }
                if( var_color == icolor_t::I_B || var_color == icolor_t::I_AB )
                {
                    if(num_added==0)
                    {
                        dump_out << "(assert" << '\n';
                        dump_out << "(and" << '\n';
                        num_added++;
                    }
                    // Add negated literal
                    if( sign(cl[i]) )
                        dump_out << logic_.printTerm(varToPTRef( v )) << '\n';
                    else
                        dump_out << "(not " << logic_.printTerm(varToPTRef( v )) << " )" << '\n';
                }
            }
            if(num_added>0) dump_out << "))" << '\n';
        }
    }

    // Add partial interpolant
    //dump_out << "(assert " << partial_interp << " )" << '\n';
    logic_.dumpFormulaToFile( dump_out, partial_interp, false );
    dump_out << "(check-sat)" << '\n';
    dump_out << "(exit)" << '\n';
    dump_out.close( );
    // Check !
    if ( int pid = fork() )
    {
        int status;
        waitpid(pid, &status, 0);
        switch ( WEXITSTATUS( status ) )
        {
            case 0:
                tool_res = false;
                break;
            case 1:
                tool_res = true;
                break;
            default:
                perror( "Error: Certifying solver returned weird answer (should be 0 or 1)" );
                exit( EXIT_FAILURE );
        }
    }
    else
    {
        if ( verbose() > 0 ) { std::cerr << "# Checking B /\\ I -> false" << '\n'; }
        execlp( config.certifying_solver, config.certifying_solver, name2, NULL );
        perror( "Error: Certifying solver had some problems (check that it is reachable and executable)" );
        exit( 1 );
    }
    if ( tool_res == true )
        //opensmt_error( "External tool says B /\\ ~(C|b,ab) -> ~I does not hold" );
    opensmt_error( "External tool says B /\\ I -> false does not hold" );

    remove(name);
    remove(name2);

    if( verbose() > 0 ) { std::cerr << "# The interpolant is sound" << '\n'; }
}


// Path interpolation
// Partitions   phi_1 ... phi_n
// Interpolants I_0 ... I_n
// Generation I_i = Itp(phi_1 ... phi_i | phi_{i+1} ... phi_n)
// Requirement  I_i /\ phi_{i+1} -> I_{i+1}
bool ProofGraph::verifyPathInterpolantsFromLeaves ( vec< PTRef > & interps)
{
    if( verbose() ) std::cerr << "# Verifying the path interpolation property " << '\n';
    unsigned no_part = pmanager.getNofPartitions();

    // m partitions, m+1 interpolants
    assert( (no_part + 1) == static_cast<unsigned>(interps.size()) );

    std::string varsDecl("");
    for(int i = 2; i <= static_cast<int>(getMaxIdVar()); ++i)
    {
        varsDecl += "(declare-fun " + logic_.printTerm(varToPTRef(i)) + " () Bool)\n";
    }

    // Try ith constraint I_i /\ phi_i -> I_{i+1}
    bool property_holds = true;
    for( unsigned j = 0; j < no_part && property_holds; j++ )
    {
        // First stage: print declarations
        const char * name = "interpolationproperty.smt2";
        std::ofstream dump_out( name );
        logic_.dumpHeaderToFile( dump_out );

        dump_out << varsDecl << '\n';

        // Print I_i /\ ~I_{i+1}
        logic_.dumpFormulaToFile( dump_out, interps[j], false );
        logic_.dumpFormulaToFile( dump_out, interps[j+1], true );
        // Print phi_i
        dump_out << "(assert (and " << '\n';
        // NOTE adding constant true in case of empty partition
        // remember that we are working on leaves only
        dump_out << "true " << '\n';
        for (clauseid_t leave_id : leaves_ids) {
            ProofNode* n = getNode(leave_id);
            const ipartitions_t & part = n->getInterpPartitionMask();
            // Clause is in partition 1<= jp <=m if bit jp is set
            int in_j = opensmt::tstbit( part, j+1 );
            if ( in_j )
            {
                printClause( dump_out, n->getClause() );
                dump_out << '\n';
            }
        }
        dump_out << "))" << '\n';
        dump_out << "(check-sat)" << '\n';
        dump_out << "(exit)" << '\n';
        dump_out.close( );

        // Check !
        bool tool_res;
        if ( int pid = fork() )
        {
            int status;
            waitpid(pid, &status, 0);
            switch ( WEXITSTATUS( status ) )
            {
                case 0:
                    tool_res = false;
                    break;
                case 1:
                    tool_res = true;
                    break;
                default:
                    perror( "# Error: Certifying solver returned weird answer (should be 0 or 1)" );
                    exit( EXIT_FAILURE );
            }
        }
        else
        {
            if( verbose() > 0 ) { std::cerr << "# Checking I_"<< j <<" /\\ phi_"<< j+1 <<" -> I_"<<j+1  << '\n'; }
            execlp( config.certifying_solver, config.certifying_solver, name, NULL );
            perror( "Error: Certifying solver had some problems (check that it is reachable and executable)" );
            exit( EXIT_FAILURE );
        }
        if ( tool_res == true )
        {
            if( verbose() > 0 ) std::cerr << "# External tool says inductiveness does not hold" << '\n';
            property_holds = false;
        }
        else remove(name);
    }
    if( verbose() )
    {
        if(property_holds) std::cerr << "# The path interpolation property is satisfied" << '\n';
        else std::cerr << "# The path interpolation property is NOT satisfied" << '\n';
    }
    return property_holds;
}

// Simultaneous abstraction
// Partitions   phi_1 ... phi_n
// Interpolants I_1 ... I_n
// Generation 	I_i = Itp(phi_i | phi_1 ... phi_{i-1}, phi_{i+1} ... phi_n)
// Requirement  I_i /\ ... /\ I_n -> false
bool ProofGraph::verifySimultaneousAbstraction( vec< PTRef > & interps)
{
    if( verbose() ) std::cerr << "# Verifying the simultaneous abstraction property " << '\n';

    // First stage: print declarations
    const char * name = "verifysymmetric.smt2";
    std::ofstream dump_out( name );
    logic_.dumpHeaderToFile( dump_out );

    // Add conjunction interpolants
    for( int i = 0; i < interps.size(); i++ ) {
        logic_.dumpFormulaToFile(dump_out, interps[i], false);
    }
    dump_out << "(check-sat)" << '\n';
    dump_out << "(exit)" << '\n';
    dump_out.close( );

    // Check !
    bool property_holds = true;
    bool tool_res;
    if ( int pid = fork() )
    {
        int status;
        waitpid(pid, &status, 0);
        switch ( WEXITSTATUS( status ) )
        {
            case 0:
                tool_res = false;
                break;
            case 1:
                tool_res = true;
                break;
            default:
                perror( "# Error: Certifying solver returned weird answer (should be 0 or 1)" );
                exit( EXIT_FAILURE );
        }
    }
    else
    {
        if( verbose() > 0 ) { std::cerr << "# Checking I_1 /\\ ... /\\ I_" << interps.size() <<" -> false" << '\n'; }
        execlp( config.certifying_solver, config.certifying_solver, name, NULL );
        perror( "Error: Certifying solver had some problems (check that it is reachable and executable)" );
        exit( EXIT_FAILURE );
    }
    if ( tool_res == true )
    {
        if(verbose()) std::cerr << "# External tool says  I_1 /\\ ... /\\ I_"<< interps.size() << " -> false  does not hold" << '\n';
        property_holds = false;
    }
    else remove(name);

    if( verbose() )
    {
        if(property_holds) std::cerr << "# The simultaneous abstraction property is satisfied" << '\n';
        else std::cerr << "# The simultaneous abstraction property is NOT satisfied" << '\n';
    }
    return property_holds;
}

// Generalized simultaneous abstraction
// Partitions   phi_1 ... phi_n
// Interpolants I_1 ... I_n
// Generation 	for 1<=i<=n-1     I_i = Itp(phi_i | phi_1 ... phi_{i-1}, phi_{i+1} ... phi_n)
//				for i=n			  I_n = Itp(phi_1 ... phi_{n-1} | phi_n)
// Requirement  I_i /\ ... /\ I_{n-1} -> I_n
bool ProofGraph::verifyGenSimultaneousAbstraction( vec< PTRef > & interps)
{
    if( verbose() ) std::cerr << "# Verifying the generalized simultaneous abstraction property " << '\n';

    // First stage: print declarations
    const char * name = "verifysymmetric.smt2";
    std::ofstream dump_out( name );
    logic_.dumpHeaderToFile( dump_out );

    // Add conjunction interpolants
    for( int i = 0; i < interps.size()-1; i++ ) {
        logic_.dumpFormulaToFile(dump_out, interps[i], false);
    }
    // Add negation last interpolant
    logic_.dumpFormulaToFile( dump_out, interps[interps.size()-1], true );
    dump_out << "(check-sat)" << '\n';
    dump_out << "(exit)" << '\n';
    dump_out.close( );

    // Check !
    bool property_holds = true;
    bool tool_res;
    if ( int pid = fork() )
    {
        int status;
        waitpid(pid, &status, 0);
        switch ( WEXITSTATUS( status ) )
        {
            case 0:
                tool_res = false;
                break;
            case 1:
                tool_res = true;
                break;
            default:
                perror( "# Error: Certifying solver returned weird answer (should be 0 or 1)" );
                exit( EXIT_FAILURE );
        }
    }
    else
    {
        if( verbose() > 0 ) { std::cerr << "# Checking I_1 /\\ ... /\\ I_"<< interps.size()-1 <<" -> I_" << interps.size() << '\n'; }
        execlp( config.certifying_solver, config.certifying_solver, name, NULL );
        perror( "Error: Certifying solver had some problems (check that it is reachable and executable)" );
        exit( EXIT_FAILURE );
    }
    if ( tool_res == true )
    {
        if(verbose()) std::cerr << "# External tool says  I_1 /\\ ... /\\ I_"<< interps.size()-1 <<" -> I_" << interps.size() << " does not hold" << '\n';
        property_holds = false;
    }
    else remove(name);
    if( verbose() )
    {
        if(property_holds) std::cerr << "# The generalized simultaneous abstraction property is satisfied" << '\n';
        else std::cerr << "# The generalized simultaneous abstraction property is NOT satisfied" << '\n';
    }
    return property_holds;
}

// State transition interpolation
// Partitions   phi_1 ... phi_n
// Interpolants I_0 ... I_n, J_1 ... J_n
// Generation 	I_i = Itp(phi_1 ... phi_i | phi_{i+1} ... phi_n)
// 				J_i = Itp(phi_i | phi_1 ... phi_{i-1}, phi_{i+1} ... phi_n)
// Requirement  I_i /\ J_{i+1} -> I_{i+1}
// NOTE the vector contains first all the path interpolants and then all the symmetric interpolants
bool ProofGraph::verifyStateTransitionInterpolants ( vec< PTRef > & interps)
{
    if( verbose() ) std::cerr << "# Verifying the state-transition interpolation property " << '\n';

    // m partitions, 2m+1 interpolants
    unsigned no_part = pmanager.getNofPartitions();
    assert( (2*no_part + 1) == static_cast<unsigned>(interps.size()) );

    // Try ith constraint I_i /\ J_{i+1} -> I_{i+1}
    bool property_holds = true;
    for( unsigned j = 0; j < no_part && property_holds; j++ )
    {
        // First stage: print declarations
        const char * name = "interpolationproperty.smt2";
        std::ofstream dump_out( name );
        logic_.dumpHeaderToFile( dump_out );

        // Print I_i /\ J_{i+1} /\ ~I_{i+1}
        logic_.dumpFormulaToFile( dump_out, interps[j], false );
        logic_.dumpFormulaToFile( dump_out, interps[j+(no_part+1)], false );
        logic_.dumpFormulaToFile( dump_out, interps[j+1], true );
        dump_out << "(check-sat)" << '\n';
        dump_out << "(exit)" << '\n';
        dump_out.close( );

        // Check !
        bool tool_res;
        if ( int pid = fork() )
        {
            int status;
            waitpid(pid, &status, 0);
            switch ( WEXITSTATUS( status ) )
            {
                case 0:
                    tool_res = false;
                    break;
                case 1:
                    tool_res = true;
                    break;
                default:
                    perror( "# Error: Certifying solver returned weird answer (should be 0 or 1)" );
                    exit( EXIT_FAILURE );
            }
        }
        else
        {
            if( verbose() > 0 ) { std::cerr << "# Checking I_"<< j <<" /\\ J_"<< j+1 <<" -> I_"<< j+1 << '\n'; }
            execlp( config.certifying_solver, config.certifying_solver, name, NULL );
            perror( "Error: Certifying solver had some problems (check that it is reachable and executable)" );
            exit( EXIT_FAILURE );
        }
        if ( tool_res == true )
        {
            if(verbose()) std::cerr << "# External tool says inductiveness does not hold" << '\n';
            property_holds = false;
        }
        else remove(name);
    }
    if( verbose() )
    {
        if(property_holds) std::cerr << "# The state-transition interpolation property is satisfied" << '\n';
        else std::cerr << "# The state-transition interpolation property is NOT satisfied" << '\n';
    }
    return property_holds;
}


// Tree interpolation
// Partitions   phi_1 ... phi_n
// Subtrees		F_1   ... F_n
// Interpolants I_1 ... I_n
// Generation 	I_j = Itp(F_j | all other formulae)
// Requirement  ( /\_(j,k) I_k /\ phi_j ) -> I_j
bool ProofGraph::verifyTreeInterpolantsFromLeaves( opensmt::InterpolationTree* itree, vec< PTRef > & interps)
{
    if( verbose() ) std::cerr << "# Verifying the tree interpolation property" << '\n';

    // m partitions, one for each node of the tree
    // TODO m or less interpolants - not necessarily all nodes are approximated
    // for nodes non approximated, interpolants are the original formulae
    assert( pmanager.getNofPartitions() == static_cast<unsigned>(interps.size()) );

    // NOTE partition ids start from 1, interpolants vector from 0
    // interpolants[i] contains interpolant for node with partition id i+1
    std::deque< opensmt::InterpolationTree* > que;
    que.push_back( itree );
    bool property_holds = true;
    do
    {
        opensmt::InterpolationTree* itr = que.front();
        que.pop_front();
        int j = itr->getPartitionId();

        // Visit j_th node and verify constraint ( /\_(j,k) I_k /\ phi_j ) -> I_j
        // NOTE partition_ids are numbered from 1

        // First stage: print declarations
        const char * name = "interpolationproperty.smt2";
        std::ofstream dump_out( name );
        logic_.dumpHeaderToFile( dump_out );

        // Print /\_(j,k) I_k and add children to queue
        for (opensmt::InterpolationTree * tree : itr->getChildren()) {
            int k = tree->getPartitionId();
            logic_.dumpFormulaToFile(dump_out, interps[k-1], false);
            que.push_back(tree);
        }
        // Print phi_j
        int jp = itr->getPartitionId();
        dump_out << "(assert (and" << '\n';
        // NOTE adding constant true in case of empty partition
        // remember that we are working on leaves only
        dump_out << "true " << '\n';
        for (clauseid_t leave_id : leaves_ids) {
            ProofNode* n = getNode(leave_id);
            const ipartitions_t & part = n->getInterpPartitionMask();
            // Clause is in partition 1<= jp <=m if bit jp is set
            int in_jp = opensmt::tstbit( part, jp );
            if ( in_jp )
            {
                printClause( dump_out, n->getClause() );
                dump_out << '\n';
            }
        }
        dump_out << "))" << '\n';
        // Print ~I_j
        logic_.dumpFormulaToFile( dump_out, interps[j-1], true );

        dump_out << "(check-sat)" << '\n';
        dump_out << "(exit)" << '\n';
        dump_out.close( );

        // Check !
        bool tool_res;
        if ( int pid = fork() )
        {
            int status;
            waitpid(pid, &status, 0);
            switch ( WEXITSTATUS( status ) )
            {
                case 0:
                    tool_res = false;
                    break;
                case 1:
                    tool_res = true;
                    break;
                default:
                    perror( "# Error: Certifying solver returned weird answer (should be 0 or 1)" );
                    exit( EXIT_FAILURE );
            }
        }
        else
        {
            //( /\_(j,k) I_k /\ phi_j ) -> I_j
            if( verbose() > 0 ) { std::cerr << "# Checking /\\_("<< j <<",k) I_k /\\ phi_"<< j << " -> I_"<< j << '\n'; }
            execlp( config.certifying_solver, config.certifying_solver, name, NULL );
            perror( "Error: Certifying solver had some problems (check that it is reachable and executable)" );
            exit( EXIT_FAILURE );
        }
        if ( tool_res == true )
        {
            std::cerr << "# External tool says inductiveness does not hold" << '\n';
            property_holds = false;
        }
        else remove(name);
    }
    while( !que.empty() && property_holds );

    if( verbose() )
    {
        if(property_holds) std::cerr << "# The tree interpolation property is satisfied" << '\n';
        else std::cerr << "# The tree interpolation property is NOT satisfied" << '\n';
    }
    return property_holds;
}
