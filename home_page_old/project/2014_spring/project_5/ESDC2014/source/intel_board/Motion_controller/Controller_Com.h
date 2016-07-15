/*

*/

#ifndef _CONTROLLER_COM_H
#define _CONTROLLER_COM_H

#include <stdio.h>
#include <stdint.h>
//for write() and read()
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <errno.h>
#include <termios.h>
#include <string.h>
#include "../lib/message.h"

int set_interface_attribs (int fd, int speed, int parity);
void set_blocking (int fd, int should_block);


class Controller_Com
{
private:
	
	uint32_t write(void *buff,uint32_t length);
	uint32_t read(void *buff,uint32_t length);
	char *write_buff;
	char *read_buff;
public:
	int fd;
	Controller_Com(const char *devname);
	~Controller_Com();
	uint8_t send_cmd(const Message &cmd);
};

#endif
