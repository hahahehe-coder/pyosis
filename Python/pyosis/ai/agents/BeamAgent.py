from typing import Tuple
from langchain.tools import tool
from .BaseAgent import BaseAgent
from .tools import *


"""
智能体工具函数定义注意事项：

所有函数参数尽量写的简单易懂
"""

@tool
def material(nMat: int = 1, strName: str = "C50", strMaterialType: str = "CONC", code: str = "JTG3362_2018", grade: str = "C50", nCrepShrk: int = 1, dDmp: float = 0.05,
        creep_shrink_params: dict = {
            "dAvgHumidity": 70.0,
            "nBirthTime": 7,
            "dTypeCoeff": 5.0,
            "nBirthByShrinking": 3
        }    
    ) -> Tuple[bool, str]:
    """创建或修改材料

    Args:
        nMat (int): 材料编号
        strName (str): 材料名称
        strMaterialType (str): 材料类型，CONC = 混凝土，STEEL = 钢材，PRESTRESSED = 预应力材料，REBAR = 普通钢筋
        code (str): 材料标准代码，{
            钢材可选：JTGD64_2015
            其他材料可选：JTG3362_2018，JTGD62_2004
        }
        grade (str): 材料等级牌号，{
            混凝土可选:C15, C20, C25, C30, C35, C40, C45, C50, C55, C60, C65, C70, C75, C80
            钢材可选:Q235，Q345，Q390，Q420
            预应力材料可选:JTG3362_2018：(Strand1720，Strand1860，Strand1960，Wire1470，Wire1570，Wire1770， Wire1860，Rebar785，Rebar930，Rebar1080)
                          JTGD62_2004：(Strand1860，Wire1670，Wire1770， Rebar785，Rebar930)
            普通钢筋:JTG3362_2018：(HPB300, HRB400, HRBF400, RRB400, HRB500)
                    JTGD62_2004：(R235, HRB335,HRB400, KL400)
        }
        nCrepShrk (int): 收缩徐变特性编号（只要混凝土材料需要，其他材料将值设置为-1）
        dDmp (float): 材料阻尼比
        creep_shrink_params (dict): 收缩徐变特性参数，只有material=CONC时有效 {
            dAvgHumidity(float): 年平均湿度（百分比）
            nBirthTime(int): 混凝土龄期（天）
            dTypeCoeff(float): 水泥种类系数
            nBirthByShrinking(int): 收缩开始时的混凝土龄期（天数）
        }
        
    Returns:
        Tuple[bool, str]: (是否成功，失败原因)
    """
    if strMaterialType == "CONC":
        isok, errorCode = osis_creep_shrink(nCrepShrk, strName + "收缩徐变", 
            creep_shrink_params.get("dAvgHumidity", 70.0),
            creep_shrink_params.get("nBirthTime", 7),
            creep_shrink_params.get("dTypeCoeff", 5.0),
            creep_shrink_params.get("nBirthByShrinking", 3)
        )
        if not isok:
            return isok, errorCode
    # if material == "PRESTRESSED":
    #     if code == "JTG3362_2018":
    #         return False, "Rebar785等级的预应力材料只能用于JTGD62_2004标准"
    return osis_material(nMat, strName, strMaterialType, code, grade, nCrepShrk, dDmp)

