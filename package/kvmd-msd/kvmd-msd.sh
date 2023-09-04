#!/bin/bash

WHOAMI=`whoami`
if [[ "$WHOAMI" != "root" ]]; then
    echo "$WHOAMI, this script must be run as root."
    exit 1
fi

usage() {
    set +x
    echo "usage:  $0 -c [make|conn|disconn|clean] -f filename.iso -s integer_number -n MSD_NAME"
    echo "
without -f filename.iso   defaults to all /mnt/msd/user/*.iso files
without -s 5              defaults to 5 (GB) as the filesize (example: 1 2 3 4 5 6 7 8 9 10)
without -n name           defaults name=ventoy -> ventoy.img
"
    exit 1
}

#CMD=$1

iso_file_name=()
count_value=0
ventoy_dir="/mnt/msd/ventoy"
iso_dir="/mnt/msd/user"
mount_dist_dir="/media/blikvm/ventoy/"
usb_gadget_sh="/usr/bin/blikvm/enable-gadget.sh"
usb_dis_gadget_sh="/usr/bin/blikvm/disable-gadget.sh"
msd_config_dir="/mnt/msd/config/"
msd_shm_dir="/dev/shm/blikvm/"
msd_json="msd.json"

while getopts "c:f:s:n:h" opt; do
  case $opt in
    f)
                FILE="$OPTARG"
                ;;
    s)
                VENTORY_SIZE="$OPTARG"
                ;;
    n)
                MSD_NAME="$OPTARG"
                ;;
    c)
                CMD="$OPTARG"
                ;;
    h|*)
                usage
                ;;
  esac
done

set -e
set -x
set -u

CMD=${CMD:-"unset"}
FILE=${FILE:-"*"}
VENTORY_SIZE=${VENTORY_SIZE:-5}
MSD_NAME=${MSD_NAME:-"ventoy"}


update_json()
{
        sudo jq --arg key $1 --arg value $2 '.[$key] = $value' $msd_config_dir$msd_json > /mnt/msd/config/temp.json
        sudo mv /mnt/msd/config/temp.json $msd_config_dir$msd_json
        sudo cp $msd_config_dir$msd_json $msd_shm_dir$msd_json
}


traverse_dir()
{
        filepath=$1

        for file in `ls -a $filepath`
        do
                if [ -d ${filepath}/$file ]
                then
                        if [[ $file != '.' && $file != '..' ]]
                        then
                                traverse_dir ${filepath}/$file
                        fi
                else
                        check_suffix ${filepath}/$file
                fi
        done
}


##get iso file
check_suffix()
{
        file=$1

        # if [ "${file##*.}"x = "iso"x ] ;then
        #echo $(basename $file)
        echo "storage: $file"
        iso_file_name[${#iso_file_name[*]}]=$file
        # fi
}

if [ ! -d $msd_config_dir ]
then
        mkdir -p $msd_config_dir
fi
if [ ! -f  $msd_config_dir$msd_json ]
then
        sudo touch $msd_config_dir$msd_json
        sudo echo '{"msd_status": "not_connected","msd_img_created": "not_created"}' | jq '.' > $msd_config_dir$msd_json

fi

case ${CMD} in
    make)
#       if [ $# -gt 2 ]
#       then
#               echo "param need point one, or default 0 use iso all img."
#       fi

        mount -o remount,rw /

        if [ -f  "$ventoy_dir/"$MSD_NAME".img" ]
        then
                echo "update file exist"
                cd  $ventoy_dir
        else
                if [ ! -d  $ventoy_dir ]
                then
                        mkdir -p  $ventoy_dir
                fi
                cd  $ventoy_dir
                echo "update file not exist,please wait..."

                sudo dd if=/dev/zero of=$MSD_NAME".img" bs=1M count=$((VENTORY_SIZE*1024)) status=progress;
        fi

        losetup -f $MSD_NAME".img"

        losetup -l | grep $MSD_NAME

        dev_name=`losetup -l | grep ventoy | awk 'NR==1{print $1}'`

        echo $dev_name

        sh /usr/bin/blikvm/ventoy-1.0.88/Ventoy2Disk.sh -i $dev_name;

        mkdir -p $mount_dist_dir

        mount  $dev_name"p1" $mount_dist_dir;

        if [ $? -ne 0 ]
        then
                echo "mount  $dev_name"p1" fail"
        fi

        #sudo cp -r "/usr/bin/blikvm/ventoy-1.0.88/plugin/ventoy" "$mount_dist_dir"

        #if [ $? -eq 0 ]
        #then
        #       echo "cp plugin ventoy sucess!!!"
        #fi

        #echo $PWD

        echo "yueliang:"${FILE}
        if [ "${FILE}" != "*" ];then
                file_name=${FILE}
                echo "param correct"
                echo $mount_dist_dir${file_name##*/}
                if [[ -f $mount_dist_dir${file_name##*/} && -f "$iso_dir/${file_name}" ]]
                then
                        echo "exist update file: ${file_name},rm it "

                        rm -f $mount_dist_dir${file_name##*/}
                        sync
                fi
                sleep 3
                echo "$iso_dir/${file_name}"
                cp "$iso_dir/${file_name}" "$mount_dist_dir"
                if [ $? -ne 0 ]
                then
                        echo "cp failed"
                else
                        echo "cp $iso_dir/${file_name} sucess!"
                fi
                sync
        else
                if [ ! -d  $iso_dir ]
                then
                        mkdir -p  $iso_dir
                        exit 1
                fi
                traverse_dir $iso_dir
                for name in ${iso_file_name[@]}
                do
                        #echo ${name##*/}
                        echo $mount_dist_dir${name##*/}
                        if [ -f $mount_dist_dir${name##*/} ]
                        then
                                echo "exist update file: ${name},rm it "
                                rm -f $mount_dist_dir${name##*/}
                                sync
                        fi
                        sleep 3
                        echo "${name} again!"
                        cp "${name}" "$mount_dist_dir";
                        if [ $? -ne 0 ]
                        then
                                echo "default cp failed"
                        else
                                echo "default cp ${name} sucess!"
                        fi
                        sync
                done
        fi
        sudo umount -f $dev_name"p1"
        sleep 3
        sudo losetup -d $dev_name

        update_json  msd_img_created created
        sudo mount -o remount,ro /
        ;;

    conn)
        cd  $ventoy_dir
        if [ -f  $ventoy_dir/$MSD_NAME".img_bak" ]
        then
                mv $MSD_NAME".img_bak" $MSD_NAME".img"
        fi
        bash $usb_dis_gadget_sh
        bash $usb_gadget_sh
        update_json msd_status connected
        ;;

    disconn)
        cd  $ventoy_dir
        if [ -f  $ventoy_dir/$MSD_NAME".img" ]
        then
                mv $MSD_NAME".img" $MSD_NAME".img_bak"
        fi
        bash $usb_dis_gadget_sh
        bash $usb_gadget_sh
        update_json msd_status not_connected
        ;;

    clean)
        if [[ -f  $ventoy_dir/$MSD_NAME".img" || -f  $ventoy_dir/$MSD_NAME".img_bak" ]]
        then
                rm -f $ventoy_dir/*
        fi
        bash $usb_dis_gadget_sh
        bash $usb_gadget_sh
        update_json  msd_img_created not_created
        update_json msd_status not_connected
        ;;

    status)
        set +x
        echo
        echo "ISO images in /mnt/msd/user"; ls -l /mnt/msd/user
        echo
        echo "Ventoy images in /mnt/msd/ventoy"; ls -l /mnt/msd/ventoy
        if [ -e /mnt/msd/ventoy/*.img* ]; then
                echo "Ventoy image exists.  Please run '$0 -c conn' to use it with target"
        else
                echo "No usable image exists.  Please run '$0 -c make' to create image"
        fi
        ;;

    *)
        #echo "unset param, please use param: make,conn,disconn..."
        usage
        ;;
esac
