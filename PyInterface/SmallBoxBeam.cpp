#include "stdafx.h"
#include "PyInterface.h"

#include "XCPUIModuleControl/xcpQBBridgeTool.h"
#include "XCPUIModuleControl/xcpQBDlgToolFactory.h"
#include "XCPQuickBuilding/xcpGeneralBridgeShowTool.h"
#include "XCPQuickBuilding/xcpGeneralBridgeData.h"
#include "XCPQuickBuilding/ShowSimSmallBoxTool.h"
#include "XCPQuickBuilding/ShowSimHollowSlabTool.h"
//#include "XCPQuickBuilding/xcpGeneralBridgeData.h"


//std::unique_ptr<xcpQBShowToolBase>& GetQBShowToolBase()
//{
//	return xcpQBBridgeTool::GetInstance()->GetDlgToolFactory()->GetShowDlgTool();
//}


//std::pair<bool, std::string> PyInterface::OSIS_QBSmallBoxBeam(const py::list spans, const bool bIsElasticConnection)
//{
//	std::string errorCode;
//	ShowSimSmallBoxTool* pTool = dynamic_cast<ShowSimSmallBoxTool*>(GetQBShowTool());		// 小箱梁工具类
//	//xcpSimplySmallBoxData* pData = dynamic_cast<xcpSimplySmallBoxData*>(pTool->GetBridgeData());
//	auto* pOverall = (xcpSimplyOverallData*)pTool->GetOverallData();
//	auto* pProtrait = (xcpSimplyPortraitData<qilSmallBoxSectionDto>*)pTool->GetPortraitData();
//	auto* pLoad = (xcpSimBeamLoadData*)pTool->GetLoadData();
//	auto* pTendon = (xcpSimplyTendonData*)pTool->GetTendonData();
//	//auto* pRebar = pTool->GetRebarData();	// 先不管钢筋
//	auto* pStage = (xcpSimBeamStageData*)pTool->GetStageData();
//
//	//xcpSimplyOverallData* 
//
//	return { true, errorCode };
//}
xcpQBShowToolBase* PyInterface::GetQBShowTool()	// 必须先在外部 create 快速建模再调用这个
{
	return xcpQBBridgeTool::GetInstance()->GetDlgToolFactory()->GetShowDlgTool().get();
}

std::pair<bool, std::string> PyInterface::OSIS_QBOverall(
	const std::string eBridgeType,
	std::vector<double> spans,
	const bool bIsElasticConnection,
	const double dKxOfAbutment1, const double dKyOfAbutment1, const double dKzOfAbutment1,
	const double dKxOfAbutment2, const double dKyOfAbutment2, const double dKzOfAbutment2,
	const double dElasticLength)
{
	std::string errorCode;
	if (eBridgeType == "HOLLOWSLAB")
	{

	}
	if (eBridgeType == "SMALLBOXBEAM")		// 仿照 XXXTool 类的 Execute 方法而写
	{
		ShowSimSmallBoxTool* pTool = dynamic_cast<ShowSimSmallBoxTool*>(GetQBShowTool());		// 小箱梁工具类
		auto* pOverall = (xcpSimplyOverallData*)pTool->GetOverallData();
		const double dOldSpan = pOverall->GetSpan();				// 旧跨径

		auto* pUnit = GetProject()->GetUnit();
		pOverall->SetSpan(pUnit->LocalLengthToSI(spans[0]));		// 实际上只需要一个跨径数据
		pOverall->SetIsElasticConnection(bIsElasticConnection);
		pOverall->SetKxOfAbutment1(pUnit->LocalLineForceToSI(dKxOfAbutment1));
		pOverall->SetKyOfAbutment1(pUnit->LocalLineForceToSI(dKyOfAbutment1));
		pOverall->SetKzOfAbutment1(pUnit->LocalLineForceToSI(dKzOfAbutment1));
		pOverall->SetKxOfAbutment2(pUnit->LocalLineForceToSI(dKxOfAbutment2));
		pOverall->SetKyOfAbutment2(pUnit->LocalLineForceToSI(dKyOfAbutment2));
		pOverall->SetKzOfAbutment2(pUnit->LocalLineForceToSI(dKzOfAbutment2));
		pOverall->SetElasticLength(pUnit->LocalLengthToSI(dElasticLength)); 

		if (pTool->UpdateRelativeValue())
		{
			pTool->UpdateDefaultValue(dOldSpan);
		}
		else
		{
			errorCode = "跨径与纵向支座数据不匹配，请修改纵向支座数据!";
			return { false, errorCode };
		}
	}
	else if (eBridgeType == "TBEAM")
	{

	}
	else if (eBridgeType == "CONTINUOUSSMALLBOXBEAM")
	{

	}
	else if (eBridgeType == "CONTINUOUSTBEAM")
	{

	}
	else
	{

	}
	return { true, errorCode };
}

