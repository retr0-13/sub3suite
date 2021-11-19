#include "Apnic.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#define IP 0

/*
 * not well implemented yet for osint, only raw...
 */
Apnic::Apnic(ScanArgs *args): AbstractOsintModule(args)
{
    manager = new MyNetworkAccessManager(this);
    log.moduleName = "Apnic";

    if(args->raw)
        connect(manager, &MyNetworkAccessManager::finished, this, &Apnic::replyFinishedRaw);
}
Apnic::~Apnic(){
    delete manager;
}

void Apnic::start(){
    QNetworkRequest request;

    QUrl url;
    if(args->raw){
        switch (args->rawOption) {
        case IP:
            url.setUrl("http://rdap.apnic.net/ip/"+args->target);
            break;
        }
        request.setUrl(url);
        manager->get(request);
        activeRequests++;
        return;
    }
}