#ifndef DATA_BASE_H
#define DATA_BASE_H

#include <Arduino.h>
#include "PeaksModel.h"
#include "RTT.h"


class DataBase
{
private:
    const static int bufferSize = 400;
    PeaksModel peaks[bufferSize];
    RTT rtt = RTT();
    int index = 0;
    bool isWriting = false;
    void printPeaks(PeaksModel peak){
        rtt.println("--------------");
        rtt.println(peak.getDate());
        rtt.println(peak.getBeginTime());
        rtt.println(peak.getEndTime());
        rtt.println(peak.getMaxValue());
        rtt.println(peak.getTonic());
        rtt.println("--------------");
    }
public:
    DataBase(){}
    
    void addPeak(String _date, String _beginTime, String _endTime, int _max, int _tonic){
        if(isWriting){
            rtt.println("add peak");
            if(index == bufferSize){
                index = 0;
            }
            peaks[index].setDate(_date);
            peaks[index].setBeginTime(_beginTime);
            peaks[index].setEndTime(_endTime);
            peaks[index].setMaxValue(_max);
            peaks[index].setTonic(_tonic);
            index++;
        }
    }

    PeaksModel getPeaksByIndex(int index){
        return peaks[index];
    }

    PeaksModel *getPeaks(){
        return peaks;
    }

    int getSize(){
        return index;
    }

    void allSent(){
        index = 0;
    }

    void addPeak(PeaksModel peaksModel){
        if(isWriting){
            rtt.println("add peak");
            if(index == bufferSize){
                index = 0;
            }
            peaks[index] = peaksModel;
            printPeaks(peaks[index]);
            index++;
        } 
    }

    void setIsWriting(bool flag){
        isWriting = flag;
    }

    ~DataBase(){}
};
#endif
