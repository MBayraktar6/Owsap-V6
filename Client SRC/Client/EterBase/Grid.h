#ifndef __INC_ETER_BASE_GRID_H__
#define __INC_ETER_BASE_GRID_H__

class CGrid
{
public:
	CGrid(int w, int h);
	CGrid(CGrid* pkGrid, int w, int h);
	~CGrid();

	void Clear();
	int FindBlank(int w, int h);
	bool IsEmpty(int iPos, int w, int h);
	bool Put(int iPos, int w, int h);
	void Get(int iPos, int w, int h);
	void Print();
	unsigned int GetSize();

protected:
	int	m_iWidth;
	int	m_iHeight;

	char* m_pGrid;
};
#endif // __INC_ETER_BASE_GRID_H__
