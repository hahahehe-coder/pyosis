#include "stdafx.h"
#include "PyInterface.h"
#include "Boundary/BoundaryInc.h"

/// <summary>
/// 边界条件
/// </summary>
/// <param name="nBd">边界编号</param>
/// <param name="eBoundaryType">边界类型：一般边界 主从约束 释放梁端约束 节点弹性支撑</param>
/// <param name="kwargs"></param>
/// <returns></returns>
std::pair<bool, std::string> PyInterface::OSIS_Boundary(const int nBd, const std::string eBoundaryType, const py::dict kwargs)
{
	return { true, "" };
}

