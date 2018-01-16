#ifndef PLUGINLOADER_H
#define PLUGINLOADER_H

#include <map>
#include "classentry.h"
#include "exsys_global.h"

namespace exsys
{

class EXTENSIONSYSTEM_DECL PluginLoader
{
public:
    PluginLoader(const std::string &file_path);
    PluginLoader();
    ~PluginLoader();

    void SetFilePath(const std::string &path);
    const std::string &GetFilePath();

    bool Load();
    void Unload();
    bool IsLoaded();

    const std::map<std::string, std::string> &GetPluginInfo();
    const std::vector<IClassEntry *> &GetClassEntries();

private:
    class Imp;
    Imp *imp_;
};

}


#endif // PLUGINLOADER_H
