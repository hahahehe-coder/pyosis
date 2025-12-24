"""
Interfaces of OSIS functions

========

"""


from typing import Any, Dict, Literal
from ..core import OSISEngine

def osis_load(eLoadType: Literal["GRAVITY", "NFORCE", "LINE", "DISPLACEMENT", "INITIAL", "UTEMP", "GTEMP", "PST", "CFORCE"], strLCName: str, params: Dict[str, Any]):
    '''
    创建荷载
    
    Args:
        eLoadType (str): 荷载类型，不区分大小写。 GRAVITY = 自重荷载，NFORCE = 节点荷载，LINE = 线荷载，DISPLACEMENT = 强迫位移，INITIAL = 初始内力，
            UTEMP = 均匀温度荷载，GTEMP = 梯度温度荷载， PST = 预应力，CFORCE = 索力
        strLCName (str): 工况名称
        params (Dict[str, Any]): 对应荷载类型所需要的参数
    Returns:
        tuple (bool, str): 是否成功，失败原因
    '''
    e = OSISEngine.GetInstance()
    eLoadType = eLoadType.upper()
    return e.OSIS_Load(eLoadType, strLCName, params)

def osis_load_nforce(strLCName: str, nNo: int, dFx: float, dFy: float, dFz: float, dMx: float, dMy: float, dMz: float):
    '''
    创建或修改节点荷载

    Args:
        strLCName (str): 荷载工况名称
        nNO (int): 节点编号
        dFx (float): 全局坐标系x方向的集中力
        dFy (float): 全局坐标系y方向的集中力
        dFz (float): 全局坐标系z方向的集中力
        dMx (float): 全局坐标系x方向的集中弯矩
        dMy (float): 全局坐标系y方向的集中弯矩
        dMz (float): 全局坐标系z方向的集中弯矩
    Returns:
        tuple (bool, str): 是否成功，失败原因
    '''
    params = locals()
    e = OSISEngine.GetInstance()
    return e.OSIS_Load("GRAVITY", strLCName, params)

def osis_loadcase(strLCName: str, eLoadCaseType: Literal["USER", "D", "DC", "DW", "DD", "CS"], dScalar: float=1.0, strPrompt: str =""):
    '''
    创建荷载工况

    Args:
        strName (str): 荷载工况名称
        eLoadCaseType (str): 荷载工况类型，不区分大小写。 USER = 用户定义的荷载， D = 桥规(JTJ 021-89)中的荷编号1(结构重力)， DC = 结构和非结构附属荷载， DW = 铺装和设备荷载， 
            DD = 桩端摩擦力， CS = 施工阶段荷载
        dScalar (float): 系数，默认1.0
        strPrompt (str): 说明，默认空
    Returns:
        tuple (bool, str): 是否成功，失败原因
    '''
    e = OSISEngine.GetInstance()
    eLoadCaseType = eLoadCaseType.upper()
    return e.OSIS_LoadCase(strLCName, eLoadCaseType, dScalar, strPrompt)

