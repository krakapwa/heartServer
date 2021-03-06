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

#include "server.h"
#include "daqADS1298.h"
#include "daqMPU6000.h"
#include "daq.h"

#include <qbluetoothlocaldevice.h>
static const QLatin1String serviceUuid("e8e10f95-1a70-4b27-9ccf-02010264e9c7");
static const QString escChar = "\n";

void* pThisCallback = NULL;

Server::Server(QObject *parent)
    : QObject(parent)

{
    sendBtRatio = 4; // ratio of samples to send via bluetooth (downsampling)
    sampleCount = 0;

    qDebug() <<  "Calling wiringPiSetupGpio()";
    wiringPiSetupGpio(); //init SPI pins

    setupRdyLed(RDYLED);
   rdyLedOff(RDYLED);

    rootPath = "/home/pi/heartServer/";
    syncUsb = new QProcess();
    syncUsb->setProcessChannelMode(QProcess::MergedChannels);
    QObject::connect(syncUsb,SIGNAL(readyRead()),this,SLOT(onReadyReadProcess()));


    started = false;

    datastream = new QDataStream(&byteArrayIn,QIODevice::WriteOnly);

    //Testing QBluetooth
 localAdapters = QBluetoothLocalDevice::allDevices();
 QBluetoothAddress localAdapterAddress = localAdapters.value(0).address();
 qDebug() << "Address of local bluetooth adapter:" + localAdapterAddress.toString();
 qDebug() << "Test";
 QBluetoothLocalDevice localAdapter(localAdapters.value(0).address());
 QBluetoothAddress clientAddress("60:D8:19:AF:11:04") ;
 localAdapter.pairingStatus(clientAddress);
 qDebug() << "PairingStatus:" +  QString::number(localAdapter.pairingStatus(clientAddress));

   rfcommServer = new QBluetoothServer(QBluetoothServiceInfo::RfcommProtocol, this);
   connect(rfcommServer, SIGNAL(newConnection()), this, SLOT(clientConnected()));
   bool result = rfcommServer->listen(localAdapterAddress);
   if (!result) {
       qDebug() << "Cannot bind chat server to" << localAdapterAddress.toString();
       //return;
   }
   else{

       qDebug() << "Successfully bound chat server to" << localAdapterAddress.toString();
   }

   //! [Create the server]

   //serviceInfo.setAttribute(QBluetoothServiceInfo::ServiceRecordHandle, (uint)0x00010010);

   //! [Class Uuuid must contain at least 1 entry]
   QBluetoothServiceInfo::Sequence classId;

   classId << QVariant::fromValue(QBluetoothUuid(QBluetoothUuid::SerialPort));
   serviceInfo.setAttribute(QBluetoothServiceInfo::BluetoothProfileDescriptorList,
                            classId);

   classId.prepend(QVariant::fromValue(QBluetoothUuid(serviceUuid)));

   serviceInfo.setAttribute(QBluetoothServiceInfo::ServiceClassIds, classId);
   serviceInfo.setAttribute(QBluetoothServiceInfo::BluetoothProfileDescriptorList,classId);
   //! [Class Uuuid must contain at least 1 entry]


   //! [Service name, description and provider]
   serviceInfo.setAttribute(QBluetoothServiceInfo::ServiceName, tr("heartKinetics Server"));
   serviceInfo.setAttribute(QBluetoothServiceInfo::ServiceDescription,
                            tr("heartKinetics Server service description"));
   serviceInfo.setAttribute(QBluetoothServiceInfo::ServiceProvider, tr("lphys.ulb.ac.be"));
   //! [Service name, description and provider]

   //! [Service UUID set]
   serviceInfo.setServiceUuid(QBluetoothUuid(serviceUuid));
   //! [Service UUID set]

   //! [Service Discoverability]
   QBluetoothServiceInfo::Sequence publicBrowse;
   publicBrowse << QVariant::fromValue(QBluetoothUuid(QBluetoothUuid::PublicBrowseGroup));
   serviceInfo.setAttribute(QBluetoothServiceInfo::BrowseGroupList,
                            publicBrowse);
   //! [Service Discoverability]

   //! [Protocol descriptor list]
   QBluetoothServiceInfo::Sequence protocolDescriptorList;
   QBluetoothServiceInfo::Sequence protocol;
   protocol << QVariant::fromValue(QBluetoothUuid(QBluetoothUuid::L2cap));
   protocolDescriptorList.append(QVariant::fromValue(protocol));
   protocol.clear();
   protocol << QVariant::fromValue(QBluetoothUuid(QBluetoothUuid::Rfcomm))
            << QVariant::fromValue(quint8(rfcommServer->serverPort()));
   protocolDescriptorList.append(QVariant::fromValue(protocol));
   serviceInfo.setAttribute(QBluetoothServiceInfo::ProtocolDescriptorList,
                            protocolDescriptorList);
   //! [Protocol descriptor list]

   //! [Register service]
   serviceInfo.registerService(localAdapterAddress);
   //! [Register service]

   if(serviceInfo.isComplete())
       qDebug() << "Bluetooth service is complete";

   if(serviceInfo.isRegistered())
       qDebug() << "Bluetooth service is registered";

    //Create ADS1298 daq objects and corresponding threads
    DaqADS1298* myDaqADS1298 = new DaqADS1298;
    QObject::connect(this, SIGNAL(daqStartContinuous()),
            myDaqADS1298, SLOT(startContinuous()));
    QObject::connect(this, SIGNAL(daqStopContinuous()),
             myDaqADS1298, SLOT(stopContinuous()));
    myDaqADS1298->setCfgFileName(rootPath + "configADS1298.txt"); //Acquisition triggered on ADS1298 DRDY

    //Create MPU6000 #1
    DaqMPU6000* myDaqMPU6000_1 = new DaqMPU6000;
    QObject::connect(this, SIGNAL(daqStartContinuous(QString)),
            myDaqMPU6000_1, SLOT(startContinuous(QString)));
    //QObject::connect(this, SIGNAL(daqStopContinuous()),
             //myDaqMPU6000_1, SLOT(stopContinuous()));
    //myDaqMPU6000->setCfgFileName("configMPU6000.txt"); //Acquisition triggered on ADS1298 DRDY
    //myDaqMPU6000->setFclk(1000000); //Acquisition triggered on ADS1298 DRDY
    myDaqMPU6000_1->setFclk(1000000); //Acquisition triggered on ADS1298 DRDY
    myDaqMPU6000_1->setNCsPin(7);
    myDaqMPU6000_1->setMosiPin(myDaqADS1298->getMosiPin());
    myDaqMPU6000_1->setMisoPin(myDaqADS1298->getMisoPin());
    myDaqMPU6000_1->setSclkPin(myDaqADS1298->getSclkPin());
    myDaqMPU6000_1->setChan(1);

    //Create MPU6000 #2
    DaqMPU6000* myDaqMPU6000_2 = new DaqMPU6000;
    //QObject::connect(this, SIGNAL(daqStartContinuous(QString)),
    //        myDaqMPU6000_2, SLOT(startContinuous(QString)));
    //QObject::connect(this, SIGNAL(daqStopContinuous()),
    //         myDaqMPU6000_2, SLOT(stopContinuous()));
    //myDaqMPU6000->setCfgFileName("configMPU6000.txt"); //Acquisition triggered on ADS1298 DRDY
    //myDaqMPU6000->setFclk(1000000); //Acquisition triggered on ADS1298 DRDY
    myDaqMPU6000_2->setFclk(1000000); //Acquisition triggered on ADS1298 DRDY
    myDaqMPU6000_2->setNCsPin(20);
    myDaqMPU6000_2->setMosiPin(myDaqADS1298->getMosiPin());
    myDaqMPU6000_2->setMisoPin(myDaqADS1298->getMisoPin());
    myDaqMPU6000_2->setSclkPin(myDaqADS1298->getSclkPin());
    myDaqMPU6000_2->setChan(1);

    //Add to daq list
    daqs.append(myDaqADS1298);
    daqs.append(myDaqMPU6000_1);
    daqs.append(myDaqMPU6000_2);

    //init BT data output buffer
    initBtBuffOut();

    //Setup daqs
    //qDebug() << "ADS1298 sampling freq: " + QString::number(daqs[0]->getFs());
    daqs[0]->setup();
    daqs[1]->setup();
    daqs[2]->setup();


    wiringPiISRargs(myDaqADS1298->getDrdyPin(), INT_EDGE_FALLING,  &Server::getData,this);
    pThisCallback = this;

    rdyLedOn(RDYLED);

    /*
    delay(1000);
    startContinuous("lol.bin");
    delay(2000);
    stopContinuous();
    //quint8 test[27] = {0};
    //qDebug() << QString::number(sizeof(test));
    */


}

