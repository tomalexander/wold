#ifndef LUA_DEMO_H_
#define LUA_DEMO_H_
#include <oolua/oolua.h>
#include <string>
#include <iostream>

using std::string;
using std::cout;
class car
{
  public:
    car(string new_state) {state = new_state; cout << "Car Created! " << state << "\n";}
    ~car() {}
    
    void go() {state = "MOVING";}
    void stop() {state = "STOPPED"; cout << "CAR: STOPPING\n";}
    void print() {cout << "CAR: " << state << "\n";}

    string state;
  private:
};

OOLUA_CLASS_NO_BASES(car)
OOLUA_TYPEDEFS No_default_constructor OOLUA_END_TYPES
OOLUA_CONSTRUCTORS_BEGIN
OOLUA_CONSTRUCTOR(std::string)
OOLUA_CONSTRUCTORS_END

OOLUA_MEM_FUNC_0(void, go)
OOLUA_MEM_FUNC_0(void, stop)
OOLUA_MEM_FUNC_0(void, print)

OOLUA_CLASS_END

EXPORT_OOLUA_FUNCTIONS_0_CONST(car)
EXPORT_OOLUA_FUNCTIONS_NON_CONST(car, go, stop, print)
#endif
