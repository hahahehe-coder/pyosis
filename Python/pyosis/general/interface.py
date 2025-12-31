from ..core import REGISTRY

@REGISTRY.register("Replot")
def osis_replot():
    """
    重新绘制窗口
    
    Returns:
        tuple (bool, str): 是否成功，失败原因
    """
    pass

@REGISTRY.register("Clear")
def osis_clear():
    """
    清空项目
    
    Returns:
        tuple (bool, str): 是否成功，失败原因
    """
    pass

@REGISTRY.register("Solve")
def osis_solve():
    """
    求解工程
    
    Returns:
        tuple (bool, str): 是否成功，失败原因
    """
    pass

@REGISTRY.register("CrpShrk")
def osis_creep_shrink(nNO: int=1, strName: str="收缩徐变1", dAvgHumidity: float=70.0, nBirthTime: int=7, dTypeCoeff: float=5.0, nBirthByShrinking: int=3):
    """
    设置收缩徐变

    Args:
        nNO (int): 收缩徐变特性编号
        strName (str): 特性名称
        dAvgHumidity (float): 年平均湿度（百分比）
        nBirthTime (int): 混凝土龄期（天）
        dTypeCoeff (float): 水泥种类系数
        nBirthByShrinking (int): 收缩开始时的混凝土龄期（天数）

    Returns:
        tuple (bool, str): 是否成功，失败原因
    """
    pass
