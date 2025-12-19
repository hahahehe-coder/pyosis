#include "stdafx.h"
#include "PyInterface.h"

template<typename ParaType, typename InfoType>
bool AppendSec(const int& nSec, const std::string& strName, ParaType& para, InfoType* pInfo, const bool& status)
{
	pInfo->setNO(nSec);
	pInfo->setName(strName);
	pInfo->setPara(para);
	pInfo->updateRegion();
	auto pSec = GET_PREP_SEC();
	if (!pInfo->generateSection()) 
	{
		return false;
	}
	yilCString strCommand;
	if (status) 
	{
		//strCommand.Format(_T("SectionDel,%d"), nSec);
	}
	else 
	{
		auto* pInfoOld = pSec->getInfo(nSec);
		pInfoOld->formatAPDLBase(strCommand);
		set<int> relatedElements = pSec->getInfo(nSec)->getRelatedElement();
		pInfo->setOffsetTypeY(pInfoOld->getOffsetTypeY());
		pInfo->setOffsetValueY(pInfoOld->getOffsetValueY());
		pInfo->setOffsetTypeZ(pInfoOld->getOffsetTypeZ());
		pInfo->setOffsetValueZ(pInfoOld->getOffsetValueZ());
		for (auto& no : relatedElements) { pInfo->appendRelatedElement(no); }	
			pSec->s_deleteByNO(nSec);
	}
	//PUSH_SHADOW_CMD(THIS_IS_DEF, strCommand);
	if (!pSec->append(pInfo))
	{
		delete pInfo; pInfo = nullptr;
		return false;
	}
	return true;
}

void AppendRebarInfo(const int nSec)
{
	auto pSec = GET_PREP_SEC();
	auto pRebar = pSec->getAuxParasInfo<PREP::SecRebarInfo>(nSec);
	if (!pRebar)
	{
		PREP::SecRebarInfo* pRebarNew = new PREP::SecRebarInfo();
		pRebarNew->setNO(nSec);
		pSec->appendAuxParas<PREP::SecRebarInfo>(pRebarNew);
	}
}

template<typename ParaType, typename InfoType>
bool ModifySec(const int& nSec, const std::string& strName, ParaType& para, InfoType* pInfo)
{
	//yilCString strCommand;
	//pInfo->formatAPDLBase(strCommand);
	//PUSH_SHADOW_CMD(THIS_IS_MOD, strCommand);
	pInfo->setNO(nSec);
	pInfo->setName(strName);
	pInfo->setPara(para);
	pInfo->updateRegion();
	if (!pInfo->generateSection())
	{
		return false;
	}
	return true;
}

template<typename InfoType>
bool AppendSteelSec(const int& nSec, const std::string& strName, InfoType* pInfo, const bool& status)
{
	auto pSec = GET_PREP_SEC();
	pInfo->setNO(nSec);
	pInfo->setName(strName);													
	pInfo->clearRibAll();                                                       
	pInfo->updateRegion();											
	//yilCString strCommand;														
	if (status) {
		//strCommand.Format(_T("SectionDel,%d"), _index);
	}
	else {
		auto* pInfoOld = pSec->getInfo(nSec);
		//pInfoOld->formatAPDLBase(strCommand);						            
		set<int> relatedElements = pSec->getInfo(nSec)->getRelatedElement();
		for (auto& no : relatedElements) 
		{ 
			pInfo->appendRelatedElement(no); 
		}	
		pInfo->setOffsetTypeY(pInfoOld->getOffsetTypeY());                      
		pInfo->setOffsetValueY(pInfoOld->getOffsetValueY());                    
		pInfo->setOffsetTypeZ(pInfoOld->getOffsetTypeZ());                      
		pInfo->setOffsetValueZ(pInfoOld->getOffsetValueZ());                    
		pSec->s_deleteByNO(nSec);
	}																			
	//PUSH_SHADOW_CMD(THIS_IS_DEF, strCommand);								
	if (!pSec->append(pInfo)) {
					
		delete pInfo;															
		pInfo = nullptr;						
		return false;															
	}		
	return true;
}

