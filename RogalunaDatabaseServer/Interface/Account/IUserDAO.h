#ifndef IUSERDAO_H
#define IUSERDAO_H

#include <QString>

struct User {
    int id;
    QString username;
    QString authority;

    User(int id, const QString& username, const QString& authority)
        : id(id), username(username), authority(authority) {}
};

// 接口定义
class IUserDAO {
public:
    virtual ~IUserDAO() {}

    // 注册用户
    virtual std::optional<User> registerUser(const QString& username, const QString& passwordHash,
                                             const std::optional<QString>& authority = std::nullopt) = 0;

    // 删除用户
    virtual bool deleteUser(int userId) = 0;

    // 修改用户
    virtual std::optional<User> updateUser(int userId,
                                           const std::optional<QString>& username = std::nullopt,
                                           const std::optional<QString>& passwordHash = std::nullopt,
                                           const std::optional<QString>& authority = std::nullopt) = 0;

    // 查找用户
    virtual std::optional<User> getUserById(int userId) = 0;
};


#endif // IUSERDAO_H
