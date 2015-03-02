/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtBluetooth module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QObject>
#include <QDebug>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fstream>
#include <iostream>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <time.h>
#include <errno.h>

#include <QTimer>

QT_USE_NAMESPACE

class Daq;
class DataOut;

class Server: public QObject
{
    Q_OBJECT

public:
    Server(QObject *parent=0);
    Server(Daq& daqIn);
    ~Server();
    void setDaq(Daq& daqIn);
    void printWriteLog(const QString &message);

private:
    void sendPacket(DataOut&);
    void showMessage(const QString &sender, const QString &message);
    void clientConnected(const QString &name);
    void clientDisconnected(const QString &name);
    void connected(const QString &name);
    int currentAdapterIndex;

    int sockfd, newsockfd;
    socklen_t clilen;
    char* cliaddr ;
    const char* servmacaddr;
    struct sockaddr_rc serv_addr, cli_addr;
    void messageReceived(const QString &sender, const QString &message);
    void clientConnected(int &sockfd);
    void clientDisconnected(int &sockfd);
    void sendMessage(const QString &message);
    void startServer();
    void restartServer();
    void setTimeout(int seconds);
    void stopServer();
    void communicate();
    void makeLog();
    void closeLog();
    QString readSocket( char* buffer, int buffer_size);
    int packsIn;
    int ratioPacks;

    int clientSocket;
    int secsTimeout;
    struct sockaddr_rc client_addr;
    unsigned int opt;
    int msgSize;
    int bytes_read;
    int nCharsCommand;
    QString btStart;
    QString btStop;
    QString btKill;
    QString btClock;
    QString btWait;
    char buf[18];
    char* bufmsg;
    QString msg;
    time_t servTime;
    std::ofstream oflog;
    QString lastTime;
    QString fName;
    bool gotTime;
    bool started;
    bool stopped;
    Daq* daq;
    QThread * daqThread;

private slots:
    void getBuffer(DataOut&);
    void getMsgDaq(QString);
signals:
    void daqStartContinuous(QString);
    void daqStopContinuous();

};
//! [declaration]

//const int bufsize = 1024;
