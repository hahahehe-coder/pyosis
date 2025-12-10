from langchain.tools import tool
from typing import Tuple
from ...general import *
from ...material import *
from ...section import *
from ...node import *
from ...element import *

"""
智能体工具函数定义注意事项：

所有函数参数尽量写的简单易懂
"""

cmd_map = {         # 工具函数与命令流对照表
    "replot": "Replot",
    "creep_shrink": "CrpShrk",
    "material": "Material",
    "material_del": "MaterialDel",
    "material_mod": "MaterialMod",

    "section": "Section",
    "section_l_shape": "Section",
    "section_circle": "Section",
    "section_t_shape": "Section",
    "section_i_shape": "Section",
    "section_rect": "Section",
    "section_offset": "SectionOffset",
    "section_mesh": "SectionMesh",
    "section_del": "SectionDel",
    "section_mod": "SectionMod",

    "node": "Node",
    "node_del": "NodeDel",
    "node_mod": "NodeMod",

    "element": "Element",
    "element_beam3d": "Element",        # 所有单元类型都对应Element命令
    "element_truss": "Element",
    "element_spring": "Element",
    "element_cable": "Element",
    "element_del": "ElementDel",
    "element_mod": "ElementMod"
}

args_default = {    # 工具函数默认调用参数
    "replot": {},
    "creep_shrink": {
        "nNO": 1,
        "strName": '收缩徐变',
        "dAvgHumidity": 70.0,
        "nBirthTime": 7,
        "dTypeCoeff": 5.0,
        "nBirthByShrinking": 3
    },
    "material": {
        "nMat": 1,
        "strName": "C50",
        "strMaterialType": "CONC",
        "code": "JTG3362_2018",
        "grade": "C50",
        "nCrepShrk": 1,
        "dDmp": 0.05,
    },
    "section": {
        "nSec": 1,
        "strName": "矩形截面",
        "eSectionType": "RECT",
        "params": {
            "TransitionType": "Fillet",
            "SecType": "Solid",
            "B": 12.0000,
            "H": 2.0000,
            "xo1": 1.0000,
            "yo1": 0.5000,
            "R": 0.0000,
            "t1": 1.0000,
            "t2": 1.0000,
            "xi1": 0.5000,
            "yi1": 0.2500,
            "HasDiaphragm": 0,
            "tw": 1.0000,
            "xi2": 0.5000,
            "yi2": 0.2500,
            "HasGroove": 0,
            "b1": 1.2000,
            "b2": 0.8000,
            "h": 0.2000
        }
    },
    "section_l_shape": {
        "nSec": 1,
        "strName": "L形截面1",
        "eSectionType": "LShape",
        "params": {
            "Dir": 0,
            "H": 3.0,
            "B": 6.0,
            "Tf1": 0.2,
            "Tf2": 0.2,
        }
    },
    "section_circle": {
        "nSec": 1,
        "strName": "圆形截面1",
        "eSectionType": "Circle",
        "params": {
            "CircleType": "Hollow",
            "D": 6.0,
            "Tw": 0.5
        }
    },
    "section_t_shape": {
        "nSec": 1,
        "strName": "T形截面1",
        "eSectionType": "TShape",
        "params": {
            "Dir": 0,
            "H": 3.0,
            "B": 6.0,
            "Tf": 0.2,
            "Tw": 0.5,
        }
    },
    "section_i_shape": {
        "nSec": 1,
        "strName": "I形截面1",
        "eSectionType": "IShape",
        "params": {
            "H": 3.0,
            "Bt": 6.0,
            "Bb": 6.0,
            "Tt": 0.5,
            "Tb": 0.5,
            "Tw": 1.0,
        }
    },

    "section_rect": {
        "nSec": 1,
        "strName": "矩形截面1",
        "eSectionType": "RECT",
        "params": {
            "TransitionType": "Fillet",
            "SecType": "Solid",
            "B": 12.0000,
            "H": 2.0000,
            "xo1": 1.0000,
            "yo1": 0.5000,
            "R": 0.0000,
            "t1": 1.0000,
            "t2": 1.0000,
            "xi1": 0.5000,
            "yi1": 0.2500,
            "HasDiaphragm": 0,
            "tw": 1.0000,
            "xi2": 0.5000,
            "yi2": 0.2500,
            "HasGroove": 0,
            "b1": 1.2000,
            "b2": 0.8000,
            "h": 0.2000
        }
    },
    "section_offset": {
        "nSec": 1,
        "offsetTypeY": "Middle",
        "dOffsetValueY": 0.0,
        "offsetTypeZ": "Center",
        "dOffsetValueZ": 0.0
    },
    "section_mesh": {
        "nSec": 1,
        "bMeshMethod": 1,
        "dMeshSize": 0.1
    },
    "section_del": {
        "nSec": 1
    },
    "section_mod": {
        "nOld": 1,
        "nNew": 2
    },

    "node": {
        "nNO": 1,
        "x": 0.0,
        "y": 0.0,
        "z": 0.0
    },
    "node_del": {
        "nNO": 1
    },
    "node_mod": {
        "nOld": 1,
        "nNew": 2
    },
    "element": {
        "nEle": 1, 
        "eElementType": "BEAM3D", 
        "params": {
            "nNode1": 1,
            "nNode2": 2,
            "nMat": 1,
            "nSec1": 1,
            "nSec2": 1,
            "nYTrans": 1,
            "nZTrans": 1,
            "dStrain": 0.00,
            "bFlag": 0,
            "dTheta": 0.00,
            "bWarping": 0
        }
    },
    "element_beam3d": {
        "nEle": 1, 
        "eElementType": "BEAM3D",   # 写全，便于转命令流
        "params": {
            "nNode1": 1,
            "nNode2": 2,
            "nMat": 1,
            "nSec1": 1,
            "nSec2": 1,
            "nYTrans": 1,
            "nZTrans": 1,
            "dStrain": 0.00,
            "bFlag": 0,
            "dTheta": 0.00,
            "bWarping": 0
        }
    },
    "element_truss": {
        "nEle": 1, 
        "eElementType": "Truss", 
        "params": {
            "nNode1": 1,
            "nNode2": 2,
            "nMat": 1,
            "nSec1": 1,
            "nSec2": 1,
            "dStrain": 0.00
        }
    },
    "element_spring": {
        "nEle": 1, 
        "eElementType": "Spring", 
        "params": {
            "nNode1": 1,
            "nNode2": 2,
            "bLinear": 1,
            "dx": 0.1,
            "dy": 0.1,
            "dz": 0.1,
            "rx": 0.1,
            "ry": 0.1,
            "rz": 0.1,
            "dBeta": 0
        }
    },
    "element_cable": {
        "nEle": 1, 
        "eElementType": "Cable", 
        "params": {
            "nNode1": 1,
            "nNode2": 2,
            "nMat": 1,
            "nSec": 1,
            "eMethod": "IF",
            "dPara": 10.0
        }
    },

    "element_del": {
        "nEle": 1
    },
    "element_mod": {
        "nOld": 1,
        "nNew": 2
    }
}

