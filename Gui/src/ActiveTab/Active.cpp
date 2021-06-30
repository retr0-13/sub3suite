#include "Active.h"
#include "ui_Active.h"

Active::Active(QWidget *parent) : QDialog(parent), ui(new Ui::Active),
    m_model(new QStandardItemModel), m_scanArguments(new ScanArguments_Active)
{
    ui->setupUi(this);
    //...
    ui->lineEdit_targets->setPlaceholderText(("Enter a new target..."));
    ui->progressBar->hide();
    //...
    ui->pushButton_stop->setDisabled(true);
    ui->pushButton_pause->setDisabled(true);
    //...
    m_model->setHorizontalHeaderLabels({"Subdomain Name:", "IpAddress"});
    ui->tableView_results->setModel(m_model);
    //...
    QPalette p = palette();
    p.setColor(QPalette::Highlight, QColor(188, 188, 141));
    p.setColor(QPalette::HighlightedText, QColor(Qt::black));
    ui->tableView_results->setPalette(p);
    //...
    ui->splitter_3->setSizes(QList<int>()<<160<<1);
    //...
    currentPath = QDir::currentPath();
}
Active::~Active(){
    delete ui;
    delete m_model;
}

void Active::on_pushButton_start_clicked(){
    ///
    /// checking input requirements before scan...
    ///
    if(!(ui->listWidget_targets->count() > 0)){
        QMessageBox::warning(this, TITLE_ERROR, "Please Enter the subdomains Wordlist for Enumeration!");
        return;
    }
    ///
    /// disabling and Enabling widgets...
    ///
    ui->pushButton_start->setDisabled(true);
    ui->pushButton_pause->setEnabled(true);
    ui->pushButton_stop->setEnabled(true);
    ui->progressBar->clearMask();
    ui->progressBar->reset();
    ui->progressBar->show();
    ///
    /// Getting scan arguments....
    ///
    m_scanArguments->targetList = ui->listWidget_targets;
    ui->progressBar->setMaximum(ui->listWidget_targets->count());
    //...
    if(ui->comboBox_option->currentIndex() == ACTIVE_DNS){
        m_scanArguments->checkForService = false;
    }
    if(ui->comboBox_option->currentIndex() == ACTIVE_HTTP){
        m_scanArguments->checkForService = true;
        m_scanArguments->service = 80;
    }
    if(ui->comboBox_option->currentIndex() == ACTIVE_HTTPS){
        m_scanArguments->checkForService = true;
        m_scanArguments->service = 443;
    }
    if(ui->comboBox_option->currentIndex() == ACTIVE_FTP){
        m_scanArguments->checkForService = true;
        m_scanArguments->service = 21;
    }
    if(ui->comboBox_option->currentIndex() == ACTIVE_SMTP){
        m_scanArguments->checkForService = true;
        m_scanArguments->service = 587;
    }
    ///
    /// Resetting the scan arguments values...
    ///
    m_scanArguments->currentTargetToEnumerate = 0;
    m_scanArguments->progress = 0;
    ///
    /// start Scan...
    ///
    startEnumeration();
}

void Active::on_pushButton_stop_clicked(){
    emit stop();
}

void Active::startEnumeration(){
    ///
    /// if the numner of threads is greater than the number of wordlists, set the
    /// number of threads to use to the number of wordlists provided...
    ///
    int wordlistCount = ui->listWidget_targets->count();
    if(m_scanArguments->maxThreads > wordlistCount)
    {
        m_scanArguments->maxThreads = wordlistCount;
    }
    m_activeThreads = m_scanArguments->maxThreads;
    ///
    /// loop to create threads for enumeration...
    ///
    for(int i = 0; i < m_scanArguments->maxThreads; i++)
    {
        ActiveEnumerator *Enumerator = new ActiveEnumerator(m_scanArguments);
        QThread *cThread = new QThread;
        Enumerator->enumerate(cThread);
        Enumerator->moveToThread(cThread);
        //...
        connect(Enumerator, SIGNAL(scanResult(QString, QString)), this, SLOT(scanResult(QString, QString)));
        connect(Enumerator, SIGNAL(progress(int)), ui->progressBar, SLOT(setValue(int)));
        connect(Enumerator, SIGNAL(scanLog(QString)), this, SLOT(logs(QString)));
        connect(cThread, SIGNAL(finished()), this, SLOT(onThreadEnd()));
        connect(cThread, SIGNAL(finished()), Enumerator, SLOT(deleteLater()));
        connect(cThread, SIGNAL(finished()), cThread, SLOT(deleteLater()));
        connect(this, SIGNAL(stop()), Enumerator, SLOT(onStop()));
        //...
        cThread->start();
    }
    // logs...
    sendStatus("[*] Testing For Active Subdomains...");
    logs("[START] Testing For Active Subdomains...");
}

