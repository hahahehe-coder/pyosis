'''
pyosis.core.command 的 Docstring

===
# OSIS命令流兼容
'''

import inspect
import functools
from typing import Dict, Any, Tuple
from .engine import OSISEngine

def osis_run(strCmd) -> Tuple[bool, str, Any]:
    '''
    直接以命令流的形式运行OSIS功能
    
    Args:
        strCmd: 完整的命令流

    Returns:
        tuple (bool, str, Any): 是否成功，失败原因，其他结果数据
    '''
    e = OSISEngine.GetInstance()
    return e.OSIS_Run(strCmd)

class OSISFunctionRegistry:
    """
    OSIS函数注册表.通过装饰器注册函数，可指定命令流名称

    ---
    Attributes:
        commands (dict): 存储所有注册的命令，键为命令流名称，值为对应的函数
    
    Example:
        >>> REGISTRY = OSISFunctionRegistry()
        >>> 
        >>> @REGISTRY.register("BdGrp")
        ... def boundary_group(name, op, params=None):      # 函数名写全名便于人和**AI**理解
        ...     '''边界组操作'''
        ...     pass
        >>> 
        >>> # 调用函数会自动转换为命令字符串
        >>> result = boundary_group(2, "a", [2, 3, 5, "8to10"]) # 会自动组装成 “ BdGrp,2,a,2,3,5,8to10; ” 并发给OSIS执行
        >>> print(result)
        (True, "")
        >>>  
        >>> @REGISTRY.register("test")
        ... def test_func(a: str, b: bool, c: float, d: int=None):
        ...     '''示例'''
        ...     pass
        >>> test_func("name", True, 1.0, None)  # 组装成 test,name,1,1.0;   # 参数值为None会被忽略
        >>> test_func("name", True, "", 1)      # 组装成 test,name,1,,1;    # 需要空参数忽略参数类型填一个空字符串： ""
        >>> 

    
    """
    
    def __init__(self):
        self.commands = {}  # func_name -> info
    
    def register(self, cmd_name=None):
        """
        注册函数装饰器
        
        Args:
            cmd_name: 命令别名，不提供则使用函数名
        """
        def decorator(func):
            name = cmd_name or func.__name__
            
            # 保存函数信息
            self.commands[func.__name__] = {
                'func': func,
                'name': func.__name__,
                'cmd_name': name,
                'doc': func.__doc__ or '',
                'module': func.__module__
            }
            
            # 创建包装函数
            @functools.wraps(func)
            def wrapper(*args, **kwargs):   # 函数执行时，都会走这个路径
                # 包装参数
                cmd = self._process_arguments(func, cmd_name, *args, **kwargs)
                # 发送到软件
                return self._execute_command(cmd)
            
            return wrapper
        
        return decorator

    def _process_arguments(self, func, cmd_name, *args, **kwargs):
        """处理参数并生成命令字符串"""
        
        # 获取函数签名
        sig = inspect.signature(func)
        
        # 绑定用户参数
        try:
            bound = sig.bind(*args, **kwargs)
            bound.apply_defaults()
        except TypeError as e:
            raise ValueError(f"参数错误: {e}")
        
        # 收集参数值
        param_values = []
        
        for param_name, param in sig.parameters.items():    # 参数值为空字符串时会加一个空的参数
            value = bound.arguments[param_name]
            
            # 自动展开list/tuple
            if isinstance(value, (list, tuple)):
                for item in value:
                    param_values.append(str(item))
            # 自动展开dict，只取值
            elif isinstance(value, dict):
                for val in value.values():
                    param_values.append(str(val))
            elif isinstance(value, bool):
                param_values.append(str(int(value)))
            elif value is None:
                # 参数值为空字符串时会加一个空的参数
                # 如果希望生成 cmd,a,b,,d;的命令格式，c参数需要忽略参数类型填写：""
                # param_values.append("")     # 加一个空字符串
                continue        # 如果为None则跳过解析
            else:
                # 普通值
                param_values.append(str(value))
        
        # 生成命令字符
        return f"{cmd_name},{','.join(param_values)};" if len(param_values) != 0 else f"{cmd_name};"
    
    def _execute_command(self, cmd) -> Tuple[bool, str, Any]:
        """执行命令（发送到软件）"""
        print(cmd)
        return OSISEngine.GetInstance().OSIS_Run(cmd)
    
    def list_commands(self):
        """列出所有命令"""
        for cmd_name, info in self.commands.items():
            print(f"{cmd_name}: {info['doc'].split(chr(10))[0] if info['doc'] else '无描述'}")
    
    def get_command(self, cmd_name):
        """获取函数信息"""
        return self.commands.get(cmd_name)

# 全局函数注册表实例
REGISTRY = OSISFunctionRegistry()       # 作用为保证参数个数与顺序正常，python函数一定要注册一下


# def deep_merge(source, destination):
#     """递归合并字典，保持源字典的键顺序，并用目标字典的值覆盖相同键的值，返回新的字典对象"""
#     result = {}
    
#     # 首先添加源字典的所有键值对
#     for key, value in source.items():
#         if key in destination:
#             # 如果键在目标字典中也存在
#             if isinstance(value, dict) and isinstance(destination[key], dict):
#                 # 如果两个值都是字典，递归合并
#                 result[key] = deep_merge(value, destination[key])
#             else:
#                 # 否则使用目标字典的值（覆盖）
#                 result[key] = destination[key]
#         else:
#             # 如果键只在源字典中存在，使用源字典的值
#             result[key] = value
    
#     # 然后添加目标字典中独有的键值对
#     for key, value in destination.items():
#         if key not in source:
#             result[key] = value
    
#     return result

# class OSISCommandPacker:
#     """OSIS命令流包装器"""

#     @staticmethod
#     def pack(func_name: str, params: dict) -> str:
#         '''
#         按照python函数名包装函数
        
#         Args:
#             func_name: 函数名
#             params: 参数字典
#         '''
#         info = registry.get_info(func_name)
#         cmd_name = info['cmd']
#         params_default = info['params_default']
#         params_final = deep_merge(params_default, params)   # 和默认参数合并以确保参数个数与顺序正常

#         cmd = cmd_name      # 以命令流名称开头
#         for key in params_final:
#             cmd += ','
#             cmd += str(params_final[key]) if params_final[key] is not None else ''
#         cmd += ';'
#         return cmd

