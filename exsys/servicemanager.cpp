#include "servicemanager.h"
#include "assert.h"

namespace exsys {

static ServiceManager *g_inst = nullptr;

ServiceManager::ServiceManager()
{
    if(g_inst != nullptr){
        throw "Service can only instance once!";
    }

    g_inst = this;
}

ServiceManager::~ServiceManager()
{
    g_inst = nullptr;
}

ServiceManager *ServiceManager::Get()
{
    if(g_inst == nullptr){
        throw "Service doesn't have a valid instance!";
    }

    return g_inst;
}

void ServiceManager::InstallService(const std::string &service_iid, void *service_interface)
{
    services_[service_iid].push_back(service_interface);
}

void ServiceManager::UninstallService(const std::string &service_iid, void *service_interface)
{
    if(services_.count(service_iid) == 0){
        return;
    }

    for(auto iter = services_[service_iid].begin(); iter != services_[service_iid].end(); ++iter){
        if(*iter == service_interface){
            services_[service_iid].erase(iter);
            break;
        }
    }

    if(services_[service_iid].size() == 0){
        services_.erase(service_iid);
    }
}

}
