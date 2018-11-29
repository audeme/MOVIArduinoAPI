/*
  GPIO through SYSFS by Guillermo A. Amaral B.
  under a Creative Commons Attribution-ShareAlike 3.0 Unported License.
  Original code available at: https://elinux.org/RPi_GPIO_Code_Samples#sysfs
  
  Alternative, more powerful implementations of GPIO are discussed a  
  https://elinux.org/RPi_GPIO_Code_Samples
*/

#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "Arduino.h"
#include "sysfsio.h"

#define SYSFS_BUFFER_MAX 3
#define GPIO_VALUE_MAX 30
#define GPIO_DIRECTION_MAX 35

int GPIOExport(int pin)
{
	char buffer[SYSFS_BUFFER_MAX];
	ssize_t bytes_written;
	int fd;
	int trialsleft=5;

	while (trialsleft>0) { 
		fd = open("/sys/class/gpio/export", O_WRONLY);
		if (-1 == fd) {
			trialsleft--;
			if (trialsleft==0) {
				fprintf(stderr, "Pi: Failed to open GPIO %d for writing!\n", pin);
				return(-1);
			}
			else delay(100);
		} else {
			trialsleft=0;
		}
	}

	bytes_written = snprintf(buffer, SYSFS_BUFFER_MAX, "%d", pin);
	write(fd, buffer, bytes_written);
	close(fd);
	return(0);
}

int GPIOUnexport(int pin)
{
	char buffer[SYSFS_BUFFER_MAX];
	ssize_t bytes_written;
	int fd;

	fd = open("/sys/class/gpio/unexport", O_WRONLY);
	if (-1 == fd) {
		fprintf(stderr, "Pi: Failed to open GPIO %d unexport for writing!\n",pin);
		return(-1);
	}

	bytes_written = snprintf(buffer, SYSFS_BUFFER_MAX, "%d", pin);
	write(fd, buffer, bytes_written);
	close(fd);
	return(0);
}

int GPIODirection(int pin, int dir)
{
	static const char s_directions_str[]  = "in\0out";
	char path[GPIO_DIRECTION_MAX];
	int fd;

	snprintf(path, GPIO_DIRECTION_MAX, "/sys/class/gpio/gpio%d/direction", pin);
	fd = open(path, O_WRONLY);
	if (-1 == fd) {
		fprintf(stderr, "Pi: Failed to setup GPIO %d for writing!\n", pin);
		return(-1);
	}

	if (-1 == write(fd, &s_directions_str[INPUT == dir ? 0 : 3], INPUT == dir ? 2 : 3)) {
		fprintf(stderr, "Pi: Failed to set GPIO %d direction!\n",pin);
		return(-1);
	}

	close(fd);
	return(0);
}

int GPIORead(int pin)
{
	char path[GPIO_VALUE_MAX];
	char value_str[3];
	int fd;

	snprintf(path, GPIO_VALUE_MAX, "/sys/class/gpio/gpio%d/value", pin);
	fd = open(path, O_RDONLY);
	if (-1 == fd) {
		fprintf(stderr, "Pi: Failed to open GPIO %d value for reading!\n",pin);
		return(-1);
	}

	if (-1 == read(fd, value_str, 3)) {
		fprintf(stderr, "Pi: Failed to read value from GPIO %d!\n",pin);
		return(-1);
	}

	close(fd);

	return(atoi(value_str));
}

int GPIOWrite(int pin, int value)
{
	static const char s_values_str[] = "01";

	char path[GPIO_VALUE_MAX];
	int fd;

	snprintf(path, GPIO_VALUE_MAX, "/sys/class/gpio/gpio%d/value", pin);
	fd = open(path, O_WRONLY);
	if (-1 == fd) {
		fprintf(stderr, "Pi: Failed to open GPIO %d for writing!\n", pin);
		return(-1);
	}

	if (1 != write(fd, &s_values_str[LOW == value ? 0 : 1], 1)) {
		fprintf(stderr, "Pi: Failed to write value to GPIO %d!\n",pin);
		return(-1);
	}

	close(fd);
	return(0);
}
