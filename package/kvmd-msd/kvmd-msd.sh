#!/bin/bash

WHOAMI=`whoami`
if [[ "$WHOAMI" != "root" ]]; then
    echo "$WHOAMI, this script must be run as root."
    exit 1
fi

usage() {
    set +x
    echo "usage:  $0 -c [make|conn|disconn|clean|delete] -f filename.iso -s integer_number -n MSD_NAME"
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

while getopts "c:f:s:n:h:t:" opt; do
  case $opt in
    f)
                FILE="$OPTARG"
                ;;
    s)
                VENTOY_SIZE="$OPTARG"
                ;;
    n)
                MSD_NAME="$OPTARG"
                ;;
    c)
                CMD="$OPTARG"
                ;;
    t)
                TYPE="$OPTARG"
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
VENTOY_SIZE=${VENTOY_SIZE:-5}
MSD_NAME=${MSD_NAME:-"ventoy"}
TYPE=${TYPE:-"ventoy"}


update_json()
{
        jq --arg key $1 --arg value $2 '.[$key] = $value' $msd_config_dir$msd_json > $msd_config_dir/temp.json
        mv $msd_config_dir/temp.json $msd_config_dir$msd_json
        cp $msd_config_dir$msd_json $msd_shm_dir$msd_json
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
                                # traverse_dir ${filepath}/$file
                                check_suffix ${filepath}/$file
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
        touch $msd_config_dir$msd_json
        echo '{"msd_status": "not_connected","msd_img_created": "not_created"}' | jq '.' > $msd_config_dir$msd_json

fi

case ${CMD} in
    forward)
        mount -o remount,rw /

        if [ -f  "$ventoy_dir/"$MSD_NAME".img" ]
        then
                bash $usb_dis_gadget_sh
                bash $usb_gadget_sh

                cd  $ventoy_dir

                losetup -f $MSD_NAME".img"

                losetup -l | grep $MSD_NAME

                dev_name=`losetup -l | grep $MSD_NAME | grep -v delete | awk 'NR==1{print $1}'`

                echo $dev_name

                mkdir -p $mount_dist_dir

                mount  $dev_name $mount_dist_dir;

                if [ $? -ne 0 ]
                then
                        echo "mount  $dev_name"p1" fail"
                fi


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
                        if [ -f $mount_dist_dir${name##*/} ] || [ -d $mount_dist_dir${name##*/} ]
                        then
                                continue
                        fi
                        sleep 3
                        echo "${name} again!"
                        cp -rf "${name}" "$mount_dist_dir";
                        if [ $? -ne 0 ]
                        then
                                echo "default cp failed"
                        else
                                echo "default cp ${name} sucess!"
                        fi
                        sync
                done

                umount -f $dev_name
                sleep 3
                losetup -d $dev_name

                bash $usb_dis_gadget_sh
                bash $usb_gadget_sh

                mount -o remount,ro /
        else
                echo "there is no iso file, please first make iso file."
        fi
        ;;

    rever)
	mount -o remount,rw /

        if [ -f  "$ventoy_dir/"$MSD_NAME".img" ]
        then
                echo "update file exist" 
                cd  $ventoy_dir
                if [ -f  $ventoy_dir/$MSD_NAME".img" ]
                then
                        mv $MSD_NAME".img" $MSD_NAME".img_bak"
                fi
                bash $usb_dis_gadget_sh
                bash $usb_gadget_sh

                if [ -f  $ventoy_dir/$MSD_NAME".img_bak" ]
                then
                        mv $MSD_NAME".img_bak" $MSD_NAME".img"
                fi

                losetup -f $MSD_NAME".img"

                losetup -l | grep $MSD_NAME

                dev_name=`losetup -l | grep $MSD_NAME | grep -v delete | awk 'NR==1{print $1}'`

                echo $dev_name

                mkdir -p $mount_dist_dir

                mount  $dev_name $mount_dist_dir;

                if [ $? -ne 0 ]
                then
                        echo "mount  $dev_name"p1" fail"
                fi

                cp -rf  "$mount_dist_dir"*  $iso_dir

                if [ $? -ne 0 ]
                then
                        echo "default cp failed"
                else
                        echo "default cp * sucess!"
                fi

                umount $mount_dist_dir 
                sleep 3
                losetup -d $dev_name

                bash $usb_dis_gadget_sh
                bash $usb_gadget_sh

                mount -o remount,ro /
        fi
	;;

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

                dd if=/dev/zero of=$MSD_NAME".img" bs=1M count=$((VENTOY_SIZE*1024)) status=progress;
        fi

        if [ "$TYPE" != "ventoy" ] 
        then
                mkfs.fat $MSD_NAME".img"
        fi

        losetup -f $MSD_NAME".img"

        losetup -l | grep $MSD_NAME

        dev_name=`losetup -l | grep ventoy | grep -v delete | awk 'NR==1{print $1}'`

        echo $dev_name

        if [ "$TYPE" = "ventoy" ] 
        then
                sh /usr/bin/blikvm/ventoy-1.0.88/Ventoy2Disk.sh -i $dev_name;
        fi

        mkdir -p $mount_dist_dir

        if [ "$TYPE" = "ventoy" ] 
        then
                mount  $dev_name"p1" $mount_dist_dir;
        else
                mount  $dev_name $mount_dist_dir;
        fi

        if [ $? -ne 0 ]
        then
                echo "mount  $dev_name"p1" fail"
        fi

        #cp -r "/usr/bin/blikvm/ventoy-1.0.88/plugin/ventoy" "$mount_dist_dir"

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
                        if [ -f $mount_dist_dir${name##*/} ] || [ -d $mount_dist_dir${name##*/} ]
                        then
                                echo "exist update file: ${name},rm it "
                                rm -rf $mount_dist_dir${name##*/}
                                sync
                        fi
                        sleep 3
                        echo "${name} again!"
                        cp -rf "${name}" "$mount_dist_dir";
                        if [ $? -ne 0 ]
                        then
                                echo "default cp failed"
                        else
                                echo "default cp ${name} sucess!"
                        fi
                        sync
                done
        fi
        if [ "$TYPE" = "ventoy" ] 
        then
                umount -f $dev_name"p1"
        else
                umount -f $dev_name
        fi
        sleep 3
        losetup -d $dev_name

        update_json  msd_img_created created
        mount -o remount,ro /
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

    delete)
        echo "delete:"$iso_dir/${FILE}
        if [ -f  $iso_dir/${FILE} ]
        then
                echo "delete:"$iso_dir/${FILE}
                rm -f $iso_dir/${FILE}
        fi
        ;;

    status)
        set +x
        echo
        echo "ISO images in $iso_dir"; ls -l $iso_dir
        echo
        echo "Ventoy images in $ventoy_dir"; ls -l $ventoy_dir
        if [ -e $ventoy_dir/*.img* ]; then
                echo
                echo "ISO images inside ventoy image(s)"
                losetup -f $ventoy_dir/*.img*
                for dev_name in `losetup -l | grep ventoy | grep -v delete | awk 'NR==1{print $1}'`; do
                        losetup -l | grep $dev_name
                        mount $dev_name"p1" $mount_dist_dir
                        ls -l $mount_dist_dir
                        umount -f $dev_name"p1"
                        losetup -d $dev_name
                done

                echo
                if [ $( grep msd_status $msd_config_dir/msd.json | grep -c not ) -eq 1 ]; then
                        echo "Ventoy image exists and is disconnected.  Run '$0 -c conn' to connect to target"
                else
                        echo "Ventoy image exists and is connected.  Run '$0 -c disconn' to disconnect from target"
                fi
        else
                echo "No usable image exists.  Please run '$0 -c make' to create image"
        fi
        ;;

    *)
        #echo "unset param, please use param: make,conn,disconn..."
        usage
        ;;
esac
