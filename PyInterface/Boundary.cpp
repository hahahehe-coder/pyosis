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
	return true;
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

		//if (!GET_PREP_PROP()->getInfo<PREP::CoorSysInfo>(nCoor))
		//{
		//	errorCode = "一般支承" + std::to_string(nBd) + "：空间坐标系" + std::to_string(nCoor) + "不存在！";
		//	return { false, errorCode };
		//}
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
	GetProject()->GetPlotControl()->StructTreeChangedOn();//用于通知界面刷新菜单
	GetProject()->GetPlotControl()->BoundaryChangedOn();	//用于通知界面刷新大数据界面
	return { true, errorCode };
}

/// <summary>
/// 分配边界给节点(一般支撑，节点弹性支撑)
/// </summary>
/// <param name="nBd">边界编号</param>
/// <param name="eOP">操作: a = 添加，s = 替换，r = 移除，aa = 添加全部，ra = 移除全部</param>
/// <param name="nodeNOs">待操作的节点编号</param>
/// <returns></returns>
std::pair<bool, std::string> PyInterface::OSIS_AsgnBd(const int nBd, const std::string eOP, const py::list nodeNOs)
{
	std::string errorCode;
	auto* pBdInfo = GET_PREP_BD()->getInfo(nBd);
	if (!pBdInfo)
	{
		errorCode = "未能获取编号为：" + std::to_string(nBd) + "的边界信息！";
		return { false, errorCode };
	}

	if (eOP == OSIS_APPEND) { //添加
		std::set<int> setNO;
		for (auto& item : nodeNOs)
		{
			int nNO = item.cast<int>();
			setNO.insert(nNO);
		}
		pBdInfo->appendEntity(setNO);


		//压入影子命令
		//yilCString strCommand;
		//pBdInfo->formatAPDL(strCommand, BD_APDL_ASGNBD_ONLY, OSIS_REMOVE, setNO);
		//PUSH_SHADOW_CMD(THIS_IS_APP, strCommand);
	}
	else if (eOP == OSIS_SUBSTITUTE) { //替换

		std::vector<int> vecOldNO;
		std::vector<int> vecNewNO;
		for (auto& item : nodeNOs)	// 此情况下，nodeNOs每个元素为 <旧编号,新编号> 对
		{
			auto it = item.cast<py::list>();
			int nOld = it[0].cast<int>();
			int nNew = it[1].cast<int>();
			vecOldNO.push_back(nOld);
			vecNewNO.push_back(nNew);
		}
		pBdInfo->substituteEntity(vecOldNO, vecNewNO);

		//压入影子命令
		//yilCString strCommand;
		//pBdInfo->formatAPDL(strCommand, vecNewNO, vecOldNO);
		//PUSH_SHADOW_CMD(THIS_IS_MOD, strCommand);
	}
	else if (eOP == OSIS_REMOVE) { //移除
		//PROCESS_ASSGIN(BdRemove);

		std::set<int> setNO;
		for (auto& item : nodeNOs)
		{
			int nNO = item.cast<int>();
			setNO.insert(nNO);
		}
		pBdInfo->removeEntity(setNO);

		//压入影子命令
		//yilCString strCommand;
		//pBdInfo->formatAPDL(strCommand, BD_APDL_ASGNBD_ONLY, OSIS_APPEND, setNO);
		//PUSH_SHADOW_CMD(THIS_IS_RMV, strCommand);
	}
	else if (eOP == OSIS_APPEND_ALL) { //添加全部
		//PROCESS_ASSGIN(BdAppendAll);

		if (pBdInfo->getType() != PrepEnum::Boundary::Release) {
			std::set<int> vecNodeNO;
			GET_PREP_NODE()->getAllNO(vecNodeNO);
			pBdInfo->setEntityVec(vecNodeNO);
		}
		else {
			std::set<int> vecEleNO;
			GET_PREP_ELEMENT()->getAllNO(vecEleNO);
			pBdInfo->setEntityVec(vecEleNO);
		}

		//压入影子命令
		//yilCString strCommand;
		//strCommand.Format(_T("AsgnBd,%d,ra"), _index);
		//PUSH_SHADOW_CMD(THIS_IS_APP, strCommand);
	}
	else if (eOP == OSIS_REMOVE_ALL) { //移除全部
		//PROCESS_ASSGIN(BdRemoveAll);
		std::set<int> setNO = pBdInfo->getEntityVec();

		pBdInfo->removeEntity(setNO);

		//if (setNO.empty()) return true;

		//压入影子命令
		//yilCString strCommand;
		//pBdInfo->formatAPDL(strCommand, BD_APDL_ASGNBD_ONLY, OSIS_APPEND, setNO);
		//PUSH_SHADOW_CMD(THIS_IS_RMV, strCommand);
	}
	else {
		errorCode = "参数 操作类型 错误！";
		return { false, errorCode };
	}

	GetProject()->GetPlotControl()->StructTreeChangedOn();//用于通知界面刷新菜单
	GetProject()->GetPlotControl()->BoundaryChangedOn();	//用于通知界面刷新大数据界面

	return { true, errorCode };
}
