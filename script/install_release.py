#!/usr/bin/python3
# eg: python3 install_release.py --releasepath=./
# after run this script, need reboot.
import subprocess
import os
import re
import argparse
import json
import copy

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
        # print("error code:",code, out_bytes)

def update_config(local_file, new_file):
    """Update the local configuration file by merging the new config file into it.
    
    Args:
    local_file (str): Path to the local configuration file.
    new_file (str): Path to the new configuration file.
    """
    def merge_dicts(local_dict, new_dict):
        """Recursively merge two dictionaries.
        
        This function will update the local_dict with values from new_dict.
        If a value in new_dict is a dictionary and exists in local_dict, 
        it will recursively merge the nested dictionaries. Otherwise, 
        it will overwrite or add the value from new_dict.
        """
        for key, value in new_dict.items():
            if isinstance(value, dict) and key in local_dict:
                # If the value is a dictionary and the key exists in local_dict, recursively merge
                merge_dicts(local_dict[key], value)
            else:
                # Otherwise, add or overwrite the value from new_dict
                local_dict[key] = value

    # Check if the local file exists
    if os.path.exists(local_file):
        # Read the local configuration file
        with open(local_file, 'r', encoding='utf-8') as f:
            local_config = json.load(f)
    else:
        local_config = {}

    # Read the new configuration file
    with open(new_file, 'r', encoding='utf-8') as f:
        new_config = json.load(f)

    # Merge the local and new configurations
    merge_dicts(local_config, new_config)

    # Save the merged configuration back to the local file
    with open(local_file, 'w', encoding='utf-8') as f:
        json.dump(local_config, f, indent=4, ensure_ascii=False)

    print(f"Successfully updated {local_file}.")

def merge_and_save_config(existing_config_path, new_config_path):
    if not os.path.exists(existing_config_path):
        print(f"No historical configuration files exist, no need to merge")
        return
    """
    读取两个配置文件，递归合并后将合并结果保存到新配置文件路径。
    
    :param existing_config_path: 现有配置文件路径
    :param new_config_path: 新配置文件路径
    """
    def merge_config(existing_config, new_config):
        """
        递归合并两个配置字典，保留现有配置中的键值，新配置中同样的键不会更新现有键。
        """
        merged = copy.deepcopy(existing_config)
        
        for key, value in new_config.items():
            if key not in merged:
                merged[key] = value
            elif isinstance(value, dict) and isinstance(merged[key], dict):
                merged[key] = merge_config(merged[key], value)
        
        return merged

    # 读取配置文件
    with open(existing_config_path, 'r') as file:
        existing_config = json.load(file)
    
    with open(new_config_path, 'r') as file:
        new_config = json.load(file)

    # 合并配置
    merged_config = merge_config(existing_config, new_config)

    # 保存合并后的配置
    with open(new_config_path, 'w') as file:
        json.dump(merged_config, file, indent=4)

    print(f"The configuration file has been successfully merged and saved to {new_config_path}")

def compare_versions(version1, version2):
    # Remove 'v' if it exists
    if version1.startswith('v'):
        version1 = version1[1:]
    if version2.startswith('v'):
        version2 = version2[1:]
    
    v1 = list(map(int, version1.split('.')))
    v2 = list(map(int, version2.split('.')))
    return v1 >= v2

def disable_service_if_exists(services):
    """
    Disables the given systemd services if they exist.

    :param services: A list of service names to check and disable.
    """
    for service in services:
        try:
            # Check if the service exists
            result = subprocess.run(
                ['systemctl', 'status', service],
                stdout=subprocess.DEVNULL,
                stderr=subprocess.DEVNULL
            )
            # If the service exists (exit code 0), disable it
            if result.returncode == 0:
                subprocess.run(['systemctl', 'disable', service], check=True)
                print(f"Service '{service}' disabled successfully.")
            else:
                print(f"Service '{service}' does not exist or is inactive, skipping.")
        except subprocess.CalledProcessError:
            # Handle case where the service does not exist or can't be disabled
            print(f"Service '{service}' could not be disabled (likely does not exist).")
        except Exception as e:
            # Catch any unexpected errors
            print(f"Unexpected error with service '{service}': {e}")

def create_directory_if_not_exists(path):
    """
    Checks if the directory exists, and if not, creates it.

    :param path: The directory path to check and create.
    """
    if not os.path.exists(path):
        try:
            os.makedirs(path)
            print(f"Directory '{path}' created successfully.")
        except OSError as e:
            print(f"Error creating directory '{path}': {e}")
    else:
        print(f"Directory '{path}' already exists.")

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
    cmd = "ps -aux | grep server_app | grep -v grep"
    execute_cmd(cmd,sh_path)
    print('kill web done')

    # kill kvmd-ustreamer
    cmd = "ps -aux | grep ustreamer | grep -v grep"
    execute_cmd(cmd,sh_path)
    print('kill ustreamer done')

    cmd = "ps -aux | grep janus | grep -v grep"
    execute_cmd(cmd,sh_path)
    print('kill janus done')

    services_to_disable = [
        'kvmd-janus',
        'kvmd-hid',
        'kvmd-main',
        'kvmd-video'
    ]
    disable_service_if_exists(services_to_disable)

    # install all software
    if os.path.exists(gArgs.releasepath):
        source_dir = '/mnt/exec/release/config'
        if os.path.exists(source_dir) and os.path.isdir(source_dir):
            cmd = "cp -R /mnt/exec/release/config /tmp"
            subprocess.check_output(cmd, shell = True, cwd=gArgs.releasepath )
        source_package = '/usr/bin/blikvm/package.json'
        if os.path.exists(source_package) :
            cmd = "cp /usr/bin/blikvm/package.json /tmp/config"
            subprocess.check_output(cmd, shell = True, cwd=gArgs.releasepath )
        create_directory_if_not_exists('/usr/bin/blikvm/')
        cmd = "bash install-kvmd-web.sh && cp package.json /usr/bin/blikvm/package.json"
        subprocess.check_output(cmd, shell = True, cwd=gArgs.releasepath )
        print('install alpha version successful, start to resatrt service, need 60s...')
        cmd = "systemctl daemon-reload && systemctl restart kvmd-web"
        subprocess.check_output(cmd, shell = True, cwd=gArgs.releasepath )
        print('restart kvmd-web successful')
        merge_and_save_config('/tmp/config/app.json', '/mnt/exec/release/config/app.json')


    else:
        print(gArgs.releasepath, ' not exit')

    
if __name__ == '__main__':
    main()
