void wifi_task_fn(void *arg);

#ifdef BOARD_PCA10056
/** Wi-Fi Settings */
#define MAIN_WLAN_SSID                    "Kommandor" /**< Destination SSID */
#define MAIN_WLAN_AUTH                    M2M_WIFI_SEC_WPA_PSK /**< Security manner */
#define MAIN_WLAN_PSK                     "mastodonwhale" /**< Password for Destination SSID */
#else
#ifdef BOARD_CUSTOM
#define MAIN_WLAN_SSID                    "NPI Shop 2.4" /**< Destination SSID */
#define MAIN_WLAN_AUTH                    M2M_WIFI_SEC_WPA_PSK /**< Security manner */
#define MAIN_WLAN_PSK                     "ellicottville" /**< Password for Destination SSID */
#endif //BOARD_PCA10056
#endif //BOARD_CUSTOM


//#define MAIN_WIFI_M2M_SERVER_IP             0xc0a80124
//#define MAIN_WIFI_M2M_SERVER_PORT         (3100)
//#define MAIN_WIFI_M2M_REPORT_INTERVAL     (1000)

//#define MAIN_WIFI_M2M_BUFFER_SIZE          1460