void Active::scanResult(QString subdomain, QString ipAddress){
    m_model->setItem(m_model->rowCount(), 0, new QStandardItem(subdomain));
    m_model->setItem(m_model->rowCount()-1, 1, new QStandardItem(ipAddress));
    ui->label_resultsCount->setNum(m_model->rowCount());
}

void Active::onThreadEnd(){
    m_endedThreads++;
    if(m_endedThreads == m_activeThreads)
    {
        m_endedThreads = 0;
        //...
        ui->pushButton_start->setEnabled(true);
        ui->pushButton_pause->setDisabled(true);
        ui->pushButton_stop->setDisabled(true);
        //...
        sendStatus("[*] Enumeration Complete!");
        logs("[END] Enumeration Complete!\n");
    }
}

void Active::on_toolButton_config_clicked(){
    /*
    BruteConfigDialog *scanConfig = new BruteConfigDialog(this, m_scanArguments);
    scanConfig->setAttribute( Qt::WA_DeleteOnClose, true );
    scanConfig->show();
    */
}

void Active::on_pushButton_loadTargets_clicked(){
    QString filename = QFileDialog::getOpenFileName(this, INFO_LOADFILE, CURRENT_PATH);
    if(!filename.isEmpty()){
        QFile file(filename);
        if(file.open(QIODevice::ReadOnly | QIODevice::Text)){
            QTextStream in(&file);
            while (!in.atEnd()){
                ui->listWidget_targets->addItem(in.readLine());
            }
            ui->label_targetsCount->setNum(ui->listWidget_targets->count());
            file.close();
        }else{
            QMessageBox::warning(this, TITLE_ERROR, "Failed To Open the File!");
        }
    }
}

void Active::on_pushButton_addTargets_clicked(){
    if(ui->lineEdit_targets->text() != EMPTY){
        ui->listWidget_targets->addItem(ui->lineEdit_targets->text());
        ui->lineEdit_targets->clear();
        ui->label_targetsCount->setNum(ui->listWidget_targets->count());
    }
}

void Active::on_lineEdit_targets_returnPressed(){
    on_pushButton_addTargets_clicked();
}

void Active::on_pushButton_clearTargets_clicked(){
    ui->listWidget_targets->clear();
    ui->label_targetsCount->clear();
    //...
    m_model->setHorizontalHeaderLabels({"Subdomain Name", "IpAddress"});
}

void Active::on_pushButton_removeTargets_clicked(){
    if(ui->listWidget_targets->selectedItems().count()){
        qDeleteAll(ui->listWidget_targets->selectedItems());
    }
    ui->label_targetsCount->setNum(ui->listWidget_targets->selectedItems().count());
}

void Active::on_pushButton_clearResults_clicked(){
    // if the current tab is subdomains clear subdomains
    if(ui->tabWidget_results->currentIndex() == 0)
    {
        m_model->clear();
        ui->label_resultsCount->clear();
        m_model->setHorizontalHeaderLabels({"Subdomain Name:", "IpAddress"});
        //...
        ui->progressBar->clearMask();
        ui->progressBar->reset();
        ui->progressBar->hide();
    }
    // if current tab is logs-tab clear logs...
    else
    {
        ui->listWidget_logs->clear();
    }
}

void Active::logs(QString log){
    sendLog(log);
    ui->listWidget_logs->addItem(log);
    if(log.startsWith("[ERROR]")){
        ui->listWidget_logs->item(ui->listWidget_logs->count()-1)->setForeground(Qt::red);
        return;
    }
    if(log.startsWith("[START]") || log.startsWith("[END]")){
        ui->listWidget_logs->item(ui->listWidget_logs->count()-1)->setFont(QFont("MS Shell Dlg 2", 8, QFont::Bold));
        return;
    }
}

/********************* When User Chnages options For Active Scan *************************/
void Active::on_comboBox_option_currentIndexChanged(int index){
    if(index == ACTIVE_DNS){
        ui->label_details->setText("Resolves the target hostname To it's IpAddress");
    }
    if(index == ACTIVE_HTTP){
        ui->label_details->setText("Resolves the target, if Resolved, Then tests for connection To port 80");
    }
    if(index == ACTIVE_HTTPS){
        ui->label_details->setText("Resolves the target, if Resolved Then tests for connection To port 443");
    }
    if(index == ACTIVE_FTP){
        ui->label_details->setText("Resolves the target, if Resolved Then tests for connection To port 20");
    }
    if(index == ACTIVE_SMTP){
        ui->label_details->setText("Resolves the target, if Resolved Then tests for connection To port 587");
    }
}

void Active::on_radioButton_hostname_clicked(){

}
void Active::on_radioButton_ip_clicked(){

}

