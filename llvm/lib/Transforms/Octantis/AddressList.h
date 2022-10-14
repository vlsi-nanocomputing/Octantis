/*-------------------------------------- The Octantis Project --------------------------------------*/
//
// AddressList Class: class useful to store and handle a list of addresses of LiM rows on which an operation must be executed
//
/*-------------------------------------------- Licence ---------------------------------------------*/
//
// © Alessio Naclerio 2021 (alessio.naclerio@studenti.polito.it) for Politecnico di Torino
//
/*--------------------------------------------------------------------------------------------------*/
#ifndef ADDRESSLIST_H
#define ADDRESSLIST_H

#include <map>
#include <iterator>
#include <string>
#include <list>

#include "llvm/IR/Function.h"
#include "AccessPattern.h"
#include "PointerInfoTable.h"

using namespace llvm;

namespace octantis {

/// Class useful to store and handle a list of addresses of LiM rows on which an operation must be executed
class AddressList
{

public:

    ///Default constructor
    AddressList(){};

    AddressList(const std::list<int*> initList):addrList(initList){}

    int getAddrListSize();

    int * operator() (int i);

    void setAddressList(const std::list<int*> lst);

    std::list<int*> getSubset(const int &subsetNumber, const int &nSubsetElements);

    void shapeAddressList(PointerInfoTable::pointerInfoStruct pis);

    void expandAddressList(PointerInfoTable::pointerInfoStruct pis, std::list<int> loopIterations);

    int extractIteratorAdvancement(const AccessPattern::arrayAccessPattern &ap, const int &i, const int &j, const int &k, const int &l, const int & arrayCols, const int & setCols);

    //DEBUG FUNCTIONS
    void printAddressList();

private:

    ///List of addresses
    std::list<int*> addrList;

    ///Iterator of list of addresses
    std::list<int*>::iterator addrListIT;

};

} //End of Octantis' namespace

#endif // ADDRESSLIST_H
