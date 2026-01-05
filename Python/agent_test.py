import queue
import threading
from ui import ChatInterface
import tkinter as tk

from pyosis.ai.agents.QuickBuildingAgent import QuickBuildingAgent
# _api_key=""      # 全局变量
# _base_url=""
# 全局变量，用于存储UI实例的引用
_ui_instance = None
_ui_lock = threading.Lock()

def get_ui_instance():
    """获取UI实例的线程安全方法"""
    with _ui_lock:
        return _ui_instance

def set_ui_instance(ui):
    """设置UI实例的线程安全方法"""
    global _ui_instance
    with _ui_lock:
        _ui_instance = ui

class LangChainAgent:
    def __init__(self, ui_instance, api_key, base_url):
        self.ui: ChatInterface = ui_instance
        self.message_queue = queue.Queue()
        
        # 初始化LangChain智能体
        self.agent = QuickBuildingAgent("qwen-flash", api_key, base_url)
        self.agent.create_agent()
        
        # 设置UI实例的全局引用
        set_ui_instance(ui_instance)

        # 启动消息处理循环
        self.after_id = self.ui.root.after(100, self.process_messages)
    
    def __del__(self):
        # 取消待处理的 after 回调
        if self.after_id and self.ui and self.ui.root:
            self.ui.root.after_cancel(self.after_id)

    def process_user_message(self, user_message):
        """处理用户消息"""
        # 在新线程中运行智能体
        threading.Thread(target=self.run_agent, args=(user_message,), daemon=True).start()
    
    def run_agent(self, user_message):
        """运行智能体处理用户消息"""
        try:
            # 开始流式输出
            stream_id = self.ui.start_ai_stream()
            
            # 运行智能体流式处理
            for chunk in self.agent.stream(user_message):
            # for chunk in self.agent.agent.stream(
            #     {"messages": [{"role": "user", "content": user_message}]}, {"configurable": {"thread_id": "1"}}, 
            #     stream_mode="updates"):
                # 提取消息内容
                for step, data in chunk.items():
                    if 'messages' in data and len(data['messages']) > 0:
                        ai_response = f"\nstep: {step}\ncontent: {data['messages'][-1].content_blocks}"     # 调试信息
                        print(ai_response)
                        if step == "tools":     # 工具调用结果不需要显示
                            continue
                        ai_response = data['messages'][-1].content                                          # 一般回复
                        # 将内容发送到UI
                        self.message_queue.put(("stream", ai_response, stream_id))
            
            # 流式输出结束
            self.message_queue.put(("end", "", stream_id))
            # 普通输出
            # self.message_queue.put(("message", self.agent.ask_agent(user_message), None))
            
        except Exception as e:
            error_msg = f"智能体处理出错: {str(e)}"
            self.message_queue.put(("error", error_msg, None))
    
    def process_messages(self):
        """处理来自队列的消息"""
        try:
            while True:
                msg_type, content, stream_id = self.message_queue.get_nowait()
                
                if msg_type == "stream":
                    self.ui.update_ai_stream(content + " ", stream_id)
                elif msg_type == "end":
                    self.ui.end_ai_stream(stream_id)
                elif msg_type == "error":
                    self.ui.add_ai_response(content)
                elif msg_type == "message":     # 普通消息
                    self.ui.add_ai_response(content)
                    
        except queue.Empty:
            pass
        finally:
            # 继续检查新消息
            self.ui.root.after(100, self.process_messages)

def create_agent_ui(api_key="sk-2aa267fb86de469b8d831bea645cf182", base_url="https://dashscope.aliyuncs.com/compatible-mode/v1"):
    """创建智能体UI的便捷函数"""
    root = tk.Tk()
    ui = ChatInterface(root)
    
    # 创建智能体并连接到UI
    agent = LangChainAgent(ui, api_key, base_url)
    
    # 设置UI的回调函数
    ui.on_send_message = agent.process_user_message
    
    # 启动UI主循环
    root.mainloop()

def set_window_size(width, height):
    """线程安全地设置窗口大小"""
    ui_instance = get_ui_instance()
    if ui_instance:
        ui_instance.resize_window(width, height)

def set_window_position(x, y):
    """线程安全地设置窗口位置"""
    ui_instance = get_ui_instance()
    if ui_instance:
        ui_instance.set_window_position(x, y)

def close_window():
    """线程安全地关闭窗口"""
    ui_instance = get_ui_instance()
    if ui_instance:
        ui_instance.close_window()

# def main(api_key="sk-2aa267fb86de469b8d831bea645cf182", base_url="https://dashscope.aliyuncs.com/compatible-mode/v1"):
#     # 创建UI
#     root = tk.Tk()
#     ui = ChatInterface(root)
    
#     # 创建智能体并连接到UI
#     agent = LangChainAgent(ui, api_key, base_url)
    
#     # 设置UI的回调函数
#     ui.on_send_message = agent.process_user_message
    
#     # 启动UI主循环
#     root.mainloop()

if __name__ == "__main__":
    api_key="sk-2aa267fb86de469b8d831bea645cf182"
    base_url="https://dashscope.aliyuncs.com/compatible-mode/v1"

    create_agent_ui(api_key, base_url)
