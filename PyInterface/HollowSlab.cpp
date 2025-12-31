#include "stdafx.h"
#include "PyInterface.h"

//#include "XCPQuickBuilding/ShowSimSmallBoxTool.h"
//#include "XCPQuickBuilding/ShowSimConSmallBoxTool.h"
//#include "XCPQuickBuilding/ShowSimTBeamTool.h"
//#include "XCPQuickBuilding/ShowSimConTBeamTool.h"
//#include "XCPQuickBuilding/ShowSimHollowSlabTool.h"
//
//#include "XCPQuickBuilding/xcpGeneralBridgeShowTool.h"
//#include "XCPQuickBuilding/xcpGeneralTendonData.h"
//#include "XCPQuickBuilding/xcpSimCommonData.h"
//#include "XCPQuickBuilding/xcpSimplyPortraitData.hpp"
//#include "YILCommonData/xcpQBDataManager.h"
//#include "XCPUIDocument/xcpConcreteSecCommonData.h"
//
//
//std::pair<bool, std::string> PyInterface::OSIS_QBInitialize(const std::string strBridgeType)
//{
//	std::string errorCode = "";
//	yilEnumType::ProjectType eBridgeType = yilEnumType::ProjectType::PT_BridgeNULL;
//	if (strBridgeType == "PT_SimpleHollowSlab")
//	{
//		eBridgeType = yilEnumType::ProjectType::PT_SimpleHollowSlab;
//		m_pQBTool = std::make_unique<ShowSimHollowSlabTool>();
//	}
//	else if (strBridgeType == "PT_SimpleLittleBox")
//	{
//		eBridgeType = yilEnumType::ProjectType::PT_SimpleLittleBox;
//		m_pQBTool = std::make_unique<ShowSimSmallBoxTool>();
//	}
//	else if (strBridgeType == "PT_SimpleContinuousLittleBox")
//	{
//		eBridgeType = yilEnumType::ProjectType::PT_SimpleContinuousLittleBox;
//		m_pQBTool = std::make_unique<ShowSimConSmallBoxTool>();
//	}
//	else if (strBridgeType == "PT_SimpleTBeam")
//	{
//		eBridgeType = yilEnumType::ProjectType::PT_SimpleTBeam;
//		m_pQBTool = std::make_unique<ShowSimTBeamTool>();
//	}
//	else if (strBridgeType == "PT_SimpleContinuousTBeam")
//	{
//		eBridgeType = yilEnumType::ProjectType::PT_SimpleContinuousTBeam;
//		m_pQBTool = std::make_unique<ShowSimConTBeamTool>();
//	}
//	else
//	{
//		errorCode = "暂不支持 " + strBridgeType + " 类型桥梁的快速建模！";
//		m_pQBTool = nullptr;
//		return { false, errorCode };
//	}
//	return { true, errorCode };
//}
//
//std::pair<bool, std::string> PyInterface::OSIS_QBHollowSlabOverall(
//	std::vector<double> spans,
//	const bool bIsElasticConnection,
//	const double dKxOfAbutment1, const double dKyOfAbutment1, const double dKzOfAbutment1,
//	const double dKxOfAbutment2, const double dKyOfAbutment2, const double dKzOfAbutment2,
//	const double dElasticLength)
//{
//	std::string errorCode = "";
//	//xcpQBDataManager* pQBDataManager = xcpQBDataManager::GetInstance();
//	//xcpSimplyHollowSlabData
//	//m_pQBTool = std::make_unique<ShowSimHollowSlabTool>();
//
//	xcpSimplyOverallData* pOverall = (xcpSimplyOverallData*)dynamic_cast<ShowSimHollowSlabTool*>(m_pQBTool.get())->GetOverallData();
//	if (!pOverall)
//	{
//		errorCode = "该功能暂时不可用！";
//		return { false, errorCode };
//	}
//
//	//const double dSpan = pOverall->GetSpan();
//	auto* pUnit = GetProject()->GetUnit();
//	pOverall->SetSpan(pUnit->LocalLengthToSI(spans[0]));		// 空心板实际上只需要一个跨径数据
//	pOverall->SetIsElasticConnection(bIsElasticConnection);
//	pOverall->SetKxOfAbutment1(pUnit->LocalLineForceToSI(dKxOfAbutment1));
//	pOverall->SetKyOfAbutment1(pUnit->LocalLineForceToSI(dKyOfAbutment1));
//	pOverall->SetKzOfAbutment1(pUnit->LocalLineForceToSI(dKzOfAbutment1));
//	pOverall->SetKxOfAbutment2(pUnit->LocalLineForceToSI(dKxOfAbutment2));
//	pOverall->SetKyOfAbutment2(pUnit->LocalLineForceToSI(dKyOfAbutment2));
//	pOverall->SetKzOfAbutment2(pUnit->LocalLineForceToSI(dKzOfAbutment2));
//	pOverall->SetElasticLength(pUnit->LocalLengthToSI(dElasticLength));
//
//	return { true, errorCode };
//}
//
//std::pair<bool, std::string> PyInterface::OSIS_QBHollowSlabPortrait(
//	const double dEleLengthMin, const double dEleLengthMax, /*单元尺寸*/
//	const double S1, const double L1, const double F1, const double Tb, const double Tw, /*纵向参数*/
//	const double D1	/*支座设置*/)
//{
//	std::string errorCode = "";
//
//	xcpSimplyPortraitData<qilHollowSlabSectionDto>* pPortrait = (xcpSimplyPortraitData<qilHollowSlabSectionDto>*)dynamic_cast<ShowSimHollowSlabTool*>(m_pQBTool.get())->GetPortraitData();
//	if (!pPortrait)
//	{
//		errorCode = "该功能暂时不可用！";
//		return { false, errorCode };
//	}
//
//	auto* pUnit = GetProject()->GetUnit();
//	//单元尺寸
//	pPortrait->SetEleMinLength(pUnit->LocalLengthToSI(dEleLengthMin));
//	pPortrait->SetEleMaxLength(pUnit->LocalLengthToSI(dEleLengthMax));
//
//	//纵向参数
//	pPortrait->SetLengthOfS1(pUnit->LocalLengthToSI(S1));
//	pPortrait->SetLengthOfL1(pUnit->LocalLengthToSI(L1));
//	pPortrait->SetLengthOfF1(pUnit->LocalLengthToSI(F1));
//	pPortrait->SetLengthOfTb(pUnit->LocalLengthToSI(Tb));
//	pPortrait->SetLengthOfTw(pUnit->LocalLengthToSI(Tw));
//
//	//支座设置
//	pPortrait->SetLengthOfD1(pUnit->LocalLengthToSI(D1));
//	return { true, errorCode };
//}
//
//std::pair<bool, std::string> PyInterface::OSIS_QBHollowSlabLoad(
//	const bool bHaveDeadLoad, const bool bHavePavement, const bool bHaveRail, const bool bHaveSidewalk, const bool bHaveEndCappingLoad, /*恒载*/
//	const bool bHaveMovingLoad, const bool bHaveTemperEff, const bool bHaveTemperGradient, const bool bHaveSupSettle, /*移动荷载*/
//	const double dDeadLoadFactor, const double dPavementIntensity, const double dRailIntensity, const double dSidewalkIntensity, const double dCrowdLoad,
//	const double dEndCappingLoad,
//	const double dTransVehDistribution, const bool bIsSelfDefine, const double dFundFreq, const double dWarming, const double dCooling,
//	const double T1, const double T2, const double dSupSettle)
//{
//	std::string errorCode = "";
//	xcpSimSlabLoadData* pLoad = (xcpSimSlabLoadData*)dynamic_cast<ShowSimHollowSlabTool*>(m_pQBTool.get())->GetLoadData();
//	if (!pLoad)
//	{
//		errorCode = "该功能暂时不可用！";
//		return { false, errorCode };
//	}
//
//	auto* pUnit = GetProject()->GetUnit();
//	pLoad->SetHaveDeadLoad(bHaveDeadLoad);
//	pLoad->SetHavePavement(bHavePavement);//铺装
//	pLoad->SetHaveRail(bHaveRail);//防撞栏杆
//	pLoad->SetHaveSidewalk(bHaveSidewalk);//人行道
//
//	pLoad->SetHaveEndCapping(bHaveEndCappingLoad);//封端混凝土
//
//	pLoad->SetHaveMovingLoad(bHaveMovingLoad);//移动荷载
//	pLoad->SetHaveTemperEff(bHaveTemperEff);//温度作用
//	pLoad->SetHaveTemperGradient(bHaveTemperGradient);//温度梯度
//	pLoad->SetHaveSupSettle(bHaveSupSettle);//支座沉降
//
//	pLoad->SetDeadLoadFactor(dDeadLoadFactor);
//	pLoad->SetPavementIntensity(pUnit->LocalLineForceToSI(dPavementIntensity));
//	pLoad->SetRailIntensity(pUnit->LocalLineForceToSI(dRailIntensity));
//	pLoad->SetSidewalkIntensity(pUnit->LocalLineForceToSI(dSidewalkIntensity));
//	pLoad->SetCrowdLoad(pUnit->LocalLineForceToSI(dCrowdLoad));//人群荷载
//
//	pLoad->SetEndCappingLoad(pUnit->LocalLineForceToSI(dEndCappingLoad));//封端混凝土
//
//	pLoad->SetTransVehDistribution(dTransVehDistribution);//横向分布系数
//	pLoad->SetIsSelfDefine(bIsSelfDefine);
//	pLoad->SetFundFreq(dFundFreq);//结构基频
//	pLoad->SetWarning(pUnit->LocalAngleToSI(dWarming));//整体升温
//	pLoad->SetCooling(pUnit->LocalAngleToSI(dCooling));//整体降温
//	pLoad->SetT1(pUnit->LocalAngleToSI(T1));
//	pLoad->SetT2(pUnit->LocalAngleToSI(T2));
//	pLoad->SetSupSettle(pUnit->LocalLengthToSI(dSupSettle));
//	return { true, errorCode };
//}
//
//std::pair<bool, std::string> PyInterface::OSIS_QBHollowSlabTendon(const py::list& mapTendonInfo)
//{
//	std::string errorCode = "";
//	xcpSimplyTendonData* pTendon = (xcpSimplyTendonData*)dynamic_cast<ShowSimHollowSlabTool*>(m_pQBTool.get())->GetTendonData();
//	xcpSimplyPortraitData<qilHollowSlabSectionDto>* pPortrait = (xcpSimplyPortraitData<qilHollowSlabSectionDto>*)dynamic_cast<ShowSimHollowSlabTool*>(m_pQBTool.get())->GetPortraitData();
//	if (!pPortrait || !pTendon)
//	{
//		errorCode = "该功能暂时不可用！";
//		return { false, errorCode };
//	}
//	xcpSimplyTendonInfo infoTemp;
//
//	auto* pTendonMap = pTendon->GetTendonInfo();
//	CString str;
//	
//	// 后面再细化，先简单实现	2025/12/15
//	for (int i = 1; i < py::len(mapTendonInfo) - 1; i++)
//	{
//		try {
//			py::dict dict = mapTendonInfo[i].cast<py::dict>();
//			infoTemp.SetTendonName(safe_cast<std::string>(dict, "strTendonName"));
//			infoTemp.SetTendonProp(safe_cast<std::string>(dict, "strTendonProp"));
//
//			//if (!UpdateLmAndEleNo(i, LeLengthCol))return;
//			//str = m_CtrlTendon.GetItemText(i, LeLengthCol);
//			//infoTemp.SetLengthOfLe(m_pUnit->LocalLengthToSI(_ttof(str)));
//
//			//if (!UpdateLmAndEleNo(i, HeLengthCol))return;
//			//str = m_CtrlTendon.GetItemText(i, HeLengthCol);
//			//infoTemp.SetLengthOfHe(m_pUnit->LocalLengthToSI(_ttof(str)));
//
//			//if (!UpdateLmAndEleNo(i, ADegreeCol))return;
//			//str = m_CtrlTendon.GetItemText(i, ADegreeCol);
//			//infoTemp.SetDegreeOfA(m_pUnit->LocalAngleToSI(_ttof(str)));
//
//			//if (!UpdateLmAndEleNo(i, HmLengthCol))return;
//			//str = m_CtrlTendon.GetItemText(i, HmLengthCol);
//			//infoTemp.SetLengthOfHm(m_pUnit->LocalLengthToSI(_ttof(str)));
//
//			//if (!UpdateLmAndEleNo(i, RLengthCol))return;
//			//str = m_CtrlTendon.GetItemText(i, RLengthCol);
//			//infoTemp.SetLengthOfR(m_pUnit->LocalLengthToSI(_ttof(str)));
//
//			//str = m_CtrlTendon.GetItemText(i, TensileStressCol);
//			//infoTemp.SetTensileStress(m_pUnit->LocalPascalToSI(_ttof(str)));//张拉应力
//
//			////str = m_CtrlTendon.GetItemText(i, EleGroupCol);
//			////infoTemp.SetEleGroup((yilCString)str);
//
//			//str = m_CtrlTendon.GetItemText(i, TieNumCol);
//			//infoTemp.SetTieNums(_ttoi(str));
//
//
//			//(*pTendonInfoMap)[i - 1] = infoTemp;
//		}
//		catch (std::runtime_error e)
//		{
//			errorCode = e.what();
//			errorCode = "钢束 " + std::to_string(i) + " 的";
//			return { false, errorCode };
//		}
//	}
//	
//	return { true, errorCode };
//}
//
//std::pair<bool, std::string> PyInterface::OSIS_QBHollowSlabStage()
//{
//	std::string errorCode = "";
//
//	return { true, errorCode };
//}


