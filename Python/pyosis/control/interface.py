"""
Interfaces of OSIS functions

========

"""


from typing import Any, Dict
from ..core import OSISEngine

def osis_acel(dG: float):
    e = OSISEngine.GetInstance()
    return e.OSIS_Acel(dG)

def osis_calc_tendon(bFlag: int):
    e = OSISEngine.GetInstance()
    return e.OSIS_CalcTendon(bFlag)

def osis_calc_con_force(bFlag: int):
    e = OSISEngine.GetInstance()
    return e.OSIS_CalcConForce(bFlag)

def osis_calc_shrink(bFlag: int):
    e = OSISEngine.GetInstance()
    return e.OSIS_CalcShrink(bFlag)

def osis_calc_creep(bFlag: int):
    e = OSISEngine.GetInstance()
    return e.OSIS_CalcCreep(bFlag)

def osis_calc_shear(bFlag: int):
    e = OSISEngine.GetInstance()
    return e.OSIS_CalcShear(bFlag)

def osis_calc_rlx(bFlag: int):
    e = OSISEngine.GetInstance()
    return e.OSIS_CalcRlx(bFlag)

def osis_mod_loc_coor(bFlag: int):
    e = OSISEngine.GetInstance()
    return e.OSIS_ModLocCoor(bFlag)

def osis_inc_tendon(bFlag: int):
    e = OSISEngine.GetInstance()
    return e.OSIS_IncTendon(bFlag)

def osis_nl(bGeom: int, bLink: int):
    e = OSISEngine.GetInstance()
    return e.OSIS_NL(bGeom, bLink)

def osis_ln_srch(bFlag: int):
    e = OSISEngine.GetInstance()
    return e.OSIS_LnSrch(bFlag)

def osis_auto_ts(bFlag: int):
    e = OSISEngine.GetInstance()
    return e.OSIS_AutoTs(bFlag)

def osis_mod_opt(nMod: int):
    e = OSISEngine.GetInstance()
    return e.OSIS_ModOpt(nMod)
