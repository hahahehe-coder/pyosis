'''
pyosis.property.pu_curve 的 Docstring

荷载-位移曲线
'''

'''
PUCurve, Index, Name，Type，Num，disp_i, force_i, ...
简介：创建或修改荷载-位移曲线
Index：位移-力（矩）曲线编号
Name：曲线名称
Type：0 = 力；1 = 力矩
Num：曲线点数
disp_i：第i个点位移值
force_i：第i个点力（矩）值
注：荷载与位移需要唯一对应@王元杰
'''

from ..core import REGISTRY

@REGISTRY.register("PUCurve")
def osis_pu_curve(nIndex: int, strName: str, eType: int, nNum: int, displacement: list[float], force: list[float]):
    '''
    创建或修改荷载-位移曲线
    
    Args:
        nIndex (int): 位移-力（矩）曲线编号
        strName (str): 曲线名称
        eType (str): 
            * 0 = 力
            * 1 = 力矩
        nNum (int): 曲线点数
        displacement (list): i个点的位移值
        force (list): i个点的力（矩）值

    Returns:
        tuple (bool, str): 返回一个元组，包含：
            - bool: 操作是否成功
            - str: 失败原因（如果操作失败）
    '''
    pass

@REGISTRY.register('PUCurveDel')
def osis_creep_shrink_del(nNO: int=1):
    """删除收缩徐变特性

    Args:
        nNO (int): 收缩徐变特性编号

    Returns:
        tuple (bool, str):
            - bool: 操作是否成功
            - str: 失败原因（如果操作失败）
    """
    pass

@REGISTRY.register('PUCurveMod')
def osis_creep_shrink_mod(nOld: int=1, nNew: int=2):
    """修改一个收缩徐变特性的编号。收缩徐变特性编号存在时，交换

    Args:
        nOld (int): 旧编号
        nNew (int): 新编号

    Returns:
        tuple (bool, str):
            - bool: 操作是否成功
            - str: 失败原因（如果操作失败）
    """
    pass

