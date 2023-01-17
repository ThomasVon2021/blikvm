# -*- coding:utf8 -*-
# eg: python3 update.py 
import json
import os
import sys
import subprocess

download_path = "/tmp/"
update_result = False
def main():
    global update_result
    sh_path = os.path.split(os.path.realpath(__file__))[0]
    make_path = sh_path + '/src'

    # clear download_path file
    cmd = "rm -rf /tmp/*"
    output = subprocess.check_output(cmd, shell = True, cwd=sh_path )

    # get tag 
    # tag=$(curl -s https://api.github.com/repos/ThomasVon2021/blikvm/releases/latest | grep tag_name | cut -f4 -d "\"")
    cmd = "curl -s https://api.github.com/repos/ThomasVon2021/blikvm/releases/latest | grep tag_name | cut -f4 -d \"\\\"\""
    output = subprocess.check_output(cmd, shell = True, cwd=sh_path )
    tag = output.decode('utf-8')

    # download packjson first
    cmd = "curl -LJo package.json https://github.com/ThomasVon2021/blikvm/releases/download/" + tag[0:-1] + "/package.json"
    output = subprocess.check_output(cmd, shell = True, cwd=download_path)

    # compare download packjson version and run packjson version
    download_json = download_path + "package.json"
    run_json = '/usr/bin/package.json'
    last_version = ''
    run_version = ''
    with open(download_json,'r',encoding='utf8')as fp_r:
        json_data = json.load(fp_r)
        last_version = json_data['version']
    with open(run_json,'r',encoding='utf8')as fp_r:
        json_data = json.load(fp_r)
        run_version = json_data['version']
    if last_version != run_version :
        print("should update")
        # download tar pack
        cmd = "curl -LJo release.tar.gz https://github.com/ThomasVon2021/blikvm/releases/download/" + tag[0:-1] + "/release.tar.gz"
        output = subprocess.check_output(cmd, shell = True, cwd=download_path)
        release_tar = download_path + "release.tar.gz"
        if os.path.exists(release_tar):   
            cmd = "tar -zxvf release.tar.gz"
            output = subprocess.check_output(cmd, shell = True, cwd=download_path)
            cmd = "python3 install_release.py"
            output = subprocess.check_output(cmd, shell = True, cwd=sh_path)        
            update_result = True
    else:
        print("not update")
    result_cnt = ""
    if update_result == True:
        result_cnt = "True"
    else:
        result_cnt = "False"

    file = open('/tmp/update_result.txt','w')
    file.write(result_cnt)
    file.close()


if __name__ == '__main__':
    main()
