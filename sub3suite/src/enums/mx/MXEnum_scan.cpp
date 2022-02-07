#include "MXEnum.h"
#include "ui_MXEnum.h"


void MXEnum::onScanThreadEnded(){
    status->activeScanThreads--;

    /* if all Scan Threads have finished... */
    if(status->activeScanThreads == 0)
    {
        /* display the scan summary on logs
        m_scanSummary();
        */

        if(status->isStopped)
            this->log("---------------- Stopped ------------\n");
        else
            this->log("------------------ End --------------\n");

        /* set the progress bar to 100% just in case... */
        if(!status->isStopped)
            ui->progressBar->setValue(ui->progressBar->maximum());

        status->isNotActive = true;
        status->isPaused = false;
        status->isStopped = false;
        status->isRunning = false;

        ui->buttonStart->setEnabled(true);
        ui->buttonStop->setDisabled(true);
    }
}

void MXEnum::startScan(){
    ScanArgs scanArgs;

    /* resetting */
    ui->progressBar->show();
    ui->progressBar->reset();
    ui->progressBar->clearMask();

    /* getting targets */
    if(ui->checkBoxMultipleTargets->isChecked()){
        foreach(const QString &target, m_targetsListModel->stringList())
            scanArgs.targets.enqueue(target);
    }else{
        scanArgs.targets.enqueue(ui->lineEditTarget->text());
    }

    /* progressbar maximum value */
    ui->progressBar->setMaximum(scanArgs.targets.length());
    scanArgs.progress = 0;

    scanArgs.outputInfoMX = true;

    QThread *cThread = new QThread;
    switch (ui->comboBoxEngine->currentIndex()) {
    case 0: // DNSLytics
        Dnslytics *dnslytics = new Dnslytics(scanArgs);
        dnslytics->startScan(cThread);
        dnslytics->moveToThread(cThread);
        connect(dnslytics, &Dnslytics::infoMX, this, &MXEnum::onResult);
        connect(dnslytics, &Dnslytics::infoLog, this, &MXEnum::onInfoLog);
        connect(dnslytics, &Dnslytics::errorLog, this, &MXEnum::onErrorLog);
        connect(dnslytics, &Dnslytics::rateLimitLog, this, &MXEnum::onRateLimitLog);
        connect(cThread, &QThread::finished, this, &MXEnum::onScanThreadEnded);
        connect(cThread, &QThread::finished, dnslytics, &Dnslytics::deleteLater);
        connect(cThread, &QThread::finished, cThread, &QThread::deleteLater);
        cThread->start();
        status->activeScanThreads++;
    }
}
