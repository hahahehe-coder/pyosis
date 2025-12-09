import os
import sys

# 默认dll位于上一级目录
# dll_dir = r"../"
dll_dir = "D:/OSIS 5/bin64"
if os.path.exists(dll_dir):
    os.add_dll_directory(dll_dir)
    sys.path.insert(0, dll_dir)


from PyInterface import PyInterface as OSISEngine  # 导入OSIS python接口

osis_engine = OSISEngine.GetInstance()

"""
clear; //清空项目信息
clc;//清空下方信息栏窗口信息
//----------------------------- CONTROL ---------------------------->>//主控数据（基本上都是默认的）
Acel,9.8066; //定义加速度
CalcTendon,1;//打开计算预应力开关
CalcConForce,1;//打开计算并发反力开关
CalcShrink,1;//打开计算收缩开关
CalcCreep,1;//打开计算徐变开关
CalcShear,1;//打开计算剪切变形开关
CalcRlx,1;//打开计算钢束松弛开关
ModLocCoor,0;//打开修改变截面局部坐标系开关
IncTendon,1;//打开考虑钢束自重开关
NL,0,0;//设置非线性参数，打开几何非线性
LnSrch,0;//设置非线性参数，打开非线性连接单元
AutoTs,0;//设置非线性参数，线形检索
ModOpt,0;//设置模态阶数
"""
# 还未完成

isok, error_code = osis_engine.OSIS_Replot()
print(isok, error_code)
if not isok:
    sys.exit(1)