#pragma once
#include "MyHeader.h"

class COneEdge
{
public:
	COneEdge(void);
	~COneEdge(void);
public:
	int m_nIndexOfNode1; // index of the node 1 in m_vaAllNodes in COneGraph;
	int m_nIndexOfNode2; // index of the node 2 in m_vaAllNodes in COneGraph;
	double m_dEdgeWeight; // the edge weight
public:
	COneEdge(const COneEdge& aOneEdge)
	{
		m_nIndexOfNode1 = aOneEdge.GetIndexOfNode1();
		m_nIndexOfNode2 = aOneEdge.GetIndexOfNode2();
		m_dEdgeWeight = aOneEdge.GetEdgeWeight();
	};
	COneEdge& operator = (const COneEdge& aOneEdge)
	{
		m_nIndexOfNode1 = aOneEdge.GetIndexOfNode1();
		m_nIndexOfNode2 = aOneEdge.GetIndexOfNode2();
		m_dEdgeWeight = aOneEdge.GetEdgeWeight();
		return *this;
	};
public:
	inline int GetIndexOfNode1() const {return m_nIndexOfNode1;};
	inline int GetIndexOfNode2() const {return m_nIndexOfNode2;};
	inline void SetIndexOfNode1(int nIndexOfNode1){m_nIndexOfNode1 = nIndexOfNode1;};
	inline void SetIndexOfNode2(int nIndexOfNode2){m_nIndexOfNode2 = nIndexOfNode2;};
	inline double GetEdgeWeight() const {return m_dEdgeWeight;};
	inline void SetEdgeWeight(double dEdgeWeight){m_dEdgeWeight = dEdgeWeight;};
};
