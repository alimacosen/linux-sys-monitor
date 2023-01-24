#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTimer>
#include "about.h"
#include <mem.h>
#include <QDebug>
#include <cpu.h>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    splineSeries_m(new QSplineSeries()),
    splineSeries_s(new QSplineSeries()),
    splineSeries_cpu0(new QSplineSeries()),
    splineSeries_cpu1(new QSplineSeries()),
    splineSeries_cpu2(new QSplineSeries()),
    splineSeries_cpu3(new QSplineSeries()),

    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QLabel *per = new QLabel("Authored by CaiFanglei", this);
    ui->statusBar->addPermanentWidget(per);

    QTimer *timer = new QTimer(this);
    //关联定时器溢出信号和相应的槽函数
    connect(timer,&QTimer::timeout, this, &MainWindow::getMemAvailable);
    connect(timer,&QTimer::timeout, this, &MainWindow::getRuntime);
    connect(timer,&QTimer::timeout, this, &MainWindow::showNowTime);
    connect(timer,&QTimer::timeout, this, &MainWindow::getSwapFree);
    connect(timer,&QTimer::timeout, this, &MainWindow::showMemline);
    connect(timer,&QTimer::timeout, this, &MainWindow::getCpu0);
    connect(timer,&QTimer::timeout, this, &MainWindow::getCpu1);
    connect(timer,&QTimer::timeout, this, &MainWindow::getCpu2);
    connect(timer,&QTimer::timeout, this, &MainWindow::getCpu3);
    connect(timer,&QTimer::timeout, this, &MainWindow::showCpuline);
    connect(timer,&QTimer::timeout, this, &MainWindow::showprocess);
    connect(timer,&QTimer::timeout, this, &MainWindow::showmodule);
    timer->start(500);

    QStringList widget, widget_2;
    widget<<"Pid"<<"Pname"<<"PPid"<<"State"<<"Mem/KB";
    widget_2<<"Mname"<<"Mem/KB"<<"Usetimes";
    ui->tableWidget->setHorizontalHeaderLabels(widget);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);  // 列均匀
    ui->tableWidget_2->setHorizontalHeaderLabels(widget_2);
    ui->tableWidget_2->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);  // 列均匀

    ui->label_HostName->setText("HostName:");
    ui->label_HostName_Val->setText(About::getHostname());
    ui->label_StartUpTime->setText("Start up Time:");
    ui->label_StartUpTime_Val->setText(About::getStarttime());
    ui->label_RunTime->setText("Run Time:");
    ui->label_SysVer->setText("System Version:");
    ui->label_SysVer_Val->setText(About::getSysversion());
    ui->label_CpuInfo->setText("Cpu Info:");
    ui->label_CpuInfo_Val->setText(About::getCpuinfo());

    ui->progressBar_3->setRange(0,100);
    ui->progressBar_4->setRange(0,100);
    ui->progressBar_5->setRange(0,100);
    ui->progressBar_6->setRange(0,100);
    ui->progressBar->setRange(0,100);
    ui->progressBar_2->setRange(0,100);

    Mem::getMemTotal();
    ui->label_MemTotal_val->setText(Mem::MemTotal);
    Mem::getSwapTotal();
    ui->label_SwapTotal_val->setText(Mem::SwapTotal);
    // ui->label_29->setStyleSheet("background-color:#FF0000");
    // ui->label_28->setStyleSheet("background-color:#00FF00");

    connect(ui->lineEdit, SIGNAL(returnPressed()), ui->SearchButton, SIGNAL(clicked()), Qt::UniqueConnection);

    QChart *chart_mem = new QChart();
    chart_mem->addSeries(splineSeries_m);
    chart_mem->addSeries(splineSeries_s);
    // chart_mem->legend()->hide();  // chart example
    chart_mem->setTitle("Mem&Swap usage");
    chart_mem->createDefaultAxes(); // 默认没有创建坐标轴
    chart_mem->axisY()->setRange(0, 100);
    chart_mem->axisX()->setRange(-120,0);

    ui->chartview->setChart(chart_mem);
    ui->chartview->setRenderHint(QPainter::Antialiasing);  // 抗锯齿

    QChart *chart_cpu = new QChart();
    chart_cpu->addSeries(splineSeries_cpu0);
    chart_cpu->addSeries(splineSeries_cpu1);
    chart_cpu->addSeries(splineSeries_cpu2);
    chart_cpu->addSeries(splineSeries_cpu3);
    //chart_cpu->legend()->hide();
    chart_cpu->setTitle("cpu usage");
    chart_cpu->createDefaultAxes();  // 默认没有创建坐标轴
    chart_cpu->axisY()->setRange(0, 100);
    chart_cpu->axisX()->setRange(-120,0);
    ui->chartview_2->setChart(chart_cpu);
    ui->chartview_2->setRenderHint(QPainter::Antialiasing);  //抗锯齿

    // ui->label_33->setStyleSheet("background-color:#FF0000");
    // ui->label_34->setStyleSheet("background-color:#00FF00");
    // ui->label_6->setStyleSheet("background-color:#0000FF");
    // ui->label_8->setStyleSheet("background-color:#000000");
}

