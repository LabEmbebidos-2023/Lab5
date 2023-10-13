// /*******************************************************************************
// *
// *   i2c.c
// *
// *   Copyright (c) 2013 Shahrooz Shahparnia (sshahrooz@gmail.com)
// *
// *   Description:
// *   i2c is a command-line utility for executing i2c commands with the
// *   Broadcom bcm2835.  It was developed and tested on a Raspberry Pi single-board
// *   computer model B.  The utility is based on the bcm2835 C library developed
// *   by Mike McCauley of Open System Consultants, http://www.open.com.au/mikem/bcm2835/.
// *
// *   Invoking spincl results in a read or write I2C transfer.  Options include the
// *   the I2C clock frequency, read/write, address, and port initialization/closing
// *   procedures.  The command usage and command-line parameters are described below
// *   in the showusage function, which prints the usage if no command-line parameters
// *   are included or if there are any command-line parameter errors.  Invoking i2c
// *   requires root privilege.
// *
// *   This file contains the main function as well as functions for displaying
// *   usage and for parsing the command line.
// *
// *   Open Source Licensing GNU GPLv3
// *
// *   Building:
// * After installing bcm2835, you can build this
// * with something like:
// * gcc -o i2c i2c.c -l bcm2835
// * sudo ./i2c
// *
// * Or you can test it before installing with:
// * gcc -o i2c -I ../../src ../../src/bcm2835.c i2c.c
// * sudo ./i2c
// *
// *   History:
// *   11/05    VERSION 1.0.0: Original
// *
// *      User input parsing (comparse) and showusage\
// *      have been adapted from: http://ipsolutionscorp.com/raspberry-pi-spi-utility/
// *      mostly to keep consistence with the spincl tool usage.
// *
// *      Compile with: gcc -o i2c i2c.c bcm2835.c
// *
// *      Examples:
// *
// *           Set up ADC (Arduino: ADC1015)
// *           sudo ./i2c -s72 -dw -ib 3 0x01 0x44 0x00 (select config register, setup mux, etc.)
// *           sudo ./i2c -s72 -dw -ib 1 0x00 (select ADC data register)
// *
// *           Bias DAC (Arduino: MCP4725) at some voltage
// *           sudo ./i2c -s99 -dw -ib 3 0x60 0x7F 0xF0 (FS output is with 0xFF 0xF0)
// *           Read ADC convergence result
// *           sudo ./i2c -s72 -dr -ib 2 (FS output is 0x7FF0 with PGA1 = 1)
// *
// *      In a DAC to ADC loop back typical results are:
// *
// *      DAC    VOUT   ADC
// *      7FFh   1.6V   677h                    Note ratio is FS_ADC*PGA_GAIN/FS_DAC = 4.096/3.3 = 1.23
// *      5FFh   1.2V   4DCh
// *      8F0h   1.8V   745h
// *      9D0h   2V     7EAh
// *      000h   10mV   004h
// *
// ********************************************************************************/

// #include <bcm2835.h>
// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <stdint.h>

// #define MODE_READ 0
// #define MODE_WRITE 1

// #define MAX_LEN 32

// char wbuf[MAX_LEN];

// typedef enum {
//     NO_ACTION,
//     I2C_BEGIN,
//     I2C_END
// } i2c_init;

// uint8_t  init = NO_ACTION;
// uint16_t clk_div = BCM2835_I2C_CLOCK_DIVIDER_148;
// uint8_t slave_address = 0x00;
// uint32_t len = 0;
// uint8_t  mode = MODE_READ;

// char buf[MAX_LEN];

// int i;
// uint8_t data;


// void readI2C(int slave, int len){
//     if (!bcm2835_init())
//     {
//       printf("bcm2835_init failed. Are you running as root??\n");
//       return 1;
//     }

//     // I2C begin if specified
//     if (init == I2C_BEGIN)
//     {
//       if (!bcm2835_i2c_begin())
//       {
//         printf("bcm2835_i2c_begin failed. Are you running as root??\n");
//         return 1;
//       }
//     }


//     // If len is 0, no need to continue, but do I2C end if specified
//     if (len == 0) {
//          if (init == I2C_END) bcm2835_i2c_end();
//          printf("... done!\n");
//          return EXIT_SUCCESS;
//     }

//     bcm2835_i2c_setSlaveAddress(slave_address);
//     bcm2835_i2c_setClockDivider(clk_div);

