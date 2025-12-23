"""
Interfaces of OSIS functions

========

"""


from typing import Any, Dict
from ..core import OSISEngine

def osis_boundary(nBd: int, eBoundaryType: str, params: Dict[str, Any]):
    e = OSISEngine.GetInstance()
    return e.OSIS_Boundary(nBd, eBoundaryType, params)

def osis_assign_boundary(nBd: int, eOP: str, nodeNOs: list):
    e = OSISEngine.GetInstance()
    return e.OSIS_AsgnBd(nBd, eOP, nodeNOs)
