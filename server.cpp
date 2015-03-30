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
void* pThisCallback = NULL;

Server::Server(QObject *parent)
    : QObject(parent)

{

    started = false;

    //Testing QBluetooth
 localAdapters = QBluetoothLocalDevice::allDevices();
 QBluetoothAddress localAdapterAddress = localAdapters.at(0).address();
 qDebug() << "Address of local bluetooth adapter:" + localAdapterAddress.toString();
 QBluetoothLocalDevice localAdapter(localAdapters.at(0).address());
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
    QThread* myDaqThreadADS1298 = new QThread;
    QObject::connect(this, SIGNAL(daqStartContinuous(QString)),
            myDaqADS1298, SLOT(startContinuous(QString)));
    QObject::connect(this, SIGNAL(daqStopContinuous()),
             myDaqADS1298, SLOT(stopContinuous()));
    myDaqADS1298->setCfgFileName("configADS1298.txt"); //Acquisition triggered on ADS1298 DRDY
    myDaqADS1298->moveToThread(myDaqThreadADS1298);
    myDaqThreadADS1298->start(); //Starting thread (not acquisition)

    /*
    //Create MPU6000 daq objects and corresponding threads
    DaqMPU6000* myDaqMPU6000 = new DaqMPU6000;
    QThread* myDaqThreadMPU6000 = new QThread;
    QObject::connect(this, SIGNAL(daqStartContinuous(QString)),
            myDaqMPU6000, SLOT(startContinuous(QString)));
    QObject::connect(this, SIGNAL(daqStopContinuous()),
             myDaqMPU6000, SLOT(stopContinuous()));
    myDaqMPU6000->setDrdyPin(myDaqADS1298->getDrdyPin()); //Acquisition triggered on ADS1298 DRDY
    myDaqMPU6000->setCfgFileName("configMPU6000.txt"); //Acquisition triggered on ADS1298 DRDY
    myDaqMPU6000->moveToThread(myDaqThreadMPU6000);
    myDaqThreadMPU6000->start(); //Starting thread (not acquisition)
    */


    //Add to daq list
    setDaq(*myDaqADS1298, *myDaqThreadADS1298);
    //setDaq(*myDaqMPU6000, *myDaqThreadMPU6000);

    //Setup daqs
    qDebug() << "Setting up daqs";
    for (int i = 0; i < daqs.size(); ++i) {
        daqs[i]->setup();
    }

    //Setup interrupt on DRDY pin of ADS1298. Will trigger acquisitions on other daqs as well.
    wiringPiISRargs(myDaqADS1298->getDrdyPin(), INT_EDGE_FALLING,  &Server::getData,this) ;

    //For testing, start automatically
    //processMessage("startStop 00-00-00_00-00-00");
    //delay(3000);
    //processMessage("startStop 00-00-00_00-00-00");
}

void Server::getData(void){
    Server* pThisCallbackCast = static_cast<Server*>(pThisCallback);
    //pThisCallbackCast->getData();

    QList<Daq*> myDaqs = pThisCallbackCast->daqs;
    for( int i=0; i<myDaqs.count(); ++i ){
        myDaqs[i]->getData();
    }
}

Server::~Server()
{
    stopServer();
}

void Server::getMsgDaq(QString msg){
   qDebug() << msg;
}

void Server::setDaq(Daq& daqIn, QThread& daqInThread){
    daqs.append(&daqIn);
    daqThreads.append(&daqInThread);
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
    if (!socket)
        return;

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
    QByteArray text = message.toUtf8() + '\n';
    foreach (QBluetoothSocket *socket, clientSockets)
        socket->write(text);
}


void Server::processMessage(const QString& msg)
{

    if(msg.contains("startStop",Qt::CaseInsensitive) ){

        if(started==false){
            QRegularExpression rx("(\\d{1,2}-\\d{1,2}-\\d{1,2}_\\d{1,2}-\\d{1,2}-\\d{1,2})");
            QRegularExpressionMatch match = rx.match(msg);
            if (match.hasMatch()) {
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
            QString msg = "Stopping acquisition ";
            sendMessage(msg);
            emit daqStopContinuous();
            started = false;
            return;
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

