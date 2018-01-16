#ifndef CLASSENTRY_H
#define CLASSENTRY_H

#include <string>
#include <vector>
#include <typeinfo>

namespace exsys
{

class IObject
{
public:
    virtual void *QueryInterface(const char *iid) = 0;
};

class IClassEntry
{
public:
    virtual ~IClassEntry(){}

    virtual const char *GetClassName() = 0;

    virtual int GetInterfaceCount() = 0;
    virtual const char *GetInterfaceIID(int index) = 0;

    virtual void *CreateObject() = 0;
    virtual void DestoryObject(void *object) = 0;

    virtual void *QueryInterface(void *object, const char *iid) = 0;
};


class IInterface
{
public:
    virtual ~IInterface() {}
    virtual const char *GetIID() = 0;
    virtual void *GetInterface(void *object) = 0;
};

template <class ClassT, class InterfaceT>
class Interface: public IInterface
{
public:
    Interface(const char *iid):
        iid_(iid)
    {}

    virtual void *GetInterface(void *object) override
    {
        return static_cast<InterfaceT *>(reinterpret_cast<ClassT *>(object));
    }

    virtual const char *GetIID() override
    {
        return iid_.c_str();
    }

private:
    std::string iid_;
};

template <class T>
class ClassEntry: public IClassEntry
{
public:
    ~ClassEntry()
    {
        for(IInterface *i: interface_entries_){
            delete i;
        }

        interface_entries_.clear();

        for(T *obj: objects_){
            delete obj;
        }
        objects_.clear();
    }

    virtual const char *GetClassName() override
    {
        return typeid(T).name();
    }

    void AddInterface(IInterface *interface)
    {
        interface_entries_.push_back(interface);
    }

    virtual int GetInterfaceCount() override
    {
        return interface_entries_.size();
    }

    virtual const char *GetInterfaceIID(int index) override
    {
        if(index < interface_entries_.size())
        {
            return interface_entries_[index]->GetIID();
        }

        return nullptr;
    }

    virtual void *CreateObject() override
    {
        T *object = new T();
        objects_.push_back(object);
        return reinterpret_cast<void *>(object);
    }

    virtual void DestoryObject(void *object) override
    {
        T *real_obj = reinterpret_cast<T *>(object);

        for(auto iter = objects_.begin(); iter != objects_.end(); ++iter)
        {
            if(*iter == real_obj){
                objects_.erase(iter);
                break;
            }
        }

        delete real_obj;
    }

    virtual void *QueryInterface(void *object, const char *iid) override
    {
        void *interface = nullptr;
        for(int i = 0; i < interface_entries_.size(); ++i){
            if(std::string(interface_entries_[i]->GetIID()) == iid){
                interface = interface_entries_[i]->GetInterface(object);
                break;
            }
        }

        return interface;
    }

private:
    std::vector<IInterface *> interface_entries_;
    std::vector<T *> objects_;
};


}

#endif // CLASSENTRY_H
