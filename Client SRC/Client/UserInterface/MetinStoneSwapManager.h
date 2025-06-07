/**
* Filename: MetinStoneSwapManager.h
* Author: blackdragonx61 (aka. Mali)
**/

#pragma once

#if defined(ENABLE_METINSTONE_SWAP)
namespace MetinStoneSwap
{
	using RangeVnumVec = std::vector<DWORD>;

	struct SwapInfo
	{
		const RangeVnumVec* range_vnums;
		DWORD swap_vnum;
		bool use_color;
	};

	using SwapVec = std::vector<SwapInfo>;

	class Manager : public singleton<Manager>
	{
	public:
		Manager();
		virtual ~Manager();

		bool Load(const char* file_name);
		BYTE GetEventFlag() const;
		void SetEventFlag(BYTE flag);
		const SwapInfo* GetSwapDataByVnum(DWORD vnum) const;

	private:
		bool __LoadRange(CTextFileLoader& rTextFileLoader);
		bool __LoadSwap(CTextFileLoader& rTextFileLoader);
		void __Refresh();
		bool __IsInRangeVec(DWORD vnum) const;

	private:
		BYTE event_flag_;
		const SwapVec* current_swap_vec_;
		std::unordered_map<BYTE, RangeVnumVec> range_map_;
		std::unordered_map<BYTE, SwapVec> swap_map_;
	};
}
#endif
