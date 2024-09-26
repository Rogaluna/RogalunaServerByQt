#include "DefaultOptionsHandler.h"

#include <QDebug>
#include <QHttpServerRequest>
#include <QUrl>

QHttpServerResponse DefaultOptionsHandler::handleRequest(QUrl arg, const QHttpServerRequest &request)
{
    Q_UNUSED(arg)
    Q_UNUSED(request)

    QHttpServerResponse response("Token has been verify.", QHttpServerResponse::StatusCode::Ok);
    response.setHeader("Access-Control-Allow-Origin", "*"); // 允许跨域
    response.addHeader("Access-Control-Allow-Methods", "*");
    response.addHeader("Access-Control-Allow-Headers", "*");
    return response;
}
