from typing import Any, Dict, Literal
from ..core import OSISEngine


def osis_section(nSec: int, strName: str, eSectionType: Literal["RECT", "ISHAPE", "TSHAPE", "CIRCLE", "LSHAPE"], params: Dict[str, Any]):
    """创建或修改截面
    
    根据指定的截面类型和参数创建或修改截面定义。重复使用截面编号会修改现有截面。
    
    Args:
        nSec: 截面编号，从1开始编号，所有类型的截面均使用同一编号序列
        strName: 截面名称，默认为"截面1"
        eSectionType: 截面类型，可选值：
            - RECT: 矩形截面
            - ISHAPE: 工字形截面  
            - TSHAPE: T形截面
            - CIRCLE: 圆形截面
            - LSHAPE: L形截面
        kwargs: 截面参数字典，具体参数根据eSectionType不同而变化，详细参数说明请查看函数完整文档
    
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
        >>> result = osis_section(2, "截面2 (工字形)", "ISHAPE", {
        ...     "H": 0.4, "Bt": 0.2, "Bb": 0.2,
        ...     "Tt": 0.016, "Tb": 0.016, "Tw": 0.01
        ... })
        >>> print(result)
        (True, "")
    
    """
    e = OSISEngine.GetInstance()
    return e.OSIS_Section(nSec, strName, eSectionType, params)


def section_Lshape(nSec: int, strName: str, Dir: Literal[0, 1], H: float, B: float, Tf1: float, Tf2: float):
    """创建或修改L形截面(LShape)。

    Args:
        nSec (int): 截面编号，从1开始编号，所有类型的截面均使用同一编号序列。
        strName (str): 截面名称。
        Dir (int): L形截面方向：
            * 0: 左下向
            * 1: 左上向
        H (float): 截面总高度
        B (float): 截面总宽度
        Tf1 (float): 竖肢厚度
        Tf2 (float): 横肢厚度

    Returns:
        tuple (bool, str): 返回一个元组，包含：
            - bool: 操作是否成功
            - str: 失败原因（如果操作失败）

    Examples:
        >>> # 创建左下向L形截面
        >>> result = section_Lshape(1, "截面1 (左下向L形)", 0, 6, 4, 1.2, 1.2)
        >>> print(result)
        (True, "")

    """ 
    params = locals()
    return osis_section(nSec, strName, "LSHAPE", params)

def section_circle(nSec: int, strName: str, CircleType: Literal["Hollow", "Solid"], D: float,Tw: float):
    """创建或修改圆形截面(Circle)。

    Args:
        nSec (int): 截面编号，从1开始编号，所有类型的截面均使用同一编号序列。
        strName (str): 截面名称。
        CircleType (str): 截面类型：
            * Hollow: 空腹截面
            * Solid: 实腹截面
        D (float): 圆形截面直径
        Tw (float): 空腹截面的壁厚
            仅当CircleType为"Hollow"时需要指定。

    Returns:
        tuple (bool, str): 返回一个元组，包含：
            - bool: 操作是否成功
            - str: 失败原因（如果操作失败）

    Examples:
        >>> # 创建实心圆形截面
        >>> result = section_circle(1, "截面1 (实心圆形)", "Solid", 6.0, 0.0)
        >>> print(result)
        (True, "")
        
    """
    param = locals()
    return osis_section(nSec, strName, "CIRCLE", param)

def section_Tshape(nSec: int, strName: str, Dir: Literal[0, 1], H: float, B: float, Tf: float, Tw: float):
    """创建或修改T形截面(TShape)。

    Args:
        nSec (int): 截面编号，从1开始编号，所有类型的截面均使用同一编号序列。
        strName (str): 截面名称
        Dir (int): 截面方向：
            * 0: T形
            * 1: 倒T形
        H (float): 截面总高度
        B (float): 翼缘宽度
        Tf (float): 翼缘厚度
        Tw (float): 腹板厚度

    Returns:
        tuple (bool, str): 返回一个元组，包含：
            - bool: 操作是否成功
            - str: 失败原因（如果操作失败）

    Examples:
        >>> # 创建基本T形截面
        >>> result = section_Tshape(1, "截面1 (T形)", 0, 2, 12, 0.2, 0.5)
        >>> print(result)
        (True, "")

    """
    params = locals()
    return osis_section(nSec, strName, "TSHAPE", params)

