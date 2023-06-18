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
    release_folder_path = sh_path + '/release'
    if os.path.exists(release_folder_path):
        os.system(f'rm -rf {release_folder_path}')
        print("Release folder deleted.")

    # build demo.bin
    cmd_make = ""
    cmd_pack = "mkdir release && "
    if gArgs.platform == "pi":
        cmd_make += "make RPI=1 SSD1306=1"
        cmd_pack += "cp package/kvmd-web/binary/pi/* release/ && cp package/ustreamer/binary/pi/* release/ && "
    elif gArgs.platform == "h616":
        cmd_make += "make H616=1 ST7789=1"
        cmd_pack += "cp package/kvmd-web/binary/h616/* release/ && cp package/ustreamer/binary/h616/* release/ && "
    else:
        print("input error platform")
        return
    print("command: ",cmd_make, " start to make")
    output = subprocess.check_output(cmd_make, shell = True, cwd=make_path )
    print("make success")

    cmd_pack += "cp package/kvmd-hid/* release/ && cp package/kvmd-main/* release/ && \
        find package/kvmd-web/ -maxdepth 1 -type f -exec cp {} release/ \; && \
        find package/ustreamer/ -maxdepth 1 -type f -exec cp {} release/ \; && \
        cp src/kvmd-main release/ && cp -R package/kvmd-msd/* release/ && \
        cp script/install_release.py release/ && \
        cp src/config/package.json release/ && tar -zcvf release.tar.gz release && rm -rf release"
    
    print("pack relase package, command: ", cmd_pack)
    output = subprocess.check_output(cmd_pack, shell = True, cwd=sh_path )
    print("pack success")

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
    
    if gArgs.platform == "h616":
        subprocess.check_output("mv release.tar.gz release-h616-v4.tar.gz", shell = True, cwd=sh_path )

if __name__ == '__main__':
    main()