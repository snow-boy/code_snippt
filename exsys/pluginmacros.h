#ifndef PLUGINMACROS_H
#define PLUGINMACROS_H
#include "exsys_global.h"
#include "classentry.h"

// usage:
//BEGIN_DEF_PLUGIN_INFO()
//    PLUGIN_INFO("key1", "value1")
//    PLUGIN_INFO("key2", "value2")
//END_DEF_PLUGIN_INFO()

//BEGIN_CLASS_ENTRIES()
//    BEGIN_CLASS_ENTRY(className)
//        INTERFACE_ENTRY(Interface1, "iid1")
//        INTERFACE_ENTRY(Interface2, "iid2")
//    END_CLASS_ENTRY()
//    BEGIN_CLASS_ENTRY(className2)
//        INTERFACE_ENTRY(Interface21, "iid21")
//        INTERFACE_ENTRY(Interface22, "iid22")
//    END_CLASS_ENTRY()
//END_CLASS_ENTRIES()


#define BEGIN_DEF_PLUGIN_INFO() \
    static const char s_plugin_info[] = "__noitom_plugin_spec_begin_tag__{"\
    "\"architecture\":\"2.0.0.0\""

#define PLUGIN_INFO(key, value)\
    ",\""\
    key\
    "\":\"" \
    value \
    "\""

#define END_DEF_PLUGIN_INFO() \
    "}";\
    extern "C" DECL_EXPORT const char * AX_GetPluginInfo() \
    {\
        return s_plugin_info+32;\
    }

#define BEGIN_CLASS_ENTRIES() \
    static std::vector<exsys::IClassEntry *> g_class_entries;\
    extern "C" DECL_EXPORT void AX_InitializePlugin() \
    {

#define BEGIN_CLASS_ENTRY(className)\
    {\
        exsys::ClassEntry<className> *entry = new exsys::ClassEntry<className>(); \
        typedef className ObjType;

#define INTERFACE_ENTRY(interface_type, iid) \
    {\
        exsys::IInterface *interface = new exsys::Interface<ObjType, interface_type>(iid);\
        entry->AddInterface(interface);\
    }

#define END_CLASS_ENTRY() \
        g_class_entries.push_back(entry);\
    }

#define END_CLASS_ENTRIES() \
    }\
    extern "C" DECL_EXPORT void AX_UninitializePlugin()\
    {\
        for(exsys::IClassEntry *entry: g_class_entries){\
            delete entry;\
        }\
        g_class_entries.clear();\
    }\
    extern "C" DECL_EXPORT int AX_EntryCount() \
    {\
        return static_cast<int>(g_class_entries.size());\
    }\
    extern "C" DECL_EXPORT exsys::IClassEntry *AX_GetEntry(int index)\
    {\
        if(g_class_entries.size() > index){\
            return g_class_entries[index];\
        }\
        return nullptr;\
    }

#endif // PLUGINMACROS_H
