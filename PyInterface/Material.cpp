#include "stdafx.h"
#include "PyInterface.h"

bool RestoreMat(bool _status, int _index, PREP::MaterialInfo* _pInfo)
{
	auto* __MM__ = GET_PREP_MAT();
	if (_status) {
		if (!__MM__->append(_pInfo)) {

			delete _pInfo; _pInfo = nullptr;
			return false;
		}
		//PUSH_SHADOW_CMD_CUSTOM(THIS_IS_DEF, "MaterialDel,%d", _index);
	}
	else {

		//PUSH_SHADOW_CMD_DEFAULT(THIS_IS_MOD, __MM__->getInfo(_index));
		if (!__MM__->switchDerivedType(_pInfo)) {

			delete _pInfo; _pInfo = nullptr;
			return false;
		}
	}
}

/// <summary>
/// 定义或修改混凝土材料
/// </summary>
/// <param name="nMat">编号</param>
/// <param name="strName">材料别名</param>
/// <param name="material">
///		CONC = 混凝土
///		STEEL = 钢材
///		PRESTRESSED = 预应力材料
///		REBAR = 普通钢筋
/// </param>
/// <param name="code">
///		混凝土 JTG3362_2018，JTGD62_2004
///		钢材 JTGD64_2015
///		预应力 JTG3362_2018，JTGD62_2004
///		普通钢筋 JTG3362_2018，JTGD62_2004
/// </param>
/// <param name="grade">等级牌号</param>
/// <param name="nCrepShrk">收缩徐变特性编号，缺省时值为-1</param>
/// <param name="dDmp">材料阻尼比</param>
/// <returns>是否成功，失败原因</returns>
std::pair<bool, std::string> PyInterface::OSIS_Material(const int nMat, const std::string strName, const std::string eMaterialType, const std::string eCode, const std::string eGrade, const int nCrepShrk, const double dDmp)
{
	std::string errorCode;
	//strName = utf8_to_wide(strName);
	PrepEnum::Material eMT = PrepEnum::Material::Unassigned;
	if (!m_pCommand->StrToMaterial(eMT, eMaterialType, 3))
	{
		errorCode = "参数 材料类型 错误!";
		return { false, errorCode };
	}
	
	bool bState = true;  //状态，true = 定义，false = 修改
	bool bStatus = true; //针对bState = true的情况：true = 初次定义；false = 修改类型
	PREP::MaterialInfo* pMat = GET_PREP_MAT()->getInfo(nMat);
	if (pMat) {

		bState = false;

		/* > > > > > > 切换材料类型 > > > > > > */
		if (pMat->getType() != eMT) { //修改一个已经存在的材料类型

			bState = true;
			bStatus = false;
		}
		/* < < < < < < 切换材料类型 < < < < < < */
	}

	PrepEnum::Code _eCode = PrepEnum::Code::Unassigned;
	if (!m_pCommand->StrToCode(_eCode, eCode, 4))
	{
		errorCode = "参数 材料标准代码 错误!";
		return { false, errorCode };
	}

	auto* pMM = GET_PREP_MAT();		// 材料管理器
	switch (eMT)
	{
	case PrepEnum::Material::Conc:
	{
		PrepEnum::Conc _eGrade = PrepEnum::Conc::Unassigned;
		if (!m_pCommand->StrToConc(_eGrade, eGrade, 4))
		{
			errorCode = "混凝土材料的参数 材料等级 错误!";
			return { false, errorCode };
		}
		if (bState)
		{
			PREP::ConcInfo* pInfo = new PREP::ConcInfo(nMat, strName, PrepEnum::Material::Conc, _eCode, _eGrade);
			if (nCrepShrk != -1)
			{
				if (!pInfo->setCrpShrk(nCrepShrk)) {
					errorCode = "编号为" + std::to_string(nCrepShrk) + "的收缩徐变特性设置失败！";
					delete pInfo; pInfo = nullptr;
					return { false, errorCode };
				}
			}
			pInfo->setDamping(dDmp);
			pInfo->check(vector<int>());
			RestoreMat(bStatus, nMat, pInfo);
		}
		else
		{
			PREP::ConcInfo* pInfo = dynamic_cast<PREP::ConcInfo*>(pMM->getInfo(nMat));
			if (!pInfo)
			{
				errorCode = "未在数据库中找到编号为：" + std::to_string(nMat) + "的材料！";
				return { false, errorCode };
			}
			pInfo->check(vector<int>());
			pInfo->modifyThis(strName, _eCode, _eGrade);

			if (!pInfo->setCrpShrk(nCrepShrk)) {
				//POP_SHADOW_CMD();
				return { false, errorCode };
			}
			pInfo->setDamping(dDmp);
		}
		m_pProject->GetPlotControl()->CrpShrkDataChangedOn();
	}
		break;
	case PrepEnum::Material::Steel:
	{
		if (_eCode != PrepEnum::Code::JTGD64_2015)
		{
			errorCode = "钢材的 材料标准代码 不支持" + eCode + "!";
			return { false, errorCode };
		}
		PrepEnum::Steel _eGrade = PrepEnum::Steel::Unassigned;
		if (!m_pCommand->StrToSteel(_eGrade, eGrade, 4))
		{
			errorCode = "钢材材料的参数 材料等级 错误!";
			return { false, errorCode };
		}
		if (bState)
		{
			PREP::SteelInfo* pInfo = new PREP::SteelInfo(nMat, strName, PrepEnum::Material::Steel, _eCode, _eGrade);
			pInfo->setDamping(dDmp);

			pInfo->check(vector<int>());
			RestoreMat(bStatus, nMat, pInfo);
		}
		else
		{
			PREP::SteelInfo* pInfo = dynamic_cast<PREP::SteelInfo*>(pMM->getInfo(nMat));
			if (!pInfo)
			{
				errorCode = "未在数据库中找到编号为：" + std::to_string(nMat) + "的材料！";
				return { false, errorCode };
			}
			pInfo->check(vector<int>());

			////压入影子命令
			//yilCString strCommand;
			//pInfo->formatAPDL(strCommand);
			//PUSH_SHADOW_CMD(THIS_IS_MOD, strCommand);

			pInfo->modifyThis(strName, _eCode, _eGrade);
			pInfo->setDamping(dDmp);
		}
	}
		break;
	case PrepEnum::Material::Prestressed:  
	{
		PrepEnum::Prestressed _eGrade = PrepEnum::Prestressed::Unassigned;
		if (!m_pCommand->StrToPrestressed(_eGrade, eGrade, 4))
		{
			errorCode = "预应力材料的参数 材料等级 错误!";
			return { false, errorCode };
		}
		if (bState)
		{
			PREP::PrestressedInfo* pInfo = new PREP::PrestressedInfo(nMat, strName, PrepEnum::Material::Prestressed, _eCode, _eGrade);
			pInfo->setDamping(dDmp);

			pInfo->check(vector<int>());
			RestoreMat(bStatus, nMat, pInfo);
		}
		else
		{
			PREP::PrestressedInfo* pInfo = dynamic_cast<PREP::PrestressedInfo*>(pMM->getInfo(nMat));
			if (!pInfo)
			{
				errorCode = "未在数据库中找到编号为：" + std::to_string(nMat) + "的材料！";
				return { false, errorCode };
			}
			pInfo->check(vector<int>());

			////压入影子命令
			//yilCString strCommand;
			//pInfo->formatAPDL(strCommand);
			//PUSH_SHADOW_CMD(THIS_IS_MOD, strCommand);

			pInfo->modifyThis(strName, _eCode, _eGrade);
			pInfo->setDamping(dDmp);
		}
	}
		break;
	case PrepEnum::Material::Rebar:        
	{
		PrepEnum::Rebar _eGrade = PrepEnum::Rebar::Unassigned;
		if (!m_pCommand->StrToRebar(_eGrade, eGrade, 4))
		{
			errorCode = "普通钢筋的参数 材料等级 错误!";
			return { false, errorCode };
		}
		if (bState)
		{
			PREP::RebarInfo* pInfo = new PREP::RebarInfo(nMat, strName, PrepEnum::Material::Rebar, _eCode, _eGrade);
			pInfo->setDamping(dDmp);

			pInfo->check(vector<int>());
			RestoreMat(bStatus, nMat, pInfo);
		}
		else
		{
			PREP::RebarInfo* pInfo = dynamic_cast<PREP::RebarInfo*>(pMM->getInfo(nMat));
			if (!pInfo)
			{
				errorCode = "未在数据库中找到编号为：" + std::to_string(nMat) + "的材料！";
				return { false, errorCode };
			}
			pInfo->check(vector<int>());

			////压入影子命令
			//yilCString strCommand;
			//pInfo->formatAPDL(strCommand);
			//PUSH_SHADOW_CMD(THIS_IS_MOD, strCommand);

			pInfo->modifyThis(strName, _eCode, _eGrade);
			pInfo->setDamping(dDmp);
		}
	}
		break;
	//	SET_MAT(AppendRebar, ResetRebar);      
	//	break;
	//case PrepEnum::Material::Custom:       SET_MAT(AppendCustom, ResetCustom);           break;	// 自定义材料单独处理

	default:
		errorCode = "材料类型设置错误，不存在类型：" + eMaterialType + "！";
		return { false, errorCode };
	}

	m_pProject->GetPlotControl()->StructTreeChangedOn();
	m_pProject->GetPlotControl()->MaterialBaseDataChangedOn();
	m_pProject->GetPlotControl()->MaterialConcDataChangedOn();
	m_pProject->GetPlotControl()->MaterialSoilDataChangedOn();

	return { true, errorCode };
	//return { true, "成功创建材料，编号:" + std::to_string(nMat) + "，名称：" + strName + "！"};
}

