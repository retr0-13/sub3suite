#include "ProjectModel.h"

#include <QFile>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QMessageBox>
#include <QGuiApplication>

#include "src/utils/Config.h"
#include "src/items/SSLItem.h"
#include "src/items/ASNItem.h"


void ProjectModel::openExistingProject(QString name, QString path){
    ProjectStruct project_info;
    project_info.name = name;
    project_info.path = path;
    project_info.isExisting = true;

    /* clear models */
    qDebug() << "Clearing current project models";
    this->clearModels();

    /* open project */
    qDebug() << "Opening Another Project";
    this->openProject(project_info);
}

void ProjectModel::saveProject(){
    qDebug() << "Saving the Project To: " << projectInfo.path;

    QFile file(projectInfo.path);
    if(file.open(QIODevice::WriteOnly))
    {
        /* compress the data then save */
        file.write(qCompress(this->getJson()));
        file.close();

        CONFIG.beginGroup("recent_projects");
        CONFIG.setValue(projectInfo.name, projectInfo.path);
        CONFIG.endGroup();
    }
    else{
        qWarning() << "Failed To Open Project File";
    }
}

void ProjectModel::closeProject(){
    QByteArray project_json(this->getJson());
    if (m_project_hash == QCryptographicHash::hash(project_json, QCryptographicHash::Md5))
        return; // no changes made to project

    int retVal = QMessageBox::warning(nullptr, "Sub3 Suite",
                               "The project has been modified. Do you want to save changes?",
                                   QMessageBox::Save |
                                   QMessageBox::Cancel,
                                   QMessageBox::Save);
    if(retVal == QMessageBox::Save)
        this->saveProject();
}

