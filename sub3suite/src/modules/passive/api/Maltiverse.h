#ifndef MALTIVERSE_H
#define MALTIVERSE_H

#include "../AbstractOsintModule.h"


namespace ModuleInfo {
struct Maltiverse{
    QString name = OSINT_MODULE_MALTIVERSE;
    QString url = "https://maltiverse.com/";
    QString url_apiDoc = "https://app.swaggerhub.com/apis-docs/maltiverse/api/1.0.0-oas3";
    QString summary = "Maltiverse";
    QMap<QString, QStringList> flags = {{"IpV4",
                                         {PLACEHOLDERTEXT_IP4, "IPv4 address IoC related methods"}},
                                        {"Hostname",
                                         {PLACEHOLDERTEXT_DOMAIN, "Hostname IoC related methods"}},
                                        {"Url",
                                         {PLACEHOLDERTEXT_CHECKSUM, "Url IOC related methods"}}};

    QMap<int, QList<int>> input_output = {{IN_DOMAIN,
                                           {OUT_IP, OUT_ASN}},
                                          {IN_IP,
                                           {OUT_EMAIL, OUT_ASN, OUT_CIDR}}};
};
}

class Maltiverse: public AbstractOsintModule{

    public:
        explicit Maltiverse(ScanArgs args);
        ~Maltiverse() override;

    public slots:
        void start() override;
        void replyFinishedAsn(QNetworkReply *reply) override;
        void replyFinishedIp(QNetworkReply *reply) override;
        void replyFinishedEmail(QNetworkReply *reply) override;
        void replyFinishedCidr(QNetworkReply *reply) override;

    private:
        QString m_key;
};
#endif // MALTIVERSE_H