/// <summary>
/// 删除材料
/// </summary>
/// <param name="nMat">材料编号</param>
/// <returns></returns>
std::pair<bool, std::string> PyInterface::OSIS_MaterialDel(const int nMat)
{
	std::string errorCode;
	/*ASSERT_FEM_STATUS();

	yilCString VarStr[1] = { _T("") };
	if (!StandardCommandParse(_Command, VarStr, 1)) return false;


	int nNO;
	if (!StrToIntGTZero(nNO, VarStr[0], 1)) return false;

	if (VarStr[0].IsEmpty()) return true;*/

	auto* pMM = GET_PREP_MAT();
	auto* pInfo = pMM->getInfo(nMat);
	if (!pInfo)
	{
		return { true, errorCode };
	}

	//	if (!pInfo->check()) return false;


	//yilCString strCommand;
	//pInfo->formatAPDL(strCommand);
	//PUSH_SHADOW_CMD(THIS_IS_DEL, strCommand);
	if (!pMM->deleteByNO(nMat)) {
		//POP_SHADOW_CMD();
		errorCode = "删除截面" + std::to_string(nMat) + "失败";
		return { false, errorCode };
	}

	//add by cf
	m_pProject->GetPlotControl()->StructTreeChangedOn();
	m_pProject->GetPlotControl()->MaterialBaseDataChangedOn();
	m_pProject->GetPlotControl()->MaterialConcDataChangedOn();
	m_pProject->GetPlotControl()->MaterialSoilDataChangedOn();

	return { true, errorCode };
}

