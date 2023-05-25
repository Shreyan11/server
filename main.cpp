#include <QCoreApplication>
#include <QTcpServer>
#include <QTcpSocket>
#include <QDebug>
#include <QStringList>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    int port = 8012;

    if (argc > 1) {
        bool ok;
        port = QString(argv[1]).toInt(&ok);
        if (!ok) {
            qDebug() << "Invalid port number:" << argv[1];
            return 1;
        }
    }

    QTcpServer server;
    if (!server.listen(QHostAddress::Any, port)) {
        qDebug() << "Server could not start!";
        return 1;
    }

    qDebug() << "Server started on port" << server.serverPort();

    QObject::connect(&server, &QTcpServer::newConnection, [&](){
        QTcpSocket *socket = server.nextPendingConnection();
        qDebug() << "Client connected from" << socket->peerAddress().toString() << ":" << socket->peerPort();
        QObject::connect(socket, &QTcpSocket::readyRead, [&](){
            QByteArray data = socket->readAll();
            qDebug() << "Received data:" << data;
        });
        socket->write("Hello World!");
        socket->flush();
        socket->waitForBytesWritten();
        socket->close();
        qDebug() << "Client disconnected";
    });

    return a.exec();
}
