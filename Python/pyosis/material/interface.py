"""
Interfaces of OSIS functions

========

"""
from typing import Any, Dict
from ..core import OSISEngine

def osis_material(nMat: int, strName: str, eMaterialType: str, eCode: str, eGrade: str, nCrepShrk: int, dDmp: float):
    e = OSISEngine.GetInstance()
    return e.OSIS_Material(nMat, strName, eMaterialType, eCode, eGrade, nCrepShrk, dDmp)

def osis_material_del(nMat: int):
    e = OSISEngine.GetInstance()
    return e.OSIS_MaterialDel(nMat)

def osis_material_mod(nOld: int, nNew: int):
    e = OSISEngine.GetInstance()
    return e.OSIS_MaterialMod(nOld, nNew)