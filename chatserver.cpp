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

#include "chatserver.h"

#include <qbluetoothserver.h>
#include <qbluetoothsocket.h>
#include <qbluetoothlocaldevice.h>

//! [Service UUID]
static const QLatin1String serviceUuid("e8e10f95-1a70-4b27-9ccf-02010264e9c8");
//static const QLatin1String serviceUuid("00001101-0000-1000-8000-00805F9B34FB");
//static const QLatin1String serviceUuid("e8e10f91-1a70-4b27-9ccf-32010264b3c5");

//! [Service UUID]

ChatServer::ChatServer(QObject *parent)
:   QObject(parent), rfcommServer(0)
{
    while(1){
            bzero(msg,msgSize);
            bytes_read = read(client, msg, sizeof(msg));

}

ChatServer::~ChatServer()
{
    stopServer();
}

void ChatServer::startServer(int sockfd)
{
    if (rfcommServer)
        return;

    //! [Create the server]
    //rfcommServer = new QBluetoothServer(QBluetoothServiceInfo::RfcommProtocol, this);
    qDebug() << "Listening for incoming connection...";
    listen(sockfd,1);
   // if (!result) {
   //     qWarning() << "Cannot bind chat server";
   //     return;
   // }
    //! [Create the server]
    clientConnected(sockfd);

}

//! [stopServer]
void ChatServer::stopServer()
{
    // Unregister service
    //serviceInfo.unregisterService();

    // Close sockets
    //qDeleteAll(clientSocket);

    // Close server
    delete rfcommServer;
    rfcommServer = 0;
}
//! [stopServer]

//! [sendMessage]
void ChatServer::sendMessage(const QString &message)
{
    QByteArray text = message.toUtf8() + '\n';

    //foreach (QBluetoothSocket *socket, clientSockets)
        //socket->write(text);
}
//! [sendMessage]

//! [clientConnected]
void ChatServer::clientConnected(int &sockfd)
{
    if (!socket)
        return;

    clientSocket=sockfd;

}
//! [clientConnected]

//! [readSocket]
void ChatServer::readSocket()
{
    if (!clientSocket)
        return;

   // while (socket->canReadLine()) {
   //     QByteArray line = socket->readLine().trimmed();
   //     emit messageReceived(socket->peerName(),
   //                          QString::fromUtf8(line.constData(), line.length()));
   // }
}
//! [readSocket]
