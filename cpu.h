#ifndef CPU_H
#define CPU_H
#include <QString>

class Cpu
{
public:
    Cpu();
    static int times;
    static QString cpu0total;
    static QString cpu0free;
    static QString cpu1total;
    static QString cpu1free;
    static QString cpu2total;
    static QString cpu2free;
    static QString cpu3total;
    static QString cpu3free;
};

#endif // CPU_H
