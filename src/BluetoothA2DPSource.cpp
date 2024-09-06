// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// Copyright 2020 Phil Schatzmann
// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD

#include "BluetoothA2DPSource.h"
#include "Arduino.h"

#define BT_APP_SIG_WORK_DISPATCH (0x01)
#define BT_APP_SIG_WORK_DISPATCH (0x01)

#define APP_RC_CT_TL_RN_VOLUME_CHANGE (1)
#define BT_APP_HEART_BEAT_EVT (0xff00)

/* event for handler "bt_av_hdl_stack_up */
enum {
  BT_APP_EVT_STACK_UP = 0,
};


/* sub states of APP_AV_STATE_CONNECTED */
enum {
  APP_AV_MEDIA_STATE_IDLE,
  APP_AV_MEDIA_STATE_STARTING,
  APP_AV_MEDIA_STATE_STARTED,
  APP_AV_MEDIA_STATE_STOPPING,
};

BluetoothA2DPSource *self_BluetoothA2DPSource;

extern "C" void ccall_bt_av_hdl_stack_evt(uint16_t event, void *p_param) {
  if (self_BluetoothA2DPSource)
    self_BluetoothA2DPSource->bt_av_hdl_stack_evt(event, p_param);
}

extern "C" void ccall_bt_app_task_handler(void *arg) {
  if (self_BluetoothA2DPSource)
    self_BluetoothA2DPSource->bt_app_task_handler(arg);
}

extern "C" void ccall_bt_app_gap_callback(esp_bt_gap_cb_event_t event,
                                          esp_bt_gap_cb_param_t *param) {
  if (self_BluetoothA2DPSource)
    self_BluetoothA2DPSource->bt_app_gap_callback(event, param);
}

extern "C" void ccall_bt_app_rc_ct_cb(esp_avrc_ct_cb_event_t event,
                                      esp_avrc_ct_cb_param_t *param) {
  if (self_BluetoothA2DPSource)
    self_BluetoothA2DPSource->bt_app_rc_ct_cb(event, param);
}

extern "C" void ccall_a2d_app_heart_beat(TIMER_ARG_TYPE arg) {
  if (self_BluetoothA2DPSource)
    self_BluetoothA2DPSource->a2d_app_heart_beat(arg);
}

extern "C" void ccall_bt_app_a2d_cb(esp_a2d_cb_event_t event,
                                    esp_a2d_cb_param_t *param) {
  if (self_BluetoothA2DPSource)
    self_BluetoothA2DPSource->bt_app_a2d_cb(event, param);
}

extern "C" void ccall_bt_app_av_sm_hdlr(uint16_t event, void *param) {
  if (self_BluetoothA2DPSource)
    self_BluetoothA2DPSource->bt_app_av_sm_hdlr(event, param);
}

extern "C" void ccall_bt_av_hdl_avrc_ct_evt(uint16_t event, void *param) {
  if (self_BluetoothA2DPSource)
    self_BluetoothA2DPSource->bt_av_hdl_avrc_ct_evt(event, param);
}

extern "C" int32_t ccall_bt_app_a2d_data_cb(uint8_t *data, int32_t len) {
  // ESP_LOGD(BT_APP_TAG, "x%x - len: %d", __func__, len);
  if (len <= 0 || data == NULL || self_BluetoothA2DPSource == NULL ||
      self_BluetoothA2DPSource->data_stream_callback == NULL) {
    return 0;
  }
  int32_t result =
      (*(self_BluetoothA2DPSource->data_stream_callback))(data, len);
  // adapt volume
  if (result > 0 && self_BluetoothA2DPSource->is_volume_used) {
    self_BluetoothA2DPSource->volume_control()->update_audio_data((Frame *)data,
                                                                  result / 4);
  }
  return result;
}

extern "C" int32_t ccall_get_channel_data_wrapper(uint8_t *data, int32_t len) {
  // ESP_LOGD(BT_APP_TAG, "x%x - len: %d", __func__, len);
  if (len < 0 || data == NULL || self_BluetoothA2DPSource == NULL ||
      self_BluetoothA2DPSource->data_stream_channels_callback == NULL) {
    return 0;
  }
  memset(data, 0, len);
  return (*(self_BluetoothA2DPSource->data_stream_channels_callback))(
             (Frame *)data, len / 4) *
         4;
}

extern "C" int32_t ccall_get_data_default(uint8_t *data, int32_t len) {
  return self_BluetoothA2DPSource->get_data_default(data, len);
}

BluetoothA2DPSource::BluetoothA2DPSource() {
  ESP_LOGD(BT_APP_TAG, "%s, ", __func__);
  self_BluetoothA2DPSource = this;
  this->ssp_enabled = false;
  this->pin_type = ESP_BT_PIN_TYPE_VARIABLE;

  // default pin code
  strcpy((char *)pin_code, "1234");
  pin_code_len = 4;

  s_a2d_state = APP_AV_STATE_IDLE;
  s_media_state = APP_AV_MEDIA_STATE_IDLE;
  s_intv_cnt = 0;
  s_connecting_heatbeat_count = 0;
  s_pkt_cnt = 0;

  s_bt_app_task_queue = NULL;
  s_bt_app_task_handle = NULL;
}

BluetoothA2DPSource::~BluetoothA2DPSource() { end(); }

void BluetoothA2DPSource::set_pin_code(const char *pin_code,
                                       esp_bt_pin_type_t pin_type) {
  ESP_LOGD(BT_APP_TAG, "%s, ", __func__);
  this->pin_type = pin_type;
  this->pin_code_len = strlen(pin_code);
  strcpy((char *)this->pin_code, pin_code);
}

void BluetoothA2DPSource::start(const char *name,
                                music_data_channels_cb_t callback) {
  std::vector<const char *> names = {name};
  start(names, callback);
}

