from ..core import OSISEngine

def osis_replot():
    """
    重新绘制窗口
    
    Returns:
        Tuple[bool, str]: (是否成功，失败原因)
    """
    e = OSISEngine.GetInstance()
    return e.OSIS_Replot()

def osis_creep_shrink(nNO: int, strName: str, dAvgHumidity: float, nBirthTime: int, dTypeCoeff: float, nBirthByShrinking: int):
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
        Tuple[bool, str]: (是否成功，失败原因)
    """
    e = OSISEngine.GetInstance()
    return e.OSIS_CrpShrk(nNO, strName, dAvgHumidity, nBirthTime, dTypeCoeff, nBirthByShrinking)
