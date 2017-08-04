#ifndef PARAMETERLOADER_H
#define PARAMETERLOADER_H

#include <QString>

class ParameterLoader
{
public:
    ParameterLoader();
    ~ParameterLoader();

    void BeginAdjustParams(const QString &file_path);

    void SetValue(const QString& key, int value);
    void SetValue(const QString& key, const QString& value);
    void SetValue(const QString& key, float value);
    void SetValue(const QString& key, double value);
    void SetValue(const QString& key, bool value);
    bool RemoveValue(const QString& key);

    void EndAdjustParams();

    void BeginReadParams(const QString &file_path);

    /*********************************************
     * 若值存在，会将值存到pValue中，并返回true， 若值
     * 不存在， 则返回false。
     * *******************************************/
    bool Value(const QString& key, bool* pValue);
    bool Value(const QString& key, int* pValue);
    bool Value(const QString& key, float* pValue);
    bool Value(const QString& key, double* pValue);
    bool Value(const QString& key, QString* pValue);

    void EndReadParams();

private:
    class Imp;
    Imp* m_pImp;
};

#endif // PARAMETERLOADER_H