std::pair<bool, std::string> PyInterface::OSIS_QBPortrait(
	const std::string eBridgeType,
	const double dEleLengthMin, const double dEleLengthMax, /*单元尺寸*/
	const double S1, const double L1, const double F1, const double Tb, const double Tw, /*纵向参数*/
	const double D1	/*支座设置*/
)
{
	std::string errorCode;
	if (eBridgeType == "HOLLOWSLAB")		// 仿照 XXXTool 类的 Execute 方法而写
	{
		
	}
	else if (eBridgeType == "SMALLBOXBEAM")
	{
		ShowSimSmallBoxTool* pTool = dynamic_cast<ShowSimSmallBoxTool*>(GetQBShowTool());		// 小箱梁工具类
		auto* pPortrait = (xcpSimplyPortraitData<qilSmallBoxSectionDto>*)pTool->GetPortraitData();

		auto* pUnit = GetProject()->GetUnit();
		//单元尺寸
		pPortrait->SetEleMinLength(pUnit->LocalLengthToSI(dEleLengthMin));
		pPortrait->SetEleMaxLength(pUnit->LocalLengthToSI(dEleLengthMax));
		
		//纵向参数
		pPortrait->SetLengthOfS1(pUnit->LocalLengthToSI(S1));
		pPortrait->SetLengthOfL1(pUnit->LocalLengthToSI(L1));
		pPortrait->SetLengthOfF1(pUnit->LocalLengthToSI(F1));
		pPortrait->SetLengthOfTb(pUnit->LocalLengthToSI(Tb));
		pPortrait->SetLengthOfTw(pUnit->LocalLengthToSI(Tw));
		
		//支座设置
		pPortrait->SetLengthOfD1(pUnit->LocalLengthToSI(D1));

		pTool->UpdateRelativeValue();
	}
	else if (eBridgeType == "TBEAM")
	{

	}
	else
	{

	}
	return { true, errorCode };
}

std::pair<bool, std::string> PyInterface::OSIS_QBLoad(
	const std::string eBridgeType,
	const bool bHaveDeadLoad, const bool bHavePavement, const bool bHaveRail, const bool bHaveSidewalk, const bool bHaveSideBeam, const bool bHaveMiddleBeam,/*恒载*/
	const bool bHaveMovingLoad, const bool bHaveTemperEff, const bool bHaveTemperGradient, const bool bHaveSupSettle, /*移动荷载*/
	const double dDeadLoadFactor, const double dPavementIntensity, const double dRailIntensity, const double dSidewalkIntensity, const double dCrowdLoad,
	const double dSideBeamPointLoad, const double dMiddleBeamPointLoad,
	const double dTransVehDistribution, const bool bIsSelfDefine, const double dFundFreq, const double dWarming, const double dCooling,
	const double T1, const double T2, const double dSupSettle)
{
	std::string errorCode;
	if (eBridgeType == "HOLLOWSLAB")		// 仿照 XXXTool 类的 Execute 方法而写
	{

	}
	else if (eBridgeType == "SMALLBOXBEAM")
	{
		ShowSimSmallBoxTool* pTool = dynamic_cast<ShowSimSmallBoxTool*>(GetQBShowTool());		// 小箱梁工具类
		auto* pLoad = (xcpSimBeamLoadData*)pTool->GetLoadData();

		auto* pUnit = GetProject()->GetUnit();
		pLoad->SetHaveDeadLoad(bHaveDeadLoad);
		pLoad->SetHavePavement(bHavePavement);//铺装
		pLoad->SetHaveRail(bHaveRail);//防撞栏杆
		pLoad->SetHaveSidewalk(bHaveSidewalk);//人行道

		pLoad->SetHaveSideBeam(bHaveSideBeam);//端横梁
		pLoad->SetHaveMiddleBeam(bHaveMiddleBeam);//中横梁

		pLoad->SetHaveMovingLoad(bHaveMovingLoad);//移动荷载
		pLoad->SetHaveTemperEff(bHaveTemperEff);//温度作用
		pLoad->SetHaveTemperGradient(bHaveTemperGradient);//温度梯度
		pLoad->SetHaveSupSettle(bHaveSupSettle);//支座沉降

		pLoad->SetDeadLoadFactor(dDeadLoadFactor);
		pLoad->SetPavementIntensity(pUnit->LocalLineForceToSI(dPavementIntensity));
		pLoad->SetRailIntensity(pUnit->LocalLineForceToSI(dRailIntensity));
		pLoad->SetSidewalkIntensity(pUnit->LocalLineForceToSI(dSidewalkIntensity));
		pLoad->SetCrowdLoad(pUnit->LocalLineForceToSI(dCrowdLoad));//人群荷载

		pLoad->SetSideBeamPointLoad(pUnit->LocalForceToSI(dSideBeamPointLoad));//端横梁
		pLoad->SetMiddleBeamPointLoad(pUnit->LocalForceToSI(dMiddleBeamPointLoad));//中横梁

		pLoad->SetTransVehDistribution(dTransVehDistribution);//横向分布系数
		pLoad->SetIsSelfDefine(bIsSelfDefine);
		pLoad->SetFundFreq(dFundFreq);//结构基频
		pLoad->SetWarning(pUnit->LocalAngleToSI(dWarming));//整体升温
		pLoad->SetCooling(pUnit->LocalAngleToSI(dCooling));//整体降温
		pLoad->SetT1(pUnit->LocalAngleToSI(T1));
		pLoad->SetT2(pUnit->LocalAngleToSI(T2));
		pLoad->SetSupSettle(pUnit->LocalLengthToSI(dSupSettle));

		pTool->UpdateRelativeValue();
	}
	else if (eBridgeType == "TBEAM")
	{

	}
	else
	{

	}
	return { true, errorCode };
}

