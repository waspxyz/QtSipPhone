#include "csipaccount.h"


cSipAccount* cSipAccount::cCurrentAcc = NULL;

cSipAccount::cSipAccount()
{
    cCurrentAcc = this;
}

void cSipAccount::error_exit(const char* title, pj_status_t status)
{
    pjsua_perror(u8"cSipAccount", title, status);
    pjsua_destroy();
}

void cSipAccount::on_incoming_call(pjsua_acc_id acc_id, pjsua_call_id call_id,
                      pjsip_rx_data *rdata)
{
    cCurrentAcc->incoming_call(acc_id, call_id, rdata);
}

void cSipAccount::on_call_state(pjsua_call_id call_id, pjsip_event *e)
{
    cCurrentAcc->call_state(call_id, e);
}

void cSipAccount::on_call_media_state(pjsua_call_id call_id)
{
    cCurrentAcc->call_media_state(call_id);
}

void cSipAccount::incoming_call(pjsua_acc_id acc_id, pjsua_call_id call_id,
                      pjsip_rx_data *rdata)
{
    pjsua_call_info ci;

    PJ_UNUSED_ARG(acc_id);
    PJ_UNUSED_ARG(rdata);

    pjsua_call_get_info(call_id, &ci);

    PJ_LOG(3, (u8"cSipAccount", "Incoming call from %.*s!",
               (int)ci.remote_info.slen, ci.remote_info.ptr));

    // delay 1s to Answer
    pj_thread_sleep(1000);

    // Automatically answer incoming calls with 200/OK
    pjsua_call_answer(call_id, 200, NULL, NULL);

}

void cSipAccount::call_state(pjsua_call_id call_id, pjsip_event *e)
{
    pjsua_call_info ci;

    PJ_UNUSED_ARG(e);

    pjsua_call_get_info(call_id, &ci);

    PJ_LOG(3, (u8"cSipAccount", "Call %d state=%.*s", call_id,
               (int)ci.state_text.slen, ci.state_text.ptr));
}

void cSipAccount::call_media_state(pjsua_call_id call_id)
{
    pjsua_call_info ci;

    pjsua_call_get_info(call_id, &ci);

    if(ci.media_status == PJSUA_CALL_MEDIA_ACTIVE)
    {
        pjsua_conf_connect(ci.conf_slot, 0);
        pjsua_conf_connect(0, ci.conf_slot);
    }
}

//********************************************************************
//Function:use param to init sip stack
//Return: 0--Success  -1--Faild
//********************************************************************
int cSipAccount::InitSPhone()
{
    // create pjsua
    m_status = pjsua_create();
    if(m_status != PJ_SUCCESS)
    {
        error_exit(u8"pjsua_create()error", m_status);
    }

    // init pjsua
    pjsua_config cfg;
    pjsua_logging_config log_cfg;

    pjsua_config_default(&cfg);
    cfg.cb.on_incoming_call = &on_incoming_call;
    cfg.cb.on_call_media_state = &on_call_media_state;
    cfg.cb.on_call_state = &on_call_state;

    pjsua_logging_config_default(&log_cfg);
    log_cfg.console_level = 4;

    m_status = pjsua_init(&cfg, &log_cfg, NULL);
    if(m_status != PJ_SUCCESS)
    {
        error_exit(u8"error in pjsua_init()", m_status);
    }

    // Add UDP transport
    pjsua_transport_config t_cfg;
    pjsua_transport_config_default(&t_cfg);
    t_cfg.port = 5060;
    m_status = pjsua_transport_create(PJSIP_TRANSPORT_UDP, &t_cfg, NULL);
    if(m_status != PJ_SUCCESS)
    {
        error_exit(u8"Error creating transport", m_status);
    }

    // Initialization is done, now start pjsua
    m_status = pjsua_start();
    if(m_status != PJ_SUCCESS)
    {
        error_exit("Error starting pjsua", m_status);
    }

    // Register to SIP server by creating SIP account
    pjsua_acc_config a_cfg;

    pjsua_acc_config_default(&a_cfg);
    a_cfg.id=pj_str((char*)QObject::tr("sip:102@127.0.0.1").toStdString().c_str());
    a_cfg.reg_uri = pj_str((char*)QObject::tr("sip:127.0.0.1").toStdString().c_str());
    a_cfg.cred_count = 1;
    a_cfg.cred_info[0].realm=pj_str((char*)QObject::tr("myvoipapp.com").toStdString().c_str());
    a_cfg.cred_info[0].scheme = pj_str((char*)u8"digest");
    a_cfg.cred_info[0].username = pj_str((char*)u8"102");
    a_cfg.cred_info[0].data_type = PJSIP_CRED_DATA_PLAIN_PASSWD;
    a_cfg.cred_info[0].data = pj_str((char*)u8"102");

    m_status = pjsua_acc_add(&a_cfg, PJ_TRUE, &m_acc_id);
    if(m_status != PJ_SUCCESS)
    {
        error_exit(u8"Error adding account", m_status);
    }

    //

    return 0;
}
