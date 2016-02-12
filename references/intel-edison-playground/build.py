#
#  Intel Edison Playground
#  Copyright (c) 2015 Damian Kolakowski. All rights reserved.
#

import glob
import os
import config

gcc 	 = config.EDISON_SDK_ROOT + 'sysroots/i386-pokysdk-darwin/usr/bin/i586-poky-linux/i586-poky-linux-gcc'
sys_root = config.EDISON_SDK_ROOT + 'sysroots/core2-32-poky-linux'
c_files	 = glob.glob("*.c")

print 'Files     : [' + ', '.join(c_files) + ']'

for c_file in c_files:
	binary_file = os.path.splitext(c_file)[0]
	print 'Compiling : [' + c_file + ']...'
	if 0 != os.system(gcc + ' -m32 -march=i586 --sysroot=' + sys_root + ' -o ' + binary_file + ' ' + c_file + ' -lbluetooth'):
		exit()

print 'Success.'
