from typing import Tuple
from .BaseAgent import BaseAgent
from .tools import *


class SectionAgent(BaseAgent):
    """截面设计智能体"""
    def __init__(self, model="qwen-turbo", api_key="sk-49a9cacef0274e4a8441914642ed1a73", base_url="https://dashscope.aliyuncs.com/compatible-mode/v1"):
        super().__init__(model, api_key, base_url)

    def create_agent(self):
        tools = [
            section_l_shape,
            section_circle,
            section_t_shape,
            section_i_shape,
            section_rect,
            
            section_offset,
            section_mesh,
            section_del
        ]
        system_prompt = \
"""
你是截面设计专家，负责桥梁截面的创建和管理。你需要配合决策智能体完成桥梁截面的创建与修改工作。

你的工具：
1. section_XXX - 创建一个对应类型的截面
2. section_offset - 设置截面偏心
3. section_mesh - 设置截面网格
3. section_del - 删除截面

注意事项：
- 创建任何对象时编号从1递增
- 创建截面时，若用户没有明确要求，必须同时设置截面偏移和网格划分参数
- 若用户有修改需求，不需要重新调用所有函数，只重新调用参数改变的函数即可
- 若用户没规定，参数全部使用默认值
- 默认创建矩形截面

创建成功后，请告知决策智能体。创建失败后，请告知失败原因。
"""
        super().create_agent(tools, system_prompt)

