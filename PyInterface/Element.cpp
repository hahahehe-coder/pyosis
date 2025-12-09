#include "stdafx.h"
#include "PyInterface.h"

/// <summary>
/// 创建或修改单元
/// </summary>
/// <param name="nEle">单元编号</param>
/// <param name="element">单元类型: Beam3D Truss Spring Cable</param>
/// <param name="kwargs">单元参数</param>
/// <returns>是否成功，失败原因</returns>
std::pair<bool, std::string> PyInterface::OSIS_Element(int nEle /*= -1*/, const std::string eElementType, const py::dict kwargs)
{
	std::string errorCode;
	if (nEle == -1)
	{
		nEle = GET_PREP_ELEMENT()->getMaxNO() + 1;
	}
	else if (nEle > OSIS_MAX_INT_INDEX)
	{
		//m_ErrCode.Format(_T("单元索引号不应超过1,000,000"));
		//g_pWarningList->AddToTemperary(4, m_ErrCode);
		errorCode = "单元索引号不应超过" + std::to_string(OSIS_MAX_INT_INDEX) + "！";
		return { false, errorCode };
	}
	PrepEnum::Element eET = PrepEnum::Element::Unassigned;
	if (!m_pCommand->StrToElement(eET, eElementType, 2))
	{
		errorCode = "参数 单元类型 错误！";
		return { false, errorCode };
	}

	bool bState = true; //状态，true = 定义，false = 修改
	PREP::ElementInfo* pEle = GET_PREP_ELEMENT()->getInfo(nEle);
	if (pEle) 
	{
		if (pEle->getType() != eET)
		{
			//m_ErrCode.Format(_T("不支持修改单元类型！"));
			//g_pWarningList->AddToTemperary(4, m_ErrCode);
			errorCode = "不支持修改单元类型！";
			return { false, errorCode };
		}

		bState = false;
	}

	auto pEM = GET_PREP_ELEMENT();
	auto pNM = GET_PREP_NODE();
	switch (eET)
	{
	case PrepEnum::Element::Beam:
	{
		//SET_ELEMENT(AppendBeam3D, ResetBeam3D);
		int nNode1 = 0, nNode2 = 0;
		int nMat = 0;	// 材料编号
		int nSec1 = 0; int nSec2 = 0;
		int nYTrans = 1; int nZTrans = 1;	// y/z轴截面变化次方
		double dStrain = 0.0;
		bool bFlag = false; //false = beta定义theta；true = keyPt定义theta
		double dTheta = 0.0;
		bool bWarping = false;

		try {
			nNode1 = safe_cast<int>(kwargs, "nNode1");
			nNode2 = safe_cast<int>(kwargs, "nNode2");
			nMat = safe_cast<int>(kwargs, "nMat");
			nSec1 = safe_cast<int>(kwargs, "nSec1");
			nSec2 = safe_cast<int>(kwargs, "nSec2");
			nYTrans = safe_cast<int>(kwargs, "nYTrans");
			nZTrans = safe_cast<int>(kwargs, "nZTrans");
			dStrain = safe_cast<double>(kwargs, "dStrain");
			bFlag = safe_cast<bool>(kwargs, "bFlag");
			dTheta = safe_cast<double>(kwargs, "dTheta");
			bWarping = safe_cast<bool>(kwargs, "bWarping");
		}
		catch (const std::runtime_error& e) {
			// 处理错误
			return { false, e.what() };
		}

		if (nNode1 == nNode2)
		{
			/*m_ErrCode.Format(_T("单元[%d]的节点重复！"), _index);
			g_pWarningList->AddToTemperary(4, m_ErrCode, -1);*/
			errorCode = "单元" + std::to_string(nEle) + "的节点重复！";
			return { false, errorCode };
		}
		if (!pNM->getInfo(nNode1))
		{
			/*m_ErrCode.Format(_T("单元[%d]：节点[%d]不存在！"), _index, nNode1);
			g_pWarningList->AddToTemperary(4, m_ErrCode, -1);*/
			errorCode = "单元" + std::to_string(nEle) + "：节点" + std::to_string(nNode1) + "不存在！";
			return { false, errorCode };
		}
		if (!pNM->getInfo(nNode2))
		{
			/*m_ErrCode.Format(_T("单元[%d]：节点[%d]不存在！"), _index, nNode2);
			g_pWarningList->AddToTemperary(4, m_ErrCode, -1);*/
			errorCode = "单元" + std::to_string(nEle) + "：节点" + std::to_string(nNode2) + "不存在！";
			return { false, errorCode };
		}
		if (!GET_PREP_MAT()->getInfo(nMat))
		{
			/*m_ErrCode.Format(_T("单元[%d]：材料[%d]不存在！"), _index, nMat);
			g_pWarningList->AddToTemperary(4, m_ErrCode, -1);*/
			errorCode = "单元" + std::to_string(nEle) + "：材料" + std::to_string(nMat) + "不存在！";
			return { false, errorCode };
		}
		if (!GET_PREP_SEC()->getInfo(nSec1))
		{
			/*m_ErrCode.Format(_T("单元[%d]：截面[%d]不存在！"), _index, nSec1);
			g_pWarningList->AddToTemperary(4, m_ErrCode, -1);*/
			errorCode = "单元" + std::to_string(nEle) + "：截面" + std::to_string(nSec1) + "不存在！";
			return { false, errorCode };
		}
		if (!GET_PREP_SEC()->getInfo(nSec2))
		{
			/*m_ErrCode.Format(_T("单元[%d]：截面[%d]不存在！"), _index, nSec2);
			g_pWarningList->AddToTemperary(4, m_ErrCode, -1);*/
			errorCode = "单元" + std::to_string(nEle) + "：截面" + std::to_string(nSec2) + "不存在！";
			return { false, errorCode };
		}

		if (bState)
		{
			if (nYTrans > 4)
			{
				/*m_ErrCode.Format(_T("单元[%d]：局部坐标系Y轴的过渡形式仅支持1到4的整数幂指数！"), _index);
				g_pWarningList->AddToTemperary(4, m_ErrCode, -1);*/
				errorCode = "单元" + std::to_string(nEle) + "：局部坐标系Y轴的过渡形式仅支持1到4的整数幂指数！";
				return { false, errorCode };
			}
			if (nZTrans > 4)
			{
				/*m_ErrCode.Format(_T("单元[%d]：局部坐标系Z轴的过渡形式仅支持1到4的整数幂指数！"), _index);
				g_pWarningList->AddToTemperary(4, m_ErrCode, -1);*/
				errorCode = "单元" + std::to_string(nEle) + "：局部坐标系Z轴的过渡形式仅支持1到4的整数幂指数！";
				return { false, errorCode };
			}
			double dBeta = 0.0;
			int nKeyPt = -1;
			if (bFlag) { //定义关键点

				nKeyPt = static_cast<int>(dTheta);

				auto* pNode3 = pNM->getInfo(nKeyPt);
				if (!pNode3)
				{
					/*m_ErrCode.Format(_T("单元[%d]：关键节点[%d]不存在！"), _index, nKeyPt);
					g_pWarningList->AddToTemperary(4, m_ErrCode, -1);*/
					errorCode = "单元" + std::to_string(nEle) + "：关键节点" + std::to_string(nKeyPt) + "不存在！";
					return { false, errorCode };
				}
			}
			else {

				dBeta = dTheta;
			}

			PREP::BeamInfo* pEleInfo = new PREP::BeamInfo();

			pEleInfo->setNO(nEle);
			pEleInfo->setType(PrepEnum::Element::Beam);

			pEleInfo->setNodeVec(std::vector<int>{nNode1, nNode2});

			pEleInfo->setMat(nMat);

			pEleInfo->setSecVec(std::vector<int>{ nSec1, nSec2 });
			pEleInfo->setTransVec(std::vector<int>{nYTrans, nZTrans});

			pEleInfo->setStrain(dStrain);

			pEleInfo->setFlag(bFlag);
			pEleInfo->setKeyPt(nKeyPt);
			pEleInfo->setBeta(dBeta);

			pEleInfo->setWarp(bWarping);

			if (!pEM->append(pEleInfo)) {

				delete pEleInfo; pEleInfo = nullptr;
				errorCode = "添加单元信息失败！";
				return { false, errorCode };
			}

			////压入影子命令
			//PUSH_SHADOW_CMD_CUSTOM(THIS_IS_DEF, "ElementDel,%d", _index);
		}
		else
		{
			double dBeta = 0.0;
			int nKeyPt = -1;
			if (bFlag) { //定义关键点

				nKeyPt = static_cast<int>(dTheta);

				auto* pNode3 = pNM->getInfo(nKeyPt);
				if (!pNode3)
				{
					/*m_ErrCode.Format(_T("单元[%d]：关键节点[%d]不存在！"), _index, nKeyPt);
					g_pWarningList->AddToTemperary(4, m_ErrCode, -1);*/
					errorCode = "单元" + std::to_string(nEle) + "：关键节点" + std::to_string(nKeyPt) + "不存在！";
					return { false, errorCode };
				}
			}
			else {
				dBeta = dTheta;
			}


			PREP::BeamInfo* pEleInfo = (PREP::BeamInfo*)(pEM->getInfo(nEle));
			if (!pEleInfo)
			{
				errorCode = "未在数据库中找到编号为：" + std::to_string(nEle) + "的单元！";
				return { false, errorCode };
			}

			if (!pEleInfo->check(std::vector<int>{nNode1, nNode2})) 
			{
				delete pEleInfo; pEleInfo = nullptr;
				errorCode = "当前单元：编号" + std::to_string(nEle) + "的属性不合法，已被占用！";
				return { false, errorCode };
			}


			//压入影子命令
			//修改前压入，如果失败再弹出（经过数据检查的覆盖性操作几乎不会失败）
			//PUSH_SHADOW_CMD_DEFAULT(THIS_IS_MOD, pEleInfo);


			pEleInfo->setNO(nEle);
			pEleInfo->setType(PrepEnum::Element::Beam);

			pEleInfo->setNodeVec(std::vector<int>{nNode1, nNode2});

			pEleInfo->setMat(nMat);

			pEleInfo->setSecVec(std::vector<int>{ nSec1, nSec2 });
			pEleInfo->setTransVec(std::vector<int>{nYTrans, nZTrans});

			pEleInfo->setStrain(dStrain);

			pEleInfo->setFlag(bFlag);
			pEleInfo->setKeyPt(nKeyPt);
			pEleInfo->setBeta(dBeta);
			pEleInfo->setWarp(bWarping);
		}
		m_pProject->GetPlotControl()->InputChangedOn();
		m_pProject->GetPlotControl()->StructTreeChangedOn();
		m_pProject->GetPlotControl()->ElementDataChangedOn();
		m_pProject->GetPlotControl()->ElemPropBeam3DsDataChangedOn();
		break;
	}
		break;
	case PrepEnum::Element::Truss:  
	{
		int nNode1 = 0, nNode2 = 0;
		int nMat = 0;	// 材料编号
		int nSec1 = 0; int nSec2 = 0;
		double dStrain = 0.0;

		nNode1 = kwargs["nNode1"].cast<int>();
		nNode2 = kwargs["nNode2"].cast<int>();
		nMat = kwargs["nMat"].cast<int>();
		nSec1 = kwargs["nSec1"].cast<int>();
		nSec2 = kwargs["nSec2"].cast<int>();
		dStrain = kwargs["dStrain"].cast<double>();

		try {
			nNode1 = safe_cast<int>(kwargs, "nNode1");
			nNode2 = safe_cast<int>(kwargs, "nNode2");
			nMat = safe_cast<int>(kwargs, "nMat");
			nSec1 = safe_cast<int>(kwargs, "nSec1");
			nSec2 = safe_cast<int>(kwargs, "nSec2");
			dStrain = safe_cast<double>(kwargs, "dStrain");
		}
		catch (const std::runtime_error& e) {
			// 处理错误
			return { false, e.what() };
		}
		if (nNode1 == nNode2)
		{
			/*m_ErrCode.Format(_T("单元[%d]的节点重复！"), _index);
			g_pWarningList->AddToTemperary(4, m_ErrCode, -1);*/
			errorCode = "单元" + std::to_string(nEle) + "的节点重复！";
			return { false, errorCode };
		}
		if (!pNM->getInfo(nNode1))
		{
			/*m_ErrCode.Format(_T("单元[%d]：节点[%d]不存在！"), _index, nNode1);
			g_pWarningList->AddToTemperary(4, m_ErrCode, -1);*/
			errorCode = "单元" + std::to_string(nEle) + "：节点" + std::to_string(nNode1) + "不存在！";
			return { false, errorCode };
		}
		if (!pNM->getInfo(nNode2))
		{
			/*m_ErrCode.Format(_T("单元[%d]：节点[%d]不存在！"), _index, nNode2);
			g_pWarningList->AddToTemperary(4, m_ErrCode, -1);*/
			errorCode = "单元" + std::to_string(nEle) + "：节点" + std::to_string(nNode2) + "不存在！";
			return { false, errorCode };
		}
		if (!GET_PREP_MAT()->getInfo(nMat))
		{
			/*m_ErrCode.Format(_T("单元[%d]：材料[%d]不存在！"), _index, nMat);
			g_pWarningList->AddToTemperary(4, m_ErrCode, -1);*/
			errorCode = "单元" + std::to_string(nEle) + "：材料" + std::to_string(nMat) + "不存在！";
			return { false, errorCode };
		}
		if (!GET_PREP_SEC()->getInfo(nSec1))
		{
			/*m_ErrCode.Format(_T("单元[%d]：截面[%d]不存在！"), _index, nSec1);
			g_pWarningList->AddToTemperary(4, m_ErrCode, -1);*/
			errorCode = "单元" + std::to_string(nEle) + "：截面" + std::to_string(nSec1) + "不存在！";
			return { false, errorCode };
		}
		nSec2 = nSec1; //先强制等截面

		if (bState)
		{
			PREP::TrussInfo* pEleInfo = new PREP::TrussInfo();

			pEleInfo->setNO(nEle);
			pEleInfo->setType(PrepEnum::Element::Truss);

			pEleInfo->setNodeVec(std::vector<int>{nNode1, nNode2});

			pEleInfo->setMat(nMat);

			pEleInfo->setSecVec(std::vector<int>{ nSec1, nSec2 });
			pEleInfo->setStrain(dStrain);

			if (!pEM->append(pEleInfo)) {

				delete pEleInfo; pEleInfo = nullptr;
				errorCode = "添加单元信息失败！";
				return { false, errorCode };
			}

			////压入影子命令
			//PUSH_SHADOW_CMD_CUSTOM(THIS_IS_DEF, "ElementDel,%d", _index);
		}
		else
		{
			PREP::TrussInfo* pEleInfo = (PREP::TrussInfo*)(pEM->getInfo(nEle));
			if (!pEleInfo)
			{
				errorCode = "未在数据库中找到编号为：" + std::to_string(nEle) + "的单元！";
				return { false, errorCode };
			}

			if (!pEleInfo->check(std::vector<int>{nNode1, nNode2}))
			{
				delete pEleInfo; pEleInfo = nullptr;
				errorCode = "当前单元：编号" + std::to_string(nEle) + "的属性不合法，已被占用！";
				return { false, errorCode };
			}

			////压入影子命令
			////修改前压入，如果失败再弹出（经过数据检查的覆盖性操作几乎不会失败）
			//PUSH_SHADOW_CMD_DEFAULT(THIS_IS_MOD, pEleInfo);

			pEleInfo->setNO(nEle);
			pEleInfo->setNodeVec(std::vector<int>{nNode1, nNode2});
			pEleInfo->setMat(nMat);
			pEleInfo->setSecVec(std::vector<int>{ nSec1, nSec2 });
			pEleInfo->setStrain(dStrain);

		}
		m_pProject->GetPlotControl()->InputChangedOn();
		m_pProject->GetPlotControl()->StructTreeChangedOn();
		m_pProject->GetPlotControl()->ElementDataChangedOn();
		m_pProject->GetPlotControl()->ElemPropBeam3DsDataChangedOn();
	}
		break;
	case PrepEnum::Element::Spring: 
	{
		// node1, node2, linear, dx, dy, dz, rx, ry, rz, beta
		int nNode1 = 0, nNode2 = 0;
		bool bLinear = 0;
		double dBeta = 0.0;

		nNode1 = kwargs["nNode1"].cast<int>();
		nNode2 = kwargs["nNode2"].cast<int>();
		bLinear = kwargs["bLinear"].cast<bool>();
		dBeta = kwargs["dBeta"].cast<double>();

		try {
			nNode1 = safe_cast<int>(kwargs, "nNode1");
			nNode2 = safe_cast<int>(kwargs, "nNode2");
			bLinear = safe_cast<bool>(kwargs, "bLinear");
			dBeta = safe_cast<double>(kwargs, "dBeta");
		}
		catch (const std::runtime_error& e) {
			// 处理错误
			return { false, e.what() };
		}

		if (nNode1 == nNode2)
		{
			/*m_ErrCode.Format(_T("单元[%d]的节点重复！"), _index);
			g_pWarningList->AddToTemperary(4, m_ErrCode, -1);*/
			errorCode = "单元" + std::to_string(nEle) + "的节点重复！";
			return { false, errorCode };
		}
		if (!pNM->getInfo(nNode1))
		{
			/*m_ErrCode.Format(_T("单元[%d]：节点[%d]不存在！"), _index, nNode1);
			g_pWarningList->AddToTemperary(4, m_ErrCode, -1);*/
			errorCode = "单元" + std::to_string(nEle) + "：节点" + std::to_string(nNode1) + "不存在！";
			return { false, errorCode };
		}
		if (!pNM->getInfo(nNode2))
		{
			/*m_ErrCode.Format(_T("单元[%d]：节点[%d]不存在！"), _index, nNode2);
			g_pWarningList->AddToTemperary(4, m_ErrCode, -1);*/
			errorCode = "单元" + std::to_string(nEle) + "：节点" + std::to_string(nNode2) + "不存在！";
			return { false, errorCode };
		}

		// 刚度相关参数
		std::vector<double> vecK(6, 0.);
		std::vector<int> vecCurveNO(6, -1);
		vecCurveNO.reserve(6); vecCurveNO.assign(6, -1);
		if (bLinear)  //线性
		{
			vecK[0] = kwargs["dx"].cast<double>();
			vecK[1] = kwargs["dy"].cast<double>();
			vecK[2] = kwargs["dz"].cast<double>();
			vecK[3] = kwargs["rx"].cast<double>();
			vecK[4] = kwargs["ry"].cast<double>();
			vecK[5] = kwargs["rz"].cast<double>();

			yilUnitScale* pUnit = m_pProject->GetUnit();
			for (int it = 0; it < 3; it++)
				vecK[it] = pUnit->LocalLineForceToSI(vecK[it]);

			for (int it = 3; it < 6; it++)
				vecK[it] = pUnit->LocalMomentToSI(vecK[it]);

			if (std::fabs(vecK[0]) < OSIS_PRECISION &&
				std::fabs(vecK[1]) < OSIS_PRECISION &&
				std::fabs(vecK[2]) < OSIS_PRECISION &&
				std::fabs(vecK[3]) < OSIS_PRECISION &&
				std::fabs(vecK[4]) < OSIS_PRECISION &&
				std::fabs(vecK[5]) < OSIS_PRECISION)
			{
				/*m_ErrCode.Format(_T("%d号弹簧单元的刚度全部为零！"), _index);
				g_pWarningList->AddToTemperary(4, m_ErrCode, -1);*/
				errorCode = std::to_string(nEle) + "号弹簧单元的刚度全部为零！";
				return { false, errorCode };
			}
		}
		else  //非线性
		{
			vecCurveNO[0] = kwargs["dx"].cast<int>();
			vecCurveNO[1] = kwargs["dy"].cast<int>();
			vecCurveNO[2] = kwargs["dz"].cast<int>();
			vecCurveNO[3] = kwargs["rx"].cast<int>();
			vecCurveNO[4] = kwargs["ry"].cast<int>();
			vecCurveNO[5] = kwargs["rz"].cast<int>();

			bool bFlag = true;
			for (auto it : vecCurveNO)
			{
				if (it != -1)
				{
					bFlag = false;
				}
				else
				{
					continue;
				}

				if (!PREP::PropertyManager::getInstance()->getInfo<PREP::PUCurveInfo>(it))
				{
					/*m_ErrCode.Format(_T("%d号力(矩) - 位移曲线不存在！"), it);
					g_pWarningList->AddToTemperary(4, m_ErrCode, -1);*/
					errorCode = std::to_string(it) + "号力(矩) - 位移曲线不存在！";
					return { false, errorCode };
				}
			}

			if (bFlag)
			{
				/*m_ErrCode.Format(_T("%d号弹簧单元至少定义一个自由度上的的刚度曲线！"), _index);
				g_pWarningList->AddToTemperary(4, m_ErrCode, -1);*/
				errorCode = std::to_string(nEle) + "号弹簧单元至少定义一个自由度上的的刚度曲线！";
				return { false, errorCode };
			}
		}

		if (bState)
		{
			PREP::SpringInfo* pEleInfo = new PREP::SpringInfo();

			pEleInfo->setNO(nEle);
			pEleInfo->setType(PrepEnum::Element::Spring);

			pEleInfo->setNodeVec(std::vector<int>{nNode1, nNode2});

			pEleInfo->setMat(-1);
			pEleInfo->setLinear(bLinear);

			map<yilEnumType::DOFType, double> mapProp;
			if (bLinear) {

				for (int i = 0; i < 6; i++)
					mapProp[static_cast<yilEnumType::DOFType>(i + 1)] = vecK[i];
			}
			else {

				auto* pPM = PREP::PropertyManager::getInstance();
				for (int i = 0; i < 6; i++)
					mapProp[static_cast<yilEnumType::DOFType>(i + 1)] = static_cast<int>(vecCurveNO[i]);
			}

			pEleInfo->setProp(mapProp);
			pEleInfo->setBeta(dBeta);

			if (!pEM->append(pEleInfo)) {

				delete pEleInfo; pEleInfo = nullptr;
				errorCode = "添加单元信息失败！";
				return { false, errorCode };
			}

			//压入影子命令
			//PUSH_SHADOW_CMD_CUSTOM(THIS_IS_DEF, "ElementDel,%d", _index);
		}
		else
		{
			PREP::SpringInfo* pEleInfo = (PREP::SpringInfo*)pEM->getInfo(nEle);
			if (!pEleInfo)
			{
				errorCode = "未在数据库中找到编号为：" + std::to_string(nEle) + "的单元！";
				return { false, errorCode };
			}

			if (!pEleInfo->check(std::vector<int>{nNode1, nNode2}))
			{
				delete pEleInfo; pEleInfo = nullptr;
				errorCode = "当前单元：编号" + std::to_string(nEle) + "的属性不合法，已被占用！";
				return { false, errorCode };
			}

			////压入影子命令
			////修改前压入，如果失败再弹出（经过数据检查的覆盖性操作几乎不会失败）
			//PUSH_SHADOW_CMD_DEFAULT(THIS_IS_MOD, pEleInfo);

			pEleInfo->setNO(nEle);
			pEleInfo->setType(PrepEnum::Element::Spring);

			pEleInfo->setNodeVec(std::vector<int>{nNode1, nNode2});

			pEleInfo->setMat(-1);
			pEleInfo->setLinear(bLinear);


			map<yilEnumType::DOFType, double> mapProp;
			if (bLinear) {

				for (int i = 0; i < 6; i++)
					mapProp[static_cast<yilEnumType::DOFType>(i + 1)] = vecK[i];
			}
			else {

				auto* pPM = GET_PREP_PROP();
				for (int i = 0; i < 6; i++)
					mapProp[static_cast<yilEnumType::DOFType>(i + 1)] = static_cast<int>(vecCurveNO[i]);
			}

			pEleInfo->setProp(mapProp);
			pEleInfo->setBeta(dBeta);
		}
		m_pProject->GetPlotControl()->InputChangedOn();
		m_pProject->GetPlotControl()->StructTreeChangedOn();
		m_pProject->GetPlotControl()->ElementDataChangedOn();
		m_pProject->GetPlotControl()->ElemPropBeam3DsDataChangedOn();
	}
		break;
	case PrepEnum::Element::Cable:  
	{
		int nNode1 = 0, nNode2 = 0;
		int nMat = 0;	// 材料编号
		int nSec;
		std::string eMet;
		PrepEnum::CableDef eMethod;
		double dPara = 0.0;

		try {
			nNode1 = safe_cast<int>(kwargs, "nNode1");
			nNode2 = safe_cast<int>(kwargs, "nNode2");
			nMat = safe_cast<int>(kwargs, "nMat");
			nSec = safe_cast<int>(kwargs, "nSec");
			eMet = safe_cast<std::string>(kwargs, "eMethod");
			dPara = safe_cast<double>(kwargs, "dPara");
		}
		catch (const std::runtime_error& e) {
			// 处理错误
			return { false, e.what() };
		}
		if (!m_pCommand->StrToCableDef(eMethod, eMet, 7))
		{
			errorCode = "参数 应力方法 错误！";
			return { false, errorCode };
		}
		if (nNode1 == nNode2)
		{
			/*m_ErrCode.Format(_T("单元[%d]的节点重复！"), _index);
			g_pWarningList->AddToTemperary(4, m_ErrCode, -1);*/
			errorCode = "单元" + std::to_string(nEle) + "的节点重复！";
			return { false, errorCode };
		}
		if (!pNM->getInfo(nNode1))
		{
			/*m_ErrCode.Format(_T("单元[%d]：节点[%d]不存在！"), _index, nNode1);
			g_pWarningList->AddToTemperary(4, m_ErrCode, -1);*/
			errorCode = "单元" + std::to_string(nEle) + "：节点" + std::to_string(nNode1) + "不存在！";
			return { false, errorCode };
		}
		if (!pNM->getInfo(nNode2))
		{
			/*m_ErrCode.Format(_T("单元[%d]：节点[%d]不存在！"), _index, nNode2);
			g_pWarningList->AddToTemperary(4, m_ErrCode, -1);*/
			errorCode = "单元" + std::to_string(nEle) + "：节点" + std::to_string(nNode2) + "不存在！";
			return { false, errorCode };
		}
		if (!GET_PREP_MAT()->getInfo(nMat))
		{
			/*m_ErrCode.Format(_T("单元[%d]：材料[%d]不存在！"), _index, nMat);
			g_pWarningList->AddToTemperary(4, m_ErrCode, -1);*/
			errorCode = "单元" + std::to_string(nEle) + "：材料" + std::to_string(nMat) + "不存在！";
			return { false, errorCode };
		}
		if (!GET_PREP_SEC()->getInfo(nSec))
		{
			/*m_ErrCode.Format(_T("单元[%d]：截面[%d]不存在！"), _index, nSec1);
			g_pWarningList->AddToTemperary(4, m_ErrCode, -1);*/
			errorCode = "单元" + std::to_string(nEle) + "：截面" + std::to_string(nSec) + "不存在！";
			return { false, errorCode };
		}
		yilUnitScale* pUnit = m_pProject->GetUnit();
		switch (eMethod)
		{
		case PrepEnum::CableDef::UnstressedLength:  dPara = pUnit->LocalLengthToSI(dPara); break;

		case PrepEnum::CableDef::InitForce:
		case PrepEnum::CableDef::HorzForce:
		case PrepEnum::CableDef::VerticalForce:     dPara = pUnit->LocalForceToSI(dPara); break;
		default:
			return { false, errorCode };
		}
		if (bState)
		{
			PREP::CableInfo* pEleInfo = new PREP::CableInfo();

			//检测拉索定义方式的合法性和一致性
			if (!pEleInfo->check(eMethod, dPara))
			{
				delete pEleInfo; pEleInfo = nullptr;
				errorCode = "当前单元：编号" + std::to_string(nEle) + "的属性不合法，已被占用！";
				return { false, errorCode };
			}

			double x1 = pNM->getAllInfo()->at(nNode1)->x;
			double y1 = pNM->getAllInfo()->at(nNode1)->y;
			double x2 = pNM->getAllInfo()->at(nNode2)->x;
			double y2 = pNM->getAllInfo()->at(nNode2)->y;

			if (!pEleInfo->check(x1, y1, x2, y2)) {

				delete pEleInfo; pEleInfo = nullptr;
				errorCode = "不支持建立竖向索单元！";
				/*m_ErrCode.Format(_T("不支持建立竖向索单元！"));
				g_pWarningList->AddToTemperary(4, m_ErrCode, -1);*/
				return { false, errorCode };
			}

			pEleInfo->setNO(nEle);
			pEleInfo->setType(PrepEnum::Element::Cable);
			pEleInfo->setNodeVec(std::vector<int>{nNode1, nNode2});
			pEleInfo->setMat(nMat);

			pEleInfo->setSec(nSec);
			pEleInfo->setState(eMethod, dPara);

			if (!pEM->append(pEleInfo)) {

				delete pEleInfo; pEleInfo = nullptr;
				errorCode = "添加单元信息失败！";
				return { false, errorCode };
			}

			//压入影子命令
			//PUSH_SHADOW_CMD_CUSTOM(THIS_IS_DEF, "ElementDel,%d", _index);
		}
		else
		{
			yilUnitScale* pUnit = m_pProject->GetUnit();
			switch (eMethod) {

			case PrepEnum::CableDef::UnstressedLength:  dPara = pUnit->LocalLengthToSI(dPara); break;

			case PrepEnum::CableDef::InitForce:
			case PrepEnum::CableDef::HorzForce:
			case PrepEnum::CableDef::VerticalForce:     dPara = pUnit->LocalForceToSI(dPara); break;
			default:
				return { false, errorCode };
			}
			PREP::CableInfo* pEleInfo = (PREP::CableInfo*)(pEM->getInfo(nEle));
			if (!pEleInfo)
			{
				errorCode = "未在数据库中找到编号为：" + std::to_string(nEle) + "的单元！";
				return { false, errorCode };
			}

			//检测拉索定义方式的合法性和一致性
			if (!pEleInfo->check(eMethod, dPara))
			{
				delete pEleInfo; pEleInfo = nullptr;
				errorCode = "当前单元：编号" + std::to_string(nEle) + "的属性不合法，已被占用！";
				return { false, errorCode };
			}

			////压入影子命令
			////修改前压入，如果失败再弹出（经过数据检查的覆盖性操作几乎不会失败）
			//PUSH_SHADOW_CMD_DEFAULT(THIS_IS_MOD, pEleInfo);

			pEleInfo->setNO(nEle);
			pEleInfo->setType(PrepEnum::Element::Cable);
			pEleInfo->setNodeVec(std::vector<int>{nNode1, nNode2});
			pEleInfo->setMat(nMat);

			pEleInfo->setSec(nSec);
			pEleInfo->setState(eMethod, dPara);
		}
		m_pProject->GetPlotControl()->InputChangedOn();
		m_pProject->GetPlotControl()->StructTreeChangedOn();
		m_pProject->GetPlotControl()->ElementDataChangedOn();
		m_pProject->GetPlotControl()->ElemPropBeam3DsDataChangedOn(); //yanggj说单元是全量刷新，可以这么写
	}
		break;
	case PrepEnum::Element::Shell:  
	{
		bool bIsThin;
		int nMat = 0;
		int nThk = 0;
		int nNode1 = 0;
		int nNode2 = 0;
		int nNode3 = 0;
		int nNode4 = 0;
		try {
			bIsThin = safe_cast<bool>(kwargs, "bIsThin");
			nMat = safe_cast<int>(kwargs, "nMat");
			nThk = safe_cast<int>(kwargs, "nThk");
			nNode1 = safe_cast<int>(kwargs, "nNode1");
			nNode2 = safe_cast<int>(kwargs, "nNode2");
			nNode3 = safe_cast<int>(kwargs, "nNode3");
		}
		catch (const std::runtime_error& e) {
			// 处理错误
			return { false, e.what() };
		}

		if (nNode1 == nNode2 || nNode2 == nNode3 || nNode1 == nNode3)
		{
			/*m_ErrCode.Format(_T("单元[%d]的节点重复！"), _index);
			g_pWarningList->AddToTemperary(4, m_ErrCode, -1);*/
			errorCode = "单元" + std::to_string(nEle) + "的节点重复！";
			return { false, errorCode };
		}
		auto* pNode1 = pNM->getInfo(nNode1);
		if (!pNode1)
		{
			/*m_ErrCode.Format(_T("单元[%d]：节点[%d]不存在！"), _index, nNode1);
			g_pWarningList->AddToTemperary(4, m_ErrCode, -1);*/
			errorCode = "单元" + std::to_string(nEle) + "：节点" + std::to_string(nNode1) + "不存在！";
			return { false, errorCode };
		}
		auto* pNode2 = pNM->getInfo(nNode2);
		if (!pNode2)
		{
			/*m_ErrCode.Format(_T("单元[%d]：节点[%d]不存在！"), _index, nNode2);
			g_pWarningList->AddToTemperary(4, m_ErrCode, -1);*/
			errorCode = "单元" + std::to_string(nEle) + "：节点" + std::to_string(nNode2) + "不存在！";
			return { false, errorCode };
		}
		auto* pNode3 = pNM->getInfo(nNode3);
		if (!pNode3)
		{
			/*m_ErrCode.Format(_T("单元[%d]：节点[%d]不存在！"), _index, nNode2);
			g_pWarningList->AddToTemperary(4, m_ErrCode, -1);*/
			errorCode = "单元" + std::to_string(nEle) + "：节点" + std::to_string(nNode3) + "不存在！";
			return { false, errorCode };
		}
		if (!GET_PREP_MAT()->getInfo(nMat))
		{
			/*m_ErrCode.Format(_T("单元[%d]：材料[%d]不存在！"), _index, nMat);
			g_pWarningList->AddToTemperary(4, m_ErrCode, -1);*/
			errorCode = "单元" + std::to_string(nEle) + "：材料" + std::to_string(nMat) + "不存在！";
			return { false, errorCode };
		}
		if (!GET_PREP_PROP()->getInfo<PREP::ShellThicknessInfo>(nThk))
		{
			/*m_ErrCode.Format(_T("单元[%d]：厚度[%d]不存在！"), _index, nThk);
			g_pWarningList->AddToTemperary(4, m_ErrCode, -1);*/
			errorCode = "单元" + std::to_string(nEle) + "：厚度" + std::to_string(nThk) + "不存在！";
			return { false, errorCode };
		}

		auto bIsFourNode = kwargs.attr("get")("nNode4");
		if (bIsFourNode)
		{
			try {
				nNode4 = safe_cast<int>(kwargs, "nNode4");
			}
			catch (const std::runtime_error& e) {
				// 处理错误
				return { false, e.what() };
			}
			auto* pNode4 = pNM->getInfo(nNode4);
			if (!pNode4)
			{
				/*m_ErrCode.Format(_T("单元[%d]：节点[%d]不存在！"), _index, nNode2);
				g_pWarningList->AddToTemperary(4, m_ErrCode, -1);*/
				errorCode = "单元" + std::to_string(nEle) + "：节点" + std::to_string(nNode4) + "不存在！";
				return { false, errorCode };
			}
			const Geo::Point3D p1(pNode1->x, pNode1->y, pNode1->z);
			const Geo::Point3D p2(pNode2->x, pNode2->y, pNode2->z);
			const Geo::Point3D p3(pNode3->x, pNode3->y, pNode3->z);
			const Geo::Point3D p4(pNode4->x, pNode4->y, pNode4->z);
			if (!Geo::Point3D::isCoplane(p1, p2, p3, p4))
			{
				/*m_ErrCode.Format(_T("单元[%d]：节点应共面"), nEle);
				g_pWarningList->AddToTemperary(4, m_ErrCode, -1);*/
				errorCode = "单元" + std::to_string(nEle) + "节点应共面!";
				return { false, errorCode };
			}

			const Geo::Vector3D vkt12 = p2 - p1;
			const Geo::Vector3D vkt23 = p3 - p2;
			const Geo::Vector3D vkt34 = p4 - p3;

			if (vkt12.cross(vkt23).dot(vkt23.cross(vkt34)) < 0)
			{
				int nNode = 0;
				nNode = nNode4;
				nNode4 = nNode3;
				nNode3 = nNode;
			}
		}
		if (bState)
		{
			PREP::ShellInfo* pEleInfo = new PREP::ShellInfo();

			pEleInfo->setNO(nEle);

			if (bIsFourNode) {
				pEleInfo->setNodeVec(std::vector<int>{nNode1, nNode2, nNode3, nNode4});
			}
			else {
				pEleInfo->setNodeVec(std::vector<int>{nNode1, nNode2, nNode3});
			}

			pEleInfo->setMat(nMat);

			pEleInfo->setIsThin(bIsThin);
			if (!pEleInfo->setThickness(nThk)) {
				delete pEleInfo; pEleInfo = nullptr;
				errorCode = "设置壳厚度属性出错！";
				return { false, errorCode };
			}

			if (!GET_PREP_ELEMENT()->append(pEleInfo)) {

				delete pEleInfo; pEleInfo = nullptr;
				errorCode = "添加单元信息失败！";
				return { false, errorCode };
			}

			////压入影子命令
			//PUSH_SHADOW_CMD_CUSTOM(THIS_IS_DEF, "ElementDel,%d", _index);
		}
		else
		{
			auto* pEleInfo = pEM->getInfo(nEle);
			if (!pEleInfo)
			{
				errorCode = "未在数据库中找到编号为：" + std::to_string(nEle) + "的单元！";
				return { false, errorCode };
			}
			//压入影子命令
			//修改前压入，如果失败再弹出（经过数据检查的覆盖性操作几乎不会失败）
			//PUSH_SHADOW_CMD_DEFAULT(THIS_IS_MOD, pEleInfo);

			PREP::ShellInfo* pInfo = (PREP::ShellInfo*)(pEleInfo);

			if (bIsFourNode) {
				pInfo->setNodeVec(std::vector<int>{nNode1, nNode2, nNode3, nNode4});
			}
			else {
				pInfo->setNodeVec(std::vector<int>{nNode1, nNode2, nNode3});
			}

			pInfo->setMat(nMat);

			pInfo->setIsThin(bIsThin);
			if (!pInfo->setThickness(nThk)) {
				errorCode = "设置壳厚度属性出错！";
				return { false, errorCode };
			}
		}
	}
	    break;
	default:
		return { false, errorCode };
	}
	return { true, errorCode };
	//return { true, "成功创建单元，编号:" + std::to_string(nEle) + "！" };
}

