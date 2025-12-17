#include "stdafx.h"
#include "PyInterface.h"


std::pair<bool, std::string> PyInterface::OSIS_Replot()
{
	std::string errorCode;
	//m_funcReplot(0, 0);		// 相当于 CGraphView::RefreshData(0, 0)
	try {
		Replot(); 
		return { true, errorCode };
	}
	catch (std::runtime_error e)
	{
		errorCode = e.what();
		return { false, errorCode };
	}
}

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
std::pair<bool, std::string> PyInterface::OSIS_CrpShrk(const int nNO, const std::string strName, const double dAvgHumidity, const int nBirthTime, const double dTypeCoeff, const int nBirthByShrinking)
{
	std::string errorCode;
	auto* pPM = GET_PREP_PROP();
	//strName = utf8_to_wide(strName);
	PREP::CreepShrinkInfo* pInfo = pPM->getInfo<PREP::CreepShrinkInfo>(nNO);
	if (pInfo)
	{
		pInfo->setName(strName);
		pInfo->setAvgHumidity(dAvgHumidity);
		pInfo->setBirthTime(nBirthTime);
		pInfo->setTypeCoeff(dTypeCoeff);
		pInfo->setBirthByShrinking(nBirthByShrinking);
	}
	else
	{
		PREP::CreepShrinkInfo* pInfo = new PREP::CreepShrinkInfo();

		pInfo->setNO(nNO);
		pInfo->setName(strName);

		pInfo->setAvgHumidity(dAvgHumidity);
		pInfo->setBirthTime(nBirthTime);
		pInfo->setTypeCoeff(dTypeCoeff);
		pInfo->setBirthByShrinking(nBirthByShrinking);

		if (!pPM->append(pInfo)) {

			delete pInfo; pInfo = nullptr;
			return { false, errorCode };
		}
	}

	////压入影子命令
	//yilCString strCommand;
	//strCommand.Format(_T("CrpShrkMod,%d,%d"), nNew, nOld);
	//PUSH_SHADOW_CMD(THIS_IS_MOD, strCommand);

	GetProject()->GetPlotControl()->StructTreeChangedOn();
	GetProject()->GetPlotControl()->CrpShrkDataChangedOn();

	return { true, errorCode };
	//return { true, "成功创建收缩徐变，编号:" + std::to_string(nNO) + "，名称：" + strName + "！"};
}




