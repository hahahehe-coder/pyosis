"""
pyosis.core 的 Docstring
--------

OSISEngine中的函数，将会分发到各个模块，被再次封装，意图为：
1.避免麻烦的OSISEngine::GetInstance().OSIS_XXX调用方式
2.将各个模块分类开以实现逻辑清晰
3.将函数命名格式转换成python的常用格式
"""

from .engine import OSISEngine