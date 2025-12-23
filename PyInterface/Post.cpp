#include "stdafx.h"
#include "PyInterface.h"
#include "YILResult/yilLoadCaseResult.h"

/// <summary>
/// 后处理 输出内力计算工况结果表格
/// </summary>
/// <param name="strLCName"></param>
/// <param name="eDataItem"></param>
/// <param name="eElementType"></param>
/// <returns></returns>
std::pair<bool, std::string> PyInterface::OSIS_PrnLcr(const std::string strLCName, const std::string eDataItem, const std::string eElementType)
{
	std::string errorCode;

	return { true, errorCode };
}


std::tuple<bool, std::string, py::dict> PyInterface::OSIS_ElemForce(const std::string strLCName, const std::string eDataItem, const std::string eElementType)
{
	std::string errorCode;
	py::dict data;

	yilEnumType::StageDataItem eDI = yilEnumType::StageDataItem::SD_NULL;
	yilEnumType::Element eEM = yilEnumType::Element::ElementNull;
	if (!GetCommand()->StrToStageDataItem(eDI, eDataItem, 2))
	{
		errorCode = "参数 StageDataItem 错误！";
	}
	if (!GetCommand()->StrToElement(eEM, eElementType, 2))
	{
		errorCode = "参数 ElementType 错误！";
		return std::make_tuple(false, errorCode, data);
	}
	std::map<yilCString, _Elongation> mapTendElongData;	//钢束名 - 伸长量信息
	yilLoadCaseResult* pLoadCaseResult = GetProject()->GetSolver()->ReadLoadCaseResult(strLCName);
	pLoadCaseResult->OutputTableData(*GetProject()->GetOutputTableData(), strLCName, eEM, eDI, "", &mapTendElongData);

	auto* pEleListResult = pLoadCaseResult->GetEleResult(eDI); // 可能有错
	yilForceResultD_COMMON elemForceData = pEleListResult->m_ForceData;

	data["Element"] = py::list();
	data["Node"] = py::list();
	data["Fx"] = py::list();
	data["Fy"] = py::list();
	data["Fz"] = py::list();
	data["Mx"] = py::list();
	data["My"] = py::list();
	data["Mz"] = py::list();
	data["Tend"] = py::list();
	for (auto& item : elemForceData.m_ivElem)
	{
		data["Element"].attr("append")(item);
	}
	for (auto& item : elemForceData.m_ivNode)
	{
		data["Node"].attr("append")(item);
	}
	for (auto& item : elemForceData.m_dvFx)
	{
		data["Fx"].attr("append")(item);
	}for (auto& item : elemForceData.m_dvFy)
	{
		data["Fy"].attr("append")(item);
	}for (auto& item : elemForceData.m_dvFz)
	{
		data["Fz"].attr("append")(item);
	}for (auto& item : elemForceData.m_dvMx)
	{
		data["Mx"].attr("append")(item);
	}for (auto& item : elemForceData.m_dvMy)
	{
		data["My"].attr("append")(item);
	}for (auto& item : elemForceData.m_dvMz)
	{
		data["Mz"].attr("append")(item);
	}for (auto& item : elemForceData.m_dvMb)
	{
		data["Tend"].attr("append")(item);
	}

	return std::make_tuple(true, errorCode, data);
}