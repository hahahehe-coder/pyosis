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
std::pair<bool, std::string> PyInterface::OSIS_LoadCase(const std::string strName, const std::string eLoadCaseType, const double dScalar, const std::string strPrompt)
{
	std::string errorCode;
	PrepEnum::LoadCase eLC;
	if (!GetCommand()->StrToLoadCase(eLC, eLoadCaseType, 2))
	{
		errorCode = "参数 荷载工况类型 错误！";
		return { false, errorCode };
	}

	auto* pLCM = GET_PREP_LC();
	PREP::LoadCaseInfo* pInfo = pLCM->getInfo(strName);
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
		pInfo->setName(strName);
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
	case PrepEnum::Load::Gravity:
	{
		
	}
		break;

	case PrepEnum::Load::NForce:
	{
		int nEntity;
		double dFX, dFY, dFZ, dMX, dMY, dMZ;
		try {
			nEntity = safe_cast<int>(kwargs, "nEntity");
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
			errorCode = "荷载" + std::to_string(nEntity) + "节点荷载值不能全部为零！";
			//g_pWarningList->AddToTemperary(4, nEntity, -1);
			return { false, errorCode };
		}

		using Info = PREP::NForceInfo;
		if (!pLc->getInfo<Info>(nEntity))		// 创建荷载
		{
			auto* pUnit = GetProject()->GetUnit();
			dFX = pUnit->LocalForceToSI(dFX);
			dFY = pUnit->LocalForceToSI(dFY);
			dFZ = pUnit->LocalForceToSI(dFZ);
			dMX = pUnit->LocalMomentToSI(dMX);
			dMY = pUnit->LocalMomentToSI(dMY);
			dMZ = pUnit->LocalMomentToSI(dMZ);
			Info* pLdInfo = new Info();

			if (!pLdInfo->setEntityNO(nEntity)) {
				delete pLdInfo; pLdInfo = nullptr;
				errorCode = "荷载作用的节点" + std::to_string(nEntity) + "设定失败！";
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
			Info* pLdInfo = pLc->getInfo<Info>(nEntity);
			if (!pLdInfo) 
			{
				errorCode = "节点" + std::to_string(nEntity) + "上的荷载修改失败！";
				return { false, errorCode };
			}

			if (!pLdInfo->check(std::vector<int>{}))
			{
				errorCode = "节点" + std::to_string(nEntity) + "上的荷载修改失败！";
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
	case PrepEnum::Load::CForce:
	{
		
	}
		break;

	case PrepEnum::Load::Line:
	{

	}
		break;

	case PrepEnum::Load::Displacement:
	{

	}
		break;

	case PrepEnum::Load::Initial:
	{

	}
		break;

	case PrepEnum::Load::UTemp:
	{
		int nEntity;
		std::string eDirect;
		double dTemp, dLength;
		PrepEnum::TempDirc eDirc = PrepEnum::TempDirc::Unassigned;
		try {
			nEntity = safe_cast<int>(kwargs, "nEntity");
			eDirect = safe_cast<std::string>(kwargs, "eDirect");
			dTemp = safe_cast<double>(kwargs, "dTemp");
			dLength = safe_cast<double>(kwargs, "dLength");
			if (!GetCommand()->StrToTempDirc(eDirc, eDirect, 4))
			{
				errorCode = "参数 单元坐标系 解析错误！";
				return { false, errorCode };
			}
			if (PrepEnum::TempDirc::X == eDirc)
			{
				dLength = -1;
			}
		}
		catch (const std::runtime_error& e) {
			// 处理错误
			return { false, e.what() };
		}

		using Info = PREP::UniformTempInfo;
		if (!pLc->getInfo<Info>(nEntity))		// 创建荷载
		{
			auto* pLdInfo = new Info();
			if (!pLdInfo->setEntityNO(nEntity)) {
				delete pLdInfo; pLdInfo = nullptr;
				errorCode = "荷载作用的节点" + std::to_string(nEntity) + "设定失败！";
				return { false, errorCode };
			}

			pLdInfo->setType(PrepEnum::Load::UTemp);
			pLdInfo->setTempDirc(eDirc);
			pLdInfo->setTemp(dTemp);
			pLdInfo->setLength(dLength);

			if (!pLc->append<Info>(pLdInfo))
			{

				delete pLdInfo; pLdInfo = nullptr;
				errorCode = "荷载加入荷载工况" + strLCName + "失败！";
				return { false, errorCode };
			}

			////压入影子命令
			//yilCString strCommand;
			//strCommand.Format(_T("LoadDel,UTEMP,%s,%d"), _lc->getName().c_str(), _entity);
			//PUSH_SHADOW_CMD(THIS_IS_DEF, strCommand);
		}
		else
		{
			Info* pLdInfo = pLc->getInfo<Info>(nEntity);
			if (!pLdInfo)
			{
				errorCode = "节点" + std::to_string(nEntity) + "上的荷载修改失败！";
				return { false, errorCode };
			}

			if (!pLdInfo->check(std::vector<int>{}))
			{
				errorCode = "节点" + std::to_string(nEntity) + "上的荷载修改失败！";
				return { false, errorCode };
			}

			//压入影子命令
			//修改前压入，如果失败再弹出（经过数据检查的覆盖性操作几乎不会失败）
			//yilCString strCommand;
			//pLdInfo->formatAPDL(strCommand);
			//PUSH_SHADOW_CMD(THIS_IS_MOD, strCommand);

			pLdInfo->setTempDirc(eDirc);
			pLdInfo->setTemp(dTemp);
			pLdInfo->setLength(dLength);
		}
	}
		break;

	case PrepEnum::Load::GTemp:
	{
		int nEntity;
		std::string eDirect, eType;
		PrepEnum::TempDirc eDirc;
		PrepEnum::GTempType eT;
		int nNum;	// 本来需要解析参数，现在直接从loadValues的长度计算

		// 梯度温度荷载参数数值
		//B：考虑温度变化的宽度，宽度可缺省
		//H1：参考位置至定义温度间距离
		//T1：H1处对应温度
		//H2：参考位置至定义温度间距离
		//T2：H2处对应温度        
		py::list loadValues;

		vector <double> vecB(nNum, 0.0);
		vector <tuple<double, double, double, double>> vecTempCurve(nNum, std::make_tuple(0.0, 0.0, 0.0, 0.0));

		try {
			nEntity = safe_cast<int>(kwargs, "nEntity");
			eDirect = safe_cast<std::string>(kwargs, "eDirect");
			eType = safe_cast<double>(kwargs, "eType");
			//nNum = safe_cast<int>(kwargs, "nNum");	// 本来有这个参数，现在直接从loadValues的长度计算
			loadValues = safe_cast<py::list>(kwargs, "loadValues");
			nNum = py::len(loadValues);
			if (!GetCommand()->StrToTempDirc(eDirc, eDirect, 4))
			{
				errorCode = "参数 局部方向 解析错误！";
				return { false, errorCode };
			}
			if (!GetCommand()->StrToGTempType(eT, eType, 5))
			{
				errorCode = "参数 梁的参考位置 解析错误！";
				return { false, errorCode };
			}
			yilUnitScale* pUnit = GetProject()->GetUnit();
			for (int i = 0; i < nNum; ++i)
			{
				auto values = loadValues[i].cast<py::list>();
				double B = values[0].cast<double>(), 
					H1 = values[1].cast<double>(),
					T1 = values[2].cast<double>(),
					H2 = values[3].cast<double>(),
					T2 = values[4].cast<double>();

				vecB[i] = pUnit->LocalLengthToSI(B);
				vecTempCurve[i] = std::make_tuple(
					pUnit->LocalLengthToSI(H1),
					pUnit->LocalTemperatureToSI(T1),
					pUnit->LocalLengthToSI(H2),
					pUnit->LocalTemperatureToSI(T2)
				);
			}
		}
		catch (const std::runtime_error& e) {
			// 处理错误
			return { false, e.what() };
		}

		using Info = PREP::GradientTempInfo;
		auto* pEM = GET_PREP_ELEMENT();
		auto* pEle = pEM->getInfo(nEntity);
		if (pEle->getType() != PrepEnum::Element::Beam) {
			errorCode = "梯度温度荷载作用单元类型有误！";
			return { false, errorCode };
		}

		if (!pLc->getInfo<Info>(nEntity))		// 创建荷载
		{
			auto* pLdInfo = new Info();
			if (!pLdInfo->setEntityNO(nEntity)) {
				delete pLdInfo; pLdInfo = nullptr;
				errorCode = "荷载作用的节点" + std::to_string(nEntity) + "设定失败！";
				return { false, errorCode };
			}

			auto* pBeam = dynamic_cast<PREP::BeamInfo*>(pEle);
			auto vSec = pBeam->getSecVec();

			pLdInfo->setType(PrepEnum::Load::GTemp);
			pLdInfo->setTempDirc(eDirc);
			pLdInfo->setGTempType(eT);
			pLdInfo->setTempCurve(vecTempCurve);
			pLdInfo->setB(vecB, vSec);

			if (!pLc->append<Info>(pLdInfo)) {

				delete pLdInfo; pLdInfo = nullptr;
				errorCode = "荷载加入荷载工况" + strLCName + "失败！";
				return { false,  errorCode };
			}

			////压入影子命令
			//yilCString strCommand;
			//strCommand.Format(_T("LoadDel,GTEMP,%s,%d"), _lc->getName().c_str(), _entity);
			//PUSH_SHADOW_CMD(THIS_IS_DEF, strCommand);

		}
		else
		{
			Info* pLdInfo = pLc->getInfo<Info>(nEntity);
			if (!pLdInfo)
			{
				errorCode = "节点" + std::to_string(nEntity) + "上的荷载修改失败！";
				return { false, errorCode };
			}

			if (!pLdInfo->check(std::vector<int>{}))
			{
				errorCode = "节点" + std::to_string(nEntity) + "上的荷载修改失败！";
				return { false, errorCode };
			}

			//压入影子命令
			//修改前压入，如果失败再弹出（经过数据检查的覆盖性操作几乎不会失败）
			//yilCString strCommand;
			//pLdInfo->formatAPDL(strCommand);
			//PUSH_SHADOW_CMD(THIS_IS_MOD, strCommand);

			auto* pBeam = dynamic_cast<PREP::BeamInfo*>(pEle);
			auto vSec = pBeam->getSecVec();
			pLdInfo->setTempDirc(eDirc);
			pLdInfo->setGTempType(eT);
			pLdInfo->setTempCurve(vecTempCurve);
			pLdInfo->setB(vecB, vSec);
		}
	}
		break;

	case PrepEnum::Load::ElementSurface:
	{

	}
		break;

	case PrepEnum::Load::Pst:
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
	GetProject()->GetPlotControl()->InputChangedOn();
	GetProject()->GetPlotControl()->StructTreeChangedOn();	//用于通知界面刷新菜单
	GetProject()->GetPlotControl()->LoadGroupChangedOn();		//用于通知界面刷新大数据界面
	GetProject()->GetPlotControl()->LoadGroupTreeChangedOn();	//用于通知界面刷新菜单
	return { true, errorCode };
}