std::pair<bool, std::string> PyInterface::OSIS_QBHollowSlab(const py::list spans, const bool bIsElasticConnection)
{
	std::string errorCode;
	double span = spans[0].cast<double>();		// 空心板，只需要第一个跨径

	// ===================材料定义===================
	// TODO 混凝土 钢筋 钢束材料
	OSIS_CrpShrk(1, "收缩徐变", 70, 7, 5.0, 3);
	OSIS_Material(1, "混凝土-C50", "CONC", "JTG3362_2018", "C50", 1, 0.05);
	OSIS_Material(2, "钢筋-HRB400", "REBAR", "JTG3362_2018", "HRB400", -1, 0.05);
	OSIS_Material(3, "钢绞线-1860", "PRESTRESSED", "JTG3362_2018", "Strand1860", -1, 0.05);

	OSIS_LiveGrade("简支空心板-移动荷载", "JTGD60_2015", "HIGHWAY_I", py::dict());	// 根据快速建模的具体实现，Init Project的时候初始化了活载


	// ===================截面定义===================
	// TODO 从pPortrait中获取


	// ===================节点定义===================
	// TODO 从pPortrait->CalcualteElement();中计算节点

	// ===================单元定义===================
	// TODO 从pPortrait->GetElement()中获取
	
	// ===================边界条件===================
	if (bIsElasticConnection)
	{

	}
	else
	{

	}

	// ==================预应力定义==================

	// ===================荷载定义===================

	// =================施工阶段定义=================

	return { true, errorCode };
}