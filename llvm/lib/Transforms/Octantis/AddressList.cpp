/*-------------------------------------- The Octantis Project --------------------------------------*/
//
// AddressList Class: class useful to store and handle a list of addresses of LiM rows on which an operation must be executed
//                      
//
/*-------------------------------------------- Licence ---------------------------------------------*/
//
// © Alessio Naclerio 2021 (alessio.naclerio@studenti.polito.it) for Politecnico di Torino
// 
//
/*--------------------------------------------------------------------------------------------------*/
#include "AddressList.h"

//LLVM Include Files
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"

//STD C++ Libraries
#include <string>
#include <algorithm>

using namespace llvm;
using namespace octantis;


int AddressList::extractIteratorAdvancement(const AccessPattern::arrayAccessPattern &ap, const int &i, const int &j, const int &k, const int &l, const int & arrayCols, const int & setCols){

    switch(ap){

        case AccessPattern::rowMajor:{

            return i * arrayCols + j;
            
        }
            break;

        case AccessPattern::colMajor:{
            
            return j * arrayCols + i;
        }
            break;

        case AccessPattern::colMajorSet1RowMajor:{

            return i * arrayCols + j + k;          

        }

            break;

        case AccessPattern::rowMajorSet1ColMajor:{

            return i + (j + k) * arrayCols;

        }
            break;

        case AccessPattern::rowMajorSet2RowMajor:{

            return i * arrayCols + j + arrayCols * k + l;

        }
            break;

        case AccessPattern::rowMajorSet2ColMajor:{

            return i * arrayCols + j + arrayCols * l + k;

        }
            break;

        case AccessPattern::colMajorSet2ColMajor:{

            return j * arrayCols + i + arrayCols * l + k;
            
        }
            break;

        case AccessPattern::colMajorSet2RowMajor:{

            return j * arrayCols + i + arrayCols * k + l;
            
        }
            break;

        default:{
            break;
        }

    }    


}



void AddressList::shapeAddressList(PointerInfoTable::pointerInfoStruct pis){

    //STOP TIENE CONTO INIZIO ULTIMO SET

    std::list<int*> tmpList;

    errs() << "Shaping\n"; printAddressList();

    addrListIT = addrList.begin();

    AccessPattern::arrayAccessPattern ap = pis.pointerAccessPattern.getApmType();

    errs() << ap << "\n";

    if(ap == AccessPattern::rowMajor || ap == AccessPattern::colMajor){

        for(int i = pis.offsetY; i < pis.stopY + 1; i = i + pis.spacingY){

            for(int j = pis.offsetX; j < pis.stopX + 1; j = j + pis.spacingX){

                advance(addrListIT, extractIteratorAdvancement(ap, i, j, 0, 0, pis.arrayCols, 0));
                tmpList.push_back(*addrListIT);
                addrListIT = addrList.begin();

            }
            
        }

    }else if(ap == AccessPattern::colMajorSet1RowMajor || ap == AccessPattern::rowMajorSet1ColMajor){

        for(int i = pis.offsetY; i < pis.stopY + 1; i = i + pis.spacingY){

            for(int j = pis.offsetX; j < pis.arrayRows - pis.setRows + 1; j = j + pis.spacingX){

                for(int k = 0; k < pis.setRows; k = k + pis.spacingInSubsetY){

                    for(int l = 0; l < pis.setCols; l = l + pis.spacingInSubsetX){

                        advance(addrListIT, extractIteratorAdvancement(ap, i, j, k, l, pis.arrayCols, pis.setCols));
                        tmpList.push_back(*addrListIT);
                        addrListIT = addrList.begin();

                    }

                }

            }
            
        }

    }else if(ap == AccessPattern::rowMajorSet2ColMajor || ap == AccessPattern::rowMajorSet2RowMajor ||
             ap == AccessPattern::colMajorSet2RowMajor || ap == AccessPattern::colMajorSet2ColMajor){


            for(int i = pis.offsetY; i < pis.stopY + 1; i = i + pis.spacingY){

                for(int j = pis.offsetX; j < pis.arrayRows - pis.setRows + 1; j = j + pis.spacingX){

                    for(int k = 0; k < pis.setRows; k = k + pis.spacingInSubsetY){

                        for(int l = 0; l < pis.setCols; l = l + pis.spacingInSubsetX){

                            advance(addrListIT, extractIteratorAdvancement(ap, i, j, k, l, pis.arrayCols, pis.setCols));
                            tmpList.push_back(*addrListIT);
                            addrListIT = addrList.begin();

                        }

                    }

                }
                
            }

    }

    for(auto IT = tmpList.begin(); IT != tmpList.end(); IT++){
        errs() << *IT << "  ";
    }  
    errs() << "\n";  

    addrList = tmpList;

    printAddressList();

}



