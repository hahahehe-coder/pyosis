
from ..core import OSISEngine

def osis_node(nNO: int, x: float, y: float, z: float):
    """创建一个节点

    Args:
        nNO (int): 节点编号，从 1 开始编号。
        x (float): 节点X坐标
        y (float): 节点Y坐标
        z (float): 节点Z坐标
        
    Returns:
        tuple (bool, str):
            - bool: 操作是否成功
            - str: 失败原因（如果操作失败）
    """
    e = OSISEngine.GetInstance()
    return e.OSIS_Node(nNO, x, y, z)

def osis_node_del(nNO: int):
    """删除一个节点

    Args:
        nNO (int): 节点编号

    Returns:
        tuple (bool, str):
            - bool: 操作是否成功
            - str: 失败原因（如果操作失败）
    """
    e = OSISEngine.GetInstance()
    return e.OSIS_NodeDel(nNO)

def osis_node_mod(nOld: int, nNew: int):
    """修改一个节点的编号。节点编号存在时，交换

    Args:
        nOld (int): 旧编号
        nNew (int): 新编号

    Returns:
        tuple (bool, str):
            - bool: 操作是否成功
            - str: 失败原因（如果操作失败）
    """
    e = OSISEngine.GetInstance()
    return e.OSIS_NodeMod(nOld, nNew)