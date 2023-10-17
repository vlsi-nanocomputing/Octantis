/*-------------------------------------- The Octantis Project --------------------------------------*/
/// /file
/// PrintConfig: Class useful to print the default configuration file.
///
/*-------------------------------------------- Licence ---------------------------------------------*/
//
// © Andrea Marchesin 2020 (andrea.marchesin@studenti.polito.it) for Politecnico di Torino
//
/*--------------------------------------------------------------------------------------------------*/
#ifndef PRINTCONFIG_H
#define PRINTCONFIG_H

#include<string>

using namespace std;

class PrintConfig
{
public:
    PrintConfig();

    /// Function to print the configuration file
    void print();
private:
    string configFileName="Octantis.txt";
};

#endif // PRINTCONFIG_H
