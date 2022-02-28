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

#include "OsmtInternalException.h"

#include <cmath>
#include <deque>

short ProofNode::hasOccurrenceBin(Var v) {
    std::vector<Lit> const & cla = getClause();
    int first=0;
    int last=cla.size()-1;

    while (first <= last) {
        int mid = (first + last) / 2;
        Lit l = cla[mid]; Var w = var(l);
        if (v > w) first = mid + 1;
        else if (v < w) last = mid - 1;
        else if( v == w ) return (sign(l) == false) ? 0 : 1;
    }
    return -1;
}

// Calculate graph info
// Assume proof filled
void ProofGraph::getGraphInfo()
{
    //Visit graph from sink keeping track of edges and nodes
    std::deque<ProofNode*> q;
    ProofNode* n;

    av_cla_size=0;
    max_cla_size=0;
    var_cla_size=0;
    num_nodes=0;
    num_edges=0;
    num_unary=0;
    num_leaves=0;
    proof_variables.clear();

    q.push_back(getRoot());
    do
    {
        n=q.front();
        q.pop_front();

        // Node not visited yet
        if (!isSetVisited1(n->getId()))
        {
            if (!n->isLeaf())
            {
                q.push_back(n->getAnt1());
                num_edges++;
                q.push_back(n->getAnt2());
                num_edges++;
                proof_variables.insert(n->getPivot());
            }
            else
            {
                assert(isLeafClauseType(n->getType()));
                num_leaves++;
            }

            //Mark node as visited
            setVisited1(n->getId());
            num_nodes++;
            av_cla_size+=n->getClauseSize();
            if (n->getClauseSize() > (size_t)max_cla_size) max_cla_size=n->getClauseSize();
            if (n->getClauseSize()==1) num_unary++;
        }
    }
    while(!q.empty());

    av_cla_size /= num_nodes;
    // Calculate sample variance for num resolvents and clause size
    for (size_t i=0;i<getGraphSize();i++)
        if (getNode(i)!=NULL)
        {
            var_cla_size+=pow(getNode(i)->getClauseSize()-av_cla_size,2);
        }
    var_cla_size/=(num_nodes-1);
    resetVisited1();
}

//Input: a vector which will contain the topological sorting of nodes
//Output: a topological sorting antecedents-resolvents
void ProofGraph::topolSortingTopDown(std::vector<clauseid_t>& DFS)
{
    std::deque<clauseid_t>q;
    ProofNode* n;
    DFS.clear();
    DFS.reserve(getGraphSize());
    if (getGraphSize() == 1)
    {
        DFS.push_back(graph[0]->getId());
        return;
    }
    clauseid_t id;
    // Enqueue leaves first
    q.assign(leaves_ids.begin(),leaves_ids.end());
    do
    {
        id=q.front(); q.pop_front();
        n=getNode(id); assert(n);
        //Node not visited yet
        if (!isSetVisited1(id))
        {
            if (n->getAnt1()!=NULL && !isSetVisited1(n->getAnt1()->getId())){}
            else if (n->getAnt2()!=NULL && !isSetVisited1(n->getAnt2()->getId())){}
            // Mark node as visited if both antecedents visited
            else
            {
                // Enqueue resolvents
                for (clauseid_t resolvent_id : n->getResolvents()) {
                    if (getNode(resolvent_id)) q.push_back(resolvent_id);
                }
                setVisited1(id);
                DFS.push_back(id);
            }
        }
    }
    while(!q.empty());
    resetVisited1();
}

