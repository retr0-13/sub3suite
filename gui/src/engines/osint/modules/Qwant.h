#ifndef QWANT_H
#define QWANT_H

#include "AbstractModule.h"

class Qwant: public AbstractModule{

    public:
        Qwant(QString target = nullptr, QObject *parent = nullptr);
        ~Qwant() override;

    public slots:
        void start() override;
        void replyFinished(QNetworkReply *) override;

    private:
        int m_offset = 0;
};

#endif // QWANT_H
