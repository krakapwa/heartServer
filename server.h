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
#ifndef SERVER_H
#define SERVER_H

#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>
#include <time.h>
#include <errno.h>
#include <QtCore/QObject>
#include <QDebug>
#include <QTimer>
#include <QList>
#include <QThread>
#include <qbluetoothserviceinfo.h>
#include <qbluetoothhostinfo.h>
#include <qbluetoothserver.h>
#include <QRegularExpression>
#include "data.h"
#include <unistd.h>
#include <QProcess>
#include <QString>
#include <QDataStream>
#include "daq.h"
#include <sstream>

QT_USE_NAMESPACE

class Daq;

const int RDYLED = 21;

class Server: public QObject
{
    Q_OBJECT

public:
    Server(QObject *parent=0);
    Server(Daq& daqIn);
    ~Server();
    void getDataADS1298();
    std::ofstream myFile;

    QList<Daq*> daqs;
  QList<QBluetoothHostInfo> localAdapters;
  QBluetoothServer *rfcommServer;
  QBluetoothServiceInfo serviceInfo;

private:
    QString rootPath;
    void setupRdyLed(int pin);
    void rdyLedOn(int pin);
    void rdyLedOff(int pin);
    void sendMessage(const QString &message);
    void sendData(std::vector<int32_t> *);
    void showMessage(const QString &sender, const QString &message);
    void connected(const QString &name);
    //static void getData();
    static void getData();
    static void getWriteData(std::ofstream *, int, int, int);
    std::vector<int32_t> buffBt;

    void startServer();
    void restartServer();
    void stopServer();
    void processMessage(const QString&);

    void startContinuous(QString);
    void stopContinuous();
    void initBtBuffOut();

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
    QString lastTime;
    QString fName;
    bool gotTime;
    bool started;
    uint8_t * btDataOut;

    QList<QBluetoothSocket *> clientSockets;

    QByteArray type;
    QByteArray byteArrayIn;
    QDataStream* datastream;
    int sampleCount; //keeps track of sent samples for downsampling
    int sendBtRatio; //Ratio of acquired data samples to send

    quint8 bufferADS1298[27];
    QByteArray bufferADS1298ar;
    uint8_t bufferMPU6000H[1];
    uint8_t bufferMPU6000L[1];
    QProcess *syncUsb;


private slots:
    void getMsgDaq(QString);
    void clientConnected();
    void clientDisconnected();
    void readSocket();
    void onReadyReadProcess();

signals:
    void messageReceived(const QString &sender, const QString &message);
    void clientConnected(const QString &name);
    void clientDisconnected(const QString &name);
    void daqStartContinuous();
    void daqStartSingleShot();
    void daqStopContinuous();

};
//! [declaration]

#endif
