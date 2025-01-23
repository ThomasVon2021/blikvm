#!/usr/bin/python3
# -*- coding:utf8 -*-
# eg: python3 update.py v1.4.2
import json
import os
import sys
import subprocess
import requests
import platform
from enum import Enum
import re

download_path = "/tmp/kvm_update/"
update_result = False

# Define board type string
pi4b_board = "Raspberry Pi 4 Model B"
cm4b_board = "Raspberry Pi Compute Module 4"
h616_board = "MangoPi Mcore"

code_owner = "ThomasVon2021"
code_repo = "blikvm"
file_name = ""

class BoardType(Enum):
    UNKNOWN = 0
    V1_CM4 = 1
    V2_HAT = 2
    V3_PCIE = 3
    V4_H616 = 4
    
# Execute command and get output
def execmd(cmd):
    result = ""
    try:
        output = subprocess.check_output(cmd, shell=True, stderr=subprocess.STDOUT)
        result = output.decode("utf-8")
    except subprocess.CalledProcessError as e:
        print("Error: ", e.returncode, e.output)
    return result

# Get board type by checking system information
def get_board_type():
    # Check if the board is Raspberry Pi 4 Model B
    if pi4b_board in execmd("cat /proc/device-tree/model"):
        type = BoardType.V2_HAT
    # Check if the board is Raspberry Pi Compute Module 4
    elif cm4b_board in execmd("cat /proc/device-tree/model"):
        type = BoardType.V3_PCIE
    # Check if the board is Mango Pi Mcore
    elif h616_board in execmd("cat /proc/device-tree/model"):
        type = BoardType.V4_H616
    else:
        type = BoardType.UNKNOWN
    return type

def download_release_file(owner, repo, tag_name, file_name, download_path):
    """
    Download a specific file from the latest release of a GitHub repository and display progress.

    Args:
        owner (str): The owner of the GitHub repository.
        repo (str): The name of the GitHub repository.
        tag_name (str): The tag name of the release to download from.
        file_name (str): The name of the file to download.
        download_path (str): The path to download the file to.

    Returns:
        bool: True if the file was downloaded successfully, False otherwise.
    """
    # Get the latest release
    releases_url = f'https://api.github.com/repos/{owner}/{repo}/releases/tags/{tag_name}'
    response = requests.get(releases_url)
    if response.status_code != 200:
        print(f'Error getting release information: {response.content}')
        return False
    release_data = response.json()

    # Find the file in the release assets
    asset = next((a for a in release_data['assets'] if a['name'] == file_name), None)
    if asset is None:
        print(f'Could not find asset with name {file_name}')
        return False

    # Download the file with progress
    file_url = asset['browser_download_url']
    response = requests.get(file_url, stream=True)
    if response.status_code != 200:
        print(f'Error downloading file: {response.content}')
        return False

    # Save the file to the specified download path and print progress percentage
    file_path = os.path.join(download_path, file_name)
    total_size = int(response.headers.get('content-length', 0))
    downloaded_size = 0
    block_size = 1024  # 1 Kilobyte
    previous_progress = 0 
    with open(file_path, 'wb') as f:
        for data in response.iter_content(block_size):
            downloaded_size += len(data)
            f.write(data)
            progress_percentage = (downloaded_size / total_size) * 100
            now_progress = int(progress_percentage)
            pre_progress = int(previous_progress)
            if now_progress != pre_progress:
                print(f"Download progress: {progress_percentage:.2f}%", flush=True)
                previous_progress = progress_percentage

    if total_size != 0 and downloaded_size != total_size:
        print(f'Error downloading file: downloaded {downloaded_size} out of {total_size} bytes')
        return False
    print(f'{file_name} downloaded to {file_path} successfully.', flush=True)
    return True

def version_to_tuple(version):
    version_numbers = re.findall(r'\d+', version)
    return tuple(map(int, version_numbers))


def main():
    print("Welcome to use the upgrade script. Please confirm that you have used git related commands before upgrading the script to ensure that update.by is in the latest state.")
    board_type = get_board_type()
    print("Board type:",board_type)
    global update_result
    sh_path = os.path.split(os.path.realpath(__file__))[0]

    specified_version = None
    # Check if a version has been passed as an argument
    if len(sys.argv) > 1:
        specified_version = sys.argv[1] 

    # Remove/clear download directory
    cmd = "rm -rf /tmp/kvm_update"
    output = subprocess.check_output(cmd, shell = True, cwd=sh_path )

    # Create the download path
    cmd = "mkdir /tmp/kvm_update"
    output = subprocess.check_output(cmd, shell = True, cwd=sh_path )

    #start update
    file = open('/tmp/kvm_update/update_status.json','w')
    file.write('{\"update_status\": 0}')
    file.close()
    a=1
    while(a>0):
        latest_version = ''
        run_version = ''
        if specified_version:
            # Use the specified version
            latest_version = specified_version
            print(f"Specified version: {latest_version}")
        else:
            # Get the latest tag from GitHub if no version is specified
            url = "https://api.github.com/repos/{owner}/{repo}/releases/latest"
            response = requests.get(url.format(owner=code_owner, repo=code_repo))
            json_response = response.json()
            if "tag_name" in json_response:
                latest_version = json_response["tag_name"]
                print("The latest release tag for blikvm is ", latest_version)
            else:
                print("Cannot find the latest release tag")
                return

        # get local tag
        run_json = '/usr/bin/blikvm/package.json'
        if not os.path.exists(run_json):
            run_version="v1.0.0"
            print("get local version failed ",run_json," is not exit")
        else:
            with open(run_json,'r',encoding='utf8')as fp_r:
                json_data = json.load(fp_r)
                run_version = json_data['version']
                print("The local version is ",run_version)
       
        # compare version
        if latest_version != run_version:
            print("Upgrading ", run_version , " ==> ", latest_version)
            # download tar pack
            cmd = ""
            if board_type == BoardType.V1_CM4 or board_type == BoardType.V2_HAT or board_type == BoardType.V3_PCIE:
                # cmd = "curl -kLJo release.tar.gz https://github.com/ThomasVon2021/blikvm/releases/download/" + tag[0:-1] + "/release.tar.gz"
                file_name = "blikvm-v1-v2-v3.deb"
            elif board_type == BoardType.V4_H616:
                # cmd = "curl -kLJo release.tar.gz https://github.com/ThomasVon2021/blikvm/releases/download/" + tag[0:-1] + "/release-h616-v4.tar.gz"
                file_name = "blikvm-v4.deb"
            else:
                print("get unknow board")
            try:
                print("Download package: ", file_name, " please wait...")
                download_release_file(code_owner,code_repo,latest_version, file_name, download_path)
            except subprocess.CalledProcessError as e:
                print("Download release package failed, check network")
                break
            print("Download release package success, start to install, please wait 60s...",  flush=True)
            release_tar = download_path + file_name
            if os.path.exists(release_tar):   
                cmd = "dpkg -i " + file_name
                output = subprocess.check_output(cmd, shell = True, cwd=download_path)
                print(output)       
                update_result = True
                print("If any abnormalities are found when upgrading from the old version to the latest version, the system can be restored by flashing it again.")
                print("If you cannot log in with your original password, it may be due to a version upgrade and reset configuration to default. If you have changed the web or SSH password, you will need to update the configuration again. Config path is /mnt/exec/release/config/app.json",  flush=True)
                print("Upgrade successful!",  flush=True)
        else:
            print("There is no latest stable version available.")
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
