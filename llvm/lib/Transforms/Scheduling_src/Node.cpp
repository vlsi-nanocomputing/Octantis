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
}

/// Constructor: it create the node, delay can be chosen
Node::Node(const std::string &id, const int &delay)
{
    _id=id;
    _color="WHITE";
    _delay=delay;
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

std::vector <std::string> Node::getChild()
{
    return _children;
}

std::vector <std::string> Node::getFather()
{
    return _fathers;
}

std::string Node::getColor()
{
    return _color;
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

    type=_id;
    type.pop_back(); //remove the last char of the string the contain a number

    it_op_recognized=find(_op_recognized.begin(), _op_recognized.end(), type); //check if it is a recognize operation
    if(it_op_recognized == _op_recognized.end()) //if it is not a recognizes operation, it is assumed that is a register
        type="REG";

    return type;
}

int Node::getDelay()
{
    return _delay;
}

void Node::setTsched(const int &time)
{
    _t_sched=time;
}

int Node::getTsched()
{
    return _t_sched;
}
