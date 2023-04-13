#include "Arduino.h"
#include "Filters.h"
#include "DataBase.h"
#include "RTT.h"

class GsrSignal
{
private:
    u_int8_t pin;
    Filters filters = Filters();
    RTT rtt = RTT();

    float threshold = 3;

    bool peakReadFlag = false;
    bool isPeak = false;
    PeaksModel peaksModel;
    float maxPeak = 0;

    Time *realTime;
    DataBase *db;

    int phase;
    int tonic;

    void (*writePeakCallBack)(PeaksModel peak);

    void detectedPeaks(int phase, int tonic){
        if(phase > threshold){
            if(!isPeak){
                isPeak = true;
                peaksModel.setDate((*realTime).getIsoDate());
                peaksModel.setBeginTime((*realTime).getIsoTime());
            }
            if(maxPeak<phase){
                maxPeak = phase;
            }
        }
        if(phase < threshold && isPeak){
            peaksModel.setEndTime((*realTime).getIsoTime());
            peaksModel.setMaxValue(maxPeak);
            peaksModel.setTonic(tonic);
            maxPeak = 0;
            isPeak = false;
            (*db).addPeak(peaksModel);
            writePeakCallBack(peaksModel);
            rtt.println("detectedPeaks");
        }
    }

public:
    GsrSignal(u_int8_t _pin, Time *time, DataBase *_db)
    {
        pin = _pin;
        realTime = time;
        db = _db;
    }

    void registerWritePeakCallBack(void (*callBack)(PeaksModel peak)){
        writePeakCallBack = callBack;
    }


    void work()
    {
        int mapingValue = map(analogRead(pin), 800, 4095, 0, 10000);
        if (mapingValue < 0){
              phase = 0;
              tonic = 0;
        }
        else{
             phase = filters.getFiltredPhase(mapingValue);
             tonic = filters.getFiltredTonic(mapingValue);
             detectedPeaks(phase, tonic);
        }        
    }

    int getPhaseValue(){
        return phase;
    }

    int getTonicValue(){
        return tonic;
    }

    ~GsrSignal() {}
};
