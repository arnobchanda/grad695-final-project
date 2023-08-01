import json
import os

input_json_file_path = os.path.dirname(__file__) + "/../include/datapointlist.json"
output_h_file_path = os.path.dirname(__file__) + "/../include/data_def.h"

def read_json_file():
    f = open(input_json_file_path)
    json_object = json.load(f)
    f.close()
    
    return json_object

def convert_json_to_struct(json_data):
    struct_def = "typedef struct dataPoint {\n"
    for var_def in json_data["var_def"]:
        var_name = var_def["var-name"]
        struct_def += "    float " + var_name + ";\n"
    struct_def += "} zData;\n"
    
    return struct_def

def write_h_file(file_buffer):
    with open(output_h_file_path, "w+") as h_file:
        h_file.write(file_buffer)
        

if __name__=="__main__":
    write_h_file(convert_json_to_struct(read_json_file()))