#include "SchedulerGraph.h"

SchedulerGraph::SchedulerGraph()
{
    //ctor
    _mem_usage=0;

    _recursion=_mem_size*6; //Backtracking factor

    std::cout << _recursion << std::endl;
}

SchedulerGraph::~SchedulerGraph()
{
    // clearing memory allocation about SDC
    if (row != nullptr)
        delete[] row;
    if (colno != nullptr)
        delete[] colno;

    if (lp != NULL)
        delete_lp(lp);
}

/// Create a new node if not exist, and add the edge with is father/child
void SchedulerGraph::_addNode(const std::string &rd, const std::string &op_n, std::string &rs1, const std::string &rs2)
{
    std::map <std::string, Node>::iterator it_graph;

    _graph[op_n]=op_n; //add a new node, with a delay equal to 0

    it_graph=_graph.find(rd); //check if exist the node
    if (it_graph ==_graph.end()) //not found
        _graph[rd]=Node(rd, 1); //add a new node with a delay to 1

    it_graph=_graph.find(rs1); //check if exist the node
    if (it_graph ==_graph.end()) //not found
        _graph[rs1]=Node(rs1, 1); //add a new node with a delay to 1

    it_graph=_graph.find(rs2); //check if exist the node
    if (it_graph ==_graph.end()) //not found
        _graph[rs2]=Node(rs2, 1); //add a new node with a delay to 1


    if (_graph[rs2].setLiM(op_n)) //insert if possible the logic in memory; if it cannot, it will try with the other memory
    {
        if (_graph[rs1].setLiM(op_n)) //insert if possible the logic in memory; if it cannot, it will create a new node copying the main information
        {
            _graph[rs1+rs1]=Node(rs1+rs1, _graph[rs1]); //Created and copied the information useful into the new node
            _graph[rs1+rs1].setLiM(op_n); //Insert the Logic inside the memory (LiM)
            rs1=rs1+rs1;
        }
    }

    //edges between rd and op_n
    _graph[rd].setFather(op_n);
    _graph[op_n].setChild(rd);
    _roots_flag[rd]=false; // rd is not an absolute father for sure
    //if don't find the id among the leaves => set as leaf, but it can be easily changed when used as father node
    //and if it is not changed then we are sure that it is a absolute leaf
    if(_leaves_flag.find(rd) == _leaves_flag.end())
        _leaves_flag[rd]=true;

    //edges between rs1/rs2 and op_n
    _graph[rs1].setChild(op_n);
    _graph[rs2].setChild(op_n);
    _graph[op_n].setFather(rs1);
    _graph[op_n].setFather(rs2);
    _leaves_flag[rs1]=false; // rs1 is not an absolute child for sure
    _leaves_flag[rs2]=false; // rs2 is not an absolute child for sure
    //if don't find the id among the roots => set as root, but it can be easily changed when used as child node
    //and if it is not changed then we are sure that it is a absolute root
    if(_roots_flag.find(rs1) == _roots_flag.end())
        _roots_flag[rs1]=true;
    if(_roots_flag.find(rs2) == _roots_flag.end())
        _roots_flag[rs2]=true;
}

/// Create a new node if not exist, and add the edge with is father/child
void SchedulerGraph::_addNode(const std::string &rd, const std::string &op_n, std::string &rs1)
{
    std::map <std::string, Node>::iterator it_graph;

    _graph[op_n]=op_n; //add a new node, with a delay equal to 0

    it_graph=_graph.find(rd); //check if exist the node
    if (it_graph ==_graph.end()) //not found
        _graph[rd]=Node(rd, 1); //add a new node with a delay to 1

    it_graph=_graph.find(rs1); //check if exist the node
    if (it_graph ==_graph.end()) //not found
        _graph[rs1]=Node(rs1, 1); //add a new node with a delay to 1

    if (_graph[rs1].setLiM(op_n)) //insert if possible the logic in memory; if it cannot, it will create a new node copying the main information
    {
        _graph[rs1+rs1]=Node(rs1+rs1, _graph[rs1]); //Created and copied the information useful into the new node
        _graph[rs1+rs1].setLiM(op_n); //Insert the Logic inside the memory (LiM)
        rs1=rs1+rs1;
    }

    //edges between rd and op_n
    _graph[rd].setFather(op_n);
    _graph[op_n].setChild(rd);
    _roots_flag[rd]=false; // rd is not an absolute father for sure
    //if don't find the id among the leaves => set as leaf, but it can be easily changed when used as father node
    //and if it is not changed then we are sure that it is a absolute leaf
    if(_leaves_flag.find(rd) == _leaves_flag.end())
        _leaves_flag[rd]=true;

    //edges between rs1 and op_n
    _graph[rs1].setChild(op_n);
    _graph[op_n].setFather(rs1);
    _leaves_flag[rs1]=false; // rs1 is not an absolute child for sure
    //if don't find the id among the roots => set as root, but it can be easily changed when used as child node
    //and if it is not changed then we are sure that it is a absolute root
    if(_roots_flag.find(rs1) == _roots_flag.end())
        _roots_flag[rs1]=true;
}