def section_Ishape(nSec: int, strName: str, H: float, Bt: float, Bb: float, Tt: float, Tb: float, Tw: float):
    """创建或修改I形截面（工字形截面）(IShape)。

    Args:
        nSec (int): 截面编号，从1开始编号，所有类型的截面均使用同一编号序列。
        strName (str): 截面名称
        H (float): 截面总高度
        Bt (float): 上翼缘宽度
        Bb (float): 下翼缘宽度
        Tt (float): 上翼缘厚度
        Tb (float): 下翼缘厚度
        Tw (float): 腹板厚度

    Returns:
        tuple (bool, str): 返回一个元组，包含：
            - bool: 操作是否成功
            - str: 失败原因（如果操作失败）

    Examples:
        >>> # 创建基本工字形截面
        >>> result = section_Ishape(1, "截面1 (工字形)", 2.0, 12.0, 12.0, 0.5, 0.5, 1.0)
        >>> print(result)
        (True, "")
    """
    params = locals()
    return osis_section(nSec, strName, "ISHAPE", params)

def section_rect(nSec: int, strName: str, TransitionType: Literal["Chamfer", "Fillet"], SecType: Literal["Solid", "Hollow"],
        B: float, H: float, xo1: float, yo1: float, R: float, t1: float, t2: float, xi1: float, yi1: float, HasDiaphragm: bool, tw: float, xi2: float, yi2: float,
        HasGroove: bool, b1: float, b2: float, h: float):
    """创建或修改矩形截面(RECT)。

    Args:
        nSec (int): 截面编号，从1开始编号，所有类型的截面均使用同一编号序列。
        strName (str): 截面名称。
        TransitionType (str): 倒角类型，可选值：
            * Chamfer: 斜倒角
            * Fillet: 圆倒角
        SecType (str): 截面类型，可选值：
            * Solid: 实腹截面
            * Hollow: 空腹截面
        B (float): 截面宽度
        H (float): 截面高度
        xo1 (float): 斜倒角宽度
        yo1 (float): 斜倒角高度
        R (float): 圆倒角半径
        t1 (float): 壁厚1
        t2 (float): 壁厚2
        xi1 (float): 内倒角宽度
        yi1 (float): 内倒角高度
        HasDiaphragm (bool): 隔板标志：
            * 0: 无隔板
            * 1: 有隔板
        tw (float): 隔板厚度
        xi2 (float): 隔板倒角宽度
        yi2 (float): 隔板倒角高度
        HasGroove (bool): 凹槽标志：
            * 0: 无凹槽
            * 1: 有凹槽
        b1 (float): 凹槽上口宽度
        b2 (float): 凹槽下口宽度
        h (float): 凹槽深度

    Returns:
        tuple (bool, str): 返回一个元组，包含：
            - bool: 操作是否成功
            - str: 失败原因（如果操作失败）

    Examples:
        >>> # 创建基本实心矩形截面
        >>> result = section_rect(1, "截面1 (矩形)", "Fillet", "Solid", 12.0, 2.0, 1.0, 0.5, 0.0, 1.0, 1.0, 0.5, 0.25, 0, 1.0, 0.5, 0.25, 0, 1.2, 0.8, 0.2 )
        >>> print(result)
        (True, "")

    Note:
        - 单位：所有尺寸参数单位均为米(m)
        - 重复使用截面编号会修改现有截面
    """
    params = locals()
    return osis_section(nSec, strName, "RECT", params)