MainWindow::~MainWindow()
{
    delete splineSeries_s;
    delete splineSeries_m;
    delete ui;
}

void MainWindow::getMemAvailable()
{
    Mem::MemAvailable.clear();
    QFile file("/proc/meminfo");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug()<<"can't open /proc/meminfo";
        return;
    }
    QTextStream read_in(&file);
    QString line = read_in.readLine();

    while (!line.isNull())
    {
        if(line.contains("MemAvailable"))
        {
            for(int i = 0; i < line.length(); i++)
            {
                if(line[i] >= '0' && line[i] <= '9')
                    Mem::MemAvailable.append(line[i]);
            }
        }
        line = read_in.readLine();
    }

    ui->label_MemAvailable_val->setText(Mem::MemAvailable);
    ui->label_MemUsed_val->setText(QString::number(Mem::MemTotal.toInt() - Mem::MemAvailable.toInt()));
    int Memusage = 100 - 100 * Mem::MemAvailable.toInt()/Mem::MemTotal.toInt();
    ui->progressBar->setValue(Memusage);

    Line temp;
    temp.x = 0;
    temp.y = Memusage;
    if(Musage.size() >= 120)
        Musage.pop_front();
    Musage.push_back(temp);
}

void MainWindow::getRuntime()
{
    QFile file("/proc/uptime");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug()<<"can't open /proc/uptime";
        return ;
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
    str = formatTime(str.toInt());
    ui->label_RunTime_Val->setText(str);
}

QString MainWindow::formatTime(int s)
{
   int mi = 60;
   int hh = 3600;
   int dd = 86400;

   long day = s / dd;
   long hour = (s - day * dd) / hh;
   long minute = (s - day * dd - hour * hh) / mi;
   long second = (s - day * dd - hour * hh - minute * mi) ;

   QString hou = QString::number(hour);
   QString min = QString::number(minute);
   QString sec = QString::number(second);
   return hou + ":" + min + ":" + sec ;
}

void MainWindow::on_ShutdownButton_clicked()
{
    QMessageBox::StandardButton reply = QMessageBox::question(this,
                                                              "Shut Down Computer",
                                                              "Are you sure to shut down the computer?",
                                                              QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes)
    {
        QProcess *proc = new QProcess;
        proc->start("shutdown -h now");
    }
    else
    {
        qDebug()<<"shutdown no is pressed";
    }
}

void MainWindow::on_RebootButton_clicked()
{
    QMessageBox::StandardButton reply = QMessageBox::question(this,
                                                              "Reboot Computer",
                                                              "Are you sure to reboot the computer?",
                                                              QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes)
    {
        QProcess *proc = new QProcess;
        proc->start("shutdown -r now");
    }
    else
    {
        qDebug()<<"reboot no is pressed";
    }
}

void MainWindow::showNowTime()
{
    QDateTime current_date_time = QDateTime::currentDateTime();
    QString current_date = current_date_time.toString("yyyy.MM.dd hh:mm:ss");
    ui->statusBar->showMessage(current_date, 1000);
}