void BluetoothA2DPSource::start(std::vector<const char *> names,
                                music_data_channels_cb_t callback) {
  ESP_LOGD(BT_APP_TAG, "%s, ", __func__);
  if (callback != NULL) {
    // we use the indicated callback
    this->data_stream_channels_callback = callback;
    start_raw(names, ccall_get_channel_data_wrapper);
  } else {
    // we use the callback which supports write_data
    start_raw(names, ccall_get_data_default);
  }
}

void BluetoothA2DPSource::start_raw(const char *name,
                                    music_data_cb_t callback) {
  std::vector<const char *> names = {name};
  start_raw(names, callback);
}

void BluetoothA2DPSource::start_raw(std::vector<const char *> names,
                                    music_data_cb_t callback) {
  ESP_LOGD(BT_APP_TAG, "%s, ", __func__);
  this->bt_names = names;
  this->data_stream_callback = callback;
  is_autoreconnect_allowed = true;

  // get last connection if not available
  if (!has_last_connection()) {
    get_last_connection();
  }

    Serial.println("has_last_connection");
    Serial.printf("Free PSRAM after Wi-Fi init: %u bytes\n", ESP.getFreePsram());
    Serial.printf("Free heap after Wi-Fi init: %u bytes\n", ESP.getFreeHeap());
  // commented out because this is not working: the only way to get reliable reconnects
  // is to close the session properly before restarting!
  // reset last connection if we do not reconnect
  //reset_last_connection();

  if (nvs_init) {
    // Initialize NVS (Non-volatile storage library).
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
        ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
  }


    Serial.println("nvs_init");
    Serial.printf("Free PSRAM after Wi-Fi init: %u bytes\n", ESP.getFreePsram());
    Serial.printf("Free heap after Wi-Fi init: %u bytes\n", ESP.getFreeHeap());


  if (reset_ble) {
    ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_BLE));
  }

  if (!bt_start()) {
    ESP_LOGE(BT_AV_TAG, "Failed to initialize controller");
    return;
  }

    Serial.println("bt_start");
    Serial.printf("Free PSRAM after Wi-Fi init: %u bytes\n", ESP.getFreePsram());
    Serial.printf("Free heap after Wi-Fi init: %u bytes\n", ESP.getFreeHeap());

  if (bluedroid_init() != ESP_OK) {
    ESP_LOGE(BT_AV_TAG, "%s initialize bluedroid failed\n", __func__);
    return;
  }

    Serial.println("bluedroid_init");
    Serial.printf("Free PSRAM after Wi-Fi init: %u bytes\n", ESP.getFreePsram());
    Serial.printf("Free heap after Wi-Fi init: %u bytes\n", ESP.getFreeHeap());

  if (esp_bluedroid_enable() != ESP_OK) {
    ESP_LOGE(BT_AV_TAG, "%s enable bluedroid failed\n", __func__);
    return;
  }

    Serial.println("esp_bluedroid_enable");
    Serial.printf("Free PSRAM after Wi-Fi init: %u bytes\n", ESP.getFreePsram());
    Serial.printf("Free heap after Wi-Fi init: %u bytes\n", ESP.getFreeHeap());


  if (ssp_enabled) {
    /* Set default parameters for Secure Simple Pairing */
    esp_bt_sp_param_t param_type = ESP_BT_SP_IOCAP_MODE;
    esp_bt_io_cap_t iocap = ESP_BT_IO_CAP_IO;
    esp_bt_gap_set_security_param(param_type, &iocap, sizeof(uint8_t));
  }


    Serial.println("ssp_enabled");
    Serial.printf("Free PSRAM after Wi-Fi init: %u bytes\n", ESP.getFreePsram());
    Serial.printf("Free heap after Wi-Fi init: %u bytes\n", ESP.getFreeHeap());

  /*
   * Set default parameters for Legacy Pairing
   * Use variable pin, input pin code when pairing
   */
  esp_bt_gap_set_pin(pin_type, 0, pin_code);

  /* create application task */
  bt_app_task_start_up();

    Serial.println("bt_app_task_start_up");
    Serial.printf("Free PSRAM after Wi-Fi init: %u bytes\n", ESP.getFreePsram());
    Serial.printf("Free heap after Wi-Fi init: %u bytes\n", ESP.getFreeHeap());


  /* Bluetooth device name, connection mode and profile set up */
  bt_app_work_dispatch(ccall_bt_av_hdl_stack_evt, BT_APP_EVT_STACK_UP, NULL, 0,
                       NULL);

    Serial.println("bt_app_work_dispatch");
    Serial.printf("Free PSRAM after Wi-Fi init: %u bytes\n", ESP.getFreePsram());
    Serial.printf("Free heap after Wi-Fi init: %u bytes\n", ESP.getFreeHeap());
}

void BluetoothA2DPSource::reset_last_connection() {
  ESP_LOGI(BT_APP_TAG, "%s, ", __func__);
  [[maybe_unused]] const char *bda_str = to_str(last_connection);
  ESP_LOGD(BT_APP_TAG, "last connection %s, ", bda_str);
  if (has_last_connection()) {
    // the device might not have noticed that we are diconnected
    disconnect();
    delay_ms(2000);
    // remove bonding - so that we can reconnect
    //ESP_LOGD(BT_APP_TAG, "resetting %s, ", bda_str);
    //esp_bt_gap_remove_bond_device(last_connection);
  }
}

bool BluetoothA2DPSource::bt_app_work_dispatch(bt_app_cb_t p_cback,
                                               uint16_t event, void *p_params,
                                               int param_len,
                                               bt_app_copy_cb_t p_copy_cback) {
  ESP_LOGD(BT_APP_TAG, "%s event 0x%x, param len %d", __func__, event,
           param_len);

  bt_app_msg_t msg;
  memset(&msg, 0, sizeof(bt_app_msg_t));

  msg.sig = BT_APP_SIG_WORK_DISPATCH;
  msg.event = event;
  msg.cb = p_cback;

  if (param_len == 0) {
    return bt_app_send_msg(&msg);
  } else if (p_params && param_len > 0) {
    if ((msg.param = malloc(param_len)) != NULL) {
      memcpy(msg.param, p_params, param_len);
      /* check if caller has provided a copy callback to do the deep copy */
      if (p_copy_cback) {
        p_copy_cback(&msg, msg.param, p_params);
      }
      return bt_app_send_msg(&msg);
    }
  }

  return false;
}

