# apt install network-manager
# systemctl enable NetworkManager
# nmcli connection down wlan0
# nmcli device wifi list
import json
import subprocess
import time
import os

def connect_to_wifi(wifi):
    ssid = wifi['SSID']
    password = wifi['Password']
    ip = wifi['IP']

    # 连接到新的WiFi网络
    print("connecting to ", ssid, " password:",password)
    connect_wifi = "nmcli device wifi connect " + ssid + " password " + password
    print(connect_wifi)
    try:
        subprocess.check_output("mount -o remount,rw /", shell = True, cwd="/" )
        output = subprocess.check_output(connect_wifi, shell = True, cwd="/" )
        subprocess.check_output("mount -o remount,ro /", shell = True, cwd="/" )
    except subprocess.CalledProcessError as e:
        print(f"Failed to connect to WiFi: {e}")
        return False

    # 检查连接是否成功
    if not is_wifi_connected():
        print("connected ", ssid, " failed")
        return False
    print("connected ", ssid, " success")
    return True

def is_wifi_connected():
    # 检查是否有IP地址分配给无线接口
    result = subprocess.run(['ifconfig', 'wlan0'], stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
    return 'inet' in result.stdout

def ping_test(ip):
    # 模拟ping测试
    ping_command = f'ping -I wlan0 -c 10 {ip}'
    result = subprocess.run(ping_command, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
    
    # 检查ping结果，如果丢包率大于5%，返回False
    if "packet loss" in result.stdout:
        loss_rate = int(result.stdout.split("packet loss")[0].split()[-1].replace("%", ""))
        print("ping ",ip ," loss rate:", loss_rate)
        return loss_rate <= 50
    return True

def main():
    sh_path = os.path.split(os.path.realpath(__file__))[0]
    # 读取配置文件
    path = sh_path + '/wifi-config.json'
    with open(path, 'r') as config_file:
        wifi_config = json.load(config_file)
    current_wifi_index = 0
    wifi_connected = False
    while True:
        wifi = wifi_config[current_wifi_index]
        if wifi_connected or connect_to_wifi(wifi):
            wifi_connected = True
            while True:
                if ping_test(wifi['IP']):
                    time.sleep(5)
                else:
                    wifi_connected = False
                    break
        current_wifi_index = (current_wifi_index + 1) % len(wifi_config)
        time.sleep(3)
if __name__ == '__main__':
    main()