void MainWindow::getSwapFree()
{
    Mem::SwapFree.clear();
    QFile file("/proc/meminfo");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug()<<"can't open /proc/meminfo";
        return;
    }
    QTextStream read_in(&file);
    QString line = read_in.readLine();

    while (!line.isNull())
    {
        if(line.contains("SwapFree"))
        {
            for(int i = 0; i < line.length(); i++)
            {
                if(line[i] >= '0' && line[i] <= '9')
                    Mem::SwapFree.append(line[i]);
            }
        }
        line = read_in.readLine();
    }
    ui->label_SwapUsed_val->setText(QString::number(Mem::SwapTotal.toInt() - Mem::SwapFree.toInt()));
    int Swapusage = 100 - 100 * Mem::SwapFree.toInt()/Mem::SwapTotal.toInt();
    ui->progressBar_2->setValue(Swapusage);

    Line temp;
    temp.x = 0;
    temp.y = Swapusage;
    if(Susage.size() >= 120)
        Susage.pop_front();
    Susage.push_back(temp);
}

void MainWindow::showMemline()
{
    splineSeries_m->setName("Mem");
    splineSeries_m->clear();
    for(std::deque<Line>::iterator myItor = Musage.begin() ; myItor!=Musage.end(); myItor++)
    {
        splineSeries_m->append(myItor->x,myItor->y);
        (myItor->x)--;
    }
    splineSeries_m->setColor("#FF0000");

    splineSeries_s->setName("Swap");
    splineSeries_s->clear();
    for(std::deque<Line>::iterator myItor = Susage.begin() ; myItor!=Susage.end(); myItor++)
    {
        splineSeries_s->append(myItor->x,myItor->y);
        (myItor->x)--;
    }
    splineSeries_s->setColor("#00FF00");
}

void MainWindow::getCpu0()
{
    QFile file("/proc/stat");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug()<<"error";
        return;
    }
    QTextStream read_in(&file);
    QString line = read_in.readLine();
    QString total;
    int total_num = 0;
    QString free;
    int free_num = 0;

    while (!line.isNull())
    {
        if(line.contains("cpu0"))
        {
            int blank = 0;
            for(int i = 0; i < line.length(); i++)
            {
                if(line[i] >= '0' && line[i] <= '9')
                    total.append(line[i]);
                if((blank == 4 || blank == 5) && line[i] >= '0' && line[i] <= '9') // idle(4) iowait(5)
                {
                    free.append(line[i]);
                }
                if(line[i] == ' ')
                {
                    blank++;
                    total_num += total.toInt();
                    total.clear();
                    free_num += free.toInt();
                    free.clear();
                }
            }
        }
        line = read_in.readLine();
    }
    int cpu0usage = 0;
    if(Cpu::cpu0total.size() != 0)
    {
        int t = total_num - Cpu::cpu0total.toInt();
        int f = free_num - Cpu::cpu0free.toInt();
        cpu0usage = (100 - 100*f/t);
        Line temp;
        temp.x = 0;
        temp.y = cpu0usage;
        if(Cpu0usage.size() >= 120)  // ATTENTION! Cpu0usage & cpu0usage are different!
            Cpu0usage.pop_front();
        Cpu0usage.push_back(temp);
    }
    Cpu::cpu0total = QString::number(total_num);
    Cpu::cpu0free = QString::number(free_num);
    ui->progressBar_3->setValue(cpu0usage);
}

