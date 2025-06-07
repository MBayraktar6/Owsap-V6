/**
* Filename: MetinStoneSwapManager.cpp
* Author: blackdragonx61 (aka. Mali)
**/

#include "StdAfx.h"

#if defined(ENABLE_METINSTONE_SWAP)
#include "MetinStoneSwapManager.h"

#include "../GameLib/ActorInstance.h"

#include "PythonNonPlayer.h"
#include "PythonCharacterManager.h"

namespace MetinStoneSwap
{
	enum
	{
		SWAP_RANGE_NUMBER,
		SWAP_SWAP_VNUM,
		SWAP_USE_MOB_COLOR,

		SWAP_MAX
	};

	Manager::Manager() :
		event_flag_(0),
		current_swap_vec_(nullptr)
	{
	}

	Manager::~Manager() = default;

	void Manager::__Refresh()
	{
		CPythonCharacterManager& rkChrMgr = CPythonCharacterManager::Instance();
		for (CPythonCharacterManager::CharacterIterator i = rkChrMgr.CharacterInstanceBegin(); i != rkChrMgr.CharacterInstanceEnd(); ++i)
		{
			CInstanceBase* instance = *i;
			if (!instance)
				continue;

			if (!instance->IsStone())
				continue;

			const DWORD race = instance->GetRace();

			if (event_flag_)
			{
				const SwapInfo* swapInfo = GetSwapDataByVnum(race);
				if (!swapInfo)
					continue;

				if (swapInfo->use_color)
				{
					const DWORD color = CPythonNonPlayer::Instance().GetMonsterColor(race);
					if (color)
						instance->SetColorOperation(color);
				}
				else
				{
					instance->RestoreRenderMode();
					instance->SetAddColor(D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f));
				}

				instance->SetRaceDataStoneShapeEvent(race);
				instance->ChangeStoneShape(race, swapInfo->swap_vnum);
			}
			else if (__IsInRangeVec(race))
			{
				const DWORD color = CPythonNonPlayer::Instance().GetMonsterColor(race);
				if (color)
					instance->SetColorOperation(color);

				instance->ChangeStoneShape(race, 0);
				instance->ResetRaceDataStoneShapeEvent();
			}
		}
	}

	bool Manager::Load(const char* file_name)
	{
		if (!file_name)
			return false;

		if (!range_map_.empty() || !swap_map_.empty())
			return true;

		CTextFileLoader fileLoader;
		if (!fileLoader.Load(file_name))
			return false;

		for (DWORD i = 0; i < fileLoader.GetChildNodeCount(); ++i)
		{
			if (!fileLoader.SetChildNode(i))
				continue;

			std::string nodeName;

			if (!fileLoader.GetCurrentNodeName(&nodeName))
				continue;

			if (!nodeName.compare("metinstone_range"))
			{
				if (!__LoadRange(fileLoader))
				{
					TraceError("Load failed 'metinstone_range' in metinstone_swap.txt.");
					return false;
				}
			}
			else if (!nodeName.compare("metinstone_swap"))
			{
				if (!__LoadSwap(fileLoader))
				{
					TraceError("Load failed 'metinstone_swap_vnum' in metinstone_swap.txt.");
					return false;
				}
			}

			fileLoader.SetParentNode();
		}

		return true;
	}

	bool Manager::__LoadRange(CTextFileLoader& text_file_loader)
	{
		BYTE range;
		if (!text_file_loader.GetTokenByte("range", &range))
		{
			TraceError("[METINSTONE_SWAP] 'range' not exist in Group 'metinstone_range' in metinstone_swap.txt.");
			return false;
		}

		CTokenVector* tokenVector;
		if (!text_file_loader.GetTokenVector("range_vnum", &tokenVector) || !tokenVector || tokenVector->empty())
		{
			TraceError("[METINSTONE_SWAP] 'List range_vnum' is empty in range(%d) in Group 'metinstone_range' in metinstone_swap.txt.", range);
			return false;
		}

		RangeVnumVec& vecRange = range_map_[range];

		for (size_t i = 0; i < tokenVector->size(); ++i)
		{
			const DWORD vnum = atoi(tokenVector->at(i).c_str());
			const CPythonNonPlayer::TMobTable* table = CPythonNonPlayer::Instance().GetTable(vnum);
			if (!table)
			{
				TraceError("[METINSTONE_SWAP] Invalid stone vnum (%u) in range(%d) in Group 'metinstone_range' in metinstone_swap.txt.", vnum, range);
				return false;
			}

			if (table->bType != CActorInstance::TYPE_STONE)
			{
				TraceError("[METINSTONE_SWAP] vnum(%u) isn't TYPE_STONE in range(%d) in Group 'metinstone_range' in metinstone_swap.txt.", vnum, range);
				return false;
			}

			if (std::find(vecRange.begin(), vecRange.end(), vnum) != vecRange.end())
			{
				TraceError("[METINSTONE_SWAP] vnum(%u) is duplicate value in range(%d) in 'List range_vnum' in Group 'metinstone_range' in metinstone_swap.txt.", vnum, range);
				return false;
			}

			vecRange.push_back(vnum);
		}

		return true;
	}

	bool Manager::__LoadSwap(CTextFileLoader& text_file_loader)
	{
		BYTE eventFlag;
		if (!text_file_loader.GetTokenByte("event_flag_value", &eventFlag))
		{
			TraceError("[METINSTONE_SWAP] 'event_flag_value' not exist in Group 'metinstone_swap_data' in metinstone_swap.txt.");
			return false;
		}

		if (!text_file_loader.SetChildNode("swap_data"))
		{
			TraceError("[METINSTONE_SWAP] 'Group swap_data' not exist in event_flag_value(%d) in Group 'metinstone_swap' in metinstone_swap.txt.", eventFlag);
			return false;
		}

		std::vector<CTokenVector>* tokenVector;
		if (!text_file_loader.GetTokenVectorNew(&tokenVector) || !tokenVector || tokenVector->empty())
		{
			TraceError("[METINSTONE_SWAP] 'Group swap_data' is empty in event_flag_value(%d) in Group 'metinstone_swap' in metinstone_swap.txt.", eventFlag);
			return false;
		}

		SwapVec& vecSwap = swap_map_[eventFlag];

		for (size_t i = 0; i < tokenVector->size(); ++i)
		{
			const CTokenVector& tokenVec = tokenVector->at(i);
			if (tokenVec.size() != SWAP_MAX)
			{
				TraceError("[METINSTONE_SWAP] 'Group swap_data' token size is not enough in event_flag_value(%d) in Group 'metinstone_swap' in metinstone_swap.txt.", eventFlag);
				return false;
			}

			const BYTE range = atoi(tokenVec[SWAP_RANGE_NUMBER].c_str());

			const auto it = range_map_.find(range);
			if (it == range_map_.end())
			{
				TraceError("[METINSTONE_SWAP] range_number(%d) is not exist in metinstone_swap.txt.", range);
				return false;
			}

			SwapInfo swap;
			swap.range_vnums = &it->second;
			swap.swap_vnum = atoi(tokenVec[SWAP_SWAP_VNUM].c_str());
			swap.use_color = !tokenVec[SWAP_USE_MOB_COLOR].compare("true");
			vecSwap.push_back(swap);
		}

		text_file_loader.SetParentNode();
		return true;
	}

	BYTE Manager::GetEventFlag() const
	{
		return event_flag_;
	}

	void Manager::SetEventFlag(BYTE flag)
	{
		const auto it = swap_map_.find(flag);
		if (it != swap_map_.end())
		{
			event_flag_ = flag;
			current_swap_vec_ = &it->second;
		}
		else
		{
			event_flag_ = 0;
			current_swap_vec_ = nullptr;
		}

		__Refresh();
	}

	const SwapInfo* Manager::GetSwapDataByVnum(DWORD vnum) const
	{
		if (!current_swap_vec_)
			return nullptr;

		for (SwapVec::const_iterator it = current_swap_vec_->begin(); it != current_swap_vec_->end(); ++it)
		{
			const RangeVnumVec* rangeVnumVec = it->range_vnums;
			if (std::find(rangeVnumVec->begin(), rangeVnumVec->end(), vnum) != rangeVnumVec->end())
				return &(*it);
		}

		return nullptr;
	}

	bool Manager::__IsInRangeVec(DWORD vnum) const
	{
		for (const auto& elem : range_map_)
		{
			const RangeVnumVec& rangeVec = elem.second;
			if (std::find(rangeVec.begin(), rangeVec.end(), vnum) != rangeVec.end())
				return true;
		}

		return false;
	}
}
#endif
