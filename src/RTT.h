#ifndef RTT_H
#define RTT_H

#include <Arduino.h>
#include "SEGGER_RTT.h"

class RTT
{
private:

    bool debug = true;
    char *toCharArray(String str)
    {
        char *buf = new char[str.length() + 1];
        str.toCharArray(buf, str.length() + 1);
        return buf;
    }

public:
    RTT() {}

    void print(char *array){
        if(debug){
            SEGGER_RTT_WriteString(0, array);
        }
    }

    void print(String str)
    {
        if(debug){
            SEGGER_RTT_WriteString(0, toCharArray(str));
        }
    }

    void print(char symbol){
        if(debug){
            SEGGER_RTT_WriteString(0, toCharArray(String(symbol)));
        }
    }

    void print(int value){
        if(debug){
            SEGGER_RTT_WriteString(0, toCharArray(String(value)));
        }
    }

    void print(float value){
        if(debug){
            SEGGER_RTT_WriteString(0, toCharArray(String(value)));
        }
    }


    void println(char *array){
        if(debug){
            SEGGER_RTT_WriteString(0, array);
            SEGGER_RTT_WriteString(0, "\n");
        }
    }

    void println(String str)
    {
        if(debug){
            SEGGER_RTT_WriteString(0, toCharArray(str));
            SEGGER_RTT_WriteString(0, "\n");
        }
    }

    void println(float value)
    {
        if(debug){
            SEGGER_RTT_WriteString(0, toCharArray(String(value)));
            SEGGER_RTT_WriteString(0, "\n");
        }
    }

    void println(char symbol){
        if(debug){
            SEGGER_RTT_WriteString(0, toCharArray(String(symbol)));
            SEGGER_RTT_WriteString(0, "\n");
        }
    }

    void println(int value){
        if(debug){
            SEGGER_RTT_WriteString(0, toCharArray(String(value)));
            SEGGER_RTT_WriteString(0, "\n");
        }
    }

    ~RTT() {}
};
#endif
