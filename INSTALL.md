
# Installation

## Install prerequisites
Make sure you have the Linux kernel headers installed as DPDK requires them to build
the kernel modules. On Ubuntu I run the following:

    // On Debian based systems
    # sudo apt-get install linux-headers-$(uname -r) libpcap-dev

    // On Red Hat systems
    # yum install -y make gcc kernel-devel elfutils-libelf-devel patch libasan libpcap-devel numactl-devel glibc-devel readline-devel pciutils git epel-release gcc-c++ autoconf automake libtool wget python ncurses-devel zlib-devel libjpeg-devel openssl-devel sqlite-devel libcurl-devel libxml2-devel libidn-devel e2fsprogs-devel pcre-devel speex-devel ldns-devel libedit-devel libyuv-devel opus-devel libvpx-devel unbound-devel libuuid-devel libsndfile-devel

On Red Hat-based systems the additional dependencies are due to the fact that you
will have to build LUA manually.

**Note:** On Red Hat proper you will need to install epel manually as it is not available
in the repositories.

**Note:** You will need to adjust the version number to match your current kernel 
version. If you upgrade your system or kernel version you will need to install the 
correct headers and rebuild the RTE_TARGET directory.

Be aware that most often there are mismatches when you update the kernel, but have
not yet rebooted. We suggested updating and then running the above if you haven't
already.

## Getting the Code

Get the DPDK and pktgen source code from github.com or dpdk.org repo via:

    # cd <InstallDir>
    # git clone git://dpdk.org/dpdk.git
    # cd <InstallDir>
    # git clone git://dpdk.org/pktgen-dpdk.git

**NOTE:** While on the dpdk site you must also pull down the dpdk kit itself. git://dpdk.org/dpdk

## Use the Quick Start Script to Install dpdk Toolkit and Configure Host

You can setup all relevant settings manually, but I recommend you use the quick 
start setup script detailed [here](https://doc.dpdk.org/guides/linux_gsg/quick_start.html).

## Manually Configure dpdk Toolkit

If you did not use the quick start script to configure huge pages you can follow
the below procedure.

**Note:** You will still need to manually compile the dpdk toolset before you follow
the below procedure. 

The commands above created a directory called Pktgen-DPDK in the current directory 
location. You now have the dpdk source.

Make sure you have HUGE TLB support in the kernel with the following commands:

For more information in huge pages see [here](https://www.kernel.org/doc/Documentation/vm/hugetlbpage.txt).
    
    # grep -i huge /boot/config-2.6.35-24-generic
    CONFIG_HUGETLBFS=y
    CONFIG_HUGETLB_PAGE=y

    # grep -i huge /proc/meminfo
    HugePages_Total:128
    HugePages_Free: 128
    HugePages_Rsvd:0
    HugePages_Surp:0
    Hugepagesize: 2048 kB

**Note:** The values in Total and Free maybe different until you reboot the machine.

Two files in /etc must be setup to support huge TLBs. If you do not have
hugeTLB support then you most likely need a newer kernel.

    # vi /etc/sysctl.conf

Add the below to the bottom of the file to enable 2M hugepages:

    vm.nr_hugepages=256

If you need more or less hugeTLB pages then you can change the value to a
number you need it to be. In some cases pktgen needs a fair number of pages
and making it too small will effect performance or pktgen will terminate on
startup looking for more pages.

**Note:** On Ubuntu 15.10 I noticed mounting /mnt/huge is not required as /dev/hugepages
is already mounted. Check your system and verify that /mnt/huge is required.
    
    # vi /etc/fstab

Add the below to the bottom of the file:

    huge /mnt/huge hugetlbfs defaults 0 0

    # mkdir /mnt/huge
    # chmod 777 /mnt/huge

Reboot your machine as the huge pages must be setup just after boot to make
sure you have contiguous memory for the 2Meg pages, setting up 1G pages can
also be done.

**Note:** If you startup Eclipse or WR Workbench before starting pktgen the first
 time after reboot, pktgen will fail to load because it can not get all of the
 huge pages as eclipse has consumed some of the huge pages. If you did start eclipse
 or WR Workbench then you need to close that application first.

This is my current machine you will have a few different numbers depending on
how your system was booted and if you had hugeTLB support enabled.
 
## Setup Prerequisites on Red Hat-based Systems

Red Hat does not provide an up-to-date LUA package so you will need to build it 
yourself. You can do this by following the below procedure.

1) Make sure you have run a `yum update -y` prior to continuing. We also recommend
   restarting after you have updated particularly if there were kernel updates.


2) Run `yum remove -y lua-devel` to make sure you don't already have an outdated
   LUA package installed.

   **Note:** This was tested with LUA 5.3.2.