//     if (mode == MODE_READ) {
//         for (i=0; i<MAX_LEN; i++) buf[i] = 'n';
//         uint8_t status = bcm2835_i2c_read(buf, len);
//         for (i=0; i<MAX_LEN; i++) {
//                 if(buf[i] != 'n') printf("Read Buf[%d] = %x\n", i, buf[i]);
//         }
//     }

//     // This I2C end is done after a transfer if specified
//     if (init == I2C_END) bcm2835_i2c_end();
//     bcm2835_close();
// }

// int main(int argc, char **argv) {

//     printf("Running ... \n");
//     readI2C(104, 7);
//     printf("... done!\n");
//     return 0;
// }


/*******************************************************************************
*
*   i2c.c
*
*   Copyright (c) 2013 Shahrooz Shahparnia (sshahrooz@gmail.com)
*
*   Description:
*   i2c is a command-line utility for executing i2c commands with the
*   Broadcom bcm2835.  It was developed and tested on a Raspberry Pi single-board
*   computer model B.  The utility is based on the bcm2835 C library developed
*   by Mike McCauley of Open System Consultants, http://www.open.com.au/mikem/bcm2835/.
*
*   Invoking spincl results in a read or write I2C transfer.  Options include the
*   the I2C clock frequency, read/write, address, and port initialization/closing
*   procedures.  The command usage and command-line parameters are described below
*   in the showusage function, which prints the usage if no command-line parameters
*   are included or if there are any command-line parameter errors.  Invoking i2c
*   requires root privilege.
*
*   This file contains the main function as well as functions for displaying
*   usage and for parsing the command line.
*
*   Open Source Licensing GNU GPLv3
*
*   Building:
* After installing bcm2835, you can build this
* with something like:
* gcc -o i2c i2c.c -l bcm2835
* sudo ./i2c
*
* Or you can test it before installing with:
* gcc -o i2c -I ../../src ../../src/bcm2835.c i2c.c
* sudo ./i2c
*
*   History:
*   11/05    VERSION 1.0.0: Original
*
*      User input parsing (comparse) and showusage\
*      have been adapted from: http://ipsolutionscorp.com/raspberry-pi-spi-utility/
*      mostly to keep consistence with the spincl tool usage.
*
*      Compile with: gcc -o i2c i2c.c bcm2835.c
*
*      Examples:
*
*           Set up ADC (Arduino: ADC1015)
*           sudo ./i2c -s72 -dw -ib 3 0x01 0x44 0x00 (select config register, setup mux, etc.)
*           sudo ./i2c -s72 -dw -ib 1 0x00 (select ADC data register)
*
*           Bias DAC (Arduino: MCP4725) at some voltage
*           sudo ./i2c -s99 -dw -ib 3 0x60 0x7F 0xF0 (FS output is with 0xFF 0xF0)
*           Read ADC convergence result
*           sudo ./i2c -s72 -dr -ib 2 (FS output is 0x7FF0 with PGA1 = 1)
*
*      In a DAC to ADC loop back typical results are:
*
*      DAC    VOUT   ADC
*      7FFh   1.6V   677h                    Note ratio is FS_ADC*PGA_GAIN/FS_DAC = 4.096/3.3 = 1.23
*      5FFh   1.2V   4DCh
*      8F0h   1.8V   745h
*      9D0h   2V     7EAh
*      000h   10mV   004h
*
********************************************************************************/

#include <bcm2835.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <time.h>

#define MAX_LEN 32


struct Timestamp{
    uint8_t seconds;
    uint8_t minutes;
    uint8_t hours;

    uint8_t day;
    uint8_t date;
    uint8_t month;
    uint8_t year;
};

struct Report{
    uint8_t seconds;
    struct Timestamp timestamp;
};



void readI2C(int slave_address, uint16_t clk_div, int len, char buf[]){
    if (!bcm2835_init())
    {
      printf("bcm2835_init failed. Are you running as root??\n");
      return;
    }

    if (!bcm2835_i2c_begin())
    {
    printf("bcm2835_i2c_begin failed. Are you running as root??\n");
    return;
    }

    // If len is 0, no need to continue, but do I2C end if specified
    if (len == 0) {
         printf("... done!\n");
         return;
    }

    bcm2835_i2c_setSlaveAddress(slave_address);
    bcm2835_i2c_setClockDivider(clk_div);

    int i;
    for (i=0; i<MAX_LEN; i++) buf[i] = 'n';
    uint8_t status = bcm2835_i2c_read(buf, len);
    for (i=0; i<MAX_LEN; i++) {
            if(buf[i] != 'n'){
                //printf("Read Buf[%d] = %x\n", i, buf[i]);
            }
    }

    bcm2835_i2c_end();
    bcm2835_close();
}