//Input: a vector which will contain the topological sorting of nodes
//Output: a topological sorting antecedents-resolvents
void ProofGraph::topolSortingBotUp(std::vector<clauseid_t>& DFS)
{
    clauseid_t id,id1,id2;
    std::vector<clauseid_t> q;
    ProofNode * node = nullptr;
    std::vector<unsigned> visited_count(getGraphSize(),0);
    DFS.clear();
    DFS.reserve(getGraphSize());

    // Building DFS vector
    q.push_back(getRoot()->getId());
    do
    {
        id = q.back();
        node = getNode(id);
        assert(node);
        visited_count[id]++;
        q.pop_back();

        // All resolvents have been visited
        if (id == getRoot()->getId() || visited_count[id] == node->getNumResolvents())
        {
            if (!node->isLeaf())
            {
                id1 = node->getAnt1()->getId();
                id2 = node->getAnt2()->getId();
                // Enqueue antecedents
                assert( visited_count[id1] < node->getAnt1()->getNumResolvents() );
                assert( visited_count[id2] < node->getAnt2()->getNumResolvents() );
                q.push_back(id1); q.push_back(id2);
            }
            DFS.push_back(id);
        }
    }
    while(!q.empty());
}

/*
 * Given two clauses A and B, and a pivot variable, computes the resolvent clause after resolution.
 *
 * PRECONDITION: Literals in the input clauses must be sorted and the clauses must contain the pivot variable.
 * POSTCONDITION: Literals in the resolvent clause are sorted and the clause does not contain the pivot.
 */
bool ProofGraph::mergeClauses(std::vector<Lit> const & A, std::vector<Lit> const & B, std::vector<Lit>& resolv, Var pivot)
{
    assert(std::is_sorted(A.begin(), A.end()));
    assert(std::is_sorted(B.begin(), B.end()));
    assert(std::find_if(A.begin(), A.end(), [pivot](Lit l) { return var(l) == pivot; }) != A.end());
    assert(std::find_if(B.begin(), B.end(), [pivot](Lit l) { return var(l) == pivot; }) != B.end());
    const std::size_t Asize = A.size();
    const std::size_t Bsize = B.size();
    if (resolv.size() < Asize + Bsize - 2) {
        resolv.resize(Asize + Bsize - 2);
    }
    assert(resolv.size() >= Asize + Bsize - 2);

    std::size_t i = 0;
    std::size_t j = 0;
    std::size_t res = 0;

    auto addIfNotPivot = [&resolv, &res, pivot](Lit l) {
        if (var(l) != pivot) {
            assert(res == 0 or resolv[res - 1] != l);
            resolv[res++] = l;
        }
    };

    while (i < Asize and j < Bsize) {
        if (A[i] <= B[j]) {
            if (A[i] == B[j]) { ++j; }
            addIfNotPivot(A[i]);
            ++i;
        } else {
            assert(B[j] < A[i]);
            addIfNotPivot(B[j]);
            ++j;
        }
    }

    while (i < Asize) {
        addIfNotPivot(A[i]);
        ++i;
    }

    while (j < Bsize) {
        addIfNotPivot(B[j]);
        ++j;
    }
    assert(resolv.size() >= res);
    resolv.resize(res);
    assert(std::is_sorted(resolv.begin(), resolv.end()));
    assert(std::find_if(resolv.begin(), resolv.end(), [pivot](Lit l) { return var(l) == pivot; }) == resolv.end());
    return true;
}


/////////INTERPOLATION////////////

/************************ OTHER *********************************/