3) Run the following commands:
   
        # cd /usr/local/src
        # curl -R -O http://www.lua.org/ftp/lua-5.3.2.tar.gz
        # tar zxf lua-5.3.2.tar.gz
        # cd /usr/local/src/lua-5.3.2
        # vi Makefile

        // Change: TO_LIB= liblua.a
        // To be: TO_LIB= liblua.a liblua.so
        // Save and close (<Esc> :wq!)

        # cd /usr/local/src/lua-5.3.2/src
        # vi Makefile
        
        // Change: CFLAGS= -O2 -Wall -Wextra -DLUA_COMPAT_5_2 $(SYSCFLAGS) $(MYCFLAGS)
        // To be: CFLAGS= -O2 -Wall -Wextra -DLUA_COMPAT_5_2 $(SYSCFLAGS) $(MYCFLAGS) -fPIC
        // Add under: LUA_A= liblua.a
        // The line: LUA_SO= liblua.so
        // Change: ALL_T= $(LUA_A) $(LUA_T) $(LUAC_T)
        // To be: ALL_T= $(LUA_A) $(LUA_T) $(LUAC_T) $(LUA_SO)
        // Add under: $(LUAC_T): $(LUAC_O) $(LUA_A)
        // $(CC) -o $@ $(LDFLAGS) $(LUAC_O) $(LUA_A) $(LIBS)
        // The lines: $(LUA_SO): $(CORE_O) $(LIB_O)
        // $(CC) -o $@ -shared $?
        // Save and close (<Esc> :wq!)

4) Export the LUA directory to your C include path so that gcc can find your headers

        # export C_INCLUDE_PATH=/usr/local/src/lua-5.3.2/src

5) Now we need to manually add the package configuration files. Run `vi /usr/lib64/pkgconfig/lua-5.3.pc`
   and add the below text. You will need to update the version appropriately if you
   are not using LUA 5.3.2. When you are finished save with `wq!`

        V= 5.3
        R= 5.3.2
        prefix= /usr
        exec_prefix=${prefix}
        libdir= /usr/lib64
        includedir=${prefix}/include

        Name: Lua
        Description: An Extensible Extension Language
        Version: ${R}
        Requires:
        Libs: -llua-${V} -lm -ldl
        Cflags: -I${includedir}/lua-${V}   

6) Now run `vi /usr/lib64/pkgconfig/lua5.3.pc` and add the below text.

        V=5.3
        R=5.3.2

        prefix=/usr
        INSTALL_BIN=${prefix}/bin
        INSTALL_INC=${prefix}/include
        INSTALL_LIB=${prefix}/lib
        INSTALL_MAN=${prefix}/share/man/man1
        INSTALL_LMOD=${prefix}/share/lua/${V}
        INSTALL_CMOD=${prefix}/lib/lua/${V}
        exec_prefix=${prefix}
        libdir=${exec_prefix}/lib
        includedir=${prefix}/include

        Name: Lua
        Description: An Extensible Extension Language
        Version: ${R}
        Requires:
        Libs: -L${libdir} -llua -lm -ldl
        Cflags: -I${includedir}

7) Move to the lua-5.3.2/src directory and run `make linux`.

8) Once the make has completed without errors copy your newly built libraries over
   to your lib64 folder with:

        # cp /usr/local/src/lua-5.3.2/src/liblua.so /usr/lib64/liblua-5.3.so
        # cp /usr/local/src/lua-5.3.2/src/liblua.a /usr/lib64/liblua-5.3.a

## Setup pktgen-DPDK

At the pktgen-DPDK level directory we have the 'tools/setup.sh' script,
which needs to be run as root once per boot. The script contains a commands to setup
the environment.

Befor you run the script you will need to run:

    # export RTE_SDK=<DPDKinstallDir>
    # export RTE_TARGET=x86_64-native-linuxapp-gcc

Make sure you run the setup script as root via `./tools/setup.sh`. The setup
script is a bash script and tries to setup the system correctly, but you may have to
change the script to match your number of huge pages you configured above and ports.

The `modprobe uio` command, in the setup script, loads the UIO support module into the
kernel plus it loads the igb-uio.ko module into the kernel. The two echo commands,
in the setup script, finish setting up the huge pages one for each socket. If you
only have a single socket system then comment out the second echo command. The last
command is to display the huge TLB setup.

Edit your .bashrc or .profile or .cshrc to add the environment variables.
I am using bash: `# vi ~/.bashrc`

