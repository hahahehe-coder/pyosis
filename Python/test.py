import os
import sys

# 默认dll位于上一级目录
# dll_dir = r"../"
#dll_dir = "D:/OSIS 5/bin64"
#if os.path.exists(dll_dir):
#    os.add_dll_directory(dll_dir)
#    sys.path.insert(0, dll_dir)


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
try:
    osis_engine.OSIS_Acel(9.8066)
    osis_engine.OSIS_CalcTendon(1)
    osis_engine.OSIS_CalcConForce(1)
    osis_engine.OSIS_CalcShrink(1)
    osis_engine.OSIS_CalcCreep(1)
    osis_engine.OSIS_CalcShear(1)
    osis_engine.OSIS_CalcRlx(1)
    osis_engine.OSIS_ModLocCoor(0)
    osis_engine.OSIS_IncTendon(1)
    osis_engine.OSIS_NL(0, 0)
    osis_engine.OSIS_LnSrch(0)
    osis_engine.OSIS_AutoTs(0)
    osis_engine.OSIS_ModOpt(0)
except Exception as e:
    print(e)
    sys.exit(1)

"""
CrpShrk,1,收缩徐变,70.00,7,5.000,3;//定义收缩徐变特性参数，基本上伴随混凝土材料有
"""
isok, error_code = osis_engine.OSIS_CrpShrk(1,"收缩徐变",70.00,7,5.000,3)
print(isok, error_code)
if not isok:
    sys.exit(1)
"""
Material,1,C50,CONC,JTG3362_2018,C50,1,0.050;//定义混凝土材料
"""
isok, error_code = osis_engine.OSIS_Material(1,"C50","CONC","JTG3362_2018","C50",1,0.050)
print(isok, error_code)
if not isok:
    sys.exit(1)
    
"""
Section,1,C50,RECT,Fillet,Solid,1.0000,2.0000,1.0000,0.5000,0.0000,1.0000,1.0000,0.5000,0.2500,0,1.0000,0.5000,0.2500,0,1.2000,0.8000,0.2000;//定义矩形截面
"""
section_params = {
    "TransitionType": "Fillet",
    "SecType": "Solid",
    "B": 1.0000,
    "H": 2.0000,
    "xo1": 1.0000,
    "yo1": 0.5000,
    "R": 0.0000,
    "t1": 1.0000,
    "t2": 1.0000,
    "xi1": 0.5000,
    "yi1": 0.2500,
    "HasDiaphragm": 0,
    "tw": 1.0000,
    "xi2": 0.5000,
    "yi2": 0.2500,
    "HasGroove": 0,
    "b1": 1.2000,
    "b2": 0.8000,
    "h": 0.2000
}
isok, error_code = osis_engine.OSIS_Section(1,"C50","RECT", section_params)
print(isok, error_code)
if not isok:
    sys.exit(1)

"""
SectionOffset,1,Middle,0.0000,Center,0.0000;//设置截面偏心
"""
isok, error_code = osis_engine.OSIS_SectionOffset(1, "Middle", 0.0000, "Center", 0.0000)
print(isok, error_code)
if not isok:
    sys.exit(1)

"""
# SectionMesh,1,0,0.1000;//设置截面网格划分参数
"""
isok, error_code = osis_engine.OSIS_SectionMesh(1, False, 0.1000)
print(isok, error_code)
if not isok:
    sys.exit(1)


"""
Node,1,0.0000,0.0000,0.0000;
Node,2,1.0000,0.0000,0.0000;
Node,3,2.0000,0.0000,0.0000;
Node,4,3.0000,0.0000,0.0000;
Node,5,4.0000,0.0000,0.0000;
Node,6,5.0000,0.0000,0.0000;
Node,7,6.0000,0.0000,0.0000;
Node,8,7.0000,0.0000,0.0000;
Node,9,8.0000,0.0000,0.0000;
Node,10,9.0000,0.0000,0.0000;
Node,11,10.0000,0.0000,0.0000;//定义节点
"""
for i in range(11):
    isok, error_code = osis_engine.OSIS_Node(i+1, i*1.0000, 0.0000, 0.0000)
    print(isok, error_code)
    if not isok:
        sys.exit(1)

"""
Element,1,BEAM3D,1,2,1,1,1,1,1,0.000E+00,0,0.00,0;
Element,2,BEAM3D,2,3,1,1,1,1,1,0.000E+00,0,0.00,0;
Element,3,BEAM3D,3,4,1,1,1,1,1,0.000E+00,0,0.00,0;
Element,4,BEAM3D,4,5,1,1,1,1,1,0.000E+00,0,0.00,0;
Element,5,BEAM3D,5,6,1,1,1,1,1,0.000E+00,0,0.00,0;
Element,6,BEAM3D,6,7,1,1,1,1,1,0.000E+00,0,0.00,0;
Element,7,BEAM3D,7,8,1,1,1,1,1,0.000E+00,0,0.00,0;
Element,8,BEAM3D,8,9,1,1,1,1,1,0.000E+00,0,0.00,0;
Element,9,BEAM3D,9,10,1,1,1,1,1,0.000E+00,0,0.00,0;
Element,10,BEAM3D,10,11,1,1,1,1,1,0.000E+00,0,0.00,0;//定义单元
"""
for i in range(10):
    isok, error_code = osis_engine.OSIS_Element(i+1, "BEAM3D", {
        "nNode1": i+1,
        "nNode2": i+2,
        "nMat": 1,
        "nSec1": 1,
        "nSec2": 1,
        "nYTrans": 1,
        "nZTrans": 1,
        "dStrain": 0.00,
        "bFlag": 0,
        "dTheta": 0.00,
        "bWarping": 0
    })
    print(isok, error_code)
    if not isok:
        sys.exit(1)

isok, error_code = osis_engine.OSIS_Replot()
print(isok, error_code)
if not isok:
    sys.exit(1)