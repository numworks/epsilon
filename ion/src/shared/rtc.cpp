#include <ion/rtc.h>

namespace Ion {
namespace RTC {

static bool consumeDigit(char text, int & target)
{
    if (text < '0' || text > '9') {
        return false;
    }
    target = target * 10 + (text - '0');
    return true;
}

bool parseDate(const char * text, DateTime & target)
{
    target.tm_mday = 0;
    target.tm_mon = 0;
    target.tm_year = 0;

    if (!consumeDigit(*text++, target.tm_mday)) return false;
    if (*text != '/') {
        if (!consumeDigit(*text++, target.tm_mday)) return false;
    }
    if (*text++ != '/') return false;
    if (!consumeDigit(*text++, target.tm_mon)) return false;
    if (*text != '/') {
        if (!consumeDigit(*text++, target.tm_mon)) return false;
    }
    if (*text++ != '/') return false;
    if (!consumeDigit(*text++, target.tm_year)) return false;
    if (!consumeDigit(*text++, target.tm_year)) return false;
    if (!consumeDigit(*text++, target.tm_year)) return false;
    if (!consumeDigit(*text++, target.tm_year)) return false;
    if (*text++ != '\0') return false;

    return true;
}

bool parseTime(const char * text, DateTime & target)
{
    target.tm_sec = 0;
    target.tm_min = 0;
    target.tm_hour = 0;

    if (!consumeDigit(*text++, target.tm_hour)) return false;
    if (!consumeDigit(*text++, target.tm_hour)) return false;
    if (*text++ != ':') return false;
    if (!consumeDigit(*text++, target.tm_min)) return false;
    if (!consumeDigit(*text++, target.tm_min)) return false;
    if (*text == '\0') return true;
    if (*text++ != ':') return false;
    if (!consumeDigit(*text++, target.tm_sec)) return false;
    if (!consumeDigit(*text++, target.tm_sec)) return false;
    if (*text++ != '\0') return false;

    return true;
}

void toStringDate(DateTime dateTime, char *text)
{
    *text++ = ((dateTime.tm_mday / 10) % 10) + '0';
    *text++ = ((dateTime.tm_mday /  1) % 10) + '0';
    *text++ = '/';
    *text++ = ((dateTime.tm_mon / 10) % 10) + '0';
    *text++ = ((dateTime.tm_mon /  1) % 10) + '0';
    *text++ = '/';
    *text++ = ((dateTime.tm_year / 1000) % 10) + '0';
    *text++ = ((dateTime.tm_year /  100) % 10) + '0';
    *text++ = ((dateTime.tm_year /   10) % 10) + '0';
    *text++ = ((dateTime.tm_year /    1) % 10) + '0';
    *text++ = '\0';
}

void toStringTime(DateTime dateTime, char *text)
{
    *text++ = ((dateTime.tm_hour / 10) % 10) + '0';
    *text++ = ((dateTime.tm_hour /  1) % 10) + '0';
    *text++ = ':';
    *text++ = ((dateTime.tm_min / 10) % 10) + '0';
    *text++ = ((dateTime.tm_min /  1) % 10) + '0';
    *text++ = ':';
    *text++ = ((dateTime.tm_sec / 10) % 10) + '0';
    *text++ = ((dateTime.tm_sec /  1) % 10) + '0';
    *text++ = '\0';
}

}
}
