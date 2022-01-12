#include "EmailFormat.h"
#include "src/utils/Config.h"

#define BEST_FORMATS 0
#define FORMATS 1
#define PING 2


EmailFormat::EmailFormat(ScanArgs args): AbstractOsintModule(args)
{
    manager = new s3sNetworkAccessManager(this);
    log.moduleName = "EmailFormat";

    if(args.outputRaw)
        connect(manager, &s3sNetworkAccessManager::finished, this, &EmailFormat::replyFinishedRawJson);
    ///
    /// getting api-key...
    ///
    
    m_key = APIKEY.value("emailformat").toString();
    
}
EmailFormat::~EmailFormat(){
    delete manager;
}

void EmailFormat::start(){
    QNetworkRequest request;
    request.setRawHeader("Content-Type", "application/json");
    request.setRawHeader("Authorization", m_key.toUtf8());

    QUrl url;
    if(args.outputRaw){
        switch(args.rawOption){
        case FORMATS:
            url.setUrl("https://www.email-format.com/api/v2/get_formats?domain="+target);
            break;
        case BEST_FORMATS:
            url.setUrl("https://www.email-format.com/api/v2/get_best_format?domain="+target);
            break;
        case PING:
            url.setUrl("https://www.email-format.com/api/v2/ping");
            break;
        }
        request.setUrl(url);
        manager->get(request);
        activeRequests++;
    }
}