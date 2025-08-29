#include <QCoreApplication>
#include "ServerManager.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    ServerManager server;
    if (server.startServer(8080)) {
        qDebug() << "医疗管理系统服务器启动成功";
    }

    return app.exec();
}
