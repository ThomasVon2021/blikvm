# Install ISO operation step:

1. Confirm that the following four paths/files exist. If you are using the official latest image of blikvm, and it is fully configured, then there is no need to check.
```bash
/usr/bin/blikvm/ventoy-1.0.88
/usr/bin/blikvm/kvmd-msd.sh
/mnt/msd/user    
/mnt/msd/ventoy
```

2. Login via SSH to blikvm, executes the `rw` command which changes the filesystem from read-only to writable:

```bash
rw
```

3. On your PC, use `scp` command to send the iso file to your KVM board.

```bash
scp ***.iso blikvm@xxx.xxx.xxx.xxx:/mnt/msd/user/
```

4. Execute the `kvmd-msd.sh` script. Please, wait until the script is done executing.

- If you don't use option paramter：
- The default size of the USB flash disk is 5GB.
- The default filename is scan /mnt/msd/user dir.
- The default msd name is ventoy.

```bash
sudo bash /usr/bin/blikvm/kvmd-msd.sh -c make
```

- If your ISO is large than 5GB. You can specify the optional parameter `-s xxx`, xxx is the size.
- If you need specify the ISO filename, you can use `-f xxx.iso`. 
- If you want to specify the msd name, you can use `-n xxx`. 

```bash
sudo bash /usr/bin/blikvm/kvmd-msd.sh -c make -s 5 -n ventoy -f xxx.iso
```

- Note: this progress takes time, please be patient. 

- If you want to execute other cmd, you can use other option. c option contains :  make/conn/disconn/clean
5. Connect to msd:

```bash
sudo bash /usr/bin/blikvm/kvmd-msd.sh -c conn
```
		
6. Disconnect from msd

```bash
sudo bash /usr/bin/blikvm/kvmd-msd.sh -c disconn
```

7. clean msd:

```bash
sudo bash /usr/bin/blikvm/kvmd-msd.sh -c clean
```
		
8. Restart your PC, enter BIOS, modify boot priority and set the BliKVM USB as the first boot device.

9. Follow the steps, select operate system, format partition and install system.

10. Notice: need install `jq` tool to support modify json files.

```bash
sudo apt install jq
```

## File transfer function operation step:

# user-->blikvm-->control PC

1. First step, from your user PC, push a file to iso direction.

```bash
scp xxx blikvm@xxxx:/mnt/msd/user/
```

2. Then in BliKVM execute the command below:

```bash
sudo bash /usr/bin/blikvm/kvmd-msd.sh -c make -s 4 -t other 
```

3. Last in BliKVM execute the command below:

```bash
sudo bash /usr/bin/blikvm/kvmd-msd.sh -c conn
```

4. If you continue copy data to Control PC

```bash
sudo bash /usr/bin/blikvm/kvmd-msd.sh -c forward
```


# Control PC-->blikvm-->user

1. First create public USB drive (if it already has a public USB drive, don't need to execute the command again).

```bash
sudo bash /usr/bin/blikvm/kvmd-msd.sh -c make -s 4 -t other 
```

2. First on the control pc, copy the data to the USB drive, then execute the command below:

```bash
sudo bash /usr/bin/blikvm/kvmd-msd.sh -c rever
```

3. Next in the user PC, pull the data:

```bash
scp blikvm@xxxx:/mnt/msd/user/*  .
```

NOTICE: When you pushing or pulling data, you must create a public USB drive first.