void Active::on_pushButton_action_clicked(){
    ///
    /// getting the position of the action button to place the context menu...
    ///
    QPoint pos = ui->pushButton_action->mapToGlobal(QPoint(0,0));
    ///
    /// showing the context menu right by the side of the action button...
    ///
    QMenu *menu = new QMenu(this);
    menu->setAttribute( Qt::WA_DeleteOnClose, true );
    menu->setObjectName("mainMenu");
    //...
    QAction actionSendToSave("Send To Save", this);
    QAction actionSendToMultiLevel("Send To Multi-level Scan");
    QAction actionSendToDnsRecords("Send To DnsRecords");
    //...
    connect(&actionSendToSave, SIGNAL(triggered()), this, SLOT(actionSendToSave()));
    connect(&actionSendToDnsRecords, SIGNAL(triggered()), this, SLOT(actionSendToDnsRecords()));
    connect(&actionSendToMultiLevel, SIGNAL(triggered()), this, SLOT(actionSendToMultiLevel()));
    //...
    menu->addSeparator();
    menu->addAction(&actionSendToDnsRecords);
    menu->addAction(&actionSendToSave);
    menu->addAction(&actionSendToMultiLevel);
    //...
    menu->setStyleSheet("QMenu::item::selected#mainMenu{background-color: rgb(170, 170, 127)} QMenu#mainMenu{background-color: qlineargradient(x1:0,  y1:0, x2:0, y2:1, stop: 0 white, stop: 0.8 rgb(246, 255, 199)); border-style: solid; border-color: black; border-width: 1px;}");
    menu->move(QPoint(pos.x()+76, pos.y()));
    menu->exec();
}

void Active::on_tableView_results_customContextMenuRequested(const QPoint &pos){
    Q_UNUSED(pos);
    ///
    /// check if user right clicked on items else dont show the context menu...
    ///
    if(!ui->tableView_results->selectionModel()->isSelected(ui->tableView_results->currentIndex())){
        return;
    }
    QMenu *menu = new QMenu(this);
    menu->setAttribute( Qt::WA_DeleteOnClose, true );
    menu->setObjectName("mainMenu");
    QAction actionSendToSave("Send Selected To Save", this);
    QAction actionSendToDnsRecords("Send Selected To DnsRecords");
    QAction actionOpenInBrowser("Open Selected in Browser");
    //...
    connect(&actionOpenInBrowser, SIGNAL(triggered()), this, SLOT(cursorOpenInBrowser()));
    connect(&actionSendToSave, SIGNAL(triggered()), this, SLOT(cursorSendToSave()));
    connect(&actionSendToDnsRecords, SIGNAL(triggered()), this, SLOT(cursorSendToDnsRecords()));
    //...
    menu->addAction(&actionOpenInBrowser);
    menu->addSeparator();
    menu->addAction(&actionSendToDnsRecords);
    menu->addAction(&actionSendToSave);
    //...
    QPoint globalCursorPos = QCursor::pos();
    QRect mouseScreenGeometry = qApp->desktop()->screen(qApp->desktop()->screenNumber(globalCursorPos))->geometry();
    QPoint localCursorPosition = globalCursorPos - mouseScreenGeometry.topLeft();
    //...
    menu->setStyleSheet("QMenu::item::selected#mainMenu{background-color: rgb(170, 170, 127)} QMenu#mainMenu{background-color: qlineargradient(x1:0,  y1:0, x2:0, y2:1, stop: 0 white, stop: 0.8 rgb(246, 255, 199)); border-style: solid; border-color: black; border-width: 1px;}");
    menu->move(localCursorPosition);
    menu->exec();
}

void Active::on_pushButton_get_clicked(){

}

void Active::actionSendToSave(){
    /*
    int resultsCount = ui->listWidget_subdomains->count();
    for(int i = 0; i != resultsCount; ++i){
        emit sendResultsToSave(ui->listWidget_subdomains->item(i)->text());
    }
    logs("[*] Sent "+QString::number(resultsCount)+" activeSubdomains Enumerated Subdomains To Save Tab...");
    emit changeTabToSave();
    */
}

void Active::actionSendToDnsRecords(){

}

void Active::actionSendToMultiLevel(){

}

void Active::cursorSendToSave(){
    /*foreach(QListWidgetItem * item, ui->listWidget_subdomains->selectedItems()){
        emit sendResultsToSave(item->text());
    }
    logs("[*] Sent "+QString::number(ui->listWidget_subdomains->count())+" activeSubdomains Enumerated Subdomains To Save Tab...");
    emit changeTabToSave();*/
}

void Active::cursorOpenInBrowser(){
    foreach(const QModelIndex &index, ui->tableView_results->selectionModel()->selectedIndexes()){
        QDesktopServices::openUrl(QUrl("https://"+index.data().toString(), QUrl::TolerantMode));
    }
}

void Active::cursorSendToDnsRecords(){

}