/// <summary>
/// 删除单元
/// </summary>
/// <param name="nEle">单元编号</param>
/// <returns></returns>
std::pair<bool, std::string> PyInterface::OSIS_ElementDel(const int nEle)
{
	std::string errorCode;
	//ASSERT_FEM_STATUS();

	PREP::ElementInfo* pEleInfo = GET_PREP_ELEMENT()->getInfo(nEle);
	if (!pEleInfo) 
	{
		return { true, errorCode };
	}

	//ForwardCommand(pEleInfo); //命令流转发

	//压入影子命令
	//修改前压入，如果失败再弹出（经过数据检查的覆盖性操作几乎不会失败）
	//PUSH_SHADOW_CMD_DEFAULT(THIS_IS_DEL, pEleInfo);


	if (!GET_PREP_ELEMENT()->deleteByNO(nEle)) {
		//POP_SHADOW_CMD();
		errorCode = "删除单元" + std::to_string(nEle) + "失败";
		return { false, errorCode };
	}

	m_pProject->GetPlotControl()->InputChangedOn();
	m_pProject->GetPlotControl()->StructTreeChangedOn();	//用于通知界面刷新菜单
	m_pProject->GetPlotControl()->ElementDataChangedOn();	// 单元表格数据
	m_pProject->GetPlotControl()->ElemPropBeam3DsDataChangedOn();

	return { true, errorCode };
}

