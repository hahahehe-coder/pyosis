from typing import Any, Dict
from ..core import OSISEngine

def osis_section(nSec: int, strName: str, eSectionType: str, kwargs: Dict[str, Any]):
    e = OSISEngine.GetInstance()
    return e.OSIS_Section(nSec, strName, eSectionType, kwargs)

def osis_section_offset(nSec: int, offsetTypeY: str, dOffsetValueY: float, offsetTypeZ: str, dOffsetValueZ: float):
    e = OSISEngine.GetInstance()
    return e.OSIS_SectionOffset(nSec, offsetTypeY, dOffsetValueY, offsetTypeZ, dOffsetValueZ)

def osis_section_mesh(nSec: int, bMeshMethod: int, dMeshSize: float):
    e = OSISEngine.GetInstance()
    return e.OSIS_SectionMesh(nSec, bMeshMethod, dMeshSize)

def osis_section_del(nSec: int):
    e = OSISEngine.GetInstance()
    return e.OSIS_SectionDel(nSec)

def osis_section_mod(nOld: int, nNew: int):
    e = OSISEngine.GetInstance()
    return e.OSIS_SectionMod(nOld, nNew)