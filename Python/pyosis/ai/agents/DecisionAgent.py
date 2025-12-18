import time
from typing import Tuple
from langchain_core.messages import AIMessage, HumanMessage
from .BaseAgent import BaseAgent
from langchain.tools import tool
from functools import partial
from .MaterialAgent import MaterialAgent
from .SectionAgent import SectionAgent
from .ModelAgent import ModelAgent
from .tools import *

debug_file = "debug.txt"
log_file = "log.txt"

# def pack_cmd(tool_call: dict) -> str:
#     return py_to_cmd(tool_call['name'], tool_call['args'])

# def pack_func_call(tool_call: dict) -> str:
#     args = deep_merge(args_default[tool_call['name']], tool_call['args'])
#     return f"name: {tool_call['name']}, args: {args}"

def log_to_file(log_file: str, content: str):
    with open(log_file, "a", encoding='utf-8') as f:
        f.write(content + "\n")
        f.close()

# 写入时间
log_to_file("debug.txt", f"\n=====Start at {time.strftime('%Y-%m-%d %H:%M:%S', time.localtime())}=====")
log_to_file("log.txt", f"\n=====Start at {time.strftime('%Y-%m-%d %H:%M:%S', time.localtime())}=====")
log_to_file("log_cmd.txt", f"\n//=====Start at {time.strftime('%Y-%m-%d %H:%M:%S', time.localtime())}=====")

material_agent = None
section_agent = None
model_agent = None

def call_agent(agent, request: str):
    ai_response = agent.invoke(request, "4")
    messages = ai_response['messages']
    last_human_index = 0
    for i in range(len(messages)-1, -1, -1):
        if isinstance(messages[i], HumanMessage):
            last_human_index = i
            break

    print("v" * 30)
    for message in messages[last_human_index + 1:]:
        if isinstance(message, AIMessage):
            print(f"content: {message.content}")
            print(f"tool_calls: {message.tool_calls}\n")
            # log_to_file(debug_file, f"content: {message.content}")
            # log_to_file(debug_file, f"tool_calls: {message.tool_calls}\n")
            for tool_call in message.tool_calls:
                log_to_file("log.txt", f"{pack_func_call(tool_call['name'], tool_call['args'])}")         # 包装成python函数调用信息
                log_to_file("log_cmd.txt", f"{py_to_cmd(tool_call['name'], tool_call['args'])}")          # 包装成命令流格式
    print("^" * 30)

    return ai_response['messages'][-1].content

@tool
def call_material_agent(request: str):
    """
    调用材料智能体

    Args:
        request (str): 对材料智能体的请求
    Returns:
        str: 材料智能体的回答
    """
    global material_agent
    return call_agent(material_agent, request)
    # for chunk in material_agent.ask_agent_stream(request, "1"):
    #     # 提取消息内容
    #     for step, data in chunk.items():
    #         ai_response = f"\nstep: {step}\ncontent: {data['messages'][-1].content_blocks}"     # 调试信息
    #         print(ai_response)
    #         yield data['messages'][-1].content                                          # 一般回复
    
@tool
def call_section_agent(request: str):
    '''
    调用截面智能体

    Args:
        request (str): 对截面智能体的请求
    Returns:
        str: 截面智能体的回答
    '''

    return call_agent(section_agent, request)
    # for chunk in section_agent.ask_agent_stream(request, "2"):
    #     # 提取消息内容
    #     for step, data in chunk.items():
    #         ai_response = f"\nstep: {step}\ncontent: {data['messages'][-1].content_blocks}"     # 调试信息
    #         print(ai_response)
    #         yield data['messages'][-1].content                                          # 一般回复

@tool
def call_model_agent(request: str):
    '''
        调用模型智能体
        
    Args:
        request (str): 对模型智能体的请求，若无具体需求，直接让其创建示例悬浇梁
    Returns:
        str: 模型智能体的回答
    '''
    response = call_agent(model_agent, request)
    # osis_replot()
    return response
    # for chunk in model_agent.ask_agent_stream(request, "3"):
    #     # 提取消息内容
    #     for step, data in chunk.items():
    #         ai_response = f"\nstep: {step}\ncontent: {data['messages'][-1].content_blocks}"     # 调试信息
    #         print(ai_response)
    #         yield data['messages'][-1].content                                          # 一般回复

