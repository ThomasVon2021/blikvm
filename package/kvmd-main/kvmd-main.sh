#!/bin/bash
sleep 3
# 后台启动进程
/usr/bin/blikvm/kvmd-main &

# 监控进程
count=0
while true; do
    # 检查进程是否存在
    if ! pgrep -x "kvmd-main" > /dev/null; then
        ((count++))
        echo "Process not found (Count: $count)"
        
        # 如果连续三次检测到进程不存在，重新启动进程
        if [ $count -ge 3 ]; then
            echo "Restarting process..."
            /usr/bin/blikvm/kvmd-main &
            count=0
        fi
    else
        count=0
    fi
    
    # 等待5秒再次检查
    sleep 5
done
