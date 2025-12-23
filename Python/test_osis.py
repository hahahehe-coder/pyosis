from pyosis.control import *
from pyosis.general import *
from pyosis.section import *
from pyosis.material import *
from pyosis.node import *
from pyosis.element import *
from pyosis.boundary import *
from pyosis.load import *
from pyosis.post import *
import json

osis_clear()

osis_acel(9.8066)
osis_calc_tendon(1)
osis_calc_con_force(1)
osis_calc_shrink(1)
osis_calc_creep(1)
osis_calc_shear(1)
osis_calc_rlx(1)
osis_mod_loc_coor(0)
osis_inc_tendon(1)
osis_nl(0, 0)
osis_ln_srch(0)
osis_auto_ts(0)
osis_mod_opt(0)

osis_section(1, "圆形截面1", "Circle", {"CircleType": "Hollow", "D": 0.219,"Tw": 0.012})
osis_section(2, "圆形截面2", "Circle", {"CircleType": "Hollow", "D": 0.180,"Tw": 0.008})
osis_section(3, "圆形截面3", "Circle", {"CircleType": "Hollow", "D": 0.114,"Tw": 0.005})
osis_section(4, "圆形截面4", "Circle", {"CircleType": "Hollow", "D": 0.089,"Tw": 0.004})
osis_section(5, "圆形截面5", "Circle", {"CircleType": "Hollow", "D": 0.045,"Tw": 0.003})

osis_material(1, "钢材1", "STEEL", "JTGD64_2015", "Q345", -1, 0.05)

# 固定节点（x,y单位：m）
osis_node(1, 0, 5, 0)
osis_node(2, 15, 5, 0)
# 荷载作用节点
osis_node(3, 7.5, 0, 0)
osis_node(4, 20, 0, 0)

# osis_element(1, "Truss", {"nNode1": 1, "nNode2": 3, "nMat": 1, "nSec1": 1, "nSec2": 1, "dStrain": 0.00})
# osis_element(2, "Truss", {"nNode1": 2, "nNode2": 3, "nMat": 1, "nSec1": 1, "nSec2": 1, "dStrain": 0.00})
# osis_element(3, "Truss", {"nNode1": 2, "nNode2": 4, "nMat": 1, "nSec1": 1, "nSec2": 1, "dStrain": 0.00})
# osis_element(4, "Truss", {"nNode1": 3, "nNode2": 4, "nMat": 1, "nSec1": 1, "nSec2": 1, "dStrain": 0.00})

osis_element(1, "BEAM3D", {"nNode1": 1, "nNode2": 3, "nMat": 1, "nSec1": 4, "nSec2": 4, "nYTrans": 1, "nZTrans": 1, "dStrain": 0.00, "bFlag": 0, "dTheta": 0.00, "bWarping": 0})
osis_element(2, "BEAM3D", {"nNode1": 2, "nNode2": 3, "nMat": 1, "nSec1": 5, "nSec2": 5, "nYTrans": 1, "nZTrans": 1, "dStrain": 0.00, "bFlag": 0, "dTheta": 0.00, "bWarping": 0})
osis_element(3, "BEAM3D", {"nNode1": 2, "nNode2": 4, "nMat": 1, "nSec1": 5, "nSec2": 5, "nYTrans": 1, "nZTrans": 1, "dStrain": 0.00, "bFlag": 0, "dTheta": 0.00, "bWarping": 0})
osis_element(4, "BEAM3D", {"nNode1": 3, "nNode2": 4, "nMat": 1, "nSec1": 5, "nSec2": 5, "nYTrans": 1, "nZTrans": 1, "dStrain": 0.00, "bFlag": 0, "dTheta": 0.00, "bWarping": 0})
            

osis_boundary(1, "GENERAL", {"nCoor": -1, "bDX": 1, "bDY": 1, "bDZ": 1, "bRX": 1, "bRY": 1, "bRZ": 1, "bRW": 1})
osis_assign_boundary(1, "a", [1, 2])

osis_loadcase("自定义工况1", "USER", 1, "施加于节点3和4的两个力")
osis_load("NFORCE", "自定义工况1", {"nNO": 3, "dFX": 0, "dFY": -1000000, "dFZ": 0, "dMX": 0, "dMY": 0, "dMZ": 0})
osis_load("NFORCE", "自定义工况1", {"nNO": 4, "dFX": 200000, "dFY": 0, "dFZ": 0, "dMX": 0, "dMY": 0, "dMZ": 0})

osis_solve()

isok, error, ef = osis_elem_force("自定义工况1", "EF", "BEAM3D")

def dict_to_json_txt(data, filename):
    """将字典以JSON格式写入文件"""
    with open(filename, 'w', encoding='utf-8') as f:
        json.dump(data, f, indent=4)
    
    print(f"字典已写入文件: {filename}")

# 使用
dict_to_json_txt(ef, "output.json")
