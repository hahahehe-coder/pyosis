#include "stdafx.h"
#include "PyInterface.h"

template<typename InfoType>
bool RestoreLiveGrade(bool _status, std::string _Name, InfoType* _pInfo)
{
	auto* __AM__ = GET_PREP_ANA(); 
	if (_status) {
			
		if (!__AM__->append<PREP::LiveGradeInfo>(_pInfo)) {
					
			delete _pInfo; _pInfo = nullptr; 
			return false; 
		}
			//PUSH_SHADOW_CMD_CUSTOM(THIS_IS_DEF, "LiveGradeDel,%s", _Name.c_str()); 
	}
	else {
			
			//PUSH_SHADOW_CMD_DEFAULT(THIS_IS_MOD, __AM__->getInfo<PREP::LiveGradeInfo>(_Name)); 
		if (!__AM__->switchDerivedType<PREP::LiveGradeInfo>(_pInfo)) {
					
			delete _pInfo; _pInfo = nullptr; 
			return false; 
		}
	}
	return true;
}

/// <summary>
/// 定义活载
/// </summary>
/// <param name="strName">活载名称</param>
/// <param name="eCode">规范类型，JTGD60_2015，CUSTOM</param>
/// <param name="eLiveLoadType">活载类型</param>
/// <param name="kwargs">附加参数</param>
/// <returns></returns>
std::pair<bool, std::string> PyInterface::OSIS_LiveGrade(const std::string strName, const std::string eCode, const std::string eLiveLoadType, const py::dict kwargs)
{
	std::string errorCode = "";
	PrepEnum::Code eC = PrepEnum::Code::Unassigned;
	if (!GetCommand()->StrToCode(eC, eCode, 2))
	{
		errorCode = "参数 规范类型 错误！";
		return { false, errorCode };
	}
	switch (eC)
	{
	case PrepEnum::Code::JTGD64_2015:
	{
		PREP::LoadEnum::LiveGrade eLGT = PREP::LoadEnum::LiveGrade::UNASSIGNEND;
		if (!GetCommand()->StrToLiveGrade(eLGT, eLiveLoadType, 3))
		{
			errorCode = "参数 活载类型 错误！";
			return { false, errorCode };
		}
		bool bState = true;  //状态，true = 定义，false = 修改
		bool bStatus = true; //针对bState = true的情况：true = 初次定义；false = 修改类型
		PREP::LiveGradeInfo* pMat = GET_PREP_ANA()->getInfo<PREP::LiveGradeInfo>(strName);
		if (pMat) {

			bState = false;

			/* > > > > > > 切换活载等级类型 > > > > > > */
			if (pMat->getGrade() != eLGT) { //修改一个已经存在的活载等级类型

				bState = true;
				bStatus = false;
			}
			/* < < < < < < 切换活载等级类型 < < < < < < */
		}

		switch (eLGT)
		{
		case PREP::LoadEnum::LiveGrade::HIGHWAY_I:
		case PREP::LoadEnum::LiveGrade::HIGHWAY_II:
		case PREP::LoadEnum::LiveGrade::VEHICLE:
		case PREP::LoadEnum::LiveGrade::FATIGUE_I:
		case PREP::LoadEnum::LiveGrade::FATIGUE_III:
		{
			if (bState)
			{
				PREP::LiveGradeInfo* pInfo = new PREP::LiveGradeInfo(PrepEnum::Code::JTGD60_2015, eLGT);
				pInfo->setName(strName);
				RestoreLiveGrade<PREP::LiveGradeInfo>(bStatus, strName, pInfo);
			}
			else
			{
				PREP::LiveGradeInfo* pInfo = GET_PREP_ANA()->getInfo<PREP::LiveGradeInfo>(strName);
				if (!pInfo) 
				{
					errorCode = errorCode = "未在数据库中找到名称为：" + strName + "的移动活载！";
					return { false, errorCode };
				}

				//压入影子命令
				//PUSH_SHADOW_CMD_DEFAULT(THIS_IS_MOD, pInfo);

				pInfo->setCode(PrepEnum::Code::JTGD60_2015);
			}
		}
			break;
		case PREP::LoadEnum::LiveGrade::FATIGUE_II:
		{
			double dCenterDis;
			try {
				dCenterDis = safe_cast<double>(kwargs, "dCenterDis");
			}
			catch (const std::runtime_error& e) {
				// 处理错误
				return { false, e.what() };
			}

			if (bState)
			{
				dCenterDis = GetProject()->GetUnit()->LocalLengthToSI(dCenterDis);

				PREP::LiveGradeInfo* pInfo = new PREP::LiveGradeInfo(PrepEnum::Code::JTGD60_2015, dCenterDis);
				pInfo->setName(strName);

				RestoreLiveGrade<PREP::LiveGradeInfo>(bStatus, strName, pInfo);
			}
			else
			{
				PREP::LiveGradeInfo* pInfo = GET_PREP_ANA()->getInfo<PREP::LiveGradeInfo>(strName);
				if (!pInfo)
				{
					errorCode = errorCode = "未在数据库中找到名称为：" + strName + "的移动活载！";
					return { false, errorCode };
				}

				double dWidth = 0.0;

				//压入影子命令
				//PUSH_SHADOW_CMD_DEFAULT(THIS_IS_MOD, pInfo);

				dWidth = GetProject()->GetUnit()->LocalLengthToSI(dWidth);

				pInfo->setCode(PrepEnum::Code::JTGD60_2015);
				pInfo->setFatigueIIVeichleCenterDis(dCenterDis);
			}
		}
			break;
		case PREP::LoadEnum::LiveGrade::CROWD:
		{
			std::string eBridgeType;
			double dPara;

			PREP::LoadEnum::CrowdBridgeType eCBT = PREP::LoadEnum::CrowdBridgeType::UNASSIGNEND;
			try {
				eBridgeType = safe_cast<std::string>(kwargs, "eBridgeType");
				dPara = safe_cast<double>(kwargs, "dPara");
				if (!GetCommand()->StrToCrowdBridgeType(eCBT, eBridgeType, 4))
				{

					errorCode = "参数 eBridgeType 错误！";
					return { false, errorCode };
				}
			}
			catch (const std::runtime_error& e) {
				// 处理错误
				return { false, e.what() };
			}

			if (bState)
			{
				dPara = GetProject()->GetUnit()->LocalLengthToSI(dPara);

				PREP::LiveGradeInfo* pInfo = new PREP::LiveGradeInfo(PrepEnum::Code::JTGD60_2015, eCBT, dPara);
				pInfo->setName(strName);
				pInfo->setCrowdBridgeType(eCBT);
				pInfo->setCrowdWidth(dPara);
				RestoreLiveGrade<PREP::LiveGradeInfo>(bStatus, strName, pInfo);
			}
			else
			{
				PREP::LiveGradeInfo* pInfo = GET_PREP_ANA()->getInfo<PREP::LiveGradeInfo>(strName);
				if (!pInfo)
				{
					errorCode = errorCode = "未在数据库中找到名称为：" + strName + "的移动活载！";
					return { false, errorCode };
				}

				double dWidth = 0.0;

				//压入影子命令
				//PUSH_SHADOW_CMD_DEFAULT(THIS_IS_MOD, pInfo);

				dWidth = GetProject()->GetUnit()->LocalLengthToSI(dWidth);

				pInfo->setCode(PrepEnum::Code::JTGD60_2015);
				pInfo->setCrowdBridgeType(eCBT);
				pInfo->setCrowdWidth(dWidth);
			}
		}
			break;
		case PREP::LoadEnum::LiveGrade::VG:
		{
			errorCode = "接口未完成！";
			return { false, errorCode };
		}
			break;
		default:
			errorCode = "参数 活载类型 错误！";
			return { false, errorCode };
		}
	}
		break;
	case PrepEnum::Code::CUSTOM:
	{
		errorCode = "接口未完成！";
		return { false, errorCode };
	}
		break;
	default:
		errorCode = "参数 规范类型 错误！";
		return { false, errorCode };
	}
	return { true, errorCode };
}