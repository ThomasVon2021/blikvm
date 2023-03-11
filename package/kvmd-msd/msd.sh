#!/bin/bash

iso_file_name=()
dist_dir="/media/blikvm/ventoy/"

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

cd ~

if [ -f ~/blikvm/ventoy.img ] 
then
	echo "update file exist"
	cd blikvm
else
	if [ ! -d blikvm ]
	then
		mkdir blikvm
	fi
	cd blikvm
	echo "update file not exist,please wait..."
    dd if=/dev/zero of=ventoy.img bs=1M count=5000 status=progress;
fi

echo "blikvm" | sudo -S losetup -f ventoy.img

echo "blikvm" | sudo -S losetup -l | grep ventoy

dev_name=`echo "blikvm" | sudo -S losetup -l | grep ventoy | awk 'NR==1{print $1}'`

echo "blikvm" | sudo -S sh /home/blikvm/blikvm/ventoy-1.0.88/Ventoy2Disk.sh -i $dev_name;

echo "blikvm" | sudo -S mkdir -p $dist_dir

echo "blikvm" | sudo -S mount  $dev_name"p1" $dist_dir;

if [ $? -ne 0 ]
then
	echo "mount  $dev_name"p1" fail"
fi

cd ../iso/

traverse_dir /home/blikvm/iso

for name in ${iso_file_name[@]}
do
	echo ${name}
	echo ${name##*/}
	echo $dist_dir${name##*/}
	if [ -f $dist_dir${name##*/} ]
	then
		echo "exist update file: ${name},rm it "
		echo "blikvm" | sudo -S rm -f $dist_dir${name##*/}
		sync
	fi
	sleep 3
	echo ${name}" again!"
	sudo cp ${name} $dist_dir;
	if [ $? -ne 0 ]
	then
		echo "cp failed"
	else 
		echo "cp ${name} sucess!"
	fi
	sync
done


sudo umount $dev_name"p1"

sudo losetup -d $dev_name