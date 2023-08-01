#include "client_utils.h"

float generate_random_float()
{
    srand(time(NULL));
    float ret = ((float)rand()/(float)RAND_MAX) * RANDOM_VALUE_LIMIT;
    return ret;
}

void generate_values(zMessage *sendBuff ,int n)
{
    switch(n)
    {
        case 1:
            sendBuff->data.var1 = generate_random_float();
            break;
        
        case 2:
            sendBuff->data.var1 = generate_random_float();
            sendBuff->data.var2 = generate_random_float();
            break;
        
        case 3: 
            sendBuff->data.var1 = generate_random_float();
            sendBuff->data.var2 = generate_random_float();
            sendBuff->data.var3 = generate_random_float();
            break;

        case 4: 
            sendBuff->data.var1 = generate_random_float();
            sendBuff->data.var2 = generate_random_float();
            sendBuff->data.var3 = generate_random_float();
            sendBuff->data.var4 = generate_random_float();
            break;
        
        case 5: 
            sendBuff->data.var1 = generate_random_float();
            sendBuff->data.var2 = generate_random_float();
            sendBuff->data.var3 = generate_random_float();
            sendBuff->data.var4 = generate_random_float();
            sendBuff->data.var5 = generate_random_float();
            break;
        
        case 6: 
            sendBuff->data.var1 = generate_random_float();
            sendBuff->data.var2 = generate_random_float();
            sendBuff->data.var3 = generate_random_float();
            sendBuff->data.var4 = generate_random_float();
            sendBuff->data.var5 = generate_random_float();
            sendBuff->data.var6 = generate_random_float();
            break;
        
        case 7: 
            sendBuff->data.var1 = generate_random_float();
            sendBuff->data.var2 = generate_random_float();
            sendBuff->data.var3 = generate_random_float();
            sendBuff->data.var4 = generate_random_float();
            sendBuff->data.var5 = generate_random_float();
            sendBuff->data.var6 = generate_random_float();
            sendBuff->data.var7 = generate_random_float();
            break;
        
        case 8: 
            sendBuff->data.var1 = generate_random_float();
            sendBuff->data.var2 = generate_random_float();
            sendBuff->data.var3 = generate_random_float();
            sendBuff->data.var4 = generate_random_float();
            sendBuff->data.var5 = generate_random_float();
            sendBuff->data.var6 = generate_random_float();
            sendBuff->data.var7 = generate_random_float();
            sendBuff->data.var8 = generate_random_float();
            break;
        
        case 9: 
            sendBuff->data.var1 = generate_random_float();
            sendBuff->data.var2 = generate_random_float();
            sendBuff->data.var3 = generate_random_float();
            sendBuff->data.var4 = generate_random_float();
            sendBuff->data.var5 = generate_random_float();
            sendBuff->data.var6 = generate_random_float();
            sendBuff->data.var7 = generate_random_float();
            sendBuff->data.var8 = generate_random_float();
            sendBuff->data.var9 = generate_random_float();
            break;
        
        case 10: 
            sendBuff->data.var1 = generate_random_float();
            sendBuff->data.var2 = generate_random_float();
            sendBuff->data.var3 = generate_random_float();
            sendBuff->data.var4 = generate_random_float();
            sendBuff->data.var5 = generate_random_float();
            sendBuff->data.var6 = generate_random_float();
            sendBuff->data.var7 = generate_random_float();
            sendBuff->data.var8 = generate_random_float();
            sendBuff->data.var9 = generate_random_float();
            sendBuff->data.var10 = generate_random_float();
            break;

        default:
            break;
    }   
}

void print_all_vars(zMessage *sendBuf)
{
    printf("Var 1: %f\n", sendBuf->data.var1);
    printf("Var 2: %f\n", sendBuf->data.var2);
    printf("Var 3: %f\n", sendBuf->data.var3);
    printf("Var 4: %f\n", sendBuf->data.var4);
    printf("Var 5: %f\n", sendBuf->data.var5);
    printf("Var 6: %f\n", sendBuf->data.var6);
    printf("Var 7: %f\n", sendBuf->data.var7);
    printf("Var 8: %f\n", sendBuf->data.var8);
    printf("Var 9: %f\n", sendBuf->data.var9);
    printf("Var 10: %f\n", sendBuf->data.var10);
}

int read_prod_no(char* config_json)
{
    FILE *fp;
    int ret;
    char json_buffer[1024];
    size_t bytes_read;

    fp = fopen(config_json, "r");
    if (fp == NULL)
    {
        perror("Cannot open JSON file");
        ret = 1;
    }
    else
    {
        bytes_read = fread(json_buffer, 1, sizeof(json_buffer), fp);
        fclose(fp);

        cJSON *json = cJSON_Parse(json_buffer);
        if (json == NULL)
        {
            printf("Error parsing JSON\n");
            ret = 1;
        }
        else
        {
            ret = cJSON_GetNumberValue(cJSON_GetObjectItem(json, "var_prod"));
        }

        cJSON_Delete(json);
    }

    return ret;
}