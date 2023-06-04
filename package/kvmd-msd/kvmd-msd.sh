#!/bin/bash

CMD=$1
FILE=$2

set -e

set -x

set -u

iso_file_name=()
ventoy_dir="/mnt/msd/ventoy"
iso_dir="/opt/bin/msd/user"
mount_dist_dir="/media/blikvm/ventoy/"
usb_gadget_sh="/usr/bin/blikvm/enable-gadget.sh"
usb_dis_gadget_sh="/usr/bin/blikvm/disable-gadget.sh"

CMD=${CMD:-"unset"}

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


case ${CMD} in
	make)
	if [ $# -gt 2 ]
	then
		echo "param need point one, or default 0 use iso all img."
	fi

	echo "blikvm" | sudo -S mount -o remount,rw /

	if [ -f  "$ventoy_dir/ventoy.img" ] 
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
		sudo dd if=/dev/zero of=ventoy.img bs=1M count=5120 status=progress;
	fi

	echo "blikvm" | sudo -S losetup -f ventoy.img

	echo "blikvm" | sudo -S losetup -l | grep ventoy

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

	echo $PWD

	if [ $# == 2 ];then
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
	elif  [ $# == 1 ];then
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
	else
		echo "sorry, param error, need one, current: $#"
		return 1
	fi
	umount -f $dev_name"p1"
	losetup -d $dev_name

	;;

	conn)
	cd  $ventoy_dir
	if [ -f  "$ventoy_dir/ventoy.img_bak" ] 
	then
		echo "blikvm" | sudo -S mv ventoy.img_bak ventoy.img
	fi
	bash $usb_dis_gadget_sh
	bash $usb_gadget_sh
	;;

	disconn)
	cd  $ventoy_dir
	if [ -f  "$ventoy_dir/ventoy.img" ] 
	then
		echo "blikvm" | sudo -S mv ventoy.img ventoy.img_bak
	fi
	bash $usb_dis_gadget_sh
	bash $usb_gadget_sh
	;;

	clean)
	if [[ -f  "$ventoy_dir/ventoy.img" || -f  "$ventoy_dir/ventoy.img_bak" ]]
	then
		echo "blikvm" | sudo -S rm -f $ventoy_dir/ventoy.*
	fi
	bash $usb_dis_gadget_sh
	bash $usb_gadget_sh
	;;
	*)
	echo "unset param, please use param: make,conn,disconn..."
	;;
esac
