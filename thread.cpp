#include "thread.h"

Thread::Thread(QObject *parent) : QThread(parent)
{
    delimiter = 1;

    fd = open(MODEMDEVICE, O_RDWR | O_NOCTTY );

    if (fd <0) { qDebug() << "Thread: not open: "  << MODEMDEVICE; exit(-1); }
    else{ qDebug() << "Thread: opened port"; }

    tcgetattr(fd,&oldtio); /* save current serial port settings */
    bzero(&newtio, sizeof(newtio)); /* clear struct for new port settings */

    // commented out to clear the UTF-8 seeting
    // may need at later stage
    //newtio.c_cflag = BAUDRATE | CRTSCTS | CS8 | CLOCAL | CREAD;
    newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
    newtio.c_iflag = IGNPAR;
    newtio.c_oflag = 0;
    newtio.c_lflag = 0;

    newtio.c_cc[VTIME]    = 0;     /* inter-character timer unused */
    newtio.c_cc[VMIN]     = 1;     /* blocking read until 1 character arrives */

    tcflush(fd, TCIFLUSH);
    tcsetattr(fd,TCSANOW,&newtio);
}

int Thread::serial_reader()
{
    FD_ZERO(&set);
    FD_SET(fd, &set);
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;
    rv = select(fd + 1, &set, NULL, NULL, &timeout);
    if(rv == -1){    }
    else if(rv == 0){    }
    else
    {
        res = read( fd, buf, 255 ); /* there was data to read */
        buf[res]=0;
    }
    return buf[0];
}

void Thread::run()
{
    while(1){
        if(this->serial_reader() == 1)
        {
            qDebug() << "Thread: Frame received";
            unsigned char frame_type = serial_reader();
            qDebug() << "Thread: Frame type: " << (char) frame_type;
            unsigned char pay_len = serial_reader();
            qDebug() << "Thread: Payload length" <<pay_len;
            unsigned char pay[255];
            for(int i=0; i< pay_len; i++)
            {
                unsigned char payload = serial_reader();
                qDebug() << payload;
                pay[i] = payload;
            }
            qDebug() << "Checking checksum";
            if(serial_reader() == (1 + pay_len) )
            {
                qDebug() << "Thread: Frame ok: Frame Type = " << (char)frame_type;
                // pass to mainwindow.cpp
                emit(passframetoMain(frame_type, (int)pay_len , pay));
            }
            else qDebug() << "Frame error: Checksum";
        }
        buf[0] = 0; // clear the buffer
    }
}

void Thread::send_usb_byte(unsigned char ubyte)
{
    qDebug() << "Thread: Sending byte: " << ubyte;
    usleep(50000);  // allows time for each byte to be read
    write(fd, &ubyte, 1);
}

void Thread::send_usb_frame(unsigned char frametype, QString frame)
{
    qDebug() << "Thread: send_usb_frame(): frame length -> " << frame.length();
    this->send_usb_byte(1);
    this->send_usb_byte(frametype);
    this->send_usb_byte(frame.length());
    for(int i=0; i<frame.length(); i++)
    {
        qDebug() <<"Thread: send_usb_frame(): char[" << i << "] -> " << (char)frame.at(i).toLatin1();
        this->send_usb_byte(frame.at(i).toLatin1());

    }
    this->send_usb_byte(1 + frame.length());
}

void Thread::send_usb_frame(unsigned char frametype, QPoint location)
{
    int payload_len = 2;
    this->send_usb_byte(1);
    this->send_usb_byte(frametype);
    this->send_usb_byte(payload_len);
    this->send_usb_byte(location.x());
    this->send_usb_byte(location.y());
    this->send_usb_byte(1 + payload_len);
}

void Thread::send_usb_byte(unsigned char frametype, int payload_len, unsigned char *payload)
{
    this->send_usb_byte(delimiter);     // send delimiter
    this->send_usb_byte(frametype);     // what type of frame is being sent
    this->send_usb_byte(payload_len);   // send payload length
    for(int i=0; i<payload_len; i++)
    {
        this->send_usb_byte(payload[i]);    // send payload bytes
    }
    this->send_usb_byte(delimiter + payload_len);   // checksum
}


