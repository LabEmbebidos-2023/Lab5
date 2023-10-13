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
