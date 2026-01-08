from pyosis.ai.agents.DecisionAgent import DecisionAgent
import threading
from ui_threaded import ThreadedChatInterface  # 导入新的线程化UI类
import sys

def log_to_file(msg,fn = "D:/log.txt"):
    fp = open("D:/log.txt", "a", encoding="utf-8")
    fp.write(msg)
    print(msg)
    fp.write('\r\n')
    fp.close()

def check_gil_status():
    """检查当前线程是否持有 GIL"""
    # Python 3.7+ 可以使用
    if hasattr(sys, 'getswitchinterval'):
        log_to_file(f"当前线程: {threading.current_thread().name}")
        log_to_file(f"线程ID: {threading.get_ident()}")
        
    # 检查是否持有 GIL（非精确，但有用）
    import _thread
    try:
        _thread.get_ident()  # 如果持有 GIL，这可以执行
        log_to_file("可能持有 GIL")
    except:
        log_to_file("可能不持有 GIL")

class LangChainAgent:
    def __init__(self, ui_instance, api_key, base_url):
        # UI实例现在是一个线程化的ChatInterface
        self.ui: ThreadedChatInterface = ui_instance
        self.is_processing = False
        
        # 初始化LangChain智能体
        self.agent = DecisionAgent("qwen-max", api_key, base_url)
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


# # 示例：如何从外部控制UI
# def external_control_example():
#     """
#     演示如何从外部线程控制UI的示例函数
#     可以在主程序中调用此函数来演示控制能力
#     """
#     import time
    
#     def control_ui():
#         """在后台线程中控制UI的示例"""
#         print("开始外部控制UI...")
        
#         # 等待UI启动
#         time.sleep(2)
        
#         # 改变窗口大小
#         print("改变窗口大小...")
#         set_window_size(800, 700)
#         time.sleep(1)
        
#         # 改变窗口位置
#         print("移动窗口位置...")
#         set_window_position(300, 100)
#         time.sleep(1)
        
#         # 改变标题
#         print("修改窗口标题...")
#         set_window_title("AI设计助手 - 在线")
#         time.sleep(1)
        
#         # 添加一条自动消息
#         print("发送自动问候...")
#         add_ai_message("我是您的AI助手，随时准备为您服务！")
#         time.sleep(2)
        
#         # 聚焦到输入框
#         print("聚焦到输入框...")
#         focus_input()
        
#         print("外部控制完成！")
    
#     # 在后台线程中运行控制逻辑
#     control_thread = threading.Thread(target=control_ui, daemon=True)
#     control_thread.start()


# def main():
#     """主函数"""
#     # 配置参数
#     api_key = "sk-2aa267fb86de469b8d831bea645cf182"
#     base_url = "https://dashscope.aliyuncs.com/compatible-mode/v1"
#     window_title = "AI 设计助手 v2.0"
    
#     print("=" * 50)
#     print("启动AI设计助手...")
#     print("=" * 50)
    
#     # 创建并启动UI（在独立线程中）
#     ui, agent = create_agent_ui(api_key, base_url, window_title)
    
#     # 启动外部控制示例（可选）
#     # external_control_example()
    
#     print("\n主程序正在运行，UI在独立线程中...")
#     print("您可以：")
#     print("1. 在UI窗口中输入消息")
#     print("2. 从其他线程调用控制函数")
#     print("3. 按Ctrl+C退出程序\n")
    
#     # 主线程可以继续执行其他任务
#     try:
#         # 这里可以添加主程序的其他逻辑
#         # 例如：定期检查任务、处理其他事件等
        
#         # 简单的监控循环
#         count = 0
#         while ui.is_alive():
#             # 模拟主程序的其他工作
#             count += 1
#             if count % 10 == 0:
#                 print(f"主程序运行中... [{count}]")
            
#             # 可以在这里添加其他逻辑
#             # 例如：处理其他事件、检查网络状态等
            
#             # 休眠以避免占用太多CPU
#             time.sleep(1)
            
#     except KeyboardInterrupt:
#         print("\n收到中断信号，正在关闭...")
#     finally:
#         # 安全关闭
#         if ui.is_alive():
#             print("关闭UI...")
#             ui.close()
#             # 等待UI线程结束
#             ui.join(timeout=2)
        
#         print("程序退出")


if __name__ == "__main__":
    import time
    create_agent_ui()
    while True:
        time.sleep(1)