bool BluetoothA2DPSource::bt_app_send_msg(bt_app_msg_t *msg) {
  if (msg == NULL) {
    return false;
  }

  if (xQueueSend(s_bt_app_task_queue, msg, 10 / portTICK_RATE_MS) != pdTRUE) {
    ESP_LOGE(BT_APP_TAG, "%s xQueue send failed", __func__);
    return false;
  }
  return true;
}

void BluetoothA2DPSource::bt_app_work_dispatched(bt_app_msg_t *msg) {
  if (msg->cb) {
    msg->cb(msg->event, msg->param);
  }
}
void BluetoothA2DPSource::bt_app_task_handler(void *arg) {
  bt_app_msg_t msg;

  for (;;) {
    /* receive message from work queue and handle it */
    if (pdTRUE ==
        xQueueReceive(s_bt_app_task_queue, &msg, (TickType_t)portMAX_DELAY)) {
      ESP_LOGD(BT_APP_TAG, "%s, signal: 0x%x, event: 0x%x", __func__, msg.sig,
               msg.event);

      switch (msg.sig) {
      case BT_APP_SIG_WORK_DISPATCH:
        bt_app_work_dispatched(&msg);
        break;
      default:
        ESP_LOGW(BT_APP_TAG, "%s, unhandled signal: %d", __func__, msg.sig);
        break;
      }

      if (msg.param) {
        free(msg.param);
      }
    }
  }
}

void BluetoothA2DPSource::bt_app_task_start_up(void) {
  ESP_LOGD(BT_AV_TAG, "%s", __func__);
  s_bt_app_task_queue = xQueueCreate(event_queue_size, sizeof(bt_app_msg_t));
  if (xTaskCreatePinnedToCore(ccall_bt_app_task_handler, "BtAppT",
                              event_stack_size, NULL, task_priority,
                              &s_bt_app_task_handle, task_core) != pdPASS) {
    ESP_LOGE(BT_AV_TAG, "xTaskCreatePinnedToCore");
  }
  return;
}

void BluetoothA2DPSource::bt_app_task_shut_down(void) {
  ESP_LOGD(BT_AV_TAG, "%s", __func__);
  if (s_bt_app_task_handle) {
    vTaskDelete(s_bt_app_task_handle);
    s_bt_app_task_handle = NULL;
  }
  if (s_bt_app_task_queue) {
    vQueueDelete(s_bt_app_task_queue);
    s_bt_app_task_queue = NULL;
  }
}

bool BluetoothA2DPSource::get_name_from_eir(uint8_t *eir, uint8_t *bdname,
                                            uint8_t *bdname_len) {
  uint8_t *rmt_bdname = NULL;
  uint8_t rmt_bdname_len = 0;

  if (!eir) {
    return false;
  }

  rmt_bdname = esp_bt_gap_resolve_eir_data(eir, ESP_BT_EIR_TYPE_CMPL_LOCAL_NAME,
                                           &rmt_bdname_len);
  if (!rmt_bdname) {
    rmt_bdname = esp_bt_gap_resolve_eir_data(
        eir, ESP_BT_EIR_TYPE_SHORT_LOCAL_NAME, &rmt_bdname_len);
  }

  if (rmt_bdname) {
    if (rmt_bdname_len > ESP_BT_GAP_MAX_BDNAME_LEN) {
      rmt_bdname_len = ESP_BT_GAP_MAX_BDNAME_LEN;
    }

    if (bdname) {
      memcpy(bdname, rmt_bdname, rmt_bdname_len);
      bdname[rmt_bdname_len] = '\0';
    }
    if (bdname_len) {
      *bdname_len = rmt_bdname_len;
    }
    return true;
  }

  return false;
}

void BluetoothA2DPSource::filter_inquiry_scan_result(
    esp_bt_gap_cb_param_t *param) {
  uint32_t cod = 0;
  int32_t rssi = -129; /* invalid value */
  uint8_t *eir = NULL;
  esp_bt_gap_dev_prop_t *p;

  ESP_LOGI(BT_AV_TAG, "Scanned device: %s", to_str(param->disc_res.bda));
  for (int i = 0; i < param->disc_res.num_prop; i++) {
    p = param->disc_res.prop + i;
    switch (p->type) {
    case ESP_BT_GAP_DEV_PROP_COD:
      cod = *(uint32_t *)(p->val);
      ESP_LOGI(BT_AV_TAG, "--Class of Device: 0x%x", cod);
      break;
    case ESP_BT_GAP_DEV_PROP_RSSI:
      rssi = *(int8_t *)(p->val); // in ESP_BT_GAP_DISC_RES_EVT
      ESP_LOGI(BT_AV_TAG, "--RSSI: %d", rssi);
      break;
    case ESP_BT_GAP_DEV_PROP_EIR:
      eir = (uint8_t *)(p->val);
      break;
    case ESP_BT_GAP_DEV_PROP_BDNAME:
    default:
      break;
    }
  }
  /* search for device with MAJOR service class as "rendering" in COD */
  if (!esp_bt_gap_is_valid_cod(cod) ||
      !(esp_bt_gap_get_cod_srvc(cod) & ESP_BT_COD_SRVC_RENDERING)) {
    ESP_LOGI(BT_AV_TAG, "--Compatiblity: Incompatible");
    return;
  }

  /* search for target device in its Extended Inqury Response */
  if (eir) {

    ESP_LOGI(BT_AV_TAG, "--Compatiblity: Compatible");
    get_name_from_eir(eir, s_peer_bdname, NULL);
    ESP_LOGI(BT_AV_TAG, "--Name: %s", s_peer_bdname);

    // check ssid names from provided list
    bool found = false;
    // ssid callback
    if (ssid_callback != nullptr) {
      found =
          ssid_callback((const char *)s_peer_bdname, param->disc_res.bda, rssi);
    } else {
      // if no callback we use the list
      for (const char *name : bt_names) {
        int len = strlen(name);
        ESP_LOGD(BT_AV_TAG, "--Checking match: %s", name);
        if (strncmp((char *)s_peer_bdname, name, len) == 0) {
          this->bt_name = (char *)s_peer_bdname;
          found = true;
          break;
        }
      }
    }
    if (found) {
      ESP_LOGI(BT_AV_TAG, "--Result: Target device found");
      s_a2d_state = APP_AV_STATE_DISCOVERED;
      memcpy(peer_bd_addr, param->disc_res.bda, ESP_BD_ADDR_LEN);
      set_last_connection(peer_bd_addr);
      ESP_LOGI(BT_AV_TAG, "Cancel device discovery ...");
      esp_bt_gap_cancel_discovery();
    } else {
      ESP_LOGI(BT_AV_TAG, "--Result: Target device not found");
    }
  }
}