@tool
def section(nSec: int = 1, strName: str = "C50", strSectionType: str = "RECT", 
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
        "h": 0.2000},
    offset_params: dict = {
        "offsetTypeY": "Middle",
        "dOffsetValueY": 0.0,
        "offsetTypeZ": "Center",
        "dOffsetValueZ": 0.0},
    mesh_params: dict = {
        "bMeshMethod": 1,
        "dMeshSize": 0.1
    }
    ) -> Tuple[bool, str]:
    """创建或修改截面

    Args:
        nSec (int): 截面编号
        strName (str): 截面名称
        strSectionType (str): 截面类型
        nBirthTime (int): 截面创建时的混凝土龄期（天）
        params (dict): 截面参数 {
            TransitionType(str): Chamfer=斜倒角，Fillet=圆倒角
            SecType(str): Solid=实腹，Hollow=空腹
            B(float): 截面宽度
            H(float): 截面高度
            xo1(float): 斜倒角宽
            yo1(float): 斜倒角高
            R(float): 截面圆倒角半径
            t1(float): 壁厚1
            t2(float): 壁厚2
            xi1(float): 内倒角宽
            yi1(float): 内倒角高
            HasDiaphragm(bool): 是否有隔板,False=无隔板，True=有隔板
            tw(float): 隔板厚
            xi2(float): 隔板倒角宽
            yi2(float): 隔板倒角高
            HasGroove(bool): 是否有凹槽,False=无凹槽，True=有凹槽
            b1(float): 凹槽上口宽
            b2(float): 凹槽下口宽
            h(float): 凹槽深度}
        offset_params (dict): 截面偏心参数 {
            offsetTypeY(str): 截面局部坐标系下Y方向的偏心，可选：Left、Middle、Right、Manual
            dOffsetValueY(float): Y方向的偏心数值，在Y方向偏心为Manual时该项起作用
            offsetTypeZ(str): 截面局部坐标系下Z方向的偏心，可选：Top、Center、Bottom、Manual
            dOffsetValueZ(float): Z方向的偏心数值，在Z方向偏心为Manual时该项起作用}
        mesh_params (dict): 截面网格划分参数 {
            bMeshMethod(bool): 0=自动划分，1=手动划分
            dMeshSize(float):网格划分尺寸，在MeshMethod=1时该项起作用}
        
    Returns:
        Tuple[bool, str]: (是否成功，失败原因)
    """
    default_params: dict = {
        "TransitionType": "Fillet",
        "SecType": "Solid",
        "B": 1.0000,
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
    isok, errorCode = osis_section(nSec, strName, strSectionType, {**default_params, **params})
    if not isok:
        return isok, errorCode
    
    isok, errorCode = osis_section_offset(nSec, 
                                          offset_params.get("offsetTypeY", "Middle"), 
                                          offset_params.get("dOffsetValueY", 0.0), 
                                          offset_params.get("offsetTypeZ", "Center"), 
                                          offset_params.get("dOffsetValueZ", 0.0))
    if not isok:
        return isok, errorCode

    return osis_section_mesh(nSec, 
                             mesh_params.get("bMeshMethod", 1), 
                             mesh_params.get("dMeshSize", 0.1))

def create_beam_agent():
    tools = [
        material,
        section,
        # section_offset,
        # section_mesh,
        node,
        element
    ]
    # qwen-max prompt
    system_prompt = """你是一个专业的悬浇梁构建助手，专门辅助用户构建简单悬浇梁。你的核心任务是确保所有传递的参数名称正常，所有操作都正确执行，并严格遵循函数间的依赖关系。如果依赖未满足，你必须优先处理依赖，再执行后续操作。

    你的能力包括：
    1. 创建材料（独立功能）。
    2. 创建截面（独立功能）：创建截面并设置截面偏心和网格划分参数。
    3. 创建节点（独立功能）。
    4. 创建单元（依赖材料、节点和截面）：在创建单元前，必须确保材料、节点和截面均已创建。
    5. 创建完整悬浇梁：创建材料-创建截面-创建节点-创建单元。操作是按一个大步骤一个大步骤执行,不要一次性完成所有步骤全部调用,也不要调用的太细,比如创建节点和单元时,请一次性创建多个节点和单元。
    6. 在之前的操作基础上继续创建悬浇梁。比如用户指定用某创建好的材料来创建悬浇梁，则不需要重新创建材料了，直接创建截面-创建节点-创建单元就好了。或者用户指定用某创建好的截面来创建悬浇梁，则不需要重新创建截面了，直接创建材料-创建节点-创建单元就好了。
    
    使用工具时，请确保：
    1. 创建任何对象时编号从1递增
    2. 操作前告诉用户你的想法，操作完成后先确认执行结果，如果失败结束当前调用链并告知用户
    3. 如果用户没有明确说明参数，使用默认参数创建
    4. 如果用户要求的操作有依赖功能，请先调用依赖功能
    5. 对于节点和单元的创建，若用户未说明，假设用户希望创建一个跨度为10米，分为10个单元的简单悬浇梁结构，则你需要创建11个节点和10个单元。
    6. 如果需要修改直接调用创建函数即可，系统会自动覆盖原有定义
    """

    # qwen-turbo prompt
    # system_prompt = """你是一个专业的悬浇梁构建助手，专门辅助用户构建简单悬浇梁。

    # 你的能力包括：
    # 1. 创建材料，独立功能
    # 2. 创建截面，独立功能
    # 3. 创建节点，独立功能
    # 4. 创建单元，依赖于材料，节点和截面
    # 5. 创建悬浇梁：严格按照流程来：创建材料-创建截面-创建节点-创建单元
    # 6. 在之前的操作基础上继续创建悬浇梁。比如用户指定用某创建好的材料来创建悬浇梁，则不需要重新创建材料了，直接创建截面-创建节点-创建单元就好了。或者用户指定用某创建好的截面来创建悬浇梁，则不需要重新创建截面了，直接创建材料-创建节点-创建单元就好了。
    

    # 如果需要修改直接调用创建函数即可，系统会自动覆盖原有定义

    # 使用工具时，请确保：
    # 1. 创建任何对象时时编号从1递增
    # 2. 操作完成后先确认执行结果，不要一次性把所有函数全部调用。如果失败结束当前调用链并告知用户
    # 3. 如果用户没有明确说明参数，使用默认参数创建
    # 4. 如果用户要求的操作有依赖功能，请先调用依赖功能
    # 5. 对于节点和单元的创建，若用户未说明，假设用户希望创建一个跨度为10米，分为10个单元的简单悬浇梁结构，则你需要创建11个节点和10个单元
    # """

#     system_prompt = """你是一个专业的悬浇梁构建助手，专门辅助用户构建简单悬浇梁。你的核心任务是确保所有传递的参数名称正常，所有操作都正确执行，并严格遵循函数间的依赖关系。如果依赖未满足，你必须优先处理依赖，再执行后续操作。

# 你的能力列表（包括依赖关系说明）：

# 1. 创建或修改收缩徐变特性（独立功能）。
# 2. 创建材料：如果是混凝土材料，在创建材料前，必须确保收缩徐变特性已存在。如果未创建，请先调用收缩徐变创建函数。其它材料类型不依赖收缩徐变特性。
# 3. 创建截面（独立功能）：先创建截面然后设置截面偏心或网格划分参数。
# 4. 创建节点（独立功能）：节点是单元构建的基础。建议一次性创建多个节点。
# 5. 创建单元（依赖材料、节点和截面）：在创建单元前，必须确保材料、节点和截面均已创建。否则，请先调用依赖功能。建议一次性创建多个单元。
# 6. 创建完整悬浇梁：创建收缩徐变特性-创建混凝土材料-创建截面与设置截面偏心和网格划分参数-创建节点-创建单元
# 7. 在之前的操作基础上继续创建悬浇梁。比如用户指定用某创建好的材料来创建悬浇梁，则不需要重新创建材料了，直接创建截面-创建节点-创建单元就好了，然后在传材料编号参数时传创建好的材料的编号即可。

# 使用工具时的关键规则：

# 1. 每次调用后，确认执行状态：如果成功，继续下一步。如果失败，立即停止当前调用链，并向用户报告错误原因。
# 2. 如果用户只请求部分操作，只需要调用该操作的依赖操作与该操作。
# 3. 如果用户要求修改对象，直接调用创建函数（系统会自动覆盖）。
# 4. 每一步操作后，检查执行结果。如果遇到错误（如依赖缺失或参数无效），立即终止并告知用户。

# 编号和参数管理：

# 1. 所有对象编号从1开始递增。
# 2. 如果用户未明确参数，你不需要显示传递这些参数，系统会自动使用默认参数。
# 3. 调用函数前，仔细核对参数名称和值，避免无效调用。

# 示例场景：
# 1. 用户请求“创建悬浇梁”：按照标准工作流程逐步执行，默认创建10m长的悬浇梁，有11个节点与10个单元。
# 2. 用户请求“创建混凝土材料”：先检查收缩徐变是否存在；如果不存在，自动创建收缩徐变，再创建材料。
# 3. 用户请求“创建截面”：先创建截面，再设置截面偏心和网格划分参数。
# 3. 用户请求“修改材料”：直接调用材料创建函数（系统覆盖），无需处理依赖。

# 请严格按照以上规则操作，确保依赖关系正确且函数实际被调用。如果有任何不确定性，先询问用户或使用默认值。
#     """


    beam_agent = BaseAgent()
    beam_agent.create_agent(
        tools=tools,
        system_prompt=system_prompt
    )
    return beam_agent

if __name__ == "__main__":
    beam_agent = create_beam_agent()