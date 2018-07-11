#ifndef THREAD_H
#define THREAD_H

#include <QThread>
#include <QDebug>
#include <QtCore>
#include <QtGui>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <sys/ioctl.h>
#include <sys/poll.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <unistd.h>

#define BAUDRATE B9600
#define MODEMDEVICE "/dev/ttyUSB0"
#define _POSIX_SOURCE 1 /* POSIX compliant source */

class Thread : public QThread
{
    Q_OBJECT
private:
    int fd, res;
    struct termios oldtio,newtio;
    char buf[255];
    unsigned char delimiter;

    fd_set set;
    struct timeval timeout;
    int rv;

public:
    explicit Thread(QObject *parent = 0);
    void run();
    void send_usb_byte(unsigned char);
    void send_usb_frame(unsigned char, QString);
    void send_usb_frame(unsigned char, QPoint);
    void send_usb_byte(unsigned char, int, unsigned char*);
    int serial_reader();

signals:
    void usb_byte(unsigned char);
    void passframetoMain(unsigned char, int, unsigned char*);

};

#endif // THREAD_H