Add the following lines: Change the $RTE_SDK to the location of the DPDK version
directory. Your SDK directory maybe named differently, but should point to the DPDK
SDK directory.

    # export RTE_SDK=<DPDKinstallDir>
    # export RTE_TARGET=x86_64-native-linuxapp-gcc
    
or use clang if you have it installed

    # export RTE_TARGET=x86_64-native-linuxapp-clang

Create the DPDK build tree if you haven't already:

    # cd $RTE_SDK
    # make install T=x86_64-native-linuxapp-gcc -j

The above command will create the x86_64-native-linuxapp-gcc directory in the
top level of the current-dkdp directory. The above command will build the basic
DPDK libraries and build tree.

Next we build pktgen:

    # cd <PktgenInstallDir>
    # make

You should now have pktgen built. To get started, see README.md. run pktgen type `sudo -E ./tools/run.sh`.
`run.sh` is a script designed to help you with the command line options of pktgen. 
You may need to modify this script for your system and configuration.

    # cat ./tools/run.sh
    #!/bin/bash

    #rkwiles@rkwiles-desk:~/projects/intel/dpdk$ lspci |grep Ether
    #06:00.0 Ethernet controller: Intel Corporation Ethernet Converged Network Adapter X520-Q1 (rev 01)
    #06:00.1 Ethernet controller: Intel Corporation Ethernet Converged Network Adapter X520-Q1 (rev 01)
    #08:00.0 Ethernet controller: Intel Corporation Ethernet Converged Network Adapter X520-Q1 (rev 01)
    #08:00.1 Ethernet controller: Intel Corporation Ethernet Converged Network Adapter X520-Q1 (rev 01)
    #09:00.0 Ethernet controller: Intel Corporation I350 Gigabit Network Connection (rev 01)
    #09:00.1 Ethernet controller: Intel Corporation I350 Gigabit Network Connection (rev 01)
    #83:00.1 Ethernet controller: Intel Corporation DH8900CC Null Device (rev 21)
    #87:00.0 Ethernet controller: Intel Corporation Ethernet Converged Network Adapter X520-Q1 (rev 01)
    #87:00.1 Ethernet controller: Intel Corporation Ethernet Converged Network Adapter X520-Q1 (rev 01)
    #89:00.0 Ethernet controller: Intel Corporation Ethernet Converged Network Adapter X520-Q1 (rev 01)
    #89:00.1 Ethernet controller: Intel Corporation Ethernet Converged Network Adapter X520-Q1 (rev 01)

if [ $name == "rkwiles-supermicro" ]; then
./app/app/${target}/pktgen -l 4-12 -n 3 --proc-type auto --socket-mem 512,512 --file-prefix pg -b 06:00.0 -b 06:00.1 -b 08:00.0 -b 08:00.1 -b 09:00.0 -b 09:00.1 -b 83:00.1 -- -T -P -m "[5:7].0, [6:8].1, [9:11].2, [10:12].3" -f themes/black-yellow.theme
fi
``
** Note: The '-m NNN' in the DPDK arguments is to have DPDK allocate 512 megs of memory.
 The '--socket-mem 256,156' DPDK command will allocate 256M from each CPU (two in this
 case). Do not use the '-m NNN' and '--socket-mem NN,NN' commands on the same command
 line.

The pktgen program follows the same format as a standard DPDK linuxapp, meaning
the first set of arguments '-l 0-4' are the standard DPDK arguments. This option
defines the number of logical cores to be used by pktgen. The 1f states 5 lcores
are used and the '3c' is just a bit array for each lcore to be used. The '-P' enables
promiscuous mode on all ports if you need that support. The '-m "..."' sets up the
port to lcore mapping for pktgen.

The second half of the command line followed by the '--' is pktgen specific
options.

The pktgen requires 2 logical cores for a minimum system. The first lcore 0 used
for pktgen command line and is used for timers also displaying the text on the
screen and lcores 1-n are used to do the packet receive and transmits along with
anything else related to packets. You do not need to start at lcore zero, but the first
lcore in the bitmap from in the least signification bit location is the display and
timer lcore used by pktgen.

The -m format now allows the developer to have different lcores for rx and tx
instead of rx/tx on the same lcore. The above format of {1:3}.0 states to use
lcore 1 for rx and lcore 3 for tx on port 0. Make sure you select different
lcores for the rx and tx so that only a single rx or tx function is on
a single physical core. If you pick the rx/tx functions on different lcores, but
on the same physical core you will not be able to get the performance you want as the
single physical core will be trying to do both Rx/Tx functions.

The '-n 2' is a required argument for DPDK and denotes the number of memory channels.

*** New setup and run python script with config files ***

Using the new tools/run.py script to setup and run pktgen with different configurations. The configuration files are located in the cfg directory with filenames ending in .cfg.

