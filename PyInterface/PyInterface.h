#pragma once

// DLL导出宏
#ifdef PYINTERFACE_EXPORTS
#define PYINTERFACE_API __declspec(dllexport)
#else
#define PYINTERFACE_API __declspec(dllimport)
#endif

#include <string>
#include <tuple>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/embed.h>
#include <locale>
#include <codecvt>

namespace py = pybind11;
#include "yilBaseCommand/yilBaseCommandInc.h"
#include "yilCommand/yilCommandInc.h"
#include "YILProjectGroup/yilProjectGroupInc.h"

typedef void (*ReplotCallback)();

class PYINTERFACE_API PyInterface
{
private:
	//yilProject* GetProject();
	yilProjectGroup* m_pProjectGroup;
	//yilBaseCommand* GetCommand();
	ReplotCallback Replot;
	PyInterface() = default;

public:
	static PyInterface& GetInstance() {
		static PyInterface instance;
		return instance;
	}
	~PyInterface() = default; // 使用默认析构函数

	// 禁止拷贝和赋值
	PyInterface(const PyInterface&) = delete;
	PyInterface& operator=(const PyInterface&) = delete;

	void SetProjectGroup(yilProjectGroup* pProjectGroup)
	{
		m_pProjectGroup = pProjectGroup;
	}

	yilProject* GetProject()
	{
		return m_pProjectGroup->GetProject();
	}

	yilBaseCommand* GetCommand()
	{
		return (yilBaseCommand*)m_pProjectGroup->GetyilCommand();
	}

	//void SetProject(yilProject* GetProject()) {
	//	GetProject() = GetProject();
	//}

	//void SetCommand(yilBaseCommand* GetCommand())
	//{
	//	GetCommand() = GetCommand();
	//}

	void SetReplotCallback(ReplotCallback callback)
	{
		Replot = callback;
	}

	// ========================================================================
	// ===============================前处理接口================================
	// ========================================================================

	/// <summary>
	/// 定义加速度
	/// </summary>
	/// <param name="dG">加速度 9.8066</param>
	/// <returns>是否成功，失败原因</returns>
	std::pair<bool, std::string> OSIS_Acel(double dG);

	/// <summary>
	/// 计算预应力开关
	/// </summary>
	/// <param name="bFlag">1：开 0：关</param>
	/// <returns>是否成功，失败原因</returns>
	std::pair<bool, std::string> OSIS_CalcTendon(bool bFlag);

	/// <summary>
	/// 计算并发反力开关
	/// </summary>
	/// <param name="bFlag">1：开 0：关</param>
	/// <returns>是否成功，失败原因</returns>
	std::pair<bool, std::string> OSIS_CalcConForce(bool bFlag);

	/// <summary>
	/// 计算收缩开关
	/// </summary>
	/// <param name="bFlag">1：开 0：关</param>
	/// <returns>是否成功，失败原因</returns>
	std::pair<bool, std::string> OSIS_CalcShrink(bool bFlag);

	/// <summary>
	/// 计算徐变开关
	/// </summary>
	/// <param name="bFlag">1：开 0：关</param>
	/// <returns>是否成功，失败原因</returns>
	std::pair<bool, std::string> OSIS_CalcCreep(bool bFlag);

	/// <summary>
	/// 计算剪切变形开关
	/// </summary>
	/// <param name="bFlag">1：开 0：关</param>
	/// <returns>是否成功，失败原因</returns>
	std::pair<bool, std::string> OSIS_CalcShear(bool bFlag);

	/// <summary>
	/// 计算钢束松弛开关
	/// </summary>
	/// <param name="bFlag">1：开 0：关</param>
	/// <returns>是否成功，失败原因</returns>
	std::pair<bool, std::string> OSIS_CalcRlx(bool bFlag);

	/// <summary>
	/// 修改变截面局部坐标系开关
	/// </summary>
	/// <param name="bFlag">1：开 0：关</param>
	/// <returns>是否成功，失败原因</returns>
	std::pair<bool, std::string> OSIS_ModLocCoor(bool bFlag);

