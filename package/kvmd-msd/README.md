# operation step:

1. Confirm that the following four paths exist. If you are using the official latest image of blikvm, and it is fully configured, there is no need to check.
```
	/usr/bin/blikvm/ventoy-1.0.88
	/usr/bin/blikvm/kvmd-msd.sh
	/mnt/msd/user    
	/mnt/msd/ventoy
```

2. SSH logs in to blikvm, executes the rw command, and changes the system permission to writable.
```
	rw
```

3. In your PC ,use scp cmd send iso file to kvm board. 
```
	scp ***.iso blikvm@xxx.xxx.xxx.xxx:/mnt/msd/user/
```	
	
4. excute msd cmd.wait until excute end. 
- If you don't use option paramter：
- The default size of the USB flash disk is 5GB.
- The default filename is scan /mnt/msd/user dir.
- The default msd name is ventoy.
```
	sudo bash /usr/bin/blikvm/kvmd-msd.sh -c make
```
- If your iso is large than 5GB.You can specify the optional parameter - s xxx, xxx is the Size.
- If you need specify iso ,you can use -f xxx.iso. 
- If you want to specify the msd name, you can use -n xxx. 
```
	sudo bash /usr/bin/blikvm/kvmd-msd.sh -c make -s 5 -n ventoy -f xxx.iso
```
- Tips: cp progress slowly, Please be patient. 

- If you want to excute other cmd, you can use other option. c option contains :  make/conn/disconn/clean
5. connect msd
```
	sudo bash  /usr/bin/blikvm/kvmd-msd.sh -c conn
```
		
6. disconnect msd
```
	sudo bash  /usr/bin/blikvm/kvmd-msd.sh -c disconn
```

7. clean msd
```
	sudo bash  /usr/bin/blikvm/kvmd-msd.sh -c clean
```
		
8. throuht web restart PC, enter BIOS, modify boot priority,set blikvm USB first.

9. according to step, select operate system , format partition and install system.

10. notice: need install jq tool to support modify json file
```
	sudo apt install jq
```


## public use drive operation step:

# user-->blkvm-->control PC
1. first step, in user pc,  push file to iso direction.
```
	scp 1.txt blikvm@xxxx:/mnt/msd/user/
```
2. then in blikvm. excute below cmd.
```
	sudo bash /usr/bin/blikvm/kvmd-msd.sh -c make -s 4 -t other 
```

3. last in blikvm. excute below cmd.
```
	sudo bash /usr/bin/blikvm/kvmd-msd.sh -c conn
```

# control PC-->blikvm-->user
1. first create public usb driver(if has public usb driver, don't need to excute the cmd again.)
```
	sudo bash /usr/bin/blikvm/kvmd-msd.sh -c make -s 4 -t other 
```

2. first in control pc, copy data to usb driver,then excute below cmd.
```
	sudo bash /usr/bin/blikvm/kvmd-msd.sh -c rever
```

3. next in user pc, pull the data.
```
	scp blikvm@xxxx:/mnt/msd/user/*  .
```

NOTICE:  whether push or pull data,  must create the public usb driver firstly.
