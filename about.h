#ifndef ABOUT_H
#define ABOUT_H
#include <QString>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QDebug>
class About
{

public:
    About();
    static QString getHostname();
    static QString getStarttime();
    static QString getSysversion();
    static QString getCpuinfo();
};

#endif // ABOUT_H
