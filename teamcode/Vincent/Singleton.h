#ifndef SINGLETON_HEADER
#define SINGLETON_HEADER

class Singleton
{
public:
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;
protected:
    constexpr Singleton() = default;
    ~Singleton() = default;
};

#endif //SINGLETON_HEADER