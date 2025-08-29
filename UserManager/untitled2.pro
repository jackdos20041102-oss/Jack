#-------------------------------------------------
# 智慧医疗管理系统服务器端项目配置文件
#-------------------------------------------------

TEMPLATE = app
TARGET = MedicalSystemServer

# Qt模块配置
QT += core network sql
QT -= gui

# C++标准配置
CONFIG += c++11 console
CONFIG -= app_bundle

# 编译器配置
DEFINES += QT_DEPRECATED_WARNINGS

# 源文件列表
SOURCES += \
    main.cpp \
    DatabaseManager.cpp \
    UserManager.cpp \
    ServerManager.cpp

# 头文件列表
HEADERS += \
    UserInfo.h \
    DatabaseManager.h \
    UserManager.h \
    ServerManager.h

# 包含路径
INCLUDEPATH += .

# 输出目录配置
CONFIG(debug, debug|release) {
    DESTDIR = debug
    OBJECTS_DIR = debug/obj
    MOC_DIR = debug/moc
} else {
    DESTDIR = release
    OBJECTS_DIR = release/obj
    MOC_DIR = release/moc
}

# 版本信息
VERSION = 1.0.0

# 编译器优化
QMAKE_CXXFLAGS += -Wall

# 多线程支持
CONFIG += thread

# 清理配置
QMAKE_CLEAN += $(TARGET)
