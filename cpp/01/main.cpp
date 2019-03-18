#include <iostream>
#include <memory>
#include <cstring>
#include <cstdlib>

#include "numbers.dat"

using ull = unsigned long long;

const unsigned minValue = 0;
const unsigned maxValue = 100000;

std::unique_ptr<bool[]>
eratosthenesSieve(unsigned until)
{
    const unsigned size = until + 1;
    auto prime = std::make_unique<bool[]>(size);
    std::memset(prime.get(), 1, size);
    prime[0] = prime[1] = false;

    for (unsigned i = 2; i <= until; ++i)
        for (ull composite = i * i; composite <= ull(until); composite += i)
            prime[composite] = false;

    return prime;
}

bool
isPrime(unsigned value)
{
    const static auto prime = eratosthenesSieve(maxValue);
    return prime[value];
}

int
findLeftPos(int value)
{
    if (value < Data[0] || value > Data[Size - 1])
        return -1;

    if (value == Data[0])
        return 0;

    int l = 0;
    int r = Size - 1;
    while (r - l > 1) {
        int mid = (l + r) / 2;
        if (Data[mid] < value)
            l = mid;
        else
            r = mid;
    }

    return Data[r] == value ? r : -1;
}

int
findRightPos(int value)
{
    if (value < Data[0] || value > Data[Size - 1])
        return -1;

    if (value == Data[Size - 1])
        return Size - 1;

    int l = 0;
    int r = Size - 1;
    while (r - l > 1) {
        int mid = (l + r) / 2;
        if (Data[mid] > value)
            r = mid;
        else
            l = mid;
    }

    return Data[l] == value ? l : -1;
}

int
countPrimesOnSegment(int left, int right)
{
    int ans = 0;
    for (int i = left; i <= right; ++i)
        ans += isPrime(unsigned(Data[i])) ? 1 : 0;
    return ans;
}

int
main(int argc, char** argv)
{
    if (argc == 1 || argc % 2 != 1)
        return -1;

    for (int i = 1; i < argc; i += 2) {
        int left = std::atoi(argv[i]);
        int right = std::atoi(argv[i + 1]);

        if (left > right) {
            std::cout << 0 << std::endl;
            continue;
        }

        int leftPos = findLeftPos(left);
        int rightPos = findRightPos(right);

        if (leftPos == -1 || rightPos == -1)
            std::cout << 0 << std::endl;
        else
            std::cout << countPrimesOnSegment(leftPos, rightPos) << std::endl;
    }

    return 0;
}