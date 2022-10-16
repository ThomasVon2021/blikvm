#!/usr/bin/env python3
# ========================================================================== #
#                                                                            #
#    video daemon for BLIKVM.                               #
#                                                                            #
#    Copyright (C) 2018  shang binbin <1767893964@qq.com>                    #
#                                                                            #
#    This program is free software: you can redistribute it and/or modify    #
#    it under the terms of the GNU General Public License as published by    #
#    the Free Software Foundation, either version 3 of the License, or       #
#    (at your option) any later version.                                     #
#                                                                            #
#    This program is distributed in the hope that it will be useful,         #
#    but WITHOUT ANY WARRANTY; without even the implied warranty of          #
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           #
#    GNU General Public License for more details.                            #
#                                                                            #
#    You should have received a copy of the GNU General Public License       #
#    along with this program.  If not, see <https://www.gnu.org/licenses/>.  #
#                                                                            #
# ========================================================================== #
import re
import os

# =====
def main() -> None:
    video_flag = os.popen('ls /dev/video0').read()
    if "video0" not in video_flag:
        print("video0 don't exit")
        return
    os.popen('v4l2-ctl --set-edid=file=1080P50EDID.txt --fix-edid-checksums')
    video_set = os.popen('v4l2-ctl --query-dv-timings').read()
    resolution_w = int(re.findall("\Active width:(.*?)", video_set))
    print("resolution_w:%d",resolution_w)

if __name__ == "__main__":
    main()