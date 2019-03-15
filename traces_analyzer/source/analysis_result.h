#pragma once

#include "traces_parser_provider.h"

#include <vector>
#include <map>
#include <memory>

enum ResultItemClassID
{
	SingleItem = 0,
	MultipleItem = 1
};

struct Trace
{
	Trace() :
		textPosition(0),
		line(0)
	{};

	TraceDescription	traceDescription;
	std::string			traceString;
	size_t				textPosition;
	size_t				line;
};

class ResultItem
{
public:
	virtual ~ResultItem();

public:
	virtual ResultItemClassID GetClassID() const = 0;
};

class SingleResultItem :
	public ResultItem
{
public:
	SingleResultItem(Trace const& trace);

public:
	ResultItemClassID GetClassID() const { return SingleItem; };

public:
	void GetTrace(Trace const& trace);

private:
	Trace const m_trace;

};

class MultipleResultItem :
	public ResultItem
{
public:
	MultipleResultItem(Trace const& beginTrace, Trace const& endTrace);

private:
	typedef std::vector<std::unique_ptr<ResultItem>> ResultItems;

public:
	ResultItemClassID GetClassID() const { return MultipleItem; };

public:
	void GetTrace(Trace const& beginTrace, Trace const& endTrace);

private:
	Trace const m_beginTrace;
	Trace const m_endTrace;

	ResultItems m_resultItems;
};


class AnalysisResult
{
public:
	AnalysisResult();
	~AnalysisResult();

private:
	typedef std::map<int, std::unique_ptr<ResultItem> > Items;

public:
	size_t      GetCountItems() const;
	ResultItem* GetItem(int i) const;

private:
	Items m_items;
};

