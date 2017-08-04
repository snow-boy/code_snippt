#include "parameterloader.h"
#include <QMap>
#include <QDomDocument>
#include <QFile>
#include <QDir>

class ParameterLoader::Imp
{
public:
    Imp()
    {}

    void LoadParams_2_0(const QString& filePath)
    {
        if(!QFile::exists(filePath)){
                    return;
        }

        QDomDocument doc;
        {
            QFile file(filePath);
            file.open(QFile::ReadOnly);
            doc.setContent(file.readAll());
        }

        QDomElement root = doc.firstChildElement("parameters");

        values_.clear();

        QDomElement actorElement;
        actorElement = root.firstChildElement();

        if(!actorElement.isNull()){
            QDomNodeList children = actorElement.childNodes();
            for(int i = 0; i < children.size(); ++i){
                values_[children.at(i).toElement().attribute("key")] =
                        children.at(i).firstChild().nodeValue();
            }
        }
    }

    void ConvertOldToCurrent(const QString& filePath)
    {
        LoadParams_2_0(filePath);
        SaveParams(filePath);
    }

    bool CheckVersion(QDomElement root)
    {
        QString version;
        if(root.hasAttribute("version")){
            version = root.attribute("version");
        }
        else if(root.hasAttribute("Version")){
            version = root.attribute("Version");
        }

        if(version == "3.0"){
            return true;
        }

        return false;
    }

    void LoadParams(const QString& filePath)
    {
        if(!QFile::exists(filePath)){
            return;
        }

        QDomDocument doc;
        {
            QFile file(filePath);
            file.open(QFile::ReadOnly);
            doc.setContent(file.readAll());
        }

        QDomElement root = doc.firstChildElement("parameters");

        if(!CheckVersion(root)){
            ConvertOldToCurrent(filePath);
            LoadParams(filePath);
            return;
        }

        values_.clear();

        if(!root.isNull()){
            QDomNodeList children = root.childNodes();
            for(int i = 0; i < children.size(); ++i){
                values_[children.at(i).toElement().attribute("key")] =
                        children.at(i).firstChild().nodeValue();
            }
        }
    }

    void SaveParams(const QString& filePath)
    {
        QDomDocument doc;
        QDomElement root;

        if(QFile::exists(filePath)){
            {
                QFile file(filePath);
                file.open(QFile::ReadOnly);
                doc.setContent(file.readAll());
            }
            root = doc.firstChildElement("parameters");
            if(!CheckVersion(root)){
                ConvertOldToCurrent(filePath);
                SaveParams(filePath);
                return;
            }
        }
        else{
            root = doc.createElement("parameters");
            doc.appendChild(root);
            root.setAttribute("version", "3.0");
        }

        for(QString key : values_.keys()){
            QDomElement paramElement = doc.createElement("param");
            actorElement.appendChild(paramElement);
            paramElement.setAttribute("key", key);
            QDomText txt = doc.createTextNode(values_[key]);
            paramElement.appendChild(txt);
        }

        {
            QFile file(filePath);
            file.open(QFile::WriteOnly);
            file.write(doc.toByteArray());
        }
    }

    QMap<QString, QString> values_;
};

void ParameterLoader::BeginAdjustParams(const QString &file_path)
{
    m_pImp->LoadParams(file_path);
}

void ParameterLoader::SetValue(const QString &key, int value)
{
    m_pImp->values_[key] = QString::number(value);
}

void ParameterLoader::SetValue(const QString &key, const QString &value)
{
    m_pImp->values_[key] = value;
}

void ParameterLoader::SetValue(const QString &key, float value)
{
    m_pImp->values_[key] = QString::number(value);
}

void ParameterLoader::SetValue(const QString &key, double value)
{
    m_pImp->values_[key] = QString::number(value);
}

void ParameterLoader::SetValue(const QString &key, bool value)
{
    m_pImp->values_[key] = QString::number(value);
}

void ParameterLoader::EndAdjustParams()
{
    m_pImp->SaveParams(m_pImp->GetParamFilePath(m_pImp->pActor_));
}

void ParameterLoader::BeginReadParams(const QString &file_path)
{
    m_pImp->LoadParams(file_path);
}

bool ParameterLoader::Value(const QString &key, bool *pValue)
{
    if(m_pImp->values_.contains(key)){
        *pValue = (m_pImp->values_[key].toInt() == 1);
        return true;
    }

    return false;
}

bool ParameterLoader::Value(const QString &key, int *pValue)
{
    if(m_pImp->values_.contains(key)){
        *pValue = m_pImp->values_[key].toInt();
        return true;
    }

    return false;
}

bool ParameterLoader::Value(const QString &key, float *pValue)
{
    if(m_pImp->values_.contains(key)){
        *pValue = m_pImp->values_[key].toDouble();
        return true;
    }

    return false;
}

bool ParameterLoader::Value(const QString &key, double *pValue)
{
    if(m_pImp->values_.contains(key)){
        *pValue = m_pImp->values_[key].toFloat();
        return true;
    }

    return false;
}

bool ParameterLoader::Value(const QString &key, QString *pValue)
{
    if(m_pImp->values_.contains(key)){
        *pValue = m_pImp->values_[key];
        return true;
    }

    return false;
}

void ParameterLoader::EndReadParams()
{
    m_pImp->values_.clear();
}

bool ParameterLoader::RemoveValue(const QString &key)
{
    if(m_pImp->values_.contains(key)){
        m_pImp->values_.remove(key);
        return true;
    }
    return false;
}

ParameterLoader::ParameterLoader()
{
    m_pImp = new Imp;
}

ParameterLoader::~ParameterLoader()
{
    delete m_pImp;
    m_pImp = nullptr;
}

