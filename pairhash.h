#ifndef PAIRHASH_H
#define PAIRHASH_H

#include<cstddef>
#include<utility>
class PairHash
{
public:
    size_t operator()(const std::pair<int, int>& p) const noexcept;

};

class PairEqual {
public:
    bool operator()(const std::pair<int, int>& a, const std::pair<int, int>& b) const noexcept;
};

#endif // PAIRHASH_H
