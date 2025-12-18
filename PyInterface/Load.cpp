#include "stdafx.h"
#include "PyInterface.h"

/// <summary>
/// 荷载
/// </summary>
/// <param name="eLoadType">荷载类型</param>
/// <param name="nLC">荷载工况编号</param>
/// <param name="kwargs">荷载参数</param>
/// <returns></returns>
std::pair<bool, std::string> PyInterface::OSIS_Load(const std::string eLoadType, const std::string strLCName, const py::dict kwargs)
{
	std::string errorCode;
	PrepEnum::Load eLT = PrepEnum::Load::Unassigned;
	if (!GetCommand()->StrToLoad(eLT, eLoadType, 1))
	{
		errorCode = "参数 荷载类型 错误！";
		return { false, errorCode };
	}
	PREP::LoadCaseInfo* pLc = GET_PREP_LC()->getInfo(strLCName);
	if (!pLc) 
	{
		errorCode = "荷载工况" + strLCName + "不存在！";
		//g_pWarningList->AddToTemperary(4, m_ErrCode, -1);
		return { false, errorCode };
	}
	switch (eLT) {
		//case PrepEnum::Load::Gravity: SET_GRAVITY(AppendGravity, ResetGravity)        break;

	case PrepEnum::Load::NForce:
	{

	}
		break;
	default:
	{
		errorCode = "暂不支持该类型的荷载！";
		return { false, errorCode };
	}

		break;
	}

	return { true, errorCode };
}