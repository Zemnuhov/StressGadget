#ifndef PEAKS_MODEL
#define PEAKS_MODEL

#include <Arduino.h>


class PeaksModel
{
private:
    String date;
    String beginTime;
    String endTime;
    int maxValue;
    int tonic;
public:
    PeaksModel(String _date, String _beginTime, String _endTime, int _maxValue, int _tonic){
        date = _date;
        beginTime = _beginTime;
        endTime = _endTime;
        maxValue = _maxValue;
        tonic = _tonic; 
    }

    PeaksModel(){}

    void setDate(String value){
        date = value;
    }

    void setBeginTime(String value){
        beginTime = value;
    }

    void setEndTime(String value){
        endTime = value;
    }

    void setMaxValue(float value){
        maxValue = value;
    }

    void setTonic(int value){
        tonic = value;
    }

    String getDate(){
        return date;
    }

    String getBeginTime(){
        return beginTime;
    }

    String getEndTime(){
        return endTime;
    }

    int getMaxValue(){
        return maxValue;
    }

    int getTonic(){
        return tonic;
    }
    
    ~PeaksModel(){}
};

#endif
