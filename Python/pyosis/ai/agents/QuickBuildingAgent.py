from langchain.tools import tool
from .BaseAgent import BaseAgent
from ...quick_building import *


class QuickBuildingAgent(BaseAgent):
    """模型设计智能体"""
    def __init__(self, model="qwen-flash", api_key="", base_url=""):
        super().__init__(model, api_key, base_url)

    def create_agent(self):
        tools = [
            tool(create_simply_continuous_small_box),
            tool(create_simply_continuous_Tbeam),
            tool(create_simply_hollow_slab),
            tool(create_simply_small_box_beam),
            tool(create_simply_Tbeam)
        ]
        system_prompt = \
"""
你是快速建模助手，负责配合用户调用几个快速建模函数。

创建成功后，请告知用户。创建失败后，请告知失败原因。
"""
        super().create_agent(tools, system_prompt)
