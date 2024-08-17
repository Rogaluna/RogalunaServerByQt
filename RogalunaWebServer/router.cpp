#include "router.h"

Router::Router() {}

void Router::addRoute(const QString &method, const QString &path, WebRequestHandler *handler)
{
    QString regexPath = QRegularExpression::escape(path);
    regexPath.replace("\\*", ".*");

    Route route{QRegularExpression("^" + regexPath + "$"), handler};
    routes[method].append(route);
}

WebRequestHandler *Router::getHandler(const QString &method, QString &path, QUrlQuery &query) const
{
    QUrl url(path);
    path = url.path();
    query = QUrlQuery(url.query());

    qDebug() << "Method:" << method;
    qDebug() << "Path:" << path;
    qDebug() << "Query:" << url.query();

    if (routes.contains(method)) {
        // 优先匹配具体路径
        for (const Route &route : routes[method]) {
            qDebug() << "Checking route: Regex =" << route.regex.pattern() << ", Path =" << path;
            if (!route.regex.pattern().contains(".*") && route.regex.match(path).hasMatch()) {
                qDebug() << "Matched specific route: Regex =" << route.regex.pattern() << ", Path =" << path;
                return route.handler;
            }
        }

        // 匹配通配符路径
        for (const Route &route : routes[method]) {
            if (route.regex.pattern().contains(".*") && route.regex.match(path).hasMatch()) {
                qDebug() << "Matched wildcard route: Regex =" << route.regex.pattern() << ", Path =" << path;
                return route.handler;
            }
        }
    }
    return nullptr;
}