/// <summary>
/// 定义或修改截面
/// </summary>
/// <param name="nSec">截面编号</param>
/// <param name="strName">截面名</param>
/// <param name="section">截面类型</param>
/// <param name="kwargs">截面参数</param>
/// <returns>是否成功，失败原因</returns>
std::pair<bool, std::string> PyInterface::OSIS_Section(const int nSec, const std::string strName, const std::string eSectionType, const py::dict kwargs)
{
	std::string errorCode = "";
	//strName = utf8_to_wide(strName);
	PrepEnum::Section eSEC = PrepEnum::Section::Unassigned;
	if (!GetCommand()->StrToSection(eSEC, eSectionType, 3))
	{
		errorCode = "参数 截面类型 错误！";
		return { false, errorCode };
	}

	bool bState = true;  //状态，true = 定义，false = 修改
	bool bStatus = true; //针对bState = true的情况：true = 初次定义；false = 修改类型
	PREP::SectionInfo* pSecInfo = GET_PREP_SEC()->getInfo(nSec);
	if (pSecInfo) {

		bState = false;

		/* > > > > > > 切换截面类型 > > > > > > */
		if (pSecInfo->getType() != eSEC) { //修改一个已经定义的截面类型

			bState = true;
			bStatus = false;
		}
		/* < < < < < < 切换截面类型 < < < < < < */
	}

	switch (eSEC) 
	{
	case PrepEnum::Section::LShape:
	{
		auto pSec = GET_PREP_SEC();

		yilCString strName;

		bool Dir{ true };
		double H, B, Tf1, Tf2{ .0 };

		try {
			Dir = safe_cast<bool>(kwargs, "Dir");
			H = safe_cast<double>(kwargs, "H");
			B = safe_cast<double>(kwargs, "B");
			Tf1 = safe_cast<double>(kwargs, "Tf1");
			Tf2 = safe_cast<double>(kwargs, "Tf2");
		}
		catch (const std::runtime_error& e) {
			// 处理错误
			return { false, e.what() };
		}

		using Para = PREP::LShapePara;
		using Info = PREP::LShapeInfo;

		auto* pUnit = GetProject()->GetUnit();
		Para para = {
			Dir,
			pUnit->LocalLengthToSI(H),
			pUnit->LocalLengthToSI(B),
			pUnit->LocalLengthToSI(Tf1),
			pUnit->LocalLengthToSI(Tf2)
		};
		if (bState)
		{
			Info* pInfo = new Info();
			if (!AppendSec<Para, Info>(nSec, strName, para, pInfo, bStatus))
			{
				errorCode = "添加截面：" + std::to_string(nSec) + "失败！";
				return { false, errorCode };
			}
			AppendRebarInfo(nSec);
		}
		else
		{
			Info* pInfo = dynamic_cast<Info*>(GET_PREP_SEC()->getInfo(nSec));
			if (!pInfo)
			{
				errorCode = "未在数据库中找到编号为：" + std::to_string(nSec) + "的截面！";
				return { false, errorCode };
			}
			if (!ModifySec<Para, Info>(nSec, strName, para, pInfo))
			{
				errorCode = "修改截面：" + std::to_string(nSec) + "失败！";
				return { false, errorCode };
			}
		}
	}
		break;
	case PrepEnum::Section::IShape:
	{
		double H{ 0.0 }, Bt{ 0.0 }, Bb{ 0.0 }, Tt{ 0.0 }, Tb{ 0.0 }, Tw{ 0.0 };
		try {
			H = safe_cast<bool>(kwargs, "H");
			Bt = safe_cast<double>(kwargs, "Bt");
			Bb = safe_cast<double>(kwargs, "Bb");
			Tt = safe_cast<double>(kwargs, "Tt");
			Tb = safe_cast<double>(kwargs, "Tb");
			Tw = safe_cast<double>(kwargs, "Tw");
		}
		catch (const std::runtime_error& e) {
			// 处理错误
			return { false, e.what() };
		}
		using Info = PREP::IShapeInfo;
		using Para = PREP::IShapePara;

		auto* pUnit = GetProject()->GetUnit();
		Para para = {
			pUnit->LocalLengthToSI(H),
			pUnit->LocalLengthToSI(Bt),
			pUnit->LocalLengthToSI(Bb),
			pUnit->LocalLengthToSI(Tt),
			pUnit->LocalLengthToSI(Tb),
			pUnit->LocalLengthToSI(Tw)
		};
		if (bState)
		{
			Info* pInfo = new Info();
			if (!AppendSec<Para, Info>(nSec, strName, para, pInfo, bStatus))
			{
				errorCode = "添加截面：" + std::to_string(nSec) + "失败！";
				return { false, errorCode };
			}
			AppendRebarInfo(nSec);
		}
		else
		{
			Info* pInfo = dynamic_cast<Info*>(GET_PREP_SEC()->getInfo(nSec));
			if (!pInfo)
			{
				errorCode = "未在数据库中找到编号为：" + std::to_string(nSec) + "的截面！";
				return { false, errorCode };
			}
			if (!ModifySec<Para, Info>(nSec, strName, para, pInfo))
			{
				errorCode = "修改截面：" + std::to_string(nSec) + "失败！";
				return { false, errorCode };
			}
		}
	}
		break;
	case PrepEnum::Section::TShape:
	{
		bool Dir{ true };
		double H{ 0.0 }, B{ 0.0 }, Tf{ 0.0 }, Tw{ 0.0 };
		try {
			Dir = safe_cast<bool>(kwargs, "Dir");
			H = safe_cast<double>(kwargs, "H");
			B = safe_cast<double>(kwargs, "B");
			Tf = safe_cast<double>(kwargs, "Tf");
			Tw = safe_cast<double>(kwargs, "Tw");
		}
		catch (const std::runtime_error& e) {
			// 处理错误
			return { false, e.what() };
		}
		using Info = PREP::TShapeInfo;
		using Para = PREP::TShapePara;

		auto* pUnit = GetProject()->GetUnit();
		Para para = {
			Dir,
			pUnit->LocalLengthToSI(H),
			pUnit->LocalLengthToSI(B),
			pUnit->LocalLengthToSI(Tf),
			pUnit->LocalLengthToSI(Tw)
		};
		if (bState)
		{
			Info* pInfo = new Info();
			if (!AppendSec<Para, Info>(nSec, strName, para, pInfo, bStatus))
			{
				errorCode = "添加截面：" + std::to_string(nSec) + "失败！";
				return { false, errorCode };
			}
			AppendRebarInfo(nSec);
		}
		else
		{
			Info* pInfo = dynamic_cast<Info*>(GET_PREP_SEC()->getInfo(nSec));
			if (!pInfo)
			{
				errorCode = "未在数据库中找到编号为：" + std::to_string(nSec) + "的截面！";
				return { false, errorCode };
			}
			if (!ModifySec<Para, Info>(nSec, strName, para, pInfo))
			{
				errorCode = "修改截面：" + std::to_string(nSec) + "失败！";
				return { false, errorCode };
			}
		}
	}
		break;
	case PrepEnum::Section::Circle:	
	{
		SecEnum::FillingType type;
		double D{ 0.0 }, Tw{ 0.0 };
		std::string strType;
		try {
			D = safe_cast<double>(kwargs, "D");
			Tw = safe_cast<double>(kwargs, "Tw");
			strType = safe_cast<std::string>(kwargs, "CircleType");
			if (!GetCommand()->StrToFillingType(type, strType, 4))
			{

				errorCode = "参数 CircleType 错误！";
				return { false, errorCode };
			}
		}
		catch (const std::runtime_error& e) {
			// 处理错误
			return { false, e.what() };
		}

		using Info = PREP::CircleInfo;
		using Para = PREP::CirclePara;

		auto* pUnit = GetProject()->GetUnit();
		Para para = {
			type,
			pUnit->LocalLengthToSI(D),
			pUnit->LocalLengthToSI(Tw)
		};
		if (bState)
		{
			Info* pInfo = new Info();
			if (!AppendSec<Para, Info>(nSec, strName, para, pInfo, bStatus))
			{
				errorCode = "添加截面：" + std::to_string(nSec) + "失败！";
				return { false, errorCode };
			}
			AppendRebarInfo(nSec);
		}
		else
		{
			Info* pInfo = dynamic_cast<Info*>(GET_PREP_SEC()->getInfo(nSec));
			if (!pInfo)
			{
				errorCode = "未在数据库中找到编号为：" + std::to_string(nSec) + "的截面！";
				return { false, errorCode };
			}
			if (!ModifySec<Para, Info>(nSec, strName, para, pInfo))
			{
				errorCode = "修改截面：" + std::to_string(nSec) + "失败！";
				return { false, errorCode };
			}
		}
	}
		break;
	case PrepEnum::Section::SmallBox:
	{
		SecEnum::GirderPosition girderPos{ SecEnum::GirderPosition::Unassigned }; std::string strGirderPos;
		double H{ 0.0 }, Bs{ 0.0 }, Bm{ 0.0 }, Bc{ 0.0 }, Bb{ 0.0 }, Tt{ 0.0 }, Tb{ 0.0 }, Tw{ 0.0 }, i{ 0.0 };
		double Tc{ 0.0 }, Tc1{ 0.0 }, x{ 0.0 }, xi1{ 0.0 }, Tt1{ 0.0 }, xi2{ 0.0 }, yi2{ 0.0 };
		bool hasSlope{ true };
		double i1{ 0.0 }, i2{ 0.0 }, R{ 0.0 };
		try {
			strGirderPos = safe_cast<std::string>(kwargs, "GirderPos");
			if (!GetCommand()->StrToGirderPosition(girderPos, strGirderPos, 4))
			{

				errorCode = "参数 GirderPos 错误！";
				return { false, errorCode };
			}
			H = safe_cast<double>(kwargs, "H");
			Bs = safe_cast<double>(kwargs, "Bs");
			Bm = safe_cast<double>(kwargs, "Bm");
			Bc = safe_cast<double>(kwargs, "Bc");
			Bb = safe_cast<double>(kwargs, "Bb");
			Tt = safe_cast<double>(kwargs, "Tt");
			Tb = safe_cast<double>(kwargs, "Tb");
			Tw = safe_cast<double>(kwargs, "Tw");
			i = safe_cast<double>(kwargs, "i");
			Tc = safe_cast<double>(kwargs, "Tc");
			Tc1 = safe_cast<double>(kwargs, "Tc1");
			x = safe_cast<double>(kwargs, "x");
			xi1 = safe_cast<double>(kwargs, "xi1");
			Tt1 = safe_cast<double>(kwargs, "Tt1");
			xi2 = safe_cast<double>(kwargs, "xi2");
			yi2 = safe_cast<double>(kwargs, "yi2");
			hasSlope = safe_cast<bool>(kwargs, "Slope");
			i1 = safe_cast<double>(kwargs, "i1");
			i2 = safe_cast<double>(kwargs, "i2");
			R = safe_cast<double>(kwargs, "R");
		}
		catch (const std::runtime_error& e) {
			// 处理错误
			return { false, e.what() };
		}

		using Info = PREP::SmallBoxInfo;
		using Para = PREP::SmallBoxPara;

		auto* pUnit = GetProject()->GetUnit();
		Para para = {
			girderPos,
			pUnit->LocalLengthToSI(H),
			pUnit->LocalLengthToSI(Bs),
			pUnit->LocalLengthToSI(Bm),
			pUnit->LocalLengthToSI(Bc),
			pUnit->LocalLengthToSI(Bb),
			pUnit->LocalLengthToSI(Tt),
			pUnit->LocalLengthToSI(Tb),
			pUnit->LocalLengthToSI(Tw),
			i,
			pUnit->LocalLengthToSI(Tc),
			pUnit->LocalLengthToSI(Tc1),
			pUnit->LocalLengthToSI(x),
			pUnit->LocalLengthToSI(xi1),
			pUnit->LocalLengthToSI(Tt1),
			pUnit->LocalLengthToSI(xi2),
			pUnit->LocalLengthToSI(yi2),
			hasSlope,
			i1,
			i2,
			pUnit->LocalLengthToSI(R)
		};
		if (bState)
		{
			Info* pInfo = new Info();
			if (!AppendSec<Para, Info>(nSec, strName, para, pInfo, bStatus))
			{
				errorCode = "添加截面：" + std::to_string(nSec) + "失败！";
				return { false, errorCode };
			}
			AppendRebarInfo(nSec);
		}
		else
		{
			Info* pInfo = dynamic_cast<Info*>(GET_PREP_SEC()->getInfo(nSec));
			if (!pInfo)
			{
				errorCode = "未在数据库中找到编号为：" + std::to_string(nSec) + "的截面！";
				return { false, errorCode };
			}
			if (!ModifySec<Para, Info>(nSec, strName, para, pInfo))
			{
				errorCode = "修改截面：" + std::to_string(nSec) + "失败！";
				return { false, errorCode };
			}
		}
	}
		break;
	case PrepEnum::Section::Rectangle:
	{
		//SET_SEC(AppendRectangle, ResetRectangle);
		// 定义所需参数
		SecEnum::EdgeTransition transitionType = SecEnum::EdgeTransition::Unassigned;
		SecEnum::FillingType fillingType = SecEnum::FillingType::Unassigned;
		double B{ 0.0 }, H{ 0.0 }, xo1{ 0.0 }, yo1{ 0.0 }, R{ 0.0 }, t1{ 0.0 }, t2{ 0.0 }, xi1{ 0.0 }, yi1{ 0.0 };
		bool HasDiaphragm{ true };
		double tw{ 0.0 }, xi2{ 0.0 }, yi2{ 0.0 };
		bool HasGroove{ true };
		double b1{ 0.0 }, b2{ 0.0 }, h{ 0.0 };
		std::string strTransition = kwargs["TransitionType"].cast<std::string>();
		std::string strFillingType = kwargs["SecType"].cast<std::string>();

		try {
			// 参数解析
			strTransition = safe_cast<std::string>(kwargs, "TransitionType");
			strFillingType = safe_cast<std::string>(kwargs, "SecType");
			if (!GetCommand()->StrToEdgeTransition(transitionType, strTransition, 4))
			{
				errorCode = "参数 TransitionType 错误！";
				return { false, errorCode };
			}
			if (!GetCommand()->StrToFillingType(fillingType, strFillingType, 5))
			{
				errorCode = "参数 SecType 错误！";
				return { false, errorCode };
			}
			B = safe_cast<double>(kwargs, "B");
			H = safe_cast<double>(kwargs, "H");
			xo1 = safe_cast<double>(kwargs, "xo1");
			yo1 = safe_cast<double>(kwargs, "yo1");
			R = safe_cast<double>(kwargs, "R");
			t1 = safe_cast<double>(kwargs, "t1");
			t2 = safe_cast<double>(kwargs, "t2");
			xi1 = safe_cast<double>(kwargs, "xi1");
			yi1 = safe_cast<double>(kwargs, "yi1");
			HasDiaphragm = safe_cast<bool>(kwargs, "HasDiaphragm");
			tw = safe_cast<double>(kwargs, "tw");
			xi2 = safe_cast<double>(kwargs, "xi2");
			yi2 = safe_cast<double>(kwargs, "yi2");
			HasGroove = safe_cast<bool>(kwargs, "HasGroove");
			b1 = safe_cast<double>(kwargs, "b1");
			b2 = safe_cast<double>(kwargs, "b2");
			h = safe_cast<double>(kwargs, "h");
		}
		catch (const std::runtime_error& e) {
			// 处理错误
			return { false, e.what() };
		}

		using Para = PREP::RectPara;
		using Info = PREP::RectInfo;
		
		auto* pUnit = GetProject()->GetUnit();
		Para para = {
			transitionType,
			pUnit->LocalLengthToSI(B),
			pUnit->LocalLengthToSI(H),
			pUnit->LocalLengthToSI(R),
			pUnit->LocalLengthToSI(xo1),
			pUnit->LocalLengthToSI(yo1),
			fillingType,
			pUnit->LocalLengthToSI(t1),
			pUnit->LocalLengthToSI(t2),
			pUnit->LocalLengthToSI(xi1),
			pUnit->LocalLengthToSI(yi1),
			HasDiaphragm,
			pUnit->LocalLengthToSI(tw),
			pUnit->LocalLengthToSI(xi2),
			pUnit->LocalLengthToSI(yi2),
			HasGroove,
			pUnit->LocalLengthToSI(b1),
			pUnit->LocalLengthToSI(b2),
			pUnit->LocalLengthToSI(h)
		};

		if (bState)
		{
			Info* pInfo = new Info();
			if (!AppendSec<Para, Info>(nSec, strName, para, pInfo, bStatus))
			{
				errorCode = "添加截面：" + std::to_string(nSec) + "失败！";
				return { false, errorCode };
			}
			AppendRebarInfo(nSec);
		}
		else
		{
			Info* pInfo = dynamic_cast<Info*>(GET_PREP_SEC()->getInfo(nSec));
			if (!pInfo)
			{
				errorCode = "未在数据库中找到编号为：" + std::to_string(nSec) + "的截面！";
				return { false, errorCode };
			}
			if (!ModifySec<Para, Info>(nSec, strName, para, pInfo))
			{
				errorCode = "修改截面：" + std::to_string(nSec) + "失败！";
				return { false, errorCode };
			}
		}
		break;
	}
		break;
	case PrepEnum::Section::RoundedEndShape:
	{
		SecEnum::FillingType fillingType{ SecEnum::FillingType::Unassigned }; std::string strFillingType;
		double B{ 0.0 }, H{ 0.0 }, R{ 0.0 };
		bool HasDiaphragm{ true };
		double b{ 0.0 }, t{ 0.0 }, xi1{ 0.0 }, yi1{ 0.0 }, tw{ 0.0 }, xi2{ 0.0 }, yi2{ 0.0 };
		try {

			strFillingType = safe_cast<std::string>(kwargs, "FillingType");
			if (!GetCommand()->StrToFillingType(fillingType, strFillingType, 4))
			{
				errorCode = "参数 FillingType 错误！";
				return { false, errorCode };
			}
			B = safe_cast<double>(kwargs, "B");
			H = safe_cast<double>(kwargs, "H");
			R = safe_cast<double>(kwargs, "R");
			HasDiaphragm = safe_cast<bool>(kwargs, "HasDiaphragm");
			b = safe_cast<double>(kwargs, "b");
			t = safe_cast<double>(kwargs, "t");
			xi1 = safe_cast<double>(kwargs, "xi1");
			yi1 = safe_cast<double>(kwargs, "yi1");
			tw = safe_cast<double>(kwargs, "tw");
			xi2 = safe_cast<double>(kwargs, "xi2");
			yi2 = safe_cast<double>(kwargs, "yi2");
		}
		catch (const std::runtime_error& e) {
			// 处理错误
			return { false, e.what() };
		}

		using Info = PREP::RoundedEndInfo;
		using Para = PREP::RoundedEndPara;

		auto* pUnit = GetProject()->GetUnit();
		Para para = {
			fillingType,
			pUnit->LocalLengthToSI(B),
			pUnit->LocalLengthToSI(H),
			pUnit->LocalLengthToSI(R),
			pUnit->LocalLengthToSI(b),
			pUnit->LocalLengthToSI(t),
			pUnit->LocalLengthToSI(xi1),
			pUnit->LocalLengthToSI(yi1),
			HasDiaphragm,
			pUnit->LocalLengthToSI(tw),
			pUnit->LocalLengthToSI(xi2),
			pUnit->LocalLengthToSI(yi2)
		};
		if (bState)
		{
			Info* pInfo = new Info();
			if (!AppendSec<Para, Info>(nSec, strName, para, pInfo, bStatus))
			{
				errorCode = "添加截面：" + std::to_string(nSec) + "失败！";
				return { false, errorCode };
			}
			AppendRebarInfo(nSec);
		}
		else
		{
			Info* pInfo = dynamic_cast<Info*>(GET_PREP_SEC()->getInfo(nSec));
			if (!pInfo)
			{
				errorCode = "未在数据库中找到编号为：" + std::to_string(nSec) + "的截面！";
				return { false, errorCode };
			}
			if (!ModifySec<Para, Info>(nSec, strName, para, pInfo))
			{
				errorCode = "修改截面：" + std::to_string(nSec) + "失败！";
				return { false, errorCode };
			}
		}
	}
		break;
	case PrepEnum::Section::ConventionalBox:
	{
		double H{ 0.0 }, BtL{ 0.0 }, BtR{ 0.0 }, BbL{ 0.0 }, BbR{ 0.0 }, Bs{ 0.0 }, Tt{ 0.0 }, Tb{ 0.0 }, Tw1{ 0.0 }, Tw2{ 0.0 };
		int CellNum{ 1 };
		double Bi1{ 0.0 }, Bi2{ 0.0 }, Bi3{ 0.0 }, Bi4{ 0.0 };
		double xi1{ 0.0 }, Tt1{ 0.0 }, xi2{ 0.0 }, Tt2{ 0.0 }, xi3{ 0.0 }, yi3{ 0.0 }, xi4{ 0.0 }, Tt4{ 0.0 }, xi5{ 0.0 }, yi5{ 0.0 }, xi6{ 0.0 }, Tt6{ 0.0 }, xi7{ 0.0 }, yi7{ 0.0 };
		double BcL{ 0.0 }, TcL{ 0.0 }, Bc1L{ 0.0 }, Tc1L{ 0.0 }, Tc2L{ 0.0 };
		bool Symmetry{ true };
		double BcR{ 0.0 }, TcR{ 0.0 }, Bc1R{ 0.0 }, Tc1R{ 0.0 }, Tc2R{ 0.0 };
		SecEnum::CrossSlopeType SlopeType{ SecEnum::CrossSlopeType::Unassigned }; std::string strSlopeType;
		double i{ 0.0 }, i1{ 0.0 }, i2{ 0.0 }, i3{ 0.0 }, i4{ 0.0 }, R1{ 0.0 }, R2{ 0.0 };

		try {
			H = safe_cast<double>(kwargs, "H");
			BtL = safe_cast<double>(kwargs, "BtL");
			BbL = safe_cast<double>(kwargs, "BbL");
			BbR = safe_cast<double>(kwargs, "BbR");
			Bs = safe_cast<double>(kwargs, "Bs");
			Tt = safe_cast<double>(kwargs, "Tt");
			Tb = safe_cast<double>(kwargs, "Tb");
			Tw1 = safe_cast<double>(kwargs, "Tw1");
			Tw2 = safe_cast<double>(kwargs, "Tw2");

			CellNum = safe_cast<int>(kwargs, "CellNum");

			Bi1 = safe_cast<double>(kwargs, "Bi1");
			Bi2 = safe_cast<double>(kwargs, "Bi2");
			Bi3 = safe_cast<double>(kwargs, "Bi3");
			Bi4 = safe_cast<double>(kwargs, "Bi4");

			xi1 = safe_cast<double>(kwargs, "xi1");
			Tt1 = safe_cast<double>(kwargs, "Tt1");
			xi2 = safe_cast<double>(kwargs, "xi2");
			Tt2 = safe_cast<double>(kwargs, "Tt2");
			xi3 = safe_cast<double>(kwargs, "xi3");
			yi3 = safe_cast<double>(kwargs, "yi3");
			xi4 = safe_cast<double>(kwargs, "xi4");
			Tt4 = safe_cast<double>(kwargs, "Tt4");
			xi5 = safe_cast<double>(kwargs, "xi5");
			yi5 = safe_cast<double>(kwargs, "yi5");
			xi6 = safe_cast<double>(kwargs, "xi6");
			Tt6 = safe_cast<double>(kwargs, "Tt6");
			xi7 = safe_cast<double>(kwargs, "xi7");
			yi7 = safe_cast<double>(kwargs, "yi7");

			BcL = safe_cast<double>(kwargs, "BcL");
			TcL = safe_cast<double>(kwargs, "TcL");
			Bc1L = safe_cast<double>(kwargs, "Bc1L");
			Tc1L = safe_cast<double>(kwargs, "Tc1L");
			Tc2L = safe_cast<double>(kwargs, "Tc2L");

			Symmetry = safe_cast<bool>(kwargs, "Symmetry");

			BcR = safe_cast<double>(kwargs, "BcR");
			TcR = safe_cast<double>(kwargs, "TcR");
			Bc1R = safe_cast<double>(kwargs, "Bc1R");
			Tc1R = safe_cast<double>(kwargs, "Tc1R");
			Tc2R = safe_cast<double>(kwargs, "Tc2R");

			strSlopeType = safe_cast<std::string>(kwargs, "SlopeType");
			if (!GetCommand()->StrToCrossSlopeType(SlopeType, strSlopeType, 44))
			{
				errorCode = "参数 SlopeType 错误！";
				return { false, errorCode };
			}

			i = safe_cast<double>(kwargs, "i");
			i1 = safe_cast<double>(kwargs, "i1");
			i2 = safe_cast<double>(kwargs, "i2");
			i3 = safe_cast<double>(kwargs, "i3");
			i4 = safe_cast<double>(kwargs, "i4");
			R1 = safe_cast<double>(kwargs, "R1");
			R2 = safe_cast<double>(kwargs, "R2");
		}
		catch (const std::runtime_error& e) {
			// 处理错误
			return { false, e.what() };
		}

		using Info = PREP::ConventionalBoxInfo;
		using Para = PREP::ConventionalBoxPara;

		auto* pUnit = GetProject()->GetUnit();
		Para para = {
			pUnit->LocalLengthToSI(H),
			pUnit->LocalLengthToSI(BtL),
			pUnit->LocalLengthToSI(BtR),
			pUnit->LocalLengthToSI(BbL),
			pUnit->LocalLengthToSI(BbR),
			pUnit->LocalLengthToSI(Bs),
			pUnit->LocalLengthToSI(Tt),
			pUnit->LocalLengthToSI(Tb),
			pUnit->LocalLengthToSI(Tw1),
			pUnit->LocalLengthToSI(Tw2),

			CellNum,
			pUnit->LocalLengthToSI(Bi1),
			pUnit->LocalLengthToSI(Bi2),
			pUnit->LocalLengthToSI(Bi3),
			pUnit->LocalLengthToSI(Bi4),
			pUnit->LocalLengthToSI(xi1),
			pUnit->LocalLengthToSI(Tt1),
			pUnit->LocalLengthToSI(xi2),
			pUnit->LocalLengthToSI(Tt2),
			pUnit->LocalLengthToSI(xi3),
			pUnit->LocalLengthToSI(yi3),
			pUnit->LocalLengthToSI(xi4),
			pUnit->LocalLengthToSI(Tt4),
			pUnit->LocalLengthToSI(xi5),
			pUnit->LocalLengthToSI(yi5),
			pUnit->LocalLengthToSI(xi6),
			pUnit->LocalLengthToSI(Tt6),
			pUnit->LocalLengthToSI(xi7),
			pUnit->LocalLengthToSI(yi7),

			Symmetry,
			pUnit->LocalLengthToSI(BcL),
			pUnit->LocalLengthToSI(TcL),
			pUnit->LocalLengthToSI(Bc1L),
			pUnit->LocalLengthToSI(Tc1L),
			pUnit->LocalLengthToSI(Tc2L),
			pUnit->LocalLengthToSI(BcR),
			pUnit->LocalLengthToSI(TcR),
			pUnit->LocalLengthToSI(Bc1R),
			pUnit->LocalLengthToSI(Tc1R),
			pUnit->LocalLengthToSI(Tc2R),

			SlopeType,
			i,
			i1,
			i2,
			i3,
			i4,
			pUnit->LocalLengthToSI(R1),
			pUnit->LocalLengthToSI(R2)
		};
		if (bState)
		{
			Info* pInfo = new Info();
			if (!AppendSec<Para, Info>(nSec, strName, para, pInfo, bStatus))
			{
				errorCode = "添加截面：" + std::to_string(nSec) + "失败！";
				return { false, errorCode };
			}
			AppendRebarInfo(nSec);
		}
		else
		{
			Info* pInfo = dynamic_cast<Info*>(GET_PREP_SEC()->getInfo(nSec));
			if (!pInfo)
			{
				errorCode = "未在数据库中找到编号为：" + std::to_string(nSec) + "的截面！";
				return { false, errorCode };
			}
			if (!ModifySec<Para, Info>(nSec, strName, para, pInfo))
			{
				errorCode = "修改截面：" + std::to_string(nSec) + "失败！";
				return { false, errorCode };
			}
		}
	}
		break;
	case PrepEnum::Section::StreamedBox:
	{
		double H{ 0.0 }, BtL{ 0.0 }, BtR{ 0.0 }, BbL{ 0.0 }, BbR{ 0.0 }, Bs{ 0.0 }, Tt{ 0.0 }, Tb1{ 0.0 }, Tb2{ 0.0 }, Tw{ 0.0 }, Ttj{ 0.0 }, Tbj{ 0.0 }, Twj{ 0.0 };
		int CellNum{ 1 };
		double Bi1{ 0.0 }, Bi2{ 0.0 }, Bi3{ 0.0 }, Bi4{ 0.0 };
		double xi1{ 0.0 }, Tt1{ 0.0 }, xi2{ 0.0 }, Tt2{ 0.0 }, xi3{ 0.0 }, yi3{ 0.0 }, xi4{ 0.0 }, Tt4{ 0.0 }, xi5{ 0.0 }, yi5{ 0.0 }, xi6{ 0.0 }, Tt6{ 0.0 }, xi7{ 0.0 }, yi7{ 0.0 };
		double BcL{ 0.0 }, TcL{ 0.0 }, Bc1L{ 0.0 }, Tc1L{ 0.0 }, Tc2L{ 0.0 };
		bool Symmetry{ true };
		double BcR{ 0.0 }, TcR{ 0.0 }, Bc1R{ 0.0 }, Tc1R{ 0.0 }, Tc2R{ 0.0 };
		SecEnum::CrossSlopeType SlopeType{ SecEnum::CrossSlopeType::Unassigned }; std::string strSlopeType;
		double i{ 0.0 }, i1{ 0.0 }, i2{ 0.0 }, i3{ 0.0 }, i4{ 0.0 }, R1{ 0.0 }, R2{ 0.0 };

		try {
			H = safe_cast<double>(kwargs, "H");
			BtL = safe_cast<double>(kwargs, "BtL");
			BbL = safe_cast<double>(kwargs, "BbL");
			BbR = safe_cast<double>(kwargs, "BbR");
			Bs = safe_cast<double>(kwargs, "Bs");
			Tt = safe_cast<double>(kwargs, "Tt");
			Tb1 = safe_cast<double>(kwargs, "Tb1");
			Tb2 = safe_cast<double>(kwargs, "Tb2");
			Tw = safe_cast<double>(kwargs, "Tw");
			Ttj = safe_cast<double>(kwargs, "Ttj");
			Tbj = safe_cast<double>(kwargs, "Tbj");
			Twj = safe_cast<double>(kwargs, "Twj");

			CellNum = safe_cast<int>(kwargs, "CellNum");

			Bi1 = safe_cast<double>(kwargs, "Bi1");
			Bi2 = safe_cast<double>(kwargs, "Bi2");
			Bi3 = safe_cast<double>(kwargs, "Bi3");
			Bi4 = safe_cast<double>(kwargs, "Bi4");

			xi1 = safe_cast<double>(kwargs, "xi1");
			Tt1 = safe_cast<double>(kwargs, "Tt1");
			xi2 = safe_cast<double>(kwargs, "xi2");
			Tt2 = safe_cast<double>(kwargs, "Tt2");
			xi3 = safe_cast<double>(kwargs, "xi3");
			yi3 = safe_cast<double>(kwargs, "yi3");
			xi4 = safe_cast<double>(kwargs, "xi4");
			Tt4 = safe_cast<double>(kwargs, "Tt4");
			xi5 = safe_cast<double>(kwargs, "xi5");
			yi5 = safe_cast<double>(kwargs, "yi5");
			xi6 = safe_cast<double>(kwargs, "xi6");
			Tt6 = safe_cast<double>(kwargs, "Tt6");
			xi7 = safe_cast<double>(kwargs, "xi7");
			yi7 = safe_cast<double>(kwargs, "yi7");

			BcL = safe_cast<double>(kwargs, "BcL");
			TcL = safe_cast<double>(kwargs, "TcL");
			Bc1L = safe_cast<double>(kwargs, "Bc1L");
			Tc1L = safe_cast<double>(kwargs, "Tc1L");
			Tc2L = safe_cast<double>(kwargs, "Tc2L");

			Symmetry = safe_cast<bool>(kwargs, "Symmetry");

			BcR = safe_cast<double>(kwargs, "BcR");
			TcR = safe_cast<double>(kwargs, "TcR");
			Bc1R = safe_cast<double>(kwargs, "Bc1R");
			Tc1R = safe_cast<double>(kwargs, "Tc1R");
			Tc2R = safe_cast<double>(kwargs, "Tc2R");

			strSlopeType = safe_cast<std::string>(kwargs, "SlopeType");
			if (!GetCommand()->StrToCrossSlopeType(SlopeType, strSlopeType, 47))
			{
				errorCode = "参数 SlopeType 错误！";
				return { false, errorCode };
			}

			i = safe_cast<double>(kwargs, "i");
			i1 = safe_cast<double>(kwargs, "i1");
			i2 = safe_cast<double>(kwargs, "i2");
			i3 = safe_cast<double>(kwargs, "i3");
			i4 = safe_cast<double>(kwargs, "i4");
			R1 = safe_cast<double>(kwargs, "R1");
			R2 = safe_cast<double>(kwargs, "R2");
		}
		catch (const std::runtime_error& e) {
			// 处理错误
			return { false, e.what() };
		}

		using Info = PREP::StreamedBoxInfo;
		using Para = PREP::StreamedBoxPara;

		auto* pUnit = GetProject()->GetUnit();
		Para para = {
			pUnit->LocalLengthToSI(H),
			pUnit->LocalLengthToSI(BtL),
			pUnit->LocalLengthToSI(BtR),
			pUnit->LocalLengthToSI(BbL),
			pUnit->LocalLengthToSI(BbR),
			pUnit->LocalLengthToSI(Bs),
			pUnit->LocalLengthToSI(Tt),
			pUnit->LocalLengthToSI(Tb1),
			pUnit->LocalLengthToSI(Tb2),
			pUnit->LocalLengthToSI(Tw),
			pUnit->LocalLengthToSI(Ttj),
			pUnit->LocalLengthToSI(Tbj),
			pUnit->LocalLengthToSI(Twj),

			CellNum,
			pUnit->LocalLengthToSI(Bi1),
			pUnit->LocalLengthToSI(Bi2),
			pUnit->LocalLengthToSI(Bi3),
			pUnit->LocalLengthToSI(Bi4),
			pUnit->LocalLengthToSI(xi1),
			pUnit->LocalLengthToSI(Tt1),
			pUnit->LocalLengthToSI(xi2),
			pUnit->LocalLengthToSI(Tt2),
			pUnit->LocalLengthToSI(xi3),
			pUnit->LocalLengthToSI(yi3),
			pUnit->LocalLengthToSI(xi4),
			pUnit->LocalLengthToSI(Tt4),
			pUnit->LocalLengthToSI(xi5),
			pUnit->LocalLengthToSI(yi5),
			pUnit->LocalLengthToSI(xi6),
			pUnit->LocalLengthToSI(Tt6),
			pUnit->LocalLengthToSI(xi7),
			pUnit->LocalLengthToSI(yi7),

			Symmetry,
			pUnit->LocalLengthToSI(BcL),
			pUnit->LocalLengthToSI(TcL),
			pUnit->LocalLengthToSI(Bc1L),
			pUnit->LocalLengthToSI(Tc1L),
			pUnit->LocalLengthToSI(Tc2L),
			pUnit->LocalLengthToSI(BcR),
			pUnit->LocalLengthToSI(TcR),
			pUnit->LocalLengthToSI(Bc1R),
			pUnit->LocalLengthToSI(Tc1R),
			pUnit->LocalLengthToSI(Tc2R),

			SlopeType,
			i,
			i1,
			i2,
			i3,
			i4,
			pUnit->LocalLengthToSI(R1),
			pUnit->LocalLengthToSI(R2)
		};
		if (bState)
		{
			Info* pInfo = new Info();
			if (!AppendSec<Para, Info>(nSec, strName, para, pInfo, bStatus))
			{
				errorCode = "添加截面：" + std::to_string(nSec) + "失败！";
				return { false, errorCode };
			}
			AppendRebarInfo(nSec);
		}
		else
		{
			Info* pInfo = dynamic_cast<Info*>(GET_PREP_SEC()->getInfo(nSec));
			if (!pInfo)
			{
				errorCode = "未在数据库中找到编号为：" + std::to_string(nSec) + "的截面！";
				return { false, errorCode };
			}
			if (!ModifySec<Para, Info>(nSec, strName, para, pInfo))
			{
				errorCode = "修改截面：" + std::to_string(nSec) + "失败！";
				return { false, errorCode };
			}
		}
	}
		break;
	case PrepEnum::Section::DoubleSideBox:
	{
		double H{ 0.0 }, Bt{ 0.0 }, bt{ 0.0 }, Bs{ 0.0 }, Bb{ 0.0 }, tt{ 0.0 }, Tb1{ 0.0 }, Tb2{ 0.0 }, Tw{ 0.0 };
		double b{ 0.0 }, n{ 0.0 };
		double Bi{ 0.0 }, xi1{ 0.0 }, Tt1{ 0.0 }, xi2{ 0.0 }, Tt2{ 0.0 }, xi3{ 0.0 }, yi3{ 0.0 }, xo4{ 0.0 }, Tt4{ 0.0 }, b1{ 0.0 };
		SecEnum::CrossSlopeType SlopeType{ SecEnum::CrossSlopeType::Unassigned }; std::string strSlopeType;
		double i{ 0.0 }, i1{ 0.0 }, i2{ 0.0 };

		try {
			H = safe_cast<double>(kwargs, "H");
			Bt = safe_cast<double>(kwargs, "Bt");
			Bs = safe_cast<double>(kwargs, "Bs");
			Bb = safe_cast<double>(kwargs, "Bb");
			tt = safe_cast<double>(kwargs, "tt");
			Tb1 = safe_cast<double>(kwargs, "Tb1");
			Tb2 = safe_cast<double>(kwargs, "Tb2");
			Tw = safe_cast<double>(kwargs, "Tw");
			b = safe_cast<double>(kwargs, "b");
			n = safe_cast<double>(kwargs, "n");
			Bi = safe_cast<double>(kwargs, "Bi");
			xi1 = safe_cast<double>(kwargs, "xi1");
			Tt1 = safe_cast<double>(kwargs, "Tt1");
			xi2 = safe_cast<double>(kwargs, "xi2");
			Tt2 = safe_cast<double>(kwargs, "Tt2");
			xi3 = safe_cast<double>(kwargs, "xi3");
			yi3 = safe_cast<double>(kwargs, "yi3");
			xo4 = safe_cast<double>(kwargs, "xo4");
			Tt4 = safe_cast<double>(kwargs, "Tt4");
			b1 = safe_cast<double>(kwargs, "b1");
			strSlopeType = safe_cast<std::string>(kwargs, "SlopeType");
			if (!GetCommand()->StrToCrossSlopeType(SlopeType, strSlopeType, 25))
			{
				errorCode = "参数 SlopeType 错误！";
				return { false, errorCode };
			}
			i = safe_cast<double>(kwargs, "i");
			i1 = safe_cast<double>(kwargs, "i1");
			i2 = safe_cast<double>(kwargs, "i2");
		}
		catch (const std::runtime_error& e) {
			// 处理错误
			return { false, e.what() };
		}

		using Info = PREP::DoubleSideBoxInfo;
		using Para = PREP::DoubleSideBoxPara;

		auto* pUnit = GetProject()->GetUnit();
		Para para = {
			pUnit->LocalLengthToSI(H),
			pUnit->LocalLengthToSI(Bt),
			pUnit->LocalLengthToSI(bt),
			pUnit->LocalLengthToSI(Bs),
			pUnit->LocalLengthToSI(Bb),
			pUnit->LocalLengthToSI(tt),
			pUnit->LocalLengthToSI(Tb1),
			pUnit->LocalLengthToSI(Tb2),
			pUnit->LocalLengthToSI(Tw),

			pUnit->LocalLengthToSI(b),
			pUnit->LocalLengthToSI(n),

			pUnit->LocalLengthToSI(Bi),
			pUnit->LocalLengthToSI(xi1),
			pUnit->LocalLengthToSI(Tt1),
			pUnit->LocalLengthToSI(xi2),
			pUnit->LocalLengthToSI(Tt2),
			pUnit->LocalLengthToSI(xi3),
			pUnit->LocalLengthToSI(yi3),
			pUnit->LocalLengthToSI(xo4),
			pUnit->LocalLengthToSI(Tt4),
			pUnit->LocalLengthToSI(b1),

			SlopeType,
			i,
			i1,
			i2
		};
		if (bState)
		{
			Info* pInfo = new Info();
			if (!AppendSec<Para, Info>(nSec, strName, para, pInfo, bStatus))
			{
				errorCode = "添加截面：" + std::to_string(nSec) + "失败！";
				return { false, errorCode };
			}
			AppendRebarInfo(nSec);
		}
		else
		{
			Info* pInfo = dynamic_cast<Info*>(GET_PREP_SEC()->getInfo(nSec));
			if (!pInfo)
			{
				errorCode = "未在数据库中找到编号为：" + std::to_string(nSec) + "的截面！";
				return { false, errorCode };
			}
			if (!ModifySec<Para, Info>(nSec, strName, para, pInfo))
			{
				errorCode = "修改截面：" + std::to_string(nSec) + "失败！";
				return { false, errorCode };
			}
		}
	}
		break;
	case PrepEnum::Section::RibbedSlab:
	{
		double H{ 0.0 }, Bt{ 0.0 }, bt{ 0.0 }, Tt{ 0.0 }, b{ 0.0 }, h{ 0.0 }, b1{ 0.0 }, b2{ 0.0 }, x{ 0.0 }, y{ 0.0 };
		SecEnum::CrossSlopeType SlopeType{ SecEnum::CrossSlopeType::Unassigned }; std::string strSlopeType;
		double i{ 0.0 }, i1{ 0.0 }, i2{ 0.0 };
		try {
			H = safe_cast<double>(kwargs, "H");
			Bt = safe_cast<double>(kwargs, "Bt");
			Tt = safe_cast<double>(kwargs, "Tt");
			b = safe_cast<double>(kwargs, "b");
			h = safe_cast<double>(kwargs, "h");
			b1 = safe_cast<double>(kwargs, "b1");
			b2 = safe_cast<double>(kwargs, "b2");
			x = safe_cast<double>(kwargs, "x");
			y = safe_cast<double>(kwargs, "y");
			strSlopeType = safe_cast<std::string>(kwargs, "SlopeType");
			if (!GetCommand()->StrToCrossSlopeType(SlopeType, strSlopeType, 14))
			{
				errorCode = "参数 SlopeType 错误！";
				return { false, errorCode };
			}
			i = safe_cast<double>(kwargs, "i");
			i1 = safe_cast<double>(kwargs, "i1");
			i2 = safe_cast<double>(kwargs, "i2");
		}
		catch (const std::runtime_error& e) {
			// 处理错误
			return { false, e.what() };
		}

		using Info = PREP::RibbedSlabInfo;
		using Para = PREP::RibbedSlabPara;

		auto* pUnit = GetProject()->GetUnit();
		Para para = {
			pUnit->LocalLengthToSI(H),
			pUnit->LocalLengthToSI(Bt),
			pUnit->LocalLengthToSI(bt),
			pUnit->LocalLengthToSI(Tt),
			pUnit->LocalLengthToSI(b),
			pUnit->LocalLengthToSI(h),
			pUnit->LocalLengthToSI(b1),
			pUnit->LocalLengthToSI(b2),
			pUnit->LocalLengthToSI(x),
			pUnit->LocalLengthToSI(y),

			SlopeType,
			i,
			i1,
			i2
		};
		if (bState)
		{
			Info* pInfo = new Info();
			if (!AppendSec<Para, Info>(nSec, strName, para, pInfo, bStatus))
			{
				errorCode = "添加截面：" + std::to_string(nSec) + "失败！";
				return { false, errorCode };
			}
			AppendRebarInfo(nSec);
		}
		else
		{
			Info* pInfo = dynamic_cast<Info*>(GET_PREP_SEC()->getInfo(nSec));
			if (!pInfo)
			{
				errorCode = "未在数据库中找到编号为：" + std::to_string(nSec) + "的截面！";
				return { false, errorCode };
			}
			if (!ModifySec<Para, Info>(nSec, strName, para, pInfo))
			{
				errorCode = "修改截面：" + std::to_string(nSec) + "失败！";
				return { false, errorCode };
			}
		}
	}
		break;
	case PrepEnum::Section::TGirder:
	{
		SecEnum::GirderPosition GirderPos{ SecEnum::GirderPosition::Unassigned }; std::string strGirderPos;
		double H{ 0.0 }, Bs{ 0.0 }, Bm{ 0.0 }, Bc{ 0.0 }, Tt1{ 0.0 }, Tt2{ 0.0 }, x{ 0.0 }, Tw{ 0.0 }, Bh{ 0.0 }, Hh{ 0.0 }, yh{ 0.0 };
		bool HasSlope{ true };
		double i1{ 0.0 }, i2{ 0.0 }, R{ 0.0 };
		try {
			strGirderPos = safe_cast<std::string>(kwargs, "GirderPos");
			if (!GetCommand()->StrToGirderPosition(GirderPos, strGirderPos, 4))
			{
				errorCode = "参数 GirderPos 错误！";
				return { false, errorCode };
			}
			H = safe_cast<double>(kwargs, "H");
			Bs = safe_cast<double>(kwargs, "Bs");
			Bm = safe_cast<double>(kwargs, "Bm");
			Bc = safe_cast<double>(kwargs, "Bc");
			Tt1 = safe_cast<double>(kwargs, "Tt1");
			Tt2 = safe_cast<double>(kwargs, "Tt2");
			x = safe_cast<double>(kwargs, "x");
			Tw = safe_cast<double>(kwargs, "Tw");
			Bh = safe_cast<double>(kwargs, "Bh");
			Hh = safe_cast<double>(kwargs, "Hh");
			yh = safe_cast<double>(kwargs, "yh");

			HasSlope = safe_cast<bool>(kwargs, "Slope");
			
			i1 = safe_cast<double>(kwargs, "i1");
			i2 = safe_cast<double>(kwargs, "i2");
			R = safe_cast<double>(kwargs, "R");
		}
		catch (const std::runtime_error& e) {
			// 处理错误
			return { false, e.what() };
		}

		using Info = PREP::TGirderInfo;
		using Para = PREP::TGirderPara;

		auto* pUnit = GetProject()->GetUnit();
		Para para = {
			GirderPos,
			pUnit->LocalLengthToSI(H),
			pUnit->LocalLengthToSI(Bs),
			pUnit->LocalLengthToSI(Bm),
			pUnit->LocalLengthToSI(Bc),
			pUnit->LocalLengthToSI(Tt1),
			pUnit->LocalLengthToSI(Tt2),
			pUnit->LocalLengthToSI(x),
			pUnit->LocalLengthToSI(Tw),
			pUnit->LocalLengthToSI(Bh),
			pUnit->LocalLengthToSI(Hh),
			pUnit->LocalLengthToSI(yh),
			pUnit->LocalLengthToSI(R),
			HasSlope,
			i1,
			i2
		};
		if (bState)
		{
			Info* pInfo = new Info();
			if (!AppendSec<Para, Info>(nSec, strName, para, pInfo, bStatus))
			{
				errorCode = "添加截面：" + std::to_string(nSec) + "失败！";
				return { false, errorCode };
			}
			AppendRebarInfo(nSec);
		}
		else
		{
			Info* pInfo = dynamic_cast<Info*>(GET_PREP_SEC()->getInfo(nSec));
			if (!pInfo)
			{
				errorCode = "未在数据库中找到编号为：" + std::to_string(nSec) + "的截面！";
				return { false, errorCode };
			}
			if (!ModifySec<Para, Info>(nSec, strName, para, pInfo))
			{
				errorCode = "修改截面：" + std::to_string(nSec) + "失败！";
				return { false, errorCode };
			}
		}
	}
		break;
	case PrepEnum::Section::HollowSlab:
	{
		SecEnum::GirderPosition GirderPos{ SecEnum::GirderPosition::Unassigned }; std::string strGirderPos;
		double H{ 0.0 }, Bs{ 0.0 }, Bm{ 0.0 }, Bj{ 0.0 }, Tt{ 0.0 }, Tb{ 0.0 }, Tw{ 0.0 }, Tc{ 0.0 }, Tc1{ 0.0 }, Bc{ 0.0 };
		double xi1{ 0.0 }, yi1{ 0.0 }, xi2{ 0.0 }, yi2{ 0.0 }, xo3{ 0.0 }, yo3{ 0.0 }, xo4{ 0.0 }, yo4{ 0.0 }, h1{ 0.0 };

		try {
			strGirderPos = safe_cast<std::string>(kwargs, "GirderPos");
			if (!GetCommand()->StrToGirderPosition(GirderPos, strGirderPos, 4))
			{
				errorCode = "参数 GirderPos 错误！";
				return { false, errorCode };
			}
			H = safe_cast<double>(kwargs, "H");
			Bs = safe_cast<double>(kwargs, "Bs");
			Bm = safe_cast<double>(kwargs, "Bm");
			Bj = safe_cast<double>(kwargs, "Bj");
			Tt = safe_cast<double>(kwargs, "Tt");
			Tb = safe_cast<double>(kwargs, "Tb");
			Tw = safe_cast<double>(kwargs, "Tw");
			Tc = safe_cast<double>(kwargs, "Tc");
			Tc1 = safe_cast<double>(kwargs, "Tc1");
			Bc = safe_cast<double>(kwargs, "Bc");
			xi1 = safe_cast<double>(kwargs, "xi1");
			yi1 = safe_cast<double>(kwargs, "yi1");
			xi2 = safe_cast<double>(kwargs, "xi2");
			yi2 = safe_cast<double>(kwargs, "yi2");
			xo3 = safe_cast<double>(kwargs, "xo3");
			yo3 = safe_cast<double>(kwargs, "yo3");
			xo4 = safe_cast<double>(kwargs, "xo4");
			yo4 = safe_cast<double>(kwargs, "yo4");
			h1 = safe_cast<double>(kwargs, "h1");
		}
		catch (const std::runtime_error& e) {
			// 处理错误
			return { false, e.what() };
		}

		using Info = PREP::HollowSlabInfo;
		using Para = PREP::HollowSlabPara;

		auto* pUnit = GetProject()->GetUnit();
		Para para = {
			GirderPos,
			pUnit->LocalLengthToSI(H),
			pUnit->LocalLengthToSI(Bs),
			pUnit->LocalLengthToSI(Bm),
			pUnit->LocalLengthToSI(Bj),
			pUnit->LocalLengthToSI(Tt),
			pUnit->LocalLengthToSI(Tb),
			pUnit->LocalLengthToSI(Tw),
			pUnit->LocalLengthToSI(Tc),
			pUnit->LocalLengthToSI(Tc1),
			pUnit->LocalLengthToSI(Bc),

			pUnit->LocalLengthToSI(xi1),
			pUnit->LocalLengthToSI(yi1),
			pUnit->LocalLengthToSI(xi2),
			pUnit->LocalLengthToSI(yi2),
			pUnit->LocalLengthToSI(xo3),
			pUnit->LocalLengthToSI(yo3),
			pUnit->LocalLengthToSI(xo4),
			pUnit->LocalLengthToSI(yo4),
			pUnit->LocalLengthToSI(h1)
		};
		if (bState)
		{
			Info* pInfo = new Info();
			if (!AppendSec<Para, Info>(nSec, strName, para, pInfo, bStatus))
			{
				errorCode = "添加截面：" + std::to_string(nSec) + "失败！";
				return { false, errorCode };
			}
			AppendRebarInfo(nSec);
		}
		else
		{
			Info* pInfo = dynamic_cast<Info*>(GET_PREP_SEC()->getInfo(nSec));
			if (!pInfo)
			{
				errorCode = "未在数据库中找到编号为：" + std::to_string(nSec) + "的截面！";
				return { false, errorCode };
			}
			if (!ModifySec<Para, Info>(nSec, strName, para, pInfo))
			{
				errorCode = "修改截面：" + std::to_string(nSec) + "失败！";
				return { false, errorCode };
			}
		}
	}
		break;
	case PrepEnum::Section::CustomShape:
	{
		errorCode = "暂不支持自定义截面！";
		return { false, errorCode };
	}
		break;

	// 钢梁截面
	case PrepEnum::Section::SteelI:
	{
		double H{ 0.0 }, Bt{ 0.0 }, Bb{ 0.0 }, Tt{ 0.0 }, Tb{ 0.0 }, Tw{ 0.0 };
		SecEnum::RibLocation Location{ SecEnum::RibLocation::Right }; std::string strLocation;
		try {
			
			H = safe_cast<double>(kwargs, "H");
			Bt = safe_cast<double>(kwargs, "Bt");
			Bb = safe_cast<double>(kwargs, "Bb");
			Tt = safe_cast<double>(kwargs, "Tt");
			Tb = safe_cast<double>(kwargs, "Tb");
			Tw = safe_cast<double>(kwargs, "Tw");
			strLocation = safe_cast<std::string>(kwargs, "WebRibPos");
			if (!GetCommand()->StrToRibLocation(Location, strLocation, 10))
			{
				errorCode = "参数 WebRibPos 错误！";
				return { false, errorCode };
			}
		}
		catch (const std::runtime_error& e) {
			// 处理错误
			return { false, e.what() };
		}

		using Info = PREP::SteelIInfo;
		//using Para = PREP::HollowSlabPara;

		if (bState)
		{
			Info* pInfo = new Info();

			pInfo->clearRibAll();

			auto* pUnit = GetProject()->GetUnit();
			pInfo->setBeamHeight(pUnit->LocalLengthToSI(H));
			pInfo->setTopFlangeWidth(pUnit->LocalLengthToSI(Bt));
			pInfo->setBotFlangeWidth(pUnit->LocalLengthToSI(Bb));
			pInfo->setTopFlangeThickness(pUnit->LocalLengthToSI(Tt));
			pInfo->setBotFlangeThickness(pUnit->LocalLengthToSI(Tb));
			pInfo->setWebThickness(pUnit->LocalLengthToSI(Tw));
			pInfo->setCentWebRibPos(Location);
			if (!AppendSteelSec<Info>(nSec, strName, pInfo, bStatus))
			{
				errorCode = "添加截面：" + std::to_string(nSec) + "失败！";
				return { false, errorCode };
			}
		}
		else
		{
			Info* pInfo = dynamic_cast<Info*>(GET_PREP_SEC()->getInfo(nSec));
			if (!pInfo)
			{
				errorCode = "未在数据库中找到编号为：" + std::to_string(nSec) + "的截面！";
				return { false, errorCode };
			}
			////压入影子命令
			//yilCString strCommand;
			//pInfo->formatAPDL(strCommand);
			//PUSH_SHADOW_CMD(THIS_IS_MOD, strCommand);

			auto* pUnit = GetProject()->GetUnit();
			pInfo->setBeamHeight(pUnit->LocalLengthToSI(H));
			pInfo->setTopFlangeWidth(pUnit->LocalLengthToSI(Bt));
			pInfo->setBotFlangeWidth(pUnit->LocalLengthToSI(Bb));
			pInfo->setTopFlangeThickness(pUnit->LocalLengthToSI(Tt));
			pInfo->setBotFlangeThickness(pUnit->LocalLengthToSI(Tb));
			pInfo->setWebThickness(pUnit->LocalLengthToSI(Tw));
			pInfo->setCentWebRibPos(Location);

			pInfo->setNO(nSec);
			pInfo->setName(strName);
			pInfo->updateRegion();
		}
	}
		break;
	case PrepEnum::Section::SteelBox:
	{
		double H{ 0.0 }, Bt{ 0.0 }, Bct{ 0.0 }, Bb{ 0.0 }, Bcb{ 0.0 }, Tt{ 0.0 }, Tb{ 0.0 }, Tw{ 0.0 };
		bool SameLayout{ true };
		try {

			H = safe_cast<double>(kwargs, "H");
			Bt = safe_cast<double>(kwargs, "Bt");
			Bct = safe_cast<double>(kwargs, "Bct");
			Bb = safe_cast<double>(kwargs, "Bb");
			Bcb = safe_cast<double>(kwargs, "Bcb");
			Tt = safe_cast<double>(kwargs, "Tt");
			Tb = safe_cast<double>(kwargs, "Tb");
			Tw = safe_cast<double>(kwargs, "Tw");
		}
		catch (const std::runtime_error& e) {
			// 处理错误
			return { false, e.what() };
		}
		using Info = PREP::SteelBoxInfo;
		if (bState)
		{
			Info* pInfo = new Info();

			pInfo->clearRibAll();

			auto* pUnit = GetProject()->GetUnit();
			pInfo->setBeamHeight(pUnit->LocalLengthToSI(H));
			pInfo->setTopFlangeWidth(pUnit->LocalLengthToSI(Bt));
			pInfo->setOuterLengthOfTopFlange(pUnit->LocalLengthToSI(Bct));
			pInfo->setBotFlangeWidth(pUnit->LocalLengthToSI(Bb));
			pInfo->setOuterLengthOfBotFlange(pUnit->LocalLengthToSI(Bcb));
			pInfo->setTopFlangeThickness(pUnit->LocalLengthToSI(Tt));
			pInfo->setBotFlangeThickness(pUnit->LocalLengthToSI(Tb));
			pInfo->setWebThickness(pUnit->LocalLengthToSI(Tw));
			pInfo->setBotIsSameWithTop(SameLayout);
			if (!AppendSteelSec<Info>(nSec, strName, pInfo, bStatus))
			{
				errorCode = "添加截面：" + std::to_string(nSec) + "失败！";
				return { false, errorCode };
			}
		}
		else
		{
			Info* pInfo = dynamic_cast<Info*>(GET_PREP_SEC()->getInfo(nSec));
			if (!pInfo)
			{
				errorCode = "未在数据库中找到编号为：" + std::to_string(nSec) + "的截面！";
				return { false, errorCode };
			}
			////压入影子命令
			//yilCString strCommand;
			//pInfo->formatAPDL(strCommand);
			//PUSH_SHADOW_CMD(THIS_IS_MOD, strCommand);

			auto* pUnit = GetProject()->GetUnit();
			pInfo->setBeamHeight(pUnit->LocalLengthToSI(H));
			pInfo->setTopFlangeWidth(pUnit->LocalLengthToSI(Bt));
			pInfo->setOuterLengthOfTopFlange(pUnit->LocalLengthToSI(Bct));
			pInfo->setBotFlangeWidth(pUnit->LocalLengthToSI(Bb));
			pInfo->setOuterLengthOfBotFlange(pUnit->LocalLengthToSI(Bcb));
			pInfo->setTopFlangeThickness(pUnit->LocalLengthToSI(Tt));
			pInfo->setBotFlangeThickness(pUnit->LocalLengthToSI(Tb));
			pInfo->setWebThickness(pUnit->LocalLengthToSI(Tw));
			pInfo->setBotIsSameWithTop(SameLayout);

			pInfo->setNO(nSec);
			pInfo->setName(strName);
			pInfo->updateRegion();
		}
	}
		break;
	case PrepEnum::Section::SteelBoxThreeCell:
	{
		double H{ 0.0 }, Bt{ 0.0 }, Bb{ 0.0 }, i{ 0.0 }, a1{ 0.0 }, a2{ 0.0 }, Dt{ 0.0 }, Tt1{ 0.0 }, Tt2{ 0.0 };
		double Tb1{ 0.0 }, Db{ 0.0 }, Tb2{ 0.0 }, Tb3{ 0.0 }, Tw1{ 0.0 }, Dw{ 0.0 }, Tw2{ 0.0 };
		bool HasMiddleWeb{ false };
		SecEnum::RibLocation RibLocation{ SecEnum::RibLocation::Right }; std::string strRibLocation;
		try {

			H = safe_cast<double>(kwargs, "H");
			Bt = safe_cast<double>(kwargs, "Bt");
			Bb = safe_cast<double>(kwargs, "Bb");
			i = safe_cast<double>(kwargs, "i");
			a1 = safe_cast<double>(kwargs, "a1");
			a2 = safe_cast<double>(kwargs, "a2");
			Dt = safe_cast<double>(kwargs, "Dt");
			Tt1 = safe_cast<double>(kwargs, "Tt1");
			Tt2 = safe_cast<double>(kwargs, "Tt2");
			Tb1 = safe_cast<double>(kwargs, "Tb1");
			Db = safe_cast<double>(kwargs, "Db");
			Tb2 = safe_cast<double>(kwargs, "Tb2");
			Tb3 = safe_cast<double>(kwargs, "Tb3");
			Tw1 = safe_cast<double>(kwargs, "Tw1");
			Dw = safe_cast<double>(kwargs, "Dw");
			HasMiddleWeb = safe_cast<bool>(kwargs, "HasWeb");
			Tw2 = safe_cast<double>(kwargs, "Tw2");
			strRibLocation = safe_cast<std::string>(kwargs, "WebRibPos");
			if (!GetCommand()->StrToRibLocation(RibLocation, strRibLocation, 21))
			{
				errorCode = "参数 WebRibPos 错误！";
				return { false, errorCode };
			}
		}
		catch (const std::runtime_error& e) {
			// 处理错误
			return { false, e.what() };
		}
		using Info = PREP::SteelBoxThreeCellInfo;

		if (bState)
		{
			Info* pInfo = new Info();

			pInfo->clearRibAll();

			auto* pUnit = GetProject()->GetUnit();
			pInfo->setBeamHeight_H(pUnit->LocalLengthToSI(H));
			pInfo->setTopFlangeWidth_Bt(pUnit->LocalLengthToSI(Bt));
			pInfo->setPlateBotFlangeWidth_Bb(pUnit->LocalLengthToSI(Bb));
			pInfo->setTopSlope_i(i);
			pInfo->setAngle_a1(a1);
			pInfo->setAngle_a2(a2);
			pInfo->setThicknessChangePointDisOfTop_Dt(pUnit->LocalLengthToSI(Dt));
			pInfo->setTopFlangeThickness_Tt1(pUnit->LocalLengthToSI(Tt1));
			pInfo->setTopFlangeThickness_Tt2(pUnit->LocalLengthToSI(Tt2));
			pInfo->setBotFlangeThickness_Tb1(pUnit->LocalLengthToSI(Tb1));
			pInfo->setThicknessChangePointDisOfInclinedBotFlange_Db(pUnit->LocalLengthToSI(Db));
			pInfo->setBeamHeight_H(pUnit->LocalLengthToSI(H));
			pInfo->setInclinedBotFlangeThickness_Tb2(pUnit->LocalLengthToSI(Tb2));
			pInfo->setInclinedBotFlangeThickness_Tb3(pUnit->LocalLengthToSI(Tb3));
			pInfo->setSideWebThickness_Tw1(pUnit->LocalLengthToSI(Tw1));
			pInfo->setDistanceOfCentWeb_Dw(pUnit->LocalLengthToSI(Dw));
			pInfo->setCentWeb(HasMiddleWeb);
			pInfo->setCentWebThickness_Tw2(pUnit->LocalLengthToSI(Tw2));
			pInfo->setCentWebRibPos(RibLocation);
			if (!AppendSteelSec<Info>(nSec, strName, pInfo, bStatus))
			{
				errorCode = "添加截面：" + std::to_string(nSec) + "失败！";
				return { false, errorCode };
			}
		}
		else
		{
			Info* pInfo = dynamic_cast<Info*>(GET_PREP_SEC()->getInfo(nSec));
			if (!pInfo)
			{
				errorCode = "未在数据库中找到编号为：" + std::to_string(nSec) + "的截面！";
				return { false, errorCode };
			}
			////压入影子命令
			//yilCString strCommand;
			//pInfo->formatAPDL(strCommand);
			//PUSH_SHADOW_CMD(THIS_IS_MOD, strCommand);

			auto* pUnit = GetProject()->GetUnit();
			pInfo->setBeamHeight_H(pUnit->LocalLengthToSI(H));
			pInfo->setTopFlangeWidth_Bt(pUnit->LocalLengthToSI(Bt));
			pInfo->setPlateBotFlangeWidth_Bb(pUnit->LocalLengthToSI(Bb));
			pInfo->setTopSlope_i(i);
			pInfo->setAngle_a1(a1);
			pInfo->setAngle_a2(a2);
			pInfo->setThicknessChangePointDisOfTop_Dt(pUnit->LocalLengthToSI(Dt));
			pInfo->setTopFlangeThickness_Tt1(pUnit->LocalLengthToSI(Tt1));
			pInfo->setTopFlangeThickness_Tt2(pUnit->LocalLengthToSI(Tt2));
			pInfo->setBotFlangeThickness_Tb1(pUnit->LocalLengthToSI(Tb1));
			pInfo->setThicknessChangePointDisOfInclinedBotFlange_Db(pUnit->LocalLengthToSI(Db));
			pInfo->setBeamHeight_H(pUnit->LocalLengthToSI(H));
			pInfo->setInclinedBotFlangeThickness_Tb2(pUnit->LocalLengthToSI(Tb2));
			pInfo->setInclinedBotFlangeThickness_Tb3(pUnit->LocalLengthToSI(Tb3));
			pInfo->setSideWebThickness_Tw1(pUnit->LocalLengthToSI(Tw1));
			pInfo->setDistanceOfCentWeb_Dw(pUnit->LocalLengthToSI(Dw));
			pInfo->setCentWeb(HasMiddleWeb);
			pInfo->setCentWebThickness_Tw2(pUnit->LocalLengthToSI(Tw2));
			pInfo->setCentWebRibPos(RibLocation);

			pInfo->setNO(nSec);
			pInfo->setName(strName);
			pInfo->updateRegion();
		}
	}
		break;
	case PrepEnum::Section::SteelBoxInclinedTF:
	{
		double H{ 0.0 }, B{ 0.0 }, Bt{ 0.0 }, Bb{ 0.0 }, i{ 0.0 }, a1{ 0.0 }, a2{ 0.0 }, Dt{ 0.0 };
		double Tt1{ 0.0 }, Tt2{ 0.0 }, Tt3{ 0.0 }, Tb1{ 0.0 }, Db{ 0.0 }, Tb2{ 0.0 }, Tb3{ 0.0 }, Tw1{ 0.0 };
		try {

			H = safe_cast<double>(kwargs, "H");
			B = safe_cast<double>(kwargs, "B");
			Bt = safe_cast<double>(kwargs, "Bt");
			Bb = safe_cast<double>(kwargs, "Bb");
			i = safe_cast<double>(kwargs, "i");
			a1 = safe_cast<double>(kwargs, "a1");
			a2 = safe_cast<double>(kwargs, "a2");
			Dt = safe_cast<double>(kwargs, "Dt");
			Tt1 = safe_cast<double>(kwargs, "Tt1");
			Tt2 = safe_cast<double>(kwargs, "Tt2");
			Tt3 = safe_cast<double>(kwargs, "Tt3");
			Tb1 = safe_cast<double>(kwargs, "Tb1");
			Db = safe_cast<double>(kwargs, "Db");
			Tb2 = safe_cast<double>(kwargs, "Tb2");
			Tb3 = safe_cast<double>(kwargs, "Tb3");
			Tw1 = safe_cast<double>(kwargs, "Tw1");
		}
		catch (const std::runtime_error& e) {
			// 处理错误
			return { false, e.what() };
		}
		using Info = PREP::SteelBoxInclinedTFInfo;

		if (bState)
		{
			Info* pInfo = new Info();

			pInfo->clearRibAll();

			auto* pUnit = GetProject()->GetUnit();
			pInfo->setBeamHeight_H(pUnit->LocalLengthToSI(H));
			pInfo->setBeamWidth_B(pUnit->LocalLengthToSI(B));
			pInfo->setTopFlangeWidth_Bt(pUnit->LocalLengthToSI(Bt));
			pInfo->setPlateBotFlangeWidth_Bb(pUnit->LocalLengthToSI(Bb));
			pInfo->setTopSlope_i(i);
			pInfo->setAngle_a1(a1);
			pInfo->setAngle_a2(a2);
			pInfo->setThicknessChangePointDisOfTop_Dt(pUnit->LocalLengthToSI(Dt));
			pInfo->setTopFlangeThickness_Tt1(pUnit->LocalLengthToSI(Tt1));
			pInfo->setTopFlangeThickness_Tt2(pUnit->LocalLengthToSI(Tt2));
			pInfo->setInclinedTopFlangeThickness_Tt3(pUnit->LocalLengthToSI(Tt3));
			pInfo->setBotFlangeThickness_Tb1(pUnit->LocalLengthToSI(Tb1));
			pInfo->setThicknessChangePointDisOfInclinedBotFlange_Db(pUnit->LocalLengthToSI(Db));
			pInfo->setInclinedBotFlangeThickness_Tb2(pUnit->LocalLengthToSI(Tb2));
			pInfo->setInclinedBotFlangeThickness_Tb3(pUnit->LocalLengthToSI(Tb3));
			pInfo->setSideWebThickness_Tw1(pUnit->LocalLengthToSI(Tw1));
			if (!AppendSteelSec<Info>(nSec, strName, pInfo, bStatus))
			{
				errorCode = "添加截面：" + std::to_string(nSec) + "失败！";
				return { false, errorCode };
			}
		}
		else
		{
			Info* pInfo = dynamic_cast<Info*>(GET_PREP_SEC()->getInfo(nSec));
			if (!pInfo)
			{
				errorCode = "未在数据库中找到编号为：" + std::to_string(nSec) + "的截面！";
				return { false, errorCode };
			}
			////压入影子命令
			//yilCString strCommand;
			//pInfo->formatAPDL(strCommand);
			//PUSH_SHADOW_CMD(THIS_IS_MOD, strCommand);

			auto* pUnit = GetProject()->GetUnit();
			pInfo->setBeamHeight_H(pUnit->LocalLengthToSI(H));
			pInfo->setBeamWidth_B(pUnit->LocalLengthToSI(B));
			pInfo->setTopFlangeWidth_Bt(pUnit->LocalLengthToSI(Bt));
			pInfo->setPlateBotFlangeWidth_Bb(pUnit->LocalLengthToSI(Bb));
			pInfo->setTopSlope_i(i);
			pInfo->setAngle_a1(a1);
			pInfo->setAngle_a2(a2);
			pInfo->setThicknessChangePointDisOfTop_Dt(pUnit->LocalLengthToSI(Dt));
			pInfo->setTopFlangeThickness_Tt1(pUnit->LocalLengthToSI(Tt1));
			pInfo->setTopFlangeThickness_Tt2(pUnit->LocalLengthToSI(Tt2));
			pInfo->setInclinedTopFlangeThickness_Tt3(pUnit->LocalLengthToSI(Tt3));
			pInfo->setBotFlangeThickness_Tb1(pUnit->LocalLengthToSI(Tb1));
			pInfo->setThicknessChangePointDisOfInclinedBotFlange_Db(pUnit->LocalLengthToSI(Db));
			pInfo->setInclinedBotFlangeThickness_Tb2(pUnit->LocalLengthToSI(Tb2));
			pInfo->setInclinedBotFlangeThickness_Tb3(pUnit->LocalLengthToSI(Tb3));
			pInfo->setSideWebThickness_Tw1(pUnit->LocalLengthToSI(Tw1));

			pInfo->setNO(nSec);
			pInfo->setName(strName);
			pInfo->updateRegion();
		}
	}
		break;
	case PrepEnum::Section::SteelCantiBox:
	{
		double H{ 0.0 }, Bt{ 0.0 }, Bb{ 0.0 }, i{ 0.0 }, a{ 0.0 }, Dt{ 0.0 }, Tt1{ 0.0 }, Tt2{ 0.0 };
		double Tb1{ 0.0 }, Tw1{ 0.0 }, Tw2{ 0.0 }, h{ 0.0 }, t{ 0.0 };
		bool HasWeb{ false };
		SecEnum::RibLocation RibLocation{ SecEnum::RibLocation::Right }; std::string strRibLocation;
		try {

			H = safe_cast<double>(kwargs, "H");
			Bt = safe_cast<double>(kwargs, "Bt");
			Bb = safe_cast<double>(kwargs, "Bb");
			i = safe_cast<double>(kwargs, "i");
			a = safe_cast<double>(kwargs, "a");
			Dt = safe_cast<double>(kwargs, "Dt");
			Tt1 = safe_cast<double>(kwargs, "Tt1");
			Tt2 = safe_cast<double>(kwargs, "Tt2");

			Tb1 = safe_cast<double>(kwargs, "Tb1");
			Tw1 = safe_cast<double>(kwargs, "Tw1");
			Tw2 = safe_cast<double>(kwargs, "Tw2");
			h = safe_cast<double>(kwargs, "h");
			t = safe_cast<double>(kwargs, "t");
			HasWeb = safe_cast<bool>(kwargs, "HasWeb");
			strRibLocation = safe_cast<std::string>(kwargs, "WebRibPos");
			if (!GetCommand()->StrToRibLocation(RibLocation, strRibLocation, 16))
			{
				errorCode = "参数 WebRibPos 错误！";
				return { false, errorCode };
			}
		}
		catch (const std::runtime_error& e) {
			// 处理错误
			return { false, e.what() };
		}
		using Info = PREP::SteelCantiBoxInfo;

		if (bState)
		{
			Info* pInfo = new Info();

			pInfo->clearRibAll();

			auto* pUnit = GetProject()->GetUnit();
			pInfo->setBeamHeight_H(pUnit->LocalLengthToSI(H));
			pInfo->setTopFlangeWidth_Bt(pUnit->LocalLengthToSI(Bt));
			pInfo->setPlateBotFlangeWidth_Bb(pUnit->LocalLengthToSI(Bb));
			pInfo->setTopSlope_i(i);
			pInfo->setAngle_a1(a);
			pInfo->setThicknessChangePointDisOfTop_Dt(pUnit->LocalLengthToSI(Dt));
			pInfo->setTopFlangeThickness_Tt1(pUnit->LocalLengthToSI(Tt1));
			pInfo->setTopFlangeThickness_Tt2(pUnit->LocalLengthToSI(Tt2));
			pInfo->setBotFlangeThickness_Tb1(pUnit->LocalLengthToSI(Tb1));
			pInfo->setSideWebThickness_Tw1(pUnit->LocalLengthToSI(Tw1));
			pInfo->setCentWeb(pUnit->LocalLengthToSI(HasWeb));
			pInfo->setCentWebThickness_Tw2(pUnit->LocalLengthToSI(Tw2));
			pInfo->setCentWebRibPos(RibLocation);
			pInfo->setExtendHeight(pUnit->LocalLengthToSI(h));
			pInfo->setExtendWidth(pUnit->LocalLengthToSI(t));
			if (!AppendSteelSec<Info>(nSec, strName, pInfo, bStatus))
			{
				errorCode = "添加截面：" + std::to_string(nSec) + "失败！";
				return { false, errorCode };
			}
		}
		else
		{
			Info* pInfo = dynamic_cast<Info*>(GET_PREP_SEC()->getInfo(nSec));
			if (!pInfo)
			{
				errorCode = "未在数据库中找到编号为：" + std::to_string(nSec) + "的截面！";
				return { false, errorCode };
			}
			////压入影子命令
			//yilCString strCommand;
			//pInfo->formatAPDL(strCommand);
			//PUSH_SHADOW_CMD(THIS_IS_MOD, strCommand);

			auto* pUnit = GetProject()->GetUnit();
			pInfo->setBeamHeight_H(pUnit->LocalLengthToSI(H));
			pInfo->setTopFlangeWidth_Bt(pUnit->LocalLengthToSI(Bt));
			pInfo->setPlateBotFlangeWidth_Bb(pUnit->LocalLengthToSI(Bb));
			pInfo->setTopSlope_i(i);
			pInfo->setAngle_a1(a);
			pInfo->setThicknessChangePointDisOfTop_Dt(pUnit->LocalLengthToSI(Dt));
			pInfo->setTopFlangeThickness_Tt1(pUnit->LocalLengthToSI(Tt1));
			pInfo->setTopFlangeThickness_Tt2(pUnit->LocalLengthToSI(Tt2));
			pInfo->setBotFlangeThickness_Tb1(pUnit->LocalLengthToSI(Tb1));
			pInfo->setSideWebThickness_Tw1(pUnit->LocalLengthToSI(Tw1));
			pInfo->setCentWeb(pUnit->LocalLengthToSI(HasWeb));
			pInfo->setCentWebThickness_Tw2(pUnit->LocalLengthToSI(Tw2));
			pInfo->setCentWebRibPos(RibLocation);
			pInfo->setExtendHeight(pUnit->LocalLengthToSI(h));
			pInfo->setExtendWidth(pUnit->LocalLengthToSI(t));

			pInfo->setNO(nSec);
			pInfo->setName(strName);
			pInfo->updateRegion();
		}
	}
		break;
	case PrepEnum::Section::SteelCantiBoxInclinedBF:
	{
		double H{ 0.0 }, Bt{ 0.0 }, Bb{ 0.0 }, Bc{ 0.0 }, i{ 0.0 }, a{ 0.0 }, Dt{ 0.0 }, Tt1{ 0.0 }, Tt2{ 0.0 };
		double Tb1{ 0.0 }, Tb2{ 0.0 }, Tw1{ 0.0 }, Tw2{ 0.0 }, h{ 0.0 }, t{ 0.0 };
		bool HasWeb{ false };
		SecEnum::RibLocation RibLocation{ SecEnum::RibLocation::Right };; std::string strRibLocation;
		try {

			H = safe_cast<double>(kwargs, "H");
			Bt = safe_cast<double>(kwargs, "Bt");
			Bb = safe_cast<double>(kwargs, "Bb");
			Bc = safe_cast<double>(kwargs, "Bc");
			i = safe_cast<double>(kwargs, "i");
			a = safe_cast<double>(kwargs, "a");
			Dt = safe_cast<double>(kwargs, "Dt");
			Tt1 = safe_cast<double>(kwargs, "Tt1");
			Tt2 = safe_cast<double>(kwargs, "Tt2");

			Tb1 = safe_cast<double>(kwargs, "Tb1");
			Tb2 = safe_cast<double>(kwargs, "Tb2");
			Tw1 = safe_cast<double>(kwargs, "Tw1");
			Tw2 = safe_cast<double>(kwargs, "Tw2");
			h = safe_cast<double>(kwargs, "h");
			t = safe_cast<double>(kwargs, "t");
			HasWeb = safe_cast<bool>(kwargs, "HasWeb");
			strRibLocation = safe_cast<std::string>(kwargs, "WebRibPos");
			if (!GetCommand()->StrToRibLocation(RibLocation, strRibLocation, 16))
			{
				errorCode = "参数 WebRibPos 错误！";
				return { false, errorCode };
			}
		}
		catch (const std::runtime_error& e) {
			// 处理错误
			return { false, e.what() };
		}
		using Info = PREP::SteelCantiBoxInclinedBFInfo;

		if (bState)
		{
			Info* pInfo = new Info();

			pInfo->clearRibAll();

			auto* pUnit = GetProject()->GetUnit();
			pInfo->setBeamHeight_H(pUnit->LocalLengthToSI(H));
			pInfo->setTopFlangeWidth_Bt(pUnit->LocalLengthToSI(Bt));
			pInfo->setPlateBotFlangeWidth_Bb(pUnit->LocalLengthToSI(Bb));
			pInfo->setExtendLength_Bc(pUnit->LocalLengthToSI(Bc));
			pInfo->setTopSlope_i(i);
			pInfo->setAngle_a1(a);
			pInfo->setThicknessChangePointDisOfTop_Dt(pUnit->LocalLengthToSI(Dt));
			pInfo->setTopFlangeThickness_Tt1(pUnit->LocalLengthToSI(Tt1));
			pInfo->setTopFlangeThickness_Tt2(pUnit->LocalLengthToSI(Tt2));
			pInfo->setBotFlangeThickness_Tb1(pUnit->LocalLengthToSI(Tb1));
			pInfo->setBotFlangeThickness_Tb2(pUnit->LocalLengthToSI(Tb2));
			pInfo->setSideWebThickness_Tw1(pUnit->LocalLengthToSI(Tw1));
			pInfo->setCentWeb(pUnit->LocalLengthToSI(HasWeb));
			pInfo->setCentWebThickness_Tw2(pUnit->LocalLengthToSI(Tw2));
			pInfo->setCentWebRibPos(RibLocation);
			pInfo->setExtendHeight(pUnit->LocalLengthToSI(h));
			pInfo->setExtendWidth(pUnit->LocalLengthToSI(t));
			if (!AppendSteelSec<Info>(nSec, strName, pInfo, bStatus))
			{
				errorCode = "添加截面：" + std::to_string(nSec) + "失败！";
				return { false, errorCode };
			}
		}
		else
		{
			Info* pInfo = dynamic_cast<Info*>(GET_PREP_SEC()->getInfo(nSec));
			if (!pInfo)
			{
				errorCode = "未在数据库中找到编号为：" + std::to_string(nSec) + "的截面！";
				return { false, errorCode };
			}
			////压入影子命令
			//yilCString strCommand;
			//pInfo->formatAPDL(strCommand);
			//PUSH_SHADOW_CMD(THIS_IS_MOD, strCommand);

			auto* pUnit = GetProject()->GetUnit();
			pInfo->setBeamHeight_H(pUnit->LocalLengthToSI(H));
			pInfo->setTopFlangeWidth_Bt(pUnit->LocalLengthToSI(Bt));
			pInfo->setPlateBotFlangeWidth_Bb(pUnit->LocalLengthToSI(Bb));
			pInfo->setExtendLength_Bc(pUnit->LocalLengthToSI(Bc));
			pInfo->setTopSlope_i(i);
			pInfo->setAngle_a1(a);
			pInfo->setThicknessChangePointDisOfTop_Dt(pUnit->LocalLengthToSI(Dt));
			pInfo->setTopFlangeThickness_Tt1(pUnit->LocalLengthToSI(Tt1));
			pInfo->setTopFlangeThickness_Tt2(pUnit->LocalLengthToSI(Tt2));
			pInfo->setBotFlangeThickness_Tb1(pUnit->LocalLengthToSI(Tb1));
			pInfo->setBotFlangeThickness_Tb2(pUnit->LocalLengthToSI(Tb2));
			pInfo->setSideWebThickness_Tw1(pUnit->LocalLengthToSI(Tw1));
			pInfo->setCentWeb(pUnit->LocalLengthToSI(HasWeb));
			pInfo->setCentWebThickness_Tw2(pUnit->LocalLengthToSI(Tw2));
			pInfo->setCentWebRibPos(RibLocation);
			pInfo->setExtendHeight(pUnit->LocalLengthToSI(h));
			pInfo->setExtendWidth(pUnit->LocalLengthToSI(t));

			pInfo->setNO(nSec);
			pInfo->setName(strName);
			pInfo->updateRegion();
		}
	}
		break;

	// 数值截面
	case PrepEnum::Section::Numerical:
	{
		errorCode = "暂不支持数值截面！";
		return { false, errorCode };
	}
		break;
	default:
		errorCode = "参数 截面类型 错误！";
		return { false, errorCode };
	}

	GetProject()->GetPlotControl()->StructTreeChangedOn();
	GetProject()->GetPlotControl()->SectionDataChangedOn();

	return { true, errorCode };
}

