#include "notif_utils.h"

void print_all_values(zMessage *recvBuff)
{
    printf("Var 1: %f\n", recvBuff->data.var1);
    printf("Var 2: %f\n", recvBuff->data.var2);
    printf("Var 3: %f\n", recvBuff->data.var3);
    printf("Var 4: %f\n", recvBuff->data.var4);
    printf("Var 5: %f\n", recvBuff->data.var5);
    printf("Var 6: %f\n", recvBuff->data.var6);
    printf("Var 7: %f\n", recvBuff->data.var7);
    printf("Var 8: %f\n", recvBuff->data.var8);
    printf("Var 9: %f\n", recvBuff->data.var9);
    printf("Var 10: %f\n", recvBuff->data.var10);
}

void update_notif_list(zMessage *notifMsg)
{
    FILE *fp;
    char json_buffer[4096];

    printf("Updating Notification List\n");
    fp = fopen(CONFIG_NOTIF_JSON, "r");
    if (fp == NULL)
    {
        perror("Cannot open JSON file");
    }
    else
    {
        printf("Found JSON file\n");
        size_t bytes_read = fread(json_buffer, 1, sizeof(json_buffer), fp);
        fclose(fp);

        cJSON *json = cJSON_Parse(json_buffer);
        if (json == NULL)
        {
            printf("Error Reading JSON\n");
        }
        else
        {
            cJSON *notif_list = cJSON_GetObjectItem(json, "notif_list");
            int notif_list_size = cJSON_GetArraySize(notif_list);
            printf("Notification List Size: %d\n", notif_list_size);
            for (int i = 0; i<notif_list_size; i++)
            {
                cJSON *elem = cJSON_GetArrayItem(notif_list, i);
                int value = cJSON_GetNumberValue(cJSON_GetObjectItem(elem, "var-num"));
                printf("Setting notification for var:%d\n", value);
                notifMsg->notif.dataPointSubscribed[value - 1] = 1;
            }

            // cJSON_Delete(notif_list);
            cJSON_Delete(json);
        }
    }
}

int check_json_file_size(char* file_name)
{
    int ret = 0;
    struct stat file_stat;
    stat(file_name, &file_stat);
    ret = file_stat.st_size;
    return ret;
}

void print_var_value(zMessage *recvBuff, int var_num)
{
    switch (var_num)
    {
        case 1:
            printf("Var 1: %f\n", recvBuff->data.var1);
            break;

        case 2:
            printf("Var 2: %f\n", recvBuff->data.var2);
            break;

        case 3:
            printf("Var 3: %f\n", recvBuff->data.var3);
            break;

        case 4:
            printf("Var 4: %f\n", recvBuff->data.var4);
            break;

        case 5:
            printf("Var 5: %f\n", recvBuff->data.var5);
            break;

        case 6:
            printf("Var 6: %f\n", recvBuff->data.var6);
            break;

        case 7:
            printf("Var 7: %f\n", recvBuff->data.var7);
            break;

        case 8:
            printf("Var 8: %f\n", recvBuff->data.var8);
            break;

        case 9:
            printf("Var 9: %f\n", recvBuff->data.var9);
            break;

        case 10:
            printf("Var 10: %f\n", recvBuff->data.var10);
            break;

    }
}