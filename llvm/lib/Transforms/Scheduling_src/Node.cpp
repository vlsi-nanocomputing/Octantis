#include "Node.h"

Node::Node()
{
    //ctor
}

/// Constructor: it create the node, default delay to 0
Node::Node(const std::string &id)
{
    _id=id;
    _color="WHITE";
    _delay=0;
    _LiM_id="";
}

/// Constructor: it create the node, delay can be chosen
Node::Node(const std::string &id, const int &delay)
{
    _id=id;
    _color="WHITE";
    _delay=delay;
    _LiM_id="";
    _LiM_type="REG";
}

/// Copy Constructor
Node::Node(const std::string &id, const Node &Old)
{
    _id=id;
    _color="WHITE";
    _delay=Old._delay;
    _fathers=Old._fathers;
    _LiM_id="";
    _LiM_type=Old._LiM_type;
}

Node::~Node()
{
    //dtor
}

void Node::setChild(const std::string &child)
{
    _children.push_back(child);
}

void Node::setColor(const std::string &color)
{
    _color=color;
}

void Node::setFather(const std::string &father)
{
    _fathers.push_back(father);
}

void Node::setTsched(const int &time)
{
    _t_sched=time;
}

/// Get the id of the Logic part inserted inside the memory
/// Return 0 if executed successfully, if a logic just exist return 1
int Node::setLiM(const std::string &id_logic)
{
    if (_LiM_id == "")
    {
        _LiM_id=id_logic;
        _LiM_type=popN(id_logic);

        //_LiM_type=id_logic;
        //_LiM_type.pop_back();
        return 0;
    }
    else
        return 1;
}

std::string Node::getId()
{
    return _id;
}

/// It recognize the type of logic gate of the node, or if it is a register
std::string Node::getType()
{
    std::string type;
    std::vector <std::string>::iterator it_op_recognized;

    type=popN(_id);

    //type=_id;
    //type.pop_back(); //remove the last char of the string the contain a number

    it_op_recognized=find(_op_recognized.begin(), _op_recognized.end(), type); //check if it is a recognize operation
    if(it_op_recognized == _op_recognized.end()) //if it is not a recognizes operation, it is assumed that is a register
        type="REG";

    return type;
}

std::string Node::getColor()
{
    return _color;
}

std::vector <std::string> Node::getChild()
{
    return _children;
}

std::vector <std::string> Node::getFather()
{
    return _fathers;
}

int Node::getDelay()
{
    return _delay;
}

int Node::getTsched()
{
    return _t_sched;
}

/// Get the id of the Logic part inserted inside the memory
std::string Node::getLiM()
{
    return _LiM_id;
}

/// Get the type of the Logic part inserted inside the memory
std::string Node::getLiMtype()
{
    return _LiM_type;
}

std::string Node::popN(std::string id)
{
    std::string type=id;

    while(std::isdigit(type.back()))
        type.pop_back();

    return type;
}
