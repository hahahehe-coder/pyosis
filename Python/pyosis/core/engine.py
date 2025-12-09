import os
import sys

# 默认dll位于上一级目录
current_dir =os.path.dirname(os.path.abspath(__file__))
dll_dir= os.path.abspath(os.path.join(current_dir, "../../.."))
# dll_dir = os.path.abspath(os.path.join(os.path.dirname(__file__), '..'))
# dll_dir = r"../"
if os.path.exists(dll_dir):
    os.add_dll_directory(dll_dir)
    sys.path.insert(0, dll_dir)

from PyInterface import PyInterface as OSISEngine