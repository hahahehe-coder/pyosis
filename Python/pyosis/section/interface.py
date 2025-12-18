from typing import Any, Dict
from ..core import OSISEngine
def osis_section(nSec: int, strName: str, eSectionType: str, kwargs: Dict[str, Any]):
    """创建或修改截面
    
    根据指定的截面类型和参数创建或修改截面定义。
    
    Args:
        nSec: 截面编号，从1开始编号，所有类型的截面均使用同一编号序列
        strName: 截面名称，默认为"截面1"
        eSectionType: 截面类型，可选值：
            - RECT: 矩形截面
            - IShape: 工字形截面  
            - TShape: T形截面
            - Circle: 圆形截面
            - LShape: L形截面
        kwargs: 截面参数字典，具体参数根据eSectionType不同而变化：
            * RECT类型: 需要传递矩形截面相关参数（B、H等）
            * IShape类型: 需要传递工字形截面参数（H、Bt、Bb等）
            * TShape类型: 需要传递T形截面参数（Dir、H、B等）
            * Circle类型: 需要传递圆形截面参数（CircleType、D等）
            * LShape类型: 需要传递L形截面参数（Dir、H、B等）
            详细参数说明请查看函数完整文档
    
    Returns:
        tuple (bool, str): 返回一个元组，包含：
            - bool: 操作是否成功
            - str: 失败原因（如果操作失败）
    
    Examples:
        >>> # 创建矩形截面
        >>> result = osis_section(1, "截面1 (矩形)", "RECT", {
        ...     "TransitionType": "Fillet", "SecType": "Solid",
        ...     "B": 0.6, "H": 0.3
        ... })
        >>> print(result)
        (True, "")
        
        >>> # 创建工字形截面  
        >>> result = osis_section(2, "截面2 (工字形)", "IShape", {
        ...     "H": 0.4, "Bt": 0.2, "Bb": 0.2,
        ...     "Tt": 0.016, "Tb": 0.016, "Tw": 0.01
        ... })
        >>> print(result)
        (True, "")
    
    Note:
        - 单位：所有尺寸参数单位均为米(m)
        - 详细参数列表请查看函数完整文档字符串
        - 重复使用截面编号会修改现有截面
    """
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