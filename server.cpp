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
static QList<Daq*> daqs;
static const QString escChar = "\n";

void* pThisCallback = NULL;

Server::Server(QObject *parent)
    : QObject(parent)

{
    qDebug() <<  "Calling wiringPiSetupGpio()";
    wiringPiSetupGpio(); //init SPI pins

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
       return;
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
    QObject::connect(this, SIGNAL(daqStartContinuous(QString)),
            myDaqADS1298, SLOT(startContinuous(QString)));
    QObject::connect(this, SIGNAL(daqStopContinuous()),
             myDaqADS1298, SLOT(stopContinuous()));
    myDaqADS1298->setCfgFileName("configADS1298.txt"); //Acquisition triggered on ADS1298 DRDY

    //Create MPU6000 daq objects and corresponding threads
    DaqMPU6000* myDaqMPU6000 = new DaqMPU6000;
    QObject::connect(this, SIGNAL(daqStartContinuous(QString)),
            myDaqMPU6000, SLOT(startContinuous(QString)));
    QObject::connect(this, SIGNAL(daqStopContinuous()),
             myDaqMPU6000, SLOT(stopContinuous()));
    myDaqMPU6000->setCfgFileName("configMPU6000.txt"); //Acquisition triggered on ADS1298 DRDY
    //myDaqMPU6000->setFclk(1000000); //Acquisition triggered on ADS1298 DRDY
    myDaqMPU6000->setFclk(1000000); //Acquisition triggered on ADS1298 DRDY
    myDaqMPU6000->setNCsPin(7);
    myDaqMPU6000->setMosiPin(myDaqADS1298->getMosiPin());
    myDaqMPU6000->setMisoPin(myDaqADS1298->getMisoPin());
    myDaqMPU6000->setFsyncPin(24);
    myDaqMPU6000->setSclkPin(myDaqADS1298->getSclkPin());
    myDaqMPU6000->setChan(0); //Acquisition triggered on ADS1298 DRDY

    //Add to daq list
    daqs.append(myDaqADS1298);
    daqs.append(myDaqMPU6000);

    //Setup daqs
    daqs[0]->setup();
    qDebug() << "ADS1298 sampling freq: " + QString::number(daqs[0]->getFs());
    //daqs[1]->setup();

    /*
    digitalWrite(7,LOW);
    delay(1000);
    digitalWrite(7,HIGH);
    */

    wiringPiISRargs(myDaqADS1298->getDrdyPin(), INT_EDGE_FALLING,  &Server::getData2,this) ;

    delay(100);
    //emit daqStartSingleShot("lol.bin");
    emit daqStartContinuous("lol.bin");
    delay(200);
    //pullUpDnControl(ADS1298_DRDY,PUD_DOWN);
    emit daqStopContinuous();
    //quint8 test[27] = {0};
    //qDebug() << QString::number(sizeof(test));

    sampleRatio = 4;

}


void Server::getData2(void){
    Server * p = static_cast<Server *>(pThisCallback);
    p->getDataADS1298();
}


void Server::getDataADS1298(){

    //int chan = daqs[0]
    uint8_t tmp[27] = {0};
    //    getWriteData(&(daqs[0]->myFile),8, 0, 27);
    digitalWrite(ADS1298_nCS,LOW);
    delayMicroseconds(1);
    wiringPiSPIDataRW(ADS1298_chan, tmp ,ADS1298_Nbytes);
    delayMicroseconds(1);
    digitalWrite(ADS1298_nCS,HIGH);


    for( int i=0; i < ADS1298_Nbytes; ++i ){
       bufferADS1298[i] =  tmp[i];
    }

    //qDebug() << QString::number(bufferADS1298.size());
    qDebug() << QString::number(tmp[20]);
    daqs[0]->myFile.write((char*)&bufferADS1298, ADS1298_Nbytes*sizeof(quint8));
    //bufferADS1298ar = QByteArray::fromRawData((char*)bufferADS1298,27*sizeof(qint8));
    sendData(bufferADS1298,ADS1298_Nbytes);
}

