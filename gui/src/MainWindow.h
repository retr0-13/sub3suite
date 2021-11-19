#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSettings>
//...
#include "src/utils/utils.h"
#include "src/project/Project.h"
#include "src/engines/ip/Ip.h"
#include "src/engines/records/DnsRecords.h"
#include "src/engines/brute/Brute.h"
#include "src/engines/osint/Osint.h"
#include "src/engines/active/Active.h"
#include "src/engines/raw/Raw.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow{
        Q_OBJECT

    private:
        Ui::MainWindow *ui;

    public:
        explicit MainWindow(QWidget *parent = nullptr);
        ~MainWindow();
        //...
        Status *status;
        ResultsModel *resultsModel;
        ProjectDataModel *projectDataModel;

    public slots:
        void onReceiveLog(QString log);
        void onReceiveStatus(QString status);
        //...
        void onChangeTabToOsint();
        void onChangeTabToActive();
        void onChangeTabToBrute();
        void onChangeTabToIp();
        void onChangeTabToRecords();

    private slots:
        void on_actionAbout_triggered();
        void on_actionAboutQt_triggered();
        void on_actionExit_triggered();
        ///
        /// tools actions...
        ///
        void on_actionIpTool_triggered();
        void on_actionASNTool_triggered();

        void on_actionCertTool_triggered();

private:
        Ip *ip = nullptr;
        Osint *osint = nullptr;
        Brute *brute = nullptr;
        Active *active = nullptr;
        DnsRecords *records = nullptr;
        Project *project = nullptr;
        Raw *raw = nullptr;
};

#endif // MAINWINDOW_H
