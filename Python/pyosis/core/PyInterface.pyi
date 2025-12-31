'''
PyInterface 的 高亮提示文件
'''

from typing import Any, Dict, Tuple

class PyInterface:
    _instance = None
    
    @classmethod
    def GetInstance(cls) -> 'PyInterface':
        if cls._instance is None:
            cls._instance = cls()
        return cls._instance

    # def setProjects(self, project: Any) -> None: ...

    def OSIS_Run(self, strCmd) -> Tuple[bool, str]:
        return True, ""

    def OSIS_Replot(self) -> Tuple[bool, str]:
        return True, ""
    
    def OSIS_Clear(self) -> Tuple[bool, str]:
        return True, ""

    def OSIS_Solve(self) -> Tuple[bool, str]:
        return True, ""
    
    def OSIS_Acel(self, dG: float) -> tuple[bool, str]:
        return True, ""

    def OSIS_CalcTendon(self, bFlag: int) -> tuple[bool, str]:
        return True, ""

    def OSIS_CalcConForce(self, bFlag: int) -> tuple[bool, str]:
        return True, ""

    def OSIS_CalcShrink(self, bFlag: int) -> tuple[bool, str]:
        return True, ""

    def OSIS_CalcCreep(self, bFlag: int) -> tuple[bool, str]:
        return True, ""

    def OSIS_CalcShear(self, bFlag: int) -> tuple[bool, str]:
        return True, ""

    def OSIS_CalcRlx(self, bFlag: int) -> tuple[bool, str]:
        return True, ""

    def OSIS_ModLocCoor(self, bFlag: int) -> tuple[bool, str]:
        return True, ""

    def OSIS_IncTendon(self, bFlag: int) -> tuple[bool, str]:
        return True, ""

    def OSIS_NL(self, bGeom: int, bLink: int) -> tuple[bool, str]:
        return True, ""

    def OSIS_LnSrch(self, bFlag: int) -> tuple[bool, str]:
        return True, ""

    def OSIS_AutoTs(self, bFlag: int) -> tuple[bool, str]:
        return True, ""

    def OSIS_ModOpt(self, nMod: int) -> tuple[bool, str]:
        return True, ""

    def OSIS_CrpShrk(self, nNO: int, strName: str, dAvgHumidity: float, nBirthTime: int, dTypeCoeff: float, nBirthByShrinking: int) -> Tuple[bool, str]: 
        return True, ""

    def OSIS_Material(self, nMat: int, strName: str, eMaterialType: str, eCode: str, eGrade: str, nCrepShrk: int, dDmp: float, params: dict={}) -> Tuple[bool, str]:    # 需要修改
        return True, ""
    
    def OSIS_MaterialDel(self, nMat: int) -> Tuple[bool, str]:
        return True, ""
    
    def OSIS_MaterialMod(self, nOld: int, nNew: int) -> Tuple[bool, str]:
        return True, ""

    def OSIS_Section(self, nSec: int, strName: str, eSectionType: str, params: Dict[str, Any]) -> Tuple[bool, str]:
        return True, ""

    def OSIS_SectionOffset(self, nSec: int, offsetTypeY: str, dOffsetValueY: float, offsetTypeZ: str, dOffsetValueZ: float) -> Tuple[bool, str]:
        return True, ""

    def OSIS_SectionMesh(self, nSec: int, bMeshMethod: int, dMeshSize: float) -> Tuple[bool, str]:
        return True, ""
    
    def OSIS_SectionDel(self, nSec: int) -> Tuple[bool, str]:
        return True, ""
    
    def OSIS_SectionMod(self, nOld: int, nNew: int) -> Tuple[bool, str]:
        return True, ""

    def OSIS_Node(self, nNO: int, x: float, y: float, z: float) -> Tuple[bool, str]:
        return True, ""

    def OSIS_NodeDel(self, nNO: int) -> Tuple[bool, str]:
        return True, ""

    def OSIS_NodeMod(self, nOld: int, nNew: int) -> Tuple[bool, str]:
        return True, ""

    def OSIS_Element(self, nEle: int, eElementType: str, params: Dict[str, Any]) -> Tuple[bool, str]:
        return True, ""
    
    def OSIS_ElementDel(self, nEle: int) -> Tuple[bool, str]:
        return True, ""

    def OSIS_ElementMod(self, nOld: int, nNew: int) -> Tuple[bool, str]:
        return True, ""
    
    def OSIS_Boundary(self, nBd: int, eBoundaryType: str, params: Dict[str, Any]) -> Tuple[bool, str]:
        return True, ""

    def OSIS_AsgnBd(self, nBd: int, eOP: str, nodeNOs: list) -> Tuple[bool, str]:
        return True, ""
    
    def OSIS_Load(self, eLoadType: str, strLCName: str, params: Dict[str, Any]) -> Tuple[bool, str]:
        return True, ""

    def OSIS_LoadCase(self, strName: str, eLoadCaseType: str, dScalar: float, strPrompt: str) -> Tuple[bool, str]:
        return True, ""
    
    def OSIS_LiveGrade(self, strName: str, eCode: str, eLiveLoadType: str, params: Dict[str, Any]) -> Tuple[bool, str]:
        return True, ""

    def OSIS_ElemForce(str, strLCName: str, eDataItem: str, eElementType: str) -> Tuple[bool, str, Any]:
        return True, "", None
    