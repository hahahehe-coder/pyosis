#include "stdafx.h"
#include "PyInterface.h"
#include "Control/ControlInc.h"


std::pair<bool, std::string> PyInterface::OSIS_Acel(double dG)
{
	std::string errorCode;
	auto* pCtrlMgr = GET_PREP_CTRL();
	if (!pCtrlMgr)
	{
		errorCode = "内部错误，控制管理器初始化失败！";
		return { false, errorCode };
	}
	pCtrlMgr->setG(dG);
	return { true, "" };
}

std::pair<bool, std::string> PyInterface::OSIS_CalcTendon(bool bFlag)
{
	std::string errorCode;
	auto* pCtrlMgr = GET_PREP_CTRL();
	if (!pCtrlMgr)
	{
		errorCode = "内部错误，控制管理器初始化失败！";
		return { false, errorCode };
	}
	pCtrlMgr->setCalcTendon(bFlag);
	return { true, "" };
}

std::pair<bool, std::string> PyInterface::OSIS_CalcConForce(bool bFlag)
{
	std::string errorCode;
	auto* pCtrlMgr = GET_PREP_CTRL();
	if (!pCtrlMgr)
	{
		errorCode = "内部错误，控制管理器初始化失败！";
		return { false, errorCode };
	}
	pCtrlMgr->setCalcConForce(bFlag);
	return { true, "" };
}

std::pair<bool, std::string> PyInterface::OSIS_CalcShrink(bool bFlag)
{
	std::string errorCode;
	auto* pCtrlMgr = GET_PREP_CTRL();
	if (!pCtrlMgr)
	{
		errorCode = "内部错误，控制管理器初始化失败！";
		return { false, errorCode };
	}
	pCtrlMgr->setCalcShrink(bFlag);
	return { true, "" };
}

std::pair<bool, std::string> PyInterface::OSIS_CalcCreep(bool bFlag)
{
	std::string errorCode;
	auto* pCtrlMgr = GET_PREP_CTRL();
	if (!pCtrlMgr)
	{
		errorCode = "内部错误，控制管理器初始化失败！";
		return { false, errorCode };
	}
	pCtrlMgr->setCalcCreep(bFlag);
	return { true, "" };
}

std::pair<bool, std::string> PyInterface::OSIS_CalcShear(bool bFlag)
{
	std::string errorCode;
	auto* pCtrlMgr = GET_PREP_CTRL();
	if (!pCtrlMgr)
	{
		errorCode = "内部错误，控制管理器初始化失败！";
		return { false, errorCode };
	}
	pCtrlMgr->setCalcShear(bFlag);
	return { true, "" };
}

std::pair<bool, std::string> PyInterface::OSIS_CalcRlx(bool bFlag)
{
	std::string errorCode;
	auto* pCtrlMgr = GET_PREP_CTRL();
	if (!pCtrlMgr)
	{
		errorCode = "内部错误，控制管理器初始化失败！";
		return { false, errorCode };
	}
	pCtrlMgr->setCalcRelaxation(bFlag);
	return { true, "" };
}

std::pair<bool, std::string> PyInterface::OSIS_ModLocCoor(bool bFlag)
{
	std::string errorCode;
	auto* pCtrlMgr = GET_PREP_CTRL();
	if (!pCtrlMgr)
	{
		errorCode = "内部错误，控制管理器初始化失败！";
		return { false, errorCode };
	}
	pCtrlMgr->setModLocCoor(bFlag);
	return { true, "" };
}

std::pair<bool, std::string> PyInterface::OSIS_IncTendon(bool bFlag)
{
	std::string errorCode;
	auto* pCtrlMgr = GET_PREP_CTRL();
	if (!pCtrlMgr)
	{
		errorCode = "内部错误，控制管理器初始化失败！";
		return { false, errorCode };
	}
	pCtrlMgr->setIncTendon(bFlag);
	return { true, "" };
}

std::pair<bool, std::string> PyInterface::OSIS_NL(bool bGeom, bool bLink)
{
	std::string errorCode;
	auto* pCtrlMgr = GET_PREP_CTRL();
	if (!pCtrlMgr)
	{
		errorCode = "内部错误，控制管理器初始化失败！";
		return { false, errorCode };
	}
	pCtrlMgr->setGeom(bGeom);
	pCtrlMgr->setLink(bLink);
	return { true, "" };
}

std::pair<bool, std::string> PyInterface::OSIS_LnSrch(bool bFlag)
{
	std::string errorCode;
	auto* pCtrlMgr = GET_PREP_CTRL();
	if (!pCtrlMgr)
	{
		errorCode = "内部错误，控制管理器初始化失败！";
		return { false, errorCode };
	}
	pCtrlMgr->setLSKey(bFlag);
	return { true, "" };
}

std::pair<bool, std::string> PyInterface::OSIS_AutoTs(bool bFlag)
{
	std::string errorCode;
	auto* pCtrlMgr = GET_PREP_CTRL();
	if (!pCtrlMgr)
	{
		errorCode = "内部错误，控制管理器初始化失败！";
		return { false, errorCode };
	}
	pCtrlMgr->setATSKey(bFlag);
	return { true, "" };
}

std::pair<bool, std::string> PyInterface::OSIS_ModOpt(int nNO)
{
	std::string errorCode;
	auto* pCtrlMgr = GET_PREP_CTRL();
	if (!pCtrlMgr)
	{
		errorCode = "内部错误，控制管理器初始化失败！";
		return { false, errorCode };
	}
	pCtrlMgr->setVibModeNum(nNO);
	return { true, "" };
}

