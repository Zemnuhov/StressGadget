#include <Arduino.h>
#include "GyverFilters.h"

class Filters
{
private:
    GKalman testFilter = GKalman(40,20,0.06);
    GFilterRA runTonicFilter = GFilterRA(0.05, 1000); 
    GFilterRA runPhaseFirstFilter = GFilterRA(0.05, 1000); 
    GFilterRA runPhaseSecondFilter = GFilterRA(0.05, 1000); 
    int previousValue = 0;

    int toPhaseValue(int value){
        int result = value - previousValue;
        previousValue = value;
        return result*5;
    }
public:
    Filters(){}
    int getFiltredTonic(int value){
        int _value = testFilter.filtered(value);
        return runTonicFilter.filtered(_value);
    }

    int getFiltredPhase(int value){
        int _value = testFilter.filtered(value);
        _value = runPhaseFirstFilter.filtered(_value);
        _value = toPhaseValue(_value);
        _value = runPhaseSecondFilter.filtered(_value);
        return _value;
    }
};
