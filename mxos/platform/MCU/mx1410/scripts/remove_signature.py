# Date    : 2019/08/27
# Author  : Yang Haibo
# Mail    : yanghb@mxchip.com

import re
import os
import sys
import struct

input_bin = sys.argv[1]
ota_bin = sys.argv[2]

filelen = os.path.getsize(input_bin)

input = open(input_bin, 'rb')
output = open(ota_bin, 'wb')
input.read(4)
b = struct.pack('<L', 0xFFFFFFFF)
output.write(b)
output.write(input.read())
input.close()
output.close()