def osis_section_offset(nSec: int, offsetTypeY: str, dOffsetValueY: float, offsetTypeZ: str, dOffsetValueZ: float):
    """设置截面偏移。

    Args:
        nSec (int): 截面编号。
        offsetTypeY (str): Y方向偏移类型，可选值：
            * Left: 左对齐
            * Middle: 居中对齐
            * Right: 右对齐
            * Manual: 手动指定偏移值
        dOffsetValueY (float): Y方向偏移值（单位：m）。
            仅当offsetTypeY为"Manual"时生效。
        offsetTypeZ (str): Z方向偏移类型，可选值：
            * Top: 顶部对齐
            * Center: 居中对齐
            * Bottom: 底部对齐
            * Manual: 手动指定偏移值
        dOffsetValueZ (float): Z方向偏移值（单位：m）。
            仅当offsetTypeZ为"Manual"时生效。

    Returns:
        tuple (bool, str): 返回一个元组，包含：
            - bool: 操作是否成功
            - str: 失败原因（如果操作失败）

    Examples:
        >>> # 设置截面Y方向左对齐，Z方向底部对齐
        >>> result = section_offset(1, "Left", 0.0, "Bottom", 0.0)
        >>> print(result)
        (True, "")
        
        >>> # 设置截面Y方向手动偏移0.1m，Z方向居中对齐
        >>> result = section_offset(1, "Manual", 0.1, "Center", 0.0)
        >>> print(result)
        (True, "")

    Note:
        - 单位：所有尺寸参数单位均为米(m)
        - 重复使用截面编号会修改现有截面
    """
    e = OSISEngine.GetInstance()
    return e.OSIS_SectionOffset(nSec, offsetTypeY, dOffsetValueY, offsetTypeZ, dOffsetValueZ)

def osis_section_mesh(nSec: int, bMeshMethod: int, dMeshSize: float):
    """设置截面网格。

    Args:
        nSec (int): 截面编号。
        offsetTypeY (str): Y方向偏移类型，可选值：
            * Left: 左对齐
            * Middle: 居中对齐
            * Right: 右对齐
            * Manual: 手动指定偏移值
        dOffsetValueY (float): Y方向偏移值（单位：m）。
            仅当offsetTypeY为"Manual"时生效。
        offsetTypeZ (str): Z方向偏移类型，可选值：
            * Top: 顶部对齐
            * Center: 居中对齐
            * Bottom: 底部对齐
            * Manual: 手动指定偏移值
        dOffsetValueZ (float): Z方向偏移值（单位：m）。
            仅当offsetTypeZ为"Manual"时生效。

    Returns:
        tuple (bool, str): 返回一个元组，包含：
            - bool: 操作是否成功
            - str: 失败原因（如果操作失败）

    Examples:
        >>> # 设置截面Y方向左对齐，Z方向底部对齐
        >>> result = section_offset(1, "Left", 0.0, "Bottom", 0.0)
        >>> print(result)
        (True, "")
        
        >>> # 设置截面Y方向手动偏移0.1m，Z方向居中对齐
        >>> result = section_offset(1, "Manual", 0.1, "Center", 0.0)
        >>> print(result)
        (True, "")
        
    Note:
        - 单位：所有尺寸参数单位均为米(m)
        - 重复使用截面编号会修改现有截面
    """
    e = OSISEngine.GetInstance()
    return e.OSIS_SectionMesh(nSec, bMeshMethod, dMeshSize)

def osis_section_del(nSec: int):
    """删除截面

    Args:
        nSec (int): 截面编号。

    Returns:
        tuple (bool, str):
            - bool: 操作是否成功
            - str: 失败原因（如果操作失败）
    """
    e = OSISEngine.GetInstance()
    return e.OSIS_SectionDel(nSec)

def osis_section_mod(nOld: int, nNew: int):
    """修改截面

    Args:
        nOld (int): 旧截面编号
        nNew (int): 新截面编号

    Returns:
        tuple (bool, str):
            - bool: 操作是否成功
            - str: 失败原因（如果操作失败）
    """
    e = OSISEngine.GetInstance()
    return e.OSIS_SectionMod(nOld, nNew)