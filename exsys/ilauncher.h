#ifndef ILAUNCHER_H
#define ILAUNCHER_H

#include <list>
#include <string>
#include "pluginmacros.h"

namespace exsys{

#define ILAUNCHER_IID "com.noitom.mocap.extensionsystem.ilauncher"

class ILauncher
{
public:
    virtual ~ILauncher(){}

    virtual int GetLaunchPriority() = 0;

    virtual bool StartUp(std::string *errMsg) = 0;

    virtual void Shutdown() = 0;
};

template <int _Priority>
class IPrioritizedLauncher: public ILauncher
{
public:
    virtual int GetLaunchPriority() override { return _Priority; }
};

typedef IPrioritizedLauncher<11> IServiceLauncher;
typedef IPrioritizedLauncher<12> IServiceLauncher2;
typedef IPrioritizedLauncher<13> IServiceLauncher3;
typedef IPrioritizedLauncher<14> IServiceLauncher4;
typedef IPrioritizedLauncher<15> IServiceLauncher5;
typedef IPrioritizedLauncher<16> IServiceLauncher6;
typedef IPrioritizedLauncher<17> IServiceLauncher7;
typedef IPrioritizedLauncher<18> IServiceLauncher8;
typedef IPrioritizedLauncher<19> IServiceLauncher9;

typedef IPrioritizedLauncher<101> IClientLauncher;

}// exsys

#endif // ILAUNCHER_H
