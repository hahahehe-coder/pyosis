from typing import Any, Dict, Tuple

class PyInterface:
    _instance = None
    
    @classmethod
    def GetInstance(cls) -> 'PyInterface':
        """获取单例实例"""
        if cls._instance is None:
            cls._instance = cls()
        return cls._instance
    # @staticmethod
    # def GetInstance() -> 'PyInterface': 
    #     return PyInterface()

    # def setProjects(self, project: Any) -> None: ...

    def OSIS_CrpShrk(self, nNO: int, strName: str, dAvgHumidity: float, nBirthTime: int, dTypeCoeff: float, nBirthByShrinking: int) -> Tuple[bool, str]: 
        return True, ""

    def OSIS_Material(self, nMat: int, strName: str, strMaterialType: str, code: str, grade: str, nCrepShrk: int, dDmp: float) -> Tuple[bool, str]:
        return True, ""
    
    def OSIS_MaterialDel(self, nMat: int) -> Tuple[bool, str]:
        return True, ""
    
    def OSIS_MaterialMod(self, nOld: int, nNew: int) -> Tuple[bool, str]:
        return True, ""

    def OSIS_Section(self, nSec: int, strName: str, strSectionType: str, params: Dict[str, Any]) -> Tuple[bool, str]:
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

    def OSIS_Element(self, nEle: int, strElementType: str, params: Dict[str, Any]) -> Tuple[bool, str]:
        return True, ""
    
    def OSIS_ElementDel(self, nEle: int) -> Tuple[bool, str]:
        return True, ""

    def OSIS_ElementMod(self, nOld: int, nNew: int) -> Tuple[bool, str]:
        return True, ""