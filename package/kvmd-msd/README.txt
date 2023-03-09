operation step:


1> put the msd dir file into ~ dir:

include: 
	--blikvm/
	--iso
	--disable_gadget.sh
	--enable_gadget.sh
	--msd.sh

2> In your PC ,through scp cmd send iso file to board.
	scp ***.iso blikvm@xxx.xxx.xxx.xxx:~/iso/
	
3> excute msd cmd.wait until excute end.
	cd ~
	sudo bash msd.sh
	
4> excute disable_gadget.sh
	sudo bash disable_gadget.sh
	
5> reconnect img, excute enable_gadget.sh
	sudo bash enable_gadget.sh
	
6> throuht web restart PC, enter BIOS, modify boot priority,set blikvm USB first.

7> according to step, select operate system , format partition and install system.