void ProjectModel::openProject(ProjectStruct projectStruct){
    projectInfo = projectStruct;

    if(projectInfo.isTemporary){
        projectInfo.name = "Temp";
        projectInfo.path = QGuiApplication::applicationDirPath()+"/projects/";

        /* set project name on the project explorer */
        explorer->project->setText(projectInfo.name);
        return;
    }

    if(projectInfo.isNew){
        /* set project name on the project explorer */
        explorer->project->setText(projectInfo.name);
        return;
    }

    /* if it is an existing project */
    /* set project name on the project explorer */
    explorer->project->setText(projectInfo.name);

    /* opening the the project */
    qDebug() << "Opening Project: " << projectInfo.path;

    QFile file(projectInfo.path);
    if(!file.open(QIODevice::ReadOnly)){
        qWarning() << "Failed To Open Project File.";
        return;
    }

    /* uncompress the file then open json */
    QByteArray project_json(qUncompress(file.readAll()));

    /* get the project hash, so as to alert on closing if any changes made */
    m_project_hash = QCryptographicHash::hash(project_json, QCryptographicHash::Md5);

    qDebug() << "Parsing the Project...";
    QJsonDocument document = QJsonDocument::fromJson(project_json);
    file.close();
    if(document.isNull() || document.isEmpty()){
        qWarning() << "Error parsing the project file";
        return;
    }

    QJsonObject mainObj = document.object();

    ///
    /// Project Information
    ///
    QJsonObject info = mainObj["general"].toObject();

    ///
    /// Project Data
    ///
    QJsonObject data = mainObj["data"].toObject();

    /* active Hostnames */
    foreach(const QJsonValue &value, data["active_Host"].toArray()){
        s3s_item::HOST *item = new s3s_item::HOST;
        json_to_host(value.toObject(), item);
        activeHost->appendRow({item, item->ipv4, item->ipv6, item->ports});

        set_Host.insert(item->text(), item);
    }

    /* active Wildcards */
    foreach(const QJsonValue &value, data["active_wildcard"].toArray()){
        s3s_item::Wildcard *item = new s3s_item::Wildcard;
        json_to_wildcard(value.toObject(), item);
        activeWildcard->appendRow({item, item->ipv4, item->ipv6});
    }

    /* active dns */
    foreach(const QJsonValue &value, data["active_dns"].toArray()){
        s3s_item::DNS *item = new s3s_item::DNS;
        json_to_dns(value.toObject(), item);
        activeDNS->appendRow(item);
    }

    /* active SSL sha1 */
    foreach(const QJsonValue &value, data["active_SSL_sha1"].toArray())
        activeSSL_sha1->appendRow(new QStandardItem(value.toString()));

    /* active SSL sha256 */
    foreach(const QJsonValue &value, data["active_SSL_sha256"].toArray())
        activeSSL_sha256->appendRow(new QStandardItem(value.toString()));

    /* active SSL alternativeNames */
    foreach(const QJsonValue &value, data["active_SSL_altNames"].toArray())
        activeSSL_altNames->appendRow(new QStandardItem(value.toString()));

    /* active SSL */
    foreach(const QJsonValue &value, data["active_SSL"].toArray()){
        s3s_item::SSL *item = new s3s_item::SSL;
        json_to_ssl(value.toObject(), item);
        activeSSL->appendRow(item);
    }

    /* active URL */
    foreach(const QJsonValue &value, data["active_URL"].toArray()){
        s3s_item::URL *item = new s3s_item::URL;
        json_to_url(value.toObject(), item);
        activeURL->appendRow({item, item->status_code, item->banner, item->content_type});
    }

    /* passive subdomainIP */
    foreach(const QJsonValue &value, data["passive_subdomainIP"].toArray())
        passiveSubdomainIp->appendRow({new QStandardItem(value.toArray()[0].toString()),
                                                 new QStandardItem(value.toArray()[1].toString())});

    /* passive ASN */
    foreach(const QJsonValue &value, data["passive_ASN"].toArray())
        passiveAsn->appendRow({new QStandardItem(value.toArray()[0].toString()),
                                                 new QStandardItem(value.toArray()[1].toString())});

    /* passive Subdomain */
    foreach(const QJsonValue &value, data["passive_subdomain"].toArray())
        passiveSubdomain->appendRow(new QStandardItem(value.toString()));

    /* passive A */
    foreach(const QJsonValue &value, data["passive_A"].toArray())
        passiveA->appendRow(new QStandardItem(value.toString()));

    /* passive AAAA */
    foreach(const QJsonValue &value, data["passive_AAAA"].toArray())
        passiveAAAA->appendRow(new QStandardItem(value.toString()));

    /* passive CIDR */
    foreach(const QJsonValue &value, data["passive_CIDR"].toArray())
        passiveCidr->appendRow(new QStandardItem(value.toString()));

    /* passive NS */
    foreach(const QJsonValue &value, data["passive_NS"].toArray())
        passiveNS->appendRow(new QStandardItem(value.toString()));

    /* passive MX */
    foreach(const QJsonValue &value, data["passive_MX"].toArray())
        passiveMX->appendRow(new QStandardItem(value.toString()));

    /* passive TXT */
    foreach(const QJsonValue &value, data["passive_TXT"].toArray())
        passiveTXT->appendRow(new QStandardItem(value.toString()));

    /* passive CNAME */
    foreach(const QJsonValue &value, data["passive_CNAME"].toArray())
        passiveCNAME->appendRow(new QStandardItem(value.toString()));

    /* passive Email */
    foreach(const QJsonValue &value, data["passive_Email"].toArray())
        passiveEmail->appendRow(new QStandardItem(value.toString()));

    /* passive URL */
    foreach(const QJsonValue &value, data["passive_URL"].toArray())
        passiveUrl->appendRow(new QStandardItem(value.toString()));

    /* passive SSL */
    foreach(const QJsonValue &value, data["passive_SSL"].toArray())
        passiveSSL->appendRow(new QStandardItem(value.toString()));

    /* enum ASN */
    foreach(const QJsonValue &value, data["enum_ASN"].toArray()){
        s3s_item::ASN *item = new s3s_item::ASN;
        json_to_asn(value.toObject(), item);
        enumASN->appendRow(item);
    }
    /* enum CIDR" */
    foreach(const QJsonValue &value, data["enum_CIDR"].toArray()){
        s3s_item::CIDR *item = new s3s_item::CIDR;
        json_to_cidr(value.toObject(), item);
        enumCIDR->appendRow(item);
    }
    /* enum IP */
    foreach(const QJsonValue &value, data["enum_IP"].toArray()){
        s3s_item::IP *item = new s3s_item::IP;
        json_to_ip(value.toObject(), item);
        enumIp->appendRow(item);
    }
    /* enum MX" */
    foreach(const QJsonValue &value, data["enum_MX"].toArray()){
        s3s_item::MX *item = new s3s_item::MX;
        json_to_mx(value.toObject(), item);
        enumMX->appendRow(item);
    }
    /* enum NS" */
    foreach(const QJsonValue &value, data["enum_NS"].toArray()){
        s3s_item::NS *item = new s3s_item::NS;
        json_to_ns(value.toObject(), item);
        enumNS->appendRow(item);
    }
    /* enum SSL" */
    foreach(const QJsonValue &value, data["enum_SSL"].toArray()){
        s3s_item::SSL *item = new s3s_item::SSL;
        json_to_ssl(value.toObject(), item);
        enumSSL->appendRow(item);
    }
    /* enum Email" */
    foreach(const QJsonValue &value, data["enum_Email"].toArray()){
        s3s_item::Email *item = new s3s_item::Email;
        json_to_email(value.toObject(), item);
        enumEmail->appendRow(item);
    }
    /* raw */
    foreach(const QJsonValue &value, data["enum_Raw"].toArray()){
        s3s_item::RAW *item = new s3s_item::RAW;
        json_to_raw(value.toObject(), item);
        raw->appendRow(item);
    }

    qDebug() << "Project " << projectInfo.name << " Opened.";
}

