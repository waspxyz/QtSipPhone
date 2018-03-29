#ifndef CSIPACCOUNT_H
#define CSIPACCOUNT_H

#include <pjsua.h>
#include <QObject>

class cSipAccount
{

public:
    cSipAccount();

public:
    int InitSPhone();

private:
    void error_exit(const char* title, pj_status_t status);
    void incoming_call(pjsua_acc_id acc_id, pjsua_call_id call_id,
                          pjsip_rx_data *rdata);
    void call_state(pjsua_call_id call_id, pjsip_event *e);
    void call_media_state(pjsua_call_id call_id);

private:
    static void on_incoming_call(pjsua_acc_id acc_id, pjsua_call_id call_id,
                          pjsip_rx_data *rdata);
    static void on_call_state(pjsua_call_id call_id, pjsip_event *e);
    static void on_call_media_state(pjsua_call_id call_id);

private:
    pjsua_acc_id m_acc_id;
    pj_status_t  m_status;
    // Save Call Back Function Object
    static cSipAccount* cCurrentAcc;
};

#endif // CSIPACCOUNT_H
