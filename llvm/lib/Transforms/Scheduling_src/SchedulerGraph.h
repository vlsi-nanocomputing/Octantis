#ifndef SCHEDULERGRAPH_H
#define SCHEDULERGRAPH_H

#include "lp_lib.h"
#include "Node.h"

#include <iostream>
#include <cstdlib>
#include <string>
#include <fstream>
#include <sstream>
#include <map>
#include <vector>
#include <list>
#include <new>
#include <algorithm>

class SchedulerGraph
{
    public:
        SchedulerGraph();
        virtual ~SchedulerGraph();

        /// It converts the input file description in a data structure: graph
        void fileToGraph(const std::string &namefile);

        void dfs_father(const std::string &child); //Test dfs algorithm over the graph, maybe in future could be removed....

        /// Generation of the SDC system
        int graphToSDC();
        /// Solve the SDC system in order to obtain the ASAP scheduling
        int asap_sdc();
        /// Solve the SDC system in order to obtain the ASAP scheduling
        /// N.B. Must be generated before the ASAP scheduling
        int alap_sdc();

        /// Resource constraint scheduling (NO loops in the graph)
        int resourceScheduling();


        void debug();

    protected:

    private:
        /// Create a new node if not exist, and add the edge with is father/child
        void _addNode(const std::string &id, const std::string &child, const std::string &father, const int &delay);

        /// It associates the variable name with the column of the SDC system (LP -> Linear Problem)
        void _setVariableNameLP(const std::string &id);
        /// Add constraint to SDC system -> see LPSolve reference
        int _add_single_var_constraint_sdc(const std::string &id, const int &constr_type, const int &val);
        /// Remove last constraint in SDC system
        int _remove_last_constraint_sdc();

        /// Generation of the mobility ordered list using the ALAP and ASAP scheduling information
        void _mobility_id();

        std::map <std::string, Node> _graph;
        std::map <std::string, bool> _roots_flag; //map id - root information
        std::map <std::string, bool> _leaves_flag; //map id - leaf information
        std::list <std::string> _roots_graph; //list of only absolute roots
        std::list <std::string> _leaves_graph; //list of only absolute leaves

        //ASAP information in different data structures:
        //first: t0 -> (id...), t1 ->(id...), ...
        //second: id_0 = t_sched, id_1 = t_sched, ...
        std::map <int, std::list <std::string>> _schedulingASAP;
        std::map <std::string, int> _resultASAP;

        //ALAP information in different data structures:
        //first: t0 -> (id...), t1 ->(id...), ...
        //second: id_0 = t_sched, id_1 = t_sched, ...
        std::map <int, std::list <std::string>> _schedulingALAP;
        std::map <std::string, int> _resultALAP;

        std::map <int, std::list <std::string>> _mobility_list; //List ordered: low mobility on the top, high mobility on the bottom

        // lp_solve variable //
        lprec *lp;
        std::map <std::string, int> _pos_id; //id <-> SDC Position
        int *colno=NULL;
        REAL *row=NULL;
        int Ncol;

        // resource constraint variable //
        std::map <std::string, int> _resConstraint;
        std::map <int, std::map <std::string, int>> _table_res_used; // il primo è Tscheduling, invece il secondo è la tipologia di constraint dove man mano incrementa con l'uso.
        std::map <int, std::list <std::string>> _scheduling;

};

#endif // SCHEDULERGRAPH_H