void Server::startContinuous(QString fname){

    QString fnamePath;
    fnamePath = rootPath + fname;
    qDebug() << "opening file " + fnamePath;
    myFile.open(fnamePath.toStdString(), std::ios::out | std::ios::binary);
    emit daqStartContinuous();
}

void Server::stopContinuous(){
    delay(10);
    emit daqStopContinuous();

    delay(1000);
    qDebug() << "Closing file";
    myFile.close();
}

void Server::getData(void){
    Server * p = static_cast<Server *>(pThisCallback);
    std::vector<int32_t>* buff;

    for(int i=0; i < p->daqs.size();++i){
        buff = p->daqs[i]->getData();
        p->myFile.write((char*)buff->data(), buff->size()*sizeof(int32_t));
        if(p->sampleCount == p->sendBtRatio){
            //qDebug() << "cat vectors";
            p->buffBt.insert(p->buffBt.end(),buff->begin(),buff->end());
        }
    }
    if(p->sampleCount == p->sendBtRatio){

        p->sendData(&(p->buffBt));
        p->sampleCount = 0;
        p->buffBt.clear();

    }
    else{
        p->sampleCount = p->sampleCount + 1;
        //qDebug() << "p->sampleCount = " + QString::number(p->sampleCount);
    }
}

void Server::sendData(std::vector<int32_t>* data)
{
    std::stringstream outstream;
    outstream << "AAAA";

    for (std::vector<int32_t>::iterator it = data->begin(); it!=data->end(); ++it) {
       outstream << std::to_string(*it) << ",";

        //qDebug() << QString::fromUtf8(std::to_string(*it).c_str());
    }
    outstream << '\n';

    //qDebug() << QString::fromUtf8(outstream.str().c_str());

    foreach (QBluetoothSocket *socket, clientSockets)
        socket->write(outstream.str().c_str());
}