To use a configuration file;
``
$ ./tools/run.py -s default  # to setup the ports and attach them to DPDK (only needed once per boot)

$ ./tools/run.py default     # Run the default configuration
``
The configuration files are python scritps or a set of variables that run.py uses to initialize and run pktgen.
Here is an example of the default.cfg file:

``
# Setup configuration
setup = {
	'devices': [
		'04:00.0 04:00.1 04:00.2 04:00.3',
		'81:00.0 81:00.1 81:00.2 81:00.3',
		'82:00.0 83:00.0'
		],

	'opts': [
		'-b igb_uio'
		]
	}

# Run command and options
run = {
	'dpdk': [
		'-l 8,9-16',
		'-n 4',
		'--proc-type auto',
		'--log-level 7',
		'--socket-mem 2048,2048',
		'--file-prefix pg'
		],

	'blacklist': [
		'-b 05:00.0 -b 05:00.1',
		'-b 04:00.0 -b 04:00.1 -b 04:00.2 -b 04:00.3',
		#'-b 81:00.0 -b 81:00.1 -b 81:00.2 -b 81:00.3',
		'-b 82:00.0 -b 83:00.0'
		],

	'pktgen': [
		'-T',
		'-P',
		'--crc-strip',
		'-m [9:10].0',
		'-m [11:12].1',
		'-m [13:14].2',
		'-m [15:16].3'
		],

	'misc': [
		'-f themes/black-yellow.theme'
		]
	}
``

``
Usage: ./app/pktgen -l CORELIST -n NUM [-m NB] [-r NUM] [-b <domain:bus:devid.func>][--proc-type primary|secondary|auto]

Copyright (c) <2010-2018>, Intel Corporation. All rights reserved. Powered by DPDK
./app/app/x86_64-dnet-linuxapp-gcc/pktgen: invalid option -- 'x'
EAL: Detected 72 lcore(s)
./app/app/x86_64-dnet-linuxapp-gcc/pktgen: invalid option -- 'x'

Usage: ./app/app/x86_64-dnet-linuxapp-gcc/pktgen [options]

EAL common options:
  -c COREMASK         Hexadecimal bitmask of cores to run on
  -l CORELIST         List of cores to run on
                      The argument format is <c1>[-c2][,c3[-c4],...]
                      where c1, c2, etc are core indexes between 0 and 128
  --lcores COREMAP    Map lcore set to physical cpu set
                      The argument format is
                            '<lcores[@cpus]>[<,lcores[@cpus]>...]'
                      lcores and cpus list are grouped by '(' and ')'
                      Within the group, '-' is used for range separator,
                      ',' is used for single number separator.
                      '( )' can be omitted for single element group,
                      '@' can be omitted if cpus and lcores have the same value
  --master-lcore ID   Core ID that is used as master
  -n CHANNELS         Number of memory channels
  -m MB               Memory to allocate (see also --socket-mem)
  -r RANKS            Force number of memory ranks (don't detect)
  -b, --pci-blacklist Add a PCI device in black list.
                      Prevent EAL from using this PCI device. The argument
                      format is <domain:bus:devid.func>.
  -w, --pci-whitelist Add a PCI device in white list.
                      Only use the specified PCI devices. The argument format
                      is <[domain:]bus:devid.func>. This option can be present
                      several times (once per device).
                      [NOTE: PCI whitelist cannot be used with -b option]
  --vdev              Add a virtual device.
                      The argument format is <driver><id>[,key=val,...]
                      (ex: --vdev=net_pcap0,iface=eth2).
  -d LIB.so|DIR       Add a driver or driver directory
                      (can be used multiple times)
  --vmware-tsc-map    Use VMware TSC map instead of native RDTSC
  --proc-type         Type of this process (primary|secondary|auto)
  --syslog            Set syslog facility
  --log-level         Set default log level
  -v                  Display version information on startup
  -h, --help          This help

EAL options for DEBUG use only:
  --huge-unlink       Unlink hugepage files after init
  --no-huge           Use malloc instead of hugetlbfs
  --no-pci            Disable PCI
  --no-hpet           Disable HPET
  --no-shconf         No shared config (mmap'd files)

EAL Linux options:
  --socket-mem        Memory to allocate on sockets (comma separated values)
  --huge-dir          Directory where hugetlbfs is mounted
  --file-prefix       Prefix for hugepage filenames
  --base-virtaddr     Base virtual address
  --create-uio-dev    Create /dev/uioX (usually done by hotplug)
  --vfio-intr         Interrupt mode for VFIO (legacy|msi|msix)
  --xen-dom0          Support running on Xen dom0 without hugetlbfs
