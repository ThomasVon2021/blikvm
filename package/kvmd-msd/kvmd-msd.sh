#!/bin/bash

set -e

set -x

set -u

iso_file_name=()
ventoy_dir="/opt/bin/msd/ventoy"
iso_dir="/opt/bin/msd/user"
mount_dist_dir="/media/blikvm/ventoy/"


if [ $# -gt 1 ]
then
	echo "param need point one, or default 0 use iso all img."
fi

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
    
    if [ "${file##*.}"x = "iso"x ] ;then
        #echo $(basename $file)
		echo "storage: $file"
		iso_file_name[${#iso_file_name[*]}]=$file
    fi    
}

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
    sudo dd if=/dev/zero of=ventoy.img bs=1M count=4096 status=progress;
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

sudo cp -r "/usr/bin/blikvm/ventoy-1.0.88/plugin/ventoy" "$mount_dist_dir"

if [ $? -eq 0 ]
then
	echo "cp plugin ventoy sucess!!!"
fi

echo $PWD

if [ $# == 1 ];then
	file_name=$1
    if [ "${file_name##*.}"x = "iso"x ]
	then
		echo "param correct"
		echo $mount_dist_dir${name##*/}
		if [ -f $mount_dist_dir${name##*/} && -f "$iso_dir/${file_name}" ]
		then
			echo "exist update file: ${name},rm it "
			echo "blikvm" | sudo -S rm -f $mount_dist_dir${name##*/}
			sync
		fi
		sleep 3
		echo "$iso_dir/${file_name}"
		sudo cp "$iso_dir/${file_name}" "$mount_dist_dir";
		if [ $? -ne 0 ]
		then
			echo "cp failed"
		else 
			echo "cp ${name} sucess!"
		fi
		sync
		
	else
		echo "param suffix is not iso"
		return 1
	fi
elif  [ $# == 0 ];then
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
		sudo cp "${name}" "$mount_dist_dir";
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
sudo umount -f $dev_name"p1"
sudo losetup -d $dev_name
