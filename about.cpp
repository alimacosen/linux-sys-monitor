#include "about.h"

About::About()
{

}

QString About::getHostname()
{
    QFile file("/etc/hostname");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug()<<"can't open /etc/hostname\n";
        return "UNKNOW";
    }
    QTextStream read_in(&file);
    return read_in.readLine();
}

QString About::getStarttime()
{
    int sec_now=QDateTime::currentDateTime().toTime_t();
    QFile file("/proc/uptime");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug()<<"can't open /proc/uptime";
        return "UNKNOW";
    }
    QTextStream read_in(&file);
    QString line = read_in.readLine();
    QString str;
    for(int i = 0; i < line.length(); i++)
    {
        if(line[i] != '.')
            str.append(line[i]);
        else
            break;
    }
    int res = sec_now - str.toInt();
    return QDateTime::fromTime_t(res).toString("yyyy-MM-dd hh:mm:ss");
}

QString About::getSysversion()
{
    QFile file("/etc/issue");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug()<<"can't open /etc/issue";
        return "UNKONW";
    }
    QTextStream read_in(&file);
    QString line = read_in.readLine();
    QString str;
    for(int i = 0; i < line.length(); i++)
    {
        if(line[i] != '\\')
            str.append(line[i]);
        else
            break;
    }
    return str;
}

QString About::getCpuinfo()
{
    QFile file("/proc/cpuinfo");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug()<<"can't open /proc/cpuinfo";
        return "UNKONW";
    }
    QTextStream read_in(&file);
    QString line = read_in.readLine();
    QString str;
    while (!line.isNull())
    {
        if(line.contains("model name"))
        {
            int flag = 0;
            for(int i = 0; i < line.length(); i++)
            {
                if(flag == 1)
                {
                    str.append(line[i]);
                }
                if(line[i] == ':')
                {
                    i++; // ignore the ' ' after ':'
                    flag = 1;
                }
            }
            break;
        }
        line = read_in.readLine();
    }
    return str;
}
