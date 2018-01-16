#ifndef SERVICEMANAGER_H
#define SERVICEMANAGER_H

#include "exsys_global.h"
#include <vector>
#include <map>

namespace exsys {

class EXTENSIONSYSTEM_DECL ServiceManager
{
public:
    ServiceManager();
    ~ServiceManager();

    static ServiceManager *Get();

    void InstallService(const std::string &service_iid, void *service_interface);
    void UninstallService(const std::string &service_iid, void *service_interface);

    template <typename T>
    T *QueryService(const std::string &iid)
    {
        T *ret = nullptr;
        if(services_.count(iid) > 0){
            ret = reinterpret_cast<T *>(services_[iid][0]);
        }

        return ret;
    }

    template <typename T>
    std::vector<T *> QueryServices(const std::string &iid)
    {
        std::vector<T *> services;
        if(services_.count(iid) > 0){
            std::vector<void *> void_services = services_[iid];
            for(int i = 0; i < void_services.size(); ++i){
                services.push_back(reinterpret_cast<T *>(void_services[i]));
            }
        }
    }

private:
    std::map<std::string, std::vector<void *>> services_;
};

}
#endif // SERVICEMANAGER_H
