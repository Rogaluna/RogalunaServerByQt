#ifndef ROGALUNAHTTPCONFIG_H
#define ROGALUNAHTTPCONFIG_H

#define ADD_PROPERTY_PTR(type, name)            \
private:                                        \
    type* name##_;                              \
public:                                         \
    /* Get 方法返回指针 */                        \
    type* get##name() const {                   \
        return name##_;                         \
}                                               \
    /* Set 方法接受指针 */                        \
    void set##name(type* value) {               \
        name##_ = value;                        \
}                                               \

class QString;
class RogalunaStorageServer;
class RogalunaDatabaseServer;
class RogalunaAccountServer;
class RogalunaCloudDriveServer;
class RogalunaLibraryServer;
class RogalunaMusicServer;

class RogalunaHttpConfig
{
    ADD_PROPERTY_PTR(QString, WebRootPath)
    ADD_PROPERTY_PTR(QString, Algorithm)
    ADD_PROPERTY_PTR(QString, SecretKey)
    ADD_PROPERTY_PTR(RogalunaStorageServer, StorageServer)
    ADD_PROPERTY_PTR(RogalunaDatabaseServer, DatabaseServer)
    ADD_PROPERTY_PTR(RogalunaAccountServer, AccountServer)
    ADD_PROPERTY_PTR(RogalunaCloudDriveServer, CloudDriveServer)
    ADD_PROPERTY_PTR(RogalunaLibraryServer, LibraryServer)
    ADD_PROPERTY_PTR(RogalunaMusicServer, MusicServer)

public:
    static RogalunaHttpConfig& getInstance() {
        static RogalunaHttpConfig instance; // 唯一实例
        return instance;
    }

    // 删除拷贝构造函数和赋值操作符，防止实例被拷贝
    RogalunaHttpConfig(const RogalunaHttpConfig&) = delete;
    RogalunaHttpConfig& operator=(const RogalunaHttpConfig&) = delete;

private:
    RogalunaHttpConfig() {};
    ~RogalunaHttpConfig() {};
};

#endif // ROGALUNAHTTPCONFIG_H