// Output: formula complexity as number of connectives, number of distinct boolean variables
void ProofGraph::getComplexityInterpolant( PTRef int_e )
{
    // Computation embedded in topological visit enode
    assert( int_e != PTRef_Undef );

    std::vector<PTRef>q;
    PTRef e_curr;
    std::set<PTRef> visited;
    std::set<Var> predicates;
    std::map< PTRef, unsigned long > complexity_map;
    unsigned num_connectives_dag = 0;
    unsigned num_and_or_dag = 0;

    q.push_back(int_e);
    do
    {
        e_curr=q.back();
        assert(e_curr != PTRef_Undef );
        //Node not visited yet
        if (visited.find(e_curr) == visited.end())
        {
            bool all_visited = false;
            // Atomic boolean expression
            if (logic_.isAtom(e_curr))
            {
                // Complexity of atom is 0
                complexity_map.insert( std::make_pair( e_curr, 0 ) );
                // Add predicate to set
                if ( e_curr != logic_.getTerm_true() && e_curr != logic_.getTerm_false() ) predicates.insert(PTRefToVar( e_curr ));
                visited.insert(e_curr);
                q.pop_back();
            }
            // Non atomic boolean expression
            else if ( logic_.isBooleanOperator(e_curr))
            {
                assert( logic_.isAnd(e_curr) || logic_.isOr(e_curr) || logic_.isNot(e_curr) || logic_.isEquality(e_curr) );
//                      assert( e_curr->getCar()->isSymb() );
//                      PTRef args = e_curr->getCdr();
//                      assert( args->isList( ) );
                all_visited = true;
                unsigned long comp_curr=0;
                unsigned long num_args=0;
                // Scan arguments of connective
                Pterm& t = logic_.getPterm(e_curr);
                for (int i = 0; i < t.size(); i++)
                {
                    PTRef sub_e = t[i];
                    assert( logic_.isBooleanOperator(sub_e) || logic_.isAtom(sub_e) );
                    if(visited.find(sub_e) == visited.end())
                    {
                        q.push_back(sub_e);
                        all_visited=false;
                    }
                    else
                    {
                        // Calculate complexity
                        comp_curr += complexity_map.find(sub_e)->second;
                        num_args++;
                    }
                }
                if(all_visited)
                {
                    unsigned additional_compl = 0;

                    // Formula tree representation
                    if( logic_.isAnd(e_curr) || logic_.isOr(e_curr) ) additional_compl = num_args - 1;
                    else if( logic_.isNot(e_curr) ) { assert(num_args==1); additional_compl = 1; }
                    complexity_map.insert(std::make_pair(e_curr, comp_curr + additional_compl));

                    // Formula dag representation
                    if( logic_.isAnd(e_curr) || logic_.isOr(e_curr) ) { additional_compl = num_args - 1; num_and_or_dag = num_and_or_dag + additional_compl; }
                    else if( logic_.isNot(e_curr) ) additional_compl = 1;
                    num_connectives_dag = num_connectives_dag + additional_compl;

                    visited.insert(e_curr);
                    q.pop_back();
                }
            }
            else throw OsmtInternalException();
        }
        else
            q.pop_back();
    }
    while(!q.empty());
    unsigned long int_size = complexity_map.find(int_e)->second;
    unsigned pred_num = predicates.size();
    std::cerr << "# Interpolant number of connectives (dag  representation): " << num_connectives_dag << '\n';
    std::cerr << "# Interpolant number of connectives (tree representation): " << int_size << '\n';
    std::cerr << "# Interpolant number of distinct variables: " << pred_num << std::endl;
}


// Input: an interpolant, a boolean
// Output: complexity of interpolant (2 ways depending on the flag)
// Improved iterative implementation using topological visit of enode
unsigned long ProofGraph::getComplexityInterpolantIterative(PTRef int_e, bool flag)
{
	assert( int_e != PTRef_Undef );

	std::vector< PTRef > DFS_enode;
	topolSortingEnode( DFS_enode, int_e );

	std::map< PTRef, unsigned long > complexity_map;
	PTRef curr_enode;

	for( size_t i = 0; i < DFS_enode.size( ) ; i++ )
	{
		curr_enode= DFS_enode[i];
		assert(curr_enode != PTRef_Undef);

		// Case atom
		if( logic_.isAtom(curr_enode) )
		{
			// Complexity of atom is 0
			complexity_map.insert( std::make_pair( curr_enode, 0 ) );
		}
		// Case boolean connective: and, or not, iff, xor, implies
		else if( logic_.isBooleanOperator(curr_enode) )
		{
//			PTRef args = curr_enode->getCdr();
//			assert( args->isList( ) );

			unsigned long comp_curr=0;
			unsigned long num_args=0;
			// Scan arguments of connective
//			for ( PTRef alist = args ; !alist->isEnil( ) ; alist = alist->getCdr( ) )
			Pterm& t = logic_.getPterm(curr_enode);
			for ( int i = 0; i < t.size(); i++ )
			{
				PTRef e = t[i];
				assert( logic_.hasSortBool(e) );
				// Calculate complexity
				comp_curr += complexity_map.find(e)->second;
				num_args++;
			}
			if( flag )
			{
				// Complexity of connective is sum of complexities of arguments plus one
				complexity_map.insert(std::make_pair(curr_enode,comp_curr + 1));
			}
			else
			{
				// Complexity of connective is sum of complexities of arguments plus number of arguments - 1
				// E.g. ennary AND counts as eight binary AND
				complexity_map.insert(std::make_pair(curr_enode,comp_curr + num_args -1));
			}
		}
	}
	return complexity_map.find(int_e)->second;
}

