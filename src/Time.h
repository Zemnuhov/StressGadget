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

    bool isValidTimeFormat(const char *timeString)
    {
        // Проверка длины строки
        if (strlen(timeString) != 19)
        {
            return false;
        }

        // Проверка разделителей
        if (timeString[4] != '-' || timeString[7] != '-' || timeString[10] != 'T' ||
            timeString[13] != ':' || timeString[16] != ':')
        {
            return false;
        }

        // Проверка года
        for (int i = 0; i < 4; i++)
        {
            if (!isdigit(timeString[i]))
            {
                return false;
            }
        }

        // Проверка месяца
        for (int i = 5; i < 7; i++)
        {
            if (!isdigit(timeString[i]))
            {
                return false;
            }
        }

        // Проверка дня
        for (int i = 8; i < 10; i++)
        {
            if (!isdigit(timeString[i]))
            {
                return false;
            }
        }

        // Проверка часа
        for (int i = 11; i < 13; i++)
        {
            if (!isdigit(timeString[i]))
            {
                return false;
            }
        }

        // Проверка минут
        for (int i = 14; i < 16; i++)
        {
            if (!isdigit(timeString[i]))
            {
                return false;
            }
        }

        // Проверка секунд
        for (int i = 17; i < 19; i++)
        {
            if (!isdigit(timeString[i]))
            {
                return false;
            }
        }

        return true;
    }

public:
    Time() {}

    void init()
    {
        rtc.begin(DateTime(F(__DATE__), F(__TIME__)));
    }

    RTC_Millis getRtc()
    {
        return rtc;
    }

    void correct(char *dateTime)
    {

        if (isValidTimeFormat(dateTime))
        {
            rtc.adjust(DateTime(dateTime));
            rtt.println("time correct");
        }
        else
        {
            rtt.println("time not correct");
        }
        rtt.print(rtc.now().hour());
        rtt.print(":");
        rtt.println(rtc.now().minute());
    }
    void work()
    {
        DateTime now = rtc.now();
        if (now.second() != previousSecondValue)
        {
            previousSecondValue = now.second();
            milliseconds = 0;
        }
        if (millis() - lastChangeMilliseconds >= 1)
        {
            lastChangeMilliseconds = millis();
            milliseconds += 1;
        }
    }

    String getIsoTime()
    {
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

    String getIsoDate()
    {
        rtt.println("get iso date");
        DateTime now = rtc.now();
        String year = String(now.year());
        String month = now.month() < 10 ? "0" + String(now.month()) : String(now.month());
        String day = now.day() < 10 ? "0" + String(now.day()) : String(now.day());
        return year + "-" + month + "-" + day;
    }

    ~Time() {}
};
