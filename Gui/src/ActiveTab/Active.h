#ifndef ACTIVE_H
#define ACTIVE_H

#include "lib-active.h"
#include "ActiveEnumerator.h"

namespace Ui {
    class Active;
}

class Active : public QDialog{
        Q_OBJECT

    public:
        explicit Active(QWidget *parent = nullptr);
        ~Active();

    private:
        Ui::Active *ui;

    private:
        void startEnumeration();
        //...
        QStandardItemModel *m_model;
        ScanArguments_Active *m_scanArguments;
        //...
        int m_activeThreads = 0;
        int m_endedThreads = 0;
        //...
        QString currentPath;

    signals:
        void stop();
        void sendStatus(QString status);
        void sendLog(QString log);

    public slots:
        void scanResult(QString subdomain, QString ipAddress);
        void onThreadEnd();
        void logs(QString log);

    private slots:
        void on_pushButton_start_clicked();
        void on_pushButton_stop_clicked();
        void on_pushButton_get_clicked();
        void on_pushButton_action_clicked();
        void on_toolButton_config_clicked();
        void on_pushButton_clearResults_clicked();
        //...
        void on_pushButton_removeTargets_clicked();
        void on_pushButton_clearTargets_clicked();
        void on_pushButton_loadTargets_clicked();
        void on_pushButton_addTargets_clicked();
        void on_lineEdit_targets_returnPressed();
        //...
        void on_radioButton_hostname_clicked();
        void on_radioButton_ip_clicked();
        void on_comboBox_option_currentIndexChanged(int index);
        void on_tableView_results_customContextMenuRequested(const QPoint &pos);
        // context...
        void actionSendToMultiLevel();
        void actionSendToSave();
        void actionSendToDnsRecords();
        // cursor...
        void cursorSendToSave();
        void cursorSendToDnsRecords();
        void cursorOpenInBrowser();
};

#endif // ACTIVE_H