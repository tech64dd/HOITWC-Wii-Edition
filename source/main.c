#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <3ds.h>

static char *months[] = {
	"????",
	"January", "February", "March",
	"April", "May", "June", "July",
	"August", "September", "October",
	"November", "December"
};

static int relMon = 11;
static int relDay = 10;
static int relYear = 1997;


// Function to check if a year is a leap year
static int isLeapYear(int year) {
	return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

// Function to return the number of days in a month
static int daysInMonth(int month, int year) {
	int daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

	// February and leap year check
	if (month == 2 && isLeapYear(year)) {
		return 29;
	}
	return daysInMonth[month - 1];
}

// Function to calculate the total number of days between two dates
static int totalDaysBetweenDates(int d1, int m1, int y1, int d2, int m2, int y2) {
    int totalDays = 0;

    // Count the days from the first date to the end of that month
    totalDays += daysInMonth(m1, y1) - d1 + 1;

    // Move to the next month
    m1++;
    if (m1 > 12) {
        m1 = 1;
        y1++;
    }

    // Count the days in the full months between the two dates
    while (y1 < y2 || (y1 == y2 && m1 < m2)) {
        totalDays += daysInMonth(m1, y1);
        m1++;
        if (m1 > 12) {
            m1 = 1;
            y1++;
        }
    }

    // Add the days from the start of the last month to the second date
    totalDays += d2 - 1;

    return totalDays;
}

// Function to calculate the difference between two dates
static int dateDifference(int d1, int m1, int y1, int d2, int m2, int y2,
                    int *dayDiff, int *monthDiff, int *yearDiff)  {
	// Ensure that the first date is earlier than the second date
	if (d1 > d2) {
		d2 += daysInMonth(m2, y2);
		m2 -= 1;
	}
	if (m1 > m2) {
		m2 += 12;
		y2 -= 1;
	}

	*dayDiff = d2 - d1;
	*monthDiff = m2 - m1;
	*yearDiff = y2 - y1;
	return totalDaysBetweenDates(d1, m1, y1, d2, m2, y2);
}

int main(int argc, char* argv[])
{
	gfxInitDefault();
	consoleInit(GFX_TOP, NULL);

	int numTotalDays, numYears, numMonths, numDays;
	int day, month, year;
	puts("The Wii uses the PowerPC 750CL CPU, a minor \niteration on the PowerPC 750 design.\n\nThis CPU is also known as the \"G3\" from Apple.");
	printf("The PowerPC 750 released on %s %d, %d.\n", months[relMon], relDay, relYear);

	time_t t = time(NULL);
	struct tm tm = *localtime(&t);
	
	day = tm.tm_mday;
	month = tm.tm_mon + 1;
	year = tm.tm_year + 1900;

	numTotalDays = dateDifference(relDay, relMon, relYear, day, month, year, &numDays, &numMonths, &numYears);
	
	printf("\nToday is: %s %d, %d.  \nIt has been %d days, or %d years, %d months,\nand %d days, since the release of the PowerPC 750.\n",
			months[month],
			day,
			year,
			numTotalDays,
			numYears,
			numMonths,
			numDays
	      );

	// Main loop
	while (aptMainLoop())
	{
		gspWaitForVBlank();
		gfxSwapBuffers();
		hidScanInput();

		// Your code goes here
		u32 kDown = hidKeysDown();
		if (kDown & KEY_START)
			break; // break in order to return to hbmenu
	}

	gfxExit();
	return 0;
}