void AddressList::expandAddressList(PointerInfoTable::pointerInfoStruct pis, std::list<int> loopIterations){
    
    int nInitZeroCols = pis.pointerAccessPattern.getInitZeros();
    int nFinalZeroCols = pis.pointerAccessPattern.getFinalZeros();
    int nRepeatEachElement = 1;
    int nRepeatList = 1;

    std::list<int*> tmpList;
    std::list<int*> tmpList1;

    std::list<int>::iterator loopIterationsIT = loopIterations.begin();

    if(nInitZeroCols != 0 || nFinalZeroCols != 0){
        //Counting how many times the list of addresses must be repeated
        for(int i = 0; i < nInitZeroCols; ++i){
            nRepeatList = nRepeatList * *loopIterationsIT;
            advance(loopIterationsIT, 1);
        }
        
        //Start the iterator from the back of loopIterations
        loopIterationsIT = loopIterations.end();
        advance(loopIterationsIT, -1);

        //Counting how many times each element of the list of addresses must be repeated
        for(int i = 0; i < nFinalZeroCols; ++i){        
            nRepeatEachElement = nRepeatEachElement * *loopIterationsIT;
            advance(loopIterationsIT, -1);
        }


        for(addrListIT = addrList.begin(); addrListIT != addrList.end(); ++addrListIT){
            for(int i = 0; i < nRepeatEachElement; ++i){
                tmpList.push_back(*addrListIT);
            }
        }

        for(int i = 0; i < nRepeatList; ++i){
            for(addrListIT = tmpList.begin(); addrListIT != tmpList.end(); ++addrListIT){
                tmpList1.push_back(*addrListIT);
            }
        }


        addrList = tmpList1;
    }

}


void AddressList::setAddressList(const std::list<int*> lst){
    addrList = lst;
}

int AddressList::getAddrListSize(){
    return addrList.size();
}

std::list<int*> AddressList::getSubset(const int &subsetNumber, const int &nSubsetElements){

    int currSubsetNumber = 0;

    std::list<int*> retSubsetList;

    std::list<int*>::iterator addrListIT = addrList.begin();

    bool foundSubset = false;


    for(int j = 0; j < addrList.size() && !foundSubset; j += nSubsetElements){

        if(currSubsetNumber == subsetNumber){

            for(int k = 0; k < nSubsetElements; k++){

                retSubsetList.push_back(*addrListIT);
                advance(addrListIT, 1);
                foundSubset = true;

            }

        }
        
        advance(addrListIT, nSubsetElements);

    }

    return retSubsetList;

}


int * AddressList::operator() (int i){
    addrListIT = addrList.begin();
    advance(addrListIT, i);
    return *addrListIT;
}

//DEBUG FUNCTIONS
void AddressList::printAddressList(){

    errs() << "\t\t\tPrinting Address List\n\t\t\t\t";
    for(addrListIT = addrList.begin(); addrListIT != addrList.end(); ++addrListIT){
        errs() << *addrListIT << "  ";
    }
    errs() << "\n\n";

}