/*
void Server::getData(void){

    //int chan = daqs[0]
    uint8_t tmp[27] = {0};
    //    getWriteData(&(daqs[0]->myFile),8, 0, 27);
    digitalWrite(8,LOW);
    //delayMicroseconds(5);
    wiringPiSPIDataRW(0, tmp ,27);
    //delay(1);
    digitalWrite(8,HIGH);


    for( int i=0; i < 27; ++i ){
       bufferADS1298[i] =  tmp[i];
    }

    daqs[0]->myFile.write((char*)&bufferADS1298, 27*sizeof(uint8_t));

    //MPU6000
    uint8_t tmpSpiDataH[1];
    uint8_t tmpSpiDataL[1] = {0};
    int chan = daqs[1]->getChan();

    //x axis accel
    tmpSpiDataH[0] = MPUREG_ACCEL_XOUT_H | READ_FLAG;
    wiringPiSPIDataRW(chan, tmpSpiDataH , 1);
    wiringPiSPIDataRW(chan, tmpSpiDataH , 1);
    wiringPiSPIDataRW(chan, tmpSpiDataL , 1);
    bufferMPU6000H[0] = tmpSpiDataH[0];
    bufferMPU6000L[0] = tmpSpiDataL[0];
    daqs[1]->myFile.write((char*)&bufferMPU6000H, 1*sizeof(uint8_t));
    daqs[1]->myFile.write((char*)&bufferMPU6000L, 1*sizeof(uint8_t));

    //y axis accel
    tmpSpiDataH[0] = MPUREG_ACCEL_YOUT_H | READ_FLAG;
    wiringPiSPIDataRW(chan, tmpSpiDataH , 1);
    wiringPiSPIDataRW(chan, tmpSpiDataH , 1);
    wiringPiSPIDataRW(chan, tmpSpiDataL , 1);
    bufferMPU6000H[0] = tmpSpiDataH[0];
    bufferMPU6000L[0] = tmpSpiDataL[0];
    daqs[1]->myFile.write((char*)&bufferMPU6000H, 1*sizeof(uint8_t));
    daqs[1]->myFile.write((char*)&bufferMPU6000L, 1*sizeof(uint8_t));

    //z axis accel
    tmpSpiDataH[0] = MPUREG_ACCEL_ZOUT_H | READ_FLAG;
    wiringPiSPIDataRW(chan, tmpSpiDataH , 1);
    wiringPiSPIDataRW(chan, tmpSpiDataH , 1);
    wiringPiSPIDataRW(chan, tmpSpiDataL , 1);
    bufferMPU6000H[0] = tmpSpiDataH[0];
    bufferMPU6000L[0] = tmpSpiDataL[0];
    daqs[1]->myFile.write((char*)&bufferMPU6000H, 1*sizeof(uint8_t));
    daqs[1]->myFile.write((char*)&bufferMPU6000L, 1*sizeof(uint8_t));

    //x axis rot
    tmpSpiDataH[0] = MPUREG_GYRO_XOUT_H | READ_FLAG;
    wiringPiSPIDataRW(chan, tmpSpiDataH , 1);
    wiringPiSPIDataRW(chan, tmpSpiDataH , 1);
    wiringPiSPIDataRW(chan, tmpSpiDataL , 1);
    bufferMPU6000H[0] = tmpSpiDataH[0];
    bufferMPU6000L[0] = tmpSpiDataL[0];
    daqs[1]->myFile.write((char*)&bufferMPU6000H, 1*sizeof(uint8_t));
    daqs[1]->myFile.write((char*)&bufferMPU6000L, 1*sizeof(uint8_t));

    //y axis rot
    tmpSpiDataH[0] = MPUREG_GYRO_YOUT_H | READ_FLAG;
    wiringPiSPIDataRW(chan, tmpSpiDataH , 1);
    wiringPiSPIDataRW(chan, tmpSpiDataH , 1);
    wiringPiSPIDataRW(chan, tmpSpiDataL , 1);
    bufferMPU6000H[0] = tmpSpiDataH[0];
    bufferMPU6000L[0] = tmpSpiDataL[0];
    daqs[1]->myFile.write((char*)&bufferMPU6000H, 1*sizeof(uint8_t));
    daqs[1]->myFile.write((char*)&bufferMPU6000L, 1*sizeof(uint8_t));

    //z axis rot
    tmpSpiDataH[0] = MPUREG_GYRO_ZOUT_H | READ_FLAG;
    wiringPiSPIDataRW(chan, tmpSpiDataH , 1);
    wiringPiSPIDataRW(chan, tmpSpiDataH , 1);
    wiringPiSPIDataRW(chan, tmpSpiDataL , 1);
    bufferMPU6000H[0] = tmpSpiDataH[0];
    bufferMPU6000L[0] = tmpSpiDataL[0];
    daqs[1]->myFile.write((char*)&bufferMPU6000H, 1*sizeof(uint8_t));
    daqs[1]->myFile.write((char*)&bufferMPU6000L, 1*sizeof(uint8_t));

}
*/

//uint8_t bufferMPU6000[27];
/*
void Server::getWriteData(std::ofstream* file,int ncsPin, int chan, int len){

    digitalWrite(ncsPin,LOW);
    wiringPiSPIDataRW(chan, bufferADS1298 ,len);
    digitalWrite(ncsPin,HIGH);

    file->write((char*)&bufferADS1298, len*sizeof(uint8_t));
}
*/

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
    //msg ="Accepted connection. Status: ";
    if(started)
        msg = msg + "Running.";
    else msg = msg + "Waiting.";

    quint8 data[27] = {0};
    for(int i = 0; i<27;++i){
        data[i] = i;
    }
    sendData(data,27);

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
    qDebug() << "sending " + message;
    QByteArray text;
    text +=QByteArray::fromHex("BBBB");
    text += message.toUtf8() + '\n';
    qDebug() << "sending(hex) " + text.toHex();
    foreach (QBluetoothSocket *socket, clientSockets)
        socket->write(text);
}

struct packOut
{
   char  type[1];
   char  data[sizeof(uint8_t)*27];
};

void Server::sendData(quint8 data[], int len)
{

    if(sampleCount < 4) {
        datastream->device()->seek(0);
        type = QByteArray::fromHex("AAAA");
        datastream->setVersion(QDataStream::Qt_5_3);
        datastream->writeRawData(type,2);

        for(int i = 0; i<len;++i){
        *datastream << data[i];
        }
        *datastream << '\n';

        //qDebug() << QString::number(byteArrayIn.size());
        foreach (QBluetoothSocket *socket, clientSockets)
        socket->write(byteArrayIn);
        sampleCount++;
    }
    else{
        sampleCount = 0;
    }
}

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
                emit daqStartContinuous(fName);
                sendMessage(msg);
                started = true;
                return;
            }
        }
        if(started == true){
            qDebug() << "will stop";
            QString msg = "Stopping acquisition";
            sendMessage(msg);
            emit daqStopContinuous();
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

