#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "dhub_map.h"
#include "cJSON.h"
#define RANDOM_VALUE_LIMIT 50

float generate_random_float(void);
void generate_values(zMessage *sendBuff ,int n);
void print_all_vars(zMessage *sendBuff);
int read_prod_no(char* config_json);