/// <summary>
/// 定义截面偏心
/// </summary>
/// <param name="nSec">截面编号</param>
/// <param name="offsetTypeY">截面局部坐标系下Y方向的偏心，可选择Left、Middle、Right、Manual</param>
/// <param name="offsetValueY">Y方向的偏心数值，在Y方向偏心为Manual时该项起作用</param>
/// <param name="offsetTypeZ">截面局部坐标系下Z方向的偏心，可选择Top、Center、Bottom、Manual</param>
/// <param name="offsetValueZ">Z方向的偏心数值，在Z方向偏心为Manual时该项起作用</param>
/// <returns>是否成功，失败原因</returns>
std::pair<bool, std::string> PyInterface::OSIS_SectionOffset(const int nSec, const std::string offsetTypeY, const double dOffsetValueY, const std::string offsetTypeZ, const double dOffsetValueZ)
{
	std::string errorCode = "";
	SecEnum::SectionOffsetY _offsetTypeY{ SecEnum::SectionOffsetY::Middle };
	SecEnum::SectionOffsetZ _offsetTypeZ{ SecEnum::SectionOffsetZ::Top };

	if (!GetCommand()->StrToSectionOffsetY(_offsetTypeY, offsetTypeY, 2))
	{
		errorCode = "参数 Y方向的偏心 错误！";
		return { false, errorCode };
	}
	if (!GetCommand()->StrToSectionOffsetZ(_offsetTypeZ, offsetTypeZ, 3))
	{
		errorCode = "参数 Z方向的偏心 错误！";
		return { false, errorCode };
	}

	PREP::SectionInfo* pInfo = GET_PREP_SEC()->getInfo(nSec);
	if (!pInfo) 
	{
		errorCode = "未找到编号为" + std::to_string(nSec) + "的截面！";
		return { false, errorCode };
	}

	////压入影子命令
	//yilCString strCommand;
	//pInfo->formatAPDLSectionOffset(strCommand);
	//PUSH_SHADOW_CMD(THIS_IS_MOD, strCommand);

	auto* pUnit = GetProject()->GetUnit();
	pInfo->setOffsetTypeY(_offsetTypeY);
	if (_offsetTypeY == SecEnum::SectionOffsetY::ManualInput) {

		double value = pUnit->LocalLengthToSI(dOffsetValueY);
		pInfo->setOffsetValueY(value);
	}

	pInfo->setOffsetTypeZ(_offsetTypeZ);
	if (_offsetTypeZ == SecEnum::SectionOffsetZ::ManualInput) {

		double value = pUnit->LocalLengthToSI(dOffsetValueZ);
		pInfo->setOffsetValueZ(value);
	}
	pInfo->updatePropByModelingPoint();
	GetProject()->GetPlotControl()->SectionDataChangedOn();

	//return { true, "" };
	return { true, errorCode };
}

