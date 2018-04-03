#include "csipaccount.h"

cSipAccount* cSipAccount::cCurrentAcc = NULL;

cSipAccount::cSipAccount()
{
	cCurrentAcc = this;
	memset(&app_config, 0, sizeof(app_config));
}

void cSipAccount::error_log(const char* title, pj_status_t status)
{
	pjsua_perror(u8"cSipAccount", title, status);
}

void cSipAccount::error_exit(const char* title, pj_status_t status)
{
	pjsua_perror(u8"cSipAccount", title, status);

	/* Close ringback port */
	if (app_config.ringback_port &&
		app_config.ringback_slot != PJSUA_INVALID_ID)
	{
		pjsua_conf_remove_port(app_config.ringback_slot);
		app_config.ringback_slot = PJSUA_INVALID_ID;
		pjmedia_port_destroy(app_config.ringback_port);
		app_config.ringback_port = NULL;
	}

	/* Close ring port */
	if (app_config.ring_port && app_config.ring_slot != PJSUA_INVALID_ID) {
		pjsua_conf_remove_port(app_config.ring_slot);
		app_config.ring_slot = PJSUA_INVALID_ID;
		pjmedia_port_destroy(app_config.ring_port);
		app_config.ring_port = NULL;
	}

	/* Close tone generators */
	for (int i = 0; i<app_config.tone_count; ++i)
	{
		pjsua_conf_remove_port(app_config.tone_slots[i]);
	}

	pj_pool_safe_release(&app_config.pool);

	status = pjsua_destroy();

	/* Reset config */
	pj_bzero(&app_config, sizeof(app_config));
}