void writeI2C(int slave_address, uint16_t clk_div, int len, char buf[]){
    if (!bcm2835_init())
    {
      printf("bcm2835_init failed. Are you running as root??\n");
      return;
    }

    if (!bcm2835_i2c_begin())
    {
    printf("bcm2835_i2c_begin failed. Are you running as root??\n");
    return;
    }

    // If len is 0, no need to continue, but do I2C end if specified
    if (len == 0) {
         printf("... done!\n");
         return;
    }

    bcm2835_i2c_setSlaveAddress(slave_address);
    bcm2835_i2c_setClockDivider(clk_div);

    int i;
    int status = bcm2835_i2c_write(buf, len);

    bcm2835_i2c_end();
    bcm2835_close();
}

void resetTime(){
    char wbuf[8];
    wbuf[0] = 0;
    wbuf[1] = 0;
    wbuf[2] = 0;
    wbuf[3] = 12;
    wbuf[4] = 1;
    wbuf[5] = 1;
    wbuf[6] = 1;
    wbuf[7] = 1;

    writeI2C(104, 2500, 8, wbuf);
}

struct Timestamp readTime(){
    char buf[MAX_LEN];
    buf[0] = 0;

    writeI2C(104, 2500, 1, buf);
    readI2C(104, 2500, 7, buf);

    struct Timestamp newTimeStamp;
    newTimeStamp.seconds = buf[0];
    newTimeStamp.minutes = buf[1];
    newTimeStamp.hours = buf[2];

    newTimeStamp.day = buf[3];
    newTimeStamp.date = buf[4];
    newTimeStamp.month = buf[5];
    newTimeStamp.year = buf[6];

    return newTimeStamp;
}

int readTemp(){
    char buf[MAX_LEN];
    readI2C(77, 2500, 1, buf);
    return buf[0];
}

void printTime(int record, struct Timestamp time, FILE *logFile){
    static char nameDays[7][4]  = {"Sun\0", "Mon\0", "Tue\0", "Wed\0", "Thu\0", "Fri\0", "Sat\0"};
    static char timeOfDay[2][3]  = {"AM\0", "PM\0"};
    unsigned char timeOfDayIndex = time.hours > 12 ? 1 : 0;

    char timeStr[200];
    sprintf(timeStr, "Record %d: %d/%d/%d %s %02d:%02d:%02x %s\n", record, time.date, time.month, time.year, nameDays[time.day % 7], time.hours, time.minutes, time.seconds, timeOfDay[timeOfDayIndex]);
    fprintf(logFile, "%s", timeStr);
    printf("%s", timeStr);
}

void printReading(int record, struct Timestamp times[3], int temp, FILE *logFile){
    system("clear");
    char tempStr[100];
    sprintf(tempStr, "%d°C\n", temp);
    fprintf(logFile,"%s", tempStr);
    printf("%s", tempStr);

    printTime(1, times[0], logFile);
    
    if(record > 1){
        printTime(2, times[1], logFile);
    }

    if(record > 2){
        printTime(3, times[2], logFile);
    }
}

int main(int argc, char **argv) {

    printf("Running ... \n");

    int currentTime = time(NULL);
    resetTime();

    struct Timestamp times[3];
    int currentTimeIndex = 0;
    int currentRecord = 0;

    FILE *logFile;

    do{
        int temp = readTemp();

        if(time(NULL) - currentTime > 10 || temp > 30){
            logFile = fopen("log.txt","w");
            currentTimeIndex = currentRecord >= 2 ? 2 : currentRecord;

            times[currentTimeIndex] = readTime();

            printReading(currentRecord, times, temp, logFile);
            currentTime = time(NULL);

            if(currentRecord >= 2){
                times[0] = times[1];
                times[1] = times[2];
            }

            currentRecord++;
            fclose(logFile);
        }

        sleep(1);
    }while(1);
 

    printf("... done!\n");
    return 0;
}
