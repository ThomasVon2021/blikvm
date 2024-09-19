#!/usr/bin/python3
# eg: python3 install_release.py --releasepath=./
# after run this script, need reboot.
import subprocess
import os
import re
import argparse

gArgs = None
def doArgParse():
    global gArgs
    optParser = argparse.ArgumentParser(description='build and tar file')
    optParser.add_argument('--releasepath', nargs='?', default='/tmp/kvm_update/release/', type=str, help='release path')
    optParser.add_argument('--alpha', nargs='?', default='false', type=str, help='Whether to use version')
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
    print("start install")
    global gArgs
    doArgParse()
    sh_path = os.path.split(os.path.realpath(__file__))[0]
    # kill run pthread
    
    # kill kvmd-main
    cmd = "ps -aux | grep kvmd-main | grep -v grep"
    execute_cmd(cmd,sh_path)
    print('kill main done')

    # kill kvmd-web
    cmd = "ps -aux | grep kvmlink | grep -v grep"
    execute_cmd(cmd,sh_path)
    print('kill web done')

    # kill kvmd-ustreamer
    cmd = "ps -aux | grep ustreamer | grep -v grep"
    execute_cmd(cmd,sh_path)
    print('kill ustreamer done')

    cmd = "ps -aux | grep janus | grep -v grep"
    execute_cmd(cmd,sh_path)
    print('kill janus done')

    # install all software
    if os.path.exists(gArgs.releasepath):
        cmd = "systemctl disable kvmd-janus && systemctl disable kvmd-hid && systemctl disable kvmd-main \
        && systemctl disable kvmd-video && bash install-kvmd-web.sh && cp package.json /usr/bin/blikvm/package.json"
        subprocess.check_output(cmd, shell = True, cwd=gArgs.releasepath )
        print('install alpha version successful, start to resatrt service, need 60s...')
        cmd = "systemctl daemon-reload && systemctl restart kvmd-web"
        subprocess.check_output(cmd, shell = True, cwd=gArgs.releasepath )
        print('restart kvmd-web successful')
    else:
        print(gArgs.releasepath, ' not exit')

    
if __name__ == '__main__':
    main()