/// <summary>
/// 修改单元编号，单元编号存在时，交换
/// </summary>
/// <param name="nOld">旧编号</param>
/// <param name="nNew">新编号</param>
/// <returns></returns>
std::pair<bool, std::string> PyInterface::OSIS_ElementMod(const int nOld, const int nNew)
{
	std::string errorCode;
	//ASSERT_FEM_STATUS();

	auto* pInfo = GET_PREP_ELEMENT()->getInfo(nOld);
	if (!pInfo)
	{
		errorCode = "未找到编号为" + std::to_string(nOld) + "的单元！";
		return { false, errorCode };
	}

	auto swapIndex = [this](int nOld, int nNew) {

		auto* pInfo = GET_PREP_ELEMENT()->getInfo(nOld);
		if (!pInfo)
			return false;

		GET_PREP_ELEMENT()->modifyIndex(nOld, nNew); //虽然这句话应该在最后执行，但是限制于下面关联参数的数据检查，先放这里

		auto* pMM = GET_PREP_MAT();
		const int nMat = pInfo->getMat();
		auto* pMatInfo = pMM->getInfo(nMat);
		if (pMatInfo) {
			pMatInfo->removeRelatedElement(nOld);
			pMatInfo->appendRelatedElement(nNew);
		}
		pMM = nullptr;


		auto* pNM = GET_PREP_NODE();
		const vector<int> vktNode = pInfo->getNodeVec();
		for (auto it : vktNode) {
			auto* pNodeInfo = pNM->getInfo(it);
			pNodeInfo->removeRelatedElement(nOld);
			pNodeInfo->appendRelatedElement(nNew);
			//显示修改，参考迈达斯，默认把相关改的都给激活
			SEL_MGR_PTR()->ActiNode(it);
		}
		pNM = nullptr;


		auto* pEGM = GET_PREP_ELEGRP();
		vector<string> vktName;
		pEGM->getAllGroupName(vktName);
		for (auto& it : vktName) {

			if (!pEGM->isValidInGrp(it, nOld))
				continue;

			auto* pRelatedInfo = pEGM->getGroupInfo(it);
			if (!pRelatedInfo) continue;

			pRelatedInfo->getData().erase(nOld);
			pRelatedInfo->getData().emplace(nNew);
		}
		m_pProject->GetPlotControl()->LoadGroupTreeChangedOn();
		pEGM = nullptr;


		auto* pBM = GET_PREP_BD();
		const set<int>& setRelatedBoundary = pInfo->getRelatedBoundary();
		if (!setRelatedBoundary.empty()) {

			for (const int it : setRelatedBoundary) {
				auto* pRelatedInfo = pBM->getInfo(it);
				if (!pRelatedInfo) continue;
				pRelatedInfo->substituteEntityIndex(vector<int>{nOld}, vector<int>{nNew});

				if (PrepEnum::Boundary::MstSlv == pRelatedInfo->getType()) {
					auto* pCastPtr = dynamic_cast<PREP::MstSlvBoundaryInfo*>(pRelatedInfo);
					if (!pCastPtr) continue;
					pCastPtr->substituteMasterNO(nOld, nNew);
				}
			}

			m_pProject->GetPlotControl()->BoundaryChangedOn();
		}
		pBM = nullptr;


		auto* pLCM = GET_PREP_LC();
		const auto& vecRelatedTendonShape = pInfo->getRelatedTendonShape();
		if (!vecRelatedTendonShape.empty()) {
			for (auto& it : vecRelatedTendonShape) {

				auto* pRelatedInfo = pLCM->getAuxParasInfo<PREP::TendonShapeInfo>(it);
				if (!pRelatedInfo) continue;

				if (pRelatedInfo->getLayoutRefType() == PREP::LoadEnum::LayoutRefType::Element) {

					auto pLayoutRef = std::dynamic_pointer_cast<PREP::LayoutRefByEle>(pRelatedInfo->getLayoutRef());
					if (!pLayoutRef) continue;
					pLayoutRef->setEle(nNew);
				}

				m_pProject->GetPlotControl()->TendonVecDataChangedOn();
			}
		}
		const auto& setRelatedLoad = pInfo->getRelatedLoad();
		if (!setRelatedLoad.empty()) {

			for (const auto& it : setRelatedLoad) {
				auto* pRelatedInfo = pLCM->getInfo(it.first);
				if (!pRelatedInfo) continue;
				pRelatedInfo->substitueElement(nOld, nNew);
			}

			m_pProject->GetPlotControl()->LoadGroupChangedOn();
			m_pProject->GetPlotControl()->LoadGroupTreeChangedOn();
		}
		pLCM = nullptr;

		return true;
		};


	if (GET_PREP_ELEMENT()->getInfo(nNew)) {
		/*m_ErrCode.Format(_T("单元[%d]已存在"), nNew);
		g_pWarningList->AddToTemperary(4, m_ErrCode);
		return false;*/

		const int nTemp = OSIS_MAX_INT_INDEX * 2;
		swapIndex(nNew, nTemp);
		swapIndex(nOld, nNew);
		swapIndex(nTemp, nOld);
	}
	else {
		swapIndex(nOld, nNew);
	}



	//压入影子命令
	//PUSH_SHADOW_CMD_CUSTOM(THIS_IS_MOD, "ElementMod,%d,%d", nNew, nOld);

	m_pProject->GetPlotControl()->InputChangedOn();
	m_pProject->GetPlotControl()->StructTreeChangedOn();
	m_pProject->GetPlotControl()->ElementDataChangedOn();
	m_pProject->GetPlotControl()->ElemPropBeam3DsDataChangedOn();
	m_pProject->GetPlotControl()->NodeDataChangedOn();
	SEL_MGR_PTR()->ElementModCmd(nOld, nNew);
	return { true, errorCode };
}
