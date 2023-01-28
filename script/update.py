# -*- coding:utf8 -*-
# eg: python3 update.py 
import json
import os
import sys
import subprocess

download_path = "/tmp/kvm_update/"
update_result = False
def main():
    global update_result
    sh_path = os.path.split(os.path.realpath(__file__))[0]
    make_path = sh_path + '/src'

    # clear download_path file
    cmd = "rm -rf /tmp/kvm_update"
    output = subprocess.check_output(cmd, shell = True, cwd=sh_path )

    cmd = "mkdir /tmp/kvm_update"
    output = subprocess.check_output(cmd, shell = True, cwd=sh_path )

    #start update
    file = open('/tmp/kvm_update/update_status.json','w')
    file.write('{\"update_status\": 0}')
    file.close()
    a=1
    while(a>0):
        # get tag 
        # tag=$(curl -s https://api.github.com/repos/ThomasVon2021/blikvm/releases/latest | grep tag_name | cut -f4 -d "\"")
        cmd = "curl -s https://api.github.com/repos/ThomasVon2021/blikvm/releases/latest | grep tag_name | cut -f4 -d \"\\\"\""
        try:
            output = subprocess.check_output(cmd, shell = True, cwd=sh_path )
            tag = output.decode('utf-8')
        except subprocess.CalledProcessError as e:
            print("tag failed")
            break
        # download packjson first
        cmd = "curl -LJo package.json https://github.com/ThomasVon2021/blikvm/releases/download/" + tag[0:-1] + "/package.json"
        try:
            output = subprocess.check_output(cmd, shell = True, cwd=download_path)
        except subprocess.CalledProcessError as e:
            print("packagejson failed")
            break

        # compare download packjson version and run packjson version
        download_json = download_path + "package.json"
        run_json = '/usr/bin/kvm_json/package.json'
        last_version = ''
        run_version = ''
        with open(download_json,'r',encoding='utf8')as fp_r:
            json_data = json.load(fp_r)
            last_version = json_data['version_int']
        with open(run_json,'r',encoding='utf8')as fp_r:
            json_data = json.load(fp_r)
            run_version = json_data['version_int']
        if last_version != run_version :
            print("Fetched update version")
            # download tar pack
            cmd = "curl -LJo release.tar.gz https://github.com/ThomasVon2021/blikvm/releases/download/" + tag[0:-1] + "/release.tar.gz"
            try:
                output = subprocess.check_output(cmd, shell = True, cwd=download_path)
            except subprocess.CalledProcessError as e:
                print("Download release package failed, check network")
                break
            release_tar = download_path + "release.tar.gz"
            if os.path.exists(release_tar):   
                cmd = "tar -zxvf release.tar.gz"
                output = subprocess.check_output(cmd, shell = True, cwd=download_path)
                cmd = "python3 install_release.py"
                output = subprocess.check_output(cmd, shell = True, cwd=sh_path)        
                update_result = True
                print("Upgrade success")
        else:
            print("Don't need update")
        a = 0
    result_cnt = ""
    if update_result == True:
        result_cnt = "{\"update_status\": 1}"
    else:
        result_cnt = "{\"update_status\": 2}"

    file = open('/tmp/kvm_update/update_status.json','w')
    file.write(result_cnt)
    file.close()


if __name__ == '__main__':
    main()
