#ifndef HIDNOTIFIER_H
#define HIDNOTIFIER_H

#include <QWindow>

class HidNotifier : public QWindow
{
    Q_OBJECT
public:
    HidNotifier();

signals:
    void SigHidArrive(const QString &dev_path);
    void SigHidRemove(const QString &dev_path);

public slots:

protected:
    virtual bool nativeEvent(const QByteArray &eventType, void *message, long *result) override;

private:
    void RegisterDevNotification();
};

#endif // HIDNOTIFIER_H
