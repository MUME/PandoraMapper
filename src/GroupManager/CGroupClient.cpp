/*
 *  Pandora MUME mapper
 *
 *  Copyright (C) 2000-2009  Azazello
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 2
 *  of the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include <QHostAddress>

#include "utils.h"

#include "GroupManager/CGroupClient.h"
#include "GroupManager/CGroupCommunicator.h"


void CGroupClient::linkSignals()
{
	connect(this, SIGNAL(disconnected()), this, SLOT(lostConnection() ) );
	connect(this, SIGNAL(connected()), this, SLOT(connectionEstablished() ) );

	connect(this, SIGNAL(error(QAbstractSocket::SocketError )), this, SLOT(errorHandler(QAbstractSocket::SocketError) ) );
	connect(this, SIGNAL(readyRead()), this, SLOT( dataIncoming() )   );

	buffer = "";
	currentMessageLen = 0;
}

CGroupClient::CGroupClient(QByteArray host, int remotePort, QObject *parent) :
	QTcpSocket(parent)
{
	print_debug(DEBUG_GROUP, "Connecting to remote host...");
	setConnectionState(Connecting);
	print_debug(DEBUG_GROUP, "NOW issuing the connect command...");
	connectToHost(host, remotePort);
	protocolState = AwaitingLogin;

	linkSignals();

	if (!waitForConnected(5000)) {
		connectionState = CGroupClient::Quiting;
	    close();
	    print_debug(DEBUG_GROUP, "Server not responding!");
	    errorHandler(QAbstractSocket::ConnectionRefusedError);
	    //getParent()->changeType(CGroupCommunicator::Off);
	    return;
	}
}

CGroupClient::CGroupClient(QObject *parent) :
	QTcpSocket(parent)
{
	connectionState = Closed;

	linkSignals();
	protocolState = Idle;
}


void CGroupClient::setSocket(qintptr socketDescriptor)
{
	if (setSocketDescriptor(socketDescriptor) == false) {
		// failure ... what to do?
		print_debug(DEBUG_GROUP, "Connection failed. Native socket not recognized by CGroupClient.");
	}

	setConnectionState(Connected);
}

void CGroupClient::setProtocolState(int val)
{
	print_debug(DEBUG_GROUP, "Protocol state: %i", val);
	protocolState = val;
}

void CGroupClient::setConnectionState(int val)
{
	print_debug(DEBUG_GROUP, "Connection state: %i", val);
	connectionState = val;
	getParent()->connectionStateChanged(this);
}


CGroupClient::~CGroupClient()
{
	disconnect();
	this->deleteLater();
}

void CGroupClient::lostConnection()
{
	setConnectionState(Closed);
}

void CGroupClient::connectionEstablished()
{
	setConnectionState(Connected);
}

void CGroupClient::errorHandler ( QAbstractSocket::SocketError socketError )
{
	CGroupCommunicator *comm = (CGroupCommunicator *)parent();
	comm->errorInConnection(this);
}

void CGroupClient::dataIncoming()
{
	QByteArray message;
	QByteArray rest;

//	print_debug(DEBUG_GROUP, "Incoming Data [conn %i, IP: %s]", socketDescriptor(),
//			(const char *) peerAddress().toString().toLocal8Bit() );

	QByteArray tmp = readAll();

	buffer += tmp;

//	print_debug(DEBUG_GROUP, "RAW data buffer: %s", (const char *) buffer);

	while ( currentMessageLen < buffer.size()) {
//		print_debug(DEBUG_GROUP, "in data-receiving cycle, buffer %s", (const char *) buffer);
		cutMessageFromBuffer();
	}
}

void CGroupClient::cutMessageFromBuffer()
{
	QByteArray rest;

	if (currentMessageLen == 0) {
		int index = buffer.indexOf(' ');

		if (index == -1) {
			print_debug(DEBUG_GROUP, "Incoming buffer contains broken message");
			// zap the buffer in this case, and hope for the best
			buffer.clear();
			return;
		}

		QByteArray len = buffer.left(index);
		currentMessageLen = len.toInt();
//		print_debug(DEBUG_GROUP, "Incoming buffer length: %i, incoming message length %i",
//				buffer.size(), currentMessageLen);

		rest = buffer.right( buffer.size() - index - 1);
		buffer = rest;

		if (buffer.size() == currentMessageLen)
			cutMessageFromBuffer();

		return;
	}

//	print_debug(DEBUG_GROUP, "cutting off one message case");
	getParent()->incomingData(this, buffer.left(currentMessageLen));
	rest = buffer.right( buffer.size() - currentMessageLen);
	buffer = rest;
	currentMessageLen = 0;
}


void CGroupClient::sendData(QByteArray data)
{
	QByteArray buff;
	QString len;

	len = QString("%1 ").arg(data.size());

//	char len[10];

//	sprintf(len, "%i ", data.size());

	buff = len.toLocal8Bit();
	buff += data;

	write(buff);
}