// Input: an interpolant
// Output: the set of predicates contained in the interpolant
// Better iterative version
void ProofGraph::getPredicatesSetFromInterpolantIterative(PTRef int_e, std::set<PTRef> & pred_set) {
	assert( int_e != PTRef_Undef );

	std::vector< PTRef > DFS_enode;
	topolSortingEnode( DFS_enode, int_e );

	std::map<PTRef, std::set<PTRef>> predicate_map;
	PTRef curr_enode;

	for( size_t i = 0 ; i < DFS_enode.size( ) ; i++ )
	{
		curr_enode = DFS_enode[i];
		assert( curr_enode != PTRef_Undef );

		std::set< PTRef > pred_set_curr;

		// Case atom
		if( logic_.isAtom(curr_enode) )
		{
			pred_set_curr.insert(curr_enode);
			predicate_map.insert(std::make_pair(curr_enode,pred_set_curr));
		}
		// Case boolean connective: and, or not, iff, xor, implies
		else if( logic_.isBooleanOperator(curr_enode) )
		{
			// Scan arguments of connective
			Pterm const & t = logic_.getPterm(curr_enode);
			for ( int i = 0; i < t.size(); i++ )
			{
				PTRef e = t[i];
				assert( logic_.hasSortBool(e) );
				// Add predicates
				std::set<PTRef> sub_pred_set = predicate_map.find(e)->second;
				for (PTRef pred : sub_pred_set) {
                    pred_set_curr.insert(pred);
                }
			}
			predicate_map.insert(std::make_pair(curr_enode,pred_set_curr));
		}
	}
	pred_set = predicate_map.find(int_e)->second;
}

// Input: an empty vector, an enode representing a boolean formula
// Output: a topological sorting of the enode subexpressions
void ProofGraph::topolSortingEnode(std::vector<PTRef>& DFS, PTRef int_e)
{
	assert( int_e != PTRef_Undef );
	assert( DFS.empty() );

	std::vector<PTRef>q;
	PTRef e_curr;
	DFS.clear();
	std::set<PTRef> visited;
	bool all_visited;

	q.push_back(int_e);
	do
	{
		e_curr=q.back();
		assert(e_curr != PTRef_Undef);
		//Node not visited yet
		if(visited.find(e_curr) == visited.end())
		{
			all_visited = false;
			// Atomic boolean expression
			if(logic_.isAtom(e_curr))
			{
				all_visited = true;
			}
			// Non atomic boolean expression
			else if( logic_.isBooleanOperator(e_curr) )
			{
//				PTRef args = e_curr->getCdr();
//				assert( args->isList( ) );

				all_visited = true;
				// Scan arguments of connective
//				for ( PTRef alist = args ; !alist->isEnil( ) ; alist = alist->getCdr( ) )
				Pterm& t = logic_.getPterm(e_curr);
				for ( int i = 0; i < t.size(); i++ )
				{
//					PTRef sub_e = alist->getCar( );
					PTRef sub_e = t[i];
					assert( logic_.hasSortBool(sub_e) );

					if(visited.find(sub_e) == visited.end())
					{
						q.push_back(sub_e);
						all_visited=false;
					}
				}
			}
			if(all_visited)
			{
				visited.insert(e_curr);
				q.pop_back();
				DFS.push_back(e_curr);
			}
		}
		else
			q.pop_back();
	}
	while(!q.empty());
}