/// <summary>
/// 定义截面网格划分
/// </summary>
/// <param name="nSec">截面编号</param>
/// <param name="bMeshMethod">网格划分方法，0=自动划分，1=手动划分</param>
/// <param name="dMeshSize">网格划分尺寸，在MeshMethod=1时该项起作用</param>
/// <returns>是否成功，失败原因</returns>
std::pair<bool, std::string> PyInterface::OSIS_SectionMesh(const int nSec, bool bMeshMethod, double dMeshSize)
{
	std::string errorCode = "";
	PREP::SectionInfo* pInfo = GET_PREP_SEC()->getInfo(nSec);
	if (!pInfo) 
	{
		errorCode = "未找到编号为" + std::to_string(nSec) + "的截面！";
		return { false, errorCode };
	}
	PREP::ConcreteSectionInfo* pConcInfo = dynamic_cast<PREP::ConcreteSectionInfo*>(pInfo);

	//压入影子命令
	//yilCString strCommand;
	//pConcInfo->formatAPDLSectionMesh(strCommand);
	//PUSH_SHADOW_CMD(THIS_IS_MOD, strCommand);

	pConcInfo->setMeshMethod(bMeshMethod);
	pConcInfo->setMeshSize(dMeshSize);

	if (!pConcInfo->updateMeshInfo()) 
	{
		errorCode = "截面" + std::to_string(nSec) + "网格参数设置失败！";
		return { false, errorCode };
	}

	return { true, errorCode };
	//return { true, "成功创建截面，编号:" + std::to_string(nSec) + "！" };
}

