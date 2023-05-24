#ifndef DATE_H
#define DATE_H

struct Date {

	short day, month, year;
	short hour, minute;
};

struct Date time_now();

#endif
