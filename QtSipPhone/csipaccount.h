#ifndef CSIPACCOUNT_H
#define CSIPACCOUNT_H

#include <pjsua.h>
#include <QObject>

class cSipAccount
{

public:
    cSipAccount();

public:
    int init_sphone(int port);
    int reg_user(QString phone, QString domain, QString pwd);

private:
    void error_exit(const char* title, pj_status_t status);

private:
    static void on_incoming_call(pjsua_acc_id acc_id, pjsua_call_id call_id,
                          pjsip_rx_data *rdata);
    static void on_call_state(pjsua_call_id call_id, pjsip_event *e);
    static void on_call_media_state(pjsua_call_id call_id);

private:
    pjsua_acc_id m_acc_id;
    pj_status_t  m_status;

};

#endif // CSIPACCOUNT_H
