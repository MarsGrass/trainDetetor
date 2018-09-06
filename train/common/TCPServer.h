/*
* =====================================================================================
*   
*   文 件 名：TCPServer.h
*
*   描    述：定义
*
*   创建时间：
*
*   更新历史：
*
*   版权所有：
* 
* =====================================================================================
*/
#pragma once
#include <QTcpServer>
#include <QTcpSocket>
#include <QMutex>

#define NET_DEFAULT_CONNECT_MAX 1

class TCPServer;
class CDataHandle;

class NetConnect : public QObject
{
	Q_OBJECT

public:
	NetConnect();
	~NetConnect();

public:
	void InitConnect(QTcpSocket *pSocket, int nSocketId, TCPServer *pServer);
	void DeleteConnect();

    QByteArray getData();
	bool sendData(QByteArray &data);

public slots:
	void OnReadReady();

public:
	int m_nSocketId; //socked ID，空闲为-1
	QTcpSocket *m_pSocket;
	TCPServer *m_pServer;
};

class TCPServer : public QObject
{
	Q_OBJECT

public:
	TCPServer(QObject * parent = 0, int nMaxConnect = NET_DEFAULT_CONNECT_MAX);
	~TCPServer(void);

	void SetConfig(QString ip, QString port);

	bool GetOpenStatus();

    QStringList GetClientList();

    int GetIndexByStringName(QString strName);

signals:
    //第一个参数0表示客户端下线，1表示客户端上下，2表示客户端有数据过来
    //第二个参数表示第几个客户端，从0开始计数
    void ReportStatusSignals(int, int);

public slots:
	void OpenSlots();

	void CloseSlots();

    void SendDataSlots(QByteArray data, int nIndexClient);

	//有新链接过来
	void OnNewConnect();

	//断开新链接
	void OnDisConnect();

private:
	QString m_ip;
	int		m_port;
	int m_nMaxConnect;
    bool m_bServerListening;

	QMutex m_ConnectMutex;
	int m_nNumId;

	QTcpServer* m_pServer;

public:
    QList<NetConnect*> m_listConnect;
};
