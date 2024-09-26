#ifndef REQUESTBODYPARSER_H
#define REQUESTBODYPARSER_H

#ifndef QBYTEARRAY_H
#include <QByteArray>
#endif

#ifndef QLIST_H
#include <QList>
#endif

struct MultipartPart {
    QString name;
    QList<QPair<QByteArray, QByteArray>> headers;
    QByteArray data;
};

/**
 * @function PARSE_MULTIPART_FORM_DATA
 * @brief 解析 requestbody中的表单数据
 * @details 描述函数的详细说明
 * @author Rogaluna
 * @date 2024-09-15
 * @param 参数名 参数的描述
 * @return 返回值的描述
 * @note 任何额外的备注信息
 * @see 相关函数或文档
 * @example 使用示例
 */
#define PARSE_MULTIPART_FORM_DATA(bodyParts, contentType, parsedParts)          \
QList<MultipartPart> parsedParts;                                           \
    do {                                                                        \
        /* 使用索引提取 boundary */                                             \
        int boundaryPos = contentType.indexOf("boundary=");                     \
        if (boundaryPos == -1) {                                                \
            qWarning("Invalid contentType: No boundary found.");                \
            break;                                                              \
    }                                                                       \
        QByteArray boundary = "--" + contentType.mid(boundaryPos + 9).toUtf8(); \
        QByteArray endBoundary = boundary + "--";                               \
                                                                                \
        int boundaryIndex = bodyParts.indexOf(boundary);                        \
        if (boundaryIndex == -1) {                                              \
            qWarning("Boundary not found in body parts.");                      \
            break;                                                              \
    }                                                                       \
                                                                                \
        /* 查找每一个 Part 并解析 */                                            \
        int partStartPos = boundaryIndex + boundary.length();                   \
        while (partStartPos < bodyParts.size()) {                               \
            int nextBoundaryIndex = bodyParts.indexOf(boundary, partStartPos);  \
            if (nextBoundaryIndex == -1) {                                      \
                nextBoundaryIndex = bodyParts.indexOf(endBoundary, partStartPos); \
                if (nextBoundaryIndex == -1) break;  /* 没有更多的部分 */          \
        }                                                                   \
                                                                                \
            QByteArray part = bodyParts.mid(partStartPos, nextBoundaryIndex - partStartPos); \
            partStartPos = nextBoundaryIndex + boundary.length();               \
                                                                                \
            /* 从 Part 中提取头部和内容 */                                       \
            int headerEndPos = part.indexOf("\r\n\r\n");                        \
            if (headerEndPos == -1) {                                           \
                qWarning("Invalid part: No header section found.");             \
                continue;                                                       \
        }                                                                   \
                                                                                \
            QByteArray headerSection = part.left(headerEndPos).trimmed();       \
            QByteArray data = part.mid(headerEndPos + 4);                       \
                                                                                \
            /* 解析 headers 和 name 属性 */                                      \
            MultipartPart multipartPart;                                        \
            QList<QByteArray> headerLines = headerSection.split('\r');          \
            for (const auto &headerLine : headerLines) {                        \
                QList<QByteArray> headerPair = headerLine.trimmed().split(':'); \
                if (headerPair.size() == 2) {                                   \
                    multipartPart.headers.append(qMakePair(headerPair[0].trimmed(), headerPair[1].trimmed())); \
            }                                                               \
                                                                                \
                /* 查找 "name" 属性 */                                           \
                int namePos = headerLine.indexOf("name=\"");                    \
                if (namePos != -1) {                                            \
                    int nameEnd = headerLine.indexOf("\"", namePos + 6);        \
                    multipartPart.name = QString::fromUtf8(headerLine.mid(namePos + 6, nameEnd - (namePos + 6))).trimmed(); \
            }                                                               \
        }                                                                   \
                                                                                \
            /* 将数据存储到结构体中 */                                           \
            multipartPart.data = data.chopped(2);                               \
            parsedParts.append(multipartPart);                                  \
    }                                                                       \
} while (0)


#endif // REQUESTBODYPARSER_H
