#include "Date.h"
#include <time.h>

struct Date time_now() {

	struct tm* date;
	struct Date _time;
	time_t tempo;
	
	tempo = time(NULL);

	date = localtime(&tempo);

	_time.hour = date->tm_hour;
	_time.minute = date->tm_min;
	_time.day = date->tm_mday;
	_time.month = date->tm_mon + 1;
	_time.year = date->tm_year + 1900;

	return _time;
}
