
from ..core import OSISEngine

def osis_replot():
    e = OSISEngine.GetInstance()
    return e.OSIS_Replot()

def osis_creep_shrink(nNO: int, strName: str, dAvgHumidity: float, nBirthTime: int, dTypeCoeff: float, nBirthByShrinking: int):
    """
    设置收缩徐变

    :param interface: OSISEngine object
    :param node_id: node id
    :param creep_shrink: creep shrink value
    """
    e = OSISEngine.GetInstance()
    return e.OSIS_CrpShrk(nNO, strName, dAvgHumidity, nBirthTime, dTypeCoeff, nBirthByShrinking)