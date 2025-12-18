#include "stdafx.h"
#include "PyInterface.h"
#include "Boundary/BoundaryInc.h"

template<typename InfoType>
bool RestoreBoundary(bool _status, int _index, InfoType* _pInfo)
{
	auto* __BM__ = GET_PREP_BD();
	if (_status) {

		if (!__BM__->append(_pInfo)) {

			delete _pInfo; _pInfo = nullptr;
			return false;
		}
		//PUSH_SHADOW_CMD_CUSTOM(THIS_IS_DEF, "BoundaryDel,%d", _index); 
	}
	else {

		//PUSH_SHADOW_CMD_DEFAULT(THIS_IS_MOD, __BM__->getInfo(_index)); 
		if (!__BM__->switchDerivedType(_pInfo)) {

			delete _pInfo; _pInfo = nullptr;
			return false;
		}
	}
}

/// <summary>
/// 边界条件
/// </summary>
/// <param name="nBd">边界编号</param>
/// <param name="eBoundaryType">边界类型：一般边界 主从约束 释放梁端约束 节点弹性支撑</param>
/// <param name="kwargs"></param>
/// <returns></returns>
std::pair<bool, std::string> PyInterface::OSIS_Boundary(const int nBd, const std::string eBoundaryType, const py::dict kwargs)
{
	std::string errorCode;

	PrepEnum::Boundary eBT = PrepEnum::Boundary::Unassigned;
	if (!GetCommand()->StrToBoundary(eBT, eBoundaryType, 2))
	{
		errorCode = "参数 边界类型 错误！";
		return { false, errorCode };
	}

	bool bState = true;  //状态，true = 定义，false = 修改
	bool bStatus = true; //针对bState = true的情况：true = 初次定义；false = 修改类型
	PREP::BoundaryInfo* pBd = GET_PREP_BD()->getInfo(nBd);
	if (pBd) {

		bState = false;

		/* > > > > > > 切换边界类型 > > > > > > */
		if (pBd->getType() != eBT) { //修改一个已经存在的边界类型

			bState = true;
			bStatus = false;
		}
		/* < < < < < < 切换边界类型 < < < < < < */
	}


	switch (eBT) 
	{
	case PrepEnum::Boundary::General:
	{
		int nCoor;
		bool bDX, bDY, bDZ;
		bool bRX, bRY, bRZ, bRW;
		try {
			nCoor = safe_cast<int>(kwargs, "nCoor");
			bDX = safe_cast<bool>(kwargs, "bDX");
			bDY = safe_cast<bool>(kwargs, "bDY");
			bDZ = safe_cast<bool>(kwargs, "bDZ");
			bRX = safe_cast<bool>(kwargs, "bRX");
			bRY = safe_cast<bool>(kwargs, "bRY");
			bRZ = safe_cast<bool>(kwargs, "bRZ");
			bRW = safe_cast<bool>(kwargs, "bRW");
		}
		catch (const std::runtime_error& e) {
			// 处理错误
			return { false, e.what() };
		}

		if (!GET_PREP_PROP()->getInfo<PREP::CoorSysInfo>(nCoor))
		{
			errorCode = "一般支承" + std::to_string(nBd) + "：空间坐标系" + std::to_string(nCoor) + "不存在！";
			return { false, errorCode };
		}
		using Info = PREP::GeneralBoundaryInfo;
		if (bState)
		{
			Info* pInfo = new Info();
			pInfo->setNO(nBd);
			pInfo->setType(eBT);
			if (nCoor != -1) {
				if (!pInfo->setCoorNO(nCoor)) {
					delete pInfo; pInfo = nullptr;
					errorCode = "一般支承" + std::to_string(nBd) + "：空间坐标系" + std::to_string(nCoor) + "设定失败！";
					return {false, errorCode};
				}
			}

			pInfo->setConstraints(std::vector<int>{bDX, bDY, bDZ, bRX, bRY, bRZ, bRW});
			pInfo->check(vector<int>());
			RestoreBoundary(bStatus, nBd, pInfo);
		}
		else
		{
			auto* pBM = GET_PREP_BD();
			Info* pInfo = dynamic_cast<Info*>(pBM->getInfo(nBd));
			pInfo->check(vector<int>());

			//压入影子命令
			//yilCString strCommand;
			//pInfo->formatAPDL(strCommand, BD_APDL_BOUNDARY_ONLY);
			//PUSH_SHADOW_CMD(THIS_IS_MOD, strCommand);

			pInfo->setNO(nBd);
			pInfo->setType(PrepEnum::Boundary::General);
			if (nCoor != -1) {
				if (!pInfo->setCoorNO(nCoor)) {
					//POP_SHADOW_CMD();
					errorCode = "一般支承" + std::to_string(nBd) + "：空间坐标系" + std::to_string(nCoor) + "设定失败！";
					return { false, errorCode };
				}
			}
			else {
				if (!pInfo->setCoorNO(-1)) {
					//POP_SHADOW_CMD();
					errorCode = "一般支承" + std::to_string(nBd) + "：空间坐标系" + std::to_string(nCoor) + "设定失败！";
					return { false, errorCode };
				}
			}

			pInfo->setConstraints(std::vector<int>{bDX, bDY, bDZ, bRX, bRY, bRZ, bRW});
		}
		
	}
		break;
	default:
	{
		errorCode = "暂不支持该类型的边界！";
		return { false, errorCode };
	}
		break;
	}
	return { true, errorCode };
}

