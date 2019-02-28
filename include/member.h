#ifndef MEMBER_H_
#define MEMBER_H_

#include "joint.h"

struct Member {
    Member(int l, int r) : j1{ l }, j2{ r } {}

    bool operator==(const Member & other) const {
        return (j1 == other.j1 && j2 == other.j2)
            || (j2 == other.j1 && j1 == other.j2);
    }

    int j1;
    int j2;
};

namespace std {
template <>
class hash<Member> {
public:
    size_t operator()(const Member & m) const {
        return (m.j1 * 1971 ^ m.j1) + (m.j2 * 1971 ^ m.j2);
    }
};
}

#endif
