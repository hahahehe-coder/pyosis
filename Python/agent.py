import os
import sys

# 添加当前目录到 Python 路径
#current_dir = os.path.dirname(os.path.abspath(__file__))
#if current_dir not in sys.path:
#    sys.path.insert(0, current_dir)

import queue
import threading
import tkinter as tk
from ui import ChatInterface
from pyosis.ai.agents.BeamAgent import create_beam_agent

class LangChainAgent:
    def __init__(self, ui_instance):
        self.ui: ChatInterface = ui_instance
        self.message_queue = queue.Queue()
        
        # 初始化LangChain智能体
        self.agent = create_beam_agent()      # 创建LangChain智能体

        # 启动消息处理循环
        self.ui.root.after(100, self.process_messages)
    
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
            for chunk in self.agent.ask_agent_stream(user_message):
            # for chunk in self.agent.agent.stream(
            #     {"messages": [{"role": "user", "content": user_message}]}, {"configurable": {"thread_id": "1"}}, 
            #     stream_mode="updates"):
                # 提取消息内容
                for step, data in chunk.items():
                    if 'messages' in data and len(data['messages']) > 0:
                        ai_response = f"\nstep: {step}\ncontent: {data['messages'][-1].content_blocks}"     # 调试信息
                        #print(ai_response)
                        #ai_response = data['messages'][-1].content                                          # 一般回复
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

def main():
    # 创建UI
    root = tk.Tk()
    ui = ChatInterface(root)
    
    # 创建智能体并连接到UI
    agent = LangChainAgent(ui)
    
    # 设置UI的回调函数
    ui.on_send_message = agent.process_user_message
    
    # 启动UI主循环
    root.mainloop()

if __name__ == "__main__":
    main()