/// It associates the variable name with the column of the SDC system (LP -> Linear Problem)
void SchedulerGraph::_setVariableNameLP(const std::string &id)
{
    std::map <std::string, int>::iterator pos_id_it;
    int i;

    pos_id_it=_pos_id.find(id); //check if exist id in variable names
    if (pos_id_it == _pos_id.end()) //not found
    {
        i=1+_pos_id.size();
        _pos_id[id]=i;
        set_col_name(lp, i, (char*)id.c_str());
    }
}

/// Add constraint to SDC system -> see LPSolve reference
int SchedulerGraph::_add_single_var_constraint_sdc(const std::string &id, const int &constr_type, const int &val)
{
    set_add_rowmode(lp, TRUE); // faster model if it is done row by row
    int ret=0;
    int j=0;

    colno[j]=_pos_id[id];
    row[j++]=1;

    if( !add_constraintex(lp, j, row, colno, constr_type, val))
        ret=3;

    //Debug
    std::cout << "T_" << id << "\t" << constr_type << " " << val << std::endl;

    set_add_rowmode(lp, FALSE); // rowmode should be turned off again when done bulding the model //

    return ret;
}

/// Remove last constraint in SDC system
int SchedulerGraph::_remove_last_constraint_sdc()
{
    int ret=0;

    if( !del_constraint(lp, get_Nrows(lp)))
        ret=3;

    return ret;
}

/// Generation of the mobility ordered list using the ALAP and ASAP scheduling information
void SchedulerGraph::_mobility_id()
{
    int mob;
    std::map <std::string, int>::iterator it_resultASAP;

    std::cout << "Debug mobility information:" << std::endl;
    //mobility is calculated with the difference of the time scheduling between ALAP and ASAP
    for (it_resultASAP=_resultASAP.begin(); it_resultASAP!=_resultASAP.end(); ++it_resultASAP)
    {
        mob=_resultALAP[it_resultASAP->first]-_resultASAP[it_resultASAP->first];
        //_result_mobility_list[it_resultASAP->first]=mob;

        _mobility_list[mob].push_back(it_resultASAP->first);
        std::cout << "Mob\t" << it_resultASAP->first << "\t" << mob << std::endl;
    }

    //Debug
    //int p=0;
    std::cout << std::endl;
    for (std::map <int, std::list <std::string>>::iterator it=_mobility_list.begin(); it!=_mobility_list.end(); ++it)
    {
        //std::cout << p;
        std::cout << it->first;
        for (std::list <std::string>::iterator it_l=(it->second).begin(); it_l!=(it->second).end(); ++it_l)
        {
            std::cout << " - " << *it_l;
        }
        std::cout << std::endl;
        //p++;
    }

    std::cout << std::endl;

}

