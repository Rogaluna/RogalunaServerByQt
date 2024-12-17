#ifndef TOKENGV_H
#define TOKENGV_H

#ifndef QCRYPTOGRAPHICHASH_H
#include <QCryptographicHash>
#endif

#ifndef QDATETIME_H
#include <QDateTime>
#endif

#ifndef QJSONWEBTOKEN_H
#include <QJsonWebToken.h>
#endif

#define GENERATE_JWT_TOKEN(claims, algorithm, secret) \
[&]() -> QString { \
        QJsonWebToken jwtObj; \
        if (QString(secret).isEmpty()) { \
            jwtObj.setRandomSecret(); \
    } else { \
            jwtObj.setSecret(secret); \
    } \
        for (const auto& claim : claims) { \
            jwtObj.appendClaim(claim.first, claim.second); \
    } \
        jwtObj.setAlgorithmStr(algorithm); \
        return jwtObj.getToken(); \
}()

#define VERIFY_JWT_TOKEN(strToken, strSecret) \
[&]() -> std::tuple<bool, QString, QString> { \
        QStringList listJwtParts = strToken.split("."); \
        if (listJwtParts.count() != 3) { \
            qDebug() << "Invalid JWT Token: Incorrect format"; \
            return std::make_tuple(false, "", ""); \
    } \
        QJsonWebToken jwtObj = QJsonWebToken::fromTokenAndSecret(strToken, strSecret); \
        QString strHeader = jwtObj.getHeaderQStr(); \
        QString strPayload = jwtObj.getPayloadQStr(); \
        if (!jwtObj.isValid()) { \
            qDebug() << "Invalid JWT Token: Verification failed"; \
            return std::make_tuple(false, strHeader, strPayload); \
    } \
        return std::make_tuple(true, strHeader, strPayload); \
}()

#define CHECK_TIMESTAMPS(jsonObj) \
[&]() -> int { \
        qint64 __currentTime__ = QDateTime::currentSecsSinceEpoch(); \
        if (!(jsonObj).contains("iat") || !(jsonObj).contains("exp")) { \
            return 0; /* 缺少 'iat' 或 'exp' */ \
    } \
        qint64 __iat__ = (jsonObj).value("iat").toVariant().toLongLong(); \
        qint64 __exp__ = (jsonObj).value("exp").toVariant().toLongLong(); \
        if (__iat__ > __currentTime__) { \
            return 1; /* 'iat' 是未来的时间 */ \
    } \
        if (__exp__ < __currentTime__) { \
            return 2; /* 'exp' 已经过期 */ \
    } \
        return 3; /* 成功，token 有效 */ \
}()

#endif // TOKENGV_H
