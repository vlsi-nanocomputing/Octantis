#include "SchedulerGraph.h"

SchedulerGraph::SchedulerGraph()
{
    //ctor
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
void SchedulerGraph::_addNode(const std::string &id, const std::string &child, const std::string &father, const int &delay)
{
    std::map <std::string, Node>::iterator it_tmp;

    it_tmp=_graph.find(id); //check if exist the node
    if (it_tmp ==_graph.end()) //not found
        _graph[id]=Node(id, delay); //add a new node

    if (child == "")
    {
        _graph[id].setFather(father);
        _graph[father].setChild(id);

        _roots_flag[id]=false; // id is not a father for sure
        //if don't find the id among the leaves => set as leaf, but it can be easily changed in case of father==""
        //and if it is not changed from the case father=="" then we are sure that it is a absolute leaf
        if(_leaves_flag.find(id) == _leaves_flag.end())
            _leaves_flag[id]=true;
    }
    if (father == "")
    {
        _graph[id].setChild(child);
        _graph[child].setFather(id);

        _leaves_flag[id]=false; // id is not a child for sure
        //if don't find the id among the roots => set as root, but it can be easily changed in case of child==""
        //and if it is not changed from the case child=="" then we are sure that it is a absolute root
        if(_roots_flag.find(id) == _roots_flag.end())
            _roots_flag[id]=true;
    }
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

    //mobility is calculated with the difference of the time scheduling between ALAP and ASAP
    for (it_resultASAP=_resultASAP.begin(); it_resultASAP!=_resultASAP.end(); ++it_resultASAP)
    {
        mob=_resultALAP[it_resultASAP->first]-_resultASAP[it_resultASAP->first];

        _mobility_list[mob].push_back(it_resultASAP->first);
        std::cout << "Mob\t" << it_resultASAP->first << "\t" << mob << std::endl;
    }

    //Debug
    int p=0;
    for (std::map <int, std::list <std::string>>::iterator it=_mobility_list.begin(); it!=_mobility_list.end(); ++it)
    {
        std::cout << p;
        for (std::list <std::string>::iterator it_l=(it->second).begin(); it_l!=(it->second).end(); ++it_l)
        {
            std::cout << " - " << *it_l;
        }
        std::cout << std::endl;
        p++;
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
    while (getline(fin, line))
    {
        is.str(line);
        is >> rd;
        is >> op;
        is >> rs1;

        op_n=op+std::to_string(n); //add a crescent number at the end of the string
        _graph[op_n]=op_n; //add a new node, with a delay equal to 0

        _addNode(rd, "", op_n, 1); //add a new node (rd) if not exist, giving the id and is father
        _addNode(rs1, op_n, "", 1); //add a new node (rs1) if not exist, giving the id and is child

        rs2.clear();
        if (op != "NOT") // in case of not operator the RS2 doesn't exist
        {
            is >> rs2;
            _addNode(rs2, op_n, "", 1);
        }

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


    std::cout << "Debug roots list:" << std::endl;
    for (std::list <std::string>::iterator it=_roots_graph.begin(); it!=_roots_graph.end(); ++it)
        std::cout << *it << std::endl;
    std::cout << "Debug leaves list:" << std::endl;
    for (std::list <std::string>::iterator it=_leaves_graph.begin(); it!=_leaves_graph.end(); ++it)
        std::cout << *it << std::endl;
}

void SchedulerGraph::dfs_father(const std::string &child)
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

}

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
    int p=0;
    for (std::map <int, std::list <std::string>>::iterator it=_schedulingASAP.begin(); it!=_schedulingASAP.end(); ++it)
    {
        std::cout << p;
        for (std::list <std::string>::iterator it_l=(it->second).begin(); it_l!=(it->second).end(); ++it_l)
        {
            std::cout << " - " << *it_l;
        }
        std::cout << std::endl;
        p++;
    }

    std::cout << std::endl;
    return ret;
}

/// Solve the SDC system in order to obtain the ASAP scheduling
/// N.B. Must be generated before the ASAP scheduling
int SchedulerGraph::alap_sdc()
{
    int j;
    int ret=0;

    std::cout << "ALAP execution" << std::endl;

    for (std::list <std::string>::iterator it_ASAP=_leaves_graph.begin(); it_ASAP!=_leaves_graph.end(); ++it_ASAP)
         _add_single_var_constraint_sdc(*it_ASAP, EQ, _resultASAP[*it_ASAP]); //add EQ constraint
    std::cout << std::endl;





    // Set the object direction to minimize //
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
    int p=0;
    for (std::map <int, std::list <std::string>>::iterator it=_schedulingALAP.begin(); it!=_schedulingALAP.end(); ++it)
    {
        std::cout << p;
        for (std::list <std::string>::iterator it_l=(it->second).begin(); it_l!=(it->second).end(); ++it_l)
        {
            std::cout << " - " << *it_l;
        }
        std::cout << std::endl;
        p++;
    }
    std::cout << std::endl;

    for (int i=0; i<(int)_leaves_graph.size(); i++)
    {
        _remove_last_constraint_sdc();
    }

    set_minim(lp);


    //debug//
    //_mobility_id();
    return ret;
}

/// Resource constraint scheduling (NO loops in the graph)
int SchedulerGraph::resourceScheduling()
{
    _resConstraint["REG"]=2;
    int i;
    int flag_scheduled;
    std::map <std::string, int>::iterator it_resConstraint;
    std::string id;
    std::string type;
    int ret=0;

    std::map <int, std::list <std::string>>::iterator it_mobility_list;
    std::list <std::string>::iterator it_node;




    for(i=0; i<(int)_schedulingASAP.size(); i++)
    {
        for(it_resConstraint=_resConstraint.begin(); it_resConstraint!=_resConstraint.end(); ++it_resConstraint)
            _table_res_used[i][it_resConstraint->first]=0;

    }


    _mobility_id();



    for (it_mobility_list=_mobility_list.begin(); it_mobility_list!=_mobility_list.end(); ++it_mobility_list)
    {
        while (!(it_mobility_list->second).empty())
        {
            id=(it_mobility_list->second).front(); //get front element of the list
            type=_graph[id].getType();
            //std::cout << id << "\t" << type << std::endl;
            (it_mobility_list->second).pop_front(); //remove front element of the list

            //std::cout << *it_mobility_list << std::endl;

            it_resConstraint=_resConstraint.find(type);
            if (it_resConstraint != _resConstraint.end()) //check if the node must be constrained
            {
                //std::cout << "prova" << std::endl;
                flag_scheduled=0;
                for(i=0; i<=it_mobility_list->first && !flag_scheduled; i++)
                {
                    //std::cout << i << std::endl;

                    if(_table_res_used[_resultASAP[id]+i][type]<_resConstraint[type]) //check if there is a place for the node
                    {
                        //std::cout << "prova" << std::endl;
                        (_scheduling[_resultASAP[id]+i]).push_back(id);
                        _table_res_used[_resultASAP[id]+i][type]+=1;
                        flag_scheduled=1;
                    }
                }
                if (flag_scheduled==0) //case with no place => new constraint and backtracking with the scheduling
                {
                    std::cout << i << std::endl;

                    std::cout << "errore non c'è' piu' postooooooooooooo"  << std::endl;
                    std::cout << id << "\t" << type << std::endl;

                    _add_single_var_constraint_sdc(id, GE, i);

                    //eventualmente fare una funzione che azzera tutte le strutture dati
                    _schedulingASAP.clear();
                    _resultASAP.clear();
                    _schedulingALAP.clear();
                    _resultALAP.clear();
                    _mobility_list.clear();
                    _table_res_used.clear();
                    _scheduling.clear();

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
                    resourceScheduling();

                    return 1;

                }
            }
            else
            {
                //std::cout << "i: " << _resultALAP[id] << " id: " << id << std::endl;
                (_scheduling[_resultALAP[id]]).push_back(id);
            }


        }
    }


    // debug
    std::cout << std::endl;
    for (std::map <int, std::list <std::string>>::iterator it=_scheduling.begin(); it!=_scheduling.end(); ++it)
    {
        std::cout << it->first;
        for (std::list <std::string>::iterator it_l=(it->second).begin(); it_l!=(it->second).end(); ++it_l)
        {
            std::cout << " - " << *it_l;
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;


    return 0;

}




void SchedulerGraph::debug()
{
    std::map <std::string, Node>::iterator it_graph;

    for(it_graph=_graph.begin(); it_graph!=_graph.end(); ++it_graph)
        std::cout << _graph[it_graph->first].getId() << "\t-> delay=" << _graph[it_graph->first].getDelay() << std::endl;

    std::cout << std::endl << std::endl;
}