def call_display_agent(request: str):
    '''
    调用显示智能体

    Args:
        request (str): 对显示智能体的请求，让其刷新界面
    Returns:
        str: 模型智能体的回答
    '''
    response = "已刷新界面"
    try:
        isok, error = osis_replot()
        if not isok:
            response = error
    except Exception as e:
        response = str(e)
    # osis_replot()
    log_to_file(debug_file, f"content: {response}")
    log_to_file(debug_file, f"tool_calls: {response}\n")
    log_to_file("log.txt", f"{pack_func_call('replot', {})}")         # 包装成python函数调用信息
    log_to_file("log_cmd.txt", f"{py_to_cmd('replot', {})}")          # 包装成命令流格式
    return "已刷新界面"

class DecisionAgent(BaseAgent):
    """决策智能体"""
    def __init__(self, model="qwen-max", api_key="", base_url=""):
        super().__init__(model, api_key, base_url)
        global material_agent
        global section_agent
        global model_agent
        material_agent = MaterialAgent('qwen-plus', api_key, base_url)
        material_agent.create_agent()
        section_agent = SectionAgent('qwen-plus', api_key, base_url)
        section_agent.create_agent()
        model_agent = ModelAgent('qwen-flash', api_key, base_url)
        model_agent.create_agent()

    def create_agent(self):
        tools = [
            call_material_agent, # 调用材料智能体
            call_section_agent,  # 调用截面智能体
            call_model_agent,    # 调用模型智能体
        ]
    
#         system_prompt = """\
# 你是一个桥梁设计总监，负责协调材料、截面、模型、显示等专业智能体的工作。
        
# 你的职责：
# 1. 分析用户需求，确定设计参数
# 2. 协调各个智能体的工作顺序
# 3. 检查设计进度，确保所有组件都正确创建
# 4. 处理设计过程中的冲突和错误
# 5. 调用操作必须一步步来，等上一步完成了才能调用下一个函数。

# 你的手下掌握着以下几个智能体：
# 1. 材料智能体：用于生成满足用户需求的材料。
# 2. 截面智能体：用于生成满足用户需求的截面。
# 3. 模型智能体：用于生成满足用户需求的模型，具体包含创建节点与创建单元。创建时需要提供材料与截面编号。
# 4. 显示智能体：用于刷新界面。

# 工作流程
# - 调用工具前告诉用户你的想法。
# - 你最重要的工作是确定当前用户的需求需要调用哪个智能体，你可能并不知道用户所描述的参数具体含义是什么，你只需要将需求与必要信息整理并传递给具体功能智能体即可。
# - 若需要创建完整悬浇梁，流程为创建材料-创建截面-创建节点-创建单元-刷新界面。

# 不要做超出用户要求的事。所有流程结束后，请告知用户可以继续下一步。若功能智能体出现问题，请告知失败原因。
# """
        system_prompt = """\
你是一个桥梁设计总监，负责协调材料、截面、模型、显示等专业智能体的工作。

你的手下掌握着以下几个智能体：
1. 材料智能体：用于生成满足用户需求的材料。
2. 截面智能体：用于生成满足用户需求的截面。
3. 模型智能体：用于生成满足用户需求的模型，具体包含创建节点与创建单元。创建一定要提供材料与截面编号，如果没有材料或者截面，新建一个。

你的职责：
1. 分析用户需求，确定设计参数
2. 协调各个智能体的工作顺序
3. 检查设计进度，确保所有组件都正确创建
4. 处理设计过程中的冲突和错误

工作原则：
1. **严格顺序执行**：每次只调用一个工具，等待该工具执行完成并返回结果后，再根据结果决定下一步
2. **确认执行结果**：每个工具调用后，必须检查返回的结果状态，确认成功后再继续
3. **分步规划**：即使你知道完整流程，也不能一次性生成所有工具调用，必须一步步来，也一定不要做用户要求之外的事情，比如只要求创建材料，你就不要继续剩下的流程

工作流程：
1. 先告诉用户你的思考和计划
2. 调用第一个需要的工具（且只能调用一个）
3. 等待工具执行完成并返回结果
4. 检查结果，确认是否成功
5. 如果成功，继续下一步；如果失败，处理错误
6. 重复2-5直到完成所有工作

工具调用规则：
- 创建悬浇梁的标准流程：创建材料 → 创建截面 → 创建节点与单元

特殊情况处理：
- 如果工具返回错误，分析错误原因并告诉用户
- 如果用户需求不完整，先询问缺失信息
- 所有流程结束后，告知用户可以继续下一步

记住：每次只做一个动作，等待结果，再决定下一步！"""
# - 创建完整悬浇梁的单元前必须确保材料与截面已经创建完成，否则创建单元会失败。
        super().create_agent(tools, system_prompt)

    def replot(self):
        osis_replot()