/// It converts the input file description in a data structure: graph
void SchedulerGraph::fileToGraph(const std::string &namefile)
{
    //Variable used to read the file
    int n=1;
    std::string line;
    std::string rd, op, op_n, rs1, rs2;
    std::istringstream is;
    std::ifstream fin;

    //Open input file
    fin.open(namefile.c_str());
    if (fin.is_open()==0)
    {
        std::cerr << "Error, the file was not found" << std::endl;
        exit(EXIT_FAILURE);
    }

    //Read file line by line until the end
    std::cout << "Input:" << std::endl;
    while (getline(fin, line))
    {
        is.str(line);
        is >> rd;
        is >> op;
        is >> rs1;

        op_n=op+std::to_string(n); //add a crescent number at the end of the string

        if (op != "NOT") // in case of not operator the RS2 doesn't exist
        {
            is >> rs2;
            _addNode(rd, op_n, rs1, rs2);
        }
        else
            _addNode(rd, op_n, rs1);

        n++;
        is.clear();

        std::cout << "Debug: " << rd << " " << op_n << " " << rs1 << " " << rs2 << std::endl;

    }
    fin.close();

    // Used to determine the absolute roots and the absolute leaves of the graph
    // _roots_flag and _leaves_flag are setted in _addNode method
    for(std::map <std::string, bool>::iterator it_roots_flag=_roots_flag.begin(), it_leaves_flag=_leaves_flag.begin();
        it_roots_flag!=_roots_flag.end(), it_leaves_flag!=_leaves_flag.end();
        ++it_roots_flag, ++it_leaves_flag)
    {
        if(it_roots_flag->second==1)
            _roots_graph.push_back(it_roots_flag->first);

        if(it_leaves_flag->second==1)
            _leaves_graph.push_back(it_leaves_flag->first);
    }


    // Debug printing
    std::cout << std::endl << std::endl;

    std::cout << "Debug roots:" << std::endl;
    for (std::map <std::string, bool>::iterator it=_roots_flag.begin(); it!=_roots_flag.end(); ++it)
        std::cout << it->first << ": " << it->second << std::endl;
    std::cout << "Debug leaves:" << std::endl;
    for (std::map <std::string, bool>::iterator it=_leaves_flag.begin(); it!=_leaves_flag.end(); ++it)
        std::cout << it->first << ": " << it->second << std::endl;


    std::cout << "Debug absolute roots:" << std::endl;
    for (std::list <std::string>::iterator it=_roots_graph.begin(); it!=_roots_graph.end(); ++it)
        std::cout << *it << std::endl;
    std::cout << "Debug absolute leaves:" << std::endl;
    for (std::list <std::string>::iterator it=_leaves_graph.begin(); it!=_leaves_graph.end(); ++it)
        std::cout << *it << std::endl;
    std::cout << std::endl;
}

//Used for debug of the graph
/*void SchedulerGraph::dfs_father(const std::string &child)
{
    std::vector <std::string> fathers;
    std::vector <std::string>::iterator it_father;

    _graph[child].setColor("GRAY");
    std::cout << child  << std::endl;

    fathers=_graph[child].getFather();

    for(it_father=fathers.begin(); it_father!=fathers.end(); ++it_father)
    {
        if(_graph[*it_father].getColor() == "WHITE")
            dfs_father(*it_father);
    }

    _graph[child].setColor("BLACK");

}*/

/// Generation of the SDC system -> see LPsolve reference
int SchedulerGraph::graphToSDC()
{
    int i;
    std::map <std::string, Node>::iterator it_graph;
    std::vector <std::string> child_tmp;
    std::vector <std::string>::iterator it_child;


    int ret=0;

    int j;

    // Building the model row by row, so we create a model with 0 rows and Ncol columns //
    Ncol=_graph.size(); // number of variable in the model
    std::cout << "Number of variable in lp solve model: " << Ncol << std::endl;

    lp=make_lp(0, Ncol);
    if(lp == NULL)
    {
        ret=1;
        std::cout << "Error: couldn't construct a new model..." << std::endl;
    }

    if(ret==0)
    {
        // Create space large enough for one row //
        colno = new (std::nothrow) int [Ncol * sizeof(*colno)];
        row = new (std::nothrow) REAL [Ncol * sizeof(*row)]; //da verificare se va sempre bene quando si aggiungono ulteriori constraint

        if(colno == nullptr || row == nullptr)
        {
            ret=2;
            std::cout << "Error: couldn't allocate memory..." << std::endl;
        }
    }

    set_add_rowmode(lp, TRUE); // faster model if it is done rows by row

    for(it_graph=_graph.begin(), i=1; it_graph!=_graph.end() && ret==0; ++it_graph, i++)
    {
        // Naming variables. Not required, but can be useful for debugging //
        _setVariableNameLP(it_graph->first);

        // Equation wanted
        // Tchild - Tid <= Delay_child

        child_tmp=_graph[it_graph->first].getChild();
        for(it_child=child_tmp.begin(); it_child!=child_tmp.end(); ++it_child)
        {
            // Naming variables. Not required, but can be useful for debugging //
            _setVariableNameLP(*it_child);

            j=0;

            colno[j]=_pos_id[it_graph->first];
            row[j++]=-1;

            colno[j]=_pos_id[*it_child];
            row[j++]=1;

            if( !add_constraintex(lp, j, row, colno, GE, _graph[*it_child].getDelay()))
                ret=3;

            std::cout << "T_" << *it_child << "\t-T_" << it_graph->first << "\t>= " << _graph[*it_child].getDelay() << std::endl;
        }
    }

    std::cout << std::endl;
    set_add_rowmode(lp, FALSE); // rowmode should be turned off again when done bulding the model //

    if(ret==0)
    {
        // Set the objective in lpsolve //
        j=0;
        for (int i=1; i<=Ncol; i++)
        {
            colno[j]=i;
            row[j++]=1;
        }
        if(!set_obj_fnex(lp, j, row, colno))
            ret=4;
    }

    return ret;
}

