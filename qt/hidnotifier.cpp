#include "hidnotifier.h"
#include <Windows.h>
#include <Dbt.h>
#include <hidsdi.h>
#include <QtDebug>

HidNotifier::HidNotifier()
{
    RegisterDevNotification();
}

bool HidNotifier::nativeEvent(const QByteArray &eventType, void *message, long *result)
{
    MSG* msg = reinterpret_cast<MSG*>(message);
    int msgType = msg->message;
    if(msgType==WM_DEVICECHANGE)
    {
        PDEV_BROADCAST_HDR lpdb = (PDEV_BROADCAST_HDR)msg->lParam;
        switch (msg->wParam) {
        case DBT_DEVICEARRIVAL:
        {
            if(lpdb->dbch_devicetype = DBT_DEVTYP_DEVICEINTERFACE)
            {
                PDEV_BROADCAST_DEVICEINTERFACE pDevInf  = (PDEV_BROADCAST_DEVICEINTERFACE)lpdb;
                QString dev_path = QString::fromWCharArray(pDevInf->dbcc_name,pDevInf->dbcc_size);
                qDebug() << "Hid arrived: " << dev_path;
                emit SigHidArrive(dev_path);
            }
            break;
        }
        case DBT_DEVICEREMOVECOMPLETE:
            if(lpdb->dbch_devicetype = DBT_DEVTYP_DEVICEINTERFACE)
            {
                PDEV_BROADCAST_DEVICEINTERFACE pDevInf  = (PDEV_BROADCAST_DEVICEINTERFACE)lpdb;
                QString dev_path = QString::fromWCharArray(pDevInf->dbcc_name,pDevInf->dbcc_size);
                qDebug() << "Hid removed: " << dev_path;
                emit SigHidRemove(dev_path);
            }
            break;
        }
    }
    return false;
}

void HidNotifier::RegisterDevNotification()
{
    GUID HidGuid;
    DEV_BROADCAST_DEVICEINTERFACE NotificationFilter;
    HDEVNOTIFY hDevNotify;

    HidD_GetHidGuid(&HidGuid);

    ZeroMemory( &NotificationFilter, sizeof(NotificationFilter));

    NotificationFilter.dbcc_size = sizeof(DEV_BROADCAST_DEVICEINTERFACE);
    NotificationFilter.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
    NotificationFilter.dbcc_classguid = HidGuid;


    hDevNotify = RegisterDeviceNotification( (HWND)winId(),
                                               &NotificationFilter,
                                               DEVICE_NOTIFY_WINDOW_HANDLE);

    if(!hDevNotify){
        Q_ASSERT(false);
    }
}
