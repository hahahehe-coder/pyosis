"""
Interfaces of OSIS functions

========

"""


from typing import Any, Dict
from ..core import OSISEngine

def osis_load(eLoadType: str, strLCName: str, params: Dict[str, Any]):
    
    e = OSISEngine.GetInstance()
    return e.OSIS_Load(eLoadType, strLCName, params)

def osis_loadcase(strName: str, eLoadCaseType: str, dScalar: float, strPrompt: str):
    e = OSISEngine.GetInstance()
    return e.OSIS_LoadCase(strName, eLoadCaseType, dScalar, strPrompt)