void BluetoothA2DPSource::bt_app_gap_callback(esp_bt_gap_cb_event_t event,
                                              esp_bt_gap_cb_param_t *param) {
  ESP_LOGD(BT_AV_TAG, "%s evt %d", __func__, event);
  switch (event) {
  case ESP_BT_GAP_DISC_RES_EVT: {
    filter_inquiry_scan_result(param);
    break;
  }
  case ESP_BT_GAP_DISC_STATE_CHANGED_EVT: {
    if (param->disc_st_chg.state == ESP_BT_GAP_DISCOVERY_STOPPED) {
      if (s_a2d_state == APP_AV_STATE_DISCOVERED) {
        s_a2d_state = APP_AV_STATE_CONNECTING;
        ESP_LOGI(BT_AV_TAG, "Device discovery stopped.");
        ESP_LOGI(BT_AV_TAG, "a2dp connecting to peer: %s", s_peer_bdname);
        esp_a2d_connect(peer_bd_addr);
      } else {
        // not discovered, continue to discover
        ESP_LOGI(BT_AV_TAG, "Device discovery failed, continue to discover...");
        esp_bt_gap_start_discovery(ESP_BT_INQ_MODE_GENERAL_INQUIRY, 10, 0);
      }
    } else if (param->disc_st_chg.state == ESP_BT_GAP_DISCOVERY_STARTED) {
      ESP_LOGI(BT_AV_TAG, "Discovery started.");
    }
    break;
  }
  case ESP_BT_GAP_RMT_SRVCS_EVT:
  case ESP_BT_GAP_RMT_SRVC_REC_EVT:
    break;
  case ESP_BT_GAP_AUTH_CMPL_EVT: {
    if (param->auth_cmpl.stat == ESP_BT_STATUS_SUCCESS) {
      ESP_LOGI(BT_AV_TAG, "authentication success: %s",
               param->auth_cmpl.device_name);
      esp_log_buffer_hex(BT_AV_TAG, param->auth_cmpl.bda, ESP_BD_ADDR_LEN);
    } else {
      ESP_LOGE(BT_AV_TAG, "authentication failed, status:%d",
               param->auth_cmpl.stat);
    }
    break;
  }
  case ESP_BT_GAP_PIN_REQ_EVT: {
    ESP_LOGI(BT_AV_TAG, "ESP_BT_GAP_PIN_REQ_EVT min_16_digit:%d",
             param->pin_req.min_16_digit);
       ESP_LOGI(BT_AV_TAG, "Input pin code: %s", pin_code);
       esp_bt_gap_pin_reply(param->pin_req.bda, true, pin_code_len, pin_code);

    // if (param->pin_req.min_16_digit) {
    //   ESP_LOGI(BT_AV_TAG, "Input pin code: 0000 0000 0000 0000");
    //   esp_bt_pin_code_t pin_code = {0};
    //   esp_bt_gap_pin_reply(param->pin_req.bda, true, 16, pin_code);
    // } else {
    //   ESP_LOGI(BT_AV_TAG, "Input pin code: 1234");
    //   esp_bt_gap_pin_reply(param->pin_req.bda, true, pin_code_len, pin_code);
    // }
    break;
  }

  /* when Security Simple Pairing passkey notified, this event comes */
  case ESP_BT_GAP_KEY_NOTIF_EVT:
    if (!ssp_enabled)
      break;
    ESP_LOGI(BT_AV_TAG, "ESP_BT_GAP_KEY_NOTIF_EVT passkey:%d",
             param->key_notif.passkey);
    break;
  /* when Security Simple Pairing passkey requested, this event comes */
  case ESP_BT_GAP_KEY_REQ_EVT:
    if (!ssp_enabled)
      break;
    ESP_LOGI(BT_AV_TAG, "ESP_BT_GAP_KEY_REQ_EVT Please enter passkey!");
    break;

  /* when Security Simple Pairing user confirmation requested, this event comes
   */
  case ESP_BT_GAP_CFM_REQ_EVT:
    ESP_LOGI(
        BT_AV_TAG,
        "ESP_BT_GAP_CFM_REQ_EVT Please compare the numeric value: %" PRIu32,
        param->cfm_req.num_val);
    esp_bt_gap_ssp_confirm_reply(param->cfm_req.bda, true);
    break;

#if ESP_IDF_VERSION > ESP_IDF_VERSION_VAL(4, 4, 4)
  case ESP_BT_GAP_ACL_CONN_CMPL_STAT_EVT:
    ESP_LOGI( BT_AV_TAG,"ESP_BT_GAP_ACL_CONN_CMPL_STAT_EVT");
    break;

  case ESP_BT_GAP_MODE_CHG_EVT:
    ESP_LOGI(BT_AV_TAG, "ESP_BT_GAP_MODE_CHG_EVT mode:%d",
             param->mode_chg.mode);
    break;
#endif

  default: {
    ESP_LOGI(BT_AV_TAG, "event: %d", event);
    break;
  }
  }
  return;
}

