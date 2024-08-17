#ifndef ROUTER_H
#define ROUTER_H

#include <QString>
#include <QHash>
#include <QRegularExpression>
#include <QUrlQuery>
#include "WebRequestHandler.h"

class Router
{
public:
    Router();

public:
    void addRoute(const QString &method, const QString &path, WebRequestHandler *handler);
    WebRequestHandler* getHandler(const QString &method, QString &path, QUrlQuery &query) const;

private:
    struct Route {
        QRegularExpression regex;
        WebRequestHandler* handler;
    };
    QHash<QString, QList<Route>> routes; // method -> list of (regex, handler)
};

#endif // ROUTER_H