def deep_merge(source, destination):
    """递归合并字典，保持源字典的键顺序，并用目标字典的值覆盖相同键的值"""
    result = {}
    
    # 首先添加源字典的所有键值对
    for key, value in source.items():
        if key in destination:
            # 如果键在目标字典中也存在
            if isinstance(value, dict) and isinstance(destination[key], dict):
                # 如果两个值都是字典，递归合并
                result[key] = deep_merge(value, destination[key])
            else:
                # 否则使用目标字典的值（覆盖）
                result[key] = destination[key]
        else:
            # 如果键只在源字典中存在，使用源字典的值
            result[key] = value
    
    # 然后添加目标字典中独有的键值对
    for key, value in destination.items():
        if key not in source:
            result[key] = value
    
    return result

def py_to_cmd(py_func_name: str, py_params: dict) -> str:       # python调用转换为命令流
    cmd = cmd_map[py_func_name]
    args = deep_merge(args_default[py_func_name], py_params)
    for key, value in args.items():
        if isinstance(value, dict):    # 可能还是字典
            # merged_dict = {**params_default[tool_call['name']], **value}
            for k, v in value.items():
                cmd += f",{v}"
        else:
            cmd += f",{value}"
    cmd += ";"
    return cmd

def pack_func_call(func_name: str, py_params: dict) -> str:
    args = deep_merge(args_default[func_name], py_params)       # 智能体可能不会把所有参数填上，所以需要默认参数
    return f"name: {func_name}, args: {args}"

@tool
def replot():
    """刷新界面
    
    Returns:
        tuple (bool, str):
            - bool: 操作是否成功
            - str: 失败原因（如果操作失败）
    """
    return osis_replot()

@tool
def creep_shrink(nNO: int = 1, strName: str = '收缩徐变', dAvgHumidity: float = 70.0, nBirthTime: int = 7, dTypeCoeff: float = 5.0, nBirthByShrinking: int = 3) -> Tuple[bool, str]:
    """创建或修改收缩徐变特性

    Args:
        nNO (int): 收缩徐变特性编号
        strName (str): 特性名称
        dAvgHumidity (float): 年平均湿度（百分比）
        nBirthTime (int): 混凝土龄期（天）
        dTypeCoeff (float): 水泥种类系数
        nBirthByShrinking (int): 收缩开始时的混凝土龄期（天数）

    Returns:
        tuple (bool, str):
            - bool: 操作是否成功
            - str: 失败原因（如果操作失败）
    """
    return osis_creep_shrink(nNO, strName, dAvgHumidity, nBirthTime, dTypeCoeff, nBirthByShrinking)

