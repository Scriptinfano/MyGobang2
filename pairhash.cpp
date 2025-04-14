#include "pairhash.h"
#include <utility>       // std::pair
#include <functional>    // std::hash
#include <cstddef>       // size_t

size_t PairHash::operator()(const std::pair<int, int>& p) const noexcept
{
    return std::hash<int>()(p.first) ^ (std::hash<int>()(p.second) << 1);
}
bool PairEqual::operator()(const std::pair<int, int>& a, const std::pair<int, int>& b) const noexcept{
    return (a.first == b.first && a.second == b.second) || (a.first == b.second && a.second == b.first);
}
