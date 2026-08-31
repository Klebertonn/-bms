#include <iostream>
#include "core/fault/fault_manager.h"
int main(){
    FaultManager fm; fm.init();
    std::cout << "before " << fm.hasFault() << " flags=" << fm.getFaults() << "\n";
    fm.addFault(FAULT_CELL_OVERVOLTAGE);
    std::cout << "after " << fm.hasFault() << " flags=" << fm.getFaults() << " hasFlag=" << fm.hasFault(FAULT_CELL_OVERVOLTAGE) << "\n";
    fm.clear();
    std::cout << "after clear " << fm.hasFault() << " flags=" << fm.getFaults() << "\n";
}