@tool
def material(nMat: int = 1, strName: str = "材料", eMaterialType: str = "CONC", eCode: str = "JTG3362_2018", eGrade: str = "C50", nCrepShrk: int = 1, dDmp: float = 0.05) -> Tuple[bool, str]:
    """创建或修改材料。

    Args:
        nMat (int): 材料编号
        strName (str): 材料名称
        eMaterialType (str): 材料类型，可选值：
            * CONC: 混凝土
            * STEEL: 钢材
            * PRESTRESSED: 预应力材料
            * REBAR: 普通钢筋
        eCode (str): 材料标准代码，可选值：
            * 钢材: JTGD64_2015
            * 其他材料: JTG3362_2018, JTGD62_2004
        eGrade (str): 材料等级牌号，根据材料类型可选：
            * 混凝土: C15, C20, C25, C30, C35, C40, C45, C50, C55, C60, C65, C70, C75, C80
            * 钢材: Q235, Q345, Q390, Q420
            * 预应力材料:
                ** JTG3362_2018: Strand1720, Strand1860, Strand1960, Wire1470, Wire1570, Wire1770, Wire1860, Rebar785, Rebar930, Rebar1080
                ** JTGD62_2004: Strand1860, Wire1670, Wire1770, Rebar785, Rebar930
            * 普通钢筋:
                ** JTG3362_2018: HPB300, HRB400, HRBF400, RRB400, HRB500
                ** JTGD62_2004: R235, HRB335, HRB400, KL400
        nCrepShrk (int): 收缩徐变特性编号（混凝土材料需要，其他材料设置为-1）
        dDmp (float): 材料阻尼比

    Returns:
        tuple (bool, str):
            - bool: 操作是否成功
            - str: 失败原因（如果操作失败）

    Examples:
        >>> result = osis_material(1, "C30", "CONC", "JTG3362_2018", "C30", 1, 0.05)
        >>> print(result)
        (True, "")
    """
    # if material == "PRESTRESSED":
    #     if code == "JTG3362_2018":
    #         return False, "Rebar785等级的预应力材料只能用于JTGD62_2004标准"
    return osis_material(nMat, strName, eMaterialType, eCode, eGrade, nCrepShrk, dDmp)

@tool
def material_del(nMat: int = 1) -> Tuple[bool, str]:
    """删除材料

    Args:
        nMat (int): 材料编号

    Returns:
        tuple (bool, str):
            - bool: 操作是否成功
            - str: 失败原因（如果操作失败）
    """
    return osis_material_del(nMat)

@tool
def material_mod(nOld: int = 1, nNew: int = 2) -> Tuple[bool, str]:
    """修改材料

    Args:
        nOld (int): 旧材料编号
        nNew (int): 新材料编号

    Returns:
        tuple (bool, str):
            - bool: 操作是否成功
            - str: 失败原因（如果操作失败）
    """
    return osis_material_mod(nOld, nNew)

