
from ..core import OSISEngine

def osis_node(nNO: int, x: float, y: float, z: float):
    e = OSISEngine.GetInstance()
    return e.OSIS_Node(nNO, x, y, z)

def osis_node_del(nNO: int):
    e = OSISEngine.GetInstance()
    return e.OSIS_NodeDel(nNO)

def osis_node_mod(nOld: int, nNew: int):
    e = OSISEngine.GetInstance()
    return e.OSIS_NodeMod(nOld, nNew)