Server::~Server()
{
    stopServer();
}

void Server::getMsgDaq(QString msg){
   qDebug() << msg;
}

/*
void Server::setDaq(Daq& daqIn){
    daqs.append(&daqIn);
}
*/

void Server::onReadyReadProcess(){
    //qDebug() << "Reading:" << syncUsb->readAllStandardOutput();

    sendMessage(QString::fromUtf8(syncUsb->readAllStandardOutput()));
}

void Server::clientConnected()
{
    QBluetoothSocket *socket = rfcommServer->nextPendingConnection();
    if (!socket)
        return;

    connect(socket, SIGNAL(readyRead()), this, SLOT(readSocket()));
    connect(socket, SIGNAL(disconnected()), this, SLOT(clientDisconnected()));
    clientSockets.append(socket);
    QString msg;
    msg ="Accepted connection. Status: ";
    if(started)
        msg = msg + "Running.";
    else msg = msg + "Waiting.";

    sendMessage(msg);

}

//! [clientDisconnected]
void Server::clientDisconnected()
{
    QBluetoothSocket *socket = qobject_cast<QBluetoothSocket *>(sender());
    if (!socket)
        return;

    //emit clientDisconnected(socket->peerName());
    qDebug() << "client disconnected";

    clientSockets.removeOne(socket);

    //socket->deleteLater();
}


void Server::readSocket()
{
    QBluetoothSocket *socket = qobject_cast<QBluetoothSocket *>(sender());
    /*
    if (!socket)
        return;
        */

    while (socket->canReadLine()) {
        QByteArray line = socket->readLine().trimmed();
        QString rcvdMessage = QString::fromUtf8(line.constData() ,  line.length());
        qDebug() << socket->peerName() + " " + rcvdMessage;
        processMessage(rcvdMessage);
    }
}


void Server::sendMessage(const QString &message)
{
    std::stringstream outstream;
    outstream << "BBBB";
    outstream << message.toStdString();
    outstream << '\n';

    foreach (QBluetoothSocket *socket, clientSockets)
        socket->write(outstream.str().c_str());

    /*
    //qDebug() << "sending " + message;
    QByteArray text;
    text +=QByteArray::fromHex("BBBB");
    text += message.toUtf8() + '\n';
    qDebug() << "sending(hex) " + text.toHex();
    foreach (QBluetoothSocket *socket, clientSockets)
        socket->write(text);
        */
}

struct packOut
{
   char  type[1];
   char  data[sizeof(uint8_t)*27];
};


void Server::processMessage(const QString& msg)
{

    if(msg.contains("startStop",Qt::CaseInsensitive) ){
        qDebug() << "got startStop command";
        if(started==false){
            QRegularExpression rx("(\\d{1,2}-\\d{1,2}-\\d{1,4}_\\d{1,2}-\\d{1,2}-\\d{1,2})");
            QRegularExpressionMatch match = rx.match(msg);
            if (match.hasMatch()) {
                qDebug() << "will start";
                QString matched = match.captured(0);
                QString msg = "Starting acquisition on " + matched;
                fName = "rpiData_" + matched;
                startContinuous(fName);
                sendMessage(msg);
                started = true;
                return;
            }
        }
        if(started == true){
            qDebug() << "will stop";
            QString msg = "Stopping acquisition";
            sendMessage(msg);
            //emit daqStopContinuous();
            stopContinuous();
            started = false;
            return;
        }

    }
    if(msg.contains("sync",Qt::CaseInsensitive) ){

        qDebug() << "got sync command";
        if(started==false){
            syncUsb->start("/bin/bash", QStringList() << "/home/pi/heartServer/syncUsb");
        }
        else{
            sendMessage("There is an acquisition running. Click start/stop before syncing.");
        }
    }
}
void Server::stopServer()
{
    // Unregister service
    //serviceInfo.unregisterService();

    // Close sockets
    //qDeleteAll(clientSocket);

    // Close server
}


void Server::setupRdyLed(int pin){
    pinMode(pin, OUTPUT); //ADS1298_START
    pullUpDnControl (pin, PUD_OFF);
}

void Server::rdyLedOn(int pin){
    digitalWrite(pin,HIGH);
}

void Server::rdyLedOff(int pin){
    digitalWrite(pin,LOW);
}


void Server::initBtBuffOut(){
    int totalChans = 0;
   for(int i=0; i<daqs.size();++i){
       totalChans +=daqs[i]->getNchans();
   }
   qDebug() << "Total channels in output BT buffer:" + QString::number(totalChans);
   buffBt.reserve(totalChans);

}