/* Set default config. */
void cSipAccount::default_config()
{
	char tmp[80];
	unsigned i;
	pjsua_app_config *cfg = &app_config;

	pjsua_config_default(&cfg->cfg);
	pj_ansi_sprintf(tmp, "PJSUA v%s %s", pj_get_version(),
		pj_get_sys_info()->info.ptr);
	pj_strdup2_with_null(app_config.pool, &cfg->cfg.user_agent, tmp);

	pjsua_logging_config_default(&cfg->log_cfg);
	pjsua_media_config_default(&cfg->media_cfg);
	pjsua_transport_config_default(&cfg->udp_cfg);
	cfg->udp_cfg.port = 0;
	pjsua_transport_config_default(&cfg->rtp_cfg);
	cfg->rtp_cfg.port = 4000;
	cfg->redir_op = PJSIP_REDIRECT_ACCEPT_REPLACE;
	cfg->duration = PJSUA_APP_NO_LIMIT_DURATION;

	for (i = 0; i<PJ_ARRAY_SIZE(app_config.wav_id); ++i) {
		cfg->wav_id[i] = PJSUA_INVALID_ID;
		cfg->wav_port[i] = PJSUA_INVALID_ID;
	}

	cfg->rec_id = PJSUA_INVALID_ID;
	cfg->rec_port = PJSUA_INVALID_ID;
	cfg->mic_level = cfg->speaker_level = 1.0;
	cfg->capture_dev = PJSUA_INVALID_ID;
	cfg->playback_dev = PJSUA_INVALID_ID;
	cfg->capture_lat = PJMEDIA_SND_DEFAULT_REC_LATENCY;
	cfg->playback_lat = PJMEDIA_SND_DEFAULT_PLAY_LATENCY;
	cfg->ringback_slot = PJSUA_INVALID_ID;
	cfg->ring_slot = PJSUA_INVALID_ID;

	for (i = 0; i<PJ_ARRAY_SIZE(cfg->acc_cfg); ++i)
		pjsua_acc_config_default(&cfg->acc_cfg[i]);

	cfg->aud_cnt = 1;
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

void cSipAccount::on_call_tsx_state(pjsua_call_id call_id, pjsip_transaction *tsx,
	pjsip_event *e)
{
	cCurrentAcc->call_tsx_state(call_id, tsx, e);
}

void cSipAccount::on_call_timeout_callback(pj_timer_heap_t *timer_heap,
	struct pj_timer_entry *entry)
{
	cCurrentAcc->call_timeout_callback(timer_heap, entry);
}


void cSipAccount::incoming_call(pjsua_acc_id acc_id, pjsua_call_id call_id,
                      pjsip_rx_data *rdata)
{
	pjsua_call_info call_info;

	PJ_UNUSED_ARG(acc_id);
	PJ_UNUSED_ARG(rdata);

	pjsua_call_get_info(call_id, &call_info);

	if (current_call == PJSUA_INVALID_ID)
		current_call = call_id;

	/* Start ringback */
	ring_start(call_id);

	pj_thread_sleep(4000);

	// Auto Answer;
	app_config.auto_answer = 200;
	pjsua_call_setting opt;

	pjsua_call_setting_default(&opt);
	opt.aud_cnt = app_config.aud_cnt;

	pjsua_call_answer2(call_id, &opt, app_config.auto_answer, NULL,NULL);
}

void cSipAccount::call_state(pjsua_call_id call_id, pjsip_event *e)
{
	pjsua_call_info call_info;

	PJ_UNUSED_ARG(e);

	pjsua_call_get_info(call_id, &call_info);

	if (call_info.state == PJSIP_INV_STATE_DISCONNECTED) {

		/* Stop all ringback for this call */
		ring_stop(call_id);

		/* Cancel duration timer, if any */
		if (app_config.call_data[call_id].timer.id != PJSUA_INVALID_ID) {
			app_call_data *cd = &app_config.call_data[call_id];
			pjsip_endpoint *endpt = pjsua_get_pjsip_endpt();

			cd->timer.id = PJSUA_INVALID_ID;
			pjsip_endpt_cancel_timer(endpt, &cd->timer);
		}

		/* Rewind play file when hangup automatically,
		* since file is not looped
		*/
		if (app_config.auto_play_hangup)
		{
			for(int i = 0; i < app_config.wav_count; i++)
				pjsua_player_set_pos(app_config.wav_id[i], 0);
		}


		PJ_LOG(3, ("cSipAccount", "Call %d is DISCONNECTED [reason=%d (%s)]",
			call_id,
			call_info.last_status,
			call_info.last_status_text.ptr));

		if (call_id == current_call) {
			//find_next_call();
		}
	}
	else {

		if (app_config.duration != PJSUA_APP_NO_LIMIT_DURATION &&
			call_info.state == PJSIP_INV_STATE_CONFIRMED)
		{
			/* Schedule timer to hangup call after the specified duration */
			app_call_data *cd = &app_config.call_data[call_id];
			pjsip_endpoint *endpt = pjsua_get_pjsip_endpt();
			pj_time_val delay;

			cd->timer.id = call_id;
			delay.sec = app_config.duration;
			delay.msec = 0;
			pjsip_endpt_schedule_timer(endpt, &cd->timer, &delay);
		}

		if (call_info.state == PJSIP_INV_STATE_EARLY) {
			int code;
			pj_str_t reason;
			pjsip_msg *msg;

			/* This can only occur because of TX or RX message */
			pj_assert(e->type == PJSIP_EVENT_TSX_STATE);

			if (e->body.tsx_state.type == PJSIP_EVENT_RX_MSG) {
				msg = e->body.tsx_state.src.rdata->msg_info.msg;
			}
			else {
				msg = e->body.tsx_state.src.tdata->msg;
			}

			code = msg->line.status.code;
			reason = msg->line.status.reason;

			/* Start ringback for 180 for UAC unless there's SDP in 180 */
			if (call_info.role == PJSIP_ROLE_UAC && code == 180 &&
				msg->body == NULL &&
				call_info.media_status == PJSUA_CALL_MEDIA_NONE)
			{
				ringback_start(call_id);
			}

			PJ_LOG(3, ("cSipAccount", "Call %d state changed to %s (%d %.*s)",
				call_id, call_info.state_text.ptr,
				code, (int)reason.slen, reason.ptr));
		}
		else {
			PJ_LOG(3, ("cSipAccount", "Call %d state changed to %s",	call_id,
				call_info.state_text.ptr));
		}

		if (current_call == PJSUA_INVALID_ID)
			current_call = call_id;

	}
}

void cSipAccount::call_timeout_callback(pj_timer_heap_t *timer_heap,
	struct pj_timer_entry *entry)
{
	pjsua_call_id call_id = entry->id;
	pjsua_msg_data msg_data_;
	pjsip_generic_string_hdr warn;
	pj_str_t hname = pj_str("Warning");
	pj_str_t hvalue = pj_str("399 pjsua \"Call duration exceeded\"");

	PJ_UNUSED_ARG(timer_heap);

	if (call_id == PJSUA_INVALID_ID) {
		PJ_LOG(1, ("cSipAccount", "Invalid call ID in timer callback"));
		return;
	}

	/* Add warning header */
	pjsua_msg_data_init(&msg_data_);
	pjsip_generic_string_hdr_init2(&warn, &hname, &hvalue);
	pj_list_push_back(&msg_data_.hdr_list, &warn);

	/* Call duration has been exceeded; disconnect the call */
	PJ_LOG(3, ("cSipAccount", "Duration (%d seconds) has been exceeded "
		"for call %d, disconnecting the call",
		app_config.duration, call_id));
	entry->id = PJSUA_INVALID_ID;
	pjsua_call_hangup(call_id, 200, NULL, &msg_data_);
}


/* General processing for media state. "mi" is the media index */
void cSipAccount::on_call_generic_media_state(pjsua_call_info *ci, unsigned mi, pj_bool_t *has_error)
{
	const char *status_name[] = {
		"None",
		"Active",
		"Local hold",
		"Remote hold",
		"Error"
	};

	PJ_UNUSED_ARG(has_error);

	pj_assert(ci->media[mi].status <= PJ_ARRAY_SIZE(status_name));
	pj_assert(PJSUA_CALL_MEDIA_ERROR == 4);

	PJ_LOG(4, ("cSipAccount", "Call %d media %d [type=%s], status is %s",
		ci->id, mi, pjmedia_type_name(ci->media[mi].type),
		status_name[ci->media[mi].status]));
}


/* Process audio media state. "mi" is the media index. */
void cSipAccount::on_call_audio_state(pjsua_call_info *ci, unsigned mi,
	pj_bool_t *has_error)
{
	PJ_UNUSED_ARG(has_error);

	/* Stop ringback */
	ring_stop(ci->id);

	/* Connect ports appropriately when media status is ACTIVE or REMOTE HOLD,
	* otherwise we should NOT connect the ports.
	*/
	if (ci->media[mi].status == PJSUA_CALL_MEDIA_ACTIVE ||
		ci->media[mi].status == PJSUA_CALL_MEDIA_REMOTE_HOLD)
	{
		pj_bool_t connect_sound = PJ_TRUE;
		pj_bool_t disconnect_mic = PJ_FALSE;
		pjsua_conf_port_id call_conf_slot;

		call_conf_slot = ci->media[mi].stream.aud.conf_slot;

		/* Loopback sound, if desired */
		if (app_config.auto_loop) {
			pjsua_conf_connect(call_conf_slot, call_conf_slot);
			connect_sound = PJ_FALSE;
		}

		/* Automatically record conversation, if desired */
		if (app_config.auto_rec && app_config.rec_port != PJSUA_INVALID_ID) {
			pjsua_conf_connect(call_conf_slot, app_config.rec_port);
		}

		/* Stream a file, if desired */
		if ((app_config.auto_play || app_config.auto_play_hangup) &&
			app_config.wav_port[0] != PJSUA_INVALID_ID)
		{
			pjsua_conf_connect(app_config.wav_port[0], call_conf_slot);
			connect_sound = PJ_FALSE;
		}

		/* Put call in conference with other calls, if desired */
		if (app_config.auto_conf) {
			pjsua_call_id call_ids[PJSUA_MAX_CALLS];
			unsigned call_cnt = PJ_ARRAY_SIZE(call_ids);
			unsigned i;

			/* Get all calls, and establish media connection between
			* this call and other calls.
			*/
			pjsua_enum_calls(call_ids, &call_cnt);

			for (i = 0; i<call_cnt; ++i) {
				if (call_ids[i] == ci->id)
					continue;

				if (!pjsua_call_has_media(call_ids[i]))
					continue;

				pjsua_conf_connect(call_conf_slot,
					pjsua_call_get_conf_port(call_ids[i]));
				pjsua_conf_connect(pjsua_call_get_conf_port(call_ids[i]),
					call_conf_slot);

				/* Automatically record conversation, if desired */
				if (app_config.auto_rec && app_config.rec_port !=
					PJSUA_INVALID_ID)
				{
					pjsua_conf_connect(pjsua_call_get_conf_port(call_ids[i]),
						app_config.rec_port);
				}
			}

			/* Also connect call to local sound device */
			connect_sound = PJ_TRUE;
		}

		/* Otherwise connect to sound device */
		if (connect_sound) {
			pjsua_conf_connect(call_conf_slot, 0);
			if (!disconnect_mic)
				pjsua_conf_connect(0, call_conf_slot);

			/* Automatically record conversation, if desired */
			if (app_config.auto_rec && app_config.rec_port != PJSUA_INVALID_ID)
			{
				pjsua_conf_connect(call_conf_slot, app_config.rec_port);
				pjsua_conf_connect(0, app_config.rec_port);
			}
		}
	}
}

/*
* Handler when a transaction within a call has changed state.
*/
void cSipAccount::call_tsx_state(pjsua_call_id call_id,	pjsip_transaction *tsx,	pjsip_event *e)
{
	const pjsip_method info_method =
	{
		PJSIP_OTHER_METHOD,
		{ "INFO", 4 }
	};

	if (pjsip_method_cmp(&tsx->method, &info_method) == 0) {
		/*
		* Handle INFO method.
		*/
		const pj_str_t STR_APPLICATION = { "application", 11 };
		const pj_str_t STR_DTMF_RELAY = { "dtmf-relay", 10 };
		pjsip_msg_body *body = NULL;
		pj_bool_t dtmf_info = PJ_FALSE;

		if (tsx->role == PJSIP_ROLE_UAC) {
			if (e->body.tsx_state.type == PJSIP_EVENT_TX_MSG)
				body = e->body.tsx_state.src.tdata->msg->body;
			else
				body = e->body.tsx_state.tsx->last_tx->msg->body;
		}
		else {
			if (e->body.tsx_state.type == PJSIP_EVENT_RX_MSG)
				body = e->body.tsx_state.src.rdata->msg_info.msg->body;
		}

		/* Check DTMF content in the INFO message */
		if (body && body->len &&
			pj_stricmp(&body->content_type.type, &STR_APPLICATION) == 0 &&
			pj_stricmp(&body->content_type.subtype, &STR_DTMF_RELAY) == 0)
		{
			dtmf_info = PJ_TRUE;
		}

		if (dtmf_info && tsx->role == PJSIP_ROLE_UAC &&
			(tsx->state == PJSIP_TSX_STATE_COMPLETED ||
			(tsx->state == PJSIP_TSX_STATE_TERMINATED &&
				e->body.tsx_state.prev_state != PJSIP_TSX_STATE_COMPLETED)))
		{
			/* Status of outgoing INFO request */
			if (tsx->status_code >= 200 && tsx->status_code < 300) {
				PJ_LOG(4, ("cSipAccount", "Call %d: DTMF sent successfully with INFO", call_id));
			}
			else if (tsx->status_code >= 300) {
				PJ_LOG(4, ("cSipAccount", "Call %d: Failed to send DTMF with INFO: %d/%.*s",	call_id,
					tsx->status_code,
					(int)tsx->status_text.slen,
					tsx->status_text.ptr));
			}
		}
		else if (dtmf_info && tsx->role == PJSIP_ROLE_UAS &&
			tsx->state == PJSIP_TSX_STATE_TRYING)
		{
			/* Answer incoming INFO with 200/OK */
			pjsip_rx_data *rdata;
			pjsip_tx_data *tdata;
			pj_status_t status;

			rdata = e->body.tsx_state.src.rdata;

			if (rdata->msg_info.msg->body) {
				status = pjsip_endpt_create_response(tsx->endpt, rdata,
					200, NULL, &tdata);
				if (status == PJ_SUCCESS)
					status = pjsip_tsx_send_msg(tsx, tdata);

				PJ_LOG(3, ("cSipAccount", "Call %d: incoming INFO:\n%.*s",
					call_id,
					(int)rdata->msg_info.msg->body->len,
					rdata->msg_info.msg->body->data));
			}
			else {
				status = pjsip_endpt_create_response(tsx->endpt, rdata,
					400, NULL, &tdata);
				if (status == PJ_SUCCESS)
					status = pjsip_tsx_send_msg(tsx, tdata);
			}
		}
	}
}


void cSipAccount::call_media_state(pjsua_call_id call_id)
{
	pjsua_call_info call_info;
	unsigned mi;
	pj_bool_t has_error = PJ_FALSE;

	pjsua_call_get_info(call_id, &call_info);

	for (mi = 0; mi<call_info.media_cnt; ++mi)
	{
		on_call_generic_media_state(&call_info, mi, &has_error);

		switch (call_info.media[mi].type) {
		case PJMEDIA_TYPE_AUDIO:
			on_call_audio_state(&call_info, mi, &has_error);
			break;
		default:
			/* Make gcc happy about enum not handled by switch/case */
			break;
		}
	}

	if (has_error) {
		pj_str_t reason = pj_str("Media failed");
		pjsua_call_hangup(call_id, 500, &reason, NULL);
	}

}

void cSipAccount::ringback_start(pjsua_call_id call_id)
{
	if (app_config.no_tones)
		return;

	if (app_config.call_data[call_id].ringback_on)
		return;

	app_config.call_data[call_id].ringback_on = PJ_TRUE;

	if (++app_config.ringback_cnt == 1 &&
		app_config.ringback_slot != PJSUA_INVALID_ID)
	{
		pjsua_conf_connect(app_config.ringback_slot, 0);
	}
}

void cSipAccount::ring_start(pjsua_call_id call_id)
{
	if (app_config.no_tones)
		return;

	if (app_config.call_data[call_id].ring_on)
		return;

	app_config.call_data[call_id].ring_on = PJ_TRUE;

	if (++app_config.ring_cnt == 1 &&
		app_config.ring_slot != PJSUA_INVALID_ID)
	{
		pjsua_conf_connect(app_config.ring_slot, 0);
	}
}

void cSipAccount::ring_stop(pjsua_call_id call_id)
{
	if (app_config.no_tones)
		return;

	if (app_config.call_data[call_id].ringback_on) {
		app_config.call_data[call_id].ringback_on = PJ_FALSE;

		pj_assert(app_config.ringback_cnt>0);
		if (--app_config.ringback_cnt == 0 &&
			app_config.ringback_slot != PJSUA_INVALID_ID)
		{
			pjsua_conf_disconnect(app_config.ringback_slot, 0);
			pjmedia_tonegen_rewind(app_config.ringback_port);
		}
	}

	if (app_config.call_data[call_id].ring_on) {
		app_config.call_data[call_id].ring_on = PJ_FALSE;

		pj_assert(app_config.ring_cnt>0);
		if (--app_config.ring_cnt == 0 &&
			app_config.ring_slot != PJSUA_INVALID_ID)
		{
			pjsua_conf_disconnect(app_config.ring_slot, 0);
			pjmedia_tonegen_rewind(app_config.ring_port);
		}
	}
}

int cSipAccount::init_config()
{
	// config the default
	default_config();

	app_config.log_cfg.log_filename = pj_str("./app.log");
	app_config.log_cfg.level = 5;

	// tone config
	app_config.tones[app_config.tone_count].freq1 = RINGBACK_FREQ1;
	app_config.tones[app_config.tone_count].freq2 = RINGBACK_FREQ2;
	app_config.tones[app_config.tone_count].on_msec = RINGBACK_ON;
	app_config.tones[app_config.tone_count].off_msec = RINGBACK_OFF;
	++app_config.tone_count;

	// wav file config
	app_config.wav_files[app_config.wav_count++] = pj_str("./music.wav");
	app_config.wav_files[app_config.wav_count++] = pj_str("./ringback.wav");

	//
	//app_config.auto_rec = PJ_TRUE;
	//app_config.rec_file = pj_str("./record.wav");

	return 0;
}

//********************************************************************
//Function:use param to init sip stack
//Return: 0--Success  -1--Faild
//********************************************************************
int cSipAccount::init_sphone()
{
	pjsua_transport_id transport_id = -1;
	pjsua_transport_config tcp_cfg;
	unsigned i;
	pj_pool_t *tmp_pool;
	pj_status_t status;

    // create pjsua
    status = pjsua_create();
    if(status != PJ_SUCCESS)
    {
        error_exit(u8"pjsua_create()error", status);
    }

	/* Create pool for application */
	app_config.pool = pjsua_pool_create("pjsua-app", 1000, 1000);
	tmp_pool = pjsua_pool_create("tmp-pjsua", 1000, 1000);
	
	// load config
	init_config();

	/* Initialize application callbacks */
	app_config.cfg.cb.on_call_state = &on_call_state;
	app_config.cfg.cb.on_call_media_state = &on_call_media_state;
	app_config.cfg.cb.on_incoming_call = &on_incoming_call;
	app_config.cfg.cb.on_call_tsx_state = &on_call_tsx_state;

	/* Set sound device latency */
	if (app_config.capture_lat > 0)
		app_config.media_cfg.snd_rec_latency = app_config.capture_lat;
	if (app_config.playback_lat)
		app_config.media_cfg.snd_play_latency = app_config.playback_lat;

	/* Initialize pjsua */
	status = pjsua_init(&app_config.cfg, &app_config.log_cfg,
		&app_config.media_cfg);
	if (status != PJ_SUCCESS) {
		pj_pool_release(tmp_pool);
		error_exit(u8"pjsua_init()error", status);
	}

	/* Initialize calls data */
	for (i = 0; i<PJ_ARRAY_SIZE(app_config.call_data); ++i) {
		app_config.call_data[i].timer.id = PJSUA_INVALID_ID;
		app_config.call_data[i].timer.cb = &on_call_timeout_callback;
	}

	/* Optionally registers WAV file */
	for (i = 0; i<app_config.wav_count; ++i) {
		pjsua_player_id wav_id;
		unsigned play_options = 0;

		if (app_config.auto_play_hangup)
			play_options |= PJMEDIA_FILE_NO_LOOP;

		status = pjsua_player_create(&app_config.wav_files[i], play_options,
			&wav_id);
		if (status != PJ_SUCCESS)
		{
			pj_pool_release(tmp_pool);
			error_exit(u8"pjsua_player_create() error", status);
		}

		if (app_config.wav_id[i] == PJSUA_INVALID_ID) 
		{
			app_config.wav_id[i] = wav_id;
			app_config.wav_port[i] = pjsua_player_get_conf_port(app_config.wav_id[i]);
		}
	}

	/* Optionally registers tone players */
	for (i = 0; i<app_config.tone_count; ++i) {
		pjmedia_port *tport;
		char name[80];
		pj_str_t label;
		pj_status_t status2;

		pj_ansi_snprintf(name, sizeof(name), "tone-%d,%d",
			app_config.tones[i].freq1,
			app_config.tones[i].freq2);
		label = pj_str(name);
		status2 = pjmedia_tonegen_create2(app_config.pool, &label,
			8000, 1, 160, 16,
			PJMEDIA_TONEGEN_LOOP, &tport);
		if (status2 != PJ_SUCCESS) 
		{
			pj_pool_release(tmp_pool);
			error_exit(u8"Unable to create tone generator", status2);
		}

		status2 = pjsua_conf_add_port(app_config.pool, tport,
			&app_config.tone_slots[i]);
		pj_assert(status2 == PJ_SUCCESS);

		status2 = pjmedia_tonegen_play(tport, 1, &app_config.tones[i], 0);
		pj_assert(status2 == PJ_SUCCESS);
	}

	/* Optionally create recorder file, if any. */
	if (app_config.rec_file.slen) {
		status = pjsua_recorder_create(&app_config.rec_file, 0, NULL, 0, 0,
			&app_config.rec_id);
		if (status != PJ_SUCCESS)
		{
			pj_pool_release(tmp_pool);
			error_exit(u8"Unable to create recorder file", status);
		}
		app_config.rec_port = pjsua_recorder_get_conf_port(app_config.rec_id);
	}

	pj_memcpy(&tcp_cfg, &app_config.udp_cfg, sizeof(tcp_cfg));

	/* Create ringback tones */
	if (app_config.no_tones == PJ_FALSE) {
		unsigned samples_per_frame;
		pjmedia_tone_desc tone[RING_CNT + RINGBACK_CNT];
		pj_str_t name;

		samples_per_frame = app_config.media_cfg.audio_frame_ptime *
			app_config.media_cfg.clock_rate *
			app_config.media_cfg.channel_count / 1000;

		/* Ringback tone (call is ringing) */
		name = pj_str("ringback");
		status = pjmedia_tonegen_create2(app_config.pool, &name,
			app_config.media_cfg.clock_rate,
			app_config.media_cfg.channel_count,
			samples_per_frame,
			16, PJMEDIA_TONEGEN_LOOP,
			&app_config.ringback_port);
		if (status != PJ_SUCCESS)
		{
			pj_pool_release(tmp_pool);
			error_exit(u8"Unable to create ringback", status);
		}

		pj_bzero(&tone, sizeof(tone));
		for (i = 0; i<RINGBACK_CNT; ++i) {
			tone[i].freq1 = RINGBACK_FREQ1;
			tone[i].freq2 = RINGBACK_FREQ2;
			tone[i].on_msec = RINGBACK_ON;
			tone[i].off_msec = RINGBACK_OFF;
		}
		tone[RINGBACK_CNT - 1].off_msec = RINGBACK_INTERVAL;

		pjmedia_tonegen_play(app_config.ringback_port, RINGBACK_CNT, tone,
			PJMEDIA_TONEGEN_LOOP);

		status = pjsua_conf_add_port(app_config.pool, app_config.ringback_port,
			&app_config.ringback_slot);
		if (status != PJ_SUCCESS)
		{
			pj_pool_release(tmp_pool);
			error_exit(u8"Unable to create ringback_slot", status);
		}

		/* Ring (to alert incoming call) */
		name = pj_str("ring");
		status = pjmedia_tonegen_create2(app_config.pool, &name,
			app_config.media_cfg.clock_rate,
			app_config.media_cfg.channel_count,
			samples_per_frame,
			16, PJMEDIA_TONEGEN_LOOP,
			&app_config.ring_port);
		if (status != PJ_SUCCESS)
		{
			pj_pool_release(tmp_pool);
			error_exit(u8"Unable to create ring", status);
		}

		for (i = 0; i<RING_CNT; ++i) {
			tone[i].freq1 = RING_FREQ1;
			tone[i].freq2 = RING_FREQ2;
			tone[i].on_msec = RING_ON;
			tone[i].off_msec = RING_OFF;
		}
		tone[RING_CNT - 1].off_msec = RING_INTERVAL;

		pjmedia_tonegen_play(app_config.ring_port, RING_CNT,
			tone, PJMEDIA_TONEGEN_LOOP);

		status = pjsua_conf_add_port(app_config.pool, app_config.ring_port,
			&app_config.ring_slot);
		if (status != PJ_SUCCESS)
		{
			pj_pool_release(tmp_pool);
			error_exit(u8"Unable to create ring_slot", status);
		}
	}

	/* Add UDP transport unless it's disabled. */
	if (!app_config.no_udp) {
		pjsua_acc_id aid;
		pjsip_transport_type_e type = PJSIP_TRANSPORT_UDP;

		status = pjsua_transport_create(type,
			&app_config.udp_cfg,
			&transport_id);
		if (status != PJ_SUCCESS)
		{
			pj_pool_release(tmp_pool);
			error_exit(u8"Unable create udp transport", status);
		}

		if (app_config.udp_cfg.port == 0) {
			pjsua_transport_info ti;
			pj_sockaddr_in *a;

			pjsua_transport_get_info(transport_id, &ti);
			a = (pj_sockaddr_in*)&ti.local_addr;

			tcp_cfg.port = pj_ntohs(a->sin_port);
		}
	}

	/* Optionally disable some codec */
	for (i = 0; i<app_config.codec_dis_cnt; ++i)
	{
		pjsua_codec_set_priority(&app_config.codec_dis[i],
			PJMEDIA_CODEC_PRIO_DISABLED);
	}

	/* Optionally set codec orders */
	for (i = 0; i<app_config.codec_cnt; ++i)
	{
		pjsua_codec_set_priority(&app_config.codec_arg[i],
			(pj_uint8_t)(PJMEDIA_CODEC_PRIO_NORMAL + i + 9));
	}

	if (app_config.capture_dev != PJSUA_INVALID_ID ||
		app_config.playback_dev != PJSUA_INVALID_ID)
	{
		status = pjsua_set_snd_dev(app_config.capture_dev,
			app_config.playback_dev);
		if (status != PJ_SUCCESS)
		{
			pj_pool_release(tmp_pool);
			error_exit(u8"Unable pjsua_set_snd_dev", status);
		}
	}

	/* Init call setting */
	pjsua_call_setting_default(&call_opt);
	call_opt.aud_cnt = app_config.aud_cnt;
	pj_pool_release(tmp_pool);

    // Initialization is done, now start pjsua
    status = pjsua_start();
    if (status != PJ_SUCCESS)
    {
	    error_exit("Error starting pjsua", status);
    }

    return PJ_SUCCESS;
}

int cSipAccount::reg_user(QString phone, QString domain, QString pwd)
{
	pj_status_t status;

	// Register to SIP server by creating SIP account
	pjsua_acc_config a_cfg;

	pjsua_acc_config_default(&a_cfg);
	QString id = u8"sip:" + phone + u8"@" + domain;
	char buf[32];
	strcpy_s(buf, id.toStdString().c_str());
	a_cfg.id = pj_str(buf);

	QString uri = u8"sip:" + domain;
	char buf_uri[32];
	strcpy_s(buf_uri, uri.toStdString().c_str());

	a_cfg.reg_uri = pj_str(buf_uri);
	a_cfg.cred_count = 1;
	a_cfg.cred_info[0].realm = pj_str(u8"myvoipapp.com");
	a_cfg.cred_info[0].scheme = pj_str(u8"digest");

	char buf_user[32];
	strcpy_s(buf_user, phone.toStdString().c_str());
	a_cfg.cred_info[0].username = pj_str(buf_user);
	a_cfg.cred_info[0].data_type = PJSIP_CRED_DATA_PLAIN_PASSWD;
	char buf_pwd[32];
	strcpy_s(buf_pwd, pwd.toStdString().c_str());
	a_cfg.cred_info[0].data = pj_str(buf_pwd);
	a_cfg.rtp_cfg = app_config.rtp_cfg;

	status = pjsua_acc_add(&a_cfg, PJ_TRUE,  NULL);
	if (status != PJ_SUCCESS)
	{
		error_log(u8"Error adding account", status);
	}

	//
	return status;
}


int cSipAccount::playback()
{
	pjsua_call_info ci;
	pjsua_call_get_info(current_call, &ci);

	if (ci.media[0].status == PJSUA_CALL_MEDIA_ACTIVE ||
		ci.media[0].status == PJSUA_CALL_MEDIA_REMOTE_HOLD)
	{
		pj_bool_t connect_sound = PJ_TRUE;
		pj_bool_t disconnect_mic = PJ_FALSE;
		pjsua_conf_port_id call_conf_slot;

		call_conf_slot = ci.media[0].stream.aud.conf_slot;

		/* Stream a file, if desired */
		if (	app_config.wav_port[1] != PJSUA_INVALID_ID)
		{
			pjsua_conf_connect(app_config.wav_port[1], call_conf_slot);
			connect_sound = PJ_FALSE;
		}
	}

	return PJ_SUCCESS;
}

int cSipAccount::stopplayback()
{
	pjsua_call_info ci;
	pj_status_t status;
	pjsua_call_get_info(current_call, &ci);

	if (ci.media[0].status == PJSUA_CALL_MEDIA_ACTIVE ||
		ci.media[0].status == PJSUA_CALL_MEDIA_REMOTE_HOLD)
	{
		pjsua_conf_port_id call_conf_slot;

		call_conf_slot = ci.media[0].stream.aud.conf_slot;

		/* Stream a file, if desired */
		if (app_config.wav_port[1] != PJSUA_INVALID_ID)
		{
			status = pjsua_conf_disconnect(app_config.wav_port[1], call_conf_slot);
		}
	}

	return status;
}

int cSipAccount::startrecord()
{
	pj_str_t rec_file;
	pj_status_t status;
	pjsua_call_info ci;
	pjsua_conf_port_id call_conf_slot;

	pjsua_call_get_info(current_call, &ci);
	call_conf_slot = ci.media[0].stream.aud.conf_slot;

	rec_file = pj_str("./record.wav");

	status = pjsua_recorder_create(&rec_file, 0, NULL, 0, 0, &app_config.rec_id);
	if (status != PJ_SUCCESS)
	{
		return status;
	}
	app_config.rec_port = pjsua_recorder_get_conf_port(app_config.rec_id);

	// 
	status = pjsua_conf_connect(call_conf_slot, app_config.rec_port);

	return status;
}

int cSipAccount::stoprecord()
{
	pj_status_t status;
	pjsua_call_info ci;
	pjsua_conf_port_id call_conf_slot;

	pjsua_call_get_info(current_call, &ci);
	call_conf_slot = ci.media[0].stream.aud.conf_slot;
	// 
	status = pjsua_conf_disconnect(call_conf_slot, app_config.rec_port);

	pjsua_recorder_destroy(app_config.rec_id);

	return PJ_SUCCESS;
}

int cSipAccount::makecall(QString url)
{
	pjsua_msg_data msg_data_;
	pj_str_t tmp;

	char buf_uri[32];
	strcpy_s(buf_uri, url.toStdString().c_str());

	tmp = pj_str(buf_uri);

	pjsua_msg_data_init(&msg_data_);

	pjsua_call_make_call(current_acc, &tmp, &call_opt, NULL,
		&msg_data_, &current_call);

	return PJ_SUCCESS;
}