void MainWindow::getCpu1()
{
    QFile file("/proc/stat");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug()<<"error";
        return;
    }
    QTextStream read_in(&file);
    QString line = read_in.readLine();
    QString total;
    int total_num = 0;
    QString free;
    int free_num = 0;

    while (!line.isNull())
    {
        if(line.contains("cpu1"))
        {
            int blank = 0;
            for(int i = 0; i < line.length(); i++)
            {
                if(line[i] >= '0' && line[i] <= '9')
                    total.append(line[i]);
                if((blank == 4 || blank == 5) && line[i] >= '0' && line[i] <= '9')
                    free.append(line[i]);
                if(line[i] == ' ')
                {
                    blank++;
                    total_num += total.toInt();
                    total.clear();
                    free_num += free.toInt();
                    free.clear();
                }
            }
        }
        line = read_in.readLine();
    }

    int cpu1usage = 0;
    if(Cpu::cpu1total.size() != 0)
    {
        int t = total_num - Cpu::cpu1total.toInt();
        int f = free_num - Cpu::cpu1free.toInt();
        cpu1usage = (100 - 100*f/t);
        Line temp;
        temp.x = 0;
        temp.y = cpu1usage;
        if(Cpu1usage.size() >= 120)
            Cpu1usage.pop_front();
        Cpu1usage.push_back(temp);
    }
    Cpu::cpu1total = QString::number(total_num);
    Cpu::cpu1free = QString::number(free_num);;
    ui->progressBar_4->setValue(cpu1usage);
}

void MainWindow::getCpu2()
{
    QFile file("/proc/stat");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug()<<"error";
        return;
    }
    QTextStream read_in(&file);
    QString line = read_in.readLine();
    QString total;
    int total_num = 0;
    QString free;
    int free_num = 0;

    while (!line.isNull())
    {
        if(line.contains("cpu2"))
        {
            int blank = 0;
            for(int i = 0; i < line.length(); i++)
            {
                if(line[i] >= '0' && line[i] <= '9')
                    total.append(line[i]);
                if((blank == 4 || blank == 5) && line[i] >= '0' && line[i] <= '9')
                    free.append(line[i]);
                if(line[i] == ' ')
                {
                    blank++;
                    total_num += total.toInt();
                    total.clear();
                    free_num += free.toInt();
                    free.clear();
                }
            }
        }
        line = read_in.readLine();
    }

    int cpu1usage = 0;
    if(Cpu::cpu2total.size() != 0)
    {
        int t = total_num - Cpu::cpu2total.toInt();
        int f = free_num - Cpu::cpu2free.toInt();
        cpu1usage = (100 - 100*f/t);
        Line temp;
        temp.x = 0;
        temp.y = cpu1usage;
        if(Cpu2usage.size() >= 120)
            Cpu2usage.pop_front();
        Cpu2usage.push_back(temp);
    }
    Cpu::cpu2total = QString::number(total_num);
    Cpu::cpu2free = QString::number(free_num);
    ui->progressBar_5->setValue(cpu1usage);
}

void MainWindow::getCpu3()
{
    QFile file("/proc/stat");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug()<<"error";
        return;
    }
    QTextStream read_in(&file);
    QString line = read_in.readLine();
    QString total;
    int total_num = 0;
    QString free;
    int free_num = 0;

    while (!line.isNull())
    {
        if(line.contains("cpu3"))
        {
            int blank = 0;
            for(int i = 0; i < line.length(); i++)
            {
                if(line[i] >= '0' && line[i] <= '9')
                    total.append(line[i]);
                if((blank == 4 || blank == 5) && line[i] >= '0' && line[i] <= '9')
                    free.append(line[i]);
                if(line[i] == ' ')
                {
                    blank++;
                    total_num += total.toInt();
                    total.clear();
                    free_num += free.toInt();
                    free.clear();
                }
            }
        }
        line = read_in.readLine();
    }

    int cpu1usage = 0;
    if(Cpu::cpu3total.size() != 0)
    {
        int t = total_num - Cpu::cpu3total.toInt();
        int f = free_num - Cpu::cpu3free.toInt();
        cpu1usage = (100 - 100*f/t);
        Line temp;
        temp.x = 0;
        temp.y = cpu1usage;
        if(Cpu3usage.size() >= 120)
            Cpu3usage.pop_front();
        Cpu3usage.push_back(temp);
    }
    Cpu::cpu3total = QString::number(total_num);
    Cpu::cpu3free = QString::number(free_num);;
    ui->progressBar_6->setValue(cpu1usage);
}

