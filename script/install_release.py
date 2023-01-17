# eg: python3 install_release.py 
# after run this script, need reboot.
import subprocess
import os
import re
import argparse

gArgs = None
def doArgParse():
    global gArgs
    optParser = argparse.ArgumentParser(description='build and tar file')
    optParser.add_argument('--releasepath', nargs='?', default='/tmp/release/', type=str, help='release path')
    gArgs = optParser.parse_args()

def execute_cmd(cmd,sh_path):
    try: 
        output = subprocess.check_output(cmd, shell = True, cwd=sh_path )
        if len(output) > 5 :
            tmp = ' '.join(output.decode('utf-8').split())
            pid = tmp.split(' ')[1]
            cmd = "kill " + pid
            output = subprocess.check_output(cmd, shell = True, cwd=sh_path )
    except subprocess.CalledProcessError as e:
        out_bytes = e.output.decode()
        code = e.returncode
        print("error code:",code, out_bytes)

def main():
    global gArgs
    doArgParse()
    sh_path = os.path.split(os.path.realpath(__file__))[0]
    # kill run pthread
    # kill blikvm-oled
    cmd = "ps -aux | grep blikvm-oled | grep -v grep"
    execute_cmd(cmd,sh_path)
    print('kill oled done')
    
    # kill kvmd-main
    cmd = "ps -aux | grep kvmd-main | grep -v grep"
    execute_cmd(cmd,sh_path)
    print('kill main done')

    # kill kvmd-web
    cmd = "ps -aux | grep kvmd-web | grep -v grep"
    execute_cmd(cmd,sh_path)
    print('kill web done')

    # kill kvmd-ustreamer
    cmd = "ps -aux | grep ustreamer | grep -v grep"
    execute_cmd(cmd,sh_path)
    print('kill ustreamer done')

    # install all software
    if os.path.exists(gArgs.releasepath):
        cmd = "bash install-kvmd-main.sh && bash install-ustreamer.sh && bash install-kvmd-oled.sh && bash install-kvmd-hid.sh \
        && bash install-kvmd-web.sh"
        subprocess.check_output(cmd, shell = True, cwd=gArgs.releasepath )
        cmd = "cp package.json /usr/bin/package.json"
        subprocess.check_output(cmd, shell = True, cwd=gArgs.releasepath )
        # cmd = "reboot"
        # subprocess.check_output(cmd, shell = True, cwd=gArgs.releasepath )
    else:
        print(gArgs.releasepath, ' not exit')

if __name__ == '__main__':
    main()
