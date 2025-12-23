
from ..core import OSISEngine

def osis_elem_force(strLCName: str, eDataItem: str, eElementType: str):
    e = OSISEngine.GetInstance()
    return e.OSIS_ElemForce(strLCName, eDataItem, eElementType)