void MainWindow::showCpuline()
{
    splineSeries_cpu0->setName("Cpu0");
    splineSeries_cpu0->clear();  // dynamic re-draw 120pix spline
    splineSeries_cpu0->setColor("#FF0000");
    for(std::deque<Line>::iterator myItor = Cpu0usage.begin(); myItor!=Cpu0usage.end(); myItor++)
    {
        splineSeries_cpu0->append(myItor->x,myItor->y);
        (myItor->x)--;
    }

    splineSeries_cpu1->setName("Cpu1");
    splineSeries_cpu1->clear();
    splineSeries_cpu1->setColor("#00FF00");
    for(std::deque<Line>::iterator myItor = Cpu1usage.begin(); myItor!=Cpu1usage.end(); myItor++)
    {
        splineSeries_cpu1->append(myItor->x,myItor->y);
        (myItor->x)--;
    }

    splineSeries_cpu2->setName("Cpu2");
    splineSeries_cpu2->clear();
    splineSeries_cpu2->setColor("#0000FF");
    for(std::deque<Line>::iterator myItor = Cpu2usage.begin(); myItor!=Cpu2usage.end(); myItor++)
    {
        splineSeries_cpu2->append(myItor->x,myItor->y);
        (myItor->x)--;
    }

    splineSeries_cpu3->setName("Cpu3");
    splineSeries_cpu3->clear();
    splineSeries_cpu3->setColor("#000000");
    for(std::deque<Line>::iterator myItor = Cpu3usage.begin(); myItor!=Cpu3usage.end(); myItor++)
    {
        splineSeries_cpu3->append(myItor->x,myItor->y);
        (myItor->x)--;
    }
}

void MainWindow::showmodule()
{
    QFile file("/proc/modules");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug()<<"can't open /proc/modules";
        return;
    }
    QTextStream read_in(&file);
    QString line = read_in.readLine();
    QString module_name, module_size, module_usedtimes;

    while(!line.isNull())
    {
        int blank = 0;
        module_name.clear();
        module_size.clear();
        module_usedtimes.clear();
        static int line_num = 0;
        for(int i = 0; i < line.length(); i++)
        {
            if(blank == 3)
                break;
            if(blank == 0)
            {
                module_name.append(line[i]);
            }
            if(blank == 1 && line[i] >= '0' && line[i] <= '9')
            {
                module_size.append(line[i]);
            }
            if(blank == 2 && line[i] >= '0' && line[i] <= '9')
            {
                module_usedtimes.append(line[i]);
            }
            if(line[i] == ' ')
            {
                blank++;
            }
        }
        ui->tableWidget_2->setItem(line_num, 0, new QTableWidgetItem(module_name.trimmed()));
        ui->tableWidget_2->setItem(line_num, 1, new QTableWidgetItem(module_size.trimmed()));
        ui->tableWidget_2->setItem(line_num, 2, new QTableWidgetItem(module_usedtimes.trimmed()));
        line = read_in.readLine();
        line_num++;
    }
}

