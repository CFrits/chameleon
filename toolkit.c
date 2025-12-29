/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2025 C.F. Wiersma
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <pthread.h>
#include <termios.h>
#include <errno.h>
#include <signal.h>
#include <fcntl.h>

#define BAUDRATE B57600
#define _POSIX_SOURCE 1 /* POSIX compliant source */

int modem_fd;
int debug = 0;
int ignore = 0;
char *modemdevice = "/dev/ttyUSB0";

// Thread variables.
pthread_t chameleon_in;

struct termios oldtio;

int received_ack = 0;

void delay_until_ack() {
    long count = 500000;

    while (count >= 0) {

        if (received_ack > 0) {
            return;
        }
        count--;
    }
}

/* msleep(): Sleep for the requested number of milliseconds. */
int msleep(long msec)
{
    struct timespec ts;
    int res;

    if (msec < 0) {
        errno = EINVAL;
        return -1;
    }

    ts.tv_sec = msec / 1000;
    ts.tv_nsec = (msec % 1000) * 1000000;

    do {
        res = nanosleep(&ts, &ts);
    } while (res && errno == EINTR);

    return res;
}

void send_ack() {
    char buffer[] = { 0x02, 0x40 };

    // Send ACK
    write(modem_fd, buffer, buffer[0]);
}


enum Panelstate { LENGTH, CMDTYPE, SUBTYPE, DATA };

void panel_protocol (unsigned char ch) {
    static unsigned char len;
    static unsigned char length;
    static unsigned char cmd;
    static unsigned char subcmd;
    static unsigned char pos = 0;
    static unsigned char datapos = 0;
    static char buffer[255];
    static unsigned char data[40];
    static enum Panelstate state = LENGTH;

    switch (state) {
        case LENGTH:
            len = ch;
            length = ch;
            state = CMDTYPE;
            break;

        case CMDTYPE:
            cmd = ch;
            state = DATA;

            if (cmd == 0x80 || cmd == 0x81) {
                state = SUBTYPE;
            }

            len--;
            break;

        case SUBTYPE:
            subcmd = ch;
            state  = DATA;
            len--;
            break;

        case DATA:
            state = DATA;

            buffer[pos++] = ch;
            data[datapos++] = ch;
            // Always set the next character to 0.
            data[datapos]   = '\0';

            len--;
            break;

        default:
            break;
    }

    if (len < 2) {
        // A complete message has been received. The length itself does not count.
        state = LENGTH;

        // A complete command has been received. Send ACK
        if (cmd != 0x40) {
            send_ack();
            msleep(10);
        }

        switch (cmd) {

            case 0x00:
                // Panel Init
                break;

            case 0x40:
                // Received ACK
                received_ack = 1;
                //fprintf(stderr, "CHAM: Received ACK\n");
                break;

            case 0x80:
            case 0x81:

                switch (subcmd) {

                    // Reset Chameleon.
                    case 0x01:
                        break;

                    case 0x02:
                        // Received the Serial number.
                        if (datapos > 0) printf("Serialnumber: %.*s", datapos, data+5);
                        printf("\n");
                        break;

                    case 0x03:

                        if (pos > 0) printf("%.*s", pos, buffer);
                        fflush(stdout);
                        break;

                    case 0x10:
                        // Heartbeat
                        break;

                    default:
                        fprintf(stderr, "CHAM: Unknown subcmd: <%02X>\n", subcmd);
                        break;

                }

                break;

            case 0xFF:
                break;

            default:
                if (length == 0x04) {
                    fprintf(stderr, "CHAM: Unknown packet: LEN <%02X> CMD <%02X> <%02X> <%02X> \n", length, cmd, data[0], data[1]);
                } else {
                    fprintf(stderr, "CHAM: Unknown packet: LEN <%02X> CMD <%02X>\n", length, cmd);
                }
                break;

        }

        // Clean buffer.
        pos = 0;
        datapos = 0;
        cmd = LENGTH;
        buffer[0] = '\0';
    }
}


void *from_chameleon(void *arg) {
    int n;
    unsigned char ch;

    while (1) {
        n = read(modem_fd, &ch, 1);

        if (n < 0) {
            perror("Error reading from modem");
            tcsetattr(0, TCSANOW, &oldtio);    /* Restore old settings */
            //exit(EXIT_FAILURE);
        }

        if (n > 0) {
            panel_protocol(ch);
        }
    }
}