	/// <summary>
	/// 考虑钢束自重开关
	/// </summary>
	/// <param name="bFlag">1：开 0：关</param>
	/// <returns>是否成功，失败原因</returns>
	std::pair<bool, std::string> OSIS_IncTendon(bool bFlag);

	/// <summary>
	/// 非线性控制开关
	/// </summary>
	/// <param name="bGeom">0：关闭几何非线性开关 1：打开几何非线性开关、大位移大转角</param>
	/// <param name="bLink">0：不考虑非线性连接单元 1：考虑非线性连接单元</param>
	/// <returns>是否成功，失败原因</returns>
	std::pair<bool, std::string> OSIS_NL(bool bGeom, bool bLink);

	/// <summary>
	/// 求解设置线性搜索
	/// </summary>
	/// <param name="bFlag">0：非线性搜索 1：线性搜索</param>
	/// <returns>是否成功，失败原因</returns>
	std::pair<bool, std::string> OSIS_LnSrch(bool bFlag);

	/// <summary>
	/// 是否定义自动计算时间荷载步
	/// </summary>
	/// <param name="bFlag">0：不自动计算时间荷载步 1：自动计算时间荷载步</param>
	/// <returns>是否成功，失败原因</returns>
	std::pair<bool, std::string> OSIS_AutoTs(bool bFlag);

	/// <summary>
	/// 定义模态分析所需的特征值最大数目
	/// </summary>
	/// <param name="nNO">需要计算的特征值最大数目（缺省值：1）</param>
	/// <returns>是否成功，失败原因</returns>
	std::pair<bool, std::string> OSIS_ModOpt(int nNO);

	/// <summary>
	/// 重绘窗口
	/// </summary>
	/// <returns>是否成功，失败原因</returns>
	std::pair<bool, std::string> OSIS_Replot();

	/// <summary>
	/// 清空项目
	/// </summary>
	/// <returns>是否成功，失败原因</returns>
	std::pair<bool, std::string> OSIS_Clear();

	/// <summary>
	/// 求解
	/// </summary>
	/// <returns>是否成功，失败原因</returns>
	std::pair<bool, std::string> OSIS_Solve();

	/// <summary>
	/// 创建或修改收缩徐变特性
	/// </summary>
	/// <param name="nNO">收缩徐变特性编号</param>
	/// <param name="strName">名称</param>
	/// <param name="dAvgHumidity">年平均湿度（百分比）</param>
	/// <param name="nBirthTime">混凝土龄期（天数）</param>
	/// <param name="dTypeCoeff">水泥种类系数</param>
	/// <param name="nBirthByShrinking">收缩开始时的混凝土龄期（天数）</param>
	/// <returns>是否成功，失败原因</returns>
	std::pair<bool, std::string> OSIS_CrpShrk(const int nNO, const std::string strName, const double dAvgHumidity, const int nBirthTime, const double dTypeCoeff, const int nBirthByShrinking);

	/// <summary>
	/// 定义或修改混凝土材料
	/// </summary>
	/// <param name="nMat">编号</param>
	/// <param name="strName">材料别名</param>
	/// <param name="eMaterialType">
	///		CONC = 混凝土
	///		STEEL = 钢材
	///		PRESTRESSED = 预应力材料
	///		REBAR = 普通钢筋
	/// </param>
	/// <param name="eCode">
	///		混凝土 JTG3362_2018，JTGD62_2004
	///		钢材 JTGD64_2015
	///		预应力 JTG3362_2018，JTGD62_2004
	///		普通钢筋 JTG3362_2018，JTGD62_2004
	/// </param>
	/// <param name="eGrade">等级牌号</param>
	/// <param name="nCrepShrk">收缩徐变特性编号，缺省时值为-1</param>
	/// <param name="dDmp">材料阻尼比</param>
	/// <returns>是否成功，失败原因</returns>
	std::pair<bool, std::string> OSIS_Material(const int nMat, const std::string strName, const std::string eMaterialType, const std::string eCode, const std::string eGrade, const int nCrepShrk, const double dDmp);

	/// <summary>
	/// 删除材料
	/// </summary>
	/// <param name="nMat">材料编号</param>
	/// <returns></returns>
	std::pair<bool, std::string> OSIS_MaterialDel(const int nMat);

