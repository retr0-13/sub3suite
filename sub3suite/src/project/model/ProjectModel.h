/*
 Copyright 2020-2022 Enock Nicholaus <3nock@protonmail.com>. All rights reserved.
 Use of this source code is governed by GPL-3.0 LICENSE that can be found in the LICENSE file.

 @brief :the main project data model
*/

#ifndef PROJECTMODEL_H
#define PROJECTMODEL_H

#include <QSet>
#include <QStandardItemModel>

#include "src/items/ASNItem.h"
#include "src/items/CIDRItem.h"
#include "src/items/IPItem.h"
#include "src/items/MXItem.h"
#include "src/items/NSItem.h"
#include "src/items/EmailItem.h"
#include "src/items/DNSItem.h"
#include "src/items/URLItem.h"
#include "src/items/HostItem.h"
#include "src/items/RawItem.h"
#include "src/items/WildcardItem.h"
#include "src/modules/active/DNSScanner.h"
#include "src/modules/active/SSLScanner.h"

#include "Explorer.h"

/* TODO:
 *      compress & decompress data before saving and loading project,
 *      use a fast compression library eg https://github.com/lz4/lz4
 */
struct ProjectStruct{
    QString name;
    QString path;
    bool isNew = false;
    bool isExisting = false;
    bool isTemporary = false;
    bool isConfigured = false;
};


class ProjectModel
{

public:
    ProjectModel();
    ~ProjectModel();

    /* serialization */
    void openProject(ProjectStruct project);
    void openExistingProject(QString name, QString path);
    void saveProject();
    void closeProject();

    /* actions */
    void clearModels();
    void setHeaderLabels();

    ProjectStruct projectInfo;

    /* for active results */
    void addActiveHost(const s3s_struct::HOST &host);
    void addActiveWildcard(const s3s_struct::Wildcard &wildcard);
    void addActiveDNS(const s3s_struct::DNS &dns);
    void addActiveURL(const s3s_struct::URL &url);
    void addActiveSSL(const QString &target, const QSslCertificate &cert);
    void addActiveSSL(const QByteArray &cert);
    void addActiveSSL_hash(const QString &hash);
    void addActiveSSL_sha1(const QString &sha1);
    void addActiveSSL_sha256(const QString &sha256);
    void addActiveSSL_altNames(const QString &alternative_names);

    /* for passive results */
    void addPassiveSubdomainIp(const QString &subdomain, const QString &ip);
    void addPassiveSubdomain(const QString &subdomain);
    void addPassiveIp(const QString &ip);
    void addPassiveA(const QString &ipv4);
    void addPassiveAAAA(const QString &ipv6);
    void addPassiveCidr(const QString &cidr);
    void addPassiveNS(const QString &ns);
    void addPassiveMX(const QString &mx);
    void addPassiveTXT(const QString &txt);
    void addPassiveCNAME(const QString &cname);
    void addPassiveEMail(const QString &email);
    void addPassiveUrl(const QString &url);
    void addPassiveAsn(const QString &asn, const QString &name);
    void addPassiveSSL(const QString &ssl);

    /* for enum */
    void addEnumASN(const s3s_struct::ASN &asn);
    void addEnumCIDR(const s3s_struct::CIDR &cidr);
    void addEnumIP(const s3s_struct::IP &ip);
    void addEnumNS(const s3s_struct::NS &ns);
    void addEnumMX(const s3s_struct::MX &xm);
    void addEnumSSL(const QString &target, const QSslCertificate &ssl);
    void addEnumEmail(const s3s_struct::Email &email);

    /* for raw */
    void addRaw(const s3s_struct::RAW &raw);

public:
    Explorer *explorer;
    /* active Results Model */
    QStandardItemModel *activeHost;
    QStandardItemModel *activeWildcard;
    QStandardItemModel *activeDNS;
    QStandardItemModel *activeA;
    QStandardItemModel *activeAAAA;
    QStandardItemModel *activeNS;
    QStandardItemModel *activeMX;
    QStandardItemModel *activeTXT;
    QStandardItemModel *activeCNAME;
    QStandardItemModel *activeSRV;
    QStandardItemModel *activeSSL;
    QStandardItemModel *activeSSL_sha1;
    QStandardItemModel *activeSSL_sha256;
    QStandardItemModel *activeSSL_altNames;
    QStandardItemModel *activeURL;

    /* passive results model */
    QStandardItemModel *passiveSubdomainIp;
    QStandardItemModel *passiveSubdomain;
    QStandardItemModel *passiveA;
    QStandardItemModel *passiveAAAA;
    QStandardItemModel *passiveCidr;
    QStandardItemModel *passiveNS;
    QStandardItemModel *passiveMX;
    QStandardItemModel *passiveTXT;
    QStandardItemModel *passiveCNAME;
    QStandardItemModel *passiveEmail;
    QStandardItemModel *passiveUrl;
    QStandardItemModel *passiveAsn;
    QStandardItemModel *passiveSSL;

    /* enum Results model */
    QStandardItemModel *enumIp;
    QStandardItemModel *enumASN;
    QStandardItemModel *enumCIDR;
    QStandardItemModel *enumNS;
    QStandardItemModel *enumMX;
    QStandardItemModel *enumSSL;
    QStandardItemModel *enumEmail;

    QStandardItemModel *raw;

    /* sets */
    QMap<QString, s3s_item::HOST*> set_Host;

private:
    QByteArray getJson();
    QByteArray m_project_hash;
};

#endif // PROJECTMODEL_H