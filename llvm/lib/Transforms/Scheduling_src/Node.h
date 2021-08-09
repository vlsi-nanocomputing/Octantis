#ifndef NODE_H
#define NODE_H

#include <string>
#include <vector>
#include <algorithm>

class Node
{
    public:
        // Constructor
        Node();
        /// Constructor: it create the node, default delay to 0
        Node(const std::string &id); // default dalay=0, used for logic gate
        /// Constructor: it create the node, delay can be chosen
        Node(const std::string &id, const int &delay);
        /// Copy Constructor
        Node(const std::string &id, const Node &Old);

        virtual ~Node();

        void setChild(const std::string &child);
        void setColor(const std::string &color);
        void setFather(const std::string &father);
        void setTsched(const int &time); //Forse non sarà usato.............................................................
        /// Get the id of the Logic part inserted inside the memory
        /// Return 0 if executed successfully, if a logic just exist return 1
        int setLiM(const std::string &id_logic);

        std::string getId();
        /// It recognize the type of logic gate of the node, or if it is a register
        std::string getType(); //Forse non serve............................................................................
        std::string getColor();
        std::vector <std::string> getChild();
        std::vector <std::string> getFather();
        int getDelay();
        int getTsched(); //Forse non sarà usato.............................................................................
        /// Get the id of the Logic part inserted inside the memory
        std::string getLiM();
        /// Get the type of the Logic part inserted inside the memory
        std::string getLiMtype();

    protected:

    private:
        std::string _id;
        //std::string _type;
        std::string _color;
        std::vector <std::string> _children;
        std::vector <std::string> _fathers;
        int _delay;
        int _t_sched; //Forse non sarà usato................................................................................

        std::string _LiM_id; //id of the logic part of the memory
        std::string _LiM_type; //type of the logic part of the memory

        std::string popN(std::string id);

        std::vector <std::string> _op_recognized {"AND", "OR", "XOR", "NOT", "XNOR", "RSH", "ADD"}; // Aggiungere il resto!!!!!!!!!!!!!!!!!!!!!!!!
};

#endif // NODE_H