// 检查tendon参数对不对(后面可能优化一下，不用单独写个函数或者写个更通用的)
bool CheckTendonParam(yilUnitScale* pUnit, const double& Le, const double& He, const double& A, const double& Hm, const double& R, const double& stress, const int& tieNums,
	std::string& errorCode)
{
	xcpSimplyTendonInfo tendonTemp;
	tendonTemp.SetLengthOfLe(pUnit->LocalLengthToSI(Le));
	double dSt = 0.0, dEnd = 0.0;
	tendonTemp.CalTendonPos(dSt, dEnd);
	/*int iEleNo, jEleNo;
	if (pPortrait->GetEachEndOfEleNo(dSt, dEnd, iEleNo, jEleNo))
	{
		striEleNo.Format(_T("%d"), iEleNo);
		strjEleNo.Format(_T("%d"), jEleNo);
	}*/
	tendonTemp.SetLengthOfHe(pUnit->LocalLengthToSI(He));

	tendonTemp.SetDegreeOfA(pUnit->LocalAngleToSI(A));

	tendonTemp.SetLengthOfHm(pUnit->LocalLengthToSI(Hm));

	tendonTemp.SetLengthOfR(pUnit->LocalLengthToSI(R));

	const double dLm = tendonTemp.GetLengthOfLm();

	if (dLm <= 0)
	{
		errorCode = "Lm <= 0, 钢束线形错误, 请修改钢束参数!";
		//m_CtrlTendon.SetItemText(_iRow, _iCol, m_strSelGridCell);
		return false;
	}
	const double dRadianA = tendonTemp.DegreeToRadian(tendonTemp.GetDegreeOfA());
	const double dLeftVal = tendonTemp.GetLengthOfR() * tan(dRadianA / 2.0);
	const double dRightVal = (tendonTemp.GetLengthOfHe() - tendonTemp.GetLengthOfHm()) / sin(dRadianA);
	if (dLeftVal > min(dRightVal, tendonTemp.GetLaValue(tendonTemp.GetLengthOfLe(), tendonTemp.GetLengthOfHe(), tendonTemp.GetDegreeOfA())))
	{
		errorCode = "圆弧半径R与其余钢束参数不匹配,钢束线形错误,请修改钢束参数!";
		//m_CtrlTendon.SetItemText(_iRow, _iCol, m_strSelGridCell);
		return false;
	}
	tendonTemp.SetTensileStress(pUnit->LocalPascalToSI(stress));//张拉应力
	tendonTemp.SetTieNums(tieNums);

	return true;
}

