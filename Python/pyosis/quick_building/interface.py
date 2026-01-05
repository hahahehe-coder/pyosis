'''
pyosis.quick_building.py.interface 的 Docstring
'''
from typing import Tuple
from ..core import *

def create_simply_small_box_beam(dSpan: float) -> Tuple[bool, str]:
    '''
    快速建模-创建小箱梁

    Args:
        dSpan (float): 跨径
    
    Returns:
        tuple (bool, str): 是否成功，失败原因
    '''
    osis_run("/create,132")
    osis_run("/control,quickCreateModel")
    return True, ""

def create_simply_Tbeam(dSpan: float) -> Tuple[bool, str]:
    '''
    快速建模-创建T梁

    Args:
        dSpan (float): 跨径
    
    Returns:
        tuple (bool, str): 是否成功，失败原因
    '''
    osis_run("/create,133")
    osis_run("/control,quickCreateModel")
    return True, ""

def create_simply_hollow_slab(dSpan: float) -> Tuple[bool, str]:
    '''
    快速建模-创建空心板

    Args:
        dSpan (float): 跨径
    
    Returns:
        tuple (bool, str): 是否成功，失败原因
    '''
    osis_run("/create,134")
    osis_run("/control,quickCreateModel")
    return True, ""

def create_simply_continuous_Tbeam(dSpan: float) -> Tuple[bool, str]:
    '''
    快速建模-创建连续T梁

    Args:
        dSpan (float): 跨径
    
    Returns:
        tuple (bool, str): 是否成功，失败原因
    '''
    osis_run("/create,113")
    osis_run("/control,quickCreateModel")
    return True, ""

def create_simply_continuous_small_box(dSpan: float) -> Tuple[bool, str]:
    '''
    快速建模-创建连续小箱梁

    Args:
        dSpan (float): 跨径
    
    Returns:
        tuple (bool, str): 是否成功，失败原因
    '''
    osis_run("/create,112")
    osis_run("/control,quickCreateModel")
    return True, ""