void BluetoothA2DPSource::bt_av_hdl_stack_evt(uint16_t event, void *p_param) {
  ESP_LOGD(BT_AV_TAG, "%s event: %d", __func__, event);

  switch (event) {
  /* when stack up worked, this event comes */
  case BT_APP_EVT_STACK_UP: {
    // set up device name
    esp_bt_dev_set_device_name(dev_name);
    // register GAP callback function
    esp_bt_gap_register_callback(ccall_bt_app_gap_callback);

    // initialize AVRCP controller
    esp_avrc_ct_init();
    esp_avrc_ct_register_callback(ccall_bt_app_rc_ct_cb);

#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 0, 0)
    esp_avrc_rn_evt_cap_mask_t evt_set = {0};
    esp_avrc_rn_evt_bit_mask_operation(ESP_AVRC_BIT_MASK_OP_SET, &evt_set,
                                       ESP_AVRC_RN_VOLUME_CHANGE);
    assert(esp_avrc_tg_set_rn_evt_cap(&evt_set) == ESP_OK);
#endif

    esp_a2d_source_init();
    esp_a2d_register_callback(&ccall_bt_app_a2d_cb);
    esp_a2d_source_register_data_callback(&ccall_bt_app_a2d_data_cb);

    /* Avoid the state error of s_a2d_state caused by the connection initiated
     * by the peer device. */
    //esp_bt_gap_set_scan_mode(ESP_BT_NON_CONNECTABLE, ESP_BT_NON_DISCOVERABLE);
    set_scan_mode_connectable(false);
    if (reconnect_status == AutoReconnect && has_last_connection()) {
      ESP_LOGW(BT_AV_TAG, "Reconnecting to %s", to_str(last_connection));
      memcpy(peer_bd_addr, last_connection, ESP_BD_ADDR_LEN);
      connect_to(last_connection);
      s_a2d_state = APP_AV_STATE_CONNECTING;
    } else {

      ESP_LOGI(BT_AV_TAG, "Starting device discovery...");
      s_a2d_state = APP_AV_STATE_DISCOVERING;
      esp_bt_gap_start_discovery(ESP_BT_INQ_MODE_GENERAL_INQUIRY, 10, 0);
    }
    /* create and start heart beat timer */
      int tmr_id = 0;
      s_tmr = xTimerCreate("connTmr", (10000 / portTICK_PERIOD_MS), pdTRUE,
                           (void *)&tmr_id, ccall_a2d_app_heart_beat);
      xTimerStart(s_tmr, portMAX_DELAY);
    break;
  }
  /* other */
  default: {
    ESP_LOGW(BT_AV_TAG, "%s unhandled event: %d", __func__, event);
    break;
  }
  }
}

void BluetoothA2DPSource::bt_app_a2d_cb(esp_a2d_cb_event_t event,
                                        esp_a2d_cb_param_t *param) {
  bt_app_work_dispatch(ccall_bt_app_av_sm_hdlr, event, param,
                       sizeof(esp_a2d_cb_param_t), NULL);
}

void BluetoothA2DPSource::a2d_app_heart_beat(void *arg) {
  bt_app_work_dispatch(ccall_bt_app_av_sm_hdlr, BT_APP_HEART_BEAT_EVT, NULL, 0,
                       NULL);
}

void BluetoothA2DPSource::process_user_state_callbacks(uint16_t event,
                                                       void *param) {
  ESP_LOGD(BT_AV_TAG, "%s", __func__);

  esp_a2d_cb_param_t *a2d = NULL;

  // callbacks
  switch (event) {
  case ESP_A2D_CONNECTION_STATE_EVT:
    a2d = (esp_a2d_cb_param_t *)(param);
    ESP_LOGD(BT_AV_TAG, "%s ESP_A2D_CONNECTION_STATE_EVT: %s", __func__,
             to_str(a2d->conn_stat.state));

    // callback on state change
    connection_state = a2d->conn_stat.state;
    if (connection_state_callback != nullptr) {
      connection_state_callback(a2d->conn_stat.state, connection_state_obj);
    }
    break;

  case ESP_A2D_AUDIO_STATE_EVT:
    a2d = (esp_a2d_cb_param_t *)(param);
    ESP_LOGD(BT_AV_TAG, "%s ESP_A2D_AUDIO_STATE_EVT: %s", __func__,
             to_str(a2d->audio_stat.state));

    // callback on state change
    if (audio_state_callback != nullptr) {
      audio_state_callback(a2d->audio_stat.state, audio_state_obj);
      audio_state = a2d->audio_stat.state;
    }
    break;
  }
}

void BluetoothA2DPSource::bt_app_av_sm_hdlr(uint16_t event, void *param) {
  ESP_LOGI(BT_AV_TAG, "%s state %s, evt 0x%x", __func__, to_state_str(s_a2d_state), event);
  process_user_state_callbacks(event, param);

  /* select handler according to different states */
  switch (s_a2d_state) {
  case APP_AV_STATE_DISCOVERING:
  case APP_AV_STATE_DISCOVERED:
    break;
  case APP_AV_STATE_UNCONNECTED:
    bt_app_av_state_unconnected_hdlr(event, param);
    break;
  case APP_AV_STATE_CONNECTING:
    bt_app_av_state_connecting_hdlr(event, param);
    break;
  case APP_AV_STATE_CONNECTED:
    bt_app_av_state_connected_hdlr(event, param);
    break;
  case APP_AV_STATE_DISCONNECTING:
    bt_app_av_state_disconnecting_hdlr(event, param);
    break;
  default:
    ESP_LOGE(BT_AV_TAG, "%s invalid state: %d", __func__, s_a2d_state);
    break;
  }
}

