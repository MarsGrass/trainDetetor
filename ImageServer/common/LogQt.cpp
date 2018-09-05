/*
* =====================================================================================
*   
*   文 件 名：LogQt.cpp
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
#include "LogQt.h"

QTextCodec *codec=QTextCodec::codecForLocale();

CLogQt* CLogQt::m_instance = new CLogQt();

CLogQt::CLogQt()
{
	m_LogFile = NULL;
	m_LogStream = NULL;
}

CLogQt::~CLogQt()
{

}

CLogQt* CLogQt::GetLogQt()
{
	return m_instance;
}

void CLogQt::myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
	if (m_instance != NULL)
	{
		m_instance->MessageOutput(type, context, msg);
	}
}

void CLogQt::MessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
	m_LogMutex.lock();
	try
	{
		// 防止程序运行跨越零点 2014-8-28
		QString date = QDateTime::currentDateTime().toString("yyyyMMdd");
		if (date != m_CurrentDate)
		{
			m_CurrentDate = date;
			if (m_LogFile)
			{
				m_LogFile->close();
				delete m_LogStream;
				delete m_LogFile;
			}

			QString sDir=QCoreApplication::applicationDirPath() + "/log/";
			QString strFilename = sDir + QString("debuglog.%1.txt").arg(m_CurrentDate);
			QDir logDir(sDir);
			if(!logDir.exists()) logDir.mkdir(sDir);
			m_LogFile = new QFile(strFilename);
			m_LogFile->open(QIODevice::Text | QIODevice::Append);
			m_LogStream = new QTextStream(m_LogFile);
		}

		QString txt;
		QString time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.z");

		switch (type) {
		 case QtDebugMsg:
			 //txt = QObject::tr("[Debug %1] (%2:%3, %4) %5").arg(time).arg(context.file).arg(context.line).arg(context.function).arg(msg.toLatin1().data());
			 txt = QObject::tr("[Debug %1] %2").arg(time).arg(msg.toLatin1().data());
			 break;
		 case QtWarningMsg:
			 txt = QObject::tr("[Warning %1] (%2:%3, %4) %5").arg(time).arg(context.file).arg(context.line).arg(context.function).arg(msg.toLatin1().data());
			 break;
		 case QtCriticalMsg:
			 txt = QObject::tr("[Critical %1] (%2:%3, %4) %5").arg(time).arg(context.file).arg(context.line).arg(context.function).arg(msg.toLatin1().data());
			 break;
		 case QtFatalMsg:
			 txt = QObject::tr("[Fatal %1] (%2:%3, %4) %5").arg(time).arg(context.file).arg(context.line).arg(context.function).arg(msg.toLatin1().data());
		}


		if (m_LogStream)
		{
			*m_LogStream << txt << endl;
			m_LogStream->flush();
		}

	}
	catch(...)
	{
	}
	if (type == QtFatalMsg)
		abort();

	m_LogMutex.unlock();
}