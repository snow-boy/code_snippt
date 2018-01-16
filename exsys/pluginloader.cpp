#include "pluginloader.h"
#include <QMap>
#include <QList>
#include <QLibrary>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QtDebug>
#include <QJsonArray>

namespace exsys {

class PluginLoader::Imp
{
public:
    typedef void (*FInitPlugin)();
    typedef void (*FUninitPlugin)();
    typedef const char *(*FGetPluginInfo)();
    typedef int (*FEntryCount)();
    typedef IClassEntry *(*FGetEntry)(int);

#define INIT_SYMBOL "AX_InitializePlugin"
#define UNINIT_SYMBOL "AX_UninitializePlugin"
#define GET_INFO_SYMBOL "AX_GetPluginInfo"
#define ENTRY_COUNT_SYMBOL "AX_EntryCount"
#define GET_ENTRY_SYMBOL "AX_GetEntry"

    std::map<std::string, std::string> plugin_info_;
    std::vector<IClassEntry *> entries_;

    FInitPlugin f_init_plugin_;
    FUninitPlugin f_uninit_plugin_;
    FGetPluginInfo f_get_plugin_info_;
    FEntryCount f_entry_count_;
    FGetEntry f_get_entry_;

    QLibrary lib_;

    Imp():
        f_init_plugin_(nullptr),
        f_uninit_plugin_(nullptr),
        f_get_plugin_info_(nullptr),
        f_entry_count_(nullptr),
        f_get_entry_(nullptr)
    {}

    bool Load()
    {
        if(lib_.isLoaded()){
            Q_ASSERT(false);
            return true;
        }

        if(!lib_.load()){
            Q_ASSERT(false);
            return false;
        }

        f_init_plugin_ = (FInitPlugin)lib_.resolve(INIT_SYMBOL);
        if(f_init_plugin_ == nullptr){
            Q_ASSERT(false);
            return false;
        }

        f_uninit_plugin_ = (FUninitPlugin)lib_.resolve(UNINIT_SYMBOL);
        if(f_uninit_plugin_ == nullptr){
            Q_ASSERT(false);
            return false;
        }

        f_get_plugin_info_ = (FGetPluginInfo)lib_.resolve(GET_INFO_SYMBOL);
        if(f_get_plugin_info_ == nullptr){
            Q_ASSERT(false);
            return false;
        }

        f_entry_count_ = (FEntryCount)lib_.resolve(ENTRY_COUNT_SYMBOL);
        if(f_entry_count_ == nullptr){
            Q_ASSERT(false);
            return false;
        }

        f_get_entry_ = (FGetEntry)lib_.resolve(GET_ENTRY_SYMBOL);
        if(f_get_entry_ == nullptr){
            Q_ASSERT(false);
            return false;
        }

        if(!ParsePluginInfo(f_get_plugin_info_())){
            Q_ASSERT(false);
        }

        f_init_plugin_();

        int entry_count = f_entry_count_();
        for(int i = 0; i < entry_count; ++i){
            IClassEntry *entry = f_get_entry_(i);
            entries_.push_back(entry);
        }

        return true;
    }

    void Unload()
    {
        if(!lib_.isLoaded()){
            return;
        }

        f_uninit_plugin_();

        entries_.clear();
        plugin_info_.clear();
    }

    bool ParsePluginInfo(const std::string& info)
    {
        QJsonParseError error;
        QJsonDocument jsonDoc = QJsonDocument::fromJson(QByteArray::fromStdString(info), &error);
        if(error.error == QJsonParseError::NoError){
            Q_ASSERT(jsonDoc.isObject());
            QJsonObject jobj = jsonDoc.object();

            for(QString key: jobj.keys()){
                plugin_info_[key.toStdString()] = jobj[key].toString().toStdString();
            }
        }
        else{
            Q_ASSERT(false);
            return false;
        }

        return true;
    }
};

PluginLoader::PluginLoader(const std::string &file_path):
    PluginLoader()
{
    imp_->lib_.setFileName(QString::fromStdString(file_path));
}

PluginLoader::PluginLoader()
{
    imp_ = new Imp();
}

PluginLoader::~PluginLoader()
{
    delete imp_;
    imp_ = nullptr;
}

void PluginLoader::SetFilePath(const std::string &path)
{
    imp_->lib_.setFileName(QString::fromStdString(path));
}

const std::string &PluginLoader::GetFilePath()
{
    return imp_->lib_.fileName().toStdString();
}

bool PluginLoader::Load()
{
    return imp_->Load();
}

void PluginLoader::Unload()
{
    imp_->Unload();
}

bool PluginLoader::IsLoaded()
{
    return imp_->lib_.isLoaded();
}

const std::map<std::string, std::string> &PluginLoader::GetPluginInfo()
{
    return imp_->plugin_info_;
}

const std::vector<IClassEntry *> &PluginLoader::GetClassEntries()
{
    return imp_->entries_;
}

}
