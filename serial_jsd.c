#include <errno.h>
#include <fcntl.h> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#define MSG_SIZE 36

int set_interface_attribs(int fd, int speed)
{
    struct termios tty;

    if (tcgetattr(fd, &tty) < 0) {
        printf("Error from tcgetattr: %s\n", strerror(errno));
        return -1;
    }

    cfsetospeed(&tty, (speed_t)speed);
    cfsetispeed(&tty, (speed_t)speed);

    tty.c_cflag |= (CLOCAL | CREAD);    /* ignore modem controls */
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;         /* 8-bit characters */
    tty.c_cflag &= ~PARENB;     /* no parity bit */
    tty.c_cflag &= ~CSTOPB;     /* only need 1 stop bit */
    tty.c_cflag &= ~CRTSCTS;    /* no hardware flowcontrol */

    /* setup for non-canonical mode */
    tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL |
 IXON);
    tty.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
    tty.c_oflag &= ~OPOST;

    /* fetch bytes as they become available */
    tty.c_cc[VMIN] = 1;
    tty.c_cc[VTIME] = 1;

    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        printf("Error from tcsetattr: %s\n", strerror(errno));
        return -1;
    }
    return 0;
}

void sendBuffer(const unsigned char *buffer)
{
    static int outFDs[] = {-1, -1, -1, -1};
    char device[16];
    int index = buffer[0] & 0x0F;

    if (index < 0 || index > 3)
    {
	    printf("Invalid message\n");
	    return;
    }

    if (outFDs[index] == -1)
    {
	sprintf(device, "/dev/serial_js%d", index);
        outFDs[index] = open(device, O_RDWR);
    }

    write(outFDs[index], buffer + 1, MSG_SIZE - 1);
}

int connect(const char *portname)
{
    int fd = -1;
    while (fd < 0)
    {
	fd = open(portname, O_RDONLY);
	if (fd < 0) {
	    printf("Error opening %s: %s\n", portname, strerror(errno));
	    sleep(5);
	}
    }

    /*baudrate 115200, 8 bits, no parity, 1 stop bit */
    set_interface_attribs(fd, B115200);

    return fd;
}

int main(int argc, char *argv[])
{
    int fd;
    unsigned char buf[2 * MSG_SIZE];
    unsigned char *pBuf = buf;

    if (argc != 2)
    {
	    printf("Error on number of program arguments %d\n", argc);
	    return -1;
    }

    fd = connect(argv[1]);

    /* simple noncanonical input */
    do {
        int rdlen;

        rdlen = read(fd, pBuf, sizeof(buf) - (pBuf - buf) - 1);
        if (rdlen > 0) {
	    unsigned char *firstBreak = strchr(buf, '\n');

            pBuf[rdlen] = 0;
	    pBuf += rdlen;

	    if (firstBreak == NULL)
	    {
		    continue;
	    }

	    if (firstBreak - buf == MSG_SIZE)
	    {
		    sendBuffer(buf);
	    }

	    memmove(buf, firstBreak + 1, (pBuf - firstBreak));
	    pBuf = strchr(buf, '\0');
	    memset(pBuf, 0, sizeof(buf) - (pBuf - buf));
        } else if (rdlen < 0) {
            printf("Error from read: %d: %s\n", rdlen, strerror(errno));
	    close(fd);

	    // Try to reconnect
	    fd = connect(argv[1]);
        }
        /* repeat read to get full message */
    } while (1);
}
