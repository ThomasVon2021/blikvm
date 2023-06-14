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
	
4. excute msd cmd.wait until excute end. The default size of the USB flash disk is 5GB.
```
	sudo bash /usr/bin/blikvm/kvmd-msd.sh -c make -s 5 -n ventoy -f xxx.iso
```
- If you want to excute other cmd, you can use other option. c option contain :  make/conn/disconn/clean--
- If your iso is large than 5GB.You can specify the optional parameter - s xxx, xxx is the Size.
- If you need specify iso ,you can use -f xxx.iso. 
- If you want to specify the msd name, you can use -n xxx. 

**Tips: cp progress slowly, Please be patient.  **

5. If you want to expand the USB flash size, find "sudo dd if=/dev/zero of=ventoy.img bs=1M count=5120 status=progress;" this line in kvmd-msd.sh, change the "count=5120". 

	
6. connect msd
```
	sudo bash  /usr/bin/blikvm/kvmd-msd.sh -c conn
```
		
7. disconnect msd
```
	sudo bash  /usr/bin/blikvm/kvmd-msd.sh -c disconn
```

8. clean msd
```
	sudo bash  /usr/bin/blikvm/kvmd-msd.sh -c clean
```
		
9. throuht web restart PC, enter BIOS, modify boot priority,set blikvm USB first.

10. according to step, select operate system , format partition and install system.