QByteArray ProjectModel::getJson(){
    QJsonArray passive_SSL_array;
    QJsonArray passive_URL_array;
    QJsonArray passive_Email_array;
    QJsonArray passive_CNAME_array;
    QJsonArray passive_TXT_array;
    QJsonArray passive_NS_array;
    QJsonArray passive_MX_array;
    QJsonArray passive_CIDR_array;
    QJsonArray passive_A_array;
    QJsonArray passive_AAAA_array;
    QJsonArray passive_Subdomain_array;
    QJsonArray passive_ASN_array;
    QJsonArray passive_SubdomainIp_array;

    QJsonArray active_SSL_sha1_array;
    QJsonArray active_SSL_sha256_array;
    QJsonArray active_SSL_altNames_array;
    QJsonArray active_Host_array;
    QJsonArray active_wildcard_array;
    QJsonArray active_SSL_array;
    QJsonArray active_DNS_array;
    QJsonArray active_URL_array;

    QJsonArray enum_IP_array;
    QJsonArray enum_MX_array;
    QJsonArray enum_NS_array;
    QJsonArray enum_ASN_array;
    QJsonArray enum_CIDR_array;
    QJsonArray enum_SSL_array;
    QJsonArray enum_Email_array;
    QJsonArray custom_array;
    QJsonArray raw_array;

    /* passive SSL */
    for(int i = 0; i != passiveSSL->rowCount(); ++i)
        passive_SSL_array.append (passiveSSL->index(i, 0).data().toString());

    /* passive URL */
    for(int i = 0; i != passiveUrl->rowCount(); ++i)
        passive_URL_array.append (passiveUrl->index(i, 0).data().toString());

    /* passive Email */
    for(int i = 0; i != passiveEmail->rowCount(); ++i)
        passive_Email_array.append (passiveEmail->index(i, 0).data().toString());

    /* passive CNAME */
    for(int i = 0; i != passiveCNAME->rowCount(); ++i)
        passive_CNAME_array.append (passiveCNAME->index(i, 0).data().toString());

    /* passive TXT */
    for(int i = 0; i != passiveTXT->rowCount(); ++i)
        passive_TXT_array.append (passiveTXT->index(i, 0).data().toString());

    /* passive NS */
    for(int i = 0; i != passiveNS->rowCount(); ++i)
        passive_NS_array.append (passiveNS->index(i, 0).data().toString());

    /* passive MX */
    for(int i = 0; i != passiveMX->rowCount(); ++i)
        passive_MX_array.append (passiveMX->index(i, 0).data().toString());

    /* passive CIDR */
    for(int i = 0; i != passiveCidr->rowCount(); ++i)
        passive_CIDR_array.append (passiveCidr->index(i, 0).data().toString());

    /* passive A */
    for(int i = 0; i != passiveA->rowCount(); ++i)
        passive_A_array.append (passiveA->index(i, 0).data().toString());

    /* passive AAAA */
    for(int i = 0; i != passiveAAAA->rowCount(); ++i)
        passive_AAAA_array.append (passiveAAAA->index(i, 0).data().toString());

    /* passive Subdomain */
    for(int i = 0; i != passiveSubdomain->rowCount(); ++i)
        passive_Subdomain_array.append (passiveSubdomain->index(i, 0).data().toString());

    /* passive ASN */
    for(int i = 0; i != passiveAsn->rowCount(); ++i){
        QJsonArray asn;
        asn.append (passiveAsn->index(i, 0).data().toString());
        asn.append (passiveAsn->index(i, 1).data().toString());
        passive_ASN_array.append(asn);
    }

    /* passive subdomainIP */
    for(int i = 0; i != passiveSubdomainIp->rowCount(); ++i){
        QJsonArray subdomainIp;
        subdomainIp.append (passiveSubdomainIp->index(i, 0).data().toString());
        subdomainIp.append (passiveSubdomainIp->index(i, 1).data().toString());
        passive_SubdomainIp_array.append(subdomainIp);
    }

    /* active SSL sha1 */
    for(int i = 0; i != activeSSL_sha1->rowCount(); ++i)
        active_SSL_sha1_array.append (activeSSL_sha1->index(i, 0).data().toString());

    /* active SSL sha256 */
    for(int i = 0; i != activeSSL_sha256->rowCount(); ++i)
        active_SSL_sha256_array.append (activeSSL_sha256->index(i, 0).data().toString());

    /* active SSL alternative names */
    for(int i = 0; i != activeSSL_altNames->rowCount(); ++i)
        active_SSL_altNames_array.append (activeSSL_altNames->index(i, 0).data().toString());

    /* active Host */
    for(int i = 0; i != activeHost->rowCount(); ++i){
        QModelIndex index = activeHost->index(i, 1);
        s3s_item::HOST *item = static_cast<s3s_item::HOST*>(activeHost->item(index.row(), 0));

        active_Host_array.append(host_to_json(item));
    }

    /* active Wildcard */
    for(int i = 0; i != activeWildcard->rowCount(); ++i){
        QModelIndex index = activeWildcard->index(i, 1);
        s3s_item::Wildcard *item = static_cast<s3s_item::Wildcard*>(activeHost->item(index.row(), 0));

        active_wildcard_array.append(wildcard_to_json(item));
    }

    /* active DNS */
    for(int i = 0; i < activeDNS->rowCount(); ++i){
        QModelIndex index = activeDNS->index(i, 1);
        s3s_item::DNS *item = static_cast<s3s_item::DNS*>(activeDNS->itemFromIndex(index));

        active_DNS_array.append(dns_to_json(item));
    }

    /* active SSL */
    for(int i = 0; i < activeSSL->rowCount(); ++i){
        QModelIndex index = activeSSL->index(i, 1);
        s3s_item::SSL *item = static_cast<s3s_item::SSL*>(activeSSL->itemFromIndex(index));

        active_SSL_array.append(ssl_to_json(item));
    }

    /* active URL */
    for(int i = 0; i < activeURL->rowCount(); ++i){
        QModelIndex index = activeURL->index(i, 1);
        s3s_item::URL *item = static_cast<s3s_item::URL*>(activeURL->item(index.row(), 0));

        active_URL_array.append(url_to_json(item));
    }

    /* enum ASN */
    for(int i = 0; i < enumASN->rowCount(); ++i){
        QModelIndex index = enumASN->index(i, 1);
        s3s_item::ASN *item = static_cast<s3s_item::ASN*>(enumASN->itemFromIndex(index));

        enum_ASN_array.append(asn_to_json(item));
    }

    /* enum CIDR */
    for(int i = 0; i < enumCIDR->rowCount(); ++i){
        QModelIndex index = enumCIDR->index(i, 1);
        s3s_item::CIDR *item = static_cast<s3s_item::CIDR*>(enumCIDR->itemFromIndex(index));

        enum_CIDR_array.append(cidr_to_json(item));
    }

    /* enum IP */
    for(int i = 0; i < enumIp->rowCount(); ++i){
        QModelIndex index = enumIp->index(i, 1);
        s3s_item::IP *item = static_cast<s3s_item::IP*>(enumIp->itemFromIndex(index));

        enum_IP_array.append(ip_to_json(item));
    }

    /* enum MX */
    for(int i = 0; i < enumMX->rowCount(); ++i){
        QModelIndex index = enumMX->index(i, 1);
        s3s_item::MX *item = static_cast<s3s_item::MX*>(enumMX->itemFromIndex(index));

        enum_MX_array.append(mx_to_json(item));
    }

    /* enum NS */
    for(int i = 0; i < enumNS->rowCount(); ++i){
        QModelIndex index = enumNS->index(i, 1);
        s3s_item::NS *item = static_cast<s3s_item::NS*>(enumNS->itemFromIndex(index));

        enum_NS_array.append(ns_to_json(item));
    }

    /* enum SSL */
    for(int i = 0; i < enumSSL->rowCount(); ++i){
        QModelIndex index = enumSSL->index(i, 1);
        s3s_item::SSL *item = static_cast<s3s_item::SSL*>(enumSSL->itemFromIndex(index));

        enum_SSL_array.append(ssl_to_json(item));
    }

    /* enum Email */
    for(int i = 0; i < enumEmail->rowCount(); ++i){
        QModelIndex index = enumEmail->index(i, 1);
        s3s_item::Email *item = static_cast<s3s_item::Email*>(enumEmail->itemFromIndex(index));

        enum_Email_array.append(email_to_json(item));
    }

    /* raw */
    for(int i = 0; i < raw->rowCount(); ++i){
        QModelIndex index = raw->index(i, 1);
        s3s_item::RAW *item = static_cast<s3s_item::RAW*>(raw->itemFromIndex(index));

        raw_array.append(raw_to_json(item));
    }

    QJsonObject general;
    general.insert("path", "C:/Users/inner peace/Desktop/sub3suite/MSVC2017_64-Release/projects");
    general.insert("name", "one");

    QJsonObject data;
    data.insert("active_Host", active_Host_array);
    data.insert("active_wildcard", active_wildcard_array);
    data.insert("active_dns", active_DNS_array);
    data.insert("active_SSL", active_SSL_array);
    data.insert("active_SSL_sha1", active_SSL_sha1_array);
    data.insert("active_SSL_sha256", active_SSL_sha256_array);
    data.insert("active_SSL_altNames", active_SSL_altNames_array);
    data.insert("active_URL", active_URL_array);
    data.insert("passive_subdomainIP", passive_SubdomainIp_array);
    data.insert("passive_subdomain", passive_Subdomain_array);
    data.insert("passive_A", passive_A_array);
    data.insert("passive_AAAA", passive_AAAA_array);
    data.insert("passive_CIDR", passive_CIDR_array);
    data.insert("passive_NS", passive_NS_array);
    data.insert("passive_MX", passive_MX_array);
    data.insert("passive_TXT", passive_TXT_array);
    data.insert("passive_CNAME", passive_CNAME_array);
    data.insert("passive_Email", passive_Email_array);
    data.insert("passive_URL", passive_URL_array);
    data.insert("passive_ASN", passive_ASN_array);
    data.insert("passive_SSL", passive_SSL_array);
    data.insert("enum_IP", enum_IP_array);
    data.insert("enum_MX", enum_MX_array);
    data.insert("enum_NS", enum_NS_array);
    data.insert("enum_ASN", enum_ASN_array);
    data.insert("enum_CIDR", enum_CIDR_array);
    data.insert("enum_SSL", enum_SSL_array);
    data.insert("enum_Email", enum_Email_array);
    data.insert("custom", custom_array);
    data.insert("enum_Raw", raw_array);

    QJsonObject mainObj;
    mainObj.insert("general", general);
    mainObj.insert("data", data);

    QJsonDocument document;
    document.setObject(mainObj);
    return document.toJson(QJsonDocument::Compact);
}