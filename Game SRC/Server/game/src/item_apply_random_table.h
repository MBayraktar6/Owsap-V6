/**
* Filename: item_apply_random_table.h
* Author: Owsap
* Description: Generate a random bonus path.
**/

#if !defined(__INC_ITEM_APPLY_RANDOM_H__) && defined(__ITEM_APPLY_RANDOM__)
#define __INC_ITEM_APPLY_RANDOM_H__

enum EApplyRandom
{
	APPLY_RANDOM_PATH1 = 1,
	APPLY_RANDOM_PATH2,
	APPLY_RANDOM_MAX_PATHS,
	ITEM_REFINE_MAX_LEVEL = 15,
};

struct SApplyRandom
{
	SApplyRandom(EApplyTypes _ApplyType, std::string _ApplyValueGroupName) : ApplyType(_ApplyType), ApplyValueGroupName(_ApplyValueGroupName) {}
	EApplyTypes ApplyType;
	std::string ApplyValueGroupName;
};

class CGroupNode;
class CGroupTextParseTreeLoader;

class CApplyRandomTable
{
public:
	enum EGetType
	{
		GET_CURRENT,
		GET_PREVIOUS,
		GET_NEXT
	};

public:
	CApplyRandomTable();
	virtual ~CApplyRandomTable();

	bool ReadApplyRandomTableFile(const char* c_pszFileName);
	bool GetApplyRandom(BYTE bIndex, BYTE bLevel, POINT_TYPE& wApplyType, POINT_VALUE& lApplyValue, BYTE& bPath);
	POINT_VALUE GetApplyRandomValue(BYTE bIndex, BYTE bLevel, BYTE bPath, POINT_TYPE wApplyType);

public:
	using ApplyRandomVector = std::vector<SApplyRandom>;
	using ApplyRandomGroupMap = std::map<std::string, ApplyRandomVector>;

private:
	bool ReadApplyRandomMapper();
	bool ReadApplyRandomTypes();

	BYTE GetApplyRandomPath(std::string stApplyValueGroupName) const;
	POINT_VALUE GetApplyRandomRowValue(std::string stApplyValueGroupName, BYTE bLevel, BYTE bPath) const;

private:
	CGroupTextParseTreeLoader* m_pLoader;
	std::string stFileName;

	CGroupNode* m_pApplyRandomValueTableNode;

	std::vector<std::string> m_vecApplyRandomGroups;
	ApplyRandomGroupMap m_mapApplyRandomGroup;
};
#endif // __ITEM_APPLY_RANDOM__
