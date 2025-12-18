"""
Interfaces of OSIS functions

========

"""
from typing import Any, Dict
from ..core import OSISEngine

def osis_material(nMat: int, strName: str, eMaterialType: str, eCode: str, eGrade: str, nCrepShrk: int, dDmp: float):
    """创建或修改材料。

    Args:
        nMat (int): 材料编号
        strName (str): 材料名称
        eMaterialType (str): 材料类型，可选值：
            * CONC: 混凝土
            * STEEL: 钢材
            * PRESTRESSED: 预应力材料
            * REBAR: 普通钢筋
        eCode (str): 材料标准代码，可选值：
            * 钢材: JTGD64_2015
            * 其他材料: JTG3362_2018, JTGD62_2004
        eGrade (str): 材料等级牌号，根据材料类型可选：
            * 混凝土: C15, C20, C25, C30, C35, C40, C45, C50, C55, C60, C65, C70, C75, C80
            * 钢材: Q235, Q345, Q390, Q420
            * 预应力材料:
                ** JTG3362_2018: Strand1720, Strand1860, Strand1960, Wire1470, Wire1570, 
                  Wire1770, Wire1860, Rebar785, Rebar930, Rebar1080
                ** JTGD62_2004: Strand1860, Wire1670, Wire1770, Rebar785, Rebar930
            * 普通钢筋:
                ** JTG3362_2018: HPB300, HRB400, HRBF400, RRB400, HRB500
                ** JTGD62_2004: R235, HRB335, HRB400, KL400
        nCrepShrk (int): 收缩徐变特性编号（混凝土材料需要，其他材料设置为-1）
        dDmp (float): 材料阻尼比

    Returns:
        tuple (bool, str):
            - bool: 操作是否成功
            - str: 失败原因（如果操作失败）

    Examples:
        >>> result = osis_material(1, "C30", "CONC", "JTG3362_2018", "C30", 1, 0.05)
        >>> print(result)
        (True, "")
    """
    e = OSISEngine.GetInstance()
    return e.OSIS_Material(nMat, strName, eMaterialType, eCode, eGrade, nCrepShrk, dDmp)

def osis_material_del(nMat: int):
    e = OSISEngine.GetInstance()
    return e.OSIS_MaterialDel(nMat)

def osis_material_mod(nOld: int, nNew: int):
    e = OSISEngine.GetInstance()
    return e.OSIS_MaterialMod(nOld, nNew)