//
// Created by ysm on 8/11/20.
//

#include <iostream>
#include "../CircuitBuffer.h"

int main()
{
    CircuitBuffer<int> cb(5);

    std::cout << "======== empty test ===========\n";
    if(cb.empty())
        std::cout << "cb empty" << std::endl;

    std::cout << "======== push test ===========\n";
    auto i = 0;
    for(; i<5; ++i)
        cb.push(i);
    std::cout << "push " << i << " elements\n";

    std::cout << "======== pop test ===========\n";
    if(cb.empty())
        std::cout << "cb empty" << std::endl;
    int v = 0;
    cb.pop(v);
    std::cout << "pop " << v << std::endl;
    cb.pop(v);
    std::cout << "pop " << v << std::endl;

    std::cout << "======== overwrite push test ===========\n";
    cb.push(6);
    cb.push(7);

    std::cout << "======== pop overwrited test ===========\n";
    while(!cb.empty()) {
        cb.pop(v);
        std::cout << "pop " << v << std::endl;
    }

    return 0;
}

