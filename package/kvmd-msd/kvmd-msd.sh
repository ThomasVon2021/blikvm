#!/bin/bash

#CMD=$1


set -e

set -x

set -u

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


while getopts "c:f:s:n:" opt; do
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
    \?)
		echo "Invalid option: -$OPTARG" >&2
		exit 1
		;;
  esac
done


CMD=${CMD:-"unset"}
FILE=${FILE:-"*"}
VENTORY_SIZE=${VENTORY_SIZE:-5}
MSD_NAME=${MSD_NAME:-"ventoy"}

 
update_json() 
{
	sudo jq --arg key $1 --arg value $2 '.[$key] = $value' $msd_config_dir$msd_json > /home/blikvm/temp.json
	sudo mv /home/blikvm/temp.json $msd_config_dir$msd_json
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
	sudo echo '{"msd_status": "not_connected","msd_img_created": false}' | jq '.' > $msd_config_dir$msd_json

fi

case ${CMD} in
	make)
#	if [ $# -gt 2 ]
#	then
#		echo "param need point one, or default 0 use iso all img."
#	fi

	echo "blikvm" | sudo -S mount -o remount,rw /

	if [ -f  "$ventoy_dir/"$MSD_NAME".img" ] 
	then
		echo "update file exist"
		cd  $ventoy_dir
	else
		if [ ! -d  $ventoy_dir ]
		then
			echo "blikvm" | sudo -S mkdir -p  $ventoy_dir
		fi
		cd  $ventoy_dir
		echo "update file not exist,please wait..."
		
		sudo dd if=/dev/zero of=$MSD_NAME".img" bs=1M count=$((VENTORY_SIZE*1024)) status=progress;
	fi

	echo "blikvm" | sudo -S losetup -f $MSD_NAME".img"

	echo "blikvm" | sudo -S losetup -l | grep $MSD_NAME

	dev_name=`echo "blikvm" | sudo -S losetup -l | grep ventoy | awk 'NR==1{print $1}'`

	echo $dev_name

	echo "blikvm" | sudo -S sh /usr/bin/blikvm/ventoy-1.0.88/Ventoy2Disk.sh -i $dev_name;

	echo "blikvm" | sudo -S mkdir -p $mount_dist_dir

	echo "blikvm" | sudo -S mount  $dev_name"p1" $mount_dist_dir;

	if [ $? -ne 0 ]
	then
		echo "mount  $dev_name"p1" fail"
	fi

	#sudo cp -r "/usr/bin/blikvm/ventoy-1.0.88/plugin/ventoy" "$mount_dist_dir"

	#if [ $? -eq 0 ]
	#then
	#	echo "cp plugin ventoy sucess!!!"
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
			
			echo "blikvm" | sudo -S rm -f $mount_dist_dir${file_name##*/}
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
			echo "blikvm" | sudo -S mkdir -p  $iso_dir
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
				echo "blikvm" | sudo -S rm -f $mount_dist_dir${name##*/}
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

	update_json  msd_img_created true 
	sudo mount -o remount,ro /

	;;

	conn)
	cd  $ventoy_dir
	if [ -f  $ventoy_dir/$MSD_NAME".img_bak" ] 
	then
		echo "blikvm" | sudo -S mv $MSD_NAME".img_bak" $MSD_NAME".img"
	fi
	bash $usb_dis_gadget_sh
	bash $usb_gadget_sh
	update_json msd_status created
	;;

	disconn)
	cd  $ventoy_dir
	if [ -f  $ventoy_dir/$MSD_NAME".img" ] 
	then
		echo "blikvm" | sudo -S mv $MSD_NAME".img" $MSD_NAME".img_bak"
	fi
	bash $usb_dis_gadget_sh
	bash $usb_gadget_sh
	update_json msd_status not_connected
	;;

	clean)
	if [[ -f  $ventoy_dir/$MSD_NAME".img" || -f  $ventoy_dir/$MSD_NAME".img_bak" ]]
	then
		echo "blikvm" | sudo -S rm -f $ventoy_dir/*
	fi
	bash $usb_dis_gadget_sh
	bash $usb_gadget_sh
	update_json  msd_img_created not_created 
	update_json msd_status not_connected
	;;
	*)
	echo "unset param, please use param: make,conn,disconn..."
	;;
esac
