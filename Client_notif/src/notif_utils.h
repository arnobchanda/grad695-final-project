#include <sys/stat.h>

#include "dhub_map.h"
#include "cJSON.h"

#define CONFIG_NOTIF_JSON "include/confignotif.json"

void print_all_values(zMessage *recvBuff);
void print_var_value(zMessage *recvBuff, int var_num);
void update_notif_list(zMessage *notifMsg);
int check_json_file_size(char* file_name);