	/// <summary>
	/// 修改材料编号
	/// </summary>
	/// <param name="nOld">旧编号</param>
	/// <param name="nNew">新编号</param>
	/// <returns></returns>
	std::pair<bool, std::string> OSIS_MaterialMod(const int nOld, const int nNew);

	/// <summary>
	/// 定义或修改截面
	/// </summary>
	/// <param name="nSec">截面编号</param>
	/// <param name="strName">截面名</param>
	/// <param name="eSectionType">截面类型</param>
	/// <param name="kwargs">截面参数</param>
	/// <returns>是否成功，失败原因</returns>
	std::pair<bool, std::string> OSIS_Section(const int nSec, const std::string strName, const std::string eSectionType, const py::dict kwargs);

	/// <summary>
	/// 删除截面
	/// </summary>
	/// <param name="nSec">截面编号</param>
	/// <returns></returns>
	std::pair<bool, std::string> OSIS_SectionDel(const int nSec);

	/// <summary>
	/// 修改截面编号
	/// </summary>
	/// <param name="nOldIndex">旧编号</param>
	/// <param name="nNewIndex">新编号</param>
	/// <returns></returns>
	std::pair<bool, std::string> OSIS_SectionMod(const int nOld, const int nNew);

	/// <summary>
	/// 定义截面偏心
	/// </summary>
	/// <param name="nSec">截面编号</param>
	/// <param name="offsetTypeY">截面局部坐标系下Y方向的偏心，可选择Left、Middle、Right、Manual</param>
	/// <param name="offsetValueY">Y方向的偏心数值，在Y方向偏心为Manual时该项起作用</param>
	/// <param name="offsetTypeZ">截面局部坐标系下Z方向的偏心，可选择Top、Center、Bottom、Manual</param>
	/// <param name="offsetValueZ">Z方向的偏心数值，在Z方向偏心为Manual时该项起作用</param>
	/// <returns>是否成功，失败原因</returns>
	std::pair<bool, std::string> OSIS_SectionOffset(const int nSec, const std::string offsetTypeY, const double offsetValueY, const std::string offsetTypeZ, const double offsetValueZ);

	/// <summary>
	/// 定义截面网格划分
	/// </summary>
	/// <param name="nSec">截面编号</param>
	/// <param name="bMeshMethod">网格划分方法，0=自动划分，1=手动划分</param>
	/// <param name="dMeshSize">网格划分尺寸，在MeshMethod=1时该项起作用</param>
	/// <returns>是否成功，失败原因</returns>
	std::pair<bool, std::string> OSIS_SectionMesh(const int nSec, bool bMeshMethod, double dMeshSize);

	/// <summary>
	/// 定义节点
	/// </summary>
	/// <param name="nNO">节点编号，为-1代表缺省</param>
	/// <param name="x"></param>
	/// <param name="y"></param>
	/// <param name="z"></param>
	/// <returns>是否成功，失败原因</returns>
	std::pair<bool, std::string> OSIS_Node(int nNO, double x, double y, double z);

	/// <summary>
	/// 删除节点
	/// </summary>
	/// <param name="nNO">节点编号</param>
	/// <returns></returns>
	std::pair<bool, std::string> OSIS_NodeDel(const int nNO);

	/// <summary>
	/// 修改节点编号，节点编号存在时，交换
	/// </summary>
	/// <param name="nOld">旧编号</param>
	/// <param name="nNew">新编号</param>
	/// <returns></returns>
	std::pair<bool, std::string> OSIS_NodeMod(const int nOld, const int nNew);

	/// <summary>
	/// 创建或修改单元
	/// </summary>
	/// <param name="nEle">单元编号</param>
	/// <param name="eElementType">单元类型: Beam3D Truss Spring Cable</param>
	/// <param name="kwargs">单元参数</param>
	/// <returns>是否成功，失败原因</returns>
	std::pair<bool, std::string> OSIS_Element(int nEle /*= -1*/, const std::string eElementType, const py::dict kwargs);

	/// <summary>
	/// 删除单元
	/// </summary>
	/// <param name="nEle">单元编号</param>
	/// <returns></returns>
	std::pair<bool, std::string> OSIS_ElementDel(const int nEle);