/// <summary>
/// 修改材料编号
/// </summary>
/// <param name="nOld">旧编号</param>
/// <param name="nNew">新编号</param>
/// <returns></returns>
std::pair<bool, std::string> PyInterface::OSIS_MaterialMod(const int nOld, const int nNew)
{
	std::string errorCode;
	//ASSERT_FEM_STATUS();

	//Cmd strCmd;
	//if (!StandardCommandParse(strCmd, _Command, 2))
	//	return false;

	auto* pInfo = GET_PREP_MAT()->getInfo(nOld);
	if (!pInfo)
	{
		errorCode = "未找到编号为" + std::to_string(nOld) + "的材料！";
		return { false, errorCode };
	}

	if (GET_PREP_MAT()->getInfo(nNew)) {
		//m_ErrCode.Format(_T("材料[%d]已存在"), nNew);
		//g_pWarningList->AddToTemperary(4, m_ErrCode);
		errorCode = "截面" + std::to_string(nNew) + "已存在！";
		return { false, errorCode };
	}

	GET_PREP_MAT()->modifyIndex(nOld, nNew);	// 虽然这句话应该在最后执行，但是限制于下面关联参数的数据检查，先放这里

	auto* pEM = GET_PREP_ELEMENT();
	const set<int>& setRelatedElement = pInfo->getRelatedElement();
	if (!setRelatedElement.empty()) {

		for (const int it : setRelatedElement) {
			auto* pRelatedInfo = pEM->getInfo(it);
			if (!pRelatedInfo) 
				continue;
			pRelatedInfo->substitueMaterial(nOld, nNew);
		}

		m_pProject->GetPlotControl()->ElementDataChangedOn();
		m_pProject->GetPlotControl()->ElemPropBeam3DsDataChangedOn();
	}
	pEM = nullptr;


	////压入影子命令
	//yilCString strCommand;
	//strCommand.Format(_T("MaterialMod,%d,%d"), nNew, nOld);
	//PUSH_SHADOW_CMD(THIS_IS_MOD, strCommand);

	m_pProject->GetPlotControl()->InputChangedOn();
	m_pProject->GetPlotControl()->StructTreeChangedOn();
	m_pProject->GetPlotControl()->MaterialBaseDataChangedOn();
	m_pProject->GetPlotControl()->MaterialConcDataChangedOn();
	m_pProject->GetPlotControl()->MaterialSoilDataChangedOn();

	return { true, errorCode };
}
