#ifndef LOG_H
#define LOG_H
#include <QObject>
#include <QtMessageHandler>
#include <QDebug>
#include <QtDebug>
#include <QTextStream>
#include <QApplication>
#include <QDateTime>
#include <QMutex>
#include <QFile>
#include <QFileInfo>
#include <QMetaEnum>
#include <QSettings>
#include <QDir>

/**
 * @brief The Log class
 * @author zend
 */

class Log : public QObject
{
    Q_OBJECT
public:
    explicit Log(QObject *parent = nullptr);
    ~Log();

    static void install(int maxSize, int maxDate, QString path, bool iDebug); //安装信息处理函数
    static void uninstall(); //卸载信息处理函数


private:
    /*static QMutex mutex; //设置互斥锁
    static int maxFileSize; //单个日志文件最大值，单位为MB
    static int maxSaveDate; //最大保存日期，单位：天
    static QString filePath; //日志存储路径
    static bool includeDebug; //是否记录debug日志*/
    /*
     * 函数功能:
     * 1、根据调试信息以及日期，保存到相应的文件。
     * 2、在保存文件前需要判断文件大小是否大于自定义值,如果大于,便按照序号从小到大新建一个。
     *
     *
    */
    static void saveLog(QString message);
    static void saveImportantLog(QString message); //另存重要的日志，QtCriticalMsg、QtFatalMsg
    static void write(QString fileName, QString message);

    static void deleteLog(); //删除过期日志

    //信息处理函数(重写的myMessageHandler)
    /*功能说明:通过调试信息保存到日志文件
     *
     *参数说明:
     * msgType: 调试信息类型或级别（qdebug, qwarning, qfatal 。。。。）
     * context: 调试信息所处文本，可使用context.file和context.line获取文本所处行数及所处文件路径，以及使用context.function获取文本所处函数名
     * msg: 调试信息内容，自定义
    */
    static void messageHandler(QtMsgType msgType, const QMessageLogContext &context, const QString &msg);
    static void outPutMsg(QString contextInfo, QString type, QString msg);

};

#endif // LOGOUTPUT_H

