#include <iostream>

#include "SchedulerGraph.h"

using namespace std;

int main()
{
    int ret;

    cout << "Inizio..." << endl << endl;
    SchedulerGraph obj;
    obj.fileToGraph("logica.txt");

    //obj.dfs_father("P5");

    obj.debug();

    ret=obj.graphToSDC();
    ret=obj.asap_sdc();
    ret=obj.alap_sdc();

    //obj.resourceScheduling();

    obj.resourceSchedulingMaxArea();

    //obj.dfs();

    cout << endl << "Fine: " << ret << endl;

    return 0;
}
