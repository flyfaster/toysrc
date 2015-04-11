#pragma once
#include <loki/Pimpl.h>

class STOCKCLIENT_API ImportDailyFrame// : private Loki::PimplOf<ImportDailyFrame>::Owner
{
public:
	ImportDailyFrame(void);
	~ImportDailyFrame(void);
	std::string GetSymbol();
	std::string GetDailyFile();
	bool DoModal();
	struct PrivateImportDailyFrame* d;
};
