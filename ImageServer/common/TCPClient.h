/*
* =====================================================================================
*   
*   文 件 名：TCPClient.h
*
*   描    述：TCP客户端模式
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
#include <QTcpSocket>

class TCPClient : public QObject
{
	Q_OBJECT

public:
	TCPClient(QObject * parent = 0);
	~TCPClient();

	void SetConfig(QString ip, QString port);

	bool GetOpenStatus();

    QByteArray GetReadData();

signals:
	void ReportStatusSignals(int);

public slots:
	void OpenSlots();

	void CloseSlots();

	void SendDataSlots(QByteArray data);


public slots:  
	void OnSocketError(QAbstractSocket::SocketError);  
	void OnSocketConnected();  
	void OnSocketDisconnected();    
	void OnReceiveData();	

private:
	QString m_ip;
	int		m_port;

	QByteArray m_data;
	bool m_bServerConnected;  

private:
	QTcpSocket* m_socket;
	//QTimer m_timer;  
};