/// Solve the SDC system in order to obtain the ASAP scheduling
int SchedulerGraph::asap_sdc()
{
    int j;
    int ret=0;

    std::cout << "ASAP execution" << std::endl;


    // Set the object direction to minimize //
    set_minim(lp);
    write_lp(lp, (char *)"modelASAP.lp"); // Generation of a file

    set_verbose(lp, IMPORTANT); // To see important messages on screen while solving

    ret=solve(lp);
    if(ret==OPTIMAL)
        ret=0;
    else
        ret=5;

    std::cout << "ret=" << ret << std::endl;

    if (ret==0)
    {
        std::cout << "Objective value: " << get_objective(lp) << std::endl;
        //int Tlast=0;

        get_variables(lp, row);

        for(j=0; j<Ncol; j++)
        {
            (_schedulingASAP[row[j]]).push_back(get_col_name(lp, j+1));
            _resultASAP[get_col_name(lp, j+1)]=row[j];

            std::cout << get_col_name(lp, j+1) << ":\t" << row[j] << std::endl;
        }
    }

    //debug
    std::cout << std::endl;
    //int p=0;
    for (std::map <int, std::list <std::string>>::iterator it=_schedulingASAP.begin(); it!=_schedulingASAP.end(); ++it)
    {
        //std::cout << p;
        std::cout << it->first;
        for (std::list <std::string>::iterator it_l=(it->second).begin(); it_l!=(it->second).end(); ++it_l)
        {
            std::cout << " - " << *it_l;
        }
        std::cout << std::endl;
        //p++;
    }

    std::cout << std::endl;
    return ret;
}

/// Solve the SDC system in order to obtain the ASAP scheduling
/// N.B. Must be generated the ASAP scheduling before
int SchedulerGraph::alap_sdc()
{
    int j;
    int ret=0;
    //int Tlast=0;

    std::cout << "ALAP execution" << std::endl;

    //Tlast=(_schedulingASAP.rbegin())->first;

    for (std::list <std::string>::iterator it_ASAP=_leaves_graph.begin(); it_ASAP!=_leaves_graph.end(); ++it_ASAP)
        _add_single_var_constraint_sdc(*it_ASAP, EQ, _resultASAP[*it_ASAP]); //add EQ constraint

    std::cout << std::endl;

    // Set the object direction to maximize //
    set_maxim(lp);
    write_lp(lp, (char *)"modelALAP.lp"); // Generation of a file

    set_verbose(lp, IMPORTANT); // To see important messages on screen while solving

    ret=solve(lp);
    if(ret==OPTIMAL)
        ret=0;
    else
        ret=5;

    std::cout << "ret=" << ret << std::endl;


    if (ret==0)
    {
        std::cout << "Objective value: " << get_objective(lp) << std::endl;

        get_variables(lp, row);


        for(j=0; j<Ncol; j++)
        {

            (_schedulingALAP[row[j]]).push_back(get_col_name(lp, j+1));
             _resultALAP[get_col_name(lp, j+1)]=row[j];

            std::cout << get_col_name(lp, j+1) << ":\t" << row[j] << std::endl;
        }
    }

    // debug
    std::cout << std::endl;
    //int p=0;
    for (std::map <int, std::list <std::string>>::iterator it=_schedulingALAP.begin(); it!=_schedulingALAP.end(); ++it)
    {
        //std::cout << p;
        std::cout << it->first;
        for (std::list <std::string>::iterator it_l=(it->second).begin(); it_l!=(it->second).end(); ++it_l)
        {
            std::cout << " - " << *it_l;
        }
        std::cout << std::endl;
        //p++;
    }
    std::cout << std::endl;

    for (int i=0; i<(int)_leaves_graph.size(); i++)
    {
        _remove_last_constraint_sdc();
    }

    set_minim(lp);

    return ret;
}

