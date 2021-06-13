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

        //void dfs_father(const std::string &child); //Test dfs algorithm over the graph, maybe in future could be removed....
        //void dfs(); //debug

        /// Generation of the SDC system
        int graphToSDC();
        /// Solve the SDC system in order to obtain the ASAP scheduling
        int asap_sdc();
        /// Solve the SDC system in order to obtain the ASAP scheduling
        /// N.B. Must be generated before the ASAP scheduling
        int alap_sdc();

        /// Resource constraint scheduling (NO loops in the graph)
        int resourceScheduling();

        /// Resource constraint scheduling for minimized area (NO loops in the graph)
        int resourceSchedulingMaxArea();


        void debug();

    protected:

    private:
        /// Create a new node if not exist, and add the edge with is father/child
        void _addNode(const std::string &rd, const std::string &op_n, std::string &rs1, const std::string &rs2);
        void _addNode(const std::string &rd, const std::string &op_n, std::string &rs1);

        /// It associates the variable name with the column of the SDC system (LP -> Linear Problem)
        void _setVariableNameLP(const std::string &id);
        /// Add constraint to SDC system -> see LPSolve reference
        int _add_single_var_constraint_sdc(const std::string &id, const int &constr_type, const int &val);
        /// Remove last constraint in SDC system
        int _remove_last_constraint_sdc();

        /// Generation of the mobility ordered list using the ALAP and ASAP scheduling information
        void _mobility_id();

        /// Clear all the data structures used to store the scheduling information
        void _clear();

        /// Connection of all HW nodes
        void _connect_HWedge(std::list <std::string> nodes);

        //void _dfs_visit(std::string id); //debug

        std::map <std::string, Node> _graph;
        std::map <std::string, bool> _roots_flag; //map id - root information
        std::map <std::string, bool> _leaves_flag; //map id - leaf information
        std::list <std::string> _roots_graph; //list of only absolute roots
        std::list <std::string> _leaves_graph; //list of only absolute leaves

        //ASAP information in different data structures:
        //The former: t0 -> (id...), t1 ->(id...), ...
        //The latter: id_0 = t_sched, id_1 = t_sched, ...
        std::map <int, std::list <std::string>> _schedulingASAP;
        std::map <std::string, int> _resultASAP;

        //ALAP information in different data structures:
        //The former: t0 -> (id...), t1 ->(id...), ...
        //The latter: id_0 = t_sched, id_1 = t_sched, ...
        std::map <int, std::list <std::string>> _schedulingALAP;
        std::map <std::string, int> _resultALAP;

        std::map <int, std::list <std::string>> _mobility_list; //List ordered: low mobility on the top, high mobility on the bottom

        // lp_solve variable //
        lprec *lp;
        std::map <std::string, int> _pos_id; //id <-> SDC Position
        int *colno=NULL;
        REAL *row=NULL;
        int Ncol;

        // Resource scheduling variable //
        std::map <std::string, std::map <std::string, int>> _type_usage; //The first is the type of the gate (And, Or, ...), the second is the id of the phisical component (we can have more And gate), the third count the time used
        std::map <std::string, std::map <std::string, std::map <int, std::string>>> _scoreboard; //The first is the type of the gate (And, Or, ...), the second is the id of the phisical component (we can have more And gate), the third is the time scheduling, the latter is the graph node id

        std::map <int, std::list <std::string>> _scheduling; //Scheduling of the _graph (only for debug purpose)

        std::map <int, std::list <std::string>> _HWscheduling; //Scheduling of the output graph (HWgraph)
        std::map <std::string, Node> _HWgraph;

        std::map <std::string, std::string> _node_to_HW; //store the information about which hardware is used by a node (node -> HWop)
        std::map <std::string, std::list<std::string>> _HW_to_node; //Used only for debug

        int _mem_size=10; //memory size constraint
        int _mem_usage;
        int _mux_size=4; //Configuring the #input of the mux, change how many times a HWoperator can be reused

        long int _recursion; //the execution is terminated if _recusion reach the maximum (Backtracking factor)

};

#endif // SCHEDULERGRAPH_H