void BluetoothA2DPSource::bt_app_av_state_unconnected_hdlr(uint16_t event,
                                                           void *param) {
  ESP_LOGD(BT_AV_TAG, "%s evt %d", __func__, event);
  //esp_a2d_cb_param_t *a2d = NULL;
  /* handle the events of intrest in unconnected state */
  switch (event) {
  case ESP_A2D_CONNECTION_STATE_EVT:
  case ESP_A2D_AUDIO_STATE_EVT:
  case ESP_A2D_AUDIO_CFG_EVT:
  case ESP_A2D_MEDIA_CTRL_ACK_EVT:
    break;
  case BT_APP_HEART_BEAT_EVT: {
    esp_a2d_connect(peer_bd_addr);
    s_a2d_state = APP_AV_STATE_CONNECTING;
    s_connecting_heatbeat_count = 0;
    break;
  }
  // case ESP_A2D_REPORT_SNK_DELAY_VALUE_EVT: {
  //     a2d = (esp_a2d_cb_param_t *)(param);
  //     ESP_LOGI(BT_AV_TAG, "%s, delay value: %u * 1/10 ms", __func__,
  //     a2d->a2d_report_delay_value_stat.delay_value); break;
  // }
  default: {
    ESP_LOGW(BT_AV_TAG, "%s unhandled event: %d", __func__, event);
    break;
  }
  }
}

void BluetoothA2DPSource::bt_app_av_state_connecting_hdlr(uint16_t event,
                                                          void *param) {
  ESP_LOGD(BT_AV_TAG, "%s evt %d", __func__, event);
  esp_a2d_cb_param_t *a2d = NULL;

  /* handle the events of intrest in connecting state */
  switch (event) {
  case ESP_A2D_CONNECTION_STATE_EVT: {
    a2d = (esp_a2d_cb_param_t *)(param);
    if (a2d->conn_stat.state == ESP_A2D_CONNECTION_STATE_CONNECTED) {
      ESP_LOGI(BT_AV_TAG, "a2dp connected");
      s_a2d_state = APP_AV_STATE_CONNECTED;
      s_media_state = APP_AV_MEDIA_STATE_IDLE;
    } else if (a2d->conn_stat.state == ESP_A2D_CONNECTION_STATE_DISCONNECTED) {
      s_a2d_state = APP_AV_STATE_UNCONNECTED;
    }
    break;
  }
  case ESP_A2D_AUDIO_STATE_EVT:
  case ESP_A2D_AUDIO_CFG_EVT:
  case ESP_A2D_MEDIA_CTRL_ACK_EVT:
    break;
  case BT_APP_HEART_BEAT_EVT:
    /**
     * Switch state to APP_AV_STATE_UNCONNECTED
     * when connecting lasts more than 2 heart beat intervals.
     */
    if (++s_connecting_heatbeat_count >= 2) {
      s_a2d_state = APP_AV_STATE_UNCONNECTED;
      s_connecting_heatbeat_count = 0;
    }
    break;
  // case ESP_A2D_REPORT_SNK_DELAY_VALUE_EVT: {
  //     a2d = (esp_a2d_cb_param_t *)(param);
  //     ESP_LOGI(BT_AV_TAG, "%s, delay value: %u * 1/10 ms", __func__,
  //     a2d->a2d_report_delay_value_stat.delay_value); break;
  // }
  default:
    ESP_LOGW(BT_AV_TAG, "%s unhandled event: %d", __func__, event);
    break;
  }
}

void BluetoothA2DPSource::bt_app_av_state_connected_hdlr(uint16_t event,
                                                         void *param) {
  ESP_LOGD(BT_AV_TAG, "%s evt %d", __func__, event);
  esp_a2d_cb_param_t *a2d = NULL;

  /* handle the events of intrest in connected state */
  switch (event) {
  case ESP_A2D_CONNECTION_STATE_EVT: {
    a2d = (esp_a2d_cb_param_t *)(param);
    if (a2d->conn_stat.state == ESP_A2D_CONNECTION_STATE_DISCONNECTED) {
      ESP_LOGI(BT_AV_TAG, "a2dp disconnected");
      s_a2d_state = APP_AV_STATE_UNCONNECTED;
    }
    break;
  }
  case ESP_A2D_AUDIO_STATE_EVT: {
    a2d = (esp_a2d_cb_param_t *)(param);
    if (ESP_A2D_AUDIO_STATE_STARTED == a2d->audio_stat.state) {
      s_pkt_cnt = 0;
    }
    break;
  }
  case ESP_A2D_AUDIO_CFG_EVT:
    // not suppposed to occur for A2DP source
    break;
  case ESP_A2D_MEDIA_CTRL_ACK_EVT:
  case BT_APP_HEART_BEAT_EVT: {
    bt_app_av_media_proc(event, param);
    break;
  }
  // case ESP_A2D_REPORT_SNK_DELAY_VALUE_EVT: {
  //     a2d = (esp_a2d_cb_param_t *)(param);
  //     ESP_LOGI(BT_AV_TAG, "%s, delay value: %u * 1/10 ms", __func__,
  //     a2d->a2d_report_delay_value_stat.delay_value); break;
  // }
  default: {
    ESP_LOGW(BT_AV_TAG, "%s unhandled event: %d", __func__, event);
    break;
  }
  }
}

void BluetoothA2DPSource::bt_app_av_state_disconnecting_hdlr(uint16_t event,
                                                             void *param) {
  ESP_LOGD(BT_AV_TAG, "%s evt %d", __func__, event);
  esp_a2d_cb_param_t *a2d = NULL;

  /* handle the events of intrest in disconnecing state */
  switch (event) {
  case ESP_A2D_CONNECTION_STATE_EVT: {
    a2d = (esp_a2d_cb_param_t *)(param);
    if (a2d->conn_stat.state == ESP_A2D_CONNECTION_STATE_DISCONNECTED) {
      ESP_LOGI(BT_AV_TAG, "a2dp disconnected");
      s_a2d_state = APP_AV_STATE_UNCONNECTED;
    }
    break;
  }
  case ESP_A2D_AUDIO_STATE_EVT:
  case ESP_A2D_AUDIO_CFG_EVT:
  case ESP_A2D_MEDIA_CTRL_ACK_EVT:
  case BT_APP_HEART_BEAT_EVT:
    break;
  // case ESP_A2D_REPORT_SNK_DELAY_VALUE_EVT: {
  //     a2d = (esp_a2d_cb_param_t *)(param);
  //     ESP_LOGI(BT_AV_TAG, "%s, delay value: 0x%u * 1/10 ms", __func__,
  //     a2d->a2d_report_delay_value_stat.delay_value); break;
  // }
  default: {
    ESP_LOGW(BT_AV_TAG, "%s unhandled event: %d", __func__, event);
    break;
  }
  }
}

