from pyosis.ai.agents.QuickBuildingAgent import QuickBuildingAgent
import threading
from ui_threaded import ThreadedChatInterface  # 导入新的线程化UI类
import sys

def log_to_file(msg,fn = "D:/log.txt"):
    fp = open("D:/log.txt", "a", encoding="utf-8")
    fp.write(msg)
    print(msg)
    fp.write('\r\n')
    fp.close()

class LangChainAgent:
    def __init__(self, ui_instance, api_key, base_url):
        # UI实例现在是一个线程化的ChatInterface
        self.ui: ThreadedChatInterface = ui_instance
        self.is_processing = False
        
        # 初始化LangChain智能体
        self.agent = QuickBuildingAgent("qwen-plus", api_key, base_url)
        self.agent.create_agent()
    
    def process_user_message(self, user_message):
        """处理用户消息"""
        if self.is_processing:
            self.ui.add_ai_response("正在处理上一个请求，请稍候...")
            return
            
        self.is_processing = True
        
        # 在新线程中运行智能体
        threading.Thread(
            target=self.run_agent, 
            args=(user_message,),
            daemon=True
        ).start()
    
    def run_agent(self, user_message):
        """运行智能体处理用户消息"""
        try:
            # 开始流式输出
            stream_id = self.ui.start_ai_stream()
            
            # 运行智能体流式处理
            for chunk in self.agent.stream(user_message):
                # 提取消息内容
                for step, data in chunk.items():
                    if 'messages' in data and len(data['messages']) > 0:
                        ai_response = f"\nstep: {step}\ncontent: {data['messages'][-1].content_blocks}"     # 调试信息
                        log_to_file(ai_response)
                        
                        if step == "tools":     # 工具调用结果不需要显示
                            continue
                            
                        ai_response = data['messages'][-1].content  # 一般回复
                        # 将内容发送到UI（线程安全）
                        self.ui.update_ai_stream(ai_response + " ", stream_id)
            
            # 流式输出结束
            self.ui.end_ai_stream(stream_id)
            
        except Exception as e:
            error_msg = f"智能体处理出错: {str(e)}"
            self.ui.add_ai_response(error_msg)
        finally:
            self.is_processing = False


def create_agent_ui(api_key="sk-2aa267fb86de469b8d831bea645cf182", 
                   base_url="https://dashscope.aliyuncs.com/compatible-mode/v1",
                   window_title="AI 设计助手"):
    """
    创建智能体UI的便捷函数
    
    Args:
        api_key: API密钥
        base_url: API基础URL
        window_title: 窗口标题
    """
    def handle_user_message(message):
        """处理用户消息的回调函数"""
        print(f"收到用户消息: {message}")
        
        # 获取全局agent实例并处理消息
        agent = get_agent_instance()
        if agent:
            agent.process_user_message(message)
        else:
            print("错误: 智能体未初始化")
    
    # 创建线程化UI
    ui = ThreadedChatInterface(
        on_send_message=handle_user_message,
        window_title=window_title
    )
    
    # 创建智能体并连接到UI
    agent = LangChainAgent(ui, api_key, base_url)
    
    # 保存agent实例引用（线程安全）
    set_agent_instance(agent)
    
    # 添加欢迎消息
    ui.add_ai_response("您好！我是AI设计助手，请问有什么可以帮助您的？")
    # check_gil_status()
    return ui, agent


# 全局变量和锁，用于线程安全的agent实例管理
_agent_instance = None
_agent_lock = threading.Lock()

def get_agent_instance():
    """获取agent实例的线程安全方法"""
    with _agent_lock:
        return _agent_instance

def set_agent_instance(agent):
    """设置agent实例的线程安全方法"""
    global _agent_instance
    with _agent_lock:
        _agent_instance = agent


# UI控制函数 - 可以从其他线程安全调用
def set_window_size(width, height):
    """线程安全地设置窗口大小"""
    agent = get_agent_instance()
    if agent and agent.ui:
        agent.ui.resize_window(width, height)

def set_window_position(x, y):
    """线程安全地设置窗口位置"""
    agent = get_agent_instance()
    if agent and agent.ui:
        agent.ui.set_window_position(x, y)

def set_window_title(title):
    """线程安全地设置窗口标题"""
    agent = get_agent_instance()
    if agent and agent.ui:
        agent.ui.set_title(title)

def focus_input():
    """线程安全地聚焦到输入框"""
    agent = get_agent_instance()
    if agent and agent.ui:
        agent.ui.focus_input()

def close_window():
    """线程安全地关闭窗口"""
    agent = get_agent_instance()
    if agent and agent.ui:
        agent.ui.close()

def add_ai_message(message):
    """线程安全地添加AI消息"""
    agent = get_agent_instance()
    if agent and agent.ui:
        agent.ui.add_ai_response(message)

def clear_chat():
    """线程安全地清空聊天记录"""
    agent = get_agent_instance()
    if agent and agent.ui:
        agent.ui.clear_chat()



if __name__ == "__main__":
    api_key="sk-2aa267fb86de469b8d831bea645cf182"
    base_url="https://dashscope.aliyuncs.com/compatible-mode/v1"

    create_agent_ui(api_key, base_url)
    import time
    while True:
        time.sleep(1)   # 主线程不干活