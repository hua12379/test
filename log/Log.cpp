#include "Log.h"
#include <QFile>

QMutex mutex; //设置互斥锁
int maxFileSize; //单个日志文件最大值，单位为MB
int maxSaveDate; //最大保存日期，单位：天
QString filePath; //日志存储路径
bool includeDebug; //是否记录debug日志

QDateTime lastDeleteTime;

Log::Log(QObject *parent)
    : QObject{parent}
{
    //设置日志输出格式，release模式下需在.pro文件加DEFINES += QT_MESSAGELOGCONTEXT
    qSetMessagePattern("%{time yyyy-MM-dd hh:mm:ss} [%{type}] %{file} [%{function}(%{line})]: %{message}");

    maxFileSize = 10 * 1000 * 1000; //默认但日志文件10M
    maxSaveDate = 180;
    filePath = "./log/";
    includeDebug = true;

    lastDeleteTime = QDateTime::currentDateTime();
}

Log::~Log()
{
}

/**************************************************************************
*  install
* -----------------------------------------------------------------------
* 功能: 启用日志记录，qDebug、qInfo等将自动记录到日志文件
* 参数: 【int】 maxSize：单日志文件大小，单位为M，值最大为1000;
* 参数: 【int】 maxDate：日志文件存储最长日期，单位为天，值最大为1000;
* 参数: 【QString】 path：日志保存路径;
* 参数: 【bool】 includeDebug：是否记录debug日志;
* 返回:
* 异常：无
* -----------------------------------------------------------------------
* 备注：无
**************************************************************************/
void Log::install(int maxSize,int maxDate, QString path, bool iDebug)
{
    if(maxSize < 1 || maxSize > 1000) {
        maxSize = 10;
    }
    maxFileSize = maxSize * 1024 * 1024;

    if(maxDate < 1 || maxDate > 1000) {
        maxDate = 180;
    }
    maxSaveDate = maxDate;

    includeDebug = iDebug;

    if(path == NULL || path.isNull() || path.isEmpty()) {
        filePath = "./log/";
    } else {
        QDir dirTmp(path);
        if(!dirTmp.exists())
        {
            if (!dirTmp.mkpath(dirTmp.absolutePath())) {
                path = "./log/";
            }
        }
        filePath = path;
    }
    if (!filePath.endsWith("/")) {
        filePath += "/";
    }
    QDir dir(filePath);
    if(!dir.exists())
    {
        QString t = dir.absolutePath();
        if(!dir.mkpath(dir.absolutePath())) {
            filePath = "./";
        }

    }

    //安装消息处理函数
    qInstallMessageHandler(messageHandler);

    deleteLog(); //删除过期日志
}


/**************************************************************************
*  uninstall
* -----------------------------------------------------------------------
* 功能: 卸载日志函数，取消qDebug、qInfo等将自动记录到日志文件
* 返回:
* 异常：无
* -----------------------------------------------------------------------
* 备注：无
**************************************************************************/
//
void Log::uninstall()
{
    qInstallMessageHandler(0);
}

//日志信息处理函数
void Log::messageHandler(QtMsgType msgType, const QMessageLogContext &context, const QString &msg)
{
    //判断信息类型
   // qInfo()<<msgType;
    QString type;
    switch (msgType) {
    case QtDebugMsg:
        type = QString("debug");
        if(!includeDebug)return;
        break;
    case QtWarningMsg:
        type = QString("warning");
        break;
    case QtCriticalMsg:
        type = QString("critical");
        break;
    case QtFatalMsg:
        type = QString("fatal");
        break;
    case QtInfoMsg:
        type = QString("info");
        break;
    default:
        type = QString("unknow");
        break;
    }

    //文件名和行数以及函数
    QString contextInfo = QString("[File:(%1), Function(%3), Line:(%2)]:").arg(context.file).arg(context.line).arg(context.function);
    outPutMsg(contextInfo, type, msg);
}

//日志信息处理函数
void Log::outPutMsg(QString contextInfo, QString type, QString msg)
{

    //获取当前时间，精确到秒
    QString currentTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
    //拼接信息字符串
    QString message = QString("[%1] [%2]: %3 %4").arg(currentTime).arg(type).arg(contextInfo).arg(msg);

    mutex.lock();  //互斥关锁
    saveLog(message);

    //额外保存错误和致命日志
    if(QString::compare(type, "critical") == 0 || QString::compare(type, "catal") == 0)
    {
        saveImportantLog(message);
    }

    //每天删除旧日志
    if(lastDeleteTime.date().dayOfYear() != QDateTime::currentDateTime().date().dayOfYear()) {
        deleteLog();
    }
    mutex.unlock(); //开锁

}


//删除过期日志
void Log::deleteLog()
{
    //获取日志文件夹地址
    QString dirName =  filePath;
    QDir dir(dirName);
    //获取文件夹下所有文件信息列表
    QFileInfoList infoList = dir.entryInfoList(QDir::Files);
    //遍历日志文件
    foreach (QFileInfo fileInfo, infoList) {
        //将文件创建时间与过期时间作比较，如果创建时间小于过期时间，则删除
        if(fileInfo.fileName().endsWith(".log") && fileInfo.lastModified() <= QDateTime::currentDateTime().addDays(-maxSaveDate))
        {
            QFile::setPermissions(dirName + "/" +fileInfo.fileName(), QFileDevice::ReadOther | QFileDevice::WriteOther);
            if(QFile::remove(dirName + "/" +fileInfo.fileName()))
            {
                qInfo()<<"Auto deleted old log file:"<<dirName<<"/"<<fileInfo.fileName();
            }
            else
            {
                qCritical()<<"Error to delete old log file:"<<dirName<<"/"<<fileInfo.fileName();
            }
        }

    }
}

//保存日志到文件
void Log::saveLog(QString message)
{

    //以天为单位给文件命名
    QString fileName = filePath + QDateTime::currentDateTime().toString("yyyy-MM-dd")+ "_log";

    write(fileName, message);

}

//保存日志到文件
void Log::saveImportantLog(QString message)
{
    //以天为单位给文件命名
    QString fileName = filePath + QDateTime::currentDateTime().toString("yyyy-MM-dd")+ "_Error_log";

    write(fileName, message);

}

void Log::write(QString fileName, QString message)
{

    //最终要写入的文件名
    QString fileNameLast = fileName + ".log";
    //绑定文件对象
    QFile file(fileNameLast);
    //判断文件大小
    if(file.size() >= maxFileSize){
        int i = 1; //当文件大小超过最大值时，给新文件添加编号
        //给新文件加入序号后缀
        QString fileNameTmp = fileName + QString("%1.txt").arg(i);
        QFile fileTmp(fileNameTmp);
        while(fileTmp.exists())
        {
            i++;
            fileNameTmp = fileName + QString("%1.txt").arg(i);
            fileTmp.setFileName(fileNameTmp);
        }
        file.rename(fileNameTmp);
        file.setFileName(fileNameLast);
    }

    //只写和拼接的方式打开文件
    bool isopen = file.open(QIODevice::WriteOnly | QIODevice::Append);
    if(isopen == true)
    {
        QTextStream write(&file);
        write << message << "\r\n";
        file.flush();
        file.close();
    }
}
