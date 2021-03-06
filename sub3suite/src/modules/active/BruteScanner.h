/*
 Copyright 2020-2022 Enock Nicholaus <3nock@protonmail.com>. All rights reserved.
 Use of this source code is governed by GPL-3.0 LICENSE that can be found in the LICENSE file.

 @brief : bruteforce resolving hostnames
*/

#ifndef BRUTESCANNER_H
#define BRUTESCANNER_H

#include <QMutex>
#include <QQueue>
#include <QDnsLookup>
#include <QHostAddress>

#include "AbstractScanner.h"
#include "src/items/HostItem.h"
#include "src/items/WildcardItem.h"


namespace brute {

enum OUTPUT{ // scan type
    SUBDOMAIN,
    TLD
};

struct ScanStat{  // scan statistics
    int nameservers = 0;
    int wordlist = 0;
    int targets = 0;
    int threads = 0;
    int resolved = 0;
    int failed = 0;
};

struct ScanConfig{ // scan configurations
    QDnsLookup::Type recordType = QDnsLookup::A;
    QQueue<QString> nameservers;
    int threads = 50;
    int timeout = 1000;

    bool setTimeout = false;
    bool noDuplicates = false;
    bool autoSaveToProject = false;
    bool checkWildcard = false;
};

struct ScanArgs { // scan arguments
    QMutex mutex;
    QHostAddress nameserver;
    brute::OUTPUT output;
    brute::ScanConfig *config;
    QQueue<QString> targets;
    QStringList wordlist;
    QString currentTarget;
    int currentWordlist;
    int progress;
    bool reScan;
};

class Scanner : public AbstractScanner{
    Q_OBJECT

    public:
        explicit Scanner(brute::ScanArgs *args);
        ~Scanner() override;

        void lookup_wildcard();

    private slots:
        void lookup() override;
        void lookupFinished();
        void lookupFinished_wildcard();

    signals:
        void next(); // next lookup
        void scanResult(s3s_struct::HOST host); // lookup results
        void wildcard(s3s_struct::Wildcard wildcard); // found wildcard

    private:
        brute::ScanArgs *m_args;
        QDnsLookup *m_dns;
        QDnsLookup *m_dns_wildcard;

        /* for wildcards */
        bool has_wildcards = true;
        QString wildcard_ip;
};

RETVAL getTarget_subdomain(brute::Scanner*, QDnsLookup*, ScanArgs*);
RETVAL getTarget_tld(QDnsLookup*, ScanArgs*);
RETVAL getTarget_reScan(QDnsLookup*, ScanArgs*);

}
#endif //BRUTE_H
