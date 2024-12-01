# RogalunaServerByQt

提供给 [rogaluna-web](https://github.com/Rogaluna/rogaluna-web) 的服务程序，使用 mingw 编译，支持 Windows 和 Linux 平台。需要配置Postgre数据库，可在运行后编辑自动生成的db_config.ini文件进行配置。

## 数据库

未提供

## 模块列表

***QJsonWebToken***  jwt验证生成器
***QTagLib***  taglib的Qt兼容库，提供音频元数据的获取和编辑

## 第三方库

> 需要编译对应的第三方库

***bcrypt***  提供加密方法

## 内部库

### *RogalunaConfigReader* 提供配置文件读取、生成的方法

* CREATE_CONFIG_HANDLER(configName)
  创建配置文件句柄
  
  ```C++
  auto configHandler = CREATE_CONFIG_HANDLER("config.ini");
  ```
  
  该宏声明一个配置文件句柄，会根据输入的文件名称分配生成的类型，比如扩展名ini的文件，就会生成INIHandler。如果声明了无法处理的文件类型，则会返回空值。目前支持的类型：
  
  | 支持的类型 |
| ---------- |
| INI        |
  
  
* VALIDATE_CONFIG(handler, defaultConfig, inputLogichandler)
  验证句柄，处理配置文件
  
  ```C++
  RogalunaConfigurator::ConfigData defaultConfig;
      defaultConfig["Storage"] = {
          {"root", "storage"},
          {"temp", "temp"},
          {"tempFilePrefix", "chunk_"}
      };
      defaultConfig["Logging"] = {
          {"level", "info"},
          {"path", "logs"}
      };
  
      VALIDATE_CONFIG(configHandler, defaultConfig, [](RogalunaConfigurator::ConfigData& configData) {
          QTextStream in(stdin);
  
          if (configData.contains("Storage")) {
              qDebug() << "Enter storage root path (default: storage):";
              QString rootInput = in.readLine().trimmed();
              if (!rootInput.isEmpty()) {
                  configData["Storage"]["root"] = rootInput;
              }
          }
  
          if (configData.contains("Logging")) {
              qDebug() << "Enter logging level (default: info):";
              QString levelInput = in.readLine().trimmed();
              if (!levelInput.isEmpty()) {
                  configData["Logging"]["level"] = levelInput;
              }
          }
      });
  ```
  
  该宏将会验证传入的句柄是否可用，如果可用（配置文件存在并且可以读取），则允许后续读取配置值，如果不可用（配置文件不存在或无法读取），则根据第二个参数defaultConfig生成配置文件，第三个参数是一个匿名函数，可以在这个匿名函数中设置程序运行中允许用户配置的值。以上示例仅在终端中可用。
* READ_CONFIG(handler)
  读取配置数据
  
  ```C++
  RogalunaConfigurator::ConfigData configData = READ_CONFIG(configHandler);
  
    // 输出配置值
    QString rootPath = configData["Storage"].value("root", "default_root").toString();
    QString logLevel = configData["Logging"].value("level", "default_level").toString();
  ```
  
  该宏将会读取配置文件中的内容，输出一个ConfigData（QMap<QString, QMap<QString, QVariant>>）类型的值。可以使用这个数据索引到键上设置的值。

