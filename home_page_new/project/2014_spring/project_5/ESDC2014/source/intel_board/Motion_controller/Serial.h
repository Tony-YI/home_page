#ifndef _SERIAL_H
#define _SERIAL_H
#include <errno.h>
#include <termios.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int set_interface_attribs (int fd, int speed, int parity);
void set_blocking (int fd, int should_block);
/* EXAMPLE USAGE
char *portname = "/dev/ttyUSB1"
int fd = open (portname, O_RDWR | O_NOCTTY | O_SYNC);
if (fd < 0)
{
        fprintf(stderr,"%s\n", strerror (errno));
        return;
}

set_interface_attribs (fd, B115200, 0);  // set speed to 115,200 bps, 8n1 (no parity)
set_blocking (fd, 0);                // set no blocking

write (fd, "hello!\n", 7);           // send 7 character greeting
char buf [100];
int n = read (fd, buf, sizeof buf);  // read up to 100 characters if ready to read
*/
#endif
