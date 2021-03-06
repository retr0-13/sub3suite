#ifndef SITEDOSSIER_H
#define SITEDOSSIER_H

#include "../AbstractOsintModule.h"


namespace ModuleInfo {
struct SiteDossier{
    QString name = OSINT_MODULE_SITEDOSSIER;
    QString url = "https://sitedossier.com/";
    QString url_apiDoc = "";
    QString summary = "";
    QMap<QString, QStringList> flags = {};
    QMap<int, QList<int>> input_output = {{IN_DOMAIN,
                                           {OUT_SUBDOMAIN}}};
};
}

class SiteDossier: public AbstractOsintModule{

    public:
        explicit SiteDossier(ScanArgs args);
        ~SiteDossier() override;

    public slots:
        void start() override;
        void replyFinishedSubdomain(QNetworkReply *reply) override;
};

#endif // SITEDOSSIER_H
