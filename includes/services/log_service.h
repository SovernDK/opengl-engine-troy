#pragma once 
#include "services/service.h"

#include <string>
#include <stack>
#include <format>

enum class LogLevel
{
    Info, Warn, Error
};

enum class CategoryLevel
{
	General, ECS, Audio, Render, Files, TTFFont
};

struct Log
{
    std::string time;
    std::string level;
    std::string category;
    std::string message;
    int repeatCount = 0;

    std::string text()
    {
        return std::format("{} {}::{} {}    x{}", time, level, category, message, repeatCount);
    }
};

class ILogger: IService
{
public:
    virtual ~ILogger() = default;

    // Add string args and handle message throught std::format 
    virtual void log(LogLevel level, CategoryLevel category, std::string message) = 0;

    virtual void log(std::string message) = 0;
	virtual void log(CategoryLevel category, std::string message) = 0;

    virtual void error(std::string message) = 0;
    virtual void error(CategoryLevel category, std::string message) = 0;

    virtual void warn(std::string message) = 0;
    virtual void warn(CategoryLevel category, std::string message) = 0;

    virtual void quit() = 0;
};

class LoggerService : public ILogger
{
private:
    const char* TRESET   = "\033[0m";
    const char* TRED     = "\033[31m";
    const char* TGREEN   = "\033[32m";
    const char* TYELLOW  = "\033[33m";
    const char* TBLUE    = "\033[34m";
    const char* TMAGENTA = "\033[35m";
    const char* TCYAN    = "\033[36m";
    const char* TWHITE   = "\033[37m";

    std::stack<Log> sessionLogs;
public:
    void log(LogLevel level, CategoryLevel category, std::string message) override;

    void log(std::string message) override;
    void log(CategoryLevel category, std::string message) override;

    void error(std::string message) override;
    void error(CategoryLevel category, std::string message) override;

    void warn(std::string message) override;
    void warn(CategoryLevel category, std::string message) override;

    void quit() override;

    const char* getColorByLevel(LogLevel lvl);
    std::string getCurrentTime();
};