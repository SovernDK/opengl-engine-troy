#pragma once

class IService 
{
public:
    IService() = default;
    virtual ~IService() = default;

    IService(const IService&) = delete;
    IService& operator=(const IService&) = delete;
};