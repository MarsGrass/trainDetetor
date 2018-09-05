/*
* =====================================================================================
*   
*   文 件 名：LogQt.h
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
#include <QtCore>
//#include <QApplication>

class CLogQt
{
public:
	static CLogQt* GetLogQt();

	static void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg);

private:
	CLogQt();
	~CLogQt();

	void MessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg);

private:
	static CLogQt* m_instance;

	QFile	*m_LogFile;
	QTextStream	*m_LogStream;
	QMutex	m_LogMutex;
	QString m_CurrentDate;
};
