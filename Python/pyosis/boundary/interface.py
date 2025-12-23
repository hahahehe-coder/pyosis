"""
Interfaces of OSIS functions

========

"""


from typing import Any, Dict, Optional, Literal
from ..core import OSISEngine

def osis_boundary(nBd: int, eBoundaryType: Literal["GENERAL", "MSTSLV", "RELEASE", "ELSTCSPT"], params: Dict[str, Any]):
    '''
    创建边界
    
    Args:
        nBd (int): 边界编号
        eBoundaryType (str): 边界类型，GENERAL = 一般边界，MSTSLV = 主从约束，RELEASE = 释放梁端约束，ELSTCSPT = 节点弹性支承
        params (Dict[str, Any]): 对应边界类型所需要的参数
    Returns:
        tuple (bool, str): 是否成功，失败原因
    '''
    e = OSISEngine.GetInstance()
    return e.OSIS_Boundary(nBd, eBoundaryType, params)

def osis_assign_boundary(nBd: int, eOP: str, nodeNOs: list[int] | list[list[int]]):
    '''
    分配边界给节点(一般支撑，节点弹性支撑)
    
    Args:
        nBd (int): 边界编号
        eOP (str): 操作, a = 添加，s = 替换，r = 移除, aa = 添加全部，ra = 移除全部
        nodeNOs (list[int] | list[list[int]]): 对于 s（替换）之外的操作，为所有要操作的节点编号列表。
            对于 s（替换）操作，需要传入两个元素一一对应的list：第一个是要被替换的，第二个是要替换成的
    Returns:
        tuple (bool, str): 是否成功，失败原因
    '''
    e = OSISEngine.GetInstance()
    return e.OSIS_AsgnBd(nBd, eOP, nodeNOs)

def osis_boundary_general(nBd: int, nCoor: int = -1, bDx: bool = 1, bDy: bool = 1, bDz: bool = 1, bMx: bool = 1, bMy: bool = 1, bMz: bool = 1):
    '''
    定义或修改一般边界
    
    Args:
        nBd (int): 编号
        nCoor (int): 局部坐标系编号，-1代表缺省
        bDx (bool): 0 = 释放，1 = 约束
        bDy (bool): 0 = 释放，1 = 约束
        bDz (bool): 0 = 释放，1 = 约束
        bMx (bool): 0 = 释放，1 = 约束
        bMy (bool): 0 = 释放，1 = 约束
        bMz (bool): 0 = 释放，1 = 约束
    Returns:
        tuple (bool, str): 是否成功，失败原因
    '''
    params = locals()  # 返回局部变量字典
    e = OSISEngine.GetInstance()
    return e.OSIS_Boundary(nBd, "GENERAL", params)