void MainWindow::showprocess()
{
      string_list.clear();
     //判断路径是否存在
      QDir dir("/proc");
      if(!dir.exists())
      {
        qDebug()<<"/proc doesn't exist";
        return;
      }

      //统计cfg格式的文件个数
      int dir_count = dir.count();
      if(dir_count <= 0)
      {
        qDebug()<<"dir_count is 0";
        return;
      }

      //存储文件名称
      for(int i = 0; i < dir_count; i++)
      {
          QString file_name = dir[i];  // 文件名称
          if(file_name[0] >= '0' && file_name[0] <= '9')
             string_list.append(file_name);
      }
      string_list.sort();
      ui->tableWidget->setRowCount(string_list.size());
      processNum = string_list.size();
      for(int i = 0; i < string_list.size(); i++)
      {
          QString name, pid, ppid, rss, state;
          QFile file("/proc/"+string_list[i]+"/status");
          if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
          {
              qDebug()<<"can't open /proc/string_list[i]/status";
              return;
          }
          QTextStream read_in(&file);
          QString line = read_in.readLine();

          int pflag = 0;      //To avoid reading Pid more times
          while (!line.isNull())
          {
              if(line.contains("Name"))
              {
                      int nflag = 0;
                      for(int j = 0; j < line.length(); j++)
                      {
                          if(nflag == 1 && line[j] != ' ')
                              name.append(line[j]);
                          if(line[j] == ':')
                              nflag = 1;
                      }
              }
              else if(line.contains("PPid"))
              {
                      for(int j = 0; j < line.length(); j++)
                      {
                          if(line[j] >= '0' && line[j] <= '9')
                             ppid.append(line[j]);
                      }
              }
              else if(line.contains("Pid"))
              {
                  if(pflag == 0)
                  {
                      for(int j = 0; j < line.length(); j++)
                      {
                          if(line[j] >= '0' && line[j] <= '9')
                              pid.append(line[j]);
                      }
                      pflag = 1;
                  }
              }
              else if(line.contains("RssAnon"))
              {
                      for(int j = 0; j < line.length(); j++)
                      {
                          if(line[j] >= '0' && line[j] <= '9')
                             rss.append(line[j]);
                      }
              }
              else if(line.contains("State"))
              {
                      int sflag = 0;
                      for(int j = 0; j < line.length(); j++)
                      {
                          if(sflag == 1 && line[j] != ' ')
                              state.append(line[j]);
                          if(line[j] == ':')
                              sflag = 1;
                      }
              }
              line = read_in.readLine();
          }
           ui->tableWidget->setItem(i,0,new QTableWidgetItem(pid.trimmed()));
           ui->tableWidget->setItem(i,1,new QTableWidgetItem(name.trimmed()));
           ui->tableWidget->setItem(i,2,new QTableWidgetItem(ppid.trimmed()));
           ui->tableWidget->setItem(i,3,new QTableWidgetItem(state.trimmed()));
           ui->tableWidget->setItem(i,4,new QTableWidgetItem(rss.trimmed()));
      }
}

void MainWindow::on_KillButton_clicked()
{
       if(ui->tableWidget->selectedItems().isEmpty())
           return;
       QMessageBox::StandardButton res = QMessageBox::question(NULL,
                                                              "Warning",
                                                              "Are you sure to kill the process?",
                                                              QMessageBox::Yes | QMessageBox::No);
       if(res == QMessageBox::Yes)
       {
           int rownum = ui->tableWidget->currentItem()->row();
           QString pid = ui->tableWidget->item(rownum, 0)->text();
           QProcess::execute("kill "+pid);
           return ;
       }
       return ;
}

void MainWindow::on_SearchButton_clicked()
{
      QString text = ui->lineEdit->text();
      if(text.isEmpty())
      {
         QMessageBox::about(NULL, "NULL", "Please input Pid or Pname first!");
          return;
      }

      int find = 0;
      for(int i = 0; i < processNum; i++)
      {
          if(ui->tableWidget->item(i,0)->text() == text || ui->tableWidget->item(i,1)->text() == text)
          {
              ui->tableWidget->selectRow(i);
              find = 1;
              break;
          }
      }
      if(find == 0)
          QMessageBox::about(NULL, "NONE", "Not Found!");
}

void MainWindow::on_DetailButton_clicked()
{
    if(ui->tableWidget->selectedItems().isEmpty())
        return;
    int rownum = ui->tableWidget->currentItem()->row();
    QString pid = ui->tableWidget->item(rownum ,0)->text();
    QFile file("/proc/"+pid+"/status");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug()<<"can't open /proc/pid/status";
        return;
    }
    QTextStream read_in(&file);
    QString infoStr = read_in.readAll();
    QMessageBox::about(NULL, "Details", infoStr.trimmed());
}

void MainWindow::on_AboutQTButton_clicked()
{
    QMessageBox::aboutQt(this, "About QT");
}

void MainWindow::on_AboutMonitorButton_clicked()
{
    QMessageBox::about(this, "About Monitor", "Welcome!\nThis is my linux monitor!");
}

void MainWindow::on_ExitButton_clicked()
{
    QMessageBox::StandardButton reply = QMessageBox::question(this,
                                                              "Quit Monitor Application",
                                                              "(╯°□°）╯︵┻━┻)\nAre you sure to quit?",
                                                              QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes)
    {
        QApplication::quit();
    }
    else
    {
        qDebug()<<"no is pressed";
    }
}
