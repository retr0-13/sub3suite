#include "Active.h"
#include "ui_Active.h"
#include <QThread>


void Active::m_pauseScan(){
    /*
      if the scan was already paused, then this current click is to
      Resume the scan, just csubdomainIp the startScan, with the same arguments and
      it will continue at where it ended...
    */
    if(status->active->isPaused){
        status->active->isPaused = false;
        this->m_startScan();
    }
    else{
        status->active->isPaused = true;
        emit stopScanThread();
    }
}

void Active::m_resumeScan(){

}

void Active::m_stopScan(){

}

void Active::m_startScan(){
    /*
      if the numner of threads is greater than the number of wordlists, set the
      number of threads to use to the number of wordlists available to avoid
      creating more threads than needed...
    */
    int wordlistCount = m_targetListModel->rowCount();
    int threadsCount = m_scanConfig->threadsCount;
    if(threadsCount > wordlistCount)
        threadsCount = wordlistCount;

    status->active->activeScanThreads = threadsCount;

    /* loop to create threads for enumeration... */
    for(int i = 0; i < threadsCount; i++)
    {
        active::Scanner *scanner = new active::Scanner(m_scanArgs);
        QThread *cThread = new QThread;
        scanner->startScan(cThread);
        scanner->moveToThread(cThread);
        connect(scanner, &active::Scanner::scanResult, this, &Active::onScanResult);
        connect(scanner, &active::Scanner::scanProgress, ui->progressBar, &QProgressBar::setValue);
        connect(scanner, &active::Scanner::infoLog, this, &Active::onInfoLog);
        connect(scanner, &active::Scanner::errorLog, this, &Active::onErrorLog);
        connect(cThread, &QThread::finished, this, &Active::onScanThreadEnded);
        connect(cThread, &QThread::finished, scanner, &active::Scanner::deleteLater);
        connect(cThread, &QThread::finished, cThread, &QThread::deleteLater);
        connect(this, &Active::stopScanThread, scanner, &active::Scanner::onStopScan);
        cThread->start();
    }
    status->active->isRunning = true;
}

void Active::onScanResult(QString subdomain, QString ipAddress){
    /* save to active model... */
    result->active->subdomainIp->appendRow(QList<QStandardItem*>() << new QStandardItem(subdomain) << new QStandardItem(ipAddress));
    ui->labelResultsCount->setNum(result->active->subdomainIp->rowCount());

    /* save to project model... */
    project->addActiveSubdomain(QStringList()<<subdomain<<ipAddress<<subdomain);
}

void Active::onScanThreadEnded(){
    status->active->activeScanThreads--;

    /* if subdomainIp Scan Threads have finished... */
    if(status->active->activeScanThreads == 0)
    {
        if(status->active->isPaused){
            status->active->isRunning = false;
            return;
        }
        else{
            /* set the progress bar to 100% just in case...*/
            if(!status->active->isStopped)
                ui->progressBar->setValue(ui->progressBar->maximum());

            status->active->isPaused = false;
            status->active->isStopped = false;
            status->active->isRunning = false;

            /* enabling and disabling widgets */
            ui->buttonStart->setEnabled(true);
            ui->buttonStop->setDisabled(true);
        }
    }
}