void BluetoothA2DPSource::bt_app_av_media_proc(uint16_t event, void *param) {
  ESP_LOGD(BT_AV_TAG, "%s evt %d", __func__, event);
  esp_a2d_cb_param_t *a2d = NULL;
  switch (s_media_state) {
  case APP_AV_MEDIA_STATE_IDLE: {
    if (event == BT_APP_HEART_BEAT_EVT) {
      ESP_LOGI(BT_AV_TAG, "a2dp media ready checking ...");
      esp_a2d_media_ctrl(ESP_A2D_MEDIA_CTRL_CHECK_SRC_RDY);
    } else if (event == ESP_A2D_MEDIA_CTRL_ACK_EVT) {
      a2d = (esp_a2d_cb_param_t *)(param);
      if (a2d->media_ctrl_stat.cmd == ESP_A2D_MEDIA_CTRL_CHECK_SRC_RDY &&
          a2d->media_ctrl_stat.status == ESP_A2D_MEDIA_CTRL_ACK_SUCCESS) {
        ESP_LOGI(BT_AV_TAG, "a2dp media ready, starting ...");
        esp_a2d_media_ctrl(ESP_A2D_MEDIA_CTRL_START);
        s_media_state = APP_AV_MEDIA_STATE_STARTING;
      }
    }
    break;
  }
  case APP_AV_MEDIA_STATE_STARTING: {
    if (event == ESP_A2D_MEDIA_CTRL_ACK_EVT) {
      a2d = (esp_a2d_cb_param_t *)(param);
      if (a2d->media_ctrl_stat.cmd == ESP_A2D_MEDIA_CTRL_START &&
          a2d->media_ctrl_stat.status == ESP_A2D_MEDIA_CTRL_ACK_SUCCESS) {
        ESP_LOGI(BT_AV_TAG, "a2dp media start successfully.");
        s_intv_cnt = 0;
        s_media_state = APP_AV_MEDIA_STATE_STARTED;
      } else {
        // not started succesfully, transfer to idle state
        ESP_LOGI(BT_AV_TAG, "a2dp media start failed.");
        s_media_state = APP_AV_MEDIA_STATE_IDLE;
      }
    }
    break;
  }

  case APP_AV_MEDIA_STATE_STARTED: {
    break;
  }

  case APP_AV_MEDIA_STATE_STOPPING: {
    if (event == ESP_A2D_MEDIA_CTRL_ACK_EVT) {
      a2d = (esp_a2d_cb_param_t *)(param);
      if (a2d->media_ctrl_stat.cmd == ESP_A2D_MEDIA_CTRL_STOP &&
          a2d->media_ctrl_stat.status == ESP_A2D_MEDIA_CTRL_ACK_SUCCESS) {
        ESP_LOGI(BT_AV_TAG,
                 "a2dp media stopped successfully, disconnecting...");
        s_media_state = APP_AV_MEDIA_STATE_IDLE;
        esp_a2d_source_disconnect(peer_bd_addr);
        s_a2d_state = APP_AV_STATE_DISCONNECTING;
      } else {
        ESP_LOGI(BT_AV_TAG, "a2dp media stopping...");
        esp_a2d_media_ctrl(ESP_A2D_MEDIA_CTRL_STOP);
      }
    }
    break;
  }
  }
}

void BluetoothA2DPSource::bt_app_rc_ct_cb(esp_avrc_ct_cb_event_t event,
                                          esp_avrc_ct_cb_param_t *param) {
  ESP_LOGD(BT_RC_CT_TAG, "%s evt %d", __func__, event);
  switch (event) {
  case ESP_AVRC_CT_CONNECTION_STATE_EVT:
  case ESP_AVRC_CT_PASSTHROUGH_RSP_EVT:
  case ESP_AVRC_CT_METADATA_RSP_EVT:
  case ESP_AVRC_CT_CHANGE_NOTIFY_EVT:
  case ESP_AVRC_CT_REMOTE_FEATURES_EVT:
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 0, 0)
  case ESP_AVRC_CT_GET_RN_CAPABILITIES_RSP_EVT:
  case ESP_AVRC_CT_SET_ABSOLUTE_VOLUME_RSP_EVT: 
#endif
  {
    bt_app_work_dispatch(ccall_bt_av_hdl_avrc_ct_evt, event, param,
                         sizeof(esp_avrc_ct_cb_param_t), NULL);
    break;
  }
  default: {
    ESP_LOGW(BT_RC_CT_TAG, "Unhandled AVRC event: %d", event);
    break;
  }
  }
}

#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 0, 0)

void BluetoothA2DPSource::bt_av_volume_changed(void) {
  if (esp_avrc_rn_evt_bit_mask_operation(ESP_AVRC_BIT_MASK_OP_TEST,
                                         &s_avrc_peer_rn_cap,
                                         ESP_AVRC_RN_VOLUME_CHANGE)) {
    esp_avrc_ct_send_register_notification_cmd(APP_RC_CT_TL_RN_VOLUME_CHANGE,
                                               ESP_AVRC_RN_VOLUME_CHANGE, 0);
  }
}