/// <summary>
/// 删除截面
/// </summary>
/// <param name="nSec">截面编号</param>
/// <returns></returns>
std::pair<bool, std::string> PyInterface::OSIS_SectionDel(const int nSec)
{
	std::string errorCode;
	//ASSERT_FEM_STATUS();

	//yilCString VarStr[1]{ _T("") };
	//if (!StandardCommandParse(_Command, VarStr, 1)) return false;

	//int nSec;
	//if (!StrToIntGTZero(nSec, VarStr[0], 1)) return false;

	PREP::SectionInfo* pSecInfo = GET_PREP_SEC()->getInfo(nSec);
	if (!pSecInfo) 
	{
		//errorCode = "未找到编号为" + std::to_string(nSec) + "的截面！";
		return { true, errorCode };		// 没找到，就是完成了
	}

	//压入影子命令
	//修改前压入，如果失败再弹出
	//yilCString strCommand;

	auto pRebar = GET_PREP_SEC()->getAuxParasInfo<PREP::SecRebarInfo>(nSec);
	if (pRebar) {

		//pRebar->formatAPDL(strCommand);
		//PUSH_SHADOW_CMD(THIS_IS_DEL, strCommand);

		if (!GET_PREP_SEC()->deleteAuxParasByNO<PREP::SecRebarInfo>(nSec)) {
			//POP_SHADOW_CMD();
			errorCode = "删除截面" + std::to_string(nSec) + "失败";
			return { false, errorCode };
		}
	}


	//pSecInfo->formatAPDLBase(strCommand);
	//PUSH_SHADOW_CMD(THIS_IS_DEL, strCommand);

	if (!GET_PREP_SEC()->deleteByNO(nSec)) {
		//POP_SHADOW_CMD();
		errorCode = "删除截面" + std::to_string(nSec) + "失败";
		return { false, errorCode };
	}

	GetProject()->GetPlotControl()->InputChangedOn();
	GetProject()->GetPlotControl()->StructTreeChangedOn();
	GetProject()->GetPlotControl()->SectionDataChangedOn();

	return { true, errorCode };
}

