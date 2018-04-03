#ifndef CSIPACCOUNT_H
#define CSIPACCOUNT_H

#include <pjsua.h>
#include <QObject>

/* Ringtones		    US	       UK  */
#define RINGBACK_FREQ1	    440	    /* 400 */
#define RINGBACK_FREQ2	    480	    /* 450 */
#define RINGBACK_ON	    2000    /* 400 */
#define RINGBACK_OFF	    4000    /* 200 */
#define RINGBACK_CNT	    1	    /* 2   */
#define RINGBACK_INTERVAL   4000    /* 2000 */

#define RING_FREQ1	    800
#define RING_FREQ2	    640
#define RING_ON		    200
#define RING_OFF	    100
#define RING_CNT	    3
#define RING_INTERVAL	    3000

#define current_acc	pjsua_acc_get_default()

#define PJSUA_APP_NO_LIMIT_DURATION	(int)0x7FFFFFFF

typedef struct app_call_data
{
	pj_timer_entry	    timer;
	pj_bool_t		    ringback_on;
	pj_bool_t		    ring_on;
} app_call_data;


/* Pjsua application data */
typedef struct pjsua_app_config
{
	pjsua_config	    cfg;
	pjsua_logging_config    log_cfg;
	pjsua_media_config	    media_cfg;
	pj_bool_t		    no_refersub;
	pj_bool_t		    ipv6;
	pj_bool_t		    enable_qos;
	pj_bool_t		    no_tcp;
	pj_bool_t		    no_udp;
	pj_bool_t		    use_tls;
	pjsua_transport_config  udp_cfg;
	pjsua_transport_config  rtp_cfg;
	pjsip_redirect_op	    redir_op;

	unsigned		    acc_cnt;
	pjsua_acc_config	    acc_cfg[PJSUA_MAX_ACC];

	app_call_data	    call_data[PJSUA_MAX_CALLS];

	pj_pool_t		   *pool;
	/* Compatibility with older pjsua */

	unsigned		    codec_cnt;
	pj_str_t				codec_arg[32];
	unsigned		    codec_dis_cnt;
	pj_str_t				codec_dis[32];
	pj_bool_t		    null_audio;
	unsigned		    wav_count;
	pj_str_t				wav_files[32];
	pjsua_player_id	    wav_id[32];
	pjsua_conf_port_id	    wav_port[32];

	unsigned		    tone_count;
	pjmedia_tone_desc	    tones[32];
	pjsua_conf_port_id	    tone_slots[32];
	pj_bool_t		    auto_play;
	pj_bool_t		    auto_play_hangup;
	pj_timer_entry	    auto_hangup_timer;
	pj_bool_t		    auto_loop;
	pj_bool_t		    auto_conf;
	pj_str_t		    rec_file;
	pj_bool_t		    auto_rec;
	pjsua_recorder_id	    rec_id;
	pjsua_conf_port_id	    rec_port;
	unsigned		    auto_answer;
	unsigned		    duration;

	float		    mic_level;
	float			speaker_level;

	int			    capture_dev, playback_dev;
	unsigned		    capture_lat, playback_lat;

	pj_bool_t		    no_tones;
	int			    ringback_slot;
	int			    ringback_cnt;
	pjmedia_port	   *ringback_port;
	int			    ring_slot;
	int			    ring_cnt;
	pjmedia_port	   *ring_port;

	unsigned		    aud_cnt;
} pjsua_app_config;

class cSipAccount
{

public:
    cSipAccount();

public:
	int init_config();
    int init_sphone();
    int reg_user(QString phone, QString domain, QString pwd);
	int playback();
	int stopplayback();
	int startrecord();
	int stoprecord();
	int makecall(QString url);

private:
    void error_exit(const char* title, pj_status_t status);
	void error_log(const char* title, pj_status_t status);
    void default_config();
	void ring_start(pjsua_call_id call_id);
	void ring_stop(pjsua_call_id call_id);
	void ringback_start(pjsua_call_id call_id);
	void on_call_generic_media_state(pjsua_call_info *ci, unsigned mi,
		pj_bool_t *has_error);
	void on_call_audio_state(pjsua_call_info *ci, unsigned mi,
		pj_bool_t *has_error);
	void call_timeout_callback(pj_timer_heap_t *timer_heap,
		struct pj_timer_entry *entry);

	void incoming_call(pjsua_acc_id acc_id, pjsua_call_id call_id,
		pjsip_rx_data *rdata);
	void call_state(pjsua_call_id call_id, pjsip_event *e);
	void call_media_state(pjsua_call_id call_id);
	void call_tsx_state(pjsua_call_id call_id, pjsip_transaction *tsx,
		pjsip_event *e);

private:
    static void on_incoming_call(pjsua_acc_id acc_id, pjsua_call_id call_id,
                          pjsip_rx_data *rdata);
    static void on_call_state(pjsua_call_id call_id, pjsip_event *e);
    static void on_call_media_state(pjsua_call_id call_id);
    static void on_call_tsx_state(pjsua_call_id call_id,   pjsip_transaction *tsx,
	    pjsip_event *e);
	static void on_call_timeout_callback(pj_timer_heap_t *timer_heap,
		struct pj_timer_entry *entry);
private:
	// Save Call Back Function Object
	static cSipAccount* cCurrentAcc;

	pjsua_call_id	    current_call;
	pjsua_app_config   app_config;
	pjsua_call_setting   call_opt;
};

#endif // CSIPACCOUNT_H