@tool
def section(nSec: int = 1, strName: str = "截面1", eSectionType: str = "RECT", 
    params: dict = {
        "TransitionType": "Fillet",
        "SecType": "Solid",
        "B": 6.0000,
        "H": 3.0000,
        "xo1": 1.0000,
        "yo1": 0.5000,
        "R": 0.0000,
        "t1": 1.0000,
        "t2": 1.0000,
        "xi1": 0.5000,
        "yi1": 0.2500,
        "HasDiaphragm": 0,
        "tw": 1.0000,
        "xi2": 0.5000,
        "yi2": 0.2500,
        "HasGroove": 0,
        "b1": 1.2000,
        "b2": 0.8000,
        "h": 0.2000}
    ) -> Tuple[bool, str]:
    """创建或修改截面
    
    根据指定的截面类型和参数创建或修改截面定义。
    
    Args:
        nSec: 截面编号，从1开始编号，所有类型的截面均使用同一编号序列
        strName: 截面名称，默认为"截面1"
        eSectionType: 截面类型，可选值：
            * RECT: 矩形截面
            * IShape: 工字形截面  
            * TShape: T形截面
            * Circle: 圆形截面
            * LShape: L形截面
        params: 截面参数字典，具体参数根据eSectionType不同而变化：
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
    default_params = args_default["section"]["params"]
    return osis_section(nSec, strName, eSectionType, {**default_params, **params})

@tool
def section_l_shape(nSec: int = 1, strName: str = "截面1 (L形)", 
    params: dict = {
        "Dir": 0,
        "H": 4,
        "B": 6,
        "Tf1": 1.2,
        "Tf2": 1.2,
    }) -> Tuple[bool, str]:
    """创建或修改L形截面(LShape)。

    Args:
        nSec (int): 截面编号，从1开始编号，所有类型的截面均使用同一编号序列。
        strName (str): 截面名称。
        params (dict): 截面参数字典，包含以下键值对：
            - Dir (int): L形截面方向：
                * 0: 左下向
                * 1: 左上向
            - H (float): 截面总高度
            - B (float): 截面总宽度
            - Tf1 (float): 竖肢厚度
            - Tf2 (float): 横肢厚度

    Returns:
        tuple (bool, str): 返回一个元组，包含：
            - bool: 操作是否成功
            - str: 失败原因（如果操作失败）

    Examples:
        >>> # 创建左下向L形截面
        >>> result = section_l_shape(1, ""截面1 (左下向L形)", {
        ...     "Dir": 0,
        ...     "H": 6,
        ...     "B": 4,
        ...     "Tf1": 1.2,
        ...     "Tf2": 1.2
        ... })
        >>> print(result)
        (True, "")
        
        >>> # 创建左上向L形截面
        >>> result = section_l_shape(2, ""截面2 (左上向L形)", {
        ...     "Dir": 1,
        ...     "H": 6,
        ...     "B": 4,
        ...     "Tf1": 1.2,
        ...     "Tf2": 1.2
        ... })
        >>> print(result)
        (True, "")

    Note:
        - 单位：所有尺寸参数单位均为米(m)
        - 重复使用截面编号会修改现有截面
    """
    eSectionType = "LShape"
    default_params = args_default["section_l_shape"]["params"]
    return osis_section(nSec, strName, eSectionType, {**default_params, **params})

@tool
def section_circle(nSec: int = 1, strName: str = "截面1 (圆形)", 
    params: dict = {
        "CircleType": "Hollow",
        "D": 6.0,
        "Tw": 0.5
    }) -> Tuple[bool, str]:
    """创建或修改圆形截面(Circle)。

    Args:
        nSec (int): 截面编号，从1开始编号，所有类型的截面均使用同一编号序列。
        strName (str): 截面名称。
        params (dict): 截面参数字典，包含以下键值对：
            - CircleType (str): 截面类型：
                * Hollow: 空腹截面
                * Solid: 实腹截面
            - D (float): 圆形截面直径
            - Tw (float): 空腹截面的壁厚
                仅当CircleType为"Hollow"时需要指定。

    Returns:
        tuple (bool, str): 返回一个元组，包含：
            - bool: 操作是否成功
            - str: 失败原因（如果操作失败）

    Examples:
        >>> # 创建实心圆形截面
        >>> result = section_circle(1, "截面1 (实心圆形)", {
        ...     "CircleType": "Solid",
        ...     "D": 6.0,
        ...     "Tw": 0.0
        ... })
        >>> print(result)
        (True, "")
        
        >>> # 创建空腹圆形截面
        >>> result = section_circle(2, "截面2 (空心圆形)", {
        ...     "CircleType": "Hollow",
        ...     "D": 6.0,
        ...     "Tw": 0.5
        ... })
        >>> print(result)
        (True, "")

    Note:
        - 单位：所有尺寸参数单位均为米(m)
        - 重复使用截面编号会修改现有截面
    """
    eSectionType = "Circle"
    default_params = args_default["section_circle"]["params"]
    return osis_section(nSec, strName, eSectionType, {**default_params, **params})

@tool
def section_t_shape(nSec: int = 1, strName: str = "截面1 (T形)",
    params: dict = {
        "Dir": 0,
        "H": 2,
        "B": 12,
        "Tf": 0.2,
        "Tw": 0.5,

    }) -> Tuple[bool, str]:
    """创建或修改T形截面(TShape)。

    Args:
        nSec (int): 截面编号，从1开始编号，所有类型的截面均使用同一编号序列。
        strName (str): 截面名称
        params (dict): 截面参数字典，包含以下键值对：
            - Dir (int): 截面方向：
                * 0: T形
                * 1: 倒T形
            - H (float): 截面总高度
            - B (float): 翼缘宽度
            - Tf (float): 翼缘厚度
            - Tw (float): 腹板厚度

    Returns:
        tuple (bool, str): 返回一个元组，包含：
            - bool: 操作是否成功
            - str: 失败原因（如果操作失败）

    Examples:
        >>> # 创建基本T形截面
        >>> result = section_tshape(1, "截面1 (T形)", {
        ...     "Dir": 0,
        ...     "H": 2,
        ...     "B": 12,
        ...     "Tf": 0.2,
        ...     "Tw": 0.5
        ... })
        >>> print(result)
        (True, "")
        
    Note:
        - 单位：所有尺寸参数单位均为米(m)
        - 重复使用截面编号会修改现有截面
    """

    eSectionType = "TShape"
    default_params = args_default["section_t_shape"]["params"]
    return osis_section(nSec, strName, eSectionType, {**default_params, **params})

@tool
def section_i_shape(nSec: int = 1, strName: str = "截面1 (工字形)",
    params: dict = {
        "H": 2.0,
        "Bt": 12.0,
        "Bb": 12.0,
        "Tt": 0.5,
        "Tb": 0.5,
        "Tw": 1.0,
    }) -> Tuple[bool, str]:
    """创建或修改I形截面（工字形截面）(IShape)。

    Args:
        nSec (int): 截面编号，从1开始编号，所有类型的截面均使用同一编号序列。
        strName (str): 截面名称
        params (dict): 截面参数字典，包含以下键值对：
            - H (float): 截面总高度
            - Bt (float): 上翼缘宽度
            - Bb (float): 下翼缘宽度
            - Tt (float): 上翼缘厚度
            - Tb (float): 下翼缘厚度
            - Tw (float): 腹板厚度

    Returns:
        tuple (bool, str): 返回一个元组，包含：
            - bool: 操作是否成功
            - str: 失败原因（如果操作失败）

    Examples:
        >>> # 创建基本工字形截面
        >>> result = section_ishape(1, "截面1 (工字形)", {
        ...     "H": 2.0,
        ...     "Bt": 12.0,
        ...     "Bb": 12.0,
        ...     "Tt": 0.5,
        ...     "Tb": 0.5,
        ...     "Tw": 1.0
        ... })
        >>> print(result)
        (True, "")

    Note:
        - 单位：所有尺寸参数单位均为米(m)
        - 重复使用截面编号会修改现有截面
    """

    eSectionType = "IShape"
    default_params = args_default["section_i_shape"]["params"]
    return osis_section(nSec, strName, eSectionType, {**default_params, **params})

@tool
def section_rect(nSec: int = 1, strName: str = "截面1 (矩形)", 
    params: dict = {
        "TransitionType": "Fillet",
        "SecType": "Solid",
        "B": 12.0000,
        "H": 2.0000,
        "xo1": 1.0000,
        "yo1": 0.5000,
        "R": 0.0000,
        "t1": 1.0000,
        "t2": 1.0000,
        "xi1": 0.5000,
        "yi1": 0.2500,
        "HasDiaphragm": 0,
        "tw": 1.0000,
        "xi2": 0.5000,
        "yi2": 0.2500,
        "HasGroove": 0,
        "b1": 1.2000,
        "b2": 0.8000,
        "h": 0.2000}
    ) -> Tuple[bool, str]:
    """创建或修改矩形截面(RECT)。

    Args:
        nSec (int): 截面编号，从1开始编号，所有类型的截面均使用同一编号序列。
        strName (str): 截面名称。
        params (dict): 截面参数字典，包含以下键值对：
            - TransitionType (str): 倒角类型，可选值：
                * Chamfer: 斜倒角
                * Fillet: 圆倒角
            - SecType (str): 截面类型，可选值：
                * Solid: 实腹截面
                * Hollow: 空腹截面
            - B (float): 截面宽度
            - H (float): 截面高度
            - xo1 (float): 斜倒角宽度
            - yo1 (float): 斜倒角高度
            - R (float): 圆倒角半径
            - t1 (float): 壁厚1
            - t2 (float): 壁厚2
            - xi1 (float): 内倒角宽度
            - yi1 (float): 内倒角高度
            - HasDiaphragm (int): 隔板标志：
                * 0: 无隔板
                * 1: 有隔板
            - tw (float): 隔板厚度
            - xi2 (float): 隔板倒角宽度
            - yi2 (float): 隔板倒角高度
            - HasGroove (int): 凹槽标志：
                * 0: 无凹槽
                * 1: 有凹槽
            - b1 (float): 凹槽上口宽度
            - b2 (float): 凹槽下口宽度
            - h (float): 凹槽深度

    Returns:
        tuple (bool, str): 返回一个元组，包含：
            - bool: 操作是否成功
            - str: 失败原因（如果操作失败）

    Examples:
        >>> # 创建基本实心矩形截面
        >>> result = section_rect(1, "截面1 (矩形)", {
        ...     "TransitionType": "Fillet",
        ...     "SecType": "Solid",
        ...     "B": 12.0,
        ...     "H": 2.0,
        ...     "xo1": 1.0,
        ...     "yo1": 0.5,
        ...     "R": 0.0,
        ...     "t1": 1.0,
        ...     "t2": 1.0,
        ...     "xi1": 0.5,
        ...     "yi1": 0.25,
        ...     "HasDiaphragm": 0,
        ...     "tw": 1.0,
        ...     "xi2": 0.5,
        ...     "yi2": 0.25,
        ...     "HasGroove": 0,
        ...     "b1": 1.2,
        ...     "b2": 0.8,
        ...     "h": 0.2
        ... })
        >>> print(result)
        (True, "")

    Note:
        - 单位：所有尺寸参数单位均为米(m)
        - 重复使用截面编号会修改现有截面
    """
    eSectionType = "RECT"
    default_params = args_default["section_rect"]["params"]
    return osis_section(nSec, strName, eSectionType, {**default_params, **params})

@tool
def section_offset(nSec: int = 1, offsetTypeY: str = "Middle", dOffsetValueY: float = 0.0, offsetTypeZ: str = "Center", dOffsetValueZ: float = 0.0) -> Tuple[bool, str]: 
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
    return osis_section_offset(nSec, offsetTypeY, dOffsetValueY, offsetTypeZ, dOffsetValueZ)

@tool
def section_mesh(nSec: int = 1, bMeshMethod: int = 1, dMeshSize: float = 0.1) -> Tuple[bool, str]:  
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
    return osis_section_mesh(nSec, bMeshMethod, dMeshSize)

@tool
def section_del(nSec: int = 1) -> Tuple[bool, str]:
    """删除截面

    Args:
        nSec (int): 截面编号。

    Returns:
        tuple (bool, str):
            - bool: 操作是否成功
            - str: 失败原因（如果操作失败）
    """
    return osis_section_del(nSec)

@tool
def section_mod(nOld: int = 1, nNew: int = 2) -> Tuple[bool, str]:
    """修改截面

    Args:
        nOld (int): 旧截面编号
        nNew (int): 新截面编号

    Returns:
        tuple (bool, str):
            - bool: 操作是否成功
            - str: 失败原因（如果操作失败）
    """
    return osis_section_mod(nOld, nNew)

@tool
def node(nNO: int = 1, x: float = 0.0, y: float = 0.0, z: float = 0.0) -> Tuple[bool, str]:
    """创建一个节点

    Args:
        nNO (int): 节点编号，从 1 开始编号。
        x (float): 节点X坐标
        y (float): 节点Y坐标
        z (float): 节点Z坐标
        
    Returns:
        tuple (bool, str):
            - bool: 操作是否成功
            - str: 失败原因（如果操作失败）
    """
    return osis_node(nNO, x, y, z)

@tool
def node_del(nNO: int = 1) -> Tuple[bool, str]:
    """删除一个节点

    Args:
        nNO (int): 节点编号

    Returns:
        tuple (bool, str):
            - bool: 操作是否成功
            - str: 失败原因（如果操作失败）
    """
    return osis_node_del(nNO)

@tool
def node_mod(nOld: int = 1, nNew: int = 2) -> Tuple[bool, str]:
    """修改一个节点的编号。节点编号存在时，交换

    Args:
        nOld (int): 旧编号
        nNew (int): 新编号

    Returns:
        tuple (bool, str):
            - bool: 操作是否成功
            - str: 失败原因（如果操作失败）
    """
    return osis_node_mod(nOld, nNew)

@tool
def element(nEle: int = 1, eElementType: str = "BEAM3D", params: dict = {
    "nNode1": 1,
    "nNode2": 2,
    "nMat": 1,
    "nSec1": 1,
    "nSec2": 1,
    "nYTrans": 1,
    "nZTrans": 1,
    "dStrain": 0.00,
    "bFlag": 0,
    "dTheta": 0.00,
    "bWarping": 0}) -> Tuple[bool, str]:
    """创建一个单元

    Args:
        nEle (int): 单元编号。从 1 开始编号，所有类型的单元均使用同一编号序列。
        params (dict): 单元参数字典，包含以下键值对：
            - nNode1 (int): 节点1编号
            - nNode2 (int): 节点2编号
            - nMat (int): 材料编号
            - nSec1 (int): 截面1编号
            - nSec2 (int): 截面2编号
            - nYTrans (int): y轴截面变化次方，可选值：1, 2, 3, 4
            - nZTrans (int): z轴截面变化次方，可选值：1, 2, 3, 4
            - dStrain (float): 应变值。默认为 0.00
            - bFlag (int): 轴向转角定义方式：
                * 0: 使用beta角定义
                * 1: 使用关键点定义
            - dTheta (float): 轴向转角参数：
                * bFlag=0时: 轴向转角(beta角)
                * bFlag=1时: 关键点
            - bWarping (int): 翘曲效应标志：
                * 1: 考虑翘曲
                * 0: 不考虑翘曲

    Returns:
        tuple (bool, str): 返回一个元组，包含：
            - bool: 操作是否成功
            - str: 失败原因（如果操作失败）
    """
    default_params = args_default["element"]["params"]
    return osis_element(nEle, eElementType, {**default_params, **params})

@tool
def element_beam3d(nEle: int = 1, params: dict = {
    "nNode1": 1,
    "nNode2": 2,
    "nMat": 1,
    "nSec1": 1,
    "nSec2": 1,
    "nYTrans": 1,
    "nZTrans": 1,
    "dStrain": 0.00,
    "bFlag": 0,
    "dTheta": 0.00,
    "bWarping": 0}):
    """创建三维梁柱单元(BEAM3D)。

    Args:
        nEle (int): 单元编号。从 1 开始编号，所有类型的单元均使用同一编号序列。
        params (dict): 单元参数字典，包含以下键值对：
            - nNode1 (int): 节点1编号
            - nNode2 (int): 节点2编号
            - nMat (int): 材料编号
            - nSec1 (int): 截面1编号
            - nSec2 (int): 截面2编号
            - nYTrans (int): y轴截面变化次方，可选值：1, 2, 3, 4
            - nZTrans (int): z轴截面变化次方，可选值：1, 2, 3, 4
            - dStrain (float): 应变值。默认为 0.00
            - bFlag (int): 轴向转角定义方式：
                * 0: 使用beta角定义
                * 1: 使用关键点定义
            - dTheta (float): 轴向转角参数：
                * bFlag=0时: 轴向转角(beta角)
                * bFlag=1时: 关键点
            - bWarping (int): 翘曲效应标志：
                * 1: 考虑翘曲
                * 0: 不考虑翘曲

    Returns:
        tuple (bool, str): 返回一个元组，包含：
            - bool: 操作是否成功
            - str: 失败原因（如果操作失败）

    Examples:
        >>> # 创建基本三维梁柱单元
        >>> result = element_beam3d(1, {
        ...     "nNode1": 1,
        ...     "nNode2": 2,
        ...     "nMat": 1,
        ...     "nSec1": 1,
        ...     "nSec2": 1,
        ...     "nYTrans": 1,
        ...     "nZTrans": 1,
        ...     "dStrain": 0.00,
        ...     "bFlag": 0,
        ...     "dTheta": 0.00,
        ...     "bWarping": 0
        ... })
        >>> print(result)
        (True, "")
    """
    strElementType: str = "BEAM3D"
    default_params = args_default["element_beam3d"]["params"]
    return osis_element(nEle, strElementType, {**default_params, **params})

@tool
def element_truss(nEle: int = 1, params: dict = {
    "nNode1": 1,
    "nNode2": 2,
    "nMat": 1,
    "nSec1": 1,
    "nSec2": 1,
    "dStrain": 0.00
    }):
    """创建桁架单元(Truss)。

    Args:
        nEle (int): 单元编号。从 1 开始编号，所有类型的单元均使用同一编号序列。
        params (dict): 单元参数字典，包含以下键值对：
            - nNode1 (int): 节点1编号
            - nNode2 (int): 节点2编号
            - nMat (int): 材料编号
            - nSec1 (int): 截面1编号
            - nSec2 (int): 截面2编号
            - dStrain (float): 应变值，默认为 0.00

    Returns:
        tuple (bool, str): 返回一个元组，包含：
            - bool: 操作是否成功
            - str: 失败原因（如果操作失败）

    Examples:
        >>> # 创建一个基本桁架单元
        >>> result = element_truss(1, {
        ...     "nNode1": 1,
        ...     "nNode2": 2,
        ...     "nMat": 1,
        ...     "nSec1": 1,
        ...     "nSec2": 1,
        ...     "dStrain": 0.00
        ... })
        >>> print(result)
        (True, "")
    """
    eElementType: str = "Truss"
    default_params = args_default["element_truss"]["params"] 
    return osis_element(nEle, eElementType, {**default_params, **params})

@tool
def element_spring(nEle: int = 1, params: dict = {
        "nNode1": 1,
        "nNode2": 2,
        "bLinear": 1,
        "dx": 0.1,
        "dy": 0.1,
        "dz": 0.1,
        "rx": 0.1,
        "ry": 0.1,
        "rz": 0.1,
        "dBeta": 0
    }):
    """创建弹簧单元(SPRING)。

    Args:
        nEle (int): 单元编号。从 1 开始编号，所有类型的单元均使用同一编号序列。
        params (dict): 单元参数字典，包含以下键值对：
            - nNode1 (int): 节点1编号
            - nNode2 (int): 节点2编号
            - bLinear (int): 弹簧类型标志：
                * 1: 线性弹簧
                * 0: 非线性弹簧
            - dx (float/int): x方向自由度参数：
                * 线性弹簧(bLinear=1): 局部坐标系下dx方向的刚度值
                * 非线性弹簧(bLinear=0): 局部坐标系下dx方向的力-位移曲线编号(PUCurve定义)
            - dy (float/int): y方向自由度参数：
                * 线性弹簧(bLinear=1): 局部坐标系下dy方向的刚度值
                * 非线性弹簧(bLinear=0): 局部坐标系下dy方向的力-位移曲线编号(PUCurve定义)
            - dz (float/int): z方向自由度参数：
                * 线性弹簧(bLinear=1): 局部坐标系下dz方向的刚度值
                * 非线性弹簧(bLinear=0): 局部坐标系下dz方向的力-位移曲线编号(PUCurve定义)
            - rx (float/int): 绕x轴旋转自由度参数：
                * 线性弹簧(bLinear=1): 局部坐标系下rx方向的刚度值
                * 非线性弹簧(bLinear=0): 局部坐标系下rx方向的力-位移曲线编号(PUCurve定义)
            - ry (float/int): 绕y轴旋转自由度参数：
                * 线性弹簧(bLinear=1): 局部坐标系下ry方向的刚度值
                * 非线性弹簧(bLinear=0): 局部坐标系下ry方向的力-位移曲线编号(PUCurve定义)
            - rz (float/int): 绕z轴旋转自由度参数：
                * 线性弹簧(bLinear=1): 局部坐标系下rz方向的刚度值
                * 非线性弹簧(bLinear=0): 局部坐标系下rz方向的力-位移曲线编号(PUCurve定义)
            - dBeta (float): 轴向转角(beta角)。默认为 0。

    Returns:
        tuple (bool, str): 返回一个元组，包含：
            - bool: 操作是否成功
            - str: 失败原因（如果操作失败）

    Examples:
        >>> # 创建线性弹簧单元
        >>> result = element_spring(1, {
        ...     "nNode1": 1,
        ...     "nNode2": 2,
        ...     "bLinear": 1,
        ...     "dx": 1000.0,
        ...     "dy": 1000.0,
        ...     "dz": 1000.0,
        ...     "rx": 0.0,
        ...     "ry": 0.0,
        ...     "rz": 0.0,
        ...     "dBeta": 0
        ... })
        >>> print(result)
        (True, "")
    """
    eElementType: str = "Spring"
    default_params = args_default["element_spring"]["params"]
    return osis_element(nEle, eElementType, {**default_params, **params})

@tool
def element_cable(nEle: int = 1, params: dict = {
        "nNode1": 1,
        "nNode2": 2,
        "nMat": 1,
        "nSec": 1,
        "eMethod": "IF",
        "dPara": 1000.0
    }):
    """创建拉索单元(Cable)。

    Args:
        nEle (int): 单元编号，从 1 开始编号，所有类型的单元均使用同一编号序列。
        params (dict): 单元参数字典，包含以下键值对：
            - nNode1 (int): 节点1编号。
            - nNode2 (int): 节点2编号。
            - nMat (int): 材料编号。
            - nSec (int): 截面编号。
            - eMethod (str): 拉索参数定义方法，可选值：
                * UL: 无应力长度控制
                * IF: 初拉力控制
                * HF: 水平力控制
                * VF: 竖向力控制
                * IS: 初应变控制
            - dPara (float): 拉索参数值，根据eMethod的不同代表：
                * UL: 无应力长度
                * IF: 初拉力大小
                * HF: 水平力大小
                * VF: 竖向力大小
                * IS: 初应变值

    Returns:
        tuple (bool, str): 返回一个元组，包含：
            - bool: 操作是否成功
            - str: 失败原因（如果操作失败）

    Examples:
        >>> # 创建初拉力控制的拉索单元
        >>> result = element_cable(1, {
        ...     "nNode1": 1,
        ...     "nNode2": 2,
        ...     "nMat": 1,
        ...     "nSec": 1,
        ...     "eMethod": "IF",
        ...     "dPara": 1000.0
        ... })
        >>> print(result)
        (True, "")
    """

@tool
def element_del(nEle: int = 1) -> Tuple[bool, str]:
    """删除一个单元

    Args:
        nEle (int): 单元编号，从 1 开始计数

    Returns:
        tuple (bool, str):
            - bool: 操作是否成功
            - str: 失败原因（如果操作失败）
    """
    return osis_element_del(nEle)

@tool
def element_mod(nOld: int = 1, nNew: int = 2) -> Tuple[bool, str]:
    """修改一个单元的编号。单元编号存在时，交换

    Args:
        nOld (int): 旧编号
        nNew (int): 新编号

    Returns:
        tuple (bool, str):
            - bool: 操作是否成功
            - str: 失败原因（如果操作失败）
    """
    return osis_element_mod(nOld, nNew)

