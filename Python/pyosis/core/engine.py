import os
import sys
# 默认dll位于上一级目录
current_dir =os.path.dirname(os.path.abspath(__file__))
dll_dir= os.path.abspath(os.path.join(current_dir, "../../../../bin64"))
from PyInterface import PyInterface as OSISEngine