	/// <summary>
	/// 修改单元编号，单元编号存在时，交换
	/// </summary>
	/// <param name="nOld">旧编号</param>
	/// <param name="nNew">新编号</param>
	/// <returns></returns>
	std::pair<bool, std::string> OSIS_ElementMod(const int nOld, const int nNew);

	/// <summary>
	/// 边界条件
	/// </summary>
	/// <param name="nBd">边界编号</param>
	/// <param name="eBoundaryType">边界类型：一般边界 主从约束 释放梁端约束 节点弹性支撑</param>
	/// <param name="kwargs">边界参数</param>
	/// <returns></returns>
	std::pair<bool, std::string> OSIS_Boundary(const int nBd, const std::string eBoundaryType, const py::dict kwargs);

	/// <summary>
	/// 分配边界给节点(一般支撑，节点弹性支撑)
	/// </summary>
	/// <param name="nBd">边界编号</param>
	/// <param name="eOP">操作: a = 添加，s = 替换，r = 移除，aa = 添加全部，ra = 移除全部</param>
	/// <param name="nodeNOs">待操作的节点编号</param>
	/// <returns></returns>
	std::pair<bool, std::string> OSIS_AsgnBd(const int nBd, const std::string eOP, const py::list nodeNOs);

	/// <summary>
	/// 定义或修改荷载工况
	/// </summary>
	/// <param name="strName">别名</param>
	/// <param name="eType">类型 USER\D\DC\DW\DD\CS</param>
	/// <param name="dScalar">系数，默认为1</param>
	/// <param name="strPrompt">说明，默认为""</param>
	/// <returns></returns>
	std::pair<bool, std::string> OSIS_LoadCase(const std::string strName, const std::string eLoadCaseType, const double dScalar, const std::string strPrompt);

	/// <summary>
	/// 荷载
	/// </summary>
	/// <param name="eLoadType">荷载类型</param>
	/// <param name="nLC">荷载工况编号</param>
	/// <param name="kwargs">荷载参数</param>
	/// <returns></returns>
	std::pair<bool, std::string> OSIS_Load(const std::string eLoadType, const std::string strLCName, const py::dict kwargs);

	// ========================================================================
	// ===============================后处理接口================================
	// ========================================================================


	/// <summary>
	/// 后处理 输出内力计算工况结果表格
	/// </summary>
	/// <param name="strLCName"></param>
	/// <param name="eDataItem"></param>
	/// <param name="eElementType"></param>
	/// <returns></returns>
	std::pair<bool, std::string> OSIS_PrnLcr(const std::string strLCName, const std::string eDataItem, const std::string eElementType);


	std::tuple<bool, std::string, py::dict> OSIS_ElemForce(const std::string strLCName, const std::string eDataItem, const std::string eElementType);

	// ========================================================================
	// ==============================复杂函数接口===============================
	// ========================================================================

	std::pair<bool, std::string> OSIS_QBHollowSlab(const py::list spans, const bool bIsElasticConnection);
};

// 将 UTF-8 string 转换为 wstring 用于显示
std::wstring utf8_to_wstring(const std::string& utf8_str);

std::string wstring_to_string(const std::wstring& wide_str);

std::string utf8_to_wide(const std::string& utf8_str);

// 安全参数解析 字典版
template<typename T>
inline T safe_cast(const py::dict& kwargs, const std::string& key) {
	if (!kwargs.contains(key)) {
		throw std::runtime_error("参数 '" + key + "' 不存在!");
	}

	try {
		return kwargs[key.c_str()].cast<T>();
	}
	catch (const py::cast_error& e) {
		throw std::runtime_error("参数 '" + key + "' 解析错误: " + e.what());
	}
}

//// 安全参数解析 单个参数版
//template<typename T>
//inline T safe_cast(const py::dict& kwargs, const std::string& key) {
//	if (!kwargs.contains(key)) {
//		throw std::runtime_error("参数 '" + key + "' 不存在!");
//	}
//
//	try {
//		auto a = kwargs[key.c_str()];
//		return kwargs[key.c_str()].cast<T>();
//	}
//	catch (const py::cast_error& e) {
//		throw std::runtime_error("参数 '" + key + "' 解析错误: " + e.what());
//	}
//}