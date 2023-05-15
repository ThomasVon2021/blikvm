#!/usr/bin/python3
# build project
# python3 build.py --platform pi
# python3 build.py --platform h616
import argparse
import os
import subprocess
import json

gArgs = None
def doArgParse():
    global gArgs
    optParser = argparse.ArgumentParser(description='build and tar file')
    optParser.add_argument('--platform', nargs='?', default='', type=str, help='x86 or arm')
    gArgs = optParser.parse_args()

def main():
    doArgParse()
    sh_path = os.path.split(os.path.realpath(__file__))[0]
    make_path = sh_path + '/src'

    # build demo.bin
    cmd = ""
    if gArgs.platform == "pi":
        cmd += "make RPI=1 SSD1306=1"
    elif gArgs.platform == "h616":
        cmd += "make H616=1 ST7789=1"
    else:
        print("input error platform")
        return
    output = subprocess.check_output(cmd, shell = True, cwd=make_path )

    # package binary  rm -rf release && rm -r release.tar.gz && 
    cmd = "mkdir release && cp package/kvmd-hid/* release/ && cp package/kvmd-main/* release/ && \
    cp package/kvmd-web/* release/ && cp package/ustreamer/* release/ && \
    cp src/kvmd-main release/ && cp -R package/kvmd-msd/* release/ && \
    cp src/config/package.json release/ && tar -zcvf release.tar.gz release && rm -rf release"
    output = subprocess.check_output(cmd, shell = True, cwd=sh_path )

    # modify package.json
    file_path = sh_path + "/src/config/package.json"
    with open(file_path,'r',encoding='utf8')as fp_r:
        json_data = json.load(fp_r)
        cmd = "md5sum release.tar.gz"
        output = subprocess.check_output(cmd, shell = True, cwd=sh_path )
        # print("md5: ",output, "str: ", output.decode('utf-8')[0:32] )
        json_data['md5value'] = output.decode('utf-8')[0:32]
        with open(file_path,'w',encoding='utf8')as fp_w:
	        json.dump(json_data,fp_w,ensure_ascii=False)

if __name__ == '__main__':
    main()