#include <Arduino.h>
#include <RTClib.h>

class Time
{
private:
    RTC_Millis rtc;
    u_int8_t previousSecondValue = 0;
    int milliseconds = 0;
    long lastChangeMilliseconds = 0;
    RTT rtt = RTT();
    
public:
    Time(){}

    void init(){
        rtc.begin(DateTime(F(__DATE__), F(__TIME__)));
    }

    RTC_Millis getRtc(){
        return rtc;
    }

    void correct(char *dateTime){
        rtt.println("time correct");
        rtc.adjust(DateTime(dateTime));
        rtt.print(rtc.now().hour());
        rtt.print(":");
        rtt.println(rtc.now().minute());
    }
    void work(){
        DateTime now = rtc.now();
        if(now.second()!= previousSecondValue){
            previousSecondValue = now.second();
            milliseconds = 0;
        }
        if(millis() - lastChangeMilliseconds >= 1){
            lastChangeMilliseconds = millis();
            milliseconds += 1;
        }
    }


    String getIsoTime(){
        rtt.println("get iso time");
        DateTime now = rtc.now();
        char isoTime[13];
        int hour = now.hour();
        int minutes = now.minute();
        int seconds = now.second();
        int millisec = milliseconds;
        isoTime[0] = '0' + hour / 10;
        isoTime[1] = '0' + hour % 10;
        isoTime[2] = ':';
        isoTime[3] = '0' + minutes / 10;
        isoTime[4] = '0' + minutes % 10;
        isoTime[5] = ':';
        isoTime[6] = '0' + seconds / 10;
        isoTime[7] = '0' + seconds % 10;
        isoTime[8] = '.';
        isoTime[9] = '0' + (millisec / 100) % 10;
        isoTime[10] = '0' + (millisec / 10) % 10;
        isoTime[11] = '0' + millisec % 10;
        isoTime[12] = '\0';
        return String(isoTime);
    }

    String getIsoDate(){
        rtt.println("get iso date");
        DateTime now = rtc.now();
        String year = String(now.year());
        String month = now.month() < 10 ? "0" + String(now.month()): String(now.month());
        String day = now.day() < 10 ? "0" + String(now.day()): String(now.day());
        return year + "-" + month + "-" + day; 
    }

    ~Time(){}
};