void ProofGraph::analyzeProofLocality(const ipartitions_t & A_mask)
{
	std::cerr << "# Analyzing proof locality" << '\n';
	unsigned num_A_local = 0, num_B_local = 0, num_AB_common = 0, num_AB_mixed = 0, num_sym_elim = 0;
	//Visit graph from sink keeping track of edges and nodes
	std::deque<ProofNode*> q;
	ProofNode* n;
	std::deque<bool> visit(getGraphSize(),false);

	q.push_back(getRoot());
	do
	{
		n=q.front();
		q.pop_front();

		//Node not visited yet
		if(!visit[n->getId()])
		{
			if(!n->isLeaf())
			{
				q.push_back(n->getAnt1());
				q.push_back(n->getAnt2());

				// Determine if resolution step is local, that is
				// 1) all variables are A or AB
				// 2) all variables are B or AB
				bool clause_has_A_local = false;
				bool clause_has_B_local = false;
				// Determine if resolution step is symbol-eliminating, that is
				// 1) At least an antecedent has local variables
				// 2) The resolvent has only AB variables
				bool resolvent_is_clean = false;
				std::vector<Lit> const & c1 = n->getAnt1()->getClause();
				std::vector<Lit> const & c2 = n->getAnt2()->getClause();
				std::vector<Lit> const & c = n->getClause();

				for(unsigned i = 0; i < c1.size(); i++)
				{
					icolor_t v_class = getVarClass( var(c1[i]), A_mask );
					if( v_class == icolor_t::I_A ){ clause_has_A_local = true; }
					else if( v_class == icolor_t::I_B ){ clause_has_B_local = true; }
					else if( v_class == icolor_t::I_AB ){}
                    else throw OsmtInternalException();
				}
				for(unsigned i = 0; i < c2.size(); i++)
				{
					icolor_t v_class = getVarClass( var(c2[i]), A_mask );
					if( v_class == icolor_t::I_A ){ clause_has_A_local = true; }
					else if( v_class == icolor_t::I_B ){ clause_has_B_local = true; }
					else if( v_class == icolor_t::I_AB ){}
                    else throw OsmtInternalException();
				}
				for(unsigned i = 0; i < c.size(); i++)
				{
					icolor_t v_class = getVarClass( var(c[i]), A_mask );
					if( v_class == icolor_t::I_A ){ clause_has_A_local = true; resolvent_is_clean = false; }
					else if( v_class == icolor_t::I_B ){ clause_has_B_local = true; resolvent_is_clean = false; }
					else if( v_class == icolor_t::I_AB ){}
                    else throw OsmtInternalException();
				}
				if (!clause_has_A_local && !clause_has_B_local)     num_AB_common++;
				else if (clause_has_A_local && !clause_has_B_local) num_A_local++;
				else if (!clause_has_A_local && clause_has_B_local) num_B_local++;
				else if (clause_has_A_local && clause_has_B_local)  num_AB_mixed++;
				if((clause_has_A_local || clause_has_B_local) && resolvent_is_clean) num_sym_elim++;
			}
			else
			{
				// Determine if leaf is symbol-eliminating, that is
				// 1) The leaf has only AB variables
				bool clause_has_A_local = false;
				bool clause_has_B_local = false;
				std::vector<Lit> const & c = n->getClause();

				for(unsigned i = 0; i < c.size(); i++)
				{
					icolor_t v_class = getVarClass( var(c[i]), A_mask );
					if( v_class == icolor_t::I_A ){ clause_has_A_local = true; }
					else if( v_class == icolor_t::I_B ){ clause_has_B_local = true; }
					else if( v_class == icolor_t::I_AB ){}
					else throw OsmtInternalException();
				}
				if (!clause_has_A_local && !clause_has_B_local) num_sym_elim++;
			}
			//Mark node as visited
			visit[n->getId()]=true;
		}
	}
	while(!q.empty());
	std::cerr << "# AB common steps  :" << num_AB_common << '\n';
	std::cerr << "# A local steps    :" << num_A_local << '\n';
	std::cerr << "# B local steps    :" << num_B_local << '\n';
	std::cerr << "# A B mixed steps  :" << num_AB_mixed << '\n';
	std::cerr << "# Sym elim steps   :" << num_sym_elim << '\n';
}
