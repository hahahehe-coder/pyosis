#include "stdafx.h"
#include "PyInterface.h"


/// <summary>
/// 定义或修改荷载工况
/// </summary>
/// <param name="strName">别名</param>
/// <param name="eType">类型 USER\D\DC\DW\DD\CS</param>
/// <param name="dScalar">系数</param>
/// <param name="strPrompt">说明</param>
/// <returns></returns>
std::pair<bool, std::string> PyInterface::OSIS_LoadCase(const std::string strLCName, const std::string eLoadCaseType, const double dScalar, const std::string strPrompt)
{
	std::string errorCode;
	PrepEnum::LoadCase eLC;
	if (!GetCommand()->StrToLoadCase(eLC, eLoadCaseType, 2))
	{
		errorCode = "参数 荷载工况类型 错误！";
		return { false, errorCode };
	}

	auto* pLCM = GET_PREP_LC();
	PREP::LoadCaseInfo* pInfo = pLCM->getInfo(strLCName);
	if (pInfo) {		// 修改

		//yilCString strCommand;
		//pInfo->formatAPDL(strCommand);
		//PUSH_SHADOW_CMD(THIS_IS_MOD, strCommand);

		pInfo->setType(eLC);
		pInfo->setScalar(dScalar);
		pInfo->setPrompt(strPrompt);
	}
	else				// 创建
	{
		pInfo = new PREP::LoadCaseInfo();
		pInfo->setName(strLCName);
		pInfo->setType(eLC);
		pInfo->setScalar(dScalar);
		pInfo->setPrompt(strPrompt);
		if (!pLCM->append(pInfo)) {

			delete pInfo; pInfo = nullptr;
			errorCode = "创建荷载工况失败！";
			return { false, errorCode };
		}

		//压入影子命令
		//yilCString strCommand;
		//strCommand.Format(_T("LoadCaseDel,%s"), strName.c_str());
		//PUSH_SHADOW_CMD(THIS_IS_DEF, strCommand);
	}


	GetProject()->GetPlotControl()->StructTreeChangedOn();
	GetProject()->GetPlotControl()->LoadCaseTreeChangedOn();
	return { true, errorCode };
}

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
	switch (eLT) 
	{
		//case PrepEnum::Load::Gravity: SET_GRAVITY(AppendGravity, ResetGravity)        break;

	case PrepEnum::Load::NForce:
	{
		int nNO;
		double dFX, dFY, dFZ, dMX, dMY, dMZ;
		try {
			nNO = safe_cast<int>(kwargs, "nNO");
			dFX = safe_cast<double>(kwargs, "dFX");
			dFY = safe_cast<double>(kwargs, "dFY");
			dFZ = safe_cast<double>(kwargs, "dFZ");
			dMX = safe_cast<double>(kwargs, "dMX");
			dMY = safe_cast<double>(kwargs, "dMY");
			dMZ = safe_cast<double>(kwargs, "dMZ");
		}
		catch (const std::runtime_error& e) {
			// 处理错误
			return { false, e.what() };
		}

		if ((std::abs(dFX) + std::abs(dFY) + std::abs(dFZ) +
			std::abs(dMX) + std::abs(dMY) + std::abs(dMZ)) < OSIS_PRECISION)
		{
			errorCode = "荷载" + std::to_string(nNO) + "节点荷载值不能全部为零！";
			//g_pWarningList->AddToTemperary(4, nNO, -1);
			return { false, errorCode };
		}

		using Info = PREP::NForceInfo;
		if (!pLc->getInfo<Info>(nNO))		// 创建荷载
		{
			auto* pUnit = GetProject()->GetUnit();
			dFX = pUnit->LocalForceToSI(dFX);
			dFY = pUnit->LocalForceToSI(dFY);
			dFZ = pUnit->LocalForceToSI(dFZ);
			dMX = pUnit->LocalMomentToSI(dMX);
			dMY = pUnit->LocalMomentToSI(dMY);
			dMZ = pUnit->LocalMomentToSI(dMZ);
			Info* pLdInfo = new Info();

			if (!pLdInfo->setEntityNO(nNO)) {
				delete pLdInfo; pLdInfo = nullptr;
				errorCode = "荷载作用的节点" + std::to_string(nNO) + "设定失败！";
				return {false, errorCode };
			}

			pLdInfo->setType(PrepEnum::Load::NForce);
			pLdInfo->setP(std::vector<double>{ dFX, dFY, dFZ, dMX, dMY, dMZ });

			if (!pLc->append<Info>(pLdInfo)) 
			{

				delete pLdInfo; pLdInfo = nullptr;
				errorCode = "荷载加入荷载工况" + strLCName + "失败！";
				return { false, errorCode };
			}

			////压入影子命令
			//yilCString strCommand;
			//strCommand.Format(_T("LoadDel,NFORCE,%s,%d"), _lc->getName().c_str(), _entity);
			//PUSH_SHADOW_CMD(THIS_IS_DEF, strCommand);
		}
		else					// 修改荷载
		{
			Info* pLdInfo = pLc->getInfo<Info>(nNO);
			if (!pLdInfo) 
			{
				errorCode = "节点" + std::to_string(nNO) + "上的荷载修改失败！";
				return { false, errorCode };
			}

			if (!pLdInfo->check(std::vector<int>{}))
			{
				errorCode = "节点" + std::to_string(nNO) + "上的荷载修改失败！";
				return { false, errorCode };
			}

			////压入影子命令
			////修改前压入，如果失败再弹出（经过数据检查的覆盖性操作几乎不会失败）
			//yilCString strCommand;
			//pLdInfo->formatAPDL(strCommand);
			//PUSH_SHADOW_CMD(THIS_IS_MOD, strCommand);

			pLdInfo->setP(std::vector<double>{ dFX, dFY, dFZ, dMX, dMY, dMZ });
		}
	}
		break;
	default:
	{
		errorCode = "暂不支持该类型的荷载！";
		return { false, errorCode };
	}

		break;
	}
	GetProject()->GetPlotControl()->InputChangedOn();
	GetProject()->GetPlotControl()->StructTreeChangedOn();	//用于通知界面刷新菜单
	GetProject()->GetPlotControl()->LoadGroupChangedOn();		//用于通知界面刷新大数据界面
	GetProject()->GetPlotControl()->LoadGroupTreeChangedOn();	//用于通知界面刷新菜单
	return { true, errorCode };
}