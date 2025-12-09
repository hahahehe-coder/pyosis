"""
Interfaces of OSIS functions

========

"""


from typing import Any, Dict
from ..core import OSISEngine

def osis_element(nEle: int, eElementType: str, kwargs: Dict[str, Any]):
    e = OSISEngine.GetInstance()
    return e.OSIS_Element(nEle, eElementType, kwargs)

def osis_element_del(nEle: int):
    e = OSISEngine.GetInstance()
    return e.OSIS_ElementDel(nEle)

def osis_element_mod(nOld: int, nNew: int):
    e = OSISEngine.GetInstance()
    return e.OSIS_ElementMod(nOld, nNew)