#ifndef WEBREQUESTHANDLER_H
#define WEBREQUESTHANDLER_H

#include <QTcpSocket>
#include <QUrlQuery>

struct MultipartPart {
    QMap<QString, QString> headers;  // 每个部分的头部
    QByteArray content;              // 每个部分的内容
};

class WebRequestHandler
{
public:
    virtual ~WebRequestHandler() = default;

    // 处理 Http 请求
    virtual void handleRequest(QTcpSocket *socket, const QString &path, const QUrlQuery &query, const QByteArray &data) = 0;

protected:
    void sendResponse(QTcpSocket *socket, const QByteArray &content, const QString &contentType)
    {
        QByteArray response;
        response.append("HTTP/1.1 200 OK\r\n");
        response.append("content-type: " + contentType.toUtf8() + "\r\n");
        response.append("content-length: " + QByteArray::number(content.size()) + "\r\n");
        response.append("\r\n");
        response.append(content);

        socket->write(response);
        socket->flush();
        socket->disconnectFromHost();
    }

    void sendErrorResponse(QTcpSocket *socket, const QString &message)
    {
        QByteArray response;
        response.append("HTTP/1.1 400 Bad Request\r\n");
        response.append("content-type: text/plain\r\n");
        response.append("content-length: " + QByteArray::number(message.toUtf8().size()) + "\r\n");
        response.append("\r\n");
        response.append(message.toUtf8());

        socket->write(response);
        socket->flush();
        socket->disconnectFromHost();
    }

    // 解析数据 first 是头数据, second 是体数据
    QPair<QByteArray, QByteArray> parseRequest(const QByteArray &data) const
    {
        // 查找头部结束位置，即 \r\n\r\n
        int headerEndIndex = data.indexOf("\r\n\r\n");
        if (headerEndIndex == -1) {
            // 如果未找到 \r\n\r\n，说明请求格式无效
            qDebug() << "Invalid HTTP request: no header found.";
            return QPair<QByteArray, QByteArray>(QByteArray(), QByteArray());
        }

        // 提取头部数据
        QByteArray headerData = data.left(headerEndIndex);

        // 提取体数据，从 \r\n\r\n 之后开始
        QByteArray bodyData = data.mid(headerEndIndex + 4);

        return QPair<QByteArray, QByteArray>(headerData, bodyData);
    }

    // 解析头数据
    QMap<QString, QString> parseHeaders(const QByteArray &headerData) const
    {
        QMap<QString, QString> headers;
        QList<QByteArray> lines = headerData.split('\n');

        for (const QByteArray &line : lines) {
            int separatorIndex = line.indexOf(':');
            if (separatorIndex != -1) {
                QString key = QString::fromUtf8(line.left(separatorIndex).trimmed()).toLower();
                QString value = QString::fromUtf8(line.mid(separatorIndex + 1).trimmed());
                headers.insert(key, value);
            }
        }

        return headers;
    }

    // 解析 boundary
    QByteArray parseBoundary(const QMap<QString, QString> &headers) const
    {
        QString contentType = headers.value("content-type");
        QString boundary;
        if (contentType.startsWith("multipart/form-data")) {
            // 提取 boundary
            int boundaryIndex = contentType.indexOf("boundary=");
            if (boundaryIndex != -1) {
                boundary = contentType.mid(boundaryIndex + 9);
            }
        }

        if (!boundary.isEmpty()) {
            QByteArray boundaryBytes = ("--" + boundary).toUtf8();
            return boundaryBytes;
        }

        return nullptr;
    }

    // 解析请求体
    QList<MultipartPart> parseBody(const QByteArray &bodyData, const QByteArray &boundary) const
    {
        QList<MultipartPart> partsList;

        int startPos = 0;
        while ((startPos = bodyData.indexOf(boundary, startPos)) != -1) {
            startPos += boundary.size();

            while (startPos < bodyData.size() && (bodyData[startPos] == '\r' || bodyData[startPos] == '\n')) {
                startPos++;
            }

            int endPos = bodyData.indexOf(boundary, startPos);
            if (endPos == -1) {
                endPos = bodyData.size();
            }

            QByteArray part = bodyData.mid(startPos, endPos - startPos).trimmed();

            int headerEndIndex = part.indexOf("\r\n\r\n");
            if (headerEndIndex == -1) {
                continue;
            }

            QByteArray partHeader = part.left(headerEndIndex);
            QByteArray partContent = part.mid(headerEndIndex + 4);

            // 使用 parseHeaders 函数解析局部头部
            QMap<QString, QString> partHeaders = parseHeaders(partHeader);

            MultipartPart multipartPart;
            multipartPart.headers = partHeaders;
            multipartPart.content = partContent;

            partsList.append(multipartPart);

            startPos = endPos;
        }

        return partsList;
    }

};

#endif // WEBREQUESTHANDLER_H