void BluetoothA2DPSource::bt_av_notify_evt_handler(
    uint8_t event_id, esp_avrc_rn_param_t *event_parameter) {
  ESP_LOGD(BT_RC_CT_TAG, "%s evt %d", __func__, event_id);
  switch (event_id) {
  case ESP_AVRC_RN_VOLUME_CHANGE:
    ESP_LOGI(BT_RC_CT_TAG, "Volume changed: %d", event_parameter->volume);
    // limit the value to 127
    uint8_t new_volume = std::min((int)event_parameter->volume, 0x7f);
    ESP_LOGI(BT_RC_CT_TAG, "Set absolute volume: volume %d", new_volume);
    esp_avrc_ct_send_set_absolute_volume_cmd(APP_RC_CT_TL_RN_VOLUME_CHANGE,
                                             new_volume);
    set_volume(new_volume);
    bt_av_volume_changed();
    break;
  }
}

#endif

void BluetoothA2DPSource::bt_av_hdl_avrc_ct_evt(uint16_t event, void *p_param) {
  ESP_LOGD(BT_RC_CT_TAG, "%s evt %d", __func__, event);
  esp_avrc_ct_cb_param_t *rc = (esp_avrc_ct_cb_param_t *)(p_param);

  switch (event) {
  /* when connection state changed, this event comes */
  case ESP_AVRC_CT_CONNECTION_STATE_EVT: {
    [[maybe_unused]] uint8_t *bda = rc->conn_stat.remote_bda;
    ESP_LOGI(BT_RC_CT_TAG,
             "AVRC conn_state event: state %d, [%02x:%02x:%02x:%02x:%02x:%02x]",
             rc->conn_stat.connected, bda[0], bda[1], bda[2], bda[3], bda[4],
             bda[5]);

#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 0, 0)
    if (rc->conn_stat.connected) {
      esp_avrc_ct_send_get_rn_capabilities_cmd(APP_RC_CT_TL_GET_CAPS);
    } else {
      s_avrc_peer_rn_cap.bits = 0;
    }
#endif
    break;
  }
  /* when passthrough responsed, this event comes */
  case ESP_AVRC_CT_PASSTHROUGH_RSP_EVT: {
    ESP_LOGI(
        BT_RC_CT_TAG,
        "AVRC passthrough response: key_code 0x%x, key_state %d, rsp_code %d",
        rc->psth_rsp.key_code, rc->psth_rsp.key_state, rc->psth_rsp.rsp_code);
    break;
  }
  /* when metadata responsed, this event comes */
  case ESP_AVRC_CT_METADATA_RSP_EVT: {
    ESP_LOGI(BT_RC_CT_TAG, "AVRC metadata response: attribute id 0x%x, %s",
             rc->meta_rsp.attr_id, rc->meta_rsp.attr_text);
    free(rc->meta_rsp.attr_text);
    break;
  }
  /* when notification changed, this event comes */
  case ESP_AVRC_CT_CHANGE_NOTIFY_EVT: {
    ESP_LOGI(BT_RC_CT_TAG, "AVRC event notification: %d",
             rc->change_ntf.event_id);
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 0, 0)
    bt_av_notify_evt_handler(rc->change_ntf.event_id,
                             &rc->change_ntf.event_parameter);
#endif
    break;
  }
  /* when indicate feature of remote device, this event comes */
  case ESP_AVRC_CT_REMOTE_FEATURES_EVT: {
    ESP_LOGI(BT_RC_CT_TAG, "AVRC remote features %" PRIx32 ", TG features %x",
             rc->rmt_feats.feat_mask, rc->rmt_feats.tg_feat_flag);
    break;
  }

#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 0, 0)
  /* when get supported notification events capability of peer device, this
   * event comes */
  case ESP_AVRC_CT_GET_RN_CAPABILITIES_RSP_EVT: {
    ESP_LOGI(BT_RC_CT_TAG, "remote rn_cap: count %d, bitmask 0x%x",
             rc->get_rn_caps_rsp.cap_count, rc->get_rn_caps_rsp.evt_set.bits);
    s_avrc_peer_rn_cap.bits = rc->get_rn_caps_rsp.evt_set.bits;

    bt_av_volume_changed();
    break;
  }
  /* when set absolute volume responsed, this event comes */
  case ESP_AVRC_CT_SET_ABSOLUTE_VOLUME_RSP_EVT: {
    ESP_LOGI(BT_RC_CT_TAG, "Set absolute volume response: volume %d",
             rc->set_volume_rsp.volume);
    set_volume(rc->set_volume_rsp.volume);
    break;
  }
#endif
  /* other */
  default: {
    ESP_LOGW(BT_RC_CT_TAG, "%s unhandled event: %d", __func__, event);
    break;
  }
  }
}

bool BluetoothA2DPSource::has_sound_data() { return this->has_sound_data_flag; }

bool BluetoothA2DPSource::write_data(SoundData *data) {
  this->sound_data = data;
  this->sound_data_current_pos = 0;
  this->has_sound_data_flag = true;
  return true;
}

int32_t BluetoothA2DPSource::get_data_default(uint8_t *data, int32_t len) {
  uint32_t result_len;
  if (has_sound_data()) {
    result_len = sound_data->get2ChannelData(sound_data_current_pos, len, data);
    if (result_len != 512) {
      ESP_LOGD(BT_APP_TAG, "=> len: %d / result_len: %d", len, result_len);
    }
    // calculate next position
    sound_data_current_pos += result_len;
    if (result_len <= 0) {
      if (sound_data->doLoop()) {
        ESP_LOGD(BT_APP_TAG, "%s - end of data: restarting", __func__);
        sound_data_current_pos = 0;
      } else {
        ESP_LOGD(BT_APP_TAG, "%s - end of data: stopping", __func__);
        has_sound_data_flag = false;
      }
    }
  } else {
    // return silence
    memset(data, 0, len);
    result_len = len;
  }

  return result_len;
}

void BluetoothA2DPSource::set_nvs_init(bool doInit) { nvs_init = doInit; }

void BluetoothA2DPSource::set_reset_ble(bool doInit) { reset_ble = doInit; }
