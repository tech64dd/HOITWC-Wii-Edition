#include <stdio.h>
#include <stdlib.h>
#include <gccore.h>
#include <time.h>
#include <wiiuse/wpad.h>

static void *xfb = NULL;
static GXRModeObj *rmode = NULL;

static char *months[] = {
	"????",
	"Gennaio", "Febbraio", "Marzo",
	"Aprile", "Maggio", "Giugno", "Luglio",
	"Agosto", "Settembre", "Ottobre",
	"Novembre", "Dicembre"
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

//---------------------------------------------------------------------------------
int main(int argc, char **argv) {
//---------------------------------------------------------------------------------

	// Initialise the video system
	VIDEO_Init();

	// This function initialises the attached controllers
	WPAD_Init();

	// Obtain the preferred video mode from the system
	// This will correspond to the settings in the Wii menu
	rmode = VIDEO_GetPreferredMode(NULL);

	// Allocate memory for the display in the uncached region
	xfb = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));

	// Initialise the console, required for printf
	console_init(xfb,20,20,rmode->fbWidth,rmode->xfbHeight,rmode->fbWidth*VI_DISPLAY_PIX_SZ);
	//SYS_STDIO_Report(true);

	// Set up the video registers with the chosen mode
	VIDEO_Configure(rmode);

	// Tell the video hardware where our display memory is
	VIDEO_SetNextFramebuffer(xfb);

	// Make the display visible
	VIDEO_SetBlack(false);

	// Flush the video register changes to the hardware
	VIDEO_Flush();

	// Wait for Video setup to complete
	VIDEO_WaitVSync();
	if(rmode->viTVMode&VI_NON_INTERLACE) VIDEO_WaitVSync();


	// The console understands VT terminal escape codes
	// This positions the cursor on row 2, column 0
	// we can use variables for this with format codes too
	// e.g. printf ("\x1b[%d;%dH", row, column );
	printf("\x1b[2;0H");


	int numTotalDays, numYears, numMonths, numDays;
	int day, month, year;
	puts("La Wii usa il PowerPC 750CL come CPU, una iterazione minore sul design del PowerPC 750. \nQuesta CPU è anche conosciuta come il\"G3\" dalla Apple.");
	printf("Il PowerPC 750 è stato rilasciato il %s %d, %d.\n", months[relMon], relDay, relYear);

	time_t t = time(NULL);
	struct tm tm = *localtime(&t);
	
	day = tm.tm_mday;
	month = tm.tm_mon + 1;
	year = tm.tm_year + 1900;

	numTotalDays = dateDifference(relDay, relMon, relYear, day, month, year, &numDays, &numMonths, &numYears);
	
	printf("Oggi è: %s %d, %d.  Sono stati %d days, o %d anni, %d mesi,\ne %d giorni, dal rilascio del PowerPC 750\n",
			months[month],
			day,
			year,
			numTotalDays,
			numYears,
			numMonths,
			numDays
	      );

	while(1) {

		// Call WPAD_ScanPads each loop, this reads the latest controller states
		WPAD_ScanPads();

		// WPAD_ButtonsDown tells us which buttons were pressed in this loop
		// this is a "one shot" state which will not fire again until the button has been released
		u32 pressed = WPAD_ButtonsDown(0);

		// We return to the launcher application via exit
		if ( pressed & WPAD_BUTTON_HOME ) exit(0);

		// Wait for the next frame
		VIDEO_WaitVSync();
	}

	return 0;
}
