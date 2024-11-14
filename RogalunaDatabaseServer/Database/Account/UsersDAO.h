#ifndef USERSDAO_H
#define USERSDAO_H

#include <BaseDAO.h>
#include <Interface/Account/IUsersDAO.h>

namespace Account {

class ROGALUNADATABASESERVER_EXPORT UserDAO : public BaseDAO, public IUserDAO
{
public:
    explicit UserDAO(QSqlDatabase& db, const QString& schema = "account", const QString& tableName = "users");

    // 注册用户
    QJsonObject registerUser(const QString& username, const QString& passwordHash,
                                     const std::optional<QString>& authority = std::nullopt) override;

    // 删除用户
    bool deleteUser(int userId) override;

    // 修改用户
    QJsonObject updateUser(int userId,
                                   const std::optional<QString>& username = std::nullopt,
                                   const std::optional<QString>& passwordHash = std::nullopt,
                                   const std::optional<QString>& authority = std::nullopt) override;

    // 查找用户
    QJsonObject getUserById(int userId) override;
    QJsonObject getUserByName(QString username) override;
};

}
#endif // USERSDAO_H