/// <summary>
/// 修改截面编号
/// </summary>
/// <param name="nOldIndex">旧编号</param>
/// <param name="nNewIndex">新编号</param>
/// <returns></returns>
std::pair<bool, std::string> PyInterface::OSIS_SectionMod(const int nOld, const int nNew)
{
	std::string errorCode;
	//ASSERT_FEM_STATUS();

	//Cmd strCmd;
	//if (!StandardCommandParse(strCmd, _Command, 2))
	//	return false;

	auto* pInfo = GET_PREP_SEC()->getInfo(nOld);
	if (!pInfo)
	{
		errorCode = "未找到编号为" + std::to_string(nOld) + "的截面！";
		return { false, errorCode };
	}

	if (GET_PREP_SEC()->getInfo(nNew)) {
		//m_ErrCode.Format(_T("截面[%d]已存在"), nNew);
		//g_pWarningList->AddToTemperary(4, m_ErrCode);
		errorCode = "截面" + std::to_string(nNew) + "已存在！";
		return { false, errorCode };
	}

	GET_PREP_SEC()->modifyIndex(nOld, nNew); //虽然这句话应该在最后执行，但是限制于下面关联参数的数据检查，先放这里
	GET_PREP_SEC()->modifyAuxParasIndex<PREP::SecRebarInfo>(nOld, nNew);

	auto* pEM = GET_PREP_ELEMENT();
	const set<int>& setRelatedElement = pInfo->getRelatedElement();
	if (!setRelatedElement.empty()) {

		for (const int it : setRelatedElement) {
			auto* pRelatedInfo = pEM->getInfo(it);
			if (!pRelatedInfo) continue;
			pRelatedInfo->substitueSection(nOld, nNew);
		}

		GetProject()->GetPlotControl()->ElementDataChangedOn();
		GetProject()->GetPlotControl()->ElemPropBeam3DsDataChangedOn();
	}
	pEM = nullptr;

	//压入影子命令
	//yilCString strCommand;
	//strCommand.Format(_T("SectionMod,%d,%d"), nNew, nOld);
	//PUSH_SHADOW_CMD(THIS_IS_MOD, strCommand);

	GetProject()->GetPlotControl()->SectionDataChangedOn();
	GetProject()->GetPlotControl()->InputChangedOn();
	GetProject()->GetPlotControl()->StructTreeChangedOn();
	return { true, errorCode };
}