std::pair<bool, std::string> PyInterface::OSIS_QBTendon(
	const std::string eBridgeType,
	const py::list tendonInfo)
{
	std::string errorCode;
	if (eBridgeType == "HOLLOWSLAB")		// 仿照 XXXTool 类的 Execute 方法而写
	{

	}
	else if (eBridgeType == "SMALLBOXBEAM")
	{
		ShowSimSmallBoxTool* pTool = dynamic_cast<ShowSimSmallBoxTool*>(GetQBShowTool());		// 小箱梁工具类
		xcpSimplyTendonData* pTendon = (xcpSimplyTendonData*)pTool->GetTendonData();
		xcpSimplyPortraitData<qilSmallBoxSectionDto>* pPortrait = (xcpSimplyPortraitData<qilSmallBoxSectionDto>*)pTool->GetPortraitData();
		pPortrait->CalcualteElement();

		auto* pUnit = GetProject()->GetUnit(); 
		std::map<int, xcpSimplyTendonInfo>* pTendonInfoMap = pTendon->GetTendonInfo();
		//std::map<int, xcpSimplyTendonInfo> tendonInfoMapTemp;

		for (int i = 0; i < py::len(tendonInfo); i++)	// python中将以 list[dict[]] 的形式组织钢束数据
		{
			std::string name, prop;
			double Le, He, A, Hm, R, stress;
			std::string elementGroupName;
			int tieNums;

			// 参数解析
			try {
				py::dict dict = tendonInfo[i].cast<py::dict>();
				name = safe_cast<std::string>(dict, "name");
				prop = safe_cast<std::string>(dict, "prop");
				Le = safe_cast<double>(dict, "Le");
				He = safe_cast<double>(dict, "He");
				A = safe_cast<double>(dict, "A");
				Hm = safe_cast<double>(dict, "Hm");
				R = safe_cast<double>(dict, "R");
				stress = safe_cast<double>(dict, "stress");
				//elementGroupName = safe_cast<std::string>(dict, "elementGroupName");
				tieNums = safe_cast<double>(dict, "tieNums");
			}
			catch (std::runtime_error e)
			{
				errorCode = e.what();
				errorCode = "第" + std::to_string(i) + "个钢束的" + errorCode;
				return { false, errorCode };
			}

			// 数据检验，仿照 XXXDlg
			if (!CheckTendonParam(pUnit, Le, He, A, Hm, R, stress, tieNums, errorCode))
			{
				return { false, errorCode };
			}
			xcpSimplyTendonInfo tendonTemp;
			tendonTemp.SetTendonName(name);
			tendonTemp.SetTendonProp(prop);
			tendonTemp.SetLengthOfLe(Le);
			tendonTemp.SetLengthOfHe(He);
			tendonTemp.SetDegreeOfA(A);
			tendonTemp.SetLengthOfHm(Hm);
			tendonTemp.SetLengthOfR(R);
			tendonTemp.SetTensileStress(stress);
			tendonTemp.SetTieNums(tieNums);
			(*pTendonInfoMap)[i] = tendonTemp;
		}
	}
	else if (eBridgeType == "TBEAM")
	{

	}
	else
	{

	}
	return { true, errorCode };
}

std::pair<bool, std::string> PyInterface::OSIS_QBStage(
	const std::string eBridgeType,
	const py::list stageInfo)
{
	std::string errorCode;
	if (eBridgeType == "HOLLOWSLAB")		// 仿照 XXXTool 类的 Execute 方法而写
	{

	}
	else if (eBridgeType == "SMALLBOXBEAM")
	{
		ShowSimSmallBoxTool* pTool = dynamic_cast<ShowSimSmallBoxTool*>(GetQBShowTool());		// 小箱梁工具类
		xcpSimBeamStageData* pStage = (xcpSimBeamStageData*)pTool->GetStageData();
		std::map<int, xcpSimBeamStageInfo>* pStageInfo = pStage->GetStageInfo();

		for (int i = 0; i < py::len(stageInfo); i++)
		{
			int stageNum = 1;
			std::string name, state, duration, age;
			xcpSimBeamStageInfo stageData;
			// 参数解析
			try {
				py::dict dict = stageInfo[i].cast<py::dict>();
				stageNum = safe_cast<int>(dict, "stageNum");
				name = safe_cast<std::string>(dict, "name");
				state = safe_cast<std::string>(dict, "state");
				duration = safe_cast<std::string>(dict, "duration");
				age = safe_cast<std::string>(dict, "age");
			}
			catch (std::runtime_error e)
			{
				errorCode = e.what();
				errorCode = "第" + std::to_string(i) + "个施工阶段的" + errorCode;
				return { false, errorCode };
			}
			stageData.SetName(name);
			stageData.SetState(state);
			stageData.SetDuration(duration);
			stageData.SetAge(age);
			(*pStageInfo)[stageNum] = stageData;
		}

	}
	else if (eBridgeType == "TBEAM")
	{

	}
	else
	{

	}
	return { true, errorCode };
}