static void send_ctrl_c() {
    char buffer[] = { 0x04, 0x81, 0x03, 0x03 };
    write(modem_fd, buffer, buffer[0]);
}

static void sigint(int sig) {
    fprintf(stderr, "SIGINT\n");

    if (ignore) {
        send_ctrl_c();
    } else {
        tcsetattr(0, TCSANOW, &oldtio);    /* Restore old settings */
        exit(0);
    }
}

void panel_transmit_string (const char *msg) {
    int l;
    char message[32];
    int len;

    len = strlen(msg);

    // The chameleon does not support messages longer than 32 characters.
    // The string needs to be split up.
    for (size_t i = 0; i < len; i += 29) {
        //received_ack = 0;
    
        l = (len - i < 29) ? (len - i) : 29;
    
        message[0] = l + 3;
        message[1] = 0x81;
        message[2] = 0x03;
    
        for (int j = 0; j < l; j++) {
            message[3 + j] = msg[i + j];
        }

        // TODO: Experimental. This code should be as short as possible.
        // The uart interrupt must not send an ACK in the middle of another message.
        //disable_interrupts();
        write(modem_fd, message, l + 3);
        //enable_interrupts();

        // Wait for an Ack.
        //wait_for_ack();

        // Wait some time anyway.
        //mp_hal_delay_ms(20);
    }
}

void open_modemdevice() {
    struct termios newtio;
    struct termios modemtio;

    modem_fd = open(modemdevice, O_RDWR | O_NOCTTY);
    
    if (modem_fd < 0) {
        perror(modemdevice);
        exit(-1);
    }       

    bzero(&modemtio, sizeof(modemtio));
    modemtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
    modemtio.c_iflag = IGNPAR;
    modemtio.c_oflag = 0;

    modemtio.c_lflag = 0;
    
    modemtio.c_cc[VTIME] = 0;   /* inter-character timer */
    modemtio.c_cc[VMIN]  = 1;   /* blocking read until at least 1 chars received */
    
    tcflush(modem_fd, TCIFLUSH);
    tcsetattr(modem_fd, TCSANOW, &modemtio);

    // Changes the terminal settting for stdin.
    tcgetattr(0,  &oldtio);     /* Save current termio settings */
        
    //cfmakeraw(&newtio);
    tcgetattr(0,  &newtio);

    // Flags like cfmakeraw
    newtio.c_iflag |= IGNBRK;
    newtio.c_lflag &= ~(ECHO | ICANON);

    tcsetattr(0, TCSANOW, &newtio);
}


int main(int argc, char* const *argv) {
    int ch;
    char buffer[255];
    int opt;
    char *command = NULL;

    while ((opt = getopt(argc, argv, "dhim:")) != -1) {

        switch (opt) {
            case 'h':
                printf("Usage: panel [-dhim:]\n" );
                printf("       -d   Debug on\n" );
                printf("       -h   This help message\n" );
                printf("       -i   Ignore Ctrl-C signal\n" );
                printf("       -m   Modem device\n" );
                exit(0);
                break;

            case 'd':
                debug = 1;
                printf("Debug is on\n" );
                break;

            case 'i':
                ignore = 1;
                printf("Ignore Ctrl signal is on\n" );
                break;

            case 'm':
                printf("Set modem to <%s>\n", argv[optind-1]);
                modemdevice = argv[optind-1];
                break;

            default:
                printf("Unknown option\n" );
                exit(0);
                break;
        }
    }

    for (int i = optind; i < argc; i++) {
        printf("Additional argument: %s\n", argv[i]);
        command = argv[i];
    }

    open_modemdevice();

    // Create a thread to read data from the socket and send it to stdout.
    if (pthread_create(&chameleon_in, NULL, from_chameleon, &modem_fd)) {
        perror("Can't create chameleon thread");
        goto error;
    }

    signal(SIGINT, sigint);

    while (1) {

        ch = getchar();

        if (ch == '\n') {
            ch = '\r';
        }

        buffer[0] = 0x04;
        buffer[1] = 0x81;
        buffer[2] = 0x03;
        buffer[3] = ch;

        write(modem_fd, buffer, 4);
        msleep(40);

        delay_until_ack();
    }

    close(modem_fd);

error:
    tcsetattr(0, TCSANOW, &oldtio);    /* Restore old settings */
    return 0;
}
