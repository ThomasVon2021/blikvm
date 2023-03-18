operation step:

1> put the kvmd-msd dir file into /usr/bin/blikvm/

include: 
	--ventoy-1.0.88/
	--kvmd-msd.sh

2> sudo mkdir -p /opt/bin/msd/user/     && sudo mkdir -p /opt/bin/msd/ventoy/

3> In your PC ,through scp cmd send iso file to board.
	scp ***.iso blikvm@xxx.xxx.xxx.xxx:/opt/bin/msd/user/
	
4> excute msd cmd.wait until excute end.
	sudo bash /usr/bin/blikvm/kvmd-msd.sh
	
5> excute disable_gadget.sh
	sudo bash /usr/bin/blikvm/disable-gadget.sh
	
6> reconnect img, excute enable_gadget.sh
	sudo bash /usr/bin/blikvm/enable-gadget.sh
	
7> throuht web restart PC, enter BIOS, modify boot priority,set blikvm USB first.

8> according to step, select operate system , format partition and install system.