/// Resource constraint scheduling (NO loops in the graph)
int SchedulerGraph::resourceScheduling()
{
    std::string id;
    std::string HWid;
    std::map <int, std::list <std::string>>::iterator it_mobility_list;
    std::vector <std::string> id_child;
    //std::string LiMid;
    std::string LiMtype;
    int Tstart, Tend;

    int ret;
    bool t_available=false;
    std::map <std::string, int>::iterator it_HW_usage;
    std::map <int, std::string>::iterator it_scoreboard;

    _mobility_id(); //Compute the mobility from ASAP and ALAP information

    std::cout << "Tstart and Tend information:" << std::endl;
    for (it_mobility_list=_mobility_list.begin(); it_mobility_list!=_mobility_list.end(); ++it_mobility_list)
    {
        while (!(it_mobility_list->second).empty())
        {
            id=(it_mobility_list->second).front(); //get front element of the list
            (it_mobility_list->second).pop_front(); //remove front element of the list

            if(_graph[id].getType() == "REG") //The Node different from the REG type are not considered
            {

                Tstart=_resultALAP[id]; //Using the ALAP case: the variable's time life is reduced
                Tend=Tstart; //initialization of Tend
                id_child=_graph[id].getChild();
                for (std::vector <std::string>::iterator it_id_child=id_child.begin(); it_id_child!=id_child.end(); ++it_id_child)
                {
                    if(_resultALAP[*it_id_child] > Tend) //find the latest time that must be used checking when the child start in the worst case that is the ALAP
                        Tend=_resultALAP[*it_id_child];
                }

                LiMtype=_graph[id].getLiMtype(); //A row could be with logic (LiM) or a simple memory row without logic

                std::cout << "Id: " << id << "(" << LiMtype << ")\t- Tstart: " << Tstart << " Tend: " << Tend << std::endl;

                if(_mem_usage<_mem_size) //If there is space, it allocates a new row
                {
                    HWid="#"+std::to_string(_mem_usage); //Create a new HW id

                    _type_usage[LiMtype][HWid]=1;
                    for(int t=Tstart; t<=Tend; t++)
                        _scoreboard[LiMtype][HWid][t]=id;
                    _scheduling[Tstart].push_back(id); //debug
                    _HWscheduling[Tstart].push_back(HWid);
                    _node_to_HW[id]=HWid;
                    _node_to_HW[_graph[id].getLiM()]=LiMtype+HWid;
                    _HW_to_node[HWid].push_back(id); //debug

                    _HWgraph[HWid]=Node(HWid, 1); //Create a new HW node
                    if(LiMtype != "REG")
                        _graph[HWid].setLiM(LiMtype+HWid);

                    _mem_usage++;
                }
                else
                {
                    t_available=false; //flag to understand if an operator is not free in a desidered time
                    for(it_HW_usage=_type_usage[LiMtype].begin(); it_HW_usage!=_type_usage[LiMtype].end() && t_available==false; ++it_HW_usage)
                    {
                        HWid=(it_HW_usage->first);
                        std::cout << "find: " << LiMtype << std::endl;

                        t_available=true;
                        for(int t=Tstart; t<=Tend && t_available; t++)
                        {
                             it_scoreboard=_scoreboard[LiMtype][HWid].find(t);
                             if(it_scoreboard != _scoreboard[LiMtype][HWid].end()) //if found => operator just used in t
                                t_available=false;
                        }
                    }

                    if(t_available) //HW operator is free and it can be reused
                    {
                        for(int t=Tstart; t<=Tend; t++)
                            _scoreboard[LiMtype][HWid][t]=id;
                        _scheduling[Tstart].push_back(id);
                        _HWscheduling[Tstart].push_back(HWid);
                        _node_to_HW[id]=HWid;
                        _node_to_HW[_graph[id].getLiM()]=LiMtype+HWid;
                        _HW_to_node[HWid].push_back(id); //debug

                        _type_usage[LiMtype][HWid]+=1; //count many times is used an HW operator

                        if(_type_usage[LiMtype][HWid]==_mux_size) //if the operator use reach the size of the mux, it will be erased from the map, in this way is not more available
                            _type_usage[LiMtype].erase(HWid);
                    }
                    if(t_available==false && _recursion>0) //erase all, add a SDC constraint, recompute all
                    {
                        std::cout << "Errore numero di registri" << std::endl;


                        _add_single_var_constraint_sdc(id, GE, Tstart+1);
                        //_add_single_var_constraint_sdc(_scheduling[Tstart].front(), GE, Tstart+1);

                        //Clear all the data structures for a new computation
                        _clear();

                        ret=asap_sdc();
                        if(ret!=0)
                        {
                            std::cout << "Errore scd asap, close execution" << std::endl;
                            exit (EXIT_FAILURE);
                        }
                        ret=alap_sdc();
                        if(ret!=0)
                        {
                            std::cout << "Errore scd alap, close execution" << std::endl;
                            exit (EXIT_FAILURE);
                        }

                        _recursion--;
                        resourceScheduling();

                        return 1;

                    }
                }
            }
        }
    }

    _connect_HWedge(_roots_graph);

    std::cout << std::endl << "Scheduling of the node:" << std::endl;
    for (std::map <int, std::list <std::string>>::iterator it=_scheduling.begin(); it!=_scheduling.end(); ++it)
    {
        std::cout << it->first;
        for (std::list <std::string>::iterator it_l=(it->second).begin(); it_l!=(it->second).end(); ++it_l)
        {
            std::cout << " - " << *it_l << "(" << _graph[*it_l].getLiMtype() << ")";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;

    std::cout << "Scheduling of the HW node:" << std::endl;
    for (std::map <int, std::list <std::string>>::iterator it=_HWscheduling.begin(); it!=_HWscheduling.end(); ++it)
    {
        std::cout << it->first;
        for (std::list <std::string>::iterator it_l=(it->second).begin(); it_l!=(it->second).end(); ++it_l)
        {
            std::cout << " - " << *it_l;// << "(" << _graph[*it_l].getLiMtype() << ")";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;

    std::cout << "Legend of HW operator:" << std::endl;
    for (std::map <std::string, std::list<std::string>>::iterator it=_HW_to_node.begin(); it!=_HW_to_node.end(); ++it)
    {
        std::cout << it->first << "(" << _graph[*(it->second).begin()].getLiMtype() << ")";
        for (std::list <std::string>::iterator it_l=(it->second).begin(); it_l!=(it->second).end(); ++it_l)
            std::cout << " - " << *it_l;

        std::cout << std::endl;
    }
    std::cout << std::endl;



    //for (std::map <std::string, std::list<std::string>>::iterator it=_HW_to_node.begin(); it!=_HW_to_node.end(); ++it)
    //    std::cout << it->first << "(" << _graph[it->first].getLiMtype() << ")" << " - " << it->second << std::endl;
    //for (std::map <std::string, std::string>::iterator it=_node_to_HW.begin(); it!=_node_to_HW.end(); ++it)
    //    std::cout << it->first << " - " << it->second << "(" << _graph[it->first].getLiMtype() << ")" << std::endl;
    //std::cout << std::endl;

    if(_recursion==0)
        std::cout << "Impossible to solve the scheduling, few operators available" << std::endl;

    return 0;
}

/// Resource constraint scheduling (NO loops in the graph)
int SchedulerGraph::resourceSchedulingMaxArea()
{
    std::string id;
    std::string HWid;
    std::map <int, std::list <std::string>>::iterator it_mobility_list;
    std::vector <std::string> id_child;
    //std::string LiMid;
    std::string LiMtype;
    int Tstart, Tend;

    int ret;
    bool t_available=true;
    std::map <std::string, std::map <std::string, int>>::iterator it_type_usage;
    std::map <std::string, int>::iterator it_HW_usage;
    std::map <int, std::string>::iterator it_scoreboard;

    _mobility_id(); //Compute the mobility from ASAP and ALAP information

    std::cout << "Tstart and Tend information:" << std::endl;
    for (it_mobility_list=_mobility_list.begin(); it_mobility_list!=_mobility_list.end(); ++it_mobility_list)
    {
        while (!(it_mobility_list->second).empty())
        {
            id=(it_mobility_list->second).front(); //get front element of the list
            (it_mobility_list->second).pop_front(); //remove front element of the list

            if(_graph[id].getType() == "REG") //The Node different from the REG type are not considered
            {

                Tstart=_resultALAP[id]; //Using the ALAP case: the variable's time life is reduced
                Tend=Tstart; //initialization of Tend
                id_child=_graph[id].getChild();
                for (std::vector <std::string>::iterator it_id_child=id_child.begin(); it_id_child!=id_child.end(); ++it_id_child)
                {
                    if(_resultALAP[*it_id_child] > Tend) //find the latest time that must be used checking when the child start in the worst case that is the ALAP
                        Tend=_resultALAP[*it_id_child];
                }

                LiMtype=_graph[id].getLiMtype(); //A row could be with logic (LiM) or a simple memory row without logic

                std::cout << "Id: " << id << "(" << LiMtype << ")\t- Tstart: " << Tstart << " Tend: " << Tend << std::endl;


                it_type_usage=_type_usage.find(LiMtype);
                if(it_type_usage != _type_usage.end()) //type desidered found
                {
                    t_available=false; //flag to understand if an operator is not free in a desidered time
                    for(it_HW_usage=_type_usage[LiMtype].begin(); it_HW_usage!=_type_usage[LiMtype].end() && t_available==false; ++it_HW_usage)
                    {
                        HWid=(it_HW_usage->first);
                        std::cout << "find: " << LiMtype << std::endl;

                        t_available=true;
                        for(int t=Tstart; t<=Tend && t_available; t++)
                        {
                             it_scoreboard=_scoreboard[LiMtype][HWid].find(t);
                             if(it_scoreboard != _scoreboard[LiMtype][HWid].end()) //if found => operator just used in t
                                t_available=false;
                        }
                    }
                    if(t_available) //HW operator is free and it can be reused
                    {
                        std::cout << "HWused: " << HWid << std::endl;
                        for(int t=Tstart; t<=Tend; t++)
                            _scoreboard[LiMtype][HWid][t]=id;
                        _scheduling[Tstart].push_back(id);
                        _HWscheduling[Tstart].push_back(HWid);
                        _node_to_HW[id]=HWid;
                        _node_to_HW[_graph[id].getLiM()]=LiMtype+HWid;
                        _HW_to_node[HWid].push_back(id); //debug

                        _type_usage[LiMtype][HWid]+=1; //count many times is used an HW operator

                        if(_type_usage[LiMtype][HWid]==_mux_size) //if the operator use reach the size of the mux, it will be erased from the map, in this way is not more available
                        {
                            std::cout << "erased HWid: " << HWid << std::endl;
                            _type_usage[LiMtype].erase(HWid);
                        }
                    }
                }
                if((it_type_usage == _type_usage.end()) || t_available==false) //type desidered not found
                {
                    if(_mem_size==0 || _mem_usage<_mem_size) //If there is space, it allocates a new row or if doensn't matter the size of the memory
                    {
                        HWid="#"+std::to_string(_mem_usage); //Create a new HW id

                        _type_usage[LiMtype][HWid]=1;
                        for(int t=Tstart; t<=Tend; t++)
                            _scoreboard[LiMtype][HWid][t]=id;
                        _scheduling[Tstart].push_back(id); //debug
                        _HWscheduling[Tstart].push_back(HWid);
                        _node_to_HW[id]=HWid;
                        _node_to_HW[_graph[id].getLiM()]=LiMtype+HWid;
                        _HW_to_node[HWid].push_back(id); //debug

                        _HWgraph[HWid]=Node(HWid, 1); //Create a new HW node
                        if(LiMtype != "REG")
                            _graph[HWid].setLiM(LiMtype+HWid);

                        _mem_usage++;
                        //std::cout << "mem_usage: " << _mem_usage << std::endl;
                    }
                    else if(_recursion>0)//(_mem_size!=0 && _mem_usage>_mem_size && _recursion>0) //erase all, add a SDC constraint, recompute all
                    {
                        std::cout << "Errore numero di registri" << std::endl;
                        _add_single_var_constraint_sdc(id, GE, Tstart+1);

                        //Clear all the data structures for a new computation
                        _clear();

                        ret=asap_sdc();
                        if(ret!=0)
                        {
                            std::cout << "Errore scd asap, close execution" << std::endl;
                            exit (EXIT_FAILURE);
                        }
                        ret=alap_sdc();
                        if(ret!=0)
                        {
                            std::cout << "Errore scd alap, close execution" << std::endl;
                            exit (EXIT_FAILURE);
                        }

                        _recursion--;
                        resourceSchedulingMaxArea();

                        return 1;
                    }
                }
            }
        }
    }

    _connect_HWedge(_roots_graph);

    std::cout << std::endl << "Scheduling of the node:" << std::endl;
    for (std::map <int, std::list <std::string>>::iterator it=_scheduling.begin(); it!=_scheduling.end(); ++it)
    {
        std::cout << it->first;
        for (std::list <std::string>::iterator it_l=(it->second).begin(); it_l!=(it->second).end(); ++it_l)
        {
            std::cout << " - " << *it_l << "(" << _graph[*it_l].getLiMtype() << ")";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;

    std::cout << "Scheduling of the HW node:" << std::endl;
    for (std::map <int, std::list <std::string>>::iterator it=_HWscheduling.begin(); it!=_HWscheduling.end(); ++it)
    {
        std::cout << it->first;
        for (std::list <std::string>::iterator it_l=(it->second).begin(); it_l!=(it->second).end(); ++it_l)
        {
            std::cout << " - " << *it_l;// << "(" << _graph[*it_l].getLiMtype() << ")";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;

    std::cout << "Legend of HW operator:" << std::endl;
    for (std::map <std::string, std::list<std::string>>::iterator it=_HW_to_node.begin(); it!=_HW_to_node.end(); ++it)
    {
        std::cout << it->first << "(" << _graph[*(it->second).begin()].getLiMtype() << ")";
        for (std::list <std::string>::iterator it_l=(it->second).begin(); it_l!=(it->second).end(); ++it_l)
            std::cout << " - " << *it_l;

        std::cout << std::endl;
    }
    std::cout << std::endl;


    //for (std::map <std::string, std::string>::iterator it=_node_to_HW.begin(); it!=_node_to_HW.end(); ++it)
    //    std::cout << it->first << " - " << it->second << "(" << _graph[it->first].getLiMtype() << ")" << std::endl;
    //std::cout << std::endl;

    if(_mem_size!=0 && _recursion==0)
        std::cout << "Impossible to solve the scheduling, few operators available" << std::endl;

    return 0;
}

/// Clear all the data structures used to store the scheduling information
void SchedulerGraph::_clear()
{
    _schedulingASAP.clear();
    _resultASAP.clear();
    _schedulingALAP.clear();
    _resultALAP.clear();
    _mobility_list.clear();
    _type_usage.clear();
    _scoreboard.clear();
    _scheduling.clear();
    _HWscheduling.clear();
    _HWgraph.clear();
    _node_to_HW.clear();
    _mem_usage=0;
}

/// Connection of all HW nodes
void SchedulerGraph::_connect_HWedge(std::list <std::string> nodes)
{
    std::list <std::string> Q;
    std::string id;
    std::string HWid;
    std::string HWchild;
    std::vector <std::string> children;
    std::vector <std::string> HWchildren;
    std::vector <std::string>::iterator it_HWchildren;
    std::vector <std::string>::iterator it_children;
    std::map <std::string, Node>::iterator it_graph;
    std::map <std::string, Node>::iterator it_HWgraph;

    for(it_graph=_graph.begin(); it_graph!=_graph.end(); ++it_graph)
    {
        children=_graph[it_graph->first].getChild();
        for(it_children=children.begin(); it_children!=children.end(); ++it_children)
        {

            HWid=_node_to_HW[it_graph->first];
            HWchild=_node_to_HW[*it_children];

            HWchildren=_HWgraph[HWid].getChild();
            it_HWchildren=find(HWchildren.begin(), HWchildren.end(), HWchild);
            if(it_HWchildren==HWchildren.end())
            {
                _HWgraph[HWid].setChild(HWchild);
                _HWgraph[HWchild].setFather(HWid);
            }
        }
    }

    //BFS
    /*Q=nodes;

    while(!Q.empty())
    {
        id=Q.front();
        Q.pop_front();

        children=_graph[id].getChild();
        for(it_children=children.begin(); it_children!=children.end(); ++it_children)
        {
            if(_graph[*it_children].getColor() == "WHITE")
            {
                _graph[*it_children].setColor("GREY");

                HWid=_node_to_HW[id];
                HWchild=_node_to_HW[*it_children];

                _HWgraph[HWid].setChild(HWchild);
                _HWgraph[HWchild].setFather(HWid);

                Q.push_back(*it_children);
            }
        }
        _graph[id].setColor("BLACK");
    }
*/
    std::cout << std::endl;
    for(it_HWgraph=_HWgraph.begin(); it_HWgraph!=_HWgraph.end(); ++it_HWgraph)
    {
        children=_HWgraph[it_HWgraph->first].getChild();
        for(it_children=children.begin(); it_children!=children.end(); ++it_children)
            std::cout << it_HWgraph->first << " -> " << *it_children << std::endl;
    }

}

/*void SchedulerGraph::dfs()
{
    std::map <std::string, Node>::iterator it_HWgraph;

    for(it_HWgraph=_HWgraph.begin(); it_HWgraph!=_HWgraph.end(); ++it_HWgraph)
        _HWgraph[it_HWgraph->first].setColor("WHITE");

    for(it_HWgraph=_HWgraph.begin(); it_HWgraph!=_HWgraph.end(); ++it_HWgraph)
    {
        if (_HWgraph[it_HWgraph->first].getColor() == "WHITE")
            _dfs_visit(it_HWgraph->first);
    }
}

void SchedulerGraph::_dfs_visit(std::string id)
{
    std::vector <std::string>::iterator it_children;
    std::vector <std::string> children;

    _HWgraph[id].setColor("GREY");
    children=_graph[id].getChild();
    for(it_children=children.begin(); it_children!=children.end(); ++it_children)
    {
        if(_graph[*it_children].getColor() == "WHITE")
        {
            std::cout << id  << "->" << *it_children << std::endl;
            _dfs_visit(*it_children);
        }
    }
    _HWgraph[id].setColor("BLACK");
}*/

void SchedulerGraph::debug()
{
    std::map <std::string, Node>::iterator it_graph;

    std::cout << "Debug delay information:" << std::endl;
    for(it_graph=_graph.begin(); it_graph!=_graph.end(); ++it_graph)
        std::cout << _graph[it_graph->first].getId() << "\t-> delay=" << _graph[it_graph->first].getDelay() << std::endl;

    std::cout << std::endl << std::endl;
}
