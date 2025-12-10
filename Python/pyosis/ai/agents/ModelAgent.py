from typing import Tuple
from .BaseAgent import BaseAgent
from .tools import *

class ModelAgent(BaseAgent):
    """模型设计智能体"""
    def __init__(self, model="qwen-flash", api_key="sk-49a9cacef0274e4a8441914642ed1a73", base_url="https://dashscope.aliyuncs.com/compatible-mode/v1"):
        super().__init__(model, api_key, base_url)

    def create_agent(self):
        tools = [
            node,
            node_del,
            element_beam3d,
            element_truss,
            element_spring,
            element_cable,
            element_del,
        ]
        system_prompt = \
"""
你是桥梁模型设计专家，负责创建桥梁的节点和单元，组成完整结构模型。
        
重要依赖关系：
- 创建模型前必须确保材料和截面已经创建
- 需要使用已创建的材料编号和截面编号，如果没有提供给你，终止调用并告诉决策智能体
- 创建单元前一定要确保使用的节点已经创建
        
你的工具：
1. node - 创建一个节点
2. node_del - 删除节点
3. element_XXX - 创建一个对应类型的单元
4. element_del - 删除单元

工作流程：
- 创建任何对象时编号从1递增
- 用户未规定的参数使用默认参数
- 若用户要修改对象，直接重新调用一次创建函数，参数中指定要修改的对象编号即可
- 若用户未说明，假设用户希望创建一个跨度为10米，分为10个单元的简单悬浇梁结构，则你需要创建11个节点和10个单元。

创建成功后，请告知决策智能体。创建失败后，请告知失败原因。
"""
        super().create_agent(tools, system_prompt)
