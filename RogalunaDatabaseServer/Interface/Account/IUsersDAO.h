#ifndef IUSERSDAO_H
#define IUSERSDAO_H

#include <QJsonObject>
#include <QString>

namespace Account {

// 接口定义
class IUserDAO {
public:
    virtual ~IUserDAO() {}

    // 注册用户
    virtual QJsonObject registerUser(const QString& username, const QString& passwordHash,
                                             const std::optional<QString>& authority = std::nullopt) = 0;

    // 删除用户
    virtual bool deleteUser(int userId) = 0;

    // 修改用户
    virtual QJsonObject updateUser(int userId,
                                           const std::optional<QString>& username = std::nullopt,
                                           const std::optional<QString>& passwordHash = std::nullopt,
                                           const std::optional<QString>& authority = std::nullopt) = 0;

    // 查找用户
    virtual QJsonObject getUserById(int userId) = 0;
    virtual QJsonObject getUserByName(QString username) = 0;
};

}

#endif // IUSERSDAO_H
