
#pragma once

struct RealTimeQuotes
{
    time_t nls_time; // NLS Time (ET)
    std::string nls_price; // NLS Price
    int nls_share_volume; // NLS Share Volume
};

typedef boost::shared_ptr<RealTimeQuotes> SharedRealTimeQuotes;
